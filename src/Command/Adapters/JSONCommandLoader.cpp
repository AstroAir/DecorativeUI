#include "JSONCommandLoader.hpp"

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>

#include "../MVCIntegration.hpp"
#include "../UICommandFactory.hpp"
#include "../WidgetMapper.hpp"

namespace DeclarativeUI::Command::Adapters {

JSONCommandLoader::JSONCommandLoader(QObject* parent) : QObject(parent) {
    // Initialize legacy loader for fallback
    legacy_loader_ = std::make_unique<DeclarativeUI::JSON::JSONUILoader>();
    qDebug() << "📋 JSONCommandLoader initialized";
}

std::shared_ptr<UI::BaseUICommand> JSONCommandLoader::loadCommandFromFile(
    const QString& filePath) {
    emit commandLoadingStarted(filePath);

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QString error = QString("Failed to open file: %1").arg(filePath);
        emit commandLoadingFailed(filePath, error);
        return nullptr;
    }

    QByteArray data = file.readAll();
    QString jsonString = QString::fromUtf8(data);

    auto command = loadCommandFromString(jsonString);
    if (command) {
        emit commandLoadingFinished(filePath);
    } else {
        emit commandLoadingFailed(filePath, "Failed to parse JSON");
    }

    return command;
}

std::shared_ptr<UI::BaseUICommand> JSONCommandLoader::loadCommandFromString(
    const QString& jsonString) {
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8(), &error);

    if (error.error != QJsonParseError::NoError) {
        qWarning() << "JSON parse error:" << error.errorString();
        return nullptr;
    }

    return loadCommandFromObject(doc.object());
}

std::shared_ptr<UI::BaseUICommand> JSONCommandLoader::loadCommandFromObject(
    const QJsonObject& jsonObject) {
    if (!validateCommandJSON(jsonObject)) {
        qWarning() << "Invalid Command JSON structure";
        return nullptr;
    }

    auto command = createCommandFromObject(jsonObject);
    if (command) {
        emit commandCreated(command);

        if (auto_mvc_integration_) {
            setupAutoIntegrations(command, jsonObject);
        }
    }

    return command;
}

std::pair<std::shared_ptr<UI::BaseUICommand>, std::unique_ptr<QWidget>>
JSONCommandLoader::loadCommandWithWidgetFromFile(const QString& filePath) {
    auto command = loadCommandFromFile(filePath);
    if (!command) {
        return {nullptr, nullptr};
    }

    auto widget = UI::WidgetMapper::instance().createWidget(command.get());
    if (widget) {
        emit widgetMapped(command, widget.get());
    }

    return {command, std::move(widget)};
}

std::pair<std::shared_ptr<UI::BaseUICommand>, std::unique_ptr<QWidget>>
JSONCommandLoader::loadCommandWithWidgetFromString(const QString& jsonString) {
    auto command = loadCommandFromString(jsonString);
    if (!command) {
        return {nullptr, nullptr};
    }

    auto widget = UI::WidgetMapper::instance().createWidget(command.get());
    if (widget) {
        emit widgetMapped(command, widget.get());
    }

    return {command, std::move(widget)};
}

std::pair<std::shared_ptr<UI::BaseUICommand>, std::unique_ptr<QWidget>>
JSONCommandLoader::loadCommandWithWidgetFromObject(
    const QJsonObject& jsonObject) {
    auto command = loadCommandFromObject(jsonObject);
    if (!command) {
        return {nullptr, nullptr};
    }

    auto widget = UI::WidgetMapper::instance().createWidget(command.get());
    if (widget) {
        emit widgetMapped(command, widget.get());
    }

    return {command, std::move(widget)};
}

std::shared_ptr<UI::BaseUICommand> JSONCommandLoader::wrapExistingWidget(
    std::unique_ptr<QWidget> widget) {
    // This would create a Command wrapper around an existing QWidget
    // For now, return nullptr as this requires more complex implementation
    Q_UNUSED(widget)
    qWarning() << "wrapExistingWidget not yet implemented";
    return nullptr;
}

std::shared_ptr<UI::BaseUICommand> JSONCommandLoader::loadAndWrapFromFile(
    const QString& filePath) {
    // Load using legacy loader and wrap in Command
    if (legacy_loader_) {
        auto widget = legacy_loader_->loadFromFile(filePath);
        if (widget) {
            return wrapExistingWidget(std::move(widget));
        }
    }
    return nullptr;
}

void JSONCommandLoader::bindStateManager(
    std::shared_ptr<Binding::StateManager> stateManager) {
    state_manager_ = stateManager;
}

void JSONCommandLoader::registerEventHandler(
    const QString& handlerName, std::function<void(const QVariant&)> handler) {
    event_handlers_[handlerName] = handler;
}

void JSONCommandLoader::registerCommandFactory(
    const QString& commandType,
    std::function<std::shared_ptr<UI::BaseUICommand>()> factory) {
    command_factories_[commandType] = factory;
}

void JSONCommandLoader::registerPropertyConverter(
    const QString& propertyType,
    std::function<QVariant(const QJsonValue&)> converter) {
    property_converters_[propertyType] = converter;
}

