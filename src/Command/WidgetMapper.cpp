#include "WidgetMapper.hpp"
#include <QDebug>
#include <QMetaMethod>
#include <QSignalMapper>
#include <QPushButton>
#include <algorithm>

namespace DeclarativeUI::Command::UI {

WidgetMapper::WidgetMapper(QObject* parent) : QObject(parent) {
    setupDefaultMappings();
    qDebug() << "🗺️ WidgetMapper initialized";
}

WidgetMapper& WidgetMapper::instance() {
    static WidgetMapper instance;
    return instance;
}

void WidgetMapper::registerMapping(const QString& command_type, const WidgetMappingConfig& config) {
    mappings_[command_type] = config;
    qDebug() << "📝 Registered mapping:" << command_type << "->" << config.widget_type;
}

std::unique_ptr<QWidget> WidgetMapper::createWidget(BaseUICommand* command) {
    if (!command) {
        qWarning() << "Cannot create widget for null command";
        return nullptr;
    }
    
    const QString command_type = command->getCommandType();
    auto it = mappings_.find(command_type);
    if (it == mappings_.end()) {
        qWarning() << "No mapping found for command type:" << command_type;
        return nullptr;
    }
    
    const auto& config = it->second;
    if (!config.factory) {
        qWarning() << "No factory function for command type:" << command_type;
        return nullptr;
    }
    
    try {
        auto widget = config.factory();
        if (!widget) {
            qWarning() << "Factory returned null widget for:" << command_type;
            return nullptr;
        }
        
        // Set up the widget-command relationship
        establishBinding(command, widget.get());
        
        // Apply custom setup if available
        if (config.custom_setup) {
            config.custom_setup(widget.get(), command);
        }
        
        emit widgetCreated(command, widget.get());
        qDebug() << "✨ Created widget for command:" << command_type;
        
        return widget;
        
    } catch (const std::exception& e) {
        handleSyncError(command, QString("Widget creation failed: %1").arg(e.what()));
        return nullptr;
    }
}

void WidgetMapper::destroyWidget(BaseUICommand* command) {
    if (!command) {
        return;
    }
    
    removeBinding(command);
    emit widgetDestroyed(command);
    qDebug() << "🗑️ Destroyed widget for command:" << command->getCommandType();
}

void WidgetMapper::establishBinding(BaseUICommand* command, QWidget* widget) {
    if (!command || !widget) {
        qWarning() << "Cannot establish binding with null command or widget";
        return;
    }
    
    // Remove existing binding if any
    removeBinding(command);
    
    // Create new binding info
    BindingInfo binding;
    binding.command = command;
    binding.widget = widget;
    
    // Set up property synchronization
    const QString command_type = command->getCommandType();
    auto mapping_it = mappings_.find(command_type);
    if (mapping_it != mappings_.end()) {
        const auto& config = mapping_it->second;
        
        // Connect property synchronization
        for (const auto& prop_config : config.property_mappings) {
            connectPropertySync(command, widget, prop_config);
        }
        
        // Connect event mappings
        for (const auto& event_config : config.event_mappings) {
            connectEventMapping(command, widget, event_config);
        }
    }
    
    // Connect to command property changes
    auto connection = connect(command, &BaseUICommand::propertyChanged,
                             this, &WidgetMapper::onCommandPropertyChanged);
    binding.connections.push_back(connection);
    
    // Store the binding
    active_bindings_[command] = std::move(binding);
    
    // Initial sync from command to widget
    syncCommandToWidget(command);
    
    emit bindingEstablished(command, widget);
    qDebug() << "🔗 Established binding for:" << command->getCommandType();
}

void WidgetMapper::removeBinding(BaseUICommand* command) {
    auto it = active_bindings_.find(command);
    if (it != active_bindings_.end()) {
        // Disconnect all connections
        for (const auto& connection : it->second.connections) {
            disconnect(connection);
        }
        
        active_bindings_.erase(it);
        emit bindingRemoved(command);
        qDebug() << "🔌 Removed binding for:" << command->getCommandType();
    }
}

void WidgetMapper::syncCommandToWidget(BaseUICommand* command, const QString& property) {
    auto it = active_bindings_.find(command);
    if (it == active_bindings_.end()) {
        return;
    }
    
    QWidget* widget = it->second.widget;
    if (!widget) {
        return;
    }
    
    const QString command_type = command->getCommandType();
    auto mapping_it = mappings_.find(command_type);
    if (mapping_it == mappings_.end()) {
        return;
    }
    
    const auto& config = mapping_it->second;
    
    // Sync specific property or all properties
    for (const auto& prop_config : config.property_mappings) {
        if (!property.isEmpty() && prop_config.command_property != property) {
            continue;
        }
        
        try {
            QVariant command_value = command->getState()->getProperty<QVariant>(prop_config.command_property);
            QVariant widget_value = command_value;
            
            // Apply converter if available
            if (prop_config.command_to_widget_converter) {
                widget_value = convertProperty(command_value, prop_config.command_to_widget_converter);
            }
            
            setWidgetProperty(widget, prop_config.widget_property, widget_value);
            
        } catch (const std::exception& e) {
            handleSyncError(command, QString("Property sync failed for %1: %2")
                          .arg(prop_config.command_property, e.what()));
        }
    }
}

void WidgetMapper::syncWidgetToCommand(QWidget* widget, BaseUICommand* command, const QString& property) {
    if (!widget || !command) {
        return;
    }
    
    const QString command_type = command->getCommandType();
    auto mapping_it = mappings_.find(command_type);
    if (mapping_it == mappings_.end()) {
        return;
    }
    
    const auto& config = mapping_it->second;
    
    // Sync specific property or all bidirectional properties
    for (const auto& prop_config : config.property_mappings) {
        if (!prop_config.bidirectional) {
            continue;
        }
        
        if (!property.isEmpty() && prop_config.widget_property != property) {
            continue;
        }
        
        try {
            QVariant widget_value = getWidgetProperty(widget, prop_config.widget_property);
            QVariant command_value = widget_value;
            
            // Apply converter if available
            if (prop_config.widget_to_command_converter) {
                command_value = convertProperty(widget_value, prop_config.widget_to_command_converter);
            }
            
            command->getState()->setProperty(prop_config.command_property, command_value);
            
        } catch (const std::exception& e) {
            handleSyncError(command, QString("Widget to command sync failed for %1: %2")
                          .arg(prop_config.widget_property, e.what()));
        }
    }
}

void WidgetMapper::connectEvents(BaseUICommand* command, QWidget* widget) {
    const QString command_type = command->getCommandType();
    auto mapping_it = mappings_.find(command_type);
    if (mapping_it == mappings_.end()) {
        return;
    }
    
    const auto& config = mapping_it->second;
    for (const auto& event_config : config.event_mappings) {
        connectEventMapping(command, widget, event_config);
    }
}

void WidgetMapper::disconnectEvents(BaseUICommand* command, QWidget* widget) {
    // Events are disconnected when binding is removed
    Q_UNUSED(command)
    Q_UNUSED(widget)
}

bool WidgetMapper::hasMapping(const QString& command_type) const {
    return mappings_.find(command_type) != mappings_.end();
}

QString WidgetMapper::getWidgetType(const QString& command_type) const {
    auto it = mappings_.find(command_type);
    return it != mappings_.end() ? it->second.widget_type : QString();
}

QStringList WidgetMapper::getSupportedCommandTypes() const {
    QStringList types;
    for (const auto& [type, config] : mappings_) {
        types.append(type);
    }
    return types;
}

void WidgetMapper::onCommandPropertyChanged(const QString& property, const QVariant& value) {
    auto* command = qobject_cast<BaseUICommand*>(sender());
    if (command) {
        syncCommandToWidget(command, property);
    }
}

void WidgetMapper::onWidgetPropertyChanged() {
    // This would be connected to specific widget property change signals
    // Implementation depends on specific widget types
}

void WidgetMapper::setupDefaultMappings() {
    // Set up default mappings for common Qt widgets
    qDebug() << "🔧 Setting up default widget mappings";
    
    // These will be implemented when we create specific command types
    // For now, just log that setup is complete
    qDebug() << "✅ Default mappings setup complete";
}

void WidgetMapper::connectPropertySync(BaseUICommand* command, QWidget* widget, const PropertySyncConfig& config) {
    // Property sync connections are handled in syncCommandToWidget/syncWidgetToCommand
    Q_UNUSED(command)
    Q_UNUSED(widget)
    Q_UNUSED(config)
}

void WidgetMapper::connectEventMapping(BaseUICommand* command, QWidget* widget, const EventMappingConfig& config) {
    // Connect widget signal to command event
    const QMetaObject* meta = widget->metaObject();
    int signal_index = meta->indexOfSignal(config.widget_signal.toUtf8().constData());
    
    if (signal_index >= 0) {
        // Create a lambda to handle the signal and forward to command
        QMetaObject::Connection connection;
        if (auto* button = qobject_cast<QPushButton*>(widget)) {
            connection = connect(button, &QPushButton::clicked, [command, config]() {
                QVariant eventData;
                if (config.signal_to_event_converter) {
                    eventData = config.signal_to_event_converter(QVariantList{});
                }
                command->handleEvent(config.command_event, eventData);
            });
        }
        
        // Store connection for later cleanup
        auto it = active_bindings_.find(command);
        if (it != active_bindings_.end()) {
            it->second.connections.push_back(connection);
        }
    }
}

QVariant WidgetMapper::convertProperty(const QVariant& value, const std::function<QVariant(const QVariant&)>& converter) {
    if (converter) {
        return converter(value);
    }
    return value;
}

void WidgetMapper::handleSyncError(BaseUICommand* command, const QString& error) {
    qWarning() << "Sync error for command" << command->getCommandType() << ":" << error;
    emit syncError(command, error);
}

bool WidgetMapper::setWidgetProperty(QWidget* widget, const QString& property, const QVariant& value) {
    return widget->setProperty(property.toUtf8().constData(), value);
}

QVariant WidgetMapper::getWidgetProperty(QWidget* widget, const QString& property) {
    return widget->property(property.toUtf8().constData());
}

bool WidgetMapper::connectToSignal(QWidget* widget, const QString& signal, QObject* receiver, const char* slot) {
    const QMetaObject* meta = widget->metaObject();
    int signal_index = meta->indexOfSignal(signal.toUtf8().constData());
    return signal_index >= 0;
}

}  // namespace DeclarativeUI::Command::UI
