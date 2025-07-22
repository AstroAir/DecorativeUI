# DeclarativeUI Framework

A modern, declarative UI framework for Qt applications that provides both traditional component-based and innovative command-based UI development approaches. DeclarativeUI enables rapid development with hot reload capabilities, reactive state management, and seamless integration with existing Qt projects.

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
- **JSON Command Loading**: Dynamic command creation from JSON definitions
- **Legacy Integration**: Comprehensive adapters for gradual migration from existing Qt code

### State Management
- **Centralized State**: Global reactive state store with dependency tracking
- **Reactive Properties**: Automatic UI updates when state changes with computed properties
- **Type Safety**: Template-based type-safe state access with compile-time validation
- **Batch Updates**: Efficient handling of multiple state changes with transaction support
- **State Persistence**: Optional state persistence and restoration capabilities

### Hot Reload System
- **Real-time Updates**: Instant UI updates without application restart
- **File Watching**: Advanced file system monitoring with debouncing and error recovery
- **Performance Monitoring**: Detailed metrics, bottleneck detection, and optimization recommendations
- **Error Recovery**: Robust error handling with fallback mechanisms and user feedback

### JSON Support
- **Dynamic UI Loading**: Load complete UIs from JSON with validation and error reporting
- **Schema Validation**: Comprehensive JSON schema validation with detailed error messages
- **Mixed Mode**: Seamlessly combine JSON-defined and programmatically-created components
- **Component Registry**: Extensible component type registration with custom component support

### Development Tools
- **Debug Dashboard**: Real-time performance monitoring with interactive widgets
- **Memory Profiling**: Memory usage tracking, leak detection, and optimization suggestions
- **Animation Engine**: Smooth animations with easing functions and timing controls
- **Exception Safety**: Comprehensive error handling with detailed diagnostics throughout the framework

## 📁 Project Structure

