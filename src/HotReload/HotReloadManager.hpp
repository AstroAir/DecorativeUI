// HotReload/HotReloadManager.hpp
#pragma once
#include <QObject>
#include <QString>
#include <QWidget>
#include <QThread>
#include <QMutex>
#include <QReadWriteLock>
#include <QTimer>
#include <QElapsedTimer>
#include <QJsonObject>
#include <functional>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <atomic>
#include <chrono>
#include <thread>
#include <queue>
#include <vector>
#include <shared_mutex>
#include "../Exceptions/UIExceptions.hpp"
#include "../JSON/JSONUILoader.hpp"
#include "FileWatcher.hpp"

namespace DeclarativeUI::HotReload {

// **Advanced reload strategy for different scenarios**
enum class ReloadStrategy {
    Immediate,      // Reload immediately
    Batched,        // Batch multiple changes
    Incremental,    // Only reload changed parts
    Smart           // AI-based decision making
};

// **Dependency tracking for incremental reloads**
struct FileDependency {
    QString file_path;
    std::unordered_set<QString> dependencies;
    std::unordered_set<QString> dependents;
    QDateTime last_modified;
    std::size_t content_hash = 0;

    bool hasChanged(const QDateTime& timestamp, std::size_t hash) const {
        return last_modified != timestamp || content_hash != hash;
    }
};

// **Performance metrics for optimization**
struct ReloadMetrics {
    std::chrono::milliseconds total_time{0};
    std::chrono::milliseconds parse_time{0};
    std::chrono::milliseconds render_time{0};
    std::chrono::milliseconds layout_time{0};
    size_t memory_usage = 0;
    size_t widget_count = 0;
    bool success = false;
};

// **Rollback point for error recovery**
struct RollbackPoint {
    QString file_path;
    QJsonObject widget_state;
    QByteArray widget_geometry;
    QDateTime timestamp;
    std::unique_ptr<QWidget> backup_widget;

    // Custom constructors to handle unique_ptr member
    RollbackPoint() = default;
    RollbackPoint(const RollbackPoint& other) 
        : file_path(other.file_path)
        , widget_state(other.widget_state)
        , widget_geometry(other.widget_geometry)
        , timestamp(other.timestamp)
        , backup_widget(nullptr) // Can't copy unique_ptr, reset to nullptr
    {}

    // Custom assignment operator to handle unique_ptr member
    RollbackPoint& operator=(const RollbackPoint& other) {
        if (this != &other) {
            file_path = other.file_path;
            widget_state = other.widget_state;
            widget_geometry = other.widget_geometry;
            timestamp = other.timestamp;
            backup_widget.reset(); // Can't copy unique_ptr, reset to nullptr
        }
        return *this;
    }

    // Move constructor and assignment
    RollbackPoint(RollbackPoint&&) = default;
    RollbackPoint& operator=(RollbackPoint&&) = default;
};

// **Advanced hot reload system with intelligent optimization**
class HotReloadManager : public QObject {
    Q_OBJECT

public:
    explicit HotReloadManager(QObject* parent = nullptr);
    ~HotReloadManager() override;

    // Delete copy constructor and assignment operator because of std::atomic members
    HotReloadManager(const HotReloadManager&) = delete;
    HotReloadManager& operator=(const HotReloadManager&) = delete;

    // **Enhanced registration with dependency tracking**
    void registerUIFile(const QString& file_path, QWidget* target_widget);
    void registerUIFileWithDependencies(const QString& file_path, QWidget* target_widget,
                                       const QStringList& dependencies);

    // **Batch registration for better performance**
    void registerUIFiles(const QStringList& file_paths, const QList<QWidget*>& target_widgets);
    void registerUIDirectory(const QString& directory_path, bool recursive = true);

    // **Advanced configuration**
    void setEnabled(bool enabled);
    [[nodiscard]] bool isEnabled() const noexcept { return enabled_.load(); }

    void setReloadStrategy(ReloadStrategy strategy);
    void setReloadDelay(int milliseconds);
    void setFileFilters(const QStringList& filters);
    void setMaxConcurrentReloads(int max_concurrent);
    void setMemoryLimit(size_t limit_bytes);

    // **Performance optimization controls**
    void enableIncrementalReloading(bool enabled);
    void enableParallelProcessing(bool enabled);
    void enableSmartCaching(bool enabled);
    void setPreloadStrategy(bool preload_dependencies);

    // **Enhanced reload operations**
    void reloadFile(const QString& file_path);
    void reloadFileIncremental(const QString& file_path);
    void reloadAll();
    void reloadBatch(const QStringList& file_paths);

    // **Rollback and recovery**
    void createRollbackPoint(const QString& file_path);
    void rollbackToPoint(const QString& file_path);
    void clearRollbackPoints();

    // **Cleanup with optimization**
    void unregisterUIFile(const QString& file_path);
    void unregisterAll();
    void optimizeMemoryUsage();

    // **Advanced error handling**
    void setErrorHandler(std::function<void(const QString&, const QString&)> handler);
    void setRecoveryHandler(std::function<bool(const QString&)> handler);

