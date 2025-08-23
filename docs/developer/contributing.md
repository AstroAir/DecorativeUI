# Contributing to DeclarativeUI

We welcome contributions to DeclarativeUI! This guide will help you get started with contributing to the project.

## Getting Started

### Development Environment Setup

1. **Prerequisites:**

   - Qt6 (6.0 or higher) with development packages
   - CMake 3.20 or higher
   - C++20 compatible compiler (GCC 10+, Clang 10+, MSVC 2019+)
   - Git

2. **Clone the repository:**

   ```bash
   git clone https://github.com/your-org/DeclarativeUI.git
   cd DeclarativeUI
   ```

3. **Set up development build:**

   ```bash
   mkdir build-dev && cd build-dev
   cmake .. -DCMAKE_BUILD_TYPE=Debug -DBUILD_EXAMPLES=ON -DBUILD_TESTS=ON -DDECLARATIVE_UI_DEBUG=ON
   cmake --build .
   ```

4. **Run tests to verify setup:**

   ```bash
   ctest --output-on-failure
   ```

### Development Workflow

1. **Create a feature branch:**

   ```bash
   git checkout -b feature/your-feature-name
   ```

2. **Make your changes following the coding standards**

3. **Write tests for your changes**

4. **Run the test suite:**

   ```bash
   cmake --build . --target test
   ```

5. **Submit a pull request**

## Coding Standards

### C++ Style Guide

We follow a modified version of the Google C++ Style Guide with these key points:

#### Naming Conventions

```cpp
// Classes: PascalCase
class MyComponent : public UIElement {
    // Public methods: camelCase
    MyComponent& setText(const QString& text);

    // Private members: snake_case with trailing underscore
    QString text_;
    std::unique_ptr<QWidget> widget_;

    // Constants: UPPER_SNAKE_CASE
    static constexpr int MAX_RETRY_COUNT = 3;
};

// Functions: camelCase
void processUserInput(const QString& input);

// Variables: snake_case
int retry_count = 0;
QString user_name;

// Namespaces: PascalCase
namespace DeclarativeUI::Components {
    // ...
}
```

#### File Organization

```cpp
// MyComponent.hpp
#pragma once

// System includes first
#include <QWidget>
#include <memory>
#include <functional>

// Qt includes
#include <QVBoxLayout>
#include <QPushButton>

// DeclarativeUI includes
#include "Core/UIElement.hpp"

// Local includes
#include "MyModel.hpp"

namespace DeclarativeUI::Components {

/**
 * @brief Brief description of the component
 *
 * Detailed description explaining the purpose,
 * usage, and any important considerations.
 */
class MyComponent : public Core::UIElement {
    Q_OBJECT

public:
    explicit MyComponent(QObject* parent = nullptr);
    ~MyComponent() override = default;

    // Move-only semantics
    MyComponent(const MyComponent&) = delete;
    MyComponent& operator=(const MyComponent&) = delete;
    MyComponent(MyComponent&&) = default;
    MyComponent& operator=(MyComponent&&) = default;

    // Public interface
    MyComponent& setText(const QString& text);
    MyComponent& onClick(std::function<void()> handler);

    void initialize() override;

private:
    QString text_;
    std::function<void()> click_handler_;
};

}  // namespace DeclarativeUI::Components
```

#### Modern C++ Features

Use modern C++ features appropriately:

```cpp
// Use auto for type deduction when type is obvious
auto widget = std::make_unique<QWidget>();
auto button = create<QPushButton>();

// Use range-based for loops
for (const auto& [key, value] : properties_) {
    // Process key-value pairs
}

// Use structured bindings
if (auto [success, result] = tryOperation(); success) {
    // Handle successful result
}

// Use concepts for template constraints
template<QtWidget T>
requires std::is_constructible_v<T>
std::unique_ptr<T> createWidget() {
    return std::make_unique<T>();
}

// Use constexpr for compile-time computation
constexpr bool isValidPropertyType() {
    return std::is_arithmetic_v<T> || std::is_same_v<T, QString>;
}
```

### Documentation Standards

#### Class Documentation

