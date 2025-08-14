# DeclarativeUI Framework

[![CI](https://github.com/DeclarativeUI/DeclarativeUI/workflows/CI/badge.svg)](https://github.com/DeclarativeUI/DeclarativeUI/actions/workflows/ci.yml)
[![Documentation](https://github.com/DeclarativeUI/DeclarativeUI/workflows/Documentation/badge.svg)](https://declarativeui.github.io/DeclarativeUI/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Qt6](https://img.shields.io/badge/Qt-6.2+-green.svg)](https://www.qt.io/)
[![C++20](https://img.shields.io/badge/C++-20-blue.svg)](https://en.cppreference.com/w/cpp/20)
[![Release](https://img.shields.io/github/v/release/DeclarativeUI/DeclarativeUI)](https://github.com/DeclarativeUI/DeclarativeUI/releases)

A modern, declarative UI framework for Qt applications that provides both traditional component-based and innovative command-based UI development approaches. DeclarativeUI enables rapid development with hot reload capabilities, reactive state management, and seamless integration with existing Qt projects.

## 📖 Documentation

**[📚 Full Documentation](https://declarativeui.github.io/DeclarativeUI/)** | **[🚀 Quick Start](https://declarativeui.github.io/DeclarativeUI/user-guide/quick-start/)** | **[📋 API Reference](https://declarativeui.github.io/DeclarativeUI/api/core/)** | **[💡 Examples](https://declarativeui.github.io/DeclarativeUI/examples/basic/)**

## 🚀 Key Features

- **🎨 Declarative Syntax**: Fluent, chainable API for building UIs with modern C++20
- **⚡ Performance Optimized**: Efficient property binding, caching, and memory management
- **🔧 Rich Component Library**: 30+ pre-built UI components covering all common use cases
- **🔄 Hot Reload System**: Real-time UI updates without application restart
- **📊 State Management**: Centralized reactive state store with automatic UI updates
- **📋 Command System**: Comprehensive component-specific commands with automatic widget mapping and undo support
- **📄 JSON Support**: Dynamic UI loading from JSON with validation and error reporting
- **🧪 Comprehensive Testing**: 24+ test executables with 95%+ code coverage

## 🎯 Quick Example

```cpp
#include "DeclarativeUI.hpp"
using namespace DeclarativeUI;

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Create a counter app with declarative syntax
    auto counter = Core::create<QWidget>()
        .windowTitle("Counter App")
        .layout<QVBoxLayout>([](auto& layout) {
            layout.spacing(10).margins(20);
        })
        .child<Components::Label>([](auto& label) {
            label.bindProperty("text", []() {
                int count = StateManager::instance().getState("count", 0);
                return QString("Count: %1").arg(count);
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

## 🚀 Installation

### Prerequisites

- **Qt6** (6.2 or later) with Core, Widgets, Network, and Test modules
- **CMake** 3.20 or higher
- **C++20 compatible compiler** (GCC 10+, Clang 12+, MSVC 2019+)

### Quick Start

=== "Windows"
    ```bat
    git clone https://github.com/DeclarativeUI/DeclarativeUI.git
    cd DeclarativeUI
    build.bat
    build\examples\basic\HelloWorldExample.exe
    ```

=== "Linux"
    ```bash
    git clone https://github.com/DeclarativeUI/DeclarativeUI.git
    cd DeclarativeUI
    cmake --preset default
    cmake --build --preset default
    ./build/examples/basic/HelloWorldExample
    ```

=== "macOS"
    ```bash
    brew install qt6
    git clone https://github.com/DeclarativeUI/DeclarativeUI.git
    cd DeclarativeUI
    cmake --preset default
    cmake --build --preset default
    ./build/examples/basic/HelloWorldExample
    ```

## 📚 Documentation

- **[📖 Getting Started](https://declarativeui.github.io/DeclarativeUI/user-guide/getting-started/)** - Complete setup and first application
- **[🎯 Quick Start](https://declarativeui.github.io/DeclarativeUI/user-guide/quick-start/)** - Get running in 5 minutes
- **[🧩 Component Guide](https://declarativeui.github.io/DeclarativeUI/user-guide/components/)** - 30+ UI components
- **[⚡ Command System](https://declarativeui.github.io/DeclarativeUI/user-guide/commands/)** - Advanced architecture
- **[📊 State Management](https://declarativeui.github.io/DeclarativeUI/user-guide/state-management/)** - Reactive state
- **[🔄 Hot Reload](https://declarativeui.github.io/DeclarativeUI/user-guide/hot-reload/)** - Real-time development
- **[📋 API Reference](https://declarativeui.github.io/DeclarativeUI/api/core/)** - Complete API docs
- **[💡 Examples](https://declarativeui.github.io/DeclarativeUI/examples/basic/)** - 25+ example applications

## 🛠️ Build Options

| Preset | Description | Use Case |
|--------|-------------|----------|
| `default` | Release build with all features | Production builds |
| `debug` | Debug build with diagnostics | Development |
| `minimal` | Library only | Integration projects |
| `command-dev` | Command system development | Advanced features |

```bash
# Use presets for easy building
cmake --preset default
cmake --build --preset default
```

## 🧩 Architecture

DeclarativeUI provides two complementary approaches with a modern animation system:

### Component System

Traditional widget-based development with modern C++20 syntax:

```cpp
auto button = std::make_unique<Components::Button>();
button->text("Click Me")
      .onClick([]() { /* handler */ })
      .fadeIn(500ms);  // Modern animation support
```

### Command System

Framework-independent UI logic with comprehensive component support:

```cpp
// Component-specific commands for all UI elements
CommandContext context;
context.setParameter("widget", "myButton");
context.setParameter("operation", "setText");
context.setParameter("text", "Click Me!");

ButtonCommand command(context);
auto result = command.execute(context);

// Automatic widget mapping and event handling
auto& mapper = WidgetMapper::instance();
auto widget = mapper.createWidget(&command);
```

### Animation System

Centralized, high-performance animation engine with modern features:

```cpp
// Fluent animation API
auto element = UIElement::create<QPushButton>("Animated Button")
    .fadeIn(AnimationConfig{500ms, QEasingCurve::OutCubic})
    .slideIn("left", 300ms)
    .onClicked([](){ /* handle click */ });

// Advanced animation sequences
auto& engine = Animation::AnimationEngine::instance();
auto animation = engine.animateProperty(widget, "opacity", 0.0, 1.0, 500);
```

**[📖 Learn More](https://declarativeui.github.io/DeclarativeUI/user-guide/components/)** about choosing the right approach for your project.

## 🧪 Testing

Comprehensive test suite with 24+ test executables:

- **Unit Tests**: 95%+ code coverage for individual components
- **Integration Tests**: End-to-end workflow validation
- **Command Tests**: Complete Command system validation
- **Performance Tests**: Benchmarking and regression testing

```bash
# Run all tests
ctest --output-on-failure

# Run specific test categories
ctest -R "unit|command|integration"
```

## 💡 Examples

25+ example applications organized by complexity:

- **[Basic Examples](https://declarativeui.github.io/DeclarativeUI/examples/basic/)** - Hello World, JSON loading, state management
- **[Component Examples](https://declarativeui.github.io/DeclarativeUI/examples/components/)** - Showcase of 30+ UI components
- **[Command Examples](https://declarativeui.github.io/DeclarativeUI/examples/commands/)** - Advanced command-based architecture
- **[Advanced Examples](https://declarativeui.github.io/DeclarativeUI/examples/advanced/)** - Real-world applications

```bash
# Build and run examples
cmake --build . --target examples
./build/examples/basic/HelloWorldExample
```

## 🤝 Contributing

We welcome contributions! Please see our [Contributing Guide](CONTRIBUTING.md) for details.

### Quick Contribution Steps

1. Fork the repository
2. Create a feature branch: `git checkout -b feature/amazing-feature`
3. Make your changes and add tests
4. Submit a pull request

### Areas for Contribution

- 🐛 **Bug fixes** - Fix issues in existing functionality
- ✨ **New features** - Add components or capabilities
- 📚 **Documentation** - Improve guides and examples
- 🧪 **Testing** - Expand test coverage
- ⚡ **Performance** - Optimize existing code

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🎯 Why DeclarativeUI?

- **Familiar**: Built on Qt6, works with existing Qt knowledge
- **Modern**: C++20 features, reactive patterns, declarative syntax
- **Flexible**: Choose between Components or Commands based on your needs
- **Productive**: Hot reload, state management, and comprehensive tooling
- **Maintainable**: Clear separation of concerns, testable architecture

---

**DeclarativeUI** - Making Qt development modern, productive, and enjoyable! 🚀

## 🔗 Links

- **[📖 Documentation](https://declarativeui.github.io/DeclarativeUI/)** - Complete guides and API reference
- **[🐛 Issues](https://github.com/DeclarativeUI/DeclarativeUI/issues)** - Bug reports and feature requests
- **[💬 Discussions](https://github.com/DeclarativeUI/DeclarativeUI/discussions)** - Community support
- **[🚀 Releases](https://github.com/DeclarativeUI/DeclarativeUI/releases)** - Download latest version
