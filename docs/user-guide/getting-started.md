# Getting Started with DeclarativeUI

This guide will help you get up and running with DeclarativeUI, a modern C++/Qt6 declarative UI framework.

## Prerequisites

Before you begin, ensure you have the following installed:

### Required Software
- **Qt6** (6.0 or higher) with development packages
- **CMake** (3.20 or higher)
- **C++20 compatible compiler**:
  - GCC 10+ (Linux)
  - Clang 10+ (macOS)
  - MSVC 2019+ (Windows)

### Platform-Specific Requirements

#### Windows
- Visual Studio 2019 or later with C++ development tools
- Qt6 installed via Qt Installer or vcpkg
- CMake (can be installed via Visual Studio Installer)

#### Linux (Ubuntu/Debian)
```bash
sudo apt update
sudo apt install build-essential cmake qt6-base-dev qt6-tools-dev
```

#### macOS
```bash
# Using Homebrew
brew install cmake qt6
```

## Installation

### Option 1: Clone and Build from Source

1. **Clone the repository:**
```bash
git clone https://github.com/your-org/DeclarativeUI.git
cd DeclarativeUI
```

2. **Build using the provided script (Windows):**
```bat
build.bat
```

3. **Or build manually:**
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_EXAMPLES=ON -DBUILD_TESTS=ON
cmake --build . --config Release
```

### Option 2: Using CMake FetchContent

Add to your `CMakeLists.txt`:
```cmake
include(FetchContent)
FetchContent_Declare(
    DeclarativeUI
    GIT_REPOSITORY https://github.com/your-org/DeclarativeUI.git
    GIT_TAG main
)
FetchContent_MakeAvailable(DeclarativeUI)

target_link_libraries(your_target DeclarativeUI Components)
```

## Your First DeclarativeUI Application

Let's create a simple "Hello World" application to get familiar with the framework.

### 1. Basic Setup

Create a new file `main.cpp`:

```cpp
#include <QApplication>
#include <QWidget>
#include <memory>

#include "Core/DeclarativeBuilder.hpp"
#include "Components/Button.hpp"
#include "Components/Label.hpp"

using namespace DeclarativeUI;

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Create main window using declarative syntax
    auto mainWindow = Core::create<QWidget>()
        .property("windowTitle", "Hello DeclarativeUI")
        .property("minimumSize", QSize(300, 200))
        .layout<QVBoxLayout>([](auto *layout) {
            layout->setSpacing(20);
            layout->setContentsMargins(20, 20, 20, 20);
        })
        .child<Components::Label>([](auto &label) {
            label.text("Welcome to DeclarativeUI!")
                 .style("QLabel { font-size: 18px; font-weight: bold; }");
        })
        .child<Components::Button>([](auto &button) {
            button.text("Click Me!")
                  .onClick([]() {
                      qDebug() << "Hello from DeclarativeUI!";
                  });
        })
        .build();
    
    mainWindow->show();
    return app.exec();
}
```

### 2. CMakeLists.txt

Create a `CMakeLists.txt` file:

```cmake
cmake_minimum_required(VERSION 3.20)
project(HelloDeclarativeUI)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Add DeclarativeUI (adjust path as needed)
add_subdirectory(path/to/DeclarativeUI)

add_executable(HelloDeclarativeUI main.cpp)

target_link_libraries(HelloDeclarativeUI 
    DeclarativeUI 
    Components
    Qt6::Core 
    Qt6::Widgets
)

set_target_properties(HelloDeclarativeUI PROPERTIES
    AUTOMOC ON
)
```

### 3. Build and Run

```bash
mkdir build && cd build
cmake ..
cmake --build .
./HelloDeclarativeUI  # Linux/macOS
# or HelloDeclarativeUI.exe on Windows
```

## Core Concepts

### Declarative Syntax

DeclarativeUI uses a fluent, declarative syntax that describes what your UI should look like:

```cpp
auto widget = Core::create<QWidget>()
    .property("windowTitle", "My App")
    .layout<QVBoxLayout>()
    .child<Components::Button>([](auto &btn) {
        btn.text("Save").onClick([]() { /* save logic */ });
    })
    .build();
