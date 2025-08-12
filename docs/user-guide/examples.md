# DeclarativeUI Examples

This document provides comprehensive examples demonstrating various features and patterns in DeclarativeUI, including both legacy components and the modern Command-based architecture. All examples are available in the `examples/` directory and are built automatically when `BUILD_EXAMPLES=ON`.

## Example Categories

The project includes 25+ examples organized into four categories:

### Basic Examples (`examples/basic/`)

- **01_hello_world.cpp** - Simple "Hello World" application demonstrating basic widget creation
- **02_json_ui_loading.cpp** - Loading UI from JSON files with dynamic creation
- **03_simple_state.cpp** - Basic state management with StateManager
- **04_basic_hot_reload.cpp** - Hot reload functionality setup
- **05_event_handling.cpp** - Event handling patterns and lambda functions
- **06_counter_app.cpp** - Interactive counter application with state updates
- **08_command_basics.cpp** - Command system introduction and basic usage
- **16_reactive_state.cpp** - Reactive state management with computed properties
- **21_file_watching.cpp** - File system monitoring with FileWatcher
- **22_hot_reload_example.cpp** - Advanced hot reload features with performance monitoring

### Component Examples (`examples/components/`)

- **06_basic_components.cpp** - Essential UI components (Button, Label, LineEdit, Container)
- **07_input_components.cpp** - Input component showcase (CheckBox, ComboBox, SpinBox, Slider)
- **08_comprehensive_components.cpp** - Complete tour of all 30+ components
- **09_enhanced_components.cpp** - Advanced component features (styling, validation, properties)
- **10_new_components.cpp** - Latest component additions (TableView, TreeView, Dialogs)

### Command System Examples (`examples/command/`)

- **CommandUIExample.cpp** - Command-based UI construction with CommandBuilder
- **IntegrationExample.cpp** - Legacy-Command integration with adapters
- **command_builder_example.cpp** - Command Builder patterns and fluent API
- **json_command_example.cpp** - JSON Command loading with dynamic commands
- **state_integration_example.cpp** - State management integration with Commands
- **main.cpp** - Complete Command system demonstration
- **integration_main.cpp** - Integration scenarios and migration strategies

### Advanced Examples (`examples/advanced/`)

- **26_command_system.cpp** - Comprehensive Command system demonstration
- **27_command_example.cpp** - Advanced command patterns and usage
- **28_advanced_example.cpp** - Advanced DeclarativeUI features and optimization
- **29_comprehensive_example.cpp** - Complete framework capabilities showcase
- **30_command_ui_builder.cpp** - Advanced Command Builder patterns
- **31_integration_example.cpp** - Complex integration scenarios
- **36_todo_app.cpp** - Complete todo application with CRUD operations
- **37_form_example.cpp** - Form handling and validation
- **38_settings_example.cpp** - Settings management application
- **39_dashboard_example.cpp** - Interactive dashboard with real-time updates

## Building and Running Examples

### Prerequisites

- Qt6 (6.2 or higher) with Core, Widgets, Network, and Test modules
- CMake 3.20 or higher
- C++20 compatible compiler

### Building Examples

```bash
# Build all examples (recommended)
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_EXAMPLES=ON -DBUILD_COMMAND_SYSTEM=ON -DBUILD_ADAPTERS=ON
cmake --build . --config Release

# Examples will be built to build/examples/
```

### Running Examples

```bash
# Run a basic example
./build/examples/basic/01_hello_world

# Run a component showcase
./build/examples/components/08_comprehensive_components

# Run a command system example
./build/examples/command/CommandUIExample

# Run an advanced application
./build/examples/advanced/36_todo_app
```

### Example Resources

Examples include JSON UI definitions in `examples/resources/`:

- `command_ui_example.json` - Command system UI definition
- `mixed_components_example.json` - Mixed Components/Commands example

## Command System Examples

The Command system provides a modern, declarative approach to UI development with automatic state management, event handling, and widget mapping.

### Simple Command Button Application

```cpp
#include <QApplication>
#include <QMainWindow>

#ifdef DECLARATIVE_UI_COMMAND_SYSTEM_ENABLED
#include "Command/CommandBuilder.hpp"
#include "Command/WidgetMapper.hpp"
#include "Binding/StateManager.hpp"

using namespace DeclarativeUI::Command;

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Create UI using CommandBuilder
    auto button = UI::CommandBuilder("Button")
        .text("Click Me!")
        .style("background-color: #3498db; color: white; padding: 15px 30px; border-radius: 8px; font-weight: bold;")
        .onClick([]() {
            QMessageBox::information(nullptr, "Success", "Command button clicked!");
        })
        .build();

    // Create widget from command
    auto widget = UI::WidgetMapper::instance().createWidget(button.get());
    widget->show();

    return app.exec();
}
#else
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    qWarning() << "Command system not enabled. Build with BUILD_COMMAND_SYSTEM=ON";
    return 1;
}
#endif
```

### Command Hierarchy Example

Create complex UIs using CommandHierarchyBuilder:

```cpp
#include <QApplication>
#include <QMainWindow>

#ifdef DECLARATIVE_UI_COMMAND_SYSTEM_ENABLED
#include "Command/CommandBuilder.hpp"
#include "Command/WidgetMapper.hpp"

using namespace DeclarativeUI::Command;

class CommandHierarchyExample : public QMainWindow {
    Q_OBJECT

public:
    CommandHierarchyExample(QWidget* parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("Command Hierarchy Example");
        setMinimumSize(400, 300);

        setupUI();
    }

private:
    void setupUI() {
        // Create complex UI using CommandHierarchyBuilder
        auto mainUI = UI::CommandHierarchyBuilder("Container")
            .layout("VBox")
            .spacing(20)
            .margins(30)
            .style("background-color: #f8f9fa;")

            // Header section
            .addChild("Label", [](UI::CommandBuilder& header) {
                header.text("🚀 Command Hierarchy Demo")
                      .style("font-size: 24px; font-weight: bold; color: #2c3e50; text-align: center;")
                      .alignment(Qt::AlignCenter);
            })

            // Button row
            .addChild("Container", [](UI::CommandBuilder& buttonRow) {
                buttonRow.layout("HBox")
                         .spacing(15)
                         .child(UI::CommandBuilder("Button")
                               .text("Primary")
                               .style("background-color: #3498db; color: white; padding: 12px 24px; border-radius: 6px;")
                               .onClick([]() { qDebug() << "Primary button clicked"; }))
                         .child(UI::CommandBuilder("Button")
                               .text("Secondary")
                               .style("background-color: #95a5a6; color: white; padding: 12px 24px; border-radius: 6px;")
                               .onClick([]() { qDebug() << "Secondary button clicked"; }))
                         .child(UI::CommandBuilder("Button")
                               .text("Success")
                               .style("background-color: #27ae60; color: white; padding: 12px 24px; border-radius: 6px;")
                               .onClick([]() { qDebug() << "Success button clicked"; }));
            })

            // Input section
            .addChild("Container", [](UI::CommandBuilder& inputSection) {
                inputSection.layout("Form")
                           .spacing(10)
                           .child(UI::CommandBuilder("TextInput")
                                 .placeholder("Enter your name")
                                 .style("padding: 10px; border: 1px solid #bdc3c7; border-radius: 4px;"))
                           .child(UI::CommandBuilder("TextInput")
                                 .placeholder("Enter your email")
                                 .style("padding: 10px; border: 1px solid #bdc3c7; border-radius: 4px;"));
            })

            // Footer
            .addChild("Label", [](UI::CommandBuilder& footer) {
                footer.text("Built with DeclarativeUI Command System")
                      .style("color: #7f8c8d; font-style: italic; text-align: center;")
                      .alignment(Qt::AlignCenter);
            })

            .build();

        // Create widget and set as central widget
        auto widget = UI::WidgetMapper::instance().createWidget(mainUI.get());
        setCentralWidget(widget.release());

        main_command_ = mainUI;
    }

private:
    std::shared_ptr<UI::BaseUICommand> main_command_;
};

#include "command_hierarchy_example.moc"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    CommandHierarchyExample window;
    window.show();

    return app.exec();
}
#else
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    qWarning() << "Command system not enabled. Build with BUILD_COMMAND_SYSTEM=ON";
    return 1;
}
#endif
```

### State Management with Commands

Demonstrate automatic state binding and reactive UI updates:

```cpp
#include <QApplication>
#include <QMainWindow>
#include <QTimer>

#ifdef DECLARATIVE_UI_COMMAND_SYSTEM_ENABLED
#include "Command/CommandBuilder.hpp"
#include "Command/WidgetMapper.hpp"
#include "Binding/StateManager.hpp"

using namespace DeclarativeUI::Command;
using namespace DeclarativeUI::Binding;

class StateManagementExample : public QMainWindow {
    Q_OBJECT

public:
    StateManagementExample(QWidget* parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("State Management Example");
        setMinimumSize(500, 400);

        setupState();
        setupUI();
        setupTimer();
    }

private:
    void setupState() {
        auto& state = StateManager::instance();

        // Initialize application state
        state.setState("counter", 0);
        state.setState("user.name", QString(""));
        state.setState("user.email", QString(""));
        state.setState("form.valid", false);
        state.setState("app.status", QString("Ready"));
        state.setState("theme", QString("light"));

        // Set up computed state
        state.observeState<int>("counter", [&state](int value) {
            state.setState("app.status", QString("Counter: %1").arg(value));
        });

        // Set up form validation
        state.observeState<QString>("user.name", [this](const QString&) { validateForm(); });
        state.observeState<QString>("user.email", [this](const QString&) { validateForm(); });

        qDebug() << "🌐 State initialized";
    }

    void setupUI() {
        auto mainUI = UI::CommandHierarchyBuilder("Container")
            .layout("VBox")
            .spacing(20)
            .margins(30)

            // Header with state binding
            .addChild("Label", [](UI::CommandBuilder& header) {
                header.text("📊 State Management Demo")
                      .style("font-size: 24px; font-weight: bold; color: #2c3e50; text-align: center;")
                      .alignment(Qt::AlignCenter);
            })

            // Status display (bound to state)
            .addChild("Label", [](UI::CommandBuilder& status) {
                status.bindToState("app.status", "text")
                      .style("font-size: 18px; color: #27ae60; text-align: center; padding: 10px; background-color: #d5f4e6; border-radius: 5px;")
                      .alignment(Qt::AlignCenter);
            })

            // Counter section
            .addChild("Container", [this](UI::CommandBuilder& counterSection) {
                counterSection.layout("VBox")
                             .spacing(15)
                             .style("border: 2px solid #3498db; border-radius: 8px; padding: 20px; background-color: #f8fbff;")

                             .child(UI::CommandBuilder("Label")
                                   .text("Counter Controls")
                                   .style("font-weight: bold; color: #3498db; font-size: 16px;")
                                   .alignment(Qt::AlignCenter))

                             .child(UI::CommandBuilder("Container")
                                   .layout("HBox")
                                   .spacing(10)
                                   .child(UI::CommandBuilder("Button")
                                         .text("-")
                                         .style("background-color: #e74c3c; color: white; padding: 10px 20px; border-radius: 5px; font-weight: bold;")
                                         .onClick([this]() { decrementCounter(); }))
                                   .child(UI::CommandBuilder("Button")
                                         .text("Reset")
                                         .style("background-color: #95a5a6; color: white; padding: 10px 20px; border-radius: 5px; font-weight: bold;")
                                         .onClick([this]() { resetCounter(); }))
                                   .child(UI::CommandBuilder("Button")
                                         .text("+")
                                         .style("background-color: #27ae60; color: white; padding: 10px 20px; border-radius: 5px; font-weight: bold;")
                                         .onClick([this]() { incrementCounter(); })));
            })

            // Form section with validation
            .addChild("Container", [](UI::CommandBuilder& formSection) {
                formSection.layout("VBox")
                          .spacing(15)
                          .style("border: 2px solid #9b59b6; border-radius: 8px; padding: 20px; background-color: #fdf7ff;")

                          .child(UI::CommandBuilder("Label")
                                .text("User Form (with State Binding)")
                                .style("font-weight: bold; color: #9b59b6; font-size: 16px;")
                                .alignment(Qt::AlignCenter))

                          .child(UI::CommandBuilder("TextInput")
                                .placeholder("Enter your name")
                                .bindToState("user.name", "text")
                                .style("padding: 10px; border: 1px solid #bdc3c7; border-radius: 4px;"))

                          .child(UI::CommandBuilder("TextInput")
                                .placeholder("Enter your email")
                                .bindToState("user.email", "text")
                                .style("padding: 10px; border: 1px solid #bdc3c7; border-radius: 4px;"))

                          .child(UI::CommandBuilder("Button")
                                .text("Submit Form")
                                .bindToState("form.valid", "enabled")
                                .style("background-color: #9b59b6; color: white; padding: 12px 24px; border-radius: 6px; font-weight: bold;")
                                .onClick([this]() { submitForm(); }));
            })

            .build();

        auto widget = UI::WidgetMapper::instance().createWidget(mainUI.get());
        setCentralWidget(widget.release());

        main_command_ = mainUI;
    }

    void setupTimer() {
        // Demonstrate automatic UI updates
        auto timer = new QTimer(this);
        connect(timer, &QTimer::timeout, [this]() {
            auto& state = StateManager::instance();
            auto counter = state.getState<int>("counter");
            if (counter && counter->get() < 100) {
                // Auto-increment occasionally to show reactive updates
                if (QRandomGenerator::global()->bounded(100) < 5) {
                    state.setState("counter", counter->get() + 1);
                }
            }
        });
        timer->start(1000); // Check every second
    }

    void incrementCounter() {
        auto& state = StateManager::instance();
        auto counter = state.getState<int>("counter");
        if (counter) {
            state.setState("counter", counter->get() + 1);
        }
    }

    void decrementCounter() {
        auto& state = StateManager::instance();
        auto counter = state.getState<int>("counter");
        if (counter && counter->get() > 0) {
            state.setState("counter", counter->get() - 1);
        }
    }

    void resetCounter() {
        auto& state = StateManager::instance();
        state.setState("counter", 0);
    }

    void validateForm() {
        auto& state = StateManager::instance();
        auto name = state.getState<QString>("user.name");
        auto email = state.getState<QString>("user.email");

        bool valid = name && email &&
                    !name->get().trimmed().isEmpty() &&
                    email->get().contains("@");

        state.setState("form.valid", valid);
    }

    void submitForm() {
        auto& state = StateManager::instance();
        auto name = state.getState<QString>("user.name")->get();
        auto email = state.getState<QString>("user.email")->get();

        QMessageBox::information(this, "Form Submitted",
                               QString("Name: %1\nEmail: %2").arg(name, email));

        // Clear form
        state.setState("user.name", QString(""));
        state.setState("user.email", QString(""));
    }

private:
    std::shared_ptr<UI::BaseUICommand> main_command_;
};

#include "state_management_example.moc"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    StateManagementExample window;
    window.show();

    return app.exec();
}
#else
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    qWarning() << "Command system not enabled. Build with BUILD_COMMAND_SYSTEM=ON";
    return 1;
}
#endif
```

