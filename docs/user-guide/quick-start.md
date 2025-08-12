# Quick Start Guide

This guide will help you get up and running with DeclarativeUI in just a few minutes.

## Prerequisites

Before you begin, ensure you have:

- **Qt6** (6.2 or later) with development packages
- **CMake** 3.20 or higher
- **C++20 compatible compiler**:
  - GCC 10+ (Linux)
  - Clang 12+ (macOS)
  - MSVC 2019+ (Windows)

## Installation

### 1. Clone the Repository

```bash
git clone https://github.com/DeclarativeUI/DeclarativeUI.git
cd DeclarativeUI
```

### 2. Build the Project

=== "Windows"

    ```bat
    # Use the provided build script
    build.bat

    # Or use CMake directly
    cmake --preset default
    cmake --build --preset default
    ```

=== "Linux/macOS"

    ```bash
    # Configure and build
    cmake --preset default
    cmake --build --preset default

    # Run tests to verify
    cd build
    ctest --output-on-failure
    ```

### 3. Verify Installation

Run a simple example to verify everything works:

=== "Windows"

    ```bat
    build\examples\basic\HelloWorldExample.exe
    ```

=== "Linux/macOS"

    ```bash
    ./build/examples/basic/HelloWorldExample
    ```

## Your First Application

Let's create a simple "Hello World" application:

### 1. Create the Source File

Create a new file `my_first_app.cpp`:

```cpp
#include <QApplication>
#include "src/Core/UIElement.hpp"
#include "src/Components/Label.hpp"
#include "src/Components/Button.hpp"
#include "src/Components/Container.hpp"

using namespace DeclarativeUI;

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Create a simple UI with declarative syntax
    auto window = Core::create<QWidget>()
        .windowTitle("My First DeclarativeUI App")
        .resize(300, 200)
        .layout<QVBoxLayout>([](auto& layout) {
            layout.spacing(10).margins(20);
        })
        .child<Components::Label>([](auto& label) {
            label.text("Hello, DeclarativeUI!")
                 .alignment(Qt::AlignCenter)
                 .style("font-size: 18px; font-weight: bold;");
        })
        .child<Components::Button>([](auto& btn) {
            btn.text("Click Me!")
               .onClick([]() {
                   qDebug() << "Button clicked!";
               });
        })
        .build();

    window->show();
    return app.exec();
}
```

### 2. Create CMakeLists.txt

Create a `CMakeLists.txt` file for your application:

```cmake
cmake_minimum_required(VERSION 3.20)
project(MyFirstApp)

# Find DeclarativeUI (assuming it's installed or in parent directory)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Add DeclarativeUI as subdirectory (if building from source)
add_subdirectory(path/to/DeclarativeUI)

# Create your application
add_executable(MyFirstApp my_first_app.cpp)

target_link_libraries(MyFirstApp
    DeclarativeUI
    Qt6::Core
    Qt6::Widgets
)

# Enable Qt MOC
set_target_properties(MyFirstApp PROPERTIES
    AUTOMOC ON
)
```

### 3. Build and Run

```bash
mkdir build && cd build
cmake ..
cmake --build .
./MyFirstApp  # Linux/macOS
# or MyFirstApp.exe  # Windows
```

## Next Steps

Now that you have DeclarativeUI working, explore these areas:

### Learn the Basics

- **[Component System](components.md)** - Learn about the 30+ built-in components
- **[State Management](state-management.md)** - Understand reactive state and property binding
- **[JSON Support](json-support.md)** - Load UIs from JSON definitions

### Explore Advanced Features

- **[Command System](commands.md)** - Modern command-based UI architecture
- **[Hot Reload](hot-reload.md)** - Real-time development workflow
- **[Integration Guide](../integration-guide.md)** - Integrate with existing Qt projects

### Browse Examples

- **[Basic Examples](../examples/basic.md)** - Simple demonstrations
- **[Component Examples](../examples/components.md)** - Component showcase
- **[Advanced Examples](../examples/advanced.md)** - Real-world applications

### Get Involved

- **[Contributing](../developer/contributing.md)** - Help improve DeclarativeUI
- **[Architecture](../developer/architecture.md)** - Understand the framework design
- **[Testing](../developer/testing.md)** - Learn about our testing practices

## Common Issues

### Qt6 Not Found

If CMake can't find Qt6, set the Qt installation path:

```bash
export CMAKE_PREFIX_PATH=/path/to/qt6
# or on Windows:
set CMAKE_PREFIX_PATH=C:\Qt\6.5.0\msvc2019_64
```

### Compiler Issues

Ensure you're using a C++20 compatible compiler:

```bash
# Check compiler version
gcc --version    # Should be 10+
clang --version  # Should be 12+
```

### Build Errors

If you encounter build errors:

1. Clean the build directory: `rm -rf build`
2. Verify Qt6 installation and PATH
3. Check compiler compatibility
4. Review the [troubleshooting guide](troubleshooting.md)

## Need Help?

- 📖 **Documentation**: Browse our comprehensive [documentation](../api/core.md)
- 💬 **Discussions**: Ask questions in [GitHub Discussions](https://github.com/DeclarativeUI/DeclarativeUI/discussions)
- 🐛 **Issues**: Report bugs or request features in [GitHub Issues](https://github.com/DeclarativeUI/DeclarativeUI/issues)
- 📧 **Contact**: Reach out to maintainers for complex questions

Welcome to DeclarativeUI! 🎉
