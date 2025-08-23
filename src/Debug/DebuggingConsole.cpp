#include "DebuggingConsole.hpp"
#include <QDateTime>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QRegularExpression>
#include <QScrollBar>
#include <QStandardPaths>
#include <QTextStream>

namespace DeclarativeUI::Debug {

DebuggingConsole::DebuggingConsole(QWidget* parent) : QWidget(parent) {
    layout_ = new QVBoxLayout(this);

    export_button_ = new QPushButton("Export Log", this);
    filter_combo_ = new QComboBox(this);
    log_display_ = new QTextEdit(this);

    filter_combo_->addItems({"All", "Error", "Warning", "Info", "Debug"});
    log_display_->setReadOnly(true);

    layout_->addWidget(export_button_);
    layout_->addWidget(filter_combo_);
    layout_->addWidget(log_display_);

    connect(export_button_, &QPushButton::clicked, this,
            &DebuggingConsole::onExportLogClicked);
    connect(filter_combo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &DebuggingConsole::onFilterChanged);

    qDebug() << "DebuggingConsole created";
}

void DebuggingConsole::appendLogLine(const QString& text) {
    if (text.isEmpty()) {
        return;
    }

    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    QString formattedLine = QString("[%1] %2").arg(timestamp, text);

    // Store the raw log line for filtering
    all_log_lines_.append(formattedLine);

    // Apply current filter
    QString currentFilter = filter_combo_->currentText();
    if (shouldShowLogLine(formattedLine, currentFilter)) {
        log_display_->append(formattedLine);

        // Auto-scroll to bottom
        QScrollBar* scrollBar = log_display_->verticalScrollBar();
        scrollBar->setValue(scrollBar->maximum());
    }

    // Limit stored lines to prevent memory issues
    const int maxLines = 10000;
    if (all_log_lines_.size() > maxLines) {
        // Remove multiple lines at once for better performance
        int linesToRemove = all_log_lines_.size() - maxLines;
        for (int i = 0; i < linesToRemove; ++i) {
            all_log_lines_.removeFirst();
        }

        // Always rebuild display when lines are removed to ensure consistency
        QString currentFilter = filter_combo_->currentText();
        log_display_->clear();
        for (const QString& line : all_log_lines_) {
            if (shouldShowLogLine(line, currentFilter)) {
                log_display_->append(line);
            }
        }

        // Auto-scroll to bottom after rebuild
        QScrollBar* scrollBar = log_display_->verticalScrollBar();
        scrollBar->setValue(scrollBar->maximum());
    }
}

void DebuggingConsole::setLogText(const QString& full_text) {
    all_log_lines_.clear();

    // Split the full text into individual lines
    QStringList lines = full_text.split('\n', Qt::SkipEmptyParts);
    for (const QString& line : lines) {
        all_log_lines_.append(line);
    }

    // Apply current filter and update display
    onFilterChanged();
}

void DebuggingConsole::addLogMessage(const QString& level,
                                     const QString& component,
                                     const QString& message) {
    // Format the message with level and component information
    QString formattedMessage =
        QString("[%1] %2: %3").arg(level, component, message);

    // Use the existing appendLogLine method
    appendLogLine(formattedMessage);
}

void DebuggingConsole::onExportLogClicked() {
    QString defaultPath =
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString defaultFileName =
        QString("debug_log_%1.txt")
            .arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss"));
    QString defaultFilePath = defaultPath + "/" + defaultFileName;

    QString fileName =
        QFileDialog::getSaveFileName(this, "Export Debug Log", defaultFilePath,
                                     "Text Files (*.txt);;All Files (*)");

    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(
            this, "Export Failed",
            QString("Could not open file for writing: %1").arg(fileName));
        return;
    }

    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);

    // Write header
    stream << "Debug Log Export\n";
    stream << "Generated: "
           << QDateTime::currentDateTime().toString(Qt::ISODate) << "\n";
    stream << "Filter: " << filter_combo_->currentText() << "\n";
    stream << "Total Lines: " << all_log_lines_.size() << "\n";
    stream << QString("=").repeated(50) << "\n\n";

    // Write log content (use currently displayed content)
    stream << log_display_->toPlainText();

    file.close();

    QMessageBox::information(this, "Export Successful",
                             QString("Log exported to: %1").arg(fileName));

    qDebug() << "Log exported to:" << fileName;
}

void DebuggingConsole::onFilterChanged() {
    QString selectedFilter = filter_combo_->currentText();

    // Clear current display
    log_display_->clear();

    // Re-populate with filtered content
    for (const QString& line : all_log_lines_) {
        if (shouldShowLogLine(line, selectedFilter)) {
            log_display_->append(line);
        }
    }

    // Auto-scroll to bottom
    QScrollBar* scrollBar = log_display_->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());

    qDebug() << "Filter changed to:" << selectedFilter;
}

bool DebuggingConsole::shouldShowLogLine(const QString& line,
                                         const QString& filter) const {
    if (filter == "All") {
        return true;
    }

    // Create case-insensitive regex patterns for each log level
    QRegularExpression errorPattern("\\b(error|err|fatal|critical)\\b",
                                    QRegularExpression::CaseInsensitiveOption);
    QRegularExpression warningPattern(
        "\\b(warning|warn)\\b", QRegularExpression::CaseInsensitiveOption);
    QRegularExpression infoPattern("\\b(info|information)\\b",
                                   QRegularExpression::CaseInsensitiveOption);
    QRegularExpression debugPattern("\\b(debug|dbg)\\b",
                                    QRegularExpression::CaseInsensitiveOption);

    if (filter == "Error") {
        return errorPattern.match(line).hasMatch();
    } else if (filter == "Warning") {
        return warningPattern.match(line).hasMatch();
    } else if (filter == "Info") {
        return infoPattern.match(line).hasMatch();
    } else if (filter == "Debug") {
        return debugPattern.match(line).hasMatch();
    }

    return true;  // Default to showing the line
}

}  // namespace DeclarativeUI::Debug
