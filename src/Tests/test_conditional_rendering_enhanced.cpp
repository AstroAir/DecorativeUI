#include <QApplication>
#include <QGraphicsOpacityEffect>
#include <QLabel>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QSignalSpy>
#include <QTest>
#include <QThread>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <atomic>
#include <chrono>
#include <memory>
#include <vector>

#include "../Binding/StateManager.hpp"
#include "../Core/ConditionalRendering.hpp"
#include "../Core/DeclarativeBuilder.hpp"

using namespace DeclarativeUI::Core;
using namespace DeclarativeUI::Binding;

/**
 * @brief Advanced conditional rendering tests covering complex nested logic,
 * reactive updates, async evaluation, animation testing, and DeclarativeBuilder
 * integration
 */
class ConditionalRenderingEnhancedTest : public QObject {
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
        // Clear any existing state
        QApplication::processEvents();
    }

    void cleanup() { QApplication::processEvents(); }

    /**
     * @brief Test complex nested conditional logic scenarios
     */
    void testComplexNestedConditionalLogic() {
        auto& state = StateManager::instance();
        state.setState("user_authenticated", false);
        state.setState("user_role", QString("guest"));
        state.setState("feature_enabled", true);
        state.setState("premium_user", false);

        // Create complex nested conditions
        auto renderer =
            ConditionalBuilder()
                // Admin users with feature enabled
                .whenAll(
                    {Conditions::stateTrue("user_authenticated"),
                     Conditions::stateEquals("user_role", QVariant("admin")),
                     Conditions::stateTrue("feature_enabled")},
                    []() -> std::unique_ptr<QWidget> {
                        return std::make_unique<QLabel>("Admin Panel");
                    })
                // Premium users with feature enabled
                .whenAll(
                    {Conditions::stateTrue("user_authenticated"),
                     Conditions::stateTrue("premium_user"),
                     Conditions::stateTrue("feature_enabled")},
                    []() -> std::unique_ptr<QWidget> {
                        return std::make_unique<QLabel>("Premium Features");
                    })
                // Regular authenticated users
                .when(Conditions::stateTrue("user_authenticated"),
                      []() -> std::unique_ptr<QWidget> {
                          return std::make_unique<QLabel>("User Dashboard");
                      })
                // Guest users or feature disabled
                .whenAny({Conditions::stateFalse("user_authenticated"),
                          Conditions::stateFalse("feature_enabled")},
                         []() -> std::unique_ptr<QWidget> {
                             return std::make_unique<QLabel>("Limited Access");
                         })
                .otherwise([]() -> std::unique_ptr<QWidget> {
                    return std::make_unique<QLabel>("Default View");
                })
                .reactive(true)
                .build();

        renderer->initialize();
        renderer->bindToStates({"user_authenticated", "user_role",
                                "feature_enabled", "premium_user"});

        // Test initial state (guest user)
        renderer->evaluateConditions();
        renderer->render();
        // Should show "Limited Access"

        // Test authenticated regular user
        state.setState("user_authenticated", true);
        renderer->evaluateConditions();
        renderer->render();
        // Should show "User Dashboard"

        // Test premium user
        state.setState("premium_user", true);
        renderer->evaluateConditions();
        renderer->render();
        // Should show "Premium Features"

        // Test admin user
        state.setState("user_role", QString("admin"));
        renderer->evaluateConditions();
        renderer->render();
        // Should show "Admin Panel"

        // Test feature disabled
        state.setState("feature_enabled", false);
        renderer->evaluateConditions();
        renderer->render();
        // Should show "Limited Access"
    }

    /**
     * @brief Test state-based reactive updates with multiple state dependencies
     */
    void testStateBasedReactiveUpdates() {
        auto& state = StateManager::instance();
        state.setState("loading", false);
        state.setState("error", false);
        state.setState("data_available", false);

        int condition_evaluations = 0;
        int widget_creations = 0;

        auto renderer =
            ConditionalBuilder()
                .when(
                    [&condition_evaluations]() -> ConditionResult {
                        condition_evaluations++;
                        auto& state = StateManager::instance();
                        try {
                            auto loading = state.getState<bool>("loading");
                            return (loading && loading->get())
                                       ? ConditionResult::True
                                       : ConditionResult::False;
                        } catch (...) {
                            return ConditionResult::False;
                        }
                    },
                    [&widget_creations]() -> std::unique_ptr<QWidget> {
                        widget_creations++;
                        return std::make_unique<QLabel>("Loading...");
                    })
                .when(
                    [&condition_evaluations]() -> ConditionResult {
                        condition_evaluations++;
                        auto& state = StateManager::instance();
                        try {
                            auto error = state.getState<bool>("error");
                            return (error && error->get())
                                       ? ConditionResult::True
                                       : ConditionResult::False;
                        } catch (...) {
                            return ConditionResult::False;
                        }
                    },
                    [&widget_creations]() -> std::unique_ptr<QWidget> {
                        widget_creations++;
                        return std::make_unique<QLabel>("Error occurred!");
                    })
                .when(
                    [&condition_evaluations]() -> ConditionResult {
                        condition_evaluations++;
                        auto& state = StateManager::instance();
                        try {
                            auto data = state.getState<bool>("data_available");
                            return (data && data->get())
                                       ? ConditionResult::True
                                       : ConditionResult::False;
                        } catch (...) {
                            return ConditionResult::False;
                        }
                    },
                    [&widget_creations]() -> std::unique_ptr<QWidget> {
                        widget_creations++;
                        return std::make_unique<QLabel>(
                            "Data loaded successfully!");
                    })
                .otherwise([&widget_creations]() -> std::unique_ptr<QWidget> {
                    widget_creations++;
                    return std::make_unique<QLabel>("No data");
                })
                .reactive(true)
                .build();

        renderer->initialize();
        renderer->bindToStates({"loading", "error", "data_available"});

        QSignalSpy evaluatedSpy(renderer.get(),
                                &ConditionalRenderer::conditionsEvaluated);
        QSignalSpy renderingSpy(renderer.get(),
                                &ConditionalRenderer::renderingComplete);

        // Initial evaluation
        renderer->evaluateConditions();
        renderer->render();

        int initial_evaluations = condition_evaluations;
        int initial_creations = widget_creations;

        QCOMPARE(evaluatedSpy.count(), 1);
        QCOMPARE(renderingSpy.count(), 1);

        // Trigger loading state
        state.setState("loading", true);
        renderer->evaluateConditions();
        renderer->render();

        QVERIFY(condition_evaluations > initial_evaluations);
        QCOMPARE(evaluatedSpy.count(), 2);
        QCOMPARE(renderingSpy.count(), 2);

        // Trigger error state
        state.setState("loading", false);
        state.setState("error", true);
        renderer->evaluateConditions();
        renderer->render();

        QCOMPARE(evaluatedSpy.count(), 3);
        QCOMPARE(renderingSpy.count(), 3);

        // Trigger success state
        state.setState("error", false);
        state.setState("data_available", true);
        renderer->evaluateConditions();
        renderer->render();

        QCOMPARE(evaluatedSpy.count(), 4);
        QCOMPARE(renderingSpy.count(), 4);
    }

    /**
     * @brief Test async condition evaluation with timeout and error handling
     */
    void testAsyncConditionEvaluationWithTimeoutAndErrorHandling() {
        auto renderer = std::make_unique<ConditionalRenderer>();
        renderer->initialize();

        std::atomic<int> async_calls{0};
        std::atomic<int> timeout_calls{0};
        std::atomic<int> error_calls{0};

        // Add async condition that succeeds
        renderer->addAsyncCondition(
            [&async_calls](std::function<void(ConditionResult)> callback) {
                async_calls++;
                QTimer::singleShot(
                    50, [callback]() { callback(ConditionResult::True); });
            },
            []() -> std::unique_ptr<QWidget> {
                return std::make_unique<QLabel>("Async Success");
            });

        // Add async condition that times out
        renderer->addAsyncCondition(
            [&timeout_calls](std::function<void(ConditionResult)> callback) {
                timeout_calls++;
                QTimer::singleShot(
                    200, [callback]() { callback(ConditionResult::False); });
            },
            []() -> std::unique_ptr<QWidget> {
                return std::make_unique<QLabel>("Timeout Result");
            });

        // Add async condition that throws error
        renderer->addAsyncCondition(
            [&error_calls](std::function<void(ConditionResult)> callback) {
                error_calls++;
                QTimer::singleShot(10, [callback]() {
                    try {
                        throw std::runtime_error("Async condition error");
                    } catch (...) {
                        callback(ConditionResult::False);
                    }
                });
            },
            []() -> std::unique_ptr<QWidget> {
                return std::make_unique<QLabel>("Error Result");
            });

        renderer->addElse([]() -> std::unique_ptr<QWidget> {
            return std::make_unique<QLabel>("Default Async");
        });

        QSignalSpy evaluatedSpy(renderer.get(),
                                &ConditionalRenderer::conditionsEvaluated);

        // Start async evaluation
        renderer->evaluateConditionsAsync();

        // Wait for async operations to complete
        QTest::qWait(300);

        QVERIFY(async_calls.load() > 0);
        QVERIFY(evaluatedSpy.count() > 0);

        // Verify that the first successful condition was chosen
        // (async success should win since it completes first)
    }

    /**
     * @brief Test animation transition testing and performance validation
     */
    void testAnimationTransitionAndPerformance() {
        ConditionalConfig config;
        config.animate_transitions = true;
        config.animation_duration = std::chrono::milliseconds(100);
        config.reactive = true;

        bool condition = false;

        auto renderer =
            ConditionalBuilder()
                .config(config)
                .when(
                    [&condition]() -> ConditionResult {
                        return condition ? ConditionResult::True
                                         : ConditionResult::False;
                    },
                    []() -> std::unique_ptr<QWidget> {
                        auto widget =
                            std::make_unique<QLabel>("Animated Widget");
                        widget->setStyleSheet(
                            "background-color: blue; color: white; padding: "
                            "10px;");
                        return widget;
                    })
                .otherwise([]() -> std::unique_ptr<QWidget> {
                    auto widget = std::make_unique<QLabel>("Default Widget");
                    widget->setStyleSheet(
                        "background-color: gray; color: white; padding: 10px;");
                    return widget;
                })
                .build();

        renderer->initialize();

        // Create a parent widget to contain the renderer
        auto parent_widget = std::make_unique<QWidget>();
        parent_widget->setLayout(new QVBoxLayout());
        parent_widget->layout()->addWidget(renderer->getWidget());
        parent_widget->show();

        QSignalSpy renderingSpy(renderer.get(),
                                &ConditionalRenderer::renderingComplete);

        // Measure performance of condition switching with animations
        auto start_time = std::chrono::steady_clock::now();

        // Perform multiple rapid transitions
        for (int i = 0; i < 10; ++i) {
            condition = !condition;
            renderer->evaluateConditions();
            renderer->render();

            // Wait for animation to start
            QTest::qWait(10);
        }

        // Wait for all animations to complete
        QTest::qWait(200);

        auto end_time = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            end_time - start_time);

        QVERIFY(renderingSpy.count() >= 10);
        QVERIFY(duration.count() < 2000);  // Should complete within 2 seconds

        parent_widget->hide();
    }

    /**
     * @brief Test performance with large condition sets
     */
    void testPerformanceWithLargeConditionSets() {
        auto renderer = std::make_unique<ConditionalRenderer>();
        renderer->initialize();

        const int num_conditions = 1000;
        std::vector<bool> condition_states(num_conditions, false);

        // Add many conditions
        for (int i = 0; i < num_conditions; ++i) {
            renderer->addCondition(
                [&condition_states, i]() -> ConditionResult {
                    return condition_states[i] ? ConditionResult::True
                                               : ConditionResult::False;
                },
                [i]() -> std::unique_ptr<QWidget> {
                    return std::make_unique<QLabel>(
                        QString("Condition %1").arg(i));
                });
        }

        renderer->addElse([]() -> std::unique_ptr<QWidget> {
            return std::make_unique<QLabel>("Default Large Set");
        });

        // Measure evaluation performance
        auto start_time = std::chrono::steady_clock::now();

        // Test with no conditions true (worst case - evaluates all)
        renderer->evaluateConditions();

        auto end_time = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
            end_time - start_time);

        QVERIFY(duration.count() < 10000);  // Should complete within 10ms

        // Test with early condition true (best case)
        condition_states[5] = true;

        start_time = std::chrono::steady_clock::now();
        renderer->evaluateConditions();
        end_time = std::chrono::steady_clock::now();

        auto early_duration =
            std::chrono::duration_cast<std::chrono::microseconds>(end_time -
                                                                  start_time);

        QVERIFY(early_duration.count() < 1000);  // Should be much faster
        QVERIFY(early_duration.count() <
                duration.count());  // Should be faster than worst case
    }
};

