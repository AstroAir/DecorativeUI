#include <QApplication>
#include <QDebug>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QWidget>

#ifdef DECLARATIVE_UI_COMMAND_SYSTEM_ENABLED
#include "Command/CommandBuilder.hpp"
#include "Command/UICommandFactory.hpp"
#include "Command/WidgetMapper.hpp"

using namespace DeclarativeUI::Command;

class CommandBuilderExampleWindow : public QMainWindow {
    Q_OBJECT

public:
    CommandBuilderExampleWindow(QWidget* parent = nullptr)
        : QMainWindow(parent) {
        setWindowTitle("Command Builder Example");
        setMinimumSize(400, 300);

        setupUI();
    }

private:
    void setupUI() {
        try {
            // Create a simple UI to demonstrate command concepts
            auto* central_widget = new QWidget();
            setCentralWidget(central_widget);

            auto* layout = new QVBoxLayout(central_widget);
            layout->setSpacing(10);
            layout->setContentsMargins(20, 20, 20, 20);

            // Title
            auto* title = new QLabel("Command Builder Example");
            title->setStyleSheet(
                "font-weight: bold; font-size: 16px; color: #2c3e50;");
            layout->addWidget(title);

            // Description
            auto* desc = new QLabel(
                "This example demonstrates the Command pattern with undo/redo "
                "functionality.");
            desc->setWordWrap(true);
            desc->setStyleSheet("color: #666; margin-bottom: 10px;");
            layout->addWidget(desc);

            // Create command invoker for undo/redo
            command_invoker_ = std::make_unique<CommandInvoker>();

            // Button that creates commands
            auto* button = new QPushButton("Execute Command");
            connect(button, &QPushButton::clicked, this,
                    &CommandBuilderExampleWindow::executeCommand);
            layout->addWidget(button);

            // Status label
            status_label_ =
                new QLabel("Ready - Click button to execute commands");
            status_label_->setStyleSheet(
                "padding: 10px; background-color: #f8f9fa; border: 1px solid "
                "#dee2e6;");
            layout->addWidget(status_label_);

            // Undo/Redo buttons
            auto* button_layout = new QHBoxLayout();

            undo_button_ = new QPushButton("Undo");
            undo_button_->setEnabled(false);
            connect(undo_button_, &QPushButton::clicked, this,
                    &CommandBuilderExampleWindow::undoCommand);
            button_layout->addWidget(undo_button_);

            redo_button_ = new QPushButton("Redo");
            redo_button_->setEnabled(false);
            connect(redo_button_, &QPushButton::clicked, this,
                    &CommandBuilderExampleWindow::redoCommand);
            button_layout->addWidget(redo_button_);

            layout->addLayout(button_layout);
            layout->addStretch();

            qDebug() << "✅ Command Builder example UI created successfully";

        } catch (const std::exception& e) {
            qWarning() << "❌ Error creating Command Builder example:"
                       << e.what();
        }
    }

private slots:
    void executeCommand() {
        static int command_count = 0;
        command_count++;

        QString command_name = QString("Command #%1").arg(command_count);

        // For this example, we'll simulate command execution
        // In a real application, commands would be registered with
        // CommandFactory and executed through the CommandInvoker

        qDebug() << "Simulating command execution:" << command_name;

        // Simulate command execution and add to history
        command_history_.append(command_name);

        // Clear redo history when new command is executed (standard behavior)
        redo_history_.clear();

        // Update UI
        status_label_->setText(QString("Executed: %1").arg(command_name));
        undo_button_->setEnabled(!command_history_.isEmpty());
        redo_button_->setEnabled(!redo_history_.isEmpty());

        qDebug() << "🖱️ Executed command:" << command_name;
    }

    void undoCommand() {
        if (!command_history_.isEmpty()) {
            QString last_command = command_history_.takeLast();
            redo_history_.append(last_command);

            status_label_->setText(QString("Undone: %1").arg(last_command));
            undo_button_->setEnabled(!command_history_.isEmpty());
            redo_button_->setEnabled(!redo_history_.isEmpty());

            qDebug() << "↶ Command undone:" << last_command;
        }
    }

    void redoCommand() {
        if (!redo_history_.isEmpty()) {
            QString command = redo_history_.takeLast();
            command_history_.append(command);

            status_label_->setText(QString("Redone: %1").arg(command));
            undo_button_->setEnabled(!command_history_.isEmpty());
            redo_button_->setEnabled(!redo_history_.isEmpty());

            qDebug() << "↷ Command redone:" << command;
        }
    }

private:
    std::unique_ptr<CommandInvoker> command_invoker_;
    QLabel* status_label_;
    QPushButton* undo_button_;
    QPushButton* redo_button_;
    QStringList command_history_;
    QStringList redo_history_;
};

#include "command-builder.moc"

#endif  // DECLARATIVE_UI_COMMAND_SYSTEM_ENABLED

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

#ifdef DECLARATIVE_UI_COMMAND_SYSTEM_ENABLED
    qDebug() << "🚀 Starting Command Builder Example";

    CommandBuilderExampleWindow window;
    window.show();

    return app.exec();
#else
    qWarning() << "❌ Command system not enabled. Please build with "
                  "BUILD_COMMAND_SYSTEM=ON";
    return 1;
#endif
}