```cpp
/**
 * @brief A customizable button component with enhanced features
 *
 * The EnhancedButton extends the basic button functionality with
 * additional features like loading states, confirmation dialogs,
 * and custom styling options.
 *
 * @example
 * auto button = std::make_unique<EnhancedButton>();
 * button->text("Save Document")
 *       .confirmationMessage("Are you sure you want to save?")
 *       .loadingText("Saving...")
 *       .onClick([this]() { saveDocument(); });
 *
 * @see Button, UIElement
 * @since 1.0.0
 */
class EnhancedButton : public Button {
    // Implementation...
};
```

#### Method Documentation

```cpp
/**
 * @brief Sets the confirmation message shown before action execution
 *
 * When set, clicking the button will show a confirmation dialog
 * with the specified message before executing the click handler.
 *
 * @param message The confirmation message to display
 * @return Reference to this button for method chaining
 *
 * @example
 * button->confirmationMessage("Delete this item?")
 *       .onClick([]() { deleteItem(); });
 */
EnhancedButton& confirmationMessage(const QString& message);
```

## Testing Guidelines

### Unit Testing

Write comprehensive unit tests for all new functionality:

```cpp
// test_my_component.cpp
#include <QtTest/QtTest>
#include "Components/MyComponent.hpp"

class TestMyComponent : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testInitialization();
    void testTextProperty();
    void testClickHandler();
    void testPropertyBinding();
    void testErrorHandling();

private:
    std::unique_ptr<MyComponent> component_;
};

void TestMyComponent::testTextProperty() {
    component_->setText("Test Text");
    component_->initialize();

    QCOMPARE(component_->getText(), QString("Test Text"));

    // Test property binding
    bool text_changed = false;
    component_->bindProperty("text", [&text_changed]() {
        text_changed = true;
        return QString("Bound Text");
    });

    component_->refresh();
    QVERIFY(text_changed);
    QCOMPARE(component_->getText(), QString("Bound Text"));
}

void TestMyComponent::testErrorHandling() {
    // Test exception safety
    QVERIFY_EXCEPTION_THROWN(
        component_->setText(QString()),
        std::invalid_argument
    );

    // Test graceful degradation
    component_->setText("Valid Text");
    QVERIFY(component_->isValid());
}

QTEST_MAIN(TestMyComponent)
#include "test_my_component.moc"
```

### Integration Testing

Test component interactions and JSON loading:

```cpp
class TestJSONIntegration : public QObject {
    Q_OBJECT

private slots:
    void testComponentCreation();
    void testPropertyBinding();
    void testEventHandling();

private:
    std::unique_ptr<JSON::JSONUILoader> loader_;
};

void TestJSONIntegration::testComponentCreation() {
    QString json = R"({
        "type": "MyComponent",
        "properties": {
            "text": "Test Component",
            "enabled": true
        },
        "events": {
            "clicked": "handleClick"
        }
    })";

    bool event_triggered = false;
    loader_->registerEventHandler("handleClick", [&event_triggered]() {
        event_triggered = true;
    });

    auto widget = loader_->loadFromString(json);
    QVERIFY(widget != nullptr);

    // Simulate click
    auto component = qobject_cast<MyComponent*>(widget.get());
    QVERIFY(component != nullptr);
    component->simulateClick();
    QVERIFY(event_triggered);
}
```

### Performance Testing

Include performance tests for critical paths:

```cpp
void TestPerformance::testLargeUICreation() {
    QElapsedTimer timer;
    timer.start();

    auto widget = Core::create<QWidget>()
        .layout<QVBoxLayout>();

    // Create 1000 components
    for (int i = 0; i < 1000; ++i) {
        widget.child<Components::Button>([i](auto& btn) {
            btn.text(QString("Button %1").arg(i));
        });
    }

    auto result = widget.build();
    qint64 elapsed = timer.elapsed();

    QVERIFY(result != nullptr);
    QVERIFY(elapsed < 1000); // Should complete within 1 second

    qDebug() << "Created 1000 components in" << elapsed << "ms";
}
```

## Adding New Features

### Adding a New Component

1. **Create the component files:**

   ```
   src/Components/MyNewComponent.hpp
   src/Components/MyNewComponent.cpp
   ```

