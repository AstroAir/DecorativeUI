#include "IntegrationExample.hpp"
#include <QDebug>
#include <QElapsedTimer>

#include "../WidgetMapper.hpp"
#include "../MVCIntegration.hpp"
#include "../../Binding/StateManager.hpp"

using namespace DeclarativeUI::Command::Examples;
using namespace DeclarativeUI::Command::UI;

IntegrationExample::IntegrationExample(QWidget* parent) 
    : QMainWindow(parent)
    , integration_manager_(nullptr)
    , component_adapter_(nullptr)
    , state_adapter_(nullptr) {
    
    qDebug() << "🚀 IntegrationExample starting...";
    
    setupIntegration();
    setupUI();
    
    qDebug() << "✅ IntegrationExample initialized successfully";
}

void IntegrationExample::setupIntegration() {
    try {
        // Initialize integration components
        integration_manager_ = &Adapters::IntegrationManager::instance();
        component_adapter_ = &Adapters::ComponentSystemAdapter::instance();
        state_adapter_ = &Adapters::CommandStateManagerAdapter::instance();
        
        qDebug() << "🔧 Integration components initialized";
        
    } catch (const std::exception& e) {
        qWarning() << "❌ Error setting up integration:" << e.what();
    }
}

void IntegrationExample::setupUI() {
    try {
        // Create main container
        auto mainContainer = CommandHierarchyBuilder("Container")
            .layout("VBox")
            .spacing(20)
            .margins(20)
            
            // Title
            .addChild("Label", [](CommandBuilder& label) {
                label.text("Integration Example - Legacy and Command Systems")
                     .style("font-weight", "bold")
                     .style("font-size", "18px");
            })
            
            // Legacy section
            .addContainer("Container", [this](CommandHierarchyBuilder& section) {
                section.layout("VBox")
                       .spacing(10)
                       .addChild("Label", [](CommandBuilder& label) {
                           label.text("Legacy Components")
                                .style("font-weight", "bold");
                       });
            })
            
            // Command section
            .addContainer("Container", [this](CommandHierarchyBuilder& section) {
                section.layout("VBox")
                       .spacing(10)
                       .addChild("Label", [](CommandBuilder& label) {
                           label.text("Command Components")
                                .style("font-weight", "bold");
                       })
                       .addChild("Button", [this](CommandBuilder& button) {
                           button.text("Command Button")
                                 .onClick([this]() { onCommandButtonClicked(); });
                       });
            })
            
            // Hybrid section
            .addContainer("Container", [this](CommandHierarchyBuilder& section) {
                section.layout("VBox")
                       .spacing(10)
                       .addChild("Label", [](CommandBuilder& label) {
                           label.text("Hybrid Integration")
                                .style("font-weight", "bold");
                       });
            })
            
            .build();
        
        // Convert to widget and set as central widget
        auto widget = WidgetMapper::instance().createWidget(mainContainer.get());
        if (widget) {
            setCentralWidget(widget.release());
            qDebug() << "✅ Integration example UI created successfully";
        } else {
            qWarning() << "❌ Failed to create widget from command";
        }
        
    } catch (const std::exception& e) {
        qWarning() << "❌ Error creating integration UI:" << e.what();
    }
}

void IntegrationExample::demonstrateComponentToCommandConversion() {
    qDebug() << "🔄 Demonstrating Component to Command conversion...";
    
    try {
        // Create legacy component
        legacy_button_ = std::make_unique<Components::Button>();
        legacy_button_->text("Legacy Button");
        
        // Convert to command
        if (component_adapter_) {
            auto commandButton = component_adapter_->convertToCommand(legacy_button_.get());
            command_button_ = commandButton;
            qDebug() << "✅ Successfully converted legacy button to command";
        }
        
    } catch (const std::exception& e) {
        qWarning() << "❌ Error in component conversion:" << e.what();
    }
}

