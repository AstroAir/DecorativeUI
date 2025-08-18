/**
 * @file PerformanceProfiler.cpp
 * @brief Implementation of performance profiler
 */

#include "PerformanceProfiler.hpp"
#include <QJsonDocument>
#include <QFile>
#include <QTextStream>
#include <QDebug>

PerformanceProfiler::PerformanceProfiler(QObject* parent)
    : QObject(parent)
    , profiling_active_(false)
    , profiling_paused_(false)
    , max_metrics_(10000)
    , update_interval_(1000)
    , auto_memory_monitoring_(true)
    , system_timer_(nullptr)
    , last_cpu_time_(0)
    , last_system_time_(0)
{
    initializeSystemMonitoring();
}

PerformanceProfiler::~PerformanceProfiler() {
    stopProfiling();
}

void PerformanceProfiler::startProfiling() {
    if (profiling_active_) return;
    
    profiling_active_ = true;
    profiling_paused_ = false;
    
    if (auto_memory_monitoring_) {
        system_timer_ = new QTimer(this);
        connect(system_timer_, &QTimer::timeout, this, &PerformanceProfiler::updateSystemMetrics);
        system_timer_->start(update_interval_);
    }
    
    recordMetric("profiling_started", QDateTime::currentMSecsSinceEpoch(), "timestamp");
    emit profilingStarted();
    
    qDebug() << "Performance profiling started";
}

void PerformanceProfiler::stopProfiling() {
    if (!profiling_active_) return;
    
    profiling_active_ = false;
    profiling_paused_ = false;
    
    if (system_timer_) {
        system_timer_->stop();
        system_timer_->deleteLater();
        system_timer_ = nullptr;
    }
    
    recordMetric("profiling_stopped", QDateTime::currentMSecsSinceEpoch(), "timestamp");
    emit profilingStopped();
    
    qDebug() << "Performance profiling stopped";
}

void PerformanceProfiler::pauseProfiling() {
    if (!profiling_active_ || profiling_paused_) return;
    
    profiling_paused_ = true;
    if (system_timer_) {
        system_timer_->stop();
    }
    
    qDebug() << "Performance profiling paused";
}

void PerformanceProfiler::resumeProfiling() {
    if (!profiling_active_ || !profiling_paused_) return;
    
    profiling_paused_ = false;
    if (system_timer_ && auto_memory_monitoring_) {
        system_timer_->start(update_interval_);
    }
    
    qDebug() << "Performance profiling resumed";
}

bool PerformanceProfiler::isProfiling() const {
    return profiling_active_ && !profiling_paused_;
}

void PerformanceProfiler::recordMetric(const QString& name, qint64 value, const QString& unit) {
    if (!profiling_active_ || profiling_paused_) return;
    
    PerformanceMetric metric(name, value, unit);
    metrics_.append(metric);
    
    // Update metric history
    metric_history_[name].append(value);
    
    // Clean up old metrics if necessary
    if (metrics_.size() > max_metrics_) {
        cleanupOldMetrics();
    }
    
    emit metricRecorded(metric);
}

void PerformanceProfiler::recordMemoryUsage(const QString& context) {
    qint64 memory_usage = getCurrentMemoryUsage();
    recordMetric(QString("memory_usage_%1").arg(context), memory_usage, "bytes");
}

void PerformanceProfiler::recordFrameTime() {
    auto now = std::chrono::steady_clock::now();
    if (last_frame_time_.time_since_epoch().count() > 0) {
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - last_frame_time_);
        recordMetric("frame_time", duration.count(), "microseconds");
        
        frame_times_.append(duration.count());
        if (frame_times_.size() > 60) { // Keep last 60 frame times
            frame_times_.removeFirst();
        }
    }
    last_frame_time_ = now;
}

