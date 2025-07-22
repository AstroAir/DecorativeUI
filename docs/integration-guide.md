# DeclarativeUI Integration Guide

## Overview

This comprehensive guide explains how to integrate DeclarativeUI into existing Qt applications and how to migrate between the legacy Component system and the modern Command-based UI architecture. It covers integration patterns, migration strategies, adapter usage, and best practices for adopting DeclarativeUI in various scenarios.

The framework provides multiple integration approaches:
- **Direct Integration**: Add DeclarativeUI to existing Qt applications
- **Component Migration**: Gradually migrate from legacy Components to modern Commands
- **Hybrid Applications**: Mix legacy Components and modern Commands in the same application
- **Legacy Adaptation**: Use adapters to integrate with existing Qt codebases

## Prerequisites

Before integrating DeclarativeUI, ensure your project meets these requirements:

### Required Dependencies
- **Qt6** (6.2 or higher) with Core, Widgets, Network, and Test modules
- **CMake** 3.20 or higher
- **C++20 compatible compiler** (GCC 10+, Clang 12+, MSVC 2019+)

### Build Configuration
Enable the necessary DeclarativeUI features:
```cmake
# Enable Command system and adapters
set(BUILD_COMMAND_SYSTEM ON)
set(BUILD_ADAPTERS ON)
set(ENABLE_COMMAND_DEBUG OFF)  # Enable for development
```

## Integration Approaches

### 1. Direct Integration with Existing Qt Applications

Add DeclarativeUI to existing Qt applications without changing existing code:

```cpp
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>

// Existing Qt application
#include "Core/DeclarativeBuilder.hpp"
#include "Components/Button.hpp"
#include "Components/Label.hpp"

using namespace DeclarativeUI;

class MainWindow : public QMainWindow {
public:
    MainWindow() {
        setupUI();
    }

private:
    void setupUI() {
        // Create DeclarativeUI components within existing Qt structure
        auto centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);

        auto layout = new QVBoxLayout(centralWidget);

        // Add DeclarativeUI components to existing layout
        auto declarativeSection = Core::create<QWidget>()
            .layout<QVBoxLayout>()
            .child<Components::Label>([](auto& label) {
                label.text("DeclarativeUI Integration")
                     .style("font-size: 18px; font-weight: bold;");
            })
            .child<Components::Button>([](auto& button) {
                button.text("DeclarativeUI Button")
                      .onClick([]() {
                          qDebug() << "DeclarativeUI button clicked!";
                      });
            })
            .build();

        layout->addWidget(declarativeSection.release());

        // Continue with existing Qt widgets
        auto qtButton = new QPushButton("Traditional Qt Button", this);
        layout->addWidget(qtButton);
    }
};
```

### 2. Component System Integration

Use the legacy Component system for gradual adoption:

```cpp
#include "Components/Button.hpp"
#include "Components/LineEdit.hpp"
#include "Components/Container.hpp"
#include "Binding/StateManager.hpp"

class ComponentBasedForm {
public:
    ComponentBasedForm() {
        setupState();
        setupUI();
    }

private:
    void setupState() {
        auto& state = StateManager::instance();
        state.setState("form.name", QString(""));
        state.setState("form.email", QString(""));
        state.setState("form.valid", false);
    }

    void setupUI() {
        auto& state = StateManager::instance();

        // Create form using Components
        form_container_ = std::make_unique<Components::Container>();
        form_container_->layout("VBox")
                       .spacing(10)
                       .margins(20);

        // Name input with state binding
        auto nameInput = std::make_unique<Components::LineEdit>();
        nameInput->placeholder("Enter your name")
                 .bindProperty("text", [&state]() {
                     auto name = state.getState<QString>("form.name");
                     return name ? name->get() : QString("");
                 })
                 .onTextChanged([&state](const QString& text) {
                     state.setState("form.name", text);
                     validateForm();
                 });

        // Email input
        auto emailInput = std::make_unique<Components::LineEdit>();
        emailInput->placeholder("Enter your email")
                  .onTextChanged([&state](const QString& text) {
                      state.setState("form.email", text);
                      validateForm();
                  });

        // Submit button with state-dependent enabling
        auto submitButton = std::make_unique<Components::Button>();
        submitButton->text("Submit")
                    .bindProperty("enabled", [&state]() {
                        auto valid = state.getState<bool>("form.valid");
                        return valid ? valid->get() : false;
                    })
                    .onClick([this]() { submitForm(); });

        // Add components to container
        form_container_->addChild(std::move(nameInput));
        form_container_->addChild(std::move(emailInput));
        form_container_->addChild(std::move(submitButton));

        form_container_->initialize();
    }

    void validateForm() {
        auto& state = StateManager::instance();
        auto name = state.getState<QString>("form.name");
        auto email = state.getState<QString>("form.email");

        bool valid = name && !name->get().isEmpty() &&
                    email && email->get().contains("@");
        state.setState("form.valid", valid);
    }

    void submitForm() {
        auto& state = StateManager::instance();
        auto name = state.getState<QString>("form.name")->get();
        auto email = state.getState<QString>("form.email")->get();

        qDebug() << "Submitting form:" << name << email;
    }

private:
    std::unique_ptr<Components::Container> form_container_;
};
```

### Event System Integration

The Command event system provides abstract event handling:

```cpp
// Register global event handler
CommandEventDispatcher::instance().registerGlobalHandler(
    CommandEventType::Click,
    [](const CommandEvent& event) {
        qDebug() << "Global click handler:"
                 << event.getSource()->getCommandType()
                 << "at" << event.getTimestamp();
    }
);

// Commands automatically dispatch events
auto button = CommandBuilder("Button")
    .text("Event Button")
    .addEventListener("click", [](const CommandEvent& event) {
        // Custom event handling
        auto clickData = event.getData("position").toPoint();
        qDebug() << "Button clicked at position:" << clickData;
    })
    .build();
```

## Integration Components

### 1. WidgetMapper

Handles Command-to-QWidget translation and synchronization:

```cpp
// Register custom widget mappings
WidgetMapper::instance().registerMapping<QPushButton>("Button");
WidgetMapper::instance().registerMapping<QLineEdit>("TextInput");
WidgetMapper::instance().registerMapping<QSlider>("Slider");

// Create widgets from commands with automatic synchronization
auto command = CommandBuilder("Button").text("Test").build();
auto widget = WidgetMapper::instance().createWidget(command.get());

// Establish bidirectional binding
WidgetMapper::instance().establishBinding(command.get(), widget.get());
```

### 2. CommandBindingManager

Manages state and property bindings:

```cpp
// Bind Command properties to StateManager
CommandBindingManager::instance().addStateBinding(command.get(), {
    "text",           // Command property
    "ui.button.text", // State key
    BindingDirection::TwoWay
});

// Bind Command properties to widget properties
CommandBindingManager::instance().addPropertyBinding(command.get(), widget.get(), {
    "enabled",        // Command property
    "enabled",        // Widget property
    BindingDirection::TwoWay
});
```

### 3. JSONCommandLoader

Loads Command hierarchies from JSON:

```cpp
JSONCommandLoader loader;

// Configure automatic features
loader.setAutoStateBinding(true);
loader.setAutoMVCIntegration(true);
loader.setAutoWidgetCreation(true);

// Register event handlers
loader.registerEventHandler("saveDocument", [](const CommandEvent& event) {
    qDebug() << "Saving document...";
});

// Load UI from JSON
auto [command, widget] = loader.loadCommandWithWidgetFromFile("ui/main.json");
widget->show();
```

