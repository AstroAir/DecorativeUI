#pragma once

#include <QDateTime>
#include <QElapsedTimer>
#include <QObject>
#include <QString>
#include <QTimer>
#include <QThread>
#include <QMutex>
#include <QReadWriteLock>
#include <QJsonObject>
#include <QJsonArray>
#include <deque>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <atomic>
#include <chrono>
#include <thread>
#include <vector>
#include <queue>
#include <memory>
#include <shared_mutex>
#include <algorithm>
#include <numeric>

namespace DeclarativeUI::HotReload {

// **Advanced performance metrics with detailed breakdown**
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

// **Real-time analytics data**
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
    AnalyticsData(const AnalyticsData& other) 
        : response_times(other.response_times)
        , memory_usage(other.memory_usage)
        , cpu_usage(other.cpu_usage)
        , start_time(other.start_time)
        , total_operations(other.total_operations.load())
        , successful_operations(other.successful_operations.load())
        , average_response_time(other.average_response_time.load())
        , peak_memory_usage(other.peak_memory_usage.load())
    {}

    // Custom assignment operator to handle atomic members
    AnalyticsData& operator=(const AnalyticsData& other) {
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

// **Bottleneck detection result**
struct BottleneckInfo {
    QString component_name;
    QString bottleneck_type;  // "CPU", "Memory", "I/O", "Network"
    double severity_score;    // 0-100, higher is worse
    QString description;
    QStringList recommendations;
    QDateTime detected_at;
};

// **Predictive model data**
struct PredictiveModel {
    std::vector<double> historical_data;
    double trend_coefficient = 0.0;
    double seasonal_factor = 1.0;
    double confidence_interval = 0.95;
    QDateTime last_update;

    double predict_next_value() const;
    double predict_value_at_time(const QDateTime& future_time) const;
};

// **Legacy compatibility**
using PerformanceMetrics = AdvancedPerformanceMetrics;

// **Advanced Performance Monitor with AI-powered analytics**
class PerformanceMonitor : public QObject {
    Q_OBJECT

public:
    explicit PerformanceMonitor(QObject *parent = nullptr);
    ~PerformanceMonitor() override;

    // Delete copy constructor and assignment operator because of std::atomic members
    PerformanceMonitor(const PerformanceMonitor&) = delete;
    PerformanceMonitor& operator=(const PerformanceMonitor&) = delete;

    // **Enhanced monitoring control**
    void startMonitoring();
    void stopMonitoring();
    void pauseMonitoring();
    void resumeMonitoring();
    bool isMonitoring() const { return monitoring_enabled_.load(); }

    // **Advanced performance tracking**
    void startOperation(const QString &operation_name);
    void endOperation(const QString &operation_name);
    void recordReloadMetrics(const QString &file_path, const AdvancedPerformanceMetrics &metrics);
    void recordMemoryUsage(size_t memory_mb);
    void recordCPUUsage(double cpu_percent);

    // **Real-time analytics**
    void enableRealTimeAnalytics(bool enabled);
    AnalyticsData getRealTimeAnalytics() const;
    QJsonObject getAnalyticsDashboard() const;

    // **Predictive modeling**
    void enablePredictiveModeling(bool enabled);
    double predictNextResponseTime() const;
    double predictMemoryUsageIn(int minutes) const;
    QJsonObject getPredictionReport() const;

    // **Bottleneck detection**
    void enableBottleneckDetection(bool enabled);
    std::vector<BottleneckInfo> detectBottlenecks() const;
    BottleneckInfo getMostCriticalBottleneck() const;

    // **Enhanced statistics**
    AdvancedPerformanceMetrics getAverageMetrics() const;
    AdvancedPerformanceMetrics getMetricsForFile(const QString &file_path) const;
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
    void setPerformanceCallback(std::function<void(const QString &, const AdvancedPerformanceMetrics &)> callback);
    void setBottleneckCallback(std::function<void(const BottleneckInfo &)> callback);

    // **Memory profiling**
    void enableMemoryProfiling(bool enabled);
    QJsonObject getMemoryProfile() const;
    void forceGarbageCollection();

    // **Performance optimization**
    void optimizePerformance();
    QStringList getOptimizationRecommendations() const;
    void applyAutomaticOptimizations(bool enabled);

    // **Enhanced reports**
    QString generateReport() const;
    QString generateDetailedReport() const;
    QJsonObject generateJSONReport() const;
    void exportReportToFile(const QString &file_path) const;
    void clearHistory();
    void clearPredictiveModels();

signals:
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
    std::unordered_map<QString, std::deque<AdvancedPerformanceMetrics>> file_metrics_;
    mutable std::shared_mutex data_mutex_;

    // **Real-time analytics**
    std::unique_ptr<AnalyticsData> analytics_data_;
    std::queue<double> response_time_window_;
    std::queue<double> memory_usage_window_;
    std::queue<double> cpu_usage_window_;
    static constexpr size_t ANALYTICS_WINDOW_SIZE = 100;

    // **Predictive modeling**
    std::unordered_map<QString, std::unique_ptr<PredictiveModel>> predictive_models_;
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
    std::function<void(const QString &, const AdvancedPerformanceMetrics &)> performance_callback_;
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

    // **Core methods**
    void checkPerformanceThresholds(const QString &file_path, const AdvancedPerformanceMetrics &metrics);
    AdvancedPerformanceMetrics calculateAverageFromHistory(const std::deque<AdvancedPerformanceMetrics> &history) const;
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
    double calculatePerformanceScore(const AdvancedPerformanceMetrics &metrics) const;
    double calculateReliabilityScore() const;
    QJsonObject metricsToJson(const AdvancedPerformanceMetrics &metrics) const;
    QString formatDuration(qint64 milliseconds) const;
    QString formatMemorySize(size_t bytes) const;
};

// **Performance measurement helper class**
class PerformanceMeasurement {
public:
    explicit PerformanceMeasurement(PerformanceMonitor *monitor,
                                    const QString &operation_name);
    ~PerformanceMeasurement();

    // **Disable copy and move**
    PerformanceMeasurement(const PerformanceMeasurement &) = delete;
    PerformanceMeasurement &operator=(const PerformanceMeasurement &) = delete;
    PerformanceMeasurement(PerformanceMeasurement &&) = delete;
    PerformanceMeasurement &operator=(PerformanceMeasurement &&) = delete;

private:
    PerformanceMonitor *monitor_;
    QString operation_name_;
};

// **Convenience macro for performance measurement**
#define MEASURE_PERFORMANCE(monitor, operation_name) \
    PerformanceMeasurement __perf_measurement(monitor, operation_name)

}  // namespace DeclarativeUI::HotReload
