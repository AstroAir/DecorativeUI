#pragma once

#include <QWidget>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QTableWidget>
#include <QTreeWidget>
#include <QLabel>
#include <QProgressBar>
// #include <QChart>  // Commented out - may not be available in this Qt installation
// #include <QChartView>
// #include <QLineSeries>
// #include <QAreaSeries>
// #include <QBarSeries>  // Commented out - may not be available in this Qt installation
#include <QTimer>
#include <QJsonObject>
#include <QJsonArray>
#include <QPushButton>
#include <QSpinBox>
#include <QCheckBox>
#include <QTextEdit>

#include <memory>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <atomic>
#include <shared_mutex>

// QT_CHARTS_USE_NAMESPACE  // Commented out due to missing Qt Charts module

namespace DeclarativeUI::Debug {

// **Performance data point**
struct PerformanceDataPoint {
    QDateTime timestamp;
    double cpu_usage = 0.0;
    size_t memory_usage_mb = 0;
    double frame_rate = 0.0;
    size_t active_animations = 0;
    size_t cache_hit_ratio = 0.0;
    size_t active_threads = 0;
    double response_time_ms = 0.0;
};

// **Memory allocation info**
struct MemoryAllocationInfo {
    void* address;
    size_t size;
    QString file;
    int line;
    QDateTime timestamp;
    QString stack_trace;
    bool is_leaked = false;
};

// **Performance bottleneck info**
struct BottleneckInfo {
    QString component;
    QString type;  // "CPU", "Memory", "I/O", "GPU"
    double severity;  // 0-100
    QString description;
    QStringList recommendations;
    QDateTime detected_at;
    bool is_resolved = false;
};

// **Real-time performance chart widget**
// Commented out due to missing Qt Charts dependency
/*
class PerformanceChart : public QChartView {
    Q_OBJECT

public:
    explicit PerformanceChart(const QString& title, QWidget* parent = nullptr);
    ~PerformanceChart() override = default;

    void addDataPoint(double value);
    void setMaxDataPoints(int max_points);
    void setYAxisRange(double min, double max);
    void setUpdateInterval(int milliseconds);
    void clear();

public slots:
    void updateChart();

private:
    QChart* chart_;
    QLineSeries* series_;
    std::vector<double> data_points_;
    int max_data_points_ = 100;
    std::unique_ptr<QTimer> update_timer_;
};
*/

// **Memory usage visualization widget**
class MemoryVisualizationWidget : public QWidget {
    Q_OBJECT

public:
    explicit MemoryVisualizationWidget(QWidget* parent = nullptr);
    ~MemoryVisualizationWidget() override = default;

    void updateMemoryData(const QJsonObject& memory_data);
    void setMemoryLimit(size_t limit_mb);

private slots:
    void onRefreshClicked();
    void onGCClicked();
    void onMemoryLeakScanClicked();

private:
    QVBoxLayout* layout_;
    QLabel* total_memory_label_;
    QLabel* used_memory_label_;
    QLabel* available_memory_label_;
    QProgressBar* memory_usage_bar_;
    QTableWidget* allocation_table_;
    QPushButton* refresh_button_;
    QPushButton* gc_button_;
    QPushButton* leak_scan_button_;
    
    size_t memory_limit_mb_ = 1024;  // 1GB default
    
    void setupUI();
    void updateAllocationTable(const QJsonArray& allocations);
};

// **Performance bottleneck detector widget**
class BottleneckDetectorWidget : public QWidget {
    Q_OBJECT

public:
    explicit BottleneckDetectorWidget(QWidget* parent = nullptr);
    ~BottleneckDetectorWidget() override = default;

    void addBottleneck(const BottleneckInfo& bottleneck);
    void updateBottlenecks(const std::vector<BottleneckInfo>& bottlenecks);
    void clearBottlenecks();

private slots:
    void onBottleneckSelected();
    void onResolveBottleneckClicked();
    void onRefreshClicked();

signals:
    void bottleneckResolutionRequested(const QString& component, const QString& type);

private:
    QVBoxLayout* layout_;
    QTreeWidget* bottleneck_tree_;
    QTextEdit* details_text_;
    QTextEdit* recommendations_text_;
    QPushButton* resolve_button_;
    QPushButton* refresh_button_;
    
