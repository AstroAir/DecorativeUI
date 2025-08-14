// HotReload/HotReloadManager.hpp
#pragma once

#include <QElapsedTimer>
#include <QJsonObject>
#include <QMutex>
#include <QObject>
#include <QReadWriteLock>
#include <QString>
#include <QThread>
#include <QTimer>
#include <QWidget>

#include <atomic>
#include <chrono>
#include <functional>
#include <memory>
#include <queue>
#include <shared_mutex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../JSON/JSONUILoader.hpp"
#include "FileWatcher.hpp"

namespace DeclarativeUI::HotReload {

/**
 * @file HotReloadManager.hpp
 * @brief High-level manager coordinating file watching, UI parsing and
 * hot-reload behavior.
 *
 * This header declares HotReloadManager, an orchestration component that:
 *  - integrates a FileWatcher to observe changes on disk,
 *  - uses JSON::JSONUILoader to parse JSON UI definitions and instantiate
 * QWidget trees,
 *  - tracks file dependencies to enable incremental and selective reloads,
 *  - provides rollback/backup facilities to recover from faulty reloads,
 *  - collects performance metrics and exposes simple analytic helpers,
 *  - exposes configurable strategies for reload timing and batching.
 *
 * Design notes:
 *  - HotReloadManager is intended to be instantiated on the application (UI)
 *    thread. Heavy operations (parsing, widget creation) are dispatched to
 * worker threads when parallel processing is enabled.
 *  - Thread-safety: internal data structures are protected by
 * shared_mutex/data_mutex_ and atomic flags are used for frequently accessed
 * configuration.
 *  - Recovery: callers may install custom error and recovery handlers to
 * implement application-specific rollback policies.
 */

/**
 * @enum ReloadStrategy
 * @brief Strategies describing how the manager decides to reload on file
 * changes.
 *
 * - Immediate: reloads synchronously as soon as change is detected.
 * - Batched: collects a set of changes and performs a single reload for the
 * batch.
 * - Incremental: attempts to reload only affected portions using dependency
 * tracking.
 * - Smart: higher-level decision making (heuristics or learned policies) to
 * choose the most efficient reload approach.
 */
enum class ReloadStrategy {
    Immediate,    ///< Reload immediately when files change.
    Batched,      ///< Aggregate changes and reload as a group.
    Incremental,  ///< Perform minimal reloads based on dependency graph.
    Smart         ///< Use heuristics to choose reload plan.
};

/**
 * @struct FileDependency
 * @brief Tracks dependency relationships between UI files for incremental
 * reloads.
 *
 * Fields:
 *  - file_path: canonical path to the tracked file.
 *  - dependencies: set of files this file depends on (includes/imports).
 *  - dependents: set of files that depend on this file (reverse edges).
 *  - last_modified: timestamp of the last observed modification.
 *  - content_hash: lightweight hash of file contents used to detect
 * content-level changes.
 *
 * Utility:
 *  - hasChanged(): compare timestamp/hash pair to quickly determine if file
 *    content or metadata differs from the recorded snapshot.
 *
 * This structure enables efficient computation of affected files when a single
 * source changes and supports incremental reload workflows where only
 * dependents are refreshed.
 */
struct FileDependency {
    QString file_path;
    std::unordered_set<QString> dependencies;
    std::unordered_set<QString> dependents;
    QDateTime last_modified;
    std::size_t content_hash = 0;

    /**
     * @brief Determine whether stored metadata differs from supplied values.
     * @param timestamp New last-modified timestamp to compare.
     * @param hash New content hash to compare.
     * @return true if either timestamp or content hash differs.
     */
    bool hasChanged(const QDateTime& timestamp, std::size_t hash) const {
        return last_modified != timestamp || content_hash != hash;
    }
};

/**
 * @struct ReloadMetrics
 * @brief Simple performance counters collected for each reload operation.
 *
 * Fields are intended to help monitor and optimize reload latency and memory:
 *  - total_time: overall time spent performing the reload.
 *  - parse_time: time spent parsing JSON and creating widget instances.
 *  - render_time: time spent rendering/painting (if measured).
 *  - layout_time: time spent on layout calculations.
 *  - memory_usage: approximate bytes allocated / used during reload.
 *  - widget_count: number of widgets created or updated.
 *  - success: boolean indicating whether reload completed without fatal error.
 *
 * These metrics are best-effort and may be populated only when instrumentation
 * is enabled or available on the platform.
 */
struct ReloadMetrics {
    std::chrono::milliseconds total_time{0};
    std::chrono::milliseconds parse_time{0};
    std::chrono::milliseconds render_time{0};
    std::chrono::milliseconds layout_time{0};
    size_t memory_usage = 0;
    size_t widget_count = 0;
    bool success = false;
};

/**
 * @struct RollbackPoint
 * @brief Snapshot of the UI state to support rollback after a failed reload.
 *
 * A RollbackPoint captures:
 *  - file_path: associated UI source file.
 *  - widget_state: serialized logical state for the widget (if available).
 *  - widget_geometry: raw geometry or QByteArray snapshot (platform-dependent).
 *  - timestamp: when the rollback point was taken.
 *  - backup_widget: optional in-memory widget copy used to restore the UI.
 *
 * Notes:
 *  - backup_widget is held in a unique_ptr and therefore RollbackPoint is not
 *    trivially copyable. The copy constructor/assignment reset backup_widget to
 * nullptr.
 *  - Consumers should keep the number and size of rollback points bounded to
 * avoid excessive memory usage.
 */
struct RollbackPoint {
    QString file_path;
    QJsonObject widget_state;
    QByteArray widget_geometry;
    QDateTime timestamp;
    std::unique_ptr<QWidget> backup_widget;

