/**
 * @file 26_command_system.cpp
 * @brief Advanced command system implementation and patterns
 *
 * This example demonstrates:
 * - Command pattern implementation
 * - Undo/redo functionality
 * - Command queuing and batching
 * - Macro commands and composition
 * - Command history and persistence
 *
 * Learning objectives:
 * - Master the command pattern
 * - Understand undo/redo implementation
 * - Learn command composition patterns
 * - See advanced command system architecture
 */

#include <QApplication>
#include <QComboBox>
#include <QDateTime>
#include <QDebug>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QProgressBar>
#include <QPushButton>
#include <QSpinBox>
#include <QTextEdit>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

// Include DeclarativeUI command system
#include "Command/CommandSystem.hpp"
#include "JSON/JSONUILoader.hpp"

using namespace DeclarativeUI;

/**
 * @brief Custom commands for demonstration
 */
class TextEditCommand : public Command::ICommand {
public:
    TextEditCommand(QTextEdit* editor, const QString& new_text,
                    const QString& old_text)
        : editor_(editor), new_text_(new_text), old_text_(old_text) {}

    Command::CommandResult<QVariant> execute(
        const Command::CommandContext& context) override {
        Q_UNUSED(context)
        if (!editor_) {
            return Command::CommandResult<QVariant>(
                QString("Text editor not available"));
        }

        editor_->setPlainText(new_text_);
        return Command::CommandResult<QVariant>(
            QVariant(QString("Text updated to: %1").arg(new_text_)));
    }

    Command::CommandResult<QVariant> undo(
        const Command::CommandContext& context) override {
        Q_UNUSED(context)
        if (!editor_) {
            return Command::CommandResult<QVariant>(
                QString("Text editor not available"));
        }

        editor_->setPlainText(old_text_);
        return Command::CommandResult<QVariant>(
            QVariant(QString("Text reverted to: %1").arg(old_text_)));
    }

    bool canUndo(const Command::CommandContext& context) const override {
        Q_UNUSED(context)
        return true;
    }

    Command::CommandMetadata getMetadata() const override {
        return Command::CommandMetadata(
            "TextEditCommand",
            QString("Edit text: '%1' -> '%2'")
                .arg(old_text_.left(20), new_text_.left(20)));
    }

private:
    QTextEdit* editor_;
    QString new_text_;
    QString old_text_;
};

class StyleCommand : public Command::ICommand {
public:
    StyleCommand(QWidget* widget, const QString& new_style,
                 const QString& old_style)
        : widget_(widget), new_style_(new_style), old_style_(old_style) {}

    Command::CommandResult<QVariant> execute(
        const Command::CommandContext& context) override {
        Q_UNUSED(context)
        if (!widget_) {
            return Command::CommandResult<QVariant>(
                QString("Widget not available"));
        }

        widget_->setStyleSheet(new_style_);
        return Command::CommandResult<QVariant>(
            QVariant(QString("Style applied")));
    }

    Command::CommandResult<QVariant> undo(
        const Command::CommandContext& context) override {
        Q_UNUSED(context)
        if (!widget_) {
            return Command::CommandResult<QVariant>(
                QString("Widget not available"));
        }

        widget_->setStyleSheet(old_style_);
        return Command::CommandResult<QVariant>(
            QVariant(QString("Style reverted")));
    }

    bool canUndo(const Command::CommandContext& context) const override {
        Q_UNUSED(context)
        return true;
    }

    Command::CommandMetadata getMetadata() const override {
        return Command::CommandMetadata("StyleCommand", "Change widget style");
    }

private:
    QWidget* widget_;
    QString new_style_;
    QString old_style_;
};

/**
 * @brief Command system demonstration
 */
class CommandSystemApp : public QObject {
    Q_OBJECT

public:
    CommandSystemApp(QObject* parent = nullptr) : QObject(parent) {
        setupCommandSystem();
        setupUILoader();
        createUI();
        setupCommandHistory();
    }