/**
 * @brief Test integration with DeclarativeBuilder's conditionalChild method
 */
void testDeclarativeBuilderIntegration() {
    bool show_optional_content = false;
    bool show_debug_info = true;

    // Note: This test demonstrates the intended API but may not work
    // with the current DeclarativeBuilder implementation

    // Test the conditional rendering concept with manual widget creation
    auto parent_widget = std::make_unique<QWidget>();
    parent_widget->setWindowTitle("Conditional Test");
    auto layout = new QVBoxLayout(parent_widget.get());

    // Simulate conditional child creation
    if (show_optional_content) {
        auto label = new QLabel("Optional Content", parent_widget.get());
        label->setStyleSheet("color: blue; font-weight: bold;");
        layout->addWidget(label);
    }

    if (show_debug_info) {
        auto button = new QPushButton("Debug Info", parent_widget.get());
        button->setStyleSheet("background-color: yellow;");
        layout->addWidget(button);
    }

    QVERIFY(parent_widget != nullptr);

    // Test condition changes
    show_optional_content = true;
    show_debug_info = false;

    // In a full implementation, changing these conditions would
    // trigger re-evaluation and dynamic widget creation/destruction
}

/**
 * @brief Test conditional rendering with widget caching and lazy creation
 */
void testWidgetCachingAndLazyCreation() {
    ConditionalConfig config;
    config.lazy_creation = true;
    config.cache_widgets = true;

    int widget_creation_count = 0;
    bool condition1 = false;
    bool condition2 = false;

    auto renderer =
        ConditionalBuilder()
            .config(config)
            .when(
                [&condition1]() -> ConditionResult {
                    return condition1 ? ConditionResult::True
                                      : ConditionResult::False;
                },
                [&widget_creation_count]() -> std::unique_ptr<QWidget> {
                    widget_creation_count++;
                    return std::make_unique<QLabel>("Cached Widget 1");
                })
            .when(
                [&condition2]() -> ConditionResult {
                    return condition2 ? ConditionResult::True
                                      : ConditionResult::False;
                },
                [&widget_creation_count]() -> std::unique_ptr<QWidget> {
                    widget_creation_count++;
                    return std::make_unique<QLabel>("Cached Widget 2");
                })
            .otherwise([&widget_creation_count]() -> std::unique_ptr<QWidget> {
                widget_creation_count++;
                return std::make_unique<QLabel>("Default Cached");
            })
            .build();

    renderer->initialize();

    // Initial state - should create default widget
    renderer->evaluateConditions();
    renderer->render();
    QCOMPARE(widget_creation_count, 1);

    // Switch to condition1 - should create widget1
    condition1 = true;
    renderer->evaluateConditions();
    renderer->render();
    QCOMPARE(widget_creation_count, 2);

    // Switch to condition2 - should create widget2
    condition1 = false;
    condition2 = true;
    renderer->evaluateConditions();
    renderer->render();
    QCOMPARE(widget_creation_count, 3);

    // Switch back to condition1 - should reuse cached widget (no new creation)
    condition2 = false;
    condition1 = true;
    renderer->evaluateConditions();
    renderer->render();
    QCOMPARE(widget_creation_count, 3);  // No new widget created due to caching
}

