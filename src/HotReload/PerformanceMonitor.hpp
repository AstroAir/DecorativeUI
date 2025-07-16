#pragma once

#include <QDateTime>
#include <QElapsedTimer>
#include <QObject>
#include <QString>
#include <QTimer>
#include <deque>
#include <functional>
#include <unordered_map>

namespace DeclarativeUI::HotReload {

struct PerformanceMetrics {
    qint64 reload_time_ms = 0;
    qint64 file_load_time_ms = 0;
    qint64 widget_creation_time_ms = 0;
    qint64 widget_replacement_time_ms = 0;
    qint64 total_time_ms = 0;
    int success_count = 0;
    int failure_count = 0;
    QDateTime timestamp;
};

class PerformanceMonitor : public QObject {
    Q_OBJECT

public:
    explicit PerformanceMonitor(QObject *parent = nullptr);
    ~PerformanceMonitor() override = default;

    // **Monitoring control**
    void startMonitoring();
    void stopMonitoring();
    bool isMonitoring() const { return monitoring_enabled_; }

    // **Performance tracking**
    void startOperation(const QString &operation_name);
    void endOperation(const QString &operation_name);
    void recordReloadMetrics(const QString &file_path,
                             const PerformanceMetrics &metrics);

    // **Statistics**
    PerformanceMetrics getAverageMetrics() const;
    PerformanceMetrics getMetricsForFile(const QString &file_path) const;
    QStringList getSlowFiles(int threshold_ms = 1000) const;
    double getSuccessRate() const;

    // **Configuration**
    void setMaxHistorySize(int size);
    void setWarningThreshold(int threshold_ms);
    void setPerformanceCallback(
        std::function<void(const QString &, const PerformanceMetrics &)>
            callback);

    // **Reports**
    QString generateReport() const;
    void clearHistory();

signals:
    void performanceWarning(const QString &file_path, qint64 time_ms);
    void slowOperationDetected(const QString &operation, qint64 time_ms);

private slots:
    void onPerformanceCheck();

private:
    bool monitoring_enabled_ = false;
    int max_history_size_ = 100;
    int warning_threshold_ms_ = 1000;

    std::unordered_map<QString, QElapsedTimer> active_operations_;
    std::deque<PerformanceMetrics> metrics_history_;
    std::unordered_map<QString, std::deque<PerformanceMetrics>> file_metrics_;

    std::function<void(const QString &, const PerformanceMetrics &)>
        performance_callback_;

    QTimer *performance_timer_;

    void checkPerformanceThresholds(const QString &file_path,
                                    const PerformanceMetrics &metrics);
    PerformanceMetrics calculateAverageFromHistory(
        const std::deque<PerformanceMetrics> &history) const;
    void pruneHistory();
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
