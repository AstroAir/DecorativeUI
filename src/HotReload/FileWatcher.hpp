#pragma once

#include <QAtomicInt>
#include <QDateTime>
#include <QElapsedTimer>
#include <QFileSystemWatcher>
#include <QMutex>
#include <QObject>
#include <QReadWriteLock>
#include <QRegularExpression>
#include <QString>
#include <QStringList>
#include <QThread>
#include <QTimer>

#include <atomic>
#include <chrono>
#include <memory>
#include <queue>
#include <shared_mutex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace DeclarativeUI::HotReload {

/**
 * @file FileWatcher.hpp
 * @brief High-performance file and directory change detection with advanced
 * features.
 *
 * This header declares FileWatcher, a feature-rich Qt-based file watching
 * component intended for hot-reload and development workflows. It provides:
 *  - efficient monitoring of files and directories (recursive optional),
 *  - configurable debouncing strategies to coalesce noisy file system events,
 *  - filtering by extension, glob-style patterns, file size and hidden flag,
 *  - batch processing and a thread-pool for off-main-thread work,
 *  - adaptive heuristics to tune debounce intervals based on change frequency,
 *  - lightweight performance counters and caches to reduce repeated IO.
 *
 * Threading and safety:
 *  - The class is implemented with worker threads and internal synchronization.
 *  - Public query/configuration methods are safe to call from other threads,
 *    but signal emission adheres to Qt threading rules (emitted on QObject
 *    thread unless explicitly moved).
 *
 * Typical usage:
 *  - instantiate FileWatcher on a QObject-managed thread (usually main thread),
 *  - configure filters and debounce strategy,
 *  - call watchFile/watchDirectory or batch variants,
 *  - connect to signals (fileChanged/fileAdded/...) to react to changes.
 *
 * Note: Implementation strives for cross-platform behaviour but may enable
 * platform-specific optimizations where available.
 */

/**
 * @struct FileChangeEvent
 * @brief Represents a single file system change event with metadata.
 *
 * Fields:
 *  - file_path: canonical path of the affected file.
 *  - timestamp: recorded timestamp when the change was observed.
 *  - file_size: size in bytes at time of observation (if available).
 *  - last_modified: filesystem last-modified timestamp for the file.
 *  - type: ChangeType enumerating Modified/Added/Removed/Renamed.
 *
 * Performance notes:
 *  - getPathHash() caches a qHash() of the file_path to avoid repeated hashing
 *    when used in unordered containers or queues.
 */
struct FileChangeEvent {
    QString file_path;
    QDateTime timestamp;
    qint64 file_size;
    QDateTime last_modified;
    enum ChangeType { Modified, Added, Removed, Renamed } type;

    // Pre-computed hash to speed-up lookups in hashed containers.
    mutable std::size_t path_hash = 0;

    std::size_t getPathHash() const {
        if (path_hash == 0) {
            path_hash = qHash(file_path);
        }
        return path_hash;
    }
};

/**
 * @enum DebounceStrategy
 * @brief Strategies used to coalesce multiple rapid file system events.
 *
 * - Fixed: always wait the configured interval before dispatching an event.
 * - Adaptive: adjust intervals based on recent change frequency for a file.
 * - Exponential: increase the backoff up to a cap on repeated events.
 * - Smart: placeholder for advanced heuristics (e.g. frequency + file size +
 * type).
 *
 * Use Adaptive for typical hot-reload scenarios where save storms occur and
 * responsiveness benefits from shorter intervals after quiescence.
 */
enum class DebounceStrategy { Fixed, Adaptive, Exponential, Smart };

/**
 * @struct FileFilter
 * @brief Declarative filter used to accept/reject files before processing.
 *
 * Filtering criteria:
 *  - extensions: list of accepted file extensions (case-insensitive), e.g.
 * {".json", ".qml"}.
 *  - patterns: glob or regex-like string patterns to match file paths.
 *  - max_file_size: maximum size in bytes to process (-1 = unlimited).
 *  - include_hidden: whether hidden files are accepted.
 *
 * Optimization:
 *  - compiled_patterns stores precompiled QRegularExpression objects for
 *    repeated evaluation. Call compilePatterns() after setting patterns.
 *
 * Thread-safety:
 *  - compilePatterns() may be called concurrently; patterns_compiled guards
 *    the compiled cache (mutable to allow const callers to trigger
 * compilation).
 */
struct FileFilter {
    QStringList extensions;
    QStringList patterns;
    qint64 max_file_size = -1;  // -1 means no limit
    bool include_hidden = false;

    // Compiled regex patterns for performance
    mutable std::vector<QRegularExpression> compiled_patterns;
    mutable bool patterns_compiled = false;