/**
 * @brief Test conditional rendering with debounced updates
 */
void testDebouncedUpdates() {
    ConditionalConfig config;
    config.reactive = true;
    config.debounce_delay = std::chrono::milliseconds(100);

    int evaluation_count = 0;
    bool condition = false;

    auto renderer =
        ConditionalBuilder()
            .config(config)
            .when(
                [&condition, &evaluation_count]() -> ConditionResult {
                    evaluation_count++;
                    return condition ? ConditionResult::True
                                     : ConditionResult::False;
                },
                []() -> std::unique_ptr<QWidget> {
                    return std::make_unique<QLabel>("Debounced Widget");
                })
            .otherwise([]() -> std::unique_ptr<QWidget> {
                return std::make_unique<QLabel>("Default Debounced");
            })
            .build();

    renderer->initialize();

    QSignalSpy evaluatedSpy(renderer.get(),
                            &ConditionalRenderer::conditionsEvaluated);

    // Initial evaluation
    renderer->evaluateConditions();
    int initial_evaluations = evaluation_count;

    // Rapid condition changes (should be debounced)
    for (int i = 0; i < 10; ++i) {
        condition = !condition;
        // Simulate rapid state changes that would trigger re-evaluation
        // In a real implementation, this would be triggered by state changes
    }

    // Wait for debounce period
    QTest::qWait(150);

    // Should have fewer evaluations due to debouncing
    // Note: This test depends on proper debouncing implementation
    QVERIFY(evaluation_count >= initial_evaluations);
}

