#include <QApplication>
#include <QMainWindow>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>

#ifdef DECLARATIVE_UI_COMMAND_SYSTEM_ENABLED
#ifdef DECLARATIVE_UI_ADAPTERS_ENABLED

#include "../../Command/Adapters/JSONCommandLoader.hpp"
#include "../../Command/WidgetMapper.hpp"

using namespace DeclarativeUI::Command;
using namespace DeclarativeUI::Command::Adapters;

class JSONCommandExampleWindow : public QMainWindow {
    Q_OBJECT

public:
    JSONCommandExampleWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("JSON Command Loading Example");
        setMinimumSize(600, 500);
        
        setupUI();
    }

private:
    void setupUI() {
        try {
            // Create JSON definition for UI
            QString jsonUI = R"({
                "type": "Container",
                "properties": {
                    "layout": "VBox",
                    "spacing": 15,
                    "margins": 25
                },
                "children": [
                    {
                        "type": "Label",
                        "properties": {
                            "text": "JSON Command Loading Example",
                            "style": "font-weight: bold; font-size: 18px;"
                        }
                    },
                    {
                        "type": "Label",
                        "properties": {
                            "text": "This UI was loaded from JSON using the Command system",
                            "style": "color: gray; font-style: italic;"
                        }
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
                                    "placeholder": "Enter your name...",
                                    "maxLength": 50
                                },
                                "bindings": {
                                    "text": "user.name"
                                }
                            },
                            {
                                "type": "Button",
                                "properties": {
                                    "text": "Greet"
                                },
                                "events": {
                                    "clicked": "onGreetClicked"
                                }
                            }
                        ]
                    },
                    {
                        "type": "Label",
                        "properties": {
                            "text": "Hello, World!",
                            "style": "font-size: 14px; padding: 10px; background-color: #f0f0f0;"
                        },
                        "bindings": {
                            "text": "greeting.message"
                        }
                    },
                    {
                        "type": "Container",
                        "properties": {
                            "layout": "HBox",
                            "spacing": 5
                        },
                        "children": [
                            {
                                "type": "Button",
                                "properties": {
                                    "text": "Button 1"
                                },
                                "events": {
                                    "clicked": "onButton1Clicked"
                                }
                            },
                            {
                                "type": "Button",
                                "properties": {
                                    "text": "Button 2"
                                },
                                "events": {
                                    "clicked": "onButton2Clicked"
                                }
                            },
                            {
                                "type": "Button",
                                "properties": {
                                    "text": "Button 3"
                                },
                                "events": {
                                    "clicked": "onButton3Clicked"
                                }
                            }
                        ]
                    }
                ]
            })";

            // Create JSON loader
            JSONCommandLoader loader;
            
            // Configure loader
            loader.setAutoMVCIntegration(true);
            loader.setAutoStateBinding(true);
            loader.setAutoEventHandling(true);
            
            // Register event handlers
            loader.registerEventHandler("onGreetClicked", [this](const QVariant&) {
                auto& stateManager = DeclarativeUI::Binding::StateManager::instance();
                auto nameState = stateManager.getState<QString>("user.name");
                QString name = nameState ? nameState->get() : "World";
                
                QString greeting = QString("Hello, %1!").arg(name);
                stateManager.setState("greeting.message", greeting);
                
                qDebug() << "👋 Greeting:" << greeting;
            });
            
            loader.registerEventHandler("onButton1Clicked", [](const QVariant&) {
                qDebug() << "🔘 Button 1 clicked via JSON event handler";
            });
            
            loader.registerEventHandler("onButton2Clicked", [](const QVariant&) {
                qDebug() << "🔘 Button 2 clicked via JSON event handler";
            });
            
            loader.registerEventHandler("onButton3Clicked", [](const QVariant&) {
                qDebug() << "🔘 Button 3 clicked via JSON event handler";
            });
            
            // Initialize state
            auto& stateManager = DeclarativeUI::Binding::StateManager::instance();
            stateManager.setState("user.name", QString(""));
            stateManager.setState("greeting.message", QString("Hello, World!"));
            
            // Load command from JSON
            auto command = loader.loadCommandFromString(jsonUI);
            if (!command) {
                qWarning() << "❌ Failed to load command from JSON";
                return;
            }
            
            // Convert to widget
            auto widget = UI::WidgetMapper::instance().createWidget(command.get());
            if (widget) {
                setCentralWidget(widget.release());
                qDebug() << "✅ JSON Command example UI created successfully";
                
                // Monitor state changes
                connect(&stateManager, &DeclarativeUI::Binding::StateManager::stateChanged,
                        this, [](const QString& key, const QVariant& value) {
                            qDebug() << "🔄 State changed:" << key << "=" << value;
                        });
                
            } else {
                qWarning() << "❌ Failed to create widget from command";
            }
            
        } catch (const std::exception& e) {
            qWarning() << "❌ Error creating JSON Command example:" << e.what();
        }
    }
};

#include "json_command_example.moc"

#endif // DECLARATIVE_UI_ADAPTERS_ENABLED
#endif // DECLARATIVE_UI_COMMAND_SYSTEM_ENABLED

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

#ifdef DECLARATIVE_UI_COMMAND_SYSTEM_ENABLED
#ifdef DECLARATIVE_UI_ADAPTERS_ENABLED
    qDebug() << "🚀 Starting JSON Command Loading Example";
    
    JSONCommandExampleWindow window;
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
