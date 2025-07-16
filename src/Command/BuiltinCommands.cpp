#include "BuiltinCommands.hpp"
#include <QDebug>
#include <QThread>
#include <QtConcurrent/QtConcurrent>

namespace DeclarativeUI {
namespace Command {
namespace Commands {

// --- SetPropertyCommand ---
SetPropertyCommand::SetPropertyCommand(const CommandContext& context)
    : ICommand(nullptr) {}

CommandResult<QVariant> SetPropertyCommand::execute(
    const CommandContext& context) {
    auto widget_name = context.getParameter<QString>("widget");
    auto property_name = context.getParameter<QString>("property");
    auto value = context.getParameter<QString>("value");

    if (!context.hasParameter("widget") || !context.hasParameter("property") ||
        !context.hasParameter("value")) {
        return CommandResult<QVariant>(
            QString("Missing required parameters: widget, property, value"));
    }

    auto* widget = findWidget(widget_name);
    if (!widget) {
        return CommandResult<QVariant>(
            QString("Widget '%1' not found").arg(widget_name));
    }

    old_value_ = widget->property(property_name.toUtf8().constData());

    bool success =
        widget->setProperty(property_name.toUtf8().constData(), value);
    if (!success) {
        return CommandResult<QVariant>(
            QString("Failed to set property '%1' on widget '%2'")
                .arg(property_name, widget_name));
    }

    widget_name_ = widget_name;
    property_name_ = property_name;
    new_value_ = value;

    return CommandResult<QVariant>(QString("Property set successfully"));
}

CommandResult<QVariant> SetPropertyCommand::undo(
    const CommandContext& context) {
    auto* widget = findWidget(widget_name_);
    if (!widget) {
        return CommandResult<QVariant>(
            QString("Widget '%1' not found for undo").arg(widget_name_));
    }

    widget->setProperty(property_name_.toUtf8().constData(), old_value_);
    return CommandResult<QVariant>(QString("Property undo successful"));
}

bool SetPropertyCommand::canUndo(const CommandContext& context) const {
    return !widget_name_.isEmpty() && !property_name_.isEmpty();
}

CommandMetadata SetPropertyCommand::getMetadata() const {
    return CommandMetadata("SetPropertyCommand",
                           "Sets a property on a UI widget");
}

QWidget* SetPropertyCommand::findWidget(const QString& name) {
    for (auto* widget : QApplication::allWidgets()) {
        if (widget->objectName() == name) {
            return widget;
        }
    }
    return nullptr;
}

// --- UpdateStateCommand ---
UpdateStateCommand::UpdateStateCommand(const CommandContext& context)
    : ICommand(nullptr) {}

CommandResult<QVariant> UpdateStateCommand::execute(
    const CommandContext& context) {
    auto state_key = context.getParameter<QString>("state_key");
    auto new_value = context.getParameter<QString>("value");

    if (state_key.isEmpty() || new_value.isEmpty()) {
        return CommandResult<QVariant>(
            QString("Missing required parameters: state_key, value"));
    }

    // Integrate with state management system here
    return CommandResult<QVariant>(
        QString("State updated: %1 = %2").arg(state_key, new_value));
}

CommandMetadata UpdateStateCommand::getMetadata() const {
    return CommandMetadata("UpdateStateCommand", "Updates application state");
}

// --- SaveFileCommand ---
SaveFileCommand::SaveFileCommand(const CommandContext& context)
    : ICommand(nullptr) {}

CommandResult<QVariant> SaveFileCommand::execute(
    const CommandContext& context) {
    auto filename = context.getParameter<QString>("filename");
    auto content = context.getParameter<QString>("content");

    if (filename.isEmpty()) {
        filename = QFileDialog::getSaveFileName(nullptr, "Save File", "",
                                                "All Files (*.*)");
        if (filename.isEmpty()) {
            return CommandResult<QVariant>(QString("File save cancelled"));
        }
    }

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return CommandResult<QVariant>(
            QString("Failed to open file for writing: %1").arg(filename));
    }
    QTextStream out(&file);
    out << content;
    file.close();

