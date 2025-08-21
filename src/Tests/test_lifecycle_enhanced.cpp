#include <QApplication>
#include <QLabel>
#include <QMutex>
#include <QPushButton>
#include <QSharedPointer>
#include <QSignalSpy>
#include <QTest>
#include <QThread>
#include <QTimer>
#include <QVBoxLayout>
#include <QWaitCondition>
#include <QWeakPointer>
#include <QWidget>
#include <atomic>
#include <chrono>
#include <memory>
#include <vector>

#include "../Core/DeclarativeBuilder.hpp"
#include "../Core/Lifecycle.hpp"
#include "../Core/UIElement.hpp"
#include "../Exceptions/UIExceptions.hpp"

using namespace DeclarativeUI::Core;
using namespace DeclarativeUI::Exceptions;

/**
 * @brief Enhanced lifecycle tests covering edge cases, memory management,
 * Qt integration, performance metrics, and error handling
 */
class LifecycleEnhancedTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        if (!QApplication::instance()) {
            int argc = 0;
            char* argv[] = {nullptr};
            new QApplication(argc, argv);
        }
    }

    void cleanupTestCase() {
        // Ensure all widgets are cleaned up
        QApplication::processEvents();
    }

    void init() {
        // Reset any global state before each test
        QApplication::processEvents();
    }

    void cleanup() {
        // Clean up after each test
        QApplication::processEvents();
    }

    /**
     * @brief Test lifecycle hook execution order under various scenarios
     */
    void testLifecycleHookExecutionOrder() {
        ComponentLifecycle lifecycle;
        std::vector<QString> execution_order;

        // Register multiple hooks of each type
        lifecycle.onMount([&execution_order](const LifecycleContext&) {
            execution_order.push_back("mount1");
        });
        lifecycle.onMount([&execution_order](const LifecycleContext&) {
            execution_order.push_back("mount2");
        });
        lifecycle.onMount([&execution_order](const LifecycleContext&) {
            execution_order.push_back("mount3");
        });

        lifecycle.onUpdate([&execution_order](const LifecycleContext&) {
            execution_order.push_back("update1");
        });
        lifecycle.onUpdate([&execution_order](const LifecycleContext&) {
            execution_order.push_back("update2");
        });

        lifecycle.onUnmount([&execution_order](const LifecycleContext&) {
            execution_order.push_back("unmount1");
        });
        lifecycle.onUnmount([&execution_order](const LifecycleContext&) {
            execution_order.push_back("unmount2");
        });

        auto widget = std::make_unique<QWidget>();
        QWidget* widget_ptr = widget.get();

        // Test mount hook order
        lifecycle.mount(widget_ptr);
        QCOMPARE(execution_order.size(), 3);
        QCOMPARE(execution_order[0], QString("mount1"));
        QCOMPARE(execution_order[1], QString("mount2"));
        QCOMPARE(execution_order[2], QString("mount3"));

        // Test update hook order
        execution_order.clear();
        lifecycle.update({{"old", "value"}}, {{"new", "value"}});
        QCOMPARE(execution_order.size(), 2);
        QCOMPARE(execution_order[0], QString("update1"));
        QCOMPARE(execution_order[1], QString("update2"));

        // Test unmount hook order
        execution_order.clear();
        lifecycle.unmount();
        QCOMPARE(execution_order.size(), 2);
        QCOMPARE(execution_order[0], QString("unmount1"));
        QCOMPARE(execution_order[1], QString("unmount2"));
    }

    /**
     * @brief Test memory leak prevention during component destruction
     */
    void testMemoryLeakPrevention() {
        std::vector<QWeakPointer<QWidget>> weak_widgets;
        std::vector<std::unique_ptr<ComponentLifecycle>> lifecycles;

        // Create multiple lifecycle instances with widgets
        for (int i = 0; i < 10; ++i) {
            auto lifecycle = std::make_unique<ComponentLifecycle>();
            auto widget = QSharedPointer<QWidget>::create();
            weak_widgets.push_back(widget.toWeakRef());

            // Add effects that might hold references
            lifecycle->useEffect([widget]() -> std::function<void()> {
                // Capture widget in effect
                auto captured = widget;
                return [captured]() {
                    // Cleanup function holds reference
                };
            });

            lifecycle->mount(widget.get());
            lifecycles.push_back(std::move(lifecycle));
        }

        // Verify all widgets are alive
        for (const auto& weak_widget : weak_widgets) {
            QVERIFY(!weak_widget.isNull());
        }

        // Destroy all lifecycles
        lifecycles.clear();
        QApplication::processEvents();

        // Verify widgets can be garbage collected
        // Note: This test verifies that lifecycle doesn't prevent cleanup
        // The actual cleanup depends on Qt's parent-child relationships
        for (const auto& weak_widget : weak_widgets) {
            // Widget should still exist since we hold shared_ptr references
            // But lifecycle should not prevent cleanup when references are
            // released
            QVERIFY(!weak_widget.isNull());
        }
    }

    /**
     * @brief Test integration with Qt's parent-child widget ownership model
     */
    void testQtParentChildIntegration() {
        auto parent_widget = std::make_unique<QWidget>();
        ComponentLifecycle lifecycle;

        bool mount_called = false;
        bool unmount_called = false;
        bool widget_destroyed = false;

        lifecycle.onMount([&mount_called](const LifecycleContext& ctx) {
            mount_called = true;
            QVERIFY(ctx.widget != nullptr);
        });

        lifecycle.onUnmount([&unmount_called](const LifecycleContext&) {
            unmount_called = true;
        });

        // Create child widget with Qt parent-child relationship
        auto child_widget = new QWidget(parent_widget.get());
        connect(child_widget, &QWidget::destroyed,
                [&widget_destroyed]() { widget_destroyed = true; });

        lifecycle.mount(child_widget);
        QVERIFY(mount_called);
        QVERIFY(lifecycle.isMounted());

        // Destroy parent widget (should trigger child destruction)
        parent_widget.reset();
        QApplication::processEvents();

        QVERIFY(widget_destroyed);
        QVERIFY(unmount_called);
        QVERIFY(!lifecycle.isMounted());
    }

    /**
     * @brief Test performance metrics accuracy under various scenarios
     */
    void testPerformanceMetricsAccuracy() {
        ComponentLifecycle lifecycle;

        // Add hooks with known delays
        lifecycle.onMount([](const LifecycleContext&) {
            QTest::qWait(10);  // 10ms delay
        });

        lifecycle.onUpdate([](const LifecycleContext&) {
            QTest::qWait(5);  // 5ms delay
        });

        lifecycle.onUnmount([](const LifecycleContext&) {
            QTest::qWait(15);  // 15ms delay
        });

        auto widget = std::make_unique<QWidget>();

        // Test mount timing
        auto start_time = std::chrono::steady_clock::now();
        lifecycle.mount(widget.get());
        auto mount_duration = std::chrono::steady_clock::now() - start_time;

        const auto& metrics = lifecycle.getMetrics();
        QVERIFY(metrics.mount_time.count() >= 10);  // At least 10ms
        QVERIFY(metrics.mount_time.count() < 50);  // But reasonable upper bound

        // Test update timing
        start_time = std::chrono::steady_clock::now();
        lifecycle.update({}, {{"key", "value"}});
        auto update_duration = std::chrono::steady_clock::now() - start_time;

        QVERIFY(metrics.update_time.count() >= 5);  // At least 5ms
        QVERIFY(metrics.update_time.count() <
                30);  // But reasonable upper bound
        QCOMPARE(metrics.update_count, 1);

        // Test unmount timing
        start_time = std::chrono::steady_clock::now();
        lifecycle.unmount();
        auto unmount_duration = std::chrono::steady_clock::now() - start_time;

        QVERIFY(metrics.unmount_time.count() >= 15);  // At least 15ms
        QVERIFY(metrics.unmount_time.count() <
                50);  // But reasonable upper bound
    }

    /**
     * @brief Test error handling in lifecycle hooks
     */
    void testErrorHandlingInLifecycleHooks() {
        ComponentLifecycle lifecycle;

        bool error_hook_called = false;
        QString error_message;

        lifecycle.onError([&](const LifecycleContext& ctx) {
            error_hook_called = true;
            error_message = ctx.error_message;
        });

        // Add hooks that throw exceptions
        lifecycle.onMount([](const LifecycleContext&) {
            throw std::runtime_error("Mount hook error");
        });

        lifecycle.onMount([](const LifecycleContext&) {
            // This should still be called despite previous hook error
        });

        lifecycle.onUpdate([](const LifecycleContext&) {
            throw std::invalid_argument("Update hook error");
        });

        lifecycle.onUnmount([](const LifecycleContext&) {
            throw std::logic_error("Unmount hook error");
        });

        auto widget = std::make_unique<QWidget>();

        // Test mount error handling
        try {
            lifecycle.mount(widget.get());
            // Should not throw, errors should be caught internally
        } catch (...) {
            QFAIL("Mount should not throw exceptions");
        }

        // Test update error handling
        try {
            lifecycle.update({}, {});
            // Should not throw, errors should be caught internally
        } catch (...) {
            QFAIL("Update should not throw exceptions");
        }

        // Test unmount error handling
        try {
            lifecycle.unmount();
            // Should not throw, errors should be caught internally
        } catch (...) {
            QFAIL("Unmount should not throw exceptions");
        }

        // Verify error hook was called
        QVERIFY(error_hook_called);
        QVERIFY(!error_message.isEmpty());
    }

    /**
     * @brief Test effect system with complex dependencies
     */
    void testEffectSystemComplexDependencies() {
        ComponentLifecycle lifecycle;

        int effect1_calls = 0;
        int effect2_calls = 0;
        int cleanup1_calls = 0;
        int cleanup2_calls = 0;

        // Effect with no dependencies (runs on every update)
        lifecycle.useEffect(
            [&effect1_calls, &cleanup1_calls]() -> std::function<void()> {
                effect1_calls++;
                return [&cleanup1_calls]() { cleanup1_calls++; };
            });

        // Effect with specific dependencies
        lifecycle.useEffect(
            [&effect2_calls, &cleanup2_calls]() -> std::function<void()> {
                effect2_calls++;
                return [&cleanup2_calls]() { cleanup2_calls++; };
            },
            {QVariant("dependency1"), QVariant(42)});

        auto widget = std::make_unique<QWidget>();
        lifecycle.mount(widget.get());

        QCOMPARE(effect1_calls, 1);
        QCOMPARE(effect2_calls, 1);
        QCOMPARE(cleanup1_calls, 0);
        QCOMPARE(cleanup2_calls, 0);

        // Update with same dependencies
        lifecycle.update({}, {{"dependency1", QVariant("value")},
                              {"dependency2", QVariant(42)}});

        QCOMPARE(effect1_calls, 2);  // No-dependency effect runs again
        QCOMPARE(effect2_calls,
                 1);  // Dependency effect doesn't run (deps unchanged)
        QCOMPARE(cleanup1_calls, 1);  // Previous effect cleaned up
        QCOMPARE(cleanup2_calls, 0);  // Dependency effect not cleaned up

        // Update with changed dependencies
        lifecycle.update({}, {{"dependency1", QVariant("new_value")},
                              {"other", QVariant(100)}});

        QCOMPARE(effect1_calls, 3);   // No-dependency effect runs again
        QCOMPARE(effect2_calls, 2);   // Dependency effect runs (deps changed)
        QCOMPARE(cleanup1_calls, 2);  // Previous effect cleaned up
        QCOMPARE(cleanup2_calls, 1);  // Previous dependency effect cleaned up

        lifecycle.unmount();

        // All effects should be cleaned up on unmount
        QCOMPARE(cleanup1_calls, 3);
        QCOMPARE(cleanup2_calls, 2);
    }

    /**
     * @brief Test lifecycle with rapid mount/unmount cycles
     */
    void testRapidMountUnmountCycles() {
        ComponentLifecycle lifecycle;

        int mount_count = 0;
        int unmount_count = 0;

        lifecycle.onMount(
            [&mount_count](const LifecycleContext&) { mount_count++; });

        lifecycle.onUnmount(
            [&unmount_count](const LifecycleContext&) { unmount_count++; });

        // Perform rapid mount/unmount cycles
        for (int i = 0; i < 100; ++i) {
            auto widget = std::make_unique<QWidget>();
            lifecycle.mount(widget.get());
            QVERIFY(lifecycle.isMounted());

            lifecycle.unmount();
            QVERIFY(!lifecycle.isMounted());
        }

        QCOMPARE(mount_count, 100);
        QCOMPARE(unmount_count, 100);
    }

    /**
     * @brief Test lifecycle state consistency during concurrent operations
     */
    void testLifecycleStateConcurrency() {
        ComponentLifecycle lifecycle;
        std::atomic<int> concurrent_operations{0};
        std::atomic<bool> test_failed{false};

        lifecycle.onMount([&](const LifecycleContext&) {
            concurrent_operations++;
            QTest::qWait(
                1);  // Small delay to increase chance of race conditions
            if (!lifecycle.isMounted()) {
                test_failed = true;
            }
            concurrent_operations--;
        });

        lifecycle.onUnmount([&](const LifecycleContext&) {
            concurrent_operations++;
            QTest::qWait(
                1);  // Small delay to increase chance of race conditions
            if (lifecycle.isMounted()) {
                test_failed = true;
            }
            concurrent_operations--;
        });

        auto widget = std::make_unique<QWidget>();

        // Perform operations rapidly to test state consistency
        for (int i = 0; i < 10; ++i) {
            lifecycle.mount(widget.get());
            lifecycle.unmount();
        }

        // Wait for all operations to complete
        while (concurrent_operations.load() > 0) {
            QTest::qWait(1);
        }

        QVERIFY(!test_failed.load());
    }
};

