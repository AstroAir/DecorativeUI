#pragma once

/**
 * @file IntegrationManager.hpp
 * @brief Central integration manager for coordinating all Command system
 * adapters in DeclarativeUI.
 *
 * This manager provides a unified interface for integrating the Command system
 * with all existing components of the DeclarativeUI framework, ensuring
 * seamless interoperability, migration, and compatibility between legacy
 * Component-based and new Command-based architectures.
 */

#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QWidget>
#include <functional>
#include <memory>
#include <unordered_map>

#include "../../Core/UIElement.hpp"
#include "../UICommand.hpp"
#include "../UICommandFactory.hpp"
#include "ComponentSystemAdapter.hpp"
#include "JSONCommandLoader.hpp"
#include "StateManagerAdapter.hpp"
#include "UIElementAdapter.hpp"

namespace DeclarativeUI::Command::Adapters {

/**
 * @class IntegrationManager
 * @brief Central integration manager that coordinates all Command system
 * adapters.
 *
 * Provides a unified API for conversion, migration, loading, compatibility, and
 * statistics. Handles adapter initialization, compatibility modes, migration
 * utilities, validation, and monitoring.
 */
class IntegrationManager : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Constructs the IntegrationManager.
     * @param parent Parent QObject.
     */
    explicit IntegrationManager(QObject* parent = nullptr);

    /**
     * @brief Destructor.
     */
    virtual ~IntegrationManager() = default;

    // **Initialization and setup**

    /**
     * @brief Initializes all adapters and integration logic.
     */
    void initialize();

    /**
     * @brief Shuts down and cleans up all adapters.
     */
    void shutdown();

    /**
     * @brief Checks if the integration manager is initialized.
     * @return True if initialized, false otherwise.
     */
    bool isInitialized() const { return initialized_; }

    // **Adapter management**

    /**
     * @brief Gets the UIElementCommandAdapter instance.
     * @return Pointer to the UIElementCommandAdapter.
     */
    UIElementCommandAdapter* getUIElementAdapter() {
        return ui_element_adapter_.get();
    }

    /**
     * @brief Gets the JSONCommandLoader instance.
     * @return Pointer to the JSONCommandLoader.
     */
    JSONCommandLoader* getJSONLoader() { return json_loader_.get(); }

    /**
     * @brief Gets the CommandStateManagerAdapter instance.
     * @return Pointer to the CommandStateManagerAdapter.
     */
    CommandStateManagerAdapter* getStateAdapter() {
        return state_adapter_.get();
    }

    /**
     * @brief Gets the ComponentSystemAdapter instance.
     * @return Pointer to the ComponentSystemAdapter.
     */
    ComponentSystemAdapter* getComponentAdapter() {
        return component_adapter_.get();
    }

    // **Unified conversion interface**

    /**
     * @brief Converts a QObject (Component or UIElement) to a Command.
     * @param source Pointer to the source QObject.
     * @return Shared pointer to the resulting Command.
     */
    std::shared_ptr<UI::BaseUICommand> convertToCommand(QObject* source);

    /**
     * @brief Converts a Command to a QObject of the specified type.
     * @param command Shared pointer to the Command.
     * @param targetType Name of the target QObject type.
     * @return Unique pointer to the resulting QObject.
     */
    std::unique_ptr<QObject> convertFromCommand(
        std::shared_ptr<UI::BaseUICommand> command, const QString& targetType);

    // **Unified loading interface**

    /**
     * @brief Loads a Command from a file (JSON, UI, etc.).
     * @param filePath Path to the file.
     * @return Shared pointer to the loaded Command.
     */
    std::shared_ptr<UI::BaseUICommand> loadFromFile(const QString& filePath);

    /**
     * @brief Loads a Command from a JSON object.
     * @param json JSON object describing the command/UI.
     * @return Shared pointer to the loaded Command.
     */
    std::shared_ptr<UI::BaseUICommand> loadFromJSON(const QJsonObject& json);

    /**
     * @brief Loads a Command from a string (JSON or UI markup).
     * @param content String containing the UI description.
     * @return Shared pointer to the loaded Command.
     */
    std::shared_ptr<UI::BaseUICommand> loadFromString(const QString& content);

    // **Migration utilities**

