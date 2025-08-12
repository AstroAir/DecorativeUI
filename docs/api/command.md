# Command System API Reference

The Command module provides a comprehensive command-based UI architecture with declarative UI construction, state management integration, event handling, and seamless widget mapping. This advanced system enables building UIs without direct QWidget dependencies while maintaining full Qt compatibility and providing powerful integration capabilities.

## Architecture Overview

The Command system introduces a sophisticated abstraction layer that separates UI logic from widget implementation:

### Core Components

- **Commands** represent UI controls without inheriting from QWidget, providing framework independence
- **CommandBuilder** provides fluent API for declarative UI construction with type safety
- **WidgetMapper** handles automatic Command-to-QWidget translation and bidirectional synchronization
- **State Binding** connects Commands to centralized reactive state management
- **Event System** provides abstract event handling with priorities, filtering, and async support
- **JSON Loading** enables dynamic UI creation from JSON definitions with validation
- **Integration Adapters** provide seamless migration from legacy Component systems

### Advanced Features

- **Asynchronous Command Execution** with progress tracking and cancellation
- **Command Composition** for building complex UI workflows
- **MVC Integration** for enterprise application patterns
- **Legacy System Adapters** for gradual migration strategies
- **Performance Monitoring** with detailed metrics and optimization recommendations
- **Exception Safety** with comprehensive error handling and recovery mechanisms

## Core Command Classes

### BaseUICommand

Base class for all UI commands, providing the foundation for the command-based UI system.

```cpp
class BaseUICommand : public QObject {
    Q_OBJECT

public:
    explicit BaseUICommand(const QString& commandType, QObject* parent = nullptr);
    virtual ~BaseUICommand() = default;

    // **Core properties**
    QString getCommandType() const;
    QUuid getId() const;

    // **State management**
    CommandState* getState() const;
    void setState(const QString& property, const QVariant& value);
    QVariant getState(const QString& property, const QVariant& defaultValue = QVariant{}) const;

    // **Hierarchy management**
    void addChild(std::shared_ptr<BaseUICommand> child);
    void removeChild(std::shared_ptr<BaseUICommand> child);
    std::vector<std::shared_ptr<BaseUICommand>> getChildren() const;
    BaseUICommand* getParent() const;

    // **Event handling**
    void addEventListener(const QString& eventType, std::function<void(const CommandEvent&)> handler);
    void removeEventListener(const QString& eventType);
    void dispatchEvent(const QString& eventType, const QVariantMap& data = {});

signals:
    void stateChanged(const QString& property, const QVariant& value);
    void childAdded(std::shared_ptr<BaseUICommand> child);
    void childRemoved(std::shared_ptr<BaseUICommand> child);
    void eventDispatched(const QString& eventType, const QVariantMap& data);
};
```

### CommandState

Manages the state of a command with property change notifications.

```cpp
class CommandState : public QObject {
    Q_OBJECT

public:
    explicit CommandState(QObject* parent = nullptr);

    // **Property management**
    void setProperty(const QString& name, const QVariant& value);
    QVariant getProperty(const QString& name, const QVariant& defaultValue = QVariant{}) const;
    bool hasProperty(const QString& name) const;
    QStringList getPropertyNames() const;

    // **Batch operations**
    void beginBatchUpdate();
    void endBatchUpdate();
    bool isBatchUpdateActive() const;

    // **Validation**
    void setValidator(const QString& property, std::function<bool(const QVariant&)> validator);
    bool validateProperty(const QString& property, const QVariant& value) const;

signals:
    void propertyChanged(const QString& name, const QVariant& value);
    void batchUpdateStarted();
    void batchUpdateEnded();
    void validationFailed(const QString& property, const QVariant& value);
};
```

## CommandBuilder

Fluent interface for declarative UI construction with method chaining.

### Header

```cpp
#include "Command/CommandBuilder.hpp"
```

### Basic Usage

```cpp
// Create a simple button
auto button = CommandBuilder("Button")
    .text("Click Me")
    .enabled(true)
    .tooltip("This is a button")
    .onClick([](){ qDebug() << "Button clicked!"; })
    .build();

// Create with automatic widget creation
auto [command, widget] = CommandBuilder("Button")
    .text("Save File")
    .style("background-color: #3498db; color: white;")
    .buildWithWidget();
```

### Core Methods

#### Construction

```cpp
CommandBuilder(const QString& commandType);
```

#### Property Setting

```cpp
CommandBuilder& text(const QString& text);
CommandBuilder& enabled(bool enabled);
CommandBuilder& visible(bool visible);
CommandBuilder& tooltip(const QString& tooltip);
CommandBuilder& style(const QString& styleSheet);
CommandBuilder& style(const QString& property, const QString& value);
CommandBuilder& property(const QString& name, const QVariant& value);
```

#### Layout and Positioning

```cpp
CommandBuilder& layout(const QString& layoutType);
CommandBuilder& spacing(int spacing);
CommandBuilder& margins(int margin);
CommandBuilder& margins(int left, int top, int right, int bottom);
CommandBuilder& alignment(Qt::Alignment alignment);
```

#### Event Handling

```cpp
CommandBuilder& onClick(std::function<void()> handler);
CommandBuilder& onTextChanged(std::function<void(const QString&)> handler);
CommandBuilder& onValueChanged(std::function<void(const QVariant&)> handler);
CommandBuilder& addEventListener(const QString& eventType, std::function<void(const CommandEvent&)> handler);
```

#### State Binding

