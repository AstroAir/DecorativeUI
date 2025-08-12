# JSON Support API Reference

The JSON module provides comprehensive support for loading, parsing, validating, and managing UI definitions from JSON files. This enables data-driven UI creation and hot reloading capabilities.

## JSONUILoader Class

The `JSONUILoader` class is responsible for loading UI definitions from JSON sources and creating Qt widgets.

### Header

```cpp
#include "JSON/JSONUILoader.hpp"
```

### Class Declaration

```cpp
namespace DeclarativeUI::JSON {
    class JSONUILoader : public QObject {
        Q_OBJECT
    public:
        explicit JSONUILoader(QObject *parent = nullptr);
        ~JSONUILoader() override = default;

        // Loading methods...
    };
}
```

### Public Methods

#### UI Loading

##### `loadFromFile(const QString &file_path) -> std::unique_ptr<QWidget>`

Loads a UI definition from a JSON file.

**Parameters:**

- `file_path`: Path to the JSON file

**Returns:** Unique pointer to the created widget

**Throws:** `JSONParsingException` on parse errors, `JSONValidationException` on validation errors

**Example:**

```cpp
auto loader = std::make_unique<JSONUILoader>();
auto widget = loader->loadFromFile("ui/main_window.json");
```

##### `loadFromString(const QString &json_string) -> std::unique_ptr<QWidget>`

Loads a UI definition from a JSON string.

**Parameters:**

- `json_string`: JSON string containing the UI definition

**Returns:** Unique pointer to the created widget

**Example:**

```cpp
QString json = R"({
    "type": "QPushButton",
    "properties": {
        "text": "Click Me",
        "minimumSize": [100, 30]
    }
})";
auto widget = loader->loadFromString(json);
```

##### `loadFromObject(const QJsonObject &json_object) -> std::unique_ptr<QWidget>`

Loads a UI definition from a QJsonObject.

**Parameters:**

- `json_object`: JSON object containing the UI definition

**Returns:** Unique pointer to the created widget

#### Validation

##### `validateJSON(const QJsonObject &json_object) const -> bool`

Validates a JSON object against the UI schema.

**Parameters:**

- `json_object`: JSON object to validate

**Returns:** `true` if valid, `false` otherwise

**Example:**

```cpp
QJsonObject ui_def = /* ... */;
if (loader->validateJSON(ui_def)) {
    auto widget = loader->loadFromObject(ui_def);
}
```

#### State Management Integration

##### `bindStateManager(std::shared_ptr<Binding::StateManager> state_manager)`

Binds a state manager for property binding support.

**Parameters:**

- `state_manager`: Shared pointer to the state manager

**Example:**

```cpp
auto state_manager = std::make_shared<StateManager>();
loader->bindStateManager(state_manager);
```

#### Event Handler Registration

##### `registerEventHandler(const QString &handler_name, std::function<void()> handler)`

Registers an event handler that can be referenced in JSON.

**Parameters:**

- `handler_name`: Name to reference in JSON
- `handler`: Event handler function

**Example:**

```cpp
loader->registerEventHandler("onButtonClick", []() {
    qDebug() << "Button was clicked!";
});
```

#### Custom Property Converters

##### `registerPropertyConverter(const QString &property_type, std::function<QVariant(const QJsonValue &)> converter)`

Registers a custom property converter for specific property types.

**Parameters:**

- `property_type`: Property type identifier
- `converter`: Conversion function

**Example:**

```cpp
loader->registerPropertyConverter("color", [](const QJsonValue &value) {
    return QColor(value.toString());
});
```

### Signals

```cpp
signals:
    void loadingStarted(const QString &source);
    void loadingFinished(const QString &source);
    void loadingFailed(const QString &source, const QString &error);
```

### JSON Schema

The JSONUILoader expects JSON objects with the following structure:

