#pragma once

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

// **UI Property Commands**
class SetPropertyCommand : public DeclarativeUI::Command::ICommand {
    Q_OBJECT

public:
    explicit SetPropertyCommand(const CommandContext& context);

    CommandResult<QVariant> execute(const CommandContext& context) override;

    CommandResult<QVariant> undo(const CommandContext& context) override;

    bool canUndo(const CommandContext& context) const override;

    CommandMetadata getMetadata() const override;

private:
    QWidget* findWidget(const QString& name);

    QString widget_name_;
    QString property_name_;
    QVariant old_value_;
    QVariant new_value_;
};

// **State Management Commands**
class UpdateStateCommand : public DeclarativeUI::Command::ICommand {
    Q_OBJECT

public:
    explicit UpdateStateCommand(const CommandContext& context);

    CommandResult<QVariant> execute(const CommandContext& context) override;

    CommandMetadata getMetadata() const override;
};

// **File Operations Commands**
class SaveFileCommand : public DeclarativeUI::Command::ICommand {
    Q_OBJECT

public:
    explicit SaveFileCommand(const CommandContext& context);

    CommandResult<QVariant> execute(const CommandContext& context) override;

    CommandMetadata getMetadata() const override;
};

class LoadFileCommand : public DeclarativeUI::Command::ICommand {
    Q_OBJECT

public:
    explicit LoadFileCommand(const CommandContext& context);

    CommandResult<QVariant> execute(const CommandContext& context) override;

    CommandMetadata getMetadata() const override;
};

// **Clipboard Commands**
class CopyToClipboardCommand : public DeclarativeUI::Command::ICommand {
    Q_OBJECT

public:
    explicit CopyToClipboardCommand(const CommandContext& context);

    CommandResult<QVariant> execute(const CommandContext& context) override;

    CommandMetadata getMetadata() const override;
};

class PasteFromClipboardCommand : public DeclarativeUI::Command::ICommand {
    Q_OBJECT

public:
    explicit PasteFromClipboardCommand(const CommandContext& context);

    CommandResult<QVariant> execute(const CommandContext& context) override;

    CommandMetadata getMetadata() const override;
};

// **UI Commands**
class ShowMessageCommand : public DeclarativeUI::Command::ICommand {
    Q_OBJECT

public:
    explicit ShowMessageCommand(const CommandContext& context);

    CommandResult<QVariant> execute(const CommandContext& context) override;

    CommandMetadata getMetadata() const override;
};

// **Async Command Example**
class DelayedCommand : public DeclarativeUI::Command::AsyncCommand {
    Q_OBJECT

public:
    explicit DelayedCommand(const CommandContext& context);

    QFuture<CommandResult<QVariant>> executeAsync(
        const CommandContext& context) override;

    CommandMetadata getMetadata() const override;
};

// **Transactional Command Example**
class DatabaseTransactionCommand
    : public DeclarativeUI::Command::TransactionalCommand {
    Q_OBJECT

public:
    explicit DatabaseTransactionCommand(const CommandContext& context);

    void beginTransaction(const CommandContext& context) override;

    void commitTransaction(const CommandContext& context) override;

    void rollbackTransaction(const CommandContext& context) override;

    CommandResult<QVariant> executeTransactional(
        const CommandContext& context) override;

    CommandMetadata getMetadata() const override;
};

// **Command Registration Function**
void registerBuiltinCommands();

}  // namespace Commands
}  // namespace Command
}  // namespace DeclarativeUI
