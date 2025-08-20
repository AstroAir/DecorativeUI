# Components Module

The Components module provides a comprehensive library of 30+ UI components built on top of the DeclarativeUI Core framework. All components inherit from `UIElement` and provide fluent interfaces for easy configuration, property binding, and modern C++20 features.

## Overview

This directory contains all UI components organized by category:

- **Button Components**: Interactive buttons and controls
- **Input Components**: Form controls and data entry widgets
- **Display Components**: Text, images, and information display
- **Container Components**: Layout and grouping widgets
- **Advanced Components**: Complex data views and specialized controls
- **Dialog Components**: Modal dialogs and file choosers
- **Other Components**: Toolbars, status bars, and utility widgets

## Key Features

### 🎨 **Fluent Interface Design**

- Method chaining for readable, declarative code
- Type-safe property configuration
- Event handling with lambda support
- CSS-like styling capabilities

### 🔗 **Reactive Integration**

- Automatic property binding to state management
- Real-time UI updates on data changes
- Two-way data binding support
- Computed property integration

### ⚡ **Modern C++20 Features**

- RAII-based resource management
- Exception safety guarantees
- Template-based type safety
- Smart pointer integration

### ✨ **Enhanced Component Features**

- **Comprehensive Accessibility**: ARIA support, screen reader compatibility, keyboard navigation
- **Advanced Validation**: Real-time validation with custom validators and error handling
- **Visual Enhancements**: Drop shadows, hover effects, animations, and custom styling
- **State Management**: Loading states, disabled states with reasons, validation states
- **Auto-completion**: Built-in auto-completion and suggestion systems for input components
- **Event Handling**: Advanced event handlers for hover, focus, double-click, right-click
- **Animation Support**: Smooth transitions and visual feedback with customizable animations
- **Group Management**: Checkbox groups, radio button groups with validation
- **Input Formatting**: Phone numbers, currency, dates, and custom formatting
- **Tooltip Integration**: Rich tooltips with accessibility support

### 🎭 **Animation Support**

- Built-in animation methods (fadeIn, fadeOut, slideIn, etc.)
- Integration with Animation module
- Smooth transitions and effects
- Performance-optimized animations

## Component Categories

### Button Components

Interactive controls for user actions.

**Available Components:**

- **Button**: Enhanced push button with accessibility, validation, visual effects, and advanced features
- **RadioButton**: Exclusive selection in groups
- **ToolButton**: Toolbar-style button with icon support

**Enhanced Button Features:**

```cpp
auto button = std::make_unique<Components::Button>();
button->text("Save Document")
      ->icon(QIcon(":/icons/save.png"))
      ->onClick([]() { qDebug() << "Document saved!"; })
      ->tooltip("Save the current document")
      ->accessibleName("Save Button")
      ->shortcut(QKeySequence::Save)
      ->dropShadow(true)
      ->hoverEffect(true)
      ->borderRadius(8)
      ->loading(false)
      ->required(true);
```

### Input Components

Form controls for data entry and user input.

**Available Components:**

- **LineEdit**: Enhanced single-line text input with validation, formatting, and accessibility
- **CheckBox**: Boolean input with tri-state support
- **ComboBox**: Dropdown selection lists
- **SpinBox**: Numeric input with increment/decrement
- **DoubleSpinBox**: Floating-point numeric input
- **Slider**: Range-based value selection
- **Dial**: Circular value selector

**Enhanced LineEdit Example:**

```cpp
auto emailEdit = std::make_unique<Components::LineEdit>();
emailEdit->placeholder("Enter email address")
         ->tooltip("Email address for notifications")
         ->accessibleName("Email Address")
         ->required(true)
         ->email(true)  // Built-in email validation
         ->autoComplete({"user@example.com", "admin@company.com"})
         ->selectAllOnFocus(true)
         ->borderRadius(4)
         ->onValidationChanged([](bool valid, const QString& error) {
             qDebug() << "Validation:" << valid << error;
         });
```

