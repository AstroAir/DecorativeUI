# Core API Reference

The Core module provides the fundamental building blocks for the DeclarativeUI framework, including the base `UIElement` class, the `DeclarativeBuilder` template for fluent UI construction, and essential utilities for performance optimization and memory management.

## UIElement Class

The `UIElement` class is the foundation of all UI components in the DeclarativeUI framework. It provides comprehensive property management, event handling, animation support, styling capabilities, lifecycle management, and performance optimization with modern C++20 features.

### Header
```cpp
#include "Core/UIElement.hpp"
```

### Class Declaration
```cpp
namespace DeclarativeUI::Core {
    class UIElement : public QObject {
        Q_OBJECT

    public:
        explicit UIElement(QObject* parent = nullptr);
        virtual ~UIElement() = default;

        // Core functionality
        virtual void initialize();
        virtual void cleanup();

        // Property management
        template<typename T>
        UIElement& property(const QString& name, const T& value);

        template<typename T>
        T getProperty(const QString& name, const T& defaultValue = T{}) const;

        // Event handling
        template<typename Func>
        UIElement& onEvent(const QString& eventName, Func&& handler);

        // Styling and appearance
        UIElement& style(const QString& styleSheet);
        UIElement& addClass(const QString& className);
        UIElement& removeClass(const QString& className);

        // Performance and optimization
        PerformanceMetrics getPerformanceMetrics() const;
        void enableCaching(bool enabled = true);

    protected:
        virtual void onInitialize() {}
        virtual void onCleanup() {}

    private:
        class Impl;
        std::unique_ptr<Impl> d;
    };
}
```

### Class Declaration
```cpp
namespace DeclarativeUI::Core {
    class UIElement : public QObject {
        Q_OBJECT
    public:
        // Constructor and destructor with RAII
        explicit UIElement(QObject *parent = nullptr);
        virtual ~UIElement() = default;

        // Modern C++ move semantics (copy disabled for safety)
        UIElement(const UIElement &) = delete;
        UIElement &operator=(const UIElement &) = delete;
        UIElement(UIElement &&) = default;
        UIElement &operator=(UIElement &&) = default;

        // Core API methods...
    };
}
```

### Key Features

- **RAII Resource Management**: Automatic cleanup of widgets and resources
- **Type-Safe Properties**: Template-based property system with perfect forwarding
- **Fluent Interface**: Method chaining for declarative syntax
- **Exception Safety**: Strong exception safety guarantees throughout
- **Performance Optimized**: Batched property updates and efficient binding system
- **Modern C++20**: Concepts, perfect forwarding, and move semantics

### Public Methods

#### Property Management

The property system provides type-safe, efficient property management with automatic Qt widget synchronization.

##### `setProperty<T>(const QString &name, T &&value) -> UIElement &`
Sets a property value with perfect forwarding and automatic type conversion.

**Template Parameters:**
- `T`: Property value type (automatically deduced)

**Parameters:**
- `name`: Property name (Qt property name or custom property)
- `value`: Property value (perfect forwarded for optimal performance)

**Returns:** Reference to this UIElement for method chaining

**Features:**
- Perfect forwarding for optimal performance
- Automatic const char* to QString conversion
- Immediate Qt widget property synchronization
- Exception safety with detailed error messages

**Example:**
```cpp
element.setProperty("text", "Hello World")           // QString conversion
       .setProperty("enabled", true)                 // bool property
       .setProperty("minimumSize", QSize(200, 100))  // Complex type
       .setProperty("styleSheet", "color: blue;");   // CSS styling
```

##### `getProperty(const QString &name) const -> PropertyValue`
Retrieves a property value safely with exception handling.

**Parameters:**
- `name`: Property name to retrieve

**Returns:** PropertyValue containing the property value

**Throws:** `PropertyBindingException` if property doesn't exist

**Example:**
```cpp
try {
    auto text = element.getProperty("text");
    // Use PropertyValue variant
} catch (const PropertyBindingException& e) {
    qWarning() << "Property not found:" << e.what();
}
```

##### `getProperties() const -> const std::unordered_map<QString, PropertyValue> &`
Returns all stored properties for inspection.

**Returns:** Const reference to the properties map

**Use Cases:**
- Debugging property states
- Serialization/deserialization
- Property validation

