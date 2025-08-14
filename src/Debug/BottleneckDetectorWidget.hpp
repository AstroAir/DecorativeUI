#pragma once

#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

namespace DeclarativeUI::Debug {

/**
 * @file BottleneckDetectorWidget.hpp
 * @brief Interactive widget for displaying and resolving detected performance
 * bottlenecks.
 *
 * The BottleneckDetectorWidget provides a lightweight UI intended for developer
 * debugging and diagnostics. It lists identified bottlenecks (collected by the
 * PerformanceMonitor) and offers simple actions such as refresh and attempt to
 * apply suggested resolutions.
 *
 * Responsibilities:
 *  - display a list of current bottlenecks with brief descriptions,
 *  - allow the developer to select an entry to view details,
 *  - trigger a refresh request to re-run detection or pull updated data,
 *  - expose a "resolve" action that requests the system attempt an automated
 *    mitigation (e.g. apply a suggested optimization).
 *
 * Thread-safety:
 *  - This widget is a QObject and must be used only on the GUI (main) thread.
 *  - Calls from background threads must be marshalled to the GUI thread (via
 *    signals/slots or QMetaObject::invokeMethod with Qt::QueuedConnection).
 *
 * Integration notes:
 *  - Connect the widget to the PerformanceMonitor or manager that emits
 * detected bottlenecks. The widget intentionally keeps a minimal internal model
 * and expects authoritative data to be supplied by the hosting code.
 *  - The "resolve" action is advisory: the widget emits requests or calls into
 *    supplied handlers; it does not perform intrusive system-level changes by
 *    itself.
 */

/**
 * @class BottleneckDetectorWidget
 * @brief A simple diagnostic QWidget to present bottleneck findings and
 * actions.
 *
 * Public behavior:
 *  - Construct the widget and add it to a diagnostics window or dock.
 *  - Callers should populate the list model (or provide callbacks) with
 * detailed BottleneckInfo items produced by the PerformanceMonitor.
 *
 * UI elements:
 *  - refresh_button_ : requests an update of detected bottlenecks.
 *  - resolve_button_ : attempts to apply a suggested mitigation for the
 * selected bottleneck.
 *  - bottleneck_list_: shows a compact list of bottleneck summaries (one per
 * row).
 *
 * Signals/slots:
 *  - onRefreshClicked(): invoked when the refresh button is pressed.
 *  - onResolveBottleneckClicked(): invoked when the resolve button is pressed.
 *  - onBottleneckSelected(): invoked when the selection in the list changes.
 *
 * Expected extension points:
 *  - Host code should connect to these slots or connect the widget's actions to
 *    concrete handling logic (e.g. call
 * PerformanceMonitor::detectBottlenecks()).
 */
class BottleneckDetectorWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Construct a BottleneckDetectorWidget.
     * @param parent Optional parent widget (default: nullptr).
     *
     * The widget does not take ownership of external monitoring services; it
     * only presents data supplied by the host. Constructing the widget creates
     * its internal buttons, list and layout.
     */
    explicit BottleneckDetectorWidget(QWidget* parent = nullptr);

    /**
     * @brief Destructor.
     *
     * Performs normal QObject cleanup. All child widgets created in the ctor
     * are deleted automatically by Qt parent-child ownership.
     */
    ~BottleneckDetectorWidget() = default;

private slots:
    /**
     * @brief Handle selection changes in the bottleneck list.
     *
     * Typical behavior: update details pane, enable/disable resolve button,
     * and optionally notify hosting code about the selected bottleneck.
     */
    void onBottleneckSelected();

    /**
     * @brief Handle user request to refresh/re-run bottleneck detection.
     *
     * Typical behavior: emit a request or call into the manager to refresh the
     * diagnostics. The actual detection work must run off the UI thread.
     */
    void onRefreshClicked();

    /**
     * @brief Handle user request to resolve the currently selected bottleneck.
     *
     * Typical behavior: dispatch a request to the host to attempt the
     * recommended mitigation. Hosts must confirm any potentially disruptive
     * actions.
     */
    void onResolveBottleneckClicked();

private:
    /**
     * @brief Refresh button that requests updated bottleneck analysis.
     *
     * Connected to onRefreshClicked().
     */
    QPushButton* refresh_button_;

    /**
     * @brief Button that triggers an attempt to resolve the selected
     * bottleneck.
     *
     * Connected to onResolveBottleneckClicked(). Hosts should validate actions
     * before applying them.
     */
    QPushButton* resolve_button_;

    /**
     * @brief List widget showing short summaries of detected bottlenecks.
     *
     * Each list row should contain a concise summary (e.g. "CPU - parsing slow
     * (score: 85)"). Selecting a row should expose more detailed information to
     * the host or display an expanded pane (not implemented in this minimal
     * widget).
     */
    QListWidget* bottleneck_list_;

    /**
     * @brief Vertical layout arranging the controls.
     */
    QVBoxLayout* layout_;
};

}  // namespace DeclarativeUI::Debug