bool JSONCommandLoader::validateCommandJSON(
    const QJsonObject& jsonObject) const {
    return CommandJSONValidator::validate(jsonObject);
}

QStringList JSONCommandLoader::getValidationErrors(
    const QJsonObject& jsonObject) const {
    return CommandJSONValidator::getValidationErrors(jsonObject);
}

QJsonObject JSONCommandLoader::convertWidgetJSONToCommandJSON(
    const QJsonObject& widgetJSON) {
    return JSONMigrationUtility::migrateToCommandFormat(widgetJSON);
}

QJsonObject JSONCommandLoader::convertCommandJSONToWidgetJSON(
    const QJsonObject& commandJSON) {
    return JSONMigrationUtility::migrateToLegacyFormat(commandJSON);
}

std::shared_ptr<UI::BaseUICommand> JSONCommandLoader::createCommandFromObject(
    const QJsonObject& commandObject) {
    QString type = commandObject.value("type").toString();
    if (type.isEmpty()) {
        qWarning() << "Command type not specified in JSON";
        return nullptr;
    }

    // Try custom factories first
    auto factoryIt = command_factories_.find(type);
    if (factoryIt != command_factories_.end()) {
        return factoryIt->second();
    }

    // Use standard factory
    auto command = UI::UICommandFactory::instance().createCommand(type);
    if (!command) {
        qWarning() << "Failed to create command of type:" << type;
        return nullptr;
    }

    // Apply properties
    if (commandObject.contains("properties")) {
        applyPropertiesFromJSON(command,
                                commandObject["properties"].toObject());
    }

    // Set up event handlers
    if (commandObject.contains("events")) {
        setupEventHandlersFromJSON(command, commandObject["events"].toObject());
    }

    // Set up state bindings
    if (commandObject.contains("bindings")) {
        setupStateBindingsFromJSON(command,
                                   commandObject["bindings"].toObject());
    }

    // Process children
    if (commandObject.contains("children")) {
        processChildrenFromJSON(command, commandObject["children"].toArray());
    }

    return command;
}

void JSONCommandLoader::applyPropertiesFromJSON(
    std::shared_ptr<UI::BaseUICommand> command, const QJsonObject& properties) {
    for (auto it = properties.begin(); it != properties.end(); ++it) {
        QString propertyName = it.key();
        QVariant value = convertPropertyValue(propertyName, it.value());
        command->getState()->setProperty(propertyName, value);
    }
}

void JSONCommandLoader::setupEventHandlersFromJSON(
    std::shared_ptr<UI::BaseUICommand> command, const QJsonObject& events) {
    for (auto it = events.begin(); it != events.end(); ++it) {
        QString eventType = it.key();
        QString handlerName = it.value().toString();

        auto handlerIt = event_handlers_.find(handlerName);
        if (handlerIt != event_handlers_.end()) {
            connect(command.get(), &UI::BaseUICommand::eventTriggered,
                    [eventType, handler = handlerIt->second](
                        const QString& type, const QVariant& data) {
                        if (type == eventType && handler) {
                            handler(data);
                        }
                    });
        }
    }
}

void JSONCommandLoader::setupStateBindingsFromJSON(
    std::shared_ptr<UI::BaseUICommand> command, const QJsonObject& bindings) {
    if (!auto_state_binding_) {
        return;
    }

    for (auto it = bindings.begin(); it != bindings.end(); ++it) {
        QString commandProperty = it.key();
        QString stateKey = it.value().toString();

        // Use MVC integration to set up binding
        UI::MVCIntegrationBridge::instance().bindCommandToStateManager(
            command, stateKey, commandProperty);
    }
}

void JSONCommandLoader::processChildrenFromJSON(
    std::shared_ptr<UI::BaseUICommand> parent, const QJsonArray& children) {
    for (const auto& childValue : children) {
        if (childValue.isObject()) {
            auto childCommand = createCommandFromObject(childValue.toObject());
            if (childCommand) {
                parent->addChild(childCommand);
            }
        }
    }
}

QVariant JSONCommandLoader::convertPropertyValue(const QString& propertyType,
                                                 const QJsonValue& value) {
    auto converterIt = property_converters_.find(propertyType);
    if (converterIt != property_converters_.end()) {
        return converterIt->second(value);
    }

    return value.toVariant();
}

void JSONCommandLoader::setupAutoIntegrations(
    std::shared_ptr<UI::BaseUICommand> command, const QJsonObject& config) {
    // Set up automatic MVC integrations based on configuration
    if (config.contains("actions")) {
        QJsonArray actions = config["actions"].toArray();
        for (const auto& actionValue : actions) {
            QString actionName = actionValue.toString();
            UI::MVCIntegrationBridge::instance().registerCommandAsAction(
                command, actionName);
        }
    }
}

void JSONCommandLoader::onLegacyLoadingFinished(const QString& source) {
    qDebug() << "Legacy loading finished:" << source;
}

void JSONCommandLoader::onLegacyLoadingFailed(const QString& source,
                                              const QString& error) {
    qWarning() << "Legacy loading failed:" << source << error;
}

