# DeclarativeUI Testing Guide

This guide provides comprehensive information on testing practices, conventions, and examples for the DeclarativeUI framework.

## Testing Philosophy

The DeclarativeUI test suite follows these principles:

- **Comprehensive Coverage**: 95%+ code coverage across all modules
- **Fast Execution**: Tests should run quickly to enable frequent execution
- **Reliable Results**: Tests should be deterministic and not flaky
- **Clear Documentation**: Each test should clearly document what it's testing
- **Performance Awareness**: Include performance benchmarks and regression detection
- **Error Resilience**: Test error conditions and recovery scenarios

## Test Categories and When to Use Them

### Unit Tests (`tests/unit/`)

**Purpose**: Test individual components, classes, and functions in isolation.

**When to use**:

- Testing a single component's functionality
- Validating property setters and getters
- Testing event handling and signals
- Verifying error conditions and edge cases

**Example Structure**:

```cpp
#include <QTest>
#include "../utils/TestUtilities.hpp"
#include "../Components/Button.hpp"

class ButtonTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        DeclarativeUI::Testing::TestUtilities::initializeQtApplication();
    }

    void testButtonCreation() {
        auto button = std::make_unique<Button>();
        button->text("Test Button")
              .enabled(true);
        button->initialize();

        auto* widget = button->getWidget();
        DeclarativeUI::Testing::AssertUtils::assertWidgetValid(widget, "Button widget");

        auto* qt_button = qobject_cast<QPushButton*>(widget);
        QVERIFY(qt_button != nullptr);
        QCOMPARE(qt_button->text(), QString("Test Button"));
        QVERIFY(qt_button->isEnabled());
    }

    void testButtonClickEvent() {
        auto button = std::make_unique<Button>();

        bool clicked = false;
        button->onClick([&clicked]() { clicked = true; });
        button->initialize();

        auto* widget = button->getWidget();
        auto* qt_button = qobject_cast<QPushButton*>(widget);

        // Simulate click
        qt_button->click();
        QVERIFY(clicked);
    }
};

QTEST_MAIN(ButtonTest)
#include "test_button.moc"
```

### Integration Tests (`tests/integration/`)

**Purpose**: Test interactions between multiple components and systems.

**When to use**:

- Testing complete workflows
- Validating system integration points
- Testing migration scenarios
- End-to-end functionality testing

**Example Structure**:

```cpp
#include <QTest>
#include "../utils/TestUtilities.hpp"
#include "../Binding/StateManager.hpp"
#include "../Components/Button.hpp"
#include "../Components/LineEdit.hpp"

class StateIntegrationTest : public QObject {
    Q_OBJECT

private slots:
    void testStateComponentIntegration() {
        auto& state_manager = StateManager::instance();

        // Set up state
        state_manager.setState("ui.button_text", QString("Dynamic Button"));

        // Create component bound to state
        auto button = std::make_unique<Button>();
        button->bindText("ui.button_text");
        button->initialize();

        // Verify initial binding
        auto* widget = button->getWidget();
        auto* qt_button = qobject_cast<QPushButton*>(widget);
        QCOMPARE(qt_button->text(), QString("Dynamic Button"));

        // Update state and verify propagation
        state_manager.setState("ui.button_text", QString("Updated Button"));
        QCOMPARE(qt_button->text(), QString("Updated Button"));
    }
};
```

### Performance Tests (`tests/performance/`)

**Purpose**: Benchmark performance and detect regressions.

**When to use**:

- Measuring component creation time
- Testing memory usage scaling
- Benchmarking concurrent operations
- Stress testing with large datasets

**Example Structure**:

```cpp
#include <QTest>
#include "../utils/TestUtilities.hpp"

class ComponentPerformanceTest : public QObject {
    Q_OBJECT

private slots:
    void benchmarkButtonCreation() {
        QBENCHMARK {
            auto button = std::make_unique<Button>();
            button->text("Benchmark Button");
            button->initialize();
        }
    }

    void testMemoryScaling() {
        auto initial_memory = DeclarativeUI::Testing::PerformanceUtils::getCurrentMemoryUsage();

        std::vector<std::unique_ptr<Button>> buttons;
        const int num_buttons = 1000;

        for (int i = 0; i < num_buttons; ++i) {
            auto button = std::make_unique<Button>();
            button->text(QString("Button %1").arg(i));
            button->initialize();
            buttons.push_back(std::move(button));
        }

        auto final_memory = DeclarativeUI::Testing::PerformanceUtils::getCurrentMemoryUsage();
        size_t memory_per_button = (final_memory - initial_memory) / num_buttons;

        DeclarativeUI::Testing::AssertUtils::assertMemoryUsage(
            memory_per_button, 10000, "Button memory usage");
    }
};
```

## Test Naming Conventions

### File Naming

- **Unit tests**: `test_<component_name>.cpp`
- **Integration tests**: `test_<workflow_name>_integration.cpp`
- **Performance tests**: `test_<component_name>_performance.cpp`
- **Error handling tests**: `test_<component_name>_error_handling.cpp`

### Class Naming

- **Test classes**: `<ComponentName>Test`
- **Performance test classes**: `<ComponentName>PerformanceTest`
- **Integration test classes**: `<WorkflowName>IntegrationTest`

### Method Naming

- **Functionality tests**: `test<FunctionName>()`
- **Error condition tests**: `test<FunctionName>ErrorHandling()`
- **Performance tests**: `benchmark<FunctionName>()`
- **Integration tests**: `test<WorkflowName>Integration()`

### Examples

```cpp
// Good naming examples
void testButtonCreation();
void testButtonClickEvent();
void testButtonErrorHandling();
void benchmarkButtonCreation();
void testStateManagementIntegration();

// Avoid these patterns
void test1(); // Not descriptive
void buttonTest(); // Doesn't follow convention
void testEverything(); // Too broad
```

## Using Test Utilities

### TestUtilities Class

```cpp
#include "../utils/TestUtilities.hpp"

// Generate random test data
QString random_text = DeclarativeUI::Testing::TestUtilities::generateRandomString(20);
int random_value = DeclarativeUI::Testing::TestUtilities::generateRandomInt(1, 100);

// Create temporary files for testing
auto temp_file = DeclarativeUI::Testing::TestUtilities::createTempFile(
    "test content", ".json");

// Wait for conditions
bool success = DeclarativeUI::Testing::TestUtilities::waitForCondition(
    [&]() { return widget->isVisible(); }, 5000);

// Measure execution time
qint64 time_ms = DeclarativeUI::Testing::TestUtilities::measureExecutionTime([&]() {
    // Code to measure
    component->initialize();
});
```

### PerformanceUtils Class

```cpp
#include "../utils/TestUtilities.hpp"

// Benchmark with multiple iterations
auto result = DeclarativeUI::Testing::PerformanceUtils::benchmark([&]() {
    auto button = std::make_unique<Button>();
    button->initialize();
}, 100); // 100 iterations

qDebug() << "Average time:" << result.avg_time_ms << "ms";
qDebug() << "Min time:" << result.min_time_ms << "ms";
qDebug() << "Max time:" << result.max_time_ms << "ms";
```

### AssertUtils Class

```cpp
#include "../utils/TestUtilities.hpp"

// Enhanced assertions with better error messages
DeclarativeUI::Testing::AssertUtils::assertWidgetValid(widget, "Button should be valid");

DeclarativeUI::Testing::AssertUtils::assertPerformance(
    execution_time, 100, "Button creation");

DeclarativeUI::Testing::AssertUtils::assertNoExceptions([&]() {
    component->initialize();
}, "Component initialization");
```

## Error Testing Best Practices

### Testing Error Conditions

