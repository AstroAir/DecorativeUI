#pragma once

/**
 * @file JSONCommandLoader.hpp
 * @brief Enhanced JSON loader for DeclarativeUI Command system.
 *
 * Provides facilities to load, validate, and migrate JSON-based UI definitions
 * into Command-based UI hierarchies, supporting property/event/state
 * integration, custom factories, migration utilities, and schema validation.
 */

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QWidget>
#include <functional>
#include <memory>
#include <unordered_map>

#include "../../Binding/StateManager.hpp"
#include "../../JSON/JSONUILoader.hpp"
#include "../UICommand.hpp"

namespace DeclarativeUI::Command::Adapters {

/**
 * @class JSONCommandLoader
 * @brief Loads Command hierarchies from JSON definitions, supporting hybrid and
 * legacy integration.
 *
 * This adapter extends JSON UI loading to the Command system, enabling seamless
 * integration between JSON-defined UIs and the Command architecture. Supports
 * property/event/state binding, custom factories, validation, migration, and
 * hybrid widget-command loading.
 */
class JSONCommandLoader : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Constructs a JSONCommandLoader.
     * @param parent Parent QObject.
     */
    explicit JSONCommandLoader(QObject* parent = nullptr);

    /**
     * @brief Destructor.
     */
    virtual ~JSONCommandLoader() = default;

    // **Loading Command hierarchies**

    /**
     * @brief Loads a Command hierarchy from a JSON file.
     * @param filePath Path to the JSON file.
     * @return Shared pointer to the root Command.
     */
    std::shared_ptr<UI::BaseUICommand> loadCommandFromFile(
        const QString& filePath);

    /**
     * @brief Loads a Command hierarchy from a JSON string.
     * @param jsonString JSON string.
     * @return Shared pointer to the root Command.
     */
    std::shared_ptr<UI::BaseUICommand> loadCommandFromString(
        const QString& jsonString);

    /**
     * @brief Loads a Command hierarchy from a QJsonObject.
     * @param jsonObject JSON object.
     * @return Shared pointer to the root Command.
     */
    std::shared_ptr<UI::BaseUICommand> loadCommandFromObject(
        const QJsonObject& jsonObject);

    // **Loading with automatic widget creation**

    /**
     * @brief Loads a Command and creates a corresponding QWidget from a file.
     * @param filePath Path to the JSON file.
     * @return Pair of Command and QWidget.
     */
    std::pair<std::shared_ptr<UI::BaseUICommand>, std::unique_ptr<QWidget>>
    loadCommandWithWidgetFromFile(const QString& filePath);

    /**
     * @brief Loads a Command and creates a corresponding QWidget from a string.
     * @param jsonString JSON string.
     * @return Pair of Command and QWidget.
     */
    std::pair<std::shared_ptr<UI::BaseUICommand>, std::unique_ptr<QWidget>>
    loadCommandWithWidgetFromString(const QString& jsonString);

    /**
     * @brief Loads a Command and creates a corresponding QWidget from a
     * QJsonObject.
     * @param jsonObject JSON object.
     * @return Pair of Command and QWidget.
     */
    std::pair<std::shared_ptr<UI::BaseUICommand>, std::unique_ptr<QWidget>>
    loadCommandWithWidgetFromObject(const QJsonObject& jsonObject);

    // **Hybrid loading and wrapping**

    /**
     * @brief Wraps an existing QWidget in a Command.
     * @param widget Unique pointer to the QWidget.
     * @return Shared pointer to the wrapped Command.
     */
    std::shared_ptr<UI::BaseUICommand> wrapExistingWidget(
        std::unique_ptr<QWidget> widget);

    /**
     * @brief Loads a QWidget using the legacy loader and wraps it in a Command.
     * @param filePath Path to the JSON file.
     * @return Shared pointer to the wrapped Command.
     */
    std::shared_ptr<UI::BaseUICommand> loadAndWrapFromFile(
        const QString& filePath);

    // **Configuration**

    /**
     * @brief Enables or disables automatic MVC integration.
     * @param enabled True to enable, false to disable.
     */
    void setAutoMVCIntegration(bool enabled) {
        auto_mvc_integration_ = enabled;
    }

    /**
     * @brief Checks if automatic MVC integration is enabled.
     * @return True if enabled.
     */
    bool isAutoMVCIntegrationEnabled() const { return auto_mvc_integration_; }

    /**
     * @brief Enables or disables automatic state binding.
     * @param enabled True to enable, false to disable.
     */
    void setAutoStateBinding(bool enabled) { auto_state_binding_ = enabled; }

