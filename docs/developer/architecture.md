# DeclarativeUI Architecture

This document provides an in-depth look at the DeclarativeUI framework architecture, design decisions, and implementation details.

## Overview

DeclarativeUI is built on a modular architecture that separates concerns and promotes code reusability. The framework follows modern C++ design principles and leverages Qt6's capabilities while providing a declarative programming model.

## Core Architecture

### Module Structure

```
DeclarativeUI/
├── Core/                   # Foundation classes
│   ├── UIElement          # Base UI element with property system
│   └── DeclarativeBuilder # Template-based builder pattern
├── JSON/                   # JSON support and validation
│   ├── JSONUILoader       # Load UIs from JSON definitions
│   ├── JSONParser         # JSON parsing utilities
│   ├── JSONValidator      # Schema validation
│   └── ComponentRegistry  # Component type registration
├── HotReload/             # Hot reload functionality
│   ├── FileWatcher        # File system monitoring
│   ├── HotReloadManager   # Reload coordination
│   └── PerformanceMonitor # Performance tracking
├── Binding/               # Property binding system
│   ├── StateManager       # Global state management
│   └── PropertyBinding    # Property binding implementation
├── Command/               # Command system
│   ├── CommandSystem      # Command execution engine
│   ├── BuiltinCommands    # Standard command implementations
│   └── CommandIntegration # UI integration helpers
└── Components/            # UI component library
    ├── Input/             # Input components
    ├── Display/           # Display components
    ├── Container/         # Layout and container components
    └── Advanced/          # Advanced components
```

### Design Principles

#### 1. Declarative Programming Model

The framework emphasizes describing _what_ the UI should look like rather than _how_ to build it:

```cpp
// Declarative approach
auto widget = create<QWidget>()
    .property("windowTitle", "My App")
    .layout<QVBoxLayout>()
    .child<Button>([](auto &btn) {
        btn.text("Save").onClick([]() { /* save logic */ });
    })
    .build();

// vs. Imperative approach
auto widget = new QWidget();
widget->setWindowTitle("My App");
auto layout = new QVBoxLayout(widget);
auto button = new QPushButton("Save");
connect(button, &QPushButton::clicked, []() { /* save logic */ });
layout->addWidget(button);
```

#### 2. RAII and Exception Safety

All components use RAII for automatic resource management:

```cpp
class UIElement {
private:
    std::unique_ptr<QWidget> widget_;  // Automatic cleanup
    std::unordered_map<QString, std::unique_ptr<QPropertyAnimation>> animations_;

public:
    ~UIElement() = default;  // Automatic cleanup via smart pointers
};
```

#### 3. Type Safety with C++20 Concepts

The framework uses concepts to ensure type safety at compile time:

```cpp
template<typename T>
concept QtWidget = std::is_base_of_v<QWidget, T> &&
                   std::is_constructible_v<T>;

template<QtWidget WidgetType>
class DeclarativeBuilder {
    // Only accepts valid Qt widget types
};
```

#### 4. Fluent Interface Design

All components provide fluent interfaces for method chaining:

```cpp
button.text("Click Me")
      .icon(QIcon(":/icon.png"))
      .onClick([]() { /* handler */ })
      .style("QPushButton { background: blue; }");
```

## Core Module

### UIElement Class

The `UIElement` class is the foundation of all UI components:

```cpp
class UIElement : public QObject {
    Q_OBJECT

protected:
    // Property system
    std::unordered_map<QString, PropertyValue> properties_;
    std::unordered_map<QString, std::function<PropertyValue()>> bindings_;
    std::unordered_map<QString, std::function<void()>> event_handlers_;

    // Widget management
    std::unique_ptr<QWidget> widget_;

    // Animation system
    std::unordered_map<QString, std::unique_ptr<QPropertyAnimation>> animations_;

    // Performance monitoring
    PerformanceMetrics performance_metrics_;

public:
    // Fluent interface
    template<typename T>
    UIElement& setProperty(const QString& name, T&& value);

    UIElement& bindProperty(const QString& property,
                           const std::function<PropertyValue()>& binding);

    UIElement& onEvent(const QString& event,
                      const std::function<void()>& handler);

    // Lifecycle management
    virtual void initialize() = 0;
    virtual void cleanup() noexcept;
    virtual void refresh();
};
```