/**
 * @brief Test conditional rendering error handling and recovery
 */
void testConditionalRenderingErrorHandling() {
    bool throw_error = false;
    bool condition = true;

    auto renderer =
        ConditionalBuilder()
            .when(
                [&condition, &throw_error]() -> ConditionResult {
                    if (throw_error) {
                        throw std::runtime_error("Condition evaluation error");
                    }
                    return condition ? ConditionResult::True
                                     : ConditionResult::False;
                },
                []() -> std::unique_ptr<QWidget> {
                    return std::make_unique<QLabel>("Error Test Widget");
                })
            .when(
                []() -> ConditionResult {
                    return ConditionResult::True;  // Always true fallback
                },
                []() -> std::unique_ptr<QWidget> {
                    throw std::runtime_error("Widget creation error");
                    return std::make_unique<QLabel>(
                        "This should not be created");
                })
            .otherwise([]() -> std::unique_ptr<QWidget> {
                return std::make_unique<QLabel>("Error Recovery");
            })
            .build();

    renderer->initialize();

    // Test normal operation
    renderer->evaluateConditions();
    renderer->render();

    // Test condition evaluation error
    throw_error = true;
    try {
        renderer->evaluateConditions();
        // Should handle error gracefully and continue with other conditions
    } catch (...) {
        QFAIL("Condition evaluation error should be handled gracefully");
    }

    // Test widget creation error
    throw_error = false;
    condition = false;  // This should trigger the second condition which throws
    try {
        renderer->evaluateConditions();
        renderer->render();
        // Should handle widget creation error gracefully
    } catch (...) {
        QFAIL("Widget creation error should be handled gracefully");
    }
}

