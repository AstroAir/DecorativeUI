#pragma once

#include <QObject>
#include <QString>
#include <QVariant>
#include <QTimer>
#include <memory>
#include <functional>
#include <unordered_map>
#include <vector>

#include "../UICommand.hpp"
#include "../CommandBinding.hpp"
#include "../../Binding/StateManager.hpp"
#include "../../Binding/PropertyBinding.hpp"

namespace DeclarativeUI::Command::Adapters {

/**
 * @brief Enhanced StateManager adapter that provides seamless integration with the Command system
 * 
 * This adapter extends the existing StateManager to work optimally with Command objects,
 * providing automatic binding, batch updates, and command-specific state management features.
 */
class CommandStateManagerAdapter : public QObject {
    Q_OBJECT
    
public:
    explicit CommandStateManagerAdapter(QObject* parent = nullptr);
    virtual ~CommandStateManagerAdapter() = default;
    
    // **Command binding management**
    void bindCommand(std::shared_ptr<UI::BaseUICommand> command, const QString& stateKey,
                    const QString& commandProperty = "value");
    void unbindCommand(std::shared_ptr<UI::BaseUICommand> command, const QString& stateKey = "");
    void unbindCommand(std::shared_ptr<UI::BaseUICommand> command, const QString& stateKey, const QString& property);
    void unbindAllCommands(std::shared_ptr<UI::BaseUICommand> command);
    
    // **Batch binding operations**
    void bindCommandProperties(std::shared_ptr<UI::BaseUICommand> command, 
                              const std::unordered_map<QString, QString>& propertyToStateMap);
    void bindCommandHierarchy(std::shared_ptr<UI::BaseUICommand> rootCommand, const QString& statePrefix = "");
    
    // **State synchronization**
    void syncCommandToState(std::shared_ptr<UI::BaseUICommand> command, const QString& property = "");
    void syncStateToCommand(std::shared_ptr<UI::BaseUICommand> command, const QString& property = "");
    void syncAllBoundCommands();
    
    // **Batch state operations**
    void beginBatchUpdate();
    void commitBatchUpdate();
    void rollbackBatchUpdate();
    bool isBatchUpdating() const { return batch_updating_; }
    
    // **Command-specific state operations**
    template<typename T>
    void setCommandState(std::shared_ptr<UI::BaseUICommand> command, const QString& property, const T& value);
    
    template<typename T>
    T getCommandState(std::shared_ptr<UI::BaseUICommand> command, const QString& property, const T& defaultValue = T{});
    
    // **State management operations**
    template<typename T>
    void createState(const QString& stateKey, const T& initialValue);
    void deleteState(const QString& stateKey);

    // **State validation for commands**
    void setCommandValidator(std::shared_ptr<UI::BaseUICommand> command, const QString& property,
                            std::function<bool(const QVariant&)> validator);
    void removeCommandValidator(std::shared_ptr<UI::BaseUICommand> command, const QString& property);
    bool validateCommandState(std::shared_ptr<UI::BaseUICommand> command, const QString& property = "");
    
    // **State persistence for commands**
    void saveCommandState(std::shared_ptr<UI::BaseUICommand> command, const QString& filePath);
    void loadCommandState(std::shared_ptr<UI::BaseUICommand> command, const QString& filePath);
    void saveAllCommandStates(const QString& filePath);
    void loadAllCommandStates(const QString& filePath);
    
    // **State change tracking**
    void enableChangeTracking(bool enabled) { change_tracking_enabled_ = enabled; }
    bool isChangeTrackingEnabled() const { return change_tracking_enabled_; }
    QStringList getChangedStates() const;
    void clearChangeTracking();
    
    // **Command state queries**
    QStringList getBoundStateKeys(std::shared_ptr<UI::BaseUICommand> command) const;
    std::vector<std::shared_ptr<UI::BaseUICommand>> getCommandsForState(const QString& stateKey) const;
    int getBoundCommandCount() const;
    
    // **Configuration**
    void setAutoSync(bool enabled) { auto_sync_enabled_ = enabled; }
    bool isAutoSyncEnabled() const { return auto_sync_enabled_; }
    
