# Exceptions Module

The Exceptions module provides a comprehensive exception handling system for DeclarativeUI applications. It includes specialized exception types, error handling utilities, and best practices for robust error management throughout the framework.

## Overview

This directory contains the complete exception hierarchy and utilities:

- **UIException**: Base exception class with context and suggestion support
- **Specialized Exceptions**: Domain-specific exception types for different subsystems
- **Exception Utilities**: Helper functions for exception handling and reporting
- **Error Recovery**: Safe execution patterns and fallback mechanisms

## Key Features

### 🛡️ **Comprehensive Exception Hierarchy**
- Base `UIException` class with rich context information
- Specialized exceptions for different framework subsystems
- Exception chaining and aggregation support
- Detailed error messages with suggestions

### 📝 **Rich Error Context**
- Contextual information for better debugging
- Suggested solutions for common problems
- Formatted error messages with hierarchy
- Integration with Qt logging system

### 🔧 **Error Recovery Utilities**
- Safe execution patterns with fallback handling
- Exception validation and categorization
- Automatic error reporting and logging
- Recovery strategies for different error types

### 🎯 **Framework Integration**
- Seamless integration with all DeclarativeUI modules
- Qt-specific error handling patterns
- Thread-safe exception handling
- Performance-optimized error paths

## Exception Hierarchy

### Base Exception: UIException

The foundation class for all DeclarativeUI exceptions.

**Key Features:**
- Rich error messages with context and suggestions
- Fluent interface for adding context information
- Cached formatted messages for performance
- Standard exception interface compliance

```cpp
class UIException : public std::exception {
public:
    explicit UIException(std::string message);
    UIException(std::string message, std::string context);
    UIException(std::string message, std::string context, std::string suggestion);
    
    // Fluent interface
    UIException& withContext(const std::string& context);
    UIException& withSuggestion(const std::string& suggestion);
    
    // Access methods
    const std::string& getMessage() const noexcept;
    const std::string& getContext() const noexcept;
    const std::string& getSuggestion() const noexcept;
};
```

### Component and Lifecycle Exceptions

**ComponentCreationException**: Component instantiation failures
```cpp
throw ComponentCreationException("Button", "Failed to create QPushButton widget");
```

**ComponentRegistrationException**: Component registry issues
```cpp
throw ComponentRegistrationException("CustomWidget", "Component already registered");
```

**InitializationException**: Component initialization failures
```cpp
throw InitializationException("AnimationEngine", "OpenGL context not available");
```

### Property and State Management Exceptions

**PropertyBindingException**: Property binding failures
```cpp
throw PropertyBindingException("text", "Source property does not exist");
```

**StateManagementException**: State system errors
```cpp
throw StateManagementException("user.profile", "State key not found");
```

### Layout and UI Exceptions

**LayoutException**: Layout management errors
```cpp
throw LayoutException("QVBoxLayout", "Cannot add widget to finalized layout");
```

**StyleException**: Styling and theming errors
```cpp
throw StyleException("QPushButton", "Invalid CSS selector syntax");
```

### JSON and Configuration Exceptions

**JSONParsingException**: JSON parsing errors with line/column information
```cpp
throw JSONParsingException("config.json", "Unexpected token", 15, 23);
```

**JSONValidationException**: Schema validation failures
```cpp
throw JSONValidationException("Invalid component type", "$.components[0].type");
```

**ConfigurationException**: Configuration errors
```cpp
throw ConfigurationException("theme.primary_color", "Invalid color format");
```

### Hot Reload and File System Exceptions

**HotReloadException**: Hot reload system errors
```cpp
throw HotReloadException("ui/main.json", "Circular dependency detected");
```

**FileWatchException**: File monitoring failures
```cpp
throw FileWatchException("/path/to/file.json", "Permission denied");
```

### Resource and Network Exceptions

**ResourceLoadException**: Asset loading failures
```cpp
throw ResourceLoadException("icons/save.png", "File not found");
```

**NetworkException**: Network operation errors
```cpp
throw NetworkException("https://api.example.com", 404, "Resource not found");
```

### Runtime and Threading Exceptions

**RuntimeException**: General runtime errors
```cpp
throw RuntimeException("widget_update", "Widget destroyed during update");
```

**ConcurrencyException**: Threading and synchronization errors
```cpp
throw ConcurrencyException("UI_THREAD", "Cannot access UI from background thread");
```

## Exception Utilities

### Exception Creation Helpers

```cpp
#include "Exceptions/UIExceptions.hpp"
using namespace DeclarativeUI::Exceptions::ExceptionUtils;

// Type-safe exception creation
auto exception = createException<ComponentCreationException>("Button", "Creation failed");

// Exception chaining
ExceptionChain chain("Multiple errors occurred");
chain.addException(createException<PropertyBindingException>("text", "Binding failed"))
     .addException(createException<StyleException>("color", "Invalid value"));
```

### Safe Execution Patterns

```cpp
// Safe execution with optional error handling
auto result = safeExecute<QString>(
    []() {
        return loadConfigurationValue("theme.color");
    },
    [](const UIException& e) {
        qWarning() << "Config load failed:" << e.what();
    }
);

if (result.has_value()) {
    applyThemeColor(result.value());
} else {
    applyDefaultThemeColor();
}
```

### Exception Validation and Categorization

```cpp
try {
    performRiskyOperation();
} catch (const UIException& e) {
    if (isRecoverableException(e)) {
        // Attempt recovery
        performRecoveryAction();
    } else if (isCriticalException(e)) {
        // Critical error - shut down gracefully
        initiateGracefulShutdown();
    }
    
    QString category = getExceptionCategory(e);
    logException(e, category);
}
```

