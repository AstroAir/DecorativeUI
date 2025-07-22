#pragma once

#include <QObject>
#include <QWidget>
#include <QTimer>
#include <QMetaProperty>
#include <QMetaMethod>
#include <memory>
#include <functional>
#include <unordered_map>
#include <vector>

#include "UICommand.hpp"
#include "../Binding/StateManager.hpp"

namespace DeclarativeUI::Command::UI {

// **Binding direction enumeration**
enum class BindingDirection {
    OneWay,          // Command -> Widget only
    OneWayToSource,  // Widget -> Command only
    TwoWay           // Bidirectional
};

// **Binding update mode**
enum class BindingUpdateMode {
    Immediate,       // Update immediately when property changes
    Deferred,        // Update after a short delay (batched)
    Manual           // Update only when explicitly requested
};

// **Property binding configuration**
struct PropertyBindingConfig {
    QString command_property;
    QString widget_property;
    BindingDirection direction = BindingDirection::TwoWay;
    BindingUpdateMode update_mode = BindingUpdateMode::Immediate;
    
    // Optional converters
    std::function<QVariant(const QVariant&)> command_to_widget_converter;
    std::function<QVariant(const QVariant&)> widget_to_command_converter;
    
    // Optional validator
    std::function<bool(const QVariant&)> validator;
    
    PropertyBindingConfig() = default;
    PropertyBindingConfig(const QString& cmd_prop, const QString& widget_prop, 
                         BindingDirection dir = BindingDirection::TwoWay)
        : command_property(cmd_prop), widget_property(widget_prop), direction(dir) {}
};

// **Event binding configuration**
struct EventBindingConfig {
    QString widget_signal;
    QString command_event;
    std::function<QVariant(const QVariantList&)> signal_converter;
    
    EventBindingConfig() = default;
    EventBindingConfig(const QString& signal, const QString& event)
        : widget_signal(signal), command_event(event) {}
};

// **State binding configuration for connecting commands to StateManager**
struct StateBindingConfig {
    QString command_property;
    QString state_key;
    BindingDirection direction = BindingDirection::TwoWay;
    std::function<QVariant(const QVariant&)> command_to_state_converter;
    std::function<QVariant(const QVariant&)> state_to_command_converter;
    
    StateBindingConfig() = default;
    StateBindingConfig(const QString& cmd_prop, const QString& state_key, 
                      BindingDirection dir = BindingDirection::TwoWay)
        : command_property(cmd_prop), state_key(state_key), direction(dir) {}
};

// **Binding information for active bindings**
struct BindingInfo {
    BaseUICommand* command = nullptr;
    QWidget* widget = nullptr;
    std::vector<PropertyBindingConfig> property_bindings;
    std::vector<EventBindingConfig> event_bindings;
    std::vector<StateBindingConfig> state_bindings;
    std::vector<QMetaObject::Connection> connections;
    QTimer* deferred_timer = nullptr;
    std::unordered_map<QString, QVariant> pending_updates;
    
    ~BindingInfo() {
        if (deferred_timer) {
            deferred_timer->deleteLater();
        }
    }
};

// **Command Binding Manager - handles all binding operations**
class CommandBindingManager : public QObject {
    Q_OBJECT
    
public:
    explicit CommandBindingManager(QObject* parent = nullptr);
    virtual ~CommandBindingManager() = default;
    
    // **Property binding management**
    void addPropertyBinding(BaseUICommand* command, QWidget* widget, 
                           const PropertyBindingConfig& config);
    void removePropertyBinding(BaseUICommand* command, const QString& property);
    void removeAllPropertyBindings(BaseUICommand* command);
    
    // **Event binding management**
    void addEventBinding(BaseUICommand* command, QWidget* widget, 
                        const EventBindingConfig& config);
    void removeEventBinding(BaseUICommand* command, const QString& signal);
    void removeAllEventBindings(BaseUICommand* command);
    
    // **State binding management**
    void addStateBinding(BaseUICommand* command, const StateBindingConfig& config);
    void removeStateBinding(BaseUICommand* command, const QString& property);
    void removeAllStateBindings(BaseUICommand* command);
    
    // **Complete binding setup/teardown**
    void establishBinding(BaseUICommand* command, QWidget* widget);
    void removeBinding(BaseUICommand* command);
    
    // **Manual synchronization**
    void syncCommandToWidget(BaseUICommand* command, const QString& property = "");
    void syncWidgetToCommand(BaseUICommand* command, const QString& property = "");
    void syncCommandToState(BaseUICommand* command, const QString& property = "");
    void syncStateToCommand(BaseUICommand* command, const QString& property = "");
    
    // **Batch operations**
    void beginBatchUpdate();
    void endBatchUpdate();
    bool isBatchUpdating() const { return batch_updating_; }
    
