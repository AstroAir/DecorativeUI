# Binding Module

The Binding module provides a comprehensive reactive state management and property binding system for DeclarativeUI applications. It enables automatic UI updates, centralized state management, and type-safe reactive programming patterns.

## Overview

This directory contains the core reactive programming infrastructure:

- **StateManager**: Centralized application state management with reactive updates
- **PropertyBinding**: Type-safe property binding between objects and UI components
- **PropertyBindingTemplate**: Template-based binding system with advanced features
- **ReactiveProperty**: Individual reactive state containers with change notifications

## Key Features

### 🔄 **Reactive State Management**
- Centralized state store with automatic UI updates
- Type-safe state access and modification
- Computed properties with dependency tracking
- Batch updates for performance optimization

### 🔗 **Property Binding System**
- One-way, two-way, and computed bindings
- Type conversion and validation support
- Performance monitoring and optimization
- Template-based type safety

### 📊 **Advanced Features**
- Undo/redo history support
- State persistence and restoration
- Dependency graph analysis
- Performance metrics and debugging

### 🛡️ **Thread Safety**
- Thread-safe state operations
- Recursive mutex protection
- Atomic operations for counters
- Qt signal/slot thread safety

## Components

### StateManager (`StateManager.hpp/.cpp`)

Central singleton managing all application state with reactive updates.

**Key Features:**
- Type-safe state registration and access
- Computed properties with automatic dependency tracking
- Undo/redo history with configurable depth
- Batch updates for performance optimization
- State persistence to disk
- Performance monitoring and debugging

**Core Methods:**
```cpp
// State creation and access
template<typename T>
std::shared_ptr<ReactiveProperty<T>> createState(const QString& key, T initial_value = T{});

template<typename T>
std::shared_ptr<ReactiveProperty<T>> getState(const QString& key);

// Computed properties
template<typename T>
std::shared_ptr<ReactiveProperty<T>> createComputed(
    const QString& key, 
    std::function<T()> computer,
    std::vector<QString> dependencies
);

// Batch operations
void batchUpdate(std::function<void()> updates);

// History management
void enableHistory(const QString& key, int max_history = 50);
void undo(const QString& key);
void redo(const QString& key);

// Persistence
void saveState(const QString& filename);
void loadState(const QString& filename);
```

### PropertyBinding (`PropertyBinding.hpp/.cpp`)

Infrastructure for binding properties between objects with automatic updates.

**Binding Directions:**
- **OneWay**: Source → Target only
- **TwoWay**: Source ↔ Target bidirectional
- **OneTime**: Single update, then disconnect

**Update Modes:**
- **Immediate**: Update immediately on change
- **Deferred**: Batch updates for performance
- **Manual**: Update only when explicitly requested

**Key Features:**
- Type conversion between different property types
- Validation with custom validator functions
- Performance monitoring and metrics
- Error handling with detailed diagnostics

### PropertyBindingTemplate (`PropertyBindingTemplate.hpp`)

Template-based binding system with advanced type safety and features.

**Template Concepts:**
```cpp
// Type constraints for bindable types
template<typename T>
concept BindableType = /* implementation */;

// Compute function validation
template<typename F>
concept ComputeFunction = /* implementation */;
```

**Advanced Features:**
- Template-based type safety at compile time
- Custom converter functions between types
- Validation with predicate functions
- Performance monitoring with detailed metrics
- Error handling with exception safety

### ReactiveProperty (`StateManager.hpp`)

Individual reactive state container with change notifications.

**Key Features:**
- Type-safe value storage and access
- Change notification signals
- Computed property support with dependency tracking
- Thread-safe operations
- History tracking (when enabled)

**Core Methods:**
```cpp
template<typename T>
class ReactiveProperty {
public:
    T get() const;
    void set(const T& value);
    void bind(std::function<T()> computer);
    
    // Qt signals
    void valueChanged(const T& value);
};
```

## Usage Examples

### Basic State Management

```cpp
#include "Binding/StateManager.hpp"
using namespace DeclarativeUI::Binding;

auto& state = StateManager::instance();

// Create reactive state
auto counter = state.createState<int>("counter", 0);
auto username = state.createState<QString>("user.name", "Guest");

// Watch for changes
connect(counter.get(), &ReactiveProperty<int>::valueChanged,
        [](int value) {
            qDebug() << "Counter changed to:" << value;
        });

// Update state
counter->set(42);  // Triggers valueChanged signal
```

### Computed Properties