**Enhanced CheckBox Example:**

```cpp
auto termsCheckbox = std::make_unique<Components::CheckBox>();
termsCheckbox->text("I agree to the terms and conditions")
             ->tooltip("You must agree to continue")
             ->accessibleName("Terms Agreement")
             ->required(true)
             ->dropShadow(true)
             ->hoverEffect(true)
             ->checkAnimation(true)
             ->onValidationChanged([](bool valid, const QString& error) {
                 if (!valid) qDebug() << "Error:" << error;
             });
```

**Enhanced Slider Example:**

```cpp
auto volumeSlider = std::make_unique<Components::Slider>();
volumeSlider->range(0, 100)
            ->value(50)
            ->tooltip("Adjust volume level")
            ->accessibleName("Volume Control")
            ->showValue(true)
            ->valueFormat("%1%")
            ->customColors(QColor("#3498db"), QColor("#ecf0f1"), QColor("#2980b9"))
            ->snapToTicks(true)
            ->onValueChanged([](int value) {
                qDebug() << "Volume:" << value << "%";
            });
```

## Enhanced Component Features Summary

### 🎯 **Core Enhanced Components**

| Component       | Enhanced Features  | Key Capabilities                                                                                     |
| --------------- | ------------------ | ---------------------------------------------------------------------------------------------------- |
| **Button**      | ✅ Fully Enhanced  | Accessibility, validation, visual effects, state management, animations, badges, progress indicators |
| **LineEdit**    | ✅ Fully Enhanced  | Comprehensive validation, auto-completion, input formatting, real-time feedback, accessibility       |
| **CheckBox**    | ✅ Fully Enhanced  | Group management, validation, visual effects, accessibility, state management                        |
| **ComboBox**    | 🔄 Header Enhanced | Auto-completion, filtering, validation, multi-select, custom delegates                               |
| **SpinBox**     | 🔄 Header Enhanced | Range validation, formatting, accessibility, advanced input modes                                    |
| **Label**       | ✅ Header Enhanced | Rich text, markdown, interactive features, animations, accessibility                                 |
| **TextEdit**    | ✅ Header Enhanced | Syntax highlighting, validation, collaboration, find/replace, accessibility                          |
| **Slider**      | ✅ Header Enhanced | Custom ticks, validation, visual feedback, range selection, accessibility                            |
| **ProgressBar** | ✅ Header Enhanced | State management, animations, multi-progress, milestones, accessibility                              |
| **ToolButton**  | ✅ Header Enhanced | Toolbar features, badges, notifications, animations, accessibility                                   |

### 🌟 **Universal Enhancement Features**

#### **Accessibility Excellence**

- **ARIA Support**: Proper roles, names, descriptions for screen readers
- **Keyboard Navigation**: Full keyboard accessibility with custom shortcuts
- **Tab Management**: Proper tab order and focus management
- **Screen Reader Support**: Comprehensive compatibility with assistive technologies

#### **Advanced Validation System**

- **Real-time Validation**: Validation as users type or on focus change
- **Custom Validators**: Support for complex custom validation logic
- **Visual Feedback**: Clear visual indication of validation states
- **Error Handling**: Comprehensive error messaging and user guidance

#### **Visual Enhancement System**

- **Modern Effects**: Drop shadows, hover effects, smooth animations
- **Customizable Styling**: Border radius, custom colors, gradients
- **Animation Support**: Smooth transitions and visual feedback
- **Theme Integration**: Consistent with the existing theme system

#### **State Management**

- **Loading States**: Built-in loading indicators and states
- **Disabled States**: Disabled states with explanatory reasons
- **Validation States**: Clear indication of valid/invalid states
- **Multi-state Support**: Components that support multiple states

#### **Event System**

- **Enhanced Events**: Hover, focus, double-click, right-click handlers
- **Validation Events**: Real-time validation feedback
- **State Change Events**: Comprehensive state change notifications
- **Custom Events**: Support for custom event handling

### 🔧 **Implementation Status**

