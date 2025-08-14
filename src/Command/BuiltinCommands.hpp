#pragma once

/**
 * @file BuiltinCommands.hpp
 * @brief Declares built-in command classes for DeclarativeUI, including UI
 * property manipulation, state management, file operations, clipboard actions,
 * UI messaging, asynchronous, and transactional commands.
 */

#include <QApplication>
#include <QClipboard>
#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>
#include <QWidget>
#include "CommandSystem.hpp"

namespace DeclarativeUI {
namespace Command {
namespace Commands {

using namespace DeclarativeUI::Command;

/**
 * @class SetPropertyCommand
 * @brief Command to set a property on a QWidget.
 *
 * Supports undo by restoring the previous property value.
 */
class SetPropertyCommand : public DeclarativeUI::Command::ICommand {
    Q_OBJECT

public:
    /**
     * @brief Constructs the command with the given context.
     * @param context Command execution context.
     */
    explicit SetPropertyCommand(const CommandContext& context);

    /**
     * @brief Executes the property set operation.
     * @param context Command execution context.
     * @return Result containing the new property value.
     */
    CommandResult<QVariant> execute(const CommandContext& context) override;

    /**
     * @brief Undoes the property set operation, restoring the old value.
     * @param context Command execution context.
     * @return Result containing the restored value.
     */
    CommandResult<QVariant> undo(const CommandContext& context) override;

    /**
     * @brief Checks if the command can be undone.
     * @param context Command execution context.
     * @return True if undo is supported.
     */
    bool canUndo(const CommandContext& context) const override;

    /**
     * @brief Gets metadata describing the command.
     * @return Command metadata.
     */
    CommandMetadata getMetadata() const override;

private:
    /**
     * @brief Finds a widget by name in the current application.
     * @param name Name of the widget.
     * @return Pointer to the widget, or nullptr if not found.
     */
    QWidget* findWidget(const QString& name);

    QString widget_name_;    ///< Name of the target widget.
    QString property_name_;  ///< Name of the property to set.
    QVariant old_value_;     ///< Previous value of the property.
    QVariant new_value_;     ///< New value to set.
};

/**
 * @class UpdateStateCommand
 * @brief Command to update application state.
 *
 * Used for modifying state variables managed by the state system.
 */
class UpdateStateCommand : public DeclarativeUI::Command::ICommand {
    Q_OBJECT

public:
    /**
     * @brief Constructs the command with the given context.
     * @param context Command execution context.
     */
    explicit UpdateStateCommand(const CommandContext& context);

    /**
     * @brief Executes the state update operation.
     * @param context Command execution context.
     * @return Result containing the updated state value.
     */
    CommandResult<QVariant> execute(const CommandContext& context) override;

    /**
     * @brief Gets metadata describing the command.
     * @return Command metadata.
     */
    CommandMetadata getMetadata() const override;
};

/**
 * @class SaveFileCommand
 * @brief Command to save data to a file using a file dialog.
 */
class SaveFileCommand : public DeclarativeUI::Command::ICommand {
    Q_OBJECT

public:
    /**
     * @brief Constructs the command with the given context.
     * @param context Command execution context.
     */
    explicit SaveFileCommand(const CommandContext& context);

    /**
     * @brief Executes the file save operation.
     * @param context Command execution context.
     * @return Result containing the file path or status.
     */
    CommandResult<QVariant> execute(const CommandContext& context) override;

    /**
     * @brief Gets metadata describing the command.
     * @return Command metadata.
     */
    CommandMetadata getMetadata() const override;
};

/**
 * @class LoadFileCommand
 * @brief Command to load data from a file using a file dialog.
 */
class LoadFileCommand : public DeclarativeUI::Command::ICommand {
    Q_OBJECT

public:
    /**
     * @brief Constructs the command with the given context.
     * @param context Command execution context.
     */
    explicit LoadFileCommand(const CommandContext& context);

    /**
     * @brief Executes the file load operation.
     * @param context Command execution context.
     * @return Result containing the file path or loaded data.
     */
    CommandResult<QVariant> execute(const CommandContext& context) override;

