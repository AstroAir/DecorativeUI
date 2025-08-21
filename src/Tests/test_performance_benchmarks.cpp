#include <QApplication>
#include <QLabel>
#include <QPushButton>
#include <QTest>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <algorithm>
#include <chrono>
#include <memory>
#include <random>
#include <vector>

#include "../Binding/StateManager.hpp"
#include "../Core/ConditionalRendering.hpp"
#include "../Core/ErrorBoundary.hpp"
#include "../Core/Lifecycle.hpp"
#include "../Core/UIElement.hpp"

using namespace DeclarativeUI::Core;
using namespace DeclarativeUI::Binding;

/**
 * @brief Performance benchmark tests for lifecycle overhead, error boundary
 * impact, and conditional rendering evaluation performance
 */
class PerformanceBenchmarkTest : public QObject {
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

    void init() { QApplication::processEvents(); }

    void cleanup() { QApplication::processEvents(); }

    /**
     * @brief Benchmark lifecycle hook execution overhead
     */
    void benchmarkLifecycleHookExecutionOverhead() {
        const int num_iterations = 10000;
        const int num_hooks_per_type = 10;

        ComponentLifecycle lifecycle;

        // Add multiple hooks of each type
        for (int i = 0; i < num_hooks_per_type; ++i) {
            lifecycle.onMount([](const LifecycleContext&) {
                // Minimal work to simulate real hook
                volatile int x = 42;
                (void)x;
            });

            lifecycle.onUnmount([](const LifecycleContext&) {
                volatile int x = 42;
                (void)x;
            });

            lifecycle.onUpdate([](const LifecycleContext&) {
                volatile int x = 42;
                (void)x;
            });
        }

        auto widget = std::make_unique<QWidget>();

        // Benchmark mount operations
        QBENCHMARK {
            for (int i = 0; i < num_iterations; ++i) {
                lifecycle.mount(widget.get());
                lifecycle.unmount();
            }
        }

        // Verify performance metrics are reasonable
        const auto& metrics = lifecycle.getMetrics();
        qDebug() << "Mount time per operation:"
                 << metrics.mount_time.count() / num_iterations << "ms";
        qDebug() << "Unmount time per operation:"
                 << metrics.unmount_time.count() / num_iterations << "ms";

        // Performance assertions
        QVERIFY(metrics.mount_time.count() <
                1000);  // Total mount time should be reasonable
        QVERIFY(metrics.unmount_time.count() <
                1000);  // Total unmount time should be reasonable
    }

    /**
     * @brief Benchmark lifecycle effect system performance
     */
    void benchmarkLifecycleEffectSystemPerformance() {
        const int num_effects = 1000;
        const int num_updates = 100;

        ComponentLifecycle lifecycle;

        std::vector<int> effect_counters(num_effects, 0);
        std::vector<int> cleanup_counters(num_effects, 0);

        // Add many effects with different dependency patterns
        for (int i = 0; i < num_effects; ++i) {
            lifecycle.useEffect(
                [&effect_counters, i]() -> std::function<void()> {
                    effect_counters[i]++;
                    return [&cleanup_counters, i]() { cleanup_counters[i]++; };
                },
                {QVariant(i % 10)});  // Group effects by dependency
        }

        auto widget = std::make_unique<QWidget>();
        lifecycle.mount(widget.get());

        // Benchmark effect updates
        QBENCHMARK {
            for (int update = 0; update < num_updates; ++update) {
                std::unordered_map<QString, QVariant> props;
                props[QString::number(update % 10)] = QVariant(update);
                lifecycle.update({}, props);
            }
        }

        // Verify effects were executed
        int total_effect_calls =
            std::accumulate(effect_counters.begin(), effect_counters.end(), 0);
        int total_cleanup_calls = std::accumulate(cleanup_counters.begin(),
                                                  cleanup_counters.end(), 0);

        qDebug() << "Total effect calls:" << total_effect_calls;
        qDebug() << "Total cleanup calls:" << total_cleanup_calls;

        QVERIFY(total_effect_calls >
                num_effects);  // Should have run effects multiple times
        QVERIFY(total_cleanup_calls > 0);  // Should have run cleanups

        lifecycle.unmount();
    }