#### **✅ Fully Implemented**

- Button: Complete implementation with all enhanced features
- LineEdit: Complete implementation with comprehensive validation
- CheckBox: Complete implementation with group management

#### **🔄 Header Enhanced (Implementation Ready)**

- ComboBox: Enhanced header with method stubs
- SpinBox: Enhanced header with validation framework
- Label: Enhanced header with rich text support
- TextEdit: Enhanced header with advanced editing features
- Slider: Enhanced header with custom visualization
- ProgressBar: Enhanced header with state management
- ToolButton: Enhanced header with toolbar features

#### **📋 Next Phase Components**

- RadioButton: Group management and validation
- GroupBox: Container enhancements and accessibility
- Frame: Visual enhancements and accessibility
- Calendar: Date validation and formatting
- DateTimeEdit: Advanced date/time handling

### Display Components

Components for showing information and content.

**Available Components:**

- **Label**: Text and rich content display
- **ProgressBar**: Progress indication with customizable appearance
- **TextEdit**: Multi-line rich text editing
- **PlainTextEdit**: Plain text editing with syntax highlighting
- **LCDNumber**: Digital display for numeric values

**Example Usage:**

```cpp
auto label = std::make_unique<Components::Label>();
label->text("Welcome to DeclarativeUI!")
     ->alignment(Qt::AlignCenter)
     ->wordWrap(true)
     ->font(QFont("Arial", 14, QFont::Bold))
     ->color(QColor("#2c3e50"));
```

### Container Components

Layout and grouping widgets for organizing UI elements.

**Available Components:**

- **Container**: Generic container with layout management
- **Layout**: Layout management utilities
- **GroupBox**: Grouped controls with borders and titles
- **TabWidget**: Tabbed interface container
- **Frame**: Decorative frame container
- **ScrollArea**: Scrollable content area
- **Splitter**: Resizable split layouts
- **Widget**: Basic widget container

**Example Usage:**

```cpp
auto container = std::make_unique<Components::Container>();
container->layout<QVBoxLayout>([](auto& layout) {
             layout.spacing(10);
             layout.setContentsMargins(20, 20, 20, 20);
         })
         ->child<Components::Button>([](auto& btn) {
             btn.text("Button 1");
         })
         ->child<Components::Button>([](auto& btn) {
             btn.text("Button 2");
         });
```

### Advanced Components

Complex components for data display and specialized functionality.

**Available Components:**

- **TableView**: Tabular data display with sorting and filtering
- **TreeView**: Hierarchical data display
- **ListView**: List-based data display
- **MenuBar**: Application menu bar
- **DateTimeEdit**: Date and time input controls
- **Calendar**: Calendar widget for date selection

**Example Usage:**

```cpp
auto tableView = std::make_unique<Components::TableView>();
tableView->setModel(myDataModel)
         ->setSortingEnabled(true)
         ->setSelectionBehavior(QAbstractItemView::SelectRows)
         ->onSelectionChanged([](const QItemSelection& selected) {
             // Handle selection changes
         });
```

### Dialog Components

Modal dialogs and specialized choosers.

**Available Components:**

- **FileDialog**: File and directory selection
- **ColorDialog**: Color picker dialog
- **FontDialog**: Font selection dialog
- **MessageBox**: Information and confirmation dialogs

**Example Usage:**

```cpp
auto fileDialog = std::make_unique<Components::FileDialog>();
fileDialog->setFileMode(QFileDialog::ExistingFile)
          ->setNameFilter("Images (*.png *.jpg *.bmp)")
          ->onFileSelected([](const QString& fileName) {
              qDebug() << "Selected file:" << fileName;
          });
```

### Other Components

Utility and specialized components.

**Available Components:**

- **ToolBar**: Application toolbar with actions
- **StatusBar**: Application status bar
- **DockWidget**: Dockable widget panels
- **Dial**: Circular control dial

## Common Usage Patterns

### Basic Component Creation

