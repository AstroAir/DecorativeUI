#include "CommandBuilder.hpp"
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QObject>
#include <algorithm>

namespace DeclarativeUI::Command::UI {

// **CommandBuilder implementation**
CommandBuilder::CommandBuilder(const QString& commandType)
    : command_type_(commandType) {
    qDebug() << "🔨 CommandBuilder created for type:" << commandType;
}

CommandBuilder& CommandBuilder::property(const QString& name,
                                         const QVariant& value) {
    properties_[name] = QJsonValue::fromVariant(value);
    return *this;
}

CommandBuilder& CommandBuilder::properties(const QJsonObject& props) {
    for (auto it = props.begin(); it != props.end(); ++it) {
        properties_[it.key()] = it.value();
    }
    return *this;
}

CommandBuilder& CommandBuilder::text(const QString& text) {
    return property("text", text);
}

CommandBuilder& CommandBuilder::enabled(bool enabled) {
    return property("enabled", enabled);
}

CommandBuilder& CommandBuilder::visible(bool visible) {
    return property("visible", visible);
}

CommandBuilder& CommandBuilder::tooltip(const QString& tooltip) {
    return property("toolTip", tooltip);
}

CommandBuilder& CommandBuilder::icon(const QString& iconPath) {
    return property("icon", iconPath);
}

CommandBuilder& CommandBuilder::size(int width, int height) {
    return property("width", width).property("height", height);
}

CommandBuilder& CommandBuilder::position(int x, int y) {
    return property("x", x).property("y", y);
}

CommandBuilder& CommandBuilder::layout(const QString& layoutType) {
    return property("layout", layoutType);
}

CommandBuilder& CommandBuilder::spacing(int spacing) {
    return property("spacing", spacing);
}

// Add missing method implementations
CommandBuilder& CommandBuilder::placeholder(const QString& placeholder) {
    return property("placeholder", placeholder);
}

CommandBuilder& CommandBuilder::maxLength(int length) {
    return property("maxLength", length);
}

CommandBuilder& CommandBuilder::onTextChanged(
    std::function<void(const QString&)> handler) {
    return onEvent("textChanged", [handler](const QVariant& value) {
        if (handler)
            handler(value.toString());
    });
}

CommandBuilder& CommandBuilder::onClick(std::function<void()> handler) {
    return onEvent("clicked", [handler](const QVariant&) {
        if (handler)
            handler();
    });
}

CommandBuilder& CommandBuilder::onValueChanged(
    std::function<void(const QVariant&)> handler) {
    return onEvent("valueChanged", handler);
}

CommandBuilder& CommandBuilder::onEvent(
    const QString& eventType, std::function<void(const QVariant&)> handler) {
    event_handlers_[eventType] = handler;
    return *this;
}

CommandBuilder& CommandBuilder::bindToState(const QString& stateKey,
                                            const QString& property) {
    state_bindings_[property] = stateKey;
    return *this;
}

CommandBuilder& CommandBuilder::bindProperty(const QString& property,
                                             const QString& stateKey) {
    return bindToState(stateKey, property);
}

CommandBuilder& CommandBuilder::registerAsAction(const QString& actionName) {
    action_registrations_.push_back(actionName);
    return *this;
}

CommandBuilder& CommandBuilder::connectToAction(const QString& actionName,
                                                const QString& eventType) {
    // Store action connection info for later setup
    properties_["_actionConnection"] =
        QString("%1:%2").arg(actionName, eventType);
    return *this;
}

CommandBuilder& CommandBuilder::validator(
    const QString& property, std::function<bool(const QVariant&)> validator) {
    validators_[property] = validator;
    return *this;
}

CommandBuilder& CommandBuilder::validator(
    std::function<bool(const QVariant&)> validator) {
    // Apply validator to the default property based on command type
    QString defaultProperty = "value";  // Default fallback
    if (command_type_ == "TextInput" || command_type_ == "LineEdit") {
        defaultProperty = "text";
    } else if (command_type_ == "Button") {
        defaultProperty = "text";
    } else if (command_type_ == "CheckBox") {
        defaultProperty = "checked";
    } else if (command_type_ == "SpinBox" || command_type_ == "DoubleSpinBox") {
        defaultProperty = "value";
    }

    validators_[defaultProperty] = validator;
    return *this;
}

CommandBuilder& CommandBuilder::required(const QString& property) {
    return validator(property, [](const QVariant& value) {
        return !value.toString().isEmpty();
    });
}

CommandBuilder& CommandBuilder::range(const QString& property,
                                      const QVariant& min,
                                      const QVariant& max) {
    return validator(property, [min, max](const QVariant& value) {
        if (value.typeId() == QMetaType::Int) {
            int val = value.toInt();
            return val >= min.toInt() && val <= max.toInt();
        } else if (value.typeId() == QMetaType::Double) {
            double val = value.toDouble();
            return val >= min.toDouble() && val <= max.toDouble();
        }
        return true;
    });
}

CommandBuilder& CommandBuilder::styleClass(const QString& className) {
    return property("styleClass", className);
}

CommandBuilder& CommandBuilder::style(const QString& property,
                                      const QVariant& value) {
    QJsonObject styles = properties_.value("styles").toObject();
    styles[property] = QJsonValue::fromVariant(value);
    return this->property("styles", styles);
}

CommandBuilder& CommandBuilder::style(const QString& styleSheet) {
    return property("styleSheet", styleSheet);
}

CommandBuilder& CommandBuilder::theme(const QString& themeName) {
    return property("theme", themeName);
}

CommandBuilder& CommandBuilder::child(const CommandBuilder& childBuilder) {
    auto childCommand = const_cast<CommandBuilder&>(childBuilder).build();
    if (childCommand) {
        children_.push_back(childCommand);
    }
    return *this;
}

CommandBuilder& CommandBuilder::child(std::shared_ptr<BaseUICommand> child) {
    if (child) {
        children_.push_back(child);
    }
    return *this;
}

CommandBuilder& CommandBuilder::children(
    const std::vector<CommandBuilder>& childBuilders) {
    for (const auto& childBuilder : childBuilders) {
        child(childBuilder);
    }
    return *this;
}

CommandBuilder& CommandBuilder::config(const BuilderConfig& config) {
    config_ = config;
    return *this;
}

CommandBuilder& CommandBuilder::autoInitialize(bool enable) {
    config_.auto_initialize = enable;
    return *this;
}

CommandBuilder& CommandBuilder::autoMVCIntegration(bool enable) {
    config_.auto_mvc_integration = enable;
    return *this;
}

std::shared_ptr<BaseUICommand> CommandBuilder::build() {
    auto command =
        UICommandFactory::instance().createCommand(command_type_, properties_);
    if (!command) {
        qWarning() << "Failed to create command of type:" << command_type_;
        return nullptr;
    }

    applyConfiguration(command);

    qDebug() << "🔨 Built command:" << command_type_;
    return command;
}

std::pair<std::shared_ptr<BaseUICommand>, std::unique_ptr<QWidget>>
CommandBuilder::buildWithWidget() {
    auto command = build();
    if (!command) {
        return {nullptr, nullptr};
    }

    std::unique_ptr<QWidget> widget;
    if (config_.auto_widget_creation) {
        widget = WidgetMapper::instance().createWidget(command.get());
    }

    return {command, std::move(widget)};
}

std::unique_ptr<CommandUIElement> CommandBuilder::buildAsUIElement() {
    auto command = build();
    if (!command) {
        return nullptr;
    }

    auto element = std::make_unique<CommandUIElement>(command);

    if (config_.auto_initialize) {
        element->initialize();
    }

    return element;
}

bool CommandBuilder::validate() const {
    return getValidationErrors().isEmpty();
}

QStringList CommandBuilder::getValidationErrors() const {
    QStringList errors;

    if (command_type_.isEmpty()) {
        errors.append("Command type is empty");
    }

    if (!UICommandFactory::instance().isRegistered(command_type_)) {
        errors.append(
            QString("Command type '%1' is not registered").arg(command_type_));
    }

    // Additional validation logic can be added here

    return errors;
}

QString CommandBuilder::toString() const {
    QJsonObject json;
    json["type"] = command_type_;
    json["properties"] = properties_;

    if (!children_.empty()) {
        QJsonArray childrenArray;
        for (const auto& child : children_) {
            QJsonObject childJson;
            childJson["type"] = child->getCommandType();
            // Add child properties if needed
            childrenArray.append(childJson);
        }
        json["children"] = childrenArray;
    }

    return QJsonDocument(json).toJson(QJsonDocument::Compact);
}

void CommandBuilder::applyConfiguration(
    std::shared_ptr<BaseUICommand> command) {
    if (!command) {
        return;
    }

    setupEventHandlers(command);
    setupStateBindings(command);
    setupValidators(command);
    setupActionRegistrations(command);
    addChildren(command);
}

void CommandBuilder::setupEventHandlers(
    std::shared_ptr<BaseUICommand> command) {
    for (const auto& [eventType, handler] : event_handlers_) {
        QObject::connect(
            command.get(), &BaseUICommand::eventTriggered,
            [eventType, handler](const QString& type, const QVariant& data) {
                if (type == eventType && handler) {
                    handler(data);
                }
            });
    }
}

void CommandBuilder::setupStateBindings(
    std::shared_ptr<BaseUICommand> command) {
    if (!config_.auto_mvc_integration) {
        return;
    }

    for (const auto& [property, stateKey] : state_bindings_) {
        MVCIntegrationBridge::instance().bindCommandToStateManager(
            command, stateKey, property);
    }
}

void CommandBuilder::setupValidators(std::shared_ptr<BaseUICommand> command) {
    for (const auto& [property, validator] : validators_) {
        command->getState()->setValidator(property, validator);
    }
}

void CommandBuilder::setupActionRegistrations(
    std::shared_ptr<BaseUICommand> command) {
    if (!config_.auto_mvc_integration) {
        return;
    }

    for (const QString& actionName : action_registrations_) {
        MVCIntegrationBridge::instance().registerCommandAsAction(command,
                                                                 actionName);
    }
}

void CommandBuilder::addChildren(std::shared_ptr<BaseUICommand> command) {
    for (auto& child : children_) {
        command->addChild(child);
    }
}

// **CommandHierarchyBuilder implementation**
CommandHierarchyBuilder::CommandHierarchyBuilder(const QString& rootCommandType)
    : root_builder_(rootCommandType) {
    setupRootAsContainer();
    qDebug() << "🏗️ CommandHierarchyBuilder created for root type:"
             << rootCommandType;
}

CommandHierarchyBuilder& CommandHierarchyBuilder::rootProperty(
    const QString& name, const QVariant& value) {
    root_builder_.property(name, value);
    return *this;
}

CommandHierarchyBuilder& CommandHierarchyBuilder::rootProperties(
    const QJsonObject& props) {
    root_builder_.properties(props);
    return *this;
}

CommandHierarchyBuilder& CommandHierarchyBuilder::layout(
    const QString& layoutType) {
    layout_type_ = layoutType;
    root_builder_.property("layout", layoutType);
    return *this;
}

CommandHierarchyBuilder& CommandHierarchyBuilder::spacing(int spacing) {
    spacing_ = spacing;
    root_builder_.property("spacing", spacing);
    return *this;
}

CommandHierarchyBuilder& CommandHierarchyBuilder::margins(int left, int top,
                                                          int right,
                                                          int bottom) {
    margin_left_ = left;
    margin_top_ = top;
    margin_right_ = right;
    margin_bottom_ = bottom;

    root_builder_.property("marginLeft", left)
        .property("marginTop", top)
        .property("marginRight", right)
        .property("marginBottom", bottom);
    return *this;
}

CommandHierarchyBuilder& CommandHierarchyBuilder::margins(int margin) {
    return margins(margin, margin, margin, margin);
}

CommandHierarchyBuilder& CommandHierarchyBuilder::style(
    const QString& styleSheet) {
    root_builder_.style(styleSheet);
    return *this;
}

CommandHierarchyBuilder& CommandHierarchyBuilder::addChild(
    const CommandBuilder& childBuilder) {
    child_builders_.emplace_back(
        std::move(const_cast<CommandBuilder&>(childBuilder)));
    return *this;
}

CommandHierarchyBuilder& CommandHierarchyBuilder::addChild(
    const QString& commandType,
    std::function<void(CommandBuilder&)> configurator) {
    CommandBuilder builder(commandType);
    if (configurator) {
        configurator(builder);
    }
    return addChild(builder);
}

CommandHierarchyBuilder& CommandHierarchyBuilder::addChild(
    std::shared_ptr<BaseUICommand> command) {
    if (command) {
        // Store the command directly for later integration
        prebuilt_commands_.push_back(command);
    }
    return *this;
}

CommandHierarchyBuilder& CommandHierarchyBuilder::addChildren(
    const std::vector<CommandBuilder>& childBuilders) {
    for (const auto& builder : childBuilders) {
        addChild(builder);
    }
    return *this;
}

CommandHierarchyBuilder& CommandHierarchyBuilder::addChildIf(
    bool condition, const CommandBuilder& childBuilder) {
    if (condition) {
        addChild(childBuilder);
    }
    return *this;
}

CommandHierarchyBuilder& CommandHierarchyBuilder::addChildIf(
    bool condition, const QString& commandType,
    std::function<void(CommandBuilder&)> configurator) {
    if (condition) {
        addChild(commandType, configurator);
    }
    return *this;
}

CommandHierarchyBuilder& CommandHierarchyBuilder::addContainer(
    const QString& containerType,
    std::function<void(CommandHierarchyBuilder&)> configurator) {
    CommandHierarchyBuilder containerBuilder(containerType);
    if (configurator) {
        configurator(containerBuilder);
    }

    auto containerCommand = containerBuilder.build();
    if (containerCommand) {
        root_builder_.child(containerCommand);
    }

    return *this;
}

CommandHierarchyBuilder& CommandHierarchyBuilder::addRepeated(
    int count, const QString& commandType,
    std::function<void(CommandBuilder&, int)> configurator) {
    for (int i = 0; i < count; ++i) {
        CommandBuilder builder(commandType);
        if (configurator) {
            configurator(builder, i);
        }
        addChild(builder);
    }
    return *this;
}

std::shared_ptr<BaseUICommand> CommandHierarchyBuilder::build() {
    // Add all child builders to the root
    for (const auto& childBuilder : child_builders_) {
        root_builder_.child(childBuilder);
    }

    // Build the root command
    auto rootCommand = root_builder_.build();

    // Add prebuilt commands as children
    for (const auto& prebuiltCommand : prebuilt_commands_) {
        if (rootCommand && prebuiltCommand) {
            rootCommand->addChild(prebuiltCommand);
        }
    }

    return rootCommand;
}

std::pair<std::shared_ptr<BaseUICommand>, std::unique_ptr<QWidget>>
CommandHierarchyBuilder::buildWithWidget() {
    auto command = build();
    if (!command) {
        return {nullptr, nullptr};
    }

    auto widget = WidgetMapper::instance().createWidget(command.get());
    return {command, std::move(widget)};
}

std::unique_ptr<CommandUIElement> CommandHierarchyBuilder::buildAsUIElement() {
    auto command = build();
    if (!command) {
        return nullptr;
    }

    return std::make_unique<CommandUIElement>(command);
}

QJsonObject CommandHierarchyBuilder::toJson() const {
    QJsonObject json;
    json["type"] = "Hierarchy";
    json["layout"] = layout_type_;
    json["spacing"] = spacing_;

    QJsonObject margins;
    margins["left"] = margin_left_;
    margins["top"] = margin_top_;
    margins["right"] = margin_right_;
    margins["bottom"] = margin_bottom_;
    json["margins"] = margins;

    QJsonArray children;
    for (const auto& childBuilder : child_builders_) {
        // Convert child builder to JSON - this would need more implementation
        QJsonObject childJson;
        childJson["type"] = "ChildBuilder";  // Placeholder
        Q_UNUSED(childBuilder);              // Mark as used to avoid warning
        children.append(childJson);
    }
    json["children"] = children;

    return json;
}

CommandHierarchyBuilder CommandHierarchyBuilder::fromJson(
    const QJsonObject& json) {
    QString rootType = json.value("type").toString("Container");
    CommandHierarchyBuilder builder(rootType);

    if (json.contains("layout")) {
        builder.layout(json["layout"].toString());
    }

    if (json.contains("spacing")) {
        builder.spacing(json["spacing"].toInt());
    }

    if (json.contains("margins")) {
        QJsonObject margins = json["margins"].toObject();
        builder.margins(margins["left"].toInt(), margins["top"].toInt(),
                        margins["right"].toInt(), margins["bottom"].toInt());
    }

    // Process children - this would need more implementation

    return builder;
}

void CommandHierarchyBuilder::setupRootAsContainer() {
    root_builder_.property("layout", layout_type_)
        .property("spacing", spacing_)
        .property("marginLeft", margin_left_)
        .property("marginTop", margin_top_)
        .property("marginRight", margin_right_)
        .property("marginBottom", margin_bottom_);
}

// **CommandLayoutBuilder implementation**
CommandLayoutBuilder::CommandLayoutBuilder(const QString& layoutType)
    : layout_type_(layoutType), hierarchy_builder_("Container") {
    hierarchy_builder_.layout(layoutType);
    qDebug() << "📐 CommandLayoutBuilder created for layout:" << layoutType;
}

CommandLayoutBuilder& CommandLayoutBuilder::spacing(int spacing) {
    hierarchy_builder_.spacing(spacing);
    return *this;
}

CommandLayoutBuilder& CommandLayoutBuilder::margins(int left, int top,
                                                    int right, int bottom) {
    hierarchy_builder_.margins(left, top, right, bottom);
    return *this;
}

CommandLayoutBuilder& CommandLayoutBuilder::margins(int margin) {
    hierarchy_builder_.margins(margin);
    return *this;
}

CommandLayoutBuilder& CommandLayoutBuilder::alignment(int alignment) {
    hierarchy_builder_.rootProperty("alignment", alignment);
    return *this;
}

CommandLayoutBuilder& CommandLayoutBuilder::addStretch(int stretch) {
    // Add a stretch item - this would need special handling in the layout
    hierarchy_builder_.rootProperty("_stretch", stretch);
    return *this;
}

CommandLayoutBuilder& CommandLayoutBuilder::addSpacing(int spacing) {
    // Add spacing item - this would need special handling in the layout
    hierarchy_builder_.rootProperty("_spacing", spacing);
    return *this;
}

CommandLayoutBuilder& CommandLayoutBuilder::addToGrid(
    const CommandBuilder& childBuilder, int row, int column, int rowSpan,
    int columnSpan) {
    grid_items_.emplace_back(
        std::move(const_cast<CommandBuilder&>(childBuilder)), row, column,
        rowSpan, columnSpan);
    return *this;
}

CommandLayoutBuilder& CommandLayoutBuilder::setRowStretch(int row,
                                                          int stretch) {
    hierarchy_builder_.rootProperty(QString("rowStretch_%1").arg(row), stretch);
    return *this;
}

CommandLayoutBuilder& CommandLayoutBuilder::setColumnStretch(int column,
                                                             int stretch) {
    hierarchy_builder_.rootProperty(QString("columnStretch_%1").arg(column),
                                    stretch);
    return *this;
}

CommandLayoutBuilder& CommandLayoutBuilder::addRow(
    const QString& label, const CommandBuilder& fieldBuilder) {
    form_rows_.emplace_back(
        label, std::move(const_cast<CommandBuilder&>(fieldBuilder)));
    return *this;
}

CommandLayoutBuilder& CommandLayoutBuilder::addRow(
    const CommandBuilder& labelBuilder, const CommandBuilder& fieldBuilder) {
    form_rows_.emplace_back(
        std::move(const_cast<CommandBuilder&>(labelBuilder)),
        std::move(const_cast<CommandBuilder&>(fieldBuilder)));
    return *this;
}

CommandLayoutBuilder& CommandLayoutBuilder::add(
    const CommandBuilder& childBuilder) {
    hierarchy_builder_.addChild(childBuilder);
    return *this;
}

CommandLayoutBuilder& CommandLayoutBuilder::add(
    const QString& commandType,
    std::function<void(CommandBuilder&)> configurator) {
    hierarchy_builder_.addChild(commandType, configurator);
    return *this;
}

std::shared_ptr<BaseUICommand> CommandLayoutBuilder::build() {
    // Handle special layout types
    if (layout_type_ == "Grid") {
        for (const auto& item : grid_items_) {
            // Add grid-specific properties to the child
            auto& childBuilder = item.builder;
            const_cast<CommandBuilder&>(childBuilder)
                .property("gridRow", item.row)
                .property("gridColumn", item.column)
                .property("gridRowSpan", item.rowSpan)
                .property("gridColumnSpan", item.columnSpan);

            hierarchy_builder_.addChild(childBuilder);
        }
    } else if (layout_type_ == "Form") {
        for (const auto& row : form_rows_) {
            if (row.has_label_builder) {
                hierarchy_builder_.addChild(row.label_builder);
            } else {
                QString labelText = row.label_text;
                hierarchy_builder_.addChild(
                    "Label",
                    [labelText](CommandBuilder& b) { b.text(labelText); });
            }
            hierarchy_builder_.addChild(row.field_builder);
        }
    }

    return hierarchy_builder_.build();
}

std::pair<std::shared_ptr<BaseUICommand>, std::unique_ptr<QWidget>>
CommandLayoutBuilder::buildWithWidget() {
    auto command = build();
    if (!command) {
        return {nullptr, nullptr};
    }

    auto widget = WidgetMapper::instance().createWidget(command.get());
    return {command, std::move(widget)};
}

}  // namespace DeclarativeUI::Command::UI