```text
DeclarativeUI/
├── CMakeLists.txt              # Main build configuration with feature flags
├── CMakePresets.json           # Build presets for different configurations
├── build.bat                   # Windows build script with options
├── build-command.bat           # Command system specific build script
├── src/                        # Core library source code
│   ├── Core/                   # Foundation classes and utilities
│   │   ├── UIElement.hpp/cpp   # Base class for all UI components
│   │   ├── DeclarativeBuilder.hpp/cpp # Template-based fluent builder
│   │   ├── CacheManager.hpp/cpp # Performance optimization and caching
│   │   ├── MemoryManager.hpp/cpp # Memory management utilities
│   │   └── ParallelProcessor.hpp/cpp # Parallel processing support
│   ├── Components/             # UI Components library (30+ components)
│   │   ├── Button.hpp/cpp      # Button components (Button, RadioButton, ToolButton)
│   │   ├── LineEdit.hpp/cpp    # Text input components
│   │   ├── Container.hpp/cpp   # Layout containers and widgets
│   │   ├── Label.hpp/cpp       # Display components
│   │   ├── CheckBox.hpp/cpp    # Checkbox and selection components
│   │   ├── ComboBox.hpp/cpp    # Dropdown and selection widgets
│   │   ├── TableView.hpp/cpp   # Advanced data display components
│   │   ├── TreeView.hpp/cpp    # Hierarchical data components
│   │   ├── FileDialog.hpp/cpp  # Dialog components
│   │   └── [20+ more components] # Complete widget library
│   ├── JSON/                   # JSON support and validation
│   │   ├── JSONUILoader.hpp/cpp # Load UIs from JSON with validation
│   │   ├── JSONParser.hpp/cpp  # Robust JSON parsing utilities
│   │   ├── JSONValidator.hpp/cpp # Schema validation and error reporting
│   │   └── ComponentRegistry.hpp/cpp # Extensible component registration
│   ├── HotReload/              # Hot reload functionality
│   │   ├── FileWatcher.hpp/cpp # Advanced file system monitoring
│   │   ├── HotReloadManager.hpp/cpp # Reload coordination and management
│   │   └── PerformanceMonitor.hpp/cpp # Performance tracking and optimization
│   ├── Binding/                # State management and property binding
│   │   ├── StateManager.hpp/cpp # Centralized reactive state store
│   │   ├── PropertyBinding.hpp/cpp # Property binding system
│   │   └── PropertyBindingTemplate.hpp # Template-based binding utilities
│   ├── Command/                # Command-based UI system (Advanced)
│   │   ├── CommandSystem.hpp/cpp # Core command infrastructure
│   │   ├── UICommand.hpp/cpp   # Base UI command classes
│   │   ├── CommandBuilder.hpp/cpp # Fluent command builders
│   │   ├── WidgetMapper.hpp/cpp # Command-to-widget mapping and sync
│   │   ├── CommandEvents.hpp/cpp # Event system with priorities
│   │   ├── MVCIntegration.hpp/cpp # MVC pattern integration
│   │   ├── BuiltinCommands.hpp/cpp # Standard command implementations
│   │   ├── CoreCommands.hpp/cpp # Core command types
│   │   ├── SpecializedCommands.hpp/cpp # Specialized command implementations
│   │   ├── CommandBinding.hpp/cpp # Command-state binding system
│   │   └── Adapters/           # Integration adapters for legacy systems
│   │       ├── ComponentSystemAdapter.hpp/cpp # Legacy component integration
│   │       ├── JSONCommandLoader.hpp/cpp # JSON command loading
│   │       ├── StateManagerAdapter.hpp/cpp # State management integration
│   │       ├── UIElementAdapter.hpp/cpp # UIElement command conversion
│   │       └── IntegrationManager.hpp/cpp # Centralized integration management
│   ├── Animation/              # Animation engine
│   │   └── AnimationEngine.hpp/cpp # Smooth animations with easing
│   ├── Debug/                  # Debug and profiling tools
│   │   ├── ProfilerDashboard.hpp/cpp # Performance monitoring dashboard
│   │   ├── PerformanceProfilerWidget.hpp/cpp # Performance profiling UI
│   │   ├── BottleneckDetectorWidget.hpp/cpp # Bottleneck detection
│   │   └── DebuggingConsole.hpp/cpp # Debug console interface
│   ├── Exceptions/             # Exception handling
│   │   └── UIExceptions.hpp    # UI-specific exception types
│   └── Tests/                  # Internal test utilities
│       └── [Test utilities and helpers]
├── tests/                      # Comprehensive test suite (24+ test executables)
│   ├── unit/                   # Unit tests (8 test suites)
│   │   ├── test_core.cpp       # Core functionality tests
│   │   ├── test_components.cpp # Component library tests
│   │   ├── test_state_manager.cpp # State management tests
│   │   ├── test_property_binding.cpp # Property binding tests
│   │   ├── test_json.cpp       # JSON support tests
│   │   ├── test_hot_reload.cpp # Hot reload tests
│   │   ├── test_exceptions.cpp # Exception handling tests
│   │   └── test_property_binding_template.cpp # Template binding tests
│   ├── integration/            # Integration tests (1 comprehensive suite)
│   │   └── test_integration.cpp # End-to-end integration tests
│   ├── command/                # Command system tests (15 test suites)
│   │   ├── test_command_system.cpp # Core command system tests
│   │   ├── test_command_builder.cpp # Command builder tests
│   │   ├── test_widget_mapper.cpp # Widget mapping tests
│   │   ├── test_builtin_commands.cpp # Built-in command tests
│   │   ├── test_command_events.cpp # Event system tests
│   │   ├── test_mvc_integration.cpp # MVC integration tests
│   │   ├── test_component_system_adapter.cpp # Legacy adapter tests
│   │   ├── test_json_command_loader.cpp # JSON command tests
│   │   ├── test_state_manager_adapter.cpp # State adapter tests
│   │   ├── test_integration_manager.cpp # Integration manager tests
│   │   └── [5+ more command test suites]
│   └── CMakeLists.txt
├── examples/                   # Example applications and demos (25+ examples)
│   ├── basic/                  # Basic usage examples (10 examples)
│   │   ├── 01_hello_world.cpp  # Simple hello world
│   │   ├── 02_json_ui_loading.cpp # JSON UI loading
│   │   ├── 03_simple_state.cpp # Basic state management
│   │   ├── 04_basic_hot_reload.cpp # Hot reload setup
│   │   ├── 06_counter_app.cpp  # Counter application
│   │   ├── 08_command_basics.cpp # Command system basics
│   │   └── [4+ more basic examples]
│   ├── components/             # Component showcase (5 examples)
│   │   ├── 06_basic_components.cpp # Essential components
│   │   ├── 07_input_components.cpp # Input component showcase
│   │   ├── 08_comprehensive_components.cpp # All components
│   │   ├── 09_enhanced_components.cpp # Advanced features
│   │   └── 10_new_components.cpp # Latest additions
│   ├── command/                # Command system examples (7 examples)
│   │   ├── CommandUIExample.cpp # Command UI construction
│   │   ├── IntegrationExample.cpp # Legacy integration
│   │   ├── command_builder_example.cpp # Builder patterns
│   │   ├── json_command_example.cpp # JSON command loading
│   │   ├── state_integration_example.cpp # State integration
│   │   └── [2+ more command examples]
│   ├── advanced/               # Advanced applications (10 examples)
│   │   ├── 26_command_system.cpp # Comprehensive command demo
│   │   ├── 28_advanced_example.cpp # Advanced features
│   │   ├── 29_comprehensive_example.cpp # Complete showcase
│   │   ├── 36_todo_app.cpp     # Todo application
│   │   ├── 37_form_example.cpp # Form handling
│   │   ├── 38_settings_example.cpp # Settings management
│   │   ├── 39_dashboard_example.cpp # Dashboard application
│   │   └── [3+ more advanced examples]
│   ├── resources/              # Example JSON UI definitions
│   │   ├── command_ui_example.json # Command system UI
│   │   └── mixed_components_example.json # Mixed component types
│   └── CMakeLists.txt
├── docs/                       # Comprehensive documentation
│   ├── api/                    # API reference documentation
│   │   ├── core.md            # Core API reference
│   │   ├── components.md      # Components API reference
│   │   ├── command.md         # Command system API
│   │   ├── binding.md         # State management API
│   │   ├── json.md            # JSON support API
│   │   └── hot-reload.md      # Hot reload API
│   ├── user-guide/            # User guides and tutorials
│   │   ├── getting-started.md # Getting started guide
│   │   ├── best-practices.md  # Best practices
│   │   └── examples.md        # Example walkthroughs
│   ├── developer/             # Developer documentation
│   │   ├── architecture.md    # System architecture
│   │   ├── contributing.md    # Contributing guidelines
│   │   └── testing.md         # Testing practices
│   ├── integration-guide.md   # Integration with existing projects
│   ├── command-ui-architecture.md # Command system architecture
│   └── overview.md            # Project overview
├── Resources/                 # UI definitions and resources
│   └── ui_definitions/        # JSON UI definition files
└── build/                     # Build output directory
    ├── examples/              # Built example applications
    ├── tests/                 # Built test applications
    └── Resources/             # Copied resources
```

