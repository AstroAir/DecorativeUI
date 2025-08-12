# Component System

The DeclarativeUI Component System provides a rich library of 30+ pre-built UI components that cover all common use cases. Components are designed with a fluent interface for easy chaining and configuration.

## Overview

Components in DeclarativeUI are built on top of Qt6 widgets but provide a modern, declarative API that makes UI construction more intuitive and maintainable.

### Key Features

- **Fluent Interface**: Method chaining for readable code
- **Property Binding**: Reactive properties that automatically update the UI
- **Event Handling**: Type-safe event system with lambda support
- **Validation**: Built-in validation for component properties
- **Styling**: CSS-like styling support

## Basic Usage

All components follow a consistent pattern:

```cpp
#include "Components/Button.hpp"
using namespace DeclarativeUI::Components;

// Create and configure a button
auto button = std::make_unique<Button>();
button->text("Click Me")
      .onClick([]() { qDebug() << "Button clicked!"; })
      .style("background-color: blue; color: white;")
      .enabled(true);

// Initialize the component (creates the underlying Qt widget)
button->initialize();

// Get the Qt widget for adding to layouts
QWidget* widget = button->getWidget();
```

## Component Categories

### Input Components

#### Button
Standard push button with click handling:

```cpp
auto button = std::make_unique<Button>();
button->text("Save Document")
      .onClick([this]() { saveDocument(); })
      .shortcut(QKeySequence::Save)
      .toolTip("Save the current document");
```

#### LineEdit
Single-line text input with validation:

```cpp
auto lineEdit = std::make_unique<LineEdit>();
lineEdit->placeholder("Enter your name")
        .validator([](const QString& text) {
            return !text.isEmpty() && text.length() >= 2;
        })
        .onTextChanged([](const QString& text) {
            qDebug() << "Text changed:" << text;
        });
```

#### CheckBox
Checkbox with tri-state support:

```cpp
auto checkBox = std::make_unique<CheckBox>();
checkBox->text("Enable notifications")
        .checked(true)
        .onStateChanged([](Qt::CheckState state) {
            bool enabled = (state == Qt::Checked);
            // Update application settings
        });
```

### Display Components

#### Label
Text and image display with rich formatting:

```cpp
auto label = std::make_unique<Label>();
label->text("Welcome to DeclarativeUI")
     .alignment(Qt::AlignCenter)
     .wordWrap(true)
     .style("font-size: 16px; color: #333;");
```

#### ProgressBar
Progress indication with custom styling:

```cpp
auto progressBar = std::make_unique<ProgressBar>();
progressBar->minimum(0)
           .maximum(100)
           .value(50)
           .textVisible(true)
           .format("%p% Complete");
```

### Container Components

#### Container
Generic container with layout management:

```cpp
auto container = std::make_unique<Container>();
container->layout<QHBoxLayout>([](auto& layout) {
             layout.spacing(15);
         })
         .child<Button>([](auto& btn) {
             btn.text("Button 1");
         })
         .child<Button>([](auto& btn) {
             btn.text("Button 2");
         });
```

#### GroupBox
Grouped controls with borders and titles:

```cpp
auto groupBox = std::make_unique<GroupBox>();
groupBox->title("User Settings")
        .checkable(true)
        .checked(true)
        .layout<QVBoxLayout>()
        .child<CheckBox>([](auto& cb) {
            cb.text("Enable notifications");
        });
```

## Property Binding

Components support reactive property binding that automatically updates the UI when data changes:

```cpp
// Bind a label's text to application state
auto label = std::make_unique<Label>();
label->bindProperty("text", []() {
    return QString("Current user: %1").arg(getCurrentUser());
});

// When state changes, the label automatically updates
StateManager::instance().setState("currentUser", "John Doe");
```

## Event Handling

Components provide type-safe event handling with lambda support:

```cpp
auto button = std::make_unique<Button>();
button->onClick([this](const auto& event) {
    // Access event details
    auto modifiers = event.modifiers();
    auto position = event.position();
    
    if (modifiers & Qt::ControlModifier) {
        // Handle Ctrl+Click
    } else {
        // Handle regular click
    }
});
```

## Validation

Many input components support built-in validation:

```cpp
auto lineEdit = std::make_unique<LineEdit>();
lineEdit->validator([](const QString& text) -> ValidationResult {
    if (text.isEmpty()) {
        return {false, "Field cannot be empty"};
    }
    if (text.length() < 3) {
        return {false, "Minimum 3 characters required"};
    }
    return {true, ""};
});
```

## Styling

Components support CSS-like styling:

```cpp
auto button = std::make_unique<Button>();
button->style(R"(
    QPushButton {
        background-color: #3498db;
        color: white;
        border: none;
        padding: 10px 20px;
        border-radius: 5px;
        font-weight: bold;
    }
    QPushButton:hover {
        background-color: #2980b9;
    }
    QPushButton:pressed {
        background-color: #21618c;
    }
)");
```

## Complete Component List

For a complete list of all available components and their APIs, see the [Components API Reference](../api/components.md).

### Input Components
- Button, RadioButton, ToolButton
- LineEdit, TextEdit, PlainTextEdit
- CheckBox, ComboBox
- SpinBox, DoubleSpinBox
- Slider, Dial

### Display Components  
- Label, ProgressBar
- LCDNumber

### Container Components
- Container, Widget
- GroupBox, TabWidget
- Frame, ScrollArea, Splitter

### Advanced Components
- TableView, TreeView, ListView
- MenuBar, ToolBar, StatusBar
- DateTimeEdit, Calendar

### Dialog Components
- FileDialog, ColorDialog
- FontDialog, MessageBox

### Specialized Components
- DockWidget

## Next Steps

- Explore [State Management](state-management.md) for reactive UIs
- Learn about [JSON Support](json-support.md) for dynamic UI loading
- Try the [Command System](commands.md) for advanced architectures
- Browse [Examples](../examples/components.md) for practical demonstrations
