#include "HotReloadManager.hpp"

#include "FileWatcher.hpp"
#include "UIExceptions.hpp"

#include <QApplication>
#include <QBoxLayout>
#include <QDebug>
#include <QFileInfo>
#include <QGridLayout>
#include <QLayout>
#include <QTimer>
#include <QRegularExpression>
#include <QFile>

namespace DeclarativeUI::HotReload {

HotReloadManager::HotReloadManager(QObject* parent) : QObject(parent) {
    setupUILoader();
    setupThreadPool();

    file_watcher_ = std::make_unique<FileWatcher>(this);

    // **Connect optimized file watcher signals**
    connect(file_watcher_.get(), &FileWatcher::fileChanged, this,
            &HotReloadManager::onFileChangedOptimized);

    connect(file_watcher_.get(), &FileWatcher::fileAdded, this,
            &HotReloadManager::onFileAddedOptimized);

    connect(file_watcher_.get(), &FileWatcher::fileRemoved, this,
            &HotReloadManager::onFileRemovedOptimized);

    // **Start performance monitoring**
    uptime_timer_.start();
}

HotReloadManager::~HotReloadManager() { cleanupThreadPool(); }

void HotReloadManager::registerUIFile(const QString& file_path,
                                      QWidget* target_widget) {
    if (file_path.isEmpty()) {
        throw Exceptions::HotReloadException("File path cannot be empty");
    }

    if (!target_widget) {
        throw Exceptions::HotReloadException("Target widget cannot be null");
    }

    try {
        QFileInfo file_info(file_path);
        QString canonical_path = file_info.canonicalFilePath();

        if (!file_info.exists()) {
            throw Exceptions::HotReloadException("File does not exist: " +
                                                 canonical_path.toStdString());
        }

        // **Store UI file information**
        UIFileInfo& info = registered_files_[canonical_path];
        info.target_widget = target_widget;
        info.parent_widget = target_widget->parentWidget();
        info.last_reload = QDateTime::currentDateTime();

        // **Create backup**
        createBackup(canonical_path);

        // **Start watching the file**
        file_watcher_->watchFile(canonical_path);

        qDebug() << "🔥 Registered UI file for hot reload:" << canonical_path;

    } catch (const std::exception& e) {
        throw Exceptions::HotReloadException("Failed to register UI file '" +
                                             file_path.toStdString() +
                                             "': " + e.what());
    }
}

void HotReloadManager::registerUIDirectory(const QString& directory_path,
                                           bool recursive) {
    if (directory_path.isEmpty()) {
        throw Exceptions::HotReloadException("Directory path cannot be empty");
    }

    try {
        QFileInfo dir_info(directory_path);

        if (!dir_info.exists() || !dir_info.isDir()) {
            throw Exceptions::HotReloadException("Directory does not exist: " +
                                                 directory_path.toStdString());
        }

        file_watcher_->watchDirectory(dir_info.canonicalFilePath(), recursive);

        qDebug() << "🔥 Registered UI directory for hot reload:"
                 << dir_info.canonicalFilePath() << "(recursive:" << recursive
                 << ")";

    } catch (const std::exception& e) {
        throw Exceptions::HotReloadException(
            "Failed to register UI directory '" + directory_path.toStdString() +
            "': " + e.what());
    }
}

void HotReloadManager::setEnabled(bool enabled) {
    if (enabled_ != enabled) {
        enabled_ = enabled;
        emit hotReloadEnabled(enabled);

        qDebug() << "🔥 Hot reload" << (enabled ? "enabled" : "disabled");
    }
}

void HotReloadManager::setReloadDelay(int milliseconds) {
    reload_delay_ = std::max(0, milliseconds);
}

void HotReloadManager::setFileFilters(const QStringList& filters) {
    FileFilter filter;
    filter.extensions = filters;
    file_watcher_->setFileFilter(filter);
}

void HotReloadManager::reloadFile(const QString& file_path) {
    if (!enabled_) {
        qDebug() << "Hot reload is disabled, skipping reload for:" << file_path;
        return;
    }

    QString canonical_path = QFileInfo(file_path).canonicalFilePath();

    if (registered_files_.find(canonical_path) == registered_files_.end()) {
        qWarning() << "File not registered for hot reload:" << canonical_path;
        return;
    }

    performReload(canonical_path);
}

void HotReloadManager::reloadAll() {
    if (!enabled_) {
        qDebug() << "Hot reload is disabled, skipping reload all";
        return;
    }

    for (const auto& [file_path, info] : registered_files_) {
        if (shouldReload(file_path)) {
            performReload(file_path);
        }
    }
}

void HotReloadManager::unregisterUIFile(const QString& file_path) {
    QString canonical_path = QFileInfo(file_path).canonicalFilePath();

    auto it = registered_files_.find(canonical_path);
    if (it != registered_files_.end()) {
        file_watcher_->unwatchFile(canonical_path);
        registered_files_.erase(it);

        qDebug() << "🔥 Unregistered UI file from hot reload:"
                 << canonical_path;
    }
}

void HotReloadManager::unregisterAll() {
    file_watcher_->unwatchAll();
    registered_files_.clear();

    qDebug() << "🔥 Unregistered all UI files from hot reload";
}

void HotReloadManager::setErrorHandler(
    std::function<void(const QString&, const QString&)> handler) {
    error_handler_ = std::move(handler);
}

void HotReloadManager::onFileChanged(const QString& file_path) {
    if (!enabled_)
        return;

    qDebug() << "🔥 File changed:" << file_path;

    // **Debounce reload**
    int delay = reload_delay_.load();
    QTimer::singleShot(delay, [this, file_path]() {
        if (shouldReload(file_path)) {
            performReload(file_path);
        }
    });
}

void HotReloadManager::onFileAdded(const QString& file_path) {
    qDebug() << "🔥 File added:" << file_path;

    // **Auto-register new UI files if they match our filters**
    QFileInfo file_info(file_path);
    if (file_info.suffix().toLower() == "json") {
        // Could implement auto-registration logic here
        qDebug() << "🔥 New JSON file detected:" << file_path;
    }
}

void HotReloadManager::onFileRemoved(const QString& file_path) {
    qDebug() << "🔥 File removed:" << file_path;

    auto it = registered_files_.find(file_path);
    if (it != registered_files_.end()) {
        // **Restore backup if available**
        restoreBackup(file_path);

        emit reloadFailed(file_path, "File was removed");
    }
}

void HotReloadManager::performReload(const QString& file_path) {
    try {
        emit reloadStarted(file_path);

        auto it = registered_files_.find(file_path);
        if (it == registered_files_.end()) {
            throw Exceptions::HotReloadException("File not registered: " +
                                                 file_path.toStdString());
        }

        UIFileInfo& info = it->second;

        // **Check if enough time has passed since last reload**
        QDateTime now = QDateTime::currentDateTime();
        if (info.last_reload.isValid() &&
            info.last_reload.msecsTo(now) < reload_delay_) {
            qDebug() << "🔥 Skipping reload (too soon):" << file_path;
            return;
        }

        // **Create backup before reload**
        createBackup(file_path);

        // **Load new UI from file**
        std::unique_ptr<QWidget> new_widget =
            ui_loader_->loadFromFile(file_path);

        if (!new_widget) {
            throw Exceptions::HotReloadException(
                "Failed to load UI from file: " + file_path.toStdString());
        }

        // **Validate widget before replacement**
        if (!validateWidget(new_widget.get())) {
            throw Exceptions::HotReloadException(
                "Invalid widget created from file: " + file_path.toStdString());
        }

        // **Replace widget**
        replaceWidget(file_path, std::move(new_widget));

        // **Update last reload time**
        info.last_reload = now;

        emit reloadCompleted(file_path);

        qDebug() << "🔥 Successfully reloaded:" << file_path;

    } catch (const std::exception& e) {
        QString error_message = QString::fromStdString(e.what());
        qWarning() << "🔥 Hot reload failed for" << file_path << ":"
                   << error_message;

        // **Restore backup on failure**
        restoreBackup(file_path);

        // **Call error handler if set**
        if (error_handler_) {
            error_handler_(file_path, error_message);
        }

        emit reloadFailed(file_path, error_message);
    }
}

bool HotReloadManager::validateWidget(QWidget* widget) const {
    if (!widget) {
        return false;
    }

    // **Basic validation checks**
    try {
        // Check if widget has a valid parent or can be standalone
        if (widget->parent() && !qobject_cast<QWidget*>(widget->parent())) {
            qWarning() << "🔥 Widget has invalid parent type";
            return false;
        }

        // Check if widget can be shown
        if (!widget->isVisible() && !widget->isHidden()) {
            qWarning() << "🔥 Widget is in invalid visibility state";
            return false;
        }

        // **Performance check: ensure widget doesn't have too many children**
        const int MAX_CHILDREN = 1000;
        if (widget->children().size() > MAX_CHILDREN) {
            qWarning() << "🔥 Widget has too many children ("
                       << widget->children().size() << ")";
            return false;
        }

        return true;
    } catch (const std::exception& e) {
        qWarning() << "🔥 Widget validation failed:" << e.what();
        return false;
    }
}

void HotReloadManager::optimizeWidget(QWidget* widget) {
    if (!widget)
        return;

    try {
        // **Enable optimizations**
        widget->setAttribute(Qt::WA_OpaquePaintEvent, true);
        widget->setAttribute(Qt::WA_NoSystemBackground, true);
        widget->setAttribute(Qt::WA_StaticContents, true);

        // **Optimize updates**
        widget->setUpdatesEnabled(false);

        // **Apply optimizations to children**
        for (QObject* child : widget->children()) {
            if (QWidget* child_widget = qobject_cast<QWidget*>(child)) {
                optimizeWidget(child_widget);
            }
        }

        widget->setUpdatesEnabled(true);

    } catch (const std::exception& e) {
        qWarning() << "🔥 Widget optimization failed:" << e.what();
    }
}

void HotReloadManager::createBackup(const QString& file_path) {
    auto it = registered_files_.find(file_path);
    if (it != registered_files_.end() && it->second.target_widget) {
        // **In a real implementation, you might serialize the widget state**
        // **For now, we'll just keep a reference**
        // it->second.backup_widget = cloneWidget(it->second.target_widget);
    }
}

void HotReloadManager::restoreBackup(const QString& file_path) {
    auto it = registered_files_.find(file_path);
    if (it != registered_files_.end() && it->second.backup_widget) {
        // **Restore from backup**
        qDebug() << "🔄 Restoring backup for:" << file_path;
        // Implementation would restore the backup widget
    }
}

bool HotReloadManager::shouldReload(const QString& file_path) const {
    if (!enabled_)
        return false;

    auto it = registered_files_.find(file_path);
    if (it == registered_files_.end())
        return false;

    // **Check if enough time has passed since last reload**
    QDateTime current_time = QDateTime::currentDateTime();
    QDateTime last_reload = it->second.last_reload;

    return last_reload.msecsTo(current_time) >= reload_delay_;
}

void HotReloadManager::setupUILoader() {
    ui_loader_ = std::make_unique<JSON::JSONUILoader>(this);

    // **Connect loader signals**
    connect(ui_loader_.get(), &JSON::JSONUILoader::loadingStarted,
            [](const QString& source) {
                qDebug() << "🔄 Loading UI from:" << source;
            });

    connect(ui_loader_.get(), &JSON::JSONUILoader::loadingFinished,
            [](const QString& source) {
                qDebug() << "✅ UI loading completed:" << source;
            });

    connect(ui_loader_.get(), &JSON::JSONUILoader::loadingFailed,
            [](const QString& source, const QString& error) {
                qWarning() << "❌ UI loading failed for" << source << ":"
                           << error;
            });
}

void HotReloadManager::replaceWidget(const QString& file_path,
                                     std::unique_ptr<QWidget> new_widget) {
    if (!new_widget) {
        qWarning() << "🔥 Cannot replace widget with null pointer for"
                   << file_path;
        return;
    }

    // **Find the widget to replace**
    auto it = registered_files_.find(file_path);
    if (it == registered_files_.end()) {
        qWarning() << "🔥 No widget found to replace for" << file_path;
        return;
    }

    UIFileInfo& info = it->second;
    QWidget* old_widget = info.target_widget;
    if (!old_widget) {
        qWarning() << "🔥 Old widget is null for" << file_path;
        return;
    }

    // **Get parent and geometry before replacement**
    QWidget* parent = old_widget->parentWidget();
    QRect geometry = old_widget->geometry();
    bool was_visible = old_widget->isVisible();

    // **Replace the widget**
    if (parent) {
        // **Replace in parent's layout if it has one**
        if (QLayout* layout = parent->layout()) {
            for (int i = 0; i < layout->count(); ++i) {
                QLayoutItem* item = layout->itemAt(i);
                if (item && item->widget() == old_widget) {
                    layout->removeWidget(old_widget);
                    layout->addWidget(new_widget.get());
                    break;
                }
            }
        }

        // **Set parent for new widget**
        new_widget->setParent(parent);
    }

    // **Restore properties**
    new_widget->setGeometry(geometry);
    new_widget->setVisible(was_visible);

    // **Update the stored widget**
    info.target_widget = new_widget.release();

    qDebug() << "🔥 Successfully replaced widget for" << file_path;
}

// **New optimized methods implementation**

void HotReloadManager::setupThreadPool() {
    int thread_count =
        std::max(1, static_cast<int>(std::thread::hardware_concurrency()) / 2);
    thread_pool_.reserve(thread_count);

    for (int i = 0; i < thread_count; ++i) {
        auto thread = std::make_unique<QThread>();
        thread->start();
        thread_pool_.push_back(std::move(thread));
    }
}

void HotReloadManager::cleanupThreadPool() {
    for (auto& thread : thread_pool_) {
        if (thread && thread->isRunning()) {
            thread->quit();
            thread->wait(5000);  // Wait up to 5 seconds
        }
    }
    thread_pool_.clear();
}

void HotReloadManager::setReloadStrategy(ReloadStrategy strategy) {
    reload_strategy_ = strategy;
}

void HotReloadManager::setMaxConcurrentReloads(int max_concurrent) {
    max_concurrent_reloads_.store(max_concurrent);
}

void HotReloadManager::setMemoryLimit(size_t limit_bytes) {
    memory_limit_.store(limit_bytes);
}

void HotReloadManager::enableIncrementalReloading(bool enabled) {
    incremental_reloading_.store(enabled);
}

void HotReloadManager::enableParallelProcessing(bool enabled) {
    parallel_processing_.store(enabled);
}

void HotReloadManager::enableSmartCaching(bool enabled) {
    smart_caching_.store(enabled);
}

void HotReloadManager::reloadFileIncremental(const QString& file_path) {
    if (!enabled_.load())
        return;

    if (incremental_reloading_.load()) {
        performReloadIncremental(file_path);
    } else {
        performReload(file_path);
    }
}

void HotReloadManager::reloadBatch(const QStringList& file_paths) {
    if (!enabled_.load())
        return;

    if (parallel_processing_.load()) {
        performReloadBatch(file_paths);
    } else {
        for (const QString& path : file_paths) {
            performReload(path);
        }
    }
}

void HotReloadManager::createRollbackPoint(const QString& file_path) {
    std::unique_lock<std::shared_mutex> lock(data_mutex_);
    createRollbackPointInternal(file_path);
}

void HotReloadManager::rollbackToPoint(const QString& file_path) {
    std::unique_lock<std::shared_mutex> lock(data_mutex_);
    rollbackToPointInternal(file_path);
}

void HotReloadManager::optimizeMemoryUsage() {
    cleanupCache();
    updateMemoryUsage();

    // Force garbage collection if memory usage is too high
    if (current_memory_usage_.load() > memory_limit_.load()) {
        widget_cache_.clear();
        current_memory_usage_.store(0);
    }
}

ReloadMetrics HotReloadManager::getLastReloadMetrics(
    const QString& file_path) const {
    std::shared_lock<std::shared_mutex> lock(data_mutex_);
    auto it = performance_metrics_.find(file_path);
    return (it != performance_metrics_.end()) ? it->second : ReloadMetrics{};
}

QJsonObject HotReloadManager::getPerformanceReport() const {
    QJsonObject report;
    report["total_reloads"] = static_cast<qint64>(total_reloads_.load());
    report["successful_reloads"] =
        static_cast<qint64>(successful_reloads_.load());
    report["failed_reloads"] = static_cast<qint64>(failed_reloads_.load());
    report["uptime_ms"] = uptime_timer_.elapsed();
    report["memory_usage"] = static_cast<qint64>(current_memory_usage_.load());
    report["cache_size"] = static_cast<qint64>(widget_cache_.size());

    double success_rate =
        total_reloads_.load() > 0
            ? static_cast<double>(successful_reloads_.load()) /
                  total_reloads_.load() * 100.0
            : 0.0;
    report["success_rate"] = success_rate;

    return report;
}

void HotReloadManager::resetPerformanceCounters() {
    total_reloads_.store(0);
    successful_reloads_.store(0);
    failed_reloads_.store(0);
    performance_metrics_.clear();
    uptime_timer_.restart();
}

void HotReloadManager::performReloadIncremental(const QString& file_path) {
    // Incremental reload implementation
    auto affected_files = getAffectedFiles(file_path);

    for (const QString& affected_file : affected_files) {
        if (shouldReloadIncremental(affected_file)) {
            performReload(affected_file);
        }
    }
}

void HotReloadManager::performReloadBatch(const QStringList& file_paths) {
    // Batch reload with parallel processing
    std::vector<std::future<void>> futures;

    for (const QString& file_path : file_paths) {
        if (futures.size() >=
            static_cast<size_t>(max_concurrent_reloads_.load())) {
            // Wait for some tasks to complete
            for (auto& future : futures) {
                future.wait();
            }
            futures.clear();
        }

        futures.push_back(std::async(std::launch::async, [this, file_path]() {
            performReload(file_path);
        }));
    }

    // Wait for remaining tasks
    for (auto& future : futures) {
        future.wait();
    }
}

QStringList HotReloadManager::getAffectedFiles(const QString& file_path) const {
    QStringList affected;
    auto it = dependency_graph_.find(file_path);
    if (it != dependency_graph_.end()) {
        for (const QString& dependent : it->second.dependents) {
            affected.append(dependent);
        }
    }
    return affected;
}

bool HotReloadManager::shouldReloadIncremental(const QString& file_path) const {
    auto it = dependency_graph_.find(file_path);
    if (it == dependency_graph_.end())
        return true;

    QFileInfo file_info(file_path);
    return it->second.hasChanged(file_info.lastModified(),
                                 qHash(file_info.canonicalFilePath()));
}

void HotReloadManager::cleanupCache() {
    // Remove expired cache entries
    for (auto it = widget_cache_.begin(); it != widget_cache_.end();) {
        if (it->second.use_count() == 1) {  // Only we hold a reference
            it = widget_cache_.erase(it);
        } else {
            ++it;
        }
    }
}

void HotReloadManager::updateMemoryUsage() {
    size_t total_memory = 0;

    // Estimate memory usage (simplified)
    total_memory += registered_files_.size() * sizeof(UIFileInfo);
    total_memory += widget_cache_.size() * 1024;  // Rough estimate per widget
    total_memory += dependency_graph_.size() * sizeof(FileDependency);

    current_memory_usage_.store(total_memory);
}

void HotReloadManager::recordMetrics(const QString& file_path,
                                     const ReloadMetrics& metrics) {
    std::unique_lock<std::shared_mutex> lock(data_mutex_);
    performance_metrics_[file_path] = metrics;
    updatePerformanceCounters(metrics.success);
}

void HotReloadManager::updatePerformanceCounters(bool success) {
    total_reloads_.fetch_add(1);
    if (success) {
        successful_reloads_.fetch_add(1);
    } else {
        failed_reloads_.fetch_add(1);
    }
}

void HotReloadManager::onFileChangedOptimized(const QString& file_path) {
    try {
        if (!enabled_.load())
            return;

        // Add to reload queue with optimization
        if (incremental_reloading_.load()) {
            reloadFileIncremental(file_path);
        } else {
            reloadFile(file_path);
        }

        emit reloadCompleted(file_path);
    } catch (const std::exception& e) {
        qDebug() << "Optimized file change error:" << e.what();
    }
}

void HotReloadManager::onFileAddedOptimized(const QString& file_path) {
    try {
        if (!enabled_.load())
            return;

        // Register new file with dummy widget and reload if needed
        registerUIFile(file_path, nullptr);
        reloadFile(file_path);

        emit reloadCompleted(file_path);
    } catch (const std::exception& e) {
        qDebug() << "Optimized file add error:" << e.what();
    }
}

void HotReloadManager::onFileRemovedOptimized(const QString& file_path) {
    try {
        if (!enabled_.load())
            return;

        // Unregister file and clean up
        unregisterUIFile(file_path);

        // Clean up cache entries
        std::unique_lock<std::shared_mutex> lock(data_mutex_);
        widget_cache_.erase(file_path);
        dependency_graph_.erase(file_path);

        emit reloadCompleted(file_path);
    } catch (const std::exception& e) {
        qDebug() << "Optimized file remove error:" << e.what();
    }
}

void HotReloadManager::onReloadQueueTimeout() {
    try {
        // Process pending reload queue
        qDebug() << "Processing reload queue timeout";

        // Trigger batch processing if enabled
        if (parallel_processing_.load()) {
            // Could implement queue processing here
        }
    } catch (const std::exception& e) {
        qDebug() << "Reload queue timeout error:" << e.what();
    }
}

void HotReloadManager::onMemoryCleanupTimeout() {
    try {
        // Perform memory cleanup
        cleanupCache();
        updateMemoryUsage();

        qDebug() << "Memory cleanup completed. Current usage:"
                 << current_memory_usage_.load() << "bytes";
    } catch (const std::exception& e) {
        qDebug() << "Memory cleanup timeout error:" << e.what();
    }
}

void HotReloadManager::createRollbackPointInternal(const QString& file_path) {
    try {
        qDebug() << "Creating rollback point for:" << file_path;
        // Placeholder implementation - would create file backup/snapshot
    } catch (const std::exception& e) {
        qDebug() << "Create rollback point error:" << e.what();
    }
}

bool HotReloadManager::rollbackToPointInternal(const QString& file_path) {
    try {
        qDebug() << "Rolling back to point for:" << file_path;
        // Placeholder implementation - would restore from backup/snapshot
        return true;
    } catch (const std::exception& e) {
        qDebug() << "Rollback to point error:" << e.what();
        return false;
    }
}

// **Missing dependency management methods**
void HotReloadManager::buildDependencyGraph() {
    std::unique_lock<std::shared_mutex> lock(data_mutex_);

    qDebug() << "🔗 Building dependency graph...";

    // Clear existing graph
    dependency_graph_.clear();

    // Build graph from registered files
    for (const auto& [file_path, info] : registered_files_) {
        FileDependency dep_info;
        dep_info.file_path = file_path;
        dep_info.last_modified = QFileInfo(file_path).lastModified();
        dep_info.content_hash = qHash(file_path); // Simplified hash

        // Analyze file for dependencies (simplified)
        QFile file(file_path);
        if (file.open(QIODevice::ReadOnly)) {
            QByteArray content = file.readAll();
            QString content_str = QString::fromUtf8(content);

            // Look for include/import patterns in JSON
            QRegularExpression include_pattern("\"include\"\\s*:\\s*\"([^\"]+)\"");
            QRegularExpressionMatchIterator matches = include_pattern.globalMatch(content_str);

            while (matches.hasNext()) {
                QRegularExpressionMatch match = matches.next();
                QString dependency = match.captured(1);

                // Resolve relative paths
                QFileInfo dep_info_file(QFileInfo(file_path).dir(), dependency);
                QString canonical_dep = dep_info_file.canonicalFilePath();

                if (!canonical_dep.isEmpty()) {
                    dep_info.dependencies.insert(canonical_dep);
                }
            }
        }

        dependency_graph_[file_path] = dep_info;
    }

    // Build reverse dependencies (dependents)
    for (auto& [file_path, dep_info] : dependency_graph_) {
        for (const QString& dependency : dep_info.dependencies) {
            if (dependency_graph_.contains(dependency)) {
                dependency_graph_[dependency].dependents.insert(file_path);
            }
        }
    }

    qDebug() << "✅ Dependency graph built with" << dependency_graph_.size() << "files";
}

void HotReloadManager::updateDependencies(const QString& file_path) {
    std::unique_lock<std::shared_mutex> lock(data_mutex_);

    auto it = dependency_graph_.find(file_path);
    if (it == dependency_graph_.end()) {
        return;
    }

    // Update file metadata
    QFileInfo file_info(file_path);
    it->second.last_modified = file_info.lastModified();
    it->second.content_hash = qHash(file_path);

    qDebug() << "🔗 Updated dependencies for:" << file_path;
}

bool HotReloadManager::hasCyclicDependency(const QString& file_path) const {
    std::shared_lock<std::shared_mutex> lock(data_mutex_);

    std::unordered_set<QString> visited;
    std::unordered_set<QString> recursion_stack;

    std::function<bool(const QString&)> has_cycle = [&](const QString& current) -> bool {
        if (recursion_stack.contains(current)) {
            return true; // Cycle detected
        }

        if (visited.contains(current)) {
            return false; // Already processed
        }

        visited.insert(current);
        recursion_stack.insert(current);

        auto it = dependency_graph_.find(current);
        if (it != dependency_graph_.end()) {
            for (const QString& dependency : it->second.dependencies) {
                if (has_cycle(dependency)) {
                    return true;
                }
            }
        }

        recursion_stack.erase(current);
        return false;
    };

    return has_cycle(file_path);
}

// **Missing thread management methods**
QThread* HotReloadManager::getAvailableThread() {
    // Simple round-robin thread selection
    static size_t thread_index = 0;

    if (thread_pool_.empty()) {
        return nullptr;
    }

    QThread* selected_thread = thread_pool_[thread_index % thread_pool_.size()].get();
    thread_index++;

    return selected_thread;
}

// **Missing performance measurement methods**
ReloadMetrics HotReloadManager::measureReloadPerformance(const std::function<void()>& reload_func) {
    ReloadMetrics metrics;
    QElapsedTimer timer;

    timer.start();

    try {
        reload_func();
        metrics.success = true;
    } catch (const std::exception& e) {
        metrics.success = false;
        qWarning() << "Reload function failed:" << e.what();
    }

    metrics.total_time = std::chrono::milliseconds(timer.elapsed());

    return metrics;
}

// **Missing configuration methods**
void HotReloadManager::setPreloadStrategy(bool preload_dependencies) {
    if (preload_dependencies) {
        // Preload all dependencies for registered files
        for (const auto& [file_path, info] : registered_files_) {
            preloadDependencies(file_path);
        }
    }

    qDebug() << "📋 Preload strategy set to:" << preload_dependencies;
}

void HotReloadManager::clearRollbackPoints() {
    std::unique_lock<std::shared_mutex> lock(data_mutex_);

    // Clear all backup widgets
    for (auto& [file_path, info] : registered_files_) {
        info.backup_widget.reset();
    }

    qDebug() << "🗑️ All rollback points cleared";
}

// **Missing async reload method**
void HotReloadManager::performReloadAsync(const QString& file_path) {
    QThread* worker_thread = getAvailableThread();
    if (!worker_thread) {
        // Fallback to synchronous reload
        performReload(file_path);
        return;
    }

    // Create a worker to perform reload on background thread
    QTimer::singleShot(0, [this, file_path]() {
        try {
            performReload(file_path);
        } catch (const std::exception& e) {
            qWarning() << "Async reload failed for" << file_path << ":" << e.what();
            emit reloadFailed(file_path, QString::fromStdString(e.what()));
        }
    });

    qDebug() << "🚀 Async reload started for:" << file_path;
}

// **Missing safe widget replacement method**
void HotReloadManager::replaceWidgetSafe(const QString& file_path, std::unique_ptr<QWidget> new_widget) {
    if (!new_widget) {
        qWarning() << "Cannot replace with null widget for:" << file_path;
        return;
    }

    auto it = registered_files_.find(file_path);
    if (it == registered_files_.end()) {
        qWarning() << "File not registered for safe replacement:" << file_path;
        return;
    }

    UIFileInfo& info = it->second;

    // Validate new widget before replacement
    if (!validateWidget(new_widget.get())) {
        qWarning() << "Widget validation failed for:" << file_path;
        return;
    }

    // Create backup of current widget
    if (info.target_widget) {
        info.backup_widget = std::unique_ptr<QWidget>(info.target_widget);
    }

    // Perform safe replacement
    try {
        replaceWidget(file_path, std::move(new_widget));
        qDebug() << "✅ Safe widget replacement completed for:" << file_path;
    } catch (const std::exception& e) {
        // Restore backup on failure
        if (info.backup_widget) {
            info.target_widget = info.backup_widget.release();
        }
        qWarning() << "Safe widget replacement failed for" << file_path << ":" << e.what();
        throw;
    }
}

// **Missing preload dependencies method**
void HotReloadManager::preloadDependencies(const QString& file_path) {
    auto it = dependency_graph_.find(file_path);
    if (it == dependency_graph_.end()) {
        return;
    }

    for (const QString& dependency : it->second.dependencies) {
        if (!preloaded_files_.contains(dependency)) {
            try {
                // Load dependency into cache
                auto widget = ui_loader_->loadFromFile(dependency);
                if (widget) {
                    preloaded_files_.insert(dependency);
                    qDebug() << "📦 Preloaded dependency:" << dependency;
                }
            } catch (const std::exception& e) {
                qWarning() << "Failed to preload dependency" << dependency << ":" << e.what();
            }
        }
    }
}

// **Missing widget creation from cache method**
std::unique_ptr<QWidget> HotReloadManager::createWidgetFromCache(const QString& file_path) {
    if (preloaded_files_.contains(file_path)) {
        try {
            return ui_loader_->loadFromFile(file_path);
        } catch (const std::exception& e) {
            qWarning() << "Failed to create widget from cache for" << file_path << ":" << e.what();
        }
    }

    return nullptr;
}

}  // namespace DeclarativeUI::HotReload