## ⚙️ Build Configuration

The build system supports flexible configuration through CMake options and presets:

### Core Build Options

| Option | Default | Description |
|--------|---------|-------------|
| `BUILD_EXAMPLES` | ON | Build example applications (25+ examples across 4 categories) |
| `BUILD_TESTS` | ON | Build comprehensive test suite (24+ test executables) |
| `BUILD_SHARED_LIBS` | OFF | Build shared libraries instead of static |
| `CMAKE_BUILD_TYPE` | Release | Build configuration (Debug/Release/RelWithDebInfo) |

### Command System Options

| Option | Default | Description |
|--------|---------|-------------|
| `BUILD_COMMAND_SYSTEM` | ON | Enable modern Command-based UI system with all features |
| `BUILD_ADAPTERS` | ON | Enable integration adapters for legacy code migration |
| `ENABLE_COMMAND_DEBUG` | OFF | Enable detailed Command system debug output and diagnostics |

### Available Build Presets

The project includes CMakePresets.json with optimized configurations:

| Preset | Configuration | Features | Use Case |
|--------|---------------|----------|----------|
| `default` | Release | All features enabled | Production builds and deployment |
| `debug` | Debug | All features + debug output | Development and debugging |
| `release` | Release | Optimized, all features | Production deployment |
| `minimal` | Release | Library only, no examples/tests | Integration into other projects |
| `command-dev` | Debug | Command system focus + debug | Command system development |
| `legacy-only` | Release | Components only, no Commands | Legacy Qt integration |

### Feature Flags

When building, the following preprocessor definitions are automatically set:

- `DECLARATIVE_UI_COMMAND_SYSTEM_ENABLED` - Command system and all related features available
- `DECLARATIVE_UI_ADAPTERS_ENABLED` - Integration adapters for legacy systems available
- `DECLARATIVE_UI_COMMAND_DEBUG` - Command debug output and diagnostics enabled
- `DECLARATIVE_UI_DEBUG` - General debug features enabled (automatically set in Debug builds)

## 🚀 Quick Start

### Prerequisites

- **Qt6** (6.2 or later) with Core, Widgets, Network, and Test modules
- **CMake** 3.20 or higher
- **C++20 compatible compiler** (GCC 10+, Clang 12+, MSVC 2019+)

### Windows Build

Use the provided build scripts for easy setup:

```bat
# Release build with all features (recommended)
build.bat

# Debug build for development
build.bat --debug

# Command system development build
build-command.bat

# Clean build with verbose output
build.bat --clean --verbose

# Build without examples (faster)
build.bat --no-examples
```

### Cross-Platform CMake Build