    /**
     * @brief Migrates an entire project to the Command system.
     * @param projectPath Path to the project directory.
     */
    void migrateProject(const QString& projectPath);

    /**
     * @brief Migrates a single file to the Command system.
     * @param filePath Path to the source file.
     * @param outputPath Optional output path for the migrated file.
     */
    void migrateFile(const QString& filePath, const QString& outputPath = "");

    /**
     * @brief Generates a migration report for a project.
     * @param projectPath Path to the project directory.
     * @return String containing the migration report.
     */
    QString generateMigrationReport(const QString& projectPath);

    // **Compatibility modes**

    /**
     * @enum CompatibilityMode
     * @brief Supported compatibility modes for integration.
     */
    enum class CompatibilityMode {
        CommandOnly,    ///< Use only the Command system.
        ComponentOnly,  ///< Use only the legacy Component system.
        Hybrid,         ///< Use both systems with automatic conversion.
        Migration       ///< Gradual migration mode with compatibility layers.
    };

    /**
     * @brief Sets the current compatibility mode.
     * @param mode Compatibility mode to set.
     */
    void setCompatibilityMode(CompatibilityMode mode);

    /**
     * @brief Gets the current compatibility mode.
     * @return Current compatibility mode.
     */
    CompatibilityMode getCompatibilityMode() const {
        return compatibility_mode_;
    }

    // **Auto-registration and discovery**

    /**
     * @brief Registers all available adapters with the integration manager.
     */
    void registerAllAdapters();

    /**
     * @brief Discovers existing Components in the application.
     */
    void discoverExistingComponents();

    /**
     * @brief Sets up automatic conversion between Components and Commands.
     */
    void setupAutoConversion();

    // **Validation and testing**

    /**
     * @brief Validates the integration setup and configuration.
     * @return True if integration is valid, false otherwise.
     */
    bool validateIntegration();

    /**
     * @brief Gets a list of integration issues detected during validation.
     * @return List of issue descriptions.
     */
    QStringList getIntegrationIssues();

    /**
     * @brief Runs integration tests to verify interoperability.
     */
    void runIntegrationTests();

    // **Configuration**

    /**
     * @brief Enables or disables automatic state binding.
     * @param enabled True to enable, false to disable.
     */
    void setAutoStateBinding(bool enabled);

    /**
     * @brief Enables or disables automatic event handling.
     * @param enabled True to enable, false to disable.
     */
    void setAutoEventHandling(bool enabled);

    /**
     * @brief Enables or disables automatic widget mapping.
     * @param enabled True to enable, false to disable.
     */
    void setAutoWidgetMapping(bool enabled);

    /**
     * @brief Enables or disables batch updates for integration operations.
     * @param enabled True to enable, false to disable.
     */
    void setBatchUpdates(bool enabled);

    // **Statistics and monitoring**

    /**
     * @struct IntegrationStats
     * @brief Tracks statistics about integration operations.
     */
    struct IntegrationStats {
        int commands_created = 0;      ///< Number of commands created.
        int components_converted = 0;  ///< Number of components converted.
        int widgets_mapped = 0;        ///< Number of widgets mapped.
        int state_bindings = 0;  ///< Number of state bindings established.
        int event_handlers = 0;  ///< Number of event handlers connected.
        int json_loads = 0;      ///< Number of JSON loads performed.
        int migration_operations =
            0;  ///< Number of migration operations performed.
    };

    /**
     * @brief Gets the current integration statistics.
     * @return IntegrationStats structure.
     */
    IntegrationStats getStatistics() const { return stats_; }

    /**
     * @brief Resets all integration statistics to zero.
     */
    void resetStatistics();

    // **Singleton access**

    /**
     * @brief Gets the singleton instance of the IntegrationManager.
     * @return Reference to the singleton instance.
     */
    static IntegrationManager& instance();

signals:
    /**
     * @brief Emitted when integration is initialized.
     */
    void integrationInitialized();

    /**
     * @brief Emitted when integration is shut down.
     */
    void integrationShutdown();

    /**
     * @brief Emitted when the compatibility mode changes.
     * @param mode New compatibility mode.
     */
    void compatibilityModeChanged(CompatibilityMode mode);

    /**
     * @brief Emitted when a conversion is performed.
     * @param source Pointer to the source object.
     * @param result Shared pointer to the resulting Command.
     */
    void conversionPerformed(QObject* source,
                             std::shared_ptr<UI::BaseUICommand> result);

