#pragma once

#include <QDateTime>
#include <QElapsedTimer>
#include <QJsonArray>
#include <QJsonObject>
#include <QMutex>
#include <QObject>
#include <QReadWriteLock>
#include <QString>
#include <QThread>
#include <QTimer>

#include <atomic>
#include <chrono>
#include <deque>
#include <functional>
#include <memory>
#include <queue>
#include <shared_mutex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace DeclarativeUI::HotReload {

/**
 * @file PerformanceMonitor.hpp
 * @brief Performance monitoring, analytics and optimization helpers for
 * hot-reload subsystem.
 *
 * This header provides types and an extensible PerformanceMonitor class
 * intended to capture detailed timing, memory and quality metrics for reload
 * operations. It also provides analytics windows, predictive modeling support
 * and basic automated optimization hooks.
 *
 * Key capabilities:
 *  - fine-grained breakdown of reload phases (parsing, validation, widget
 * creation),
 *  - memory and CPU observations, peak and delta reporting,
 *  - sliding-window real-time analytics and lightweight predictive models,
 *  - bottleneck detection and human-friendly recommendations,
 *  - pluggable callbacks for integration with external telemetry systems.
 *
 * Threading and safety:
 *  - PerformanceMonitor is designed for concurrent use from multiple threads.
 *  - Internal mutable containers are guarded by shared_mutex where required.
 *  - Atomic primitives are used for counters updated from hot paths.
 *
 * Usage:
 *  - Instantiate PerformanceMonitor on the main or monitoring thread.
 *  - Surround critical sections with PerformanceMeasurement
 * (MEASURE_PERFORMANCE) or call startOperation()/endOperation() to time named
 * actions.
 *  - Record reload metrics via recordReloadMetrics() for persistent analysis.
 *
 * Notes:
 *  - The monitor does not perform intrusive sampling by default; enabling
 *    memory profiling or predictive modeling may add overhead.
 *  - Exported reports are best effort and depend on platform ability to report
 *    memory and CPU usage.
 */

/**
 * @struct AdvancedPerformanceMetrics
 * @brief Detailed, phase-level metrics captured for a single reload operation.
 *
 * This structure holds timing (ms), memory (MB) and meta information that can
 * be used for diagnostics, trend analysis and automated decisions. Fields are
 * intentionally explicit to make attribution of time/cost straightforward.
 *
 * Fields:
 *  - reload_time_ms: total wall-clock time observed for the reload (ms).
 *  - file_load_time_ms: time spent reading files from disk.
 *  - parsing_time_ms: time spent parsing JSON and building intermediate ASTs.
 *  - validation_time_ms: time spent validating parsed JSON (schema/rules).
 *  - widget_creation_time_ms: time spent instantiating QWidget objects.
 *  - widget_replacement_time_ms: time spent swapping widgets into the UI.
 *  - layout_time_ms: time spent computing layouts after replacement.
 *  - rendering_time_ms: approximate time spent on painting (if measured).
 *  - total_time_ms: alias for convenience (may equal reload_time_ms).
 *
 * Memory / CPU:
 *  - memory_before_mb / memory_after_mb / memory_peak_mb: memory snapshots
 * (MB).
 *  - memory_leaked_mb: best-effort estimate of leaked memory during operation.
 *  - cpu_usage_percent: average CPU usage observed for operation (0-100).
 *  - thread_count: number of threads used for the operation.
 *
 * Quality / context:
 *  - success_count / failure_count / warning_count: counts related to operation
 * outcome.
 *  - file_path / operation_type / timestamp / file_size_bytes / widget_count:
 *    contextual metadata useful when aggregating results.
 *
 * Scoring:
 *  - performance_score / reliability_score / efficiency_score: simple 0-100
 * scores computed from heuristics. Use
 * getPerformanceScore()/getReliabilityScore() in PerformanceMonitor to compute
 * global indices.
 */
struct AdvancedPerformanceMetrics {
    qint64 reload_time_ms = 0;
    qint64 file_load_time_ms = 0;
    qint64 parsing_time_ms = 0;
    qint64 validation_time_ms = 0;
    qint64 widget_creation_time_ms = 0;
    qint64 widget_replacement_time_ms = 0;
    qint64 layout_time_ms = 0;
    qint64 rendering_time_ms = 0;
    qint64 total_time_ms = 0;