### JSON Command Loading

Load UI definitions from JSON with automatic state binding and event handling:

#### JSON Definition (ui/dashboard.json)

```json
{
  "type": "Container",
  "properties": {
    "layout": "VBox",
    "spacing": 20,
    "margins": [30, 30, 30, 30]
  },
  "style": {
    "background-color": "#f8f9fa"
  },
  "children": [
    {
      "type": "Label",
      "properties": {
        "text": "📊 JSON Dashboard",
        "alignment": "center"
      },
      "style": {
        "font-size": "24px",
        "font-weight": "bold",
        "color": "#2c3e50",
        "margin-bottom": "20px"
      },
      "bindings": {
        "text": "dashboard.title"
      }
    },
    {
      "type": "Container",
      "properties": {
        "layout": "HBox",
        "spacing": 15
      },
      "children": [
        {
          "type": "Container",
          "properties": {
            "layout": "VBox",
            "spacing": 10
          },
          "style": {
            "border": "2px solid #3498db",
            "border-radius": "8px",
            "padding": "20px",
            "background-color": "#f8fbff"
          },
          "children": [
            {
              "type": "Label",
              "properties": {
                "text": "Statistics",
                "alignment": "center"
              },
              "style": {
                "font-weight": "bold",
                "color": "#3498db",
                "font-size": "16px"
              }
            },
            {
              "type": "Label",
              "id": "usersLabel",
              "bindings": {
                "text": "stats.users"
              },
              "style": {
                "font-size": "18px",
                "color": "#27ae60",
                "text-align": "center"
              }
            },
            {
              "type": "Label",
              "id": "revenueLabel",
              "bindings": {
                "text": "stats.revenue"
              },
              "style": {
                "font-size": "18px",
                "color": "#e74c3c",
                "text-align": "center"
              }
            }
          ]
        },
        {
          "type": "Container",
          "properties": {
            "layout": "VBox",
            "spacing": 10
          },
          "style": {
            "border": "2px solid #27ae60",
            "border-radius": "8px",
            "padding": "20px",
            "background-color": "#f8fff8"
          },
          "children": [
            {
              "type": "Label",
              "properties": {
                "text": "Actions",
                "alignment": "center"
              },
              "style": {
                "font-weight": "bold",
                "color": "#27ae60",
                "font-size": "16px"
              }
            },
            {
              "type": "Button",
              "properties": {
                "text": "📈 Generate Report"
              },
              "style": {
                "background-color": "#3498db",
                "color": "white",
                "padding": "12px 24px",
                "border-radius": "6px",
                "font-weight": "bold"
              },
              "events": {
                "click": "generateReport"
              }
            },
            {
              "type": "Button",
              "properties": {
                "text": "🔄 Refresh Data"
              },
              "style": {
                "background-color": "#27ae60",
                "color": "white",
                "padding": "12px 24px",
                "border-radius": "6px",
                "font-weight": "bold"
              },
              "events": {
                "click": "refreshData"
              }
            },
            {
              "type": "Button",
              "properties": {
                "text": "⚙️ Settings"
              },
              "style": {
                "background-color": "#95a5a6",
                "color": "white",
                "padding": "12px 24px",
                "border-radius": "6px",
                "font-weight": "bold"
              },
              "events": {
                "click": "openSettings"
              },
              "bindings": {
                "enabled": "ui.settingsEnabled"
              }
            }
          ]
        }
      ]
    },
    {
      "type": "Container",
      "properties": {
        "layout": "Form",
        "spacing": 10
      },
      "style": {
        "border": "2px solid #9b59b6",
        "border-radius": "8px",
        "padding": "20px",
        "background-color": "#fdf7ff"
      },
      "children": [
        {
          "type": "Label",
          "properties": {
            "text": "User Input",
            "alignment": "center"
          },
          "style": {
            "font-weight": "bold",
            "color": "#9b59b6",
            "font-size": "16px",
            "margin-bottom": "10px"
          }
        },
        {
          "type": "TextInput",
          "id": "nameInput",
          "properties": {
            "placeholder": "Enter your name"
          },
          "style": {
            "padding": "10px",
            "border": "1px solid #bdc3c7",
            "border-radius": "4px"
          },
          "bindings": {
            "text": "user.name"
          },
          "events": {
            "textChanged": "validateInput"
          }
        },
        {
          "type": "TextInput",
          "id": "emailInput",
          "properties": {
            "placeholder": "Enter your email"
          },
          "style": {
            "padding": "10px",
            "border": "1px solid #bdc3c7",
            "border-radius": "4px"
          },
          "bindings": {
            "text": "user.email"
          },
          "events": {
            "textChanged": "validateInput"
          }
        }
      ]
    }
  ]
}
```