```cpp
#include "Components/Button.hpp"
using namespace DeclarativeUI::Components;

// Create and configure component
auto button = std::make_unique<Button>();
button->text("Save Document")
      ->onClick([]() { /* save logic */ })
      ->style("QPushButton { background-color: #27ae60; color: white; }");

// Initialize and show
button->initialize();
button->getWidget()->show();
```

### Property Binding Integration

```cpp
#include "Binding/StateManager.hpp"

auto& state = StateManager::instance();
state.createState<QString>("status", "Ready");

auto label = std::make_unique<Components::Label>();
label->bindProperty("text", []() {
    auto status = StateManager::instance().getState<QString>("status");
    return QString("Status: %1").arg(status->get());
});
```

### Animation Integration

```cpp
auto button = std::make_unique<Components::Button>();
button->text("Animated Button")
      ->onClick([&button]() {
          button->fadeOut(300)
                ->then([&button]() {
                    button->text("Clicked!");
                    button->fadeIn(300);
                });
      });
```

### Event Handling

```cpp
auto lineEdit = std::make_unique<Components::LineEdit>();
lineEdit->onTextChanged([](const QString& text) {
            // Validate input
            if (text.length() > 10) {
                // Show warning
            }
        })
        ->onReturnPressed([]() {
            // Submit form
        });
```

### Styling and Theming

```cpp
auto button = std::make_unique<Components::Button>();
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

## Component Lifecycle

All components follow a consistent lifecycle:

1. **Construction**: Create component instance
2. **Configuration**: Set properties using fluent interface
3. **Initialization**: Call `initialize()` to create Qt widget
4. **Usage**: Component is ready for display and interaction
5. **Cleanup**: Automatic RAII-based resource management

```cpp
// 1. Construction
auto component = std::make_unique<Components::Button>();

// 2. Configuration
component->text("Click Me")->onClick(handler);

// 3. Initialization
component->initialize();

// 4. Usage
component->getWidget()->show();

// 5. Cleanup (automatic when component goes out of scope)
```

## Error Handling

Components use the DeclarativeUI exception system for robust error handling:

```cpp
try {
    auto button = std::make_unique<Components::Button>();
    button->initialize();
} catch (const Exceptions::ComponentCreationException& e) {
    qCritical() << "Failed to create button:" << e.what();
} catch (const Exceptions::UIException& e) {
    qCritical() << "UI error:" << e.what();
}
```

## Performance Considerations

- **Lazy Initialization**: Qt widgets created only when `initialize()` is called
- **Memory Management**: RAII ensures automatic cleanup
- **Event Optimization**: Efficient signal/slot connections
- **Animation Performance**: Hardware-accelerated animations when available

## Dependencies

- **Qt6**: Core, Widgets modules
- **Core**: UIElement base class and utilities
- **Exceptions**: Error handling system
- **Animation**: Animation integration (optional)
- **Binding**: State management integration (optional)

## Testing

All components have comprehensive unit tests:

```bash
cd build
ctest -R Components
```

## Integration Examples

### Form Creation

```cpp
auto form = Core::create<QWidget>()
    .layout<QFormLayout>()
    .child<Components::LineEdit>([](auto& input) {
        input.placeholder("First Name").bindToState("user.firstName");
    })
    .child<Components::LineEdit>([](auto& input) {
        input.placeholder("Last Name").bindToState("user.lastName");
    })
    .child<Components::Button>([](auto& btn) {
        btn.text("Submit").onClick([]() { /* submit form */ });
    })
    .build();
```

### Dashboard Layout

```cpp
auto dashboard = std::make_unique<Components::Container>();
dashboard->layout<QGridLayout>()
         ->child<Components::Label>([](auto& label) {
             label.text("System Status").font(QFont("Arial", 16, QFont::Bold));
         })
         ->child<Components::ProgressBar>([](auto& progress) {
             progress.bindToState("system.cpuUsage");
         })
         ->child<Components::TableView>([](auto& table) {
             table.setModel(systemDataModel);
         });
```