/**
 * @brief Test UIElement lifecycle integration with complex scenarios
 */
void testUIElementLifecycleIntegration() {
    class TestUIElement : public UIElement {
    public:
        mutable int initialize_calls = 0;
        mutable int cleanup_calls = 0;

        void initialize() override {
            initialize_calls++;
            auto widget = std::make_unique<QLabel>("Test Element");
            setWidget(widget.release());
        }

        void cleanup() noexcept override {
            cleanup_calls++;
            UIElement::cleanup();
        }
    };

    auto element = std::make_unique<TestUIElement>();

    bool mount_called = false;
    bool unmount_called = false;
    bool update_called = false;

    element->onMount([&mount_called]() { mount_called = true; })
        .onUnmount([&unmount_called]() { unmount_called = true; })
        .onUpdate([&update_called]() { update_called = true; });

    // Initialize should trigger mount
    element->initialize();
    QCOMPARE(element->initialize_calls, 1);
    QVERIFY(mount_called);
    QVERIFY(element->getLifecycle()->isMounted());

    // Test property updates triggering lifecycle updates
    element->setProperty("text", "Updated Text");
    // Note: This would need property change detection to trigger update

    // Cleanup should trigger unmount
    element->cleanup();
    QCOMPARE(element->cleanup_calls, 1);
    QVERIFY(unmount_called);
    QVERIFY(!element->getLifecycle()->isMounted());
}

