# Fundamentals Examples

Core concepts and essential patterns that every DeclarativeUI developer should understand.

## Prerequisites

- Completed **01-getting-started** examples
- Understanding of basic C++ and Qt concepts
- Familiarity with object-oriented programming

## Examples in this Category

### State Management (`state-management/`)

**Difficulty:** ⭐⭐⭐☆☆
**Concepts:** State management, reactive updates, data binding

Learn how to manage application state and create reactive user interfaces.

**What you'll learn:**

- Basic state management patterns
- Reactive UI updates
- Data flow in DeclarativeUI applications
- State synchronization between components

**Run this example:**

```bash
cmake --build . --target StateManagementExample
./examples/02-fundamentals/StateManagementExample
```

### JSON UI Loading (`json-ui-loading/`)

**Difficulty:** ⭐⭐⭐☆☆
**Concepts:** JSON-driven UI, dynamic UI creation, resource management

Discover how to create user interfaces from JSON configuration files.

**What you'll learn:**

- Loading UI definitions from JSON
- Dynamic UI creation
- Resource management
- Separation of UI structure from code

**Run this example:**

```bash
cmake --build . --target JSONUILoadingExample
./examples/02-fundamentals/JSONUILoadingExample
```

### Event Handling (`event-handling/`)

**Difficulty:** ⭐⭐☆☆☆
**Concepts:** Advanced event handling, custom events, event propagation

Advanced event handling patterns beyond basic signals and slots.

**What you'll learn:**

- Custom event creation
- Event propagation and filtering
- Advanced signal/slot patterns
- Event-driven architecture

**Run this example:**

```bash
cmake --build . --target EventHandlingExample
./examples/02-fundamentals/EventHandlingExample
```

### Property Binding (`property-binding/`)

**Difficulty:** ⭐⭐⭐☆☆
**Concepts:** Property binding, reactive programming, data synchronization

Learn how to create reactive connections between component properties.

**What you'll learn:**

- Property binding concepts
- Automatic UI updates
- Bidirectional data binding
- Reactive programming patterns

**Run this example:**

```bash
cmake --build . --target PropertyBindingExample
./examples/02-fundamentals/PropertyBindingExample
```

## Learning Path

1. **Start with:** `state-management/` - Foundation of reactive UIs
2. **Then:** `property-binding/` - Learn reactive connections
3. **Next:** `event-handling/` - Master event patterns
4. **Finally:** `json-ui-loading/` - Dynamic UI creation

## Key Concepts Covered

### State Management

- **Centralized state** - Single source of truth
- **Reactive updates** - Automatic UI synchronization
- **State mutations** - Controlled state changes
- **Observer patterns** - Watching for state changes

### Property Binding

- **One-way binding** - Data flows in one direction
- **Two-way binding** - Bidirectional data synchronization
- **Computed properties** - Derived values
- **Binding expressions** - Dynamic property connections

### Event Handling

- **Event types** - Different kinds of events
- **Event bubbling** - Event propagation through hierarchy
- **Event filtering** - Intercepting and modifying events
- **Custom events** - Creating application-specific events

### JSON UI Loading

- **UI schemas** - Defining UI structure in JSON
- **Dynamic creation** - Runtime UI generation
- **Resource loading** - Managing external resources
- **Configuration-driven** - Separating UI from logic

## Best Practices

1. **Keep state minimal** - Only store what you need
2. **Use reactive patterns** - Let the framework handle updates
3. **Separate concerns** - Keep UI logic separate from business logic
4. **Handle errors gracefully** - Always validate JSON and handle failures
5. **Document your state** - Make state structure clear

## Common Patterns

### State Updates

```cpp
// Good: Centralized state updates
stateManager.updateValue("counter", newValue);

// Avoid: Direct widget manipulation
widget->setText(QString::number(newValue));
```

### Property Binding

```cpp
// Good: Declarative binding
bindProperty(slider, "value", spinBox, "value");

// Avoid: Manual synchronization
connect(slider, &QSlider::valueChanged, [=](int value) {
    spinBox->setValue(value);
});
```

## Next Steps

After mastering these fundamentals, explore:

- **03-components/** - Advanced component usage
- **04-advanced-features/** - Hot reload and reactive state
- **05-command-system/** - Command-based architecture