### 4. Component System Adapter

Provides compatibility with legacy Components:

```cpp
#ifdef DECLARATIVE_UI_ADAPTERS_ENABLED
// Convert legacy component to Command
auto legacyButton = std::make_unique<Components::Button>();
legacyButton->text("Legacy Button");

auto command = ComponentSystemAdapter::instance().convertToCommand(legacyButton.get());

// Create hybrid container
auto hybrid = ComponentSystemAdapter::instance().createHybridContainer();
hybrid->setLayout("HBox");
hybrid->addComponent(std::move(legacyButton));
hybrid->addCommand(CommandBuilder("Button").text("Command Button").build());

auto widget = hybrid->toWidget();
#endif
```

### 5. Integration Manager

Central coordinator for integration operations:

```cpp
#ifdef DECLARATIVE_UI_ADAPTERS_ENABLED
auto& manager = IntegrationManager::instance();
manager.initialize();

// Set compatibility mode for gradual migration
manager.setCompatibilityMode(IntegrationManager::CompatibilityMode::Hybrid);

// Enable debug logging
manager.setDebugMode(true);

// Validate integration
if (!manager.validateIntegration()) {
    auto issues = manager.getIntegrationIssues();
    for (const QString& issue : issues) {
        qWarning() << "Integration issue:" << issue;
    }
}
#endif
```

## Migration Strategies

### 1. Gradual Migration Approach

The recommended approach for migrating existing applications to the Command system.

#### Phase 1: Command Introduction

Start by introducing Commands alongside existing components:

```cpp
class MigrationExample : public QMainWindow {
public:
    MigrationExample() {
        setupLegacyUI();
        introduceCommands();
    }

private:
    void setupLegacyUI() {
        // Existing legacy UI code
        auto centralWidget = new QWidget;
        setCentralWidget(centralWidget);

        auto layout = new QVBoxLayout(centralWidget);

        // Legacy button
        legacy_button_ = new QPushButton("Legacy Button");
        connect(legacy_button_, &QPushButton::clicked, this, &MigrationExample::onLegacyClick);
        layout->addWidget(legacy_button_);
    }

    void introduceCommands() {
        // Add Command-based components alongside legacy ones
        auto commandButton = CommandBuilder("Button")
            .text("Command Button")
            .onClick([this]() { onCommandClick(); })
            .build();

        auto widget = WidgetMapper::instance().createWidget(commandButton.get());
        centralWidget()->layout()->addWidget(widget.release());

        command_button_ = commandButton;
    }

    void onLegacyClick() {
        qDebug() << "Legacy button clicked";
    }

    void onCommandClick() {
        qDebug() << "Command button clicked";
    }

private:
    QPushButton* legacy_button_;
    std::shared_ptr<BaseUICommand> command_button_;
};
```

#### Phase 2: State Unification

Unify state management using StateManager:

```cpp
class UnifiedStateExample : public QMainWindow {
public:
    UnifiedStateExample() {
        setupState();
        setupUI();
    }

private:
    void setupState() {
        auto& state = StateManager::instance();
        state.setState("counter", 0);
        state.setState("message", QString("Welcome"));
    }

    void setupUI() {
        auto centralWidget = new QWidget;
        setCentralWidget(centralWidget);
        auto layout = new QVBoxLayout(centralWidget);

        // Legacy component bound to state
        legacy_label_ = new QLabel();
        layout->addWidget(legacy_label_);

        // Connect legacy component to state manually
        auto& state = StateManager::instance();
        state.observeState<QString>("message", [this](const QString& value) {
            legacy_label_->setText(value);
        });

        // Command component with automatic state binding
        auto commandInput = CommandBuilder("TextInput")
            .placeholder("Enter message")
            .bindToState("message", "text")
            .build();

        auto inputWidget = WidgetMapper::instance().createWidget(commandInput.get());
        layout->addWidget(inputWidget.release());

        command_input_ = commandInput;
    }

private:
    QLabel* legacy_label_;
    std::shared_ptr<BaseUICommand> command_input_;
};
```

#### Phase 3: Hybrid Containers

Use hybrid containers to mix legacy and Command components:

```cpp
#ifdef DECLARATIVE_UI_ADAPTERS_ENABLED
class HybridExample : public QMainWindow {
public:
    HybridExample() {
        setupHybridUI();
    }

private:
    void setupHybridUI() {
        // Create hybrid container
        auto hybrid = ComponentSystemAdapter::instance().createHybridContainer();
        hybrid->setLayout("VBox");
        hybrid->setSpacing(10);

        // Add legacy component
        auto legacyButton = std::make_unique<Components::Button>();
        legacyButton->text("Legacy in Hybrid")
                   .onClick([this]() { onLegacyAction(); });
        legacyButton->initialize();

        auto legacyWidget = legacyButton->getWidget();
        hybrid->addWidget(std::unique_ptr<QWidget>(legacyWidget));

        // Add Command component
        auto commandButton = CommandBuilder("Button")
            .text("Command in Hybrid")
            .onClick([this]() { onCommandAction(); })
            .build();

        hybrid->addCommand(commandButton);

        // Convert to widget and display
        auto widget = hybrid->toWidget();
        setCentralWidget(widget.release());

        hybrid_container_ = std::move(hybrid);
    }

    void onLegacyAction() {
        qDebug() << "Legacy action in hybrid container";
    }

    void onCommandAction() {
        qDebug() << "Command action in hybrid container";
    }

private:
    std::unique_ptr<ComponentSystemAdapter::HybridContainer> hybrid_container_;
};
#endif
```

#### Phase 4: Full Command Migration

Complete migration to Command-based architecture:

```cpp
class FullCommandExample : public QMainWindow {
public:
    FullCommandExample() {
        setupState();
        setupCommandUI();
    }

private:
    void setupState() {
        auto& state = StateManager::instance();
        state.setState("app.title", QString("Modern Command App"));
        state.setState("user.name", QString(""));
        state.setState("form.valid", false);
    }

    void setupCommandUI() {
        // Create complete UI using CommandHierarchyBuilder
        auto mainUI = CommandHierarchyBuilder("Container")
            .layout("VBox")
            .spacing(20)
            .margins(30)

            // Header with state binding
            .addChild("Label", [](CommandBuilder& header) {
                header.bindToState("app.title", "text")
                      .style("font-size: 24px; font-weight: bold;")
                      .alignment(Qt::AlignCenter);
            })

            // Form section
            .addChild("Container", [](CommandBuilder& form) {
                form.layout("Form")
                    .spacing(10)
                    .child(CommandBuilder("TextInput")
                          .placeholder("Enter your name")
                          .bindToState("user.name", "text")
                          .addEventListener("textChanged", "validateForm"))
                    .child(CommandBuilder("Button")
                          .text("Submit")
                          .bindToState("form.valid", "enabled")
                          .addEventListener("click", "submitForm"));
            })

            // Status section
            .addChild("Label", [](CommandBuilder& status) {
                status.text("Ready")
                      .style("color: #27ae60; font-style: italic;");
            })
            .build();

        // Create widget and set as central widget
        auto widget = WidgetMapper::instance().createWidget(mainUI.get());
        setCentralWidget(widget.release());

        // Set up event handlers
        setupEventHandlers();

        main_command_ = mainUI;
    }

    void setupEventHandlers() {
        CommandEventDispatcher::instance().registerGlobalHandler(
            CommandEventType::Custom,
            [this](const CommandEvent& event) {
                QString eventType = event.getData("eventType").toString();

                if (eventType == "validateForm") {
                    validateForm();
                } else if (eventType == "submitForm") {
                    submitForm();
                }
            }
        );
    }

    void validateForm() {
        auto& state = StateManager::instance();
        QString name = state.getState<QString>("user.name")->get();
        bool valid = !name.trimmed().isEmpty();
        state.setState("form.valid", valid);
    }

    void submitForm() {
        auto& state = StateManager::instance();
        QString name = state.getState<QString>("user.name")->get();
        qDebug() << "Form submitted for user:" << name;

        QMessageBox::information(this, "Success",
                               QString("Welcome, %1!").arg(name));
    }

private:
    std::shared_ptr<BaseUICommand> main_command_;
};
```