    std::vector<BottleneckInfo> bottlenecks_;
    
    void setupUI();
    void updateBottleneckTree();
    QString formatBottleneckSeverity(double severity) const;
};

// **Real-time debugging console**
class DebuggingConsole : public QWidget {
    Q_OBJECT

public:
    explicit DebuggingConsole(QWidget* parent = nullptr);
    ~DebuggingConsole() override = default;

    void addLogMessage(const QString& level, const QString& component, const QString& message);
    void setLogLevel(const QString& level);
    void enableAutoScroll(bool enabled);
    void clearLog();

public slots:
    void onFilterChanged();
    void onExportLogClicked();

private:
    QVBoxLayout* layout_;
    QTextEdit* log_display_;
    QHBoxLayout* controls_layout_;
    QPushButton* clear_button_;
    QPushButton* export_button_;
    QCheckBox* auto_scroll_checkbox_;
    QSpinBox* max_lines_spinbox_;
    
    QString current_log_level_ = "INFO";
    bool auto_scroll_enabled_ = true;
    int max_log_lines_ = 1000;
    
    void setupUI();
    QString formatLogMessage(const QString& level, const QString& component, const QString& message) const;
    QColor getLogLevelColor(const QString& level) const;
};

// **Performance profiler widget**
class PerformanceProfilerWidget : public QWidget {
    Q_OBJECT

public:
    explicit PerformanceProfilerWidget(QWidget* parent = nullptr);
    ~PerformanceProfilerWidget() override = default;

    void updateProfileData(const QJsonObject& profile_data);
    void startProfiling();
    void stopProfiling();
    void resetProfiling();

private slots:
    void onStartProfilingClicked();
    void onStopProfilingClicked();
    void onResetProfilingClicked();
    void onExportProfileClicked();

signals:
    void profilingStarted();
    void profilingStopped();
    void profilingReset();

private:
    QVBoxLayout* layout_;
    QHBoxLayout* controls_layout_;
    QPushButton* start_button_;
    QPushButton* stop_button_;
    QPushButton* reset_button_;
    QPushButton* export_button_;
    
    QTabWidget* profile_tabs_;
    QTableWidget* function_table_;
    QTableWidget* memory_table_;
    QTableWidget* thread_table_;
    
    // PerformanceChart* cpu_chart_;     // Commented out due to missing Qt Charts
    // PerformanceChart* memory_chart_;  // Commented out due to missing Qt Charts  
    // PerformanceChart* frame_rate_chart_;  // Commented out due to missing Qt Charts
    
    bool is_profiling_ = false;
    
    void setupUI();
    void updateFunctionTable(const QJsonArray& functions);
    void updateMemoryTable(const QJsonArray& memory_data);
    void updateThreadTable(const QJsonArray& threads);
};

// **Main profiler dashboard**
class ProfilerDashboard : public QMainWindow {
    Q_OBJECT

public:
    explicit ProfilerDashboard(QWidget* parent = nullptr);
    ~ProfilerDashboard() override = default;

    // **Dashboard control**
    void show();
    void hide();
    void updateDashboard();
    void setUpdateInterval(int milliseconds);

    // **Data updates**
    void updatePerformanceData(const PerformanceDataPoint& data_point);
    void updateMemoryData(const QJsonObject& memory_data);
    void updateBottlenecks(const std::vector<BottleneckInfo>& bottlenecks);
    void addLogMessage(const QString& level, const QString& component, const QString& message);

    // **Configuration**
    void enableRealTimeUpdates(bool enabled);
    void setPerformanceThresholds(double cpu_threshold, size_t memory_threshold_mb, double frame_rate_threshold);

public slots:
    void onRealTimeUpdate();
    void onExportReportClicked();
    void onSettingsClicked();

signals:
    void dashboardClosed();
    void settingsChanged(const QJsonObject& settings);
    void exportRequested(const QString& format);

private:
    // **UI Components**
    QWidget* central_widget_;
    QTabWidget* main_tabs_;
    
