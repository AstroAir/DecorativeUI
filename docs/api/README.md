# DeclarativeUI API Reference

This section provides comprehensive API documentation for all DeclarativeUI modules and classes.

## 📚 Module Overview

### [Core Module](core.md)

Foundation classes that provide the base functionality for all UI components.

**Key Classes:**

- **UIElement** - Base class for all UI components with property management, event handling, and lifecycle management
- **DeclarativeBuilder** - Template-based builder pattern for fluent UI construction
- **PropertyValue** - Type-safe property value container
- **PerformanceMetrics** - Performance monitoring and optimization

**Core Concepts:**

- RAII-based resource management
- Fluent interface design
- Type-safe property system
- Exception safety guarantees

### [JSON Support Module](json.md)

Comprehensive JSON support for loading, parsing, validating, and managing UI definitions.

**Key Classes:**

- **JSONUILoader** - Load UI definitions from JSON files, strings, or objects
- **JSONParser** - Advanced JSON parsing with comments, includes, and preprocessing
- **JSONValidator** - Schema validation for UI JSON definitions
- **ComponentRegistry** - Component type registration and factory management

**Features:**

- JSON schema validation
- Custom property converters
- Event handler registration
- State manager integration
- Error handling and recovery

### [Hot Reload Module](hot-reload.md)

Real-time UI updates without application restart for rapid development workflows.

**Key Classes:**

- **HotReloadManager** - Coordinates hot reload system and manages file registration
- **FileWatcher** - Low-level file system monitoring with debouncing
- **PerformanceMonitor** - Tracks and analyzes hot reload performance metrics

**Features:**

- File system monitoring
- Automatic UI reconstruction
- Error handling and recovery
- Performance optimization
- Backup and restore functionality

### [Binding System Module](binding.md)

Comprehensive property binding system with state management and dependency tracking.

**Key Classes:**

- **StateManager** - Centralized state management with dependency tracking
- **PropertyBinding** - Type-safe property binding between objects
- **ReactiveProperty** - Reactive property implementation
- **PropertyBindingManager** - Manages multiple property bindings

**Features:**

- Bidirectional property binding
- State observation and dependency tracking
- Batch updates for performance
- History support with undo/redo
- Thread-safe operations

### [Command System Module](command.md)

Centralized command handling with support for synchronous/asynchronous execution and undo/redo.

**Key Classes:**

- **CommandSystem** - Core command execution engine
- **CommandInvoker** - Handles command execution with various modes
- **CommandManager** - High-level command management with history
- **ICommand** - Base interface for all commands
- **BuiltinCommands** - Standard command implementations

**Features:**

- Synchronous and asynchronous execution
- Command composition and chaining
- Undo/redo functionality
- Command interceptors
- UI integration helpers

### [Components Library Module](components.md)

Rich set of UI components built on top of the DeclarativeUI Core framework.

**Component Categories:**

#### Input Components

- **Button** - Customizable push button with click handling
- **LineEdit** - Single-line text input with validation
- **CheckBox** - Checkbox with tri-state support
- **ComboBox** - Dropdown selection with custom items
- **SpinBox** - Numeric input with spin controls
- **Slider** - Value selection slider with customization

#### Display Components

- **Label** - Text and image display with rich formatting
- **ProgressBar** - Progress indication with custom styling
- **TextEdit** - Multi-line text display and editing

#### Container Components

- **Container** - Generic container for grouping components
- **Layout** - Layout management with various layout types
- **GroupBox** - Grouped controls with border and title
- **TabWidget** - Tabbed interface with dynamic content

#### Advanced Components

- **TableView** - Tabular data display with sorting and selection
- **TreeView** - Hierarchical data display with expansion
- **MenuBar** - Application menu bar with actions
- **DateTimeEdit** - Date and time input with calendar

## 🚀 Quick Reference

### Common Patterns

#### Creating Components

```cpp
// Using DeclarativeBuilder
auto widget = Core::create<QWidget>()
    .property("windowTitle", "My App")
    .layout<QVBoxLayout>()
    .child<Components::Button>([](auto &btn) {
        btn.text("Click Me").onClick([]() { /* handler */ });
    })
    .build();

// Using Components directly
auto button = std::make_unique<Components::Button>();
button->text("Save")
      .onClick([]() { /* save logic */ })
      .initialize();
```

#### State Management

