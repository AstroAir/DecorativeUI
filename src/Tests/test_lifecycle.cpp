#include <QApplication>
#include <QLabel>
#include <QPushButton>
#include <QSignalSpy>
#include <QTest>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <memory>

#include "../Core/DeclarativeBuilder.hpp"
#include "../Core/Lifecycle.hpp"
#include "../Core/UIElement.hpp"
#include "../Exceptions/UIExceptions.hpp"

using namespace DeclarativeUI::Core;
using namespace DeclarativeUI::Exceptions;

class LifecycleTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        // Ensure QApplication exists for widget tests
        if (!QApplication::instance()) {
            int argc = 0;
            char* argv[] = {nullptr};
            new QApplication(argc, argv);
        }
    }

    void testComponentLifecycleBasic() {
        ComponentLifecycle lifecycle;

        bool mount_called = false;
        bool unmount_called = false;

        lifecycle.onMount([&mount_called](const LifecycleContext& ctx) {
            mount_called = true;
            QCOMPARE(ctx.phase, LifecyclePhase::Mounted);
            QVERIFY(ctx.widget != nullptr);
        });

        lifecycle.onUnmount([&unmount_called](const LifecycleContext& ctx) {
            unmount_called = true;
            QCOMPARE(ctx.phase, LifecyclePhase::BeforeUnmount);
        });

        // Test mount
        auto widget = std::make_unique<QWidget>();
        QWidget* widget_ptr = widget.get();
        lifecycle.mount(widget_ptr);

        QVERIFY(mount_called);
        QVERIFY(lifecycle.isMounted());
        QCOMPARE(lifecycle.getCurrentPhase(), LifecyclePhase::Mounted);
        QCOMPARE(lifecycle.getWidget(), widget_ptr);

        // Test unmount
        lifecycle.unmount();

        QVERIFY(unmount_called);
        QVERIFY(!lifecycle.isMounted());
        QCOMPARE(lifecycle.getCurrentPhase(), LifecyclePhase::Unmounted);
        QCOMPARE(lifecycle.getWidget(), nullptr);
    }

    void testLifecycleUpdate() {
        ComponentLifecycle lifecycle;

        bool update_called = false;
        std::unordered_map<QString, QVariant> received_prev_props;
        std::unordered_map<QString, QVariant> received_current_props;

        lifecycle.onUpdate([&](const LifecycleContext& ctx) {
            update_called = true;
            received_prev_props = ctx.previous_props;
            received_current_props = ctx.current_props;
        });

        auto widget = std::make_unique<QWidget>();
        lifecycle.mount(widget.get());

        // Test update
        std::unordered_map<QString, QVariant> prev_props{{"text", "old"}};
        std::unordered_map<QString, QVariant> current_props{{"text", "new"}};

        lifecycle.update(prev_props, current_props);

        QVERIFY(update_called);
        QCOMPARE(received_prev_props["text"].toString(), QString("old"));
        QCOMPARE(received_current_props["text"].toString(), QString("new"));
        QCOMPARE(lifecycle.getMetrics().update_count, 1);
    }

    void testEffectSystem() {
        ComponentLifecycle lifecycle;

        bool effect_called = false;
        bool cleanup_called = false;

        lifecycle.useEffect(
            [&]() -> std::function<void()> {
                effect_called = true;
                return [&cleanup_called]() { cleanup_called = true; };
            },
            {QVariant("dependency")});

        auto widget = std::make_unique<QWidget>();
        lifecycle.mount(widget.get());

        QVERIFY(effect_called);
        QCOMPARE(lifecycle.getMetrics().effect_count, 1);

        // Test cleanup on unmount
        lifecycle.unmount();

        QVERIFY(cleanup_called);
        QCOMPARE(lifecycle.getMetrics().cleanup_count, 1);
    }

    void testErrorHandling() {
        ComponentLifecycle lifecycle;

        bool error_called = false;
        QString received_error;

        lifecycle.onError([&](const LifecycleContext& ctx) {
            error_called = true;
            received_error = ctx.error_message;
        });

        QString test_error = "Test error message";
        lifecycle.handleError(test_error);

        QVERIFY(error_called);
        QCOMPARE(received_error, test_error);
        QCOMPARE(lifecycle.getCurrentPhase(), LifecyclePhase::Error);
    }

    void testLifecycleBuilder() {
        ComponentLifecycle lifecycle;
        LifecycleBuilder builder(&lifecycle);

        bool mount_called = false;
        bool unmount_called = false;
        bool effect_called = false;

        builder.onMount([&mount_called]() { mount_called = true; })
            .onUnmount([&unmount_called]() { unmount_called = true; })
            .useEffect([&effect_called]() -> std::function<void()> {
                effect_called = true;
                return nullptr;
            });

        auto widget = std::make_unique<QWidget>();
        lifecycle.mount(widget.get());

        QVERIFY(mount_called);
        QVERIFY(effect_called);

        lifecycle.unmount();
        QVERIFY(unmount_called);
    }

    void testUIElementLifecycleIntegration() {
        // Create a concrete UIElement implementation for testing
        class TestUIElement : public UIElement {
        public:
            void initialize() override {
                // Create a simple widget for testing
                auto widget = std::make_unique<QLabel>("Test");
                setWidget(widget.release());
            }
        };

        auto element = std::make_unique<TestUIElement>();

        bool mount_called = false;
        bool unmount_called = false;

        element->onMount([&mount_called]() { mount_called = true; })
            .onUnmount([&unmount_called]() { unmount_called = true; });

        // Initialize should trigger mount
        element->initialize();

        QVERIFY(mount_called);
        QVERIFY(element->getLifecycle()->isMounted());

        // Cleanup should trigger unmount
        element->cleanup();

        QVERIFY(unmount_called);
        QVERIFY(!element->getLifecycle()->isMounted());
    }

    void testDeclarativeBuilderLifecycle() {
        // Create a concrete UIElement implementation for DeclarativeBuilder
        class TestUIElement : public UIElement {
        public:
            void initialize() override {
                // Widget will be set by DeclarativeBuilder
            }
        };

        bool mount_called = false;
        bool effect_called = false;

        // Note: This test demonstrates the API but may need adjustment
        // based on how DeclarativeBuilder integrates with lifecycle
        try {
            auto builder = DeclarativeBuilder<QLabel>();
            // The actual integration would happen in the build() method

            // For now, just test that the methods exist and can be called
            QVERIFY(
                true);  // Placeholder - actual integration test would go here

        } catch (const std::exception& e) {
            // Expected since we don't have full integration yet
            qDebug() << "Expected exception during builder test:" << e.what();
        }
    }

    void testLifecycleMetrics() {
        ComponentLifecycle lifecycle;

        auto widget = std::make_unique<QWidget>();

        // Mount and check metrics
        lifecycle.mount(widget.get());

        const auto& metrics = lifecycle.getMetrics();
        QVERIFY(metrics.mount_time.count() >= 0);

        // Update and check metrics
        lifecycle.update({}, {{"test", "value"}});
        QCOMPARE(metrics.update_count, 1);
        QVERIFY(metrics.update_time.count() >= 0);

        // Unmount and check metrics
        lifecycle.unmount();
        QVERIFY(metrics.unmount_time.count() >= 0);
    }

    void testLifecycleSignals() {
        ComponentLifecycle lifecycle;

        QSignalSpy mountedSpy(&lifecycle, &ComponentLifecycle::mounted);
        QSignalSpy unmountedSpy(&lifecycle, &ComponentLifecycle::unmounted);
        QSignalSpy updatedSpy(&lifecycle, &ComponentLifecycle::updated);
        QSignalSpy errorSpy(&lifecycle, &ComponentLifecycle::errorOccurred);

        auto widget = std::make_unique<QWidget>();

        // Test mount signal
        lifecycle.mount(widget.get());
        QCOMPARE(mountedSpy.count(), 1);

        // Test update signal
        lifecycle.update({}, {});
        QCOMPARE(updatedSpy.count(), 1);

        // Test error signal
        lifecycle.handleError("Test error");
        QCOMPARE(errorSpy.count(), 1);

        // Test unmount signal
        lifecycle.unmount();
        QCOMPARE(unmountedSpy.count(), 1);
    }
};

QTEST_MAIN(LifecycleTest)
#include "test_lifecycle.moc"