```json
{
  "type": "QWidget",
  "id": "optional_id",
  "properties": {
    "property_name": "property_value"
  },
  "events": {
    "event_name": "handler_name"
  },
  "bindings": {
    "property_name": "state_key"
  },
  "layout": {
    "type": "QVBoxLayout",
    "spacing": 10,
    "margins": [10, 10, 10, 10]
  },
  "children": [
    {
      "type": "QPushButton",
      "properties": {
        "text": "Child Button"
      }
    }
  ]
}
```

## JSONParser Class

The `JSONParser` class provides advanced JSON parsing capabilities with support for comments, includes, and custom preprocessing.

### Header

```cpp
#include "JSON/JSONParser.hpp"
```

### Public Methods

#### Basic Parsing

##### `parseFile(const QString &file_path) -> QJsonObject`

Parses a JSON file with advanced features.

**Parameters:**

- `file_path`: Path to the JSON file

**Returns:** Parsed JSON object

##### `parseString(const QString &json_string) -> QJsonObject`

Parses a JSON string with preprocessing.

**Parameters:**

- `json_string`: JSON string to parse

**Returns:** Parsed JSON object

##### `parseUrl(const QUrl &url) -> QJsonObject`

Parses JSON from a URL (network or local).

**Parameters:**

- `url`: URL to fetch and parse

**Returns:** Parsed JSON object

#### Advanced Parsing

##### `parseWithContext(const QString &source, JSONParsingContext &context) -> QJsonObject`

Parses JSON with full context tracking.

**Parameters:**

- `source`: JSON source string
- `context`: Parsing context for error tracking

**Returns:** Parsed JSON object

#### Configuration

##### `setStrictMode(bool strict) -> JSONParser &`

Enables or disables strict parsing mode.

##### `setAllowComments(bool allow) -> JSONParser &`

Enables or disables comment support in JSON.

##### `setAllowTrailingCommas(bool allow) -> JSONParser &`

Enables or disables trailing comma support.

##### `setMaxDepth(int max_depth) -> JSONParser &`

Sets maximum parsing depth for security.

##### `setIncludeResolver(std::function<QString(const QString &)> resolver) -> JSONParser &`

Sets a custom include file resolver.

#### Utility Methods

##### `static formatJsonPath(const JSONPath &path) -> QString`

Formats a JSON path for error messages.

##### `static getValueAtPath(const QJsonObject &root, const JSONPath &path) -> QJsonValue`

Gets a value at a specific JSON path.

##### `static setValueAtPath(QJsonObject &root, const JSONPath &path, const QJsonValue &value) -> bool`

Sets a value at a specific JSON path.

### Usage Example

```cpp
JSONParser parser;
parser.setAllowComments(true)
      .setAllowTrailingCommas(true)
      .setStrictMode(false);

auto json_obj = parser.parseFile("ui/main.json");
```

## JSONValidator Class

The `JSONValidator` class provides comprehensive validation for UI JSON definitions.

### Header

```cpp
#include "JSON/JSONValidator.hpp"
```

### UIJSONValidator Class

#### Public Methods

##### `validate(const QJsonObject &ui_definition) -> bool`

Validates a complete UI definition.

**Parameters:**

- `ui_definition`: UI definition to validate

**Returns:** `true` if valid, `false` otherwise

##### `validateComponent(const QJsonObject &component, const QString &component_type = "") -> bool`

Validates a single component definition.

##### `validateLayout(const QJsonObject &layout) -> bool`

Validates a layout definition.

##### `validateProperties(const QJsonObject &properties, const QString &widget_type = "") -> bool`

Validates component properties.

##### `validateEvents(const QJsonObject &events) -> bool`

Validates event handler definitions.

##### `validateBindings(const QJsonObject &bindings) -> bool`

Validates property binding definitions.

#### Configuration

##### `setStrictMode(bool strict)`

Enables strict validation mode.

##### `setAllowAdditionalProperties(bool allow)`

Controls whether additional properties are allowed.

##### `addCustomValidator(const QString &rule_name, std::function<bool(const QJsonObject &, const JSONPath &)> validator)`

Adds a custom validation rule.

