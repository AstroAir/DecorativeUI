# Core API Reference

The Core module provides the fundamental building blocks for the DeclarativeUI framework, including the base `UIElement` class and the `DeclarativeBuilder` template for fluent UI construction.

## UIElement Class

The `UIElement` class is the foundation of all UI components in the DeclarativeUI framework. It provides property management, event handling, animation support, and lifecycle management.

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
        // Constructor and destructor
        explicit UIElement(QObject *parent = nullptr);
        virtual ~UIElement() = default;
        
        // Move semantics (copy disabled)
        UIElement(const UIElement &) = delete;
        UIElement &operator=(const UIElement &) = delete;
        UIElement(UIElement &&) = default;
        UIElement &operator=(UIElement &&) = default;
        
        // Core API methods...
    };
}
```

### Public Methods

#### Property Management

##### `setProperty<T>(const QString &name, T &&value) -> UIElement &`
Sets a property value with perfect forwarding for optimal performance.

**Parameters:**
- `name`: Property name
- `value`: Property value (forwarded)

**Returns:** Reference to this UIElement for method chaining

**Example:**
```cpp
element.setProperty("text", "Hello World")
       .setProperty("enabled", true)
       .setProperty("minimumSize", QSize(200, 100));
```

##### `getProperty(const QString &name) const -> PropertyValue`
Retrieves a property value safely.

**Parameters:**
- `name`: Property name

**Returns:** PropertyValue containing the property value

**Throws:** May throw if property doesn't exist

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

The `DeclarativeBuilder` is a template class that provides a fluent interface for building UI widgets declaratively.

### Header
```cpp
#include "Core/DeclarativeBuilder.hpp"
```

### Class Declaration
```cpp
template <QtWidget WidgetType>
class DeclarativeBuilder {
public:
    explicit DeclarativeBuilder();
    ~DeclarativeBuilder() = default;
    
    // Fluent interface methods...
};
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
