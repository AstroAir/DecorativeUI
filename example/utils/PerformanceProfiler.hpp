/**
 * @file PerformanceProfiler.hpp
 * @brief Performance profiling and monitoring utilities
 */

#pragma once

#include <QElapsedTimer>
#include <QJsonArray>
#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QTimer>
#include <chrono>
#include <map>
#include <memory>

/**
 * @brief Performance metric data structure
 */
struct PerformanceMetric {
    QString name;
    qint64 value;
    QString unit;
    QDateTime timestamp;
    QString category;

    PerformanceMetric(const QString& n, qint64 v, const QString& u,
                      const QString& c = "general")
        : name(n),
          value(v),
          unit(u),
          timestamp(QDateTime::currentDateTime()),
          category(c) {}
};

/**
 * @brief Performance profiler for monitoring application performance
 */
class PerformanceProfiler : public QObject {
    Q_OBJECT

public:
    explicit PerformanceProfiler(QObject* parent = nullptr);
    ~PerformanceProfiler();

    // Profiling control
    void startProfiling();
    void stopProfiling();
    void pauseProfiling();
    void resumeProfiling();
    bool isProfiling() const;

    // Metric recording
    void recordMetric(const QString& name, qint64 value,
                      const QString& unit = "ms");
    void recordMemoryUsage(const QString& context = "general");
    void recordFrameTime();
    void recordCustomMetric(const QString& name, const QVariant& value);

    // Timer utilities
    void startTimer(const QString& name);
    void endTimer(const QString& name);
    qint64 getTimerValue(const QString& name) const;

    // Data access
    QList<PerformanceMetric> getMetrics(const QString& category = "") const;
    QJsonObject getMetricsAsJson() const;
    QStringList getAvailableCategories() const;

    // Statistics
    double getAverageMetric(const QString& name) const;
    qint64 getMaxMetric(const QString& name) const;
    qint64 getMinMetric(const QString& name) const;
    int getMetricCount(const QString& name) const;

    // Reporting
    QString generateReport() const;
    bool exportToFile(const QString& filename) const;
    void clearMetrics();

    // Configuration
    void setMaxMetrics(int max_count);
    void setUpdateInterval(int interval_ms);
    void enableAutoMemoryMonitoring(bool enabled);

signals:
    void metricRecorded(const PerformanceMetric& metric);
    void profilingStarted();
    void profilingStopped();
    void reportGenerated(const QString& report);

private slots:
    void updateSystemMetrics();

private:
    void initializeSystemMonitoring();
    qint64 getCurrentMemoryUsage() const;
    double getCurrentCpuUsage() const;
    void cleanupOldMetrics();

    // State
    bool profiling_active_;
    bool profiling_paused_;
    int max_metrics_;
    int update_interval_;
    bool auto_memory_monitoring_;

    // Data storage
    QList<PerformanceMetric> metrics_;
    std::map<QString, std::chrono::steady_clock::time_point> active_timers_;
    std::map<QString, QList<qint64>> metric_history_;

    // System monitoring
    QTimer* system_timer_;
    qint64 last_cpu_time_;
    qint64 last_system_time_;

    // Frame timing
    std::chrono::steady_clock::time_point last_frame_time_;
    QList<qint64> frame_times_;
};