    /**
     * @brief Gets metadata describing the command.
     * @return Command metadata.
     */
    CommandMetadata getMetadata() const override;
};

/**
 * @class CopyToClipboardCommand
 * @brief Command to copy data to the system clipboard.
 */
class CopyToClipboardCommand : public DeclarativeUI::Command::ICommand {
    Q_OBJECT

public:
    /**
     * @brief Constructs the command with the given context.
     * @param context Command execution context.
     */
    explicit CopyToClipboardCommand(const CommandContext& context);

    /**
     * @brief Executes the copy operation.
     * @param context Command execution context.
     * @return Result indicating success or failure.
     */
    CommandResult<QVariant> execute(const CommandContext& context) override;

    /**
     * @brief Gets metadata describing the command.
     * @return Command metadata.
     */
    CommandMetadata getMetadata() const override;
};

/**
 * @class PasteFromClipboardCommand
 * @brief Command to paste data from the system clipboard.
 */
class PasteFromClipboardCommand : public DeclarativeUI::Command::ICommand {
    Q_OBJECT

public:
    /**
     * @brief Constructs the command with the given context.
     * @param context Command execution context.
     */
    explicit PasteFromClipboardCommand(const CommandContext& context);

    /**
     * @brief Executes the paste operation.
     * @param context Command execution context.
     * @return Result containing the pasted data.
     */
    CommandResult<QVariant> execute(const CommandContext& context) override;

    /**
     * @brief Gets metadata describing the command.
     * @return Command metadata.
     */
    CommandMetadata getMetadata() const override;
};

/**
 * @class ShowMessageCommand
 * @brief Command to display a message box to the user.
 */
class ShowMessageCommand : public DeclarativeUI::Command::ICommand {
    Q_OBJECT

public:
    /**
     * @brief Constructs the command with the given context.
     * @param context Command execution context.
     */
    explicit ShowMessageCommand(const CommandContext& context);

    /**
     * @brief Executes the message display operation.
     * @param context Command execution context.
     * @return Result indicating user response or status.
     */
    CommandResult<QVariant> execute(const CommandContext& context) override;

    /**
     * @brief Gets metadata describing the command.
     * @return Command metadata.
     */
    CommandMetadata getMetadata() const override;
};

/**
 * @class DelayedCommand
 * @brief Example of an asynchronous command that executes after a delay.
 */
class DelayedCommand : public DeclarativeUI::Command::AsyncCommand {
    Q_OBJECT

public:
    /**
     * @brief Constructs the command with the given context.
     * @param context Command execution context.
     */
    explicit DelayedCommand(const CommandContext& context);

    /**
     * @brief Executes the command asynchronously.
     * @param context Command execution context.
     * @return QFuture representing the asynchronous result.
     */
    QFuture<CommandResult<QVariant>> executeAsync(
        const CommandContext& context) override;

    /**
     * @brief Gets metadata describing the command.
     * @return Command metadata.
     */
    CommandMetadata getMetadata() const override;
};

/**
 * @class DatabaseTransactionCommand
 * @brief Example of a transactional command for database operations.
 *
 * Supports begin, commit, and rollback of transactions.
 */
class DatabaseTransactionCommand
    : public DeclarativeUI::Command::TransactionalCommand {
    Q_OBJECT

public:
    /**
     * @brief Constructs the command with the given context.
     * @param context Command execution context.
     */
    explicit DatabaseTransactionCommand(const CommandContext& context);

    /**
     * @brief Begins a database transaction.
     * @param context Command execution context.
     */
    void beginTransaction(const CommandContext& context) override;

    /**
     * @brief Commits the database transaction.
     * @param context Command execution context.
     */
    void commitTransaction(const CommandContext& context) override;

    /**
     * @brief Rolls back the database transaction.
     * @param context Command execution context.
     */
    void rollbackTransaction(const CommandContext& context) override;

    /**
     * @brief Executes the transactional command.
     * @param context Command execution context.
     * @return Result of the transactional operation.
     */
    CommandResult<QVariant> executeTransactional(
        const CommandContext& context) override;

    /**
     * @brief Gets metadata describing the command.
     * @return Command metadata.
     */
    CommandMetadata getMetadata() const override;
};

/**
 * @brief Registers all built-in commands with the command system.
 */
void registerBuiltinCommands();

}  // namespace Commands
}  // namespace Command
}  // namespace DeclarativeUI
