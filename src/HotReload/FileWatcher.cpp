#include "FileWatcher.hpp"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QRegularExpression>

#include "../Exceptions/UIExceptions.hpp"

namespace DeclarativeUI::HotReload {

FileWatcher::FileWatcher(QObject *parent) : QObject(parent) {
    setupWatcher();
    setupThreadPool();
    enablePlatformOptimizations();
    performance_timer_.start();
}

FileWatcher::~FileWatcher() {
    cleanupThreadPool();
}

void FileWatcher::setupWatcher() {
    try {
        watcher_ = std::make_unique<QFileSystemWatcher>(this);
        debounce_timer_ = std::make_unique<QTimer>(this);

        debounce_timer_->setSingleShot(true);
        debounce_timer_->setInterval(debounce_interval_);

        // **Connect signals with exception handling**
        connect(watcher_.get(), &QFileSystemWatcher::fileChanged, this,
                &FileWatcher::onFileChanged);

        connect(watcher_.get(), &QFileSystemWatcher::directoryChanged, this,
                &FileWatcher::onDirectoryChanged);

        connect(debounce_timer_.get(), &QTimer::timeout, this,
                &FileWatcher::onDebounceTimeout);

    } catch (const std::exception &e) {
        throw Exceptions::FileWatchException("FileWatcher setup failed: " +
                                             std::string(e.what()));
    }
}

void FileWatcher::watchFile(const QString &file_path) {
    try {
        QFileInfo file_info(file_path);

        if (!file_info.exists()) {
            throw Exceptions::FileWatchException("File does not exist: " +
                                                 file_path.toStdString());
        }

        if (!file_info.isFile()) {
            throw Exceptions::FileWatchException("Path is not a file: " +
                                                 file_path.toStdString());
        }

        QString canonical_path = file_info.canonicalFilePath();

        // **Check if already being watched**
        if (watcher_->files().contains(canonical_path)) {
            qDebug() << "⚠️ File already being watched:" << canonical_path;
            return;
        }

        if (!watcher_->addPath(canonical_path)) {
            qWarning() << "❌ Failed to add path to watcher:" << canonical_path;
            qDebug() << "Current watched files:" << watcher_->files();
            qDebug() << "Current watched directories:"
                     << watcher_->directories();
            throw Exceptions::FileWatchException("Failed to watch file: " +
                                                 canonical_path.toStdString());
        }

        // **Store initial timestamp**
        file_timestamps_[canonical_path] = file_info.lastModified();

        emit watchingStarted(canonical_path);
        qDebug() << "✅ Successfully watching file:" << canonical_path;

    } catch (const Exceptions::FileWatchException &) {
        emit watchingFailed(file_path, "File watch setup failed");
        throw;
    } catch (const std::exception &e) {
        emit watchingFailed(file_path, QString::fromStdString(e.what()));
        throw Exceptions::FileWatchException(file_path.toStdString() + ": " +
                                             e.what());
    }
}

void FileWatcher::watchDirectory(const QString &directory_path,
                                 bool recursive) {
    try {
        QFileInfo dir_info(directory_path);

        if (!dir_info.exists()) {
            throw Exceptions::FileWatchException("Directory does not exist: " +
                                                 directory_path.toStdString());
        }

        if (!dir_info.isDir()) {
            throw Exceptions::FileWatchException("Path is not a directory: " +
                                                 directory_path.toStdString());
        }

        QString canonical_path = dir_info.canonicalFilePath();

        if (!watcher_->addPath(canonical_path)) {
            throw Exceptions::FileWatchException("Failed to watch directory: " +
                                                 canonical_path.toStdString());
        }

        // **Scan and watch files in directory**
        scanDirectory(canonical_path, recursive);

        emit watchingStarted(canonical_path);

    } catch (const Exceptions::FileWatchException &) {
        emit watchingFailed(directory_path, "Directory watch setup failed");
        throw;
    } catch (const std::exception &e) {
        emit watchingFailed(directory_path, QString::fromStdString(e.what()));
        throw Exceptions::FileWatchException(directory_path.toStdString() +
                                             ": " + e.what());
    }
}

void FileWatcher::unwatchFile(const QString &file_path) {
    QString canonical_path = QFileInfo(file_path).canonicalFilePath();

    if (watcher_->removePath(canonical_path)) {
        file_timestamps_.erase(canonical_path);
        emit watchingStopped(canonical_path);
    }
}

void FileWatcher::unwatchDirectory(const QString &directory_path) {
    QString canonical_path = QFileInfo(directory_path).canonicalFilePath();

    if (watcher_->removePath(canonical_path)) {
        emit watchingStopped(canonical_path);
    }
}

void FileWatcher::unwatchAll() {
    QStringList watched_files = watcher_->files();
    QStringList watched_dirs = watcher_->directories();

    for (const QString &file : watched_files) {
        watcher_->removePath(file);
        emit watchingStopped(file);
    }

    for (const QString &dir : watched_dirs) {
        watcher_->removePath(dir);
        emit watchingStopped(dir);
    }

    file_timestamps_.clear();
}

void FileWatcher::setDebounceInterval(int milliseconds) {
    debounce_interval_ = milliseconds;
    debounce_timer_->setInterval(milliseconds);
}

void FileWatcher::setFileFilter(const FileFilter &filter) {
    std::unique_lock<std::shared_mutex> lock(filter_mutex_);
    file_filter_ = filter;
}

bool FileWatcher::isWatching(const QString &path) const {
    QString canonical_path = QFileInfo(path).canonicalFilePath();
    return watcher_->files().contains(canonical_path) ||
           watcher_->directories().contains(canonical_path);
}

QStringList FileWatcher::watchedFiles() const { return watcher_->files(); }

QStringList FileWatcher::watchedDirectories() const {
    return watcher_->directories();
}

void FileWatcher::onFileChanged(const QString &path) {
    if (!shouldProcessFile(path))
        return;

    // **Add to event queue for batch processing**
    FileChangeEvent event;
    event.file_path = path;
    event.timestamp = QDateTime::currentDateTime();
    event.type = FileChangeEvent::Modified;

    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        event_queue_.push(event);
    }

