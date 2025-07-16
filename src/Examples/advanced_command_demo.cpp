#include <QApplication>
#include <QDebug>
#include <QFuture>
#include <QFutureWatcher>
#include <QHBoxLayout>
#include <QKeySequence>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QProgressBar>
#include <QPushButton>
#include <QShortcut>
#include <QStatusBar>
#include <QTextEdit>
#include <QTimer>
#include <QVBoxLayout>
#include <QtConcurrent/QtConcurrent>
#include <chrono>
#include <thread>


#include "../Command/CommandIntegration.hpp"
#include "../Command/CommandSystem.hpp"


using namespace DeclarativeUI;
using namespace DeclarativeUI::Command;
using namespace DeclarativeUI::Command::Integration;

// **Custom command example**
class CalculatorCommand : public ICommand {
    Q_OBJECT

public:
    explicit CalculatorCommand(QObject* parent = nullptr) : ICommand(parent) {}

    CommandResult<QVariant> execute(const CommandContext& context) override {
        auto operation = context.getParameter<QString>("operation");
        auto operand1 = context.getParameter<double>("operand1");
        auto operand2 = context.getParameter<double>("operand2");

        if (operation.isEmpty() || operand1 == 0.0 || operand2 == 0.0) {
            return CommandResult<QVariant>(
                QString("Missing required parameters"));
        }

        double result = 0.0;
        QString op = operation;

        if (op == "add") {
            result = operand1 + operand2;
        } else if (op == "subtract") {
            result = operand1 - operand2;
        } else if (op == "multiply") {
            result = operand1 * operand2;
        } else if (op == "divide") {
            if (operand2 == 0.0) {
                return CommandResult<QVariant>(QString("Division by zero"));
            }
            result = operand1 / operand2;
        } else {
            return CommandResult<QVariant>(QString("Unknown operation"));
        }

        return CommandResult<QVariant>(QVariant(result));
    }

    CommandMetadata getMetadata() const override {
        return CommandMetadata("CalculatorCommand",
                               "Performs basic arithmetic operations");
    }
};

// **Async file processing command**
class FileProcessingCommand : public AsyncCommand {
    Q_OBJECT

public:
    explicit FileProcessingCommand(QObject* parent = nullptr)
        : AsyncCommand(parent) {}

    QFuture<CommandResult<QVariant>> executeAsync(
        const CommandContext& context) override {
        auto filename = context.getParameter<QString>("filename");
        auto operation = context.getParameter<QString>("operation");

        if (filename.isEmpty() || operation.isEmpty()) {
            return QtConcurrent::run([=]() {
                return CommandResult<QVariant>(
                    QString("Missing required parameters"));
            });
        }

        return QtConcurrent::run([this, filename, operation]() {
            updateMessage("Starting file processing...");
            updateProgress(0);

            // Simulate file processing with progress updates
            for (int i = 0; i <= 100; i += 10) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                updateProgress(i);
                updateMessage(
                    QString("Processing %1... %2%").arg(filename).arg(i));

                // Simulate cancellation check
                if (getState() == CommandState::Cancelled) {
                    return CommandResult<QVariant>(
                        QString("Operation cancelled"));
                }
            }

            updateMessage("File processing completed");
            return CommandResult<QVariant>(
                QString("Successfully processed %1").arg(filename));
        });
    }

    CommandMetadata getMetadata() const override {
        return CommandMetadata("FileProcessingCommand",
                               "Processes files asynchronously");
    }
};

// **Main application window**
class CommandDemoWindow : public QMainWindow {
    Q_OBJECT

public:
    CommandDemoWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
        setupUI();
        setupCommands();
        setupMenus();
        setupToolbars();
        setupShortcuts();
    }

private slots:
    void onCommandExecuted(const QString& command_name,
                           const CommandResult<QVariant>& result) {
        if (result.isSuccess()) {
            statusBar()->showMessage(
                QString("Command '%1' executed successfully").arg(command_name),
                2000);
            logTextEdit_->append(
                QString("[SUCCESS] %1: %2")
                    .arg(command_name, result.getResult().toString()));
        } else {
            statusBar()->showMessage(
                QString("Command '%1' failed").arg(command_name), 2000);
            logTextEdit_->append(
                QString("[ERROR] %1: %2").arg(command_name, result.getError()));
        }
    }

    void onCommandProgress(const QUuid& command_id, int percentage) {
        progressBar_->setValue(percentage);
        if (percentage == 100) {
            QTimer::singleShot(1000, [this]() { progressBar_->setValue(0); });
        }
    }

    void executeCalculation() {
        CommandContext context;
        context.setParameter("operation", operationEdit_->text());
        context.setParameter("operand1", operand1Edit_->text().toDouble());
        context.setParameter("operand2", operand2Edit_->text().toDouble());

        auto result = CommandManager::instance().getInvoker().execute(
            "calculator", context);

        if (result.isSuccess()) {
            resultLabel_->setText(
                QString("Result: %1").arg(result.getResult().toDouble()));
        } else {
            resultLabel_->setText(QString("Error: %1").arg(result.getError()));
        }
    }

    void executeAsyncFileProcessing() {
        CommandContext context;
        context.setParameter("filename", QString("example.txt"));
        context.setParameter("operation", QString("process"));

        auto future = CommandManager::instance().getInvoker().executeAsync(
            "file_processing", context);

        // Monitor the async operation
        auto* watcher = new QFutureWatcher<CommandResult<QVariant>>(this);
        connect(watcher, &QFutureWatcher<CommandResult<QVariant>>::finished,
                [this, watcher]() {
                    auto result = watcher->result();
                    onCommandExecuted("file_processing", result);
                    watcher->deleteLater();
                });
        watcher->setFuture(future);
    }

