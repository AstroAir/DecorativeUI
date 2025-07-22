#include <QApplication>
#include <QTest>
#include <memory>
#include <stdexcept>

#include "../Exceptions/UIExceptions.hpp"

using namespace DeclarativeUI::Exceptions;
using namespace DeclarativeUI::Exceptions::ExceptionUtils;

class ExceptionsTest : public QObject {
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
    }

    void cleanup() {
        // Clean up after each test
    }

    // **Base UIException Tests**
    void testUIExceptionBasic() {
        UIException exception("Test error message");
        
        QCOMPARE(exception.getMessage(), std::string("Test error message"));
        QCOMPARE(exception.getContext(), std::string(""));
        QCOMPARE(exception.getSuggestion(), std::string(""));
        
        std::string what_msg = exception.what();
        QVERIFY(what_msg.find("Test error message") != std::string::npos);
    }

    void testUIExceptionWithContext() {
        UIException exception("Error occurred", "In test function");
        
        QCOMPARE(exception.getMessage(), std::string("Error occurred"));
        QCOMPARE(exception.getContext(), std::string("In test function"));
        
        std::string what_msg = exception.what();
        QVERIFY(what_msg.find("Error occurred") != std::string::npos);
        QVERIFY(what_msg.find("In test function") != std::string::npos);
    }

    void testUIExceptionWithSuggestion() {
        UIException exception("Error occurred", "In test function", "Try again");
        
        QCOMPARE(exception.getMessage(), std::string("Error occurred"));
        QCOMPARE(exception.getContext(), std::string("In test function"));
        QCOMPARE(exception.getSuggestion(), std::string("Try again"));
        
        std::string what_msg = exception.what();
        QVERIFY(what_msg.find("Try again") != std::string::npos);
    }

    void testUIExceptionFluentInterface() {
        UIException exception("Base error");
        
        exception.withContext("Added context")
                 .withSuggestion("Added suggestion");
        
        QCOMPARE(exception.getContext(), std::string("Added context"));
        QCOMPARE(exception.getSuggestion(), std::string("Added suggestion"));
        
        std::string what_msg = exception.what();
        QVERIFY(what_msg.find("Added context") != std::string::npos);
        QVERIFY(what_msg.find("Added suggestion") != std::string::npos);
    }

    // **Component Exception Tests**
    void testComponentCreationException() {
        ComponentCreationException exception("Button");
        
        QCOMPARE(exception.getComponentType(), std::string("Button"));
        QVERIFY(exception.getMessage().find("Failed to create component") != std::string::npos);
        QVERIFY(exception.getContext().find("Button") != std::string::npos);
    }

    void testComponentCreationExceptionWithReason() {
        ComponentCreationException exception("Label", "Memory allocation failed");
        
        QCOMPARE(exception.getComponentType(), std::string("Label"));
        QVERIFY(exception.getMessage().find("Memory allocation failed") != std::string::npos);
    }

    void testComponentRegistrationException() {
        ComponentRegistrationException exception("CustomWidget");
        
        QCOMPARE(exception.getComponentName(), std::string("CustomWidget"));
        QVERIFY(exception.getMessage().find("Component registration failed") != std::string::npos);
    }

    void testComponentRegistrationExceptionWithReason() {
        ComponentRegistrationException exception("CustomWidget", "Duplicate registration");
        
        QCOMPARE(exception.getComponentName(), std::string("CustomWidget"));
        QVERIFY(exception.getMessage().find("Duplicate registration") != std::string::npos);
    }

    // **Property and State Exception Tests**
    void testPropertyBindingException() {
        PropertyBindingException exception("text");
        
        QCOMPARE(exception.getPropertyName(), std::string("text"));
        QVERIFY(exception.getMessage().find("Property binding failed") != std::string::npos);
    }

    void testStateManagementException() {
        StateManagementException exception("user_state", "Invalid state transition");
        
        QCOMPARE(exception.getStateKey(), std::string("user_state"));
        QVERIFY(exception.getMessage().find("Invalid state transition") != std::string::npos);
    }

    // **Layout Exception Tests**
    void testLayoutException() {
        LayoutException exception("Invalid layout configuration");
        
        QVERIFY(exception.getMessage().find("Invalid layout configuration") != std::string::npos);
    }

    void testLayoutExceptionWithType() {
        LayoutException exception("QVBoxLayout", "Cannot add widget to layout");
        
        QCOMPARE(exception.getLayoutType(), std::string("QVBoxLayout"));
        QVERIFY(exception.getMessage().find("Cannot add widget to layout") != std::string::npos);
    }

    // **JSON Exception Tests**
    void testJSONParsingException() {
        JSONParsingException exception("config.json", "Unexpected token");
        
        QCOMPARE(exception.getFilePath(), std::string("config.json"));
        QVERIFY(exception.getMessage().find("Unexpected token") != std::string::npos);
        QVERIFY(!exception.hasLineInfo());
    }

    void testJSONParsingExceptionWithLineInfo() {
        JSONParsingException exception("config.json", "Missing comma", 15, 23);
        
        QCOMPARE(exception.getFilePath(), std::string("config.json"));
        QCOMPARE(exception.getLineNumber(), 15);
        QCOMPARE(exception.getColumnNumber(), 23);
        QVERIFY(exception.hasLineInfo());
        
        std::string what_msg = exception.what();
        QVERIFY(what_msg.find("Line: 15") != std::string::npos);
        QVERIFY(what_msg.find("Column: 23") != std::string::npos);
    }

    void testJSONValidationException() {
        JSONValidationException exception("Required field missing");
        
        QVERIFY(exception.getMessage().find("Required field missing") != std::string::npos);
        QVERIFY(!exception.hasMultipleErrors());
    }

    void testJSONValidationExceptionWithPath() {
        JSONValidationException exception("Type mismatch", "$.properties.text");
        
        QCOMPARE(exception.getJsonPath(), std::string("$.properties.text"));
        QVERIFY(exception.getMessage().find("Type mismatch") != std::string::npos);
    }

    void testJSONValidationExceptionMultipleErrors() {
        std::vector<std::string> errors = {
            "Missing required field 'name'",
            "Invalid type for field 'value'",
            "Unknown property 'extra'"
        };
        
        JSONValidationException exception(errors);
        
        QVERIFY(exception.hasMultipleErrors());
        auto validation_errors = exception.getValidationErrors();
        QCOMPARE(validation_errors.size(), 3);
        QCOMPARE(validation_errors[0], std::string("Missing required field 'name'"));
        QCOMPARE(validation_errors[1], std::string("Invalid type for field 'value'"));
        QCOMPARE(validation_errors[2], std::string("Unknown property 'extra'"));
        
        std::string what_msg = exception.what();
        QVERIFY(what_msg.find("1. Missing required field 'name'") != std::string::npos);
        QVERIFY(what_msg.find("2. Invalid type for field 'value'") != std::string::npos);
        QVERIFY(what_msg.find("3. Unknown property 'extra'") != std::string::npos);
    }

    // **Hot Reload Exception Tests**
    void testHotReloadException() {
        HotReloadException exception("Reload failed");
        
        QVERIFY(exception.getMessage().find("Reload failed") != std::string::npos);
    }

    void testHotReloadExceptionWithFile() {
        HotReloadException exception("ui/main.json", "File not found");
        
        QCOMPARE(exception.getFilePath(), std::string("ui/main.json"));
        QVERIFY(exception.getMessage().find("File not found") != std::string::npos);
    }

    void testFileWatchException() {
        FileWatchException exception("/path/to/file.json", "Permission denied");
        
        QCOMPARE(exception.getFilePath(), std::string("/path/to/file.json"));
        QVERIFY(exception.getMessage().find("Permission denied") != std::string::npos);
    }

    // **Resource and Network Exception Tests**
    void testResourceLoadException() {
        ResourceLoadException exception("icons/save.png", "File not found");
        
        QCOMPARE(exception.getResourcePath(), std::string("icons/save.png"));
        QVERIFY(exception.getMessage().find("File not found") != std::string::npos);
    }

    void testNetworkException() {
        NetworkException exception("https://api.example.com", "Connection timeout");
        
        QCOMPARE(exception.getUrl(), std::string("https://api.example.com"));
        QVERIFY(exception.getMessage().find("Connection timeout") != std::string::npos);
        QVERIFY(!exception.hasStatusCode());
    }

    void testNetworkExceptionWithStatusCode() {
        NetworkException exception("https://api.example.com", 404, "Not found");
        
        QCOMPARE(exception.getUrl(), std::string("https://api.example.com"));
        QCOMPARE(exception.getStatusCode(), 404);
        QVERIFY(exception.hasStatusCode());
        
        std::string what_msg = exception.what();
        QVERIFY(what_msg.find("Status: 404") != std::string::npos);
    }

    // **Other Exception Types Tests**
    void testStyleException() {
        StyleException exception("QPushButton", "Invalid CSS property");
        
        QCOMPARE(exception.getSelector(), std::string("QPushButton"));
        QVERIFY(exception.getMessage().find("Invalid CSS property") != std::string::npos);
    }

    void testEventHandlingException() {
        EventHandlingException exception("clicked", "Handler not found");
        
        QCOMPARE(exception.getEventName(), std::string("clicked"));
        QVERIFY(exception.getMessage().find("Handler not found") != std::string::npos);
    }

    void testConfigurationException() {
        ConfigurationException exception("theme.color", "Invalid color value");
        
        QCOMPARE(exception.getConfigKey(), std::string("theme.color"));
        QVERIFY(exception.getMessage().find("Invalid color value") != std::string::npos);
    }

    void testInitializationException() {
        InitializationException exception("DatabaseConnection", "Connection failed");
        
        QCOMPARE(exception.getComponentName(), std::string("DatabaseConnection"));
        QVERIFY(exception.getMessage().find("Connection failed") != std::string::npos);
    }

    void testRuntimeException() {
        RuntimeException exception("widget_update", "Null pointer access");
        
        QCOMPARE(exception.getOperation(), std::string("widget_update"));
        QVERIFY(exception.getMessage().find("Null pointer access") != std::string::npos);
    }

    void testConcurrencyException() {
        ConcurrencyException exception("UI_Thread", "Deadlock detected");
        
        QCOMPARE(exception.getThreadContext(), std::string("UI_Thread"));
        QVERIFY(exception.getMessage().find("Deadlock detected") != std::string::npos);
    }

    // **Exception Chain Tests**
    void testExceptionChain() {
        ExceptionChain chain("Primary error occurred");
        
        auto secondary = std::make_unique<UIException>("Secondary error");
        auto tertiary = std::make_unique<ComponentCreationException>("Button", "Creation failed");
        
        chain.addException(std::move(secondary))
             .addException(std::move(tertiary));
        
        QCOMPARE(chain.getExceptionCount(), 2);
        
        std::string what_msg = chain.what();
        QVERIFY(what_msg.find("Primary error occurred") != std::string::npos);
        QVERIFY(what_msg.find("Chained exceptions:") != std::string::npos);
        QVERIFY(what_msg.find("1. Secondary error") != std::string::npos);
        QVERIFY(what_msg.find("2. Failed to create component: Creation failed") != std::string::npos);
    }

    // **Exception Utilities Tests**
    void testCreateException() {
        auto exception = createException<ComponentCreationException>("TestWidget");
        
        QVERIFY(exception != nullptr);
        QCOMPARE(exception->getComponentType(), std::string("TestWidget"));
    }

    void testSafeExecute() {
        // Test successful execution
        auto result = safeExecute<int>([]() -> int {
            return 42;
        });
        
        QVERIFY(result.has_value());
        QCOMPARE(result.value(), 42);
        
        // Test execution with UIException
        bool error_handled = false;
        auto error_result = safeExecute<int>(
            []() -> int {
                throw UIException("Test error");
            },
            [&error_handled](const UIException& e) {
                error_handled = true;
                QVERIFY(e.getMessage().find("Test error") != std::string::npos);
            }
        );
        
        QVERIFY(!error_result.has_value());
        QVERIFY(error_handled);
        
        // Test execution with std::exception
        bool std_error_handled = false;
        auto std_error_result = safeExecute<int>(
            []() -> int {
                throw std::runtime_error("Standard error");
            },
            [&std_error_handled](const UIException& e) {
                std_error_handled = true;
                QVERIFY(e.getMessage().find("Unexpected exception") != std::string::npos);
            }
        );
        
        QVERIFY(!std_error_result.has_value());
        QVERIFY(std_error_handled);
    }

    // **Exception Macros Tests**
    void testExceptionMacros() {
        // Test basic exception throwing macro
        try {
            THROW_UI_EXCEPTION(ComponentCreationException, "TestComponent");
            QFAIL("Exception should have been thrown");
        } catch (const ComponentCreationException& e) {
            QCOMPARE(e.getComponentType(), std::string("TestComponent"));
        }
        
        // Test exception with context macro
        try {
            THROW_UI_EXCEPTION_WITH_CONTEXT(UIException, "test context", "Test message");
            QFAIL("Exception should have been thrown");
        } catch (const UIException& e) {
            QCOMPARE(e.getMessage(), std::string("Test message"));
            QCOMPARE(e.getContext(), std::string("test context"));
        }
        
        // Test exception with suggestion macro
        try {
            THROW_UI_EXCEPTION_WITH_SUGGESTION(UIException, "try this", "Test message");
            QFAIL("Exception should have been thrown");
        } catch (const UIException& e) {
            QCOMPARE(e.getMessage(), std::string("Test message"));
            QCOMPARE(e.getSuggestion(), std::string("try this"));
        }
    }

    // **Exception Inheritance Tests**
    void testExceptionInheritance() {
        ComponentCreationException component_ex("TestWidget");
        UIException& base_ref = component_ex;
        
        // Test that derived exception can be caught as base
        try {
            throw component_ex;
        } catch (const UIException& e) {
            QVERIFY(e.getMessage().find("Failed to create component") != std::string::npos);
        }
        
        // Test polymorphic behavior
        QCOMPARE(base_ref.getMessage(), component_ex.getMessage());
        QCOMPARE(base_ref.getContext(), component_ex.getContext());
    }
};

QTEST_MAIN(ExceptionsTest)
#include "test_exceptions.moc"
