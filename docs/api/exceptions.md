# Exceptions API Reference

The Exceptions module provides a comprehensive exception handling system for DeclarativeUI applications with specialized exception types, error context, and recovery utilities.

## Base Exception Classes

### UIException Class

The foundation class for all DeclarativeUI exceptions with rich context support.

#### Header

```cpp
#include "Exceptions/UIExceptions.hpp"
```

#### Class Declaration

```cpp
namespace DeclarativeUI::Exceptions {
    class UIException : public std::exception {
    public:
        explicit UIException(std::string message);
        UIException(std::string message, std::string context);
        UIException(std::string message, std::string context, std::string suggestion);
        
        // Fluent interface and access methods...
    };
}
```

#### Methods

##### Constructors

```cpp
explicit UIException(std::string message);
UIException(std::string message, std::string context);
UIException(std::string message, std::string context, std::string suggestion);
```

##### Fluent Interface

```cpp
UIException& withContext(const std::string& context);
UIException& withSuggestion(const std::string& suggestion);
```
Adds context information and suggestions using method chaining.

##### Access Methods

```cpp
const char* what() const noexcept override;
const std::string& getMessage() const noexcept;
const std::string& getContext() const noexcept;
const std::string& getSuggestion() const noexcept;
```

## Specialized Exception Types

### Component and Lifecycle Exceptions

#### ComponentCreationException

```cpp
class ComponentCreationException : public UIException {
public:
    explicit ComponentCreationException(const std::string& component_type);
    ComponentCreationException(const std::string& component_type, const std::string& reason);
    
    const std::string& getComponentType() const noexcept;
};
```

#### ComponentRegistrationException

```cpp
class ComponentRegistrationException : public UIException {
public:
    explicit ComponentRegistrationException(const std::string& component_name);
    ComponentRegistrationException(const std::string& component_name, const std::string& reason);
    
    const std::string& getComponentName() const noexcept;
};
```

#### InitializationException

```cpp
class InitializationException : public UIException {
public:
    explicit InitializationException(const std::string& component_name);
    InitializationException(const std::string& component_name, const std::string& reason);
    
    const std::string& getComponentName() const noexcept;
};
```

### Property and State Management Exceptions

#### PropertyBindingException

```cpp
class PropertyBindingException : public UIException {
public:
    explicit PropertyBindingException(const std::string& property_name);
    PropertyBindingException(const std::string& property_name, const std::string& reason);
    
    const std::string& getPropertyName() const noexcept;
};
```

#### StateManagementException

```cpp
class StateManagementException : public UIException {
public:
    explicit StateManagementException(const std::string& state_key);
    StateManagementException(const std::string& state_key, const std::string& reason);
    
    const std::string& getStateKey() const noexcept;
};
```

### Layout and UI Exceptions

#### LayoutException

```cpp
class LayoutException : public UIException {
public:
    explicit LayoutException(const std::string& layout_error);
    LayoutException(const std::string& layout_type, const std::string& error_details);
    
    const std::string& getLayoutType() const noexcept;
};
```

#### StyleException

```cpp
class StyleException : public UIException {
public:
    explicit StyleException(const std::string& style_error);
    StyleException(const std::string& selector, const std::string& style_error);
    
    const std::string& getSelector() const noexcept;
};
```

### JSON and Configuration Exceptions

#### JSONParsingException

```cpp
class JSONParsingException : public UIException {
public:
    explicit JSONParsingException(const std::string& file_path, const std::string& error);
    JSONParsingException(const std::string& file_path, const std::string& error, int line, int column);
    
    const std::string& getFilePath() const noexcept;
    int getLineNumber() const noexcept;
    int getColumnNumber() const noexcept;
    bool hasLineInfo() const noexcept;
};
```

#### JSONValidationException

```cpp
class JSONValidationException : public UIException {
public:
    explicit JSONValidationException(const std::string& validation_error);
    JSONValidationException(const std::string& validation_error, const std::string& json_path);
    JSONValidationException(const std::vector<std::string>& validation_errors);
    
    const std::string& getJsonPath() const noexcept;
    const std::vector<std::string>& getValidationErrors() const noexcept;
    bool hasMultipleErrors() const noexcept;
};
```

#### ConfigurationException

```cpp
class ConfigurationException : public UIException {
public:
    explicit ConfigurationException(const std::string& config_error);
    ConfigurationException(const std::string& config_key, const std::string& config_error);
    
    const std::string& getConfigKey() const noexcept;
};
```

### Hot Reload and File System Exceptions

#### HotReloadException

```cpp
class HotReloadException : public UIException {
public:
    explicit HotReloadException(const std::string& reload_error);
    HotReloadException(const std::string& file_path, const std::string& reload_error);
    
    const std::string& getFilePath() const noexcept;
};
```

#### FileWatchException

```cpp
class FileWatchException : public UIException {
public:
    explicit FileWatchException(const std::string& file_path);
    FileWatchException(const std::string& file_path, const std::string& reason);
    
    const std::string& getFilePath() const noexcept;
};
```

### Resource and Network Exceptions

#### ResourceLoadException

```cpp
class ResourceLoadException : public UIException {
public:
    explicit ResourceLoadException(const std::string& resource_path);
    ResourceLoadException(const std::string& resource_path, const std::string& reason);
    
    const std::string& getResourcePath() const noexcept;
};
```

#### NetworkException

```cpp
class NetworkException : public UIException {
public:
    explicit NetworkException(const std::string& network_error);
    NetworkException(const std::string& url, const std::string& network_error);
    NetworkException(const std::string& url, int status_code, const std::string& network_error);
    
    const std::string& getUrl() const noexcept;
    int getStatusCode() const noexcept;
    bool hasStatusCode() const noexcept;
};
```