```

### Component Hierarchy

Components are organized in a hierarchy:
- **Core**: Base classes (UIElement, DeclarativeBuilder)
- **Components**: UI widgets (Button, Label, LineEdit, etc.)
- **JSON**: JSON-based UI loading
- **HotReload**: Real-time UI updates
- **Binding**: State management and property binding
- **Command**: Command system for actions

### Property Binding

Connect UI elements to data sources:

```cpp
#include "Binding/StateManager.hpp"

// Set up state
auto& state = StateManager::instance();
state.setState("counter", 0);

// Bind label to state
auto label = std::make_unique<Components::Label>();
label->bindProperty("text", []() {
    auto counter = StateManager::instance().getState<int>("counter");
    return QString("Count: %1").arg(counter->get());
});
```

## Working with Components

### Input Components

```cpp
// Button with click handler
auto button = std::make_unique<Components::Button>();
button->text("Submit")
      .onClick([]() { qDebug() << "Form submitted!"; });

// Text input with validation
auto lineEdit = std::make_unique<Components::LineEdit>();
lineEdit->placeholder("Enter email...")
        .onTextChanged([](const QString& text) {
            // Validate email format
            bool valid = text.contains("@");
            // Update UI based on validation
        });

// Dropdown selection
auto comboBox = std::make_unique<Components::ComboBox>();
comboBox->addItem("Option 1")
        .addItem("Option 2")
        .onCurrentTextChanged([](const QString& text) {
            qDebug() << "Selected:" << text;
        });
```

### Layout Management

```cpp
auto mainWidget = Core::create<QWidget>()
    .layout<QVBoxLayout>([](auto *layout) {
        layout->setSpacing(10);
        layout->setContentsMargins(20, 20, 20, 20);
    })
    .child<QWidget>([](auto &row) {
        row.layout<QHBoxLayout>()
           .child<Components::Label>([](auto &label) {
               label.text("Name:");
           })
           .child<Components::LineEdit>([](auto &edit) {
               edit.placeholder("Enter name...");
           });
    })
    .build();
```

## JSON-Based UI Definition

DeclarativeUI supports loading UIs from JSON files for rapid prototyping:

### 1. Create UI Definition (ui/main.json)

```json
{
    "type": "QWidget",
    "properties": {
        "windowTitle": "JSON UI Example",
        "minimumSize": [400, 300]
    },
    "layout": {
        "type": "VBoxLayout",
        "spacing": 15,
        "margins": [20, 20, 20, 20]
    },
    "children": [
        {
            "type": "QLabel",
            "properties": {
                "text": "Welcome to JSON UI!",
                "alignment": 4,
                "styleSheet": "QLabel { font-size: 16px; font-weight: bold; }"
            }
        },
        {
            "type": "QPushButton",
            "properties": {
                "text": "Click Me",
                "minimumHeight": 40
            },
            "events": {
                "clicked": "handleButtonClick"
            }
        }
    ]
}
```

### 2. Load JSON UI

```cpp
#include "JSON/JSONUILoader.hpp"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    auto loader = std::make_unique<JSON::JSONUILoader>();
    
    // Register event handler
    loader->registerEventHandler("handleButtonClick", []() {
        qDebug() << "Button clicked from JSON UI!";
    });
    
    // Load UI from JSON
    auto widget = loader->loadFromFile("ui/main.json");
    if (widget) {
        widget->show();
    }
    
    return app.exec();
}
```

## Hot Reload Development

Enable hot reload for rapid development:

```cpp
#include "HotReload/HotReloadManager.hpp"

class MyApplication : public QObject {
    Q_OBJECT
    
public:
    MyApplication() {
        setupUI();
        setupHotReload();
    }
    
private:
    void setupUI() {
        ui_loader_ = std::make_unique<JSON::JSONUILoader>();
        main_widget_ = ui_loader_->loadFromFile("ui/main.json");
        main_widget_->show();
    }
    
