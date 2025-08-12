# Testing Guide for DeclarativeUI

This document provides comprehensive guidelines for testing DeclarativeUI components, modules, and applications.

## Testing Philosophy

DeclarativeUI follows a multi-layered testing approach:

1. **Unit Tests** - Test individual components in isolation
2. **Integration Tests** - Test component interactions and JSON loading
3. **Performance Tests** - Ensure acceptable performance characteristics
4. **End-to-End Tests** - Test complete application workflows
5. **Visual Tests** - Verify UI appearance and behavior

## Test Structure

### Directory Organization

```
tests/
├── unit/                   # Unit tests for individual components
│   ├── core/
│   │   ├── test_ui_element.cpp
│   │   └── test_declarative_builder.cpp
│   ├── components/
│   │   ├── test_button.cpp
│   │   ├── test_line_edit.cpp
│   │   └── test_combo_box.cpp
│   ├── json/
│   │   ├── test_json_loader.cpp
│   │   └── test_component_registry.cpp
│   └── binding/
│       ├── test_state_manager.cpp
│       └── test_property_binding.cpp
├── integration/            # Integration tests
│   ├── test_json_ui_loading.cpp
│   ├── test_hot_reload.cpp
│   └── test_state_binding.cpp
├── performance/            # Performance benchmarks
│   ├── test_component_creation.cpp
│   ├── test_state_updates.cpp
│   └── test_hot_reload_performance.cpp
├── visual/                 # Visual regression tests
│   ├── test_component_appearance.cpp
│   └── reference_images/
├── fixtures/               # Test data and utilities
│   ├── test_ui_definitions/
│   ├── mock_components.hpp
│   └── test_helpers.hpp
└── CMakeLists.txt
```

## Unit Testing

### Basic Component Testing

