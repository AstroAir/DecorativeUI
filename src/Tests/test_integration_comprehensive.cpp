#include <QApplication>
#include <QLabel>
#include <QPushButton>
#include <QSignalSpy>
#include <QTest>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <atomic>
#include <chrono>
#include <memory>
#include <stdexcept>
#include <vector>

#include "../Binding/StateManager.hpp"
#include "../Core/ConditionalRendering.hpp"
#include "../Core/ErrorBoundary.hpp"
#include "../Core/Lifecycle.hpp"
#include "../Core/UIElement.hpp"
#include "../Exceptions/UIExceptions.hpp"

using namespace DeclarativeUI::Core;
using namespace DeclarativeUI::Binding;
using namespace DeclarativeUI::Exceptions;

/**
 * @brief Comprehensive integration tests verifying cross-feature functionality
 * between lifecycle hooks, error boundaries, conditional rendering, and state
 * management
 */
class IntegrationComprehensiveTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        if (!QApplication::instance()) {
            int argc = 0;
            char* argv[] = {nullptr};
            new QApplication(argc, argv);
        }
    }

    void cleanupTestCase() { QApplication::processEvents(); }

    void init() {
        // Reset state manager
        auto& state = StateManager::instance();
        QApplication::processEvents();
    }

    void cleanup() { QApplication::processEvents(); }

    /**
     * @brief Test lifecycle hooks working correctly within error boundaries
     */
    void testLifecycleHooksInErrorBoundaries() {
        auto boundary = std::make_unique<ErrorBoundary>();
        boundary->initialize();

        // Create a UIElement with lifecycle hooks that might throw errors
        class TestUIElementWithErrors : public UIElement {
        public:
            mutable bool throw_in_mount = false;
            mutable bool throw_in_unmount = false;
            mutable bool throw_in_update = false;
            mutable int mount_calls = 0;
            mutable int unmount_calls = 0;
            mutable int update_calls = 0;

            void initialize() override {
                auto widget = std::make_unique<QLabel>("Error Test Element");

                onMount([this]() {
                    mount_calls++;
                    if (throw_in_mount) {
                        throw std::runtime_error("Mount hook error");
                    }
                });

                onUnmount([this]() {
                    unmount_calls++;
                    if (throw_in_unmount) {
                        throw std::runtime_error("Unmount hook error");
                    }
                });

                onUpdate([this]() {
                    update_calls++;
                    if (throw_in_update) {
                        throw std::runtime_error("Update hook error");
                    }
                });

                setWidget(widget.release());
            }
        };

        auto element = std::make_unique<TestUIElementWithErrors>();

        bool boundary_error_caught = false;
        QString boundary_error_message;

        QSignalSpy errorSpy(boundary.get(), &ErrorBoundary::errorCaught);

        // Set up error boundary to catch lifecycle errors
        boundary->setChildFactory([&element]() -> std::unique_ptr<QWidget> {
            element->initialize();
            return std::unique_ptr<QWidget>(element->getWidget());
        });

        // Test normal lifecycle operation
        auto child_widget = boundary->getChild();
        QVERIFY(child_widget != nullptr);
        QCOMPARE(element->mount_calls, 1);

        // Test lifecycle error in mount
        element->throw_in_mount = true;
        try {
            // Simulate remounting
            element->cleanup();
            element->initialize();
        } catch (const std::exception& e) {
            boundary->catchError(e, "LifecycleComponent");
        }

        QCOMPARE(errorSpy.count(), 1);
        QVERIFY(boundary->hasError());

        // Test lifecycle error in update
        element->throw_in_mount = false;
        element->throw_in_update = true;

        try {
            // Simulate property update that triggers lifecycle update
            element->setProperty("text", "Updated");
        } catch (const std::exception& e) {
            boundary->catchError(e, "LifecycleUpdateComponent");
        }

        QCOMPARE(errorSpy.count(), 2);

        // Test error boundary recovery
        boundary->reset();
        QVERIFY(!boundary->hasError());
    }

    /**
     * @brief Test conditional rendering components properly mounting/unmounting
     * through lifecycle
     */
    void testConditionalRenderingWithLifecycle() {
        auto& state = StateManager::instance();
        state.setState("show_component", false);

        int mount_count = 0;
        int unmount_count = 0;
        int effect_count = 0;
        int cleanup_count = 0;

        // Create conditional renderer with lifecycle-aware components
        auto renderer =
            ConditionalBuilder()
                .whenStateTrue(
                    "show_component",
                    [&]() -> std::unique_ptr<QWidget> {
                        // Create a widget with lifecycle hooks
                        class LifecycleAwareWidget : public QWidget {
                        public:
                            LifecycleAwareWidget(int& mount_ref,
                                                 int& unmount_ref,
                                                 int& effect_ref,
                                                 int& cleanup_ref)
                                : mount_count_(mount_ref),
                                  unmount_count_(unmount_ref),
                                  effect_count_(effect_ref),
                                  cleanup_count_(cleanup_ref) {
                                // Simulate lifecycle integration
                                // In a real implementation, this would use
                                // ComponentLifecycle
                                connect(this, &QWidget::destroyed,
                                        [&unmount_ref]() { unmount_ref++; });

                                // Simulate mount
                                mount_ref++;

                                // Simulate effect
                                effect_ref++;
                            }

                            ~LifecycleAwareWidget() {
                                // Simulate cleanup
                                cleanup_count_++;
                            }

                        private:
                            int& mount_count_;
                            int& unmount_count_;
                            int& effect_count_;
                            int& cleanup_count_;
                        };

                        return std::make_unique<LifecycleAwareWidget>(
                            mount_count, unmount_count, effect_count,
                            cleanup_count);
                    })
                .otherwise([]() -> std::unique_ptr<QWidget> {
                    return std::make_unique<QLabel>("Component Hidden");
                })
                .reactive(true)
                .build();

        renderer->initialize();
        renderer->bindToState("show_component");

        QSignalSpy evaluatedSpy(renderer.get(),
                                &ConditionalRenderer::conditionsEvaluated);
        QSignalSpy renderingSpy(renderer.get(),
                                &ConditionalRenderer::renderingComplete);

        // Initial state - component should be hidden
        renderer->evaluateConditions();
        renderer->render();

        QCOMPARE(mount_count, 0);
        QCOMPARE(effect_count, 0);
        QCOMPARE(evaluatedSpy.count(), 1);

        // Show component - should trigger mount and effects
        state.setState("show_component", true);
        renderer->evaluateConditions();
        renderer->render();

        QCOMPARE(mount_count, 1);
        QCOMPARE(effect_count, 1);
        QCOMPARE(evaluatedSpy.count(), 2);

        // Hide component - should trigger unmount and cleanup
        state.setState("show_component", false);
        renderer->evaluateConditions();
        renderer->render();
        QApplication::processEvents();  // Allow widget destruction

        QCOMPARE(unmount_count, 1);
        QCOMPARE(cleanup_count, 1);
        QCOMPARE(evaluatedSpy.count(), 3);
    }

    /**
     * @brief Test error boundaries catching errors from conditional rendering
     * logic
     */
    void testErrorBoundariesCatchingConditionalRenderingErrors() {
        auto boundary = std::make_unique<ErrorBoundary>();
        boundary->initialize();

        bool throw_condition_error = false;
        bool throw_widget_error = false;
        bool condition_state = false;

        QSignalSpy errorSpy(boundary.get(), &ErrorBoundary::errorCaught);

        // Create conditional renderer that can throw errors
        auto renderer =
            ConditionalBuilder()
                .when(
                    [&throw_condition_error,
                     &condition_state]() -> ConditionResult {
                        if (throw_condition_error) {
                            throw std::runtime_error(
                                "Condition evaluation error");
                        }
                        return condition_state ? ConditionResult::True
                                               : ConditionResult::False;
                    },
                    [&throw_widget_error]() -> std::unique_ptr<QWidget> {
                        if (throw_widget_error) {
                            throw std::runtime_error("Widget creation error");
                        }
                        return std::make_unique<QLabel>("Conditional Widget");
                    })
                .otherwise([]() -> std::unique_ptr<QWidget> {
                    return std::make_unique<QLabel>("Default Widget");
                })
                .build();

        // Wrap conditional renderer in error boundary
        boundary->setChildFactory([&renderer]() -> std::unique_ptr<QWidget> {
            renderer->initialize();
            renderer->evaluateConditions();
            renderer->render();
            return std::unique_ptr<QWidget>(renderer->getWidget());
        });

        // Test normal operation
        auto child = boundary->getChild();
        QVERIFY(child != nullptr);
        QVERIFY(!boundary->hasError());

        // Test condition evaluation error
        throw_condition_error = true;
        try {
            renderer->evaluateConditions();
            QFAIL("Should have thrown condition error");
        } catch (const std::exception& e) {
            boundary->catchError(e, "ConditionalRenderingComponent");
        }

        QCOMPARE(errorSpy.count(), 1);
        QVERIFY(boundary->hasError());

        // Reset and test widget creation error
        boundary->reset();
        throw_condition_error = false;
        throw_widget_error = true;
        condition_state = true;

        try {
            renderer->evaluateConditions();
            renderer->render();
            QFAIL("Should have thrown widget creation error");
        } catch (const std::exception& e) {
            boundary->catchError(e, "ConditionalWidgetComponent");
        }

        QCOMPARE(errorSpy.count(), 2);
        QVERIFY(boundary->hasError());
    }

    /**
     * @brief Test state management integration across all three systems
     */
    void testStateManagementIntegrationAcrossAllSystems() {
        auto& state = StateManager::instance();
        state.setState("user_authenticated", false);
        state.setState("loading", false);
        state.setState("error_occurred", false);

        // Create integrated system with all three features
        auto boundary = std::make_unique<ErrorBoundary>();
        boundary->initialize();

        int lifecycle_mount_count = 0;
        int lifecycle_unmount_count = 0;

        // Create conditional renderer with lifecycle-aware components
        auto renderer =
            ConditionalBuilder()
                .whenStateTrue(
                    "loading",
                    [&]() -> std::unique_ptr<QWidget> {
                        lifecycle_mount_count++;
                        auto widget = std::make_unique<QLabel>("Loading...");

                        // Simulate lifecycle integration
                        QTimer::singleShot(0, [&lifecycle_unmount_count,
                                               widget = widget.get()]() {
                            connect(widget, &QWidget::destroyed,
                                    [&lifecycle_unmount_count]() {
                                        lifecycle_unmount_count++;
                                    });
                        });

                        return widget;
                    })
                .whenStateTrue(
                    "error_occurred",
                    [&]() -> std::unique_ptr<QWidget> {
                        lifecycle_mount_count++;
                        auto widget =
                            std::make_unique<QLabel>("Error occurred!");
                        widget->setStyleSheet("color: red;");

                        QTimer::singleShot(0, [&lifecycle_unmount_count,
                                               widget = widget.get()]() {
                            connect(widget, &QWidget::destroyed,
                                    [&lifecycle_unmount_count]() {
                                        lifecycle_unmount_count++;
                                    });
                        });

                        return widget;
                    })
                .whenStateTrue(
                    "user_authenticated",
                    [&]() -> std::unique_ptr<QWidget> {
                        lifecycle_mount_count++;
                        auto widget =
                            std::make_unique<QLabel>("Welcome, User!");
                        widget->setStyleSheet("color: green;");

                        QTimer::singleShot(0, [&lifecycle_unmount_count,
                                               widget = widget.get()]() {
                            connect(widget, &QWidget::destroyed,
                                    [&lifecycle_unmount_count]() {
                                        lifecycle_unmount_count++;
                                    });
                        });

                        return widget;
                    })
                .otherwise([&]() -> std::unique_ptr<QWidget> {
                    lifecycle_mount_count++;
                    auto widget = std::make_unique<QLabel>("Please log in");

                    QTimer::singleShot(
                        0, [&lifecycle_unmount_count, widget = widget.get()]() {
                            connect(widget, &QWidget::destroyed,
                                    [&lifecycle_unmount_count]() {
                                        lifecycle_unmount_count++;
                                    });
                        });

                    return widget;
                })
                .reactive(true)
                .build();

        renderer->initialize();
        renderer->bindToStates(
            {"user_authenticated", "loading", "error_occurred"});

        // Wrap in error boundary
        boundary->setChild(std::unique_ptr<QWidget>(renderer->getWidget()));

        QSignalSpy boundaryErrorSpy(boundary.get(),
                                    &ErrorBoundary::errorCaught);
        QSignalSpy rendererEvaluatedSpy(
            renderer.get(), &ConditionalRenderer::conditionsEvaluated);

        // Test state transitions

        // 1. Initial state - should show "Please log in"
        renderer->evaluateConditions();
        renderer->render();
        QCOMPARE(lifecycle_mount_count, 1);
        QCOMPARE(rendererEvaluatedSpy.count(), 1);

        // 2. Start loading
        state.setState("loading", true);
        renderer->evaluateConditions();
        renderer->render();
        QApplication::processEvents();
        QCOMPARE(lifecycle_mount_count, 2);
        QCOMPARE(lifecycle_unmount_count, 1);

        // 3. Loading complete, user authenticated
        state.setState("loading", false);
        state.setState("user_authenticated", true);
        renderer->evaluateConditions();
        renderer->render();
        QApplication::processEvents();
        QCOMPARE(lifecycle_mount_count, 3);
        QCOMPARE(lifecycle_unmount_count, 2);

        // 4. Error occurs
        state.setState("error_occurred", true);
        renderer->evaluateConditions();
        renderer->render();
        QApplication::processEvents();
        QCOMPARE(lifecycle_mount_count, 4);
        QCOMPARE(lifecycle_unmount_count, 3);

        // Verify error boundary didn't catch any errors during normal operation
        QCOMPARE(boundaryErrorSpy.count(), 0);
        QVERIFY(!boundary->hasError());

        // Test error boundary catching state-related errors
        try {
            // Simulate an error during state transition
            throw std::runtime_error("State transition error");
        } catch (const std::exception& e) {
            boundary->catchError(e, "StateTransitionComponent");
        }

        QCOMPARE(boundaryErrorSpy.count(), 1);
        QVERIFY(boundary->hasError());
    }
};

QTEST_MAIN(IntegrationComprehensiveTest)
#include "test_integration_comprehensive.moc"