### DeclarativeBuilder Template

The builder pattern implementation uses CRTP for type safety:

```cpp
template<QtWidget WidgetType>
class DeclarativeBuilder {
private:
    std::unique_ptr<UIElement> element_;
    std::vector<std::function<void(WidgetType*)>> configurators_;
    std::unique_ptr<QLayout> layout_;
    std::vector<std::unique_ptr<QWidget>> children_;

public:
    template<typename T>
    DeclarativeBuilder& property(const QString& name, T&& value) {
        element_->setProperty(name, std::forward<T>(value));
        return *this;
    }

    template<QtWidget ChildType>
    DeclarativeBuilder& child(
        std::function<void(DeclarativeBuilder<ChildType>&)> config) {
        auto child_builder = DeclarativeBuilder<ChildType>{};
        config(child_builder);
        auto child_widget = child_builder.build();
        children_.push_back(std::move(child_widget));
        return *this;
    }

    std::unique_ptr<WidgetType> build() {
        auto widget = element_->createWidget<WidgetType>();
        applyConfiguration(widget.get());
        return widget;
    }
};
```

## JSON Module

### Component Registration System

The JSON module uses a factory pattern for component creation:

```cpp
class ComponentRegistry {
private:
    std::unordered_map<QString, std::unique_ptr<IComponentFactory>> factories_;

public:
    template<typename WidgetType>
    void registerComponent(
        const QString& type_name,
        std::function<std::unique_ptr<WidgetType>(const QJsonObject&)> factory) {

        auto component_factory = std::make_unique<ComponentFactoryImpl<WidgetType>>(
            std::move(factory));
        factories_[type_name] = std::move(component_factory);
    }

    std::unique_ptr<QWidget> createComponent(
        const QString& type_name,
        const QJsonObject& config) {

        auto it = factories_.find(type_name);
        if (it != factories_.end()) {
            return it->second->create(config);
        }
        return nullptr;
    }
};
```

### JSON Validation Pipeline

The validation system uses a multi-stage approach:

```cpp
class UIJSONValidator {
private:
    struct ValidationContext {
        QJsonObject root_object;
        JSONPath current_path;
        std::vector<ValidationResult> results;
        bool strict_mode = false;
        int current_depth = 0;
        int max_validation_depth = 100;
    };

public:
    bool validate(const QJsonObject& ui_definition) {
        ValidationContext context;
        context.root_object = ui_definition;

        // Stage 1: Basic structure validation
        if (!validateBasicStructure(ui_definition, context)) {
            return false;
        }

        // Stage 2: Component-specific validation
        if (!validateComponentStructure(ui_definition, context.current_path)) {
            return false;
        }

        // Stage 3: Property validation
        if (!validateProperties(ui_definition, context)) {
            return false;
        }

        // Stage 4: Cross-reference validation
        if (!validateReferences(ui_definition, context)) {
            return false;
        }

        return !context.hasErrors();
    }
};
```

## Hot Reload Module

### File Monitoring Architecture

The hot reload system uses a layered approach:

```cpp
class HotReloadManager {
private:
    struct UIFileInfo {
        QWidget* target_widget;
        QWidget* parent_widget;
        QDateTime last_reload;
        QString backup_content;
    };

    std::unique_ptr<FileWatcher> file_watcher_;
    std::unique_ptr<JSON::JSONUILoader> ui_loader_;
    std::unordered_map<QString, UIFileInfo> registered_files_;

public:
    void registerUIFile(const QString& file_path, QWidget* target_widget) {
        // Create backup
        createBackup(file_path);

        // Register with file watcher
        file_watcher_->watchFile(file_path);

        // Store file info
        UIFileInfo& info = registered_files_[file_path];
        info.target_widget = target_widget;
        info.parent_widget = target_widget->parentWidget();
        info.last_reload = QDateTime::currentDateTime();
    }

private:
    void performReload(const QString& file_path) {
        emit reloadStarted(file_path);

        try {
            // Load new UI
            auto new_widget = ui_loader_->loadFromFile(file_path);

            // Validate new widget
            if (!validateWidget(new_widget.get())) {
                throw std::runtime_error("Widget validation failed");
            }

            // Replace widget
            replaceWidget(file_path, std::move(new_widget));

            emit reloadCompleted(file_path);

        } catch (const std::exception& e) {
            // Restore backup on failure
            restoreBackup(file_path);
            emit reloadFailed(file_path, QString::fromStdString(e.what()));
        }
    }
};
```

### Performance Monitoring

The performance monitor tracks reload metrics:

```cpp
struct PerformanceMetrics {
    QString file_path;
    qint64 total_time_ms;
    qint64 parse_time_ms;
    qint64 validation_time_ms;
    qint64 creation_time_ms;
    qint64 replacement_time_ms;
    QDateTime timestamp;
    bool success;
    QString error_message;
};

class PerformanceMonitor {
private:
    std::deque<PerformanceMetrics> metrics_history_;
    std::unordered_map<QString, std::deque<PerformanceMetrics>> file_metrics_;
    std::unordered_map<QString, QElapsedTimer> active_operations_;

public:
    void startOperation(const QString& operation_name) {
        active_operations_[operation_name].start();
    }

    qint64 endOperation(const QString& operation_name) {
        auto it = active_operations_.find(operation_name);
        if (it != active_operations_.end()) {
            qint64 elapsed = it->second.elapsed();
            active_operations_.erase(it);
            return elapsed;
        }
        return 0;
    }

    void recordReloadMetrics(const QString& file_path,
                           const PerformanceMetrics& metrics) {
        metrics_history_.push_back(metrics);
        file_metrics_[file_path].push_back(metrics);

        // Check performance thresholds
        if (metrics.total_time_ms > warning_threshold_ms_) {
            emit performanceWarning(file_path, metrics.total_time_ms);
        }

        pruneHistory();
    }
};
```

## Binding Module

### State Management Architecture

The state manager uses a reactive programming model:

```cpp
class StateManager {
private:
    struct StateInfo {
        std::shared_ptr<QObject> state;
        std::function<bool(const QVariant&)> validator;
        std::deque<QVariant> history;
        int history_position = 0;
        bool history_enabled = false;
        qint64 last_update_time = 0;
    };

    std::unordered_map<QString, StateInfo> states_;
    std::unordered_map<QString, std::vector<QString>> dependencies_;
    std::unordered_map<QString, std::vector<QString>> dependents_;

    bool batching_ = false;
    std::vector<std::function<void()>> pending_updates_;

public:
    template<typename T>
    void setState(const QString& key, T&& value) {
        auto& info = states_[key];

        // Validate if validator exists
        if (info.validator && !info.validator(QVariant::fromValue(value))) {
            throw std::invalid_argument("State validation failed");
        }

        // Update history
        if (info.history_enabled) {
            addToHistory(key, QVariant::fromValue(value));
        }

        // Update state
        if (!info.state) {
            info.state = std::make_shared<ReactiveProperty<T>>(std::forward<T>(value));
        } else {
            auto reactive_prop = std::static_pointer_cast<ReactiveProperty<T>>(info.state);
            reactive_prop->set(std::forward<T>(value));
        }

        // Update dependents
        if (!batching_) {
            updateDependents(key);
        } else {
            pending_updates_.push_back([this, key]() { updateDependents(key); });
        }

        emit stateChanged(key, QVariant::fromValue(value));
    }

    void batchUpdate(std::function<void()> updates) {
        if (batching_) {
            updates();
            return;
        }

        batching_ = true;
        updates();

        // Process pending updates
        for (auto& update : pending_updates_) {
            update();
        }
        pending_updates_.clear();

        batching_ = false;
    }
};
```