void PerformanceProfiler::recordCustomMetric(const QString& name, const QVariant& value) {
    qint64 numeric_value = 0;
    
    if (value.type() == QVariant::Int || value.type() == QVariant::LongLong) {
        numeric_value = value.toLongLong();
    } else if (value.type() == QVariant::Double) {
        numeric_value = static_cast<qint64>(value.toDouble() * 1000); // Convert to integer with 3 decimal precision
    } else {
        numeric_value = value.toString().length(); // Use string length as fallback
    }
    
    recordMetric(name, numeric_value, "custom");
}

void PerformanceProfiler::startTimer(const QString& name) {
    active_timers_[name] = std::chrono::steady_clock::now();
}

void PerformanceProfiler::endTimer(const QString& name) {
    auto it = active_timers_.find(name);
    if (it != active_timers_.end()) {
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - it->second);
        recordMetric(name, duration.count(), "ms");
        active_timers_.erase(it);
    }
}

qint64 PerformanceProfiler::getTimerValue(const QString& name) const {
    auto it = active_timers_.find(name);
    if (it != active_timers_.end()) {
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - it->second);
        return duration.count();
    }
    return -1;
}

QList<PerformanceMetric> PerformanceProfiler::getMetrics(const QString& category) const {
    if (category.isEmpty()) {
        return metrics_;
    }
    
    QList<PerformanceMetric> filtered_metrics;
    for (const auto& metric : metrics_) {
        if (metric.category == category) {
            filtered_metrics.append(metric);
        }
    }
    return filtered_metrics;
}

QJsonObject PerformanceProfiler::getMetricsAsJson() const {
    QJsonObject json;
    QJsonArray metrics_array;
    
    for (const auto& metric : metrics_) {
        QJsonObject metric_obj;
        metric_obj["name"] = metric.name;
        metric_obj["value"] = metric.value;
        metric_obj["unit"] = metric.unit;
        metric_obj["timestamp"] = metric.timestamp.toString(Qt::ISODate);
        metric_obj["category"] = metric.category;
        metrics_array.append(metric_obj);
    }
    
    json["metrics"] = metrics_array;
    json["total_count"] = metrics_.size();
    json["profiling_active"] = profiling_active_;
    json["generated_at"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    return json;
}

QStringList PerformanceProfiler::getAvailableCategories() const {
    QStringList categories;
    for (const auto& metric : metrics_) {
        if (!categories.contains(metric.category)) {
            categories.append(metric.category);
        }
    }
    return categories;
}

double PerformanceProfiler::getAverageMetric(const QString& name) const {
    auto it = metric_history_.find(name);
    if (it == metric_history_.end() || it->second.isEmpty()) {
        return 0.0;
    }
    
    qint64 sum = 0;
    for (qint64 value : it->second) {
        sum += value;
    }
    return static_cast<double>(sum) / it->second.size();
}

qint64 PerformanceProfiler::getMaxMetric(const QString& name) const {
    auto it = metric_history_.find(name);
    if (it == metric_history_.end() || it->second.isEmpty()) {
        return 0;
    }
    
    return *std::max_element(it->second.begin(), it->second.end());
}

qint64 PerformanceProfiler::getMinMetric(const QString& name) const {
    auto it = metric_history_.find(name);
    if (it == metric_history_.end() || it->second.isEmpty()) {
        return 0;
    }
    
    return *std::min_element(it->second.begin(), it->second.end());
}

int PerformanceProfiler::getMetricCount(const QString& name) const {
    auto it = metric_history_.find(name);
    if (it == metric_history_.end()) {
        return 0;
    }
    return it->second.size();
}

QString PerformanceProfiler::generateReport() const {
    QString report;
    QTextStream stream(&report);
    
    stream << "Performance Profiling Report\n";
    stream << "============================\n\n";
    stream << "Generated: " << QDateTime::currentDateTime().toString() << "\n";
    stream << "Total Metrics: " << metrics_.size() << "\n";
    stream << "Profiling Active: " << (profiling_active_ ? "Yes" : "No") << "\n\n";
    
    // Summary by category
    QStringList categories = getAvailableCategories();
    for (const QString& category : categories) {
        auto category_metrics = getMetrics(category);
        stream << "Category: " << category << " (" << category_metrics.size() << " metrics)\n";
        
        // Show recent metrics for this category
        int shown = 0;
        for (auto it = category_metrics.rbegin(); it != category_metrics.rend() && shown < 5; ++it, ++shown) {
            stream << "  " << it->name << ": " << it->value << " " << it->unit 
                   << " (" << it->timestamp.toString("hh:mm:ss") << ")\n";
        }
        stream << "\n";
    }
    
    emit reportGenerated(report);
    return report;
}

bool PerformanceProfiler::exportToFile(const QString& filename) const {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Cannot open file for writing:" << filename;
        return false;
    }
    
    if (filename.endsWith(".json")) {
        QJsonDocument doc(getMetricsAsJson());
        file.write(doc.toJson());
    } else {
        QTextStream stream(&file);
        stream << generateReport();
    }
    
    return true;
}

