#pragma once

#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <memory>

#include "../../Components/Button.hpp"
#include "../../Components/Widget.hpp"
#include "../../Core/UIElement.hpp"
#include "../Adapters/ComponentSystemAdapter.hpp"
#include "../Adapters/IntegrationManager.hpp"
#include "../Adapters/JSONCommandLoader.hpp"
#include "../Adapters/StateManagerAdapter.hpp"
#include "../Adapters/UIElementAdapter.hpp"
#include "../CommandBuilder.hpp"
#include "Command/UICommand.hpp"

namespace DeclarativeUI::Command::Examples {

/**
 * @brief Comprehensive example demonstrating integration between Command system
 * and existing components
 *
 * This example shows how to:
 * 1. Use existing Components alongside new Commands
 * 2. Convert between Components and Commands
 * 3. Establish bidirectional synchronization
 * 4. Load UI from JSON with mixed component types
 * 5. Migrate existing code to use Commands
 */
class IntegrationExample : public QMainWindow {
    Q_OBJECT

public:
    explicit IntegrationExample(QWidget* parent = nullptr);
    virtual ~IntegrationExample() = default;

    // **Example scenarios**
    void demonstrateComponentToCommandConversion();
    void demonstrateCommandToComponentConversion();
    void demonstrateBidirectionalSync();
    void demonstrateHybridContainer();
    void demonstrateJSONLoading();
    void demonstrateMigrationScenario();
    void demonstrateStateIntegration();

private slots:
    void onLegacyButtonClicked();
    void onCommandButtonClicked();
    void onHybridContainerChanged();
    void onStateChanged(const QString& key, const QVariant& value);

private:
    void setupUI();
    void setupIntegration();
    void createLegacySection();
    void createCommandSection();
    void createHybridSection();
    void createMigrationSection();

    // **Integration components**
    Adapters::IntegrationManager* integration_manager_;
    Adapters::ComponentSystemAdapter* component_adapter_;
    Adapters::CommandStateManagerAdapter* state_adapter_;

    // **UI components (mixed legacy and command)**
    std::unique_ptr<Components::Button> legacy_button_;
    std::shared_ptr<UI::BaseUICommand> command_button_;
    std::unique_ptr<Adapters::ComponentSystemAdapter::HybridContainer>
        hybrid_container_;

    // **Synchronized pairs**
    std::unique_ptr<Components::Button> sync_legacy_button_;
    std::shared_ptr<UI::BaseUICommand> sync_command_button_;

    // **State keys**
    static constexpr const char* COUNTER_STATE = "integration.counter";
    static constexpr const char* MESSAGE_STATE = "integration.message";
    static constexpr const char* SYNC_STATE = "integration.sync_value";
};

/**
 * @brief Example of gradual migration from Component to Command system
 */
class MigrationExample {
public:
    // **Step 1: Original Component-based code**
    static std::unique_ptr<QWidget> createLegacyUI() {
        auto container = std::make_unique<QWidget>();
        auto layout = new QVBoxLayout(container.get());

        // Create legacy components
        auto button = new QPushButton("Legacy Button");
        auto label = new QLabel("Legacy Label");
        auto input = new QLineEdit();
        input->setPlaceholderText("Legacy Input");

        layout->addWidget(label);
        layout->addWidget(input);
        layout->addWidget(button);

        // Legacy event handling
        QObject::connect(button, &QPushButton::clicked, [label, input]() {
            label->setText(QString("Input: %1").arg(input->text()));
        });

        return container;
    }

    // **Step 2: Hybrid approach - wrap legacy components in Commands**
    static std::shared_ptr<UI::BaseUICommand> createHybridUI() {
        using namespace Adapters;

        // Create legacy components
        auto legacyButton = std::make_unique<Components::Button>();
        legacyButton->text("Hybrid Button");

        // Convert to Commands
        auto buttonCommand =
            ComponentSystemAdapter::instance().convertToCommand(
                legacyButton.get());

        // Build hybrid UI using CommandBuilder
        auto container =
            UI::CommandBuilder("Container")
                .layout("VBox")
                .spacing(10)
                .child(UI::CommandBuilder("Label").text("Hybrid Label"))
                .child(UI::CommandBuilder("TextInput")
                           .placeholder("Hybrid Input")
                           .bindToState("hybrid.input"))
                .child(buttonCommand)  // Add converted legacy component
                .build();

        return container;
    }

