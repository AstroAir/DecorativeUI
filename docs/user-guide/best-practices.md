# DeclarativeUI Best Practices

This guide outlines recommended practices for developing applications with DeclarativeUI to ensure maintainable, performant, and robust code.

## Code Organization

### Project Structure

Organize your DeclarativeUI project with a clear structure:

```
MyApp/
├── src/
│   ├── main.cpp
│   ├── components/          # Custom components
│   │   ├── SearchBox.hpp
│   │   └── SearchBox.cpp
│   ├── views/              # Application views/windows
│   │   ├── MainWindow.hpp
│   │   └── MainWindow.cpp
│   ├── models/             # Data models and state
│   │   ├── AppState.hpp
│   │   └── AppState.cpp
│   └── utils/              # Utility functions
├── ui/                     # JSON UI definitions
│   ├── main_window.json
│   ├── dialogs/
│   └── components/
├── resources/              # Images, icons, etc.
└── CMakeLists.txt
```

### Header Organization

```cpp
// MyComponent.hpp
#pragma once

// System includes first
#include <QWidget>
#include <memory>

// DeclarativeUI includes
#include "Core/UIElement.hpp"
#include "Components/Button.hpp"

// Local includes last
#include "MyModel.hpp"

namespace MyApp {
    class MyComponent : public DeclarativeUI::Core::UIElement {
        // Implementation...
    };
}
```

## Component Design

### Fluent Interface Design

Design components with fluent interfaces for better usability:

```cpp
class CustomButton : public Core::UIElement {
public:
    // Return reference for chaining
    CustomButton& text(const QString& text) {
        text_ = text;
        return *this;
    }
    
    CustomButton& variant(ButtonVariant variant) {
        variant_ = variant;
        return *this;
    }
    
    CustomButton& size(ButtonSize size) {
        size_ = size;
        return *this;
    }
    
    // Event handlers
    CustomButton& onClick(std::function<void()> handler) {
        click_handler_ = std::move(handler);
        return *this;
    }
};

// Usage
auto button = std::make_unique<CustomButton>();
button->text("Save")
      .variant(ButtonVariant::Primary)
      .size(ButtonSize::Large)
      .onClick([]() { /* handle click */ });
```

### Component Composition

Prefer composition over inheritance for complex components:

```cpp
class FormField : public Core::UIElement {
public:
    FormField& label(const QString& text) {
        label_text_ = text;
        return *this;
    }
    
    FormField& required(bool required = true) {
        is_required_ = required;
        return *this;
    }
    
    FormField& input(std::unique_ptr<Core::UIElement> input) {
        input_element_ = std::move(input);
        return *this;
    }
    
    void initialize() override {
        auto container = new QWidget();
        auto layout = new QVBoxLayout(container);
        
        // Create label
        auto label = new QLabel(label_text_);
        if (is_required_) {
            label->setText(label_text_ + " *");
            label->setStyleSheet("QLabel { color: red; }");
        }
        layout->addWidget(label);
        
        // Add input element
        if (input_element_) {
            input_element_->initialize();
            layout->addWidget(input_element_->getWidget());
        }
        
        setWidget(container);
    }
    
private:
    QString label_text_;
    bool is_required_ = false;
    std::unique_ptr<Core::UIElement> input_element_;
};
```

## State Management

### Centralized State

Use StateManager for application-wide state:

```cpp
class AppState {
public:
    static void initialize() {
        auto& state = StateManager::instance();
        
        // Initialize application state
        state.setState("user", QVariantMap{});
        state.setState("theme", QString("light"));
        state.setState("language", QString("en"));
        
        // Set up computed states
        setupComputedStates();
        
        // Enable history for user actions
        state.enableHistory("user", 50);
    }
    
    static void setupComputedStates() {
        auto& state = StateManager::instance();
        
        // Computed state for user display name
        state.observeState<QVariantMap>("user", [&state](const QVariantMap& user) {
            QString displayName = user.value("firstName").toString() + " " + 
                                 user.value("lastName").toString();
            state.setState("userDisplayName", displayName.trimmed());
        });
    }
    
    // Convenience methods
    static void setUser(const QVariantMap& user) {
        StateManager::instance().setState("user", user);
    }
    
    static QVariantMap getUser() {
        auto user = StateManager::instance().getState<QVariantMap>("user");
        return user ? user->get() : QVariantMap{};
    }
};
```

### Local Component State

For component-specific state, use local state management:

```cpp
class CounterComponent : public Core::UIElement {
public:
    CounterComponent() {
        // Initialize local state
        count_ = 0;
        min_value_ = 0;
        max_value_ = 100;
    }
    
    CounterComponent& range(int min, int max) {
        min_value_ = min;
        max_value_ = max;
        return *this;
    }
    
    CounterComponent& onValueChanged(std::function<void(int)> handler) {
        value_changed_handler_ = std::move(handler);
        return *this;
    }
    
private:
    void updateValue(int delta) {
        int new_value = qBound(min_value_, count_ + delta, max_value_);
        if (new_value != count_) {
            count_ = new_value;
            updateDisplay();
            if (value_changed_handler_) {
                value_changed_handler_(count_);
            }
        }
    }
    
    void updateDisplay() {
        if (display_label_) {
            display_label_->setText(QString::number(count_));
        }
    }
    
private:
    int count_;
    int min_value_;
    int max_value_;
    QLabel* display_label_ = nullptr;
    std::function<void(int)> value_changed_handler_;
};
```

