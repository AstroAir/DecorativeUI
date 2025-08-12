# DeclarativeUI - Modern C++/Qt6 Declarative UI Framework

## Project Overview

DeclarativeUI is a modern, declarative UI framework built on top of Qt6 that enables developers to create dynamic, responsive user interfaces using both C++ code and JSON definitions. The framework emphasizes modern C++20 features, RAII principles, and provides powerful features like hot reloading, property binding, and a comprehensive command system.

## Key Features

### 🚀 **Declarative Syntax**

- Fluent C++ API for building UIs declaratively
- JSON-based UI definitions for rapid prototyping
- Template-based builder pattern with type safety

### 🔥 **Hot Reload**

- Real-time UI updates without application restart
- File watching with automatic reload triggers
- Performance monitoring and optimization
- Backup and restore functionality for safe reloading

### 🔗 **Advanced Property Binding**

- Bidirectional property binding between UI elements
- State management with dependency tracking
- Batch updates for performance optimization
- History support with undo/redo functionality

### ⚡ **Command System**

- Centralized command handling and execution
- Built-in commands for common operations
- Keyboard shortcut integration
- Asynchronous command execution support

### 🎨 **Rich Component Library**

- Comprehensive set of UI components
- Input components: Button, LineEdit, CheckBox, ComboBox, SpinBox, Slider
- Display components: Label, ProgressBar, TextEdit
- Container components: Layout, GroupBox, TabWidget
- Advanced components: TableView, TreeView, MenuBar, DateTimeEdit

### 🛡️ **Modern C++ Design**

- C++20 standard with concepts and templates
- RAII and exception-safe resource management
- Move semantics and perfect forwarding
- Type-safe property system

## Architecture Overview

The DeclarativeUI framework is organized into several key modules:

```
DeclarativeUI/
├── Core/                   # Foundation classes
│   ├── UIElement          # Base UI element with property system
│   └── DeclarativeBuilder # Template-based builder pattern
├── JSON/                   # JSON support and validation
│   ├── JSONUILoader       # Load UIs from JSON definitions
│   ├── JSONParser         # JSON parsing utilities
│   ├── JSONValidator      # Schema validation
│   └── ComponentRegistry  # Component type registration
├── HotReload/             # Hot reload functionality
│   ├── FileWatcher        # File system monitoring
│   ├── HotReloadManager   # Reload coordination
│   └── PerformanceMonitor # Performance tracking
├── Binding/               # Property binding system
│   ├── StateManager       # Global state management
│   └── PropertyBinding    # Property binding implementation
├── Command/               # Command system
│   ├── CommandSystem      # Command execution engine
│   ├── BuiltinCommands    # Standard command implementations
│   └── CommandIntegration # UI integration helpers
└── Components/            # UI component library
    ├── Input/             # Input components
    ├── Display/           # Display components
    ├── Container/         # Layout and container components
    └── Advanced/          # Advanced components
```

## Core Concepts

### UIElement

The foundation of all UI components, providing:

- Property management with type safety
- Event handling system
- Animation support
- Theme and styling capabilities
- Performance monitoring
- Responsive design features

### DeclarativeBuilder

A template-based builder that enables fluent UI construction:

- Type-safe widget creation
- Property binding integration
- Event handler registration
- Layout management
- Exception-safe building

### State Management

Centralized state management with:

- Global state store
- Dependency tracking
- Batch updates
- History and undo/redo
- Performance monitoring

### Hot Reload System

Real-time development workflow:

- File system monitoring
- Automatic UI reconstruction
- Error handling and recovery
- Performance optimization

## Dependencies

### Required Dependencies

- **Qt6** (6.0 or higher)
  - Qt6::Core - Core Qt functionality
  - Qt6::Widgets - Widget system
  - Qt6::Network - Network operations for advanced features
  - Qt6::Test - Unit testing framework
- **CMake** (3.20 or higher) - Build system
- **C++20 compatible compiler**
  - GCC 10+ / Clang 10+ / MSVC 2019+

### Build Requirements

- Modern C++20 compiler with concepts support
- Qt6 development packages
- CMake build system
- Platform-specific build tools (Make, Ninja, or Visual Studio)

## Supported Platforms

- **Windows** (Windows 10/11, MSVC 2019+)
- **Linux** (Ubuntu 20.04+, GCC 10+)
- **macOS** (macOS 11+, Clang 12+)

## Performance Characteristics

- **Memory Management**: RAII-based with smart pointers
- **Update Performance**: Batched property updates (~60 FPS)
- **Hot Reload**: Sub-second reload times for typical UIs
- **Binding System**: Optimized dependency tracking
- **Component Creation**: Template-based zero-cost abstractions

## Design Philosophy

### Modern C++

- Leverage C++20 features for type safety and performance
- Use RAII for automatic resource management
- Prefer value semantics and move operations
- Template metaprogramming for compile-time optimizations

### Declarative Paradigm

- Describe what the UI should look like, not how to build it
- Separate UI structure from business logic
- Enable rapid iteration and prototyping
- Support both code and data-driven UI definitions

### Developer Experience

- Minimize boilerplate code
- Provide clear error messages
- Enable hot reload for rapid development
- Comprehensive documentation and examples

## Getting Started

For detailed setup and usage instructions, see:

- [User Guide](user-guide/getting-started.md) - Installation and basic usage
- [API Documentation](api/README.md) - Detailed API reference
- [Examples](examples/basic.md) - Code examples and tutorials
- [Developer Guide](developer/architecture.md) - Contributing and advanced topics

## License

This project is licensed under the MIT License - see the LICENSE file for details.
