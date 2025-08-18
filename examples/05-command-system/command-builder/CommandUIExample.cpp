/**
 * @file CommandUIExample.cpp
 * @brief Example demonstrating UI construction using Commands
 */

#include <QApplication>
#include <QDebug>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QCheckBox>
#include <QGroupBox>
#include <QMenuBar>
#include <QStatusBar>

#ifdef DECLARATIVE_UI_COMMAND_SYSTEM_ENABLED

#include "Command/CommandBuilder.hpp"
#include "Command/CommandSystem.hpp"

using namespace DeclarativeUI::Command;

class CommandUIExample : public QMainWindow {
    Q_OBJECT

public:
    CommandUIExample(QWidget* parent = nullptr) : QMainWindow(parent) {
        qDebug() << "🚀 Command UI Example starting...";

        setupUI();
        setupCommands();
        setupEventHandling();
        createMenuBar();
        createStatusBar();

        qDebug() << "✅ Command UI Example initialized successfully";
    }

private:

    void setupUI() {
        try {
            // Create UI demonstrating command-based construction concepts
            auto* central_widget = new QWidget();
            setCentralWidget(central_widget);

            auto* layout = new QVBoxLayout(central_widget);
            layout->setSpacing(15);
            layout->setContentsMargins(25, 25, 25, 25);

            // Title
            auto* title = new QLabel("Command-Based UI Construction");
            title->setStyleSheet("font-weight: bold; font-size: 18px; color: #2c3e50;");
            layout->addWidget(title);

            // Description
            auto* desc = new QLabel("This demonstrates UI construction patterns using the Command system");
            desc->setStyleSheet("color: #666; margin-bottom: 20px;");
            desc->setWordWrap(true);
            layout->addWidget(desc);

            // Button container
            auto* button_group = new QGroupBox("Command Buttons");
            auto* button_layout = new QHBoxLayout(button_group);
            button_layout->setSpacing(10);

            auto* button1 = new QPushButton("Command Button 1");
            connect(button1, &QPushButton::clicked, this, &CommandUIExample::onCommandButton1Clicked);
            button_layout->addWidget(button1);

            auto* button2 = new QPushButton("Command Button 2");
            connect(button2, &QPushButton::clicked, this, &CommandUIExample::onCommandButton2Clicked);
            button_layout->addWidget(button2);

            layout->addWidget(button_group);

            // Input section
            auto* input_group = new QGroupBox("Command Input");
            auto* input_layout = new QVBoxLayout(input_group);

            input_ = new QLineEdit();
            input_->setPlaceholderText("Enter text via Command pattern...");
            connect(input_, &QLineEdit::textChanged, this, &CommandUIExample::onTextChanged);
            input_layout->addWidget(input_);

            layout->addWidget(input_group);

            // Checkbox section
            auto* check_group = new QGroupBox("Command Checkboxes");
            auto* check_layout = new QVBoxLayout(check_group);
            check_layout->setSpacing(5);

            auto* checkbox1 = new QCheckBox("Command CheckBox 1");
            checkbox1->setChecked(true);
            connect(checkbox1, &QCheckBox::toggled, this, &CommandUIExample::onCheckBox1Toggled);
            check_layout->addWidget(checkbox1);

            auto* checkbox2 = new QCheckBox("Command CheckBox 2");
            connect(checkbox2, &QCheckBox::toggled, this, &CommandUIExample::onCheckBox2Toggled);
            check_layout->addWidget(checkbox2);

            layout->addWidget(check_group);

            layout->addStretch();

            qDebug() << "✅ Command UI created successfully";

        } catch (const std::exception& e) {
            qWarning() << "❌ Error creating Command UI:" << e.what();
        }
    }

    void setupCommands() {
        // Initialize command system
        command_invoker_ = std::make_unique<CommandInvoker>();
        qDebug() << "🔧 Commands configured";
    }

    void setupEventHandling() {
        qDebug() << "⚡ Event handling configured";
    }

    void createMenuBar() {
        auto* file_menu = menuBar()->addMenu("&File");

        auto* exit_action = file_menu->addAction("E&xit");
        connect(exit_action, &QAction::triggered, this, &QWidget::close);

        auto* examples_menu = menuBar()->addMenu("&Examples");

        auto* simple_action = examples_menu->addAction("&Simple Commands");
        connect(simple_action, &QAction::triggered, [this]() {
            statusBar()->showMessage("Simple Commands example selected", 2000);
        });
    }

    void createStatusBar() {
        statusBar()->showMessage("Command UI Example - Ready");
    }

private slots:

    void onCommandButton1Clicked() {
        qDebug() << "🖱️ Command Button 1 clicked!";
        statusBar()->showMessage("Button 1 clicked via Command pattern", 2000);
    }

    void onCommandButton2Clicked() {
        qDebug() << "🖱️ Command Button 2 clicked!";
        statusBar()->showMessage("Button 2 clicked via Command pattern", 2000);
    }

    void onTextChanged(const QString& text) {
        qDebug() << "📝 Command input changed:" << text;
        statusBar()->showMessage(QString("Text length = %1 characters").arg(text.length()), 1000);
    }

    void onCheckBox1Toggled(bool checked) {
        qDebug() << "☑️ Command CheckBox 1 toggled:" << checked;
        statusBar()->showMessage(QString("CheckBox 1 is %1").arg(checked ? "checked" : "unchecked"), 2000);
    }

    void onCheckBox2Toggled(bool checked) {
        qDebug() << "☑️ Command CheckBox 2 toggled:" << checked;
        statusBar()->showMessage(QString("CheckBox 2 is %1").arg(checked ? "checked" : "unchecked"), 2000);
    }

private:
    std::unique_ptr<CommandInvoker> command_invoker_;
    QLineEdit* input_;
};

#endif  // DECLARATIVE_UI_COMMAND_SYSTEM_ENABLED

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

#ifdef DECLARATIVE_UI_COMMAND_SYSTEM_ENABLED
    CommandUIExample window;
    window.setWindowTitle("DeclarativeUI - Command UI Example");
    window.resize(500, 400);
    window.show();

    qDebug() << "🎯 Command UI Example running...";
    return app.exec();
#else
    qWarning() << "❌ Command system not enabled. Please build with "
                  "DECLARATIVE_UI_COMMAND_SYSTEM_ENABLED.";

    // Show a simple message for users
    QWidget window;
    window.setWindowTitle("Command System Not Available");
    auto* layout = new QVBoxLayout(&window);
    auto* label = new QLabel("The Command System is not enabled in this build.\n\n"
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

#include "CommandUIExample.moc"
