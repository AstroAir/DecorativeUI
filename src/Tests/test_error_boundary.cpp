#include <QApplication>
#include <QLabel>
#include <QPushButton>
#include <QSignalSpy>
#include <QTest>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <memory>
#include <stdexcept>

#include "../Core/ErrorBoundary.hpp"
#include "../Exceptions/UIExceptions.hpp"

using namespace DeclarativeUI::Core;
using namespace DeclarativeUI::Exceptions;

class ErrorBoundaryTest : public QObject {
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

    void testErrorBoundaryBasic() {
        auto boundary = std::make_unique<ErrorBoundary>();

        QVERIFY(!boundary->hasError());
        QCOMPARE(boundary->getStats().total_errors, 0);

        // Test error catching
        QString test_error = "Test error message";
        boundary->catchError(test_error, "TestComponent");

        QVERIFY(boundary->hasError());
        QCOMPARE(boundary->getLastError().message, test_error);
        QCOMPARE(boundary->getLastError().component_name,
                 QString("TestComponent"));
        QCOMPARE(boundary->getStats().total_errors, 1);
    }

    void testErrorBoundaryWithChild() {
        auto boundary = std::make_unique<ErrorBoundary>();
        boundary->initialize();

        // Create a child widget
        auto child = std::make_unique<QLabel>("Test Child");
        QLabel* child_ptr = child.get();
        boundary->setChild(std::move(child));

        QCOMPARE(boundary->getChild(), child_ptr);
        QVERIFY(!boundary->hasError());

        // Trigger an error
        boundary->catchError("Child error", "ChildComponent");

        QVERIFY(boundary->hasError());
        QCOMPARE(boundary->getStats().total_errors, 1);
    }

    void testErrorBoundaryFallback() {
        ErrorBoundaryConfig config;
        config.strategy = ErrorRecoveryStrategy::ShowFallback;

        auto boundary = std::make_unique<ErrorBoundary>();
        boundary->setConfig(config);
        boundary->initialize();

        // Create child
        auto child = std::make_unique<QPushButton>("Click Me");
        boundary->setChild(std::move(child));

        // Trigger error
        boundary->catchError("Fallback test error", "TestComponent");

        QVERIFY(boundary->hasError());
        QCOMPARE(boundary->getStats().fallback_displays, 1);
    }

    void testErrorBoundaryRetry() {
        ErrorBoundaryConfig config;
        config.strategy = ErrorRecoveryStrategy::Retry;
        config.max_retry_attempts = 2;
        config.retry_delay = std::chrono::milliseconds(10);

        auto boundary = std::make_unique<ErrorBoundary>();
        boundary->setConfig(config);
        boundary->initialize();

        bool factory_called = false;
        boundary->setChildFactory(
            [&factory_called]() -> std::unique_ptr<QWidget> {
                factory_called = true;
                return std::make_unique<QLabel>("Retry Child");
            });

        // Trigger error
        boundary->catchError("Retry test error", "TestComponent");

        QVERIFY(boundary->hasError());

        // Wait for retry
        QTest::qWait(50);

        QVERIFY(factory_called);
        QVERIFY(boundary->getStats().retry_attempts > 0);
    }

    void testErrorBoundaryBuilder() {
        bool error_reported = false;
        QString reported_message;

        auto boundary = ErrorBoundaryBuilder()
                            .strategy(ErrorRecoveryStrategy::ShowFallback)
                            .maxRetries(3)
                            .retryDelay(std::chrono::milliseconds(100))
                            .logErrors(true)
                            .simpleFallback("Something went wrong!")
                            .onError([&](const ErrorInfo& error) {
                                error_reported = true;
                                reported_message = error.message;
                            })
                            .child([]() -> std::unique_ptr<QWidget> {
                                return std::make_unique<QLabel>("Test Child");
                            })
                            .build();

        boundary->initialize();

        QVERIFY(boundary->getChild() != nullptr);
        QCOMPARE(boundary->getConfig().strategy,
                 ErrorRecoveryStrategy::ShowFallback);
        QCOMPARE(boundary->getConfig().max_retry_attempts, 3);

        // Test error reporting
        boundary->catchError("Builder test error", "BuilderComponent");

        QVERIFY(error_reported);
        QCOMPARE(reported_message, QString("Builder test error"));
    }

    void testErrorBoundarySignals() {
        auto boundary = std::make_unique<ErrorBoundary>();
        boundary->initialize();

        QSignalSpy errorSpy(boundary.get(), &ErrorBoundary::errorCaught);
        QSignalSpy fallbackSpy(boundary.get(), &ErrorBoundary::fallbackShown);
        QSignalSpy retrySpy(boundary.get(), &ErrorBoundary::retryAttempted);

        // Test error signal
        boundary->catchError("Signal test error", "SignalComponent");
        QCOMPARE(errorSpy.count(), 1);

        // Test fallback signal
        boundary->showFallback();
        QCOMPARE(fallbackSpy.count(), 1);

        // Test retry signal (if retry is configured)
        ErrorBoundaryConfig config;
        config.strategy = ErrorRecoveryStrategy::Retry;
        config.max_retry_attempts = 1;
        boundary->setConfig(config);

        boundary->setChildFactory([]() -> std::unique_ptr<QWidget> {
            return std::make_unique<QLabel>("Retry Test");
        });

        boundary->retry();
        QCOMPARE(retrySpy.count(), 1);
    }

