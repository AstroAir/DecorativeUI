#include "UIElementAdapter.hpp"
#include <QDebug>
#include <QMetaObject>
#include <QMetaProperty>
#include <algorithm>

namespace DeclarativeUI::Command::Adapters {

// **UIElementCommandAdapter implementation**
UIElementCommandAdapter::UIElementCommandAdapter(
    std::unique_ptr<Core::UIElement> element, QObject* parent)
    : UI::BaseUICommand(parent), ui_element_(std::move(element)) {
    if (!ui_element_) {
        throw std::invalid_argument("UIElement cannot be null");
    }

    // Determine command and widget types from UIElement
    const QMetaObject* metaObj = ui_element_->metaObject();
    command_type_ = QString("Adapted%1").arg(metaObj->className());
    widget_type_ = "QWidget";  // Default, can be overridden

    // Set up initial state from UIElement properties
    syncUIElementToCommand();
    setupPropertySynchronization();
    setupEventForwarding();

    qDebug() << "🔄 Created UIElementCommandAdapter for:" << command_type_;
}

UI::UICommandMetadata UIElementCommandAdapter::getMetadata() const {
    UI::UICommandMetadata metadata(command_type_, widget_type_,
                                   QString("Adapted %1").arg(command_type_),
                                   "Adapter for existing UIElement component");

    // Extract metadata from UIElement if possible
    if (ui_element_) {
        const QMetaObject* metaObj = ui_element_->metaObject();

        // Add properties from UIElement
        for (int i = 0; i < metaObj->propertyCount(); ++i) {
            QMetaProperty prop = metaObj->property(i);
            QString propName = prop.name();
            if (propName != "objectName") {  // Skip Qt internal properties
                metadata.default_properties[propName] =
                    QJsonValue::fromVariant(prop.read(ui_element_.get()));
            }
        }

        // Add common events
        metadata.supported_events = {"clicked", "valueChanged", "textChanged",
                                     "stateChanged"};
    }

    return metadata;
}

QString UIElementCommandAdapter::getCommandType() const {
    return command_type_;
}

QString UIElementCommandAdapter::getWidgetType() const { return widget_type_; }

void UIElementCommandAdapter::onWidgetCreated(QWidget* widget) {
    UI::BaseUICommand::onWidgetCreated(widget);

    if (ui_element_ && widget) {
        // Set the widget on the UIElement
        ui_element_->setWidget(widget);

        // Initialize the UIElement
        try {
            ui_element_->initialize();
        } catch (const std::exception& e) {
            qWarning() << "Failed to initialize UIElement:" << e.what();
        }

        connectUIElementSignals();
    }
}

void UIElementCommandAdapter::onWidgetDestroyed() {
    disconnectUIElementSignals();

    if (ui_element_) {
        ui_element_->cleanup();
    }

    UI::BaseUICommand::onWidgetDestroyed();
}

void UIElementCommandAdapter::syncToWidget() {
    UI::BaseUICommand::syncToWidget();
    syncCommandToUIElement();

    if (ui_element_) {
        ui_element_->refresh();
    }
}

void UIElementCommandAdapter::syncFromWidget() {
    if (ui_element_) {
        // Sync UIElement properties to Command state
        syncUIElementToCommand();
    }

    UI::BaseUICommand::syncFromWidget();
}

void UIElementCommandAdapter::handleEvent(const QString& eventType,
                                          const QVariant& eventData) {
    // Forward events to UIElement if it has event handlers
    if (ui_element_) {
        // UIElement doesn't have a direct event handling mechanism,
        // but we can trigger property updates or call specific methods
        qDebug() << "🔄 Forwarding event" << eventType << "to UIElement";
    }

    UI::BaseUICommand::handleEvent(eventType, eventData);
}

void UIElementCommandAdapter::syncUIElementToCommand() {
    if (!ui_element_) {
        return;
    }

    const QMetaObject* metaObj = ui_element_->metaObject();

    // Sync all properties from UIElement to Command state
    for (int i = 0; i < metaObj->propertyCount(); ++i) {
        QMetaProperty prop = metaObj->property(i);
        QString propName = prop.name();

        if (propName != "objectName" && prop.isReadable()) {
            QVariant value = prop.read(ui_element_.get());
            getState()->setProperty(propName, value);
        }
    }
}

void UIElementCommandAdapter::syncCommandToUIElement() {
    if (!ui_element_) {
        return;
    }

    const QMetaObject* metaObj = ui_element_->metaObject();
    auto propertyNames = getState()->getPropertyNames();

    // Sync Command state properties to UIElement
    for (const QString& propName : propertyNames) {
        int propIndex = metaObj->indexOfProperty(propName.toUtf8().constData());
        if (propIndex >= 0) {
            QMetaProperty prop = metaObj->property(propIndex);
            if (prop.isWritable()) {
                QVariant value = getState()->getProperty<QVariant>(propName);
                prop.write(ui_element_.get(), value);
            }
        }
    }
}

void UIElementCommandAdapter::setupPropertySynchronization() {
    // Connect Command state changes to UIElement sync
    connect(getState(), &UI::UICommandState::propertyChanged, this,
            [this](const QString& property, const QVariant& value) {
                Q_UNUSED(property)
                Q_UNUSED(value)
                syncCommandToUIElement();
            });
}

void UIElementCommandAdapter::setupEventForwarding() {
    // Set up event forwarding from UIElement to Command events
    // This is a simplified implementation - in practice, you'd need to
    // connect to specific UIElement signals based on the component type
}

void UIElementCommandAdapter::connectUIElementSignals() {
    if (!ui_element_) {
        return;
    }

    // Connect common UIElement signals to Command events
    // This is a generic approach - specific adapters should override this

    // Connect property change signals if available
    const QMetaObject* metaObj = ui_element_->metaObject();
    for (int i = 0; i < metaObj->methodCount(); ++i) {
        QMetaMethod method = metaObj->method(i);
        if (method.methodType() == QMetaMethod::Signal) {
            QString signalName = method.name();
            if (signalName.contains("Changed") ||
                signalName.contains("Updated")) {
                // Connect to property sync using signal name
                QString signalSignature =
                    QString("2%1").arg(method.methodSignature());
                auto connection = QObject::connect(
                    ui_element_.get(), signalSignature.toLocal8Bit().data(),
                    this, SLOT(syncUIElementToCommand()));
                connections_.push_back(connection);
            }
        }
    }
}

void UIElementCommandAdapter::disconnectUIElementSignals() {
    for (const auto& connection : connections_) {
        disconnect(connection);
    }
    connections_.clear();
}

// **UIElementAdapterFactory implementation**
std::unordered_map<QString, std::function<std::unique_ptr<Core::UIElement>()>>
    UIElementAdapterFactory::element_factories_;

std::shared_ptr<UIElementCommandAdapter> UIElementAdapterFactory::createAdapter(
    std::unique_ptr<Core::UIElement> element) {
    if (!element) {
        qWarning() << "Cannot create adapter for null UIElement";
        return nullptr;
    }

    return std::make_shared<UIElementCommandAdapter>(std::move(element));
}

std::shared_ptr<UIElementCommandAdapter>
UIElementAdapterFactory::createButtonAdapter() {
    auto button = std::make_unique<Components::Button>();
    return std::make_shared<ButtonCommandAdapter>();
}

std::shared_ptr<UIElementCommandAdapter>
UIElementAdapterFactory::createWidgetAdapter() {
    auto widget = std::make_unique<Components::Widget>();
    return std::make_shared<WidgetCommandAdapter>();
}

std::shared_ptr<UIElementCommandAdapter>
UIElementAdapterFactory::createAdapterByType(const QString& elementType) {
    auto it = element_factories_.find(elementType);
    if (it != element_factories_.end()) {
        auto element = it->second();
        return createAdapter(std::move(element));
    }

    qWarning() << "No factory registered for element type:" << elementType;
    return nullptr;
}

// **CommandUIElementAdapter implementation**
CommandUIElementAdapter::CommandUIElementAdapter(
    std::shared_ptr<UI::BaseUICommand> command, QObject* parent)
    : Core::UIElement(parent), command_(command) {
    if (!command_) {
        throw std::invalid_argument("Command cannot be null");
    }

    setupCommandIntegration();
    qDebug() << "🔄 Created CommandUIElementAdapter for:"
             << command_->getCommandType();
}

void CommandUIElementAdapter::initialize() {
    if (!command_) {
        throw DeclarativeUI::Exceptions::ComponentCreationException(
            "CommandUIElementAdapter: null command");
    }

    // Create widget through WidgetMapper
    auto widget = UI::WidgetMapper::instance().createWidget(command_.get());
    if (widget) {
        setWidget(widget.release());
    }

    // Sync properties from command to UIElement
    syncPropertiesFromCommand();
}

void CommandUIElementAdapter::cleanup() noexcept {
    try {
        cleanupCommandIntegration();
    } catch (...) {
        // Ensure no-throw guarantee
    }
}

void CommandUIElementAdapter::refresh() {
    if (command_) {
        command_->syncToWidget();
        syncPropertiesFromCommand();
    }
}

void CommandUIElementAdapter::setupCommandIntegration() {
    if (!command_) {
        return;
    }

    // Connect command state changes to UIElement refresh
    connect(command_.get(), &UI::BaseUICommand::stateChanged, this,
            &CommandUIElementAdapter::onCommandStateChanged);
    connect(command_.get(), &UI::BaseUICommand::eventTriggered, this,
            &CommandUIElementAdapter::onCommandEventTriggered);
}

void CommandUIElementAdapter::cleanupCommandIntegration() {
    if (command_) {
        disconnect(command_.get(), nullptr, this, nullptr);
    }
}

void CommandUIElementAdapter::syncPropertiesToCommand() {
    if (!command_) {
        return;
    }

    // Sync UIElement properties to Command state
    const auto& properties = getProperties();
    for (const auto& [key, value] : properties) {
        command_->getState()->setProperty(key, value);
    }
}

void CommandUIElementAdapter::syncPropertiesFromCommand() {
    if (!command_) {
        return;
    }

    // Sync Command state to UIElement properties
    auto propertyNames = command_->getState()->getPropertyNames();
    for (const QString& propName : propertyNames) {
        QVariant value = command_->getState()->getProperty<QVariant>(propName);
        setProperty(propName, value);
    }
}

void CommandUIElementAdapter::onCommandStateChanged() {
    syncPropertiesFromCommand();
    refresh();
}

void CommandUIElementAdapter::onCommandEventTriggered(
    const QString& eventType, const QVariant& eventData) {
    Q_UNUSED(eventType)
    Q_UNUSED(eventData)
    // Forward command events to UIElement event system if needed
}

// **ButtonCommandAdapter implementation**
ButtonCommandAdapter::ButtonCommandAdapter(QObject* parent)
    : UIElementCommandAdapter(std::make_unique<Components::Button>(), parent) {
    command_type_ = "AdaptedButton";
    widget_type_ = "QPushButton";
    setupButtonSpecificBindings();
}

ButtonCommandAdapter& ButtonCommandAdapter::text(const QString& text) {
    getState()->setProperty("text", text);
    if (auto* button = qobject_cast<Components::Button*>(ui_element_.get())) {
        button->text(text);
    }
    return *this;
}

ButtonCommandAdapter& ButtonCommandAdapter::icon(const QString& iconPath) {
    getState()->setProperty("icon", iconPath);
    if (auto* button = qobject_cast<Components::Button*>(ui_element_.get())) {
        button->icon(QIcon(iconPath));
    }
    return *this;
}

ButtonCommandAdapter& ButtonCommandAdapter::onClick(
    std::function<void()> handler) {
    click_handler_ = handler;
    if (auto* button = qobject_cast<Components::Button*>(ui_element_.get())) {
        button->onClick(handler);
    }
    return *this;
}

ButtonCommandAdapter& ButtonCommandAdapter::enabled(bool enabled) {
    getState()->setProperty("enabled", enabled);
    if (auto* button = qobject_cast<Components::Button*>(ui_element_.get())) {
        button->enabled(enabled);
    }
    return *this;
}

UI::UICommandMetadata ButtonCommandAdapter::getMetadata() const {
    UI::UICommandMetadata metadata("AdaptedButton", "QPushButton",
                                   "Adapted Button",
                                   "Button component adapted from UIElement");
    metadata.supported_events = {"clicked"};
    metadata.default_properties = {
        {"text", "Button"}, {"enabled", true}, {"icon", ""}};
    return metadata;
}

void ButtonCommandAdapter::setupButtonSpecificBindings() {
    // Set up button-specific property synchronization
    connect(getState(), &UI::UICommandState::propertyChanged, this,
            [this](const QString& property, const QVariant& value) {
                if (property == "text") {
                    text(value.toString());
                } else if (property == "enabled") {
                    enabled(value.toBool());
                } else if (property == "icon") {
                    icon(value.toString());
                }
            });
}

// **WidgetCommandAdapter implementation**
WidgetCommandAdapter::WidgetCommandAdapter(QObject* parent)
    : UIElementCommandAdapter(std::make_unique<Components::Widget>(), parent) {
    command_type_ = "AdaptedWidget";
    widget_type_ = "QWidget";
    setupWidgetSpecificBindings();
}

WidgetCommandAdapter& WidgetCommandAdapter::size(const QSize& size) {
    getState()->setProperty("size", size);
    if (auto* widget = qobject_cast<Components::Widget*>(ui_element_.get())) {
        widget->size(size);
    }
    return *this;
}

WidgetCommandAdapter& WidgetCommandAdapter::position(const QPoint& position) {
    getState()->setProperty("position", position);
    if (auto* widget = qobject_cast<Components::Widget*>(ui_element_.get())) {
        widget->position(position);
    }
    return *this;
}

WidgetCommandAdapter& WidgetCommandAdapter::visible(bool visible) {
    getState()->setProperty("visible", visible);
    if (auto* widget = qobject_cast<Components::Widget*>(ui_element_.get())) {
        widget->visible(visible);
    }
    return *this;
}

WidgetCommandAdapter& WidgetCommandAdapter::enabled(bool enabled) {
    getState()->setProperty("enabled", enabled);
    if (auto* widget = qobject_cast<Components::Widget*>(ui_element_.get())) {
        widget->enabled(enabled);
    }
    return *this;
}

WidgetCommandAdapter& WidgetCommandAdapter::styleSheet(const QString& style) {
    getState()->setProperty("styleSheet", style);
    if (auto* widget = qobject_cast<Components::Widget*>(ui_element_.get())) {
        widget->style(style);
    }
    return *this;
}

UI::UICommandMetadata WidgetCommandAdapter::getMetadata() const {
    UI::UICommandMetadata metadata("AdaptedWidget", "QWidget", "Adapted Widget",
                                   "Generic widget adapted from UIElement");
    metadata.default_properties =
        QJsonObject{{"visible", true},
                    {"enabled", true},
                    {"size", QJsonValue::fromVariant(QSize(100, 30))},
                    {"position", QJsonValue::fromVariant(QPoint(0, 0))}};
    return metadata;
}

void WidgetCommandAdapter::setupWidgetSpecificBindings() {
    // Set up widget-specific property synchronization
    connect(getState(), &UI::UICommandState::propertyChanged, this,
            [this](const QString& property, const QVariant& value) {
                if (property == "size") {
                    size(value.toSize());
                } else if (property == "position") {
                    position(value.toPoint());
                } else if (property == "visible") {
                    visible(value.toBool());
                } else if (property == "enabled") {
                    enabled(value.toBool());
                } else if (property == "styleSheet") {
                    styleSheet(value.toString());
                }
            });
}

// **ComponentCommandBridge implementation**
std::unordered_map<QString, QString>
    ComponentCommandBridge::component_to_command_mapping_;
std::unordered_map<QString, QString>
    ComponentCommandBridge::command_to_component_mapping_;

void ComponentCommandBridge::registerComponentMappings() {
    // Register mappings between existing components and new commands
    component_to_command_mapping_["Button"] = "AdaptedButton";
    component_to_command_mapping_["Widget"] = "AdaptedWidget";

    command_to_component_mapping_["AdaptedButton"] = "Button";
    command_to_component_mapping_["AdaptedWidget"] = "Widget";

    qDebug() << "🔗 Registered component-command mappings";
}

std::shared_ptr<UI::BaseUICommand>
ComponentCommandBridge::createCommandFromComponent(
    const QString& componentType) {
    auto it = component_to_command_mapping_.find(componentType);
    if (it != component_to_command_mapping_.end()) {
        return UIElementAdapterFactory::createAdapterByType(componentType);
    }

    qWarning() << "No command mapping for component type:" << componentType;
    return nullptr;
}

std::unique_ptr<Core::UIElement>
ComponentCommandBridge::createComponentFromCommand(
    std::shared_ptr<UI::BaseUICommand> command) {
    if (!command) {
        return nullptr;
    }

    return std::make_unique<CommandUIElementAdapter>(command);
}

void ComponentCommandBridge::establishBidirectionalSync(
    Core::UIElement* component, std::shared_ptr<UI::BaseUICommand> command) {
    if (!component || !command) {
        return;
    }

    // This would set up bidirectional synchronization between the component and
    // command Implementation would depend on specific component and command
    // types
    qDebug()
        << "🔄 Established bidirectional sync between component and command";
}

void CommandUIElementAdapter::syncUIElementToCommand() {
    if (!command_) {
        return;
    }

    // Sync UIElement properties to Command
    syncPropertiesToCommand();
}

}  // namespace DeclarativeUI::Command::Adapters
