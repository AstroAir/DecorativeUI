# Command-Based UI Architecture

## Overview

The Command-based UI architecture introduces a new abstraction layer that sits between the MVC pattern and Qt's QWidget system. This architecture provides complete decoupling from QWidget while maintaining extensibility and proper MVC separation.

## Core Concepts

### 1. Command Abstraction Layer

Commands represent UI controls without inheriting from QWidget, ensuring complete abstraction:

```cpp
// Traditional Qt approach
QPushButton* button = new QPushButton("Click Me");
connect(button, &QPushButton::clicked, this, &MyClass::onButtonClicked);

// Command-based approach
auto button = CommandBuilder("Button")
    .text("Click Me")
    .onClick([this]() { onButtonClicked(); })
    .build();
```

### 2. Dynamic Widget Mapping

The WidgetMapper creates corresponding QWidget instances based on Command types:

```cpp
// Command creation (abstract)
auto command = UICommandFactory::instance().createCommand("Button");

// Widget creation (concrete)
auto widget = WidgetMapper::instance().createWidget(command.get());
```

### 3. Bidirectional Binding

State synchronization between Command objects and QWidget instances:

```cpp
auto textInput = CommandBuilder("TextInput")
    .bindToState("user.name")  // Bidirectional binding to state
    .onTextChanged([](const QString& text) {
        qDebug() << "Text changed:" << text;
    })
    .build();
```

## Architecture Components

### BaseUICommand

The foundation class for all UI commands:

```cpp
class BaseUICommand : public QObject {
    // Pure abstraction - no QWidget inheritance
    virtual UICommandMetadata getMetadata() const = 0;
    virtual QString getCommandType() const = 0;
    virtual QString getWidgetType() const = 0;
    
    // State management
    UICommandState* getState() const;
    
    // Widget lifecycle (managed by WidgetMapper)
    virtual void onWidgetCreated(QWidget* widget);
    virtual void syncToWidget();
    virtual void syncFromWidget();
};
```

### UICommandFactory

Creates Command objects dynamically:

```cpp
// Register command types
factory.registerCommand<ButtonCommand>("Button", "QPushButton");

// Create commands
auto command = factory.createCommand("Button");
auto [command, widget] = factory.createCommandWithWidget("Button");
```

### WidgetMapper

Handles Command-to-QWidget translation:

```cpp
// Register widget mappings
mapper.registerMapping<QPushButton>("Button");

// Create widgets from commands
auto widget = mapper.createWidget(command.get());
```

### CommandBuilder

Fluent interface for declarative UI construction:

```cpp
auto form = CommandHierarchyBuilder("Container")
    .layout("VBox")
    .spacing(10)
    .addChild("Label", [](CommandBuilder& label) {
        label.text("Name:");
    })
    .addChild("TextInput", [](CommandBuilder& input) {
        input.placeholder("Enter name")
             .bindToState("form.name")
             .required("text");
    })
    .addChild("Button", [](CommandBuilder& button) {
        button.text("Submit")
              .onClick([]() { /* submit logic */ });
    })
    .build();
```

## Command Types

### Core Commands

- **ButtonCommand**: Clickable buttons
- **LabelCommand**: Text/image display
- **TextInputCommand**: Text input fields
- **MenuItemCommand**: Menu items and actions
- **ContainerCommand**: Layout containers

### Specialized Commands (Inheritance)

- **RadioButtonCommand** extends ButtonCommand
- **CheckBoxCommand** extends ButtonCommand
- **ToggleButtonCommand** extends ButtonCommand
- **ToolButtonCommand** extends ButtonCommand
- **SpinBoxCommand**: Numeric input
- **SliderCommand**: Value selection
- **ProgressBarCommand**: Progress indication

## MVC Integration

### State Management Integration

```cpp
// Bind command to StateManager
MVCIntegrationBridge::instance().bindCommandToStateManager(
    command, "user.preferences.theme", "value"
);

// Create command with automatic state binding
auto command = MVC::createBoundCommand("Slider", "volume.level");
```

### Action System Integration

```cpp
// Register command as action
MVCIntegrationBridge::instance().registerCommandAsAction(
    command, "file.save"
);

// Execute action through command system
MVCIntegrationBridge::instance().executeCommandAction("file.save");
```

## Event System

### Command Events

Abstract event system that works at the Command level:

```cpp
// Register event handlers
Events::onClick(button.get(), []() {
    qDebug() << "Button clicked!";
});

Events::onValueChanged(slider.get(), [](const QVariant& value) {
    qDebug() << "Slider value:" << value.toInt();
});

// Custom events
auto event = Events::createCustomEvent(command.get(), "dataLoaded");
event->setData("recordCount", 150);
Events::dispatch(std::move(event));
```