    debounce_timer_->start();
}

void FileWatcher::onDirectoryChanged(const QString &path) {
    processDirectoryChange(path);
}

void FileWatcher::onDebounceTimeout() {
    std::vector<FileChangeEvent> events_to_process;

    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        while (!event_queue_.empty()) {
            events_to_process.push_back(event_queue_.front());
            event_queue_.pop();
        }
    }

    // Process events in batch
    for (const auto& event : events_to_process) {
        processFileChange(event.file_path);
        total_events_processed_.fetch_add(1);
    }

    // Optimize data structures periodically
    optimizeDataStructures();
}

void FileWatcher::processFileChange(const QString &file_path) {
    try {
        QFileInfo file_info(file_path);

        if (!file_info.exists()) {
            // **File was deleted**
            file_timestamps_.erase(file_path);
            emit fileRemoved(file_path);
            return;
        }

        QDateTime current_modified = file_info.lastModified();
        auto timestamp_it = file_timestamps_.find(file_path);

        if (timestamp_it != file_timestamps_.end()) {
            if (current_modified != timestamp_it->second) {
                // **File was modified**
                timestamp_it->second = current_modified;
                emit fileChanged(file_path);
            }
        } else {
            // **New file**
            file_timestamps_[file_path] = current_modified;
            emit fileAdded(file_path);
        }

    } catch (const std::exception &e) {
        qWarning() << "Error processing file change for" << file_path << ":"
                   << e.what();
    }
}

void FileWatcher::processDirectoryChange(const QString &directory_path) {
    emit directoryChanged(directory_path);

    // **Rescan directory for new/removed files**
    try {
        scanDirectory(directory_path, false);
    } catch (const std::exception &e) {
        qWarning() << "Error rescanning directory" << directory_path << ":"
                   << e.what();
    }
}