## Performance Optimization

### Lazy Initialization

Initialize components only when needed:

```cpp
class TabContainer : public Core::UIElement {
public:
    TabContainer& addTab(const QString& title, 
                        std::function<std::unique_ptr<Core::UIElement>()> factory) {
        tab_factories_[title] = std::move(factory);
        return *this;
    }
    
    void initialize() override {
        tab_widget_ = new QTabWidget();
        
        // Create placeholder tabs
        for (const auto& [title, factory] : tab_factories_) {
            auto placeholder = new QWidget();
            tab_widget_->addTab(placeholder, title);
        }
        
        // Load content on demand
        connect(tab_widget_, &QTabWidget::currentChanged,
                this, &TabContainer::loadTabContent);
        
        setWidget(tab_widget_);
    }
    
private:
    void loadTabContent(int index) {
        QString title = tab_widget_->tabText(index);
        
        // Check if already loaded
        if (loaded_tabs_.contains(title)) {
            return;
        }
        
        // Create and load content
        auto factory = tab_factories_[title];
        if (factory) {
            auto content = factory();
            content->initialize();
            
            tab_widget_->removeTab(index);
            tab_widget_->insertTab(index, content->getWidget(), title);
            tab_widget_->setCurrentIndex(index);
            
            loaded_tabs_.insert(title);
            tab_content_[title] = std::move(content);
        }
    }
    
private:
    QTabWidget* tab_widget_ = nullptr;
    std::unordered_map<QString, std::function<std::unique_ptr<Core::UIElement>()>> tab_factories_;
    std::unordered_map<QString, std::unique_ptr<Core::UIElement>> tab_content_;
    QSet<QString> loaded_tabs_;
};
```

### Batch State Updates

Use batch updates for multiple state changes:

```cpp
void updateUserProfile(const QString& firstName, const QString& lastName, 
                      const QString& email) {
    StateManager::instance().batchUpdate([&]() {
        auto& state = StateManager::instance();
        
        QVariantMap user = state.getState<QVariantMap>("user")->get();
        user["firstName"] = firstName;
        user["lastName"] = lastName;
        user["email"] = email;
        user["lastModified"] = QDateTime::currentDateTime();
        
        state.setState("user", user);
        state.setState("profileModified", true);
    });
}
```

### Efficient Property Binding

Use efficient binding patterns:

```cpp
// Good: Bind to specific state keys
label->bindProperty("text", []() {
    auto name = StateManager::instance().getState<QString>("userName");
    return name ? name->get() : QString("Unknown");
});

// Avoid: Expensive computations in bindings
label->bindProperty("text", []() {
    // Don't do heavy computation here
    return performExpensiveCalculation();
});

// Better: Use computed state
StateManager::instance().observeState<QString>("rawData", [](const QString& data) {
    QString processed = performExpensiveCalculation(data);
    StateManager::instance().setState("processedData", processed);
});

label->bindProperty("text", []() {
    auto processed = StateManager::instance().getState<QString>("processedData");
    return processed ? processed->get() : QString();
});
```

## Error Handling

### Exception Safety

Ensure exception safety in component initialization:

```cpp
class SafeComponent : public Core::UIElement {
public:
    void initialize() override {
        try {
            // Create widgets
            auto container = std::make_unique<QWidget>();
            auto layout = std::make_unique<QVBoxLayout>();
            
            // Configure widgets
            setupWidgets(container.get(), layout.get());
            
            // Only set widget if everything succeeded
            container->setLayout(layout.release());
            setWidget(container.release());
            
        } catch (const std::exception& e) {
            qCritical() << "Failed to initialize component:" << e.what();
            
            // Create fallback UI
            auto fallback = new QLabel("Component failed to load");
            fallback->setStyleSheet("QLabel { color: red; }");
            setWidget(fallback);
        }
    }
    
private:
    void setupWidgets(QWidget* container, QLayout* layout) {
        // Widget setup that might throw
        if (!validateConfiguration()) {
            throw std::runtime_error("Invalid configuration");
        }
        
        // Continue setup...
    }
    
    bool validateConfiguration() const {
        // Validation logic
        return true;
    }
};
```

### Graceful Degradation

Handle missing resources gracefully:

```cpp
class ImageComponent : public Core::UIElement {
public:
    ImageComponent& source(const QString& path) {
        image_path_ = path;
        return *this;
    }
    
    void initialize() override {
        auto label = new QLabel();
        
        QPixmap pixmap(image_path_);
        if (pixmap.isNull()) {
            // Fallback to placeholder
            label->setText("🖼️ Image not found");
            label->setAlignment(Qt::AlignCenter);
            label->setStyleSheet("QLabel { color: gray; border: 1px dashed gray; }");
        } else {
            label->setPixmap(pixmap.scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
        
        setWidget(label);
    }
    
private:
    QString image_path_;
};
```

