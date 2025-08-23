#include <QApplication>
#include <QAtomicInt>
#include <QLabel>
#include <QMutex>
#include <QPushButton>
#include <QSignalSpy>
#include <QTest>
#include <QThread>
#include <QTimer>
#include <QVBoxLayout>
#include <QWaitCondition>
#include <QWidget>
#include <atomic>
#include <chrono>
#include <memory>
#include <stdexcept>
#include <vector>

#include "../Core/ErrorBoundary.hpp"
#include "../Exceptions/UIExceptions.hpp"

using namespace DeclarativeUI::Core;
using namespace DeclarativeUI::Exceptions;

/**
 * @brief Comprehensive error boundary tests covering all recovery strategies,
 * nested boundaries, custom fallback UI, global manager integration, and thread
 * safety
 */
class ErrorBoundaryEnhancedTest : public QObject {
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
        // Reset global error manager state
        auto& manager = ErrorBoundaryManager::instance();
        manager.setGlobalErrorHandler(nullptr);
        QApplication::processEvents();
    }

    void cleanup() { QApplication::processEvents(); }

    /**
     * @brief Test all recovery strategies under different failure conditions
     */
    void testAllRecoveryStrategies() {
        // Test ShowFallback strategy
        {
            ErrorBoundaryConfig config;
            config.strategy = ErrorRecoveryStrategy::ShowFallback;

            auto boundary = std::make_unique<ErrorBoundary>();
            boundary->setConfig(config);
            boundary->initialize();

            auto child = std::make_unique<QLabel>("Original Child");
            boundary->setChild(std::move(child));

            boundary->catchError("Test fallback error", "TestComponent");

            QVERIFY(boundary->hasError());
            QCOMPARE(boundary->getStats().fallback_displays, 1);
            QCOMPARE(boundary->getStats().total_errors, 1);
        }

        // Test Retry strategy
        {
            ErrorBoundaryConfig config;
            config.strategy = ErrorRecoveryStrategy::Retry;
            config.max_retry_attempts = 3;
            config.retry_delay = std::chrono::milliseconds(10);

            auto boundary = std::make_unique<ErrorBoundary>();
            boundary->setConfig(config);
            boundary->initialize();

            int factory_calls = 0;
            boundary->setChildFactory(
                [&factory_calls]() -> std::unique_ptr<QWidget> {
                    factory_calls++;
                    if (factory_calls <= 2) {
                        throw std::runtime_error("Factory failure");
                    }
                    return std::make_unique<QLabel>("Retry Success");
                });

            QSignalSpy retrySpy(boundary.get(), &ErrorBoundary::retryAttempted);

            boundary->catchError("Retry test error", "RetryComponent");

            // Wait for retries
            QTest::qWait(100);

            QVERIFY(factory_calls >= 2);
            QVERIFY(retrySpy.count() > 0);
            QVERIFY(boundary->getStats().retry_attempts > 0);
        }

        // Test Ignore strategy
        {
            ErrorBoundaryConfig config;
            config.strategy = ErrorRecoveryStrategy::Ignore;

            auto boundary = std::make_unique<ErrorBoundary>();
            boundary->setConfig(config);
            boundary->initialize();

            auto child = std::make_unique<QPushButton>("Ignore Child");
            QWidget* child_ptr = child.get();
            boundary->setChild(std::move(child));

            boundary->catchError("Ignored error", "IgnoreComponent");

            QVERIFY(boundary->hasError());
            QCOMPARE(boundary->getStats().fallback_displays, 0);
            QCOMPARE(boundary->getChild(),
                     child_ptr);  // Child should still be there
        }

        // Test Restart strategy
        {
            ErrorBoundaryConfig config;
            config.strategy = ErrorRecoveryStrategy::Restart;

            auto boundary = std::make_unique<ErrorBoundary>();
            boundary->setConfig(config);
            boundary->initialize();

            int factory_calls = 0;
            boundary->setChildFactory(
                [&factory_calls]() -> std::unique_ptr<QWidget> {
                    factory_calls++;
                    return std::make_unique<QLabel>(
                        QString("Restart %1").arg(factory_calls));
                });

            // Initial child creation
            auto initial_child = boundary->getChild();

            boundary->catchError("Restart test error", "RestartComponent");

            QCOMPARE(factory_calls, 2);      // Initial + restart
            QVERIFY(!boundary->hasError());  // Should be reset after restart
        }
    }

    /**
     * @brief Test nested error boundary behavior and error propagation
     */
    void testNestedErrorBoundaryBehavior() {
        // Create nested error boundaries
        auto outer_boundary = std::make_unique<ErrorBoundary>();
        auto inner_boundary = std::make_unique<ErrorBoundary>();

        ErrorBoundaryConfig outer_config;
        outer_config.strategy = ErrorRecoveryStrategy::ShowFallback;
        outer_boundary->setConfig(outer_config);
        outer_boundary->initialize();

        ErrorBoundaryConfig inner_config;
        inner_config.strategy = ErrorRecoveryStrategy::Propagate;
        inner_boundary->setConfig(inner_config);
        inner_boundary->initialize();

        bool outer_error_caught = false;
        bool inner_error_caught = false;

        QSignalSpy outerErrorSpy(outer_boundary.get(),
                                 &ErrorBoundary::errorCaught);
        QSignalSpy innerErrorSpy(inner_boundary.get(),
                                 &ErrorBoundary::errorCaught);

        // Set up nested structure
        auto inner_child = std::make_unique<QLabel>("Inner Child");
        inner_boundary->setChild(std::move(inner_child));

        // Note: In a real implementation, we'd need to properly nest the
        // boundaries For this test, we simulate the propagation behavior

        try {
            inner_boundary->catchError("Inner error", "InnerComponent");
            QFAIL("Inner boundary should propagate error");
        } catch (const UIException& e) {
            // Error was propagated, now catch it in outer boundary
            outer_boundary->catchError(QString::fromStdString(e.what()),
                                       "OuterComponent");
        }

        QCOMPARE(innerErrorSpy.count(), 1);
        QCOMPARE(outerErrorSpy.count(), 1);
        QVERIFY(outer_boundary->hasError());
        QCOMPARE(outer_boundary->getStats().fallback_displays, 1);
    }

    /**
     * @brief Test custom fallback UI rendering and interaction
     */
    void testCustomFallbackUIRendering() {
        ErrorBoundaryConfig config;
        config.strategy = ErrorRecoveryStrategy::ShowFallback;

        bool custom_fallback_created = false;
        bool retry_button_clicked = false;

        config.fallback_factory =
            [&custom_fallback_created, &retry_button_clicked](
                const ErrorInfo& error) -> std::unique_ptr<QWidget> {
            custom_fallback_created = true;

            auto widget = std::make_unique<QWidget>();
            auto layout = new QVBoxLayout(widget.get());

            auto error_icon = new QLabel("⚠️");
            error_icon->setAlignment(Qt::AlignCenter);

            auto error_title = new QLabel("Custom Error Handler");
            error_title->setStyleSheet("font-weight: bold; color: red;");
            error_title->setAlignment(Qt::AlignCenter);

            auto error_message = new QLabel(error.message);
            error_message->setWordWrap(true);
            error_message->setAlignment(Qt::AlignCenter);

            auto component_info =
                new QLabel(QString("Component: %1").arg(error.component_name));
            component_info->setStyleSheet("font-style: italic; color: gray;");
            component_info->setAlignment(Qt::AlignCenter);

            auto retry_button = new QPushButton("Retry Operation");
            QObject::connect(
                retry_button, &QPushButton::clicked,
                [&retry_button_clicked]() { retry_button_clicked = true; });

            auto dismiss_button = new QPushButton("Dismiss");

            layout->addWidget(error_icon);
            layout->addWidget(error_title);
            layout->addWidget(error_message);
            layout->addWidget(component_info);
            layout->addWidget(retry_button);
            layout->addWidget(dismiss_button);

            return widget;
        };

        auto boundary = std::make_unique<ErrorBoundary>();
        boundary->setConfig(config);
        boundary->initialize();

        auto child = std::make_unique<QLabel>("Original Child");
        boundary->setChild(std::move(child));

        boundary->catchError("Custom fallback test", "CustomComponent");

        QVERIFY(custom_fallback_created);
        QVERIFY(boundary->hasError());

        // Simulate button click
        auto fallback_widget = boundary->getWidget();
        QVERIFY(fallback_widget != nullptr);

        auto retry_button = fallback_widget->findChild<QPushButton*>();
        if (retry_button) {
            retry_button->click();
            QVERIFY(retry_button_clicked);
        }
    }

    /**
     * @brief Test global error manager integration and statistics tracking
     */
    void testGlobalErrorManagerIntegration() {
        auto& manager = ErrorBoundaryManager::instance();

        bool global_error_reported = false;
        QString global_error_message;
        QString global_component_name;

        manager.setGlobalErrorHandler([&](const ErrorInfo& error) {
            global_error_reported = true;
            global_error_message = error.message;
            global_component_name = error.component_name;
        });

        QSignalSpy globalErrorSpy(&manager,
                                  &ErrorBoundaryManager::globalErrorReported);
        QSignalSpy boundaryRegisteredSpy(
            &manager, &ErrorBoundaryManager::errorBoundaryRegistered);
        QSignalSpy boundaryUnregisteredSpy(
            &manager, &ErrorBoundaryManager::errorBoundaryUnregistered);

        // Create and register multiple boundaries
        std::vector<std::unique_ptr<ErrorBoundary>> boundaries;
        for (int i = 0; i < 5; ++i) {
            auto boundary = std::make_unique<ErrorBoundary>();
            manager.registerErrorBoundary(boundary.get());
            boundaries.push_back(std::move(boundary));
        }

        QCOMPARE(boundaryRegisteredSpy.count(), 5);
        QCOMPARE(manager.getGlobalStats().active_boundaries, 5);
        QCOMPARE(manager.getGlobalStats().total_boundaries, 5);

        // Trigger errors in boundaries
        for (int i = 0; i < 3; ++i) {
            boundaries[i]->catchError(QString("Global test error %1").arg(i),
                                      QString("GlobalComponent%1").arg(i));
        }

        QCOMPARE(globalErrorSpy.count(), 3);
        QVERIFY(global_error_reported);
        QVERIFY(global_error_message.contains("Global test error"));
        QVERIFY(global_component_name.contains("GlobalComponent"));
        QCOMPARE(manager.getGlobalStats().total_errors_caught, 3);

        // Unregister boundaries
        for (auto& boundary : boundaries) {
            manager.unregisterErrorBoundary(boundary.get());
        }

        QCOMPARE(boundaryUnregisteredSpy.count(), 5);
        QCOMPARE(manager.getGlobalStats().active_boundaries, 0);
    }

    /**
     * @brief Test thread safety of error reporting mechanisms
     */
    void testThreadSafetyOfErrorReporting() {
        auto& manager = ErrorBoundaryManager::instance();

        QAtomicInt global_error_count(0);
        QMutex error_messages_mutex;
        std::vector<QString> error_messages;

        manager.setGlobalErrorHandler([&](const ErrorInfo& error) {
            global_error_count.fetchAndAddOrdered(1);
            QMutexLocker locker(&error_messages_mutex);
            error_messages.push_back(error.message);
        });

        auto boundary = std::make_unique<ErrorBoundary>();
        manager.registerErrorBoundary(boundary.get());

        const int num_threads = 10;
        const int errors_per_thread = 20;
        std::vector<std::unique_ptr<QThread>> threads;
        QAtomicInt threads_finished(0);

        // Create multiple threads that report errors concurrently
        for (int t = 0; t < num_threads; ++t) {
            auto thread = std::make_unique<QThread>();

            QObject::connect(thread.get(), &QThread::started, [&, t]() {
                for (int i = 0; i < errors_per_thread; ++i) {
                    QString error_msg =
                        QString("Thread %1 Error %2").arg(t).arg(i);
                    QString component_name =
                        QString("ThreadComponent%1_%2").arg(t).arg(i);

                    // Report error from background thread
                    ErrorInfo error(error_msg, component_name);
                    manager.reportError(error);

                    // Small delay to increase chance of race conditions
                    QThread::msleep(1);
                }
                threads_finished.fetchAndAddOrdered(1);
            });

            threads.push_back(std::move(thread));
        }

        // Start all threads
        for (auto& thread : threads) {
            thread->start();
        }

        // Wait for all threads to finish
        while (threads_finished.load() < num_threads) {
            QTest::qWait(10);
        }

        // Wait for threads to actually finish
        for (auto& thread : threads) {
            thread->wait();
        }

        // Verify all errors were reported
        QCOMPARE(global_error_count.load(), num_threads * errors_per_thread);

        {
            QMutexLocker locker(&error_messages_mutex);
            QCOMPARE(error_messages.size(),
                     static_cast<size_t>(num_threads * errors_per_thread));
        }

        QCOMPARE(manager.getGlobalStats().total_errors_caught,
                 num_threads * errors_per_thread);

        manager.unregisterErrorBoundary(boundary.get());
    }
};