    void testErrorBoundaryManager() {
        auto& manager = ErrorBoundaryManager::instance();

        // Test global error handler
        bool global_error_reported = false;
        QString global_error_message;

        manager.setGlobalErrorHandler([&](const ErrorInfo& error) {
            global_error_reported = true;
            global_error_message = error.message;
        });

        // Create boundary and register it
        auto boundary = std::make_unique<ErrorBoundary>();
        manager.registerErrorBoundary(boundary.get());

        const auto& stats = manager.getGlobalStats();
        QVERIFY(stats.active_boundaries > 0);

        // Trigger error (should be reported globally)
        boundary->catchError("Global test error", "GlobalComponent");

        QVERIFY(global_error_reported);
        QCOMPARE(global_error_message, QString("Global test error"));

        // Unregister boundary
        manager.unregisterErrorBoundary(boundary.get());
    }

    void testErrorBoundaryConvenienceFunctions() {
        // Test ErrorBoundaries::create
        auto child = std::make_unique<QLabel>("Test Child");
        auto boundary1 = ErrorBoundaries::create(std::move(child));

        QVERIFY(boundary1 != nullptr);
        QVERIFY(boundary1->getChild() != nullptr);

        // Test ErrorBoundaries::createWithFallback
        auto child2 = std::make_unique<QPushButton>("Test Button");
        auto boundary2 = ErrorBoundaries::createWithFallback(
            std::move(child2), "Custom fallback message");

        QVERIFY(boundary2 != nullptr);
        QVERIFY(boundary2->getChild() != nullptr);

        // Test error with custom fallback
        boundary2->initialize();
        boundary2->catchError("Fallback test", "TestComponent");

        QVERIFY(boundary2->hasError());
    }

    void testErrorBoundaryRecoveryStrategies() {
        // Test Ignore strategy
        ErrorBoundaryConfig ignoreConfig;
        ignoreConfig.strategy = ErrorRecoveryStrategy::Ignore;

        auto boundary1 = std::make_unique<ErrorBoundary>();
        boundary1->setConfig(ignoreConfig);
        boundary1->catchError("Ignored error", "TestComponent");

        QVERIFY(boundary1->hasError());

        // Test Restart strategy
        ErrorBoundaryConfig restartConfig;
        restartConfig.strategy = ErrorRecoveryStrategy::Restart;

        auto boundary2 = std::make_unique<ErrorBoundary>();
        boundary2->setConfig(restartConfig);
        boundary2->initialize();

        bool factory_called = false;
        boundary2->setChildFactory(
            [&factory_called]() -> std::unique_ptr<QWidget> {
                factory_called = true;
                return std::make_unique<QLabel>("Restarted Child");
            });

        boundary2->catchError("Restart test error", "TestComponent");

        QVERIFY(factory_called);
    }

    void testErrorBoundaryErrorHistory() {
        auto boundary = std::make_unique<ErrorBoundary>();

        // Generate multiple errors
        boundary->catchError("Error 1", "Component1");
        boundary->catchError("Error 2", "Component2");
        boundary->catchError("Error 3", "Component3");

        const auto& history = boundary->getErrorHistory();
        QCOMPARE(history.size(), 3);
        QCOMPARE(history[0].message, QString("Error 1"));
        QCOMPARE(history[1].message, QString("Error 2"));
        QCOMPARE(history[2].message, QString("Error 3"));

        QCOMPARE(boundary->getStats().total_errors, 3);
    }

    void testErrorBoundaryExceptionHandling() {
        auto boundary = std::make_unique<ErrorBoundary>();

        // Test std::exception catching
        try {
            throw std::runtime_error("Runtime error test");
        } catch (const std::exception& e) {
            boundary->catchError(e, "ExceptionComponent");
        }

        QVERIFY(boundary->hasError());
        QCOMPARE(boundary->getLastError().message,
                 QString("Runtime error test"));
        QCOMPARE(boundary->getLastError().component_name,
                 QString("ExceptionComponent"));

        // Test exception_ptr catching
        std::exception_ptr eptr;
        try {
            throw std::invalid_argument("Invalid argument test");
        } catch (...) {
            eptr = std::current_exception();
        }

        boundary->catchError(eptr, "ExceptionPtrComponent");

        QCOMPARE(boundary->getLastError().message,
                 QString("Invalid argument test"));
        QCOMPARE(boundary->getStats().total_errors, 2);
    }
};

QTEST_MAIN(ErrorBoundaryTest)
#include "test_error_boundary.moc"