    /**
     * @brief Emitted when a migration operation starts.
     * @param path Path of the migration target.
     */
    void migrationStarted(const QString& path);

    /**
     * @brief Emitted when a migration operation completes.
     * @param path Path of the migration target.
     */
    void migrationCompleted(const QString& path);

    /**
     * @brief Emitted when an integration error occurs.
     * @param error Error message.
     */
    void integrationError(const QString& error);

private:
    // **Adapter instances**
    std::unique_ptr<UIElementCommandAdapter>
        ui_element_adapter_;  ///< Adapter for UIElement to Command.
    std::unique_ptr<JSONCommandLoader>
        json_loader_;  ///< Adapter for loading commands from JSON.
    std::unique_ptr<CommandStateManagerAdapter>
        state_adapter_;  ///< Adapter for state management.
    std::unique_ptr<ComponentSystemAdapter>
        component_adapter_;  ///< Adapter for legacy Components.

    // **Configuration**
    bool initialized_ = false;  ///< Whether the manager is initialized.
    CompatibilityMode compatibility_mode_ =
        CompatibilityMode::Hybrid;     ///< Current compatibility mode.
    bool auto_state_binding_ = true;   ///< Automatic state binding enabled.
    bool auto_event_handling_ = true;  ///< Automatic event handling enabled.
    bool auto_widget_mapping_ = true;  ///< Automatic widget mapping enabled.
    bool batch_updates_ = true;        ///< Batch updates enabled.

    // **Statistics**
    IntegrationStats stats_;  ///< Integration statistics.

    // **Type registry for automatic conversion**
    std::unordered_map<
        QString, std::function<std::shared_ptr<UI::BaseUICommand>(QObject*)>>
        type_converters_;  ///< Type converters.
    std::unordered_map<QString, QString>
        type_mappings_;  ///< Type mappings for conversion.

    // **Helper methods**

    /**
     * @brief Initializes all adapter instances.
     */
    void initializeAdapters();

    /**
     * @brief Sets up type converters for automatic conversion.
     */
    void setupTypeConverters();

    /**
     * @brief Sets up default type mappings for conversion.
     */
    void setupDefaultMappings();

    /**
     * @brief Registers built-in converters.
     */
    void registerBuiltinConverters();

    /**
     * @brief Connects adapter signals to the integration manager.
     */
    void connectAdapterSignals();

    /**
     * @brief Handles a conversion request for a source object.
     * @param source Pointer to the source object.
     */
    void handleConversionRequest(QObject* source);

    /**
     * @brief Handles a migration request for a given path.
     * @param path Path to migrate.
     */
    void handleMigrationRequest(const QString& path);

    /**
     * @brief Updates integration statistics for a given operation.
     * @param operation Name of the operation.
     */
    void updateStatistics(const QString& operation);

    /**
     * @brief Detects the type of a source QObject for conversion.
     * @param source Pointer to the source object.
     * @return Type name as a string.
     */
    QString detectSourceType(QObject* source);

    /**
     * @brief Detects the file type for loading.
     * @param filePath Path to the file.
     * @return File type as a string.
     */
    QString detectFileType(const QString& filePath);

private slots:
    /**
     * @brief Slot called when an adapter reports an error.
     * @param error Error message.
     */
    void onAdapterError(const QString& error);

    /**
     * @brief Slot called when a conversion is completed.
     * @param source Pointer to the source object.
     * @param result Shared pointer to the resulting Command.
     */
    void onConversionCompleted(QObject* source,
                               std::shared_ptr<UI::BaseUICommand> result);
};

/**
 * @class CompatibilityLayer
 * @brief Compatibility layer that provides transparent access to both old and
 * new systems.
 *
 * Offers unified factory, property, event, state, and layout management APIs
 * for both Component and Command systems.
 */
class CompatibilityLayer {
public:
    // **Unified factory interface**

    /**
     * @brief Creates an instance of the specified type, using Command or
     * Component system as appropriate.
     * @tparam T The desired type.
     * @param type Name of the type to create.
     * @return Shared pointer to the created object, or nullptr if not found.
     */
    template <typename T>
    static std::shared_ptr<T> create(const QString& type);

