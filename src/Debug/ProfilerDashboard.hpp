#pragma once

#include <QCheckBox>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QMainWindow>
#include <QProgressBar>
#include <QPushButton>
#include <QSpinBox>
#include <QTabWidget>
#include <QTableWidget>
#include <QTextEdit>
#include <QTimer>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QWidget>

#include <memory>
#include <shared_mutex>
#include <vector>

namespace DeclarativeUI::Debug {

/**
 * @file ProfilerDashboard.hpp
 * @brief Developer-facing diagnostics UI and integration helpers for runtime
 * profiling and performance analysis.
 *
 * This header defines lightweight data structures and multiple QWidget-based
 * components used to present, record and interact with runtime performance
 * telemetry. The components are intentionally frontend-only — heavy collection,
 * sampling and analysis logic is expected to live in the application's backend
 * (e.g. PerformanceMonitor). The file also provides a simple
 * ProfilerIntegration class that facilitates connecting the dashboard UI to
 * data collection.
 *
 * Main responsibilities:
 *  - Define compact container types for single-sample performance and memory
 * data.
 *  - Provide widgets to visualize memory usage, detected bottlenecks, live
 * logs, and control profiling sessions.
 *  - Expose a ProfilerDashboard QMainWindow that aggregates the widgets into a
 *    coherent developer tool.
 *  - Offer integration glue (ProfilerIntegration) to start/stop collection and
 *    push data into the dashboard.
 *
 * Threading and ownership:
 *  - All QWidget-derived classes must be used only on the GUI (main) thread.
 *  - ProfilerIntegration may run timers and collect data on worker threads but
 *    must marshal updates to UI elements via signals/slots or
 * QMetaObject::invokeMethod with queued connections.
 *
 * Documentation notes:
 *  - Each widget exposes public methods intended for hosts to call when new
 * data is available. The widgets generally do not retain heavyweight state and
 * rely on host code to provide authoritative, validated JSON or typed data.
 */

/**
 * @struct PerformanceDataPoint
 * @brief Single timestamped measurement representing a variety of runtime
 * metrics.
 *
 * This struct is a compact snapshot of common telemetry collected periodically:
 *  - timestamp: wall-clock capture time.
 *  - cpu_usage: approximate percentage CPU used by the process or subsystem.
 *  - memory_usage_mb: resident memory used (megabytes).
 *  - frame_rate: observed frames per second for UI rendering.
 *  - active_animations: number of animations currently active.
 *  - cache_hit_ratio: percentage or scaled metric representing cache
 * effectiveness.
 *  - active_threads: number of worker threads active at capture time.
 *  - response_time_ms: measured latency for a representative operation
 * (milliseconds).
 *
 * Use the vector<PerformanceDataPoint> history container in the dashboard for
 * time-series charts and threshold checks.
 */
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

/**
 * @struct MemoryAllocationInfo
 * @brief Describes a single memory allocation sample for leak investigation.
 *
 * Fields:
 *  - address: pointer address returned by the allocator.
 *  - size: allocation size in bytes.
 *  - file: source file (if available) where allocation occurred.
 *  - line: source line number associated with allocation (if available).
 *  - timestamp: when the allocation was observed.
 *  - stack_trace: optional stack trace captured at allocation time.
 *  - is_leaked: boolean best-effort flag set after leak analysis.
 *
 * MemoryAllocationInfo instances are intended to populate the allocation table
 * in the MemoryVisualizationWidget and assist developers with root-cause
 * analysis.
 */
struct MemoryAllocationInfo {
    void* address;
    size_t size;
    QString file;
    int line;
    QDateTime timestamp;
    QString stack_trace;
    bool is_leaked = false;
};

/**
 * @struct BottleneckInfo
 * @brief Structured description of a detected performance bottleneck.
 *
 * Fields:
 *  - component: logical name of the subsystem (e.g. "Renderer", "Parser").
 *  - type: classification such as "CPU", "Memory", "I/O", "GPU".
 *  - severity: numeric severity score (0-100) where larger values indicate more
 * severe issues.
 *  - description: human-readable explanation of the issue and observed
 * symptoms.
 *  - recommendations: list of suggested mitigations or next steps.
 *  - detected_at: timestamp when the bottleneck was discovered.
 *  - is_resolved: flag that can be set by host code after mitigation.
 *
 * BottleneckInfo is used by the BottleneckDetectorWidget and ProfilerDashboard
 * to present diagnostic findings and suggested remediations.
 */
struct BottleneckInfo {
    QString component;
    QString type;     // "CPU", "Memory", "I/O", "GPU"
    double severity;  // 0-100
    QString description;
    QStringList recommendations;
    QDateTime detected_at;
    bool is_resolved = false;
};

/**
 * @class MemoryVisualizationWidget
 * @brief UI helper that presents memory usage summary and allocation details.
 *
 * Responsibilities:
 *  - Display total/used/available memory indicators and a usage progress bar.
 *  - Present a table of recent allocations (MemoryAllocationInfo) for
 * inspection.
 *  - Provide controls to refresh data, request garbage collection (if
 * supported) and run a basic leak scan.
 *
 * Public methods:
 *  - updateMemoryData(): accept a JSON object from the host describing memory
 *    snapshots and allocations; the widget translates the JSON into UI content.
 *  - setMemoryLimit(): configure the maximum memory displayed by the progress
 * bar.
 *
 * UI interactions:
 *  - onRefreshClicked(): host should be wired to re-collect and call
 * updateMemoryData().
 *  - onGCClicked(): advisory request to perform garbage collection on supported
 * runtimes.
 *  - onMemoryLeakScanClicked(): advisory request to run a deeper leak analysis;
 * expensive work must be performed off the UI thread.
 */
class MemoryVisualizationWidget : public QWidget {
    Q_OBJECT

public:
    explicit MemoryVisualizationWidget(QWidget* parent = nullptr);
    ~MemoryVisualizationWidget() override = default;