```cpp
// Create computed property that depends on other state
auto displayName = state.createComputed<QString>(
    "user.displayName",
    []() {
        auto firstName = state.getState<QString>("user.firstName");
        auto lastName = state.getState<QString>("user.lastName");
        return QString("%1 %2").arg(firstName->get(), lastName->get());
    },
    {"user.firstName", "user.lastName"}  // Dependencies
);

// Computed property updates automatically when dependencies change
state.getState<QString>("user.firstName")->set("John");
state.getState<QString>("user.lastName")->set("Doe");
// displayName now contains "John Doe"
```

### Property Binding

```cpp
#include "Binding/PropertyBinding.hpp"

// Create binding between slider and progress bar
auto slider = new QSlider();
auto progressBar = new QProgressBar();

auto binding = std::make_shared<PropertyBinding<int>>(
    slider, "value",           // Source
    progressBar, "value",      // Target
    BindingDirection::OneWay   // Direction
);

// Add to binding manager
PropertyBindingManager manager;
manager.addBinding(binding);

// Slider changes automatically update progress bar
slider->setValue(75);  // Progress bar shows 75%
```

### Two-Way Binding with State

```cpp
// Bind UI component to state with two-way synchronization
auto lineEdit = new QLineEdit();
auto nameState = state.getState<QString>("user.name");

auto binding = std::make_shared<PropertyBinding<QString>>(
    nameState.get(), "value",    // State as source
    lineEdit, "text",            // LineEdit as target
    BindingDirection::TwoWay     // Bidirectional
);

manager.addBinding(binding);

// Changes in either direction are synchronized
nameState->set("Alice");         // LineEdit shows "Alice"
lineEdit->setText("Bob");        // State becomes "Bob"
```

### Batch Updates for Performance

```cpp
// Batch multiple state changes for optimal performance
state.batchUpdate([]() {
    state.getState<QString>("user.firstName")->set("Jane");
    state.getState<QString>("user.lastName")->set("Smith");
    state.getState<int>("user.age")->set(30);
    state.getState<QString>("user.email")->set("jane@example.com");
});
// All dependent computed properties update once at the end
```

### Validation and Type Conversion

```cpp
// Binding with validation and type conversion
auto spinBox = new QSpinBox();
auto progressBar = new QProgressBar();

auto binding = std::make_shared<PropertyBinding<int>>(
    spinBox, "value",
    progressBar, "value",
    BindingDirection::OneWay
);

// Add validator
binding->setValidator([](int value) {
    return value >= 0 && value <= 100;  // Only allow 0-100 range
});

// Add converter (if needed for different types)
binding->setConverter([](int value) {
    return qBound(0, value, 100);  // Clamp to valid range
});

manager.addBinding(binding);
```

### History and Undo/Redo

```cpp
// Enable history for specific state
state.enableHistory("document.content", 100);  // Keep 100 history entries

auto content = state.getState<QString>("document.content");
content->set("Initial content");
content->set("Modified content");
content->set("Final content");

// Undo/redo operations
state.undo("document.content");    // Back to "Modified content"
state.undo("document.content");    // Back to "Initial content"
state.redo("document.content");    // Forward to "Modified content"
```

### State Persistence

```cpp
// Save current state to file
state.saveState("app_state.json");

// Later, restore state from file
state.loadState("app_state.json");

// All reactive properties and UI bindings are automatically updated
```

## Integration with Components

The Binding module integrates seamlessly with DeclarativeUI components:

```cpp
#include "Components/LineEdit.hpp"
#include "Components/Label.hpp"

// Components can bind directly to state
auto input = std::make_unique<Components::LineEdit>();
input->bindToState("user.name");  // Two-way binding to state

auto label = std::make_unique<Components::Label>();
label->bindProperty("text", []() {
    auto name = StateManager::instance().getState<QString>("user.name");
    return QString("Hello, %1!").arg(name->get());
});
```

## Performance Considerations

- **Batch Updates**: Use `batchUpdate()` for multiple state changes
- **Computed Dependencies**: Minimize dependency chains for better performance
- **Binding Cleanup**: Remove unused bindings to prevent memory leaks
- **History Limits**: Set appropriate history limits to control memory usage

## Thread Safety

All components are designed for thread-safe operation:
- StateManager uses recursive mutexes for nested calls
- ReactiveProperty operations are atomic where possible
- Qt signals/slots handle cross-thread communication safely

## Dependencies

- **Qt6**: Core, Widgets modules for signal/slot system
- **C++20**: Modern language features and concepts
- **Internal**: Core module for UIElement integration

## Testing

Comprehensive unit tests available in `tests/unit/`:
- State management lifecycle
- Property binding accuracy
- Template concept validation
- Performance benchmarks
- Thread safety verification

```bash
cd build
ctest -R Binding
```

## Future Enhancements

- **Reactive Collections**: Observable arrays and maps
- **Async State**: Support for asynchronous state updates
- **State Middleware**: Plugin system for state transformations
- **DevTools Integration**: Browser-like debugging tools