    /**
     * @brief Checks if automatic state binding is enabled.
     * @return True if enabled.
     */
    bool isAutoStateBindingEnabled() const { return auto_state_binding_; }

    /**
     * @brief Enables or disables automatic event handling.
     * @param enabled True to enable, false to disable.
     */
    void setAutoEventHandling(bool enabled) { auto_event_handling_ = enabled; }

    /**
     * @brief Checks if automatic event handling is enabled.
     * @return True if enabled.
     */
    bool isAutoEventHandlingEnabled() const { return auto_event_handling_; }

    // **State management integration**

    /**
     * @brief Binds a StateManager instance for state integration.
     * @param stateManager Shared pointer to the StateManager.
     */
    void bindStateManager(std::shared_ptr<Binding::StateManager> stateManager);

    // **Event handler registration**

    /**
     * @brief Registers a named event handler for use in JSON.
     * @param handlerName Name of the handler.
     * @param handler Function to handle the event.
     */
    void registerEventHandler(const QString& handlerName,
                              std::function<void(const QVariant&)> handler);

    // **Custom command factory registration**

    /**
     * @brief Registers a custom factory for a Command type.
     * @param commandType Name of the Command type.
     * @param factory Factory function to create the Command.
     */
    void registerCommandFactory(
        const QString& commandType,
        std::function<std::shared_ptr<UI::BaseUICommand>()> factory);

    // **Property converter registration**

    /**
     * @brief Registers a converter for a property type.
     * @param propertyType Name of the property type.
     * @param converter Function to convert from QJsonValue to QVariant.
     */
    void registerPropertyConverter(
        const QString& propertyType,
        std::function<QVariant(const QJsonValue&)> converter);

    // **Validation**

    /**
     * @brief Validates a Command JSON object.
     * @param jsonObject JSON object to validate.
     * @return True if valid.
     */
    bool validateCommandJSON(const QJsonObject& jsonObject) const;

    /**
     * @brief Gets validation errors for a Command JSON object.
     * @param jsonObject JSON object to validate.
     * @return List of error messages.
     */
    QStringList getValidationErrors(const QJsonObject& jsonObject) const;

    // **Migration helpers**

    /**
     * @brief Converts a legacy widget JSON definition to Command JSON format.
     * @param widgetJSON Legacy widget JSON.
     * @return Converted Command JSON.
     */
    QJsonObject convertWidgetJSONToCommandJSON(const QJsonObject& widgetJSON);

    /**
     * @brief Converts a Command JSON definition to legacy widget JSON format.
     * @param commandJSON Command JSON.
     * @return Converted legacy widget JSON.
     */
    QJsonObject convertCommandJSONToWidgetJSON(const QJsonObject& commandJSON);

signals:
    /**
     * @brief Emitted when command loading starts.
     * @param source Source identifier (file, string, etc.).
     */
    void commandLoadingStarted(const QString& source);

    /**
     * @brief Emitted when command loading finishes.
     * @param source Source identifier.
     */
    void commandLoadingFinished(const QString& source);

    /**
     * @brief Emitted when command loading fails.
     * @param source Source identifier.
     * @param error Error message.
     */
    void commandLoadingFailed(const QString& source, const QString& error);

    /**
     * @brief Emitted when a Command is created.
     * @param command Shared pointer to the created Command.
     */
    void commandCreated(std::shared_ptr<UI::BaseUICommand> command);

    /**
     * @brief Emitted when a Command is mapped to a QWidget.
     * @param command Shared pointer to the Command.
     * @param widget Pointer to the QWidget.
     */
    void widgetMapped(std::shared_ptr<UI::BaseUICommand> command,
                      QWidget* widget);

private:
    // **Configuration flags**
    bool auto_mvc_integration_ = true;  ///< Automatic MVC integration enabled.
    bool auto_state_binding_ = true;    ///< Automatic state binding enabled.
    bool auto_event_handling_ = true;   ///< Automatic event handling enabled.

    // **Dependencies**
    std::shared_ptr<Binding::StateManager>
        state_manager_;  ///< State manager for binding.
    std::unique_ptr<JSON::JSONUILoader>
        legacy_loader_;  ///< Legacy JSON UI loader.

    // **Custom factories and handlers**
    std::unordered_map<QString,
                       std::function<std::shared_ptr<UI::BaseUICommand>()>>
        command_factories_;  ///< Command factories.
    std::unordered_map<QString, std::function<void(const QVariant&)>>
        event_handlers_;  ///< Event handlers.
    std::unordered_map<QString, std::function<QVariant(const QJsonValue&)>>
        property_converters_;  ///< Property converters.