    /**
     * @brief Creates an instance from a JSON configuration.
     * @tparam T The desired type.
     * @param config JSON object describing the type.
     * @return Shared pointer to the created object, or nullptr if not found.
     */
    template <typename T>
    static std::shared_ptr<T> createFromJSON(const QJsonObject& config);

    // **Unified property access**

    /**
     * @brief Sets a property on a QObject, regardless of system.
     * @param object Pointer to the object.
     * @param property Name of the property.
     * @param value Value to set.
     */
    static void setProperty(QObject* object, const QString& property,
                            const QVariant& value);

    /**
     * @brief Gets a property value from a QObject, regardless of system.
     * @param object Pointer to the object.
     * @param property Name of the property.
     * @return Value of the property.
     */
    static QVariant getProperty(QObject* object, const QString& property);

    // **Unified event handling**

    /**
     * @brief Connects an event to a handler function.
     * @param source Pointer to the source object.
     * @param event Name of the event.
     * @param handler Handler function to invoke.
     */
    static void connectEvent(QObject* source, const QString& event,
                             std::function<void()> handler);

    /**
     * @brief Disconnects an event handler.
     * @param source Pointer to the source object.
     * @param event Name of the event.
     */
    static void disconnectEvent(QObject* source, const QString& event);

    // **Unified state management**

    /**
     * @brief Binds a QObject property to a state variable.
     * @param object Pointer to the object.
     * @param stateKey State variable key.
     * @param property Name of the property to bind (default: "value").
     */
    static void bindToState(QObject* object, const QString& stateKey,
                            const QString& property = "value");

    /**
     * @brief Unbinds a QObject from a state variable.
     * @param object Pointer to the object.
     * @param stateKey State variable key (optional).
     */
    static void unbindFromState(QObject* object, const QString& stateKey = "");

    // **Unified layout management**

    /**
     * @brief Adds a child object to a container's layout.
     * @param container Pointer to the container.
     * @param child Pointer to the child object.
     */
    static void addToLayout(QObject* container, QObject* child);

    /**
     * @brief Removes a child object from a container's layout.
     * @param container Pointer to the container.
     * @param child Pointer to the child object.
     */
    static void removeFromLayout(QObject* container, QObject* child);

    /**
     * @brief Sets the layout type for a container.
     * @param container Pointer to the container.
     * @param layoutType Name of the layout type.
     */
    static void setLayoutType(QObject* container, const QString& layoutType);

private:
    /**
     * @brief Checks if the object is a Command.
     * @param object Pointer to the object.
     * @return True if the object is a Command.
     */
    static bool isCommand(QObject* object);

    /**
     * @brief Checks if the object is a Component.
     * @param object Pointer to the object.
     * @return True if the object is a Component.
     */
    static bool isComponent(QObject* object);

    /**
     * @brief Checks if the object is a UIElement.
     * @param object Pointer to the object.
     * @return True if the object is a UIElement.
     */
    static bool isUIElement(QObject* object);

    /**
     * @brief Casts a QObject to a Command pointer.
     * @param object Pointer to the object.
     * @return Shared pointer to the Command.
     */
    static std::shared_ptr<UI::BaseUICommand> asCommand(QObject* object);

    /**
     * @brief Casts a QObject to a UIElement pointer.
     * @param object Pointer to the object.
     * @return Pointer to the UIElement.
     */
    static Core::UIElement* asUIElement(QObject* object);

    /**
     * @brief Casts a QObject to a QWidget pointer.
     * @param object Pointer to the object.
     * @return Pointer to the QWidget.
     */
    static QWidget* asWidget(QObject* object);
};

/**
 * @class MigrationAssistant
 * @brief Migration assistant for converting existing projects to use the
 * Command system.
 *
 * Provides project analysis, migration planning, execution, backup, rollback,
 * and validation utilities.
 */
class MigrationAssistant {
public:
    // **Project analysis**

    /**
     * @struct ProjectAnalysis
     * @brief Holds analysis results for a project.
     */
    struct ProjectAnalysis {
        QString project_path;              ///< Path to the project.
        int total_files = 0;               ///< Total number of files.
        int ui_files = 0;                  ///< Number of UI files.
        int component_files = 0;           ///< Number of Component files.
        int json_files = 0;                ///< Number of JSON files.
        QStringList unsupported_features;  ///< List of unsupported features.
        QStringList recommended_changes;   ///< List of recommended changes.
        QString complexity_level;          ///< Project complexity ("Simple",
                                           ///< "Moderate", "Complex").
        int estimated_effort_hours =
            0;  ///< Estimated migration effort in hours.
    };