#### Widget Creation

##### `createWidget<T>() -> std::unique_ptr<T>`
Creates a widget of the specified type with stored properties applied.

**Template Parameters:**
- `T`: Widget type (must satisfy QtWidget concept)

**Returns:** Unique pointer to the created widget

**Example:**
```cpp
auto button = element.createWidget<QPushButton>();
```

#### Property Binding

##### `bindProperty(const QString &property, const std::function<PropertyValue()> &binding) -> UIElement &`
Binds a property to a function that computes its value dynamically.

**Parameters:**
- `property`: Property name to bind
- `binding`: Function that returns the property value

**Returns:** Reference to this UIElement for method chaining

**Example:**
```cpp
element.bindProperty("text", []() { 
    return QString("Count: %1").arg(counter); 
});
```

#### Event Handling

##### `onEvent(const QString &event, const std::function<void()> &handler) -> UIElement &`
Registers an event handler for the specified event.

**Parameters:**
- `event`: Event name (e.g., "clicked", "textChanged")
- `handler`: Event handler function

**Returns:** Reference to this UIElement for method chaining

**Example:**
```cpp
element.onEvent("clicked", []() {
    qDebug() << "Button clicked!";
});
```

#### Animation

##### `animate(const QString &property, const QVariant &target_value, const AnimationConfig &config = {}) -> UIElement &`
Animates a property to a target value.

**Parameters:**
- `property`: Property name to animate
- `target_value`: Target value for the animation
- `config`: Animation configuration (duration, easing, etc.)

**Returns:** Reference to this UIElement for method chaining

#### Styling and Effects

##### `addClass(const QString &css_class) -> UIElement &`
Adds a CSS class for styling.

##### `removeClass(const QString &css_class) -> UIElement &`
Removes a CSS class.

##### `setTheme(const ThemeConfig &theme) -> UIElement &`
Applies a theme configuration.

##### `setShadow(const QColor &color, const QPointF &offset, qreal blur_radius) -> UIElement &`
Adds a drop shadow effect.

##### `setOpacity(qreal opacity, const AnimationConfig &config = {}) -> UIElement &`
Sets opacity with optional animation.

#### Responsive Design

##### `setResponsive(bool responsive = true) -> UIElement &`
Enables responsive design features.

##### `setBreakpoint(int width, std::function<void()> callback) -> UIElement &`
Sets a responsive breakpoint with callback.

#### Accessibility

##### `setAccessibleName(const QString &name) -> UIElement &`
Sets the accessible name for screen readers.

##### `setAccessibleDescription(const QString &description) -> UIElement &`
Sets the accessible description.

##### `setAccessibleRole(const QString &role) -> UIElement &`
Sets the accessible role.

#### Widget Lifecycle

##### `virtual void initialize() = 0`
Pure virtual method for widget initialization. Must be implemented by derived classes.

##### `virtual void cleanup() noexcept`
Virtual method for cleanup operations. Called during destruction.

##### `virtual void refresh()`
Refreshes the widget state and updates bound properties.

##### `virtual void invalidate()`
Marks the widget as needing refresh.

#### Widget Management

##### `setWidget(QWidget *widget)`
Sets the underlying Qt widget.

##### `getWidget() const noexcept -> QWidget *`
Gets the underlying Qt widget.

#### Performance Monitoring

##### `getPerformanceMetrics() const -> const PerformanceMetrics &`
Returns performance metrics for this element.

### Signals

```cpp
signals:
    void propertyUpdated(const QString &property);
    void styleChanged();
    void animationStarted(const QString &property);
    void animationFinished(const QString &property);
    void breakpointChanged(int width);
    void validated(bool success);
    void refreshed();
    void invalidated();
    void widgetResized();
```

### Protected Members

The UIElement class provides several protected members for derived classes:

- `properties_`: Property storage map
- `bindings_`: Property binding functions
- `event_handlers_`: Event handler functions
- `widget_`: Managed Qt widget
- `animations_`: Active animations
- `theme_`: Theme configuration

## DeclarativeBuilder Template

The `DeclarativeBuilder` is a template class that provides a fluent interface for building UI widgets declaratively with modern C++20 features, move semantics, and performance optimizations.

### Header
```cpp
#include "Core/DeclarativeBuilder.hpp"
```

