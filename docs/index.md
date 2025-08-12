# DeclarativeUI Framework

A modern, declarative UI framework for Qt applications that provides both traditional component-based and innovative command-based UI development approaches.

![DeclarativeUI Logo](https://img.shields.io/badge/DeclarativeUI-v1.0.0-blue?style=for-the-badge)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg?style=for-the-badge)](https://opensource.org/licenses/MIT)
[![Qt6](https://img.shields.io/badge/Qt-6.2+-green.svg?style=for-the-badge)](https://www.qt.io/)
[![C++20](https://img.shields.io/badge/C++-20-blue.svg?style=for-the-badge)](https://en.cppreference.com/w/cpp/20)

## 🚀 Key Features

### Core Framework
- **Declarative Syntax**: Fluent, chainable API for building UIs with modern C++20
- **Modern C++20**: Type-safe, RAII-based design with move semantics and concepts
- **Qt6 Integration**: Full compatibility with Qt6 widgets, layouts, and event system
- **Performance Optimized**: Efficient property binding, caching, and memory management

### Component System
- **Rich Component Library**: 30+ pre-built UI components covering all common use cases
- **Custom Components**: Easy creation of reusable custom components with fluent interfaces
- **Property Binding**: Reactive property system with automatic UI updates and validation
- **Event Handling**: Type-safe event system with lambda support and flexible registration

### Command System (Advanced)
- **Command-Based Architecture**: Modern UI development with separation of concerns
- **Declarative Builders**: CommandBuilder and CommandHierarchyBuilder for fluent construction
- **Widget Mapping**: Automatic Command-to-QWidget translation with bidirectional sync
- **State Integration**: Seamless integration with centralized state management

### State Management
- **Centralized State**: Global reactive state store with dependency tracking
- **Reactive Properties**: Automatic UI updates when state changes with computed properties
- **Type Safety**: Template-based type-safe state access with compile-time validation
- **Batch Updates**: Efficient handling of multiple state changes with transaction support

### Hot Reload System
- **Real-time Updates**: Instant UI updates without application restart
- **File Watching**: Advanced file system monitoring with debouncing and error recovery
- **Performance Monitoring**: Detailed metrics, bottleneck detection, and optimization recommendations
- **Error Recovery**: Robust error handling with fallback mechanisms and user feedback

## 🎯 Quick Example

```cpp
#include "DeclarativeUI.hpp"
using namespace DeclarativeUI;

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    
    // Create a simple counter app with declarative syntax
    auto counter = Core::create<QWidget>()
        .layout<QVBoxLayout>([](auto& layout) {
            layout.spacing(10).margins(20);
        })
        .child<Components::Label>([](auto& label) {
            label.text("Counter: 0")
                 .alignment(Qt::AlignCenter)
                 .bindProperty("text", []() {
                     return QString("Counter: %1").arg(StateManager::instance().getState("count", 0));
                 });
        })
        .child<Components::Button>([](auto& btn) {
            btn.text("Increment")
               .onClick([]() {
                   int current = StateManager::instance().getState("count", 0);
                   StateManager::instance().setState("count", current + 1);
               });
        })
        .build();
    
    counter->show();
    return app.exec();
}
```

## 🚀 Getting Started

### Prerequisites

- **Qt6** (6.2 or later) with Core, Widgets, Network, and Test modules
- **CMake** 3.20 or higher  
- **C++20 compatible compiler** (GCC 10+, Clang 12+, MSVC 2019+)

### Quick Installation

=== "Windows"

    ```bat
    # Clone the repository
    git clone https://github.com/DeclarativeUI/DeclarativeUI.git
    cd DeclarativeUI
    
    # Build with provided script
    build.bat
    
    # Run an example
    build\examples\basic\HelloWorldExample.exe
    ```

=== "Linux"

    ```bash
    # Clone the repository
    git clone https://github.com/DeclarativeUI/DeclarativeUI.git
    cd DeclarativeUI
    
    # Build with CMake
    cmake --preset default
    cmake --build --preset default
    
    # Run tests
    cd build && ctest --output-on-failure
    
    # Run an example
    ./examples/basic/HelloWorldExample
    ```

=== "macOS"

    ```bash
    # Install Qt6 via Homebrew
    brew install qt6
    
    # Clone and build
    git clone https://github.com/DeclarativeUI/DeclarativeUI.git
    cd DeclarativeUI
    cmake --preset default
    cmake --build --preset default
    
    # Run an example
    ./build/examples/basic/HelloWorldExample
    ```

## 📚 Documentation

- **[Getting Started Guide](user-guide/getting-started.md)** - Step-by-step setup and first application
- **[API Reference](api/core.md)** - Complete API documentation
- **[Examples](examples/basic.md)** - 25+ example applications
- **[Integration Guide](integration-guide.md)** - Integrating with existing Qt projects
- **[Developer Guide](developer/contributing.md)** - Contributing and development

## 🎯 Why DeclarativeUI?

DeclarativeUI bridges the gap between traditional Qt development and modern UI frameworks:

- **Familiar**: Built on Qt6, works with existing Qt knowledge
- **Modern**: C++20 features, reactive patterns, declarative syntax  
- **Flexible**: Choose between Components or Commands based on your needs
- **Productive**: Hot reload, state management, and comprehensive tooling
- **Maintainable**: Clear separation of concerns, testable architecture

Whether you're building a simple desktop application or a complex enterprise system, DeclarativeUI provides the tools and patterns to create maintainable, performant Qt applications with modern development workflows.

## 🤝 Contributing

We welcome contributions! See our [Contributing Guide](developer/contributing.md) for details on:

- Setting up the development environment
- Coding standards and best practices
- Submitting pull requests
- Reporting issues

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](license.md) file for details.