    // **Helper methods**

    /**
     * @brief Creates a Command from a JSON object.
     * @param commandObject JSON object.
     * @return Shared pointer to the Command.
     */
    std::shared_ptr<UI::BaseUICommand> createCommandFromObject(
        const QJsonObject& commandObject);

    /**
     * @brief Applies properties from JSON to a Command.
     * @param command Shared pointer to the Command.
     * @param properties JSON object of properties.
     */
    void applyPropertiesFromJSON(std::shared_ptr<UI::BaseUICommand> command,
                                 const QJsonObject& properties);

    /**
     * @brief Sets up event handlers from JSON.
     * @param command Shared pointer to the Command.
     * @param events JSON object of events.
     */
    void setupEventHandlersFromJSON(std::shared_ptr<UI::BaseUICommand> command,
                                    const QJsonObject& events);

    /**
     * @brief Sets up state bindings from JSON.
     * @param command Shared pointer to the Command.
     * @param bindings JSON object of bindings.
     */
    void setupStateBindingsFromJSON(std::shared_ptr<UI::BaseUICommand> command,
                                    const QJsonObject& bindings);

    /**
     * @brief Processes child commands from a JSON array.
     * @param parent Shared pointer to the parent Command.
     * @param children JSON array of child definitions.
     */
    void processChildrenFromJSON(std::shared_ptr<UI::BaseUICommand> parent,
                                 const QJsonArray& children);

    /**
     * @brief Converts a property value using a registered converter.
     * @param propertyType Name of the property type.
     * @param value QJsonValue to convert.
     * @return Converted QVariant.
     */
    QVariant convertPropertyValue(const QString& propertyType,
                                  const QJsonValue& value);

    /**
     * @brief Validates a Command JSON object and collects errors.
     * @param commandObject JSON object.
     * @param errors List to append error messages.
     */
    void validateCommandObject(const QJsonObject& commandObject,
                               QStringList& errors) const;

    /**
     * @brief Sets up automatic integrations (MVC, state, events) for a Command.
     * @param command Shared pointer to the Command.
     * @param config JSON configuration.
     */
    void setupAutoIntegrations(std::shared_ptr<UI::BaseUICommand> command,
                               const QJsonObject& config);

private slots:
    /**
     * @brief Handles legacy loader finished event.
     * @param source Source identifier.
     */
    void onLegacyLoadingFinished(const QString& source);

    /**
     * @brief Handles legacy loader failure event.
     * @param source Source identifier.
     * @param error Error message.
     */
    void onLegacyLoadingFailed(const QString& source, const QString& error);
};

/**
 * @class CommandComponentRegistry
 * @brief Registry for Command-aware components, supporting JSON loading and
 * legacy integration.
 *
 * Allows registration of Command factories, mapping of legacy components, and
 * creation of Commands from JSON.
 */
class CommandComponentRegistry {
public:
    /**
     * @brief Gets the singleton instance.
     * @return Reference to the registry.
     */
    static CommandComponentRegistry& instance();

    /**
     * @brief Registers a Command factory for a type.
     * @tparam CommandType The Command type.
     * @param typeName Name of the type.
     */
    template <typename CommandType>
    void registerCommandComponent(const QString& typeName);

    /**
     * @brief Registers a Command factory for a type.
     * @param typeName Name of the type.
     * @param factory Factory function taking JSON config.
     */
    void registerCommandComponent(
        const QString& typeName,
        std::function<std::shared_ptr<UI::BaseUICommand>(const QJsonObject&)>
            factory);

    /**
     * @brief Creates a Command from JSON configuration.
     * @param typeName Name of the type.
     * @param config JSON configuration.
     * @return Shared pointer to the Command.
     */
    std::shared_ptr<UI::BaseUICommand> createCommand(const QString& typeName,
                                                     const QJsonObject& config);

    /**
     * @brief Synchronizes with the legacy ComponentRegistry.
     */
    void syncWithComponentRegistry();

    /**
     * @brief Registers a legacy component as a Command type.
     * @param componentType Name of the legacy component.
     * @param commandType Name of the Command type.
     */
    void registerLegacyComponentAsCommand(const QString& componentType,
                                          const QString& commandType);

    /**
     * @brief Checks if a Command component is registered.
     * @param typeName Name of the type.
     * @return True if registered.
     */
    bool hasCommandComponent(const QString& typeName) const;