void PerformanceProfiler::clearMetrics() {
    metrics_.clear();
    metric_history_.clear();
    frame_times_.clear();
    qDebug() << "Performance metrics cleared";
}

void PerformanceProfiler::setMaxMetrics(int max_count) {
    max_metrics_ = max_count;
    if (metrics_.size() > max_metrics_) {
        cleanupOldMetrics();
    }
}

void PerformanceProfiler::setUpdateInterval(int interval_ms) {
    update_interval_ = interval_ms;
    if (system_timer_ && system_timer_->isActive()) {
        system_timer_->setInterval(interval_ms);
    }
}

void PerformanceProfiler::enableAutoMemoryMonitoring(bool enabled) {
    auto_memory_monitoring_ = enabled;
    
    if (profiling_active_) {
        if (enabled && !system_timer_) {
            system_timer_ = new QTimer(this);
            connect(system_timer_, &QTimer::timeout, this, &PerformanceProfiler::updateSystemMetrics);
            system_timer_->start(update_interval_);
        } else if (!enabled && system_timer_) {
            system_timer_->stop();
            system_timer_->deleteLater();
            system_timer_ = nullptr;
        }
    }
}

void PerformanceProfiler::updateSystemMetrics() {
    if (!profiling_active_ || profiling_paused_) return;
    
    // Record memory usage
    recordMemoryUsage("system");
    
    // Record CPU usage (simplified)
    double cpu_usage = getCurrentCpuUsage();
    recordMetric("cpu_usage", static_cast<qint64>(cpu_usage * 100), "percent");
    
    // Record frame rate if we have frame time data
    if (!frame_times_.isEmpty()) {
        double avg_frame_time = 0;
        for (qint64 time : frame_times_) {
            avg_frame_time += time;
        }
        avg_frame_time /= frame_times_.size();
        
        // Convert microseconds to FPS
        double fps = 1000000.0 / avg_frame_time;
        recordMetric("fps", static_cast<qint64>(fps), "fps");
    }
}

void PerformanceProfiler::initializeSystemMonitoring() {
    last_cpu_time_ = 0;
    last_system_time_ = 0;
}

qint64 PerformanceProfiler::getCurrentMemoryUsage() const {
    // Simplified memory usage calculation
    // In a real implementation, this would query actual system memory usage
    return QRandomGenerator::global()->bounded(50, 500) * 1024 * 1024; // 50-500 MB
}

double PerformanceProfiler::getCurrentCpuUsage() const {
    // Simplified CPU usage calculation
    // In a real implementation, this would query actual CPU usage
    return QRandomGenerator::global()->bounded(0, 100);
}

void PerformanceProfiler::cleanupOldMetrics() {
    if (metrics_.size() <= max_metrics_) return;
    
    int to_remove = metrics_.size() - max_metrics_;
    metrics_.erase(metrics_.begin(), metrics_.begin() + to_remove);
    
    // Also clean up metric history
    for (auto& pair : metric_history_) {
        auto& history = pair.second;
        if (history.size() > max_metrics_ / 10) { // Keep 10% of max for history
            int history_to_remove = history.size() - (max_metrics_ / 10);
            history.erase(history.begin(), history.begin() + history_to_remove);
        }
    }
}

#include "PerformanceProfiler.moc"
