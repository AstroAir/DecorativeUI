#pragma once

#include <QObject>
#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QWidget>
#include <memory>
#include <functional>
#include <unordered_map>

#include "../UICommand.hpp"
#include "../UICommandFactory.hpp"
#include "../CommandBuilder.hpp"
#include "../WidgetMapper.hpp"
#include "../MVCIntegration.hpp"
#include "../../JSON/JSONUILoader.hpp"
#include "../../JSON/ComponentRegistry.hpp"
#include "../../Binding/StateManager.hpp"

namespace DeclarativeUI::Command::Adapters {

/**
 * @brief Enhanced JSON loader that creates Command hierarchies instead of direct QWidget hierarchies
 * 
 * This adapter extends the existing JSON UI loading capability to work with the Command system,
 * providing seamless integration between JSON-defined UIs and the Command architecture.
 */
class JSONCommandLoader : public QObject {
    Q_OBJECT
    
public:
    explicit JSONCommandLoader(QObject* parent = nullptr);
    virtual ~JSONCommandLoader() = default;
    
    // **Load Command hierarchy from JSON**
    std::shared_ptr<UI::BaseUICommand> loadCommandFromFile(const QString& filePath);
    std::shared_ptr<UI::BaseUICommand> loadCommandFromString(const QString& jsonString);
    std::shared_ptr<UI::BaseUICommand> loadCommandFromObject(const QJsonObject& jsonObject);
    
    // **Load Command hierarchy with automatic widget creation**
    std::pair<std::shared_ptr<UI::BaseUICommand>, std::unique_ptr<QWidget>> 
    loadCommandWithWidgetFromFile(const QString& filePath);
    
    std::pair<std::shared_ptr<UI::BaseUICommand>, std::unique_ptr<QWidget>> 
    loadCommandWithWidgetFromString(const QString& jsonString);
    
    std::pair<std::shared_ptr<UI::BaseUICommand>, std::unique_ptr<QWidget>> 
    loadCommandWithWidgetFromObject(const QJsonObject& jsonObject);
    
    // **Hybrid loading - use existing JSONUILoader but wrap in Commands**
    std::shared_ptr<UI::BaseUICommand> wrapExistingWidget(std::unique_ptr<QWidget> widget);
    std::shared_ptr<UI::BaseUICommand> loadAndWrapFromFile(const QString& filePath);
    
    // **Configuration**
    void setAutoMVCIntegration(bool enabled) { auto_mvc_integration_ = enabled; }
    bool isAutoMVCIntegrationEnabled() const { return auto_mvc_integration_; }
    
    void setAutoStateBinding(bool enabled) { auto_state_binding_ = enabled; }
    bool isAutoStateBindingEnabled() const { return auto_state_binding_; }
    
    void setAutoEventHandling(bool enabled) { auto_event_handling_ = enabled; }
    bool isAutoEventHandlingEnabled() const { return auto_event_handling_; }
    
    // **State management integration**
    void bindStateManager(std::shared_ptr<Binding::StateManager> stateManager);
    
    // **Event handler registration**
    void registerEventHandler(const QString& handlerName, std::function<void(const QVariant&)> handler);
    
    // **Custom command factory registration**
    void registerCommandFactory(const QString& commandType, std::function<std::shared_ptr<UI::BaseUICommand>()> factory);
    
    // **Property converter registration**
    void registerPropertyConverter(const QString& propertyType, 
                                  std::function<QVariant(const QJsonValue&)> converter);
    
    // **Validation**
    bool validateCommandJSON(const QJsonObject& jsonObject) const;
    QStringList getValidationErrors(const QJsonObject& jsonObject) const;
    
