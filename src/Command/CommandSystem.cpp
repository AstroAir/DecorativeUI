#include "CommandSystem.hpp"
#include <QCoreApplication>
#include <QDebug>
#include <QtConcurrent/QtConcurrent>

namespace DeclarativeUI::Command {

CommandMetadata::CommandMetadata() = default;
CommandMetadata::CommandMetadata(const QString& cmd_name,
                                 const QString& cmd_desc)
    : name(cmd_name), description(cmd_desc) {}

CommandContext::CommandContext() = default;
CommandContext::CommandContext(const QJsonObject& data) : data_(data) {}

bool CommandContext::hasParameter(const QString& key) const {
    return data_.contains(key);
}

QJsonObject CommandContext::getData() const { return data_; }
void CommandContext::setData(const QJsonObject& data) { data_ = data; }

// CommandResult implementations
template <typename T>
CommandResult<T>::CommandResult() = default;

template <typename T>
CommandResult<T>::CommandResult(const T& result) : result_(result), success_(true) {}

template <typename T>
CommandResult<T>::CommandResult(const QString& error) : error_(error), success_(false) {}

template <typename T>
bool CommandResult<T>::isSuccess() const { return success_; }

template <typename T>
bool CommandResult<T>::isError() const { return !success_; }

template <typename T>
const T& CommandResult<T>::getResult() const { return result_; }

template <typename T>
const QString& CommandResult<T>::getError() const { return error_; }

template <typename T>
QJsonObject CommandResult<T>::toJson() const {
    QJsonObject json;
    json["success"] = success_;
    if (success_) {
        json["result"] = QJsonValue::fromVariant(QVariant::fromValue(result_));
    } else {
        json["error"] = error_;
    }
    return json;
}

// Explicit template instantiation
template class CommandResult<QVariant>;

// CommandError implementation
CommandError::CommandError(const QString& message, const QString& code)
    : message_(message),
      code_(code),
      timestamp_(QDateTime::currentDateTime()) {}

const char* CommandError::what() const noexcept {
    return message_.toUtf8().constData();
}

const QString& CommandError::getMessage() const { return message_; }
const QString& CommandError::getCode() const { return code_; }
const QDateTime& CommandError::getTimestamp() const { return timestamp_; }

CommandError& CommandError::setContext(const CommandContext& context) {
    context_ = context;
    return *this;
}

CommandError& CommandError::setStackTrace(const QStringList& trace) {
    stack_trace_ = trace;
    return *this;
}

QJsonObject CommandError::toJson() const {
    QJsonObject json;
    json["message"] = message_;
    json["code"] = code_;
    json["timestamp"] = timestamp_.toString(Qt::ISODate);
    json["context"] = context_.getData();
    json["stack_trace"] = QJsonArray::fromStringList(stack_trace_);
    return json;
}

// ICommand implementation
ICommand::ICommand(QObject* parent) : QObject(parent) {}
ICommand::~ICommand() = default;

CommandResult<QVariant> ICommand::undo(const CommandContext& context) {
    return CommandResult<QVariant>(QString("Undo not implemented"));
}

CommandResult<QVariant> ICommand::redo(const CommandContext& context) {
    return CommandResult<QVariant>(QString("Redo not implemented"));
}

bool ICommand::canExecute(const CommandContext& context) const {
    return true;
}

bool ICommand::canUndo(const CommandContext& context) const { return false; }
bool ICommand::canRedo(const CommandContext& context) const { return false; }

void ICommand::onBeforeExecute(const CommandContext& context) {}
void ICommand::onAfterExecute(const CommandContext& context,
                            const CommandResult<QVariant>& result) {}
void ICommand::onError(const CommandContext& context,
                     const CommandError& error) {}

CommandState ICommand::getState() const { return state_; }
void ICommand::setState(CommandState state) {
    state_ = state;
    emit stateChanged(state);
}

const ICommand::ExecutionStats& ICommand::getStats() const { return stats_; }

void ICommand::updateProgress(int percentage) { emit progressUpdated(percentage); }
void ICommand::updateMessage(const QString& message) { emit messageUpdated(message); }

// AsyncCommand implementation
AsyncCommand::AsyncCommand(QObject* parent) : ICommand(parent) {}

CommandResult<QVariant> AsyncCommand::execute(const CommandContext& context) {
    auto future = executeAsync(context);
    future.waitForFinished();
    return future.result();
}

// TransactionalCommand implementation
TransactionalCommand::TransactionalCommand(QObject* parent) : ICommand(parent) {}

CommandResult<QVariant> TransactionalCommand::execute(const CommandContext& context) {
    try {
        beginTransaction(context);
        auto result = executeTransactional(context);
        if (result.isSuccess()) {
            commitTransaction(context);
        } else {
            rollbackTransaction(context);
        }
        return result;
    } catch (const std::exception& e) {
        rollbackTransaction(context);
        return CommandResult<QVariant>(
            QString("Transaction failed: %1").arg(e.what()));
    }
}

// CompositeCommand implementation
CompositeCommand::CompositeCommand(QObject* parent) : ICommand(parent) {}

void CompositeCommand::addCommand(std::unique_ptr<ICommand> command) {
    commands_.push_back(std::move(command));
}

CommandResult<QVariant> CompositeCommand::execute(const CommandContext& context) {
    QVariantList results;

    for (auto& command : commands_) {
        auto result = command->execute(context);
        if (result.isError()) {
            // Rollback previous commands
            rollbackCommands(context);
            return result;
        }
        results.append(result.getResult());
    }

    return CommandResult<QVariant>(QVariant::fromValue(results));
}

CommandResult<QVariant> CompositeCommand::undo(const CommandContext& context) {
    return rollbackCommands(context);
}

CommandMetadata CompositeCommand::getMetadata() const {
    return CommandMetadata("CompositeCommand",
                           "Executes multiple commands as a single unit");
}

CommandResult<QVariant> CompositeCommand::rollbackCommands(const CommandContext& context) {
    // Undo commands in reverse order
    for (auto it = commands_.rbegin(); it != commands_.rend(); ++it) {
        (*it)->undo(context);
    }
    return CommandResult<QVariant>(QVariant());
}

// CommandFactory implementation
CommandFactory& CommandFactory::instance() {
    static CommandFactory factory;
    return factory;
}

CommandFactory::CommandFactory() = default;

std::unique_ptr<ICommand> CommandFactory::createCommand(
    const QString& command_name, const CommandContext& context) {
    auto it = creators_.find(command_name);
    if (it != creators_.end()) {
        return it->second(context);
    }
    return nullptr;
}

void CommandFactory::registerCommand(
    const QString& command_name,
    std::function<std::unique_ptr<ICommand>(const CommandContext&)> creator) {
    creators_[command_name] = std::move(creator);
}

void CommandFactory::unregisterCommand(const QString& command_name) {
    creators_.erase(command_name);
}

std::vector<QString> CommandFactory::getRegisteredCommands() const {
    std::vector<QString> result;
    for (const auto& [name, creator] : creators_) {
        result.push_back(name);
    }
    return result;
}

// CommandInvoker implementation
CommandInvoker::CommandInvoker(QObject* parent) : QObject(parent) {
    batch_timer_ = new QTimer(this);
    batch_timer_->setSingleShot(true);
    connect(batch_timer_, &QTimer::timeout, this,
            &CommandInvoker::processBatch);
}

CommandResult<QVariant> CommandInvoker::execute(const QString& command_name,
                                const CommandContext& context) {
    return executeInternal(command_name, context, ExecutionMode::Synchronous);
}

QFuture<CommandResult<QVariant>> CommandInvoker::executeAsync(
    const QString& command_name, const CommandContext& context) {
    return QtConcurrent::run(
        [this, command_name, context]() -> CommandResult<QVariant> {
            return executeInternal(command_name, context,
                                   ExecutionMode::Asynchronous);
        });
}

void CommandInvoker::executeDeferred(
    const QString& command_name, const CommandContext& context,
    std::chrono::milliseconds delay) {
    QTimer::singleShot(delay, [this, command_name, context]() {
        executeInternal(command_name, context, ExecutionMode::Deferred);
    });
}

void CommandInvoker::executeBatch(const std::vector<QString>& command_names,
                  const CommandContext& context) {
    for (const auto& name : command_names) {
        batch_queue_.push({name, context});
    }

    if (!batch_timer_->isActive()) {
        batch_timer_->start(batch_interval_);
    }
}

void CommandInvoker::cancelCommand(const QUuid& command_id) {
    auto it = running_commands_.find(command_id);
    if (it != running_commands_.end()) {
        it->second->setState(CommandState::Cancelled);
        running_commands_.erase(it);
    }
}

std::vector<std::pair<QUuid, CommandState>> CommandInvoker::getRunningCommands() const {
    std::vector<std::pair<QUuid, CommandState>> result;
    for (const auto& [id, command] : running_commands_) {
        result.emplace_back(id, command->getState());
    }
    return result;
}

void CommandInvoker::processBatch() {
    while (!batch_queue_.empty()) {
        auto item = batch_queue_.front();
        batch_queue_.pop();

        executeInternal(item.command_name, item.context,
                        ExecutionMode::Batched);
    }
}

CommandResult<QVariant> CommandInvoker::executeInternal(const QString& command_name,
                                        const CommandContext& context,
                                        ExecutionMode mode) {
    auto command = CommandFactory::instance().createCommand(command_name, context);
    if (!command) {
        return CommandResult<QVariant>(
            QString("Command '%1' not found").arg(command_name));
    }

    auto command_id = command->getMetadata().id;

    {
        QWriteLocker locker(&commands_lock_);
        running_commands_[command_id] = std::move(command);
    }

    auto* cmd_ptr = running_commands_[command_id].get();

    // Connect progress signals
    connect(cmd_ptr, &ICommand::progressUpdated, this,
            [this, command_id](int percentage) {
                emit commandProgress(command_id, percentage);
            });

    try {
        cmd_ptr->setState(CommandState::Executing);

        auto start_time = std::chrono::high_resolution_clock::now();
        auto result = cmd_ptr->execute(context);
        auto end_time = std::chrono::high_resolution_clock::now();

        // Track execution time for potential metrics
        [[maybe_unused]] auto duration =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                end_time - start_time);

        if (result.isSuccess()) {
            cmd_ptr->setState(CommandState::Completed);
            emit commandExecuted(command_name, result);
        } else {
            cmd_ptr->setState(CommandState::Failed);
            emit commandFailed(command_name, CommandError(result.getError()));
        }

        {
            QWriteLocker locker(&commands_lock_);
            running_commands_.erase(command_id);
        }

        return result;

    } catch (const std::exception& e) {
        cmd_ptr->setState(CommandState::Failed);
        auto error = CommandError(
            QString("Command execution failed: %1").arg(e.what()));
        emit commandFailed(command_name, error);

        {
            QWriteLocker locker(&commands_lock_);
            running_commands_.erase(command_id);
        }

        return CommandResult<QVariant>(error.getMessage());
    }
}