#### C++ Application

```cpp
#include <QApplication>
#include <QMainWindow>
#include <QTimer>
#include <QRandomGenerator>

#ifdef DECLARATIVE_UI_COMMAND_SYSTEM_ENABLED
#include "Command/Adapters/JSONCommandLoader.hpp"
#include "Command/WidgetMapper.hpp"
#include "Binding/StateManager.hpp"

using namespace DeclarativeUI::Command;
using namespace DeclarativeUI::Binding;

class JSONDashboardExample : public QMainWindow {
    Q_OBJECT

public:
    JSONDashboardExample(QWidget* parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("JSON Dashboard Example");
        setMinimumSize(700, 500);

        setupState();
        setupEventHandlers();
        loadUI();
        setupDataSimulation();
    }

private:
    void setupState() {
        auto& state = StateManager::instance();

        // Initialize dashboard state
        state.setState("dashboard.title", QString("📊 JSON Dashboard"));
        state.setState("stats.users", QString("👥 Users: 1,234"));
        state.setState("stats.revenue", QString("💰 Revenue: $12,345"));
        state.setState("user.name", QString(""));
        state.setState("user.email", QString(""));
        state.setState("ui.settingsEnabled", true);

        qDebug() << "🌐 Dashboard state initialized";
    }

    void setupEventHandlers() {
        loader_ = std::make_unique<Adapters::JSONCommandLoader>();

        // Configure automatic features
        loader_->setAutoStateBinding(true);
        loader_->setAutoMVCIntegration(true);
        loader_->setAutoWidgetCreation(true);

        // Register event handlers
        loader_->registerEventHandler("generateReport", [this](const UI::CommandEvent& event) {
            generateReport();
        });

        loader_->registerEventHandler("refreshData", [this](const UI::CommandEvent& event) {
            refreshData();
        });

        loader_->registerEventHandler("openSettings", [this](const UI::CommandEvent& event) {
            openSettings();
        });

        loader_->registerEventHandler("validateInput", [this](const UI::CommandEvent& event) {
            validateInput();
        });

        qDebug() << "📡 Event handlers registered";
    }

    void loadUI() {
        try {
            // Load UI from JSON file
            auto [command, widget] = loader_->loadCommandWithWidgetFromFile("ui/dashboard.json");

            if (widget) {
                setCentralWidget(widget.release());
                main_command_ = command;
                qDebug() << "✅ JSON UI loaded successfully";
            } else {
                qWarning() << "❌ Failed to create widget from JSON";
                createFallbackUI();
            }

        } catch (const std::exception& e) {
            qWarning() << "❌ Error loading JSON UI:" << e.what();
            createFallbackUI();
        }
    }

    void createFallbackUI() {
        // Fallback UI if JSON loading fails
        auto fallbackLabel = new QLabel("Failed to load JSON UI. Check that ui/dashboard.json exists.");
        fallbackLabel->setAlignment(Qt::AlignCenter);
        fallbackLabel->setStyleSheet("color: #e74c3c; font-size: 16px; padding: 50px;");
        setCentralWidget(fallbackLabel);
    }

    void setupDataSimulation() {
        // Simulate changing data
        auto timer = new QTimer(this);
        connect(timer, &QTimer::timeout, [this]() {
            updateSimulatedData();
        });
        timer->start(3000); // Update every 3 seconds
    }

    void updateSimulatedData() {
        auto& state = StateManager::instance();

        // Simulate changing statistics
        static int users = 1234;
        static int revenue = 12345;

        users += QRandomGenerator::global()->bounded(10, 50);
        revenue += QRandomGenerator::global()->bounded(100, 500);

        state.setState("stats.users", QString("👥 Users: %1").arg(users));
        state.setState("stats.revenue", QString("💰 Revenue: $%1").arg(revenue));
    }

    void generateReport() {
        qDebug() << "📈 Generating report...";
        QMessageBox::information(this, "Report", "📊 Report generated successfully!\n\nThe report has been saved to reports/dashboard_report.pdf");
    }

    void refreshData() {
        qDebug() << "🔄 Refreshing data...";
        updateSimulatedData();
        QMessageBox::information(this, "Refresh", "🔄 Data refreshed successfully!");
    }

    void openSettings() {
        qDebug() << "⚙️ Opening settings...";
        QMessageBox::information(this, "Settings", "⚙️ Settings dialog would open here.\n\nThis demonstrates event handling from JSON-defined UI.");
    }

    void validateInput() {
        auto& state = StateManager::instance();
        auto name = state.getState<QString>("user.name");
        auto email = state.getState<QString>("user.email");

        bool hasName = name && !name->get().trimmed().isEmpty();
        bool hasEmail = email && email->get().contains("@");

        if (hasName && hasEmail) {
            qDebug() << "✅ Input validation passed";
        } else {
            qDebug() << "⚠️ Input validation: name=" << hasName << "email=" << hasEmail;
        }
    }

private:
    std::unique_ptr<Adapters::JSONCommandLoader> loader_;
    std::shared_ptr<UI::BaseUICommand> main_command_;
};

#include "json_dashboard_example.moc"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    JSONDashboardExample window;
    window.show();

    qDebug() << "💡 This example demonstrates:";
    qDebug() << "   - Loading UI from JSON definitions";
    qDebug() << "   - Automatic state binding";
    qDebug() << "   - Event handling from JSON";
    qDebug() << "   - Dynamic data updates";
    qDebug() << "   - Modern styling and layout";

    return app.exec();
}
#else
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    qWarning() << "Command system not enabled. Build with BUILD_COMMAND_SYSTEM=ON";
    return 1;
}
#endif
```

### Integration Example

Demonstrate integration between legacy components and the Command system:

```cpp
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>

// Legacy components
#include "Components/Button.hpp"
#include "Components/Widget.hpp"

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
using namespace DeclarativeUI::Binding;

class IntegrationExample : public QMainWindow {
    Q_OBJECT

public:
    IntegrationExample(QWidget* parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("Legacy-Command Integration Example");
        setMinimumSize(800, 600);

        initializeIntegration();
        setupState();
        setupUI();
    }

private:
    void initializeIntegration() {
        #ifdef DECLARATIVE_UI_ADAPTERS_ENABLED
        // Initialize integration manager
        auto& manager = Adapters::IntegrationManager::instance();
        manager.initialize();
        manager.setCompatibilityMode(Adapters::IntegrationManager::CompatibilityMode::Hybrid);

        qDebug() << "🌉 Integration manager initialized";
        #endif
    }

    void setupState() {
        auto& state = StateManager::instance();

        // Shared state between legacy and Command components
        state.setState("shared.counter", 0);
        state.setState("shared.message", QString("Integration Demo"));
        state.setState("shared.theme", QString("light"));

        qDebug() << "🌐 Shared state initialized";
    }

    void setupUI() {
        auto centralWidget = new QWidget;
        setCentralWidget(centralWidget);

        auto mainLayout = new QVBoxLayout(centralWidget);
        mainLayout->setSpacing(20);
        mainLayout->setContentsMargins(30, 30, 30, 30);

        // Header
        auto header = new QLabel("🔗 Legacy-Command Integration Demo");
        header->setStyleSheet("font-size: 24px; font-weight: bold; color: #2c3e50; text-align: center; padding: 15px; background-color: #ecf0f1; border-radius: 8px;");
        header->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(header);

        // Legacy section
        createLegacySection(mainLayout);

        // Command section
        createCommandSection(mainLayout);

        // Hybrid section
        createHybridSection(mainLayout);

        // State display
        createStateDisplay(mainLayout);

        qDebug() << "✅ Integration UI created";
    }

    void createLegacySection(QVBoxLayout* parentLayout) {
        auto legacyGroup = new QGroupBox("📦 Legacy Components");
        legacyGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #e74c3c; border: 2px solid #e74c3c; border-radius: 5px; margin: 5px; padding-top: 10px; }");

        auto legacyLayout = new QVBoxLayout(legacyGroup);

        // Legacy DeclarativeUI button
        legacy_button_ = std::make_unique<Button>();
        legacy_button_->text("Legacy Button")
                     .onClick([this]() {
                         onLegacyButtonClicked();
                     });

        legacy_button_->initialize();
        auto* legacyButtonWidget = legacy_button_->getWidget();
        legacyButtonWidget->setStyleSheet("QPushButton { background-color: #e74c3c; color: white; padding: 10px; border-radius: 5px; }");
        legacyLayout->addWidget(legacyButtonWidget);

        // Legacy Qt button
        legacy_qt_button_ = new QPushButton("Legacy Qt Button");
        legacy_qt_button_->setStyleSheet("QPushButton { background-color: #c0392b; color: white; padding: 10px; border-radius: 5px; }");
        connect(legacy_qt_button_, &QPushButton::clicked, this, &IntegrationExample::onLegacyQtButtonClicked);
        legacyLayout->addWidget(legacy_qt_button_);

        parentLayout->addWidget(legacyGroup);
    }

    void createCommandSection(QVBoxLayout* parentLayout) {
        try {
            // Create Command-based section
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
                         .bindToState("shared.message", "text")
                         .addEventListener("textChanged", "updateMessage");
                })

                .build();

            // Convert to widget
            auto commandWidget = UI::WidgetMapper::instance().createWidget(commandContainer.get());
            if (commandWidget) {
                parentLayout->addWidget(commandWidget.release());
                command_container_ = commandContainer;
            }

        } catch (const std::exception& e) {
            qWarning() << "❌ Error creating Command section:" << e.what();
        }
    }

    void createHybridSection(QVBoxLayout* parentLayout) {
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

            parentLayout->addWidget(hybridGroup);

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

        parentLayout->addWidget(hybridGroup);
        #endif
    }

    void createStateDisplay(QVBoxLayout* parentLayout) {
        auto stateGroup = new QGroupBox("🌐 Shared State Display");
        stateGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #27ae60; border: 2px solid #27ae60; border-radius: 5px; margin: 5px; padding-top: 10px; }");

        auto stateLayout = new QVBoxLayout(stateGroup);

        // Counter display
        counter_label_ = new QLabel("Counter: 0");
        counter_label_->setStyleSheet("font-size: 16px; color: #27ae60;");
        stateLayout->addWidget(counter_label_);

        // Message display
        message_label_ = new QLabel("Message: Integration Demo");
        message_label_->setStyleSheet("font-size: 16px; color: #27ae60;");
        stateLayout->addWidget(message_label_);

        // Bind legacy widgets to state
        auto& state = StateManager::instance();
        state.observeState<int>("shared.counter", [this](int value) {
            counter_label_->setText(QString("Counter: %1").arg(value));
        });

        state.observeState<QString>("shared.message", [this](const QString& value) {
            message_label_->setText(QString("Message: %1").arg(value));
        });

        parentLayout->addWidget(stateGroup);
    }

    // Event handlers
    void onLegacyButtonClicked() {
        auto& state = StateManager::instance();
        auto counter = state.getState<int>("shared.counter");
        if (counter) {
            state.setState("shared.counter", counter->get() + 1);
        }
        qDebug() << "🔴 Legacy button clicked";
    }

    void onLegacyQtButtonClicked() {
        auto& state = StateManager::instance();
        auto counter = state.getState<int>("shared.counter");
        if (counter) {
            state.setState("shared.counter", counter->get() + 10);
        }
        qDebug() << "🔴 Legacy Qt button clicked";
    }

    void onCommandButtonClicked() {
        auto& state = StateManager::instance();
        auto counter = state.getState<int>("shared.counter");
        if (counter) {
            state.setState("shared.counter", counter->get() + 100);
        }
        qDebug() << "🔵 Command button clicked";
    }

    void onHybridLegacyClicked() {
        qDebug() << "🟣 Hybrid legacy clicked";
        QMessageBox::information(this, "Hybrid", "Hybrid legacy component clicked!");
    }

    void onHybridCommandClicked() {
        qDebug() << "🟣 Hybrid command clicked";
        QMessageBox::information(this, "Hybrid", "Hybrid command component clicked!");
    }

private:
    // Legacy components
    std::unique_ptr<Button> legacy_button_;
    QPushButton* legacy_qt_button_;

    // Command components
    std::shared_ptr<UI::BaseUICommand> command_container_;

    // Hybrid components
    #ifdef DECLARATIVE_UI_ADAPTERS_ENABLED
    std::unique_ptr<Adapters::ComponentSystemAdapter::HybridContainer> hybrid_container_;
    #endif

    // UI elements
    QLabel* counter_label_;
    QLabel* message_label_;
};

#include "integration_example.moc"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    IntegrationExample window;
    window.show();

    qDebug() << "💡 This example demonstrates:";
    qDebug() << "   - Legacy DeclarativeUI components";
    qDebug() << "   - Legacy Qt widgets";
    qDebug() << "   - Modern Command system";
    qDebug() << "   - Hybrid containers";
    qDebug() << "   - Shared state management";
    qDebug() << "   - Integration patterns";

    return app.exec();
}
#else
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    qWarning() << "Command system not enabled. Build with BUILD_COMMAND_SYSTEM=ON";
    return 1;
}
#endif
```