    /**
     * @brief Gets a list of registered Command types.
     * @return List of type names.
     */
    QStringList getRegisteredCommandTypes() const;

    /**
     * @brief Clears the registry (for testing).
     */
    void clear();

private:
    CommandComponentRegistry() { registerBuiltinCommandComponents(); }

    std::unordered_map<
        QString,
        std::function<std::shared_ptr<UI::BaseUICommand>(const QJsonObject&)>>
        command_factories_;  ///< Command factories.
    std::unordered_map<QString, QString>
        legacy_to_command_mapping_;  ///< Legacy to Command type mapping.

    /**
     * @brief Registers built-in Command components.
     */
    void registerBuiltinCommandComponents();
};

/**
 * @class CommandJSONValidator
 * @brief Validates Command-based JSON UI definitions against schema and rules.
 */
class CommandJSONValidator {
public:
    // **Validation**

    /**
     * @brief Validates a Command JSON object.
     * @param commandJSON JSON object.
     * @return True if valid.
     */
    static bool validate(const QJsonObject& commandJSON);

    /**
     * @brief Gets validation errors for a Command JSON object.
     * @param commandJSON JSON object.
     * @return List of error messages.
     */
    static QStringList getValidationErrors(const QJsonObject& commandJSON);

    // **Schema access**

    /**
     * @brief Gets the schema for a Command.
     * @return JSON schema object.
     */
    static QJsonObject getCommandSchema();

    /**
     * @brief Gets the schema for a property.
     * @return JSON schema object.
     */
    static QJsonObject getPropertySchema();

    /**
     * @brief Gets the schema for an event.
     * @return JSON schema object.
     */
    static QJsonObject getEventSchema();

    /**
     * @brief Gets the schema for a binding.
     * @return JSON schema object.
     */
    static QJsonObject getBindingSchema();

    // **Validation rules**

    /**
     * @brief Validates a Command type.
     * @param type Name of the type.
     * @return True if valid.
     */
    static bool validateCommandType(const QString& type);

    /**
     * @brief Validates a property value.
     * @param property Name of the property.
     * @param value Value to validate.
     * @return True if valid.
     */
    static bool validatePropertyValue(const QString& property,
                                      const QJsonValue& value);

    /**
     * @brief Validates an event handler.
     * @param event Name of the event.
     * @param handler Handler value.
     * @return True if valid.
     */
    static bool validateEventHandler(const QString& event,
                                     const QJsonValue& handler);

    /**
     * @brief Validates a state binding.
     * @param binding JSON object for the binding.
     * @return True if valid.
     */
    static bool validateStateBinding(const QJsonObject& binding);

private:
    /**
     * @brief Validates a JSON object against a schema.
     * @param obj JSON object.
     * @param schema Schema object.
     * @param path Path for error reporting.
     * @return List of error messages.
     */
    static QStringList validateObject(const QJsonObject& obj,
                                      const QJsonObject& schema,
                                      const QString& path = "");

    /**
     * @brief Checks if a value matches the expected type.
     * @param value JSON value.
     * @param expectedType Expected type as string.
     * @return True if matches.
     */
    static bool matchesType(const QJsonValue& value,
                            const QString& expectedType);
};

/**
 * @class JSONMigrationUtility
 * @brief Utility for migrating legacy JSON UI definitions to Command format and
 * vice versa.
 */
class JSONMigrationUtility {
public:
    // **Migration**

    /**
     * @brief Migrates legacy JSON to Command JSON format.
     * @param legacyJSON Legacy JSON object.
     * @return Converted Command JSON.
     */
    static QJsonObject migrateToCommandFormat(const QJsonObject& legacyJSON);

    /**
     * @brief Migrates legacy children array to Command format.
     * @param legacyChildren Legacy children array.
     * @return Converted Command children array.
     */
    static QJsonArray migrateChildrenToCommandFormat(
        const QJsonArray& legacyChildren);

    /**
     * @brief Migrates Command JSON to legacy format.
     * @param commandJSON Command JSON object.
     * @return Converted legacy JSON.
     */
    static QJsonObject migrateToLegacyFormat(const QJsonObject& commandJSON);

    // **Migration analysis**

    /**
     * @brief Generates a migration report for a legacy JSON UI.
     * @param legacyJSON Legacy JSON object.
     * @return Migration report as a string.
     */
    static QString generateMigrationReport(const QJsonObject& legacyJSON);

    /**
     * @brief Gets a list of unsupported features in legacy JSON.
     * @param legacyJSON Legacy JSON object.
     * @return List of unsupported features.
     */
    static QStringList getUnsupportedFeatures(const QJsonObject& legacyJSON);

