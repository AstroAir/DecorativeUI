#pragma once

#include <QComboBox>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

namespace DeclarativeUI::Debug {

/**
 * @file DebuggingConsole.hpp
 * @brief Lightweight in-app debugging console used by developers during UI
 * development.
 *
 * The DebuggingConsole provides a simple, embeddable widget that displays
 * runtime logs, allows the developer to filter log categories, and export the
 * current log view to a file. It is intended for use in debug builds or a
 * developer tools dock and is intentionally small and non-intrusive.
 *
 * Responsibilities:
 *  - render textual log output with basic filtering by category/severity,
 *  - provide a button to export the currently visible log contents to disk,
 *  - emit or forward user actions to host code (hooks may be added by the
 * host),
 *  - be safe to construct and use on the GUI thread only.
 *
 * Threading and integration:
 *  - This widget is a QObject and must be used only on the GUI (main) thread.
 *  - Log producers on background threads should marshal log text to the GUI
 *    thread (via signals/slots or QMetaObject::invokeMethod with
 *    Qt::QueuedConnection) before calling widget methods that update the UI.
 *  - The widget deliberately does not own log storage; hosts can push or set
 *    the displayed contents as desired.
 *
 * Usage example:
 *  - Create and add the DebuggingConsole to a dock or diagnostics window.
 *  - Connect host log stream (or PerformanceMonitor) signals to the widget to
 *    append text.
 *  - Optionally connect to export and filter change slots to customize
 * behavior.
 */

/**
 * @class DebuggingConsole
 * @brief Simple GUI console that shows log text, supports filtering, and
 * exports.
 *
 * Public behavior:
 *  - Construct the widget and add it to a diagnostics area.
 *  - The widget exposes two user controls:
 *      * export_button_: saves current visible log text to a file when clicked.
 *      * filter_combo_: allows the user to select a log category to display.
 *  - log_display_ is a read-only QTextEdit used to show appended log messages.
 *
 * Extension points:
 *  - Host code may subclass or add signals/slots to feed logs into the console.
 *  - Export behavior may be replaced by connecting to onExportLogClicked() or
 *    overriding it in a derived class.
 */
class DebuggingConsole : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Construct a DebuggingConsole.
     * @param parent Optional parent widget; ownership follows Qt parent-child
     * rules.
     *
     * This constructor creates UI controls (export button, filter combo box and
     * text area) and arranges them in a vertical layout. The text area is set
     * to read-only by default. The combo box should be populated by the host
     * with filter options (e.g. "All", "Info", "Warning", "Error", or custom
     * categories).
     */
    explicit DebuggingConsole(QWidget* parent = nullptr);

    /**
     * @brief Default destructor.
     *
     * Performs normal QObject cleanup; owned child widgets are deleted
     * automatically by Qt parent-child ownership semantics.
     */
    ~DebuggingConsole() = default;

public slots:
    /**
     * @brief Append a line of text to the console.
     * @param text UTF-8 or QString log line to append.
     *
     * This slot is intended to be safe to call via queued connections from
     * background threads. The widget will ensure the appended text is shown
     * and the view scrolled to the bottom.
     */
    void appendLogLine(const QString& text);

    /**
     * @brief Replace the entire displayed log text.
     * @param full_text The complete text that should be displayed in the
     * console.
     *
     * Hosts can use this method to set the console contents from a stored
     * log buffer when the console is first shown or when reloading history.
     */
    void setLogText(const QString& full_text);

    /**
     * @brief Add a formatted log message with level, component, and message.
     * @param level The log level (e.g., "INFO", "ERROR", "WARNING").
     * @param component The component or module name.
     * @param message The actual log message.
     *
     * This method formats the message and calls appendLogLine internally.
     */
    void addLogMessage(const QString& level, const QString& component, const QString& message);

private slots:
    /**
     * @brief Handler invoked when the export button is clicked.
     *
     * Default behavior: present a file-save dialog (if implemented by host)
     * and write the current visible contents of log_display_ to disk. Hosts
     * may override or connect to this slot to customize file naming, path or
     * storage policy.
     */
    void onExportLogClicked();

    /**
     * @brief Handler invoked when the filter selection changes.
     *
     * Typical behavior: the widget requests the host to supply filtered log
     * text or applies a local filter on existing contents. The combo box text
     * provides the selected filter key which the host can interpret.
     */
    void onFilterChanged();

private:
    /**
     * @brief Button used to export the visible log contents to a file.
     *
     * Connected to onExportLogClicked().
     */
    QPushButton* export_button_;

    /**
     * @brief Combo box providing a set of filters or categories for the log
     * view.
     *
     * Example entries: "All", "Info", "Warning", "Error", "Performance".
     * Hosts must populate the combo box with the desired options.
     */
    QComboBox* filter_combo_;

    /**
     * @brief Read-only text area that displays the console log lines.
     *
     * Use appendLogLine() to add lines; setLogText() to replace the whole view.
     */
    QTextEdit* log_display_;

    /**
     * @brief Vertical layout that arranges controls within the widget.
     */
    QVBoxLayout* layout_;

    /**
     * @brief Storage for all log lines for filtering purposes.
     */
    QStringList all_log_lines_;

    /**
     * @brief Helper method to determine if a log line should be shown based on the current filter.
     * @param line The log line to check.
     * @param filter The current filter setting.
     * @return True if the line should be shown, false otherwise.
     */
    bool shouldShowLogLine(const QString& line, const QString& filter) const;
};

}  // namespace DeclarativeUI::Debug