    // **Migration helpers**
    QJsonObject convertWidgetJSONToCommandJSON(const QJsonObject& widgetJSON);
    QJsonObject convertCommandJSONToWidgetJSON(const QJsonObject& commandJSON);
    
signals:
    void commandLoadingStarted(const QString& source);
    void commandLoadingFinished(const QString& source);
    void commandLoadingFailed(const QString& source, const QString& error);
    void commandCreated(std::shared_ptr<UI::BaseUICommand> command);
    void widgetMapped(std::shared_ptr<UI::BaseUICommand> command, QWidget* widget);
    
private:
    // **Configuration**
    bool auto_mvc_integration_ = true;
    bool auto_state_binding_ = true;
    bool auto_event_handling_ = true;
    
    // **Dependencies**
    std::shared_ptr<Binding::StateManager> state_manager_;
    std::unique_ptr<JSON::JSONUILoader> legacy_loader_;
    
    // **Custom factories and handlers**
    std::unordered_map<QString, std::function<std::shared_ptr<UI::BaseUICommand>()>> command_factories_;
    std::unordered_map<QString, std::function<void(const QVariant&)>> event_handlers_;
    std::unordered_map<QString, std::function<QVariant(const QJsonValue&)>> property_converters_;
    
    // **Helper methods**
    std::shared_ptr<UI::BaseUICommand> createCommandFromObject(const QJsonObject& commandObject);
    void applyPropertiesFromJSON(std::shared_ptr<UI::BaseUICommand> command, const QJsonObject& properties);
    void setupEventHandlersFromJSON(std::shared_ptr<UI::BaseUICommand> command, const QJsonObject& events);
    void setupStateBindingsFromJSON(std::shared_ptr<UI::BaseUICommand> command, const QJsonObject& bindings);
    void processChildrenFromJSON(std::shared_ptr<UI::BaseUICommand> parent, const QJsonArray& children);
    
    QVariant convertPropertyValue(const QString& propertyType, const QJsonValue& value);
    void validateCommandObject(const QJsonObject& commandObject, QStringList& errors) const;
    void setupAutoIntegrations(std::shared_ptr<UI::BaseUICommand> command, const QJsonObject& config);
    
private slots:
    void onLegacyLoadingFinished(const QString& source);
    void onLegacyLoadingFailed(const QString& source, const QString& error);
};

/**
 * @brief Command-aware Component Registry that integrates with the existing ComponentRegistry
 */
class CommandComponentRegistry {
public:
    static CommandComponentRegistry& instance();
    
    // **Register Command factories for JSON loading**
    template<typename CommandType>
    void registerCommandComponent(const QString& typeName);
    
    void registerCommandComponent(const QString& typeName, 
                                 std::function<std::shared_ptr<UI::BaseUICommand>(const QJsonObject&)> factory);
    
    // **Create Command from JSON configuration**
    std::shared_ptr<UI::BaseUICommand> createCommand(const QString& typeName, const QJsonObject& config);
    
    // **Integration with existing ComponentRegistry**
    void syncWithComponentRegistry();
    void registerLegacyComponentAsCommand(const QString& componentType, const QString& commandType);
    
    // **Query methods**
    bool hasCommandComponent(const QString& typeName) const;
    QStringList getRegisteredCommandTypes() const;
    
    // **Clear registry (for testing)**
    void clear();
    
private:
    CommandComponentRegistry() { registerBuiltinCommandComponents(); }
    
    std::unordered_map<QString, std::function<std::shared_ptr<UI::BaseUICommand>(const QJsonObject&)>> command_factories_;
    std::unordered_map<QString, QString> legacy_to_command_mapping_;
    
    void registerBuiltinCommandComponents();
};

/**
 * @brief JSON Schema validator for Command-based UI definitions
 */
class CommandJSONValidator {
public:
    // **Validate Command JSON structure**
    static bool validate(const QJsonObject& commandJSON);
    static QStringList getValidationErrors(const QJsonObject& commandJSON);
    
    // **Schema definitions**
    static QJsonObject getCommandSchema();
    static QJsonObject getPropertySchema();
    static QJsonObject getEventSchema();
    static QJsonObject getBindingSchema();
    