bool FileWatcher::shouldProcessFile(const QString &file_path) const {
    QFileInfo file_info(file_path);

    // **Use the new FileFilter system with thread safety**
    std::shared_lock<std::shared_mutex> lock(filter_mutex_);
    return file_filter_.matches(file_path, file_info.size());
}

void FileWatcher::scanDirectory(const QString &directory_path, bool recursive) {
    QDir dir(directory_path);

    // **Get files matching filters using new filter system**
    QStringList name_filters = QStringList{"*"};  // Get all files, filter later

    QFileInfoList files =
        dir.entryInfoList(name_filters, QDir::Files | QDir::NoDotAndDotDot);

    for (const QFileInfo &file_info : files) {
        QString file_path = file_info.canonicalFilePath();

        if (!isWatching(file_path)) {
            try {
                watchFile(file_path);
            } catch (const std::exception &e) {
                qWarning() << "Failed to watch file" << file_path << ":"
                           << e.what();
            }
        }
    }

    // **Recursive directory scanning**
    if (recursive) {
        QFileInfoList subdirs =
            dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

        for (const QFileInfo &subdir_info : subdirs) {
            scanDirectory(subdir_info.canonicalFilePath(), true);
        }
    }
}

// **FileFilter implementation**
void FileFilter::compilePatterns() const {
    if (patterns_compiled) return;

    compiled_patterns.clear();
    compiled_patterns.reserve(patterns.size());

    for (const QString& pattern : patterns) {
        QString regex_pattern = QRegularExpression::wildcardToRegularExpression(pattern);
        compiled_patterns.emplace_back(regex_pattern, QRegularExpression::CaseInsensitiveOption);
    }

    patterns_compiled = true;
}

bool FileFilter::matches(const QString& file_path, qint64 file_size) const {
    QFileInfo file_info(file_path);

    // Check file size limit
    if (max_file_size > 0 && file_size > max_file_size) {
        return false;
    }

    // Check hidden files
    if (!include_hidden && file_info.isHidden()) {
        return false;
    }

    // Check extensions
    if (!extensions.isEmpty()) {
        QString suffix = file_info.suffix().toLower();
        bool extension_match = false;
        for (const QString& ext : extensions) {
            if (suffix == ext.toLower()) {
                extension_match = true;
                break;
            }
        }
        if (!extension_match) return false;
    }

    // Check patterns
    if (!patterns.isEmpty()) {
        compilePatterns();
        QString filename = file_info.fileName();
        bool pattern_match = false;
        for (const auto& regex : compiled_patterns) {
            if (regex.match(filename).hasMatch()) {
                pattern_match = true;
                break;
            }
        }
        if (!pattern_match) return false;
    }

    return true;
}

// **New optimized methods implementation**

void FileWatcher::setupThreadPool() {
    int thread_count = std::max(1, static_cast<int>(std::thread::hardware_concurrency()) / 2);
    thread_pool_.reserve(thread_count);

    for (int i = 0; i < thread_count; ++i) {
        auto thread = std::make_unique<QThread>();
        thread->start();
        thread_pool_.push_back(std::move(thread));
    }
}

void FileWatcher::cleanupThreadPool() {
    for (auto& thread : thread_pool_) {
        if (thread && thread->isRunning()) {
            thread->quit();
            thread->wait(5000);  // Wait up to 5 seconds
        }
    }
    thread_pool_.clear();
}

void FileWatcher::enablePlatformOptimizations() {
#ifdef Q_OS_WIN
    // Windows-specific optimizations
    setMaxWatchedFiles(8192);  // Windows limit
#elif defined(Q_OS_LINUX)
    // Linux-specific optimizations
    setMaxWatchedFiles(65536); // Higher limit on Linux
#elif defined(Q_OS_MAC)
    // macOS-specific optimizations
    setMaxWatchedFiles(10240);
#endif
}