```cpp
CommandBuilder& bindToState(const QString& stateKey, const QString& property = "value");
CommandBuilder& bindProperty(const QString& property, const QString& stateKey);
CommandBuilder& bindTwoWay(const QString& property, const QString& stateKey);
```

#### Child Management

```cpp
CommandBuilder& child(const CommandBuilder& childBuilder);
CommandBuilder& child(std::shared_ptr<BaseUICommand> child);
CommandBuilder& children(const std::vector<CommandBuilder>& childBuilders);
```

#### Build Methods

```cpp
std::shared_ptr<BaseUICommand> build();
std::pair<std::shared_ptr<BaseUICommand>, std::unique_ptr<QWidget>> buildWithWidget();
std::unique_ptr<CommandUIElement> buildAsUIElement();
```

### Advanced Features

#### Validation

```cpp
CommandBuilder& validator(const QString& property, std::function<bool(const QVariant&)> validator);
CommandBuilder& required(bool required = true);
```

#### Actions and MVC Integration

```cpp
CommandBuilder& registerAsAction(const QString& actionName);
CommandBuilder& connectToAction(const QString& actionName);
```

#### Configuration

```cpp
CommandBuilder& config(const BuilderConfig& config);
CommandBuilder& autoInitialize(bool enable);
CommandBuilder& autoMVCIntegration(bool enable);
```

## CommandHierarchyBuilder

Specialized builder for creating complex UI hierarchies with containers and layouts.

### Header

```cpp
#include "Command/CommandBuilder.hpp"
```

### Basic Usage

```cpp
// Create a container with multiple children
auto container = CommandHierarchyBuilder("Container")
    .layout("VBox")
    .spacing(10)
    .margins(20)
    .addChild("Label", [](CommandBuilder& label) {
        label.text("Welcome to DeclarativeUI")
             .style("font-weight: bold; font-size: 18px;");
    })
    .addChild("Button", [](CommandBuilder& button) {
        button.text("Get Started")
              .onClick([]() { qDebug() << "Getting started!"; });
    })
    .build();
```

### Core Methods

#### Construction

```cpp
CommandHierarchyBuilder(const QString& containerType = "Container");
```

#### Container Configuration

```cpp
CommandHierarchyBuilder& layout(const QString& layoutType);
CommandHierarchyBuilder& spacing(int spacing);
CommandHierarchyBuilder& margins(int margin);
CommandHierarchyBuilder& margins(int left, int top, int right, int bottom);
CommandHierarchyBuilder& style(const QString& styleSheet);
```

#### Child Management

```cpp
// Add child with configuration function
CommandHierarchyBuilder& addChild(const QString& childType,
    std::function<void(CommandBuilder&)> configurator);

// Add pre-built command
CommandHierarchyBuilder& addChild(std::shared_ptr<BaseUICommand> child);

// Add child builder
CommandHierarchyBuilder& addChild(const CommandBuilder& childBuilder);
```

#### Grid Layout Support

```cpp
CommandHierarchyBuilder& addToGrid(const CommandBuilder& child, int row, int column);
CommandHierarchyBuilder& addToGrid(const CommandBuilder& child, int row, int column,
                                  int rowSpan, int columnSpan);
CommandHierarchyBuilder& setGridSpacing(int spacing);
```

#### Form Layout Support

```cpp
CommandHierarchyBuilder& addFormRow(const QString& label, const CommandBuilder& field);
CommandHierarchyBuilder& addFormRow(const CommandBuilder& label, const CommandBuilder& field);
```

#### Build Methods

```cpp
std::shared_ptr<BaseUICommand> build();
std::pair<std::shared_ptr<BaseUICommand>, std::unique_ptr<QWidget>> buildWithWidget();
```

### Advanced Examples

#### Complex Dashboard

```cpp
auto dashboard = CommandHierarchyBuilder("Container")
    .layout("VBox")
    .spacing(15)
    .margins(20)

    // Header section
    .addChild("Label", [](CommandBuilder& header) {
        header.text("📊 Application Dashboard")
              .style("font-size: 24px; font-weight: bold; color: #2c3e50;")
              .alignment(Qt::AlignCenter);
    })

    // Statistics section
    .addChild("Container", [](CommandBuilder& statsContainer) {
        statsContainer.layout("HBox")
                     .spacing(20)
                     .child(CommandBuilder("Label")
                           .text("👥 Users: 1,234")
                           .style("background: #3498db; color: white; padding: 15px; border-radius: 5px;"))
                     .child(CommandBuilder("Label")
                           .text("💰 Revenue: $12,345")
                           .style("background: #2ecc71; color: white; padding: 15px; border-radius: 5px;"));
    })

    // Action buttons
    .addChild("Container", [](CommandBuilder& buttonContainer) {
        buttonContainer.layout("HBox")
                      .spacing(10)
                      .child(CommandBuilder("Button")
                            .text("📈 Generate Report")
                            .onClick([]() { qDebug() << "Generating report..."; }))
                      .child(CommandBuilder("Button")
                            .text("🔄 Refresh Data")
                            .onClick([]() { qDebug() << "Refreshing data..."; }));
    })
    .build();
```

#### Form with Validation

```cpp
auto form = CommandHierarchyBuilder("Container")
    .layout("Form")
    .spacing(10)
    .addFormRow("Name:", CommandBuilder("TextInput")
        .placeholder("Enter your name")
        .required(true)
        .validator("text", [](const QVariant& value) {
            return !value.toString().trimmed().isEmpty();
        }))
    .addFormRow("Email:", CommandBuilder("TextInput")
        .placeholder("Enter your email")
        .validator("text", [](const QVariant& value) {
            return value.toString().contains("@");
        }))
    .addChild("Button", [](CommandBuilder& submit) {
        submit.text("Submit Form")
              .onClick([]() { qDebug() << "Form submitted!"; });
    })
    .build();
```