```cpp
// test_button.cpp
#include <QtTest/QtTest>
#include <QSignalSpy>
#include "Components/Button.hpp"
#include "fixtures/test_helpers.hpp"

class TestButton : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Basic functionality tests
    void testInitialization();
    void testTextProperty();
    void testIconProperty();
    void testEnabledProperty();
    void testStyleProperty();

    // Event handling tests
    void testClickHandler();
    void testMultipleHandlers();
    void testHandlerRemoval();

    // Property binding tests
    void testPropertyBinding();
    void testBindingUpdates();
    void testBindingDisconnection();

    // Error handling tests
    void testInvalidProperties();
    void testExceptionSafety();
    void testResourceCleanup();

    // Performance tests
    void testCreationPerformance();
    void testUpdatePerformance();

private:
    std::unique_ptr<Components::Button> button_;
    TestHelpers::MockStateManager* mock_state_;
};

void TestButton::initTestCase() {
    // One-time setup for all tests
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
}

void TestButton::init() {
    // Setup before each test
    button_ = std::make_unique<Components::Button>();
    mock_state_ = new TestHelpers::MockStateManager();
}

void TestButton::cleanup() {
    // Cleanup after each test
    button_.reset();
    delete mock_state_;
}

void TestButton::testInitialization() {
    QVERIFY(button_ != nullptr);
    QVERIFY(!button_->isInitialized());

    button_->initialize();

    QVERIFY(button_->isInitialized());
    QVERIFY(button_->getWidget() != nullptr);
    QVERIFY(qobject_cast<QPushButton*>(button_->getWidget()) != nullptr);
}

void TestButton::testTextProperty() {
    const QString test_text = "Test Button";

    button_->text(test_text);
    button_->initialize();

    auto qt_button = qobject_cast<QPushButton*>(button_->getWidget());
    QVERIFY(qt_button != nullptr);
    QCOMPARE(qt_button->text(), test_text);

    // Test property retrieval
    auto property_value = button_->getProperty("text");
    QVERIFY(std::holds_alternative<QString>(property_value));
    QCOMPARE(std::get<QString>(property_value), test_text);
}

void TestButton::testClickHandler() {
    bool clicked = false;

    button_->onClick([&clicked]() {
        clicked = true;
    });
    button_->initialize();

    // Simulate click
    auto qt_button = qobject_cast<QPushButton*>(button_->getWidget());
    QVERIFY(qt_button != nullptr);

    QTest::mouseClick(qt_button, Qt::LeftButton);

    QVERIFY(clicked);
}

void TestButton::testPropertyBinding() {
    mock_state_->setState("button_text", QString("Initial Text"));

    button_->bindProperty("text", [this]() {
        return mock_state_->getState<QString>("button_text")->get();
    });
    button_->initialize();

    auto qt_button = qobject_cast<QPushButton*>(button_->getWidget());
    QCOMPARE(qt_button->text(), QString("Initial Text"));

    // Update state and refresh
    mock_state_->setState("button_text", QString("Updated Text"));
    button_->refresh();

    QCOMPARE(qt_button->text(), QString("Updated Text"));
}

void TestButton::testExceptionSafety() {
    // Test that exceptions during initialization don't leak resources
    button_->text(""); // Invalid empty text

    QVERIFY_EXCEPTION_THROWN(button_->initialize(), std::invalid_argument);

    // Button should still be in a valid state
    QVERIFY(!button_->isInitialized());
    QVERIFY(button_->getWidget() == nullptr);

    // Should be able to recover
    button_->text("Valid Text");
    QVERIFY_NO_EXCEPTION_THROWN(button_->initialize());
    QVERIFY(button_->isInitialized());
}

void TestButton::testCreationPerformance() {
    QElapsedTimer timer;
    timer.start();

    const int num_buttons = 1000;
    std::vector<std::unique_ptr<Components::Button>> buttons;
    buttons.reserve(num_buttons);

    for (int i = 0; i < num_buttons; ++i) {
        auto button = std::make_unique<Components::Button>();
        button->text(QString("Button %1").arg(i));
        button->initialize();
        buttons.push_back(std::move(button));
    }

    qint64 elapsed = timer.elapsed();
    qDebug() << "Created" << num_buttons << "buttons in" << elapsed << "ms";

    // Should create 1000 buttons in less than 1 second
    QVERIFY(elapsed < 1000);

    // Average creation time should be reasonable
    double avg_time = static_cast<double>(elapsed) / num_buttons;
    QVERIFY(avg_time < 1.0); // Less than 1ms per button
}

QTEST_MAIN(TestButton)
#include "test_button.moc"
```

### State Manager Testing