    /**
     * @brief Update the widget from a platform-agnostic JSON memory payload.
     * @param memory_data JSON object containing summary fields and an
     * allocations array.
     *
     * The expected schema is flexible; the host and widget must agree on keys
     * (e.g. total_mb, used_mb, available_mb, allocations[]).
     */
    void updateMemoryData(const QJsonObject& memory_data);

    /**
     * @brief Configure the upper bound used by the memory usage visualization.
     * @param limit_mb Memory upper limit in megabytes used to scale the
     * progress bar.
     */
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

/**
 * @class BottleneckDetectorWidget
 * @brief Widget that lists detected bottlenecks and exposes resolution actions.
 *
 * This widget provides a compact tree + details UI:
 *  - bottleneck_tree_: hierarchical list of detected issues grouped by
 * component/type.
 *  - details_text_: narrative description of the selected bottleneck.
 *  - recommendations_text_: suggested mitigation steps the developer can
 * follow.
 *  - resolve_button_: emits a resolution request for the selected entry.
 *  - refresh_button_: requests re-evaluation or an update from the host
 * detection logic.
 *
 * Public API:
 *  - addBottleneck(): append a single BottleneckInfo to the current view.
 *  - updateBottlenecks(): replace the current set with an authoritative list.
 *  - clearBottlenecks(): clear all entries from the UI.
 *
 * Signals:
 *  - bottleneckResolutionRequested: emitted when the user requests an automated
 * or host-driven attempt to resolve the selected bottleneck. The host should
 * perform any necessary confirmation and actions.
 *
 * Threading:
 *  - All public methods must be invoked on the GUI thread. Hosts collecting
 * data on other threads must post updates via signals/slots.
 */
class BottleneckDetectorWidget : public QWidget {
    Q_OBJECT

public:
    explicit BottleneckDetectorWidget(QWidget* parent = nullptr);
    ~BottleneckDetectorWidget() override = default;

    /**
     * @brief Add a single bottleneck entry to the current list.
     * @param bottleneck Structured description of the issue to display.
     */
    void addBottleneck(const BottleneckInfo& bottleneck);

    /**
     * @brief Replace the current list of displayed bottlenecks with a new set.
     * @param bottlenecks Vector of BottleneckInfo instances to display.
     */
    void updateBottlenecks(const std::vector<BottleneckInfo>& bottlenecks);