#### Results Access

##### `getValidationResults() const -> std::vector<ValidationResult>`

Gets all validation results.

##### `getErrors() const -> std::vector<ValidationResult>`

Gets only error results.

##### `getWarnings() const -> std::vector<ValidationResult>`

Gets only warning results.

### Usage Example

```cpp
UIJSONValidator validator;
validator.setStrictMode(true);

if (validator.validate(ui_definition)) {
    // UI definition is valid
    auto widget = loader->loadFromObject(ui_definition);
} else {
    // Handle validation errors
    for (const auto &error : validator.getErrors()) {
        qWarning() << "Validation error:" << error.message;
    }
}
```

## ComponentRegistry Class

The `ComponentRegistry` class manages registration and creation of UI components from JSON.

### Header

```cpp
#include "JSON/ComponentRegistry.hpp"
```

### Public Methods

#### Singleton Access

##### `static instance() -> ComponentRegistry &`

Gets the global component registry instance.

#### Component Registration

##### `registerComponent<WidgetType>(const QString &type_name, std::function<std::unique_ptr<WidgetType>(const QJsonObject &)> factory)`

Registers a component factory function.

**Template Parameters:**

- `WidgetType`: Widget type to register

**Parameters:**

- `type_name`: Type name for JSON reference
- `factory`: Factory function that creates the widget

**Example:**

```cpp
ComponentRegistry::instance().registerComponent<CustomButton>(
    "CustomButton",
    [](const QJsonObject &config) {
        auto button = std::make_unique<CustomButton>();
        // Configure button from config...
        return button;
    }
);
```

#### Component Creation

##### `createComponent(const QString &type_name, const QJsonObject &config) -> std::unique_ptr<QWidget>`

Creates a component instance from JSON configuration.

**Parameters:**

- `type_name`: Registered component type name
- `config`: JSON configuration object

**Returns:** Unique pointer to the created widget

#### Query Methods

##### `hasComponent(const QString &type_name) const noexcept -> bool`

Checks if a component type is registered.

##### `getRegisteredTypes() const -> QStringList`

Gets all registered component type names.

#### Utility

##### `clear() noexcept`

Clears all registered components (useful for testing).

### Built-in Components

The registry automatically registers these Qt widget types:

- `QWidget`, `QMainWindow`, `QDialog`
- `QPushButton`, `QLabel`, `QLineEdit`
- `QCheckBox`, `QRadioButton`, `QComboBox`
- `QSpinBox`, `QDoubleSpinBox`, `QSlider`
- `QProgressBar`, `QTextEdit`, `QPlainTextEdit`
- `QGroupBox`, `QTabWidget`, `QSplitter`
- `QTableView`, `QTreeView`, `QListView`

### Usage Example

```cpp
// Register a custom component
ComponentRegistry::instance().registerComponent<MyCustomWidget>(
    "MyCustomWidget",
    [](const QJsonObject &config) {
        auto widget = std::make_unique<MyCustomWidget>();

        // Apply configuration
        if (config.contains("customProperty")) {
            widget->setCustomProperty(
                config["customProperty"].toString()
            );
        }

        return widget;
    }
);

// Use in JSON
QString json = R"({
    "type": "MyCustomWidget",
    "properties": {
        "customProperty": "value"
    }
})";
```

## Error Handling

All JSON classes use a consistent exception hierarchy:

- `JSONException` - Base exception class
- `JSONParsingException` - JSON parsing errors
- `JSONValidationException` - Schema validation errors
- `ComponentCreationException` - Component creation errors
- `ComponentRegistrationException` - Component registration errors

### Exception Safety

- All methods provide strong exception safety guarantees
- RAII ensures proper resource cleanup
- Validation occurs before object creation
- Detailed error messages with context information

## Thread Safety

- `ComponentRegistry` is thread-safe for read operations
- `JSONUILoader` instances should be used from a single thread
- `JSONParser` and `JSONValidator` are stateless and thread-safe
- State manager integration requires main thread usage