    void show() {
        if (main_widget_) {
            main_widget_->show();
        }
    }

private slots:
    void onEditTextClicked() {
        auto text_edit = main_widget_->findChild<QTextEdit*>("contentEditor");
        auto input_line = main_widget_->findChild<QLineEdit*>("textInput");

        if (!text_edit || !input_line)
            return;

        QString old_text = text_edit->toPlainText();
        QString new_text = input_line->text();

        if (new_text.isEmpty()) {
            new_text = "Sample text content";
        }

        // Create and execute command directly
        auto command =
            std::make_unique<TextEditCommand>(text_edit, new_text, old_text);
        Command::CommandContext context;

        auto result = command->execute(context);

        if (result.isSuccess()) {
            logCommand("TEXT_EDIT", "SUCCESS", result.getResult().toString());
            input_line->clear();
            // Store command for undo functionality
            executed_commands_.push_back(std::move(command));
        } else {
            logCommand("TEXT_EDIT", "ERROR", result.getError());
        }

        updateCommandHistory();
    }

    void onChangeStyleClicked() {
        auto text_edit = main_widget_->findChild<QTextEdit*>("contentEditor");
        auto style_combo = main_widget_->findChild<QComboBox*>("styleCombo");

        if (!text_edit || !style_combo)
            return;

        QString old_style = text_edit->styleSheet();
        QString style_name = style_combo->currentText();
        QString new_style;

        if (style_name == "Default") {
            new_style = "";
        } else if (style_name == "Dark Theme") {
            new_style =
                "QTextEdit { background-color: #2c3e50; color: #ecf0f1; "
                "border: 2px solid #34495e; }";
        } else if (style_name == "Light Blue") {
            new_style =
                "QTextEdit { background-color: #ebf3fd; color: #2c3e50; "
                "border: 2px solid #3498db; }";
        } else if (style_name == "Green Theme") {
            new_style =
                "QTextEdit { background-color: #d5f4e6; color: #2c3e50; "
                "border: 2px solid #27ae60; }";
        }

        // Create and execute command directly
        auto command =
            std::make_unique<StyleCommand>(text_edit, new_style, old_style);
        Command::CommandContext context;

        auto result = command->execute(context);

        if (result.isSuccess()) {
            logCommand("STYLE_CHANGE", "SUCCESS",
                       result.getResult().toString());
            // Store command for undo functionality
            executed_commands_.push_back(std::move(command));
        } else {
            logCommand("STYLE_CHANGE", "ERROR", result.getError());
        }

        updateCommandHistory();
    }

    void onUndoClicked() {
        if (!executed_commands_.empty()) {
            auto command = std::move(executed_commands_.back());
            executed_commands_.pop_back();

            Command::CommandContext context;
            auto result = command->undo(context);

            if (result.isSuccess()) {
                logCommand("UNDO", "SUCCESS", result.getResult().toString());
                redo_commands_.push_back(std::move(command));
            } else {
                logCommand("UNDO", "ERROR", result.getError());
                // Put command back if undo failed
                executed_commands_.push_back(std::move(command));
            }
        } else {
            logCommand("UNDO", "ERROR", "No commands to undo");
        }

        updateCommandHistory();
        updateUndoRedoButtons();
    }

    void onRedoClicked() {
        if (!redo_commands_.empty()) {
            auto command = std::move(redo_commands_.back());
            redo_commands_.pop_back();

            Command::CommandContext context;
            auto result = command->execute(context);

            if (result.isSuccess()) {
                logCommand("REDO", "SUCCESS", result.getResult().toString());
                executed_commands_.push_back(std::move(command));
            } else {
                logCommand("REDO", "ERROR", result.getError());
                // Put command back if redo failed
                redo_commands_.push_back(std::move(command));
            }
        } else {
            logCommand("REDO", "ERROR", "No commands to redo");
        }

        updateCommandHistory();
        updateUndoRedoButtons();
    }

    void onClearHistoryClicked() {
        executed_commands_.clear();
        redo_commands_.clear();
        logCommand("SYSTEM", "INFO", "Command history cleared");
        updateCommandHistory();
        updateUndoRedoButtons();
    }