    void setSyncDelay(int milliseconds) { sync_delay_ = milliseconds; }
    int getSyncDelay() const { return sync_delay_; }
    
    // **Integration with existing StateManager**
    Binding::StateManager& getStateManager() { return state_manager_; }
    const Binding::StateManager& getStateManager() const { return state_manager_; }
    
    // **Singleton access**
    static CommandStateManagerAdapter& instance();
    
signals:
    void commandBound(std::shared_ptr<UI::BaseUICommand> command, const QString& stateKey);
    void commandUnbound(std::shared_ptr<UI::BaseUICommand> command, const QString& stateKey);
    void commandStateChanged(std::shared_ptr<UI::BaseUICommand> command, const QString& property, const QVariant& value);
    void batchUpdateStarted();
    void batchUpdateCommitted();
    void batchUpdateRolledBack();
    void validationFailed(std::shared_ptr<UI::BaseUICommand> command, const QString& property, const QString& error);
    void syncError(std::shared_ptr<UI::BaseUICommand> command, const QString& error);
    
private slots:
    void onStateChanged(const QString& key, const QVariant& value);
    void onCommandPropertyChanged(const QString& property, const QVariant& value);
    void onDelayedSync();
    
private:
    // **Binding information**
    struct CommandBinding {
        std::shared_ptr<UI::BaseUICommand> command;
        QString command_property;
        QString state_key;
        QMetaObject::Connection command_connection;
        QMetaObject::Connection state_connection;
        std::function<bool(const QVariant&)> validator;
    };
    
    std::vector<CommandBinding> bindings_;
    std::unordered_map<std::shared_ptr<UI::BaseUICommand>, std::vector<size_t>> command_to_bindings_;
    std::unordered_map<QString, std::vector<size_t>> state_to_bindings_;
    
    // **Batch update management**
    bool batch_updating_ = false;
    struct BatchState {
        std::unordered_map<QString, QVariant> original_states;
        std::vector<CommandBinding> original_bindings;
    };
    std::unique_ptr<BatchState> batch_state_;
    
    // **Change tracking**
    bool change_tracking_enabled_ = false;
    std::unordered_set<QString> changed_states_;
    
    // **Configuration**
    bool auto_sync_enabled_ = true;
    int sync_delay_ = 50;  // milliseconds
    QTimer* sync_timer_;
    std::unordered_set<std::shared_ptr<UI::BaseUICommand>> pending_sync_commands_;
    
    // **Dependencies**
    Binding::StateManager& state_manager_;
    
    // **Helper methods**
    void establishBinding(CommandBinding& binding);
    void removeBinding(size_t bindingIndex);
    void scheduleSync(std::shared_ptr<UI::BaseUICommand> command);
    void performSync(std::shared_ptr<UI::BaseUICommand> command);
    
    QString generateStateKey(std::shared_ptr<UI::BaseUICommand> command, const QString& property);
    void validateBinding(const CommandBinding& binding);
    void handleSyncError(std::shared_ptr<UI::BaseUICommand> command, const QString& error);
    
    size_t findBinding(std::shared_ptr<UI::BaseUICommand> command, const QString& property);
    void updateBindingIndices();
};

/**
 * @brief Property binding adapter that integrates existing PropertyBinding with Commands
 */
class CommandPropertyBindingAdapter {
public:
    // **Create property binding for command**
    static std::unique_ptr<Binding::IPropertyBinding> createBinding(
        std::shared_ptr<UI::BaseUICommand> command, const QString& commandProperty,
        QObject* target, const QString& targetProperty);
    
    // **Create bidirectional binding**
    static std::pair<std::unique_ptr<Binding::IPropertyBinding>, std::unique_ptr<Binding::IPropertyBinding>>
    createBidirectionalBinding(
        std::shared_ptr<UI::BaseUICommand> command, const QString& commandProperty,
        QObject* target, const QString& targetProperty);
    
    // **Batch binding creation**
    static std::vector<std::unique_ptr<Binding::IPropertyBinding>> createBindings(
        std::shared_ptr<UI::BaseUICommand> command,
        const std::vector<std::pair<QString, std::pair<QObject*, QString>>>& bindings);
    