    void setupHotReload() {
        hot_reload_manager_ = std::make_unique<HotReload::HotReloadManager>();
        
        // Register UI file for hot reloading
        hot_reload_manager_->registerUIFile("ui/main.json", main_widget_.get());
        
        // Connect reload signals
        connect(hot_reload_manager_.get(), 
                &HotReload::HotReloadManager::reloadCompleted,
                this, &MyApplication::onUIReloaded);
    }
    
private slots:
    void onUIReloaded(const QString& file_path) {
        qDebug() << "UI reloaded:" << file_path;
        // UI is automatically updated
    }
    
private:
    std::unique_ptr<JSON::JSONUILoader> ui_loader_;
    std::unique_ptr<QWidget> main_widget_;
    std::unique_ptr<HotReload::HotReloadManager> hot_reload_manager_;
};
```

## State Management

Manage application state centrally:

```cpp
#include "Binding/StateManager.hpp"

class CounterApp {
public:
    CounterApp() {
        setupState();
        setupUI();
    }
    
private:
    void setupState() {
        auto& state = StateManager::instance();
        
        // Initialize state
        state.setState("counter", 0);
        state.setState("counterText", QString("Count: 0"));
        
        // Set up computed state
        state.observeState<int>("counter", [&state](const int& value) {
            state.setState("counterText", QString("Count: %1").arg(value));
        });
    }
    
    void setupUI() {
        auto& state = StateManager::instance();
        
        main_widget_ = Core::create<QWidget>()
            .property("windowTitle", "Counter App")
            .layout<QVBoxLayout>()
            .child<Components::Label>([&state](auto &label) {
                label.bindProperty("text", [&state]() {
                    auto text = state.getState<QString>("counterText");
                    return text ? text->get() : QString("Count: 0");
                });
            })
            .child<Components::Button>([&state](auto &button) {
                button.text("Increment")
                      .onClick([&state]() {
                          auto counter = state.getState<int>("counter");
                          if (counter) {
                              counter->set(counter->get() + 1);
                          }
                      });
            })
            .build();
    }
    
private:
    std::unique_ptr<QWidget> main_widget_;
};
```

## Next Steps

Now that you have the basics, explore these advanced topics:

1. **[API Documentation](../api/)** - Detailed API reference
2. **[Examples](../examples/)** - More complex examples
3. **[Developer Guide](../developer/)** - Contributing and advanced topics

### Recommended Learning Path

1. Start with simple declarative UI creation
2. Learn component configuration and event handling
3. Explore JSON-based UI definitions
4. Implement state management and property binding
5. Set up hot reload for development
6. Build more complex applications with advanced components

### Common Patterns

- Use declarative syntax for UI structure
- Leverage JSON for rapid prototyping
- Implement state management early
- Enable hot reload during development
- Follow RAII principles for memory management
- Use property binding for reactive UIs

### Getting Help

- Check the [API documentation](../api/) for detailed reference
- Look at [examples](../examples/) for common patterns
- Review [developer documentation](../developer/) for advanced topics
- Report issues on the project repository

## Build Configuration

### Build Options

The DeclarativeUI build system supports several configuration options:

- `BUILD_EXAMPLES` (ON/OFF) - Build example applications
- `BUILD_TESTS` (ON/OFF) - Build test applications
- `BUILD_SHARED_LIBS` (ON/OFF) - Build shared libraries instead of static
- `CMAKE_BUILD_TYPE` (Debug/Release) - Build configuration

### Example Build Commands

```bash
# Release build with examples and tests
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_EXAMPLES=ON -DBUILD_TESTS=ON

# Debug build for development
cmake .. -DCMAKE_BUILD_TYPE=Debug -DDECLARATIVE_UI_DEBUG=ON

# Minimal build without examples
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_EXAMPLES=OFF -DBUILD_TESTS=OFF
```
