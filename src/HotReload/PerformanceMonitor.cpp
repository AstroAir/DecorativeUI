#include "PerformanceMonitor.hpp"

#include <QDebug>
#include <QStringList>
#include <algorithm>

namespace DeclarativeUI::HotReload {

PerformanceMonitor::PerformanceMonitor(QObject *parent) : QObject(parent) {
    performance_timer_ = new QTimer(this);
    performance_timer_->setInterval(5000);  // Check every 5 seconds
    connect(performance_timer_, &QTimer::timeout, this,
            &PerformanceMonitor::onPerformanceCheck);
}

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

}  // namespace DeclarativeUI::HotReload
