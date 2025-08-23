#include "ProfilerDashboard.hpp"

#include <QApplication>
#include <QComboBox>
#include <QDateTime>
#include <QDebug>
#include <QFileDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QJsonDocument>
#include <QListWidget>
#include <QMenuBar>
#include <QMessageBox>
#include <QProgressBar>
#include <QSplitter>
#include <QStatusBar>
#include <mutex>
#include <shared_mutex>

namespace DeclarativeUI::Debug {

// **PerformanceChart implementation**
// Commented out due to missing Qt Charts dependency
/*
PerformanceChart::PerformanceChart(const QString& title, QWidget* parent)
    : QChartView(parent) {

    chart_ = new QChart();
    chart_->setTitle(title);
    chart_->setAnimationOptions(QChart::SeriesAnimations);

    series_ = new QLineSeries();
    chart_->addSeries(series_);
    chart_->createDefaultAxes();

    setChart(chart_);
    setRenderHint(QPainter::Antialiasing);

    // Setup update timer
    update_timer_ = std::make_unique<QTimer>(this);
    connect(update_timer_.get(), &QTimer::timeout, this,
&PerformanceChart::updateChart); update_timer_->start(1000);  // Update every
second
}

void PerformanceChart::addDataPoint(double value) {
    data_points_.push_back(value);

    // Keep only the last max_data_points_
    if (static_cast<int>(data_points_.size()) > max_data_points_) {
        data_points_.erase(data_points_.begin());
    }
}

void PerformanceChart::setMaxDataPoints(int max_points) {
    max_data_points_ = max_points;
}

void PerformanceChart::setYAxisRange(double min, double max) {
    auto axes = chart_->axes(Qt::Vertical);
    if (!axes.isEmpty()) {
        axes.first()->setRange(min, max);
    }
}

void PerformanceChart::setUpdateInterval(int milliseconds) {
    update_timer_->setInterval(milliseconds);
}

void PerformanceChart::clear() {
    data_points_.clear();
    series_->clear();
}

void PerformanceChart::updateChart() {
    series_->clear();

    for (size_t i = 0; i < data_points_.size(); ++i) {
        series_->append(static_cast<qreal>(i), data_points_[i]);
    }
}
*/

// **MemoryVisualizationWidget implementation**
MemoryVisualizationWidget::MemoryVisualizationWidget(QWidget* parent)
    : QWidget(parent) {
    setupUI();
}

void MemoryVisualizationWidget::setupUI() {
    layout_ = new QVBoxLayout(this);

    // Memory overview
    auto overview_group = new QGroupBox("Memory Overview", this);
    auto overview_layout = new QGridLayout(overview_group);

    total_memory_label_ = new QLabel("Total: 0 MB", this);
    used_memory_label_ = new QLabel("Used: 0 MB", this);
    available_memory_label_ = new QLabel("Available: 0 MB", this);
    memory_usage_bar_ = new QProgressBar(this);

    overview_layout->addWidget(new QLabel("Total Memory:"), 0, 0);
    overview_layout->addWidget(total_memory_label_, 0, 1);
    overview_layout->addWidget(new QLabel("Used Memory:"), 1, 0);
    overview_layout->addWidget(used_memory_label_, 1, 1);
    overview_layout->addWidget(new QLabel("Available Memory:"), 2, 0);
    overview_layout->addWidget(available_memory_label_, 2, 1);
    overview_layout->addWidget(memory_usage_bar_, 3, 0, 1, 2);

    layout_->addWidget(overview_group);

    // Controls
    auto controls_layout = new QHBoxLayout();
    refresh_button_ = new QPushButton("Refresh", this);
    gc_button_ = new QPushButton("Force GC", this);
    leak_scan_button_ = new QPushButton("Scan Leaks", this);

    controls_layout->addWidget(refresh_button_);
    controls_layout->addWidget(gc_button_);
    controls_layout->addWidget(leak_scan_button_);
    controls_layout->addStretch();

    layout_->addLayout(controls_layout);

    // Allocation table
    allocation_table_ = new QTableWidget(this);
    allocation_table_->setColumnCount(5);
    allocation_table_->setHorizontalHeaderLabels(
        {"Address", "Size", "File", "Line", "Timestamp"});
    allocation_table_->horizontalHeader()->setStretchLastSection(true);

    layout_->addWidget(allocation_table_);

    // Connect signals
    connect(refresh_button_, &QPushButton::clicked, this,
            &MemoryVisualizationWidget::onRefreshClicked);
    connect(gc_button_, &QPushButton::clicked, this,
            &MemoryVisualizationWidget::onGCClicked);
    connect(leak_scan_button_, &QPushButton::clicked, this,
            &MemoryVisualizationWidget::onMemoryLeakScanClicked);
}

void MemoryVisualizationWidget::updateMemoryData(
    const QJsonObject& memory_data) {
    qint64 total_memory = memory_data["total_allocated_bytes"].toInt();
    qint64 used_memory = memory_data["current_allocated_bytes"].toInt();
    qint64 available_memory = memory_limit_mb_ * 1024 * 1024 - used_memory;

    total_memory_label_->setText(
        QString("Total: %1 MB").arg(total_memory / (1024 * 1024)));
    used_memory_label_->setText(
        QString("Used: %1 MB").arg(used_memory / (1024 * 1024)));
    available_memory_label_->setText(
        QString("Available: %1 MB").arg(available_memory / (1024 * 1024)));

    int usage_percentage = static_cast<int>(
        (static_cast<double>(used_memory) / (memory_limit_mb_ * 1024 * 1024)) *
        100);
    memory_usage_bar_->setValue(usage_percentage);

    // Update allocation table if data is available
    if (memory_data.contains("allocations")) {
        updateAllocationTable(memory_data["allocations"].toArray());
    }
}

void MemoryVisualizationWidget::setMemoryLimit(size_t limit_mb) {
    memory_limit_mb_ = limit_mb;
}

void MemoryVisualizationWidget::onRefreshClicked() {
    // Emit signal to request memory data refresh
    qDebug() << "🔥 Memory data refresh requested";
}

void MemoryVisualizationWidget::onGCClicked() {
    // Emit signal to trigger garbage collection
    qDebug() << "🔥 Garbage collection requested";
}

void MemoryVisualizationWidget::onMemoryLeakScanClicked() {
    // Emit signal to trigger memory leak scan
    qDebug() << "🔥 Memory leak scan requested";
}

void MemoryVisualizationWidget::updateAllocationTable(
    const QJsonArray& allocations) {
    allocation_table_->setRowCount(allocations.size());

    for (int i = 0; i < allocations.size(); ++i) {
        QJsonObject allocation = allocations[i].toObject();

        allocation_table_->setItem(
            i, 0, new QTableWidgetItem(allocation["address"].toString()));
        allocation_table_->setItem(
            i, 1,
            new QTableWidgetItem(QString::number(allocation["size"].toInt())));
        allocation_table_->setItem(
            i, 2, new QTableWidgetItem(allocation["file"].toString()));
        allocation_table_->setItem(
            i, 3,
            new QTableWidgetItem(QString::number(allocation["line"].toInt())));
        allocation_table_->setItem(
            i, 4, new QTableWidgetItem(allocation["timestamp"].toString()));
    }
}

// **ProfilerDashboard implementation**
ProfilerDashboard::ProfilerDashboard(QWidget* parent) : QMainWindow(parent) {
    setupUI();
    setupMenuBar();
    setupStatusBar();
    connectSignals();

    // Setup update timer
    update_timer_ = std::make_unique<QTimer>(this);
    connect(update_timer_.get(), &QTimer::timeout, this,
            &ProfilerDashboard::onRealTimeUpdate);

    if (real_time_updates_enabled_) {
        update_timer_->start(update_interval_ms_);
    }

    qDebug() << "🔥 Profiler Dashboard initialized";
}

void ProfilerDashboard::setupUI() {
    central_widget_ = new QWidget(this);
    setCentralWidget(central_widget_);

    auto main_layout = new QVBoxLayout(central_widget_);

    // Main tabs
    main_tabs_ = new QTabWidget(this);
    main_layout->addWidget(main_tabs_);

    setupPerformanceTab();
    setupMemoryTab();
    setupBottleneckTab();
    setupProfilerTab();
    setupDebugTab();

    // Controls
    auto controls_layout = new QHBoxLayout();

    real_time_checkbox_ = new QCheckBox("Real-time Updates", this);
    real_time_checkbox_->setChecked(real_time_updates_enabled_);

    export_button_ = new QPushButton("Export Report", this);
    settings_button_ = new QPushButton("Settings", this);

    controls_layout->addWidget(real_time_checkbox_);
    controls_layout->addStretch();
    controls_layout->addWidget(export_button_);
    controls_layout->addWidget(settings_button_);

    main_layout->addLayout(controls_layout);

    setWindowTitle("DeclarativeUI Profiler Dashboard");
    resize(1200, 800);
}

void ProfilerDashboard::setupPerformanceTab() {
    performance_tab_ = new QWidget();
    [[maybe_unused]] auto layout = new QGridLayout(performance_tab_);

    // Create performance charts - commented out due to missing Qt Charts
    // dependency
    /*
    cpu_chart_ = new PerformanceChart("CPU Usage (%)", this);
    memory_chart_ = new PerformanceChart("Memory Usage (MB)", this);
    frame_rate_chart_ = new PerformanceChart("Frame Rate (FPS)", this);
    response_time_chart_ = new PerformanceChart("Response Time (ms)", this);

    cpu_chart_->setYAxisRange(0, 100);
    frame_rate_chart_->setYAxisRange(0, 120);

    layout->addWidget(cpu_chart_, 0, 0);
    layout->addWidget(memory_chart_, 0, 1);
    layout->addWidget(frame_rate_chart_, 1, 0);
    layout->addWidget(response_time_chart_, 1, 1);
    */

    main_tabs_->addTab(performance_tab_, "Performance");
}

void ProfilerDashboard::setupMemoryTab() {
    memory_widget_ = new MemoryVisualizationWidget(this);
    main_tabs_->addTab(memory_widget_, "Memory");
}

void ProfilerDashboard::setupBottleneckTab() {
    // Commented out due to missing BottleneckDetectorWidget implementation
    /*
    bottleneck_widget_ = new BottleneckDetectorWidget(this);
    main_tabs_->addTab(bottleneck_widget_, "Bottlenecks");
    */

    // Create a placeholder tab for now
    auto* placeholder = new QWidget(this);
    auto* layout = new QVBoxLayout(placeholder);
    auto* label = new QLabel("Bottleneck Detection (Coming Soon)", placeholder);
    layout->addWidget(label);
    main_tabs_->addTab(placeholder, "Bottlenecks");
}

void ProfilerDashboard::setupProfilerTab() {
    // Commented out due to missing PerformanceProfilerWidget implementation
    /*
    profiler_widget_ = new PerformanceProfilerWidget(this);
    main_tabs_->addTab(profiler_widget_, "Profiler");
    */

    // Create a placeholder tab for now
    auto* placeholder = new QWidget(this);
    auto* layout = new QVBoxLayout(placeholder);
    auto* label = new QLabel("Performance Profiler (Coming Soon)", placeholder);
    layout->addWidget(label);
    main_tabs_->addTab(placeholder, "Profiler");
}

void ProfilerDashboard::setupDebugTab() {
    debug_console_ = new DebuggingConsole(this);
    main_tabs_->addTab(debug_console_, "Debug Console");
}

void ProfilerDashboard::setupMenuBar() {
    auto file_menu = menuBar()->addMenu("&File");

    auto export_action = file_menu->addAction("&Export Report...");
    connect(export_action, &QAction::triggered, this,
            &ProfilerDashboard::onExportReportClicked);

    file_menu->addSeparator();

    auto exit_action = file_menu->addAction("E&xit");
    connect(exit_action, &QAction::triggered, this, &QWidget::close);

    auto view_menu = menuBar()->addMenu("&View");

    auto refresh_action = view_menu->addAction("&Refresh");
    connect(refresh_action, &QAction::triggered, this,
            &ProfilerDashboard::updateDashboard);

    auto settings_menu = menuBar()->addMenu("&Settings");

    auto preferences_action = settings_menu->addAction("&Preferences...");
    connect(preferences_action, &QAction::triggered, this,
            &ProfilerDashboard::onSettingsClicked);
}

void ProfilerDashboard::setupStatusBar() {
    status_label_ = new QLabel("Ready", this);
    statusBar()->addWidget(status_label_);

    auto update_label = new QLabel(
        QString("Update Interval: %1ms").arg(update_interval_ms_), this);
    statusBar()->addPermanentWidget(update_label);
}

void ProfilerDashboard::connectSignals() {
    connect(real_time_checkbox_, &QCheckBox::toggled, this,
            &ProfilerDashboard::enableRealTimeUpdates);
    connect(export_button_, &QPushButton::clicked, this,
            &ProfilerDashboard::onExportReportClicked);
    connect(settings_button_, &QPushButton::clicked, this,
            &ProfilerDashboard::onSettingsClicked);
}

void ProfilerDashboard::updatePerformanceData(
    const PerformanceDataPoint& data_point) {
    std::unique_lock<std::shared_mutex> lock(data_mutex_);

    performance_history_.push_back(data_point);

    // Keep only recent data
    if (performance_history_.size() > 1000) {
        performance_history_.erase(performance_history_.begin());
    }

    // Update charts - commented out due to missing Qt Charts dependency
    /*
    cpu_chart_->addDataPoint(data_point.cpu_usage);
    memory_chart_->addDataPoint(static_cast<double>(data_point.memory_usage_mb));
    frame_rate_chart_->addDataPoint(data_point.frame_rate);
    response_time_chart_->addDataPoint(data_point.response_time_ms);
    */

    // Check thresholds
    checkPerformanceThresholds(data_point);
}

void ProfilerDashboard::updateMemoryData(const QJsonObject& memory_data) {
    if (memory_widget_) {
        memory_widget_->updateMemoryData(memory_data);
    }
}

void ProfilerDashboard::updateBottlenecks(
    const std::vector<BottleneckInfo>& bottlenecks) {
    std::unique_lock<std::shared_mutex> lock(data_mutex_);
    current_bottlenecks_ = bottlenecks;

    // Commented out due to missing BottleneckDetectorWidget implementation
    /*
    if (bottleneck_widget_) {
        bottleneck_widget_->updateBottlenecks(bottlenecks);
    }
    */
}

void ProfilerDashboard::addLogMessage(const QString& level,
                                      const QString& component,
                                      const QString& message) {
    if (debug_console_) {
        debug_console_->addLogMessage(level, component, message);
    }
}

void ProfilerDashboard::enableRealTimeUpdates(bool enabled) {
    real_time_updates_enabled_ = enabled;

    if (enabled) {
        update_timer_->start(update_interval_ms_);
    } else {
        update_timer_->stop();
    }

    updateStatusBar();
}

void ProfilerDashboard::setUpdateInterval(int milliseconds) {
    update_interval_ms_ = milliseconds;

    if (real_time_updates_enabled_) {
        update_timer_->setInterval(milliseconds);
    }
}

void ProfilerDashboard::updateDashboard() {
    // Trigger a manual update of all dashboard components
    qDebug() << "🔥 Dashboard manual update triggered";
    updateStatusBar();
}

void ProfilerDashboard::onRealTimeUpdate() {
    // This would typically collect fresh data from the profiler integration
    updateStatusBar();
}

void ProfilerDashboard::onExportReportClicked() {
    QString filename = QFileDialog::getSaveFileName(
        this, "Export Performance Report",
        QString("performance_report_%1.json")
            .arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss")),
        "JSON Files (*.json)");

    if (!filename.isEmpty()) {
        exportReport("json", filename);
    }
}

void ProfilerDashboard::onSettingsClicked() {
    // Open settings dialog
    qDebug() << "🔥 Settings dialog requested";
}

void ProfilerDashboard::updateStatusBar() {
    QString status = real_time_updates_enabled_ ? "Real-time monitoring active"
                                                : "Manual mode";
    status_label_->setText(status);
}

void ProfilerDashboard::checkPerformanceThresholds(
    const PerformanceDataPoint& data_point) {
    if (data_point.cpu_usage > cpu_threshold_) {
        qWarning() << "🔥 CPU usage threshold exceeded:" << data_point.cpu_usage
                   << "%";
    }

    if (data_point.memory_usage_mb > memory_threshold_mb_) {
        qWarning() << "🔥 Memory usage threshold exceeded:"
                   << data_point.memory_usage_mb << "MB";
    }

    if (data_point.frame_rate < frame_rate_threshold_) {
        qWarning() << "🔥 Frame rate below threshold:" << data_point.frame_rate
                   << "FPS";
    }
}

QJsonObject ProfilerDashboard::generateReport() const {
    std::shared_lock<std::shared_mutex> lock(data_mutex_);

    QJsonObject report;
    report["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    report["dashboard_version"] = "1.0";

    // Performance summary
    QJsonObject performance_summary;
    if (!performance_history_.empty()) {
        const auto& latest = performance_history_.back();
        performance_summary["cpu_usage"] = latest.cpu_usage;
        performance_summary["memory_usage_mb"] =
            static_cast<qint64>(latest.memory_usage_mb);
        performance_summary["frame_rate"] = latest.frame_rate;
        performance_summary["response_time_ms"] = latest.response_time_ms;
    }
    report["performance_summary"] = performance_summary;

    // Bottlenecks
    QJsonArray bottlenecks_array;
    for (const auto& bottleneck : current_bottlenecks_) {
        QJsonObject bottleneck_obj;
        bottleneck_obj["component"] = bottleneck.component;
        bottleneck_obj["type"] = bottleneck.type;
        bottleneck_obj["severity"] = bottleneck.severity;
        bottleneck_obj["description"] = bottleneck.description;
        bottlenecks_array.append(bottleneck_obj);
    }
    report["bottlenecks"] = bottlenecks_array;

    return report;
}

void ProfilerDashboard::exportReport(const QString& format,
                                     const QString& filename) const {
    QJsonObject report = generateReport();

    if (format.toLower() == "json") {
        QJsonDocument doc(report);

        QFile file(filename);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(doc.toJson());
            file.close();

            QMessageBox::information(
                const_cast<ProfilerDashboard*>(this), "Export Successful",
                QString("Report exported to: %1").arg(filename));
        } else {
            QMessageBox::warning(const_cast<ProfilerDashboard*>(this),
                                 "Export Failed",
                                 "Failed to write report file.");
        }
    }
}

void ProfilerDashboard::closeEvent(QCloseEvent* event) {
    emit dashboardClosed();
    QMainWindow::closeEvent(event);
}

// **ProfilerIntegration implementation**
ProfilerIntegration& ProfilerIntegration::instance() {
    static ProfilerIntegration instance;
    return instance;
}

ProfilerIntegration::ProfilerIntegration(QObject* parent) : QObject(parent) {
    dashboard_ = std::make_unique<ProfilerDashboard>();

    collection_timer_ = std::make_unique<QTimer>(this);
    connect(collection_timer_.get(), &QTimer::timeout, this,
            &ProfilerIntegration::onCollectionTimer);

    connect(dashboard_.get(), &ProfilerDashboard::dashboardClosed, this,
            &ProfilerIntegration::onDashboardClosed);

    if (auto_collection_enabled_) {
        collection_timer_->start(collection_interval_ms_);
    }

    qDebug() << "🔥 Profiler Integration initialized";
}

void ProfilerIntegration::showDashboard() {
    if (dashboard_) {
        dashboard_->show();
        dashboard_->raise();
        dashboard_->activateWindow();
    }
}

void ProfilerIntegration::hideDashboard() {
    if (dashboard_) {
        dashboard_->hide();
    }
}

bool ProfilerIntegration::isDashboardVisible() const {
    return dashboard_ && dashboard_->isVisible();
}

void ProfilerIntegration::startDataCollection() {
    if (!collection_timer_->isActive()) {
        collection_timer_->start(collection_interval_ms_);
        qDebug() << "🔥 Data collection started";
    }
}

void ProfilerIntegration::stopDataCollection() {
    if (collection_timer_->isActive()) {
        collection_timer_->stop();
        qDebug() << "🔥 Data collection stopped";
    }
}

void ProfilerIntegration::onCollectionTimer() {
    collectPerformanceData();
    collectMemoryData();
    collectBottleneckData();
}

void ProfilerIntegration::onDashboardClosed() {
    qDebug() << "🔥 Dashboard closed";
}

PerformanceDataPoint ProfilerIntegration::collectCurrentPerformanceData()
    const {
    PerformanceDataPoint data_point;
    data_point.timestamp = QDateTime::currentDateTime();

    // In a real implementation, these would collect actual system metrics
    data_point.cpu_usage = 45.0;         // Placeholder
    data_point.memory_usage_mb = 256;    // Placeholder
    data_point.frame_rate = 60.0;        // Placeholder
    data_point.active_animations = 5;    // Placeholder
    data_point.cache_hit_ratio = 85.0;   // Placeholder
    data_point.active_threads = 8;       // Placeholder
    data_point.response_time_ms = 12.5;  // Placeholder

    return data_point;
}

void ProfilerIntegration::collectPerformanceData() {
    auto data_point = collectCurrentPerformanceData();

    if (dashboard_) {
        dashboard_->updatePerformanceData(data_point);
    }

    QJsonObject data;
    data["cpu_usage"] = data_point.cpu_usage;
    data["memory_usage_mb"] = static_cast<qint64>(data_point.memory_usage_mb);
    data["frame_rate"] = data_point.frame_rate;

    emit dataCollected(data);
}

void ProfilerIntegration::collectMemoryData() {
    QJsonObject memory_data = collectCurrentMemoryData();

    if (dashboard_) {
        dashboard_->updateMemoryData(memory_data);
    }
}

void ProfilerIntegration::collectBottleneckData() {
    auto bottlenecks = collectCurrentBottlenecks();

    if (dashboard_) {
        dashboard_->updateBottlenecks(bottlenecks);
    }

    for (const auto& bottleneck : bottlenecks) {
        emit bottleneckDetected(bottleneck);
    }
}

QJsonObject ProfilerIntegration::collectCurrentMemoryData() const {
    QJsonObject memory_data;

    // Placeholder data - in real implementation, this would collect from
    // MemoryManager
    memory_data["total_allocated_bytes"] = 268435456;    // 256MB
    memory_data["current_allocated_bytes"] = 134217728;  // 128MB
    memory_data["peak_allocated_bytes"] = 201326592;     // 192MB
    memory_data["allocation_count"] = 1024;
    memory_data["gc_count"] = 5;

    return memory_data;
}

std::vector<BottleneckInfo> ProfilerIntegration::collectCurrentBottlenecks()
    const {
    std::vector<BottleneckInfo> bottlenecks;

    // Placeholder bottleneck detection
    BottleneckInfo cpu_bottleneck;
    cpu_bottleneck.component = "Animation Engine";
    cpu_bottleneck.type = "CPU";
    cpu_bottleneck.severity = 65.0;
    cpu_bottleneck.description =
        "High CPU usage detected in animation processing";
    cpu_bottleneck.recommendations = {"Reduce animation complexity",
                                      "Enable GPU acceleration"};
    cpu_bottleneck.detected_at = QDateTime::currentDateTime();

    bottlenecks.push_back(cpu_bottleneck);

    return bottlenecks;
}

// **Widget implementations are now in separate files**
// BottleneckDetectorWidget - see BottleneckDetectorWidget.cpp
// DebuggingConsole - see DebuggingConsole.cpp
// PerformanceProfilerWidget - see PerformanceProfilerWidget.cpp

// **ProfilerDashboard show/hide implementations**
void ProfilerDashboard::show() {
    QMainWindow::show();
    qDebug() << "ProfilerDashboard shown";
}

void ProfilerDashboard::hide() {
    QMainWindow::hide();
    qDebug() << "ProfilerDashboard hidden";
}

}  // namespace DeclarativeUI::Debug
