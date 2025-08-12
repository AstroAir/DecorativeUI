# Basic Examples

This section covers the basic examples that demonstrate fundamental DeclarativeUI concepts. These examples are perfect for getting started and understanding the core framework features.

## Overview

The basic examples are located in `examples/basic/` and cover:

- Simple UI creation with declarative syntax
- JSON UI loading and validation
- Basic state management and property binding
- Hot reload functionality
- Event handling patterns

All examples can be built and run after building the project:

```bash
# Build all examples
cmake --build . --target examples

# Run a specific example
./build/examples/basic/HelloWorldExample
```

## Example 01: Hello World

**File**: `examples/basic/01_hello_world.cpp`

The simplest possible DeclarativeUI application:

```cpp
#include <QApplication>
#include "src/Core/UIElement.hpp"
#include "src/Components/Label.hpp"

using namespace DeclarativeUI;

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Create a simple label with declarative syntax
    auto label = std::make_unique<Components::Label>();
    label->text("Hello, DeclarativeUI!")
         .alignment(Qt::AlignCenter)
         .style("font-size: 18px; font-weight: bold; padding: 20px;");

    label->initialize();
    label->getWidget()->show();

    return app.exec();
}
```

**Key Concepts**:

- Basic component creation
- Fluent interface usage
- Component initialization
- Widget retrieval

## Example 02: JSON UI Loading

**File**: `examples/basic/02_json_ui_loading.cpp`

Demonstrates loading UI from JSON definitions:

```cpp
#include <QApplication>
#include "src/JSON/JSONUILoader.hpp"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Load UI from JSON file
    JSON::JSONUILoader loader;
    auto widget = loader.loadFromFile("examples/resources/simple_ui.json");

    if (widget) {
        widget->show();
    } else {
        qCritical() << "Failed to load UI from JSON";
        return 1;
    }

    return app.exec();
}
```

**JSON Definition** (`examples/resources/simple_ui.json`):

```json
{
  "type": "Container",
  "properties": {
    "windowTitle": "JSON UI Example",
    "layout": "VBox",
    "spacing": 10,
    "margins": [20, 20, 20, 20]
  },
  "children": [
    {
      "type": "Label",
      "properties": {
        "text": "This UI was loaded from JSON!",
        "alignment": "AlignCenter",
        "style": "font-size: 16px; font-weight: bold;"
      }
    },
    {
      "type": "Button",
      "properties": {
        "text": "Click Me"
      },
      "events": {
        "clicked": "handleButtonClick"
      }
    }
  ]
}
```

**Key Concepts**:

- JSON UI loading
- Component configuration from JSON
- Event handler registration
- Error handling

## Example 03: Simple State

**File**: `examples/basic/03_simple_state.cpp`

Basic state management and property binding:

```cpp
#include <QApplication>
#include "src/Binding/StateManager.hpp"
#include "src/Components/Label.hpp"
#include "src/Components/Button.hpp"
#include "src/Components/Container.hpp"

using namespace DeclarativeUI;

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Initialize state
    Binding::StateManager::instance().setState("counter", 0);

    // Create UI with state binding
    auto window = Core::create<QWidget>()
        .windowTitle("Simple State Example")
        .layout<QVBoxLayout>([](auto& layout) {
            layout.spacing(10).margins(20);
        })
        .child<Components::Label>([](auto& label) {
            label.bindProperty("text", []() {
                int count = Binding::StateManager::instance().getState("counter", 0);
                return QString("Count: %1").arg(count);
            });
        })
        .child<Components::Button>([](auto& btn) {
            btn.text("Increment")
               .onClick([]() {
                   int current = Binding::StateManager::instance().getState("counter", 0);
                   Binding::StateManager::instance().setState("counter", current + 1);
               });
        })
        .child<Components::Button>([](auto& btn) {
            btn.text("Reset")
               .onClick([]() {
                   Binding::StateManager::instance().setState("counter", 0);
               });
        })
        .build();

    window->show();
    return app.exec();
}
```

**Key Concepts**:

- State initialization
- Property binding to state
- State updates from UI events
- Automatic UI refresh

## Example 04: Basic Hot Reload

**File**: `examples/basic/04_basic_hot_reload.cpp`

Real-time UI updates without application restart:

```cpp
#include <QApplication>
#include "src/HotReload/HotReloadManager.hpp"
#include "src/JSON/JSONUILoader.hpp"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Set up hot reload
    HotReload::HotReloadManager reloadManager;
    JSON::JSONUILoader loader;

    // Load initial UI
    auto widget = loader.loadFromFile("examples/resources/hot_reload_ui.json");
    if (!widget) {
        qCritical() << "Failed to load initial UI";
        return 1;
    }

    // Enable hot reload for the JSON file
    reloadManager.watchFile("examples/resources/hot_reload_ui.json", [&](const QString& filePath) {
        qDebug() << "Reloading UI from:" << filePath;

        auto newWidget = loader.loadFromFile(filePath);
        if (newWidget) {
            // Replace the current widget
            widget->hide();
            widget = std::move(newWidget);
            widget->show();
            qDebug() << "UI reloaded successfully";
        } else {
            qWarning() << "Failed to reload UI, keeping current version";
        }
    });

    widget->show();
    return app.exec();
}
```

**Key Concepts**:

- File watching setup
- Hot reload manager usage
- UI replacement on file changes
- Error handling during reload

## Example 06: Counter App

**File**: `examples/basic/06_counter_app.cpp`

A more complete counter application with multiple features:

```cpp
#include <QApplication>
#include "src/Binding/StateManager.hpp"
#include "src/Components/Label.hpp"
#include "src/Components/Button.hpp"
#include "src/Components/Container.hpp"
#include "src/Components/LineEdit.hpp"

using namespace DeclarativeUI;

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Initialize state
    auto& state = Binding::StateManager::instance();
    state.setState("counter", 0);
    state.setState("step", 1);

    // Create counter application
    auto window = Core::create<QWidget>()
        .windowTitle("Counter Application")
        .resize(300, 200)
        .layout<QVBoxLayout>([](auto& layout) {
            layout.spacing(15).margins(20);
        })
        .child<Components::Label>([](auto& title) {
            title.text("Counter Application")
                 .alignment(Qt::AlignCenter)
                 .style("font-size: 20px; font-weight: bold; margin-bottom: 10px;");
        })
        .child<Components::Label>([](auto& counter) {
            counter.bindProperty("text", []() {
                int count = Binding::StateManager::instance().getState("counter", 0);
                return QString("Current Count: %1").arg(count);
            })
            .alignment(Qt::AlignCenter)
            .style("font-size: 16px; padding: 10px; background: #f0f0f0; border-radius: 5px;");
        })
        .child<Components::Container>([](auto& controls) {
            controls.layout<QHBoxLayout>([](auto& layout) {
                layout.spacing(10);
            })
            .child<Components::Button>([](auto& btn) {
                btn.text("- Decrement")
                   .onClick([]() {
                       auto& state = Binding::StateManager::instance();
                       int current = state.getState("counter", 0);
                       int step = state.getState("step", 1);
                       state.setState("counter", current - step);
                   });
            })
            .child<Components::Button>([](auto& btn) {
                btn.text("+ Increment")
                   .onClick([]() {
                       auto& state = Binding::StateManager::instance();
                       int current = state.getState("counter", 0);
                       int step = state.getState("step", 1);
                       state.setState("counter", current + step);
                   });
            });
        })
        .child<Components::Container>([](auto& stepControl) {
            stepControl.layout<QHBoxLayout>()
            .child<Components::Label>([](auto& label) {
                label.text("Step:");
            })
            .child<Components::LineEdit>([](auto& input) {
                input.text("1")
                     .onTextChanged([](const QString& text) {
                         bool ok;
                         int step = text.toInt(&ok);
                         if (ok && step > 0) {
                             Binding::StateManager::instance().setState("step", step);
                         }
                     });
            });
        })
        .child<Components::Button>([](auto& reset) {
            reset.text("Reset")
                 .onClick([]() {
                     Binding::StateManager::instance().setState("counter", 0);
                 })
                 .style("background: #e74c3c; color: white;");
        })
        .build();

    window->show();
    return app.exec();
}
```

**Key Concepts**:

- Complex UI layout with nested containers
- Multiple state variables
- State-driven UI updates
- Input validation and state updates
- Styling and visual design

## Running the Examples

### Build and Run

```bash
# Build all examples
cmake --build . --target examples

# Run specific examples
./build/examples/basic/HelloWorldExample
./build/examples/basic/JSONUILoadingExample
./build/examples/basic/SimpleStateExample
./build/examples/basic/BasicHotReloadExample
./build/examples/basic/CounterAppExample
```

### Modifying Examples

Try modifying the examples to experiment:

1. **Change styling**: Update CSS styles in the examples
2. **Add components**: Add new UI elements to see how they work
3. **Modify state**: Change state structure and see UI updates
4. **Edit JSON**: Modify JSON files and see hot reload in action

## Next Steps

After exploring the basic examples:

- **[Component Examples](components.md)** - Explore the full component library
- **[Command Examples](command.md)** - Learn the advanced Command System
- **[Advanced Examples](advanced.md)** - See real-world applications
- **[User Guide](../user-guide/getting-started.md)** - Comprehensive tutorials
