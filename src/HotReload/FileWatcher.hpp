#pragma once

#include <QDateTime>
#include <QFileSystemWatcher>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QTimer>

#include <memory>
#include <unordered_map>

namespace DeclarativeUI::HotReload {

// **File change detection with debouncing**
class FileWatcher : public QObject {
    Q_OBJECT

public:
    explicit FileWatcher(QObject *parent = nullptr);
    ~FileWatcher() override = default;

    // **Watch file or directory**
    void watchFile(const QString &file_path);
    void watchDirectory(const QString &directory_path, bool recursive = false);

    // **Stop watching**
    void unwatchFile(const QString &file_path);
    void unwatchDirectory(const QString &directory_path);
    void unwatchAll();

    // **Configuration**
    void setDebounceInterval(int milliseconds);
    void setFileFilters(const QStringList &filters);

    // **Status**
    [[nodiscard]] bool isWatching(const QString &path) const;
    [[nodiscard]] QStringList watchedFiles() const;
    [[nodiscard]] QStringList watchedDirectories() const;

signals:
    void fileChanged(const QString &file_path);
    void fileAdded(const QString &file_path);
    void fileRemoved(const QString &file_path);
    void directoryChanged(const QString &directory_path);
    void watchingStarted(const QString &path);
    void watchingStopped(const QString &path);
    void watchingFailed(const QString &path, const QString &error);

private slots:
    void onFileChanged(const QString &path);
    void onDirectoryChanged(const QString &path);
    void onDebounceTimeout();

private:
    std::unique_ptr<QFileSystemWatcher> watcher_;
    std::unique_ptr<QTimer> debounce_timer_;

    int debounce_interval_ = 100;  // milliseconds
    QStringList file_filters_;

    // **Track file modification times to avoid duplicate events**
    std::unordered_map<QString, QDateTime> file_timestamps_;

    // **Pending changes for debouncing**
    QStringList pending_changes_;

    void setupWatcher();
    void processFileChange(const QString &file_path);
    void processDirectoryChange(const QString &directory_path);
    bool shouldProcessFile(const QString &file_path) const;
    void scanDirectory(const QString &directory_path, bool recursive);
};

}  // namespace DeclarativeUI::HotReload