### Property Binding Implementation

Property bindings use Qt's meta-object system:

```cpp
template<typename SourceType, typename TargetType>
class PropertyBinding : public IPropertyBinding {
private:
    QPointer<QObject> source_object_;
    QPointer<QObject> target_object_;
    QString source_property_;
    QString target_property_;
    BindingDirection direction_;

    std::function<TargetType(const SourceType&)> converter_;
    std::function<bool(const TargetType&)> validator_;

public:
    PropertyBinding(QObject* source, const QString& source_prop,
                   QObject* target, const QString& target_prop,
                   BindingDirection direction)
        : source_object_(source), target_object_(target),
          source_property_(source_prop), target_property_(target_prop),
          direction_(direction) {

        connectSignals();
    }

    void update() override {
        if (!isValid()) return;

        // Get source value
        QVariant source_value = source_object_->property(source_property_.toUtf8());
        SourceType typed_source = source_value.value<SourceType>();

        // Convert if needed
        TargetType target_value = converter_ ?
            converter_(typed_source) :
            static_cast<TargetType>(typed_source);

        // Validate if needed
        if (validator_ && !validator_(target_value)) {
            return;
        }

        // Set target value
        target_object_->setProperty(target_property_.toUtf8(),
                                   QVariant::fromValue(target_value));
    }

private:
    void connectSignals() {
        // Connect property change signals
        auto source_meta = source_object_->metaObject();
        int prop_index = source_meta->indexOfProperty(source_property_.toUtf8());

        if (prop_index >= 0) {
            auto prop = source_meta->property(prop_index);
            if (prop.hasNotifySignal()) {
                auto notify_signal = prop.notifySignal();
                auto update_slot = metaObject()->method(
                    metaObject()->indexOfSlot("update()"));

                connect(source_object_, notify_signal, this, update_slot);
            }
        }

        // For bidirectional binding, connect reverse direction
        if (direction_ == BindingDirection::TwoWay) {
            // Similar setup for target -> source
        }
    }
};
```

## Command Module

### Command Execution Pipeline

The command system uses a pipeline architecture:

```cpp
class CommandInvoker {
private:
    struct ExecutionContext {
        QUuid command_id;
        QString command_name;
        CommandContext context;
        std::unique_ptr<ICommand> command;
        QElapsedTimer timer;
        ExecutionMode mode;
    };

    std::map<QUuid, ExecutionContext> running_commands_;
    std::queue<ExecutionContext> batch_queue_;
    QTimer* batch_timer_;

public:
    CommandResult<QVariant> executeInternal(const QString& command_name,
                                           const CommandContext& context,
                                           ExecutionMode mode) {
        // Stage 1: Create command
        auto command = CommandFactory::instance().createCommand(command_name, context);
        if (!command) {
            return CommandResult<QVariant>("Command not found: " + command_name);
        }

        // Stage 2: Validate execution
        if (!command->canExecute(context)) {
            return CommandResult<QVariant>("Command cannot be executed");
        }

        // Stage 3: Run interceptors
        for (auto& interceptor : interceptors_) {
            if (!interceptor->beforeExecution(command_name, context)) {
                return CommandResult<QVariant>("Command blocked by interceptor");
            }
        }

        // Stage 4: Execute command
        ExecutionContext exec_context;
        exec_context.command_id = QUuid::createUuid();
        exec_context.command_name = command_name;
        exec_context.context = context;
        exec_context.command = std::move(command);
        exec_context.mode = mode;
        exec_context.timer.start();

        auto result = exec_context.command->execute(context);

        // Stage 5: Post-execution processing
        qint64 elapsed = exec_context.timer.elapsed();

        for (auto& interceptor : interceptors_) {
            interceptor->afterExecution(command_name, context, result, elapsed);
        }

        return result;
    }
};
```