    // Custom constructors and assignment semantics to avoid copying unique_ptr
    RollbackPoint() = default;
    RollbackPoint(const RollbackPoint& other)
        : file_path(other.file_path),
          widget_state(other.widget_state),
          widget_geometry(other.widget_geometry),
          timestamp(other.timestamp),
          backup_widget(nullptr)  // Can't copy unique_ptr, reset to nullptr
    {}

    RollbackPoint& operator=(const RollbackPoint& other) {
        if (this != &other) {
            file_path = other.file_path;
            widget_state = other.widget_state;
            widget_geometry = other.widget_geometry;
            timestamp = other.timestamp;
            backup_widget.reset();  // Can't copy unique_ptr
        }
        return *this;
    }

    // Move semantics allowed
    RollbackPoint(RollbackPoint&&) = default;
    RollbackPoint& operator=(RollbackPoint&&) = default;
};

/**
 * @class HotReloadManager
 * @brief Central manager coordinating hot-reload lifecycle for JSON-driven UI.
 *
 * Responsibilities:
 *  - Register UI files and directories for hot-reload monitoring.
 *  - Maintain dependency graph used for incremental reloads.
 *  - Schedule and execute reloads according to configured ReloadStrategy.
 *  - Provide rollback/backup points to recover after failed reloads.
 *  - Expose configuration setters (debounce, concurrency, memory limits).
 *  - Emit signals to notify application code about reload lifecycle events.
 *
 * Threading and lifetime:
 *  - This class inherits QObject and is typically created on the main/UI
 * thread.
 *  - Heavy work (parsing, widget creation) may be dispatched to worker threads
 *    configured via setupThreadPool().
 *  - Data structures shared across threads are guarded by data_mutex_.
 *
 * Usage:
 *  - registerUIFile() to begin watching a file and attach it to a live QWidget.
 *  - Optionally call registerUIFileWithDependencies() to supply a precomputed
 * dependency list.
 *  - Connect to reloadStarted/reloadCompleted/reloadFailed to perform
 * application-level updates.
 */
class HotReloadManager : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Construct a HotReloadManager.
     * @param parent Optional QObject parent.
     *
     * Constructor initializes internal timers, file watcher and the UI loader.
     */
    explicit HotReloadManager(QObject* parent = nullptr);
    ~HotReloadManager() override;

    // Non-copyable due to internal atomic and unique resources
    HotReloadManager(const HotReloadManager&) = delete;
    HotReloadManager& operator=(const HotReloadManager&) = delete;

    /**
     * @brief Register a single UI file to be watched and linked to a target
     * widget.
     * @param file_path Path to the JSON UI definition file.
     * @param target_widget Pointer to the widget instance to be
     * replaced/updated on reload.
     *
     * The manager does not take ownership of target_widget; callers must manage
     * lifecycle.
     */
    void registerUIFile(const QString& file_path, QWidget* target_widget);

    /**
     * @brief Register a UI file and supply its dependency list for incremental
     * reloads.
     * @param file_path Path to the JSON UI file.
     * @param target_widget Target widget to manage.
     * @param dependencies List of files this UI depends on (imports/includes).
     *
     * Providing dependencies upfront can speed up incremental reload decisions.
     */
    void registerUIFileWithDependencies(const QString& file_path,
                                        QWidget* target_widget,
                                        const QStringList& dependencies);

    /**
     * @brief Register multiple UI files in a single batch operation.
     * @param file_paths List of file paths to register.
     * @param target_widgets Parallel list of pointers to target widgets
     * (matching indices).
     *
     * Batch registration avoids repeated expensive bookkeeping and is
     * recommended when initializing many UIs at startup.
     */
    void registerUIFiles(const QStringList& file_paths,
                         const QList<QWidget*>& target_widgets);

