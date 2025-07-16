#include "FileWatcher.hpp"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QRegularExpression>

#include "../Exceptions/UIExceptions.hpp"

namespace DeclarativeUI::HotReload {

FileWatcher::FileWatcher(QObject *parent) : QObject(parent) { setupWatcher(); }

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

void FileWatcher::setFileFilters(const QStringList &filters) {
    file_filters_ = filters;
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

    // **Add to pending changes for debouncing**
    if (!pending_changes_.contains(path)) {
        pending_changes_.append(path);
    }

    debounce_timer_->start();
}

void FileWatcher::onDirectoryChanged(const QString &path) {
    processDirectoryChange(path);
}

void FileWatcher::onDebounceTimeout() {
    // **Process all pending changes**
    for (const QString &path : pending_changes_) {
        processFileChange(path);
    }

    pending_changes_.clear();
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
    if (file_filters_.isEmpty())
        return true;

    QFileInfo file_info(file_path);
    QString suffix = file_info.suffix().toLower();

    for (const QString &filter : file_filters_) {
        if (filter.contains('*')) {
            // **Wildcard matching using QRegularExpression**
            QString pattern =
                QRegularExpression::wildcardToRegularExpression(filter);
            QRegularExpression regex(pattern,
                                     QRegularExpression::CaseInsensitiveOption);
            if (regex.match(file_info.fileName()).hasMatch()) {
                return true;
            }
        } else if (suffix == filter.toLower()) {
            return true;
        }
    }

    return false;
}

void FileWatcher::scanDirectory(const QString &directory_path, bool recursive) {
    QDir dir(directory_path);

    // **Get files matching filters**
    QStringList name_filters =
        file_filters_.isEmpty() ? QStringList{"*"} : file_filters_;

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

}  // namespace DeclarativeUI::HotReload
