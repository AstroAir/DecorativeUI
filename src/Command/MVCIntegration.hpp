#pragma once

#include <QJsonObject>
#include <QObject>
#include <QString>
#include <functional>
#include <memory>
#include <unordered_map>

#include "../Binding/StateManager.hpp"
#include "../Core/UIElement.hpp"
#include "CommandBinding.hpp"
#include "CommandSystem.hpp"
#include "UICommand.hpp"
#include "UICommandFactory.hpp"
#include "WidgetMapper.hpp"

namespace DeclarativeUI::Command::UI {

// **MVC Integration Bridge - connects Command UI system with existing MVC
// architecture**
class MVCIntegrationBridge : public QObject {
    Q_OBJECT

public:
    explicit MVCIntegrationBridge(QObject* parent = nullptr);
    virtual ~MVCIntegrationBridge() = default;

    // **Command-to-UIElement integration**
    std::unique_ptr<Core::UIElement> createUIElementFromCommand(
        std::shared_ptr<BaseUICommand> command);
    std::shared_ptr<BaseUICommand> createCommandFromUIElement(
        Core::UIElement* element);

    // **State management integration**
    void bindCommandToStateManager(std::shared_ptr<BaseUICommand> command,
                                   const QString& stateKey,
                                   const QString& property = "value");
    void unbindCommandFromStateManager(std::shared_ptr<BaseUICommand> command,
                                       const QString& property);

    // **Command system integration**
    void registerCommandAsAction(std::shared_ptr<BaseUICommand> command,
                                 const QString& actionName);
    void executeCommandAction(const QString& actionName,
                              const CommandContext& context = {});

    // **Property binding integration**
    void establishPropertyBinding(std::shared_ptr<BaseUICommand> command,
                                  const QString& commandProperty,
                                  const QString& stateKey);
    void removePropertyBinding(std::shared_ptr<BaseUICommand> command,
                               const QString& commandProperty);

    // **Event integration**
    void connectCommandToAction(std::shared_ptr<BaseUICommand> command,
                                const QString& eventType,
                                const QString& actionName);
    void disconnectCommandFromAction(std::shared_ptr<BaseUICommand> command,
                                     const QString& eventType);

    // **Batch operations**
    void beginMVCTransaction();
    void commitMVCTransaction();
    void rollbackMVCTransaction();

    // **Configuration**
    void setAutoStateBinding(bool enabled) { auto_state_binding_ = enabled; }
    bool isAutoStateBindingEnabled() const { return auto_state_binding_; }

    void setAutoActionRegistration(bool enabled) {
        auto_action_registration_ = enabled;
    }
    bool isAutoActionRegistrationEnabled() const {
        return auto_action_registration_;
    }

    // **Query methods**
    QStringList getBoundStateKeys(std::shared_ptr<BaseUICommand> command) const;
    QStringList getRegisteredActions(
        std::shared_ptr<BaseUICommand> command) const;
    std::shared_ptr<BaseUICommand> getCommandForAction(
        const QString& actionName) const;

    // **Singleton access**
    static MVCIntegrationBridge& instance();

signals:
    void commandBoundToState(std::shared_ptr<BaseUICommand> command,
                             const QString& stateKey);
    void commandUnboundFromState(std::shared_ptr<BaseUICommand> command,
                                 const QString& stateKey);
    void commandRegisteredAsAction(std::shared_ptr<BaseUICommand> command,
                                   const QString& actionName);
    void commandActionExecuted(const QString& actionName,
                               const CommandResult<QVariant>& result);
    void mvcTransactionStarted();
    void mvcTransactionCommitted();
    void mvcTransactionRolledBack();

private:
    // **State binding management**
    struct StateBinding {
        std::shared_ptr<BaseUICommand> command;
        QString command_property;
        QString state_key;
        QMetaObject::Connection state_connection;
        QMetaObject::Connection command_connection;
    };
    std::vector<StateBinding> state_bindings_;

    // **Action registration management**
    struct ActionRegistration {
        std::shared_ptr<BaseUICommand> command;
        QString action_name;
        QString event_type;
        QMetaObject::Connection event_connection;
    };
    std::unordered_map<QString, ActionRegistration> action_registrations_;

    // **Transaction management**
    struct TransactionState {
        std::vector<StateBinding> original_state_bindings;
        std::unordered_map<QString, ActionRegistration>
            original_action_registrations;
        bool in_transaction = false;
    };
    TransactionState transaction_state_;

    // **Configuration**
    bool auto_state_binding_ = true;
    bool auto_action_registration_ = true;

    // **Helper methods**
    void setupAutoBindings(std::shared_ptr<BaseUICommand> command);
    void cleanupBindings(std::shared_ptr<BaseUICommand> command);
    QString generateStateKey(std::shared_ptr<BaseUICommand> command,
                             const QString& property);
    QString generateActionName(std::shared_ptr<BaseUICommand> command,
                               const QString& eventType);