```cpp
// test_state_manager.cpp
class TestStateManager : public QObject {
    Q_OBJECT

private slots:
    void init();
    void cleanup();

    void testBasicStateOperations();
    void testStateObservation();
    void testBatchUpdates();
    void testStateValidation();
    void testHistoryAndUndo();
    void testDependencyTracking();
    void testPerformanceMonitoring();
    void testThreadSafety();

private:
    StateManager* state_manager_;
};

void TestStateManager::init() {
    // Use a separate instance for testing
    state_manager_ = new StateManager();
}

void TestStateManager::testBasicStateOperations() {
    // Test setting and getting state
    state_manager_->setState("test_int", 42);
    state_manager_->setState("test_string", QString("hello"));

    auto int_state = state_manager_->getState<int>("test_int");
    auto string_state = state_manager_->getState<QString>("test_string");

    QVERIFY(int_state != nullptr);
    QVERIFY(string_state != nullptr);
    QCOMPARE(int_state->get(), 42);
    QCOMPARE(string_state->get(), QString("hello"));

    // Test state existence
    QVERIFY(state_manager_->hasState("test_int"));
    QVERIFY(state_manager_->hasState("test_string"));
    QVERIFY(!state_manager_->hasState("nonexistent"));

    // Test state removal
    QVERIFY(state_manager_->removeState("test_int"));
    QVERIFY(!state_manager_->hasState("test_int"));
    QVERIFY(!state_manager_->removeState("nonexistent"));
}

void TestStateManager::testStateObservation() {
    int observation_count = 0;
    QString last_observed_value;

    // Set up observer
    state_manager_->observeState<QString>("observed_state",
        [&](const QString& value) {
            observation_count++;
            last_observed_value = value;
        });

    // Initial state setting should trigger observer
    state_manager_->setState("observed_state", QString("initial"));
    QCOMPARE(observation_count, 1);
    QCOMPARE(last_observed_value, QString("initial"));

    // State updates should trigger observer
    state_manager_->setState("observed_state", QString("updated"));
    QCOMPARE(observation_count, 2);
    QCOMPARE(last_observed_value, QString("updated"));
}

void TestStateManager::testBatchUpdates() {
    QSignalSpy spy(state_manager_, &StateManager::stateChanged);

    state_manager_->batchUpdate([this]() {
        state_manager_->setState("batch1", 1);
        state_manager_->setState("batch2", 2);
        state_manager_->setState("batch3", 3);
    });

    // All states should be set
    QCOMPARE(state_manager_->getState<int>("batch1")->get(), 1);
    QCOMPARE(state_manager_->getState<int>("batch2")->get(), 2);
    QCOMPARE(state_manager_->getState<int>("batch3")->get(), 3);

    // Should have received exactly 3 signals (one per state)
    QCOMPARE(spy.count(), 3);
}

void TestStateManager::testStateValidation() {
    // Set up validator that only allows positive numbers
    state_manager_->setValidator<int>("positive_int", [](const int& value) {
        return value > 0;
    });

    // Valid value should be accepted
    QVERIFY_NO_EXCEPTION_THROWN(state_manager_->setState("positive_int", 42));
    QCOMPARE(state_manager_->getState<int>("positive_int")->get(), 42);

    // Invalid value should be rejected
    QVERIFY_EXCEPTION_THROWN(
        state_manager_->setState("positive_int", -1),
        std::invalid_argument
    );

    // State should remain unchanged
    QCOMPARE(state_manager_->getState<int>("positive_int")->get(), 42);
}

void TestStateManager::testHistoryAndUndo() {
    state_manager_->enableHistory("history_test", 10);

    // Set initial value
    state_manager_->setState("history_test", 1);
    QVERIFY(!state_manager_->canUndo("history_test")); // No previous value

    // Update value
    state_manager_->setState("history_test", 2);
    QVERIFY(state_manager_->canUndo("history_test"));
    QVERIFY(!state_manager_->canRedo("history_test"));

    // Undo
    state_manager_->undo("history_test");
    QCOMPARE(state_manager_->getState<int>("history_test")->get(), 1);
    QVERIFY(state_manager_->canRedo("history_test"));

    // Redo
    state_manager_->redo("history_test");
    QCOMPARE(state_manager_->getState<int>("history_test")->get(), 2);
    QVERIFY(!state_manager_->canRedo("history_test"));
}

void TestStateManager::testThreadSafety() {
    const int num_threads = 4;
    const int operations_per_thread = 100;

    QVector<QFuture<void>> futures;

    for (int t = 0; t < num_threads; ++t) {
        auto future = QtConcurrent::run([this, t, operations_per_thread]() {
            for (int i = 0; i < operations_per_thread; ++i) {
                QString key = QString("thread_%1_key_%2").arg(t).arg(i);
                state_manager_->setState(key, i);

                auto state = state_manager_->getState<int>(key);
                QVERIFY(state != nullptr);
                QCOMPARE(state->get(), i);
            }
        });
        futures.append(future);
    }

    // Wait for all threads to complete
    for (auto& future : futures) {
        future.waitForFinished();
    }

    // Verify all states were set correctly
    for (int t = 0; t < num_threads; ++t) {
        for (int i = 0; i < operations_per_thread; ++i) {
            QString key = QString("thread_%1_key_%2").arg(t).arg(i);
            auto state = state_manager_->getState<int>(key);
            QVERIFY(state != nullptr);
            QCOMPARE(state->get(), i);
        }
    }
}

QTEST_MAIN(TestStateManager)
#include "test_state_manager.moc"
```