    // **Integration with existing binding system**
    static void integrateWithExistingBindings(std::shared_ptr<UI::BaseUICommand> command);
    
private:
    static void setupCommandToTargetBinding(std::shared_ptr<UI::BaseUICommand> command, 
                                           const QString& commandProperty,
                                           QObject* target, const QString& targetProperty);
    static void setupTargetToCommandBinding(QObject* target, const QString& targetProperty,
                                           std::shared_ptr<UI::BaseUICommand> command, 
                                           const QString& commandProperty);
};

/**
 * @brief State validation adapter for Commands
 */
class CommandStateValidator {
public:
    // **Register validators for command types**
    static void registerCommandValidator(const QString& commandType, const QString& property,
                                        std::function<bool(const QVariant&)> validator);
    
    // **Validate command state**
    static bool validateCommand(std::shared_ptr<UI::BaseUICommand> command);
    static QStringList getValidationErrors(std::shared_ptr<UI::BaseUICommand> command);
    
    // **Built-in validators**
    static std::function<bool(const QVariant&)> requiredValidator();
    static std::function<bool(const QVariant&)> rangeValidator(const QVariant& min, const QVariant& max);
    static std::function<bool(const QVariant&)> regexValidator(const QString& pattern);
    static std::function<bool(const QVariant&)> lengthValidator(int minLength, int maxLength = -1);
    
private:
    static std::unordered_map<QString, std::unordered_map<QString, std::function<bool(const QVariant&)>>> validators_;
};

// **Template implementations**
template<typename T>
void CommandStateManagerAdapter::setCommandState(std::shared_ptr<UI::BaseUICommand> command, 
                                                 const QString& property, const T& value) {
    if (!command) {
        return;
    }
    
    // Set the property on the command
    command->getState()->setProperty(property, QVariant::fromValue(value));
    
    // If auto-sync is enabled, sync to state manager
    if (auto_sync_enabled_) {
        scheduleSync(command);
    }
}

template<typename T>
T CommandStateManagerAdapter::getCommandState(std::shared_ptr<UI::BaseUICommand> command, 
                                              const QString& property, const T& defaultValue) {
    if (!command) {
        return defaultValue;
    }
    
    return command->getState()->getProperty<T>(property, defaultValue);
}

// **Convenience functions for Command state management**
namespace State {

// **Quick binding functions**
inline void bind(std::shared_ptr<UI::BaseUICommand> command, const QString& stateKey, 
                const QString& property = "value") {
    CommandStateManagerAdapter::instance().bindCommand(command, stateKey, property);
}

inline void unbind(std::shared_ptr<UI::BaseUICommand> command, const QString& stateKey = "") {
    CommandStateManagerAdapter::instance().unbindCommand(command, stateKey);
}

// **Quick state access**
template<typename T>
inline void set(std::shared_ptr<UI::BaseUICommand> command, const QString& property, const T& value) {
    CommandStateManagerAdapter::instance().setCommandState(command, property, value);
}

template<typename T>
inline T get(std::shared_ptr<UI::BaseUICommand> command, const QString& property, const T& defaultValue = T{}) {
    return CommandStateManagerAdapter::instance().getCommandState(command, property, defaultValue);
}

// **Batch operations**
inline void beginBatch() {
    CommandStateManagerAdapter::instance().beginBatchUpdate();
}

inline void commitBatch() {
    CommandStateManagerAdapter::instance().commitBatchUpdate();
}

inline void rollbackBatch() {
    CommandStateManagerAdapter::instance().rollbackBatchUpdate();
}

// **Validation helpers**
inline bool validate(std::shared_ptr<UI::BaseUICommand> command) {
    return CommandStateValidator::validateCommand(command);
}

inline QStringList getErrors(std::shared_ptr<UI::BaseUICommand> command) {
    return CommandStateValidator::getValidationErrors(command);
}

}  // namespace State

// **Template implementations**
template<typename T>
void CommandStateManagerAdapter::createState(const QString& stateKey, const T& initialValue) {
    state_manager_.setState(stateKey, QVariant::fromValue(initialValue));
    qDebug() << "🆕 Created state:" << stateKey << "with value:" << initialValue;
}

}  // namespace DeclarativeUI::Command::Adapters