    void connectStateBinding(StateBinding& binding);
    void disconnectStateBinding(StateBinding& binding);
    void connectActionRegistration(ActionRegistration& registration);
    void disconnectActionRegistration(ActionRegistration& registration);

private slots:
    void onStateChanged(const QString& key, const QVariant& value);
    void onCommandPropertyChanged(const QString& property,
                                  const QVariant& value);
    void onCommandEventTriggered(const QString& eventType,
                                 const QVariant& eventData);
};

// **Command-based UIElement wrapper**
class CommandUIElement : public Core::UIElement {
    Q_OBJECT

public:
    explicit CommandUIElement(std::shared_ptr<BaseUICommand> command,
                              QObject* parent = nullptr);
    virtual ~CommandUIElement() = default;

    // **UIElement interface implementation**
    void initialize() override;
    void cleanup() noexcept override;
    void refresh() override;

    // **Command access**
    std::shared_ptr<BaseUICommand> getCommand() const { return command_; }

    // **Property forwarding to command**
    template <typename T>
    CommandUIElement& setCommandProperty(const QString& name, T&& value);

    template <typename T>
    T getCommandProperty(const QString& name,
                         const T& defaultValue = T{}) const;

    // **Event forwarding to command**
    CommandUIElement& onCommandEvent(
        const QString& eventType, std::function<void(const QVariant&)> handler);

protected:
    std::shared_ptr<BaseUICommand> command_;

private:
    void setupCommandIntegration();
    void cleanupCommandIntegration();

private slots:
    void onCommandStateChanged();
    void onCommandEventTriggered(const QString& eventType,
                                 const QVariant& eventData);
};

// **Factory for creating Command-based UI elements**
class CommandUIElementFactory {
public:
    // **Create UIElement from command type**
    static std::unique_ptr<CommandUIElement> createElement(
        const QString& commandType);
    static std::unique_ptr<CommandUIElement> createElement(
        const QString& commandType, const QJsonObject& config);

    // **Create UIElement from existing command**
    static std::unique_ptr<CommandUIElement> createElement(
        std::shared_ptr<BaseUICommand> command);

    // **Batch creation**
    static std::vector<std::unique_ptr<CommandUIElement>> createElements(
        const QStringList& commandTypes);
    static std::vector<std::unique_ptr<CommandUIElement>>
    createElementsFromJson(const QJsonArray& configs);

    // **Create with automatic MVC integration**
    static std::unique_ptr<CommandUIElement> createWithMVCIntegration(
        const QString& commandType, const QString& stateKey = "");
};

// **Convenience functions for MVC integration**
namespace MVC {

// **Quick command creation with state binding**
inline std::shared_ptr<BaseUICommand> createBoundCommand(
    const QString& commandType, const QString& stateKey) {
    auto command = UICommandFactory::instance().createCommand(commandType);
    if (command) {
        MVCIntegrationBridge::instance().bindCommandToStateManager(command,
                                                                   stateKey);
    }
    return command;
}

// **Quick command creation with action registration**
inline std::shared_ptr<BaseUICommand> createActionCommand(
    const QString& commandType, const QString& actionName) {
    auto command = UICommandFactory::instance().createCommand(commandType);
    if (command) {
        MVCIntegrationBridge::instance().registerCommandAsAction(command,
                                                                 actionName);
    }
    return command;
}

// **Create command with both state and action integration**
inline std::shared_ptr<BaseUICommand> createIntegratedCommand(
    const QString& commandType, const QString& stateKey,
    const QString& actionName) {
    auto command = UICommandFactory::instance().createCommand(commandType);
    if (command) {
        MVCIntegrationBridge::instance().bindCommandToStateManager(command,
                                                                   stateKey);
        MVCIntegrationBridge::instance().registerCommandAsAction(command,
                                                                 actionName);
    }
    return command;
}

// **Create UIElement with MVC integration**
inline std::unique_ptr<CommandUIElement> createUIElement(
    const QString& commandType, const QString& stateKey = "") {
    return CommandUIElementFactory::createWithMVCIntegration(commandType,
                                                             stateKey);
}

}  // namespace MVC

// **Template implementations**
template <typename T>
CommandUIElement& CommandUIElement::setCommandProperty(const QString& name,
                                                       T&& value) {
    if (command_) {
        command_->getState()->setProperty(
            name, QVariant::fromValue(std::forward<T>(value)));
    }
    return *this;
}

template <typename T>
T CommandUIElement::getCommandProperty(const QString& name,
                                       const T& defaultValue) const {
    if (command_) {
        return command_->getState()->getProperty<T>(name, defaultValue);
    }
    return defaultValue;
}

}  // namespace DeclarativeUI::Command::UI
