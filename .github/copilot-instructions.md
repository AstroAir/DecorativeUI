# DeclarativeUI Framework - AI Coding Assistant Instructions

## Project Overview

DeclarativeUI is a modern Qt6-based framework providing declarative UI development with C++20. The codebase centers around a fluent builder pattern, reactive state management, and an innovative command system for UI operations.

## Architecture & Core Concepts

### Main Components Structure

- **`src/Core/`** - UIElement base class, DeclarativeBuilder fluent API, memory management
- **`src/Components/`** - UI component library (30+ widgets with declarative wrappers)
- **`src/Command/`** - Command pattern implementation for all UI operations with automatic Qt widget mapping
- **`src/Binding/`** - Reactive state management (StateManager singleton, PropertyBinding system)
- **`src/Animation/`** - Centralized animation engine (migrated from Qt's QPropertyAnimation)
- **`src/JSON/`** - Dynamic UI loading from JSON with validation
- **`src/HotReload/`** - Real-time file watching and UI updates
- **`src/Debug/`** - Performance profiling and debugging tools

### Key Design Patterns

**Fluent Builder Pattern**: All UI construction uses method chaining via `DeclarativeBuilder`

```cpp
auto widget = Core::create<QWidget>()
    .layout<QVBoxLayout>([](auto& layout) { layout.spacing(10); })
    .child<Components::Button>([](auto& btn) { btn.text("Click"); })
    .build();
```

**Command System**: Every UI operation maps to commands with automatic undo support

```cpp
CommandSystem::execute<ButtonCommand>(widget, "setText", "New Text");
```

**Reactive State**: Central StateManager with automatic UI binding updates

```cpp
StateManager::instance().setState("count", 42);
// UI automatically updates via property bindings
```

## Build System & Development

### CMake Configuration

- **Build Type**: Use CMake presets (`cmake --preset debug|release|windows-mingw`)
- **Key Options**: `BUILD_EXAMPLES=ON`, `BUILD_TESTS=ON`, `BUILD_COMMAND_SYSTEM=ON`
- **Quick Build**: Run `build.bat` (Windows) or use ninja directly

### Testing Strategy (95%+ coverage)

- **Unit Tests**: `tests/unit/` - Component-specific testing
- **Integration**: `tests/integration/` - Cross-component workflows
- **Command Tests**: `tests/command/` - Command system and adapters
- **Performance**: `tests/performance/` - Benchmarking and stress tests
- **Run Tests**: `ctest --test-dir build` or individual executables in `build/tests/`

## Development Workflows

### Adding New Components

1. Create component class inheriting from appropriate Qt widget
2. Add declarative wrapper in `src/Components/`
3. Register in `ComponentUtils.hpp` type mapping
4. Add corresponding command class in `src/Command/ComponentCommands/`
5. Update `CommandFactory` registration
6. Add comprehensive tests in `tests/unit/test_components.cpp`

### State Management Integration

- Use `StateManager::setState(key, value)` for reactive updates
- Bind UI properties: `bindProperty("text", [](){ return StateManager::getState("key"); })`
- Computed properties automatically track dependencies

### Hot Reload Development

- JSON UI files in `Resources/ui_definitions/` auto-reload
- C++ components require recompilation but state persists
- Use `FileWatcher` for custom file monitoring

## Code Quality Standards

### C++20 Requirements

- Use concepts for type safety: `requires std::derived_from<T, QWidget>`
- Modern memory management with smart pointers
- Structured bindings and auto where appropriate

### Qt Integration Patterns

- Prefer Qt's built-in types (`QString`, `QVariant`, `QJsonObject`)
- Use `QMetaObject` for dynamic property access
- Leverage Qt's signal/slot for async operations

### Error Handling

- Custom exception hierarchy in `src/Exceptions/`
- Safe execution wrappers for all public APIs
- Comprehensive validation with detailed error messages

## Performance Considerations

### Memory Management

- `MemoryManager` provides optimized allocation for UI components
- Use `CacheManager` for expensive computations
- `ParallelProcessor` for multi-threaded operations

### Animation Performance

- New Animation engine replaces legacy QPropertyAnimation
- Centralized animation management prevents conflicts
- Use `AnimationConfig` for complex sequences

## Integration Points

### External Dependencies

- **Qt6**: Core, Widgets, Network, Test, Concurrent modules required
- **CMake 3.20+**: Build system with preset configurations
- **C++20 Compiler**: GCC 10+, Clang 12+, MSVC 2019+

### Cross-Module Communication

- Commands coordinate between systems via `CommandContext`
- State changes propagate through `PropertyBinding` system
- Adapters bridge Command system with other modules (5 adapter types)

## Common Pitfalls & Solutions

- **Animation Conflicts**: Always use centralized Animation engine, not direct QPropertyAnimation
- **Memory Leaks**: Use framework's `MemoryManager` for UI components
- **Thread Safety**: StateManager is thread-safe, but use Qt's signal/slot for cross-thread UI updates
- **Command Registration**: New commands must be registered in `CommandFactory::registerCommands()`
- **Build Issues**: Ensure Qt6 environment variables are set; use provided build scripts