    /**
     * @brief Compile patterns into QRegularExpression objects for faster
     * matching.
     *
     * This function is idempotent and safe to call multiple times; subsequent
     * calls are no-ops unless patterns are modified.
     */
    void compilePatterns() const;

    /**
     * @brief Test whether a given file path and size pass the filter.
     * @param file_path canonical or relative path of the file.
     * @param file_size size in bytes (may be -1 if unknown).
     * @return true if the file should be processed.
     */
    bool matches(const QString &file_path, qint64 file_size) const;
};

/**
 * @class FileWatcher
 * @brief Advanced file and directory watcher with debouncing, filtering and
 * batching.
 *
 * Public API provides fine-grained control:
 *  - watchFile/watchDirectory + batch variants to register interests,
 *  - unwatch* and unwatchAll to remove interests,
 *  - configuration setters for debounce interval/strategy, filters, thread pool
 * size,
 *  - status queries for currently watched paths.
 *
 * Signals:
 *  - fileChanged/fileAdded/fileRemoved: emitted when individual file events are
 * delivered.
 *  - directoryChanged: emitted when a watched directory contents change.
 *  - watchingStarted/watchingStopped/watchingFailed: lifecycle notifications.
 *
 * Internal behaviour:
 *  - QFileSystemWatcher is used as primary event source where available.
 *  - The watcher coalesces events using an internal debounce timer and
 * strategy.
 *  - Events are enqueued and optionally batch-processed on worker threads to
 *    avoid blocking the main/UI thread during IO-heavy operations.
 */
class FileWatcher : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Construct a FileWatcher.
     * @param parent Optional QObject parent for Qt ownership semantics.
     *
     * Construction initializes internal timers and thread pool lazily when
     * needed.
     */
    explicit FileWatcher(QObject *parent = nullptr);
    ~FileWatcher() override;

    // Watch methods

    /**
     * @brief Start watching a single file for changes.
     * @param file_path Path to the file to watch (absolute preferred).
     */
    void watchFile(const QString &file_path);

    /**
     * @brief Start watching a directory. If recursive is true, subdirectories
     * are also watched.
     * @param directory_path Directory path to watch.
     * @param recursive Whether to include subdirectories recursively.
     */
    void watchDirectory(const QString &directory_path, bool recursive = false);

    /**
     * @brief Add multiple files to the watch list in a batch to reduce
     * overhead.
     * @param file_paths List of file paths to watch.
     */
    void watchFiles(const QStringList &file_paths);

    /**
     * @brief Add multiple directories to the watch list.
     * @param directory_paths List of directories.
     * @param recursive Whether to watch recursively.
     */
    void watchDirectories(const QStringList &directory_paths,
                          bool recursive = false);

    /**
     * @brief Stop watching a specific file.
     * @param file_path Path previously added via watchFile/watchFiles.
     */
    void unwatchFile(const QString &file_path);

    /**
     * @brief Stop watching a specific directory.
     * @param directory_path Directory path previously watched.
     */
    void unwatchDirectory(const QString &directory_path);

    /**
     * @brief Remove all watches and perform optimized cleanup of internal
     * caches.
     */
    void unwatchAll();

    // Configuration

    /**
     * @brief Set the base debounce interval used by Fixed and initial Adaptive
     * strategies.
     * @param milliseconds debounce timeout in ms.
     */
    void setDebounceInterval(int milliseconds);

    /**
     * @brief Select the debouncing strategy applied to coalesce events.
     * @param strategy DebounceStrategy enum value.
     */
    void setDebounceStrategy(DebounceStrategy strategy);

    /**
     * @brief Install a file filter to reject irrelevant files early.
     * @param filter FileFilter descriptor; copy is stored internally.
     */
    void setFileFilter(const FileFilter &filter);

    /**
     * @brief Cap the total number of files that will be tracked; helps bound
     * memory.
     * @param max_files maximum number of watched files (approximate).
     */
    void setMaxWatchedFiles(int max_files);

    /**
     * @brief Configure the size of the internal thread pool used for processing
     * events.
     * @param thread_count number of worker threads to create (0 =
     * single-threaded).
     */
    void setThreadPoolSize(int thread_count);

    // Performance tuning

    /**
     * @brief Enable or disable batch processing of change events.
     * @param enabled true to enable batching.
     */
    void enableBatchProcessing(bool enabled);

    /**
     * @brief Set the maximum number of events processed per batch.
     * @param batch_size maximum events allowed in a single batch.
     */
    void setMaxBatchSize(int batch_size);

    /**
     * @brief Set processing priority for worker threads.
     * @param priority QThread::Priority value.
     */
    void setProcessingPriority(QThread::Priority priority);

    // Status queries

    /**
     * @brief Query whether a path (file or directory) is currently watched.
     * @param path file or directory path to test.
     * @return true if path is being monitored.
     */
    [[nodiscard]] bool isWatching(const QString &path) const;

    /**
     * @brief Return a list of watched files.
     * @return QStringList of file paths.
     */
    [[nodiscard]] QStringList watchedFiles() const;

    /**
     * @brief Return a list of watched directories.
     * @return QStringList of directory paths.
     */
    [[nodiscard]] QStringList watchedDirectories() const;