### 2. JSON Migration Strategy

Migrate JSON UI definitions to use the Command system with enhanced features.

#### Legacy JSON Format

Traditional DeclarativeUI JSON format:

```json
{
    "type": "Widget",
    "properties": {
        "windowTitle": "Legacy App",
        "layout": "VBox"
    },
    "children": [
        {
            "type": "Button",
            "properties": {
                "text": "Legacy Button",
                "enabled": true
            }
        },
        {
            "type": "LineEdit",
            "properties": {
                "placeholderText": "Enter text"
            }
        }
    ]
}
```

#### Modern Command JSON Format

Enhanced JSON format with Command system features:

```json
{
    "type": "Container",
    "properties": {
        "layout": "VBox",
        "spacing": 15,
        "margins": [20, 20, 20, 20]
    },
    "style": {
        "background-color": "#f8f9fa",
        "border-radius": "8px"
    },
    "children": [
        {
            "type": "Label",
            "properties": {
                "text": "Modern Command App",
                "alignment": "center"
            },
            "style": {
                "font-size": "24px",
                "font-weight": "bold",
                "color": "#2c3e50",
                "margin-bottom": "20px"
            },
            "bindings": {
                "text": "app.title"
            }
        },
        {
            "type": "Button",
            "id": "actionButton",
            "properties": {
                "text": "Command Button"
            },
            "style": {
                "background-color": "#3498db",
                "color": "white",
                "padding": "12px 24px",
                "border-radius": "6px",
                "font-weight": "bold"
            },
            "events": {
                "click": "handleButtonClick"
            },
            "bindings": {
                "enabled": "ui.button.enabled",
                "text": "ui.button.text"
            },
            "validation": {
                "enabled": "form.isValid === true"
            }
        },
        {
            "type": "TextInput",
            "id": "userInput",
            "properties": {
                "placeholder": "Enter your message"
            },
            "style": {
                "padding": "10px",
                "border": "1px solid #bdc3c7",
                "border-radius": "4px"
            },
            "bindings": {
                "text": "user.message"
            },
            "events": {
                "textChanged": "validateInput"
            },
            "validation": {
                "required": true,
                "minLength": 3,
                "maxLength": 100
            }
        }
    ]
}
```

#### JSON Migration Utility

Use the JSONMigrationUtility to convert legacy JSON:

```cpp
#include "Command/Adapters/JSONMigrationUtility.hpp"

class JSONMigrationExample {
public:
    void migrateJSONFiles() {
        // Load legacy JSON
        QJsonObject legacyJSON = loadJSONFromFile("ui/legacy_main.json");

        // Convert to Command format
        QJsonObject commandJSON = JSONMigrationUtility::migrateToCommandFormat(legacyJSON);

        // Add modern features
        enhanceWithCommandFeatures(commandJSON);

        // Save migrated JSON
        saveJSONToFile("ui/command_main.json", commandJSON);

        // Generate migration report
        QString report = JSONMigrationUtility::generateMigrationReport(legacyJSON);
        qDebug() << "Migration Report:" << report;
    }

private:
    void enhanceWithCommandFeatures(QJsonObject& json) {
        // Add state bindings
        if (json.contains("children")) {
            QJsonArray children = json["children"].toArray();
            for (auto& child : children) {
                QJsonObject childObj = child.toObject();

                // Add bindings for buttons
                if (childObj["type"].toString() == "Button") {
                    QJsonObject bindings;
                    bindings["enabled"] = "ui.button.enabled";
                    childObj["bindings"] = bindings;

                    QJsonObject events;
                    events["click"] = "handleButtonClick";
                    childObj["events"] = events;
                }

                // Add bindings for inputs
                if (childObj["type"].toString() == "TextInput") {
                    QJsonObject bindings;
                    bindings["text"] = "form.input.text";
                    childObj["bindings"] = bindings;

                    QJsonObject events;
                    events["textChanged"] = "validateInput";
                    childObj["events"] = events;
                }

                child = childObj;
            }
            json["children"] = children;
        }
    }

    QJsonObject loadJSONFromFile(const QString& filePath) {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly)) {
            qWarning() << "Failed to open file:" << filePath;
            return QJsonObject();
        }

        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        return doc.object();
    }

    void saveJSONToFile(const QString& filePath, const QJsonObject& json) {
        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly)) {
            qWarning() << "Failed to create file:" << filePath;
            return;
        }

        QJsonDocument doc(json);
        file.write(doc.toJson(QJsonDocument::Indented));
    }
};
```

#### Loading Migrated JSON

Load the migrated JSON with Command system features:

```cpp
class ModernJSONLoader {
public:
    void loadModernUI() {
        JSONCommandLoader loader;

        // Configure automatic features
        loader.setAutoStateBinding(true);
        loader.setAutoMVCIntegration(true);
        loader.setAutoWidgetCreation(true);

        // Register event handlers
        setupEventHandlers(loader);

        // Initialize state
        setupInitialState();

        // Load UI from migrated JSON
        auto [command, widget] = loader.loadCommandWithWidgetFromFile("ui/command_main.json");

        if (widget) {
            widget->show();
            main_widget_ = std::move(widget);
            main_command_ = command;
        }
    }

private:
    void setupEventHandlers(JSONCommandLoader& loader) {
        loader.registerEventHandler("handleButtonClick", [this](const CommandEvent& event) {
            qDebug() << "Modern button clicked!";
            handleButtonAction();
        });

        loader.registerEventHandler("validateInput", [this](const CommandEvent& event) {
            QString text = event.getData("text").toString();
            validateUserInput(text);
        });
    }

    void setupInitialState() {
        auto& state = StateManager::instance();
        state.setState("app.title", QString("Modern Command App"));
        state.setState("ui.button.enabled", true);
        state.setState("ui.button.text", QString("Click Me"));
        state.setState("user.message", QString(""));
        state.setState("form.isValid", false);
    }

    void handleButtonAction() {
        auto& state = StateManager::instance();
        QString message = state.getState<QString>("user.message")->get();

        if (!message.isEmpty()) {
            QMessageBox::information(main_widget_.get(), "Message",
                                   QString("You entered: %1").arg(message));
        }
    }

    void validateUserInput(const QString& text) {
        auto& state = StateManager::instance();
        bool valid = text.length() >= 3 && text.length() <= 100;
        state.setState("form.isValid", valid);
        state.setState("ui.button.enabled", valid);
    }

private:
    std::unique_ptr<QWidget> main_widget_;
    std::shared_ptr<BaseUICommand> main_command_;
};
```

### 3. State Integration

**Bind existing components to Command state:**
```cpp
// Legacy component
auto legacyInput = new QLineEdit();

// Command component
auto commandInput = CommandBuilder("TextInput")
    .bindToState("form.input")
    .build();

// Synchronize both to same state
auto& stateAdapter = CommandStateManagerAdapter::instance();
stateAdapter.bindCommand(commandInput, "form.input", "text");

// Legacy component can be manually synchronized
connect(legacyInput, &QLineEdit::textChanged, [](const QString& text) {
    StateManager::instance().setState("form.input", text);
});
```