2. **Implement the component:**

   ```cpp
   // MyNewComponent.hpp
   class MyNewComponent : public Core::UIElement {
       Q_OBJECT

   public:
       explicit MyNewComponent(QObject* parent = nullptr);

       // Fluent interface
       MyNewComponent& property1(const QString& value);
       MyNewComponent& property2(int value);
       MyNewComponent& onEvent(std::function<void()> handler);

       void initialize() override;

   private:
       QString property1_;
       int property2_ = 0;
       std::function<void()> event_handler_;
   };
   ```

3. **Add to CMakeLists.txt:**

   ```cmake
   # In src/Components/CMakeLists.txt
   add_library(Components
       # ... existing files ...
       MyNewComponent.hpp
       MyNewComponent.cpp
   )
   ```

4. **Register with ComponentRegistry:**

   ```cpp
   // In ComponentRegistry.cpp
   void ComponentRegistry::registerBuiltinComponents() {
       // ... existing registrations ...

       registerComponent<MyNewComponent>("MyNewComponent",
           [](const QJsonObject& config) {
               auto component = std::make_unique<MyNewComponent>();
               // Apply configuration from JSON
               return component;
           });
   }
   ```

5. **Write tests:**

   ```cpp
   // tests/test_my_new_component.cpp
   class TestMyNewComponent : public QObject {
       // Test implementation
   };
   ```

6. **Add documentation:**

   ```cpp
   /**
    * @brief Brief description of MyNewComponent
    *
    * Detailed description and usage examples.
    */
   ```

### Adding a New Module

1. **Create module directory structure:**

   ```
   src/MyModule/
   ├── CMakeLists.txt
   ├── MyModuleClass.hpp
   ├── MyModuleClass.cpp
   └── README.md
   ```

2. **Create CMakeLists.txt:**

   ```cmake
   add_library(MyModule
       MyModuleClass.hpp
       MyModuleClass.cpp
   )

   target_link_libraries(MyModule
       Qt6::Core
       Qt6::Widgets
   )

   target_include_directories(MyModule PUBLIC
       ${CMAKE_CURRENT_SOURCE_DIR}
   )
   ```

3. **Add to main CMakeLists.txt:**

   ```cmake
   add_subdirectory(src/MyModule)

   target_link_libraries(DeclarativeUI
       # ... existing libraries ...
       MyModule
   )
   ```

## Pull Request Process

### Before Submitting

1. **Ensure all tests pass:**

   ```bash
   cmake --build . --target test
   ```

2. **Run static analysis (if available):**

   ```bash
   clang-tidy src/**/*.cpp
   ```

3. **Check code formatting:**

   ```bash
   clang-format -i src/**/*.{hpp,cpp}
   ```

4. **Update documentation if needed**

5. **Add changelog entry**

### Pull Request Template

```markdown
## Description

Brief description of the changes made.

## Type of Change

- [ ] Bug fix (non-breaking change which fixes an issue)
- [ ] New feature (non-breaking change which adds functionality)
- [ ] Breaking change (fix or feature that would cause existing functionality to not work as expected)
- [ ] Documentation update

## Testing

- [ ] Unit tests added/updated
- [ ] Integration tests added/updated
- [ ] Manual testing performed

## Checklist

- [ ] Code follows the project's coding standards
- [ ] Self-review of code completed
- [ ] Code is commented, particularly in hard-to-understand areas
- [ ] Corresponding changes to documentation made
- [ ] No new warnings introduced
- [ ] All tests pass locally

## Related Issues

Fixes #(issue number)
```

### Review Process

1. **Automated checks must pass**
2. **At least one maintainer review required**
3. **All feedback addressed**
4. **Documentation updated if needed**
5. **Changelog updated**

## Release Process

### Version Numbering

We use Semantic Versioning (SemVer):

- **MAJOR**: Incompatible API changes
- **MINOR**: Backwards-compatible functionality additions
- **PATCH**: Backwards-compatible bug fixes

### Release Checklist

1. **Update version numbers**
2. **Update CHANGELOG.md**
3. **Run full test suite**
4. **Build release packages**
5. **Tag release in Git**
6. **Update documentation**
7. **Announce release**

## Getting Help

- **Documentation**: Check the [API documentation](../api/README.md) and [user guide](../user-guide/getting-started.md)
- **Issues**: Search existing issues or create a new one
- **Discussions**: Use GitHub Discussions for questions and ideas
- **Code Review**: Maintainers will provide feedback on pull requests

Thank you for contributing to DeclarativeUI!