### Event Priorities and Filtering

```cpp
// High priority handler
CommandEventDispatcher::instance().registerHandler(
    command.get(), 
    CommandEventType::Clicked,
    [](const CommandEvent& event) { /* high priority logic */ },
    CommandEventPriority::High
);

// Filtered handler
CommandEventDispatcher::instance().registerFilteredHandler(
    command.get(),
    CommandEventType::ValueChanged,
    [](const CommandEvent& event) { /* handler */ },
    [](const CommandEvent& event) { 
        // Filter: only handle values > 50
        auto valueEvent = static_cast<const ValueChangeEvent&>(event);
        return valueEvent.getNewValue().toInt() > 50;
    }
);
```

## Benefits

### 1. Complete Decoupling

Commands are completely independent of QWidget implementation:

```cpp
// Command logic is widget-agnostic
class CustomCommand : public BaseUICommand {
    // No QWidget dependencies
    // Pure business logic
};
```

### 2. Extensibility Through Inheritance

```cpp
// Extend existing commands
class CustomButtonCommand : public ButtonCommand {
    // Add custom behavior while inheriting base functionality
    void setCustomProperty(const QString& value) {
        getState()->setProperty("customProp", value);
    }
};
```

### 3. Declarative UI Construction

```cpp
// Build complex UIs declaratively
auto dashboard = CommandHierarchyBuilder("Container")
    .layout("Grid")
    .addToGrid(Patterns::labeledInput("Name", "Enter name"), 0, 0)
    .addToGrid(Patterns::labeledInput("Email", "Enter email"), 1, 0)
    .addToGrid(Patterns::button("Save"), 2, 0)
    .build();
```

### 4. Automatic State Synchronization

```cpp
// Automatic bidirectional binding
auto slider = CommandBuilder("Slider")
    .bindToState("audio.volume")  // Automatically syncs both ways
    .build();
```

### 5. Type-Safe Widget Creation

```cpp
// Type-safe widget mapping
mapper.registerMapping<QSlider>("Slider");  // Compile-time type checking
```

## Usage Examples

### Simple Button

```cpp
auto button = CommandBuilder("Button")
    .text("Click Me")
    .tooltip("Example button")
    .onClick([]() { qDebug() << "Clicked!"; })
    .build();

auto [command, widget] = WidgetMapper::instance().createWidget(button.get());
```

### Form with Validation

```cpp
auto form = CommandHierarchyBuilder("Container")
    .layout("VBox")
    .addChild("TextInput", [](CommandBuilder& input) {
        input.placeholder("Email")
             .validator("text", [](const QVariant& value) {
                 QString email = value.toString();
                 return email.contains("@");
             })
             .bindToState("form.email");
    })
    .addChild("Button", [](CommandBuilder& submit) {
        submit.text("Submit")
              .registerAsAction("form.submit");
    })
    .build();
```

### Data Binding Example

```cpp
// Counter with synchronized display
auto counter = CommandHierarchyBuilder("Container")
    .layout("HBox")
    .addChild("Label", [](CommandBuilder& label) {
        label.bindToState("counter.value", "text");  // Display current value
    })
    .addChild("Button", [](CommandBuilder& increment) {
        increment.text("+")
                 .onClick([]() {
                     // Increment counter in state
                     auto& state = StateManager::instance();
                     int current = state.getState<int>("counter.value")->get();
                     state.setState("counter.value", current + 1);
                 });
    })
    .build();
```

## Best Practices

1. **Use CommandBuilder for declarative construction**
2. **Leverage inheritance for specialized behavior**
3. **Bind to StateManager for data synchronization**
4. **Use event system for loose coupling**
5. **Register commands and widgets for type safety**
6. **Validate inputs at the Command level**
7. **Use MVC integration for action handling**

## Migration Guide

### From Traditional Qt

```cpp
// Before: Direct QWidget usage
QPushButton* button = new QPushButton("Save");
connect(button, &QPushButton::clicked, this, &MyClass::save);
layout->addWidget(button);

// After: Command-based approach
auto button = CommandBuilder("Button")
    .text("Save")
    .registerAsAction("file.save")
    .build();

auto container = CommandHierarchyBuilder("Container")
    .addChild(button)
    .build();
```

### Integration Steps

1. **Create Commands**: Replace QWidget creation with Command creation
2. **Set up Bindings**: Connect Commands to StateManager
3. **Register Actions**: Use MVC integration for action handling
4. **Migrate Events**: Use Command event system instead of Qt signals
5. **Build Hierarchies**: Use CommandBuilder for UI construction

This architecture provides a clean separation between UI logic and widget implementation while maintaining the flexibility and power of Qt's widget system.