    /**
     * @brief Analyzes a project for migration.
     * @param projectPath Path to the project.
     * @return ProjectAnalysis structure.
     */
    static ProjectAnalysis analyzeProject(const QString& projectPath);

    // **Migration planning**

    /**
     * @struct MigrationPlan
     * @brief Holds a migration plan for a project.
     */
    struct MigrationPlan {
        ProjectAnalysis analysis;             ///< Analysis of the project.
        QStringList migration_steps;          ///< Steps for migration.
        QStringList file_conversion_order;    ///< Order of file conversion.
        QStringList backup_recommendations;   ///< Backup recommendations.
        QStringList testing_recommendations;  ///< Testing recommendations.
        QString estimated_timeline;           ///< Estimated migration timeline.
    };

    /**
     * @brief Creates a migration plan from a project analysis.
     * @param analysis ProjectAnalysis structure.
     * @return MigrationPlan structure.
     */
    static MigrationPlan createMigrationPlan(const ProjectAnalysis& analysis);

    // **Migration execution**

    /**
     * @brief Executes a migration plan.
     * @param plan MigrationPlan structure.
     * @param progressCallback Optional callback for progress updates.
     * @return True if migration succeeded, false otherwise.
     */
    static bool executeMigrationPlan(
        const MigrationPlan& plan,
        std::function<void(const QString&)> progressCallback = nullptr);

    // **Rollback support**

    /**
     * @brief Creates a backup of the project before migration.
     * @param projectPath Path to the project.
     * @param backupPath Path to the backup location.
     * @return True if backup succeeded, false otherwise.
     */
    static bool createBackup(const QString& projectPath,
                             const QString& backupPath);

    /**
     * @brief Rolls back a migration using a backup.
     * @param projectPath Path to the project.
     * @param backupPath Path to the backup.
     * @return True if rollback succeeded, false otherwise.
     */
    static bool rollbackMigration(const QString& projectPath,
                                  const QString& backupPath);

    // **Validation**

    /**
     * @brief Validates the migration of a project.
     * @param projectPath Path to the project.
     * @return True if migration is valid, false otherwise.
     */
    static bool validateMigration(const QString& projectPath);

    /**
     * @brief Gets a list of migration issues for a project.
     * @param projectPath Path to the project.
     * @return List of issue descriptions.
     */
    static QStringList getMigrationIssues(const QString& projectPath);

private:
    static void analyzeFile(const QString& filePath, ProjectAnalysis& analysis);
    static void convertFile(const QString& filePath, const QString& outputPath);
    static QString assessComplexity(const ProjectAnalysis& analysis);
    static int estimateEffort(const ProjectAnalysis& analysis);
};

/**
 * @class IntegrationPerformanceMonitor
 * @brief Performance monitor for tracking integration overhead and metrics.
 *
 * Provides metrics for conversion, creation, synchronization, and memory usage.
 * Supports benchmarking and memory tracking for integration operations.
 */
class IntegrationPerformanceMonitor {
public:
    // **Performance metrics**

    /**
     * @struct PerformanceMetrics
     * @brief Holds performance metrics for integration operations.
     */
    struct PerformanceMetrics {
        double avg_conversion_time_ms =
            0.0;                            ///< Average conversion time in ms.
        double avg_creation_time_ms = 0.0;  ///< Average creation time in ms.
        double avg_sync_time_ms = 0.0;  ///< Average synchronization time in ms.
        int total_conversions = 0;      ///< Total number of conversions.
        int total_creations = 0;        ///< Total number of creations.
        int total_syncs = 0;            ///< Total number of synchronizations.
        double memory_overhead_mb = 0.0;  ///< Memory overhead in MB.
    };

    /**
     * @brief Starts performance monitoring.
     */
    static void startMonitoring();

    /**
     * @brief Stops performance monitoring.
     */
    static void stopMonitoring();

    /**
     * @brief Gets the current performance metrics.
     * @return PerformanceMetrics structure.
     */
    static PerformanceMetrics getMetrics();