    // Memory metrics
    size_t memory_before_mb = 0;
    size_t memory_after_mb = 0;
    size_t memory_peak_mb = 0;
    size_t memory_leaked_mb = 0;

    // CPU metrics
    double cpu_usage_percent = 0.0;
    int thread_count = 0;

    // Quality metrics
    int success_count = 0;
    int failure_count = 0;
    int warning_count = 0;

    // Context information
    QString file_path;
    QString operation_type;
    QDateTime timestamp;
    qint64 file_size_bytes = 0;
    int widget_count = 0;

    // Performance scores (0-100)
    double performance_score = 100.0;
    double reliability_score = 100.0;
    double efficiency_score = 100.0;
};

/**
 * @struct AnalyticsData
 * @brief Aggregated sliding-window data for real-time analytics.
 *
 * This container collects per-operation samples and lightweight aggregates used
 * by the real-time dashboard and alerting subsystems.
 *
 * Thread-safety:
 *  - Atomic members are used so copies can be made safely for reporting.
 *  - Vectors are copied when creating snapshots for external consumers; callers
 *    should avoid modifying returned structures concurrently.
 */
struct AnalyticsData {
    std::vector<double> response_times;
    std::vector<double> memory_usage;
    std::vector<double> cpu_usage;
    std::chrono::steady_clock::time_point start_time;
    std::atomic<size_t> total_operations{0};
    std::atomic<size_t> successful_operations{0};
    std::atomic<double> average_response_time{0.0};
    std::atomic<double> peak_memory_usage{0.0};

    // Custom copy constructor to handle atomic members
    AnalyticsData() = default;
    AnalyticsData(const AnalyticsData &other)
        : response_times(other.response_times),
          memory_usage(other.memory_usage),
          cpu_usage(other.cpu_usage),
          start_time(other.start_time),
          total_operations(other.total_operations.load()),
          successful_operations(other.successful_operations.load()),
          average_response_time(other.average_response_time.load()),
          peak_memory_usage(other.peak_memory_usage.load()) {}

    // Custom assignment operator to handle atomic members
    AnalyticsData &operator=(const AnalyticsData &other) {
        if (this != &other) {
            response_times = other.response_times;
            memory_usage = other.memory_usage;
            cpu_usage = other.cpu_usage;
            start_time = other.start_time;
            total_operations.store(other.total_operations.load());
            successful_operations.store(other.successful_operations.load());
            average_response_time.store(other.average_response_time.load());
            peak_memory_usage.store(other.peak_memory_usage.load());
        }
        return *this;
    }
};

/**
 * @struct BottleneckInfo
 * @brief Result description produced by bottleneck detection routines.
 *
 * Fields:
 *  - component_name: logical name of the subsystem/component identified.
 *  - bottleneck_type: category such as "CPU", "Memory", "I/O", "Network".
 *  - severity_score: numeric severity (0-100) where higher means more severe.
 *  - description: human-readable explanation of the finding.
 *  - recommendations: list of practical steps to mitigate the bottleneck.
 *  - detected_at: detection timestamp.
 *
 * Consumers can use recommendations to drive automated or manual optimizations.
 */
struct BottleneckInfo {
    QString component_name;
    QString bottleneck_type;  // "CPU", "Memory", "I/O", "Network"
    double severity_score;    // 0-100, higher is worse
    QString description;
    QStringList recommendations;
    QDateTime detected_at;
};

/**
 * @struct PredictiveModel
 * @brief Lightweight predictive model used for simple forecasting of metrics.
 *
 * This structure stores historical data and parameters describing a basic
 * forecasting model (trend + seasonal adjustment). Implementations are
 * intentionally simple and intended for best-effort predictions.
 *
 * Methods:
 *  - predict_next_value(): predict next scalar value using stored state.
 *  - predict_value_at_time(): predict value at a future timestamp.
 *
 * Note: predictions are probabilistic; callers should treat outputs as hints.
 */
struct PredictiveModel {
    std::vector<double> historical_data;
    double trend_coefficient = 0.0;
    double seasonal_factor = 1.0;
    double confidence_interval = 0.95;
    QDateTime last_update;

    double predict_next_value() const;
    double predict_value_at_time(const QDateTime &future_time) const;
};

// **Legacy compatibility typedef**
using PerformanceMetrics = AdvancedPerformanceMetrics;