// **CommandComponentRegistry implementation**
CommandComponentRegistry& CommandComponentRegistry::instance() {
    static CommandComponentRegistry instance;
    return instance;
}

void CommandComponentRegistry::registerCommandComponent(
    const QString& typeName,
    std::function<std::shared_ptr<UI::BaseUICommand>(const QJsonObject&)>
        factory) {
    command_factories_[typeName] = factory;
}

std::shared_ptr<UI::BaseUICommand> CommandComponentRegistry::createCommand(
    const QString& typeName, const QJsonObject& config) {
    auto it = command_factories_.find(typeName);
    if (it != command_factories_.end()) {
        return it->second(config);
    }
    return nullptr;
}

void CommandComponentRegistry::syncWithComponentRegistry() {
    // Sync with existing ComponentRegistry
    qDebug() << "Syncing with ComponentRegistry";
}

void CommandComponentRegistry::registerLegacyComponentAsCommand(
    const QString& componentType, const QString& commandType) {
    legacy_to_command_mapping_[componentType] = commandType;
}

bool CommandComponentRegistry::hasCommandComponent(
    const QString& typeName) const {
    return command_factories_.find(typeName) != command_factories_.end();
}

QStringList CommandComponentRegistry::getRegisteredCommandTypes() const {
    QStringList types;
    for (const auto& [type, factory] : command_factories_) {
        types.append(type);
    }
    return types;
}

void CommandComponentRegistry::clear() {
    command_factories_.clear();
    legacy_to_command_mapping_.clear();
}

void CommandComponentRegistry::registerBuiltinCommandComponents() {
    // Register built-in command components
    registerCommandComponent<UI::ButtonCommand>("Button");
    registerCommandComponent<UI::LabelCommand>("Label");
    registerCommandComponent<UI::TextInputCommand>("TextInput");
    registerCommandComponent<UI::ContainerCommand>("Container");
}

// **CommandJSONValidator implementation**
bool CommandJSONValidator::validate(const QJsonObject& commandJSON) {
    return getValidationErrors(commandJSON).isEmpty();
}

QStringList CommandJSONValidator::getValidationErrors(
    const QJsonObject& commandJSON) {
    QStringList errors;

    if (!commandJSON.contains("type")) {
        errors.append("Missing required 'type' field");
    } else if (!commandJSON["type"].isString()) {
        errors.append("'type' field must be a string");
    }

    // Additional validation logic would go here
    return errors;
}

QJsonObject CommandJSONValidator::getCommandSchema() {
    // Return JSON schema for Command objects
    return QJsonObject{
        {"type", "object"},
        {"required", QJsonArray{"type"}},
        {"properties",
         QJsonObject{{"type", QJsonObject{{"type", "string"}}},
                     {"properties", QJsonObject{{"type", "object"}}},
                     {"events", QJsonObject{{"type", "object"}}},
                     {"bindings", QJsonObject{{"type", "object"}}},
                     {"children", QJsonObject{{"type", "array"}}}}}};
}

// **JSONMigrationUtility implementation**
QJsonObject JSONMigrationUtility::migrateToCommandFormat(
    const QJsonObject& legacyJSON) {
    QJsonObject commandJSON = legacyJSON;

    // Convert legacy component types to command types
    if (commandJSON.contains("type")) {
        QString legacyType = commandJSON["type"].toString();
        QString commandType = mapComponentType(legacyType);
        commandJSON["type"] = commandType;
    }

    // Migrate children
    if (commandJSON.contains("children")) {
        commandJSON["children"] =
            migrateChildrenToCommandFormat(commandJSON["children"].toArray());
    }

    return commandJSON;
}

QJsonArray JSONMigrationUtility::migrateChildrenToCommandFormat(
    const QJsonArray& legacyChildren) {
    QJsonArray commandChildren;

    for (const auto& child : legacyChildren) {
        if (child.isObject()) {
            commandChildren.append(migrateToCommandFormat(child.toObject()));
        }
    }

    return commandChildren;
}

QJsonObject JSONMigrationUtility::migrateToLegacyFormat(
    const QJsonObject& commandJSON) {
    // Reverse migration - convert Command JSON back to legacy format
    QJsonObject legacyJSON = commandJSON;

    if (legacyJSON.contains("type")) {
        QString commandType = legacyJSON["type"].toString();
        // Map back to legacy type if needed
        legacyJSON["type"] = commandType;
    }

    return legacyJSON;
}

QString JSONMigrationUtility::generateMigrationReport(
    const QJsonObject& legacyJSON) {
    QString report = "Migration Report:\n";
    report += QString("- Legacy type: %1\n").arg(legacyJSON["type"].toString());
    report += QString("- Command type: %1\n")
                  .arg(mapComponentType(legacyJSON["type"].toString()));
    return report;
}

QString JSONMigrationUtility::mapComponentType(const QString& legacyType) {
    // Map legacy component types to command types
    static QHash<QString, QString> typeMap = {{"Widget", "Container"},
                                              {"PushButton", "Button"},
                                              {"QLabel", "Label"},
                                              {"QLineEdit", "TextInput"}};

    return typeMap.value(legacyType, legacyType);
}

}  // namespace DeclarativeUI::Command::Adapters
