#include "CommandBinding.hpp"
#include <QDebug>
#include <QMetaMethod>
#include <QMetaObject>
#include <algorithm>

namespace DeclarativeUI::Command::UI {

CommandBindingManager::CommandBindingManager(QObject* parent)
    : QObject(parent) {
    qDebug() << "🔗 CommandBindingManager initialized";
}

CommandBindingManager& CommandBindingManager::instance() {
    static CommandBindingManager instance;
    return instance;
}

void CommandBindingManager::addPropertyBinding(
    BaseUICommand* command, QWidget* widget,
    const PropertyBindingConfig& config) {
    if (!command || !widget) {
        handleError(command,
                    "Cannot add property binding: null command or widget");
        return;
    }

    BindingInfo* info = getBindingInfo(command);
    if (!info) {
        handleError(command, "No binding info found for command");
        return;
    }

    // Remove existing binding for the same property
    auto it = std::find_if(
        info->property_bindings.begin(), info->property_bindings.end(),
        [&config](const PropertyBindingConfig& existing) {
            return existing.command_property == config.command_property;
        });
    if (it != info->property_bindings.end()) {
        info->property_bindings.erase(it);
    }

    info->property_bindings.push_back(config);
    connectPropertyBinding(info, config);

    emit propertyBindingAdded(command, config.command_property);
    qDebug() << "🔗 Added property binding:" << config.command_property << "<->"
             << config.widget_property;
}

void CommandBindingManager::removePropertyBinding(BaseUICommand* command,
                                                  const QString& property) {
    BindingInfo* info = getBindingInfo(command);
    if (!info) {
        return;
    }

    auto it = std::find_if(info->property_bindings.begin(),
                           info->property_bindings.end(),
                           [&property](const PropertyBindingConfig& config) {
                               return config.command_property == property;
                           });
    if (it != info->property_bindings.end()) {
        info->property_bindings.erase(it);
        emit propertyBindingRemoved(command, property);
        qDebug() << "🔌 Removed property binding:" << property;
    }
}

void CommandBindingManager::removeAllPropertyBindings(BaseUICommand* command) {
    BindingInfo* info = getBindingInfo(command);
    if (!info) {
        return;
    }

    for (const auto& config : info->property_bindings) {
        emit propertyBindingRemoved(command, config.command_property);
    }

    info->property_bindings.clear();
    qDebug() << "🧹 Removed all property bindings for command";
}

void CommandBindingManager::addEventBinding(BaseUICommand* command,
                                            QWidget* widget,
                                            const EventBindingConfig& config) {
    if (!command || !widget) {
        handleError(command,
                    "Cannot add event binding: null command or widget");
        return;
    }

    BindingInfo* info = getBindingInfo(command);
    if (!info) {
        handleError(command, "No binding info found for command");
        return;
    }

    // Remove existing binding for the same signal
    auto it =
        std::find_if(info->event_bindings.begin(), info->event_bindings.end(),
                     [&config](const EventBindingConfig& existing) {
                         return existing.widget_signal == config.widget_signal;
                     });
    if (it != info->event_bindings.end()) {
        info->event_bindings.erase(it);
    }

    info->event_bindings.push_back(config);
    connectEventBinding(info, config);

    emit eventBindingAdded(command, config.widget_signal);
    qDebug() << "📡 Added event binding:" << config.widget_signal << "->"
             << config.command_event;
}

void CommandBindingManager::removeEventBinding(BaseUICommand* command,
                                               const QString& signal) {
    BindingInfo* info = getBindingInfo(command);
    if (!info) {
        return;
    }

    auto it =
        std::find_if(info->event_bindings.begin(), info->event_bindings.end(),
                     [&signal](const EventBindingConfig& config) {
                         return config.widget_signal == signal;
                     });
    if (it != info->event_bindings.end()) {
        info->event_bindings.erase(it);
        emit eventBindingRemoved(command, signal);
        qDebug() << "🔌 Removed event binding:" << signal;
    }
}

void CommandBindingManager::removeAllEventBindings(BaseUICommand* command) {
    BindingInfo* info = getBindingInfo(command);
    if (!info) {
        return;
    }

    for (const auto& config : info->event_bindings) {
        emit eventBindingRemoved(command, config.widget_signal);
    }

    info->event_bindings.clear();
    qDebug() << "🧹 Removed all event bindings for command";
}

void CommandBindingManager::addStateBinding(BaseUICommand* command,
                                            const StateBindingConfig& config) {
    if (!command) {
        handleError(command, "Cannot add state binding: null command");
        return;
    }

    BindingInfo* info = getBindingInfo(command);
    if (!info) {
        handleError(command, "No binding info found for command");
        return;
    }

    // Remove existing binding for the same property
    auto it = std::find_if(
        info->state_bindings.begin(), info->state_bindings.end(),
        [&config](const StateBindingConfig& existing) {
            return existing.command_property == config.command_property;
        });
    if (it != info->state_bindings.end()) {
        info->state_bindings.erase(it);
    }

    info->state_bindings.push_back(config);
    connectStateBinding(info, config);

    emit stateBindingAdded(command, config.command_property);
    qDebug() << "🌐 Added state binding:" << config.command_property << "<->"
             << config.state_key;
}

void CommandBindingManager::removeStateBinding(BaseUICommand* command,
                                               const QString& property) {
    BindingInfo* info = getBindingInfo(command);
    if (!info) {
        return;
    }

    auto it =
        std::find_if(info->state_bindings.begin(), info->state_bindings.end(),
                     [&property](const StateBindingConfig& config) {
                         return config.command_property == property;
                     });
    if (it != info->state_bindings.end()) {
        info->state_bindings.erase(it);
        emit stateBindingRemoved(command, property);
        qDebug() << "🔌 Removed state binding:" << property;
    }
}

void CommandBindingManager::removeAllStateBindings(BaseUICommand* command) {
    BindingInfo* info = getBindingInfo(command);
    if (!info) {
        return;
    }

    for (const auto& config : info->state_bindings) {
        emit stateBindingRemoved(command, config.command_property);
    }

    info->state_bindings.clear();
    qDebug() << "🧹 Removed all state bindings for command";
}

void CommandBindingManager::establishBinding(BaseUICommand* command,
                                             QWidget* widget) {
    if (!command || !widget) {
        handleError(command,
                    "Cannot establish binding: null command or widget");
        return;
    }

    // Remove existing binding if any
    removeBinding(command);

    // Create new binding info
    auto info = std::make_unique<BindingInfo>();
    info->command = command;
    info->widget = widget;

    // Set up deferred timer if needed
    if (default_update_mode_ == BindingUpdateMode::Deferred) {
        info->deferred_timer = new QTimer(this);
        info->deferred_timer->setSingleShot(true);
        info->deferred_timer->setInterval(deferred_interval_);
        connect(info->deferred_timer, &QTimer::timeout, this,
                &CommandBindingManager::onDeferredUpdate);
    }

    // Connect to command property changes
    auto connection = connect(command, &BaseUICommand::propertyChanged, this,
                              &CommandBindingManager::onCommandPropertyChanged);
    info->connections.push_back(connection);

    // Store the binding
    bindings_[command] = std::move(info);

    emit bindingEstablished(command, widget);
    qDebug() << "🔗 Established binding for command:"
             << command->getCommandType();
}

void CommandBindingManager::removeBinding(BaseUICommand* command) {
    auto it = bindings_.find(command);
    if (it != bindings_.end()) {
        // Disconnect all connections
        for (const auto& connection : it->second->connections) {
            disconnect(connection);
        }

        bindings_.erase(it);
        emit bindingRemoved(command);
        qDebug() << "🔌 Removed binding for command:"
                 << command->getCommandType();
    }
}

void CommandBindingManager::syncCommandToWidget(BaseUICommand* command,
                                                const QString& property) {
    BindingInfo* info = getBindingInfo(command);
    if (!info || !info->widget) {
        return;
    }

    for (const auto& config : info->property_bindings) {
        if (!property.isEmpty() && config.command_property != property) {
            continue;
        }

        if (config.direction == BindingDirection::OneWayToSource) {
            continue;  // Skip reverse-only bindings
        }

        try {
            QVariant commandValue = command->getState()->getProperty<QVariant>(
                config.command_property);
            QVariant widgetValue =
                convertValue(commandValue, config.command_to_widget_converter);

            if (validateValue(widgetValue, config.validator)) {
                setWidgetProperty(info->widget, config.widget_property,
                                  widgetValue);
            }
        } catch (const std::exception& e) {
            handleError(
                command,
                QString("Command to widget sync failed: %1").arg(e.what()));
        }
    }
}

void CommandBindingManager::syncWidgetToCommand(BaseUICommand* command,
                                                const QString& property) {
    BindingInfo* info = getBindingInfo(command);
    if (!info || !info->widget) {
        return;
    }

    for (const auto& config : info->property_bindings) {
        if (!property.isEmpty() && config.widget_property != property) {
            continue;
        }

        if (config.direction == BindingDirection::OneWay) {
            continue;  // Skip forward-only bindings
        }

        try {
            QVariant widgetValue =
                getWidgetProperty(info->widget, config.widget_property);
            QVariant commandValue =
                convertValue(widgetValue, config.widget_to_command_converter);

            if (validateValue(commandValue, config.validator)) {
                command->getState()->setProperty(config.command_property,
                                                 commandValue);
            }
        } catch (const std::exception& e) {
            handleError(
                command,
                QString("Widget to command sync failed: %1").arg(e.what()));
        }
    }
}

void CommandBindingManager::syncCommandToState(BaseUICommand* command,
                                               const QString& property) {
    BindingInfo* info = getBindingInfo(command);
    if (!info) {
        return;
    }

    auto& stateManager = DeclarativeUI::Binding::StateManager::instance();

    for (const auto& config : info->state_bindings) {
        if (!property.isEmpty() && config.command_property != property) {
            continue;
        }

        if (config.direction == BindingDirection::OneWayToSource) {
            continue;  // Skip reverse-only bindings
        }

        try {
            QVariant commandValue = command->getState()->getProperty<QVariant>(
                config.command_property);
            QVariant stateValue =
                convertValue(commandValue, config.command_to_state_converter);

            // Set state value using the StateManager interface
            stateManager.setState(config.state_key, stateValue);
            qDebug() << "🌐 Syncing command to state:"
                     << config.command_property << "->" << config.state_key;
        } catch (const std::exception& e) {
            handleError(
                command,
                QString("Command to state sync failed: %1").arg(e.what()));
        }
    }
}

void CommandBindingManager::syncStateToCommand(BaseUICommand* command,
                                               const QString& property) {
    BindingInfo* info = getBindingInfo(command);
    if (!info) {
        return;
    }

    auto& stateManager = DeclarativeUI::Binding::StateManager::instance();

    for (const auto& config : info->state_bindings) {
        if (!property.isEmpty() && config.command_property != property) {
            continue;
        }

        if (config.direction == BindingDirection::OneWay) {
            continue;  // Skip forward-only bindings
        }

        try {
            // Get state value using the StateManager interface
            auto statePtr = stateManager.getState<QVariant>(config.state_key);
            QVariant stateValue = statePtr ? statePtr->get() : QVariant{};
            QVariant commandValue =
                convertValue(stateValue, config.state_to_command_converter);

            command->getState()->setProperty(config.command_property,
                                             commandValue);
            qDebug() << "🌐 Syncing state to command:" << config.state_key
                     << "->" << config.command_property;
        } catch (const std::exception& e) {
            handleError(
                command,
                QString("State to command sync failed: %1").arg(e.what()));
        }
    }
}

void CommandBindingManager::beginBatchUpdate() {
    batch_updating_ = true;
    qDebug() << "📦 Batch update started";
}

void CommandBindingManager::endBatchUpdate() {
    if (!batch_updating_) {
        return;
    }

    batch_updating_ = false;

    // Process all batched operations
    for (const auto& operation : batch_operations_) {
        operation();
    }
    batch_operations_.clear();

    qDebug() << "📦 Batch update completed";
}

void CommandBindingManager::setDefaultUpdateMode(BindingUpdateMode mode) {
    default_update_mode_ = mode;
}

void CommandBindingManager::setDeferredUpdateInterval(int milliseconds) {
    deferred_interval_ = milliseconds;
}

bool CommandBindingManager::hasBinding(BaseUICommand* command) const {
    return bindings_.find(command) != bindings_.end();
}

std::vector<QString> CommandBindingManager::getBoundProperties(
    BaseUICommand* command) const {
    std::vector<QString> properties;
    auto it = bindings_.find(command);
    if (it != bindings_.end()) {
        for (const auto& config : it->second->property_bindings) {
            properties.push_back(config.command_property);
        }
    }
    return properties;
}

std::vector<QString> CommandBindingManager::getBoundEvents(
    BaseUICommand* command) const {
    std::vector<QString> events;
    auto it = bindings_.find(command);
    if (it != bindings_.end()) {
        for (const auto& config : it->second->event_bindings) {
            events.push_back(config.widget_signal);
        }
    }
    return events;
}

std::vector<QString> CommandBindingManager::getBoundStates(
    BaseUICommand* command) const {
    std::vector<QString> states;
    auto it = bindings_.find(command);
    if (it != bindings_.end()) {
        for (const auto& config : it->second->state_bindings) {
            states.push_back(config.state_key);
        }
    }
    return states;
}

void CommandBindingManager::onCommandPropertyChanged(const QString& property,
                                                     const QVariant& value) {
    auto* command = qobject_cast<BaseUICommand*>(sender());
    if (!command) {
        return;
    }

    if (batch_updating_) {
        batch_operations_.push_back([this, command, property, value]() {
            processUpdate(command, property, value);
        });
    } else {
        processUpdate(command, property, value);
    }
}

void CommandBindingManager::onWidgetPropertyChanged() {
    // This would be connected to specific widget property change signals
    // Implementation depends on specific widget types and their change signals
}

void CommandBindingManager::onStateChanged(const QString& key,
                                           const QVariant& value) {
    // Handle state manager changes and update bound commands
    for (const auto& [command, info] : bindings_) {
        for (const auto& config : info->state_bindings) {
            if (config.state_key == key &&
                (config.direction == BindingDirection::OneWayToSource ||
                 config.direction == BindingDirection::TwoWay)) {
                QVariant commandValue =
                    convertValue(value, config.state_to_command_converter);
                command->getState()->setProperty(config.command_property,
                                                 commandValue);
            }
        }
    }
}

void CommandBindingManager::onDeferredUpdate() {
    auto* timer = qobject_cast<QTimer*>(sender());
    if (!timer) {
        return;
    }

    // Find the binding info associated with this timer
    for (const auto& [command, info] : bindings_) {
        if (info->deferred_timer == timer) {
            // Process pending updates
            for (const auto& [property, value] : info->pending_updates) {
                processUpdate(command, property, value);
            }
            info->pending_updates.clear();
            break;
        }
    }
}

BindingInfo* CommandBindingManager::getBindingInfo(BaseUICommand* command) {
    auto it = bindings_.find(command);
    return it != bindings_.end() ? it->second.get() : nullptr;
}

void CommandBindingManager::connectPropertyBinding(
    BindingInfo* info, const PropertyBindingConfig& config) {
    // Property bindings are handled through the onCommandPropertyChanged slot
    // and manual sync methods
}

void CommandBindingManager::connectEventBinding(
    BindingInfo* info, const EventBindingConfig& config) {
    if (!info->widget) {
        return;
    }

    // Connect widget signal to command event
    bool connected = connectToWidgetSignal(
        info->widget, config.widget_signal,
        [info, config](const QVariantList& args) {
            QVariant eventData;
            if (config.signal_converter) {
                eventData = config.signal_converter(args);
            }
            info->command->handleEvent(config.command_event, eventData);
        });

    if (!connected) {
        handleError(
            info->command,
            QString("Failed to connect signal: %1").arg(config.widget_signal));
    }
}

void CommandBindingManager::connectStateBinding(
    BindingInfo* info, const StateBindingConfig& config) {
    // Connect to state manager changes
    auto& stateManager = DeclarativeUI::Binding::StateManager::instance();
    auto connection = connect(
        &stateManager, &DeclarativeUI::Binding::StateManager::stateChanged,
        this, &CommandBindingManager::onStateChanged);
    info->connections.push_back(connection);
}

void CommandBindingManager::scheduleUpdate(BaseUICommand* command,
                                           const QString& property,
                                           const QVariant& value) {
    BindingInfo* info = getBindingInfo(command);
    if (!info) {
        return;
    }

    if (default_update_mode_ == BindingUpdateMode::Deferred &&
        info->deferred_timer) {
        info->pending_updates[property] = value;
        info->deferred_timer->start();
    } else {
        processUpdate(command, property, value);
    }
}

void CommandBindingManager::processUpdate(BaseUICommand* command,
                                          const QString& property,
                                          const QVariant& value) {
    syncCommandToWidget(command, property);
    syncCommandToState(command, property);
}

QVariant CommandBindingManager::convertValue(
    const QVariant& value,
    const std::function<QVariant(const QVariant&)>& converter) {
    return converter ? converter(value) : value;
}

bool CommandBindingManager::validateValue(
    const QVariant& value,
    const std::function<bool(const QVariant&)>& validator) {
    return validator ? validator(value) : true;
}

void CommandBindingManager::handleError(BaseUICommand* command,
                                        const QString& error) {
    qWarning() << "Binding error:" << error;
    emit syncError(command, error);
}

bool CommandBindingManager::setWidgetProperty(QWidget* widget,
                                              const QString& property,
                                              const QVariant& value) {
    return widget->setProperty(property.toUtf8().constData(), value);
}

QVariant CommandBindingManager::getWidgetProperty(QWidget* widget,
                                                  const QString& property) {
    return widget->property(property.toUtf8().constData());
}

bool CommandBindingManager::connectToWidgetSignal(
    QWidget* widget, const QString& signal,
    const std::function<void(const QVariantList&)>& handler) {
    // This is a simplified implementation
    // In a real implementation, you would use Qt's meta-object system to
    // connect to the signal
    Q_UNUSED(widget)
    Q_UNUSED(signal)
    Q_UNUSED(handler)
    return true;
}

}  // namespace DeclarativeUI::Command::UI