private:
    void setupUI() {
        auto* centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);

        auto* layout = new QVBoxLayout(centralWidget);

        // Calculator section
        auto* calcGroup = new QWidget();
        auto* calcLayout = new QHBoxLayout(calcGroup);

        operand1Edit_ = new QLineEdit("10");
        operationEdit_ = new QLineEdit("add");
        operand2Edit_ = new QLineEdit("5");
        auto* calcButton = new QPushButton("Calculate");
        resultLabel_ = new QLabel("Result: ");

        calcLayout->addWidget(new QLabel("Operand 1:"));
        calcLayout->addWidget(operand1Edit_);
        calcLayout->addWidget(new QLabel("Operation:"));
        calcLayout->addWidget(operationEdit_);
        calcLayout->addWidget(new QLabel("Operand 2:"));
        calcLayout->addWidget(operand2Edit_);
        calcLayout->addWidget(calcButton);
        calcLayout->addWidget(resultLabel_);

        connect(calcButton, &QPushButton::clicked, this,
                &CommandDemoWindow::executeCalculation);

        // Async processing section
        auto* asyncGroup = new QWidget();
        auto* asyncLayout = new QHBoxLayout(asyncGroup);

        auto* asyncButton = new QPushButton("Start Async Processing");
        progressBar_ = new QProgressBar();
        progressBar_->setRange(0, 100);

        asyncLayout->addWidget(asyncButton);
        asyncLayout->addWidget(progressBar_);

        connect(asyncButton, &QPushButton::clicked, this,
                &CommandDemoWindow::executeAsyncFileProcessing);

        // Log section
        logTextEdit_ = new QTextEdit();
        logTextEdit_->setMaximumHeight(200);
        logTextEdit_->setReadOnly(true);

        layout->addWidget(calcGroup);
        layout->addWidget(asyncGroup);
        layout->addWidget(new QLabel("Command Log:"));
        layout->addWidget(logTextEdit_);

        // Status bar
        statusBar()->showMessage("Ready");

        setWindowTitle("Command System Demo");
        resize(800, 600);
    }

    void setupCommands() {
        // Register custom commands
        CommandFactory::instance().registerCommand(
            "calculator", [](const CommandContext&) {
                return std::make_unique<CalculatorCommand>();
            });
        CommandFactory::instance().registerCommand(
            "file_processing", [](const CommandContext&) {
                return std::make_unique<FileProcessingCommand>();
            });

        // Connect to command system signals
        auto& invoker = CommandManager::instance().getInvoker();
        connect(&invoker, &CommandInvoker::commandExecuted, this,
                &CommandDemoWindow::onCommandExecuted);
        connect(&invoker, &CommandInvoker::commandProgress, this,
                &CommandDemoWindow::onCommandProgress);
    }

    void setupMenus() {
        // Create command-based menus
        CommandMenu fileMenu;
        fileMenu.addAction("New", "file.new")
            .addAction("Open", "file.load")
            .addAction("Save", "file.save")
            .addSeparator()
            .addAction("Exit", "app.exit");

        CommandMenu editMenu;
        editMenu.addAction("Copy", "clipboard.copy")
            .addAction("Paste", "clipboard.paste")
            .addSeparator()
            .addAction("Undo", "edit.undo")
            .addAction("Redo", "edit.redo");

        CommandMenu helpMenu;
        helpMenu.addAction("About", "help.about");

        // Build Qt menus
        auto* fileQtMenu = fileMenu.buildMenu(this);
        fileQtMenu->setTitle("File");

        auto* editQtMenu = editMenu.buildMenu(this);
        editQtMenu->setTitle("Edit");

        auto* helpQtMenu = helpMenu.buildMenu(this);
        helpQtMenu->setTitle("Help");

        menuBar()->addMenu(fileQtMenu);
        menuBar()->addMenu(editQtMenu);
        menuBar()->addMenu(helpQtMenu);
    }

    void setupToolbars() {
        CommandToolBar mainToolbar;
        mainToolbar.addButton("New", "file.new")
            .addButton("Open", "file.load")
            .addButton("Save", "file.save")
            .addSeparator()
            .addButton("Copy", "clipboard.copy")
            .addButton("Paste", "clipboard.paste");

        auto* qtToolbar = mainToolbar.buildToolBar(this);
        qtToolbar->setWindowTitle("Main Toolbar");
        addToolBar(qtToolbar);
    }

    void setupShortcuts() {
        // Create shortcuts using the command system
        auto* newShortcut = new QShortcut(QKeySequence::New, this);
        connect(newShortcut, &QShortcut::activated, []() {
            CommandManager::instance().getInvoker().execute("file.new");
        });

        auto* openShortcut = new QShortcut(QKeySequence::Open, this);
        connect(openShortcut, &QShortcut::activated, []() {
            CommandManager::instance().getInvoker().execute("file.load");
        });

        auto* saveShortcut = new QShortcut(QKeySequence::Save, this);
        connect(saveShortcut, &QShortcut::activated, []() {
            CommandManager::instance().getInvoker().execute("file.save");
        });
    }

    QLineEdit* operand1Edit_;
    QLineEdit* operationEdit_;
    QLineEdit* operand2Edit_;
    QLabel* resultLabel_;
    QProgressBar* progressBar_;
    QTextEdit* logTextEdit_;
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Initialize command system
    Utils::initializeCommandSystem();

    // Create and show main window
    CommandDemoWindow window;
    window.show();

    return app.exec();
}

#include "advanced_command_demo.moc"