## WidgetMapper

Handles Command-to-QWidget translation and synchronization, enabling Commands to work seamlessly with Qt's widget system.

### Header

```cpp
#include "Command/WidgetMapper.hpp"
```

### Core Functionality

The WidgetMapper provides bidirectional synchronization between Commands and QWidgets:

- **Widget Creation**: Creates QWidget instances from Commands
- **Property Synchronization**: Keeps Command state and widget properties in sync
- **Event Mapping**: Routes widget events to Command event handlers
- **Lifecycle Management**: Manages widget creation, updates, and destruction

### Basic Usage

```cpp
// Create widget from command
auto button = CommandBuilder("Button").text("Click Me").build();
auto widget = WidgetMapper::instance().createWidget(button.get());

// Widget is automatically synchronized with command state
button->setState("text", "Updated Text");  // Widget text updates automatically
```

### Core Methods

#### Singleton Access

```cpp
static WidgetMapper& instance();
```

#### Widget Mapping Registration

```cpp
// Register mapping for custom widget types
template<typename WidgetType>
void registerMapping(const QString& command_type);

// Register with custom configuration
void registerMapping(const QString& command_type, const WidgetMappingConfig& config);
```

#### Widget Creation and Management

```cpp
// Create widget from command
std::unique_ptr<QWidget> createWidget(BaseUICommand* command);

// Destroy widget and clean up bindings
void destroyWidget(BaseUICommand* command);

// Get widget associated with command
QWidget* getWidget(BaseUICommand* command) const;
```

#### Synchronization Management

```cpp
// Establish bidirectional binding between command and widget
void establishBinding(BaseUICommand* command, QWidget* widget);

// Remove binding
void removeBinding(BaseUICommand* command);

// Manual synchronization
void syncCommandToWidget(BaseUICommand* command);
void syncWidgetToCommand(BaseUICommand* command);
```

### Widget Mapping Configuration

```cpp
struct WidgetMappingConfig {
    QString widget_type;
    std::function<std::unique_ptr<QWidget>()> factory;
    std::vector<PropertyMappingConfig> property_mappings;
    std::vector<EventMappingConfig> event_mappings;
    std::function<void(QWidget*, BaseUICommand*)> custom_setup;
};

// Property mapping configuration
struct PropertyMappingConfig {
    QString command_property;
    QString widget_property;
    BindingDirection direction = BindingDirection::TwoWay;
    std::function<QVariant(const QVariant&)> command_to_widget_converter;
    std::function<QVariant(const QVariant&)> widget_to_command_converter;
};

// Event mapping configuration
struct EventMappingConfig {
    QString widget_signal;
    QString command_event;
    std::function<QVariantMap(const QVariantList&)> parameter_mapper;
};
```

### Registration Examples

#### Basic Widget Registration

```cpp
// Register QPushButton for "Button" commands
WidgetMapper::instance().registerMapping<QPushButton>("Button");

// Register QLineEdit for "TextInput" commands
WidgetMapper::instance().registerMapping<QLineEdit>("TextInput");
```

#### Custom Widget Registration

```cpp
// Register custom widget with specific configuration
WidgetMappingConfig config;
config.widget_type = "CustomSlider";
config.factory = []() -> std::unique_ptr<QWidget> {
    return std::make_unique<QSlider>(Qt::Horizontal);
};

// Configure property mappings
config.property_mappings.push_back({
    "value",           // Command property
    "value",           // Widget property
    BindingDirection::TwoWay
});

config.property_mappings.push_back({
    "minimum",         // Command property
    "minimum",         // Widget property
    BindingDirection::CommandToWidget
});

// Configure event mappings
config.event_mappings.push_back({
    "valueChanged(int)",    // Widget signal
    "valueChanged",         // Command event
    [](const QVariantList& args) -> QVariantMap {
        return {{"value", args.first()}};
    }
});

// Custom setup function
config.custom_setup = [](QWidget* widget, BaseUICommand* command) {
    auto slider = qobject_cast<QSlider*>(widget);
    if (slider) {
        slider->setRange(0, 100);
        slider->setTickPosition(QSlider::TicksBelow);
    }
};

WidgetMapper::instance().registerMapping("Slider", config);
```

### Advanced Features

#### Batch Operations

```cpp
// Disable synchronization during batch updates
WidgetMapper::instance().setSyncEnabled(false);

// Perform multiple command updates
command1->setState("text", "New Text 1");
command2->setState("text", "New Text 2");
command3->setState("enabled", false);

// Re-enable and sync all at once
WidgetMapper::instance().setSyncEnabled(true);
WidgetMapper::instance().syncAll();
```

#### Error Handling

```cpp
// Set error handler for mapping failures
WidgetMapper::instance().setErrorHandler([](BaseUICommand* command, const QString& error) {
    qWarning() << "Widget mapping error for" << command->getCommandType() << ":" << error;
});
```

#### Performance Monitoring

```cpp
// Enable performance monitoring
WidgetMapper::instance().setPerformanceMonitoringEnabled(true);

// Get performance metrics
auto metrics = WidgetMapper::instance().getPerformanceMetrics();
qDebug() << "Average sync time:" << metrics.average_sync_time_ms << "ms";
qDebug() << "Total widgets created:" << metrics.widgets_created;
```