## Memory Management

### RAII Patterns

The framework uses consistent RAII patterns:

```cpp
class ComponentManager {
private:
    std::vector<std::unique_ptr<UIElement>> components_;
    std::unordered_map<QString, std::weak_ptr<UIElement>> named_components_;

public:
    template<typename T, typename... Args>
    std::shared_ptr<T> createComponent(const QString& name, Args&&... args) {
        auto component = std::make_shared<T>(std::forward<Args>(args)...);

        // Store in containers
        components_.push_back(component);
        if (!name.isEmpty()) {
            named_components_[name] = component;
        }

        return component;
    }

    ~ComponentManager() {
        // Automatic cleanup via smart pointers
        // Components are destroyed in reverse order
    }
};
```

### Exception Safety Guarantees

All operations provide strong exception safety:

```cpp
class SafeUIBuilder {
public:
    std::unique_ptr<QWidget> buildSafe() noexcept {
        try {
            return build();
        } catch (const std::exception& e) {
            qCritical() << "Build failed:" << e.what();
            return createFallbackWidget();
        } catch (...) {
            qCritical() << "Build failed with unknown exception";
            return createFallbackWidget();
        }
    }

private:
    std::unique_ptr<QWidget> createFallbackWidget() noexcept {
        try {
            auto widget = std::make_unique<QLabel>("Failed to load UI");
            widget->setStyleSheet("QLabel { color: red; }");
            return widget;
        } catch (...) {
            return nullptr;
        }
    }
};
```

## Threading Model

DeclarativeUI follows Qt's threading model:

- All UI operations must occur on the main thread
- State management is thread-safe for read operations
- Command execution can be asynchronous but UI updates are marshaled to main thread
- Hot reload operations are performed on the main thread

```cpp
class ThreadSafeStateManager {
private:
    mutable QReadWriteLock states_lock_;

public:
    template<typename T>
    std::shared_ptr<ReactiveProperty<T>> getState(const QString& key) const {
        QReadLocker locker(&states_lock_);
        auto it = states_.find(key);
        if (it != states_.end()) {
            return std::static_pointer_cast<ReactiveProperty<T>>(it->second.state);
        }
        return nullptr;
    }

    template<typename T>
    void setState(const QString& key, T&& value) {
        // Ensure UI updates happen on main thread
        if (QThread::currentThread() != QCoreApplication::instance()->thread()) {
            QMetaObject::invokeMethod(this, [this, key, value = std::forward<T>(value)]() {
                setStateInternal(key, std::move(value));
            }, Qt::QueuedConnection);
        } else {
            setStateInternal(key, std::forward<T>(value));
        }
    }
};
```

This architecture provides a solid foundation for building declarative UIs while maintaining performance, type safety, and extensibility.

## Performance Considerations

### Compile-Time Optimizations

The framework leverages C++20 features for compile-time optimizations:

```cpp
// Concept-based template specialization
template<QtWidget T>
constexpr bool is_layout_widget_v = std::is_base_of_v<QLayout, T>;

template<QtWidget T>
requires is_layout_widget_v<T>
class LayoutBuilder {
    // Specialized implementation for layout widgets
};

// Constexpr property validation
template<typename T>
constexpr bool is_valid_property_type() {
    return std::is_arithmetic_v<T> ||
           std::is_same_v<T, QString> ||
           std::is_same_v<T, QVariant>;
}
```

### Runtime Optimizations

- Property updates are batched to minimize redraws
- Component creation is lazy where possible
- State changes use efficient dependency tracking
- Hot reload operations are optimized for minimal disruption

### Memory Efficiency

- Smart pointers prevent memory leaks
- Weak references avoid circular dependencies
- Component pooling for frequently created/destroyed widgets
- Efficient property storage using variant types
