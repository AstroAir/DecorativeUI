/**
 * @file PerformanceMonitor.cpp
 * @brief Implementation of performance monitoring widget
 */

#include "PerformanceMonitor.hpp"
#include <QDebug>
#include <QProcess>

PerformanceMonitor::PerformanceMonitor(QWidget* parent)
    : QWidget(parent)
    , main_layout_(nullptr)
    , metrics_group_(nullptr)
    , controls_group_(nullptr)
    , cpu_label_(nullptr)
    , cpu_bar_(nullptr)
    , memory_label_(nullptr)
    , memory_bar_(nullptr)
    , fps_label_(nullptr)
    , start_button_(nullptr)
    , stop_button_(nullptr)
    , clear_button_(nullptr)
    , metrics_log_(nullptr)
    , update_timer_(nullptr)
    , monitoring_active_(false)
{
    setupUI();
    createMetricsDisplay();
    createControls();
}

void PerformanceMonitor::setupUI() {
    main_layout_ = new QVBoxLayout(this);
    main_layout_->setContentsMargins(8, 8, 8, 8);
}

void PerformanceMonitor::createMetricsDisplay() {
    metrics_group_ = new QGroupBox("Performance Metrics");
    auto metrics_layout = new QVBoxLayout(metrics_group_);
    
    // CPU Usage
    auto cpu_layout = new QHBoxLayout();
    cpu_label_ = new QLabel("CPU Usage: 0%");
    cpu_bar_ = new QProgressBar();
    cpu_bar_->setRange(0, 100);
    cpu_layout->addWidget(cpu_label_);
    cpu_layout->addWidget(cpu_bar_);
    
    // Memory Usage
    auto memory_layout = new QHBoxLayout();
    memory_label_ = new QLabel("Memory Usage: 0 MB");
    memory_bar_ = new QProgressBar();
    memory_bar_->setRange(0, 1000); // Up to 1GB
    memory_layout->addWidget(memory_label_);
    memory_layout->addWidget(memory_bar_);
    
    // FPS
    fps_label_ = new QLabel("FPS: 60");
    
    metrics_layout->addLayout(cpu_layout);
    metrics_layout->addLayout(memory_layout);
    metrics_layout->addWidget(fps_label_);
    
    main_layout_->addWidget(metrics_group_);
}

void PerformanceMonitor::createControls() {
    controls_group_ = new QGroupBox("Monitoring Controls");
    auto controls_layout = new QHBoxLayout(controls_group_);
    
    start_button_ = new QPushButton("Start Monitoring");
    stop_button_ = new QPushButton("Stop Monitoring");
    clear_button_ = new QPushButton("Clear Metrics");
    
    stop_button_->setEnabled(false);
    
    controls_layout->addWidget(start_button_);
    controls_layout->addWidget(stop_button_);
    controls_layout->addWidget(clear_button_);
    controls_layout->addStretch();
    
    // Metrics log
    metrics_log_ = new QTextEdit();
    metrics_log_->setReadOnly(true);
    metrics_log_->setMaximumHeight(150);
    metrics_log_->setPlainText("Performance monitoring ready...");
    
    auto log_layout = new QVBoxLayout();
    log_layout->addWidget(new QLabel("Metrics Log:"));
    log_layout->addWidget(metrics_log_);
    
    auto main_controls_layout = new QVBoxLayout(controls_group_);
    main_controls_layout->addLayout(controls_layout);
    main_controls_layout->addLayout(log_layout);
    
    // Connect signals
    connect(start_button_, &QPushButton::clicked, this, &PerformanceMonitor::onStartMonitoring);
    connect(stop_button_, &QPushButton::clicked, this, &PerformanceMonitor::onStopMonitoring);
    connect(clear_button_, &QPushButton::clicked, this, &PerformanceMonitor::onClearMetrics);
    
    main_layout_->addWidget(controls_group_);
    
    // Add description
    auto description = new QLabel(
        "Monitor real-time performance metrics including CPU usage, memory consumption, "
        "and frame rate. This demonstrates the DeclarativeUI performance monitoring capabilities."
    );
    description->setWordWrap(true);
    description->setStyleSheet("QLabel { color: #666; font-style: italic; }");
    main_layout_->addWidget(description);
}

void PerformanceMonitor::onStartMonitoring() {
    monitoring_active_ = true;
    start_button_->setEnabled(false);
    stop_button_->setEnabled(true);
    
    // Create update timer
    update_timer_ = new QTimer(this);
    connect(update_timer_, &QTimer::timeout, this, &PerformanceMonitor::updateMetrics);
    update_timer_->start(1000); // Update every second
    
    metrics_log_->append(QString("[%1] Monitoring started")
                        .arg(QDateTime::currentDateTime().toString("hh:mm:ss")));
    
    qDebug() << "Performance monitoring started";
}

void PerformanceMonitor::onStopMonitoring() {
    monitoring_active_ = false;
    start_button_->setEnabled(true);
    stop_button_->setEnabled(false);
    
    if (update_timer_) {
        update_timer_->stop();
        update_timer_->deleteLater();
        update_timer_ = nullptr;
    }
    
    metrics_log_->append(QString("[%1] Monitoring stopped")
                        .arg(QDateTime::currentDateTime().toString("hh:mm:ss")));
    
    qDebug() << "Performance monitoring stopped";
}

void PerformanceMonitor::onClearMetrics() {
    metrics_log_->clear();
    metrics_log_->setPlainText("Metrics cleared.");
    
    // Reset displays
    cpu_bar_->setValue(0);
    memory_bar_->setValue(0);
    cpu_label_->setText("CPU Usage: 0%");
    memory_label_->setText("Memory Usage: 0 MB");
    fps_label_->setText("FPS: 60");
    
    qDebug() << "Performance metrics cleared";
}

void PerformanceMonitor::updateMetrics() {
    if (!monitoring_active_) return;
    
    // Simulate performance metrics (in a real implementation, these would be actual system metrics)
    static int counter = 0;
    counter++;
    
    // Simulate CPU usage (0-100%)
    int cpu_usage = (counter * 7) % 100;
    cpu_bar_->setValue(cpu_usage);
    cpu_label_->setText(QString("CPU Usage: %1%").arg(cpu_usage));
    
    // Simulate memory usage (0-500 MB)
    int memory_usage = (counter * 13) % 500;
    memory_bar_->setValue(memory_usage);
    memory_label_->setText(QString("Memory Usage: %1 MB").arg(memory_usage));
    
    // Simulate FPS (50-60)
    int fps = 50 + (counter % 11);
    fps_label_->setText(QString("FPS: %1").arg(fps));
    
    // Log metrics periodically
    if (counter % 5 == 0) {
        metrics_log_->append(QString("[%1] CPU: %2%, Memory: %3MB, FPS: %4")
                            .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                            .arg(cpu_usage)
                            .arg(memory_usage)
                            .arg(fps));
        
        // Auto-scroll to bottom
        auto cursor = metrics_log_->textCursor();
        cursor.movePosition(QTextCursor::End);
        metrics_log_->setTextCursor(cursor);
    }
}

void PerformanceMonitor::displayMetrics() {
    // This method would display collected metrics
    qDebug() << "Displaying performance metrics";
}

#include "PerformanceMonitor.moc"
