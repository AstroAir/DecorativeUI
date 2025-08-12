# DeclarativeUI Test Suite

This directory contains comprehensive tests for the DeclarativeUI framework with 95%+ code coverage across all modules.

## Test Structure

### Unit Tests (`unit/`)

- **Core Tests** (`test_core.cpp`): Tests for UIElement and DeclarativeBuilder classes
- **Advanced Core Tests** (`test_core_advanced.cpp`): Tests for CacheManager, MemoryManager, and ParallelProcessor
- **Component Tests** (`test_components.cpp`): Tests for basic UI components (Button, LineEdit, etc.)
- **Container Component Tests** (`test_container_components.cpp`): Tests for container components (ScrollArea, Splitter, etc.)
- **JSON Tests** (`test_json.cpp`): Enhanced tests for JSON parsing, validation, and component registry
- **State Tests** (`test_state.cpp`): Tests for state management and reactive bindings
- **Exception Tests** (`test_exceptions.cpp`): Tests for exception handling and error recovery
- **Thread Safety Tests** (`test_thread_safety.cpp`): Comprehensive thread safety and concurrency tests

### Integration Tests (`integration/`)

- **Integration Tests** (`test_integration.cpp`): Basic cross-component functionality tests
- **End-to-End Workflow Tests** (`test_end_to_end_workflows.cpp`): Complete UI creation and migration workflows
- **Error Handling Integration Tests** (`test_error_handling_integration.cpp`): Integration-level error handling and recovery

### Command Tests (`command/`)

- **Command System Tests** (`test_command_system.cpp`): Core command system functionality
- **Command Performance Tests** (`test_command_performance.cpp`): Performance and stress testing for command system
- **Adapter Tests**: Comprehensive tests for all 5 integration adapters
  - `test_component_system_adapter.cpp`: Component-to-Command conversion
  - `test_json_command_loader.cpp`: JSON command loading and parsing
  - `test_state_manager_adapter.cpp`: State management integration
  - `test_uielement_adapter.cpp`: UI element adaptation
  - `test_integration_manager.cpp`: Adapter coordination and management
- **Command Integration Tests** (`test_command_integration.cpp`): Integration between commands and other systems

### Performance Tests (`performance/`)

- **Component Performance Tests** (`test_component_performance.cpp`): Benchmarking and performance regression tests
- Memory usage scaling tests
- Concurrent operation performance tests
- Stress testing with large datasets

### Test Utilities (`utils/`)

- **TestUtilities.hpp**: Comprehensive test utilities, mock objects, and assertion helpers
- **TestRunner.hpp**: Advanced test runner with reporting and benchmarking capabilities
- Data generators for various test scenarios
- Performance measurement utilities

## Running Tests

### All Tests

```bash
cd build
ctest --output-on-failure
```

### Specific Test Categories

```bash
# Unit tests only
ctest -R ".*Test" --output-on-failure

# Command system tests only
ctest -R "Command.*Test" --output-on-failure

# Integration tests only
ctest -R "IntegrationTest" --output-on-failure
```

### Individual Tests

```bash
# Run specific test executable
./tests/unit/CoreTest
./tests/command/CommandSystemTest
./tests/integration/IntegrationTest
```

## Test Categories

### Unit Tests (`unit/`)

These tests focus on individual components and modules:

- **CoreTest**: Tests core framework functionality
- **ComponentTest**: Tests UI component implementations
- **PropertyBindingTest**: Tests property binding system
- **StateManagerTest**: Tests state management functionality
- **PropertyBindingTemplateTest**: Tests property binding templates
- **JSONTest**: Tests JSON parsing and loading
- **HotReloadTest**: Tests hot reload functionality
- **ExceptionsTest**: Tests exception handling

### Integration Tests (`integration/`)

These tests verify interaction between multiple components:

- **IntegrationTest**: Tests cross-component functionality and workflows

### Command System Tests (`command/`)

These tests focus on the command system architecture:

- **CommandSystemTest**: Core command system functionality
- **BuiltinCommandsTest**: Built-in command implementations
- **CommandIntegrationTest**: Command system integration
- **CommandBindingTest**: Command binding mechanisms
- **CommandBuilderTest**: Command builder patterns
- **CommandCoreTest**: Core command functionality
- **CommandEventsTest**: Command event system
- **CommandStateTest**: Command state management
- **WidgetMapperTest**: Widget mapping functionality
- **ComponentSystemAdapterTest**: Component system adapters
- **IntegrationManagerTest**: Integration management
- **JSONCommandLoaderTest**: JSON command loading
- **MVCIntegrationTest**: MVC pattern integration
- **StateManagerAdapterTest**: State manager adapters
- **UIElementAdapterTest**: UIElement adapters

## Adding New Tests

### Unit Tests

1. Create test file in `unit/` directory
2. Add executable to `unit/CMakeLists.txt`
3. Follow naming convention: `test_<module>.cpp`

### Integration Tests

1. Create test file in `integration/` directory
2. Add executable to `integration/CMakeLists.txt`
3. Focus on cross-component interactions

### Command System Tests

1. Create test file in `command/` directory
2. Add executable to `command/CMakeLists.txt`
3. Follow naming convention: `test_<command_feature>.cpp`

## Test Framework

All tests use Qt Test framework (`Qt6::Test`). Each test should:

1. Include `<QtTest/QtTest>`
2. Inherit from `QObject`
3. Use `Q_OBJECT` macro
4. Implement test methods as private slots
5. Use Qt Test macros (`QVERIFY`, `QCOMPARE`, etc.)

## Example Test Structure

```cpp
#include <QtTest/QtTest>
#include "YourComponent.hpp"

class YourComponentTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void testBasicFunctionality();
    void testErrorHandling();
    void cleanupTestCase();
};

void YourComponentTest::initTestCase()
{
    // Setup code
}

void YourComponentTest::testBasicFunctionality()
{
    // Test implementation
    QVERIFY(condition);
    QCOMPARE(actual, expected);
}

void YourComponentTest::testErrorHandling()
{
    // Error handling tests
}

void YourComponentTest::cleanupTestCase()
{
    // Cleanup code
}

QTEST_MAIN(YourComponentTest)
#include "test_your_component.moc"
```

## Dependencies

Tests depend on:

- Qt6::Test
- DeclarativeUI library
- Components library
- Qt6::Core
- Qt6::Widgets

## Troubleshooting

### Common Issues

1. **Test not found**: Ensure test is added to CMakeLists.txt and CTest
2. **Linking errors**: Verify all required libraries are linked
3. **Resource issues**: Check that test resources are copied correctly

### Debug Mode

Run tests with debug output:

```bash
ctest --output-on-failure --verbose
```

Or run individual test with debug:

```bash
./tests/unit/CoreTest -v2
```