## State Binding System

The state binding system connects Commands to the centralized StateManager, enabling reactive UI updates and data synchronization.

### Header

```cpp
#include "Command/CommandBinding.hpp"
```

### Core Concepts

State binding provides automatic synchronization between Command properties and StateManager state:

- **Bidirectional Binding**: Changes in either Command or StateManager are reflected in both
- **Type Safety**: Automatic type conversion between Command properties and state values
- **Batch Updates**: Efficient handling of multiple state changes
- **Validation**: State validation before applying changes

### Basic Usage

```cpp
// Create command with state binding
auto input = CommandBuilder("TextInput")
    .bindToState("user.name", "text")  // Bind text property to user.name state
    .build();

// Changes to state automatically update the command
StateManager::instance().setState("user.name", "John Doe");
// Command text property is now "John Doe"

// Changes to command automatically update state
input->setState("text", "Jane Smith");
// StateManager state "user.name" is now "Jane Smith"
```

### Binding Configuration

```cpp
struct StateBindingConfig {
    QString command_property;
    QString state_key;
    BindingDirection direction = BindingDirection::TwoWay;
    std::function<QVariant(const QVariant&)> command_to_state_converter;
    std::function<QVariant(const QVariant&)> state_to_command_converter;
};

enum class BindingDirection {
    OneWay,              // State to Command only
    OneWayToSource,      // Command to State only
    TwoWay               // Bidirectional
};
```

### CommandBindingManager

Central manager for all Command-State bindings.

#### Core Methods

```cpp
// Singleton access
static CommandBindingManager& instance();

// State binding management
void addStateBinding(BaseUICommand* command, const StateBindingConfig& config);
void removeStateBinding(BaseUICommand* command, const QString& property);
void removeAllStateBindings(BaseUICommand* command);

// Manual synchronization
void syncCommandToState(BaseUICommand* command, const QString& property = "");
void syncStateToCommand(BaseUICommand* command, const QString& property = "");

// Batch operations
void beginBatchUpdate();
void endBatchUpdate();
```

### Advanced Binding Examples

#### Custom Type Conversion

```cpp
// Bind slider value (0-100) to volume state (0.0-1.0)
StateBindingConfig volumeBinding;
volumeBinding.command_property = "value";
volumeBinding.state_key = "audio.volume";
volumeBinding.direction = BindingDirection::TwoWay;

// Convert slider value to volume (0-100 -> 0.0-1.0)
volumeBinding.command_to_state_converter = [](const QVariant& value) {
    return value.toInt() / 100.0;
};

// Convert volume to slider value (0.0-1.0 -> 0-100)
volumeBinding.state_to_command_converter = [](const QVariant& value) {
    return static_cast<int>(value.toDouble() * 100);
};

CommandBindingManager::instance().addStateBinding(slider.get(), volumeBinding);
```

#### Conditional Binding

```cpp
// Bind button enabled state based on form validation
auto submitButton = CommandBuilder("Button")
    .text("Submit")
    .build();

// Create conditional binding
StateBindingConfig enabledBinding;
enabledBinding.command_property = "enabled";
enabledBinding.state_key = "form.valid";
enabledBinding.direction = BindingDirection::OneWay;

// Only enable button when form is valid
enabledBinding.state_to_command_converter = [](const QVariant& value) {
    return value.toBool();
};

CommandBindingManager::instance().addStateBinding(submitButton.get(), enabledBinding);
```

### Convenience Functions

```cpp
// Simple bidirectional binding
bindToState(command.get(), "text", "user.name");

// One-way binding from state to command
bindToState(command.get(), "enabled", "form.valid", BindingDirection::OneWay);

// Bind multiple properties
bindProperties(command.get(), {
    {"text", "user.name"},
    {"enabled", "form.valid"},
    {"visible", "ui.showAdvanced"}
});
```

## Command Event System

Abstract event system that works at the Command level, providing loose coupling and flexible event handling.

### Header

```cpp
#include "Command/CommandEvents.hpp"
```

### Core Concepts

The Command event system provides:

- **Abstract Events**: Events that work independently of Qt's signal/slot system
- **Event Filtering**: Filter events based on type, source, or custom criteria
- **Event Priorities**: Control event handler execution order
- **Event Queuing**: Queue events for batch processing
- **Global Interceptors**: Intercept all events for logging or debugging

### Event Types

```cpp
enum class CommandEventType {
    Click,
    TextChanged,
    ValueChanged,
    StateChanged,
    FocusGained,
    FocusLost,
    Custom
};
```

### CommandEvent Class

```cpp
class CommandEvent {
public:
    explicit CommandEvent(CommandEventType type, BaseUICommand* source = nullptr);

    // Event properties
    CommandEventType getType() const;
    QString getTypeName() const;
    BaseUICommand* getSource() const;
    QDateTime getTimestamp() const;
    QUuid getId() const;

    // Event data
    void setData(const QString& key, const QVariant& value);
    QVariant getData(const QString& key, const QVariant& defaultValue = QVariant{}) const;
    bool hasData(const QString& key) const;
    QStringList getDataKeys() const;

    // Event control
    void accept();
    void ignore();
    bool isAccepted() const;

    void stopPropagation();
    bool isPropagationStopped() const;
};
```

### Basic Event Handling