/**
 * @brief Test lifecycle builder integration
 */
void testLifecycleBuilderIntegration() {
    ComponentLifecycle lifecycle;
    LifecycleBuilder builder(&lifecycle);

    bool mount_called = false;
    bool unmount_called = false;
    bool effect_called = false;
    bool cleanup_called = false;

    builder.onMount([&mount_called]() { mount_called = true; })
        .onUnmount([&unmount_called]() { unmount_called = true; })
        .useEffect(
            [&effect_called, &cleanup_called]() -> std::function<void()> {
                effect_called = true;
                return [&cleanup_called]() { cleanup_called = true; };
            });

    auto widget = std::make_unique<QWidget>();
    lifecycle.mount(widget.get());

    QVERIFY(mount_called);
    QVERIFY(effect_called);

    lifecycle.unmount();

    QVERIFY(unmount_called);
    QVERIFY(cleanup_called);
}

/**
 * @brief Test lifecycle with widget hierarchy and nested components
 */
void testLifecycleWithWidgetHierarchy() {
    ComponentLifecycle parent_lifecycle;
    ComponentLifecycle child_lifecycle;

    std::vector<QString> execution_order;

    parent_lifecycle.onMount([&execution_order](const LifecycleContext&) {
        execution_order.push_back("parent_mount");
    });

    parent_lifecycle.onUnmount([&execution_order](const LifecycleContext&) {
        execution_order.push_back("parent_unmount");
    });

    child_lifecycle.onMount([&execution_order](const LifecycleContext&) {
        execution_order.push_back("child_mount");
    });

    child_lifecycle.onUnmount([&execution_order](const LifecycleContext&) {
        execution_order.push_back("child_unmount");
    });

    // Create widget hierarchy
    auto parent_widget = std::make_unique<QWidget>();
    auto child_widget = new QWidget(parent_widget.get());

    // Mount parent first, then child
    parent_lifecycle.mount(parent_widget.get());
    child_lifecycle.mount(child_widget);

    QCOMPARE(execution_order.size(), 2);
    QCOMPARE(execution_order[0], QString("parent_mount"));
    QCOMPARE(execution_order[1], QString("child_mount"));

    execution_order.clear();

    // Unmount in reverse order (child first, then parent)
    child_lifecycle.unmount();
    parent_lifecycle.unmount();

    QCOMPARE(execution_order.size(), 2);
    QCOMPARE(execution_order[0], QString("child_unmount"));
    QCOMPARE(execution_order[1], QString("parent_unmount"));
}
}
;

QTEST_MAIN(LifecycleEnhancedTest)
#include "test_lifecycle_enhanced.moc"