```bash
# 1. Create and enter build directory
mkdir build && cd build

# 2. Configure with preset (recommended)
cmake --preset=default ..

# 3. Build the project
cmake --build . --config Release

# 4. Run tests to verify installation
ctest --config Release --output-on-failure

# 5. Run an example
./examples/basic/HelloWorldExample  # Linux/macOS
# or examples\basic\HelloWorldExample.exe  # Windows
```

### Alternative Manual Configuration

```bash
# Configure with custom options
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_EXAMPLES=ON \
  -DBUILD_TESTS=ON \
  -DBUILD_COMMAND_SYSTEM=ON \
  -DBUILD_ADAPTERS=ON

# Build specific targets
cmake --build . --target DeclarativeUI        # Main library only
cmake --build . --target examples             # All examples
cmake --build . --target tests                # All tests
```

### Verify Installation

```bash
# Run the comprehensive example
./examples/advanced/ComprehensiveExample

# Check available examples
ls examples/*/  # Linux/macOS
dir examples\*\ # Windows

# Run specific test suites
./tests/unit/CoreTest
./tests/command/CommandSystemTest
```

## 📚 Library Architecture

### DeclarativeUI (Main Library)

The core library provides the foundation for declarative UI development:

#### Core Module
- **UIElement** - Base class with property management, event handling, and lifecycle
- **DeclarativeBuilder** - Template-based fluent builder for type-safe widget creation
- **CacheManager** - Performance optimization and memory management
- **MemoryManager** - RAII-based resource management

#### JSON Module
- **JSONUILoader** - Load complete UIs from JSON definitions
- **JSONParser** - Robust JSON parsing with error reporting
- **JSONValidator** - Schema validation and type checking
- **ComponentRegistry** - Extensible component type registration

#### HotReload Module
- **FileWatcher** - Advanced file system monitoring with debouncing
- **HotReloadManager** - Coordinated reload operations with error recovery
- **PerformanceMonitor** - Real-time performance tracking and optimization

#### Binding Module
- **StateManager** - Centralized reactive state management
- **PropertyBinding** - Automatic property synchronization
- **ReactiveProperty** - Type-safe reactive properties with computed values

#### Command Module (Advanced)
- **CommandSystem** - Core command infrastructure with async support
- **UICommand** - Base classes for UI commands
- **CommandBuilder** - Fluent API for declarative command construction
- **WidgetMapper** - Automatic Command-to-QWidget translation
- **CommandEvents** - Abstract event system with priorities and filtering
- **MVCIntegration** - MVC pattern integration bridge

### Components Library

Rich collection of 30+ pre-built UI components covering all common use cases:

#### Input Components
- **Button, RadioButton, ToolButton** - Various button types with styling and event handling
- **LineEdit** - Single-line text input with validation and formatting
- **CheckBox** - Checkbox with tri-state support and custom styling
- **ComboBox** - Dropdown selection with custom models and validation
- **SpinBox, DoubleSpinBox** - Numeric input with range validation and formatting
- **Slider** - Value selection with customizable appearance and precision
- **Dial** - Rotary input control for value selection

#### Display Components
- **Label** - Text and image display with rich formatting and alignment
- **ProgressBar** - Progress indication with custom styling and animation
- **TextEdit, PlainTextEdit** - Multi-line text editing with syntax highlighting support
- **LCDNumber** - Digital display component with customizable segments

#### Container Components
- **Container** - Generic container with advanced layout management
- **Widget** - Base widget container with property binding
- **Layout** - Advanced layout management (VBox, HBox, Grid, Form)
- **GroupBox** - Grouped controls with borders, titles, and collapsible sections
- **TabWidget** - Tabbed interface with dynamic tab management
- **Frame** - Decorative frames and separators with styling
- **ScrollArea** - Scrollable content areas with custom scrollbars
- **Splitter** - Resizable split layouts with persistence

#### Advanced Components
- **TableView** - Tabular data display with sorting, filtering, and editing
- **TreeView** - Hierarchical data display with custom models
- **ListView** - List display with custom delegates and selection
- **MenuBar** - Application menu bars with dynamic menu creation
- **ToolBar** - Customizable toolbars with action management
- **StatusBar** - Status display with multiple sections and indicators
- **DateTimeEdit** - Date and time input with calendar popup and validation
- **Calendar** - Calendar widget for date selection

#### Dialog Components
- **FileDialog** - File selection dialogs with filters and preview
- **ColorDialog** - Color selection with custom palettes
- **FontDialog** - Font selection with preview and styling
- **MessageBox** - Message dialogs with custom buttons and icons

#### Specialized Components
- **DockWidget** - Dockable widgets for flexible layouts

## 🎯 Command System (Advanced)

The Command-based UI architecture represents the next evolution in Qt UI development, providing a modern alternative to traditional widget-based approaches.

### Core Philosophy