### Runtime and Threading Exceptions

#### RuntimeException

```cpp
class RuntimeException : public UIException {
public:
    explicit RuntimeException(const std::string& runtime_error);
    RuntimeException(const std::string& operation, const std::string& runtime_error);
    
    const std::string& getOperation() const noexcept;
};
```

#### ConcurrencyException

```cpp
class ConcurrencyException : public UIException {
public:
    explicit ConcurrencyException(const std::string& concurrency_error);
    ConcurrencyException(const std::string& thread_context, const std::string& concurrency_error);
    
    const std::string& getThreadContext() const noexcept;
};
```

## Exception Utilities

### ExceptionChain Class

```cpp
namespace ExceptionUtils {
    class ExceptionChain : public UIException {
    public:
        explicit ExceptionChain(const std::string& primary_message);
        
        ExceptionChain& addException(std::unique_ptr<UIException> exception);
        ExceptionChain& addException(const UIException& exception);
        
        const std::vector<std::unique_ptr<UIException>>& getChainedExceptions() const noexcept;
        size_t getExceptionCount() const noexcept;
    };
}
```

### Utility Functions

#### Exception Creation

```cpp
template<typename ExceptionType, typename... Args>
std::unique_ptr<ExceptionType> createException(Args&&... args);
```

#### Safe Execution

```cpp
template<typename T>
std::optional<T> safeExecute(
    std::function<T()> operation,
    std::function<void(const UIException&)> error_handler = nullptr
);
```

#### Exception Analysis

```cpp
std::string formatExceptionDetails(const UIException& exception);
std::string getExceptionHierarchy(const std::exception& exception);
QStringList getExceptionMessages(const UIException& exception);
```

#### Exception Validation

```cpp
bool isRecoverableException(const UIException& exception) noexcept;
bool isCriticalException(const UIException& exception) noexcept;
std::string getExceptionCategory(const UIException& exception) noexcept;
```

#### Logging and Reporting

```cpp
void logException(const UIException& exception, const std::string& severity = "ERROR");
void reportException(const UIException& exception, const std::string& context = "");
```

## Convenience Macros

### Exception Throwing

```cpp
#define THROW_UI_EXCEPTION(ExceptionType, ...)
#define THROW_UI_EXCEPTION_WITH_CONTEXT(ExceptionType, context, ...)
#define THROW_UI_EXCEPTION_WITH_SUGGESTION(ExceptionType, suggestion, ...)
```

### Safe Execution

```cpp
#define SAFE_UI_EXECUTE(operation, error_handler)
```

### Qt Integration

```cpp
#define HANDLE_UI_EXCEPTION_QT(exception)
```

## Usage Examples

### Basic Exception Handling

```cpp
try {
    auto button = std::make_unique<Components::Button>();
    button->initialize();
} catch (const ComponentCreationException& e) {
    qCritical() << "Component creation failed:" << e.what();
    qDebug() << "Component type:" << QString::fromStdString(e.getComponentType());
} catch (const UIException& e) {
    qCritical() << "UI error:" << e.what();
    qDebug() << "Context:" << QString::fromStdString(e.getContext());
    qDebug() << "Suggestion:" << QString::fromStdString(e.getSuggestion());
}
```

### Exception with Context and Suggestions

```cpp
void validateUserInput(const QString& input) {
    if (input.isEmpty()) {
        throw PropertyBindingException("user_input")
            .withContext("Form validation")
            .withSuggestion("Provide a non-empty input value");
    }
}
```

### JSON Parsing with Detailed Errors

```cpp
QJsonObject parseConfigFile(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        throw ResourceLoadException(filePath.toStdString(), "Cannot open file for reading");
    }
    
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
    
    if (parseError.error != QJsonParseError::NoError) {
        throw JSONParsingException(
            filePath.toStdString(),
            parseError.errorString().toStdString(),
            parseError.offset
        );
    }
    
    return doc.object();
}
```

### Exception Chaining

```cpp
void complexOperation() {
    ExceptionUtils::ExceptionChain errors("Multiple errors occurred during operation");
    
    try {
        loadConfiguration();
    } catch (const JSONParsingException& e) {
        errors.addException(std::make_unique<JSONParsingException>(e));
    }
    
    try {
        initializeComponents();
    } catch (const ComponentCreationException& e) {
        errors.addException(std::make_unique<ComponentCreationException>(e));
    }
    
    if (errors.getExceptionCount() > 0) {
        throw errors;
    }
}
```

### Safe Execution Pattern

```cpp
auto result = ExceptionUtils::safeExecute<QString>(
    []() {
        return loadConfigurationValue("theme.primary_color");
    },
    [](const UIException& e) {
        qWarning() << "Failed to load theme color:" << e.what();
    }
);

QString themeColor = result.value_or("#3498db");  // Default blue
```

### Macro Usage

```cpp
// Convenient exception throwing
THROW_UI_EXCEPTION_WITH_CONTEXT(
    PropertyBindingException,
    "Widget initialization",
    "opacity", "Property not found"
);

// Safe execution with error handling
auto config = SAFE_UI_EXECUTE(
    []() { return loadConfiguration(); },
    [](const UIException& e) { HANDLE_UI_EXCEPTION_QT(e); }
);
```

### Exception Recovery

```cpp
try {
    performRiskyOperation();
} catch (const UIException& e) {
    if (ExceptionUtils::isRecoverableException(e)) {
        qWarning() << "Recoverable error:" << e.what();
        performRecoveryAction();
    } else if (ExceptionUtils::isCriticalException(e)) {
        qCritical() << "Critical error:" << e.what();
        initiateGracefulShutdown();
    }
    
    // Log the exception
    ExceptionUtils::logException(e, "WARNING");
}
```