## Usage Examples

### Basic Exception Handling

```cpp
#include "Exceptions/UIExceptions.hpp"
using namespace DeclarativeUI::Exceptions;

try {
    auto button = std::make_unique<Components::Button>();
    button->initialize();
} catch (const ComponentCreationException& e) {
    qCritical() << "Button creation failed:" << e.what();
    qDebug() << "Context:" << QString::fromStdString(e.getContext());
    qDebug() << "Suggestion:" << QString::fromStdString(e.getSuggestion());
} catch (const UIException& e) {
    qCritical() << "UI error:" << e.what();
}
```

### Exception with Context and Suggestions

```cpp
void validateConfiguration(const QJsonObject& config) {
    if (!config.contains("version")) {
        throw ConfigurationException("version")
            .withContext("Configuration file validation")
            .withSuggestion("Add 'version' field to configuration file");
    }
    
    if (config["version"].toString().isEmpty()) {
        throw ConfigurationException("version", "Version field is empty")
            .withSuggestion("Set version to a valid semantic version (e.g., '1.0.0')");
    }
}
```

### JSON Parsing with Detailed Error Information

```cpp
QJsonObject parseUIDefinition(const QString& filePath) {
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
            parseError.offset  // Convert offset to line/column if needed
        );
    }
    
    return doc.object();
}
```

### Exception Chaining for Complex Operations

```cpp
void loadAndApplyTheme(const QString& themePath) {
    ExceptionChain errors("Theme loading failed");
    
    try {
        auto themeData = parseUIDefinition(themePath);
        validateThemeData(themeData);
        applyThemeToApplication(themeData);
    } catch (const JSONParsingException& e) {
        errors.addException(std::make_unique<JSONParsingException>(e));
    } catch (const JSONValidationException& e) {
        errors.addException(std::make_unique<JSONValidationException>(e));
    } catch (const StyleException& e) {
        errors.addException(std::make_unique<StyleException>(e));
    }
    
    if (errors.getExceptionCount() > 0) {
        throw errors;
    }
}
```

### Qt Integration Macros

```cpp
// Convenient exception throwing with context
THROW_UI_EXCEPTION_WITH_CONTEXT(
    PropertyBindingException,
    "Widget property binding setup",
    "opacity", "Property not found"
);

// Exception throwing with suggestion
THROW_UI_EXCEPTION_WITH_SUGGESTION(
    ComponentCreationException,
    "Try initializing Qt application first",
    "QWidget", "No QApplication instance"
);

// Safe execution macro
auto result = SAFE_UI_EXECUTE(
    []() { return riskyOperation(); },
    [](const UIException& e) { HANDLE_UI_EXCEPTION_QT(e); }
);
```

## Best Practices

### Exception Design Guidelines

1. **Use Specific Exception Types**: Choose the most specific exception type available
2. **Provide Context**: Always include relevant context information
3. **Suggest Solutions**: When possible, provide actionable suggestions
4. **Chain Related Errors**: Use exception chaining for complex failure scenarios

### Error Handling Patterns

```cpp
// Good: Specific exception with context and suggestion
throw PropertyBindingException("text")
    .withContext("LineEdit component initialization")
    .withSuggestion("Ensure the target widget has a 'text' property");

// Good: Safe execution with fallback
auto config = safeExecute<Config>(
    []() { return loadConfiguration(); },
    [](const UIException& e) { logException(e); }
).value_or(getDefaultConfiguration());

// Good: Exception validation for recovery decisions
catch (const UIException& e) {
    if (isRecoverableException(e)) {
        attemptRecovery();
    } else {
        reportCriticalError(e);
    }
}
```

### Performance Considerations

- **Lazy Message Formatting**: Exception messages are formatted only when accessed
- **Minimal Overhead**: Exception creation is optimized for the non-error path
- **Thread Safety**: All exception utilities are thread-safe
- **Memory Efficiency**: Smart pointer usage for exception chaining

## Integration with Framework Modules

### Core Module Integration

```cpp
// UIElement uses exceptions for initialization errors
void UIElement::initialize() {
    try {
        createWidget();
    } catch (const std::exception& e) {
        throw ComponentCreationException(getComponentType(), e.what());
    }
}
```

### JSON Module Integration

```cpp
// JSONUILoader provides detailed parsing errors
QWidget* JSONUILoader::loadFromFile(const QString& filePath) {
    try {
        return parseAndCreateWidget(filePath);
    } catch (const QJsonParseError& e) {
        throw JSONParsingException(filePath.toStdString(), e.errorString().toStdString());
    }
}
```

### State Management Integration

```cpp
// StateManager validates state operations
template<typename T>
void StateManager::setState(const QString& key, const T& value) {
    if (key.isEmpty()) {
        throw StateManagementException(key.toStdString(), "State key cannot be empty");
    }
    // ... implementation
}
```

## Dependencies

- **C++ Standard Library**: For standard exception interface
- **Qt6**: Core module for QString and Qt-specific integrations
- **Internal**: Used by all other DeclarativeUI modules

## Testing

Exception handling is thoroughly tested:

```bash
cd build
ctest -R Exceptions
```

## Future Enhancements

- **Exception Analytics**: Automatic exception pattern analysis
- **Recovery Strategies**: Pluggable recovery mechanism system
- **Internationalization**: Localized error messages
- **Remote Reporting**: Optional crash reporting integration
