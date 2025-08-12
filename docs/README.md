# DeclarativeUI Documentation

Welcome to the DeclarativeUI documentation! This comprehensive guide will help you understand, use, and contribute to the DeclarativeUI framework.

DeclarativeUI provides two complementary approaches for building user interfaces:

- **🏗️ Legacy Component System**: Traditional widget-based components with fluent API
- **⚡ Command System (Recommended)**: Modern command-based architecture with automatic state management and widget independence

## 📚 Documentation Structure

### [Project Overview](overview.md)

Get started with understanding what DeclarativeUI is, its key features, architecture overview, and core concepts.

### [API Documentation](api/)

Detailed reference documentation for all DeclarativeUI modules and classes:

- **[Core API](api/core.md)** - UIElement and DeclarativeBuilder foundation classes
- **[JSON Support](api/json.md)** - JSON UI loading, parsing, validation, and component registry
- **[Hot Reload](api/hot-reload.md)** - File watching, reload management, and performance monitoring
- **[Binding System](api/binding.md)** - State management and property binding
- **[Command System](api/command.md)** - Modern command-based UI architecture with state binding and event handling
- **[Components Library](api/components.md)** - Complete UI component reference

### [User Guide](user-guide/)

Step-by-step guides for using DeclarativeUI in your applications:

- **[Getting Started](user-guide/getting-started.md)** - Installation, setup, and your first application
- **[Examples](user-guide/examples.md)** - Comprehensive code examples and usage patterns
- **[Best Practices](user-guide/best-practices.md)** - Recommended patterns and optimization techniques

### [Developer Documentation](developer/)

In-depth information for contributors and advanced users:

- **[Architecture](developer/architecture.md)** - Framework design, implementation details, and design decisions
- **[Contributing](developer/contributing.md)** - How to contribute code, documentation, and tests
- **[Testing](developer/testing.md)** - Comprehensive testing guidelines and examples

## 🚀 Quick Start

### Installation

```bash
# Clone the repository
git clone https://github.com/your-org/DeclarativeUI.git
cd DeclarativeUI

# Build the project
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_EXAMPLES=ON
cmake --build .
```

### Your First Application

```cpp
#include <QApplication>
#include "Core/DeclarativeBuilder.hpp"
#include "Components/Button.hpp"
#include "Components/Label.hpp"

using namespace DeclarativeUI;

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    auto window = Core::create<QWidget>()
        .property("windowTitle", "Hello DeclarativeUI")
        .property("minimumSize", QSize(300, 200))
        .layout<QVBoxLayout>()
        .child<Components::Label>([](auto &label) {
            label.text("Welcome to DeclarativeUI!");
        })
        .child<Components::Button>([](auto &button) {
            button.text("Click Me!")
                  .onClick([]() {
                      qDebug() << "Hello from DeclarativeUI!";
                  });
        })
        .build();

    window->show();
    return app.exec();
}
```

## 🎯 Key Features

### Declarative Syntax

Build UIs using a fluent, declarative API that describes what your UI should look like:

```cpp
auto widget = create<QWidget>()
    .layout<QVBoxLayout>()
    .child<Button>([](auto &btn) {
        btn.text("Save").onClick([]() { /* save logic */ });
    })
    .build();
```

### JSON-Based UI Definitions

Create UIs from JSON for rapid prototyping and dynamic interfaces:

```json
{
  "type": "QWidget",
  "layout": { "type": "VBoxLayout" },
  "children": [
    {
      "type": "QPushButton",
      "properties": { "text": "Click Me" },
      "events": { "clicked": "handleClick" }
    }
  ]
}
```

### Hot Reload Development

Make changes to your UI definitions and see them instantly without restarting your application:

```cpp
auto hot_reload = std::make_unique<HotReload::HotReloadManager>();
hot_reload->registerUIFile("ui/main.json", main_widget);
// Edit main.json and see changes immediately!
```

### State Management

Centralized, reactive state management with property binding:

```cpp
auto& state = StateManager::instance();
state.setState("counter", 0);

label->bindProperty("text", []() {
    auto counter = StateManager::instance().getState<int>("counter");
    return QString("Count: %1").arg(counter->get());
});
```

### Command System

Centralized command handling with undo/redo support:

```cpp
CommandManager::instance().getInvoker().execute("save_file", context);
```

## 📖 Learning Path

### For Beginners

1. Start with [Project Overview](overview.md) to understand the framework
2. Follow the [Getting Started Guide](user-guide/getting-started.md)
3. Explore [Examples](user-guide/examples.md) to see common patterns
4. Review [Best Practices](user-guide/best-practices.md) for optimal usage

### For Experienced Developers

1. Review [Architecture Documentation](developer/architecture.md) for design insights
2. Check [API Documentation](api/) for detailed reference
3. Explore [Contributing Guidelines](developer/contributing.md) to get involved
4. Study [Testing Documentation](developer/testing.md) for quality assurance

### For Contributors

1. Read [Contributing Guidelines](developer/contributing.md) thoroughly
2. Understand the [Architecture](developer/architecture.md) and design decisions
3. Follow [Testing Guidelines](developer/testing.md) for quality code
4. Check existing issues and discussions for contribution opportunities

## 🔧 Requirements

### System Requirements

- **Qt6** (6.0 or higher) with development packages
- **CMake** (3.20 or higher)
- **C++20 compatible compiler**:
  - GCC 10+ (Linux)
  - Clang 10+ (macOS)
  - MSVC 2019+ (Windows)

### Supported Platforms

- Windows 10/11 (MSVC 2019+)
- Linux (Ubuntu 20.04+, GCC 10+)
- macOS (macOS 11+, Clang 12+)

## 📋 Examples Overview

### Basic Components

- Simple button with click handling
- Form inputs with validation
- Layout management
- Custom component creation

### State Management

- Counter application with undo/redo
- Data binding between components
- Computed state properties
- Batch state updates

### JSON UIs

- Loading UIs from JSON files
- Dynamic UI generation
- Event handler registration
- Property binding in JSON

### Hot Reload

- Development workflow setup
- File watching configuration
- Error handling and recovery
- Performance monitoring

### Advanced Features

- Custom command implementation
- Component composition patterns
- Performance optimization techniques
- Testing strategies

## 🤝 Community and Support

### Getting Help

- **Documentation**: Comprehensive guides and API reference
- **Examples**: Working code samples for common scenarios
- **Issues**: Report bugs or request features on GitHub
- **Discussions**: Ask questions and share ideas

### Contributing

We welcome contributions! Whether you're:

- Fixing bugs
- Adding new features
- Improving documentation
- Writing tests
- Sharing examples

Please read our [Contributing Guidelines](developer/contributing.md) to get started.

### Code of Conduct

We are committed to providing a welcoming and inclusive environment for all contributors. Please be respectful and constructive in all interactions.

## 📄 License

DeclarativeUI is licensed under the MIT License. See the LICENSE file for details.

## 🔗 Quick Links

- [Project Overview](overview.md) - Start here for an introduction
- [Getting Started](user-guide/getting-started.md) - Your first DeclarativeUI app
- [API Reference](api/) - Complete API documentation
- [Examples](user-guide/examples.md) - Code examples and patterns
- [Contributing](developer/contributing.md) - How to contribute
- [Architecture](developer/architecture.md) - Framework design details

---

**Happy coding with DeclarativeUI!** 🎉

If you find this documentation helpful, please consider starring the project and sharing it with others who might benefit from declarative UI development in C++/Qt6.