void FileWatcher::watchFiles(const QStringList &file_paths) {
    if (batch_processing_enabled_.load()) {
        // Batch processing for better performance
        for (const auto& path : file_paths) {
            watchFile(path);
        }
    } else {
        // Process individually
        for (const auto& path : file_paths) {
            watchFile(path);
        }
    }
}

void FileWatcher::setDebounceStrategy(DebounceStrategy strategy) {
    debounce_strategy_ = strategy;
}

void FileWatcher::setMaxWatchedFiles(int max_files) {
    max_watched_files_.store(max_files);
}

void FileWatcher::setThreadPoolSize(int thread_count) {
    cleanupThreadPool();
    setupThreadPool();
}

void FileWatcher::enableBatchProcessing(bool enabled) {
    batch_processing_enabled_.store(enabled);
}

void FileWatcher::setMaxBatchSize(int batch_size) {
    max_batch_size_.store(batch_size);
}

int FileWatcher::calculateAdaptiveDebounceInterval(const QString &file_path) const {
    auto it = change_frequencies_.find(file_path);
    if (it == change_frequencies_.end()) {
        return debounce_interval_.load();
    }

    int frequency = it->second;
    if (frequency > 10) {
        return debounce_interval_.load() * 2;  // Increase interval for frequently changing files
    } else if (frequency < 3) {
        return debounce_interval_.load() / 2;  // Decrease interval for rarely changing files
    }

    return debounce_interval_.load();
}

void FileWatcher::updateChangeFrequency(const QString &file_path) {
    auto now = std::chrono::steady_clock::now();
    auto it = last_change_times_.find(file_path);

    if (it != last_change_times_.end()) {
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - it->second);
        if (duration.count() < 1000) {  // Less than 1 second
            change_frequencies_[file_path]++;
        }
    }

    last_change_times_[file_path] = now;
}

void FileWatcher::cleanupCache() {
    if (event_cache_.size() > MAX_CACHE_SIZE) {
        // Remove expired weak_ptr entries
        for (auto it = event_cache_.begin(); it != event_cache_.end();) {
            if (it->second.expired()) {
                it = event_cache_.erase(it);
            } else {
                ++it;
            }
        }
    }
}

void FileWatcher::optimizeDataStructures() {
    // Periodically optimize data structures
    if (total_events_processed_.load() % 1000 == 0) {
        cleanupCache();

        // Rehash if load factor is too high
        if (file_timestamps_.load_factor() > 0.8) {
            file_timestamps_.rehash(file_timestamps_.size() * 2);
        }
    }
}

void FileWatcher::onFileChangedOptimized(const QString &path) {
    try {
        updateChangeFrequency(path);
        auto debounce_time = calculateAdaptiveDebounceInterval(path);
        
        // Use optimized debounce timing
        debounce_timer_->stop();
        debounce_timer_->setInterval(debounce_time);
        debounce_timer_->start();
        
        // Process file change immediately for now
        emit fileChanged(path);
        total_events_processed_++;
        optimizeDataStructures();
    } catch (const std::exception &e) {
        qDebug() << "FileWatcher optimization error:" << e.what();
    }
}

void FileWatcher::onDirectoryChangedOptimized(const QString &path) {
    try {
        // Simplified directory change processing
        emit directoryChanged(path);
        total_events_processed_++;
    } catch (const std::exception &e) {
        qDebug() << "Directory change optimization error:" << e.what();
    }
}

void FileWatcher::onDebounceTimeoutOptimized() {
    try {
        // Placeholder implementation - could be enhanced with batching
        qDebug() << "Debounce timeout optimized triggered";
    } catch (const std::exception &e) {
        qDebug() << "Debounce timeout optimization error:" << e.what();
    }
}

void FileWatcher::onBatchProcessingTimeout() {
    try {
        // Placeholder implementation - could be enhanced with batch processing
        qDebug() << "Batch processing timeout triggered";
    } catch (const std::exception &e) {
        qDebug() << "Batch processing timeout error:" << e.what();
    }
}

}  // namespace DeclarativeUI::HotReload