    // **Validation rules**
    static bool validateCommandType(const QString& type);
    static bool validatePropertyValue(const QString& property, const QJsonValue& value);
    static bool validateEventHandler(const QString& event, const QJsonValue& handler);
    static bool validateStateBinding(const QJsonObject& binding);
    
private:
    static QStringList validateObject(const QJsonObject& obj, const QJsonObject& schema, const QString& path = "");
    static bool matchesType(const QJsonValue& value, const QString& expectedType);
};

/**
 * @brief Migration utility for converting existing JSON UI definitions to Command format
 */
class JSONMigrationUtility {
public:
    // **Convert legacy JSON to Command JSON**
    static QJsonObject migrateToCommandFormat(const QJsonObject& legacyJSON);
    static QJsonArray migrateChildrenToCommandFormat(const QJsonArray& legacyChildren);
    
    // **Convert Command JSON to legacy format (for backward compatibility)**
    static QJsonObject migrateToLegacyFormat(const QJsonObject& commandJSON);
    
    // **Migration analysis**
    static QString generateMigrationReport(const QJsonObject& legacyJSON);
    static QStringList getUnsupportedFeatures(const QJsonObject& legacyJSON);
    static QStringList getRecommendations(const QJsonObject& legacyJSON);
    
    // **Batch migration**
    static void migrateDirectory(const QString& sourceDir, const QString& targetDir);
    static void migrateFile(const QString& sourceFile, const QString& targetFile);
    
private:
    static QJsonObject migrateComponent(const QJsonObject& component);
    static QJsonObject migrateProperties(const QJsonObject& properties);
    static QJsonObject migrateEvents(const QJsonObject& events);
    static QString mapComponentType(const QString& legacyType);
    static QString mapPropertyName(const QString& legacyProperty);
    static QString mapEventName(const QString& legacyEvent);
};

// **Template implementation**
template<typename CommandType>
void CommandComponentRegistry::registerCommandComponent(const QString& typeName) {
    static_assert(std::is_base_of_v<UI::BaseUICommand, CommandType>, 
                  "CommandType must inherit from BaseUICommand");
    
    command_factories_[typeName] = [](const QJsonObject& config) -> std::shared_ptr<UI::BaseUICommand> {
        auto command = std::make_shared<CommandType>();
        
        // Apply configuration from JSON
        if (config.contains("properties")) {
            QJsonObject properties = config["properties"].toObject();
            for (auto it = properties.begin(); it != properties.end(); ++it) {
                command->getState()->setProperty(it.key(), it.value().toVariant());
            }
        }
        
        return command;
    };
}

// **Convenience functions for JSON Command loading**
namespace JSON {

// **Quick loading functions**
inline std::shared_ptr<UI::BaseUICommand> loadCommand(const QString& filePath) {
    JSONCommandLoader loader;
    return loader.loadCommandFromFile(filePath);
}

inline std::shared_ptr<UI::BaseUICommand> loadCommandFromString(const QString& jsonString) {
    JSONCommandLoader loader;
    return loader.loadCommandFromString(jsonString);
}

// **Loading with automatic widget creation**
inline std::pair<std::shared_ptr<UI::BaseUICommand>, std::unique_ptr<QWidget>> 
loadCommandWithWidget(const QString& filePath) {
    JSONCommandLoader loader;
    return loader.loadCommandWithWidgetFromFile(filePath);
}

// **Migration helpers**
inline QJsonObject migrateToCommand(const QJsonObject& legacyJSON) {
    return JSONMigrationUtility::migrateToCommandFormat(legacyJSON);
}

inline QJsonObject migrateToLegacy(const QJsonObject& commandJSON) {
    return JSONMigrationUtility::migrateToLegacyFormat(commandJSON);
}

// **Validation helpers**
inline bool validateCommand(const QJsonObject& commandJSON) {
    return CommandJSONValidator::validate(commandJSON);
}

inline QStringList getValidationErrors(const QJsonObject& commandJSON) {
    return CommandJSONValidator::getValidationErrors(commandJSON);
}

}  // namespace JSON

}  // namespace DeclarativeUI::Command::Adapters