The Command system separates UI logic from widget implementation, enabling:
- **Framework Independence**: Commands work without direct Qt dependencies
- **Testability**: UI logic can be unit tested without widgets
- **Flexibility**: Easy switching between different UI backends
- **Maintainability**: Clear separation of concerns

### Key Features

#### Declarative UI Construction
```cpp
// Create complex UIs with fluent syntax
auto dashboard = CommandHierarchyBuilder("Container")
    .layout("VBox")
    .spacing(15)
    .margins(20)
    .addChild("Label", [](CommandBuilder& header) {
        header.text("📊 Application Dashboard")
              .style("font-size: 24px; font-weight: bold;")
              .alignment(Qt::AlignCenter);
    })
    .addChild("Container", [](CommandBuilder& stats) {
        stats.layout("HBox")
             .spacing(20)
             .child(CommandBuilder("Label")
                   .text("👥 Users: 1,234")
                   .style("background: #3498db; color: white; padding: 15px;"))
             .child(CommandBuilder("Label")
                   .text("💰 Revenue: $12,345")
                   .style("background: #2ecc71; color: white; padding: 15px;"));
    })
    .build();
```

#### State Management Integration
```cpp
// Automatic state synchronization
auto input = CommandBuilder("TextInput")
    .placeholder("Enter your name")
    .bindToState("user.name", "text")  // Two-way binding
    .build();

// State changes automatically update UI
StateManager::instance().setState("user.name", "John Doe");
// Input field now displays "John Doe"
```

#### Event System
```cpp
// Type-safe event handling with priorities
auto button = CommandBuilder("Button")
    .text("Save Document")
    .addEventListener("click", [](const CommandEvent& event) {
        // Access event data
        auto modifiers = event.getData("modifiers").value<Qt::KeyboardModifiers>();
        if (modifiers & Qt::ControlModifier) {
            // Save as...
        } else {
            // Regular save
        }
    })
    .build();
```

#### JSON Loading
```cpp
// Load complete UIs from JSON
JSONCommandLoader loader;
loader.registerEventHandler("saveDocument", [](const CommandEvent& event) {
    qDebug() << "Saving document...";
});

auto [command, widget] = loader.loadCommandWithWidgetFromFile("ui/editor.json");
widget->show();
```

### Legacy Integration

#### Gradual Migration
```cpp
// Convert existing Components to Commands
auto legacyButton = std::make_unique<Components::Button>();
legacyButton->text("Legacy Button");

auto commandButton = ComponentSystemAdapter::instance()
    .convertToCommand(legacyButton.get());

// Establish bidirectional synchronization
ComponentSystemAdapter::instance()
    .establishSync(legacyButton.get(), commandButton);
```

#### Hybrid Containers
```cpp
// Mix Components and Commands in the same container
auto hybrid = ComponentSystemAdapter::instance().createHybridContainer();
hybrid->setLayout("VBox");
hybrid->addComponent(std::move(legacyButton));
hybrid->addCommand(CommandBuilder("Button").text("Command Button").build());

auto widget = hybrid->toWidget();
```

### Advanced Features

#### Custom Command Types
```cpp
// Register custom command mappings
WidgetMapper::instance().registerMapping<QSlider>("CustomSlider");

// Use in builders
auto slider = CommandBuilder("CustomSlider")
    .property("minimum", 0)
    .property("maximum", 100)
    .property("value", 50)
    .build();
```

#### MVC Integration
```cpp
// Register commands as MVC actions
MVCIntegrationBridge::instance().registerCommandAsAction(
    saveButton, "document.save"
);

// Execute through MVC system
MVCIntegrationBridge::instance().executeCommandAction("document.save");
```

## 📖 Examples & Demos

The project includes 25+ comprehensive examples demonstrating all framework features. All examples are built to `build/examples/` and organized by complexity:

### Basic Examples (`examples/basic/`) - 10 Examples

Perfect for getting started with DeclarativeUI fundamentals:

| Example | Description | Key Features |
|---------|-------------|--------------|
| **01_hello_world** | Simple "Hello World" application | Basic widget creation, declarative syntax |
| **02_json_ui_loading** | Loading UI from JSON files | JSON parsing, dynamic UI creation |
| **03_simple_state** | Basic state management | StateManager usage, property binding |
| **04_basic_hot_reload** | Hot reload functionality | FileWatcher, real-time updates |
| **05_event_handling** | Event handling patterns | Event registration, lambda handlers |
| **06_counter_app** | Interactive counter application | State updates, reactive UI |
| **08_command_basics** | Command system introduction | CommandBuilder, basic commands |
| **16_reactive_state** | Reactive state management | Computed properties, dependencies |
| **21_file_watching** | File system monitoring | FileWatcher configuration, callbacks |
| **22_hot_reload_example** | Advanced hot reload features | Performance monitoring, error handling |