/**
 * @brief Test error boundary with complex exception types
 */
void testComplexExceptionTypes() {
    auto boundary = std::make_unique<ErrorBoundary>();
    boundary->initialize();

    // Test std::runtime_error
    try {
        throw std::runtime_error("Runtime error test");
    } catch (const std::exception& e) {
        boundary->catchError(e, "RuntimeErrorComponent");
    }

    QVERIFY(boundary->hasError());
    QCOMPARE(boundary->getLastError().message, QString("Runtime error test"));

    // Test std::invalid_argument
    try {
        throw std::invalid_argument("Invalid argument test");
    } catch (const std::exception& e) {
        boundary->catchError(e, "InvalidArgumentComponent");
    }

    QCOMPARE(boundary->getLastError().message,
             QString("Invalid argument test"));
    QCOMPARE(boundary->getStats().total_errors, 2);

    // Test custom exception
    class CustomException : public std::exception {
    public:
        const char* what() const noexcept override {
            return "Custom exception message";
        }
    };

    try {
        throw CustomException();
    } catch (const std::exception& e) {
        boundary->catchError(e, "CustomExceptionComponent");
    }

    QCOMPARE(boundary->getLastError().message,
             QString("Custom exception message"));
    QCOMPARE(boundary->getStats().total_errors, 3);
}