## Legacy Component Examples

For reference, here are examples using the traditional DeclarativeUI component system:

### Simple Legacy Button Application

```cpp
#include <QApplication>
#include "Core/DeclarativeBuilder.hpp"
#include "Components/Button.hpp"
#include "Components/Label.hpp"

using namespace DeclarativeUI;

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    auto window = Core::create<QWidget>()
        .property("windowTitle", "Simple Button Example")
        .property("minimumSize", QSize(250, 150))
        .layout<QVBoxLayout>([](auto *layout) {
            layout->setSpacing(20);
            layout->setContentsMargins(20, 20, 20, 20);
        })
        .child<Components::Label>([](auto &label) {
            label.text("Click the button below!")
                 .style("QLabel { font-size: 14px; }");
        })
        .child<Components::Button>([](auto &button) {
            button.text("Click Me!")
                  .onClick([]() {
                      QMessageBox::information(nullptr, "Success", "Button clicked!");
                  });
        })
        .build();

    window->show();
    return app.exec();
}
```

### Legacy Form Input Example

```cpp
#include "Components/LineEdit.hpp"
#include "Components/CheckBox.hpp"
#include "Components/ComboBox.hpp"

class LegacyFormExample : public QWidget {
    Q_OBJECT

public:
    LegacyFormExample(QWidget *parent = nullptr) : QWidget(parent) {
        setupUI();
    }

private:
    void setupUI() {
        auto layout = new QFormLayout(this);

        // Name input
        name_edit_ = std::make_unique<Components::LineEdit>();
        name_edit_->placeholder("Enter your name...")
                  ->onTextChanged([this](const QString& text) {
                      validateForm();
                  });
        name_edit_->initialize();
        layout->addRow("Name:", name_edit_->getWidget());

        // Email input
        email_edit_ = std::make_unique<Components::LineEdit>();
        email_edit_->placeholder("Enter your email...")
                   ->onTextChanged([this](const QString& text) {
                       validateForm();
                   });
        email_edit_->initialize();
        layout->addRow("Email:", email_edit_->getWidget());

        // Country selection
        country_combo_ = std::make_unique<Components::ComboBox>();
        country_combo_->addItem("United States")
                      .addItem("Canada")
                      .addItem("United Kingdom")
                      .addItem("Germany")
                      .addItem("France");
        country_combo_->initialize();
        layout->addRow("Country:", country_combo_->getWidget());

        // Newsletter subscription
        newsletter_check_ = std::make_unique<Components::CheckBox>();
        newsletter_check_->text("Subscribe to newsletter");
        newsletter_check_->initialize();
        layout->addRow("", newsletter_check_->getWidget());

        // Submit button
        submit_button_ = std::make_unique<Components::Button>();
        submit_button_->text("Submit")
                     ->enabled(false)
                     ->onClick([this]() {
                         submitForm();
                     });
        submit_button_->initialize();
        layout->addRow("", submit_button_->getWidget());
    }

    void validateForm() {
        bool valid = !name_edit_->getText().isEmpty() &&
                    email_edit_->getText().contains("@");

        submit_button_->enabled(valid);
    }

    void submitForm() {
        QString message = QString("Form submitted!\nName: %1\nEmail: %2\nCountry: %3\nNewsletter: %4")
            .arg(name_edit_->getText())
            .arg(email_edit_->getText())
            .arg(country_combo_->getCurrentText())
            .arg(newsletter_check_->isChecked() ? "Yes" : "No");

        QMessageBox::information(this, "Form Submitted", message);
    }

private:
    std::unique_ptr<Components::LineEdit> name_edit_;
    std::unique_ptr<Components::LineEdit> email_edit_;
    std::unique_ptr<Components::ComboBox> country_combo_;
    std::unique_ptr<Components::CheckBox> newsletter_check_;
    std::unique_ptr<Components::Button> submit_button_;
};
```

## Comparison: Legacy vs Command System

### Feature Comparison

| Feature                 | Legacy Components        | Command System                 |
| ----------------------- | ------------------------ | ------------------------------ |
| **Declarative Syntax**  | ✅ Fluent API            | ✅ Enhanced fluent API         |
| **State Binding**       | ⚠️ Manual binding        | ✅ Automatic binding           |
| **Event Handling**      | ⚠️ Qt signals/slots      | ✅ Abstract event system       |
| **JSON Loading**        | ✅ Basic JSON support    | ✅ Enhanced JSON with bindings |
| **Widget Independence** | ❌ Inherits from QWidget | ✅ Complete abstraction        |
| **Hot Reload**          | ✅ Supported             | ✅ Enhanced support            |
| **Performance**         | ✅ Good                  | ✅ Optimized                   |
| **Migration Path**      | N/A                      | ✅ Hybrid mode available       |

### When to Use Each Approach

#### Use Legacy Components When

- Working with existing DeclarativeUI codebases
- Need proven stability
- Simple UI requirements
- Team familiarity with traditional patterns

#### Use Command System When

- Starting new projects
- Need advanced state management
- Want modern declarative patterns
- Require widget independence
- Building complex, reactive UIs

### Migration Example

Here's how to migrate from legacy to Command system:

```cpp
// Legacy approach
class LegacyCounter : public QWidget {
public:
    LegacyCounter() {
        auto layout = new QVBoxLayout(this);

        // Manual widget creation and management
        counter_label_ = new QLabel("Count: 0");
        layout->addWidget(counter_label_);

        auto button = new QPushButton("Increment");
        connect(button, &QPushButton::clicked, [this]() {
            count_++;
            counter_label_->setText(QString("Count: %1").arg(count_));
        });
        layout->addWidget(button);
    }

private:
    QLabel* counter_label_;
    int count_ = 0;
};

// Command system approach
#ifdef DECLARATIVE_UI_COMMAND_SYSTEM_ENABLED
class ModernCounter : public QMainWindow {
public:
    ModernCounter() {
        setupState();
        setupUI();
    }

private:
    void setupState() {
        auto& state = StateManager::instance();
        state.setState("counter", 0);
    }

    void setupUI() {
        auto ui = UI::CommandHierarchyBuilder("Container")
            .layout("VBox")
            .spacing(20)
            .margins(30)

            .addChild("Label", [](UI::CommandBuilder& label) {
                label.bindToState("counter", "text")
                     .style("font-size: 24px; font-weight: bold;");
            })

            .addChild("Button", [this](UI::CommandBuilder& button) {
                button.text("Increment")
                      .onClick([this]() {
                          auto& state = StateManager::instance();
                          auto counter = state.getState<int>("counter");
                          if (counter) {
                              state.setState("counter", counter->get() + 1);
                          }
                      });
            })

            .build();

        auto widget = UI::WidgetMapper::instance().createWidget(ui.get());
        setCentralWidget(widget.release());
    }
};
#endif
```

## Best Practices

### Command System Best Practices

1. **Use State Binding**: Leverage automatic state synchronization

   ```cpp
   auto input = CommandBuilder("TextInput")
       .bindToState("user.name", "text")
       .build();
   ```

2. **Organize with Hierarchy Builders**: Structure complex UIs

   ```cpp
   auto form = CommandHierarchyBuilder("Container")
       .layout("Form")
       .addChild("TextInput", [](CommandBuilder& input) {
           input.placeholder("Name").bindToState("user.name", "text");
       })
       .build();
   ```

3. **Use JSON for Dynamic UIs**: Enable runtime UI changes

   ```cpp
   JSONCommandLoader loader;
   auto ui = loader.loadCommandFromFile("ui/dynamic.json");
   ```

4. **Handle Events Properly**: Use the Command event system

   ```cpp
   auto button = CommandBuilder("Button")
       .addEventListener("click", [](const CommandEvent& event) {
           // Handle event with full context
       })
       .build();
   ```

### Legacy Component Best Practices

1. **Initialize Components**: Always call initialize()

   ```cpp
   auto button = std::make_unique<Components::Button>();
   button->text("Click Me");
   button->initialize(); // Required!
   ```

2. **Manage Lifecycle**: Properly manage component lifecycles

   ```cpp
   class MyWidget : public QWidget {
   private:
       std::vector<std::unique_ptr<Components::Button>> buttons_;
   };
   ```

3. **Use Fluent API**: Chain method calls for readability
   ```cpp
   button->text("Save")
         .enabled(true)
         .onClick([this]() { save(); });
   ```

## Summary

The DeclarativeUI framework provides both traditional component-based development and modern Command-based architecture:

- **Legacy Components**: Proven, stable, widget-based approach
- **Command System**: Modern, reactive, widget-independent architecture
- **Integration**: Seamless migration path with hybrid mode
- **Flexibility**: Choose the right approach for your project needs

Both approaches support the core DeclarativeUI principles of declarative syntax, state management, and rapid development, with the Command system providing enhanced capabilities for modern application development.

## JSON UI Examples

### JSON Configuration File

Create `ui/dashboard.json`:

```json
{
  "type": "QWidget",
  "properties": {
    "windowTitle": "Dashboard Example",
    "minimumSize": [600, 400],
    "styleSheet": "QWidget { background-color: #f5f5f5; }"
  },
  "layout": {
    "type": "VBoxLayout",
    "spacing": 20,
    "margins": [20, 20, 20, 20]
  },
  "children": [
    {
      "type": "QLabel",
      "properties": {
        "text": "📊 Dashboard",
        "alignment": 4,
        "styleSheet": "QLabel { font-size: 24px; font-weight: bold; color: #2c3e50; }"
      }
    },
    {
      "type": "QWidget",
      "layout": {
        "type": "HBoxLayout",
        "spacing": 15
      },
      "children": [
        {
          "type": "QGroupBox",
          "properties": {
            "title": "Statistics",
            "styleSheet": "QGroupBox { font-weight: bold; }"
          },
          "layout": {
            "type": "VBoxLayout",
            "spacing": 10
          },
          "children": [
            {
              "type": "QLabel",
              "id": "users_count",
              "bindings": {
                "text": "users_count"
              },
              "properties": {
                "styleSheet": "QLabel { font-size: 18px; color: #27ae60; }"
              }
            },
            {
              "type": "QLabel",
              "id": "revenue_label",
              "bindings": {
                "text": "revenue_text"
              },
              "properties": {
                "styleSheet": "QLabel { font-size: 18px; color: #e74c3c; }"
              }
            }
          ]
        },
        {
          "type": "QGroupBox",
          "properties": {
            "title": "Actions",
            "styleSheet": "QGroupBox { font-weight: bold; }"
          },
          "layout": {
            "type": "VBoxLayout",
            "spacing": 10
          },
          "children": [
            {
              "type": "QPushButton",
              "properties": {
                "text": "📈 Generate Report",
                "minimumHeight": 40,
                "styleSheet": "QPushButton { background-color: #3498db; color: white; font-weight: bold; border-radius: 5px; }"
              },
              "events": {
                "clicked": "generateReport"
              }
            },
            {
              "type": "QPushButton",
              "properties": {
                "text": "🔄 Refresh Data",
                "minimumHeight": 40,
                "styleSheet": "QPushButton { background-color: #2ecc71; color: white; font-weight: bold; border-radius: 5px; }"
              },
              "events": {
                "clicked": "refreshData"
              }
            }
          ]
        }
      ]
    }
  ]
}
```

### Loading JSON UI with State Binding

