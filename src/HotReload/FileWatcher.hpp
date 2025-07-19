#pragma once

#include <QDateTime>
#include <QFileSystemWatcher>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <QThread>
#include <QMutex>
#include <QReadWriteLock>
#include <QAtomicInt>
#include <QElapsedTimer>

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <atomic>
#include <chrono>
#include <thread>
#include <vector>
#include <queue>
#include <functional>
#include <shared_mutex>

namespace DeclarativeUI::HotReload {

// **Advanced file change event with metadata**
struct FileChangeEvent {
    QString file_path;
    QDateTime timestamp;
    qint64 file_size;
    QDateTime last_modified;
    enum ChangeType { Modified, Added, Removed, Renamed } type;

    // **Performance optimization: pre-computed hash**
    mutable std::size_t path_hash = 0;

    std::size_t getPathHash() const {
        if (path_hash == 0) {
            path_hash = qHash(file_path);
        }
        return path_hash;
    }
};

// **Intelligent debouncing strategy**
enum class DebounceStrategy {
    Fixed,        // Fixed interval
    Adaptive,     // Adapts based on file change frequency
    Exponential,  // Exponential backoff
    Smart         // AI-based prediction
};

// **File filter with performance optimizations**
struct FileFilter {
    QStringList extensions;
    QStringList patterns;
    qint64 max_file_size = -1;  // -1 means no limit
    bool include_hidden = false;

    // **Compiled regex patterns for performance**
    mutable std::vector<QRegularExpression> compiled_patterns;
    mutable bool patterns_compiled = false;

    void compilePatterns() const;
    bool matches(const QString& file_path, qint64 file_size) const;
};

// **High-performance file change detection with advanced features**
class FileWatcher : public QObject {
    Q_OBJECT

public:
    explicit FileWatcher(QObject *parent = nullptr);
    ~FileWatcher() override;

    // **Enhanced watch methods with performance optimizations**
    void watchFile(const QString &file_path);
    void watchDirectory(const QString &directory_path, bool recursive = false);

    // **Batch operations for better performance**
    void watchFiles(const QStringList &file_paths);
    void watchDirectories(const QStringList &directory_paths, bool recursive = false);

    // **Stop watching with optimized cleanup**
    void unwatchFile(const QString &file_path);
    void unwatchDirectory(const QString &directory_path);
    void unwatchAll();

    // **Advanced configuration**
    void setDebounceInterval(int milliseconds);
    void setDebounceStrategy(DebounceStrategy strategy);
    void setFileFilter(const FileFilter &filter);
    void setMaxWatchedFiles(int max_files);
    void setThreadPoolSize(int thread_count);

    // **Performance tuning**
    void enableBatchProcessing(bool enabled);
    void setMaxBatchSize(int batch_size);
    void setProcessingPriority(QThread::Priority priority);

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
    // **Core watching infrastructure**
    std::unique_ptr<QFileSystemWatcher> watcher_;
    std::unique_ptr<QTimer> debounce_timer_;

    // **Multi-threaded processing**
    std::unique_ptr<QThread> worker_thread_;
    std::vector<std::unique_ptr<QThread>> thread_pool_;

    // **Configuration with atomic access for thread safety**
    std::atomic<int> debounce_interval_{100};
    std::atomic<int> max_watched_files_{10000};
    std::atomic<int> max_batch_size_{100};
    std::atomic<bool> batch_processing_enabled_{true};
    DebounceStrategy debounce_strategy_ = DebounceStrategy::Adaptive;

    // **Advanced file filtering**
    FileFilter file_filter_;
    mutable std::shared_mutex filter_mutex_;

    // **Performance optimized data structures**
    std::unordered_map<QString, QDateTime> file_timestamps_;
    std::unordered_map<QString, qint64> file_sizes_;
    std::unordered_set<QString> watched_files_;
    std::unordered_set<QString> watched_directories_;

    // **Thread-safe event queue with priority**
    std::queue<FileChangeEvent> event_queue_;
    std::priority_queue<FileChangeEvent> priority_queue_;
    mutable std::mutex queue_mutex_;

    // **Adaptive debouncing state**
    std::unordered_map<QString, std::chrono::steady_clock::time_point> last_change_times_;
    std::unordered_map<QString, int> change_frequencies_;

    // **Performance monitoring**
    std::atomic<size_t> total_events_processed_{0};
    std::atomic<size_t> events_filtered_{0};
    QElapsedTimer performance_timer_;

    // **Memory management**
    static constexpr size_t MAX_CACHE_SIZE = 10000;
    std::unordered_map<QString, std::weak_ptr<FileChangeEvent>> event_cache_;

    // **Core methods**
    void setupWatcher();
    void setupThreadPool();
    void cleanupThreadPool();

    // **Enhanced processing methods**
    void processFileChange(const QString &file_path);
    void processDirectoryChange(const QString &directory_path);
    void processBatchChanges(const std::vector<FileChangeEvent> &events);

    // **Filtering and validation**
    bool shouldProcessFile(const QString &file_path) const;
    bool isFileChanged(const QString &file_path, const QDateTime &timestamp, qint64 size) const;

    // **Directory scanning with optimization**
    void scanDirectory(const QString &directory_path, bool recursive);
    void scanDirectoryAsync(const QString &directory_path, bool recursive);

    // **Adaptive debouncing logic**
    int calculateAdaptiveDebounceInterval(const QString &file_path) const;
    void updateChangeFrequency(const QString &file_path);

    // **Memory and performance optimization**
    void cleanupCache();
    void optimizeDataStructures();

    // **Platform-specific optimizations**
    void enablePlatformOptimizations();

private slots:
    void onFileChangedOptimized(const QString &path);
    void onDirectoryChangedOptimized(const QString &path);
    void onDebounceTimeoutOptimized();
    void onBatchProcessingTimeout();
};

}  // namespace DeclarativeUI::HotReload