/**
 * @class PerformanceMonitor
 * @brief Central class for collecting, analyzing and reporting performance
 * metrics.
 *
 * Responsibilities:
 *  - Collect detailed phase-level metrics for reload operations.
 *  - Maintain sliding-window analytics and compute basic trend statistics.
 *  - Provide predictive modeling hooks and bottleneck detection mechanisms.
 *  - Expose callbacks and timers to emit warnings/notifications when thresholds
 * are exceeded.
 *
 * Thread safety:
 *  - PerformanceMonitor supports concurrent recordings from worker threads.
 *  - Internal state is protected with a shared_mutex and atomics where
 * appropriate.
 *
 * Typical usage:
 *  - Call startMonitoring()/stopMonitoring() to control monitoring lifecycle.
 *  - Use MEASURE_PERFORMANCE(monitor, "op") or startOperation()/endOperation()
 *    to instrument specific code regions.
 *  - Register callbacks via setPerformanceCallback() / setBottleneckCallback()
 *    to integrate with external telemetry or UI diagnostics.
 */
class PerformanceMonitor : public QObject {
    Q_OBJECT

public:
    explicit PerformanceMonitor(QObject *parent = nullptr);
    ~PerformanceMonitor() override;

    // Delete copy constructor and assignment operator because of std::atomic
    // members
    PerformanceMonitor(const PerformanceMonitor &) = delete;
    PerformanceMonitor &operator=(const PerformanceMonitor &) = delete;

    // **Enhanced monitoring control**

    /**
     * @brief Start the background monitoring timers and services.
     *
     * This enables periodic checks (thresholds, predictions, bottleneck scans)
     * and allows recorded metrics to be aggregated into history.
     */
    void startMonitoring();

    /** @brief Stop monitoring and release timers/threads. */
    void stopMonitoring();

    /** @brief Temporarily pause collection while retaining historical data. */
    void pauseMonitoring();

    /** @brief Resume collection after pause. */
    void resumeMonitoring();

    /** @return true when monitoring is currently enabled. */
    bool isMonitoring() const { return monitoring_enabled_.load(); }

    // **Advanced performance tracking**

    /**
     * @brief Mark the beginning of a named operation for timing.
     * @param operation_name Stable name used to pair start/end calls.
     *
     * startOperation/endOperation may be called from arbitrary threads.
     */
    void startOperation(const QString &operation_name);

    /**
     * @brief Mark the completion of a previously started operation.
     * @param operation_name Name passed to startOperation.
     */
    void endOperation(const QString &operation_name);

    /**
     * @brief Record detailed metrics for a completed reload operation.
     * @param file_path Associated file path (optional).
     * @param metrics AdvancedPerformanceMetrics instance with collected values.
     *
     * This method appends metrics to history, updates analytics and triggers
     * threshold checks and callbacks.
     */
    void recordReloadMetrics(const QString &file_path,
                             const AdvancedPerformanceMetrics &metrics);

    /** @brief Record a sampled memory usage value (MB). */
    void recordMemoryUsage(size_t memory_mb);

    /** @brief Record a sampled CPU usage percentage (0-100). */
    void recordCPUUsage(double cpu_percent);

    // **Real-time analytics**

    /** @brief Enable or disable real-time analytics aggregation. */
    void enableRealTimeAnalytics(bool enabled);

    /** @brief Snapshot current analytics data for consumption (thread-safe). */
    AnalyticsData getRealTimeAnalytics() const;

    /** @brief Return a JSON object suitable for dashboarding. */
    QJsonObject getAnalyticsDashboard() const;

    // **Predictive modeling**

    /** @brief Enable or disable predictive modeling features. */
    void enablePredictiveModeling(bool enabled);

    /** @brief Predict the next response time using trained models. */
    double predictNextResponseTime() const;

    /** @brief Predict memory usage after N minutes (best-effort). */
    double predictMemoryUsageIn(int minutes) const;

    /** @brief Produce a JSON report of model predictions. */
    QJsonObject getPredictionReport() const;

    // **Bottleneck detection**

    /** @brief Toggle automatic bottleneck detection. */
    void enableBottleneckDetection(bool enabled);

    /** @brief Run analysis and return detected bottlenecks. */
    std::vector<BottleneckInfo> detectBottlenecks() const;

    /** @brief Return the most severe bottleneck (if any). */
    BottleneckInfo getMostCriticalBottleneck() const;