    // **Step 3: Full Command-based approach**
    static std::shared_ptr<UI::BaseUICommand> createCommandUI() {
        return UI::CommandHierarchyBuilder("Container")
            .layout("VBox")
            .spacing(10)
            .addChild("Label",
                      [](UI::CommandBuilder& label) {
                          label.text("Command Label")
                              .bindToState("command.label", "text");
                      })
            .addChild("TextInput",
                      [](UI::CommandBuilder& input) {
                          input.placeholder("Command Input")
                              .bindToState("command.input")
                              .onTextChanged([](const QString& text) {
                                  // Update label through state
                                  Adapters::State::set(
                                      nullptr, "command.label",
                                      QString("Input: %1").arg(text));
                              });
                      })
            .addChild("Button",
                      [](UI::CommandBuilder& button) {
                          button.text("Command Button")
                              .onClick([]() {
                                  qDebug() << "Command button clicked!";
                              })
                              .registerAsAction("command.button.click");
                      })
            .build();
    }

    // **Migration utility functions**
    static void demonstrateMigrationSteps() {
        qDebug() << "=== Migration Demonstration ===";

        // Step 1: Create legacy UI
        qDebug() << "Step 1: Creating legacy UI...";
        auto legacyUI = createLegacyUI();
        qDebug() << "Legacy UI created with" << legacyUI->children().size()
                 << "children";

        // Step 2: Create hybrid UI
        qDebug() << "Step 2: Creating hybrid UI...";
        auto hybridUI = createHybridUI();
        qDebug() << "Hybrid UI created with command type:"
                 << hybridUI->getCommandType();

        // Step 3: Create full command UI
        qDebug() << "Step 3: Creating command UI...";
        auto commandUI = createCommandUI();
        qDebug() << "Command UI created with" << commandUI->getChildren().size()
                 << "children";

        // Demonstrate conversion
        qDebug() << "Step 4: Converting legacy to command...";
        auto& adapter = Adapters::ComponentSystemAdapter::instance();
        auto convertedCommands =
            adapter.convertComponentHierarchy(legacyUI.get());
        qDebug() << "Converted" << convertedCommands.size()
                 << "components to commands";
    }
};

/**
 * @brief Example of JSON loading with mixed component types
 */
class JSONIntegrationExample {
public:
    // **JSON definition with mixed component types**
    static QString getMixedComponentJSON() {
        return R"JSON({
            "type": "Container",
            "properties": {
                "layout": "VBox",
                "spacing": 15,
                "margins": 20
            },
            "children": [
                {
                    "type": "Label",
                    "properties": {
                        "text": "Mixed Component Example",
                        "style": {"font-weight": "bold", "font-size": "16px"}
                    }
                },
                {
                    "type": "AdaptedButton",
                    "properties": {
                        "text": "Legacy Button (Adapted)",
                        "enabled": true
                    },
                    "events": {
                        "clicked": "onLegacyButtonClicked"
                    },
                    "bindings": {
                        "enabled": "ui.buttons.enabled"
                    }
                },
                {
                    "type": "Button",
                    "properties": {
                        "text": "Command Button",
                        "icon": ":/icons/command.png"
                    },
                    "events": {
                        "clicked": "onCommandButtonClicked"
                    },
                    "actions": ["ui.button.command"]
                },
                {
                    "type": "Container",
                    "properties": {
                        "layout": "HBox",
                        "spacing": 10
                    },
                    "children": [
                        {
                            "type": "TextInput",
                            "properties": {
                                "placeholder": "Enter text...",
                                "maxLength": 100
                            },
                            "bindings": {
                                "text": "form.input.text"
                            },
                            "validation": {
                                "required": true,
                                "minLength": 3
                            }
                        },
                        {
                            "type": "Button",
                            "properties": {
                                "text": "Submit",
                                "enabled": false
                            },
                            "bindings": {
                                "enabled": "form.input.valid"
                            },
                            "events": {
                                "clicked": "onSubmitClicked"
                            }
                        }
                    ]
                }
            ]
        })JSON";
    }

    // **Load and demonstrate mixed JSON**
    static std::shared_ptr<DeclarativeUI::Command::UI::BaseUICommand>
    loadMixedUI() {
        DeclarativeUI::Command::Adapters::JSONCommandLoader loader;

        // Configure loader for mixed components
        loader.setAutoMVCIntegration(true);
        loader.setAutoStateBinding(true);
        loader.setAutoEventHandling(true);

        // Register event handlers
        loader.registerEventHandler(
            "onLegacyButtonClicked", [](const QVariant&) {
                qDebug() << "Legacy button clicked through JSON!";
            });

        loader.registerEventHandler(
            "onCommandButtonClicked", [](const QVariant&) {
                qDebug() << "Command button clicked through JSON!";
            });

        loader.registerEventHandler("onSubmitClicked", [](const QVariant&) {
            qDebug() << "Submit button clicked through JSON!";
        });

        // Load the UI
        QString jsonString = getMixedComponentJSON();
        return loader.loadCommandFromString(jsonString);
    }
};

