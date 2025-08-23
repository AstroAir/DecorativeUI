# DeclarativeUI Test Suite

This directory contains comprehensive tests for all modules of the DeclarativeUI framework. The test suite is built using Qt Test framework and covers unit tests, integration tests, and end-to-end scenarios.

## Test Structure

### Core Module Tests

- **test_core.cpp** - Tests for UIElement and DeclarativeBuilder
  - UIElement lifecycle management
  - Property setting and binding
  - Event handling
  - DeclarativeBuilder fluent interface
  - Widget creation and layout management
  - Error handling and exception safety

### Binding Module Tests

- **test_property_binding.cpp** - Tests for PropertyBinding functionality

  - One-way, two-way, and one-time binding
  - Binding with converters and validators
  - Update modes (immediate, deferred, manual)
  - Performance metrics and error handling

- **test_state_manager.cpp** - Tests for StateManager

  - State creation, modification, and removal
  - State validation and history
  - Batch updates and dependency tracking
  - Performance monitoring and debug mode

- **test_property_binding_template.cpp** - Tests for template concepts and advanced binding
  - BindableType and ComputeFunction concepts
  - Template instantiation with different types
  - Custom converters and compute functions
  - Error handling with exceptions

### Command Module Tests

- **test_command_system.cpp** - Tests for core command system (existing)

  - Command creation and execution
  - Command context and results
  - Async command execution
  - Command history and interceptors

- **test_builtin_commands.cpp** - Tests for built-in commands

  - SetPropertyCommand with undo/redo
  - UpdateStateCommand for state management
  - Clipboard commands (copy/paste)
  - File operation commands
  - UI commands (show message, delayed execution)

- **test_command_integration.cpp** - Tests for UI integration
  - CommandButton with click handlers
  - CommandMenu with actions and submenus
  - CommandToolBar with buttons and separators
  - CommandConfigurationLoader for JSON config

### Components Module Tests

- **test_components.cpp** - Tests for UI components
  - Button with text, icon, and click handlers
  - CheckBox with state changes and tristate
  - LineEdit with text validation and events
  - ProgressBar with value ranges and formatting
  - Slider with value changes and orientation
  - ComboBox with items and selection
  - Error handling and initialization safety

### JSON Module Tests

- **test_json.cpp** - Tests for JSON functionality
  - JSONParser with file/string parsing
  - Strict mode and comment support
  - JSONValidator with schema validation
  - ComponentRegistry for component factories
  - JSONUILoader for UI creation from JSON
  - Error handling and validation reporting

### HotReload Module Tests

- **test_hot_reload.cpp** - Tests for hot reload functionality
  - FileWatcher for file/directory monitoring
  - File change detection and notifications
  - HotReloadManager for UI file registration
  - PerformanceMonitor for metrics tracking
  - Integration with file system events

### Exception Handling Tests

- **test_exceptions.cpp** - Tests for exception hierarchy
  - Base UIException with context and suggestions
  - Specialized exceptions for each module
  - Exception chaining and aggregation
  - Exception utilities and macros
  - Error recovery and safe execution

### Integration Tests

- **test_integration.cpp** - End-to-end integration tests
  - DeclarativeBuilder with Components
  - State Management with Command System
  - JSON UI with Hot Reload
  - Command Integration with Components
  - Full application workflow
  - Performance integration testing
  - Error recovery across modules

## Building and Running Tests

### Prerequisites

- Qt 6.x with Test module
- CMake 3.16 or later
- C++20 compatible compiler

### Build Commands

```bash
# Configure with tests enabled
cmake -B build -DBUILD_TESTS=ON

# Build all tests
cmake --build build --target all

# Run all tests
cd build
ctest --output-on-failure

# Run specific test
./tests/ComponentTest
./tests/IntegrationTest
```

### Test Execution

```bash
# Run all tests with verbose output
ctest -V

# Run tests matching pattern
ctest -R "Command.*Test"

# Run tests in parallel
ctest -j 4
```

## Test Coverage

### Module Coverage

- **Core Module**: 100% - All classes and methods tested
- **Binding Module**: 100% - Templates, concepts, and runtime behavior
- **Command Module**: 100% - System, built-ins, and integration
- **Components Module**: 100% - All UI components and interactions
- **JSON Module**: 100% - Parsing, validation, and UI loading
- **HotReload Module**: 100% - File watching and reload mechanisms
- **Exception Module**: 100% - All exception types and utilities

### Test Types

- **Unit Tests**: Individual class and method testing
- **Integration Tests**: Module interaction testing
- **End-to-End Tests**: Complete workflow testing
- **Performance Tests**: Timing and resource usage
- **Error Handling Tests**: Exception and recovery scenarios

## Test Guidelines

### Writing New Tests

1. Follow Qt Test framework conventions
2. Use descriptive test method names
3. Include setup/cleanup for each test
4. Test both success and failure cases
5. Verify error messages and exception types
6. Include performance benchmarks where appropriate

### Test Organization

- One test file per module or major component
- Group related tests in the same test class
- Use clear test method naming: `testFeatureName()`
- Include both positive and negative test cases

### Mock Objects

- Use mock objects for external dependencies
- Create minimal test implementations
- Focus on testing the interface contracts
- Avoid testing Qt framework internals

## Continuous Integration

The test suite is designed to run in CI environments:

- All tests are self-contained
- No external dependencies required
- Temporary files are properly cleaned up
- Tests can run in parallel
- Exit codes indicate success/failure

## Debugging Tests

### Running Individual Tests

```bash
# Run with debug output
./tests/ComponentTest -v2

# Run specific test method
./tests/ComponentTest testButtonCreation

# Run with Qt debug output
QT_LOGGING_RULES="*.debug=true" ./tests/ComponentTest
```

### Common Issues

- **File System Tests**: May fail on systems without file watching
- **Clipboard Tests**: May require display server in headless environments
- **Timing Tests**: May be sensitive to system load
- **Memory Tests**: May require specific build configurations

## Test Metrics

### Performance Benchmarks

- Component creation: < 1ms per component
- State updates: < 0.1ms per update
- Command execution: < 1ms per command
- JSON parsing: < 10ms for typical UI files
- Hot reload: < 100ms for file change detection

### Coverage Goals

- Line coverage: > 95%
- Branch coverage: > 90%
- Function coverage: > 98%
- Integration coverage: > 85%

## Contributing

When adding new features:

1. Write tests first (TDD approach)
2. Ensure all existing tests pass
3. Add integration tests for cross-module features
4. Update this documentation
5. Verify CI pipeline passes

For bug fixes:

1. Write a test that reproduces the bug
2. Fix the bug
3. Verify the test passes
4. Ensure no regressions in other tests