## Compatibility Modes

The Integration Manager provides different compatibility modes to support various migration scenarios.

### 1. Hybrid Mode (Recommended)

Best for gradual migration and mixed environments:

```cpp
#ifdef DECLARATIVE_UI_ADAPTERS_ENABLED
auto& manager = IntegrationManager::instance();
manager.initialize();
manager.setCompatibilityMode(IntegrationManager::CompatibilityMode::Hybrid);

// Both systems coexist seamlessly
auto legacyButton = std::make_unique<Components::Button>();
auto commandButton = CommandBuilder("Button").text("Command").build();

// Create hybrid container
auto hybrid = ComponentSystemAdapter::instance().createHybridContainer();
hybrid->addComponent(std::move(legacyButton));
hybrid->addCommand(commandButton);
#endif
```

**Benefits:**
- Seamless coexistence of legacy and Command components
- Automatic conversion when needed
- Gradual migration path
- Full feature compatibility

### 2. Command Only Mode

For new projects or complete migration:

```cpp
#ifdef DECLARATIVE_UI_ADAPTERS_ENABLED
IntegrationManager::instance().setCompatibilityMode(
    IntegrationManager::CompatibilityMode::CommandOnly
);

// Legacy components are automatically wrapped as Commands
auto legacyButton = std::make_unique<Components::Button>();
auto wrappedCommand = IntegrationManager::instance().convertToCommand(legacyButton.get());
#endif
```

**Benefits:**
- Modern Command-based architecture
- Automatic legacy component wrapping
- Full Command system features
- Consistent API surface

### 3. Migration Mode

For projects in active migration:

```cpp
#ifdef DECLARATIVE_UI_ADAPTERS_ENABLED
IntegrationManager::instance().setCompatibilityMode(
    IntegrationManager::CompatibilityMode::Migration
);

// Provides warnings and migration assistance
manager.setMigrationWarningsEnabled(true);
manager.setMigrationLoggingEnabled(true);

// Analyze migration progress
auto analysis = MigrationAssistant::analyzeProject(QCoreApplication::applicationDirPath());
qDebug() << "Migration progress:" << analysis.completion_percentage << "%";
#endif
```

**Benefits:**
- Migration progress tracking
- Compatibility warnings
- Migration assistance tools
- Detailed logging and analysis

### 4. Development Mode

For development and testing:

```cpp
#ifdef DECLARATIVE_UI_ADAPTERS_ENABLED
IntegrationManager::instance().setCompatibilityMode(
    IntegrationManager::CompatibilityMode::Development
);

// Enable debug features
manager.setDebugMode(true);
manager.setPerformanceMonitoringEnabled(true);
manager.setValidationEnabled(true);

// Get detailed metrics
auto metrics = manager.getPerformanceMetrics();
qDebug() << "Average conversion time:" << metrics.avg_conversion_time_ms << "ms";
#endif
```

**Benefits:**
- Comprehensive debugging information
- Performance monitoring
- Validation and error checking
- Development assistance tools

## Best Practices

### 1. Migration Strategy

#### Start with Hybrid Mode

Begin integration with hybrid mode for existing projects:

```cpp
// Initialize hybrid mode
#ifdef DECLARATIVE_UI_ADAPTERS_ENABLED
auto& manager = IntegrationManager::instance();
manager.initialize();
manager.setCompatibilityMode(IntegrationManager::CompatibilityMode::Hybrid);

// Create mixed UI
auto hybrid = ComponentSystemAdapter::instance().createHybridContainer();
hybrid->setLayout("VBox");

// Add existing legacy components
auto legacyWidget = std::make_unique<Components::Button>();
legacyWidget->text("Legacy Button");
hybrid->addComponent(std::move(legacyWidget));

// Add new Command components
auto commandWidget = CommandBuilder("Button")
    .text("Command Button")
    .bindToState("ui.button.enabled", "enabled")
    .build();
hybrid->addCommand(commandWidget);
#endif
```

#### Gradual Component Conversion

Convert components incrementally:

```cpp
class GradualMigration {
public:
    void convertButtonsToCommands() {
        // Phase 1: Identify legacy buttons
        auto legacyButtons = findLegacyButtons();

        // Phase 2: Convert one by one
        for (auto* legacyButton : legacyButtons) {
            convertButtonToCommand(legacyButton);
        }

        // Phase 3: Update event handlers
        updateEventHandlers();

        // Phase 4: Migrate state bindings
        migrateStateBindings();
    }

private:
    void convertButtonToCommand(QPushButton* legacyButton) {
        // Extract properties
        QString text = legacyButton->text();
        bool enabled = legacyButton->isEnabled();

        // Create Command equivalent
        auto command = CommandBuilder("Button")
            .text(text)
            .enabled(enabled)
            .build();

        // Replace in layout
        auto layout = legacyButton->parent()->layout();
        auto widget = WidgetMapper::instance().createWidget(command.get());

        layout->replaceWidget(legacyButton, widget.release());
        legacyButton->deleteLater();

        // Store command reference
        converted_commands_.push_back(command);
    }

    std::vector<QPushButton*> findLegacyButtons() {
        // Implementation to find legacy buttons
        return {};
    }

    void updateEventHandlers() {
        // Migrate Qt signal/slot connections to Command events
    }

    void migrateStateBindings() {
        // Convert manual state updates to automatic bindings
    }

private:
    std::vector<std::shared_ptr<BaseUICommand>> converted_commands_;
};
```

### 2. State Management Integration

#### Unified State Architecture

Use StateManager as the single source of truth:

```cpp
class UnifiedStateExample {
public:
    void setupUnifiedState() {
        auto& state = StateManager::instance();

        // Initialize application state
        state.setState("app.theme", QString("light"));
        state.setState("user.preferences.notifications", true);
        state.setState("ui.sidebar.visible", true);

        // Create Commands with automatic state binding
        auto themeToggle = CommandBuilder("Button")
            .text("Toggle Theme")
            .bindToState("app.theme", "value")
            .onClick([&state]() {
                QString current = state.getState<QString>("app.theme")->get();
                state.setState("app.theme", current == "light" ? "dark" : "light");
            })
            .build();

        // Legacy components can observe the same state
        legacy_label_ = new QLabel();
        state.observeState<QString>("app.theme", [this](const QString& theme) {
            legacy_label_->setText(QString("Current theme: %1").arg(theme));
            updateLegacyTheme(theme);
        });
    }

private:
    void updateLegacyTheme(const QString& theme) {
        // Update legacy components based on theme
        QString styleSheet = (theme == "dark")
            ? "background-color: #2c3e50; color: white;"
            : "background-color: white; color: black;";
        legacy_label_->setStyleSheet(styleSheet);
    }

private:
    QLabel* legacy_label_;
};
```

#### Batch State Operations

Use batch operations for performance:

```cpp
void performBatchStateUpdate() {
    auto& state = StateManager::instance();

    // Begin batch update
    state.beginBatchUpdate();

    // Multiple state changes
    state.setState("form.name", QString("John Doe"));
    state.setState("form.email", QString("john@example.com"));
    state.setState("form.valid", true);
    state.setState("ui.submitButton.enabled", true);

    // Commit all changes at once
    state.endBatchUpdate();

    // All bound Commands update simultaneously
}
```

### 3. Event System Integration

#### Modern Event Handling

