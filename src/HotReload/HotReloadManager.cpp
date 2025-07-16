// HotReload/HotReloadManager.cpp
#include "HotReloadManager.hpp"
#include <QApplication>
#include <QBoxLayout>
#include <QDebug>
#include <QFileInfo>
#include <QGridLayout>
#include <QLayout>
#include <QTimer>


namespace DeclarativeUI::HotReload {

HotReloadManager::HotReloadManager(QObject* parent) : QObject(parent) {
    setupUILoader();

    file_watcher_ = std::make_unique<FileWatcher>(this);

    // **Connect file watcher signals**
    connect(file_watcher_.get(), &FileWatcher::fileChanged, this,
            &HotReloadManager::onFileChanged);

    connect(file_watcher_.get(), &FileWatcher::fileAdded, this,
            &HotReloadManager::onFileAdded);

    connect(file_watcher_.get(), &FileWatcher::fileRemoved, this,
            &HotReloadManager::onFileRemoved);
}

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
    file_watcher_->setFileFilters(filters);
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
    QTimer::singleShot(reload_delay_, [this, file_path]() {
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

}  // namespace DeclarativeUI::HotReload