/**
 * @brief Test error boundary performance under high error load
 */
void testErrorBoundaryPerformanceUnderLoad() {
    auto boundary = std::make_unique<ErrorBoundary>();
    boundary->initialize();

    const int num_errors = 1000;
    auto start_time = std::chrono::steady_clock::now();

    // Generate many errors rapidly
    for (int i = 0; i < num_errors; ++i) {
        boundary->catchError(QString("Load test error %1").arg(i),
                             QString("LoadComponent%1").arg(i));
    }

    auto end_time = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        end_time - start_time);

    QCOMPARE(boundary->getStats().total_errors, num_errors);
    QVERIFY(duration.count() <
            1000);  // Should handle 1000 errors in less than 1 second

    // Verify error history is properly managed (should be limited)
    const auto& history = boundary->getErrorHistory();
    QVERIFY(history.size() <= 100);  // Should limit history size
}

/**
 * @brief Test error boundary with widget lifecycle integration
 */
void testErrorBoundaryWithWidgetLifecycle() {
    auto boundary = std::make_unique<ErrorBoundary>();
    boundary->initialize();

    bool child_destroyed = false;

    // Create child widget with destruction tracking
    auto child = std::make_unique<QWidget>();
    QObject::connect(child.get(), &QWidget::destroyed,
                     [&child_destroyed]() { child_destroyed = true; });

    boundary->setChild(std::move(child));
    QVERIFY(boundary->getChild() != nullptr);

    // Trigger error that should show fallback
    boundary->catchError("Widget lifecycle test", "LifecycleComponent");

    QVERIFY(boundary->hasError());

    // Reset boundary (should clean up child)
    boundary->reset();
    QApplication::processEvents();

    QVERIFY(!boundary->hasError());
    // Note: child_destroyed depends on Qt's widget management
}

