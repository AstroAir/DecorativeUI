/**
 * @file 02_json_ui_loading.cpp
 * @brief Basic JSON UI loading without hot reload
 * 
 * This example demonstrates:
 * - Loading UI from JSON files
 * - Basic JSONUILoader usage
 * - Event handler registration
 * - Fallback UI creation
 * 
 * Learning objectives:
 * - Understand how to load UI from JSON
 * - Learn event handler registration
 * - See error handling and fallback strategies
 */

#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QWidget>
#include <QMessageBox>
#include <QDebug>
#include <QFileInfo>

// Include DeclarativeUI components
#include "JSON/JSONUILoader.hpp"
#include "JSON/ComponentRegistry.hpp"

#ifdef DECLARATIVE_UI_COMMAND_SYSTEM_ENABLED
#ifdef DECLARATIVE_UI_ADAPTERS_ENABLED
#include "Command/Adapters/JSONCommandLoader.hpp"
#include "Command/WidgetMapper.hpp"
#include "Binding/StateManager.hpp"
#endif
#endif

using namespace DeclarativeUI;

/**
 * @brief JSON UI Loading demonstration
 */
class JSONUIApp : public QObject {
    Q_OBJECT

public:
    JSONUIApp(QObject* parent = nullptr) : QObject(parent) {
        setupUILoader();
        createUI();
    }

    void show() {
        if (main_widget_) {
            main_widget_->show();
        }
    }

private slots:
    void onGreetClicked() {
        // This handler will be called from JSON-defined button
        QMessageBox::information(
            main_widget_.get(),
            "JSON UI Event",
            "🎉 This button was defined in JSON!\n\n"
            "The UI structure came from a JSON file,\n"
            "but this event handler was registered in C++."
        );
        
        qDebug() << "Greet button clicked from JSON UI!";
    }

    void onClearClicked() {
        // Find and clear text fields
        auto text_edit = main_widget_->findChild<QTextEdit*>("messageText");
        auto line_edit = main_widget_->findChild<QLineEdit*>("nameInput");
        
        if (text_edit) {
            text_edit->clear();
            qDebug() << "Text area cleared";
        }
        
        if (line_edit) {
            line_edit->clear();
            qDebug() << "Name input cleared";
        }
    }

    void onShowInfoClicked() {
        QMessageBox::information(
            main_widget_.get(),
            "JSON UI Info",
            "<h3>JSON UI Loading Example</h3>"
            "<p>This UI was loaded from: <code>resources/basic_ui.json</code></p>"
            "<p><b>Key concepts:</b></p>"
            "<ul>"
            "<li>Declarative UI definition in JSON</li>"
            "<li>Event handler registration</li>"
            "<li>Component property configuration</li>"
            "<li>Fallback UI for error cases</li>"
            "</ul>"
        );
    }

    void onReloadUIClicked() {
        // Demonstrate reloading the UI
        try {
            auto new_widget = ui_loader_->loadFromFile("resources/basic_ui.json");
            if (new_widget) {
                // Replace the current widget
                main_widget_ = std::move(new_widget);
                main_widget_->show();
                
                QMessageBox::information(
                    main_widget_.get(),
                    "UI Reloaded",
                    "✅ UI successfully reloaded from JSON!"
                );
                
                qDebug() << "UI reloaded successfully";
            }
        } catch (const std::exception& e) {
            QMessageBox::warning(
                main_widget_.get(),
                "Reload Failed",
                QString("❌ Failed to reload UI:\n%1").arg(e.what())
            );
        }
    }

#ifdef DECLARATIVE_UI_COMMAND_SYSTEM_ENABLED
#ifdef DECLARATIVE_UI_ADAPTERS_ENABLED
    // Command system event handlers
    void onCommandGreetClicked() {
        auto& stateManager = Binding::StateManager::instance();
        auto nameState = stateManager.getState<QString>("user.name");
        QString name = nameState ? nameState->get() : "World";

        QString message = QString("🎉 Hello, %1!\n\nThis button was created using the Command system!").arg(name);
        stateManager.setState("app.message", message);

        QMessageBox::information(
            main_widget_.get(),
            "Command System Event",
            message
        );

        qDebug() << "⚡ Command greet button clicked for:" << name;
    }

