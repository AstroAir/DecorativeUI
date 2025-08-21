#include <QApplication>
#include <QDebug>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

#ifdef DECLARATIVE_UI_COMMAND_SYSTEM_ENABLED
#ifdef DECLARATIVE_UI_ADAPTERS_ENABLED

#include "Command/Adapters/JSONCommandLoader.hpp"
#include "Command/CommandSystem.hpp"

using namespace DeclarativeUI::Command;
using namespace DeclarativeUI::Command::Adapters;

class JSONCommandExampleWindow : public QMainWindow {
    Q_OBJECT

public:
    JSONCommandExampleWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("JSON Command Loading Example");
        setMinimumSize(600, 500);

        setupUI();
        setupJSONExample();
    }

private:
    void setupUI() {
        try {
            // Create UI demonstrating JSON command loading concepts
            auto* central_widget = new QWidget();
            setCentralWidget(central_widget);

            auto* layout = new QVBoxLayout(central_widget);
            layout->setSpacing(15);
            layout->setContentsMargins(25, 25, 25, 25);

            // Title
            auto* title = new QLabel("JSON Command Loading Example");
            title->setStyleSheet(
                "font-weight: bold; font-size: 18px; color: #2c3e50;");
            layout->addWidget(title);

            // Description
            auto* desc = new QLabel(
                "This demonstrates loading UI definitions from JSON using the "
                "Command system");
            desc->setStyleSheet("color: #666; margin-bottom: 20px;");
            desc->setWordWrap(true);
            layout->addWidget(desc);

            // JSON display
            auto* json_group = new QGroupBox("Sample JSON Definition");
            auto* json_layout = new QVBoxLayout(json_group);

            json_display_ = new QTextEdit();
            json_display_->setMaximumHeight(200);
            json_display_->setStyleSheet(
                "font-family: monospace; background-color: #f8f9fa;");
            json_layout->addWidget(json_display_);

            layout->addWidget(json_group);

            // Load button
            auto* button_group = new QGroupBox("JSON Loading");
            auto* button_layout = new QHBoxLayout(button_group);

            auto* load_button = new QPushButton("Load JSON UI");
            connect(load_button, &QPushButton::clicked, this,
                    &JSONCommandExampleWindow::onLoadJSON);
            button_layout->addWidget(load_button);

            auto* clear_button = new QPushButton("Clear");
            connect(clear_button, &QPushButton::clicked, this,
                    &JSONCommandExampleWindow::onClear);
            button_layout->addWidget(clear_button);

            layout->addWidget(button_group);

            // Status
            status_label_ = new QLabel("Status: Ready to load JSON");
            status_label_->setStyleSheet(
                "padding: 10px; background-color: #f8f9fa; border: 1px solid "
                "#dee2e6;");
            layout->addWidget(status_label_);

            layout->addStretch();

            qDebug() << "✅ JSON Command UI created successfully";

        } catch (const std::exception& e) {
            qWarning() << "❌ Error creating JSON Command UI:" << e.what();
        }
    }

    void setupJSONExample() {
        // Sample JSON for demonstration
        sample_json_ = R"({
    "type": "Container",
    "properties": {
        "layout": "VBox",
        "spacing": 10,
        "margins": 20
    },
    "children": [
        {
            "type": "Label",
            "properties": {
                "text": "Hello from JSON!",
                "style": "font-weight: bold; color: #2c3e50;"
            }
        },
        {
            "type": "Button",
            "properties": {
                "text": "JSON Button"
            },
            "events": {
                "clicked": "onJSONButtonClicked"
            }
        },
        {
            "type": "TextInput",
            "properties": {
                "placeholder": "Enter text here...",
                "maxLength": 100
            }
        }
    ]
})";

        // Display the sample JSON
        json_display_->setPlainText(sample_json_);

        qDebug() << "🔧 JSON example configured";
    }

private slots:
    void onLoadJSON() {
        qDebug() << "📄 Loading JSON UI...";
        status_label_->setText("Status: Loading JSON UI definition...");

        // In a real implementation, this would use JSONCommandLoader
        // For now, we'll simulate the loading process
        status_label_->setText(
            "Status: JSON UI loaded successfully! (simulated)");
    }

    void onClear() {
        qDebug() << "🗑️ Clearing JSON display...";
        json_display_->clear();
        status_label_->setText("Status: JSON display cleared");
    }

private:
    QTextEdit* json_display_;
    QLabel* status_label_;
    QString sample_json_;
};

#endif  // DECLARATIVE_UI_ADAPTERS_ENABLED
#endif  // DECLARATIVE_UI_COMMAND_SYSTEM_ENABLED

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

#ifdef DECLARATIVE_UI_COMMAND_SYSTEM_ENABLED
#ifdef DECLARATIVE_UI_ADAPTERS_ENABLED
    qDebug() << "🚀 Starting JSON Command Loading Example";

    JSONCommandExampleWindow window;
    window.show();

    return app.exec();
#else
    qWarning()
        << "❌ Adapters not enabled. Please build with BUILD_ADAPTERS=ON";

    // Show a simple message for users
    QWidget window;
    window.setWindowTitle("Adapters Not Available");
    auto* layout = new QVBoxLayout(&window);
    auto* label = new QLabel(
        "The Command System Adapters are not enabled in this build.\n\n"
        "To enable them, build with:\n"
        "cmake -DBUILD_ADAPTERS=ON ..");
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("padding: 20px; font-size: 14px;");
    layout->addWidget(label);
    window.resize(400, 200);
    window.show();

    return app.exec();
#endif
#else
    qWarning() << "❌ Command system not enabled. Please build with "
                  "BUILD_COMMAND_SYSTEM=ON";

    // Show a simple message for users
    QWidget window;
    window.setWindowTitle("Command System Not Available");
    auto* layout = new QVBoxLayout(&window);
    auto* label = new QLabel(
        "The Command System is not enabled in this build.\n\n"
        "To enable it, build with:\n"
        "cmake -DBUILD_COMMAND_SYSTEM=ON ..");
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("padding: 20px; font-size: 14px;");
    layout->addWidget(label);
    window.resize(400, 200);
    window.show();

    return app.exec();
#endif
}

#include "json-commands.moc"