/**
 * @brief Example of state integration between systems
 */
class StateIntegrationExample {
public:
    static void demonstrateStateIntegration() {
        using namespace DeclarativeUI::Command::Adapters;

        qDebug() << "=== State Integration Demonstration ===";

        // Create mixed UI components
        auto legacyButton =
            std::make_unique<DeclarativeUI::Components::Button>();
        legacyButton->text("Legacy Button");

        auto commandButton =
            DeclarativeUI::Command::UI::CommandBuilder("Button")
                .text("Command Button")
                .build();

        // Get state adapter
        auto& stateAdapter = DeclarativeUI::Command::Adapters::
            CommandStateManagerAdapter::instance();

        // Bind both to the same state
        stateAdapter.bindCommand(commandButton, "shared.button.text", "text");

        // Convert legacy button to command and bind
        auto adaptedLegacyButton =
            ComponentSystemAdapter::instance().convertToCommand(
                legacyButton.get());
        stateAdapter.bindCommand(adaptedLegacyButton, "shared.button.text",
                                 "text");

        // Demonstrate state synchronization
        qDebug() << "Setting shared state...";
        stateAdapter.setCommandState(commandButton, "text",
                                     QString("Synchronized Text"));

        // Both buttons should now have the same text
        qDebug() << "Command button text:"
                 << commandButton->getState()->getProperty<QString>("text");
        qDebug() << "Adapted legacy button text:"
                 << adaptedLegacyButton->getState()->getProperty<QString>(
                        "text");

        // Demonstrate batch updates
        qDebug() << "Demonstrating batch updates...";
        stateAdapter.beginBatchUpdate();
        stateAdapter.setCommandState(commandButton, "text",
                                     QString("Batch Update 1"));
        stateAdapter.setCommandState(commandButton, "enabled", false);
        stateAdapter.commitBatchUpdate();

        qDebug() << "Batch update completed";
    }
};

/**
 * @brief Performance comparison between systems
 */
class PerformanceComparisonExample {
public:
    static void comparePerformance() {
        qDebug() << "=== Performance Comparison ===";

        const int iterations = 1000;
        QElapsedTimer timer;

        // Test legacy component creation
        timer.start();
        for (int i = 0; i < iterations; ++i) {
            auto button = std::make_unique<DeclarativeUI::Components::Button>();
            button->text(QString("Button %1").arg(i));
        }
        qint64 legacyTime = timer.elapsed();

        // Test command creation
        timer.restart();
        for (int i = 0; i < iterations; ++i) {
            auto button = DeclarativeUI::Command::UI::CommandBuilder("Button")
                              .text(QString("Button %1").arg(i))
                              .build();
        }
        qint64 commandTime = timer.elapsed();

        // Test conversion overhead
        timer.restart();
        for (int i = 0; i < iterations; ++i) {
            auto legacyButton =
                std::make_unique<DeclarativeUI::Components::Button>();
            auto commandButton = DeclarativeUI::Command::Adapters::
                                     ComponentSystemAdapter::instance()
                                         .convertToCommand(legacyButton.get());
        }
        qint64 conversionTime = timer.elapsed();

        qDebug() << "Legacy component creation:" << legacyTime << "ms for"
                 << iterations << "iterations";
        qDebug() << "Command creation:" << commandTime << "ms for" << iterations
                 << "iterations";
        qDebug() << "Conversion overhead:" << conversionTime << "ms for"
                 << iterations << "iterations";
        qDebug() << "Command overhead:"
                 << ((double)commandTime / legacyTime - 1.0) * 100 << "%";
        qDebug() << "Conversion overhead:"
                 << ((double)conversionTime / legacyTime - 1.0) * 100 << "%";
    }
};

}  // namespace DeclarativeUI::Command::Examples
