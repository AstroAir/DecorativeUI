# Binding System API Reference

The Binding module provides a comprehensive property binding system with state management, dependency tracking, and performance optimization. It enables reactive UI updates and centralized state management.

## StateManager Class

The `StateManager` class provides centralized state management with dependency tracking, history support, and performance monitoring.

### Header

```cpp
#include "Binding/StateManager.hpp"
```

### Class Declaration

```cpp
namespace DeclarativeUI::Binding {
    class StateManager {
    public:
        static StateManager& instance();

        // State management methods...
    private:
        StateManager() = default; // Singleton
    };
}
```

### Public Methods

#### Singleton Access

##### `static instance() -> StateManager&`

Gets the global StateManager instance.

**Returns:** Reference to the singleton instance

**Example:**

```cpp
auto& state_manager = StateManager::instance();
```

#### State Management

##### `setState<T>(const QString& key, T&& value)`

Sets a state value with perfect forwarding.

**Template Parameters:**

- `T`: Value type

**Parameters:**

- `key`: State key identifier
- `value`: State value (forwarded)

**Example:**

```cpp
state_manager.setState("counter", 42);
state_manager.setState("username", QString("john_doe"));
state_manager.setState("settings", MySettings{});
```

##### `getState<T>(const QString& key) -> std::shared_ptr<ReactiveProperty<T>>`

Gets a reactive property for the specified state.

**Template Parameters:**

- `T`: Expected value type

**Parameters:**

- `key`: State key identifier

**Returns:** Shared pointer to reactive property, or nullptr if not found

**Example:**

```cpp
auto counter = state_manager.getState<int>("counter");
if (counter) {
    int value = counter->get();
    counter->set(value + 1);
}
```

##### `hasState(const QString& key) const -> bool`

Checks if a state exists.

**Parameters:**

- `key`: State key identifier

**Returns:** `true` if state exists, `false` otherwise

##### `removeState(const QString& key) -> bool`

Removes a state from the manager.

**Parameters:**

- `key`: State key identifier

**Returns:** `true` if state was removed, `false` if not found

#### State Observation

##### `observeState<T>(const QString& key, std::function<void(const T&)> observer)`

Adds an observer for state changes.

**Template Parameters:**

- `T`: Expected value type

**Parameters:**

- `key`: State key identifier
- `observer`: Function called when state changes

**Example:**

```cpp
state_manager.observeState<int>("counter", [](const int& value) {
    qDebug() << "Counter changed to:" << value;
});
```

##### `removeObserver(const QString& key, const std::function<void(const QVariant&)>& observer)`

Removes a specific observer.

#### Batch Operations

##### `batchUpdate(std::function<void()> updates)`

Performs multiple state updates in a batch for performance.

**Parameters:**

- `updates`: Function containing state updates

**Example:**

```cpp
state_manager.batchUpdate([]() {
    StateManager::instance().setState("x", 10);
    StateManager::instance().setState("y", 20);
    StateManager::instance().setState("z", 30);
});
```

#### Validation

##### `setValidator<T>(const QString& key, std::function<bool(const T&)> validator)`

Sets a validator function for a state.

**Template Parameters:**

- `T`: Value type

**Parameters:**

- `key`: State key identifier
- `validator`: Validation function

**Example:**

```cpp
state_manager.setValidator<int>("counter", [](const int& value) {
    return value >= 0; // Only allow non-negative values
});
```

##### `removeValidator(const QString& key)`

Removes the validator for a state.

#### History and Undo/Redo

##### `enableHistory(const QString& key, int max_history_size = 50)`

Enables history tracking for a state.

**Parameters:**

- `key`: State key identifier
- `max_history_size`: Maximum number of history entries

##### `disableHistory(const QString& key)`

Disables history tracking for a state.

##### `canUndo(const QString& key) const -> bool`

Checks if undo is available for a state.

##### `canRedo(const QString& key) const -> bool`

Checks if redo is available for a state.

##### `undo(const QString& key)`

Undoes the last change to a state.

##### `redo(const QString& key)`

Redoes the last undone change to a state.

#### Dependency Tracking

##### `addDependency(const QString& dependent, const QString& dependency)`