    /**
     * @brief Register a directory for hot-reload monitoring.
     * @param directory_path Directory to watch (recursively if requested).
     * @param recursive If true, watches subdirectories recursively.
     */
    void registerUIDirectory(const QString& directory_path,
                             bool recursive = true);

    /** Enable or disable the hot-reload manager. When disabled, file events are
     * ignored. */
    void setEnabled(bool enabled);
    [[nodiscard]] bool isEnabled() const noexcept { return enabled_.load(); }

    /** Configure strategy and timing used when scheduling reloads. */
    void setReloadStrategy(ReloadStrategy strategy);
    void setReloadDelay(int milliseconds);

    /** Configure filters and resource constraints. */
    void setFileFilters(const QStringList& filters);
    void setMaxConcurrentReloads(int max_concurrent);
    void setMemoryLimit(size_t limit_bytes);

    /** Performance and behavior toggles. */
    void enableIncrementalReloading(bool enabled);
    void enableParallelProcessing(bool enabled);
    void enableSmartCaching(bool enabled);
    void setPreloadStrategy(bool preload_dependencies);

    /** Manual reload operations. These may be executed synchronously or
     * scheduled. */
    void reloadFile(const QString& file_path);
    void reloadFileIncremental(const QString& file_path);
    void reloadAll();
    void reloadBatch(const QStringList& file_paths);

    /** Rollback and recovery APIs. */
    void createRollbackPoint(const QString& file_path);
    void rollbackToPoint(const QString& file_path);
    void clearRollbackPoints();

    /** Unregister and cleanup APIs. */
    void unregisterUIFile(const QString& file_path);
    void unregisterAll();
    void optimizeMemoryUsage();

    /** Customizable error and recovery handlers. */
    void setErrorHandler(
        std::function<void(const QString&, const QString&)> handler);
    void setRecoveryHandler(std::function<bool(const QString&)> handler);

    /** Performance reporting and metrics accessors. */
    ReloadMetrics getLastReloadMetrics(const QString& file_path) const;
    QJsonObject getPerformanceReport() const;
    void resetPerformanceCounters();

signals:
    /**
     * @brief Emitted when reload of a particular file begins.
     * @param file_path File being reloaded.
     */
    void reloadStarted(const QString& file_path);

    /**
     * @brief Emitted when reload completes successfully.
     * @param file_path File that finished reloading.
     */
    void reloadCompleted(const QString& file_path);

    /**
     * @brief Emitted when reload fails.
     * @param file_path File that failed to reload.
     * @param error Human-readable error message.
     */
    void reloadFailed(const QString& file_path, const QString& error);

    /**
     * @brief Emitted when hot-reload is enabled or disabled.
     * @param enabled Current enablement state.
     */
    void hotReloadEnabled(bool enabled);

private slots:
    // Internal slots connected to FileWatcher events. These are entry points
    // for change handling.
    void onFileChanged(const QString& file_path);
    void onFileAdded(const QString& file_path);
    void onFileRemoved(const QString& file_path);

private:
    /**
     * @brief Per-UI tracked information used by the manager.
     *
     * Fields:
     *  - target_widget: pointer to the live widget in the application (not
     * owned).
     *  - parent_layout_position: optional hint where widget is located in
     * layout.
     *  - parent_widget: pointer to the parent container (not owned).
     *  - backup_widget: optional snapshot for rollback.
     *  - last_reload: timestamp of the last successful reload.
     *  - last_metrics: metrics captured for the last reload.
     *  - rollback_points: history of rollback snapshots.
     *  - is_reloading: atomic flag indicating a reload is in progress.
     *  - last_access: used by caching policies to evict stale entries.
     *
     * The copy/assignment semantics clear unique_ptr-backed members to avoid
     * double ownership.
     */
    struct UIFileInfo {
        QWidget* target_widget = nullptr;
        QString parent_layout_position;
        QWidget* parent_widget = nullptr;
        std::unique_ptr<QWidget> backup_widget;
        QDateTime last_reload;
        ReloadMetrics last_metrics;
        std::vector<RollbackPoint> rollback_points;
        std::atomic<bool> is_reloading{false};
        std::chrono::steady_clock::time_point last_access;

        UIFileInfo() = default;
        UIFileInfo(const UIFileInfo& other)
            : target_widget(other.target_widget),
              parent_layout_position(other.parent_layout_position),
              parent_widget(other.parent_widget),
              backup_widget(nullptr),
              last_reload(other.last_reload),
              last_metrics(other.last_metrics),
              rollback_points(other.rollback_points),
              is_reloading(other.is_reloading.load()),
              last_access(other.last_access) {}