    // **Performance monitoring**
    ReloadMetrics getLastReloadMetrics(const QString& file_path) const;
    QJsonObject getPerformanceReport() const;
    void resetPerformanceCounters();

signals:
    void reloadStarted(const QString& file_path);
    void reloadCompleted(const QString& file_path);
    void reloadFailed(const QString& file_path, const QString& error);
    void hotReloadEnabled(bool enabled);

private slots:
    void onFileChanged(const QString& file_path);
    void onFileAdded(const QString& file_path);
    void onFileRemoved(const QString& file_path);

private:
    // **Enhanced UI file information with performance tracking**
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

        // Custom copy constructor to handle atomic and unique_ptr members
        UIFileInfo() = default;
        UIFileInfo(const UIFileInfo& other) 
            : target_widget(other.target_widget)
            , parent_layout_position(other.parent_layout_position)
            , parent_widget(other.parent_widget)
            , backup_widget(nullptr) // Can't copy unique_ptr, reset to nullptr
            , last_reload(other.last_reload)
            , last_metrics(other.last_metrics)
            , rollback_points(other.rollback_points)
            , is_reloading(other.is_reloading.load())
            , last_access(other.last_access)
        {}

        // Custom assignment operator to handle atomic and unique_ptr members
        UIFileInfo& operator=(const UIFileInfo& other) {
            if (this != &other) {
                target_widget = other.target_widget;
                parent_layout_position = other.parent_layout_position;
                parent_widget = other.parent_widget;
                backup_widget.reset(); // Can't copy unique_ptr, reset to nullptr
                last_reload = other.last_reload;
                last_metrics = other.last_metrics;
                rollback_points = other.rollback_points;
                is_reloading.store(other.is_reloading.load());
                last_access = other.last_access;
            }
            return *this;
        }
    };

    // **Core infrastructure with thread safety**
    std::unique_ptr<FileWatcher> file_watcher_;
    std::unique_ptr<JSON::JSONUILoader> ui_loader_;
    std::unique_ptr<QThread> worker_thread_;
    std::vector<std::unique_ptr<QThread>> thread_pool_;

    // **Thread-safe data structures**
    std::unordered_map<QString, UIFileInfo> registered_files_;
    std::unordered_map<QString, FileDependency> dependency_graph_;
    std::unordered_map<QString, std::shared_ptr<QWidget>> widget_cache_;
    mutable std::shared_mutex data_mutex_;

    // **Configuration with atomic access**
    std::atomic<bool> enabled_{true};
    std::atomic<int> reload_delay_{100};
    std::atomic<int> max_concurrent_reloads_{4};
    std::atomic<size_t> memory_limit_{100 * 1024 * 1024}; // 100MB
    std::atomic<bool> incremental_reloading_{true};
    std::atomic<bool> parallel_processing_{true};
    std::atomic<bool> smart_caching_{true};
    ReloadStrategy reload_strategy_ = ReloadStrategy::Smart;

    // **Performance monitoring**
    std::unordered_map<QString, ReloadMetrics> performance_metrics_;
    std::atomic<size_t> total_reloads_{0};
    std::atomic<size_t> successful_reloads_{0};
    std::atomic<size_t> failed_reloads_{0};
    QElapsedTimer uptime_timer_;

    // **Event handling and recovery**
    std::function<void(const QString&, const QString&)> error_handler_;
    std::function<bool(const QString&)> recovery_handler_;
    std::queue<QString> reload_queue_;
    std::mutex queue_mutex_;

    // **Memory management**
    std::atomic<size_t> current_memory_usage_{0};
    std::unordered_set<QString> preloaded_files_;

    // **Core methods with optimizations**
    void performReload(const QString& file_path);
    void performReloadIncremental(const QString& file_path);
    void performReloadBatch(const QStringList& file_paths);
    void performReloadAsync(const QString& file_path);

    // **Widget management**
    void replaceWidget(const QString& file_path, std::unique_ptr<QWidget> new_widget);
    void replaceWidgetSafe(const QString& file_path, std::unique_ptr<QWidget> new_widget);
    std::unique_ptr<QWidget> createWidgetFromCache(const QString& file_path);

    // **Backup and rollback**
    void createBackup(const QString& file_path);
    void createRollbackPointInternal(const QString& file_path);
    void restoreBackup(const QString& file_path);
    bool rollbackToPointInternal(const QString& file_path);

    // **Dependency management**
    void buildDependencyGraph();
    void updateDependencies(const QString& file_path);
    QStringList getAffectedFiles(const QString& file_path) const;
    bool hasCyclicDependency(const QString& file_path) const;

    // **Performance optimization**
    bool shouldReload(const QString& file_path) const;
    bool shouldReloadIncremental(const QString& file_path) const;
    bool validateWidget(QWidget* widget) const;
    void optimizeWidget(QWidget* widget);
    void preloadDependencies(const QString& file_path);
    void cleanupCache();
    void updateMemoryUsage();

    // **Thread management**
    void setupUILoader();
    void setupThreadPool();
    void cleanupThreadPool();
    QThread* getAvailableThread();

    // **Monitoring and analytics**
    void recordMetrics(const QString& file_path, const ReloadMetrics& metrics);
    void updatePerformanceCounters(bool success);
    ReloadMetrics measureReloadPerformance(const std::function<void()>& reload_func);

private slots:
    void onFileChangedOptimized(const QString& file_path);
    void onFileAddedOptimized(const QString& file_path);
    void onFileRemovedOptimized(const QString& file_path);
    void onReloadQueueTimeout();
    void onMemoryCleanupTimeout();
};

}  // namespace DeclarativeUI::HotReload