    void onBatchCommandsClicked() {
        // Demonstrate batch command execution
        auto text_edit = main_widget_->findChild<QTextEdit*>("contentEditor");
        if (!text_edit)
            return;

        QString old_text = text_edit->toPlainText();
        QString old_style = text_edit->styleSheet();

        // Create batch of commands
        std::vector<std::unique_ptr<Command::ICommand>> commands;

        commands.push_back(std::make_unique<TextEditCommand>(
            text_edit, "Batch operation text", old_text));

        commands.push_back(std::make_unique<StyleCommand>(
            text_edit, "QTextEdit { background-color: #f39c12; color: white; }",
            old_style));

        // Execute batch
        Command::CommandContext context;
        for (auto& command : commands) {
            auto result = command->execute(context);
            if (result.isSuccess()) {
                logCommand("BATCH", "SUCCESS", result.getResult().toString());
                executed_commands_.push_back(std::move(command));
            } else {
                logCommand("BATCH", "ERROR", result.getError());
            }
        }

        updateCommandHistory();
        updateUndoRedoButtons();
    }

    void onSaveHistoryClicked() {
        // Save command history to JSON
        QJsonArray history_array;

        // Create history from our executed commands
        for (const auto& command : executed_commands_) {
            QJsonObject cmd_obj;
            cmd_obj["description"] = command->getMetadata().description;
            cmd_obj["timestamp"] =
                QDateTime::currentDateTime().toString(Qt::ISODate);
            cmd_obj["success"] = true;
            history_array.append(cmd_obj);
        }

        QJsonDocument doc(history_array);
        QString json_string = doc.toJson();

        logCommand(
            "SYSTEM", "INFO",
            QString("History saved (%1 commands)").arg(history_array.size()));

        // Display in a dialog or save to file
        auto log_display = main_widget_->findChild<QTextEdit*>("commandLog");
        if (log_display) {
            log_display->append("=== SAVED HISTORY ===");
            log_display->append(json_string);
            log_display->append("=== END HISTORY ===");
        }
    }

    void onClearLogClicked() {
        auto log_display = main_widget_->findChild<QTextEdit*>("commandLog");
        if (log_display) {
            log_display->clear();
            logCommand("SYSTEM", "INFO", "Command log cleared");
        }
    }

private:
    std::unique_ptr<QWidget> main_widget_;
    std::unique_ptr<JSON::JSONUILoader> ui_loader_;
    Command::CommandManager* command_manager_;
    std::vector<std::unique_ptr<Command::ICommand>> executed_commands_;
    std::vector<std::unique_ptr<Command::ICommand>> redo_commands_;

    void setupCommandSystem() {
        // Initialize command system
        command_manager_ = &Command::CommandManager::instance();

        // Enable command history
        command_manager_->enableCommandHistory(true);
        command_manager_->enableAuditTrail(true);

        // Connect command system signals
        connect(command_manager_, &Command::CommandManager::commandExecuted,
                this, [this](const QString& command_name) {
                    logCommand("EXECUTED", "INFO", command_name);
                    updateUndoRedoButtons();
                });

        connect(command_manager_, &Command::CommandManager::commandUndone, this,
                [this](const QString& command_name) {
                    logCommand("UNDONE", "INFO", command_name);
                    updateUndoRedoButtons();
                });

        connect(command_manager_, &Command::CommandManager::commandRedone, this,
                [this](const QString& command_name) {
                    logCommand("REDONE", "INFO", command_name);
                    updateUndoRedoButtons();
                });

        qDebug() << "✅ Command system initialized";
    }

    void setupUILoader() {
        ui_loader_ = std::make_unique<JSON::JSONUILoader>();

        // Register event handlers
        ui_loader_->registerEventHandler("editText",
                                         [this]() { onEditTextClicked(); });

        ui_loader_->registerEventHandler("changeStyle",
                                         [this]() { onChangeStyleClicked(); });

        ui_loader_->registerEventHandler("undoCommand",
                                         [this]() { onUndoClicked(); });

        ui_loader_->registerEventHandler("redoCommand",
                                         [this]() { onRedoClicked(); });

        ui_loader_->registerEventHandler("clearHistory",
                                         [this]() { onClearHistoryClicked(); });

        ui_loader_->registerEventHandler(
            "batchCommands", [this]() { onBatchCommandsClicked(); });

        ui_loader_->registerEventHandler("saveHistory",
                                         [this]() { onSaveHistoryClicked(); });

        ui_loader_->registerEventHandler("clearLog",
                                         [this]() { onClearLogClicked(); });

        qDebug() << "✅ Event handlers registered";
    }