    /**
     * @brief Resets all performance metrics.
     */
    static void resetMetrics();

    // **Benchmarking**

    /**
     * @brief Benchmarks conversion performance for a given source type.
     * @param sourceType Name of the source type.
     * @param iterations Number of iterations to run.
     */
    static void benchmarkConversion(const QString& sourceType,
                                    int iterations = 1000);

    /**
     * @brief Benchmarks creation performance for a given command type.
     * @param commandType Name of the command type.
     * @param iterations Number of iterations to run.
     */
    static void benchmarkCreation(const QString& commandType,
                                  int iterations = 1000);

    /**
     * @brief Benchmarks synchronization performance.
     * @param iterations Number of iterations to run.
     */
    static void benchmarkSync(int iterations = 1000);

    // **Memory tracking**

    /**
     * @brief Tracks memory usage for integration operations.
     */
    static void trackMemoryUsage();

    /**
     * @brief Gets the current memory overhead in MB.
     * @return Memory overhead in MB.
     */
    static double getMemoryOverhead();

private:
    static PerformanceMetrics metrics_;
    static bool monitoring_enabled_;
    static QElapsedTimer timer_;
};

// **Template implementations**

/**
 * @brief Creates an instance of the specified type, using Command or Component
 * system as appropriate.
 * @tparam T The desired type.
 * @param type Name of the type to create.
 * @return Shared pointer to the created object, or nullptr if not found.
 */
template <typename T>
std::shared_ptr<T> CompatibilityLayer::create(const QString& type) {
    // Try Command system first
    if (UI::UICommandFactory::instance().isRegistered(type)) {
        auto command = UI::UICommandFactory::instance().createCommand(type);
        return std::dynamic_pointer_cast<T>(command);
    }
    // Fall back to Component system
    // This would need integration with the existing component creation system
    return nullptr;
}

/**
 * @brief Creates an instance from a JSON configuration.
 * @tparam T The desired type.
 * @param config JSON object describing the type.
 * @return Shared pointer to the created object, or nullptr if not found.
 */
template <typename T>
std::shared_ptr<T> CompatibilityLayer::createFromJSON(
    const QJsonObject& config) {
    if (!config.contains("type")) {
        return nullptr;
    }
    QString type = config["type"].toString();
    return create<T>(type);
}

// **Global convenience functions**
namespace Integration {

/**
 * @brief Gets a reference to the global IntegrationManager singleton.
 * @return Reference to the IntegrationManager.
 */
inline IntegrationManager& manager() { return IntegrationManager::instance(); }

/**
 * @brief Converts a QObject to a Command using the integration manager.
 * @param object Pointer to the object.
 * @return Shared pointer to the Command.
 */
inline std::shared_ptr<UI::BaseUICommand> toCommand(QObject* object) {
    return IntegrationManager::instance().convertToCommand(object);
}

/**
 * @brief Converts a Command to a QObject of the specified type.
 * @param command Shared pointer to the Command.
 * @param type Name of the target type.
 * @return Unique pointer to the QObject.
 */
inline std::unique_ptr<QObject> fromCommand(
    std::shared_ptr<UI::BaseUICommand> command, const QString& type) {
    return IntegrationManager::instance().convertFromCommand(command, type);
}

/**
 * @brief Loads a Command from a file.
 * @param path Path to the file.
 * @return Shared pointer to the loaded Command.
 */
inline std::shared_ptr<UI::BaseUICommand> load(const QString& path) {
    return IntegrationManager::instance().loadFromFile(path);
}

/**
 * @brief Migrates a project to the Command system.
 * @param projectPath Path to the project.
 */
inline void migrate(const QString& projectPath) {
    IntegrationManager::instance().migrateProject(projectPath);
}

/**
 * @brief Analyzes a project and returns a migration report.
 * @param projectPath Path to the project.
 * @return Migration report as a string.
 */
inline QString analyze(const QString& projectPath) {
    return IntegrationManager::instance().generateMigrationReport(projectPath);
}

/**
 * @brief Sets the compatibility mode for integration.
 * @param mode Compatibility mode to set.
 */
inline void setMode(IntegrationManager::CompatibilityMode mode) {
    IntegrationManager::instance().setCompatibilityMode(mode);
}

}  // namespace Integration

}  // namespace DeclarativeUI::Command::Adapters
