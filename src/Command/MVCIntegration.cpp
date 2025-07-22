#include "MVCIntegration.hpp"
#include <QDebug>
#include <QJsonArray>
#include <algorithm>

namespace DeclarativeUI::Command::UI {

// **MVCIntegrationBridge implementation**
MVCIntegrationBridge::MVCIntegrationBridge(QObject* parent) : QObject(parent) {
    qDebug() << "🌉 MVCIntegrationBridge initialized";
}

MVCIntegrationBridge& MVCIntegrationBridge::instance() {
    static MVCIntegrationBridge instance;
    return instance;
}

std::unique_ptr<Core::UIElement> MVCIntegrationBridge::createUIElementFromCommand(std::shared_ptr<BaseUICommand> command) {
    if (!command) {
        qWarning() << "Cannot create UIElement from null command";
        return nullptr;
    }
    
    auto element = std::make_unique<CommandUIElement>(command);
    
    if (auto_state_binding_ || auto_action_registration_) {
        setupAutoBindings(command);
    }
    
    qDebug() << "🔄 Created UIElement from command:" << command->getCommandType();
    return element;
}

std::shared_ptr<BaseUICommand> MVCIntegrationBridge::createCommandFromUIElement(Core::UIElement* element) {
    if (!element) {
        qWarning() << "Cannot create command from null UIElement";
        return nullptr;
    }
    
    // This would require analyzing the UIElement and creating an appropriate command
    // For now, return nullptr as this is a complex reverse-engineering operation
    qWarning() << "Creating command from UIElement not yet implemented";
    return nullptr;
}

void MVCIntegrationBridge::bindCommandToStateManager(std::shared_ptr<BaseUICommand> command, 
                                                    const QString& stateKey, const QString& property) {
    if (!command) {
        qWarning() << "Cannot bind null command to state";
        return;
    }
    
    QString prop = property.isEmpty() ? "value" : property;
    
    // Check if binding already exists
    auto it = std::find_if(state_bindings_.begin(), state_bindings_.end(),
                          [&](const StateBinding& binding) {
                              return binding.command == command && 
                                     binding.command_property == prop &&
                                     binding.state_key == stateKey;
                          });
    
    if (it != state_bindings_.end()) {
        qDebug() << "State binding already exists for" << prop << "->" << stateKey;
        return;
    }
    
    StateBinding binding;
    binding.command = command;
    binding.command_property = prop;
    binding.state_key = stateKey;
    
    connectStateBinding(binding);
    state_bindings_.push_back(binding);
    
    emit commandBoundToState(command, stateKey);
    qDebug() << "🌐 Bound command property" << prop << "to state" << stateKey;
}

void MVCIntegrationBridge::unbindCommandFromStateManager(std::shared_ptr<BaseUICommand> command, const QString& property) {
    if (!command) {
        return;
    }
    
    auto it = std::remove_if(state_bindings_.begin(), state_bindings_.end(),
                            [&](StateBinding& binding) {
                                if (binding.command == command && 
                                    (property.isEmpty() || binding.command_property == property)) {
                                    disconnectStateBinding(binding);
                                    emit commandUnboundFromState(command, binding.state_key);
                                    return true;
                                }
                                return false;
                            });
    
    state_bindings_.erase(it, state_bindings_.end());
    qDebug() << "🔌 Unbound command from state manager";
}

void MVCIntegrationBridge::registerCommandAsAction(std::shared_ptr<BaseUICommand> command, const QString& actionName) {
    if (!command) {
        qWarning() << "Cannot register null command as action";
        return;
    }
    
    // Remove existing registration if any
    auto it = action_registrations_.find(actionName);
    if (it != action_registrations_.end()) {
        disconnectActionRegistration(it->second);
        action_registrations_.erase(it);
    }
    
    ActionRegistration registration;
    registration.command = command;
    registration.action_name = actionName;
    registration.event_type = "clicked";  // Default event type
    
    connectActionRegistration(registration);
    action_registrations_[actionName] = registration;
    
    emit commandRegisteredAsAction(command, actionName);
    qDebug() << "📋 Registered command as action:" << actionName;
}

void MVCIntegrationBridge::executeCommandAction(const QString& actionName, const CommandContext& context) {
    auto it = action_registrations_.find(actionName);
    if (it == action_registrations_.end()) {
        qWarning() << "Action not found:" << actionName;
        return;
    }
    
    const auto& registration = it->second;
    if (registration.command) {
        // Trigger the command event
        registration.command->handleEvent(registration.event_type);
        
        // Also execute through the command system if available
        auto& commandManager = DeclarativeUI::Command::CommandManager::instance();
        auto result = commandManager.getInvoker().execute(actionName, context);
        
        emit commandActionExecuted(actionName, result);
        qDebug() << "⚡ Executed command action:" << actionName;
    }
}

void MVCIntegrationBridge::establishPropertyBinding(std::shared_ptr<BaseUICommand> command, 
                                                   const QString& commandProperty, const QString& stateKey) {
    bindCommandToStateManager(command, stateKey, commandProperty);
}

void MVCIntegrationBridge::removePropertyBinding(std::shared_ptr<BaseUICommand> command, const QString& commandProperty) {
    unbindCommandFromStateManager(command, commandProperty);
}

void MVCIntegrationBridge::connectCommandToAction(std::shared_ptr<BaseUICommand> command, 
                                                 const QString& eventType, const QString& actionName) {
    if (!command) {
        return;
    }
    
    auto it = action_registrations_.find(actionName);
    if (it != action_registrations_.end()) {
        it->second.event_type = eventType;
        // Reconnect with new event type
        disconnectActionRegistration(it->second);
        connectActionRegistration(it->second);
    }
}

void MVCIntegrationBridge::disconnectCommandFromAction(std::shared_ptr<BaseUICommand> command, const QString& eventType) {
    Q_UNUSED(eventType)
    
    for (auto it = action_registrations_.begin(); it != action_registrations_.end();) {
        if (it->second.command == command) {
            disconnectActionRegistration(it->second);
            it = action_registrations_.erase(it);
        } else {
            ++it;
        }
    }
}

void MVCIntegrationBridge::beginMVCTransaction() {
    if (transaction_state_.in_transaction) {
        qWarning() << "Already in MVC transaction";
        return;
    }
    
    transaction_state_.in_transaction = true;
    transaction_state_.original_state_bindings = state_bindings_;
    transaction_state_.original_action_registrations = action_registrations_;
    
    emit mvcTransactionStarted();
    qDebug() << "📦 MVC transaction started";
}

void MVCIntegrationBridge::commitMVCTransaction() {
    if (!transaction_state_.in_transaction) {
        qWarning() << "No MVC transaction to commit";
        return;
    }
    
    transaction_state_.in_transaction = false;
    transaction_state_.original_state_bindings.clear();
    transaction_state_.original_action_registrations.clear();
    
    emit mvcTransactionCommitted();
    qDebug() << "✅ MVC transaction committed";
}

void MVCIntegrationBridge::rollbackMVCTransaction() {
    if (!transaction_state_.in_transaction) {
        qWarning() << "No MVC transaction to rollback";
        return;
    }
    
    // Restore original state
    state_bindings_ = transaction_state_.original_state_bindings;
    action_registrations_ = transaction_state_.original_action_registrations;
    
    transaction_state_.in_transaction = false;
    transaction_state_.original_state_bindings.clear();
    transaction_state_.original_action_registrations.clear();
    
    emit mvcTransactionRolledBack();
    qDebug() << "↶ MVC transaction rolled back";
}

QStringList MVCIntegrationBridge::getBoundStateKeys(std::shared_ptr<BaseUICommand> command) const {
    QStringList keys;
    for (const auto& binding : state_bindings_) {
        if (binding.command == command) {
            keys.append(binding.state_key);
        }
    }
    return keys;
}

QStringList MVCIntegrationBridge::getRegisteredActions(std::shared_ptr<BaseUICommand> command) const {
    QStringList actions;
    for (const auto& [actionName, registration] : action_registrations_) {
        if (registration.command == command) {
            actions.append(actionName);
        }
    }
    return actions;
}

std::shared_ptr<BaseUICommand> MVCIntegrationBridge::getCommandForAction(const QString& actionName) const {
    auto it = action_registrations_.find(actionName);
    return it != action_registrations_.end() ? it->second.command : nullptr;
}

void MVCIntegrationBridge::setupAutoBindings(std::shared_ptr<BaseUICommand> command) {
    if (!command) {
        return;
    }
    
    if (auto_state_binding_) {
        QString stateKey = generateStateKey(command, "value");
        bindCommandToStateManager(command, stateKey, "value");
    }
    
    if (auto_action_registration_) {
        QString actionName = generateActionName(command, "clicked");
        registerCommandAsAction(command, actionName);
    }
}

void MVCIntegrationBridge::cleanupBindings(std::shared_ptr<BaseUICommand> command) {
    unbindCommandFromStateManager(command, "");
    disconnectCommandFromAction(command, "");
}

QString MVCIntegrationBridge::generateStateKey(std::shared_ptr<BaseUICommand> command, const QString& property) {
    return QString("%1_%2_%3").arg(command->getCommandType(), command->getId().toString(), property);
}

QString MVCIntegrationBridge::generateActionName(std::shared_ptr<BaseUICommand> command, const QString& eventType) {
    return QString("%1_%2_%3").arg(command->getCommandType(), command->getId().toString(), eventType);
}

void MVCIntegrationBridge::connectStateBinding(StateBinding& binding) {
    auto& stateManager = DeclarativeUI::Binding::StateManager::instance();
    
    // Connect state changes to command
    binding.state_connection = connect(&stateManager, &DeclarativeUI::Binding::StateManager::stateChanged,
                                      this, &MVCIntegrationBridge::onStateChanged);
    
    // Connect command changes to state
    binding.command_connection = connect(binding.command.get(), &BaseUICommand::propertyChanged,
                                        this, &MVCIntegrationBridge::onCommandPropertyChanged);
}

void MVCIntegrationBridge::disconnectStateBinding(StateBinding& binding) {
    disconnect(binding.state_connection);
    disconnect(binding.command_connection);
}

void MVCIntegrationBridge::connectActionRegistration(ActionRegistration& registration) {
    registration.event_connection = connect(registration.command.get(), &BaseUICommand::eventTriggered,
                                           this, &MVCIntegrationBridge::onCommandEventTriggered);
}

void MVCIntegrationBridge::disconnectActionRegistration(ActionRegistration& registration) {
    disconnect(registration.event_connection);
}

void MVCIntegrationBridge::onStateChanged(const QString& key, const QVariant& value) {
    for (const auto& binding : state_bindings_) {
        if (binding.state_key == key) {
            binding.command->getState()->setProperty(binding.command_property, value);
        }
    }
}

void MVCIntegrationBridge::onCommandPropertyChanged(const QString& property, const QVariant& value) {
    auto* command = qobject_cast<BaseUICommand*>(sender());
    if (!command) {
        return;
    }
    
    for (const auto& binding : state_bindings_) {
        if (binding.command.get() == command && binding.command_property == property) {
            // Update state manager - this would need to be implemented based on StateManager interface
            qDebug() << "🌐 Updating state" << binding.state_key << "with value" << value;
        }
    }
}

void MVCIntegrationBridge::onCommandEventTriggered(const QString& eventType, const QVariant& eventData) {
    auto* command = qobject_cast<BaseUICommand*>(sender());
    if (!command) {
        return;
    }
    
    for (const auto& [actionName, registration] : action_registrations_) {
        if (registration.command.get() == command && registration.event_type == eventType) {
            executeCommandAction(actionName);
            break;
        }
    }
}

// **CommandUIElement implementation**
CommandUIElement::CommandUIElement(std::shared_ptr<BaseUICommand> command, QObject* parent) 
    : Core::UIElement(parent), command_(command) {
    
    if (command_) {
        setupCommandIntegration();
    }
    
    qDebug() << "🎯 CommandUIElement created for:" << (command_ ? command_->getCommandType() : "null");
}

void CommandUIElement::initialize() {
    if (!command_) {
        throw DeclarativeUI::Exceptions::ComponentCreationException("CommandUIElement: null command");
    }
    
    // Create widget through WidgetMapper
    auto widget = WidgetMapper::instance().createWidget(command_.get());
    if (widget) {
        setWidget(widget.release());
    }
}

void CommandUIElement::cleanup() noexcept {
    try {
        cleanupCommandIntegration();
        Core::UIElement::cleanup();
    } catch (...) {
        // Ensure no-throw guarantee
    }
}

void CommandUIElement::refresh() {
    if (command_) {
        command_->syncToWidget();
    }
    Core::UIElement::refresh();
}

CommandUIElement& CommandUIElement::onCommandEvent(const QString& eventType, std::function<void(const QVariant&)> handler) {
    if (command_) {
        connect(command_.get(), &BaseUICommand::eventTriggered, this, 
                [eventType, handler](const QString& type, const QVariant& data) {
                    if (type == eventType && handler) {
                        handler(data);
                    }
                });
    }
    return *this;
}

void CommandUIElement::setupCommandIntegration() {
    if (!command_) {
        return;
    }
    
    // Connect command state changes to UIElement refresh
    connect(command_.get(), &BaseUICommand::stateChanged, this, &CommandUIElement::onCommandStateChanged);
    connect(command_.get(), &BaseUICommand::eventTriggered, this, &CommandUIElement::onCommandEventTriggered);
}

void CommandUIElement::cleanupCommandIntegration() {
    if (command_) {
        disconnect(command_.get(), nullptr, this, nullptr);
    }
}

void CommandUIElement::onCommandStateChanged() {
    refresh();
}

void CommandUIElement::onCommandEventTriggered(const QString& eventType, const QVariant& eventData) {
    Q_UNUSED(eventType)
    Q_UNUSED(eventData)
    // Event handling is done through the onCommandEvent method
}

// **CommandUIElementFactory implementation**
std::unique_ptr<CommandUIElement> CommandUIElementFactory::createElement(const QString& commandType) {
    auto command = UICommandFactory::instance().createCommand(commandType);
    return command ? std::make_unique<CommandUIElement>(command) : nullptr;
}

std::unique_ptr<CommandUIElement> CommandUIElementFactory::createElement(const QString& commandType, const QJsonObject& config) {
    auto command = UICommandFactory::instance().createCommand(commandType, config);
    return command ? std::make_unique<CommandUIElement>(command) : nullptr;
}

std::unique_ptr<CommandUIElement> CommandUIElementFactory::createElement(std::shared_ptr<BaseUICommand> command) {
    return command ? std::make_unique<CommandUIElement>(command) : nullptr;
}

std::vector<std::unique_ptr<CommandUIElement>> CommandUIElementFactory::createElements(const QStringList& commandTypes) {
    std::vector<std::unique_ptr<CommandUIElement>> elements;
    elements.reserve(commandTypes.size());
    
    for (const QString& type : commandTypes) {
        auto element = createElement(type);
        if (element) {
            elements.push_back(std::move(element));
        }
    }
    
    return elements;
}

std::vector<std::unique_ptr<CommandUIElement>> CommandUIElementFactory::createElementsFromJson(const QJsonArray& configs) {
    std::vector<std::unique_ptr<CommandUIElement>> elements;
    elements.reserve(configs.size());
    
    for (const auto& value : configs) {
        if (value.isObject()) {
            QJsonObject config = value.toObject();
            if (config.contains("type")) {
                QString type = config["type"].toString();
                auto element = createElement(type, config);
                if (element) {
                    elements.push_back(std::move(element));
                }
            }
        }
    }
    
    return elements;
}

std::unique_ptr<CommandUIElement> CommandUIElementFactory::createWithMVCIntegration(const QString& commandType, const QString& stateKey) {
    auto element = createElement(commandType);
    if (element && !stateKey.isEmpty()) {
        MVCIntegrationBridge::instance().bindCommandToStateManager(element->getCommand(), stateKey);
    }
    return element;
}

}  // namespace DeclarativeUI::Command::UI