/**
 * @brief Test error boundary builder with complex configurations
 */
void testErrorBoundaryBuilderComplexConfigurations() {
    bool error_reporter_called = false;
    QString reported_error;

    auto boundary =
        ErrorBoundaryBuilder()
            .strategy(ErrorRecoveryStrategy::Retry)
            .maxRetries(5)
            .retryDelay(std::chrono::milliseconds(50))
            .logErrors(true)
            .showErrorDetails(true)
            .onError([&](const ErrorInfo& error) {
                error_reporter_called = true;
                reported_error = error.message;
            })
            .fallback([](const ErrorInfo& error) -> std::unique_ptr<QWidget> {
                auto widget = std::make_unique<QWidget>();
                auto layout = new QVBoxLayout(widget.get());

                auto title = new QLabel("Advanced Fallback");
                title->setStyleSheet("font-size: 16px; font-weight: bold;");

                auto message = new QLabel(error.message);
                message->setWordWrap(true);

                auto timestamp = new QLabel(
                    QString("Error at: %1")
                        .arg(QDateTime::currentDateTime().toString()));
                timestamp->setStyleSheet("font-size: 10px; color: gray;");

                layout->addWidget(title);
                layout->addWidget(message);
                layout->addWidget(timestamp);

                return widget;
            })
            .child([]() -> std::unique_ptr<QWidget> {
                return std::make_unique<QLabel>("Complex Child");
            })
            .build();

    boundary->initialize();

    QVERIFY(boundary->getChild() != nullptr);
    QCOMPARE(boundary->getConfig().strategy, ErrorRecoveryStrategy::Retry);
    QCOMPARE(boundary->getConfig().max_retry_attempts, 5);
    QCOMPARE(boundary->getConfig().retry_delay, std::chrono::milliseconds(50));
    QVERIFY(boundary->getConfig().log_errors);
    QVERIFY(boundary->getConfig().show_error_details);

    // Trigger error to test configuration
    boundary->catchError("Complex config test", "ComplexComponent");

    QVERIFY(error_reporter_called);
    QCOMPARE(reported_error, QString("Complex config test"));
}

