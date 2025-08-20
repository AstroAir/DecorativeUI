#include <QApplication>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QSignalSpy>
#include <QTest>
#include <QTimer>
#include <memory>

#include "../../src/Debug/BottleneckDetectorWidget.hpp"

using namespace DeclarativeUI::Debug;

class BottleneckDetectorWidgetTest : public QObject {
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
        widget_ = std::make_unique<BottleneckDetectorWidget>();
    }

    void cleanup() {
        // Clean up after each test
        widget_.reset();
    }

    // **Basic BottleneckDetectorWidget Tests**
    void testWidgetCreation() {
        QVERIFY(widget_ != nullptr);

        // Check that all required UI elements are present
        [[maybe_unused]] auto* refreshButton =
            widget_->findChild<QPushButton*>("refresh_button_");
        [[maybe_unused]] auto* resolveButton =
            widget_->findChild<QPushButton*>("resolve_button_");
        [[maybe_unused]] auto* bottleneckList =
            widget_->findChild<QListWidget*>("bottleneck_list_");

        // Note: The actual object names might be different, so let's check by
        // type
        QList<QPushButton*> buttons = widget_->findChildren<QPushButton*>();
        QVERIFY(buttons.size() >=
                2);  // Should have at least refresh and resolve buttons

        QListWidget* listWidget = widget_->findChild<QListWidget*>();
        QVERIFY(listWidget != nullptr);
    }

    void testInitialState() {
        QListWidget* listWidget = widget_->findChild<QListWidget*>();
        QVERIFY(listWidget != nullptr);

        // Should have sample bottlenecks added during construction
        QVERIFY(listWidget->count() > 0);

        // Resolve button should initially be disabled
        QList<QPushButton*> buttons = widget_->findChildren<QPushButton*>();
        bool foundDisabledResolveButton = false;
        for (QPushButton* button : buttons) {
            if (button->text().contains("Resolve")) {
                foundDisabledResolveButton = !button->isEnabled();
                break;
            }
        }
        QVERIFY(foundDisabledResolveButton);
    }

    void testSampleBottlenecks() {
        QListWidget* listWidget = widget_->findChild<QListWidget*>();
        QVERIFY(listWidget != nullptr);

        // Check that sample bottlenecks are present
        QVERIFY(listWidget->count() >=
                3);  // Should have several sample bottlenecks

        // Check content of sample bottlenecks
        bool foundCpuBottleneck = false;
        bool foundMemoryBottleneck = false;
        bool foundIoBottleneck = false;

        for (int i = 0; i < listWidget->count(); ++i) {
            QListWidgetItem* item = listWidget->item(i);
            QString text = item->text();

            if (text.contains("CPU", Qt::CaseInsensitive)) {
                foundCpuBottleneck = true;
            }
            if (text.contains("Memory", Qt::CaseInsensitive)) {
                foundMemoryBottleneck = true;
            }
            if (text.contains("I/O", Qt::CaseInsensitive)) {
                foundIoBottleneck = true;
            }
        }

        QVERIFY(foundCpuBottleneck);
        QVERIFY(foundMemoryBottleneck);
        QVERIFY(foundIoBottleneck);
    }

    void testBottleneckSelection() {
        QListWidget* listWidget = widget_->findChild<QListWidget*>();
        QVERIFY(listWidget != nullptr);
        QVERIFY(listWidget->count() > 0);

        // Select the first item
        listWidget->setCurrentRow(0);

        // Simulate the selection signal
        emit listWidget->itemClicked(listWidget->item(0));

        // Process events to allow signal handling
        QTest::qWait(100);

        // Resolve button should now be enabled
        QList<QPushButton*> buttons = widget_->findChildren<QPushButton*>();
        bool foundEnabledResolveButton = false;
        for (QPushButton* button : buttons) {
            if (button->text().contains("Resolve")) {
                foundEnabledResolveButton = button->isEnabled();
                break;
            }
        }
        QVERIFY(foundEnabledResolveButton);
    }

    void testRefreshButton() {
        QList<QPushButton*> buttons = widget_->findChildren<QPushButton*>();
        QPushButton* refreshButton = nullptr;

        for (QPushButton* button : buttons) {
            if (button->text().contains("Refresh")) {
                refreshButton = button;
                break;
            }
        }

        QVERIFY(refreshButton != nullptr);
        QVERIFY(refreshButton->isEnabled());

        // Click the refresh button
        QTest::mouseClick(refreshButton, Qt::LeftButton);

        // Process events
        QTest::qWait(100);

        // Button should be temporarily disabled during refresh
        QVERIFY(!refreshButton->isEnabled() ||
                refreshButton->text().contains("Refreshing"));
    }

    void testResolveButton() {
        QListWidget* listWidget = widget_->findChild<QListWidget*>();
        QVERIFY(listWidget != nullptr);
        QVERIFY(listWidget->count() > 0);

        // Select an item first
        listWidget->setCurrentRow(0);
        emit listWidget->itemClicked(listWidget->item(0));
        QTest::qWait(100);

        QList<QPushButton*> buttons = widget_->findChildren<QPushButton*>();
        QPushButton* resolveButton = nullptr;

        for (QPushButton* button : buttons) {
            if (button->text().contains("Resolve")) {
                resolveButton = button;
                break;
            }
        }

        QVERIFY(resolveButton != nullptr);
        QVERIFY(resolveButton->isEnabled());

        // Note: We can't easily test the message box interaction in unit tests,
        // but we can verify the button is clickable
        QVERIFY(resolveButton->isVisible());
    }

    void testTooltips() {
        QListWidget* listWidget = widget_->findChild<QListWidget*>();
        QVERIFY(listWidget != nullptr);

        if (listWidget->count() > 0) {
            QListWidgetItem* item = listWidget->item(0);
            QVERIFY(!item->toolTip().isEmpty());
        }
    }

    void testWidgetLayout() {
        // Verify the widget has a proper layout
        QVERIFY(widget_->layout() != nullptr);

        // Check that the layout contains the expected number of widgets
        QLayout* layout = widget_->layout();
        QVERIFY(layout->count() >= 3);  // Should have at least buttons and list
    }

    void testMultipleSelections() {
        QListWidget* listWidget = widget_->findChild<QListWidget*>();
        QVERIFY(listWidget != nullptr);

        if (listWidget->count() >= 2) {
            // Select first item
            listWidget->setCurrentRow(0);
            emit listWidget->itemClicked(listWidget->item(0));
            QTest::qWait(50);

            // Select second item
            listWidget->setCurrentRow(1);
            emit listWidget->itemClicked(listWidget->item(1));
            QTest::qWait(50);

            // Should still have resolve button enabled
            QList<QPushButton*> buttons = widget_->findChildren<QPushButton*>();
            bool foundEnabledResolveButton = false;
            for (QPushButton* button : buttons) {
                if (button->text().contains("Resolve")) {
                    foundEnabledResolveButton = button->isEnabled();
                    break;
                }
            }
            QVERIFY(foundEnabledResolveButton);
        }
    }

private:
    std::unique_ptr<BottleneckDetectorWidget> widget_;
};

QTEST_MAIN(BottleneckDetectorWidgetTest)
#include "test_bottleneck_detector_widget.moc"
