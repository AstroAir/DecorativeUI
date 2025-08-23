# JSON Module

The JSON module provides comprehensive JSON parsing, validation, and UI loading capabilities for DeclarativeUI applications. It enables declarative UI definition through JSON configuration files.

## Overview

This directory contains the core components for JSON-based UI functionality:

- **JSONParser**: Advanced JSON parsing with references, includes, and custom types
- **JSONValidator**: Comprehensive validation framework for UI JSON documents
- **JSONUILoader**: Load and instantiate UI components from JSON definitions
- **ComponentRegistry**: Type-safe component factory registration and creation

## Components

### JSONParser (`JSONParser.hpp/.cpp`)

High-level JSON parsing and preprocessing utility with advanced features:

- Parse JSON from files, strings, and URLs
- Support for JSON references and includes (local and external)
- Custom type parsers for application-specific values
- JSON Schema-like validation capabilities
- Comment and trailing comma support
- Comprehensive error reporting with path information

**Key Features:**

- **Reference Resolution**: Support for JSON references (`$ref`) and includes
- **Custom Parsers**: Extensible type-specific parsing
- **Preprocessing**: Comment removal and trailing comma handling
- **Validation**: Built-in JSON Schema-like validation
- **Error Handling**: Detailed error messages with JSON path context
- **Utility Functions**: Path navigation, value manipulation, pretty printing

**Core Classes:**

- `JSONPath`: Path navigation and manipulation
- `JSONReferenceResolver`: Reference and include resolution
- `JSONParser`: Main parsing interface
- `JSONSchemaValidator`: Schema-based validation
- Utility functions for common JSON operations

### JSONValidator (`JSONValidator.hpp/.cpp`)

Flexible validation framework for DeclarativeUI JSON documents:

- Express validation rules (predicate-based and class-based)
- Execute validation on UI JSON documents, components, properties, layouts, events
- Accumulate structured ValidationResult objects with path, severity, messages
- Register built-in validators and custom validators
- Integrate lightweight JSON Schema-like validation

**Key Features:**

- **Rule-Based Validation**: Flexible validation rule system
- **Component Validation**: UI component-specific validation
- **Property Validation**: Widget property compatibility checking
- **Layout Validation**: Layout structure validation
- **Event Validation**: Event binding validation
- **Custom Validators**: Extensible validation framework

**Built-in Validators:**

- `RequiredPropertyValidator`: Ensures required properties are present
- `PropertyTypeValidator`: Validates property value types
- `PropertyCompatibilityValidator`: Ensures properties are compatible with widget types
- `LayoutStructureValidator`: Validates layout hierarchies
- `EventBindingValidator`: Validates event handler bindings

### JSONUILoader (`JSONUILoader.hpp/.cpp`)

Load and instantiate QWidget-based UI components from JSON definitions:

- Create widget hierarchies from JSON configuration
- Apply properties, layouts, and styling
- Handle component composition and nesting
- Integrate with ComponentRegistry for type resolution
- Support for custom component types

**Key Features:**

- **Widget Creation**: Instantiate Qt widgets from JSON
- **Property Application**: Set widget properties from JSON
- **Layout Management**: Create and configure layouts
- **Component Composition**: Support for nested components
- **Error Handling**: Comprehensive error reporting
- **Extensibility**: Support for custom widget types

### ComponentRegistry (`ComponentRegistry.hpp/.cpp`)

Global registry mapping textual type names to component factories:

- Type-safe component factory registration
- Create widget instances by type name
- Query registered types and capabilities
- Built-in component support for standard Qt widgets
- Extensible factory system for custom components

**Key Features:**

- **Type Safety**: Template-based factory registration
- **Factory Pattern**: Flexible component creation
- **Built-in Components**: Standard Qt widget support
- **Custom Components**: Extensible for application-specific widgets
- **Thread Safety**: Safe for concurrent access

**Built-in Components:**

- QWidget, QLabel, QPushButton
- QLineEdit, QTextEdit, QCheckBox
- QRadioButton, QComboBox, QSpinBox
- QSlider, QProgressBar, QGroupBox
- Layout components (QVBoxLayout, QHBoxLayout, QGridLayout)

