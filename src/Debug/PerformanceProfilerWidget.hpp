#pragma once

#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

namespace DeclarativeUI::Debug {

/**
 * @file PerformanceProfilerWidget.hpp
 * @brief Lightweight UI for controlling and visualizing runtime profiling
 * sessions.
 *
 * The PerformanceProfilerWidget is an embeddable developer-facing control panel
 * that provides basic controls to start, stop, reset and export profiling data
 * for the application's hot-reload and UI subsystems. It is intentionally
 * minimal — the widget surfaces actions and simple status feedback while the
 * heavy sampling/collection work is performed by the application's profiling
 * backend (e.g. PerformanceMonitor).
 *
 * Key responsibilities:
 *  - provide start/stop controls for profiling sessions,
 *  - show progress/coverage feedback while profiling is active,
 *  - allow reset of collected data and export of recorded profiles,
 *  - expose slots that host code can connect to profiler back-end functions.
 *
 * Threading and integration:
 *  - This widget is a QObject and must be used only on the GUI (main) thread.
 *  - Profiling operations that perform I/O or heavy processing must run on
 *    worker threads; this widget should only issue requests (signals/slots).
 *  - Hosts should connect the widget's slots to profiling implementation
 *    functions (or vice versa) and update the widget's UI elements from the
 *    GUI thread.
 *
 * Typical usage:
 *  - Create and add the PerformanceProfilerWidget to a diagnostics dock.
 *  - Connect start/stop/reset/export intents to the profiling backend.
 *  - Update progress_bar_ and status_label_ as the backend reports progress.
 */

/**
 * @class PerformanceProfilerWidget
 * @brief Simple control panel to manage short-lived profiling sessions.
 *
 * Public behavior:
 *  - Construct the widget and place it in a developer tools area.
 *  - The widget exposes four main actions:
 *      * Start profiling session (onStartProfilingClicked)
 *      * Stop profiling session (onStopProfilingClicked)
 *      * Reset collected profiling data (onResetProfilingClicked)
 *      * Export the recorded profile to disk (onExportProfileClicked)
 *
 * UI elements:
 *  - start_button_ / stop_button_: toggle profiling capture lifecycle.
 *  - reset_button_: clear any currently collected profile buffers.
 *  - export_button_: request writing the current profile to a file.
 *  - progress_bar_: optional progress or sampling coverage indicator.
 *  - status_label_: textual status / summary (e.g. "Idle", "Profiling — 12s").
 *
 * Extension points and integration notes:
 *  - Hosts should connect these slots to concrete profiler APIs and update
 *    progress_bar_ / status_label_ via signals emitted by the profiler.
 *  - Export should be implemented by the host (e.g. presenting file dialogs
 *    and serializing internal profiler buffers). The widget only provides the
 *    user-triggered entry point.
 *  - All updates to UI elements must occur on the GUI thread.
 */
class PerformanceProfilerWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Construct a PerformanceProfilerWidget.
     * @param parent Optional parent widget (default: nullptr).
     *
     * The constructor creates UI controls and arranges them in a vertical
     * layout. No profiling subsystem is created by the widget; hosts must
     * connect slots to an actual profiler implementation.
     */
    explicit PerformanceProfilerWidget(QWidget* parent = nullptr);

    /**
     * @brief Default destructor.
     *
     * Performs standard QObject cleanup; child widgets are deleted
     * automatically by Qt parent-child rules.
     */
    ~PerformanceProfilerWidget() = default;

private slots:
    /**
     * @brief Slot invoked when the user clicks the "Start Profiling" button.
     *
     * Expected host behavior: begin sampling, enable relevant backend timers,
     * and update the UI (disable start button, enable stop button, set status).
     */
    void onStartProfilingClicked();

    /**
     * @brief Slot invoked when the user clicks the "Stop Profiling" button.
     *
     * Expected host behavior: stop sampling, finalize profile buffers, and
     * update the UI (enable start, disable stop, show summary in
     * status_label_).
     */
    void onStopProfilingClicked();

    /**
     * @brief Slot invoked when the user clicks the "Reset Profiling" button.
     *
     * Expected host behavior: clear in-memory profiling buffers and reset any
     * coverage/progress indicators. The widget should reflect cleared state.
     */
    void onResetProfilingClicked();

    /**
     * @brief Slot invoked when the user clicks the "Export Profile" button.
     *
     * Expected host behavior: serialize and persist collected profile data to a
     * selectable file. The widget only provides the trigger; file I/O must be
     * handled by the profiling backend or host code.
     */
    void onExportProfileClicked();

private:
    /**
     * @brief Button that starts a profiling session.
     *
     * Hosts should connect this action to the profiling backend's start
     * routine.
     */
    QPushButton* start_button_;

    /**
     * @brief Button that stops an active profiling session.
     *
     * Hosts should connect this action to the profiling backend's stop routine.
     */
    QPushButton* stop_button_;

    /**
     * @brief Button that resets collected profiling data.
     *
     * Use with care: resetting discards in-memory samples.
     */
    QPushButton* reset_button_;

    /**
     * @brief Button that exports the current profiling results to disk.
     *
     * The actual export implementation belongs to the host; this button emits
     * the user intent.
     */
    QPushButton* export_button_;

    /**
     * @brief Progress bar indicating sampling coverage or elapsed time.
     *
     * Interpret the bar according to the profiling backend (percentage sampled,
     * elapsed seconds mapped to range, etc.). Hosts should update its value.
     */
    QProgressBar* progress_bar_;

    /**
     * @brief Label showing current profiling status or brief summary.
     *
     * Examples: "Idle", "Profiling — 5.3s", "Export complete".
     */
    QLabel* status_label_;

    /**
     * @brief Vertical layout arranging the widget's controls.
     */
    QVBoxLayout* layout_;
};

}  // namespace DeclarativeUI::Debug