    /**
     * @brief Benchmark error boundary impact on normal operation
     */
    void benchmarkErrorBoundaryImpactOnNormalOperation() {
        const int num_iterations = 1000;

        // Test without error boundary (baseline)
        auto baseline_start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < num_iterations; ++i) {
            auto widget = std::make_unique<QLabel>(QString("Widget %1").arg(i));
            widget->setText(QString("Updated %1").arg(i));
            // Simulate some work
            volatile int x = i * 2;
            (void)x;
        }

        auto baseline_end = std::chrono::high_resolution_clock::now();
        auto baseline_duration =
            std::chrono::duration_cast<std::chrono::microseconds>(
                baseline_end - baseline_start);

        // Test with error boundary
        auto boundary = std::make_unique<ErrorBoundary>();
        boundary->initialize();

        auto boundary_start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < num_iterations; ++i) {
            boundary->setChildFactory([i]() -> std::unique_ptr<QWidget> {
                auto widget = std::make_unique<QLabel>(
                    QString("Boundary Widget %1").arg(i));
                widget->setText(QString("Boundary Updated %1").arg(i));
                // Simulate some work
                volatile int x = i * 2;
                (void)x;
                return widget;
            });

            auto child = boundary->getChild();
            (void)child;
        }

        auto boundary_end = std::chrono::high_resolution_clock::now();
        auto boundary_duration =
            std::chrono::duration_cast<std::chrono::microseconds>(
                boundary_end - boundary_start);

        qDebug() << "Baseline duration:" << baseline_duration.count() << "μs";
        qDebug() << "Error boundary duration:" << boundary_duration.count()
                 << "μs";
        qDebug() << "Overhead:"
                 << (boundary_duration.count() - baseline_duration.count())
                 << "μs";
        qDebug() << "Overhead percentage:"
                 << (100.0 *
                     (boundary_duration.count() - baseline_duration.count()) /
                     baseline_duration.count())
                 << "%";

        // Error boundary overhead should be reasonable (less than 50% overhead)
        QVERIFY(boundary_duration.count() < baseline_duration.count() * 1.5);
    }

    /**
     * @brief Benchmark error boundary error handling performance
     */
    void benchmarkErrorBoundaryErrorHandlingPerformance() {
        const int num_errors = 1000;

        auto boundary = std::make_unique<ErrorBoundary>();
        boundary->initialize();

        // Benchmark error catching and handling
        QBENCHMARK {
            for (int i = 0; i < num_errors; ++i) {
                boundary->catchError(QString("Benchmark error %1").arg(i),
                                     QString("BenchmarkComponent%1").arg(i));
            }
        }

        const auto& stats = boundary->getStats();
        QCOMPARE(stats.total_errors, num_errors);

        // Verify error history management performance
        const auto& history = boundary->getErrorHistory();
        QVERIFY(history.size() <=
                100);  // Should limit history size for performance

        qDebug() << "Errors processed:" << stats.total_errors;
        qDebug() << "History size:" << history.size();
    }

    /**
     * @brief Benchmark conditional rendering evaluation performance with large
     * condition sets
     */
    void benchmarkConditionalRenderingEvaluationPerformance() {
        const int num_conditions = 10000;
        const int num_evaluations = 100;

        auto renderer = std::make_unique<ConditionalRenderer>();
        renderer->initialize();

        std::vector<bool> condition_states(num_conditions, false);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, num_conditions - 1);

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

        // Benchmark worst case (no conditions true)
        auto worst_case_start = std::chrono::high_resolution_clock::now();

        for (int eval = 0; eval < num_evaluations; ++eval) {
            renderer->evaluateConditions();
        }

        auto worst_case_end = std::chrono::high_resolution_clock::now();
        auto worst_case_duration =
            std::chrono::duration_cast<std::chrono::microseconds>(
                worst_case_end - worst_case_start);

        // Benchmark best case (early condition true)
        condition_states[5] = true;  // Early condition

        auto best_case_start = std::chrono::high_resolution_clock::now();

        for (int eval = 0; eval < num_evaluations; ++eval) {
            renderer->evaluateConditions();
        }

        auto best_case_end = std::chrono::high_resolution_clock::now();
        auto best_case_duration =
            std::chrono::duration_cast<std::chrono::microseconds>(
                best_case_end - best_case_start);

        // Benchmark random case
        condition_states[5] = false;
        for (int i = 0; i < num_conditions / 10; ++i) {
            condition_states[dis(gen)] = true;
        }

        auto random_case_start = std::chrono::high_resolution_clock::now();

        for (int eval = 0; eval < num_evaluations; ++eval) {
            renderer->evaluateConditions();
        }

        auto random_case_end = std::chrono::high_resolution_clock::now();
        auto random_case_duration =
            std::chrono::duration_cast<std::chrono::microseconds>(
                random_case_end - random_case_start);

        qDebug() << "Worst case (all conditions false):"
                 << worst_case_duration.count() << "μs";
        qDebug() << "Best case (early condition true):"
                 << best_case_duration.count() << "μs";
        qDebug() << "Random case:" << random_case_duration.count() << "μs";
        qDebug() << "Worst case per evaluation:"
                 << worst_case_duration.count() / num_evaluations << "μs";
        qDebug() << "Best case per evaluation:"
                 << best_case_duration.count() / num_evaluations << "μs";

        // Performance assertions
        QVERIFY(worst_case_duration.count() <
                100000);  // Should complete within 100ms
        QVERIFY(best_case_duration.count() <
                worst_case_duration.count());  // Best case should be faster
        QVERIFY(best_case_duration.count() <
                10000);  // Best case should be very fast
    }

    /**
     * @brief Benchmark conditional rendering with state management integration
     */
    void benchmarkConditionalRenderingWithStateManagement() {
        const int num_state_changes = 1000;

        auto& state = StateManager::instance();
        state.setState("benchmark_condition", false);

        auto renderer = ConditionalBuilder()
                            .whenStateTrue("benchmark_condition",
                                           []() -> std::unique_ptr<QWidget> {
                                               return std::make_unique<QLabel>(
                                                   "State True");
                                           })
                            .whenStateFalse("benchmark_condition",
                                            []() -> std::unique_ptr<QWidget> {
                                                return std::make_unique<QLabel>(
                                                    "State False");
                                            })
                            .reactive(true)
                            .build();

        renderer->initialize();
        renderer->bindToState("benchmark_condition");

        // Benchmark state-based condition evaluation
        QBENCHMARK {
            for (int i = 0; i < num_state_changes; ++i) {
                state.setState("benchmark_condition", i % 2 == 0);
                renderer->evaluateConditions();
                renderer->render();
            }
        }

        qDebug() << "State changes processed:" << num_state_changes;
    }

    /**
     * @brief Benchmark memory usage and allocation patterns
     */
    void benchmarkMemoryUsageAndAllocationPatterns() {
        const int num_components = 1000;

        // Benchmark lifecycle memory usage
        std::vector<std::unique_ptr<ComponentLifecycle>> lifecycles;
        lifecycles.reserve(num_components);

        for (int i = 0; i < num_components; ++i) {
            auto lifecycle = std::make_unique<ComponentLifecycle>();

            // Add hooks to simulate real usage
            lifecycle->onMount([](const LifecycleContext&) {});
            lifecycle->onUnmount([](const LifecycleContext&) {});
            lifecycle->useEffect(
                []() -> std::function<void()> { return []() {}; });

            lifecycles.push_back(std::move(lifecycle));
        }

        // Benchmark error boundary memory usage
        std::vector<std::unique_ptr<ErrorBoundary>> boundaries;
        boundaries.reserve(num_components);

        for (int i = 0; i < num_components; ++i) {
            auto boundary = std::make_unique<ErrorBoundary>();
            boundary->initialize();
            boundaries.push_back(std::move(boundary));
        }

        // Benchmark conditional renderer memory usage
        std::vector<std::unique_ptr<ConditionalRenderer>> renderers;
        renderers.reserve(num_components);

        for (int i = 0; i < num_components; ++i) {
            auto renderer =
                ConditionalBuilder()
                    .when([]() { return ConditionResult::True; },
                          []() -> std::unique_ptr<QWidget> {
                              return std::make_unique<QLabel>("Memory Test");
                          })
                    .build();

            renderer->initialize();
            renderers.push_back(std::move(renderer));
        }

        qDebug() << "Created" << num_components << "of each component type";
        qDebug() << "Memory test completed - check memory usage externally";

        // Cleanup
        lifecycles.clear();
        boundaries.clear();
        renderers.clear();

        QApplication::processEvents();
    }
};

QTEST_MAIN(PerformanceBenchmarkTest)
#include "test_performance_benchmarks.moc"