## Usage Examples

### Basic JSON UI Loading

```cpp
#include "JSON/JSONUILoader.hpp"

auto loader = std::make_unique<JSONUILoader>();

// Load UI from JSON file
std::unique_ptr<QWidget> widget = loader->loadFromFile("ui/main.json");

// Load UI from JSON string
QString json_content = R"({
    "type": "QWidget",
    "properties": {
        "windowTitle": "My Application",
        "geometry": [100, 100, 800, 600]
    },
    "children": [
        {
            "type": "QLabel",
            "properties": {
                "text": "Hello, World!",
                "alignment": "AlignCenter"
            }
        }
    ]
})";

std::unique_ptr<QWidget> widget2 = loader->loadFromString(json_content);
```

### JSON Validation

```cpp
#include "JSON/JSONValidator.hpp"

UIJSONValidator validator;
validator.registerBuiltinValidators();

// Validate UI definition
QJsonObject ui_definition = /* ... */;
if (validator.validate(ui_definition)) {
    qDebug() << "UI definition is valid";
} else {
    QStringList errors = validator.getValidationErrors();
    for (const QString& error : errors) {
        qWarning() << "Validation error:" << error;
    }
}

// Validate individual component
QJsonObject component = /* ... */;
if (validator.validateComponent(component, "QLabel")) {
    qDebug() << "Component is valid";
}
```

### Component Registration

```cpp
#include "JSON/ComponentRegistry.hpp"

// Register custom component
ComponentRegistry::instance().registerComponent<MyCustomWidget>(
    "MyCustomWidget",
    [](const QJsonObject& config) {
        auto widget = std::make_unique<MyCustomWidget>();
        // Configure widget from JSON
        return widget;
    }
);

// Create component instance
auto widget = ComponentRegistry::instance().createComponent(
    "MyCustomWidget",
    config_json
);
```

### Advanced JSON Parsing

```cpp
#include "JSON/JSONParser.hpp"

JSONParser parser;

// Enable advanced features
parser.setAllowComments(true);
parser.setAllowTrailingCommas(true);

// Register custom type parser
parser.registerCustomParser("Color", [](const QJsonValue& value) {
    // Parse custom color format
    return QJsonValue(/* processed color */);
});

// Parse with includes and references
QJsonObject result = parser.parseFile("ui/main.json");
```

## JSON Schema Support

The module supports a subset of JSON Schema for validation:

```json
{
  "type": "object",
  "required": ["type"],
  "properties": {
    "type": {
      "type": "string",
      "enum": ["QWidget", "QLabel", "QPushButton"]
    },
    "properties": {
      "type": "object"
    },
    "children": {
      "type": "array",
      "items": {
        "$ref": "#"
      }
    }
  }
}
```

## Error Handling

Comprehensive error handling with detailed context:

- JSON parsing errors with line/column information
- Validation errors with JSON path context
- Component creation errors with type information
- Property application errors with property names

## Performance Considerations

- Efficient JSON parsing with minimal memory allocation
- Lazy evaluation of references and includes
- Caching of parsed components and schemas
- Optimized validation with early termination

## Dependencies

- Qt6::Core
- Qt6::Widgets
- Qt6::Network (for URL-based includes)

## Testing

Unit tests are available in `tests/unit/test_json.cpp`:

```bash
cd build
ctest -R JSONTest
```

## Configuration Files

Example JSON UI definition:

```json
{
  "type": "QWidget",
  "properties": {
    "windowTitle": "Example Application",
    "geometry": [100, 100, 800, 600]
  },
  "layout": {
    "type": "QVBoxLayout",
    "children": [
      {
        "type": "QLabel",
        "properties": {
          "text": "Welcome to DeclarativeUI",
          "alignment": "AlignCenter"
        }
      },
      {
        "type": "QPushButton",
        "properties": {
          "text": "Click Me"
        },
        "events": {
          "clicked": "handleButtonClick"
        }
      }
    ]
  }
}
```