Replace Qt signals with Command events:

```cpp
class ModernEventHandling {
public:
    void setupEventSystem() {
        // Register global event handlers
        CommandEventDispatcher::instance().registerGlobalHandler(
            CommandEventType::Click,
            [this](const CommandEvent& event) {
                logUserInteraction(event);
            }
        );

        // Create Commands with event handlers
        auto button = CommandBuilder("Button")
            .text("Modern Button")
            .addEventListener("click", [this](const CommandEvent& event) {
                handleButtonClick(event);
            })
            .build();

        // Legacy components can dispatch Command events
        legacy_button_ = new QPushButton("Legacy Button");
        connect(legacy_button_, &QPushButton::clicked, [this]() {
            CommandEvent event(CommandEventType::Click);
            event.setData("source", "legacy_button");
            event.setData("timestamp", QDateTime::currentDateTime());

            CommandEventDispatcher::instance().dispatchEvent(event);
        });
    }

private:
    void logUserInteraction(const CommandEvent& event) {
        qDebug() << "User interaction:"
                 << event.getTypeName()
                 << "at" << event.getTimestamp()
                 << "from" << event.getData("source").toString();
    }

    void handleButtonClick(const CommandEvent& event) {
        qDebug() << "Modern button clicked!";
    }

private:
    QPushButton* legacy_button_;
};
```

### 4. Performance Optimization

#### Efficient Widget Creation

Create widgets only when needed:

```cpp
class LazyWidgetCreation {
public:
    void setupLazyUI() {
        // Create Commands without widgets initially
        auto tabContent = CommandHierarchyBuilder("Container")
            .layout("VBox")
            .addChild("Label", [](CommandBuilder& label) {
                label.text("Tab Content");
            })
            .addChild("Button", [](CommandBuilder& button) {
                button.text("Action Button");
            })
            .build();

        // Store command reference
        tab_commands_["tab1"] = tabContent;

        // Create widget only when tab is activated
        connect(tab_widget_, &QTabWidget::currentChanged, [this](int index) {
            QString tabKey = QString("tab%1").arg(index + 1);
            if (tab_commands_.contains(tabKey) && !tab_widgets_.contains(tabKey)) {
                auto widget = WidgetMapper::instance().createWidget(tab_commands_[tabKey].get());
                tab_widgets_[tabKey] = widget.get();
                tab_widget_->widget(index)->layout()->addWidget(widget.release());
            }
        });
    }

private:
    QTabWidget* tab_widget_;
    QMap<QString, std::shared_ptr<BaseUICommand>> tab_commands_;
    QMap<QString, QWidget*> tab_widgets_;
};
```

#### Memory Management

Proper lifecycle management:

```cpp
class MemoryManagement {
public:
    ~MemoryManagement() {
        // Clean up Command-Widget bindings
        for (auto& command : commands_) {
            WidgetMapper::instance().removeBinding(command.get());
        }

        // Remove event handlers
        CommandEventDispatcher::instance().unregisterAllHandlers(nullptr);

        // Clear state bindings
        for (auto& command : commands_) {
            CommandBindingManager::instance().removeAllStateBindings(command.get());
        }
    }

private:
    std::vector<std::shared_ptr<BaseUICommand>> commands_;
};
```

## Migration Tools and Utilities

### 1. Integration Analysis

Analyze your existing codebase for migration planning:

```cpp
#ifdef DECLARATIVE_UI_ADAPTERS_ENABLED
class IntegrationAnalyzer {
public:
    void analyzeProject(const QString& projectPath) {
        // Analyze existing project structure
        auto analysis = MigrationAssistant::analyzeProject(projectPath);

        qDebug() << "=== Migration Analysis ===";
        qDebug() << "Project complexity:" << analysis.complexity_level;
        qDebug() << "UI files found:" << analysis.ui_files;
        qDebug() << "Legacy components:" << analysis.legacy_components;
        qDebug() << "Estimated effort:" << analysis.estimated_effort_hours << "hours";
        qDebug() << "Recommended approach:" << analysis.recommended_approach;

        // Generate detailed report
        generateAnalysisReport(analysis);

        // Create migration plan
        auto plan = MigrationAssistant::createMigrationPlan(analysis);
        executeMigrationPlan(plan);
    }

private:
    void generateAnalysisReport(const MigrationAnalysis& analysis) {
        QFile reportFile("migration_analysis.md");
        if (reportFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream stream(&reportFile);
            stream << "# Migration Analysis Report\n\n";
            stream << "## Project Overview\n";
            stream << "- **Complexity Level**: " << analysis.complexity_level << "\n";
            stream << "- **UI Files**: " << analysis.ui_files << "\n";
            stream << "- **Legacy Components**: " << analysis.legacy_components << "\n";
            stream << "- **Estimated Effort**: " << analysis.estimated_effort_hours << " hours\n\n";

            stream << "## Recommendations\n";
            stream << analysis.recommendations.join("\n- ");
        }
    }

    void executeMigrationPlan(const MigrationPlan& plan) {
        qDebug() << "Executing migration plan with" << plan.steps.size() << "steps";

        for (const auto& step : plan.steps) {
            qDebug() << "Migration step:" << step.description;

            // Execute step based on type
            switch (step.type) {
                case MigrationStepType::AnalyzeComponents:
                    analyzeComponents(step);
                    break;
                case MigrationStepType::ConvertToCommands:
                    convertToCommands(step);
                    break;
                case MigrationStepType::UpdateStateBindings:
                    updateStateBindings(step);
                    break;
                case MigrationStepType::MigrateEvents:
                    migrateEvents(step);
                    break;
            }
        }
    }

    void analyzeComponents(const MigrationStep& step) {
        // Implementation for component analysis
    }

    void convertToCommands(const MigrationStep& step) {
        // Implementation for command conversion
    }

    void updateStateBindings(const MigrationStep& step) {
        // Implementation for state binding updates
    }

    void migrateEvents(const MigrationStep& step) {
        // Implementation for event migration
    }
};
#endif
```

### 2. JSON Migration Utilities

Convert legacy JSON UI definitions to Command format:

