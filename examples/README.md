# DeclarativeUI Examples

This directory contains all examples for the DeclarativeUI project, organized by complexity and feature focus.

## Directory Structure

```text
examples/
├── CMakeLists.txt          # Main examples configuration
├── basic/                  # Basic examples and tutorials
│   ├── CMakeLists.txt
│   ├── 01_hello_world.cpp
│   ├── 02_json_ui_loading.cpp
│   ├── 03_simple_state.cpp
│   ├── 04_basic_hot_reload.cpp
│   ├── 05_event_handling.cpp
│   ├── 06_counter_app.cpp
│   ├── 08_command_basics.cpp
│   ├── 08_command_basics_simple.cpp
│   ├── 16_reactive_state.cpp
│   ├── 21_file_watching.cpp
│   └── 22_hot_reload_example.cpp
├── components/             # Component demonstrations
│   ├── CMakeLists.txt
│   ├── 06_basic_components.cpp
│   ├── 07_input_components.cpp
│   ├── 08_comprehensive_components.cpp
│   ├── 09_enhanced_components.cpp
│   └── 10_new_components.cpp
├── command/                # Command system examples
│   ├── CMakeLists.txt
│   ├── CommandUIExample.cpp
│   ├── CommandUIExample.hpp
│   ├── IntegrationExample.cpp
│   ├── IntegrationExample.hpp
│   ├── command_builder_example.cpp
│   ├── integration_main.cpp
│   ├── json_command_example.cpp
│   ├── main.cpp
│   └── state_integration_example.cpp
├── advanced/               # Advanced applications
│   ├── CMakeLists.txt
│   ├── 26_command_system.cpp
│   ├── 27_command_example.cpp
│   ├── 28_advanced_example.cpp
│   ├── 29_comprehensive_example.cpp
│   ├── 30_command_ui_builder.cpp
│   ├── 31_integration_example.cpp
│   ├── 36_todo_app.cpp
│   ├── 37_form_example.cpp
│   ├── 38_settings_example.cpp
│   └── 39_dashboard_example.cpp
└── resources/              # Shared example resources
    ├── command_ui_example.json
    └── mixed_components_example.json
```

## Running Examples

### Build All Examples

```bash
cd build
cmake .. -DBUILD_EXAMPLES=ON
cmake --build . --config Release
```

### Run Specific Examples

```bash
# Basic examples
./examples/basic/HelloWorldExample
./examples/basic/CounterAppExample

# Component examples
./examples/components/BasicComponentsExample
./examples/components/InputComponentsExample

# Command system examples
./examples/command/CommandUIExample
./examples/command/IntegrationExample

# Advanced examples
./examples/advanced/TodoAppExample
./examples/advanced/DashboardExample
```

## Example Categories

### Basic Examples (`basic/`)

Perfect for learning the fundamentals:

- **HelloWorldExample**: Simple "Hello World" application
- **JSONUILoadingExample**: Loading UI from JSON files
- **SimpleStateExample**: Basic state management
- **BasicHotReloadExample**: Hot reload functionality
- **EventHandlingExample**: Event handling patterns
- **CounterAppExample**: Simple counter application
- **CommandBasicsExample**: Command system introduction
- **CommandBasicsSimpleExample**: Simplified command basics
- **ReactiveStateExample**: Reactive state management
- **FileWatchingExample**: File watching functionality
- **HotReloadExample**: Advanced hot reload features

### Component Examples (`components/`)

Showcasing UI components:

- **BasicComponentsExample**: Basic UI components showcase
- **InputComponentsExample**: Input component demonstrations
- **ComprehensiveComponentsExample**: All components showcase
- **EnhancedComponentsExample**: Enhanced component features
- **NewComponentsExample**: Latest component additions

### Command System Examples (`command/`)

Demonstrating the command architecture:

Note: Several command examples are currently disabled in CMake due to API mismatches. Sources are available for reference and will be re-enabled after alignment.

- **CommandUIExample**: Command-based UI construction (disabled)
- **IntegrationExample**: Legacy-Command integration (disabled)
- **CommandBuilderExample**: Command Builder patterns (disabled)
- **JSONCommandExample**: Loading UI from JSON with Commands (disabled)
- **StateIntegrationExample**: State management with Commands (disabled)
- **CommandMainExample**: Main Command system demo (disabled)
- **IntegrationMainExample**: Integration scenarios (disabled)

### Advanced Examples (`advanced/`)

Complex real-world applications:

- **CommandSystemExample**: Comprehensive Command system demo
- **CommandExample**: Advanced Command usage patterns
- **AdvancedExample**: Advanced DeclarativeUI features
- **ComprehensiveExample**: Complete feature showcase
- **CommandUIBuilderExample**: Advanced Command Builder patterns
- **IntegrationExampleAdvanced**: Complex integration scenarios
- **TodoAppExample**: Complete todo application
- **FormExample**: Form handling and validation
- **SettingsExample**: Settings management
- **DashboardExample**: Dashboard application

## Learning Path

### Beginner

1. Start with `basic/HelloWorldExample`
2. Try `basic/SimpleStateExample`
3. Explore `basic/EventHandlingExample`
4. Build `basic/CounterAppExample`

### Intermediate

1. Study `components/BasicComponentsExample`
2. Try `basic/JSONUILoadingExample`
3. Explore `basic/HotReloadExample`
4. Build `components/ComprehensiveComponentsExample`

### Advanced

1. Learn `command/CommandUIExample`
2. Study `command/IntegrationExample`
3. Try `advanced/TodoAppExample`
4. Build `advanced/DashboardExample`

## Key Concepts Demonstrated

### State Management

- Property binding
- Reactive updates
- State synchronization
- Event-driven updates

### UI Construction

- Declarative UI building
- Component composition
- Layout management
- Dynamic UI creation

### Command System

- Command-based architecture
- Builder patterns
- Integration with legacy components
- JSON-driven UI construction

### Hot Reload

- File watching
- Dynamic reloading
- Performance monitoring
- Development workflow

## Adding New Examples

### Basic Example

1. Create `.cpp` file in `basic/` directory
2. Add executable to `basic/CMakeLists.txt`
3. Focus on single concept demonstration
4. Include clear comments and documentation

### Component Example

1. Create `.cpp` file in `components/` directory
2. Add executable to `components/CMakeLists.txt`
3. Showcase specific UI components
4. Demonstrate component features and usage

### Command Example

1. Create `.cpp` file in `command/` directory
2. Add executable to `command/CMakeLists.txt`
3. Demonstrate command system features
4. Show integration patterns

### Advanced Example

1. Create `.cpp` file in `advanced/` directory
2. Add executable to `advanced/CMakeLists.txt`
3. Build complete applications
4. Combine multiple features

## Example Template

```cpp
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

// Include DeclarativeUI headers
#include "src/Core/DeclarativeBuilder.hpp"
#include "src/Components/Button.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Your example code here
    auto widget = std::make_unique<QWidget>();
    auto layout = std::make_unique<QVBoxLayout>(widget.get());

    auto label = std::make_unique<QLabel>("Hello, DeclarativeUI!");
    layout->addWidget(label.get());

    widget->show();

    return app.exec();
}
```

## Dependencies

Examples depend on:

- DeclarativeUI library
- Components library
- Qt6::Core
- Qt6::Widgets
- Qt6::Network (for some examples)

## Resources

Examples may use resources from:

- `Resources/` (main project resources)
- `examples/resources/` (example-specific resources)

Resources are automatically copied to the build directory during build.