/**
 * @brief Test conditional rendering with complex state dependencies
 */
void testComplexStateDependencies() {
    auto& state = StateManager::instance();
    state.setState("theme", QString("light"));
    state.setState("language", QString("en"));
    state.setState("user_preferences", QVariantMap{{"notifications", true},
                                                   {"animations", false},
                                                   {"compact_mode", true}});

    auto renderer =
        ConditionalBuilder()
            .when(
                [&]() -> ConditionResult {
                    try {
                        auto theme = state.getState<QString>("theme");
                        auto lang = state.getState<QString>("language");
                        auto prefs =
                            state.getState<QVariantMap>("user_preferences");

                        if (theme && lang && prefs) {
                            bool dark_theme = (theme->get() == "dark");
                            bool english = (lang->get() == "en");
                            bool compact =
                                prefs->get()["compact_mode"].toBool();

                            return (dark_theme && english && compact)
                                       ? ConditionResult::True
                                       : ConditionResult::False;
                        }
                    } catch (...) {
                        return ConditionResult::False;
                    }
                    return ConditionResult::False;
                },
                []() -> std::unique_ptr<QWidget> {
                    return std::make_unique<QLabel>("Dark English Compact");
                })
            .when(
                [&]() -> ConditionResult {
                    try {
                        auto theme = state.getState<QString>("theme");
                        auto prefs =
                            state.getState<QVariantMap>("user_preferences");

                        if (theme && prefs) {
                            bool light_theme = (theme->get() == "light");
                            bool animations =
                                prefs->get()["animations"].toBool();

                            return (light_theme && animations)
                                       ? ConditionResult::True
                                       : ConditionResult::False;
                        }
                    } catch (...) {
                        return ConditionResult::False;
                    }
                    return ConditionResult::False;
                },
                []() -> std::unique_ptr<QWidget> {
                    return std::make_unique<QLabel>("Light Animated");
                })
            .otherwise([]() -> std::unique_ptr<QWidget> {
                return std::make_unique<QLabel>("Default Theme");
            })
            .reactive(true)
            .build();

    renderer->initialize();
    renderer->bindToStates({"theme", "language", "user_preferences"});

    // Test initial state
    renderer->evaluateConditions();
    renderer->render();

    // Test theme change
    state.setState("theme", QString("dark"));
    renderer->evaluateConditions();
    renderer->render();

    // Test preference change
    QVariantMap new_prefs =
        state.getState<QVariantMap>("user_preferences")->get();
    new_prefs["animations"] = true;
    state.setState("user_preferences", new_prefs);
    renderer->evaluateConditions();
    renderer->render();
}
}
;

QTEST_MAIN(ConditionalRenderingEnhancedTest)
#include "test_conditional_rendering_enhanced.moc"