signals:
    /**
     * @brief Emitted when a watched file is detected as changed (content
     * updated).
     * @param file_path Path to the changed file.
     */
    void fileChanged(const QString &file_path);

    /**
     * @brief Emitted when a new file is detected in a watched directory.
     * @param file_path Path to the added file.
     */
    void fileAdded(const QString &file_path);

    /**
     * @brief Emitted when a watched file is removed.
     * @param file_path Path to the removed file.
     */
    void fileRemoved(const QString &file_path);

    /**
     * @brief Emitted when a watched directory's contents change.
     * @param directory_path Directory path.
     */
    void directoryChanged(const QString &directory_path);

    /**
     * @brief Emitted when watching starts for a given path.
     * @param path Path that started being watched.
     */
    void watchingStarted(const QString &path);

    /**
     * @brief Emitted when watching stops for a given path.
     * @param path Path that stopped being watched.
     */
    void watchingStopped(const QString &path);

    /**
     * @brief Emitted when an operation related to watching fails (permissions,
     * IO, limits).
     * @param path Path related to the failure.
     * @param error Human-readable error description.
     */
    void watchingFailed(const QString &path, const QString &error);

private slots:
    // Internal Qt callbacks used by QFileSystemWatcher and timers.
    void onFileChanged(const QString &path);
    void onDirectoryChanged(const QString &path);
    void onDebounceTimeout();

private:
    // Core watching infrastructure
    std::unique_ptr<QFileSystemWatcher> watcher_;
    std::unique_ptr<QTimer> debounce_timer_;

    // Multi-threaded processing
    std::unique_ptr<QThread> worker_thread_;
    std::vector<std::unique_ptr<QThread>> thread_pool_;

    // Configuration with atomic access for thread safety
    std::atomic<int> debounce_interval_{100};
    std::atomic<int> max_watched_files_{10000};
    std::atomic<int> max_batch_size_{100};
    std::atomic<bool> batch_processing_enabled_{true};
    DebounceStrategy debounce_strategy_ = DebounceStrategy::Adaptive;

    // Advanced file filtering
    FileFilter file_filter_;
    mutable std::shared_mutex filter_mutex_;

    // Performance optimized data structures
    std::unordered_map<QString, QDateTime> file_timestamps_;
    std::unordered_map<QString, qint64> file_sizes_;
    std::unordered_set<QString> watched_files_;
    std::unordered_set<QString> watched_directories_;

    // Thread-safe event queue with optional priority ordering
    std::queue<FileChangeEvent> event_queue_;
    std::priority_queue<FileChangeEvent> priority_queue_;
    mutable std::mutex queue_mutex_;

    // Adaptive debouncing state
    std::unordered_map<QString, std::chrono::steady_clock::time_point>
        last_change_times_;
    std::unordered_map<QString, int> change_frequencies_;

    // Performance monitoring
    std::atomic<size_t> total_events_processed_{0};
    std::atomic<size_t> events_filtered_{0};
    QElapsedTimer performance_timer_;

    // Memory management
    static constexpr size_t MAX_CACHE_SIZE = 10000;
    std::unordered_map<QString, std::weak_ptr<FileChangeEvent>> event_cache_;

    // Core methods
    void setupWatcher();
    void setupThreadPool();
    void cleanupThreadPool();

    // Enhanced processing methods
    void processFileChange(const QString &file_path);
    void processDirectoryChange(const QString &directory_path);
    void processBatchChanges(const std::vector<FileChangeEvent> &events);

    // Filtering and validation
    bool shouldProcessFile(const QString &file_path) const;
    bool isFileChanged(const QString &file_path, const QDateTime &timestamp,
                       qint64 size) const;

    // Directory scanning with optimization
    void scanDirectory(const QString &directory_path, bool recursive);
    void scanDirectoryAsync(const QString &directory_path, bool recursive);

    // Adaptive debouncing logic
    int calculateAdaptiveDebounceInterval(const QString &file_path) const;
    void updateChangeFrequency(const QString &file_path);

    // Memory and performance optimization
    void cleanupCache();
    void optimizeDataStructures();

    // Platform-specific optimizations
    void enablePlatformOptimizations();

private slots:
    // Optimized callbacks used when advanced features are enabled.
    void onFileChangedOptimized(const QString &path);
    void onDirectoryChangedOptimized(const QString &path);
    void onDebounceTimeoutOptimized();
    void onBatchProcessingTimeout();
};

}  // namespace DeclarativeUI::HotReload