```cpp
class JSONMigrationTool {
public:
    void migrateJSONFiles(const QString& sourceDir, const QString& targetDir) {
        QDir source(sourceDir);
        QDir target(targetDir);

        // Ensure target directory exists
        if (!target.exists()) {
            target.mkpath(".");
        }

        // Find all JSON files
        auto jsonFiles = source.entryList(QStringList() << "*.json", QDir::Files);

        for (const QString& fileName : jsonFiles) {
            migrateJSONFile(source.filePath(fileName), target.filePath(fileName));
        }

        // Generate migration summary
        generateMigrationSummary(jsonFiles.size());
    }

private:
    void migrateJSONFile(const QString& sourcePath, const QString& targetPath) {
        qDebug() << "Migrating:" << sourcePath << "to" << targetPath;

        // Load legacy JSON
        QJsonObject legacyJSON = loadJSONFromFile(sourcePath);
        if (legacyJSON.isEmpty()) {
            qWarning() << "Failed to load JSON from:" << sourcePath;
            return;
        }

        // Convert to Command format
        QJsonObject commandJSON = JSONMigrationUtility::migrateToCommandFormat(legacyJSON);

        // Enhance with modern features
        enhanceWithModernFeatures(commandJSON);

        // Save migrated JSON
        saveJSONToFile(targetPath, commandJSON);

        // Generate migration report for this file
        QString report = JSONMigrationUtility::generateMigrationReport(legacyJSON);
        saveReportToFile(targetPath + ".migration_report.txt", report);
    }

    void enhanceWithModernFeatures(QJsonObject& json) {
        // Add state bindings
        addStateBindings(json);

        // Add event handlers
        addEventHandlers(json);

        // Add validation rules
        addValidationRules(json);

        // Add styling
        addModernStyling(json);
    }

    void addStateBindings(QJsonObject& json) {
        // Implementation to add state bindings to JSON
    }

    void addEventHandlers(QJsonObject& json) {
        // Implementation to add event handlers to JSON
    }

    void addValidationRules(QJsonObject& json) {
        // Implementation to add validation rules to JSON
    }

    void addModernStyling(QJsonObject& json) {
        // Implementation to add modern styling to JSON
    }

    QJsonObject loadJSONFromFile(const QString& filePath) {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly)) {
            return QJsonObject();
        }

        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        return doc.object();
    }

    void saveJSONToFile(const QString& filePath, const QJsonObject& json) {
        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly)) {
            qWarning() << "Failed to save JSON to:" << filePath;
            return;
        }

        QJsonDocument doc(json);
        file.write(doc.toJson(QJsonDocument::Indented));
    }

    void saveReportToFile(const QString& filePath, const QString& report) {
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            stream << report;
        }
    }

    void generateMigrationSummary(int filesProcessed) {
        qDebug() << "=== JSON Migration Summary ===";
        qDebug() << "Files processed:" << filesProcessed;
        qDebug() << "Migration completed successfully!";
    }
};
```

### 3. Validation and Testing Tools

Comprehensive validation for integration correctness:

```cpp
class IntegrationValidator {
public:
    bool validateCompleteIntegration() {
        bool allValid = true;

        // Validate integration manager
        allValid &= validateIntegrationManager();

        // Validate widget mappings
        allValid &= validateWidgetMappings();

        // Validate state bindings
        allValid &= validateStateBindings();

        // Validate event system
        allValid &= validateEventSystem();

        // Validate JSON loading
        allValid &= validateJSONLoading();

        return allValid;
    }

private:
    bool validateIntegrationManager() {
        #ifdef DECLARATIVE_UI_ADAPTERS_ENABLED
        auto& manager = IntegrationManager::instance();

        bool valid = manager.validateIntegration();
        if (!valid) {
            auto issues = manager.getIntegrationIssues();
            qWarning() << "Integration Manager issues:";
            for (const QString& issue : issues) {
                qWarning() << "  -" << issue;
            }
        }

        return valid;
        #else
        qWarning() << "Integration Manager not available (adapters disabled)";
        return false;
        #endif
    }

    bool validateWidgetMappings() {
        // Test widget creation for all registered command types
        auto& mapper = WidgetMapper::instance();
        auto& factory = UICommandFactory::instance();

        auto commandTypes = factory.getRegisteredTypes();
        bool allValid = true;

        for (const QString& type : commandTypes) {
            auto command = factory.createCommand(type);
            if (command) {
                auto widget = mapper.createWidget(command.get());
                if (!widget) {
                    qWarning() << "Failed to create widget for command type:" << type;
                    allValid = false;
                }
            }
        }

        return allValid;
    }

    bool validateStateBindings() {
        // Test state binding functionality
        auto& state = StateManager::instance();
        auto& bindingManager = CommandBindingManager::instance();

        // Create test command
        auto testCommand = CommandBuilder("Button")
            .text("Test")
            .bindToState("test.value", "text")
            .build();

        // Test bidirectional binding
        state.setState("test.value", QString("State Value"));
        QString commandValue = testCommand->getState("text").toString();

        if (commandValue != "State Value") {
            qWarning() << "State to Command binding failed";
            return false;
        }

        // Test Command to State binding
        testCommand->setState("text", "Command Value");
        QString stateValue = state.getState<QString>("test.value")->get();

        if (stateValue != "Command Value") {
            qWarning() << "Command to State binding failed";
            return false;
        }

        return true;
    }

    bool validateEventSystem() {
        // Test event dispatching and handling
        bool eventReceived = false;

        auto testCommand = CommandBuilder("Button")
            .text("Test")
            .addEventListener("click", [&eventReceived](const CommandEvent& event) {
                eventReceived = true;
            })
            .build();

        // Dispatch test event
        testCommand->dispatchEvent("click", {});

        if (!eventReceived) {
            qWarning() << "Event system validation failed";
            return false;
        }

        return true;
    }

    bool validateJSONLoading() {
        // Test JSON command loading
        QString testJSON = R"({
            "type": "Button",
            "properties": {
                "text": "Test Button"
            }
        })";

        JSONCommandLoader loader;
        auto command = loader.loadCommandFromString(testJSON);

        if (!command) {
            qWarning() << "JSON loading validation failed";
            return false;
        }

        if (command->getState("text").toString() != "Test Button") {
            qWarning() << "JSON property loading failed";
            return false;
        }

        return true;
    }
};
```

## Complete Integration Example

Here's a comprehensive example demonstrating the complete integration of legacy and Command systems:

```cpp
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>

// Legacy components
#include "Components/Button.hpp"
#include "Components/Widget.hpp"

// Command system
#ifdef DECLARATIVE_UI_COMMAND_SYSTEM_ENABLED
#include "Command/CommandBuilder.hpp"
#include "Command/WidgetMapper.hpp"
#include "Binding/StateManager.hpp"

#ifdef DECLARATIVE_UI_ADAPTERS_ENABLED
#include "Command/Adapters/ComponentSystemAdapter.hpp"
#include "Command/Adapters/IntegrationManager.hpp"
#endif

using namespace DeclarativeUI::Command;
using namespace DeclarativeUI::Components;

class CompleteIntegrationExample : public QMainWindow {
    Q_OBJECT

public:
    CompleteIntegrationExample(QWidget* parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("Complete Integration Example");
        setMinimumSize(800, 600);

        initializeIntegration();
        setupState();
        setupUI();
        setupEventHandlers();
    }

private:
    void initializeIntegration() {
        #ifdef DECLARATIVE_UI_ADAPTERS_ENABLED
        // Initialize integration manager
        auto& manager = Adapters::IntegrationManager::instance();
        manager.initialize();
        manager.setCompatibilityMode(Adapters::IntegrationManager::CompatibilityMode::Hybrid);
        manager.setDebugMode(true);

        qDebug() << "🌉 Integration manager initialized";
        #endif
    }

    void setupState() {
        auto& state = DeclarativeUI::Binding::StateManager::instance();

        // Initialize application state
        state.setState("app.title", QString("Complete Integration Demo"));
        state.setState("user.name", QString(""));
        state.setState("user.email", QString(""));
        state.setState("form.valid", false);
        state.setState("counter", 0);
        state.setState("theme", QString("light"));

        qDebug() << "🌐 Application state initialized";
    }

    void setupUI() {
        auto centralWidget = new QWidget;
        setCentralWidget(centralWidget);

        auto mainLayout = new QVBoxLayout(centralWidget);
        mainLayout->setSpacing(20);
        mainLayout->setContentsMargins(30, 30, 30, 30);

        // Header
        createHeader(mainLayout);

        // Legacy section
        createLegacySection(mainLayout);

        // Command section
        createCommandSection(mainLayout);

        // Hybrid section
        createHybridSection(mainLayout);

        // State demonstration
        createStateSection(mainLayout);

        qDebug() << "✅ Complete UI created";
    }

    void createHeader(QVBoxLayout* layout) {
        auto& state = DeclarativeUI::Binding::StateManager::instance();

        header_label_ = new QLabel();
        header_label_->setAlignment(Qt::AlignCenter);
        header_label_->setStyleSheet("font-size: 24px; font-weight: bold; color: #2c3e50; padding: 20px; background-color: #ecf0f1; border-radius: 8px;");

        // Bind to state
        state.observeState<QString>("app.title", [this](const QString& title) {
            header_label_->setText(title);
        });

        layout->addWidget(header_label_);
    }

    void createLegacySection(QVBoxLayout* layout) {
        auto legacyGroup = new QGroupBox("📦 Legacy Components");
        legacyGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #e74c3c; border: 2px solid #e74c3c; border-radius: 5px; margin: 5px; padding-top: 10px; }");

        auto legacyLayout = new QVBoxLayout(legacyGroup);

        // Legacy button using traditional DeclarativeUI
        legacy_button_ = std::make_unique<Button>();
        legacy_button_->text("Legacy Button")
                     .onClick([this]() {
                         onLegacyButtonClicked();
                     });

        legacy_button_->initialize();
        auto* legacyButtonWidget = legacy_button_->getWidget();
        legacyButtonWidget->setStyleSheet("QPushButton { background-color: #e74c3c; color: white; padding: 10px; border-radius: 5px; }");
        legacyLayout->addWidget(legacyButtonWidget);

        // Legacy Qt widget
        legacy_qt_button_ = new QPushButton("Legacy Qt Button");
        legacy_qt_button_->setStyleSheet("QPushButton { background-color: #c0392b; color: white; padding: 10px; border-radius: 5px; }");
        connect(legacy_qt_button_, &QPushButton::clicked, this, &CompleteIntegrationExample::onLegacyQtButtonClicked);
        legacyLayout->addWidget(legacy_qt_button_);

        layout->addWidget(legacyGroup);
    }

    void createCommandSection(QVBoxLayout* layout) {
        try {
            // Create Command-based section using CommandHierarchyBuilder
            auto commandContainer = UI::CommandHierarchyBuilder("Container")
                .layout("VBox")
                .spacing(10)
                .style("border: 2px solid #3498db; border-radius: 5px; padding: 15px; background-color: #f8fbff;")

                .addChild("Label", [](UI::CommandBuilder& title) {
                    title.text("⚡ Command System Components")
                         .style("font-weight: bold; color: #3498db; font-size: 16px;");
                })

                .addChild("Button", [this](UI::CommandBuilder& cmdButton) {
                    cmdButton.text("Command Button")
                            .style("background-color: #3498db; color: white; padding: 10px; border-radius: 5px;")
                            .onClick([this]() {
                                onCommandButtonClicked();
                            });
                })

                .addChild("TextInput", [this](UI::CommandBuilder& input) {
                    input.placeholder("Command input...")
                         .style("padding: 8px; border: 1px solid #bdc3c7; border-radius: 3px;")
                         .bindToState("user.name", "text")
                         .addEventListener("textChanged", "validateForm");
                })

                .build();

            // Convert to widget
            auto commandWidget = UI::WidgetMapper::instance().createWidget(commandContainer.get());
            if (commandWidget) {
                layout->addWidget(commandWidget.release());
                command_container_ = commandContainer;
            }

        } catch (const std::exception& e) {
            qWarning() << "❌ Error creating Command section:" << e.what();
        }
    }

    void createHybridSection(QVBoxLayout* layout) {
        #ifdef DECLARATIVE_UI_ADAPTERS_ENABLED
        try {
            auto hybridGroup = new QGroupBox("🔄 Hybrid Integration");
            hybridGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #9b59b6; border: 2px solid #9b59b6; border-radius: 5px; margin: 5px; padding-top: 10px; }");

            auto hybridLayout = new QVBoxLayout(hybridGroup);

            // Create hybrid container
            auto& adapter = Adapters::ComponentSystemAdapter::instance();
            hybrid_container_ = adapter.createHybridContainer();
            hybrid_container_->setLayout("HBox");
            hybrid_container_->setSpacing(10);

            // Add legacy component
            auto legacyWidget = std::make_unique<Button>();
            legacyWidget->text("Hybrid Legacy")
                       .onClick([this]() {
                           onHybridLegacyClicked();
                       });

            legacyWidget->initialize();
            auto* legacyQWidget = legacyWidget->getWidget();
            legacyQWidget->setStyleSheet("QPushButton { background-color: #9b59b6; color: white; padding: 8px; border-radius: 3px; }");
            hybrid_container_->addWidget(std::unique_ptr<QWidget>(legacyQWidget));

            // Add Command component
            auto commandButton = UI::CommandBuilder("Button")
                .text("Hybrid Command")
                .style("background-color: #8e44ad; color: white; padding: 8px; border-radius: 3px;")
                .onClick([this]() {
                    onHybridCommandClicked();
                })
                .build();

            hybrid_container_->addCommand(commandButton);

            // Convert to widget
            auto hybridWidget = hybrid_container_->toWidget();
            if (hybridWidget) {
                hybridLayout->addWidget(hybridWidget.release());
            }

            layout->addWidget(hybridGroup);

        } catch (const std::exception& e) {
            qWarning() << "❌ Error creating Hybrid section:" << e.what();
        }
        #else
        auto hybridGroup = new QGroupBox("🔄 Hybrid Integration (Disabled)");
        hybridGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #95a5a6; border: 2px solid #95a5a6; border-radius: 5px; margin: 5px; padding-top: 10px; }");

        auto hybridLayout = new QVBoxLayout(hybridGroup);
        auto disabledInfo = new QLabel("Hybrid integration requires BUILD_ADAPTERS=ON");
        disabledInfo->setStyleSheet("color: #7f8c8d; font-style: italic;");
        hybridLayout->addWidget(disabledInfo);

        layout->addWidget(hybridGroup);
        #endif
    }

    void createStateSection(QVBoxLayout* layout) {
        auto stateGroup = new QGroupBox("🌐 State Management Demo");
        stateGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #27ae60; border: 2px solid #27ae60; border-radius: 5px; margin: 5px; padding-top: 10px; }");

        auto stateLayout = new QVBoxLayout(stateGroup);

        // Counter display (legacy widget bound to state)
        counter_label_ = new QLabel("Counter: 0");
        counter_label_->setStyleSheet("font-size: 18px; font-weight: bold; color: #27ae60;");
        stateLayout->addWidget(counter_label_);

        // Bind legacy widget to state
        auto& state = DeclarativeUI::Binding::StateManager::instance();
        state.observeState<int>("counter", [this](int value) {
            counter_label_->setText(QString("Counter: %1").arg(value));
        });

        // Counter controls using Commands
        try {
            auto counterControls = UI::CommandHierarchyBuilder("Container")
                .layout("HBox")
                .spacing(10)

                .addChild("Button", [this](UI::CommandBuilder& btn) {
                    btn.text("-")
                       .style("background-color: #e74c3c; color: white; padding: 8px 15px; border-radius: 3px;")
                       .onClick([this]() {
                           decrementCounter();
                       });
                })

                .addChild("Button", [this](UI::CommandBuilder& btn) {
                    btn.text("Reset")
                       .style("background-color: #95a5a6; color: white; padding: 8px 15px; border-radius: 3px;")
                       .onClick([this]() {
                           resetCounter();
                       });
                })

                .addChild("Button", [this](UI::CommandBuilder& btn) {
                    btn.text("+")
                       .style("background-color: #27ae60; color: white; padding: 8px 15px; border-radius: 3px;")
                       .onClick([this]() {
                           incrementCounter();
                       });
                })

                .build();

            auto controlsWidget = UI::WidgetMapper::instance().createWidget(counterControls.get());
            if (controlsWidget) {
                stateLayout->addWidget(controlsWidget.release());
                counter_controls_ = counterControls;
            }

        } catch (const std::exception& e) {
            qWarning() << "❌ Error creating state controls:" << e.what();
        }

        layout->addWidget(stateGroup);
    }

    void setupEventHandlers() {
        // Set up Command event handlers
        UI::CommandEventDispatcher::instance().registerGlobalHandler(
            UI::CommandEventType::Custom,
            [this](const UI::CommandEvent& event) {
                QString eventType = event.getData("eventType").toString();

                if (eventType == "validateForm") {
                    validateForm();
                }
            }
        );
    }

    // Event handlers
    void onLegacyButtonClicked() {
        qDebug() << "🔴 Legacy DeclarativeUI button clicked";
        QMessageBox::information(this, "Legacy", "Legacy DeclarativeUI button clicked!");
    }

    void onLegacyQtButtonClicked() {
        qDebug() << "🔴 Legacy Qt button clicked";
        QMessageBox::information(this, "Legacy Qt", "Legacy Qt button clicked!");
    }

    void onCommandButtonClicked() {
        qDebug() << "🔵 Command button clicked";
        QMessageBox::information(this, "Command", "Command system button clicked!");
    }

    void onHybridLegacyClicked() {
        qDebug() << "🟣 Hybrid legacy clicked";
        QMessageBox::information(this, "Hybrid", "Hybrid legacy component clicked!");
    }

    void onHybridCommandClicked() {
        qDebug() << "🟣 Hybrid command clicked";
        QMessageBox::information(this, "Hybrid", "Hybrid command component clicked!");
    }

    void incrementCounter() {
        auto& state = DeclarativeUI::Binding::StateManager::instance();
        auto counter = state.getState<int>("counter");
        if (counter) {
            state.setState("counter", counter->get() + 1);
        }
    }

    void decrementCounter() {
        auto& state = DeclarativeUI::Binding::StateManager::instance();
        auto counter = state.getState<int>("counter");
        if (counter && counter->get() > 0) {
            state.setState("counter", counter->get() - 1);
        }
    }

    void resetCounter() {
        auto& state = DeclarativeUI::Binding::StateManager::instance();
        state.setState("counter", 0);
    }

    void validateForm() {
        auto& state = DeclarativeUI::Binding::StateManager::instance();
        auto name = state.getState<QString>("user.name");
        bool valid = name && !name->get().trimmed().isEmpty();
        state.setState("form.valid", valid);

        qDebug() << "Form validation:" << (valid ? "valid" : "invalid");
    }

private:
    // Legacy components
    std::unique_ptr<Button> legacy_button_;
    QPushButton* legacy_qt_button_;

    // Command components
    std::shared_ptr<UI::BaseUICommand> command_container_;
    std::shared_ptr<UI::BaseUICommand> counter_controls_;

    // Hybrid components
    #ifdef DECLARATIVE_UI_ADAPTERS_ENABLED
    std::unique_ptr<Adapters::ComponentSystemAdapter::HybridContainer> hybrid_container_;
    #endif

    // UI elements
    QLabel* header_label_;
    QLabel* counter_label_;
};

#include "complete_integration_example.moc"

#endif // DECLARATIVE_UI_COMMAND_SYSTEM_ENABLED

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    #ifdef DECLARATIVE_UI_COMMAND_SYSTEM_ENABLED
    qDebug() << "🚀 Starting Complete Integration Example";

    CompleteIntegrationExample window;
    window.show();

    qDebug() << "💡 This example demonstrates:";
    qDebug() << "   - Legacy DeclarativeUI components";
    qDebug() << "   - Legacy Qt widgets";
    qDebug() << "   - Modern Command system";
    qDebug() << "   - Hybrid containers";
    qDebug() << "   - State management integration";
    qDebug() << "   - Event system integration";

    return app.exec();
    #else
    qWarning() << "❌ Command system not enabled. Please build with BUILD_COMMAND_SYSTEM=ON";
    qWarning() << "💡 This example requires the Command system for integration demonstration";

    return 1;
    #endif
}
```