```cpp
// Register event handler on command
auto button = CommandBuilder("Button")
    .text("Click Me")
    .addEventListener("click", [](const CommandEvent& event) {
        qDebug() << "Button clicked at" << event.getTimestamp();

        // Access event data
        auto position = event.getData("position").toPoint();
        auto modifiers = event.getData("modifiers").value<Qt::KeyboardModifiers>();

        qDebug() << "Click position:" << position;
        qDebug() << "Modifiers:" << modifiers;
    })
    .build();

// Dispatch custom event
CommandEvent customEvent(CommandEventType::Custom, button.get());
customEvent.setData("action", "custom_action");
customEvent.setData("value", 42);
button->dispatchEvent("custom", customEvent.getData());
```

### CommandEventDispatcher

Central event dispatcher for managing event routing and handling.

#### Core Methods

```cpp
// Singleton access
static CommandEventDispatcher& instance();

// Event dispatching
void dispatchEvent(std::unique_ptr<CommandEvent> event);
void dispatchEvent(const CommandEvent& event);

// Event handler registration
QUuid registerHandler(BaseUICommand* command, CommandEventType eventType,
                     CommandEventHandler handler,
                     CommandEventPriority priority = CommandEventPriority::Normal);

QUuid registerGlobalHandler(CommandEventType eventType, CommandEventHandler handler);

// Handler management
void unregisterHandler(const QUuid& handlerId);
void unregisterAllHandlers(BaseUICommand* command);

// Event filtering
void addGlobalFilter(CommandEventFilter filter,
                    CommandEventPriority priority = CommandEventPriority::Normal);
void removeGlobalFilter(CommandEventFilter filter);

// Event interception
void setInterceptor(CommandEventType eventType, CommandEventHandler interceptor);
void removeInterceptor(CommandEventType eventType);
```

### Advanced Event Handling

#### Event Priorities

```cpp
enum class CommandEventPriority {
    Highest = 0,
    High = 25,
    Normal = 50,
    Low = 75,
    Lowest = 100
};

// Register high-priority handler
CommandEventDispatcher::instance().registerHandler(
    command.get(),
    CommandEventType::Click,
    [](const CommandEvent& event) {
        qDebug() << "High priority click handler";
    },
    CommandEventPriority::High
);
```

#### Event Filtering

```cpp
// Add global filter to log all events
CommandEventDispatcher::instance().addGlobalFilter(
    [](const CommandEvent& event) -> bool {
        qDebug() << "Event:" << event.getTypeName()
                 << "from" << event.getSource()->getCommandType();
        return true;  // Allow event to continue
    }
);

// Filter events based on source type
CommandEventDispatcher::instance().addGlobalFilter(
    [](const CommandEvent& event) -> bool {
        auto source = event.getSource();
        if (source && source->getCommandType() == "Button") {
            return true;  // Allow button events
        }
        return false;  // Block non-button events
    }
);
```

#### Custom Event Types

```cpp
// Define custom event
class CustomValidationEvent : public CommandEvent {
public:
    CustomValidationEvent(BaseUICommand* source, bool isValid, const QString& message)
        : CommandEvent(CommandEventType::Custom, source) {
        setData("isValid", isValid);
        setData("message", message);
        setData("eventSubtype", "validation");
    }

    bool isValid() const {
        return getData("isValid").toBool();
    }

    QString getMessage() const {
        return getData("message").toString();
    }
};

// Dispatch custom event
auto validationEvent = std::make_unique<CustomValidationEvent>(
    input.get(), false, "Email format is invalid"
);
CommandEventDispatcher::instance().dispatchEvent(std::move(validationEvent));
```

#### Event Queuing

```cpp
// Enable event queuing for batch processing
CommandEventDispatcher::instance().setEventQueueEnabled(true);
CommandEventDispatcher::instance().setMaxQueueSize(1000);

// Events are now queued instead of processed immediately
button->dispatchEvent("click", {});
input->dispatchEvent("textChanged", {{"text", "new value"}});

// Process all queued events
CommandEventDispatcher::instance().processEventQueue();
```

## JSON Command Loading

Load Command hierarchies from JSON definitions, enabling dynamic UI creation and configuration.

### Header

```cpp
#include "Command/Adapters/JSONCommandLoader.hpp"
```

### JSONCommandLoader Class

Loads Command hierarchies from JSON with support for state binding, event handling, and MVC integration.

#### Core Methods

```cpp
// Load from file
std::shared_ptr<BaseUICommand> loadCommandFromFile(const QString& filePath);

// Load from string
std::shared_ptr<BaseUICommand> loadCommandFromString(const QString& jsonString);

// Load from JSON object
std::shared_ptr<BaseUICommand> loadCommandFromObject(const QJsonObject& jsonObject);

// Load with automatic widget creation
std::pair<std::shared_ptr<BaseUICommand>, std::unique_ptr<QWidget>>
loadCommandWithWidgetFromFile(const QString& filePath);
```

#### Configuration

```cpp
// Enable automatic features
loader.setAutoStateBinding(true);
loader.setAutoMVCIntegration(true);
loader.setAutoWidgetCreation(true);

// Set validation mode
loader.setValidationMode(JSONValidationMode::Strict);

// Register custom event handlers
loader.registerEventHandler("customAction", [](const CommandEvent& event) {
    qDebug() << "Custom action triggered";
});
```

### JSON Format

#### Basic Command Definition

```json
{
  "type": "Button",
  "properties": {
    "text": "Click Me",
    "enabled": true,
    "tooltip": "This is a button"
  },
  "style": {
    "background-color": "#3498db",
    "color": "white",
    "padding": "10px",
    "border-radius": "5px"
  },
  "events": {
    "click": "handleButtonClick"
  },
  "bindings": {
    "enabled": "ui.button.enabled",
    "text": "ui.button.text"
  }
}
```