    return CommandResult<QVariant>(QString("File saved: %1").arg(filename));
}

CommandMetadata SaveFileCommand::getMetadata() const {
    return CommandMetadata("SaveFileCommand", "Saves content to a file");
}

// --- LoadFileCommand ---
LoadFileCommand::LoadFileCommand(const CommandContext& context)
    : ICommand(nullptr) {}

CommandResult<QVariant> LoadFileCommand::execute(
    const CommandContext& context) {
    auto filename = context.getParameter<QString>("filename");

    if (filename.isEmpty()) {
        filename = QFileDialog::getOpenFileName(nullptr, "Load File", "",
                                                "All Files (*.*)");
        if (filename.isEmpty()) {
            return CommandResult<QVariant>(QString("File load cancelled"));
        }
    }

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return CommandResult<QVariant>(
            QString("Failed to open file for reading: %1").arg(filename));
    }
    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    return CommandResult<QVariant>(content);
}

CommandMetadata LoadFileCommand::getMetadata() const {
    return CommandMetadata("LoadFileCommand", "Loads content from a file");
}

// --- CopyToClipboardCommand ---
CopyToClipboardCommand::CopyToClipboardCommand(const CommandContext& context)
    : ICommand(nullptr) {}

CommandResult<QVariant> CopyToClipboardCommand::execute(
    const CommandContext& context) {
    auto text = context.getParameter<QString>("text");

    if (text.isEmpty()) {
        return CommandResult<QVariant>(QString("No text to copy"));
    }

    QApplication::clipboard()->setText(text);
    return CommandResult<QVariant>(
        QString("Copied to clipboard: %1").arg(text));
}

CommandMetadata CopyToClipboardCommand::getMetadata() const {
    return CommandMetadata("CopyToClipboardCommand",
                           "Copies text to clipboard");
}

// --- PasteFromClipboardCommand ---
PasteFromClipboardCommand::PasteFromClipboardCommand(
    const CommandContext& context)
    : ICommand(nullptr) {}

CommandResult<QVariant> PasteFromClipboardCommand::execute(
    const CommandContext& context) {
    auto text = QApplication::clipboard()->text();
    return CommandResult<QVariant>(text);
}

CommandMetadata PasteFromClipboardCommand::getMetadata() const {
    return CommandMetadata("PasteFromClipboardCommand",
                           "Pastes text from clipboard");
}

// --- ShowMessageCommand ---
ShowMessageCommand::ShowMessageCommand(const CommandContext& context)
    : ICommand(nullptr) {}

CommandResult<QVariant> ShowMessageCommand::execute(
    const CommandContext& context) {
    auto message = context.getParameter<QString>("message");
    auto title = context.getParameter<QString>("title", "Information");

    if (message.isEmpty()) {
        return CommandResult<QVariant>(QString("No message to show"));
    }

    QMessageBox::information(nullptr, title, message);
    return CommandResult<QVariant>(QString("Message shown"));
}

CommandMetadata ShowMessageCommand::getMetadata() const {
    return CommandMetadata("ShowMessageCommand", "Shows a message dialog");
}

// --- DelayedCommand ---
DelayedCommand::DelayedCommand(const CommandContext& context)
    : AsyncCommand(nullptr) {}

QFuture<CommandResult<QVariant>> DelayedCommand::executeAsync(
    const CommandContext& context) {
    auto delay = context.getParameter<int>("delay", 1000);

    return QtConcurrent::run([delay]() -> CommandResult<QVariant> {
        QThread::msleep(delay);
        return CommandResult<QVariant>(
            QString("Delayed command completed after %1ms").arg(delay));
    });
}

CommandMetadata DelayedCommand::getMetadata() const {
    return CommandMetadata("DelayedCommand", "Executes a command with delay");
}

// --- DatabaseTransactionCommand ---
DatabaseTransactionCommand::DatabaseTransactionCommand(
    const CommandContext& context)
    : TransactionalCommand(nullptr) {}

void DatabaseTransactionCommand::beginTransaction(
    const CommandContext& context) {
    // Begin database transaction
}

void DatabaseTransactionCommand::commitTransaction(
    const CommandContext& context) {
    // Commit database transaction
}

void DatabaseTransactionCommand::rollbackTransaction(
    const CommandContext& context) {
    // Rollback database transaction
}

CommandResult<QVariant> DatabaseTransactionCommand::executeTransactional(
    const CommandContext& context) {
    // Execute database operations
    return CommandResult<QVariant>(QString("Database transaction completed"));
}

CommandMetadata DatabaseTransactionCommand::getMetadata() const {
    return CommandMetadata("DatabaseTransactionCommand",
                           "Executes database operations in transaction");
}

// --- registerBuiltinCommands ---
void registerBuiltinCommands() {
    auto& factory = CommandFactory::instance();

    factory.registerCommand(
        "set_property",
        [](const CommandContext& context) -> std::unique_ptr<ICommand> {
            return std::make_unique<SetPropertyCommand>(context);
        });

    factory.registerCommand(
        "update_state",
        [](const CommandContext& context) -> std::unique_ptr<ICommand> {
            return std::make_unique<UpdateStateCommand>(context);
        });

    factory.registerCommand(
        "save_file",
        [](const CommandContext& context) -> std::unique_ptr<ICommand> {
            return std::make_unique<SaveFileCommand>(context);
        });

    factory.registerCommand(
        "load_file",
        [](const CommandContext& context) -> std::unique_ptr<ICommand> {
            return std::make_unique<LoadFileCommand>(context);
        });

    factory.registerCommand(
        "copy_to_clipboard",
        [](const CommandContext& context) -> std::unique_ptr<ICommand> {
            return std::make_unique<CopyToClipboardCommand>(context);
        });

    factory.registerCommand(
        "paste_from_clipboard",
        [](const CommandContext& context) -> std::unique_ptr<ICommand> {
            return std::make_unique<PasteFromClipboardCommand>(context);
        });

    factory.registerCommand(
        "show_message",
        [](const CommandContext& context) -> std::unique_ptr<ICommand> {
            return std::make_unique<ShowMessageCommand>(context);
        });

    factory.registerCommand(
        "delayed",
        [](const CommandContext& context) -> std::unique_ptr<ICommand> {
            return std::unique_ptr<ICommand>(new DelayedCommand(context));
        });

    factory.registerCommand(
        "db_transaction",
        [](const CommandContext& context) -> std::unique_ptr<ICommand> {
            return std::make_unique<DatabaseTransactionCommand>(context);
        });
}

}  // namespace Commands
}  // namespace Command
}  // namespace DeclarativeUI