Adds a dependency relationship between states.

**Parameters:**

- `dependent`: State that depends on another
- `dependency`: State that the dependent relies on

**Example:**

```cpp
// fullName depends on firstName and lastName
state_manager.addDependency("fullName", "firstName");
state_manager.addDependency("fullName", "lastName");
```

##### `removeDependency(const QString& dependent, const QString& dependency)`

Removes a dependency relationship.

##### `getDependencies(const QString& key) const -> QStringList`

Gets all dependencies for a state.

##### `updateDependents(const QString& key)`

Manually triggers update of all dependent states.

#### Performance and Debugging

##### `enablePerformanceMonitoring(bool enabled)`

Enables performance monitoring for state operations.

##### `getPerformanceReport() const -> QString`

Gets a performance report for state operations.

##### `enableDebugMode(bool enabled)`

Enables debug logging for state changes.

##### `clearState() noexcept`

Clears all state data (useful for testing).

### Signals

```cpp
signals:
    void stateChanged(const QString& key, const QVariant& value);
    void stateAdded(const QString& key);
    void stateRemoved(const QString& key);
    void performanceWarning(const QString& key, qint64 time_ms);
```

### Usage Example

```cpp
#include "Binding/StateManager.hpp"

class CounterApplication {
public:
    CounterApplication() {
        setupState();
        setupObservers();
    }

private:
    void setupState() {
        auto& state = StateManager::instance();

        // Initialize counter state
        state.setState("counter", 0);
        state.enableHistory("counter", 100);

        // Set up validation
        state.setValidator<int>("counter", [](const int& value) {
            return value >= 0 && value <= 1000;
        });

        // Set up computed state
        state.setState("counterText", QString("Count: 0"));
        state.addDependency("counterText", "counter");
    }

    void setupObservers() {
        auto& state = StateManager::instance();

        // Update counter text when counter changes
        state.observeState<int>("counter", [&state](const int& value) {
            state.setState("counterText", QString("Count: %1").arg(value));
        });
    }

    void increment() {
        auto counter = StateManager::instance().getState<int>("counter");
        if (counter) {
            counter->set(counter->get() + 1);
        }
    }

    void undo() {
        if (StateManager::instance().canUndo("counter")) {
            StateManager::instance().undo("counter");
        }
    }
};
```

## PropertyBinding Classes

The PropertyBinding system provides type-safe, bidirectional property binding between objects.

### Header

```cpp
#include "Binding/PropertyBinding.hpp"
```

### Binding Direction Enum

```cpp
enum class BindingDirection {
    OneWay,  // Source -> Target only
    TwoWay,  // Source <-> Target bidirectional
    OneTime  // Single evaluation at binding time
};

enum class UpdateMode {
    Immediate,  // Update immediately when source changes
    Deferred,   // Batch updates for performance
    Manual      // Manual update only
};
```

### IPropertyBinding Interface

Base interface for all property bindings.

```cpp
class IPropertyBinding {
public:
    virtual ~IPropertyBinding() = default;

    virtual void update() = 0;
    virtual void disconnect() = 0;
    virtual bool isValid() const = 0;
    virtual QString getSourcePath() const = 0;
    virtual QString getTargetPath() const = 0;
    virtual BindingDirection getDirection() const = 0;
};
```

### PropertyBinding Template Class

Type-safe property binding implementation.

```cpp
template<typename SourceType, typename TargetType = SourceType>
class PropertyBinding : public IPropertyBinding {
public:
    PropertyBinding(QObject* source_object, const QString& source_property,
                   QObject* target_object, const QString& target_property,
                   BindingDirection direction = BindingDirection::OneWay);

    // IPropertyBinding interface implementation...
};
```

#### Constructor

##### `PropertyBinding(QObject* source_object, const QString& source_property, QObject* target_object, const QString& target_property, BindingDirection direction = BindingDirection::OneWay)`

**Parameters:**

- `source_object`: Source object containing the property
- `source_property`: Name of the source property
- `target_object`: Target object to update
- `target_property`: Name of the target property
- `direction`: Binding direction

#### Methods

##### `update() override`

Manually triggers a binding update.

##### `disconnect() override`

Disconnects the binding.