### Component Examples (`examples/components/`) - 5 Examples

Showcase the rich component library:

| Example | Description | Components Demonstrated |
|---------|-------------|------------------------|
| **06_basic_components** | Essential UI components | Button, Label, LineEdit, Container |
| **07_input_components** | Input component showcase | CheckBox, ComboBox, SpinBox, Slider |
| **08_comprehensive_components** | All 30+ components | Complete component library tour |
| **09_enhanced_components** | Advanced component features | Styling, validation, custom properties |
| **10_new_components** | Latest component additions | TableView, TreeView, DateTimeEdit, Dialogs |

### Command System Examples (`examples/command/`) - 7 Examples

Explore the advanced Command-based architecture:

| Example | Description | Command Features |
|---------|-------------|------------------|
| **CommandUIExample** | Command-based UI construction | CommandBuilder, WidgetMapper |
| **IntegrationExample** | Legacy-Command integration | ComponentSystemAdapter, hybrid containers |
| **command_builder_example** | Command Builder patterns | Fluent API, method chaining |
| **json_command_example** | JSON Command loading | JSONCommandLoader, dynamic commands |
| **state_integration_example** | State management with Commands | Command-state binding, reactive updates |
| **main** | Complete Command system demo | Full Command workflow |
| **integration_main** | Integration scenarios | Migration strategies, compatibility |

### Advanced Examples (`examples/advanced/`) - 10 Examples

Real-world applications demonstrating complete workflows:

| Example | Description | Advanced Features |
|---------|-------------|-------------------|
| **26_command_system** | Comprehensive Command demo | All Command features integrated |
| **27_command_example** | Command system patterns | Advanced command usage |
| **28_advanced_example** | Advanced DeclarativeUI features | Performance optimization, memory management |
| **29_comprehensive_example** | Complete feature showcase | All framework capabilities |
| **30_command_ui_builder** | Advanced Command Builder patterns | Complex hierarchies, custom builders |
| **31_integration_example** | Complex integration scenarios | Multi-system integration, adapters |
| **36_todo_app** | Complete todo application | CRUD operations, persistence, validation |
| **37_form_example** | Form handling and validation | Input validation, error handling, submission |
| **38_settings_example** | Settings management application | Configuration persistence, UI preferences |
| **39_dashboard_example** | Interactive dashboard | Data visualization, real-time updates |

### Running Examples

```bash
# Run a specific example
./build/examples/basic/HelloWorldExample

# Run all basic examples
for example in build/examples/basic/*; do $example; done

# Run with different configurations
./build/examples/advanced/ComprehensiveExample --theme=dark
./build/examples/command/CommandUIExample --debug
```

### Example Resources

Examples include JSON UI definitions in `examples/resources/`:
- `command_ui_example.json` - Command system UI definition
- `mixed_components_example.json` - Mixed Components/Commands
- Additional JSON files for various examples

## 🧪 Testing Framework

Comprehensive test suite with 24+ test executables covering all framework components. Tests are built to `build/tests/` and organized by scope:

### Unit Tests (`tests/unit/`) - 8 Test Suites

Core component testing with isolated test cases:

| Test Suite | Coverage | Key Test Areas |
|------------|----------|----------------|
| **test_core** | Core framework | UIElement lifecycle, DeclarativeBuilder, property system |
| **test_components** | Component library | All 30+ components, widget creation, property binding |
| **test_property_binding** | Binding system | Property synchronization, reactive updates |
| **test_state_manager** | State management | State operations, computed properties, batch updates |
| **test_property_binding_template** | Template binding | Template-based property binding patterns |
| **test_json** | JSON support | Parsing, validation, UI loading, error handling |
| **test_hot_reload** | Hot reload | File watching, reload operations, performance |
| **test_exceptions** | Exception handling | Error conditions, exception safety, recovery |

### Integration Tests (`tests/integration/`) - 1 Test Suite

Cross-component integration testing:

| Test Suite | Coverage | Integration Areas |
|------------|----------|-------------------|
| **test_integration** | Full system | Component-JSON-HotReload-State integration workflows |

### Command System Tests (`tests/command/`) - 15 Test Suites

Comprehensive Command system testing:

| Test Suite | Coverage | Command Features |
|------------|----------|------------------|
| **test_command_system** | Core Command infrastructure | Command execution, lifecycle, error handling |
| **test_builtin_commands** | Built-in commands | SetProperty, UpdateState, File operations |
| **test_command_integration** | Command integration | Cross-system command integration |
| **test_command_binding** | Command binding | State binding, property synchronization |
| **test_command_builder** | Command builders | CommandBuilder, CommandHierarchyBuilder |
| **test_command_core** | Command core functionality | BaseUICommand, CommandState, metadata |
| **test_command_events** | Command event system | Event dispatching, filtering, priorities |
| **test_command_state** | Command state management | State operations, validation, batch updates |
| **test_widget_mapper** | Widget mapping | Command-to-widget translation, synchronization |
| **test_component_system_adapter** | Legacy integration | Component-Command conversion, sync |
| **test_integration_manager** | Integration management | Adapter coordination, lifecycle |
| **test_json_command_loader** | JSON Command loading | JSON-to-Command conversion, validation |
| **test_mvc_integration** | MVC integration | MVC pattern integration, action registration |
| **test_state_manager_adapter** | State adapter | StateManager-Command integration |
| **test_uielement_adapter** | UIElement adapter | UIElement-Command conversion |

### Running Tests

```bash
# Run all tests
ctest --config Release --output-on-failure

# Run specific test category
ctest -R "unit" --output-on-failure
ctest -R "command" --output-on-failure
ctest -R "integration" --output-on-failure

# Run individual test suite
./build/tests/unit/test_core
./build/tests/unit/test_components
./build/tests/command/test_command_system
./build/tests/command/test_widget_mapper
./build/tests/integration/test_integration

# Run with verbose output
./build/tests/unit/test_core --verbose
./build/tests/command/test_command_system --verbose

# Run performance tests
./build/tests/integration/test_integration --benchmark
```

### Test Coverage

The test suite provides comprehensive coverage:
- **Unit Tests**: 95%+ code coverage for individual components
- **Integration Tests**: End-to-end workflow validation
- **Performance Tests**: Benchmarking and regression testing
- **Error Handling**: Exception safety and error recovery
- **Memory Safety**: RAII compliance and leak detection

## 📋 Dependencies

### Required Dependencies
- **Qt6** (6.2 or later) - Core, Widgets, Network, Test modules
- **CMake** 3.20 or higher
- **C++20 compatible compiler**:
  - GCC 10+ (Linux)
  - Clang 12+ (macOS/Linux)
  - MSVC 2019+ (Windows)

### Optional Dependencies
- **Doxygen** - For generating API documentation
- **Valgrind** - For memory leak detection (Linux)
- **Qt Creator** - Recommended IDE with CMake support

### Platform Support
- ✅ **Windows** 10/11 (MSVC 2019+, MinGW)
- ✅ **Linux** (Ubuntu 20.04+, Fedora 34+, Arch Linux)
- ✅ **macOS** 11+ (Intel and Apple Silicon)

## 🛠️ Development Guide

### Project Structure for Contributors

```text
src/
├── Core/           # Foundation classes - start here for core changes
├── Components/     # UI components - add new widgets here
├── JSON/          # JSON support - extend for new JSON features
├── HotReload/     # Hot reload system - performance optimizations
├── Binding/       # State management - reactive system enhancements
└── Command/       # Command system - advanced UI architecture
    ├── Adapters/  # Integration adapters - legacy compatibility
    └── Tests/     # Command-specific tests
```

### Adding New Components

1. **Create component files**:
   ```bash
   # Create header and implementation
   touch src/Components/MyComponent.hpp
   touch src/Components/MyComponent.cpp
   ```

2. **Implement component class**:
   ```cpp
   // MyComponent.hpp
   class MyComponent : public Core::UIElement {
       Q_OBJECT
   public:
       explicit MyComponent(QObject* parent = nullptr);

       // Fluent interface methods
       MyComponent& customProperty(const QString& value);

       void initialize() override;
   };
   ```

3. **Register in build system**:
   ```cmake
   # Add to src/Components/CMakeLists.txt
   MyComponent.hpp
   MyComponent.cpp
   ```

4. **Add to component registry** (for JSON support):
   ```cpp
   // In ComponentRegistry.cpp
   registry["MyComponent"] = []() { return std::make_unique<MyComponent>(); };
   ```

### Adding New Examples

1. **Choose appropriate category**:
   - `examples/basic/` - Simple demonstrations (< 100 lines)
   - `examples/components/` - Component showcases
   - `examples/command/` - Command system features
   - `examples/advanced/` - Real-world applications (> 200 lines)

2. **Create example file**:
   ```cpp
   // examples/basic/my_example.cpp
   #include <QApplication>
   #include "Components/MyComponent.hpp"

   int main(int argc, char* argv[]) {
       QApplication app(argc, argv);

       // Example implementation

       return app.exec();
   }
   ```