## Integration Testing

### JSON UI Loading Tests

```cpp
// test_json_ui_loading.cpp
class TestJSONUILoading : public QObject {
    Q_OBJECT

private slots:
    void init();
    void cleanup();

    void testBasicWidgetCreation();
    void testNestedWidgets();
    void testPropertyApplication();
    void testEventBinding();
    void testStateBinding();
    void testLayoutManagement();
    void testErrorHandling();

private:
    std::unique_ptr<JSON::JSONUILoader> loader_;
    std::unique_ptr<StateManager> state_manager_;
};

void TestJSONUILoading::testBasicWidgetCreation() {
    QString json = R"({
        "type": "QPushButton",
        "properties": {
            "text": "Test Button",
            "enabled": true,
            "minimumSize": [100, 30]
        }
    })";

    auto widget = loader_->loadFromString(json);
    QVERIFY(widget != nullptr);

    auto button = qobject_cast<QPushButton*>(widget.get());
    QVERIFY(button != nullptr);
    QCOMPARE(button->text(), QString("Test Button"));
    QVERIFY(button->isEnabled());
    QCOMPARE(button->minimumSize(), QSize(100, 30));
}

void TestJSONUILoading::testStateBinding() {
    state_manager_->setState("button_text", QString("Dynamic Text"));
    loader_->bindStateManager(
        std::shared_ptr<StateManager>(state_manager_.get(), [](auto*){}));

    QString json = R"({
        "type": "QPushButton",
        "bindings": {
            "text": "button_text"
        }
    })";

    auto widget = loader_->loadFromString(json);
    auto button = qobject_cast<QPushButton*>(widget.get());

    QVERIFY(button != nullptr);
    QCOMPARE(button->text(), QString("Dynamic Text"));

    // Update state and verify binding
    state_manager_->setState("button_text", QString("Updated Text"));
    // Note: In real implementation, this would trigger automatic update
    QCOMPARE(button->text(), QString("Updated Text"));
}

void TestJSONUILoading::testErrorHandling() {
    // Test invalid JSON
    QString invalid_json = "{ invalid json }";
    QVERIFY_EXCEPTION_THROWN(
        loader_->loadFromString(invalid_json),
        JSON::JSONParsingException
    );

    // Test missing required properties
    QString missing_type = R"({
        "properties": {
            "text": "Button"
        }
    })";
    QVERIFY_EXCEPTION_THROWN(
        loader_->loadFromString(missing_type),
        JSON::JSONValidationException
    );

    // Test unknown component type
    QString unknown_type = R"({
        "type": "UnknownWidget"
    })";
    QVERIFY_EXCEPTION_THROWN(
        loader_->loadFromString(unknown_type),
        JSON::ComponentCreationException
    );
}
```

## Performance Testing

### Component Creation Benchmarks

```cpp
// test_component_creation.cpp
class TestComponentCreationPerformance : public QObject {
    Q_OBJECT

private slots:
    void benchmarkButtonCreation();
    void benchmarkComplexWidgetCreation();
    void benchmarkJSONLoading();
    void benchmarkStateUpdates();

private:
    void createButtons(int count);
    void createComplexWidget();
};

void TestComponentCreationPerformance::benchmarkButtonCreation() {
    QBENCHMARK {
        createButtons(100);
    }
}

void TestComponentCreationPerformance::createButtons(int count) {
    std::vector<std::unique_ptr<Components::Button>> buttons;
    buttons.reserve(count);

    for (int i = 0; i < count; ++i) {
        auto button = std::make_unique<Components::Button>();
        button->text(QString("Button %1").arg(i))
              .onClick([]() { /* handler */ });
        button->initialize();
        buttons.push_back(std::move(button));
    }
}

void TestComponentCreationPerformance::benchmarkJSONLoading() {
    QString json = R"({
        "type": "QWidget",
        "layout": {
            "type": "VBoxLayout"
        },
        "children": [)" +
        // Generate 100 button definitions
        [](){
            QStringList buttons;
            for (int i = 0; i < 100; ++i) {
                buttons << QString(R"({
                    "type": "QPushButton",
                    "properties": {
                        "text": "Button %1"
                    }
                })").arg(i);
            }
            return buttons.join(",");
        }() + R"(]
    })";

    JSON::JSONUILoader loader;

    QBENCHMARK {
        auto widget = loader.loadFromString(json);
        Q_UNUSED(widget);
    }
}
```