// CommandInterceptor implementation
CommandInterceptor::CommandInterceptor(QObject* parent) : QObject(parent) {}
CommandInterceptor::~CommandInterceptor() = default;

// CommandManager implementation
CommandManager& CommandManager::instance() {
    static CommandManager instance;
    return instance;
}

CommandManager::CommandManager() = default;

CommandInvoker& CommandManager::getInvoker() { return invoker_; }

void CommandManager::addInterceptor(std::unique_ptr<CommandInterceptor> interceptor) {
    interceptors_.push_back(std::move(interceptor));
}

void CommandManager::enableCommandHistory(bool enabled) { history_enabled_ = enabled; }
bool CommandManager::canUndo() const { return !command_history_.empty(); }
bool CommandManager::canRedo() const { return !redo_stack_.empty(); }

void CommandManager::undo() {
    if (canUndo()) {
        auto& last_command = command_history_.back();
        last_command.command->undo(last_command.context);
        redo_stack_.push_back(std::move(last_command));
        command_history_.pop_back();
    }
}

void CommandManager::redo() {
    if (canRedo()) {
        auto& last_redo = redo_stack_.back();
        last_redo.command->execute(last_redo.context);
        command_history_.push_back(std::move(last_redo));
        redo_stack_.pop_back();
    }
}

void CommandManager::enableAuditTrail(bool enabled) { audit_enabled_ = enabled; }
QJsonArray CommandManager::getAuditTrail() const { return audit_trail_; }

}  // namespace DeclarativeUI::Command

#include "CommandSystem.moc"