## Summary

This integration guide provides a comprehensive approach to adopting the Command-based UI architecture:

1. **Gradual Migration**: Start with hybrid mode and gradually convert components
2. **State Unification**: Use StateManager as the single source of truth
3. **Event Modernization**: Migrate to the Command event system
4. **JSON Enhancement**: Upgrade JSON definitions with modern features
5. **Validation**: Use comprehensive validation tools
6. **Best Practices**: Follow recommended patterns for optimal results

The Command system provides a modern, flexible foundation for UI development while maintaining full compatibility with existing Qt and DeclarativeUI components. This enables teams to modernize their applications incrementally without disrupting existing functionality.

## Example: Complete Migration

```cpp
class MigrationExample {
public:
    void migrateApplication() {
        // 1. Initialize integration
        auto& manager = IntegrationManager::instance();
        manager.initialize();
        manager.setCompatibilityMode(IntegrationManager::CompatibilityMode::Migration);
        
        // 2. Analyze existing code
        auto analysis = MigrationAssistant::analyzeProject(QCoreApplication::applicationDirPath());
        qDebug() << "Found" << analysis.ui_files << "UI files to migrate";
        
        // 3. Create hybrid container for gradual migration
        auto hybrid = ComponentSystemAdapter::instance().createHybridContainer();
        
        // 4. Add existing components
        auto legacyButton = std::make_unique<Components::Button>();
        legacyButton->text("Legacy Button");
        hybrid->addComponent(std::move(legacyButton));
        
        // 5. Add new Command components
        auto commandButton = CommandBuilder("Button")
            .text("Command Button")
            .bindToState("ui.button.text", "text")
            .onClick([]() { qDebug() << "Command button clicked!"; })
            .build();
        hybrid->addCommand(commandButton);
        
        // 6. Enable synchronization
        hybrid->enableAutoSync(true);
        
        // 7. Convert to widget for display
        auto widget = hybrid->toWidget();
        
        // 8. Set up state management
        auto& stateAdapter = CommandStateManagerAdapter::instance();
        stateAdapter.setAutoSync(true);
        
        qDebug() << "Migration completed successfully!";
    }
};
```

## Troubleshooting

### Common Issues

1. **Conversion Failures**
   - Check if component type is registered
   - Verify adapter is properly initialized
   - Use IntegrationManager validation tools

2. **State Synchronization Issues**
   - Ensure state keys are consistent
   - Check binding directions
   - Verify StateManager integration

3. **Performance Problems**
   - Monitor conversion overhead
   - Use batch operations
   - Cache converted components

4. **JSON Loading Errors**
   - Validate JSON schema
   - Check component type mappings
   - Verify event handler registration

### Debug Tools

```cpp
// Enable debug logging
IntegrationManager::instance().setDebugMode(true);

// Monitor performance
IntegrationPerformanceMonitor::startMonitoring();
// ... perform operations ...
auto metrics = IntegrationPerformanceMonitor::getMetrics();
qDebug() << "Average conversion time:" << metrics.avg_conversion_time_ms << "ms";

// Check integration statistics
auto stats = IntegrationManager::instance().getStatistics();
qDebug() << "Commands created:" << stats.commands_created;
qDebug() << "Components converted:" << stats.components_converted;
```

This integration system provides a smooth migration path from the existing Component system to the new Command architecture while maintaining full backward compatibility and enabling hybrid applications.