    // **Performance monitoring tab**
    QWidget* performance_tab_;
    // PerformanceChart* cpu_chart_;          // Commented out due to missing Qt Charts
    // PerformanceChart* memory_chart_;       // Commented out due to missing Qt Charts
    // PerformanceChart* frame_rate_chart_;   // Commented out due to missing Qt Charts
    // PerformanceChart* response_time_chart_; // Commented out due to missing Qt Charts
    
    // **Memory analysis tab**
    MemoryVisualizationWidget* memory_widget_;
    
    // **Bottleneck detection tab**
    BottleneckDetectorWidget* bottleneck_widget_;
    
    // **Profiling tab**
    PerformanceProfilerWidget* profiler_widget_;
    
    // **Debug console tab**
    DebuggingConsole* debug_console_;
    
    // **Status bar and controls**
    QLabel* status_label_;
    QPushButton* export_button_;
    QPushButton* settings_button_;
    QCheckBox* real_time_checkbox_;
    
    // **Configuration**
    std::unique_ptr<QTimer> update_timer_;
    bool real_time_updates_enabled_ = true;
    int update_interval_ms_ = 1000;
    
    // **Performance thresholds**
    double cpu_threshold_ = 80.0;
    size_t memory_threshold_mb_ = 512;
    double frame_rate_threshold_ = 30.0;
    
    // **Data storage**
    std::vector<PerformanceDataPoint> performance_history_;
    std::vector<BottleneckInfo> current_bottlenecks_;
    mutable std::shared_mutex data_mutex_;
    
    void setupUI();
    void setupMenuBar();
    void setupStatusBar();
    void setupPerformanceTab();
    void setupMemoryTab();
    void setupBottleneckTab();
    void setupProfilerTab();
    void setupDebugTab();
    
    void connectSignals();
    void updateStatusBar();
    void checkPerformanceThresholds(const PerformanceDataPoint& data_point);
    
    QJsonObject generateReport() const;
    void exportReport(const QString& format, const QString& filename) const;

protected:
    void closeEvent(QCloseEvent* event) override;
};

// **Profiler integration class**
class ProfilerIntegration : public QObject {
    Q_OBJECT

public:
    static ProfilerIntegration& instance();
    explicit ProfilerIntegration(QObject* parent = nullptr);
    ~ProfilerIntegration() override = default;

    // **Dashboard management**
    void showDashboard();
    void hideDashboard();
    bool isDashboardVisible() const;

    // **Data collection**
    void startDataCollection();
    void stopDataCollection();
    void collectPerformanceData();
    void collectMemoryData();
    void collectBottleneckData();

    // **Configuration**
    void enableAutoCollection(bool enabled);
    void setCollectionInterval(int milliseconds);
    void enablePersistentLogging(bool enabled);

    // **Integration with other systems**
    void integrateWithMemoryManager();
    void integrateWithAnimationEngine();
    void integrateWithCacheManager();
    void integrateWithParallelProcessor();

signals:
    void dataCollected(const QJsonObject& data);
    void bottleneckDetected(const BottleneckInfo& bottleneck);
    void performanceAlert(const QString& metric, double value);

private slots:
    void onCollectionTimer();
    void onDashboardClosed();

private:
    std::unique_ptr<ProfilerDashboard> dashboard_;
    std::unique_ptr<QTimer> collection_timer_;
    
    bool auto_collection_enabled_ = true;
    bool persistent_logging_enabled_ = false;
    int collection_interval_ms_ = 1000;
    
    // **Integration flags**
    bool memory_manager_integrated_ = false;
    bool animation_engine_integrated_ = false;
    bool cache_manager_integrated_ = false;
    bool parallel_processor_integrated_ = false;
    
    void setupIntegrations();
    PerformanceDataPoint collectCurrentPerformanceData() const;
    QJsonObject collectCurrentMemoryData() const;
    std::vector<BottleneckInfo> collectCurrentBottlenecks() const;
};

}  // namespace DeclarativeUI::Debug