    // **Configuration**
    void setDefaultUpdateMode(BindingUpdateMode mode);
    BindingUpdateMode getDefaultUpdateMode() const { return default_update_mode_; }
    
    void setDeferredUpdateInterval(int milliseconds);
    int getDeferredUpdateInterval() const { return deferred_interval_; }
    
    // **Query methods**
    bool hasBinding(BaseUICommand* command) const;
    std::vector<QString> getBoundProperties(BaseUICommand* command) const;
    std::vector<QString> getBoundEvents(BaseUICommand* command) const;
    std::vector<QString> getBoundStates(BaseUICommand* command) const;
    
    // **Singleton access**
    static CommandBindingManager& instance();
    
signals:
    void bindingEstablished(BaseUICommand* command, QWidget* widget);
    void bindingRemoved(BaseUICommand* command);
    void propertyBindingAdded(BaseUICommand* command, const QString& property);
    void propertyBindingRemoved(BaseUICommand* command, const QString& property);
    void eventBindingAdded(BaseUICommand* command, const QString& signal);
    void eventBindingRemoved(BaseUICommand* command, const QString& signal);
    void stateBindingAdded(BaseUICommand* command, const QString& property);
    void stateBindingRemoved(BaseUICommand* command, const QString& property);
    void syncError(BaseUICommand* command, const QString& error);
    
private slots:
    void onCommandPropertyChanged(const QString& property, const QVariant& value);
    void onWidgetPropertyChanged();
    void onStateChanged(const QString& key, const QVariant& value);
    void onDeferredUpdate();
    
private:
    // **Active bindings storage**
    std::unordered_map<BaseUICommand*, std::unique_ptr<BindingInfo>> bindings_;
    
    // **Configuration**
    BindingUpdateMode default_update_mode_ = BindingUpdateMode::Immediate;
    int deferred_interval_ = 50;  // milliseconds
    bool batch_updating_ = false;
    std::vector<std::function<void()>> batch_operations_;
    
    // **Helper methods**
    BindingInfo* getBindingInfo(BaseUICommand* command);
    void connectPropertyBinding(BindingInfo* info, const PropertyBindingConfig& config);
    void connectEventBinding(BindingInfo* info, const EventBindingConfig& config);
    void connectStateBinding(BindingInfo* info, const StateBindingConfig& config);
    
    void updateProperty(BaseUICommand* command, QWidget* widget, 
                       const PropertyBindingConfig& config, const QVariant& value);
    void scheduleUpdate(BaseUICommand* command, const QString& property, const QVariant& value);
    void processUpdate(BaseUICommand* command, const QString& property, const QVariant& value);
    
    QVariant convertValue(const QVariant& value, 
                         const std::function<QVariant(const QVariant&)>& converter);
    bool validateValue(const QVariant& value, 
                      const std::function<bool(const QVariant&)>& validator);
    
    void handleError(BaseUICommand* command, const QString& error);
    
    // **Qt Meta-Object utilities**
    bool setWidgetProperty(QWidget* widget, const QString& property, const QVariant& value);
    QVariant getWidgetProperty(QWidget* widget, const QString& property);
    bool connectToWidgetSignal(QWidget* widget, const QString& signal, 
                              const std::function<void(const QVariantList&)>& handler);
};

// **Convenience functions for common binding scenarios**
namespace Binding {

// **Simple property binding**
inline void bindProperty(BaseUICommand* command, QWidget* widget, 
                        const QString& property, BindingDirection direction = BindingDirection::TwoWay) {
    PropertyBindingConfig config(property, property, direction);
    CommandBindingManager::instance().addPropertyBinding(command, widget, config);
}

// **Property binding with different names**
inline void bindProperty(BaseUICommand* command, QWidget* widget, 
                        const QString& commandProperty, const QString& widgetProperty,
                        BindingDirection direction = BindingDirection::TwoWay) {
    PropertyBindingConfig config(commandProperty, widgetProperty, direction);
    CommandBindingManager::instance().addPropertyBinding(command, widget, config);
}

// **State binding**
inline void bindToState(BaseUICommand* command, const QString& property, const QString& stateKey,
                       BindingDirection direction = BindingDirection::TwoWay) {
    StateBindingConfig config(property, stateKey, direction);
    CommandBindingManager::instance().addStateBinding(command, config);
}

// **Event binding**
inline void bindEvent(BaseUICommand* command, QWidget* widget, 
                     const QString& signal, const QString& event) {
    EventBindingConfig config(signal, event);
    CommandBindingManager::instance().addEventBinding(command, widget, config);
}

}  // namespace Binding

}  // namespace DeclarativeUI::Command::UI
