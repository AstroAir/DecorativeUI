#include <QApplication>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QSignalSpy>
#include <QTemporaryFile>
#include <QTest>
#include <QTimer>
#include <memory>

#include "../../src/Debug/PerformanceProfilerWidget.hpp"

using namespace DeclarativeUI::Debug;

class PerformanceProfilerWidgetTest : public QObject {
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
        widget_ = std::make_unique<PerformanceProfilerWidget>();
    }

    void cleanup() {
        // Clean up after each test
        widget_.reset();
    }

    // **Basic PerformanceProfilerWidget Tests**
    void testWidgetCreation() {
        QVERIFY(widget_ != nullptr);

        // Check that all required UI elements are present
        QList<QPushButton*> buttons = widget_->findChildren<QPushButton*>();
        QVERIFY(buttons.size() >= 4);  // Start, Stop, Reset, Export buttons

        QProgressBar* progressBar = widget_->findChild<QProgressBar*>();
        QVERIFY(progressBar != nullptr);

        QLabel* statusLabel = widget_->findChild<QLabel*>();
        QVERIFY(statusLabel != nullptr);
    }

    void testInitialState() {
        QList<QPushButton*> buttons = widget_->findChildren<QPushButton*>();

        QPushButton* startButton = nullptr;
        QPushButton* stopButton = nullptr;

        for (QPushButton* button : buttons) {
            if (button->text().contains("Start")) {
                startButton = button;
            } else if (button->text().contains("Stop")) {
                stopButton = button;
            }
        }

        QVERIFY(startButton != nullptr);
        QVERIFY(stopButton != nullptr);

        // Initially, start button should be enabled, stop button disabled
        QVERIFY(startButton->isEnabled());
        QVERIFY(!stopButton->isEnabled());

        // Progress bar should be at 0
        QProgressBar* progressBar = widget_->findChild<QProgressBar*>();
        QVERIFY(progressBar != nullptr);
        QCOMPARE(progressBar->value(), 0);

        // Status should be "Ready"
        QLabel* statusLabel = widget_->findChild<QLabel*>();
        QVERIFY(statusLabel != nullptr);
        QVERIFY(statusLabel->text().contains("Ready"));
    }

    void testStartProfiling() {
        QList<QPushButton*> buttons = widget_->findChildren<QPushButton*>();
        QPushButton* startButton = nullptr;
        QPushButton* stopButton = nullptr;

        for (QPushButton* button : buttons) {
            if (button->text().contains("Start")) {
                startButton = button;
            } else if (button->text().contains("Stop")) {
                stopButton = button;
            }
        }

        QVERIFY(startButton != nullptr);
        QVERIFY(stopButton != nullptr);

        // Click start button
        QTest::mouseClick(startButton, Qt::LeftButton);
        QTest::qWait(100);

        // After starting, start button should be disabled, stop button enabled
        QVERIFY(!startButton->isEnabled());
        QVERIFY(stopButton->isEnabled());

        // Status should indicate profiling
        QLabel* statusLabel = widget_->findChild<QLabel*>();
        QVERIFY(statusLabel != nullptr);
        QVERIFY(statusLabel->text().contains("Profiling"));
    }

    void testStopProfiling() {
        // First start profiling
        testStartProfiling();

        QList<QPushButton*> buttons = widget_->findChildren<QPushButton*>();
        QPushButton* startButton = nullptr;
        QPushButton* stopButton = nullptr;

        for (QPushButton* button : buttons) {
            if (button->text().contains("Start")) {
                startButton = button;
            } else if (button->text().contains("Stop")) {
                stopButton = button;
            }
        }

        // Click stop button
        QTest::mouseClick(stopButton, Qt::LeftButton);
        QTest::qWait(100);

        // After stopping, start button should be enabled, stop button disabled
        QVERIFY(startButton->isEnabled());
        QVERIFY(!stopButton->isEnabled());

        // Status should indicate stopped with duration
        QLabel* statusLabel = widget_->findChild<QLabel*>();
        QVERIFY(statusLabel != nullptr);
        QVERIFY(statusLabel->text().contains("Stopped"));
        QVERIFY(statusLabel->text().contains("Duration"));

        // Progress bar should be at 100
        QProgressBar* progressBar = widget_->findChild<QProgressBar*>();
        QVERIFY(progressBar != nullptr);
        QCOMPARE(progressBar->value(), 100);
    }

    void testResetProfiling() {
        // First start and stop profiling
        testStopProfiling();

        QList<QPushButton*> buttons = widget_->findChildren<QPushButton*>();
        QPushButton* resetButton = nullptr;

        for (QPushButton* button : buttons) {
            if (button->text().contains("Reset")) {
                resetButton = button;
                break;
            }
        }

        QVERIFY(resetButton != nullptr);

        // Click reset button
        QTest::mouseClick(resetButton, Qt::LeftButton);
        QTest::qWait(100);

        // After reset, should return to initial state
        QLabel* statusLabel = widget_->findChild<QLabel*>();
        QVERIFY(statusLabel != nullptr);
        QVERIFY(statusLabel->text().contains("Ready"));

        QProgressBar* progressBar = widget_->findChild<QProgressBar*>();
        QVERIFY(progressBar != nullptr);
        QCOMPARE(progressBar->value(), 0);
    }

    void testProgressUpdates() {
        // Start profiling
        QList<QPushButton*> buttons = widget_->findChildren<QPushButton*>();
        QPushButton* startButton = nullptr;

        for (QPushButton* button : buttons) {
            if (button->text().contains("Start")) {
                startButton = button;
                break;
            }
        }

        QVERIFY(startButton != nullptr);
        QTest::mouseClick(startButton, Qt::LeftButton);

        // Wait for progress updates
        QTest::qWait(500);

        // Status should show elapsed time
        QLabel* statusLabel = widget_->findChild<QLabel*>();
        QVERIFY(statusLabel != nullptr);
        QString statusText = statusLabel->text();
        QVERIFY(statusText.contains("Profiling"));
        QVERIFY(statusText.contains("s"));  // Should contain seconds indicator
    }

    void testExportButton() {
        QList<QPushButton*> buttons = widget_->findChildren<QPushButton*>();
        QPushButton* exportButton = nullptr;

        for (QPushButton* button : buttons) {
            if (button->text().contains("Export")) {
                exportButton = button;
                break;
            }
        }

        QVERIFY(exportButton != nullptr);
        QVERIFY(exportButton->isEnabled());

        // Note: We can't easily test the file dialog interaction in unit tests,
        // but we can verify the button is present and clickable
        QVERIFY(exportButton->isVisible());
    }

    void testProgressBarRange() {
        QProgressBar* progressBar = widget_->findChild<QProgressBar*>();
        QVERIFY(progressBar != nullptr);

        QCOMPARE(progressBar->minimum(), 0);
        QCOMPARE(progressBar->maximum(), 100);
    }

    void testButtonLabels() {
        QList<QPushButton*> buttons = widget_->findChildren<QPushButton*>();

        QStringList expectedLabels = {"Start Profiling", "Stop Profiling",
                                      "Reset", "Export Profile"};
        QStringList foundLabels;

        for (QPushButton* button : buttons) {
            foundLabels.append(button->text());
        }

        for (const QString& expectedLabel : expectedLabels) {
            bool found = false;
            for (const QString& foundLabel : foundLabels) {
                if (foundLabel.contains(expectedLabel, Qt::CaseInsensitive)) {
                    found = true;
                    break;
                }
            }
            QVERIFY2(found, QString("Expected button label '%1' not found")
                                .arg(expectedLabel)
                                .toLocal8Bit());
        }
    }

    void testWidgetLayout() {
        // Verify the widget has a proper layout
        QVERIFY(widget_->layout() != nullptr);

        // Check that the layout contains the expected number of widgets
        QLayout* layout = widget_->layout();
        QVERIFY(layout->count() >=
                6);  // 4 buttons + progress bar + status label
    }

    void testMultipleStartStopCycles() {
        QList<QPushButton*> buttons = widget_->findChildren<QPushButton*>();
        QPushButton* startButton = nullptr;
        QPushButton* stopButton = nullptr;

        for (QPushButton* button : buttons) {
            if (button->text().contains("Start")) {
                startButton = button;
            } else if (button->text().contains("Stop")) {
                stopButton = button;
            }
        }

        QVERIFY(startButton != nullptr);
        QVERIFY(stopButton != nullptr);

        // Perform multiple start/stop cycles
        for (int i = 0; i < 3; ++i) {
            // Start
            QTest::mouseClick(startButton, Qt::LeftButton);
            QTest::qWait(100);
            QVERIFY(!startButton->isEnabled());
            QVERIFY(stopButton->isEnabled());

            // Stop
            QTest::mouseClick(stopButton, Qt::LeftButton);
            QTest::qWait(100);
            QVERIFY(startButton->isEnabled());
            QVERIFY(!stopButton->isEnabled());
        }
    }

private:
    std::unique_ptr<PerformanceProfilerWidget> widget_;
};

QTEST_MAIN(PerformanceProfilerWidgetTest)
#include "test_performance_profiler_widget.moc"
