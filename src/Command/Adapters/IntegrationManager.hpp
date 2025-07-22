#pragma once

#include <QObject>
#include <QString>
#include <QJsonObject>
#include <QWidget>
#include <memory>
#include <functional>
#include <unordered_map>
#include <vector>

#include "../UICommand.hpp"
#include "../UICommandFactory.hpp"
#include "../CommandBuilder.hpp"
#include "UIElementAdapter.hpp"
#include "JSONCommandLoader.hpp"
#include "StateManagerAdapter.hpp"
#include "ComponentSystemAdapter.hpp"
#include "../../Core/UIElement.hpp"
#include "../../JSON/JSONUILoader.hpp"
#include "../../Binding/StateManager.hpp"
#include "../../Components/Button.hpp"

namespace DeclarativeUI::Command::Adapters {

/**
 * @brief Central integration manager that coordinates all Command system adapters
 * 
 * This manager provides a unified interface for integrating the Command system
 * with all existing components of the DeclarativeUI framework, ensuring seamless
 * interoperability and migration capabilities.
 */
class IntegrationManager : public QObject {
    Q_OBJECT
    
public:
    explicit IntegrationManager(QObject* parent = nullptr);
    virtual ~IntegrationManager() = default;
    
    // **Initialization and setup**
    void initialize();
    void shutdown();
    bool isInitialized() const { return initialized_; }
    
    // **Adapter management**
    UIElementCommandAdapter* getUIElementAdapter() { return ui_element_adapter_.get(); }
    JSONCommandLoader* getJSONLoader() { return json_loader_.get(); }
    CommandStateManagerAdapter* getStateAdapter() { return state_adapter_.get(); }
    ComponentSystemAdapter* getComponentAdapter() { return component_adapter_.get(); }
    
    // **Unified conversion interface**
    std::shared_ptr<UI::BaseUICommand> convertToCommand(QObject* source);
    std::unique_ptr<QObject> convertFromCommand(std::shared_ptr<UI::BaseUICommand> command, const QString& targetType);
    
    // **Unified loading interface**
    std::shared_ptr<UI::BaseUICommand> loadFromFile(const QString& filePath);
    std::shared_ptr<UI::BaseUICommand> loadFromJSON(const QJsonObject& json);
    std::shared_ptr<UI::BaseUICommand> loadFromString(const QString& content);
    
    // **Migration utilities**
    void migrateProject(const QString& projectPath);
    void migrateFile(const QString& filePath, const QString& outputPath = "");
    QString generateMigrationReport(const QString& projectPath);
    
    // **Compatibility modes**
    enum class CompatibilityMode {
        CommandOnly,      // Use only Command system
        ComponentOnly,    // Use only existing Component system
        Hybrid,          // Use both systems with automatic conversion
        Migration        // Gradual migration mode with compatibility layers
    };
    
    void setCompatibilityMode(CompatibilityMode mode);
    CompatibilityMode getCompatibilityMode() const { return compatibility_mode_; }
    
    // **Auto-registration and discovery**
    void registerAllAdapters();
    void discoverExistingComponents();
    void setupAutoConversion();
    
    // **Validation and testing**
    bool validateIntegration();
    QStringList getIntegrationIssues();
    void runIntegrationTests();
    
    // **Configuration**
    void setAutoStateBinding(bool enabled);
    void setAutoEventHandling(bool enabled);
    void setAutoWidgetMapping(bool enabled);
    void setBatchUpdates(bool enabled);
    
    // **Statistics and monitoring**
    struct IntegrationStats {
        int commands_created = 0;
        int components_converted = 0;
        int widgets_mapped = 0;
        int state_bindings = 0;
        int event_handlers = 0;
        int json_loads = 0;
        int migration_operations = 0;
    };
    
    IntegrationStats getStatistics() const { return stats_; }
    void resetStatistics();
    
    // **Singleton access**
    static IntegrationManager& instance();
    
signals:
    void integrationInitialized();
    void integrationShutdown();
    void compatibilityModeChanged(CompatibilityMode mode);
    void conversionPerformed(QObject* source, std::shared_ptr<UI::BaseUICommand> result);
    void migrationStarted(const QString& path);
    void migrationCompleted(const QString& path);
    void integrationError(const QString& error);
    
private:
    // **Adapter instances**
    std::unique_ptr<UIElementCommandAdapter> ui_element_adapter_;
    std::unique_ptr<JSONCommandLoader> json_loader_;
    std::unique_ptr<CommandStateManagerAdapter> state_adapter_;
    std::unique_ptr<ComponentSystemAdapter> component_adapter_;
    
    // **Configuration**
    bool initialized_ = false;
    CompatibilityMode compatibility_mode_ = CompatibilityMode::Hybrid;
    bool auto_state_binding_ = true;
    bool auto_event_handling_ = true;
    bool auto_widget_mapping_ = true;
    bool batch_updates_ = true;
    
    // **Statistics**
    IntegrationStats stats_;
    
    // **Type registry for automatic conversion**
    std::unordered_map<QString, std::function<std::shared_ptr<UI::BaseUICommand>(QObject*)>> type_converters_;
    std::unordered_map<QString, QString> type_mappings_;
    
    // **Helper methods**
    void initializeAdapters();
    void setupTypeConverters();
    void setupDefaultMappings();
    void registerBuiltinConverters();
    void connectAdapterSignals();
    
    void handleConversionRequest(QObject* source);
    void handleMigrationRequest(const QString& path);
    void updateStatistics(const QString& operation);
    