## Testing

### Component Testing

Write testable components:

```cpp
class TestableButton : public Core::UIElement {
public:
    // Expose internal state for testing
    bool isInitialized() const { return button_ != nullptr; }
    QString getCurrentText() const { 
        return button_ ? button_->text() : QString(); 
    }
    
    // Allow injection of dependencies for testing
    TestableButton& setClickHandler(std::function<void()> handler) {
        click_handler_ = std::move(handler);
        return *this;
    }
    
    // Trigger events programmatically for testing
    void simulateClick() {
        if (click_handler_) {
            click_handler_();
        }
    }
    
private:
    QPushButton* button_ = nullptr;
    std::function<void()> click_handler_;
};

// Test
void testButtonClick() {
    bool clicked = false;
    
    auto button = std::make_unique<TestableButton>();
    button->setClickHandler([&clicked]() { clicked = true; });
    button->initialize();
    
    button->simulateClick();
    
    assert(clicked);
}
```

### State Testing

Test state management separately:

```cpp
void testUserState() {
    // Clear state for testing
    StateManager::instance().clearState();
    
    // Set up test state
    QVariantMap testUser{
        {"firstName", "John"},
        {"lastName", "Doe"},
        {"email", "john@example.com"}
    };
    
    StateManager::instance().setState("user", testUser);
    
    // Test computed state
    auto displayName = StateManager::instance().getState<QString>("userDisplayName");
    assert(displayName && displayName->get() == "John Doe");
    
    // Test state updates
    testUser["firstName"] = "Jane";
    StateManager::instance().setState("user", testUser);
    
    displayName = StateManager::instance().getState<QString>("userDisplayName");
    assert(displayName && displayName->get() == "Jane Doe");
}
```

## Documentation

### Component Documentation

Document component APIs clearly:

```cpp
/**
 * @brief A customizable search input component
 * 
 * SearchBox provides a text input with an integrated search button.
 * It supports placeholder text, search callbacks, and custom styling.
 * 
 * @example
 * auto search = std::make_unique<SearchBox>();
 * search->placeholder("Search products...")
 *       .onSearch([](const QString& query) {
 *           performSearch(query);
 *       })
 *       .style("SearchBox { border-radius: 5px; }");
 * search->initialize();
 */
class SearchBox : public Core::UIElement {
public:
    /**
     * @brief Sets the placeholder text
     * @param text Placeholder text to display when empty
     * @return Reference to this SearchBox for method chaining
     */
    SearchBox& placeholder(const QString& text);
    
    /**
     * @brief Sets the search callback
     * @param handler Function called when search is triggered
     * @return Reference to this SearchBox for method chaining
     */
    SearchBox& onSearch(std::function<void(const QString&)> handler);
};
```

### JSON Schema Documentation

Document JSON UI schemas:

```json
{
    "$schema": "http://json-schema.org/draft-07/schema#",
    "title": "DeclarativeUI Component Schema",
    "type": "object",
    "required": ["type"],
    "properties": {
        "type": {
            "type": "string",
            "description": "Widget type (e.g., 'QPushButton', 'QLabel')"
        },
        "id": {
            "type": "string",
            "description": "Optional unique identifier for the component"
        },
        "properties": {
            "type": "object",
            "description": "Widget properties (text, size, etc.)"
        },
        "events": {
            "type": "object",
            "description": "Event handler mappings"
        },
        "bindings": {
            "type": "object",
            "description": "Property bindings to state"
        },
        "children": {
            "type": "array",
            "description": "Child components",
            "items": { "$ref": "#" }
        }
    }
}
```

## Deployment

### Resource Management

Organize resources properly for deployment:

```cpp
// Use Qt resource system for bundled resources
class ResourceManager {
public:
    static QString getUIDefinition(const QString& name) {
        QFile file(QString(":/ui/%1.json").arg(name));
        if (file.open(QIODevice::ReadOnly)) {
            return file.readAll();
        }
        
        // Fallback to file system in development
        QFile devFile(QString("ui/%1.json").arg(name));
        if (devFile.open(QIODevice::ReadOnly)) {
            return devFile.readAll();
        }
        
        return QString();
    }
    
    static QPixmap getIcon(const QString& name) {
        return QPixmap(QString(":/icons/%1.png").arg(name));
    }
};
```

### Configuration Management

Handle different deployment environments:

```cpp
class Config {
public:
    static bool isDevelopment() {
        #ifdef QT_DEBUG
            return true;
        #else
            return QCoreApplication::arguments().contains("--dev");
        #endif
    }
    
    static bool isHotReloadEnabled() {
        return isDevelopment() && 
               QCoreApplication::arguments().contains("--hot-reload");
    }
    
    static QString getUIPath() {
        if (isDevelopment()) {
            return "ui/";
        } else {
            return ":/ui/";
        }
    }
};
```

Following these best practices will help you build maintainable, performant, and robust DeclarativeUI applications.
