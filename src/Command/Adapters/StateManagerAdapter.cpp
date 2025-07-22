#include "StateManagerAdapter.hpp"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <algorithm>

namespace DeclarativeUI::Command::Adapters {

CommandStateManagerAdapter::CommandStateManagerAdapter(QObject* parent) 
    : QObject(parent), state_manager_(Binding::StateManager::instance()) {
    
    // Initialize sync timer
    sync_timer_ = new QTimer(this);
    sync_timer_->setSingleShot(true);
    sync_timer_->setInterval(sync_delay_);
    connect(sync_timer_, &QTimer::timeout, this, &CommandStateManagerAdapter::onDelayedSync);
    
    // Connect to state manager changes
    connect(&state_manager_, &Binding::StateManager::stateChanged,
            this, &CommandStateManagerAdapter::onStateChanged);
    
    qDebug() << "🌐 CommandStateManagerAdapter initialized";
}

CommandStateManagerAdapter& CommandStateManagerAdapter::instance() {
    static CommandStateManagerAdapter instance;
    return instance;
}

void CommandStateManagerAdapter::bindCommand(std::shared_ptr<UI::BaseUICommand> command, 
                                            const QString& stateKey, const QString& commandProperty) {
    if (!command) {
        qWarning() << "Cannot bind null command to state";
        return;
    }
    
    // Check if binding already exists
    size_t existingIndex = findBinding(command, commandProperty);
    if (existingIndex != SIZE_MAX) {
        qDebug() << "Binding already exists, updating state key";
        bindings_[existingIndex].state_key = stateKey;
        return;
    }
    
    CommandBinding binding;
    binding.command = command;
    binding.command_property = commandProperty;
    binding.state_key = stateKey;
    
    establishBinding(binding);
    
    size_t bindingIndex = bindings_.size();
    bindings_.push_back(binding);
    
    // Update indices
    command_to_bindings_[command].push_back(bindingIndex);
    state_to_bindings_[stateKey].push_back(bindingIndex);
    
    emit commandBound(command, stateKey);
    qDebug() << "🔗 Bound command property" << commandProperty << "to state" << stateKey;
}

void CommandStateManagerAdapter::unbindCommand(std::shared_ptr<UI::BaseUICommand> command, const QString& stateKey) {
    if (!command) {
        return;
    }
    
    auto cmdIt = command_to_bindings_.find(command);
    if (cmdIt == command_to_bindings_.end()) {
        return;
    }
    
    std::vector<size_t> indicesToRemove;
    
    for (size_t index : cmdIt->second) {
        if (index < bindings_.size()) {
            const auto& binding = bindings_[index];
            if (stateKey.isEmpty() || binding.state_key == stateKey) {
                indicesToRemove.push_back(index);
                emit commandUnbound(command, binding.state_key);
            }
        }
    }
    
    // Remove bindings in reverse order to maintain indices
    std::sort(indicesToRemove.rbegin(), indicesToRemove.rend());
    for (size_t index : indicesToRemove) {
        removeBinding(index);
    }
    
    updateBindingIndices();
}

void CommandStateManagerAdapter::unbindCommand(std::shared_ptr<UI::BaseUICommand> command,
                                              const QString& stateKey, const QString& property) {
    if (!command) {
        return;
    }

    auto cmdIt = command_to_bindings_.find(command);
    if (cmdIt == command_to_bindings_.end()) {
        return;
    }

    auto& bindingIndices = cmdIt->second;

    // Find and remove specific binding
    for (auto it = bindingIndices.begin(); it != bindingIndices.end(); ++it) {
        const auto& binding = bindings_[*it];
        if (binding.state_key == stateKey && binding.command_property == property) {
            emit commandUnbound(command, binding.state_key);
            removeBinding(*it);
            bindingIndices.erase(it);
            break;
        }
    }

    if (bindingIndices.empty()) {
        command_to_bindings_.erase(cmdIt);
    }

    updateBindingIndices();
}

void CommandStateManagerAdapter::unbindAllCommands(std::shared_ptr<UI::BaseUICommand> command) {
    unbindCommand(command, "");
}

void CommandStateManagerAdapter::bindCommandProperties(std::shared_ptr<UI::BaseUICommand> command, 
                                                      const std::unordered_map<QString, QString>& propertyToStateMap) {
    for (const auto& [property, stateKey] : propertyToStateMap) {
        bindCommand(command, stateKey, property);
    }
}

void CommandStateManagerAdapter::bindCommandHierarchy(std::shared_ptr<UI::BaseUICommand> rootCommand, const QString& statePrefix) {
    if (!rootCommand) {
        return;
    }
    
    // Bind root command
    QString rootStateKey = statePrefix.isEmpty() ? 
        generateStateKey(rootCommand, "value") : 
        QString("%1.%2").arg(statePrefix, generateStateKey(rootCommand, "value"));
    
    bindCommand(rootCommand, rootStateKey, "value");
    
    // Recursively bind children
    for (auto child : rootCommand->getChildren()) {
        QString childPrefix = QString("%1.%2").arg(rootStateKey, child->getId().toString());
        bindCommandHierarchy(child, childPrefix);
    }
}

void CommandStateManagerAdapter::syncCommandToState(std::shared_ptr<UI::BaseUICommand> command, const QString& property) {
    auto cmdIt = command_to_bindings_.find(command);
    if (cmdIt == command_to_bindings_.end()) {
        return;
    }
    
    for (size_t index : cmdIt->second) {
        if (index < bindings_.size()) {
            const auto& binding = bindings_[index];
            if (property.isEmpty() || binding.command_property == property) {
                try {
                    QVariant value = command->getState()->getProperty<QVariant>(binding.command_property);
                    state_manager_.setState(binding.state_key, value);
                    
                    if (change_tracking_enabled_) {
                        changed_states_.insert(binding.state_key);
                    }
                } catch (const std::exception& e) {
                    handleSyncError(command, QString("Command to state sync failed: %1").arg(e.what()));
                }
            }
        }
    }
}

void CommandStateManagerAdapter::syncStateToCommand(std::shared_ptr<UI::BaseUICommand> command, const QString& property) {
    auto cmdIt = command_to_bindings_.find(command);
    if (cmdIt == command_to_bindings_.end()) {
        return;
    }
    
    for (size_t index : cmdIt->second) {
        if (index < bindings_.size()) {
            const auto& binding = bindings_[index];
            if (property.isEmpty() || binding.command_property == property) {
                try {
                    auto statePtr = state_manager_.getState<QVariant>(binding.state_key);
                    QVariant value = statePtr ? statePtr->get() : QVariant{};
                    command->getState()->setProperty(binding.command_property, value);
                } catch (const std::exception& e) {
                    handleSyncError(command, QString("State to command sync failed: %1").arg(e.what()));
                }
            }
        }
    }
}

void CommandStateManagerAdapter::syncAllBoundCommands() {
    for (const auto& binding : bindings_) {
        if (binding.command) {
            syncCommandToState(binding.command, binding.command_property);
        }
    }
}

void CommandStateManagerAdapter::beginBatchUpdate() {
    if (batch_updating_) {
        qWarning() << "Already in batch update mode";
        return;
    }
    
    batch_updating_ = true;
    batch_state_ = std::make_unique<BatchState>();
    
    // Save current state
    batch_state_->original_bindings = bindings_;
    
    emit batchUpdateStarted();
    qDebug() << "📦 Batch update started";
}

void CommandStateManagerAdapter::commitBatchUpdate() {
    if (!batch_updating_) {
        qWarning() << "Not in batch update mode";
        return;
    }
    
    batch_updating_ = false;
    batch_state_.reset();
    
    emit batchUpdateCommitted();
    qDebug() << "✅ Batch update committed";
}

void CommandStateManagerAdapter::rollbackBatchUpdate() {
    if (!batch_updating_) {
        qWarning() << "Not in batch update mode";
        return;
    }
    
    // Restore original state
    bindings_ = batch_state_->original_bindings;
    updateBindingIndices();
    
    batch_updating_ = false;
    batch_state_.reset();
    
    emit batchUpdateRolledBack();
    qDebug() << "↶ Batch update rolled back";
}

void CommandStateManagerAdapter::setCommandValidator(std::shared_ptr<UI::BaseUICommand> command, 
                                                    const QString& property,
                                                    std::function<bool(const QVariant&)> validator) {
    size_t bindingIndex = findBinding(command, property);
    if (bindingIndex != SIZE_MAX) {
        bindings_[bindingIndex].validator = validator;
    }
}

void CommandStateManagerAdapter::removeCommandValidator(std::shared_ptr<UI::BaseUICommand> command, const QString& property) {
    size_t bindingIndex = findBinding(command, property);
    if (bindingIndex != SIZE_MAX) {
        bindings_[bindingIndex].validator = nullptr;
    }
}

bool CommandStateManagerAdapter::validateCommandState(std::shared_ptr<UI::BaseUICommand> command, const QString& property) {
    auto cmdIt = command_to_bindings_.find(command);
    if (cmdIt == command_to_bindings_.end()) {
        return true;
    }
    
    for (size_t index : cmdIt->second) {
        if (index < bindings_.size()) {
            const auto& binding = bindings_[index];
            if (property.isEmpty() || binding.command_property == property) {
                if (binding.validator) {
                    QVariant value = command->getState()->getProperty<QVariant>(binding.command_property);
                    if (!binding.validator(value)) {
                        emit validationFailed(command, binding.command_property, "Validation failed");
                        return false;
                    }
                }
            }
        }
    }
    
    return true;
}

QStringList CommandStateManagerAdapter::getBoundStateKeys(std::shared_ptr<UI::BaseUICommand> command) const {
    QStringList keys;
    auto cmdIt = command_to_bindings_.find(command);
    if (cmdIt != command_to_bindings_.end()) {
        for (size_t index : cmdIt->second) {
            if (index < bindings_.size()) {
                keys.append(bindings_[index].state_key);
            }
        }
    }
    return keys;
}

std::vector<std::shared_ptr<UI::BaseUICommand>> CommandStateManagerAdapter::getCommandsForState(const QString& stateKey) const {
    std::vector<std::shared_ptr<UI::BaseUICommand>> commands;
    auto stateIt = state_to_bindings_.find(stateKey);
    if (stateIt != state_to_bindings_.end()) {
        for (size_t index : stateIt->second) {
            if (index < bindings_.size()) {
                commands.push_back(bindings_[index].command);
            }
        }
    }
    return commands;
}

int CommandStateManagerAdapter::getBoundCommandCount() const {
    return static_cast<int>(command_to_bindings_.size());
}

QStringList CommandStateManagerAdapter::getChangedStates() const {
    QStringList changed;
    for (const QString& state : changed_states_) {
        changed.append(state);
    }
    return changed;
}

void CommandStateManagerAdapter::clearChangeTracking() {
    changed_states_.clear();
}

void CommandStateManagerAdapter::establishBinding(CommandBinding& binding) {
    // Connect command property changes to state updates
    binding.command_connection = connect(binding.command.get(), &UI::BaseUICommand::propertyChanged,
                                        this, &CommandStateManagerAdapter::onCommandPropertyChanged);
    
    // Initial sync from command to state
    if (auto_sync_enabled_) {
        syncCommandToState(binding.command, binding.command_property);
    }
}

void CommandStateManagerAdapter::removeBinding(size_t bindingIndex) {
    if (bindingIndex >= bindings_.size()) {
        return;
    }
    
    const auto& binding = bindings_[bindingIndex];
    
    // Disconnect signals
    disconnect(binding.command_connection);
    disconnect(binding.state_connection);
    
    // Remove from indices
    auto cmdIt = command_to_bindings_.find(binding.command);
    if (cmdIt != command_to_bindings_.end()) {
        auto& indices = cmdIt->second;
        indices.erase(std::remove(indices.begin(), indices.end(), bindingIndex), indices.end());
        if (indices.empty()) {
            command_to_bindings_.erase(cmdIt);
        }
    }
    
    auto stateIt = state_to_bindings_.find(binding.state_key);
    if (stateIt != state_to_bindings_.end()) {
        auto& indices = stateIt->second;
        indices.erase(std::remove(indices.begin(), indices.end(), bindingIndex), indices.end());
        if (indices.empty()) {
            state_to_bindings_.erase(stateIt);
        }
    }
    
    // Remove binding
    bindings_.erase(bindings_.begin() + bindingIndex);
}

void CommandStateManagerAdapter::scheduleSync(std::shared_ptr<UI::BaseUICommand> command) {
    if (!auto_sync_enabled_) {
        return;
    }
    
    pending_sync_commands_.insert(command);
    
    if (!sync_timer_->isActive()) {
        sync_timer_->start();
    }
}

void CommandStateManagerAdapter::performSync(std::shared_ptr<UI::BaseUICommand> command) {
    syncCommandToState(command, "");
}

QString CommandStateManagerAdapter::generateStateKey(std::shared_ptr<UI::BaseUICommand> command, const QString& property) {
    return QString("%1_%2_%3").arg(command->getCommandType(), command->getId().toString(), property);
}

void CommandStateManagerAdapter::validateBinding(const CommandBinding& binding) {
    if (!binding.command) {
        throw std::invalid_argument("Command cannot be null");
    }
    if (binding.command_property.isEmpty()) {
        throw std::invalid_argument("Command property cannot be empty");
    }
    if (binding.state_key.isEmpty()) {
        throw std::invalid_argument("State key cannot be empty");
    }
}

void CommandStateManagerAdapter::handleSyncError(std::shared_ptr<UI::BaseUICommand> command, const QString& error) {
    qWarning() << "State sync error:" << error;
    emit syncError(command, error);
}

size_t CommandStateManagerAdapter::findBinding(std::shared_ptr<UI::BaseUICommand> command, const QString& property) {
    auto cmdIt = command_to_bindings_.find(command);
    if (cmdIt == command_to_bindings_.end()) {
        return SIZE_MAX;
    }
    
    for (size_t index : cmdIt->second) {
        if (index < bindings_.size() && bindings_[index].command_property == property) {
            return index;
        }
    }
    
    return SIZE_MAX;
}

void CommandStateManagerAdapter::updateBindingIndices() {
    command_to_bindings_.clear();
    state_to_bindings_.clear();
    
    for (size_t i = 0; i < bindings_.size(); ++i) {
        const auto& binding = bindings_[i];
        command_to_bindings_[binding.command].push_back(i);
        state_to_bindings_[binding.state_key].push_back(i);
    }
}

void CommandStateManagerAdapter::onStateChanged(const QString& key, const QVariant& value) {
    auto stateIt = state_to_bindings_.find(key);
    if (stateIt != state_to_bindings_.end()) {
        for (size_t index : stateIt->second) {
            if (index < bindings_.size()) {
                const auto& binding = bindings_[index];
                if (binding.command) {
                    binding.command->getState()->setProperty(binding.command_property, value);
                    emit commandStateChanged(binding.command, binding.command_property, value);
                }
            }
        }
    }
}

void CommandStateManagerAdapter::onCommandPropertyChanged(const QString& property, const QVariant& value) {
    auto* command = qobject_cast<UI::BaseUICommand*>(sender());
    if (!command) {
        return;
    }
    
    // Find the shared_ptr for this command
    std::shared_ptr<UI::BaseUICommand> sharedCommand;
    for (const auto& [cmd, indices] : command_to_bindings_) {
        if (cmd.get() == command) {
            sharedCommand = cmd;
            break;
        }
    }
    
    if (sharedCommand) {
        if (auto_sync_enabled_) {
            scheduleSync(sharedCommand);
        }
        emit commandStateChanged(sharedCommand, property, value);
    }
}

void CommandStateManagerAdapter::onDelayedSync() {
    for (auto command : pending_sync_commands_) {
        performSync(command);
    }
    pending_sync_commands_.clear();
}

void CommandStateManagerAdapter::deleteState(const QString& stateKey) {
    state_manager_.removeState(stateKey);
    qDebug() << "🗑️ Deleted state:" << stateKey;
}

}  // namespace DeclarativeUI::Command::Adapters