    void createUI() {
        try {
            QString ui_file = "resources/command_system_ui.json";

            if (QFileInfo::exists(ui_file)) {
                main_widget_ = ui_loader_->loadFromFile(ui_file);
                if (main_widget_) {
                    main_widget_->setWindowTitle(
                        "26 - Command System | DeclarativeUI");
                    connectUIEvents();
                    updateUndoRedoButtons();
                    qDebug() << "✅ UI loaded from JSON";
                    return;
                }
            }

            // Fallback to programmatic UI
            main_widget_ = createProgrammaticUI();

        } catch (const std::exception& e) {
            qCritical() << "UI creation failed:" << e.what();
            main_widget_ = createProgrammaticUI();
        }
    }

    void connectUIEvents() {
        // Additional UI event connections if needed
        updateCommandHistory();
        qDebug() << "✅ UI events connected";
    }

    void setupCommandHistory() {
        // Initialize with a welcome message
        QTimer::singleShot(100, [this]() {
            logCommand(
                "SYSTEM", "INFO",
                "Command system ready - try editing text or changing styles!");
        });
    }

    std::unique_ptr<QWidget> createProgrammaticUI() {
        auto widget = std::make_unique<QWidget>();
        widget->setWindowTitle(
            "26 - Command System (Fallback) | DeclarativeUI");
        widget->setMinimumSize(900, 700);

        auto layout = new QVBoxLayout(widget.get());
        layout->setSpacing(15);
        layout->setContentsMargins(20, 20, 20, 20);

        // Header
        auto header = new QLabel("⚡ Advanced Command System");
        header->setStyleSheet(
            "font-size: 20px; font-weight: bold; color: #2c3e50;");
        header->setAlignment(Qt::AlignCenter);
        layout->addWidget(header);

        // Content editor
        auto editor_group = new QGroupBox("Content Editor");
        auto editor_layout = new QVBoxLayout(editor_group);

        auto content_editor = new QTextEdit();
        content_editor->setObjectName("contentEditor");
        content_editor->setPlainText(
            "Welcome to the Command System demo!\nTry editing this text and "
            "changing styles.");
        content_editor->setMaximumHeight(150);
        editor_layout->addWidget(content_editor);

        layout->addWidget(editor_group);

        // Controls
        auto controls_group = new QGroupBox("Command Controls");
        auto controls_layout = new QGridLayout(controls_group);

        auto text_input = new QLineEdit();
        text_input->setObjectName("textInput");
        text_input->setPlaceholderText("Enter new text...");

        auto edit_button = new QPushButton("📝 Edit Text");
        connect(edit_button, &QPushButton::clicked, this,
                &CommandSystemApp::onEditTextClicked);

        auto style_combo = new QComboBox();
        style_combo->setObjectName("styleCombo");
        style_combo->addItems(
            {"Default", "Dark Theme", "Light Blue", "Green Theme"});

        auto style_button = new QPushButton("🎨 Change Style");
        connect(style_button, &QPushButton::clicked, this,
                &CommandSystemApp::onChangeStyleClicked);

        controls_layout->addWidget(new QLabel("New Text:"), 0, 0);
        controls_layout->addWidget(text_input, 0, 1);
        controls_layout->addWidget(edit_button, 0, 2);
        controls_layout->addWidget(new QLabel("Style:"), 1, 0);
        controls_layout->addWidget(style_combo, 1, 1);
        controls_layout->addWidget(style_button, 1, 2);

        layout->addWidget(controls_group);

        // Undo/Redo buttons
        auto undo_redo_layout = new QHBoxLayout();

        auto undo_button = new QPushButton("↶ Undo");
        undo_button->setObjectName("undoButton");
        connect(undo_button, &QPushButton::clicked, this,
                &CommandSystemApp::onUndoClicked);

        auto redo_button = new QPushButton("↷ Redo");
        redo_button->setObjectName("redoButton");
        connect(redo_button, &QPushButton::clicked, this,
                &CommandSystemApp::onRedoClicked);

        auto batch_button = new QPushButton("📦 Batch Commands");
        connect(batch_button, &QPushButton::clicked, this,
                &CommandSystemApp::onBatchCommandsClicked);

        undo_redo_layout->addWidget(undo_button);
        undo_redo_layout->addWidget(redo_button);
        undo_redo_layout->addWidget(batch_button);
        undo_redo_layout->addStretch();

        layout->addLayout(undo_redo_layout);

        // Command history
        auto history_group = new QGroupBox("Command History");
        auto history_layout = new QVBoxLayout(history_group);

        auto history_count = new QLabel("History: 0 commands");
        history_count->setObjectName("historyCount");

        auto history_list = new QListWidget();
        history_list->setObjectName("historyList");
        history_list->setMaximumHeight(100);

        history_layout->addWidget(history_count);
        history_layout->addWidget(history_list);

        layout->addWidget(history_group);

        // Command log
        auto log_group = new QGroupBox("Command Log");
        auto log_layout = new QVBoxLayout(log_group);

        auto command_log = new QTextEdit();
        command_log->setObjectName("commandLog");
        command_log->setReadOnly(true);
        command_log->setMaximumHeight(120);
        command_log->setStyleSheet(
            "QTextEdit { background-color: #2c3e50; color: #ecf0f1; "
            "font-family: monospace; }");

        auto clear_log_button = new QPushButton("🗑️ Clear Log");
        connect(clear_log_button, &QPushButton::clicked, this,
                &CommandSystemApp::onClearLogClicked);

        log_layout->addWidget(command_log);
        log_layout->addWidget(clear_log_button);

        layout->addWidget(log_group);

        qDebug() << "✅ Programmatic UI created";
        return widget;
    }

