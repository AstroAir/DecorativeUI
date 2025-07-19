# DeclarativeUI Examples

This directory contains a comprehensive collection of examples demonstrating all aspects of the DeclarativeUI framework. The examples are organized by complexity and functionality to provide a structured learning path.

## 📁 Directory Structure

### 01_basic/ - Fundamental Concepts (Examples 01-05)
**Learning Path: Start Here**

- **01_hello_world** - Minimal DeclarativeUI application
- **02_json_ui_loading** - Basic JSON UI loading without hot reload
- **03_simple_state** - Basic state management with reactive updates
- **04_basic_hot_reload** - Simple hot reload setup
- **05_event_handling** - Basic event handling and user interactions

### 02_components/ - UI Components (Examples 06-15)
**Learning Path: After mastering basics**

- **06_basic_components** ✅ - Button, Label, LineEdit showcase
- **07_input_components** ✅ - CheckBox, ComboBox, SpinBox, Slider
- **08_display_components** 🚧 - ProgressBar, TextEdit, advanced labels
- **09_layout_components** 🚧 - Container, Layout, GroupBox
- **10_advanced_components** 🚧 - TableView, TreeView, TabWidget
- **11_new_components** 🚧 - RadioButton, Dial, LCDNumber, Calendar
- **12_dialog_components** 🚧 - MessageBox, FileDialog, ColorDialog
- **13_container_components** 🚧 - Splitter, ScrollArea, DockWidget
- **14_menu_components** 🚧 - MenuBar, ToolBar, StatusBar
- **15_custom_components** 🚧 - Creating and registering custom components

### 03_state_management/ - State Management (Examples 16-20)
**Learning Path: For reactive applications**

- **16_reactive_state** ✅ - Advanced reactive state patterns
- **17_computed_state** 🚧 - Computed properties and derived state
- **18_state_validation** 🚧 - State validation and error handling
- **19_state_history** 🚧 - Undo/redo functionality
- **20_state_persistence** 🚧 - Saving and loading state

### 04_hot_reload/ - Hot Reload (Examples 21-25)
**Learning Path: For development workflow**

- **21_file_watching** ✅ - File watcher configuration and setup
- **22_directory_watching** 🚧 - Watching entire directories
- **23_reload_strategies** 🚧 - Different reload strategies and error handling
- **24_performance_monitoring** 🚧 - Hot reload performance optimization
- **25_production_hot_reload** 🚧 - Hot reload in production environments

### 05_advanced/ - Advanced Features (Examples 26-35)
**Learning Path: For complex applications**

- **26_command_system** ✅ - Command pattern implementation
- **27_data_binding** 🚧 - Advanced data binding patterns
- **28_form_validation** 🚧 - Complex form validation
- **29_async_operations** 🚧 - Asynchronous operations and loading states
- **30_error_handling** 🚧 - Comprehensive error handling strategies
- **31_testing_integration** 🚧 - Testing DeclarativeUI applications
- **32_performance_optimization** 🚧 - Performance best practices
- **33_memory_management** 🚧 - Memory management and cleanup
- **34_internationalization** 🚧 - i18n and localization
- **35_accessibility** 🚧 - Accessibility features and compliance

### 06_applications/ - Real-World Applications (Examples 36-40)
**Learning Path: Complete application examples**

- **36_todo_app** ✅ - Complete todo application
- **37_settings_manager** 🚧 - Application settings management
- **38_data_dashboard** 🚧 - Real-time data dashboard
- **39_text_editor** 🚧 - Simple text editor with hot reload
- **40_plugin_system** 🚧 - Extensible plugin architecture

## 🚀 Getting Started

### Prerequisites
- Qt 6.x installed
- CMake 3.16 or higher
- C++17 compatible compiler

### Building Examples
```bash
# From the project root
mkdir build && cd build
cmake .. -DBUILD_EXAMPLES=ON
cmake --build . --config Release

# Examples will be built to build/examples/
```

### Running Examples
```bash
# Basic examples
./examples/01_basic/01_hello_world
./examples/01_basic/02_json_ui_loading
./examples/01_basic/03_simple_state
./examples/01_basic/04_basic_hot_reload
./examples/01_basic/05_event_handling

# Component examples
./examples/02_components/06_basic_components
./examples/02_components/07_input_components

# State management examples
./examples/03_state_management/16_reactive_state

# Hot reload examples
./examples/04_hot_reload/21_file_watching

# Advanced examples
./examples/05_advanced/26_command_system

# Real-world applications
./examples/06_applications/36_todo_app
```

## 📚 Learning Path

### Beginner (Start Here)
1. **01_hello_world** - Understand basic structure
2. **02_json_ui_loading** - Learn declarative UI definition
3. **03_simple_state** - Grasp reactive state management
4. **04_basic_hot_reload** - Experience development workflow
5. **05_event_handling** - Master user interactions

### Intermediate
6. **06_basic_components** - Explore fundamental components
7. **07-15_component_examples** - Master all UI components
8. **16-20_state_examples** - Advanced state management
9. **21-25_hot_reload_examples** - Development optimization

### Advanced
10. **26-35_advanced_examples** - Complex application patterns
11. **36-40_application_examples** - Real-world implementations

## 🔧 Legacy Examples

The following legacy examples are maintained for backward compatibility:
- `BasicExample` (main.cpp)
- `HotReloadExample` (hot_reload_example.cpp)
- `FormExample` (form_example.cpp)
- `DashboardExample` (dashboard_example.cpp)
- `AdvancedExample` (advanced_example.cpp)
- `CommandExample` (command_example.cpp)

## 📖 Documentation

Each example includes:
- **Comprehensive comments** explaining concepts
- **Learning objectives** clearly stated
- **JSON UI definitions** where applicable
- **Fallback implementations** for error handling
- **Debug output** for understanding execution flow

## 🤝 Contributing

When adding new examples:
1. Follow the numbering convention
2. Include comprehensive documentation
3. Provide both JSON and programmatic UI options
4. Add appropriate error handling
5. Update this README

## 📝 Status Legend

- ✅ **Complete** - Fully implemented and tested
- 🚧 **Planned** - Designed but not yet implemented
- ⚠️ **Deprecated** - Legacy example, use new structured version

---

**Next Steps**: Start with `01_hello_world` and work your way through the examples in order for the best learning experience!