#### Container with Children

```json
{
  "type": "Container",
  "properties": {
    "layout": "VBox",
    "spacing": 10,
    "margins": [20, 20, 20, 20]
  },
  "children": [
    {
      "type": "Label",
      "properties": {
        "text": "Welcome to DeclarativeUI",
        "alignment": "center"
      },
      "style": {
        "font-size": "18px",
        "font-weight": "bold"
      }
    },
    {
      "type": "Button",
      "properties": {
        "text": "Get Started"
      },
      "events": {
        "click": "startApplication"
      }
    }
  ]
}
```

#### Form with Validation

```json
{
  "type": "Container",
  "properties": {
    "layout": "Form"
  },
  "children": [
    {
      "type": "TextInput",
      "id": "nameInput",
      "properties": {
        "placeholder": "Enter your name",
        "required": true
      },
      "bindings": {
        "text": "form.name"
      },
      "validation": {
        "required": true,
        "minLength": 2,
        "pattern": "^[a-zA-Z\\s]+$"
      },
      "events": {
        "textChanged": "validateName"
      }
    },
    {
      "type": "TextInput",
      "id": "emailInput",
      "properties": {
        "placeholder": "Enter your email"
      },
      "bindings": {
        "text": "form.email"
      },
      "validation": {
        "required": true,
        "pattern": "^[^@]+@[^@]+\\.[^@]+$"
      }
    },
    {
      "type": "Button",
      "properties": {
        "text": "Submit"
      },
      "bindings": {
        "enabled": "form.valid"
      },
      "events": {
        "click": "submitForm"
      }
    }
  ]
}
```

### Advanced Features

#### State Binding Configuration

```json
{
  "type": "Slider",
  "properties": {
    "minimum": 0,
    "maximum": 100,
    "value": 50
  },
  "bindings": {
    "value": {
      "stateKey": "audio.volume",
      "direction": "TwoWay",
      "converter": {
        "toState": "value / 100.0",
        "fromState": "value * 100"
      }
    }
  }
}
```

#### Custom Event Handlers

```json
{
  "type": "Button",
  "properties": {
    "text": "Custom Action"
  },
  "events": {
    "click": {
      "handler": "customHandler",
      "parameters": {
        "action": "save",
        "target": "document"
      }
    }
  }
}
```

#### Conditional Properties

```json
{
  "type": "Button",
  "properties": {
    "text": "Submit"
  },
  "conditionalProperties": {
    "enabled": {
      "condition": "form.valid === true",
      "value": true
    },
    "style": {
      "condition": "form.hasErrors === true",
      "value": "background-color: #e74c3c;"
    }
  }
}
```

### Usage Examples

#### Basic Loading

```cpp
// Load UI from JSON file
JSONCommandLoader loader;
auto command = loader.loadCommandFromFile("ui/main.json");

// Create widget
auto widget = WidgetMapper::instance().createWidget(command.get());
widget->show();
```

#### With Event Handlers

```cpp
JSONCommandLoader loader;

// Register event handlers
loader.registerEventHandler("handleButtonClick", [](const CommandEvent& event) {
    qDebug() << "Button clicked!";
});

loader.registerEventHandler("validateName", [](const CommandEvent& event) {
    QString text = event.getData("text").toString();
    bool valid = !text.trimmed().isEmpty();

    // Update validation state
    StateManager::instance().setState("form.name.valid", valid);
});

// Load and display UI
auto [command, widget] = loader.loadCommandWithWidgetFromFile("ui/form.json");
widget->show();
```

#### Integration with StateManager

```cpp
JSONCommandLoader loader;
loader.setAutoStateBinding(true);

// Initialize state
auto& state = StateManager::instance();
state.setState("form.name", QString(""));
state.setState("form.email", QString(""));
state.setState("form.valid", false);

// Load UI - bindings are automatically established
auto command = loader.loadCommandFromFile("ui/form.json");

// State changes automatically update UI
state.setState("form.name", "John Doe");  // Updates name input
```

## MVC Integration

The Command system integrates seamlessly with MVC patterns through the MVCIntegrationBridge.

### Header

```cpp
#include "Command/MVCIntegration.hpp"
```

### Core Features

- **Action Registration**: Register Commands as MVC actions
- **State Binding**: Connect Commands to centralized state management
- **Event Integration**: Route Command events to MVC controllers
- **Property Binding**: Bidirectional property synchronization

### Basic Usage

```cpp
// Register command as MVC action
auto saveButton = CommandBuilder("Button")
    .text("Save File")
    .build();

MVCIntegrationBridge::instance().registerCommandAsAction(saveButton, "file.save");

// Execute action through MVC system
MVCIntegrationBridge::instance().executeCommandAction("file.save");

// Bind command to state
MVCIntegrationBridge::instance().establishPropertyBinding(
    saveButton, "enabled", "document.canSave"
);
```

## Best Practices

### Command Design

1. **Single Responsibility**: Keep commands focused on single operations
2. **Meaningful Names**: Use descriptive command types and property names
3. **State Management**: Use state binding for reactive UI updates
4. **Event Handling**: Leverage the Command event system for loose coupling
5. **Validation**: Implement proper validation at the Command level

### Performance Optimization

1. **Batch Operations**: Use batch updates for multiple state changes
2. **Lazy Loading**: Create widgets only when needed
3. **Event Queuing**: Use event queuing for high-frequency events
4. **Memory Management**: Properly manage Command and widget lifecycles

