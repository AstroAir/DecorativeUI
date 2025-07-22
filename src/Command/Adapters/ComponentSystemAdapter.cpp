#include "ComponentSystemAdapter.hpp"
#include <QDebug>
#include <QMetaObject>
#include <QMetaProperty>

namespace DeclarativeUI::Command::Adapters {

ComponentSystemAdapter::ComponentSystemAdapter(QObject* parent) : QObject(parent) {
    setupBuiltinConverters();
    qDebug() << "🔄 ComponentSystemAdapter initialized";
}

ComponentSystemAdapter& ComponentSystemAdapter::instance() {
    static ComponentSystemAdapter instance;
    return instance;
}

std::shared_ptr<UI::BaseUICommand> ComponentSystemAdapter::convertToCommand(Components::Button* button) {
    return ButtonAdapter::toCommand(button);
}

std::shared_ptr<UI::BaseUICommand> ComponentSystemAdapter::convertToCommand(Components::Widget* widget) {
    return WidgetAdapter::toCommand(widget);
}

std::shared_ptr<UI::BaseUICommand> ComponentSystemAdapter::convertToCommand(Components::Layout* layout) {
    return LayoutAdapter::toCommand(layout);
}

std::unique_ptr<Components::Button> ComponentSystemAdapter::convertToButton(std::shared_ptr<UI::BaseUICommand> command) {
    return ButtonAdapter::toComponent(command);
}

std::unique_ptr<Components::Widget> ComponentSystemAdapter::convertToWidget(std::shared_ptr<UI::BaseUICommand> command) {
    return WidgetAdapter::toComponent(command);
}

std::unique_ptr<Components::Layout> ComponentSystemAdapter::convertToLayout(std::shared_ptr<UI::BaseUICommand> command) {
    return LayoutAdapter::toComponent(command);
}

std::unique_ptr<ComponentSystemAdapter::HybridContainer> ComponentSystemAdapter::createHybridContainer() {
    return std::make_unique<HybridContainer>();
}

void ComponentSystemAdapter::establishSync(Components::Button* button, std::shared_ptr<UI::BaseUICommand> command) {
    ButtonAdapter::establishSync(button, command);
    
    SyncInfo syncInfo;
    syncInfo.component = button;
    syncInfo.command = command;
    setupComponentToCommandSync(button, command, syncInfo);
    setupCommandToComponentSync(command, button, syncInfo);
    
    active_syncs_[button] = syncInfo;
    emit syncEstablished(button, command);
}

void ComponentSystemAdapter::establishSync(Components::Widget* widget, std::shared_ptr<UI::BaseUICommand> command) {
    WidgetAdapter::establishSync(widget, command);
    
    SyncInfo syncInfo;
    syncInfo.component = widget;
    syncInfo.command = command;
    setupComponentToCommandSync(widget, command, syncInfo);
    setupCommandToComponentSync(command, widget, syncInfo);
    
    active_syncs_[widget] = syncInfo;
    emit syncEstablished(widget, command);
}

void ComponentSystemAdapter::removeSync(Components::Button* button) {
    auto it = active_syncs_.find(button);
    if (it != active_syncs_.end()) {
        cleanupSync(it->second);
        active_syncs_.erase(it);
        emit syncRemoved(button);
    }
}

void ComponentSystemAdapter::removeSync(Components::Widget* widget) {
    auto it = active_syncs_.find(widget);
    if (it != active_syncs_.end()) {
        cleanupSync(it->second);
        active_syncs_.erase(it);
        emit syncRemoved(widget);
    }
}

std::vector<std::shared_ptr<UI::BaseUICommand>> ComponentSystemAdapter::convertComponentHierarchy(QWidget* rootWidget) {
    std::vector<std::shared_ptr<UI::BaseUICommand>> commands;
    
    if (!rootWidget) {
        return commands;
    }
    
    // Convert root widget
    auto rootCommand = createCommandFromComponent(rootWidget);
    if (rootCommand) {
        commands.push_back(rootCommand);
        
        // Recursively convert children
        for (QObject* child : rootWidget->children()) {
            if (auto* childWidget = qobject_cast<QWidget*>(child)) {
                auto childCommands = convertComponentHierarchy(childWidget);
                for (auto childCommand : childCommands) {
                    rootCommand->addChild(childCommand);
                    commands.push_back(childCommand);
                }
            }
        }
    }
    
    return commands;
}

std::unique_ptr<QWidget> ComponentSystemAdapter::convertCommandHierarchy(std::shared_ptr<UI::BaseUICommand> rootCommand) {
    if (!rootCommand) {
        return nullptr;
    }
    
    // Create widget from command
    auto widget = UI::WidgetMapper::instance().createWidget(rootCommand.get());
    if (!widget) {
        return nullptr;
    }
    
    // Convert children
    for (auto child : rootCommand->getChildren()) {
        auto childWidget = convertCommandHierarchy(child);
        if (childWidget) {
            childWidget->setParent(widget.get());
        }
    }
    
    return widget;
}

void ComponentSystemAdapter::migrateComponentToCommand(QWidget* component, std::shared_ptr<UI::BaseUICommand> command) {
    if (!component || !command) {
        return;
    }
    
    // Copy properties from component to command
    const QMetaObject* metaObj = component->metaObject();
    for (int i = 0; i < metaObj->propertyCount(); ++i) {
        QMetaProperty prop = metaObj->property(i);
        if (prop.isReadable()) {
            QString propName = prop.name();
            QVariant value = prop.read(component);
            command->getState()->setProperty(propName, value);
        }
    }
}

void ComponentSystemAdapter::migrateCommandToComponent(std::shared_ptr<UI::BaseUICommand> command, QWidget* component) {
    if (!command || !component) {
        return;
    }
    
    // Copy properties from command to component
    auto propertyNames = command->getState()->getPropertyNames();
    const QMetaObject* metaObj = component->metaObject();
    
    for (const QString& propName : propertyNames) {
        int propIndex = metaObj->indexOfProperty(propName.toUtf8().constData());
        if (propIndex >= 0) {
            QMetaProperty prop = metaObj->property(propIndex);
            if (prop.isWritable()) {
                QVariant value = command->getState()->getProperty<QVariant>(propName);
                prop.write(component, value);
            }
        }
    }
}

bool ComponentSystemAdapter::hasConverter(const QString& componentType) const {
    return converters_.find(componentType) != converters_.end();
}

bool ComponentSystemAdapter::hasReverseConverter(const QString& commandType) const {
    return command_to_component_map_.find(commandType) != command_to_component_map_.end();
}

QStringList ComponentSystemAdapter::getSupportedComponentTypes() const {
    QStringList types;
    for (const auto& [type, converter] : converters_) {
        types.append(type);
    }
    return types;
}

QStringList ComponentSystemAdapter::getSupportedCommandTypes() const {
    QStringList types;
    for (const auto& [commandType, componentType] : command_to_component_map_) {
        types.append(commandType);
    }
    return types;
}

void ComponentSystemAdapter::setupBuiltinConverters() {
    // Register built-in converters
    registerConverter<Components::Button, UI::ButtonCommand>();
    registerConverter<Components::Widget, UI::ContainerCommand>();
    
    qDebug() << "🔧 Built-in converters registered";
}

void ComponentSystemAdapter::setupComponentToCommandSync(QObject* component, std::shared_ptr<UI::BaseUICommand> command, SyncInfo& syncInfo) {
    // Connect component property changes to command updates
    const QMetaObject* metaObj = component->metaObject();
    for (int i = 0; i < metaObj->methodCount(); ++i) {
        QMetaMethod method = metaObj->method(i);
        if (method.methodType() == QMetaMethod::Signal) {
            QString signalName = method.name();
            if (signalName.contains("Changed") || signalName.contains("Updated")) {
                // Use signal name for connection
                QString signalSignature = QString("2%1").arg(method.methodSignature());
                auto connection = QObject::connect(component, signalSignature.toLocal8Bit().data(),
                                                 this, SLOT(onComponentChanged()));
                // Store the component and command for the slot to use
                sync_context_[component] = command;
                syncInfo.connections.push_back(connection);
            }
        }
    }
}

void ComponentSystemAdapter::setupCommandToComponentSync(std::shared_ptr<UI::BaseUICommand> command, QObject* component, SyncInfo& syncInfo) {
    // Connect command state changes to component updates
    auto connection = connect(command.get(), &UI::BaseUICommand::stateChanged, this, 
                             [this, command, component]() {
                                 migrateCommandToComponent(command, qobject_cast<QWidget*>(component));
                             });
    syncInfo.connections.push_back(connection);
}

void ComponentSystemAdapter::cleanupSync(SyncInfo& syncInfo) {
    for (const auto& connection : syncInfo.connections) {
        disconnect(connection);
    }
    syncInfo.connections.clear();
}

std::shared_ptr<UI::BaseUICommand> ComponentSystemAdapter::createCommandFromComponent(QObject* component) {
    if (!component) {
        return nullptr;
    }
    
    QString componentType = component->metaObject()->className();
    auto it = converters_.find(componentType);
    if (it != converters_.end()) {
        auto command = it->second.to_command(component);
        emit componentConverted(component, command);
        return command;
    }
    
    handleConversionError(QString("No converter found for component type: %1").arg(componentType));
    return nullptr;
}

std::unique_ptr<QObject> ComponentSystemAdapter::createComponentFromCommand(std::shared_ptr<UI::BaseUICommand> command) {
    if (!command) {
        return nullptr;
    }
    
    QString commandType = command->getCommandType();
    auto it = command_to_component_map_.find(commandType);
    if (it != command_to_component_map_.end()) {
        QString componentType = it->second;
        auto converterIt = converters_.find(componentType);
        if (converterIt != converters_.end()) {
            auto component = converterIt->second.to_component(command);
            emit commandConverted(command, component.get());
            return component;
        }
    }
    
    handleConversionError(QString("No converter found for command type: %1").arg(commandType));
    return nullptr;
}

void ComponentSystemAdapter::handleConversionError(const QString& error) {
    qWarning() << "Conversion error:" << error;
    emit conversionError(error);
}

void ComponentSystemAdapter::onComponentChanged() {
    QObject* sender = QObject::sender();
    if (sender && sync_context_.contains(sender)) {
        auto command = sync_context_[sender];
        migrateComponentToCommand(qobject_cast<QWidget*>(sender), command);
    }
}

// **HybridContainer implementation**
ComponentSystemAdapter::HybridContainer::HybridContainer(QObject* parent) : QObject(parent) {
    qDebug() << "🏗️ HybridContainer created";
}

void ComponentSystemAdapter::HybridContainer::addComponent(std::unique_ptr<QObject> component) {
    if (component) {
        items_.emplace_back(component.release());
        setupItemConnections(items_.back());
        emit itemAdded(items_.back().component);
        
        if (auto_sync_enabled_) {
            syncItem(items_.back());
        }
    }
}

void ComponentSystemAdapter::HybridContainer::addCommand(std::shared_ptr<UI::BaseUICommand> command) {
    if (command) {
        items_.emplace_back(command);
        setupItemConnections(items_.back());
        emit itemAdded(command.get());
        
        if (auto_sync_enabled_) {
            syncItem(items_.back());
        }
    }
}

void ComponentSystemAdapter::HybridContainer::addWidget(std::unique_ptr<QWidget> widget) {
    if (widget) {
        items_.emplace_back(widget.release());
        setupItemConnections(items_.back());
        emit itemAdded(items_.back().widget);
        
        if (auto_sync_enabled_) {
            syncItem(items_.back());
        }
    }
}

void ComponentSystemAdapter::HybridContainer::removeComponent(QObject* component) {
    auto it = std::find_if(items_.begin(), items_.end(), 
                          [component](const HybridItem& item) {
                              return item.component == component;
                          });
    
    if (it != items_.end()) {
        cleanupItemConnections(*it);
        emit itemRemoved(component);
        items_.erase(it);
    }
}

void ComponentSystemAdapter::HybridContainer::removeCommand(std::shared_ptr<UI::BaseUICommand> command) {
    auto it = std::find_if(items_.begin(), items_.end(), 
                          [command](const HybridItem& item) {
                              return item.command == command;
                          });
    
    if (it != items_.end()) {
        cleanupItemConnections(*it);
        emit itemRemoved(command.get());
        items_.erase(it);
    }
}

void ComponentSystemAdapter::HybridContainer::clear() {
    for (auto& item : items_) {
        cleanupItemConnections(item);
        if (item.type == HybridItem::Component) {
            emit itemRemoved(item.component);
        } else if (item.type == HybridItem::Command) {
            emit itemRemoved(item.command.get());
        } else if (item.type == HybridItem::Widget) {
            emit itemRemoved(item.widget);
        }
    }
    items_.clear();
}

void ComponentSystemAdapter::HybridContainer::setLayout(const QString& layoutType) {
    if (layout_type_ != layoutType) {
        layout_type_ = layoutType;
        emit layoutChanged(layoutType);
    }
}

void ComponentSystemAdapter::HybridContainer::setSpacing(int spacing) {
    spacing_ = spacing;
    qDebug() << "🔧 HybridContainer spacing set to:" << spacing;
}

void ComponentSystemAdapter::HybridContainer::setMargins(int left, int top, int right, int bottom) {
    margin_left_ = left;
    margin_top_ = top;
    margin_right_ = right;
    margin_bottom_ = bottom;
    qDebug() << "🔧 HybridContainer margins set to:" << left << top << right << bottom;
}

std::vector<QObject*> ComponentSystemAdapter::HybridContainer::getComponents() const {
    std::vector<QObject*> components;
    for (const auto& item : items_) {
        if (item.type == HybridItem::Component) {
            components.push_back(item.component);
        }
    }
    return components;
}

std::vector<std::shared_ptr<UI::BaseUICommand>> ComponentSystemAdapter::HybridContainer::getCommands() const {
    std::vector<std::shared_ptr<UI::BaseUICommand>> commands;
    for (const auto& item : items_) {
        if (item.type == HybridItem::Command) {
            commands.push_back(item.command);
        }
    }
    return commands;
}

std::vector<QWidget*> ComponentSystemAdapter::HybridContainer::getWidgets() const {
    std::vector<QWidget*> widgets;
    for (const auto& item : items_) {
        if (item.type == HybridItem::Widget) {
            widgets.push_back(item.widget);
        }
    }
    return widgets;
}

int ComponentSystemAdapter::HybridContainer::getItemCount() const {
    return static_cast<int>(items_.size());
}

std::shared_ptr<UI::BaseUICommand> ComponentSystemAdapter::HybridContainer::toCommand() {
    auto container = UI::CommandBuilder("Container")
        .property("layout", layout_type_)
        .property("spacing", spacing_)
        .property("marginLeft", margin_left_)
        .property("marginTop", margin_top_)
        .property("marginRight", margin_right_)
        .property("marginBottom", margin_bottom_)
        .build();
    
    // Add all items as children
    for (const auto& item : items_) {
        std::shared_ptr<UI::BaseUICommand> childCommand;
        
        if (item.type == HybridItem::Command) {
            childCommand = item.command;
        } else if (item.type == HybridItem::Component) {
            childCommand = ComponentSystemAdapter::instance().createCommandFromComponent(item.component);
        } else if (item.type == HybridItem::Widget) {
            // Convert widget to command (simplified)
            childCommand = ComponentSystemAdapter::instance().createCommandFromComponent(item.widget);
        }
        
        if (childCommand) {
            container->addChild(childCommand);
        }
    }
    
    return container;
}

std::unique_ptr<QWidget> ComponentSystemAdapter::HybridContainer::toWidget() {
    auto command = toCommand();
    return ComponentSystemAdapter::instance().convertCommandHierarchy(command);
}

void ComponentSystemAdapter::HybridContainer::syncAll() {
    for (auto& item : items_) {
        syncItem(item);
    }
}

void ComponentSystemAdapter::HybridContainer::syncItem(HybridItem& item) {
    // Synchronization logic for individual items
    Q_UNUSED(item)
    // Implementation would depend on specific item types
}

void ComponentSystemAdapter::HybridContainer::setupItemConnections(HybridItem& item) {
    // Set up connections for item synchronization
    Q_UNUSED(item)
    // Implementation would depend on specific item types
}

void ComponentSystemAdapter::HybridContainer::cleanupItemConnections(HybridItem& item) {
    // Clean up connections for item
    Q_UNUSED(item)
    // Implementation would depend on specific item types
}

// **ButtonAdapter implementation**
std::shared_ptr<UI::BaseUICommand> ButtonAdapter::toCommand(Components::Button* button) {
    if (!button) {
        return nullptr;
    }
    
    auto command = UI::CommandBuilder("Button")
        .text(std::get<QString>(button->getProperty("text")))
        .enabled(std::get<bool>(button->getProperty("enabled")))
        .build();
    
    syncButtonToCommand(button, command);
    return command;
}

std::unique_ptr<Components::Button> ButtonAdapter::toComponent(std::shared_ptr<UI::BaseUICommand> command) {
    if (!command) {
        return nullptr;
    }
    
    auto button = std::make_unique<Components::Button>();
    syncCommandToButton(command, button.get());
    return button;
}

void ButtonAdapter::establishSync(Components::Button* button, std::shared_ptr<UI::BaseUICommand> command) {
    setupButtonPropertySync(button, command);
    setupButtonEventSync(button, command);
}

void ButtonAdapter::syncButtonToCommand(Components::Button* button, std::shared_ptr<UI::BaseUICommand> command) {
    if (!button || !command) {
        return;
    }
    
    command->getState()->setProperty("text", std::get<QString>(button->getProperty("text")));
    command->getState()->setProperty("enabled", std::get<bool>(button->getProperty("enabled")));
}

void ButtonAdapter::syncCommandToButton(std::shared_ptr<UI::BaseUICommand> command, Components::Button* button) {
    if (!command || !button) {
        return;
    }
    
    button->text(command->getState()->getProperty<QString>("text"));
    button->enabled(command->getState()->getProperty<bool>("enabled"));
}

void ButtonAdapter::setupButtonPropertySync(Components::Button* button, std::shared_ptr<UI::BaseUICommand> command) {
    // Set up property synchronization
    Q_UNUSED(button)
    Q_UNUSED(command)
    // Implementation would connect button property changes to command updates
}

void ButtonAdapter::setupButtonEventSync(Components::Button* button, std::shared_ptr<UI::BaseUICommand> command) {
    // Set up event synchronization
    Q_UNUSED(button)
    Q_UNUSED(command)
    // Implementation would connect button events to command events
}

// **WidgetAdapter implementation**
std::shared_ptr<UI::BaseUICommand> WidgetAdapter::toCommand(Components::Widget* widget) {
    if (!widget) {
        return nullptr;
    }
    
    std::shared_ptr<UI::BaseUICommand> command;

    try {
        QSize size = std::get<QSize>(widget->getProperty("size"));
        QRect geometry = std::get<QRect>(widget->getProperty("geometry"));
        bool visible = std::get<bool>(widget->getProperty("visible"));
        bool enabled = std::get<bool>(widget->getProperty("enabled"));

        command = UI::CommandBuilder("Container")
            .size(size.width(), size.height())
            .position(geometry.x(), geometry.y())
            .visible(visible)
            .enabled(enabled)
            .build();
    } catch (const std::bad_variant_access&) {
        // Fallback if properties don't exist or have wrong types
        command = UI::CommandBuilder("Container").build();
    }

    syncWidgetToCommand(widget, command);
    return command;
}

std::unique_ptr<Components::Widget> WidgetAdapter::toComponent(std::shared_ptr<UI::BaseUICommand> command) {
    if (!command) {
        return nullptr;
    }
    
    auto widget = std::make_unique<Components::Widget>();
    syncCommandToWidget(command, widget.get());
    return widget;
}

void WidgetAdapter::establishSync(Components::Widget* widget, std::shared_ptr<UI::BaseUICommand> command) {
    setupWidgetPropertySync(widget, command);
    setupWidgetEventSync(widget, command);
}

void WidgetAdapter::syncWidgetToCommand(Components::Widget* widget, std::shared_ptr<UI::BaseUICommand> command) {
    if (!widget || !command) {
        return;
    }
    
    command->getState()->setProperty("size", widget->getSize());
    command->getState()->setProperty("position", widget->getPosition());
    command->getState()->setProperty("visible", widget->isVisible());
    command->getState()->setProperty("enabled", widget->isEnabled());
}

void WidgetAdapter::syncCommandToWidget(std::shared_ptr<UI::BaseUICommand> command, Components::Widget* widget) {
    if (!command || !widget) {
        return;
    }
    
    widget->size(command->getState()->getProperty<QSize>("size"));
    widget->position(command->getState()->getProperty<QPoint>("position"));
    widget->visible(command->getState()->getProperty<bool>("visible"));
    widget->enabled(command->getState()->getProperty<bool>("enabled"));
}

void WidgetAdapter::setupWidgetPropertySync(Components::Widget* widget, std::shared_ptr<UI::BaseUICommand> command) {
    // Set up property synchronization
    Q_UNUSED(widget)
    Q_UNUSED(command)
    // Implementation would connect widget property changes to command updates
}

void WidgetAdapter::setupWidgetEventSync(Components::Widget* widget, std::shared_ptr<UI::BaseUICommand> command) {
    // Set up event synchronization
    Q_UNUSED(widget)
    Q_UNUSED(command)
    // Implementation would connect widget events to command events
}

// **LayoutAdapter implementation**
std::shared_ptr<UI::BaseUICommand> LayoutAdapter::toCommand(Components::Layout* layout) {
    if (!layout) {
        return nullptr;
    }
    
    return convertLayoutHierarchy(layout);
}

std::unique_ptr<Components::Layout> LayoutAdapter::toComponent(std::shared_ptr<UI::BaseUICommand> command) {
    if (!command) {
        return nullptr;
    }
    
    return convertCommandHierarchy(command);
}

std::shared_ptr<UI::BaseUICommand> LayoutAdapter::convertLayoutHierarchy(Components::Layout* layout) {
    // Convert layout type to string
    QString layoutType = "VBox"; // Default
    try {
        auto typeProperty = layout->getProperty("layoutType");
        if (std::holds_alternative<QString>(typeProperty)) {
            layoutType = std::get<QString>(typeProperty);
        }
    } catch (...) {
        // Use default if property doesn't exist
    }

    auto command = UI::CommandBuilder("Container")
        .property("layout", layoutType)
        .build();
    
    convertLayoutProperties(layout, command);
    convertLayoutChildren(layout, command);
    
    return command;
}

std::unique_ptr<Components::Layout> LayoutAdapter::convertCommandHierarchy(std::shared_ptr<UI::BaseUICommand> command) {
    auto layout = std::make_unique<Components::Layout>();
    
    convertCommandProperties(command, layout.get());
    convertCommandChildren(command, layout.get());
    
    return layout;
}

void LayoutAdapter::convertLayoutProperties(Components::Layout* layout, std::shared_ptr<UI::BaseUICommand> command) {
    Q_UNUSED(layout)
    Q_UNUSED(command)
    // Implementation would copy layout properties to command
}

void LayoutAdapter::convertCommandProperties(std::shared_ptr<UI::BaseUICommand> command, Components::Layout* layout) {
    Q_UNUSED(command)
    Q_UNUSED(layout)
    // Implementation would copy command properties to layout
}

void LayoutAdapter::convertLayoutChildren(Components::Layout* layout, std::shared_ptr<UI::BaseUICommand> command) {
    Q_UNUSED(layout)
    Q_UNUSED(command)
    // Implementation would convert layout children to command children
}

void LayoutAdapter::convertCommandChildren(std::shared_ptr<UI::BaseUICommand> command, Components::Layout* layout) {
    Q_UNUSED(command)
    Q_UNUSED(layout)
    // Implementation would convert command children to layout children
}

}  // namespace DeclarativeUI::Command::Adapters
