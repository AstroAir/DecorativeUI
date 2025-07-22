#include <QApplication>
#include <QMainWindow>
#include <QDebug>

#ifdef DECLARATIVE_UI_COMMAND_SYSTEM_ENABLED
#ifdef DECLARATIVE_UI_ADAPTERS_ENABLED

#include "Command/CommandBuilder.hpp"
#include "../../Command/Adapters/StateManagerAdapter.hpp"
#include "../../Binding/StateManager.hpp"

using namespace DeclarativeUI::Command;
using namespace DeclarativeUI::Command::Adapters;

class StateIntegrationExampleWindow : public QMainWindow {
    Q_OBJECT

public:
    StateIntegrationExampleWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("State Integration Example");
        setMinimumSize(500, 400);
        
        setupStateManager();
        setupUI();
    }

private:
    void setupStateManager() {
        // Initialize state manager
        auto& stateManager = DeclarativeUI::Binding::StateManager::instance();
        
        // Set initial state values
        stateManager.setState("counter", 0);
        stateManager.setState("message", QString("Hello, Command System!"));
        stateManager.setState("enabled", true);
        
        qDebug() << "🌐 State manager initialized";
    }
    
    void setupUI() {
        try {
            auto& stateAdapter = CommandStateManagerAdapter::instance();
            
            // Create counter button
            auto counterButton = UI::CommandBuilder("Button")
                .text("Count: 0")
                .onClick([&stateAdapter]() {
                    auto& stateManager = DeclarativeUI::Binding::StateManager::instance();
                    auto counterState = stateManager.getState<int>("counter");
                    if (counterState) {
                        int newValue = counterState->get() + 1;
                        stateManager.setState("counter", newValue);
                        qDebug() << "Counter incremented to:" << newValue;
                    }
                })
                .build();
            
            // Bind counter button text to state
            stateAdapter.bindCommand(counterButton, "counter", "text");
            
            // Create message label
            auto messageLabel = UI::CommandBuilder("Label")
                .text("Hello, Command System!")
                .build();
            
            // Bind message label to state
            stateAdapter.bindCommand(messageLabel, "message", "text");
            
            // Create input field
            auto messageInput = UI::CommandBuilder("TextInput")
                .placeholder("Enter new message...")
                .onTextChanged([](const QString& text) {
                    auto& stateManager = DeclarativeUI::Binding::StateManager::instance();
                    stateManager.setState("message", text);
                    qDebug() << "Message updated to:" << text;
                })
                .build();
            
            // Create toggle button
            auto toggleButton = UI::CommandBuilder("Button")
                .text("Toggle Enabled")
                .onClick([]() {
                    auto& stateManager = DeclarativeUI::Binding::StateManager::instance();
                    auto enabledState = stateManager.getState<bool>("enabled");
                    if (enabledState) {
                        bool newValue = !enabledState->get();
                        stateManager.setState("enabled", newValue);
                        qDebug() << "Enabled toggled to:" << newValue;
                    }
                })
                .build();
            
            // Create main container
            auto mainContainer = UI::CommandBuilder("Container")
                .property("layout", "VBox")
                .property("spacing", 15)
                .property("margins", 20)
                .child(UI::CommandBuilder("Label")
                    .text("State Integration Example")
                    .style("font-weight", "bold")
                    .style("font-size", "18px"))
                .child(messageLabel)
                .child(messageInput)
                .child(counterButton)
                .child(toggleButton)
                .child(UI::CommandBuilder("Label")
                    .text("All components are synchronized through the state manager")
                    .style("font-style", "italic")
                    .style("color", "gray"))
                .build();

            // Convert to widget and set as central widget
            auto widget = UI::WidgetMapper::instance().createWidget(mainContainer.get());
            if (widget) {
                setCentralWidget(widget.release());
                qDebug() << "✅ State integration example UI created successfully";
                
                // Set up state change monitoring
                connect(&DeclarativeUI::Binding::StateManager::instance(), &DeclarativeUI::Binding::StateManager::stateChanged,
                        this, [this](const QString& key, const QVariant& value) {
                            qDebug() << "🔄 State changed:" << key << "=" << value;
                            
                            // Update counter button text
                            if (key == "counter") {
                                // The binding should handle this automatically
                            }
                        });
                
            } else {
                qWarning() << "❌ Failed to create widget from command";
            }
        } catch (const std::exception& e) {
            qWarning() << "❌ Error creating state integration example:" << e.what();
        }
    }
};

#include "state_integration_example.moc"

#endif // DECLARATIVE_UI_ADAPTERS_ENABLED
#endif // DECLARATIVE_UI_COMMAND_SYSTEM_ENABLED

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

#ifdef DECLARATIVE_UI_COMMAND_SYSTEM_ENABLED
#ifdef DECLARATIVE_UI_ADAPTERS_ENABLED
    qDebug() << "🚀 Starting State Integration Example";
    
    StateIntegrationExampleWindow window;
    window.show();
    
    return app.exec();
#else
    qWarning() << "❌ Adapters not enabled. Please build with BUILD_ADAPTERS=ON";
    return 1;
#endif
#else
    qWarning() << "❌ Command system not enabled. Please build with BUILD_COMMAND_SYSTEM=ON";
    return 1;
#endif
}