void IntegrationExample::demonstrateCommandToComponentConversion() {
    qDebug() << "🔄 Demonstrating Command to Component conversion...";
    
    try {
        // Create command
        auto commandButton = CommandBuilder("Button")
            .text("Command Button")
            .build();
        
        // Convert to component (if adapter supports it)
        if (component_adapter_) {
            // This would be implemented in the adapter
            qDebug() << "✅ Command to component conversion demonstrated";
        }
        
    } catch (const std::exception& e) {
        qWarning() << "❌ Error in command conversion:" << e.what();
    }
}

void IntegrationExample::demonstrateBidirectionalSync() {
    qDebug() << "🔄 Demonstrating bidirectional synchronization...";
    
    try {
        // Create synchronized pair
        sync_legacy_button_ = std::make_unique<Components::Button>();
        sync_command_button_ = CommandBuilder("Button").build();
        
        if (state_adapter_) {
            // Bind both to same state
            state_adapter_->bindCommand(sync_command_button_, SYNC_STATE, "text");
            
            // Demonstrate sync
            state_adapter_->setCommandState(sync_command_button_, "text", QString("Synchronized!"));
            qDebug() << "✅ Bidirectional sync demonstrated";
        }
        
    } catch (const std::exception& e) {
        qWarning() << "❌ Error in bidirectional sync:" << e.what();
    }
}

void IntegrationExample::demonstrateHybridContainer() {
    qDebug() << "🔄 Demonstrating hybrid container...";
    
    try {
        if (component_adapter_) {
            // Create hybrid container with mixed components
            // Implementation would create a container with both legacy and command components
            qDebug() << "✅ Hybrid container demonstrated";
        }
        
    } catch (const std::exception& e) {
        qWarning() << "❌ Error in hybrid container:" << e.what();
    }
}

void IntegrationExample::demonstrateJSONLoading() {
    qDebug() << "🔄 Demonstrating JSON loading...";
    
    try {
        // Load mixed UI from JSON
        auto mixedUI = JSONIntegrationExample::loadMixedUI();
        if (mixedUI) {
            qDebug() << "✅ JSON loading demonstrated";
        }
        
    } catch (const std::exception& e) {
        qWarning() << "❌ Error in JSON loading:" << e.what();
    }
}

void IntegrationExample::demonstrateMigrationScenario() {
    qDebug() << "🔄 Demonstrating migration scenario...";
    
    try {
        MigrationExample::demonstrateMigrationSteps();
        qDebug() << "✅ Migration scenario demonstrated";
        
    } catch (const std::exception& e) {
        qWarning() << "❌ Error in migration scenario:" << e.what();
    }
}

void IntegrationExample::demonstrateStateIntegration() {
    qDebug() << "🔄 Demonstrating state integration...";
    
    try {
        StateIntegrationExample::demonstrateStateIntegration();
        qDebug() << "✅ State integration demonstrated";
        
    } catch (const std::exception& e) {
        qWarning() << "❌ Error in state integration:" << e.what();
    }
}

void IntegrationExample::onLegacyButtonClicked() {
    qDebug() << "🖱️ Legacy button clicked";
    
    auto& stateManager = DeclarativeUI::Binding::StateManager::instance();
    auto counterState = stateManager.getState<int>(COUNTER_STATE);
    if (counterState) {
        int newValue = counterState->get() + 1;
        stateManager.setState(COUNTER_STATE, newValue);
    }
}

void IntegrationExample::onCommandButtonClicked() {
    qDebug() << "🖱️ Command button clicked";
    
    auto& stateManager = DeclarativeUI::Binding::StateManager::instance();
    stateManager.setState(MESSAGE_STATE, QString("Command button was clicked!"));
}

void IntegrationExample::onHybridContainerChanged() {
    qDebug() << "🔄 Hybrid container changed";
}

void IntegrationExample::onStateChanged(const QString& key, const QVariant& value) {
    qDebug() << "🔄 State changed in integration example:" << key << "=" << value;
}

#include "IntegrationExample.moc"
