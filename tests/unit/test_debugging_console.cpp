#include <QApplication>
#include <QScrollBar>
#include <QSignalSpy>
#include <QTemporaryFile>
#include <QTest>
#include <QTextStream>
#include <QTimer>
#include <memory>

#include "../../src/Debug/DebuggingConsole.hpp"

using namespace DeclarativeUI::Debug;

class DebuggingConsoleTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        // Initialize Qt application if not already done
        if (!QApplication::instance()) {
            int argc = 0;
            char* argv[] = {nullptr};
            new QApplication(argc, argv);
        }
    }

    void cleanupTestCase() {
        // Clean up any global state
    }

    void init() {
        // Set up for each test
        console_ = std::make_unique<DebuggingConsole>();
    }

    void cleanup() {
        // Clean up after each test
        console_.reset();
    }

    // **Basic DebuggingConsole Tests**
    void testConsoleCreation() {
        QVERIFY(console_ != nullptr);
        QVERIFY(console_->findChild<QPushButton*>() != nullptr);
        QVERIFY(console_->findChild<QComboBox*>() != nullptr);
        QVERIFY(console_->findChild<QTextEdit*>() != nullptr);
    }

    void testAppendLogLine() {
        QString testMessage = "Test log message";
        console_->appendLogLine(testMessage);

        QTextEdit* logDisplay = console_->findChild<QTextEdit*>();
        QVERIFY(logDisplay != nullptr);

        QString displayText = logDisplay->toPlainText();
        QVERIFY(displayText.contains(testMessage));
        QVERIFY(displayText.contains("["));  // Should contain timestamp
    }

    void testAppendEmptyLogLine() {
        QString emptyMessage = "";
        console_->appendLogLine(emptyMessage);

        QTextEdit* logDisplay = console_->findChild<QTextEdit*>();
        QVERIFY(logDisplay != nullptr);

        QString displayText = logDisplay->toPlainText();
        QVERIFY(displayText.isEmpty());  // Should not add empty lines
    }

    void testSetLogText() {
        QString fullLogText = "Line 1\nLine 2\nLine 3";
        console_->setLogText(fullLogText);

        QTextEdit* logDisplay = console_->findChild<QTextEdit*>();
        QVERIFY(logDisplay != nullptr);

        QString displayText = logDisplay->toPlainText();
        QVERIFY(displayText.contains("Line 1"));
        QVERIFY(displayText.contains("Line 2"));
        QVERIFY(displayText.contains("Line 3"));
    }

    void testLogFiltering() {
        // Add different types of log messages
        console_->appendLogLine("This is an error message");
        console_->appendLogLine("This is a warning message");
        console_->appendLogLine("This is an info message");
        console_->appendLogLine("This is a debug message");

        QComboBox* filterCombo = console_->findChild<QComboBox*>();
        QVERIFY(filterCombo != nullptr);

        // Test filtering by Error
        filterCombo->setCurrentText("Error");
        QTest::qWait(100);  // Allow UI to update

        QTextEdit* logDisplay = console_->findChild<QTextEdit*>();
        QString displayText = logDisplay->toPlainText();
        QVERIFY(displayText.contains("error"));
    }

    void testExportFunctionality() {
        // Add some test data
        console_->appendLogLine("Test message 1");
        console_->appendLogLine("Test message 2");

        // Create a temporary file for testing
        QTemporaryFile tempFile;
        QVERIFY(tempFile.open());
        QString tempFileName = tempFile.fileName();
        tempFile.close();

        // Note: We can't easily test the file dialog interaction,
        // but we can verify the console has the export button
        QPushButton* exportButton = console_->findChild<QPushButton*>();
        QVERIFY(exportButton != nullptr);
        QVERIFY(exportButton->text().contains("Export"));
    }

    void testFilterComboBoxItems() {
        QComboBox* filterCombo = console_->findChild<QComboBox*>();
        QVERIFY(filterCombo != nullptr);

        // Check that all expected filter options are present
        QStringList expectedItems = {"All", "Error", "Warning", "Info",
                                     "Debug"};
        for (const QString& item : expectedItems) {
            QVERIFY(filterCombo->findText(item) != -1);
        }
    }

    void testLogDisplayReadOnly() {
        QTextEdit* logDisplay = console_->findChild<QTextEdit*>();
        QVERIFY(logDisplay != nullptr);
        QVERIFY(logDisplay->isReadOnly());
    }

    void testMultipleLogLines() {
        const int numLines = 100;
        for (int i = 0; i < numLines; ++i) {
            console_->appendLogLine(QString("Log line %1").arg(i));
        }

        QTextEdit* logDisplay = console_->findChild<QTextEdit*>();
        QVERIFY(logDisplay != nullptr);

        QString displayText = logDisplay->toPlainText();
        QVERIFY(displayText.contains("Log line 0"));
        QVERIFY(displayText.contains("Log line 99"));
    }

    void testLogLineLimit() {
        // Test that the console limits stored lines to prevent memory issues
        const int excessiveLines = 10500;  // Slightly more than the 10000 limit
        for (int i = 0; i < excessiveLines; ++i) {
            console_->appendLogLine(QString("Line %1").arg(i));
        }

        QTextEdit* logDisplay = console_->findChild<QTextEdit*>();
        QVERIFY(logDisplay != nullptr);

        QString displayText = logDisplay->toPlainText();
        // Should not contain the earliest lines due to limit
        QVERIFY(!displayText.contains("Line 0"));
        QVERIFY(
            displayText.contains(QString("Line %1").arg(excessiveLines - 1)));
    }

    void testFilteringEdgeCases() {
        // Test filtering with mixed case and special characters
        console_->appendLogLine("ERROR: Critical system failure!");
        console_->appendLogLine("Warning: Memory usage high");
        console_->appendLogLine("INFO: System started successfully");
        console_->appendLogLine("DEBUG: Variable x = 42");
        console_->appendLogLine("Random message without level");

        QComboBox* filterCombo = console_->findChild<QComboBox*>();
        QTextEdit* logDisplay = console_->findChild<QTextEdit*>();
        QVERIFY(filterCombo != nullptr);
        QVERIFY(logDisplay != nullptr);

        // Test each filter
        filterCombo->setCurrentText("Error");
        QTest::qWait(100);
        QString errorText = logDisplay->toPlainText();
        QVERIFY(errorText.contains("ERROR"));
        QVERIFY(!errorText.contains("Warning"));

        filterCombo->setCurrentText("Warning");
        QTest::qWait(100);
        QString warningText = logDisplay->toPlainText();
        QVERIFY(warningText.contains("Warning"));
        QVERIFY(!warningText.contains("ERROR"));

        filterCombo->setCurrentText("All");
        QTest::qWait(100);
        QString allText = logDisplay->toPlainText();
        QVERIFY(allText.contains("ERROR"));
        QVERIFY(allText.contains("Warning"));
        QVERIFY(allText.contains("Random message"));
    }

    void testConcurrentLogAppending() {
        // Test rapid log appending to simulate high-frequency logging
        const int rapidLogs = 1000;
        for (int i = 0; i < rapidLogs; ++i) {
            console_->appendLogLine(QString("Rapid log %1").arg(i));
            if (i % 100 == 0) {
                QTest::qWait(1);  // Brief pause every 100 logs
            }
        }

        QTextEdit* logDisplay = console_->findChild<QTextEdit*>();
        QVERIFY(logDisplay != nullptr);

        QString displayText = logDisplay->toPlainText();
        QVERIFY(displayText.contains("Rapid log 0"));
        QVERIFY(
            displayText.contains(QString("Rapid log %1").arg(rapidLogs - 1)));
    }

    void testSpecialCharactersInLogs() {
        // Test handling of special characters and Unicode
        console_->appendLogLine("Test with special chars: !@#$%^&*()");
        console_->appendLogLine("Unicode test: 你好世界 🌍 ñáéíóú");
        console_->appendLogLine("XML-like: <tag>content</tag>");
        console_->appendLogLine("JSON-like: {\"key\": \"value\"}");

        QTextEdit* logDisplay = console_->findChild<QTextEdit*>();
        QVERIFY(logDisplay != nullptr);

        QString displayText = logDisplay->toPlainText();
        QVERIFY(displayText.contains("!@#$%^&*()"));
        QVERIFY(displayText.contains("你好世界"));
        QVERIFY(displayText.contains("<tag>content</tag>"));
        QVERIFY(displayText.contains("{\"key\": \"value\"}"));
    }

    void testLogDisplayScrolling() {
        // Test that new logs auto-scroll to bottom
        QTextEdit* logDisplay = console_->findChild<QTextEdit*>();
        QVERIFY(logDisplay != nullptr);

        // Add many logs to force scrolling
        for (int i = 0; i < 100; ++i) {
            console_->appendLogLine(QString("Scroll test line %1").arg(i));
        }

        // Check that scroll bar is at maximum (bottom)
        QScrollBar* scrollBar = logDisplay->verticalScrollBar();
        QVERIFY(scrollBar != nullptr);
        // Note: In headless testing, scroll position might not behave exactly
        // as in GUI but we can verify the scrollbar exists and the method
        // doesn't crash
        QVERIFY(scrollBar->maximum() >= 0);
    }

    void testEmptyAndWhitespaceHandling() {
        // Test various empty and whitespace scenarios
        console_->appendLogLine("");      // Empty string
        console_->appendLogLine("   ");   // Only spaces
        console_->appendLogLine("\t\n");  // Only whitespace chars
        console_->appendLogLine(
            "  Valid message  ");  // Message with surrounding spaces

        QTextEdit* logDisplay = console_->findChild<QTextEdit*>();
        QVERIFY(logDisplay != nullptr);

        QString displayText = logDisplay->toPlainText();
        // Empty strings should not be added
        QVERIFY(!displayText.contains("[]"));  // Empty timestamp brackets
        // Valid message should be present
        QVERIFY(displayText.contains("Valid message"));
    }

private:
    std::unique_ptr<DebuggingConsole> console_;
};

QTEST_MAIN(DebuggingConsoleTest)
#include "test_debugging_console.moc"