    QString detectSourceType(QObject* source);
    QString detectFileType(const QString& filePath);
    
private slots:
    void onAdapterError(const QString& error);
    void onConversionCompleted(QObject* source, std::shared_ptr<UI::BaseUICommand> result);
};

/**
 * @brief Compatibility layer that provides transparent access to both old and new systems
 */
class CompatibilityLayer {
public:
    // **Unified factory interface**
    template<typename T>
    static std::shared_ptr<T> create(const QString& type);
    
    template<typename T>
    static std::shared_ptr<T> createFromJSON(const QJsonObject& config);
    
    // **Unified property access**
    static void setProperty(QObject* object, const QString& property, const QVariant& value);
    static QVariant getProperty(QObject* object, const QString& property);
    
    // **Unified event handling**
    static void connectEvent(QObject* source, const QString& event, std::function<void()> handler);
    static void disconnectEvent(QObject* source, const QString& event);
    
    // **Unified state management**
    static void bindToState(QObject* object, const QString& stateKey, const QString& property = "value");
    static void unbindFromState(QObject* object, const QString& stateKey = "");
    
    // **Unified layout management**
    static void addToLayout(QObject* container, QObject* child);
    static void removeFromLayout(QObject* container, QObject* child);
    static void setLayoutType(QObject* container, const QString& layoutType);
    
private:
    static bool isCommand(QObject* object);
    static bool isComponent(QObject* object);
    static bool isUIElement(QObject* object);
    
    static std::shared_ptr<UI::BaseUICommand> asCommand(QObject* object);
    static Core::UIElement* asUIElement(QObject* object);
    static QWidget* asWidget(QObject* object);
};

/**
 * @brief Migration assistant for converting existing projects to use the Command system
 */
class MigrationAssistant {
public:
    // **Project analysis**
    struct ProjectAnalysis {
        QString project_path;
        int total_files = 0;
        int ui_files = 0;
        int component_files = 0;
        int json_files = 0;
        QStringList unsupported_features;
        QStringList recommended_changes;
        QString complexity_level;  // "Simple", "Moderate", "Complex"
        int estimated_effort_hours = 0;
    };
    
    static ProjectAnalysis analyzeProject(const QString& projectPath);
    
    // **Migration planning**
    struct MigrationPlan {
        ProjectAnalysis analysis;
        QStringList migration_steps;
        QStringList file_conversion_order;
        QStringList backup_recommendations;
        QStringList testing_recommendations;
        QString estimated_timeline;
    };
    
    static MigrationPlan createMigrationPlan(const ProjectAnalysis& analysis);
    
    // **Migration execution**
    static bool executeMigrationPlan(const MigrationPlan& plan, 
                                   std::function<void(const QString&)> progressCallback = nullptr);
    
    // **Rollback support**
    static bool createBackup(const QString& projectPath, const QString& backupPath);
    static bool rollbackMigration(const QString& projectPath, const QString& backupPath);
    
    // **Validation**
    static bool validateMigration(const QString& projectPath);
    static QStringList getMigrationIssues(const QString& projectPath);
    
private:
    static void analyzeFile(const QString& filePath, ProjectAnalysis& analysis);
    static void convertFile(const QString& filePath, const QString& outputPath);
    static QString assessComplexity(const ProjectAnalysis& analysis);
    static int estimateEffort(const ProjectAnalysis& analysis);
};

/**
 * @brief Performance monitor for tracking integration overhead
 */
class IntegrationPerformanceMonitor {
public:
    // **Performance metrics**
    struct PerformanceMetrics {
        double avg_conversion_time_ms = 0.0;
        double avg_creation_time_ms = 0.0;
        double avg_sync_time_ms = 0.0;
        int total_conversions = 0;
        int total_creations = 0;
        int total_syncs = 0;
        double memory_overhead_mb = 0.0;
    };
    
    static void startMonitoring();
    static void stopMonitoring();
    static PerformanceMetrics getMetrics();
    static void resetMetrics();
    
    // **Benchmarking**
    static void benchmarkConversion(const QString& sourceType, int iterations = 1000);
    static void benchmarkCreation(const QString& commandType, int iterations = 1000);
    static void benchmarkSync(int iterations = 1000);
    
    // **Memory tracking**
    static void trackMemoryUsage();
    static double getMemoryOverhead();
    
private:
    static PerformanceMetrics metrics_;
    static bool monitoring_enabled_;
    static QElapsedTimer timer_;
};

// **Template implementations**
template<typename T>
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

template<typename T>
std::shared_ptr<T> CompatibilityLayer::createFromJSON(const QJsonObject& config) {
    if (!config.contains("type")) {
        return nullptr;
    }
    
    QString type = config["type"].toString();
    return create<T>(type);
}

// **Global convenience functions**
namespace Integration {

// **Quick access to integration manager**
inline IntegrationManager& manager() {
    return IntegrationManager::instance();
}

// **Quick conversion functions**
inline std::shared_ptr<UI::BaseUICommand> toCommand(QObject* object) {
    return IntegrationManager::instance().convertToCommand(object);
}

inline std::unique_ptr<QObject> fromCommand(std::shared_ptr<UI::BaseUICommand> command, const QString& type) {
    return IntegrationManager::instance().convertFromCommand(command, type);
}

// **Quick loading functions**
inline std::shared_ptr<UI::BaseUICommand> load(const QString& path) {
    return IntegrationManager::instance().loadFromFile(path);
}

// **Migration helpers**
inline void migrate(const QString& projectPath) {
    IntegrationManager::instance().migrateProject(projectPath);
}

inline QString analyze(const QString& projectPath) {
    return IntegrationManager::instance().generateMigrationReport(projectPath);
}

// **Compatibility mode switching**
inline void setMode(IntegrationManager::CompatibilityMode mode) {
    IntegrationManager::instance().setCompatibilityMode(mode);
}

}  // namespace Integration

}  // namespace DeclarativeUI::Command::Adapters