        UIFileInfo& operator=(const UIFileInfo& other) {
            if (this != &other) {
                target_widget = other.target_widget;
                parent_layout_position = other.parent_layout_position;
                parent_widget = other.parent_widget;
                backup_widget.reset();
                last_reload = other.last_reload;
                last_metrics = other.last_metrics;
                rollback_points = other.rollback_points;
                is_reloading.store(other.is_reloading.load());
                last_access = other.last_access;
            }
            return *this;
        }
    };

    // Core subsystems
    std::unique_ptr<FileWatcher>
        file_watcher_;  ///< Observes file system changes.
    std::unique_ptr<JSON::JSONUILoader>
        ui_loader_;  ///< Parses JSON and creates widgets.
    std::unique_ptr<QThread>
        worker_thread_;  ///< Optional worker thread for serialized work.
    std::vector<std::unique_ptr<QThread>>
        thread_pool_;  ///< Worker threads for parallel processing.

    // Shared state guarded by data_mutex_
    std::unordered_map<QString, UIFileInfo>
        registered_files_;  ///< Map of registered UI files.
    std::unordered_map<QString, FileDependency>
        dependency_graph_;  ///< Dependency graph for incremental reloads.
    std::unordered_map<QString, std::shared_ptr<QWidget>>
        widget_cache_;  ///< Cached widgets for fast replacement.
    mutable std::shared_mutex data_mutex_;  ///< Protects above maps.

    // Configuration (atomic for fast concurrent reads)
    std::atomic<bool> enabled_{true};
    std::atomic<int> reload_delay_{
        100};  ///< Milliseconds delay applied before performing reload.
    std::atomic<int> max_concurrent_reloads_{4};
    std::atomic<size_t> memory_limit_{100 * 1024 *
                                      1024};  ///< Memory limit in bytes.
    std::atomic<bool> incremental_reloading_{true};
    std::atomic<bool> parallel_processing_{true};
    std::atomic<bool> smart_caching_{true};
    ReloadStrategy reload_strategy_ = ReloadStrategy::Smart;

    // Monitoring
    std::unordered_map<QString, ReloadMetrics> performance_metrics_;
    std::atomic<size_t> total_reloads_{0};
    std::atomic<size_t> successful_reloads_{0};
    std::atomic<size_t> failed_reloads_{0};
    QElapsedTimer uptime_timer_;

    // Handlers and internal queues
    std::function<void(const QString&, const QString&)> error_handler_;
    std::function<bool(const QString&)> recovery_handler_;
    std::queue<QString> reload_queue_;
    std::mutex queue_mutex_;

    // Memory/caching
    std::atomic<size_t> current_memory_usage_{0};
    std::unordered_set<QString> preloaded_files_;

    // Core methods implementing reload workflows (internal, optimized)
    void performReload(const QString& file_path);
    void performReloadIncremental(const QString& file_path);
    void performReloadBatch(const QStringList& file_paths);
    void performReloadAsync(const QString& file_path);

    // Widget lifecycle helpers
    void replaceWidget(const QString& file_path,
                       std::unique_ptr<QWidget> new_widget);
    void replaceWidgetSafe(const QString& file_path,
                           std::unique_ptr<QWidget> new_widget);
    std::unique_ptr<QWidget> createWidgetFromCache(const QString& file_path);

    // Backup / rollback
    void createBackup(const QString& file_path);
    void createRollbackPointInternal(const QString& file_path);
    void restoreBackup(const QString& file_path);
    bool rollbackToPointInternal(const QString& file_path);

    // Dependency management
    void buildDependencyGraph();
    void updateDependencies(const QString& file_path);
    QStringList getAffectedFiles(const QString& file_path) const;
    bool hasCyclicDependency(const QString& file_path) const;

    // Performance and heuristics
    bool shouldReload(const QString& file_path) const;
    bool shouldReloadIncremental(const QString& file_path) const;
    bool validateWidget(QWidget* widget) const;
    void optimizeWidget(QWidget* widget);
    void preloadDependencies(const QString& file_path);
    void cleanupCache();
    void updateMemoryUsage();

    // Thread and loader setup
    void setupUILoader();
    void setupThreadPool();
    void cleanupThreadPool();
    QThread* getAvailableThread();

    // Analytics
    void recordMetrics(const QString& file_path, const ReloadMetrics& metrics);
    void updatePerformanceCounters(bool success);
    ReloadMetrics measureReloadPerformance(
        const std::function<void()>& reload_func);

private slots:
    // Optimized versions of file event handlers used when advanced features
    // active.
    void onFileChangedOptimized(const QString& file_path);
    void onFileAddedOptimized(const QString& file_path);
    void onFileRemovedOptimized(const QString& file_path);
    void onReloadQueueTimeout();
    void onMemoryCleanupTimeout();
};

}  // namespace DeclarativeUI::HotReload