3. **Register in build system**:
   ```cmake
   # Add to examples/basic/CMakeLists.txt
   add_executable(MyExample my_example.cpp)
   target_link_libraries(MyExample DeclarativeUI Qt6::Widgets)
   ```

### Adding New Tests

1. **Create test file**:
   ```cpp
   // tests/unit/MyComponentTest.cpp
   #include <QtTest/QtTest>
   #include "Components/MyComponent.hpp"

   class MyComponentTest : public QObject {
       Q_OBJECT
   private slots:
       void testBasicFunctionality();
       void testPropertyBinding();
   };
   ```

2. **Register test**:
   ```cmake
   # Add to tests/unit/CMakeLists.txt
   add_executable(MyComponentTest MyComponentTest.cpp)
   target_link_libraries(MyComponentTest DeclarativeUI Qt6::Test)
   add_test(NAME MyComponentTest COMMAND MyComponentTest)
   ```

### Code Style Guidelines

- **Modern C++20**: Use auto, range-based loops, smart pointers
- **RAII**: All resources managed automatically
- **Const correctness**: Mark methods const when possible
- **Exception safety**: Use RAII and proper exception handling
- **Documentation**: Document public APIs with Doxygen comments

### Performance Considerations

- **Property binding**: Minimize unnecessary updates
- **Memory management**: Use smart pointers, avoid raw pointers
- **Hot reload**: Optimize file watching and reload operations
- **Command system**: Batch operations when possible

## 🔧 Troubleshooting

### Common Build Issues

| Issue | Symptoms | Solution |
|-------|----------|----------|
| **Qt6 not found** | CMake error: "Could not find Qt6" | Install Qt6 and add to PATH, or set `CMAKE_PREFIX_PATH` |
| **C++20 not supported** | Compiler errors about C++20 features | Update compiler: GCC 10+, Clang 12+, MSVC 2019+ |
| **Missing dependencies** | Link errors for Qt modules | Install Qt6 development packages |
| **Resource loading fails** | Runtime errors loading JSON/resources | Verify Resources directory copied to build output |
| **Hot reload not working** | File changes not detected | Check file permissions and path separators |

### Debug Configuration

Enable comprehensive debugging:

```bash
# Full debug build with all diagnostics
cmake .. \
  -DCMAKE_BUILD_TYPE=Debug \
  -DDECLARATIVE_UI_DEBUG=ON \
  -DENABLE_COMMAND_DEBUG=ON

# Build and run with debug output
cmake --build . --config Debug
./examples/basic/HelloWorldExample --debug
```

### Performance Issues

| Problem | Diagnosis | Solution |
|---------|-----------|----------|
| **Slow hot reload** | Check PerformanceMonitor output | Reduce file watch scope, optimize JSON |
| **Memory leaks** | Use Valgrind or similar tools | Check smart pointer usage, RAII compliance |
| **UI lag** | Profile with built-in tools | Optimize property bindings, batch updates |
| **Large memory usage** | Monitor with PerformanceMonitor | Enable memory optimization, clear caches |

### Getting Help

- **Documentation**: Check `docs/` directory for comprehensive guides
- **Examples**: Review `examples/` for usage patterns
- **Tests**: Run tests to verify installation: `ctest --output-on-failure`
- **Debug Output**: Enable debug flags for detailed diagnostics

## 📚 Documentation

Comprehensive documentation is available in the `docs/` directory:

- **[API Reference](docs/api/)** - Complete API documentation for all modules
- **[User Guide](docs/user-guide/)** - Step-by-step tutorials and best practices
- **[Developer Guide](docs/developer/)** - Architecture, contributing, and testing
- **[Integration Guide](docs/integration-guide.md)** - Integrating with existing projects
- **[Examples Guide](docs/examples/)** - Detailed example walkthroughs

## 🤝 Contributing

We welcome contributions! Please see:
- **[Contributing Guide](docs/developer/contributing.md)** - How to contribute
- **[Architecture Overview](docs/developer/architecture.md)** - System design
- **[Testing Guide](docs/developer/testing.md)** - Testing practices

## 📄 License

This project is licensed under the MIT License - see the LICENSE file for details.

## 🎯 Why DeclarativeUI?

DeclarativeUI bridges the gap between traditional Qt development and modern UI frameworks:

- **Familiar**: Built on Qt6, works with existing Qt knowledge
- **Modern**: C++20 features, reactive patterns, declarative syntax
- **Flexible**: Choose between Components or Commands based on your needs
- **Productive**: Hot reload, state management, and comprehensive tooling
- **Maintainable**: Clear separation of concerns, testable architecture

Whether you're building a simple desktop application or a complex enterprise system, DeclarativeUI provides the tools and patterns to create maintainable, performant Qt applications with modern development workflows.