### Class Declaration
```cpp
namespace DeclarativeUI::Core {

template <QtWidget WidgetType>
class DeclarativeBuilder {
public:
    explicit DeclarativeBuilder();
    ~DeclarativeBuilder() = default;

    // Move-only semantics for performance
    DeclarativeBuilder(const DeclarativeBuilder&) = delete;
    DeclarativeBuilder& operator=(const DeclarativeBuilder&) = delete;
    DeclarativeBuilder(DeclarativeBuilder&&) = default;
    DeclarativeBuilder& operator=(DeclarativeBuilder&&) = default;

    // Fluent property setting
    template <typename T>
    DeclarativeBuilder& property(const QString& name, T&& value);

    // Event handling
    DeclarativeBuilder& on(const QString& event, std::function<void()> handler);

    // Property binding
    DeclarativeBuilder& bind(const QString& property,
                             std::function<PropertyValue()> binding);

    // Child management
    template <typename ChildType>
    DeclarativeBuilder& child(
        std::function<void(DeclarativeBuilder<ChildType>&)> config);

    // Layout management
    template <typename LayoutType>
    DeclarativeBuilder& layout(
        std::function<void(LayoutType*)> config = nullptr);

    // Build final widget with RAII
    [[nodiscard]] std::unique_ptr<WidgetType> build();

    // Exception-safe building
    [[nodiscard]] std::unique_ptr<WidgetType> buildSafe() noexcept;

private:
    std::unique_ptr<UIElement> element_;
    std::vector<std::function<void(WidgetType*)>> configurators_;
    std::unique_ptr<QLayout> layout_;
    std::vector<std::unique_ptr<QWidget>> children_;

    // Performance optimization
    mutable bool is_built_ = false;
    mutable bool has_cached_widget_ = false;
    mutable std::unique_ptr<WidgetType> cached_widget_;

    void applyConfiguration(WidgetType* widget);
    void applyConfigurationBatch(WidgetType* widget);

    // Memory pool for frequent allocations
    static thread_local std::vector<std::unique_ptr<WidgetType>> widget_pool_;
};

// Factory functions for declarative syntax
template <typename T>
[[nodiscard]] DeclarativeBuilder<T> create() {
    static_assert(is_qt_widget_v<T>, "T must be a QWidget-derived type");
    return DeclarativeBuilder<T>{};
}

// Specialized builders for common widgets
[[nodiscard]] inline auto button() { return create<QPushButton>(); }
[[nodiscard]] inline auto label() { return create<QLabel>(); }
[[nodiscard]] inline auto widget() { return create<QWidget>(); }

}
```

### Public Methods

#### Property Configuration

##### `property<T>(const QString &name, T &&value) -> DeclarativeBuilder &`
Sets a property value with perfect forwarding.

**Example:**
```cpp
auto button = create<QPushButton>()
    .property("text", "Click Me")
    .property("enabled", true)
    .build();
```

#### Event Handling

##### `on(const QString &event, std::function<void()> handler) -> DeclarativeBuilder &`
Registers an event handler.

**Example:**
```cpp
auto button = create<QPushButton>()
    .on("clicked", []() { qDebug() << "Clicked!"; })
    .build();
```

#### Property Binding

##### `bind(const QString &property, std::function<PropertyValue()> binding) -> DeclarativeBuilder &`
Binds a property to a dynamic value.

**Example:**
```cpp
auto label = create<QLabel>()
    .bind("text", []() { return getCurrentTime(); })
    .build();
```

#### Child Management

##### `child<ChildType>(std::function<void(DeclarativeBuilder<ChildType> &)> config) -> DeclarativeBuilder &`
Adds a child widget with configuration.

**Example:**
```cpp
auto widget = create<QWidget>()
    .child<QPushButton>([](auto &btn) {
        btn.property("text", "Child Button");
    })
    .build();
```

#### Layout Management

##### `layout<LayoutType>(std::function<void(LayoutType *)> config = nullptr) -> DeclarativeBuilder &`
Sets the layout for the widget.

**Example:**
```cpp
auto widget = create<QWidget>()
    .layout<QVBoxLayout>([](auto *layout) {
        layout->setSpacing(10);
    })
    .build();
```

#### Building

##### `build() -> std::unique_ptr<WidgetType>`
Builds and returns the configured widget.