    void onCommandClearClicked() {
        auto& stateManager = Binding::StateManager::instance();
        stateManager.setState("user.name", QString(""));
        stateManager.setState("app.message", QString("All fields cleared via Command system"));

        QMessageBox::information(
            main_widget_.get(),
            "Command System",
            "🧹 Fields cleared using Command system state management!"
        );

        qDebug() << "⚡ Command clear button clicked";
    }

    void onCommandShowInfoClicked() {
        QMessageBox::information(
            main_widget_.get(),
            "Command System Info",
            "ℹ️ <b>Command System Features:</b><br><br>"
            "<ul>"
            "<li>Declarative UI construction</li>"
            "<li>State management integration</li>"
            "<li>Event handling system</li>"
            "<li>Widget mapping</li>"
            "<li>JSON loading with Commands</li>"
            "<li>Legacy component integration</li>"
            "</ul>"
        );

        qDebug() << "⚡ Command info button clicked";
    }
#endif
#endif

private:
    std::unique_ptr<JSON::JSONUILoader> ui_loader_;
    std::unique_ptr<QWidget> main_widget_;

#ifdef DECLARATIVE_UI_COMMAND_SYSTEM_ENABLED
#ifdef DECLARATIVE_UI_ADAPTERS_ENABLED
    std::unique_ptr<Command::Adapters::JSONCommandLoader> command_loader_;
#endif
#endif

    void setupUILoader() {
        ui_loader_ = std::make_unique<JSON::JSONUILoader>();

        // Register event handlers that can be called from JSON
        ui_loader_->registerEventHandler("greetUser", [this]() {
            onGreetClicked();
        });

        ui_loader_->registerEventHandler("clearFields", [this]() {
            onClearClicked();
        });

        ui_loader_->registerEventHandler("showInfo", [this]() {
            onShowInfoClicked();
        });

        ui_loader_->registerEventHandler("reloadUI", [this]() {
            onReloadUIClicked();
        });

#ifdef DECLARATIVE_UI_COMMAND_SYSTEM_ENABLED
#ifdef DECLARATIVE_UI_ADAPTERS_ENABLED
        // Set up Command system JSON loader
        command_loader_ = std::make_unique<Command::Adapters::JSONCommandLoader>();
        command_loader_->setAutoMVCIntegration(true);
        command_loader_->setAutoStateBinding(true);
        command_loader_->setAutoEventHandling(true);

        // Register Command system event handlers
        command_loader_->registerEventHandler("greetUserCommand", [this](const QVariant&) {
            onCommandGreetClicked();
        });

        command_loader_->registerEventHandler("clearFieldsCommand", [this](const QVariant&) {
            onCommandClearClicked();
        });

        command_loader_->registerEventHandler("showInfoCommand", [this](const QVariant&) {
            onCommandShowInfoClicked();
        });

        // Initialize state manager
        auto& stateManager = Binding::StateManager::instance();
        stateManager.setState("app.title", QString("JSON UI Loading with Commands"));
        stateManager.setState("user.name", QString(""));
        stateManager.setState("app.message", QString("Welcome to enhanced JSON loading!"));

        qDebug() << "⚡ Command system JSON loader configured";
#endif
#endif

        qDebug() << "✅ Event handlers registered";
    }

    void createUI() {
        try {
            QString ui_file = "resources/basic_ui.json";
            
            // Check if JSON file exists
            if (!QFileInfo::exists(ui_file)) {
                qWarning() << "JSON file not found:" << ui_file;
                qWarning() << "Creating fallback UI instead";
                main_widget_ = createFallbackUI();
                return;
            }

            // Load UI from JSON
            main_widget_ = ui_loader_->loadFromFile(ui_file);
            
            if (!main_widget_) {
                throw std::runtime_error("Failed to load UI from JSON");
            }

            main_widget_->setWindowTitle("02 - JSON UI Loading | DeclarativeUI");
            qDebug() << "✅ UI loaded from JSON:" << ui_file;

        } catch (const std::exception& e) {
            qCritical() << "JSON UI loading failed:" << e.what();
            qDebug() << "Creating fallback UI...";
            main_widget_ = createFallbackUI();
        }
    }