    // **Enhanced statistics and helpers */

    AdvancedPerformanceMetrics getAverageMetrics() const;
    AdvancedPerformanceMetrics getMetricsForFile(
        const QString &file_path) const;
    QStringList getSlowFiles(int threshold_ms = 1000) const;
    QStringList getMemoryHeavyFiles(size_t threshold_mb = 50) const;
    double getSuccessRate() const;
    double getPerformanceScore() const;
    double getReliabilityScore() const;

    // **Advanced configuration**

    void setMaxHistorySize(int size);
    void setWarningThreshold(int threshold_ms);
    void setMemoryWarningThreshold(size_t threshold_mb);
    void setCPUWarningThreshold(double threshold_percent);

    /**
     * @brief Register a callback invoked when a new performance sample is
     * recorded.
     * @param callback Callable receiving file path and metrics.
     */
    void setPerformanceCallback(
        std::function<void(const QString &, const AdvancedPerformanceMetrics &)>
            callback);

    /**
     * @brief Register a callback invoked when bottlenecks are detected.
     * @param callback Callable receiving BottleneckInfo.
     */
    void setBottleneckCallback(
        std::function<void(const BottleneckInfo &)> callback);

    // **Memory profiling**

    void enableMemoryProfiling(bool enabled);
    QJsonObject getMemoryProfile() const;
    void forceGarbageCollection();

    // **Performance optimization control**

    void optimizePerformance();
    QStringList getOptimizationRecommendations() const;
    void applyAutomaticOptimizations(bool enabled);

    // **Reporting and history**

    QString generateReport() const;
    QString generateDetailedReport() const;
    QJsonObject generateJSONReport() const;
    void exportReportToFile(const QString &file_path) const;
    void clearHistory();
    void clearPredictiveModels();

signals:
    /** Emitted when a performance metric exceeds configured warning threshold.
     */
    void performanceWarning(const QString &file_path, qint64 time_ms);
    void memoryWarning(size_t memory_mb);
    void cpuWarning(double cpu_percent);
    void slowOperationDetected(const QString &operation, qint64 time_ms);
    void bottleneckDetected(const BottleneckInfo &bottleneck);
    void performanceOptimized(const QString &optimization_applied);
    void predictionUpdated(const QString &metric, double predicted_value);

private slots:
    void onPerformanceCheck();
    void onRealTimeAnalyticsUpdate();
    void onBottleneckDetectionCheck();
    void onPredictiveModelUpdate();
    void onMemoryCleanup();

private:
    // **Core monitoring state with thread safety**
    std::atomic<bool> monitoring_enabled_{false};
    std::atomic<bool> real_time_analytics_enabled_{false};
    std::atomic<bool> predictive_modeling_enabled_{false};
    std::atomic<bool> bottleneck_detection_enabled_{false};
    std::atomic<bool> memory_profiling_enabled_{false};
    std::atomic<bool> automatic_optimizations_enabled_{false};

    // **Configuration with atomic access**
    std::atomic<int> max_history_size_{1000};
    std::atomic<int> warning_threshold_ms_{1000};
    std::atomic<size_t> memory_warning_threshold_mb_{100};
    std::atomic<double> cpu_warning_threshold_percent_{80.0};

    // **Thread-safe data structures**
    std::unordered_map<QString, QElapsedTimer> active_operations_;
    std::deque<AdvancedPerformanceMetrics> metrics_history_;
    std::unordered_map<QString, std::deque<AdvancedPerformanceMetrics>>
        file_metrics_;
    mutable std::shared_mutex data_mutex_;

    // **Real-time analytics**
    std::unique_ptr<AnalyticsData> analytics_data_;
    std::queue<double> response_time_window_;
    std::queue<double> memory_usage_window_;
    std::queue<double> cpu_usage_window_;
    static constexpr size_t ANALYTICS_WINDOW_SIZE = 100;

    // **Predictive modeling**
    std::unordered_map<QString, std::unique_ptr<PredictiveModel>>
        predictive_models_;
    std::chrono::steady_clock::time_point last_prediction_update_;

    // **Bottleneck detection**
    std::vector<BottleneckInfo> detected_bottlenecks_;
    std::unordered_set<QString> known_bottlenecks_;
    std::chrono::steady_clock::time_point last_bottleneck_check_;