```cpp
// Set up state
auto& state = StateManager::instance();
state.setState("counter", 0);

// Bind to UI
label->bindProperty("text", []() {
    auto counter = StateManager::instance().getState<int>("counter");
    return QString("Count: %1").arg(counter->get());
});
```

#### JSON UI Loading

```cpp
auto loader = std::make_unique<JSON::JSONUILoader>();
loader->registerEventHandler("buttonClick", []() {
    qDebug() << "Button clicked!";
});

auto widget = loader->loadFromFile("ui/main.json");
```

#### Hot Reload Setup

```cpp
auto hot_reload = std::make_unique<HotReload::HotReloadManager>();
hot_reload->registerUIFile("ui/main.json", main_widget);
// Edit main.json and see changes immediately!
```

#### Command Execution

```cpp
CommandContext context;
context.setParameter("target", my_widget);
context.setParameter("property", "text");
context.setParameter("value", "New Text");

auto result = CommandManager::instance()
    .getInvoker()
    .execute("set_property", context);
```

## 📖 API Documentation Structure

Each module documentation includes:

### Class Reference

- **Class Declaration** - Full class signature with inheritance
- **Constructor/Destructor** - Object lifecycle management
- **Public Methods** - Complete method reference with parameters and return types
- **Signals** - Qt signals emitted by the class
- **Properties** - Accessible properties and their types

### Method Documentation

- **Method Signature** - Complete signature with template parameters
- **Parameters** - Detailed parameter descriptions
- **Return Values** - Return type and value descriptions
- **Exceptions** - Possible exceptions thrown
- **Usage Examples** - Practical code examples

### Usage Patterns

- **Basic Usage** - Simple usage examples
- **Advanced Patterns** - Complex scenarios and best practices
- **Integration Examples** - How to use with other modules
- **Performance Considerations** - Optimization tips and guidelines

### Error Handling

- **Exception Types** - Custom exception hierarchy
- **Error Recovery** - How to handle and recover from errors
- **Debugging Tips** - Common issues and solutions

## 🔍 Finding What You Need

### By Functionality

- **UI Creation**: [Core Module](core.md) → DeclarativeBuilder
- **JSON UIs**: [JSON Module](json.md) → JSONUILoader
- **State Management**: [Binding Module](binding.md) → StateManager
- **Hot Reload**: [Hot Reload Module](hot-reload.md) → HotReloadManager
- **Commands**: [Command Module](command.md) → CommandSystem
- **UI Components**: [Components Module](components.md) → Specific components

### By Use Case

- **Getting Started**: Start with [Core Module](core.md) and [Components Module](components.md)
- **Rapid Prototyping**: Use [JSON Module](json.md) for data-driven UIs
- **Development Workflow**: Set up [Hot Reload Module](hot-reload.md)
- **Complex Applications**: Leverage [Binding Module](binding.md) and [Command Module](command.md)

### By Component Type

- **Input Controls**: [Components Module](components.md) → Input Components
- **Display Elements**: [Components Module](components.md) → Display Components
- **Layout Management**: [Components Module](components.md) → Container Components
- **Advanced Widgets**: [Components Module](components.md) → Advanced Components

## 🛠️ Development Tools

### Type Safety

All APIs use C++20 concepts for compile-time type checking:

```cpp
template<QtWidget T>
concept ValidWidget = std::is_base_of_v<QWidget, T>;
```

### Exception Safety

All classes provide strong exception safety guarantees with RAII:

```cpp
class UIElement {
    std::unique_ptr<QWidget> widget_;  // Automatic cleanup
    // Exception-safe operations
};
```

### Performance Monitoring

Built-in performance tracking for optimization:

```cpp
auto metrics = element->getPerformanceMetrics();
qDebug() << "Render time:" << metrics.render_time_ms;
```

## 📋 Version Compatibility

- **DeclarativeUI 1.0+**: All documented APIs
- **Qt6 6.0+**: Required for all functionality
- **C++20**: Required for concepts and modern features

## 🤝 Contributing to Documentation

Found an error or want to improve the documentation?

1. Check the [Contributing Guidelines](../developer/contributing.md)
2. Submit issues for documentation bugs
3. Propose improvements via pull requests
4. Add examples and usage patterns

---

**Navigate to specific modules using the links above, or start with the [Core Module](core.md) for foundational concepts.**
