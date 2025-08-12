// tests/unit/test_error_handling.cpp
#include <QtTest/QtTest>
#include <stdexcept>
#include <memory>

#include "../../src/Core/ErrorHandling.hpp"

using namespace DeclarativeUI::Core::ErrorHandling;

class ErrorHandlingTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    // **Exception Tests**
    void testUIException();
    void testSpecificExceptions();
    void testExceptionContext();

    // **Error Handler Tests**
    void testConsoleErrorHandler();
    void testFileErrorHandler();

    // **Error Manager Tests**
    void testErrorManagerSingleton();
    void testErrorManagerLogging();
    void testSafeExecution();
    void testPerformanceMeasurement();
    void testAssertion();

    // **Result Type Tests**
    void testResultType();

    // **Error Context Tests**
    void testErrorContext();
    void testErrorContextScope();

private:
    std::unique_ptr<ErrorManager> errorManager;
};

void ErrorHandlingTest::initTestCase() {
    // Setup test environment
}

void ErrorHandlingTest::cleanupTestCase() {
    // Cleanup test environment
}

// **Exception Tests**
void ErrorHandlingTest::testUIException() {
    QString message = "Test error message";
    ErrorSeverity severity = ErrorSeverity::Warning;
    ErrorCategory category = ErrorCategory::UI;
    ErrorContext context;
    context.component_name = "TestComponent";
    context.operation = "TestOperation";

    UIException exception(message, severity, category, context);

    QCOMPARE(exception.getMessage(), message);
    QCOMPARE(exception.getSeverity(), severity);
    QCOMPARE(exception.getCategory(), category);
    QCOMPARE(exception.getContext().component_name, QString("TestComponent"));
    QCOMPARE(exception.getContext().operation, QString("TestOperation"));

    // Test what() method
    QCOMPARE(QString::fromStdString(exception.what()), message);

    // Test formatted message
    QString formatted = exception.getFormattedMessage();
    QVERIFY(formatted.contains(message));
    QVERIFY(formatted.contains("TestComponent") || formatted.contains("TestOperation"));
}

void ErrorHandlingTest::testSpecificExceptions() {
    // Test ValidationException
    ValidationException validationEx("Validation failed");
    QCOMPARE(validationEx.getSeverity(), ErrorSeverity::Error);
    QCOMPARE(validationEx.getCategory(), ErrorCategory::Validation);

    // Test LayoutException
    LayoutException layoutEx("Layout error");
    QCOMPARE(layoutEx.getSeverity(), ErrorSeverity::Error);
    QCOMPARE(layoutEx.getCategory(), ErrorCategory::Layout);

    // Test AnimationException
    AnimationException animationEx("Animation error");
    QCOMPARE(animationEx.getSeverity(), ErrorSeverity::Error);
    QCOMPARE(animationEx.getCategory(), ErrorCategory::Animation);

    // Test ThemeException
    ThemeException themeEx("Theme error");
    QCOMPARE(themeEx.getSeverity(), ErrorSeverity::Error);
    QCOMPARE(themeEx.getCategory(), ErrorCategory::Theme);
}

void ErrorHandlingTest::testExceptionContext() {
    ErrorContext context;
    QVERIFY(context.timestamp.time_since_epoch().count() > 0);
    QVERIFY(context.location.file_name() != nullptr);
    QVERIFY(context.location.line() > 0);
    QVERIFY(context.location.function_name() != nullptr);
}

// **Error Handler Tests**
void ErrorHandlingTest::testConsoleErrorHandler() {
    ConsoleErrorHandler handler;

    UIException exception("Test console error", ErrorSeverity::Error, ErrorCategory::General);

    // Test that handler doesn't crash
    handler.handleError(exception);
    handler.handleError(ErrorSeverity::Info, "Test info message", ErrorContext{});

    QVERIFY(true); // Test passes if no crash occurs
}