##### `isValid() const override -> bool`

Checks if the binding is valid and connected.

##### `setUpdateMode(UpdateMode mode)`

Sets the update mode for the binding.

##### `setEnabled(bool enabled)`

Enables or disables the binding.

##### `setValidator(std::function<bool(const TargetType&)> validator)`

Sets a validation function for target values.

##### `setConverter(std::function<TargetType(const SourceType&)> converter)`

Sets a conversion function between source and target types.

### PropertyBindingManager Class

Manages multiple property bindings.

```cpp
class PropertyBindingManager : public QObject {
    Q_OBJECT

public:
    explicit PropertyBindingManager(QObject *parent = nullptr);
    ~PropertyBindingManager() override;

    // Binding management...
};
```

#### Methods

##### `addBinding(std::shared_ptr<IPropertyBinding> binding)`

Adds a binding to the manager.

##### `removeBinding(std::shared_ptr<IPropertyBinding> binding)`

Removes a binding from the manager.

##### `removeAllBindings()`

Removes all bindings.

##### `updateAllBindings()`

Updates all managed bindings.

##### `enableAllBindings()` / `disableAllBindings()`

Enables or disables all bindings.

##### `getBindingCount() const -> int`

Gets the number of managed bindings.

##### `getBindingsForWidget(QWidget *widget) const -> std::vector<std::shared_ptr<IPropertyBinding>>`

Gets all bindings for a specific widget.

### Usage Examples

#### Basic Property Binding

```cpp
#include "Binding/PropertyBinding.hpp"

// Create objects
auto slider = new QSlider();
auto label = new QLabel();

// Create one-way binding: slider value -> label text
auto binding = std::make_shared<PropertyBinding<int, QString>>(
    slider, "value",
    label, "text",
    BindingDirection::OneWay
);

// Set up converter
binding->setConverter([](const int& value) {
    return QString("Value: %1").arg(value);
});

// Add to manager
PropertyBindingManager manager;
manager.addBinding(binding);
```

#### Bidirectional Binding

```cpp
// Create two-way binding between line edits
auto edit1 = new QLineEdit();
auto edit2 = new QLineEdit();

auto binding = std::make_shared<PropertyBinding<QString>>(
    edit1, "text",
    edit2, "text",
    BindingDirection::TwoWay
);

manager.addBinding(binding);
```

#### State Manager Integration

```cpp
// Bind widget property to state manager
auto& state = StateManager::instance();
state.setState("sliderValue", 50);

auto slider = new QSlider();
auto state_property = state.getState<int>("sliderValue");

// Create binding between slider and state
auto binding = std::make_shared<PropertyBinding<int>>(
    state_property.get(), "value",
    slider, "value",
    BindingDirection::TwoWay
);

manager.addBinding(binding);
```

#### Complex Binding with Validation

```cpp
auto spinBox = new QSpinBox();
auto progressBar = new QProgressBar();

auto binding = std::make_shared<PropertyBinding<int>>(
    spinBox, "value",
    progressBar, "value",
    BindingDirection::OneWay
);

// Set up validation
binding->setValidator([](const int& value) {
    return value >= 0 && value <= 100;
});

// Set up converter with clamping
binding->setConverter([](const int& value) {
    return qBound(0, value, 100);
});

// Set deferred update mode for performance
binding->setUpdateMode(UpdateMode::Deferred);

manager.addBinding(binding);
```

## Best Practices

### State Management

- Use meaningful state keys
- Enable history for user-modifiable states
- Set up validation for critical states
- Use batch updates for multiple related changes
- Monitor performance for frequently updated states

### Property Binding

- Prefer one-way bindings when possible for performance
- Use validators to ensure data integrity
- Set up proper converters for type mismatches
- Use deferred updates for high-frequency changes
- Clean up bindings when widgets are destroyed

### Performance Optimization

- Use batch updates for multiple state changes
- Enable performance monitoring in development
- Use deferred binding updates for animations
- Avoid circular dependencies in state relationships
- Clear unused states and bindings regularly

### Error Handling

- Always validate binding objects before use
- Handle conversion errors gracefully
- Set up proper error callbacks
- Test binding scenarios thoroughly
- Use debug mode during development