    /** @brief Remove all displayed bottlenecks and reset UI state. */
    void clearBottlenecks();

private slots:
    void onBottleneckSelected();
    void onResolveBottleneckClicked();
    void onRefreshClicked();

signals:
    void bottleneckResolutionRequested(const QString& component,
                                       const QString& type);

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

/**
 * @class DebuggingConsole
 * @brief Compact, embeddable log console used by the ProfilerDashboard.
 *
 * The console supports adding timestamped log messages, filtering by level,
 * clearing and exporting the visible log. The widget is intentionally minimal:
 * the host provides log text and controls export/filter semantics.
 *
 * Public API:
 *  - addLogMessage(): append a single formatted message to the view.
 *  - setLogLevel(): change the active filter level (host-driven filtering may
 * be applied).
 *  - enableAutoScroll(): toggle automatic scrolling to the latest message.
 *  - clearLog(): clear the visible contents.
 *
 * Styling:
 *  - getLogLevelColor() returns a color for each level; hosts can use this when
 *    pushing HTML-formatted content to the QTextEdit.
 */
class DebuggingConsole : public QWidget {
    Q_OBJECT

public:
    explicit DebuggingConsole(QWidget* parent = nullptr);
    ~DebuggingConsole() override = default;

    void addLogMessage(const QString& level, const QString& component,
                       const QString& message);
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
    QString formatLogMessage(const QString& level, const QString& component,
                             const QString& message) const;
    QColor getLogLevelColor(const QString& level) const;
};

/**
 * @class PerformanceProfilerWidget
 * @brief Minimal control and summary UI for short-lived profiling sessions.
 *
 * The widget exposes buttons to start/stop/reset/export profiling sessions and
 * provides tabular summary views for function, memory and thread-level data.
 * Heavy capture logic and serialization belong to the backend; the widget only
 * signals user intent and displays the returned results.
 *
 * Public API:
 *  - updateProfileData(): take a JSON payload from the profiler backend and
 * update UI tables.
 *  - startProfiling()/stopProfiling()/resetProfiling(): user-triggered
 * lifecycle calls.
 *
 * Signals:
 *  - profilingStarted/profilingStopped/profilingReset: emitted on user action
 * to allow hosts to act.
 */
class PerformanceProfilerWidget : public QWidget {
    Q_OBJECT

public:
    explicit PerformanceProfilerWidget(QWidget* parent = nullptr);
    ~PerformanceProfilerWidget() override = default;

    /**
     * @brief Update UI with serialized profile data returned by the backend.
     * @param profile_data JSON object with function, memory, thread arrays and
     * metadata.
     *
     * The widget expects the host to provide a stable schema. Parsing and heavy
     * aggregation should occur on the host before calling this method.
     */
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

    bool is_profiling_ = false;

    void setupUI();
    void updateFunctionTable(const QJsonArray& functions);
    void updateMemoryTable(const QJsonArray& memory_data);
    void updateThreadTable(const QJsonArray& threads);
};

/**
 * @class ProfilerDashboard
 * @brief Aggregated QMainWindow that composes the various profiling and
 * diagnostics widgets.
 *
 * The ProfilerDashboard acts as a central developer tool. It subscribes to data
 * supplied by a ProfilerIntegration (or directly from the runtime) and updates
 * the child widgets. It also exposes configuration and export facilities.
 *
 * Key features:
 *  - Real-time updates controlled by setUpdateInterval() and
 * enableRealTimeUpdates().
 *  - Threshold-driven alerts (CPU, memory, frame-rate) and status bar
 * reporting.
 *  - Exportable JSON and human-readable reports via
 * generateReport()/exportReport().
 *
 * Threading:
 *  - Dashboard methods that mutate UI elements must be invoked on the GUI
 * thread.
 *  - Data collection should occur on background threads with results posted
 * using signals.
 */
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
    void addLogMessage(const QString& level, const QString& component,
                       const QString& message);

    // **Configuration**
    void enableRealTimeUpdates(bool enabled);
    void setPerformanceThresholds(double cpu_threshold,
                                  size_t memory_threshold_mb,
                                  double frame_rate_threshold);

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

/**
 * @class ProfilerIntegration
 * @brief Lightweight integration layer that manages collection timers and
 * forwards data into the dashboard.
 *
 * ProfilerIntegration centralizes start/stop semantics for periodic collection
 * and provides convenience functions that gather data from integrated
 * subsystems. It is intentionally minimal; hosts can extend it to integrate
 * specific memory managers, animation engines or cache systems.
 *
 * Threading:
 *  - Collection may use a QTimer running on the integration's thread. When
 *    data is ready the class emits signals (dataCollected, bottleneckDetected)
 *    which should be connected to dashboard slots using queued connections when
 * crossing threads.
 */
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