    /**
     * @brief Gets migration recommendations for legacy JSON.
     * @param legacyJSON Legacy JSON object.
     * @return List of recommendations.
     */
    static QStringList getRecommendations(const QJsonObject& legacyJSON);

    // **Batch migration**

    /**
     * @brief Migrates all files in a directory to Command format.
     * @param sourceDir Source directory.
     * @param targetDir Target directory.
     */
    static void migrateDirectory(const QString& sourceDir,
                                 const QString& targetDir);

    /**
     * @brief Migrates a single file to Command format.
     * @param sourceFile Source file path.
     * @param targetFile Target file path.
     */
    static void migrateFile(const QString& sourceFile,
                            const QString& targetFile);

private:
    static QJsonObject migrateComponent(const QJsonObject& component);
    static QJsonObject migrateProperties(const QJsonObject& properties);
    static QJsonObject migrateEvents(const QJsonObject& events);
    static QString mapComponentType(const QString& legacyType);
    static QString mapPropertyName(const QString& legacyProperty);
    static QString mapEventName(const QString& legacyEvent);
};

// **Template implementation**
/**
 * @brief Registers a Command factory for a type.
 * @tparam CommandType The Command type.
 * @param typeName Name of the type.
 */
template <typename CommandType>
void CommandComponentRegistry::registerCommandComponent(
    const QString& typeName) {
    static_assert(std::is_base_of_v<UI::BaseUICommand, CommandType>,
                  "CommandType must inherit from BaseUICommand");

    command_factories_[typeName] =
        [](const QJsonObject& config) -> std::shared_ptr<UI::BaseUICommand> {
        auto command = std::make_shared<CommandType>();

        // Apply configuration from JSON
        if (config.contains("properties")) {
            QJsonObject properties = config["properties"].toObject();
            for (auto it = properties.begin(); it != properties.end(); ++it) {
                command->getState()->setProperty(it.key(),
                                                 it.value().toVariant());
            }
        }

        return command;
    };
}

// **Convenience functions for JSON Command loading**
namespace JSON {

/**
 * @brief Loads a Command from a JSON file.
 * @param filePath Path to the JSON file.
 * @return Shared pointer to the loaded Command.
 */
inline std::shared_ptr<UI::BaseUICommand> loadCommand(const QString& filePath) {
    JSONCommandLoader loader;
    return loader.loadCommandFromFile(filePath);
}

/**
 * @brief Loads a Command from a JSON string.
 * @param jsonString JSON string.
 * @return Shared pointer to the loaded Command.
 */
inline std::shared_ptr<UI::BaseUICommand> loadCommandFromString(
    const QString& jsonString) {
    JSONCommandLoader loader;
    return loader.loadCommandFromString(jsonString);
}

/**
 * @brief Loads a Command and QWidget from a JSON file.
 * @param filePath Path to the JSON file.
 * @return Pair of Command and QWidget.
 */
inline std::pair<std::shared_ptr<UI::BaseUICommand>, std::unique_ptr<QWidget>>
loadCommandWithWidget(const QString& filePath) {
    JSONCommandLoader loader;
    return loader.loadCommandWithWidgetFromFile(filePath);
}

/**
 * @brief Migrates legacy JSON to Command JSON format.
 * @param legacyJSON Legacy JSON object.
 * @return Converted Command JSON.
 */
inline QJsonObject migrateToCommand(const QJsonObject& legacyJSON) {
    return JSONMigrationUtility::migrateToCommandFormat(legacyJSON);
}

/**
 * @brief Migrates Command JSON to legacy format.
 * @param commandJSON Command JSON object.
 * @return Converted legacy JSON.
 */
inline QJsonObject migrateToLegacy(const QJsonObject& commandJSON) {
    return JSONMigrationUtility::migrateToLegacyFormat(commandJSON);
}

/**
 * @brief Validates a Command JSON object.
 * @param commandJSON Command JSON object.
 * @return True if valid.
 */
inline bool validateCommand(const QJsonObject& commandJSON) {
    return CommandJSONValidator::validate(commandJSON);
}

/**
 * @brief Gets validation errors for a Command JSON object.
 * @param commandJSON Command JSON object.
 * @return List of error messages.
 */
inline QStringList getValidationErrors(const QJsonObject& commandJSON) {
    return CommandJSONValidator::getValidationErrors(commandJSON);
}

}  // namespace JSON

}  // namespace DeclarativeUI::Command::Adapters