### Error Handling

1. **Validation**: Validate inputs before applying changes
2. **Error Propagation**: Use proper error handling in event handlers
3. **Fallback Strategies**: Implement fallback behavior for failed operations
4. **Logging**: Use event interceptors for comprehensive logging

### Testing Strategies

1. **Unit Testing**: Test Commands in isolation from widgets
2. **State Testing**: Test state binding and synchronization
3. **Event Testing**: Test event handling and propagation
4. **Integration Testing**: Test complete Command-Widget-State integration

### Migration Guidelines

1. **Gradual Migration**: Start with hybrid mode for existing projects
2. **Component Wrapping**: Use adapters to wrap existing components
3. **State Consolidation**: Migrate to centralized state management
4. **Event Modernization**: Replace direct signal connections with Command events

## Example: Complete Application

```cpp
class ModernApplication : public QMainWindow {
    Q_OBJECT

public:
    ModernApplication(QWidget* parent = nullptr) : QMainWindow(parent) {
        setupState();
        setupUI();
        setupEventHandlers();
    }

private:
    void setupState() {
        auto& state = StateManager::instance();
        state.setState("app.title", QString("Modern DeclarativeUI App"));
        state.setState("user.name", QString(""));
        state.setState("user.email", QString(""));
        state.setState("form.valid", false);
    }

    void setupUI() {
        // Create UI using CommandHierarchyBuilder
        auto mainUI = CommandHierarchyBuilder("Container")
            .layout("VBox")
            .spacing(20)
            .margins(30)

            // Header
            .addChild("Label", [](CommandBuilder& header) {
                header.bindToState("app.title", "text")
                      .style("font-size: 24px; font-weight: bold; text-align: center;");
            })

            // User form
            .addChild("Container", [](CommandBuilder& form) {
                form.layout("Form")
                    .child(CommandBuilder("TextInput")
                          .placeholder("Enter your name")
                          .bindToState("user.name", "text")
                          .addEventListener("textChanged", "validateForm"))
                    .child(CommandBuilder("TextInput")
                          .placeholder("Enter your email")
                          .bindToState("user.email", "text")
                          .addEventListener("textChanged", "validateForm"));
            })

            // Action buttons
            .addChild("Container", [](CommandBuilder& buttons) {
                buttons.layout("HBox")
                       .spacing(10)
                       .child(CommandBuilder("Button")
                             .text("Submit")
                             .bindToState("form.valid", "enabled")
                             .addEventListener("click", "submitForm"))
                       .child(CommandBuilder("Button")
                             .text("Clear")
                             .addEventListener("click", "clearForm"));
            })
            .build();

        // Create widget and set as central widget
        auto widget = WidgetMapper::instance().createWidget(mainUI.get());
        setCentralWidget(widget.release());

        // Store command reference
        main_command_ = mainUI;
    }

    void setupEventHandlers() {
        auto& dispatcher = CommandEventDispatcher::instance();

        // Register event handlers
        dispatcher.registerGlobalHandler(CommandEventType::Custom,
            [this](const CommandEvent& event) {
                QString eventType = event.getData("eventType").toString();

                if (eventType == "validateForm") {
                    validateForm();
                } else if (eventType == "submitForm") {
                    submitForm();
                } else if (eventType == "clearForm") {
                    clearForm();
                }
            });
    }

    void validateForm() {
        auto& state = StateManager::instance();
        QString name = state.getState<QString>("user.name")->get();
        QString email = state.getState<QString>("user.email")->get();

        bool valid = !name.trimmed().isEmpty() && email.contains("@");
        state.setState("form.valid", valid);
    }

    void submitForm() {
        auto& state = StateManager::instance();
        QString name = state.getState<QString>("user.name")->get();
        QString email = state.getState<QString>("user.email")->get();

        qDebug() << "Form submitted - Name:" << name << "Email:" << email;
        QMessageBox::information(this, "Success", "Form submitted successfully!");
    }

    void clearForm() {
        auto& state = StateManager::instance();
        state.setState("user.name", QString(""));
        state.setState("user.email", QString(""));
        state.setState("form.valid", false);
    }

private:
    std::shared_ptr<BaseUICommand> main_command_;
};
```

This comprehensive Command system provides a modern, flexible approach to UI development in Qt applications while maintaining full compatibility with existing Qt widgets and patterns.

## Built-in Commands

The system includes several built-in commands for common operations.

### SetPropertyCommand

Sets a property on a QObject.

**Required Parameters:**

- `target`: Target object (QObject\*)
- `property`: Property name (QString)
- `value`: Property value (QVariant)

**Example:**

```cpp
CommandContext context;
context.setParameter("target", my_widget);
context.setParameter("property", "text");
context.setParameter("value", "New Text");

invoker.execute("set_property", context);
```

### UpdateStateCommand

Updates a value in the StateManager.

**Required Parameters:**

- `key`: State key (QString)
- `value`: New value (QVariant)

### SaveFileCommand / LoadFileCommand

File operations with dialog support.

**Parameters:**

- `file_path`: File path (QString, optional - shows dialog if not provided)
- `content`: Content to save (QByteArray, for save command)

### CopyToClipboardCommand / PasteFromClipboardCommand

Clipboard operations.

### ShowMessageCommand

Shows a message box.

**Required Parameters:**

- `title`: Message title (QString)
- `message`: Message text (QString)
- `type`: Message type (QString: "information", "warning", "critical")