    // **Memory profiling**
    std::vector<size_t> memory_snapshots_;
    std::unordered_map<QString, size_t> memory_usage_by_file_;
    size_t baseline_memory_usage_ = 0;

    // **Performance optimization**
    std::vector<QString> applied_optimizations_;
    std::unordered_map<QString, double> optimization_impact_;

    // **Callbacks and timers**
    std::function<void(const QString &, const AdvancedPerformanceMetrics &)>
        performance_callback_;
    std::function<void(const BottleneckInfo &)> bottleneck_callback_;

    std::unique_ptr<QTimer> performance_timer_;
    std::unique_ptr<QTimer> analytics_timer_;
    std::unique_ptr<QTimer> bottleneck_timer_;
    std::unique_ptr<QTimer> prediction_timer_;
    std::unique_ptr<QTimer> memory_cleanup_timer_;

    // **Performance counters**
    std::atomic<size_t> total_operations_{0};
    std::atomic<size_t> successful_operations_{0};
    std::atomic<size_t> failed_operations_{0};
    std::atomic<double> total_response_time_{0.0};
    std::atomic<size_t> peak_memory_usage_{0};
    std::atomic<double> peak_cpu_usage_{0.0};

    // **Core internal helpers**
    void checkPerformanceThresholds(const QString &file_path,
                                    const AdvancedPerformanceMetrics &metrics);
    AdvancedPerformanceMetrics calculateAverageFromHistory(
        const std::deque<AdvancedPerformanceMetrics> &history) const;
    void pruneHistory();

    // **Analytics methods**
    void updateRealTimeAnalytics(const AdvancedPerformanceMetrics &metrics);
    void updateAnalyticsWindow(std::queue<double> &window, double value);
    double calculateTrend(const std::queue<double> &window) const;

    // **Predictive modeling methods**
    void updatePredictiveModels(const AdvancedPerformanceMetrics &metrics);
    void trainModel(PredictiveModel &model, const std::vector<double> &data);
    double calculateSeasonalFactor(const std::vector<double> &data) const;

    // **Bottleneck detection methods**
    void analyzeBottlenecks();
    BottleneckInfo detectCPUBottleneck() const;
    BottleneckInfo detectMemoryBottleneck() const;
    BottleneckInfo detectIOBottleneck() const;
    double calculateBottleneckSeverity(const QString &type, double value) const;

    // **Memory profiling methods**
    void captureMemorySnapshot();
    size_t getCurrentMemoryUsage() const;
    void analyzeMemoryLeaks();

    // **Optimization methods**
    void analyzeOptimizationOpportunities();
    void applyPerformanceOptimization(const QString &optimization);
    double measureOptimizationImpact(const QString &optimization);

    // **Utility methods**
    double calculatePerformanceScore(
        const AdvancedPerformanceMetrics &metrics) const;
    double calculateReliabilityScore() const;
    QJsonObject metricsToJson(const AdvancedPerformanceMetrics &metrics) const;
    QString formatDuration(qint64 milliseconds) const;
    QString formatMemorySize(size_t bytes) const;
};

/**
 * @class PerformanceMeasurement
 * @brief RAII helper to time a scoped operation and register it with
 * PerformanceMonitor.
 *
 * Usage:
 *  - Construct a PerformanceMeasurement at the start of a scope with a pointer
 *    to the monitor and an operation name. The destructor will call
 * endOperation.
 *
 * Thread-safety:
 *  - Instances are local to the scope that creates them; the underlying monitor
 *    accepts concurrent start/end calls.
 */
class PerformanceMeasurement {
public:
    explicit PerformanceMeasurement(PerformanceMonitor *monitor,
                                    const QString &operation_name);
    ~PerformanceMeasurement();

    // **Disable copy and move to avoid accidental double-end**
    PerformanceMeasurement(const PerformanceMeasurement &) = delete;
    PerformanceMeasurement &operator=(const PerformanceMeasurement &) = delete;
    PerformanceMeasurement(PerformanceMeasurement &&) = delete;
    PerformanceMeasurement &operator=(PerformanceMeasurement &&) = delete;

private:
    PerformanceMonitor *monitor_;
    QString operation_name_;
};

/** Convenience macro to measure performance of a scope. */
#define MEASURE_PERFORMANCE(monitor, operation_name) \
    PerformanceMeasurement __perf_measurement(monitor, operation_name)

}  // namespace DeclarativeUI::HotReload