    void updateCommandHistory() {
        auto history_list =
            main_widget_->findChild<QListWidget*>("historyList");
        if (!history_list)
            return;

        history_list->clear();

        // Display our executed commands
        for (int i = executed_commands_.size() - 1; i >= 0; --i) {
            const auto& command = executed_commands_[i];
            QString item_text =
                QString("[%1] ✅ %2")
                    .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                    .arg(command->getMetadata().description);

            history_list->addItem(item_text);
        }

        // Update history count
        auto count_label = main_widget_->findChild<QLabel*>("historyCount");
        if (count_label) {
            count_label->setText(
                QString("History: %1 commands").arg(executed_commands_.size()));
        }
    }

    void updateUndoRedoButtons() {
        auto undo_button = main_widget_->findChild<QPushButton*>("undoButton");
        auto redo_button = main_widget_->findChild<QPushButton*>("redoButton");

        if (undo_button) {
            undo_button->setEnabled(!executed_commands_.empty());
        }

        if (redo_button) {
            redo_button->setEnabled(!redo_commands_.empty());
        }
    }

    void logCommand(const QString& type, const QString& level,
                    const QString& message) {
        auto log_display = main_widget_->findChild<QTextEdit*>("commandLog");
        if (log_display) {
            QString timestamp =
                QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
            QString log_entry = QString("[%1] %2 | %3 | %4")
                                    .arg(timestamp, level, type, message);

            log_display->append(log_entry);

            // Auto-scroll to bottom
            auto cursor = log_display->textCursor();
            cursor.movePosition(QTextCursor::End);
            log_display->setTextCursor(cursor);
        }

        qDebug() << "Command:" << type << level << message;
    }
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    app.setApplicationName("DeclarativeUI Command System");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("DeclarativeUI Examples");

    try {
        qDebug() << "🚀 Starting Command System example...";

        CommandSystemApp command_app;
        command_app.show();

        qDebug() << "💡 This example demonstrates:";
        qDebug() << "   - Command pattern implementation";
        qDebug() << "   - Undo/redo functionality";
        qDebug() << "   - Command queuing and batching";
        qDebug() << "   - Command history and persistence";
        qDebug() << "   - Advanced command system architecture";

        return app.exec();

    } catch (const std::exception& e) {
        qCritical() << "❌ Application error:" << e.what();
        return -1;
    }
}

#include "26_command_system.moc"