## UI Integration

### CommandableUIElement

Extends UIElement with command capabilities.

```cpp
class CommandableUIElement : public Core::UIElement {
    Q_OBJECT

public:
    CommandableUIElement& onCommand(const QString& command_name, const CommandContext& context = {});
    CommandableUIElement& onShortcut(const QKeySequence& shortcut, const QString& command_name, const CommandContext& context = {});

    virtual CommandResult<QVariant> executeCommand(const QString& command_name, const CommandContext& context = {});
    virtual QFuture<CommandResult<QVariant>> executeCommandAsync(const QString& command_name, const CommandContext& context = {});
};
```

### Usage Example

```cpp
auto button = create<CommandableUIElement>()
    .onCommand("save_file")
    .onShortcut(QKeySequence::Save, "save_file")
    .build();
```

## Custom Command Implementation

### Basic Command

```cpp
class MyCommand : public ICommand {
    Q_OBJECT

public:
    explicit MyCommand(const CommandContext& context) : ICommand() {
        // Initialize from context
    }

    CommandResult<QVariant> execute(const CommandContext& context) override {
        try {
            // Perform command logic
            QString result = performOperation(context);
            return CommandResult<QVariant>::success(QVariant(result));
        } catch (const std::exception& e) {
            return CommandResult<QVariant>::error(QString::fromStdString(e.what()));
        }
    }

    CommandMetadata getMetadata() const override {
        CommandMetadata metadata;
        metadata.name = "my_command";
        metadata.description = "My custom command";
        metadata.category = "Custom";
        metadata.required_parameters = {"param1", "param2"};
        metadata.supports_undo = true;
        return metadata;
    }

    CommandResult<QVariant> undo(const CommandContext& context) override {
        // Implement undo logic
        return CommandResult<QVariant>::success(QVariant());
    }

private:
    QString performOperation(const CommandContext& context) {
        // Implementation
        return "Operation completed";
    }
};
```

### Async Command

```cpp
class MyAsyncCommand : public AsyncCommand {
    Q_OBJECT

public:
    QFuture<CommandResult<QVariant>> executeAsync(const CommandContext& context) override {
        return QtConcurrent::run([this, context]() -> CommandResult<QVariant> {
            // Long-running operation
            QThread::sleep(2); // Simulate work
            return CommandResult<QVariant>::success(QVariant("Async result"));
        });
    }

    CommandMetadata getMetadata() const override {
        CommandMetadata metadata;
        metadata.name = "my_async_command";
        metadata.is_async = true;
        return metadata;
    }
};
```

## Best Practices

### Command Design

- Keep commands focused on single operations
- Use meaningful command names and descriptions
- Implement proper error handling
- Support undo when possible
- Use appropriate command types (sync/async/transactional)

### Performance

- Use async commands for long-running operations
- Implement proper progress reporting
- Use batch execution for multiple related commands
- Monitor command execution times

### Error Handling

- Always return proper CommandResult objects
- Provide meaningful error messages
- Implement proper exception handling
- Use command interceptors for logging

### Testing

- Test command execution in isolation
- Test undo/redo functionality
- Test error conditions
- Use mock contexts for unit testing

## Integration Adapters

The Command system provides comprehensive integration adapters for seamless migration from legacy systems and integration with existing Qt applications.

### ComponentSystemAdapter

Provides bidirectional integration between the legacy Component system and the modern Command system.

#### Header

```cpp
#include "Command/Adapters/ComponentSystemAdapter.hpp"
```

#### Key Features

- **Component-to-Command Conversion**: Automatically convert existing Components to Commands
- **Hybrid Containers**: Mix Components and Commands in the same container
- **Bidirectional Synchronization**: Keep Components and Commands in sync
- **Gradual Migration**: Migrate applications incrementally without breaking changes

#### Usage Examples

##### Converting Components to Commands

```cpp
// Convert existing component to command
auto legacyButton = std::make_unique<Components::Button>();
legacyButton->text("Legacy Button");

auto commandButton = ComponentSystemAdapter::instance()
    .convertToCommand(legacyButton.get());

// Establish bidirectional synchronization
ComponentSystemAdapter::instance()
    .establishSync(legacyButton.get(), commandButton);
```

##### Hybrid Containers

```cpp
// Create hybrid container that supports both Components and Commands
auto hybrid = ComponentSystemAdapter::instance().createHybridContainer();
hybrid->setLayout("VBox");
hybrid->setSpacing(10);

// Add legacy component
auto legacyLabel = std::make_unique<Components::Label>();
legacyLabel->text("Legacy Label");
hybrid->addComponent(std::move(legacyLabel));

// Add command-based button
auto commandButton = CommandBuilder("Button")
    .text("Command Button")
    .onClick([]() { qDebug() << "Command button clicked!"; })
    .build();
hybrid->addCommand(commandButton);

// Convert to widget for display
auto widget = hybrid->toWidget();
widget->show();
```

### JSONCommandLoader

Enables loading Command hierarchies from JSON definitions with automatic widget creation.

#### Header

```cpp
#include "Command/Adapters/JSONCommandLoader.hpp"
```

#### Key Features

- **JSON-to-Command Conversion**: Load complete Command hierarchies from JSON
- **Automatic Widget Creation**: Generate QWidgets from Commands automatically
- **Event Handler Registration**: Register C++ event handlers for JSON-defined events
- **State Binding Integration**: Automatic state binding for JSON-defined Commands
- **Validation and Error Reporting**: Comprehensive JSON validation with detailed error messages
