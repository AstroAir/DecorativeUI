# State Management

DeclarativeUI provides a powerful reactive state management system that enables automatic UI updates when data changes. The state system is built around the `StateManager` class and supports property binding, computed properties, and batch updates.

## Overview

The state management system provides:

- **Centralized State**: Single source of truth for application data
- **Reactive Updates**: Automatic UI updates when state changes
- **Type Safety**: Template-based type-safe state access
- **Performance**: Efficient batching and dependency tracking
- **Persistence**: Optional state persistence and restoration

## Basic Usage

### Setting and Getting State

```cpp
#include "Binding/StateManager.hpp"
using namespace DeclarativeUI::Binding;

// Set state values
StateManager::instance().setState("user.name", QString("John Doe"));
StateManager::instance().setState("user.age", 30);
StateManager::instance().setState("app.theme", QString("dark"));

// Get state values with defaults
QString name = StateManager::instance().getState("user.name", QString("Unknown"));
int age = StateManager::instance().getState("user.age", 0);
bool isDarkTheme = StateManager::instance().getState("app.theme", QString("light")) == "dark";
```

### State Watching

Monitor state changes with watchers:

```cpp
// Watch for changes to user name
StateManager::instance().watchState("user.name", [](const QVariant& newValue) {
    qDebug() << "User name changed to:" << newValue.toString();
});

// Watch multiple state keys
StateManager::instance().watchState({"user.name", "user.email"}, [](const QString& key, const QVariant& value) {
    qDebug() << "User property changed:" << key << "=" << value;
});
```

## Property Binding

### Component Property Binding

Bind component properties to state for automatic updates:

```cpp
#include "Components/Label.hpp"

auto label = std::make_unique<Components::Label>();
label->bindProperty("text", []() {
    QString name = StateManager::instance().getState("user.name", QString("Guest"));
    return QString("Welcome, %1!").arg(name);
});

// When state changes, label automatically updates
StateManager::instance().setState("user.name", "Alice");
// Label now shows "Welcome, Alice!"
```

### Two-Way Binding

Create bidirectional binding between UI and state:

```cpp
#include "Components/LineEdit.hpp"

auto nameInput = std::make_unique<Components::LineEdit>();

// Bind input value to state
nameInput->bindProperty("text", []() {
    return StateManager::instance().getState("user.name", QString(""));
});

// Update state when input changes
nameInput->onTextChanged([](const QString& text) {
    StateManager::instance().setState("user.name", text);
});
```

### Computed Properties

Create derived state that automatically updates:

```cpp
// Register a computed property
StateManager::instance().addComputedProperty("user.displayName",
    [](const StateManager& state) -> QVariant {
        QString firstName = state.getState("user.firstName", QString(""));
        QString lastName = state.getState("user.lastName", QString(""));
        return QString("%1 %2").arg(firstName, lastName).trimmed();
    },
    {"user.firstName", "user.lastName"}  // Dependencies
);

// Use computed property in UI
auto label = std::make_unique<Components::Label>();
label->bindProperty("text", []() {
    return StateManager::instance().getState("user.displayName", QString("Unknown User"));
});
```

## Advanced Features

### Batch Updates

Perform multiple state changes efficiently:

```cpp
// Batch multiple updates to avoid unnecessary UI refreshes
StateManager::instance().batchUpdate([](StateManager& state) {
    state.setState("user.firstName", "John");
    state.setState("user.lastName", "Doe");
    state.setState("user.email", "john.doe@example.com");
    state.setState("user.age", 30);
});
// All bound UI elements update once after the batch
```

### State Validation

Add validation to state changes:

```cpp
// Add validator for user age
StateManager::instance().addValidator("user.age", [](const QVariant& value) -> ValidationResult {
    int age = value.toInt();
    if (age < 0 || age > 150) {
        return {false, "Age must be between 0 and 150"};
    }
    return {true, ""};
});

// Invalid state changes will be rejected
StateManager::instance().setState("user.age", -5);  // Will fail validation
```

### State History

Track state changes for undo/redo functionality:

```cpp
// Enable history tracking
StateManager::instance().enableHistory(true);

// Make some changes
StateManager::instance().setState("user.name", "John");
StateManager::instance().setState("user.name", "Jane");

// Undo last change
StateManager::instance().undo();  // name is back to "John"

// Redo
StateManager::instance().redo();  // name is "Jane" again

// Check history
bool canUndo = StateManager::instance().canUndo();
bool canRedo = StateManager::instance().canRedo();
```

### State Persistence

Save and restore state across application sessions:

```cpp
// Save state to file
StateManager::instance().saveToFile("app_state.json");

// Restore state from file
StateManager::instance().loadFromFile("app_state.json");

// Auto-save on changes
StateManager::instance().enableAutoSave("app_state.json", 5000);  // Save every 5 seconds
```

## Integration with Components

### Automatic Binding

Many components support automatic state binding:

```cpp
// Create a form that automatically binds to state
auto form = Core::create<QWidget>()
    .layout<QFormLayout>()
    .child<Components::LineEdit>([](auto& input) {
        input.placeholder("First Name")
             .bindToState("user.firstName");  // Two-way binding
    })
    .child<Components::LineEdit>([](auto& input) {
        input.placeholder("Last Name")
             .bindToState("user.lastName");
    })
    .child<Components::Label>([](auto& label) {
        label.bindProperty("text", []() {
            return QString("Full Name: %1").arg(
                StateManager::instance().getState("user.displayName", QString(""))
            );
        });
    })
    .build();
```

### State-Driven UI

Create UI that responds to state changes:

```cpp
// Create a status indicator that changes based on connection state
auto statusLabel = std::make_unique<Components::Label>();
statusLabel->bindProperty("text", []() {
    bool connected = StateManager::instance().getState("network.connected", false);
    return connected ? "🟢 Connected" : "🔴 Disconnected";
});

statusLabel->bindProperty("style", []() {
    bool connected = StateManager::instance().getState("network.connected", false);
    return connected ? "color: green;" : "color: red;";
});

// Update connection status
StateManager::instance().setState("network.connected", true);
// Label automatically updates to show "🟢 Connected" in green
```

## Performance Optimization

### Efficient Updates

Use batch updates and computed properties for optimal performance:

```cpp
// Instead of multiple individual updates
StateManager::instance().setState("user.firstName", "John");
StateManager::instance().setState("user.lastName", "Doe");
StateManager::instance().setState("user.email", "john@example.com");

// Use batch updates
StateManager::instance().batchUpdate([](StateManager& state) {
    state.setState("user.firstName", "John");
    state.setState("user.lastName", "Doe");
    state.setState("user.email", "john@example.com");
});
```

### Dependency Optimization

Minimize dependencies in computed properties:

```cpp
// Good: Specific dependencies
StateManager::instance().addComputedProperty("user.fullName",
    [](const StateManager& state) -> QVariant {
        return QString("%1 %2").arg(
            state.getState("user.firstName", QString("")),
            state.getState("user.lastName", QString(""))
        );
    },
    {"user.firstName", "user.lastName"}  // Only these dependencies
);

// Avoid: Too many dependencies
// This would cause unnecessary recalculations
```

## Best Practices

### State Organization

Organize state hierarchically:

```cpp
// Good: Hierarchical organization
StateManager::instance().setState("app.ui.theme", "dark");
StateManager::instance().setState("app.ui.language", "en");
StateManager::instance().setState("user.profile.name", "John");
StateManager::instance().setState("user.settings.notifications", true);

// Avoid: Flat structure
StateManager::instance().setState("appTheme", "dark");
StateManager::instance().setState("userName", "John");
```

### State Immutability

Treat state as immutable when possible:

```cpp
// Good: Replace entire objects
QJsonObject userProfile = StateManager::instance().getState("user.profile", QJsonObject{});
userProfile["name"] = "John Doe";
userProfile["email"] = "john@example.com";
StateManager::instance().setState("user.profile", userProfile);

// Avoid: Modifying state objects directly
// This won't trigger UI updates
```

### Error Handling

Handle state errors gracefully:

```cpp
try {
    StateManager::instance().setState("user.age", -5);
} catch (const StateValidationException& e) {
    qWarning() << "State validation failed:" << e.what();
    // Show user-friendly error message
}
```

## Examples

For practical examples of state management:

- **[Basic State Example](../examples/basic.md#state-management)** - Simple state usage
- **[Reactive UI Example](../examples/advanced.md#reactive-dashboard)** - Complex reactive interfaces
- **[Form Handling](../examples/advanced.md#form-validation)** - Form state management

## API Reference

For detailed API documentation, see:

- **[State Management API](../api/binding.md)** - Complete StateManager API
- **[Property Binding API](../api/binding.md#property-binding)** - PropertyBinding system

## Next Steps

- Learn about [JSON Support](json-support.md) for dynamic UIs
- Explore [Hot Reload](hot-reload.md) for rapid development
- Try the [Command System](commands.md) for advanced architectures
- Browse [Advanced Examples](../examples/advanced.md) for real-world usage