**Returns:** Unique pointer to the built widget

**Throws:** May throw on build errors

##### `buildSafe() noexcept -> std::unique_ptr<WidgetType>`
Builds the widget with exception safety.

**Returns:** Unique pointer to the built widget, or nullptr on error

### Factory Functions

#### `create<T>() -> DeclarativeBuilder<T>`
Creates a new DeclarativeBuilder for the specified widget type.

**Template Parameters:**
- `T`: Widget type (must satisfy QtWidget concept)

**Example:**
```cpp
auto button = create<QPushButton>()
    .property("text", "Hello")
    .build();
```

### Usage Examples

#### Basic Widget Creation
```cpp
using namespace DeclarativeUI::Core;

auto button = create<QPushButton>()
    .property("text", "Click Me")
    .property("minimumSize", QSize(100, 30))
    .on("clicked", []() {
        qDebug() << "Button clicked!";
    })
    .build();
```

#### Complex Layout
```cpp
auto mainWidget = create<QWidget>()
    .layout<QVBoxLayout>()
    .child<QLabel>([](auto &label) {
        label.property("text", "Welcome");
    })
    .child<QPushButton>([](auto &btn) {
        btn.property("text", "OK")
           .on("clicked", []() { /* handle click */ });
    })
    .build();
```

#### With Property Binding
```cpp
int counter = 0;
auto label = create<QLabel>()
    .bind("text", [&counter]() {
        return QString("Count: %1").arg(counter);
    })
    .build();
```

## Type Concepts

The Core module uses C++20 concepts for type safety:

### QtWidget Concept
```cpp
template<typename T>
concept QtWidget = std::is_base_of_v<QWidget, T> && 
                   std::is_constructible_v<T>;
```

This concept ensures that template parameters are valid Qt widget types.

## Exception Safety

All Core classes provide strong exception safety guarantees:
- RAII for resource management
- Exception-safe building with `buildSafe()`
- Automatic cleanup on destruction
- Safe property access methods

## Thread Safety

- UIElement instances are not thread-safe and should be used from the main thread
- Property updates are batched for performance
- Signal emissions are queued for thread safety
- DeclarativeBuilder uses thread-local memory pools for optimization

## Performance Optimizations

The Core module implements several performance optimizations:

### Memory Management
- **Move semantics**: Efficient transfer of resources without copying
- **RAII**: Automatic resource management with exception safety
- **Memory pooling**: Thread-local widget pools reduce allocation overhead
- **Small vector optimization**: Configurators use optimized storage

### Compilation Optimizations
- **Template specialization**: Compile-time optimizations for common widget types
- **Concept constraints**: Compile-time type checking with `QtWidget` concept
- **Perfect forwarding**: Zero-overhead property value forwarding

### Runtime Optimizations
- **Batch configuration**: Multiple properties applied in single pass
- **Cached widgets**: Optional widget caching for repeated builds
- **Lazy evaluation**: Configuration applied only during build phase

## Best Practices

### Design Patterns
1. **Use move semantics** when passing builders around to avoid copies
2. **Prefer `buildSafe()`** in production code for robust error handling
3. **Cache frequently used widgets** when building similar UIs repeatedly
4. **Use property binding** for reactive UIs that respond to state changes
5. **Leverage factory functions** (`button()`, `label()`, etc.) for common widget types

### Error Handling
```cpp
// Robust error handling
auto widget = create<QWidget>()
    .property("windowTitle", "My App")
    .buildSafe();

if (!widget) {
    qCritical() << "Failed to create main widget";
    return -1;
}
```

### Memory Efficiency
```cpp
// Efficient resource usage
{
    auto builder = create<QWidget>()
        .property("windowTitle", "Temporary");

    auto widget = std::move(builder).build();  // Move builder
    // builder is now in moved-from state
} // Automatic cleanup
```

### State Integration
```cpp
// Integrate with state management
#include "Binding/StateManager.hpp"

auto& state = StateManager::instance();
state.setState("theme", QString("dark"));

auto themedWidget = create<QWidget>()
    .bind("styleSheet", [&state]() {
        auto theme = state.getState<QString>("theme");
        return theme->get() == "dark" ?
            PropertyValue("background: #2b2b2b; color: white;") :
            PropertyValue("background: white; color: black;");
    })
    .build();
```