## Visual Testing

### Component Appearance Tests

```cpp
// test_component_appearance.cpp
class TestComponentAppearance : public QObject {
    Q_OBJECT

private slots:
    void testButtonAppearance();
    void testLayoutAppearance();
    void testThemeApplication();

private:
    QPixmap captureWidget(QWidget* widget);
    bool compareImages(const QPixmap& actual, const QString& reference_path);
};

void TestComponentAppearance::testButtonAppearance() {
    auto button = std::make_unique<Components::Button>();
    button->text("Test Button")
          .style("QPushButton { background-color: blue; color: white; }");
    button->initialize();

    auto widget = button->getWidget();
    widget->resize(100, 30);
    widget->show();

    QTest::qWaitForWindowExposed(widget);

    QPixmap actual = captureWidget(widget);
    QVERIFY(compareImages(actual, "reference_images/blue_button.png"));
}

QPixmap TestComponentAppearance::captureWidget(QWidget* widget) {
    return widget->grab();
}

bool TestComponentAppearance::compareImages(const QPixmap& actual,
                                          const QString& reference_path) {
    QPixmap reference(reference_path);
    if (reference.isNull()) {
        // Save actual as new reference if reference doesn't exist
        actual.save(reference_path);
        return true;
    }

    // Compare images (simplified - real implementation would use
    // more sophisticated comparison with tolerance)
    return actual.toImage() == reference.toImage();
}
```

## Test Utilities

### Mock Components

```cpp
// fixtures/mock_components.hpp
namespace TestHelpers {

class MockStateManager : public StateManager {
public:
    // Override methods for testing
    template<typename T>
    void setState(const QString& key, T&& value) override {
        states_[key] = QVariant::fromValue(std::forward<T>(value));
        emit stateChanged(key, states_[key]);
    }

    template<typename T>
    std::shared_ptr<ReactiveProperty<T>> getState(const QString& key) const override {
        auto it = states_.find(key);
        if (it != states_.end()) {
            return std::make_shared<MockReactiveProperty<T>>(it->second.value<T>());
        }
        return nullptr;
    }

private:
    QVariantMap states_;
};

class MockUIElement : public Core::UIElement {
public:
    void initialize() override {
        initialized_ = true;
        widget_ = std::make_unique<QWidget>();
    }

    bool isInitialized() const { return initialized_; }

private:
    bool initialized_ = false;
};

}  // namespace TestHelpers
```

## Running Tests

### CMake Test Configuration

```cmake
# tests/CMakeLists.txt
enable_testing()

find_package(Qt6 REQUIRED COMPONENTS Test)

# Function to add a test
function(add_declarative_test test_name)
    add_executable(${test_name} ${test_name}.cpp)
    target_link_libraries(${test_name}
        DeclarativeUI
        Components
        Qt6::Test
        Qt6::Widgets
    )
    set_target_properties(${test_name} PROPERTIES
        AUTOMOC ON
    )
    add_test(NAME ${test_name} COMMAND ${test_name})
endfunction()

# Add all tests
add_declarative_test(test_button)
add_declarative_test(test_state_manager)
add_declarative_test(test_json_ui_loading)
add_declarative_test(test_component_creation_performance)
```

### Running Tests

```bash
# Run all tests
cmake --build . --target test

# Run specific test
./test_button

# Run tests with verbose output
ctest --verbose

# Run performance tests only
ctest -R "performance"

# Run tests with coverage
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON
cmake --build .
ctest
gcov *.gcno
```

This comprehensive testing approach ensures DeclarativeUI components are reliable, performant, and maintainable.