```cpp
#include "JSON/JSONUILoader.hpp"
#include "Binding/StateManager.hpp"

class DashboardApp : public QObject {
    Q_OBJECT

public:
    DashboardApp() {
        setupState();
        setupUI();
    }

private:
    void setupState() {
        auto& state = StateManager::instance();

        // Initialize dashboard state
        state.setState("users_count", QString("👥 Users: 1,234"));
        state.setState("revenue_text", QString("💰 Revenue: $12,345"));

        // Simulate data updates
        timer_ = new QTimer(this);
        connect(timer_, &QTimer::timeout, this, &DashboardApp::updateData);
        timer_->start(5000); // Update every 5 seconds
    }

    void setupUI() {
        ui_loader_ = std::make_unique<JSON::JSONUILoader>();

        // Bind state manager
        ui_loader_->bindStateManager(
            std::shared_ptr<Binding::StateManager>(&StateManager::instance(), [](auto*){}));

        // Register event handlers
        ui_loader_->registerEventHandler("generateReport", [this]() {
            generateReport();
        });

        ui_loader_->registerEventHandler("refreshData", [this]() {
            refreshData();
        });

        // Load UI from JSON
        main_widget_ = ui_loader_->loadFromFile("ui/dashboard.json");
        if (main_widget_) {
            main_widget_->show();
        }
    }

private slots:
    void updateData() {
        auto& state = StateManager::instance();

        // Simulate changing data
        static int users = 1234;
        static int revenue = 12345;

        users += QRandomGenerator::global()->bounded(10, 50);
        revenue += QRandomGenerator::global()->bounded(100, 500);

        state.setState("users_count", QString("👥 Users: %1").arg(users));
        state.setState("revenue_text", QString("💰 Revenue: $%1").arg(revenue));
    }

    void generateReport() {
        QMessageBox::information(main_widget_.get(), "Report",
                               "📊 Report generated successfully!");
    }

    void refreshData() {
        updateData();
        QMessageBox::information(main_widget_.get(), "Refresh",
                               "🔄 Data refreshed!");
    }

private:
    std::unique_ptr<JSON::JSONUILoader> ui_loader_;
    std::unique_ptr<QWidget> main_widget_;
    QTimer* timer_;
};
```

## Hot Reload Example

### Development Setup with Hot Reload

```cpp
#include "HotReload/HotReloadManager.hpp"

class HotReloadApp : public QObject {
    Q_OBJECT

public:
    HotReloadApp() {
        setupUI();
        setupHotReload();
    }

private:
    void setupUI() {
        ui_loader_ = std::make_unique<JSON::JSONUILoader>();

        // Register event handlers
        ui_loader_->registerEventHandler("saveFile", [this]() {
            QMessageBox::information(main_widget_.get(), "Save", "File saved!");
        });

        ui_loader_->registerEventHandler("openFile", [this]() {
            QMessageBox::information(main_widget_.get(), "Open", "File opened!");
        });

        // Load initial UI
        main_widget_ = ui_loader_->loadFromFile("ui/editor.json");
        if (main_widget_) {
            main_widget_->show();
        }
    }

    void setupHotReload() {
        hot_reload_manager_ = std::make_unique<HotReload::HotReloadManager>();

        // Set up error handling
        hot_reload_manager_->setErrorHandler(
            [this](const QString& file, const QString& error) {
                qWarning() << "Hot reload failed for" << file << ":" << error;
                QMessageBox::warning(main_widget_.get(), "Hot Reload Error",
                                   QString("Failed to reload %1:\n%2").arg(file, error));
            });

        // Register UI file for hot reloading
        hot_reload_manager_->registerUIFile("ui/editor.json", main_widget_.get());

        // Connect hot reload signals
        connect(hot_reload_manager_.get(),
                &HotReload::HotReloadManager::reloadStarted,
                this, &HotReloadApp::onReloadStarted);

        connect(hot_reload_manager_.get(),
                &HotReload::HotReloadManager::reloadCompleted,
                this, &HotReloadApp::onReloadCompleted);

        qDebug() << "🔥 Hot reload enabled for ui/editor.json";
        qDebug() << "🔥 Edit the JSON file to see live updates!";
    }

private slots:
    void onReloadStarted(const QString& file_path) {
        qDebug() << "🔥 Reloading:" << file_path;
    }

    void onReloadCompleted(const QString& file_path) {
        qDebug() << "✅ Reload completed:" << file_path;

        // Reload the UI
        try {
            auto new_widget = ui_loader_->loadFromFile(file_path);
            if (new_widget) {
                main_widget_->close();
                main_widget_ = std::move(new_widget);
                main_widget_->show();

                // Re-register for hot reload
                hot_reload_manager_->registerUIFile(file_path, main_widget_.get());
            }
        } catch (const std::exception& e) {
            qCritical() << "Failed to reload UI:" << e.what();
        }
    }

private:
    std::unique_ptr<JSON::JSONUILoader> ui_loader_;
    std::unique_ptr<QWidget> main_widget_;
    std::unique_ptr<HotReload::HotReloadManager> hot_reload_manager_;
};
```

## Advanced Examples

### Custom Component Creation

```cpp
// Custom component that combines multiple widgets
class SearchBox : public Core::UIElement {
    Q_OBJECT

public:
    explicit SearchBox(QObject* parent = nullptr) : UIElement(parent) {}

    SearchBox& placeholder(const QString& text) {
        placeholder_text_ = text;
        return *this;
    }

    SearchBox& onSearch(std::function<void(const QString&)> handler) {
        search_handler_ = std::move(handler);
        return *this;
    }

    void initialize() override {
        auto container = new QWidget();
        auto layout = new QHBoxLayout(container);
        layout->setContentsMargins(0, 0, 0, 0);

        // Search input
        search_input_ = new QLineEdit();
        search_input_->setPlaceholderText(placeholder_text_);
        layout->addWidget(search_input_);

        // Search button
        search_button_ = new QPushButton("🔍");
        search_button_->setMaximumWidth(40);
        layout->addWidget(search_button_);

        // Connect signals
        connect(search_button_, &QPushButton::clicked, this, &SearchBox::performSearch);
        connect(search_input_, &QLineEdit::returnPressed, this, &SearchBox::performSearch);

        setWidget(container);
    }

private slots:
    void performSearch() {
        if (search_handler_) {
            search_handler_(search_input_->text());
        }
    }

private:
    QString placeholder_text_;
    std::function<void(const QString&)> search_handler_;
    QLineEdit* search_input_ = nullptr;
    QPushButton* search_button_ = nullptr;
};
```

These examples demonstrate the key features and patterns in DeclarativeUI. Start with the basic examples and gradually work your way up to more complex scenarios involving state management, JSON UIs, and hot reload functionality.