    std::unique_ptr<QWidget> createFallbackUI() {
        auto widget = std::make_unique<QWidget>();
        widget->setWindowTitle("02 - JSON UI Loading (Fallback) | DeclarativeUI");
        widget->setMinimumSize(500, 400);

        auto layout = new QVBoxLayout(widget.get());
        layout->setSpacing(15);
        layout->setContentsMargins(20, 20, 20, 20);

        // Header
        auto header = new QLabel("📄 JSON UI Loading Example");
        header->setStyleSheet("font-size: 20px; font-weight: bold; color: #2c3e50;");
        header->setAlignment(Qt::AlignCenter);
        layout->addWidget(header);

        // Status
        auto status = new QLabel("⚠️ Fallback UI (JSON file not found)");
        status->setStyleSheet("color: #e67e22; font-style: italic;");
        status->setAlignment(Qt::AlignCenter);
        layout->addWidget(status);

        // Description
        auto description = new QLabel(
            "This fallback UI demonstrates error handling.\n"
            "The JSON file 'resources/basic_ui.json' was not found,\n"
            "so we're showing this programmatically created UI instead."
        );
        description->setWordWrap(true);
        description->setAlignment(Qt::AlignCenter);
        layout->addWidget(description);

        layout->addStretch();

        // Sample form
        auto form_layout = new QVBoxLayout();
        
        auto name_label = new QLabel("Name:");
        auto name_input = new QLineEdit();
        name_input->setPlaceholderText("Enter your name...");
        
        auto message_label = new QLabel("Message:");
        auto message_text = new QTextEdit();
        message_text->setPlaceholderText("Enter a message...");
        message_text->setMaximumHeight(100);

        form_layout->addWidget(name_label);
        form_layout->addWidget(name_input);
        form_layout->addWidget(message_label);
        form_layout->addWidget(message_text);

        layout->addLayout(form_layout);

        // Buttons
        auto button_layout = new QHBoxLayout();
        
        auto greet_button = new QPushButton("👋 Greet");
        auto clear_button = new QPushButton("🗑️ Clear");
        auto info_button = new QPushButton("ℹ️ Info");

        connect(greet_button, &QPushButton::clicked, this, &JSONUIApp::onGreetClicked);
        connect(clear_button, &QPushButton::clicked, this, &JSONUIApp::onClearClicked);
        connect(info_button, &QPushButton::clicked, this, &JSONUIApp::onShowInfoClicked);

        button_layout->addWidget(greet_button);
        button_layout->addWidget(clear_button);
        button_layout->addWidget(info_button);
        button_layout->addStretch();

        layout->addLayout(button_layout);
        layout->addStretch();

        qDebug() << "✅ Fallback UI created";
        return widget;
    }
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    app.setApplicationName("DeclarativeUI JSON Loading");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("DeclarativeUI Examples");

    try {
        qDebug() << "🚀 Starting JSON UI Loading example...";
        
        JSONUIApp json_app;
        json_app.show();

        qDebug() << "💡 This example demonstrates:";
        qDebug() << "   - Loading UI from JSON files";
        qDebug() << "   - Event handler registration";
        qDebug() << "   - Error handling with fallback UI";

#ifdef DECLARATIVE_UI_COMMAND_SYSTEM_ENABLED
#ifdef DECLARATIVE_UI_ADAPTERS_ENABLED
        qDebug() << "   - Command system JSON loading (enabled)";
        qDebug() << "   - State management integration";
        qDebug() << "   - Mixed legacy/command components";
#else
        qDebug() << "   - Command system JSON loading (build with BUILD_ADAPTERS=ON)";
#endif
#else
        qDebug() << "   - Command system JSON loading (build with BUILD_COMMAND_SYSTEM=ON)";
#endif

        return app.exec();

    } catch (const std::exception& e) {
        qCritical() << "❌ Application error:" << e.what();
        return -1;
    }
}

#include "02_json_ui_loading.moc"
