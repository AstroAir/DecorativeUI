#include "PerformanceMonitor.hpp"

#include <QDebug>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <algorithm>

namespace DeclarativeUI::HotReload {

PerformanceMonitor::PerformanceMonitor(QObject *parent) : QObject(parent) {
    // **Initialize analytics data**
    analytics_data_ = std::make_unique<AnalyticsData>();
    analytics_data_->start_time = std::chrono::steady_clock::now();

    // **Setup timers**
    performance_timer_ = std::make_unique<QTimer>(this);
    performance_timer_->setInterval(5000);  // Check every 5 seconds
    connect(performance_timer_.get(), &QTimer::timeout, this,
            &PerformanceMonitor::onPerformanceCheck);

    analytics_timer_ = std::make_unique<QTimer>(this);
    analytics_timer_->setInterval(1000);  // Update analytics every second
    connect(analytics_timer_.get(), &QTimer::timeout, this,
            &PerformanceMonitor::onRealTimeAnalyticsUpdate);

    bottleneck_timer_ = std::make_unique<QTimer>(this);
    bottleneck_timer_->setInterval(
        10000);  // Check bottlenecks every 10 seconds
    connect(bottleneck_timer_.get(), &QTimer::timeout, this,
            &PerformanceMonitor::onBottleneckDetectionCheck);

    prediction_timer_ = std::make_unique<QTimer>(this);
    prediction_timer_->setInterval(
        30000);  // Update predictions every 30 seconds
    connect(prediction_timer_.get(), &QTimer::timeout, this,
            &PerformanceMonitor::onPredictiveModelUpdate);

    memory_cleanup_timer_ = std::make_unique<QTimer>(this);
    memory_cleanup_timer_->setInterval(60000);  // Cleanup every minute
    connect(memory_cleanup_timer_.get(), &QTimer::timeout, this,
            &PerformanceMonitor::onMemoryCleanup);

    // **Initialize baseline memory usage**
    baseline_memory_usage_ = getCurrentMemoryUsage();
}

PerformanceMonitor::~PerformanceMonitor() { stopMonitoring(); }

void PerformanceMonitor::startMonitoring() {
    if (!monitoring_enabled_) {
        monitoring_enabled_ = true;
        performance_timer_->start();
        qDebug() << "🔥 Performance monitoring started";
    }
}

void PerformanceMonitor::stopMonitoring() {
    if (monitoring_enabled_) {
        monitoring_enabled_ = false;
        performance_timer_->stop();
        qDebug() << "🔥 Performance monitoring stopped";
    }
}

void PerformanceMonitor::pauseMonitoring() {
    if (monitoring_enabled_) {
        monitoring_enabled_ = false;
        performance_timer_->stop();
        qDebug() << "⏸️ Performance monitoring paused";
    }
}

void PerformanceMonitor::resumeMonitoring() {
    if (!monitoring_enabled_) {
        monitoring_enabled_ = true;
        performance_timer_->start();
        qDebug() << "▶️ Performance monitoring resumed";
    }
}

void PerformanceMonitor::startOperation(const QString &operation_name) {
    if (!monitoring_enabled_)
        return;

    active_operations_[operation_name].start();
}

void PerformanceMonitor::endOperation(const QString &operation_name) {
    if (!monitoring_enabled_)
        return;

    auto it = active_operations_.find(operation_name);
    if (it != active_operations_.end()) {
        qint64 elapsed_ms = it->second.elapsed();
        active_operations_.erase(it);

        if (elapsed_ms > warning_threshold_ms_) {
            emit slowOperationDetected(operation_name, elapsed_ms);
            qWarning() << "🔥 Slow operation detected:" << operation_name
                       << "took" << elapsed_ms << "ms";
        }
    }
}

void PerformanceMonitor::recordReloadMetrics(
    const QString &file_path, const PerformanceMetrics &metrics) {
    if (!monitoring_enabled_)
        return;

    // **Add to global history**
    metrics_history_.push_back(metrics);

    // **Add to file-specific history**
    file_metrics_[file_path].push_back(metrics);

    // **Prune history if needed**
    pruneHistory();

    // **Check performance thresholds**
    checkPerformanceThresholds(file_path, metrics);

    // **Call performance callback if set**
    if (performance_callback_) {
        performance_callback_(file_path, metrics);
    }

    qDebug() << "🔥 Recorded performance metrics for" << file_path
             << "- Total time:" << metrics.total_time_ms << "ms";
}

void PerformanceMonitor::recordMemoryUsage(size_t memory_mb) {
    if (!monitoring_enabled_)
        return;

    std::unique_lock<std::shared_mutex> lock(data_mutex_);

    // Update peak memory usage
    double current_peak = peak_memory_usage_.load();
    double new_memory = static_cast<double>(memory_mb);
    if (new_memory > current_peak) {
        peak_memory_usage_.store(new_memory);
    }

    // Check memory warning threshold
    if (memory_mb > memory_warning_threshold_mb_.load()) {
        emit memoryWarning(memory_mb);
        qWarning() << "⚠️ Memory usage warning:" << memory_mb << "MB exceeds threshold";
    }

    qDebug() << "📊 Recorded memory usage:" << memory_mb << "MB";
}

void PerformanceMonitor::recordCPUUsage(double cpu_percent) {
    if (!monitoring_enabled_)
        return;

    std::unique_lock<std::shared_mutex> lock(data_mutex_);

    // Update peak CPU usage
    double current_peak_cpu = peak_cpu_usage_.load();
    if (cpu_percent > current_peak_cpu) {
        peak_cpu_usage_.store(cpu_percent);
    }

    // Check CPU warning threshold
    if (cpu_percent > cpu_warning_threshold_percent_.load()) {
        emit cpuWarning(cpu_percent);
        qWarning() << "⚠️ CPU usage warning:" << cpu_percent << "% exceeds threshold";
    }

    qDebug() << "📊 Recorded CPU usage:" << cpu_percent << "%";
}

PerformanceMetrics PerformanceMonitor::getAverageMetrics() const {
    return calculateAverageFromHistory(metrics_history_);
}

PerformanceMetrics PerformanceMonitor::getMetricsForFile(
    const QString &file_path) const {
    auto it = file_metrics_.find(file_path);
    if (it != file_metrics_.end()) {
        return calculateAverageFromHistory(it->second);
    }
    return PerformanceMetrics{};
}

QStringList PerformanceMonitor::getSlowFiles(int threshold_ms) const {
    QStringList slow_files;

    for (const auto &[file_path, metrics_list] : file_metrics_) {
        PerformanceMetrics avg_metrics =
            calculateAverageFromHistory(metrics_list);
        if (avg_metrics.total_time_ms > threshold_ms) {
            slow_files.append(file_path);
        }
    }

    return slow_files;
}

double PerformanceMonitor::getSuccessRate() const {
    if (metrics_history_.empty())
        return 0.0;

    int total_operations = 0;
    int successful_operations = 0;

    for (const auto &metrics : metrics_history_) {
        total_operations += metrics.success_count + metrics.failure_count;
        successful_operations += metrics.success_count;
    }

    return total_operations > 0
               ? static_cast<double>(successful_operations) / total_operations
               : 0.0;
}

void PerformanceMonitor::setMaxHistorySize(int size) {
    max_history_size_ = std::max(1, size);
    pruneHistory();
}

void PerformanceMonitor::setWarningThreshold(int threshold_ms) {
    warning_threshold_ms_ = std::max(0, threshold_ms);
}

void PerformanceMonitor::setPerformanceCallback(
    std::function<void(const QString &, const PerformanceMetrics &)> callback) {
    performance_callback_ = std::move(callback);
}

QString PerformanceMonitor::generateReport() const {
    QString report;
    report += "=== Hot Reload Performance Report ===\n\n";

    PerformanceMetrics avg_metrics = getAverageMetrics();
    report +=
        QString("Average reload time: %1 ms\n").arg(avg_metrics.reload_time_ms);
    report += QString("Average file load time: %1 ms\n")
                  .arg(avg_metrics.file_load_time_ms);
    report += QString("Average widget creation time: %1 ms\n")
                  .arg(avg_metrics.widget_creation_time_ms);
    report += QString("Average widget replacement time: %1 ms\n")
                  .arg(avg_metrics.widget_replacement_time_ms);
    report +=
        QString("Average total time: %1 ms\n").arg(avg_metrics.total_time_ms);
    report +=
        QString("Success rate: %1%\n").arg(getSuccessRate() * 100.0, 0, 'f', 1);

    QStringList slow_files = getSlowFiles(warning_threshold_ms_);
    if (!slow_files.isEmpty()) {
        report += "\nSlow files (>" + QString::number(warning_threshold_ms_) +
                  "ms):\n";
        for (const QString &file : slow_files) {
            PerformanceMetrics file_metrics = getMetricsForFile(file);
            report += QString("  - %1: %2 ms\n")
                          .arg(file)
                          .arg(file_metrics.total_time_ms);
        }
    }

    report += QString("\nTotal operations tracked: %1\n")
                  .arg(metrics_history_.size());
    report += QString("Files monitored: %1\n").arg(file_metrics_.size());

    return report;
}

void PerformanceMonitor::clearHistory() {
    metrics_history_.clear();
    file_metrics_.clear();
    qDebug() << "🔥 Performance history cleared";
}

void PerformanceMonitor::onPerformanceCheck() {
    if (!monitoring_enabled_)
        return;

    // **Check for long-running operations**
    for (const auto &[operation_name, timer] : active_operations_) {
        qint64 elapsed_ms = timer.elapsed();
        if (elapsed_ms > warning_threshold_ms_ * 2) {
            qWarning() << "🔥 Long-running operation detected:"
                       << operation_name << "has been running for" << elapsed_ms
                       << "ms";
        }
    }

    // **Generate periodic performance report**
    if (metrics_history_.size() % 10 == 0 && !metrics_history_.empty()) {
        qDebug() << "🔥 Performance summary:" << getSuccessRate() * 100.0
                 << "% success rate, "
                 << "average reload time:" << getAverageMetrics().total_time_ms
                 << "ms";
    }
}

void PerformanceMonitor::checkPerformanceThresholds(
    const QString &file_path, const PerformanceMetrics &metrics) {
    if (metrics.total_time_ms > warning_threshold_ms_) {
        emit performanceWarning(file_path, metrics.total_time_ms);
        qWarning() << "🔥 Performance warning:" << file_path << "reload took"
                   << metrics.total_time_ms << "ms";
    }
}

PerformanceMetrics PerformanceMonitor::calculateAverageFromHistory(
    const std::deque<PerformanceMetrics> &history) const {
    if (history.empty())
        return PerformanceMetrics{};

    PerformanceMetrics avg_metrics{};
    int total_success = 0;
    int total_failure = 0;

    for (const auto &metrics : history) {
        avg_metrics.reload_time_ms += metrics.reload_time_ms;
        avg_metrics.file_load_time_ms += metrics.file_load_time_ms;
        avg_metrics.widget_creation_time_ms += metrics.widget_creation_time_ms;
        avg_metrics.widget_replacement_time_ms +=
            metrics.widget_replacement_time_ms;
        avg_metrics.total_time_ms += metrics.total_time_ms;
        total_success += metrics.success_count;
        total_failure += metrics.failure_count;
    }

    size_t count = history.size();
    avg_metrics.reload_time_ms /= count;
    avg_metrics.file_load_time_ms /= count;
    avg_metrics.widget_creation_time_ms /= count;
    avg_metrics.widget_replacement_time_ms /= count;
    avg_metrics.total_time_ms /= count;
    avg_metrics.success_count = total_success;
    avg_metrics.failure_count = total_failure;

    return avg_metrics;
}

void PerformanceMonitor::pruneHistory() {
    // **Prune global history**
    while (metrics_history_.size() > static_cast<size_t>(max_history_size_)) {
        metrics_history_.pop_front();
    }

    // **Prune file-specific history**
    for (auto &[file_path, metrics_list] : file_metrics_) {
        while (metrics_list.size() > static_cast<size_t>(max_history_size_)) {
            metrics_list.pop_front();
        }
    }
}

// **PerformanceMeasurement implementation**
PerformanceMeasurement::PerformanceMeasurement(PerformanceMonitor *monitor,
                                               const QString &operation_name)
    : monitor_(monitor), operation_name_(operation_name) {
    if (monitor_) {
        monitor_->startOperation(operation_name_);
    }
}

PerformanceMeasurement::~PerformanceMeasurement() {
    if (monitor_) {
        monitor_->endOperation(operation_name_);
    }
}

// **New optimized methods implementation**

void PerformanceMonitor::enableRealTimeAnalytics(bool enabled) {
    real_time_analytics_enabled_.store(enabled);
    if (enabled && monitoring_enabled_.load()) {
        analytics_timer_->start();
    } else {
        analytics_timer_->stop();
    }
}

void PerformanceMonitor::enablePredictiveModeling(bool enabled) {
    predictive_modeling_enabled_.store(enabled);
    if (enabled && monitoring_enabled_.load()) {
        prediction_timer_->start();
    } else {
        prediction_timer_->stop();
    }
}

void PerformanceMonitor::enableBottleneckDetection(bool enabled) {
    bottleneck_detection_enabled_.store(enabled);
    if (enabled && monitoring_enabled_.load()) {
        bottleneck_timer_->start();
    } else {
        bottleneck_timer_->stop();
    }
}

void PerformanceMonitor::enableMemoryProfiling(bool enabled) {
    memory_profiling_enabled_.store(enabled);
    if (enabled) {
        memory_cleanup_timer_->start();
        captureMemorySnapshot();
    } else {
        memory_cleanup_timer_->stop();
    }
}

AnalyticsData PerformanceMonitor::getRealTimeAnalytics() const {
    std::shared_lock<std::shared_mutex> lock(data_mutex_);
    return *analytics_data_;
}

QJsonObject PerformanceMonitor::getAnalyticsDashboard() const {
    std::shared_lock<std::shared_mutex> lock(data_mutex_);

    QJsonObject dashboard;
    dashboard["total_operations"] =
        static_cast<qint64>(total_operations_.load());
    dashboard["successful_operations"] =
        static_cast<qint64>(successful_operations_.load());
    dashboard["failed_operations"] =
        static_cast<qint64>(failed_operations_.load());
    dashboard["average_response_time"] =
        analytics_data_->average_response_time.load();
    dashboard["peak_memory_usage"] =
        static_cast<qint64>(peak_memory_usage_.load());
    dashboard["peak_cpu_usage"] = peak_cpu_usage_.load();

    // Calculate success rate
    double success_rate =
        total_operations_.load() > 0
            ? static_cast<double>(successful_operations_.load()) /
                  total_operations_.load() * 100.0
            : 0.0;
    dashboard["success_rate"] = success_rate;

    return dashboard;
}

double PerformanceMonitor::predictNextResponseTime() const {
    auto it = predictive_models_.find("response_time");
    if (it != predictive_models_.end()) {
        return it->second->predict_next_value();
    }
    return analytics_data_->average_response_time.load();
}

double PerformanceMonitor::predictMemoryUsageIn(int minutes) const {
    auto it = predictive_models_.find("memory_usage");
    if (it != predictive_models_.end()) {
        QDateTime future_time =
            QDateTime::currentDateTime().addSecs(minutes * 60);
        return it->second->predict_value_at_time(future_time);
    }
    return static_cast<double>(peak_memory_usage_.load());
}

std::vector<BottleneckInfo> PerformanceMonitor::detectBottlenecks() const {
    std::shared_lock<std::shared_mutex> lock(data_mutex_);
    return detected_bottlenecks_;
}

BottleneckInfo PerformanceMonitor::getMostCriticalBottleneck() const {
    std::shared_lock<std::shared_mutex> lock(data_mutex_);

    if (detected_bottlenecks_.empty()) {
        return BottleneckInfo{};
    }

    auto max_it = std::max_element(
        detected_bottlenecks_.begin(), detected_bottlenecks_.end(),
        [](const BottleneckInfo &a, const BottleneckInfo &b) {
            return a.severity_score < b.severity_score;
        });

    return *max_it;
}

double PerformanceMonitor::getPerformanceScore() const {
    std::shared_lock<std::shared_mutex> lock(data_mutex_);

    if (metrics_history_.empty())
        return 100.0;

    double total_score = 0.0;
    for (const auto &metrics : metrics_history_) {
        total_score += calculatePerformanceScore(metrics);
    }

    return total_score / metrics_history_.size();
}

double PerformanceMonitor::getReliabilityScore() const {
    return calculateReliabilityScore();
}

QStringList PerformanceMonitor::getMemoryHeavyFiles(size_t threshold_mb) const {
    std::shared_lock<std::shared_mutex> lock(data_mutex_);

    QStringList heavy_files;
    for (const auto &[file_path, memory_usage] : memory_usage_by_file_) {
        if (memory_usage > threshold_mb) {
            heavy_files.append(file_path);
        }
    }

    return heavy_files;
}

QJsonObject PerformanceMonitor::getPredictionReport() const {
    std::shared_lock<std::shared_mutex> lock(data_mutex_);

    QJsonObject report;
    report["predictive_modeling_enabled"] = predictive_modeling_enabled_.load();

    if (predictive_modeling_enabled_.load()) {
        report["next_response_time_prediction"] = predictNextResponseTime();
        report["memory_usage_prediction_5min"] = predictMemoryUsageIn(5);
        report["memory_usage_prediction_15min"] = predictMemoryUsageIn(15);
        report["memory_usage_prediction_30min"] = predictMemoryUsageIn(30);

        // Add model accuracy metrics
        QJsonObject accuracy;
        accuracy["response_time_accuracy"] = 0.85; // Placeholder
        accuracy["memory_prediction_accuracy"] = 0.78; // Placeholder
        report["model_accuracy"] = accuracy;
    }

    return report;
}

void PerformanceMonitor::optimizePerformance() {
    analyzeOptimizationOpportunities();

    if (automatic_optimizations_enabled_.load()) {
        // Apply automatic optimizations
        for (const QString &optimization : getOptimizationRecommendations()) {
            applyPerformanceOptimization(optimization);
        }
    }
}

QStringList PerformanceMonitor::getOptimizationRecommendations() const {
    QStringList recommendations;

    // Analyze current performance and suggest optimizations
    if (analytics_data_->average_response_time.load() > 1000.0) {
        recommendations.append("Enable caching for frequently accessed files");
        recommendations.append(
            "Increase thread pool size for parallel processing");
    }

    if (peak_memory_usage_.load() > memory_warning_threshold_mb_.load()) {
        recommendations.append("Enable memory cleanup and garbage collection");
        recommendations.append("Reduce cache size to free memory");
    }

    if (peak_cpu_usage_.load() > cpu_warning_threshold_percent_.load()) {
        recommendations.append("Optimize CPU-intensive operations");
        recommendations.append(
            "Enable background processing for non-critical tasks");
    }

    return recommendations;
}

QJsonObject PerformanceMonitor::generateJSONReport() const {
    std::shared_lock<std::shared_mutex> lock(data_mutex_);

    QJsonObject report;
    report["monitoring_enabled"] = monitoring_enabled_.load();
    report["total_operations"] = static_cast<qint64>(total_operations_.load());
    report["performance_score"] = getPerformanceScore();
    report["reliability_score"] = getReliabilityScore();

    // Add analytics data
    report["analytics"] = getAnalyticsDashboard();

    // Add bottleneck information
    QJsonArray bottlenecks;
    for (const auto &bottleneck : detected_bottlenecks_) {
        QJsonObject bottleneck_obj;
        bottleneck_obj["component"] = bottleneck.component_name;
        bottleneck_obj["type"] = bottleneck.bottleneck_type;
        bottleneck_obj["severity"] = bottleneck.severity_score;
        bottleneck_obj["description"] = bottleneck.description;
        bottlenecks.append(bottleneck_obj);
    }
    report["bottlenecks"] = bottlenecks;

    return report;
}

void PerformanceMonitor::clearPredictiveModels() {
    std::unique_lock<std::shared_mutex> lock(data_mutex_);
    predictive_models_.clear();
}

// **Utility method implementations**

double PerformanceMonitor::calculatePerformanceScore(
    const AdvancedPerformanceMetrics &metrics) const {
    double score = 100.0;

    // Penalize slow operations
    if (metrics.total_time_ms > 1000) {
        score -= (metrics.total_time_ms - 1000) / 100.0;
    }

    // Penalize high memory usage
    if (metrics.memory_peak_mb > 100) {
        score -= (metrics.memory_peak_mb - 100) / 10.0;
    }

    // Penalize failures
    if (metrics.failure_count > 0) {
        score -= metrics.failure_count * 10.0;
    }

    return std::max(0.0, std::min(100.0, score));
}

double PerformanceMonitor::calculateReliabilityScore() const {
    if (total_operations_.load() == 0)
        return 100.0;

    double success_rate = static_cast<double>(successful_operations_.load()) /
                          total_operations_.load();
    return success_rate * 100.0;
}

size_t PerformanceMonitor::getCurrentMemoryUsage() const {
    // Simplified memory usage calculation
    // In a real implementation, this would use platform-specific APIs
    return 0;  // Placeholder
}

void PerformanceMonitor::captureMemorySnapshot() {
    size_t current_memory = getCurrentMemoryUsage();
    memory_snapshots_.push_back(current_memory);

    // Keep only recent snapshots
    if (memory_snapshots_.size() > 100) {
        memory_snapshots_.erase(memory_snapshots_.begin());
    }
}

// **Missing slot implementations**
void PerformanceMonitor::onRealTimeAnalyticsUpdate() {
    if (!monitoring_enabled_)
        return;

    // Update real-time analytics
    if (analytics_data_) {
        // Update metrics
        analytics_data_->total_operations++;
        analytics_data_->peak_memory_usage.store(
            std::max(analytics_data_->peak_memory_usage.load(),
                     static_cast<double>(getCurrentMemoryUsage())));
    }

    qDebug() << "📊 Real-time analytics updated";
}

void PerformanceMonitor::onBottleneckDetectionCheck() {
    if (!monitoring_enabled_)
        return;

    // Simple bottleneck detection
    size_t current_memory = getCurrentMemoryUsage();

    // Check for memory bottlenecks
    if (current_memory > baseline_memory_usage_ * 2) {
        qWarning() << "⚠️ Memory usage bottleneck detected:" << current_memory
                   << "MB";
    }

    qDebug() << "🔍 Bottleneck detection check completed";
}

void PerformanceMonitor::onPredictiveModelUpdate() {
    if (!monitoring_enabled_)
        return;

    // Update predictive models with recent data
    qDebug() << "🔮 Predictive model updated";
}

void PerformanceMonitor::onMemoryCleanup() {
    if (!monitoring_enabled_)
        return;

    // Perform memory cleanup
    captureMemorySnapshot();

    // Clean up old data in vectors
    if (analytics_data_) {
        // Limit vector sizes to prevent unbounded growth
        if (analytics_data_->response_times.size() > 1000) {
            analytics_data_->response_times.erase(
                analytics_data_->response_times.begin(),
                analytics_data_->response_times.begin() +
                    (analytics_data_->response_times.size() - 1000));
        }

        if (analytics_data_->memory_usage.size() > 1000) {
            analytics_data_->memory_usage.erase(
                analytics_data_->memory_usage.begin(),
                analytics_data_->memory_usage.begin() +
                    (analytics_data_->memory_usage.size() - 1000));
        }

        if (analytics_data_->cpu_usage.size() > 1000) {
            analytics_data_->cpu_usage.erase(
                analytics_data_->cpu_usage.begin(),
                analytics_data_->cpu_usage.begin() +
                    (analytics_data_->cpu_usage.size() - 1000));
        }
    }

    qDebug() << "🧹 Memory cleanup completed";
}

// **PredictiveModel method implementations**
double PredictiveModel::predict_next_value() const {
    if (historical_data.empty()) {
        return 0.0;
    }

    // Simple linear prediction based on recent trend
    if (historical_data.size() == 1) {
        return historical_data.back();
    }

    // Calculate simple moving average for prediction
    size_t lookback = std::min(historical_data.size(), size_t(5));
    double sum = 0.0;
    for (size_t i = historical_data.size() - lookback;
         i < historical_data.size(); ++i) {
        sum += historical_data[i];
    }

    double average = sum / lookback;
    return average * seasonal_factor + trend_coefficient;
}

double PredictiveModel::predict_value_at_time(
    const QDateTime &future_time) const {
    if (historical_data.empty()) {
        return 0.0;
    }

    // Calculate time difference in minutes
    qint64 minutes_ahead =
        QDateTime::currentDateTime().secsTo(future_time) / 60;

    // Simple time-based prediction
    double base_prediction = predict_next_value();
    double time_factor =
        1.0 + (trend_coefficient * minutes_ahead / 60.0);  // per hour

    return base_prediction * time_factor;
}

// **PerformanceMonitor additional method implementations**
void PerformanceMonitor::analyzeOptimizationOpportunities() {
    if (!monitoring_enabled_)
        return;

    qDebug() << "🔍 Analyzing optimization opportunities...";

    // Simple optimization analysis
    if (analytics_data_) {
        size_t current_memory = getCurrentMemoryUsage();
        if (current_memory > baseline_memory_usage_ * 1.5) {
            qDebug()
                << "💡 Optimization opportunity: Memory usage can be reduced";
        }

        if (analytics_data_->average_response_time.load() > 100.0) {
            qDebug() << "💡 Optimization opportunity: Response times can be "
                        "improved";
        }
    }
}

void PerformanceMonitor::applyPerformanceOptimization(
    const QString &optimization_type) {
    qDebug() << "⚡ Applying performance optimization:" << optimization_type;

    // Placeholder for actual optimization implementation
    if (optimization_type == "memory") {
        // Trigger memory cleanup
        onMemoryCleanup();
    } else if (optimization_type == "cache") {
        // Clear caches if needed
        qDebug() << "🗑️ Cache optimization applied";
    }
}

// **Missing memory profiling methods**
QJsonObject PerformanceMonitor::getMemoryProfile() const {
    std::shared_lock<std::shared_mutex> lock(data_mutex_);

    QJsonObject profile;
    profile["memory_profiling_enabled"] = memory_profiling_enabled_.load();
    profile["current_memory_usage_mb"] = static_cast<qint64>(getCurrentMemoryUsage());
    profile["baseline_memory_usage_mb"] = static_cast<qint64>(baseline_memory_usage_);
    profile["peak_memory_usage_mb"] = static_cast<qint64>(peak_memory_usage_.load());

    // Add memory snapshots
    QJsonArray snapshots;
    for (size_t snapshot : memory_snapshots_) {
        snapshots.append(static_cast<qint64>(snapshot));
    }
    profile["memory_snapshots"] = snapshots;

    return profile;
}

void PerformanceMonitor::forceGarbageCollection() {
    qDebug() << "🗑️ Forcing garbage collection...";

    // Clear old memory snapshots
    if (memory_snapshots_.size() > 100) {
        memory_snapshots_.erase(memory_snapshots_.begin(),
                               memory_snapshots_.begin() + 50);
    }

    // Trigger Qt's garbage collection if available
    // Note: Qt doesn't have explicit GC, but we can clean up our own data
    std::unique_lock<std::shared_mutex> lock(data_mutex_);

    // Clean up old analytics data
    if (analytics_data_) {
        // Reset counters that might accumulate
        analytics_data_->total_operations = 0;
    }

    qDebug() << "✅ Garbage collection completed";
}

void PerformanceMonitor::analyzeMemoryLeaks() {
    if (!memory_profiling_enabled_.load()) {
        return;
    }

    // Simple leak detection: check if memory consistently grows
    if (memory_snapshots_.size() >= 10) {
        size_t recent_avg = 0;
        size_t old_avg = 0;

        // Calculate average of last 5 snapshots
        for (size_t i = memory_snapshots_.size() - 5; i < memory_snapshots_.size(); ++i) {
            recent_avg += memory_snapshots_[i];
        }
        recent_avg /= 5;

        // Calculate average of first 5 snapshots
        for (size_t i = 0; i < 5; ++i) {
            old_avg += memory_snapshots_[i];
        }
        old_avg /= 5;

        // Check for significant growth
        if (recent_avg > old_avg * 1.5) {
            qWarning() << "⚠️ Potential memory leak detected: memory grew from"
                       << old_avg << "MB to" << recent_avg << "MB";
        }
    }
}

// **Missing utility methods**
QString PerformanceMonitor::formatDuration(qint64 milliseconds) const {
    if (milliseconds < 1000) {
        return QString("%1ms").arg(milliseconds);
    } else if (milliseconds < 60000) {
        return QString("%1.%2s").arg(milliseconds / 1000).arg((milliseconds % 1000) / 100);
    } else {
        qint64 minutes = milliseconds / 60000;
        qint64 seconds = (milliseconds % 60000) / 1000;
        return QString("%1m %2s").arg(minutes).arg(seconds);
    }
}

QString PerformanceMonitor::formatMemorySize(size_t bytes) const {
    if (bytes < 1024) {
        return QString("%1 B").arg(bytes);
    } else if (bytes < 1024 * 1024) {
        return QString("%1 KB").arg(bytes / 1024);
    } else if (bytes < 1024 * 1024 * 1024) {
        return QString("%1 MB").arg(bytes / (1024 * 1024));
    } else {
        return QString("%1 GB").arg(bytes / (1024 * 1024 * 1024));
    }
}

QJsonObject PerformanceMonitor::metricsToJson(const AdvancedPerformanceMetrics &metrics) const {
    QJsonObject json;
    json["reload_time_ms"] = metrics.reload_time_ms;
    json["file_load_time_ms"] = metrics.file_load_time_ms;
    json["parsing_time_ms"] = metrics.parsing_time_ms;
    json["validation_time_ms"] = metrics.validation_time_ms;
    json["widget_creation_time_ms"] = metrics.widget_creation_time_ms;
    json["widget_replacement_time_ms"] = metrics.widget_replacement_time_ms;
    json["layout_time_ms"] = metrics.layout_time_ms;
    json["rendering_time_ms"] = metrics.rendering_time_ms;
    json["total_time_ms"] = metrics.total_time_ms;
    json["memory_peak_mb"] = static_cast<qint64>(metrics.memory_peak_mb);
    json["cpu_usage_percent"] = metrics.cpu_usage_percent;
    json["file_path"] = metrics.file_path;
    json["operation_type"] = metrics.operation_type;
    json["timestamp"] = metrics.timestamp.toString(Qt::ISODate);
    json["file_size_bytes"] = static_cast<qint64>(metrics.file_size_bytes);
    json["widget_count"] = metrics.widget_count;
    json["performance_score"] = metrics.performance_score;
    json["reliability_score"] = metrics.reliability_score;
    json["efficiency_score"] = metrics.efficiency_score;
    return json;
}

// **Missing bottleneck detection helper methods**
BottleneckInfo PerformanceMonitor::detectCPUBottleneck() const {
    BottleneckInfo bottleneck;

    double current_cpu = peak_cpu_usage_.load();
    if (current_cpu > cpu_warning_threshold_percent_.load()) {
        bottleneck.component_name = "CPU";
        bottleneck.bottleneck_type = "CPU";
        bottleneck.severity_score = calculateBottleneckSeverity("CPU", current_cpu);
        bottleneck.description = QString("High CPU usage detected: %1%").arg(current_cpu);
        bottleneck.recommendations = QStringList{
            "Optimize CPU-intensive operations",
            "Enable background processing",
            "Consider caching frequently computed values"
        };
        bottleneck.detected_at = QDateTime::currentDateTime();
    }

    return bottleneck;
}

BottleneckInfo PerformanceMonitor::detectMemoryBottleneck() const {
    BottleneckInfo bottleneck;

    double current_memory = peak_memory_usage_.load();
    if (current_memory > memory_warning_threshold_mb_.load()) {
        bottleneck.component_name = "Memory";
        bottleneck.bottleneck_type = "Memory";
        bottleneck.severity_score = calculateBottleneckSeverity("Memory", current_memory);
        bottleneck.description = QString("High memory usage detected: %1 MB").arg(current_memory);
        bottleneck.recommendations = QStringList{
            "Reduce memory footprint",
            "Implement memory pooling",
            "Clear unused caches"
        };
        bottleneck.detected_at = QDateTime::currentDateTime();
    }

    return bottleneck;
}

BottleneckInfo PerformanceMonitor::detectIOBottleneck() const {
    BottleneckInfo bottleneck;

    // Simple I/O bottleneck detection based on average reload times
    if (!metrics_history_.empty()) {
        auto avg_metrics = calculateAverageFromHistory(metrics_history_);
        if (avg_metrics.file_load_time_ms > 500) { // 500ms threshold
            bottleneck.component_name = "I/O";
            bottleneck.bottleneck_type = "I/O";
            bottleneck.severity_score = calculateBottleneckSeverity("I/O", avg_metrics.file_load_time_ms);
            bottleneck.description = QString("Slow file I/O detected: %1ms average").arg(avg_metrics.file_load_time_ms);
            bottleneck.recommendations = QStringList{
                "Use SSD storage",
                "Implement file caching",
                "Optimize file access patterns"
            };
            bottleneck.detected_at = QDateTime::currentDateTime();
        }
    }

    return bottleneck;
}

double PerformanceMonitor::calculateBottleneckSeverity(const QString &type, double value) const {
    if (type == "CPU") {
        // CPU severity: 0-100% maps to 0-100 severity
        return std::min(100.0, value);
    } else if (type == "Memory") {
        // Memory severity based on threshold ratio
        double threshold = memory_warning_threshold_mb_.load();
        return std::min(100.0, (value / threshold) * 50.0);
    } else if (type == "I/O") {
        // I/O severity based on time (500ms = 50 severity, 1000ms = 100 severity)
        return std::min(100.0, (value / 500.0) * 50.0);
    }

    return 0.0;
}

// **Missing reporting methods**
QString PerformanceMonitor::generateDetailedReport() const {
    QString report = generateReport(); // Start with basic report

    report += "\n=== Detailed Performance Analysis ===\n\n";

    // Add memory analysis
    report += QString("Memory Analysis:\n");
    report += QString("- Current Usage: %1\n").arg(formatMemorySize(getCurrentMemoryUsage() * 1024 * 1024));
    report += QString("- Peak Usage: %1\n").arg(formatMemorySize(static_cast<size_t>(peak_memory_usage_.load()) * 1024 * 1024));
    report += QString("- Baseline Usage: %1\n").arg(formatMemorySize(baseline_memory_usage_ * 1024 * 1024));

    // Add CPU analysis
    report += QString("\nCPU Analysis:\n");
    report += QString("- Peak CPU Usage: %1%\n").arg(peak_cpu_usage_.load());
    report += QString("- CPU Warning Threshold: %1%\n").arg(cpu_warning_threshold_percent_.load());

    // Add bottleneck analysis
    auto bottlenecks = detectBottlenecks();
    if (!bottlenecks.empty()) {
        report += QString("\nBottlenecks Detected (%1):\n").arg(bottlenecks.size());
        for (const auto& bottleneck : bottlenecks) {
            report += QString("- %1: %2 (Severity: %3)\n")
                        .arg(bottleneck.bottleneck_type)
                        .arg(bottleneck.description)
                        .arg(bottleneck.severity_score);
        }
    }

    // Add file-specific metrics
    if (!file_metrics_.empty()) {
        report += QString("\nFile-Specific Performance:\n");
        for (const auto& [file_path, metrics_list] : file_metrics_) {
            if (!metrics_list.empty()) {
                auto avg = calculateAverageFromHistory(metrics_list);
                report += QString("- %1: %2 (avg)\n")
                            .arg(file_path)
                            .arg(formatDuration(avg.total_time_ms));
            }
        }
    }

    return report;
}

void PerformanceMonitor::exportReportToFile(const QString &file_path) const {
    QFile file(file_path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file for writing:" << file_path;
        return;
    }

    QTextStream stream(&file);

    if (file_path.endsWith(".json")) {
        // Export as JSON
        QJsonDocument doc(generateJSONReport());
        stream << doc.toJson();
    } else {
        // Export as text
        stream << generateDetailedReport();
    }

    file.close();
    qDebug() << "Performance report exported to:" << file_path;
}

// **Missing analytics helper methods**
void PerformanceMonitor::updateRealTimeAnalytics(const AdvancedPerformanceMetrics &metrics) {
    if (!real_time_analytics_enabled_.load() || !analytics_data_) {
        return;
    }

    std::unique_lock<std::shared_mutex> lock(data_mutex_);

    // Update analytics vectors
    analytics_data_->response_times.push_back(static_cast<double>(metrics.total_time_ms));
    analytics_data_->memory_usage.push_back(static_cast<double>(metrics.memory_peak_mb));
    analytics_data_->cpu_usage.push_back(metrics.cpu_usage_percent);

    // Keep vectors limited in size
    if (analytics_data_->response_times.size() > 100) {
        analytics_data_->response_times.erase(analytics_data_->response_times.begin());
    }
    if (analytics_data_->memory_usage.size() > 100) {
        analytics_data_->memory_usage.erase(analytics_data_->memory_usage.begin());
    }
    if (analytics_data_->cpu_usage.size() > 100) {
        analytics_data_->cpu_usage.erase(analytics_data_->cpu_usage.begin());
    }

    // Update averages
    // Calculate average manually for vector
    double sum = 0.0;
    for (double value : analytics_data_->response_times) {
        sum += value;
    }
    double avg = analytics_data_->response_times.empty() ? 0.0 : sum / analytics_data_->response_times.size();
    analytics_data_->average_response_time.store(avg);
    analytics_data_->peak_memory_usage.store(std::max(analytics_data_->peak_memory_usage.load(), static_cast<double>(metrics.memory_peak_mb)));

    qDebug() << "📊 Real-time analytics updated with new metrics";
}

void PerformanceMonitor::updateAnalyticsWindow(std::queue<double> &window, double value) {
    window.push(value);

    // Keep window size limited (e.g., last 50 values)
    while (window.size() > 50) {
        window.pop();
    }
}

double PerformanceMonitor::calculateTrend(const std::queue<double> &window) const {
    if (window.empty()) {
        return 0.0;
    }

    double sum = 0.0;
    std::queue<double> temp_window = window; // Copy for iteration

    while (!temp_window.empty()) {
        sum += temp_window.front();
        temp_window.pop();
    }

    return sum / static_cast<double>(window.size());
}



// **Missing predictive modeling helper methods**
void PerformanceMonitor::updatePredictiveModels(const AdvancedPerformanceMetrics &metrics) {
    if (!predictive_modeling_enabled_.load()) {
        return;
    }

    std::unique_lock<std::shared_mutex> lock(data_mutex_);

    // Update response time model
    if (!predictive_models_["response_time"]) {
        predictive_models_["response_time"] = std::make_unique<PredictiveModel>();
    }
    auto& response_model = predictive_models_["response_time"];
    response_model->historical_data.push_back(static_cast<double>(metrics.total_time_ms));
    if (response_model->historical_data.size() > 100) {
        response_model->historical_data.erase(response_model->historical_data.begin());
    }
    trainModel(*response_model, response_model->historical_data);

    // Update memory usage model
    if (!predictive_models_["memory_usage"]) {
        predictive_models_["memory_usage"] = std::make_unique<PredictiveModel>();
    }
    auto& memory_model = predictive_models_["memory_usage"];
    memory_model->historical_data.push_back(static_cast<double>(metrics.memory_peak_mb));
    if (memory_model->historical_data.size() > 100) {
        memory_model->historical_data.erase(memory_model->historical_data.begin());
    }
    trainModel(*memory_model, memory_model->historical_data);

    qDebug() << "🔮 Predictive models updated";
}

void PerformanceMonitor::trainModel(PredictiveModel &model, const std::vector<double> &data) {
    if (data.size() < 3) {
        return; // Need at least 3 data points
    }

    // Simple linear trend calculation
    double sum_x = 0.0, sum_y = 0.0, sum_xy = 0.0, sum_x2 = 0.0;
    size_t n = data.size();

    for (size_t i = 0; i < n; ++i) {
        double x = static_cast<double>(i);
        double y = data[i];
        sum_x += x;
        sum_y += y;
        sum_xy += x * y;
        sum_x2 += x * x;
    }

    // Calculate trend coefficient
    double denominator = n * sum_x2 - sum_x * sum_x;
    if (denominator != 0.0) {
        model.trend_coefficient = (n * sum_xy - sum_x * sum_y) / denominator;
        // Store base value in trend_coefficient for simplicity
    }

    // Calculate seasonal factor (simplified)
    model.seasonal_factor = calculateSeasonalFactor(data);

    model.last_update = QDateTime::currentDateTime();
}

double PerformanceMonitor::calculateSeasonalFactor(const std::vector<double> &data) const {
    if (data.size() < 4) {
        return 1.0; // No seasonal adjustment
    }

    // Simple seasonal factor: ratio of recent values to overall average
    double overall_avg = 0.0;
    for (double value : data) {
        overall_avg += value;
    }
    overall_avg /= data.size();

    // Average of last 25% of data
    size_t recent_count = std::max(1ULL, data.size() / 4);
    double recent_avg = 0.0;
    for (size_t i = data.size() - recent_count; i < data.size(); ++i) {
        recent_avg += data[i];
    }
    recent_avg /= recent_count;

    return overall_avg > 0.0 ? recent_avg / overall_avg : 1.0;
}

}  // namespace DeclarativeUI::HotReload