```cpp
void testInvalidInput() {
    auto component = std::make_unique<LineEdit>();

    // Test with null input
    component->text(QString());
    component->initialize();

    // Should handle gracefully
    auto* widget = component->getWidget();
    QVERIFY(widget != nullptr);
}

void testExceptionHandling() {
    DeclarativeUI::Testing::AssertUtils::assertNoExceptions([&]() {
        // Code that should not throw
        auto component = std::make_unique<Button>();
        component->initialize();
    }, "Button initialization");
}
```

### Testing Recovery Scenarios

```cpp
void testErrorRecovery() {
    auto& state_manager = StateManager::instance();

    // Set invalid state
    state_manager.setState("invalid.key", QVariant());

    // Component should handle gracefully
    auto button = std::make_unique<Button>();
    button->bindText("invalid.key");
    button->initialize();

    // Should have fallback behavior
    auto* widget = button->getWidget();
    auto* qt_button = qobject_cast<QPushButton*>(widget);
    QVERIFY(!qt_button->text().isEmpty()); // Should have default text
}
```

## Thread Safety Testing

### Concurrent Access Testing

```cpp
void testConcurrentAccess() {
    auto& state_manager = StateManager::instance();

    const int num_threads = 4;
    const int operations_per_thread = 100;
    std::atomic<int> success_count{0};

    QVector<QFuture<void>> futures;

    for (int t = 0; t < num_threads; ++t) {
        auto future = QtConcurrent::run([&, t]() {
            for (int i = 0; i < operations_per_thread; ++i) {
                QString key = QString("thread_%1_key_%2").arg(t).arg(i);
                QString value = QString("value_%1_%2").arg(t).arg(i);

                state_manager.setState(key, value);
                auto retrieved = state_manager.getState<QString>(key);

                if (retrieved && retrieved->get() == value) {
                    success_count.fetch_add(1);
                }
            }
        });
        futures.append(future);
    }

    // Wait for completion
    for (auto& future : futures) {
        future.waitForFinished();
    }

    QCOMPARE(success_count.load(), num_threads * operations_per_thread);
}
```

## Adding New Tests

### Step-by-Step Process

1. **Identify Test Category**

   - Unit test for individual component
   - Integration test for workflow
   - Performance test for benchmarking

2. **Create Test File**

   ```bash
   # For unit test
   touch tests/unit/test_new_component.cpp

   # For integration test
   touch tests/integration/test_new_workflow_integration.cpp
   ```

3. **Implement Test Class**

   ```cpp
   #include <QTest>
   #include "../utils/TestUtilities.hpp"

   class NewComponentTest : public QObject {
       Q_OBJECT

   private slots:
       void initTestCase() {
           DeclarativeUI::Testing::TestUtilities::initializeQtApplication();
       }

       void testBasicFunctionality() {
           // Test implementation
       }
   };

   QTEST_MAIN(NewComponentTest)
   #include "test_new_component.moc"
   ```

4. **Update CMakeLists.txt**

   ```cmake
   add_executable(NewComponentTest test_new_component.cpp)
   target_link_libraries(NewComponentTest
       DeclarativeUI
       Components
       Qt6::Core
       Qt6::Widgets
       Qt6::Test
   )
   ```

5. **Run and Verify**
   ```bash
   cd build
   make NewComponentTest
   ./tests/unit/NewComponentTest
   ```

## Continuous Integration

### Test Execution in CI

```yaml
# Example CI configuration
- name: Run Unit Tests
  run: ctest -L unit --output-on-failure

- name: Run Integration Tests
  run: ctest -L integration --output-on-failure

- name: Run Performance Tests
  run: ctest -L performance --output-on-failure

- name: Generate Coverage Report
  run: |
    gcov --coverage
    lcov --capture --directory . --output-file coverage.info
```

### Performance Regression Detection

```yaml
- name: Performance Regression Check
  run: |
    ./tests/performance/ComponentPerformanceTest --benchmark-output=results.json
    python scripts/check_performance_regression.py results.json baseline.json
```

This testing guide ensures consistent, comprehensive, and maintainable tests across the DeclarativeUI framework.