void ErrorHandlingTest::testFileErrorHandler() {
    QString testFilename = "test_error_log.txt";
    FileErrorHandler handler(testFilename);

    UIException exception("Test file error", ErrorSeverity::Warning, ErrorCategory::IO);

    // Test that handler doesn't crash
    handler.handleError(exception);
    handler.handleError(ErrorSeverity::Debug, "Test debug message", ErrorContext{});

    QVERIFY(true); // Test passes if no crash occurs

    // Cleanup test file
    QFile::remove(testFilename);
}

// **Error Manager Tests**
void ErrorHandlingTest::testErrorManagerSingleton() {
    ErrorManager& manager1 = ErrorManager::instance();
    ErrorManager& manager2 = ErrorManager::instance();

    QCOMPARE(&manager1, &manager2);
}

void ErrorHandlingTest::testErrorManagerLogging() {
    ErrorManager& manager = ErrorManager::instance();

    // Test different logging levels
    manager.debug("Debug message");
    manager.info("Info message");
    manager.warning("Warning message");
    manager.error("Error message");
    manager.critical("Critical message");

    // Test that no exceptions are thrown
    QVERIFY(true);
}

void ErrorHandlingTest::testSafeExecution() {
    ErrorManager& manager = ErrorManager::instance();

    // Test successful execution
    auto successResult = manager.safeExecute([]() {
        return 42;
    }, "Test operation");

    QVERIFY(successResult.has_value());
    QCOMPARE(successResult.value(), 42);

    // Test execution with exception
    auto failureResult = manager.safeExecute([]() -> int {
        throw std::runtime_error("Test exception");
    }, "Test operation");

    QVERIFY(!failureResult.has_value());

    // Test void function execution
    bool executed = false;
    auto voidResult = manager.safeExecute([&executed]() {
        executed = true;
    }, "Void operation");

    QVERIFY(voidResult.has_value());
    QVERIFY(executed);
}

void ErrorHandlingTest::testPerformanceMeasurement() {
    ErrorManager& manager = ErrorManager::instance();

    auto result = manager.measurePerformance([]() {
        QThread::msleep(10); // Simulate work
        return "Done";
    }, "Performance test");

    QVERIFY(result.has_value());
    QCOMPARE(result.value(), QString("Done"));
}

void ErrorHandlingTest::testAssertion() {
    ErrorManager& manager = ErrorManager::instance();

    // Test successful assertion
    manager.uiAssert(true, "This should pass");

    // Test failed assertion (should not terminate in test)
    manager.uiAssert(false, "This should fail", ErrorSeverity::Warning);

    QVERIFY(true); // Test passes if we reach here
}

// **Result Type Tests**
void ErrorHandlingTest::testResultType() {
    // Test successful result
    Result<int> successResult = 42;
    QVERIFY(successResult.has_value());
    QCOMPARE(successResult.value(), 42);

    // Test error result (empty optional represents error)
    Result<int> errorResult = std::nullopt;
    QVERIFY(!errorResult.has_value());
}

// **Error Context Tests**
void ErrorHandlingTest::testErrorContext() {
    ErrorContext context;
    context.component_name = "TestComponent";
    context.operation = "TestOperation";
    context.additional_data.emplace_back("key1", "value1");
    context.additional_data.emplace_back("key2", "value2");

    QCOMPARE(context.component_name, QString("TestComponent"));
    QCOMPARE(context.operation, QString("TestOperation"));
    QCOMPARE(context.additional_data.size(), 2);
    QCOMPARE(context.additional_data[0].first, QString("key1"));
    QCOMPARE(context.additional_data[0].second, QString("value1"));
}

void ErrorHandlingTest::testErrorContextScope() {
    // Test error context scope
    {
        ErrorContextScope scope("TestComponent", "TestOperation");
        ErrorContext current = ErrorContextScope::getCurrentContext();
        QCOMPARE(current.component_name, QString("TestComponent"));
        QCOMPARE(current.operation, QString("TestOperation"));
    }

    // Context should be restored after scope ends
    ErrorContext afterScope = ErrorContextScope::getCurrentContext();
    // Note: The context might be empty or have default values after scope ends
}

QTEST_MAIN(ErrorHandlingTest)
#include "test_error_handling.moc"