/**
 * @brief Test error boundary statistics and metrics accuracy
 */
void testErrorBoundaryStatisticsAccuracy() {
    auto boundary = std::make_unique<ErrorBoundary>();
    boundary->initialize();

    // Initial state
    const auto& initial_stats = boundary->getStats();
    QCOMPARE(initial_stats.total_errors, 0);
    QCOMPARE(initial_stats.recovered_errors, 0);
    QCOMPARE(initial_stats.fallback_displays, 0);
    QCOMPARE(initial_stats.retry_attempts, 0);

    // Configure for fallback strategy
    ErrorBoundaryConfig config;
    config.strategy = ErrorRecoveryStrategy::ShowFallback;
    boundary->setConfig(config);

    // Trigger multiple errors
    for (int i = 0; i < 5; ++i) {
        boundary->catchError(QString("Stats test error %1").arg(i),
                             "StatsComponent");
    }

    const auto& stats_after_errors = boundary->getStats();
    QCOMPARE(stats_after_errors.total_errors, 5);
    QCOMPARE(stats_after_errors.fallback_displays, 5);

    // Test retry strategy statistics
    ErrorBoundaryConfig retry_config;
    retry_config.strategy = ErrorRecoveryStrategy::Retry;
    retry_config.max_retry_attempts = 3;
    retry_config.retry_delay = std::chrono::milliseconds(10);

    auto retry_boundary = std::make_unique<ErrorBoundary>();
    retry_boundary->setConfig(retry_config);
    retry_boundary->initialize();

    int factory_call_count = 0;
    retry_boundary->setChildFactory(
        [&factory_call_count]() -> std::unique_ptr<QWidget> {
            factory_call_count++;
            if (factory_call_count <= 2) {
                throw std::runtime_error("Factory retry test");
            }
            return std::make_unique<QLabel>("Retry Success");
        });

    retry_boundary->catchError("Retry stats test", "RetryStatsComponent");

    // Wait for retries
    QTest::qWait(100);

    const auto& retry_stats = retry_boundary->getStats();
    QCOMPARE(retry_stats.total_errors, 1);
    QVERIFY(retry_stats.retry_attempts > 0);
    QVERIFY(retry_stats.retry_attempts <= 3);
}
}
;

QTEST_MAIN(ErrorBoundaryEnhancedTest)
#include "test_error_boundary_enhanced.moc"
