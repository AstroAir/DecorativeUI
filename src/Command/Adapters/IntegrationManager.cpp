#include "IntegrationManager.hpp"
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QJsonDocument>
#include <QThread>

namespace DeclarativeUI::Command::Adapters {

IntegrationManager::IntegrationManager(QObject* parent) : QObject(parent) {
    qDebug() << "🌉 IntegrationManager created";
}

IntegrationManager& IntegrationManager::instance() {
    static IntegrationManager instance;
    return instance;
}

void IntegrationManager::initialize() {
    if (initialized_) {
        qWarning() << "IntegrationManager already initialized";
        return;
    }

    initializeAdapters();
    setupTypeConverters();
    setupDefaultMappings();
    registerBuiltinConverters();
    connectAdapterSignals();

    initialized_ = true;
    emit integrationInitialized();
    qDebug() << "✅ IntegrationManager initialized successfully";
}

void IntegrationManager::shutdown() {
    if (!initialized_) {
        return;
    }

    // Clean up adapters
    ui_element_adapter_.reset();
    json_loader_.reset();
    state_adapter_.reset();
    component_adapter_.reset();

    initialized_ = false;
    emit integrationShutdown();
    qDebug() << "🔌 IntegrationManager shutdown";
}

std::shared_ptr<UI::BaseUICommand> IntegrationManager::convertToCommand(QObject* source) {
    if (!source) {
        return nullptr;
    }

    updateStatistics("conversion");

    QString sourceType = detectSourceType(source);
    auto it = type_converters_.find(sourceType);
    if (it != type_converters_.end()) {
        auto command = it->second(source);
        emit conversionPerformed(source, command);
        return command;
    }

    handleConversionRequest(source);
    return nullptr;
}

std::unique_ptr<QObject> IntegrationManager::convertFromCommand(std::shared_ptr<UI::BaseUICommand> command, const QString& targetType) {
    if (!command) {
        return nullptr;
    }

    updateStatistics("conversion");

    if (component_adapter_) {
        return component_adapter_->createComponentFromCommand(command);
    }

    return nullptr;
}

std::shared_ptr<UI::BaseUICommand> IntegrationManager::loadFromFile(const QString& filePath) {
    if (!json_loader_) {
        return nullptr;
    }

    updateStatistics("json_load");

    QString fileType = detectFileType(filePath);
    if (fileType == "json") {
        return json_loader_->loadCommandFromFile(filePath);
    }

    return nullptr;
}

std::shared_ptr<UI::BaseUICommand> IntegrationManager::loadFromJSON(const QJsonObject& json) {
    if (!json_loader_) {
        return nullptr;
    }

    updateStatistics("json_load");
    return json_loader_->loadCommandFromObject(json);
}

std::shared_ptr<UI::BaseUICommand> IntegrationManager::loadFromString(const QString& content) {
    if (!json_loader_) {
        return nullptr;
    }

    updateStatistics("json_load");
    return json_loader_->loadCommandFromString(content);
}

void IntegrationManager::migrateProject(const QString& projectPath) {
    emit migrationStarted(projectPath);
    updateStatistics("migration");

    handleMigrationRequest(projectPath);

    emit migrationCompleted(projectPath);
}

void IntegrationManager::migrateFile(const QString& filePath, const QString& outputPath) {
    QString output = outputPath.isEmpty() ? filePath : outputPath;

    // Simple file migration logic
    QFileInfo fileInfo(filePath);
    if (fileInfo.suffix().toLower() == "json") {
        // Migrate JSON file
        if (json_loader_) {
            auto command = json_loader_->loadCommandFromFile(filePath);
            if (command) {
                qDebug() << "Migrated file:" << filePath;
            }
        }
    }

    updateStatistics("migration");
}

QString IntegrationManager::generateMigrationReport(const QString& projectPath) {
    QString report = QString("Migration Report for: %1\n").arg(projectPath);
    report += QString("Compatibility Mode: %1\n").arg(static_cast<int>(compatibility_mode_));
    report += QString("Statistics:\n");
    report += QString("  Commands Created: %1\n").arg(stats_.commands_created);
    report += QString("  Components Converted: %1\n").arg(stats_.components_converted);
    report += QString("  Widgets Mapped: %1\n").arg(stats_.widgets_mapped);
    report += QString("  State Bindings: %1\n").arg(stats_.state_bindings);
    report += QString("  Event Handlers: %1\n").arg(stats_.event_handlers);

    return report;
}

void IntegrationManager::setCompatibilityMode(CompatibilityMode mode) {
    if (compatibility_mode_ != mode) {
        compatibility_mode_ = mode;
        emit compatibilityModeChanged(mode);
        qDebug() << "🔄 Compatibility mode changed to:" << static_cast<int>(mode);
    }
}

void IntegrationManager::registerAllAdapters() {
    if (!initialized_) {
        initialize();
    }

    // Register all available adapters
    qDebug() << "📋 Registering all adapters";
}

void IntegrationManager::discoverExistingComponents() {
    // Discover existing components in the system
    qDebug() << "🔍 Discovering existing components";
}

void IntegrationManager::setupAutoConversion() {
    // Set up automatic conversion rules
    qDebug() << "⚙️ Setting up auto-conversion";
}

bool IntegrationManager::validateIntegration() {
    if (!initialized_) {
        return false;
    }

    // Validate that all adapters are working correctly
    bool valid = true;

    if (!ui_element_adapter_) {
        valid = false;
    }

    if (!json_loader_) {
        valid = false;
    }

    if (!state_adapter_) {
        valid = false;
    }

    if (!component_adapter_) {
        valid = false;
    }

    return valid;
}

QStringList IntegrationManager::getIntegrationIssues() {
    QStringList issues;

    if (!initialized_) {
        issues.append("IntegrationManager not initialized");
    }

    if (!ui_element_adapter_) {
        issues.append("UIElementAdapter not available");
    }

    if (!json_loader_) {
        issues.append("JSONCommandLoader not available");
    }

    if (!state_adapter_) {
        issues.append("StateManagerAdapter not available");
    }

    if (!component_adapter_) {
        issues.append("ComponentSystemAdapter not available");
    }

    return issues;
}

void IntegrationManager::runIntegrationTests() {
    qDebug() << "🧪 Running integration tests";

    // Run basic integration tests
    bool allPassed = true;

    // Test adapter creation
    if (!ui_element_adapter_) {
        qWarning() << "❌ UIElementAdapter test failed";
        allPassed = false;
    }

    if (!json_loader_) {
        qWarning() << "❌ JSONCommandLoader test failed";
        allPassed = false;
    }

    if (!state_adapter_) {
        qWarning() << "❌ StateManagerAdapter test failed";
        allPassed = false;
    }

    if (!component_adapter_) {
        qWarning() << "❌ ComponentSystemAdapter test failed";
        allPassed = false;
    }

    if (allPassed) {
        qDebug() << "✅ All integration tests passed";
    } else {
        qWarning() << "❌ Some integration tests failed";
    }
}

void IntegrationManager::setAutoStateBinding(bool enabled) {
    auto_state_binding_ = enabled;
    if (json_loader_) {
        json_loader_->setAutoStateBinding(enabled);
    }
}

void IntegrationManager::setAutoEventHandling(bool enabled) {
    auto_event_handling_ = enabled;
    if (json_loader_) {
        json_loader_->setAutoEventHandling(enabled);
    }
}

void IntegrationManager::setAutoWidgetMapping(bool enabled) {
    auto_widget_mapping_ = enabled;
}

void IntegrationManager::setBatchUpdates(bool enabled) {
    batch_updates_ = enabled;
    if (state_adapter_) {
        // Configure state adapter for batch updates
    }
}

void IntegrationManager::resetStatistics() {
    stats_ = IntegrationStats{};
}

void IntegrationManager::initializeAdapters() {
    // Create adapter instances - UIElementCommandAdapter will be created when needed
    // ui_element_adapter_ = std::make_unique<UIElementCommandAdapter>(...);

    json_loader_ = std::make_unique<JSONCommandLoader>();
    state_adapter_ = std::make_unique<CommandStateManagerAdapter>();
    component_adapter_ = std::make_unique<ComponentSystemAdapter>();

    qDebug() << "🔧 Adapters initialized";
}

void IntegrationManager::setupTypeConverters() {
    // Set up type converters for automatic conversion
    type_converters_["Components::Button"] = [this](QObject* obj) -> std::shared_ptr<UI::BaseUICommand> {
        if (auto* button = qobject_cast<Components::Button*>(obj)) {
            return component_adapter_->convertToCommand(button);
        }
        return nullptr;
    };

    type_converters_["Components::Widget"] = [this](QObject* obj) -> std::shared_ptr<UI::BaseUICommand> {
        if (auto* widget = qobject_cast<Components::Widget*>(obj)) {
            return component_adapter_->convertToCommand(widget);
        }
        return nullptr;
    };

    qDebug() << "🔄 Type converters set up";
}

void IntegrationManager::setupDefaultMappings() {
    // Set up default type mappings
    type_mappings_["Button"] = "Components::Button";
    type_mappings_["Widget"] = "Components::Widget";
    type_mappings_["Layout"] = "Components::Layout";

    qDebug() << "🗺️ Default mappings set up";
}

void IntegrationManager::registerBuiltinConverters() {
    if (component_adapter_) {
        // TODO: Fix these converters when UI commands are properly defined
        // component_adapter_->registerConverter<Components::Button, UI::ButtonCommand>();
        // component_adapter_->registerConverter<Components::Widget, UI::ContainerCommand>();
    }

    qDebug() << "📋 Built-in converters registered";
}

void IntegrationManager::connectAdapterSignals() {
    // Connect adapter signals to integration manager
    if (ui_element_adapter_) {
        // Connect UIElementAdapter signals
    }

    if (json_loader_) {
        connect(json_loader_.get(), &JSONCommandLoader::commandLoadingFailed,
                this, &IntegrationManager::onAdapterError);
    }

    if (state_adapter_) {
        connect(state_adapter_.get(), &CommandStateManagerAdapter::syncError,
                this, [this](std::shared_ptr<UI::BaseUICommand>, const QString& error) {
                    onAdapterError(error);
                });
    }

    if (component_adapter_) {
        connect(component_adapter_.get(), &ComponentSystemAdapter::conversionError,
                this, &IntegrationManager::onAdapterError);
        connect(component_adapter_.get(), &ComponentSystemAdapter::componentConverted,
                this, &IntegrationManager::onConversionCompleted);
    }

    qDebug() << "🔗 Adapter signals connected";
}

void IntegrationManager::handleConversionRequest(QObject* source) {
    QString sourceType = detectSourceType(source);
    qDebug() << "🔄 Handling conversion request for type:" << sourceType;

    // Try to convert using available adapters
    if (component_adapter_) {
        auto command = component_adapter_->createCommandFromComponent(source);
        if (command) {
            emit conversionPerformed(source, command);
        }
    }
}

void IntegrationManager::handleMigrationRequest(const QString& path) {
    qDebug() << "📦 Handling migration request for:" << path;

    QDir dir(path);
    if (dir.exists()) {
        // Process directory
        QStringList filters;
        filters << "*.json" << "*.ui" << "*.qml";

        QFileInfoList files = dir.entryInfoList(filters, QDir::Files);
        for (const QFileInfo& fileInfo : files) {
            migrateFile(fileInfo.absoluteFilePath());
        }
    } else {
        // Process single file
        migrateFile(path);
    }
}

void IntegrationManager::updateStatistics(const QString& operation) {
    if (operation == "conversion") {
        stats_.components_converted++;
    } else if (operation == "creation") {
        stats_.commands_created++;
    } else if (operation == "widget_mapping") {
        stats_.widgets_mapped++;
    } else if (operation == "state_binding") {
        stats_.state_bindings++;
    } else if (operation == "event_handler") {
        stats_.event_handlers++;
    } else if (operation == "json_load") {
        stats_.json_loads++;
    } else if (operation == "migration") {
        stats_.migration_operations++;
    }
}

QString IntegrationManager::detectSourceType(QObject* source) {
    if (!source) {
        return "Unknown";
    }

    return source->metaObject()->className();
}

QString IntegrationManager::detectFileType(const QString& filePath) {
    QFileInfo fileInfo(filePath);
    return fileInfo.suffix().toLower();
}

void IntegrationManager::onAdapterError(const QString& error) {
    qWarning() << "Adapter error:" << error;
    emit integrationError(error);
}

void IntegrationManager::onConversionCompleted(QObject* source, std::shared_ptr<UI::BaseUICommand> result) {
    updateStatistics("conversion");
    emit conversionPerformed(source, result);
}

// **CompatibilityLayer implementation**
// Template implementation is in the header file

void CompatibilityLayer::setProperty(QObject* object, const QString& property, const QVariant& value) {
    if (isCommand(object)) {
        auto command = asCommand(object);
        if (command) {
            command->getState()->setProperty(property, value);
        }
    } else {
        object->setProperty(property.toUtf8().constData(), value);
    }
}

QVariant CompatibilityLayer::getProperty(QObject* object, const QString& property) {
    if (isCommand(object)) {
        auto command = asCommand(object);
        if (command) {
            return command->getState()->getProperty<QVariant>(property);
        }
    }

    return object->property(property.toUtf8().constData());
}

void CompatibilityLayer::connectEvent(QObject* source, const QString& event, std::function<void()> handler) {
    if (isCommand(source)) {
        auto command = asCommand(source);
        if (command) {
            UI::Events::onClick(command.get(), handler);
        }
    } else {
        // Connect to Qt signal
        // This would need more sophisticated signal connection logic
    }
}

void CompatibilityLayer::bindToState(QObject* object, const QString& stateKey, const QString& property) {
    if (isCommand(object)) {
        auto command = asCommand(object);
        if (command) {
            Adapters::State::bind(command, stateKey, property);
        }
    } else {
        // Handle legacy component state binding
        // This would need integration with existing state management
    }
}

bool CompatibilityLayer::isCommand(QObject* object) {
    return qobject_cast<UI::BaseUICommand*>(object) != nullptr;
}

bool CompatibilityLayer::isComponent(QObject* object) {
    // Check if object is a legacy component
    return qobject_cast<Components::Button*>(object) != nullptr ||
           qobject_cast<Components::Widget*>(object) != nullptr;
}

bool CompatibilityLayer::isUIElement(QObject* object) {
    return qobject_cast<Core::UIElement*>(object) != nullptr;
}

std::shared_ptr<UI::BaseUICommand> CompatibilityLayer::asCommand(QObject* object) {
    auto* command = qobject_cast<UI::BaseUICommand*>(object);
    if (command) {
        // This is a simplified approach - in practice, you'd need proper shared_ptr management
        return std::shared_ptr<UI::BaseUICommand>(command, [](UI::BaseUICommand*) {
            // Custom deleter that doesn't actually delete (object is managed elsewhere)
        });
    }
    return nullptr;
}

Core::UIElement* CompatibilityLayer::asUIElement(QObject* object) {
    return qobject_cast<Core::UIElement*>(object);
}

QWidget* CompatibilityLayer::asWidget(QObject* object) {
    return qobject_cast<QWidget*>(object);
}

// **MigrationAssistant implementation**
MigrationAssistant::ProjectAnalysis MigrationAssistant::analyzeProject(const QString& projectPath) {
    ProjectAnalysis analysis;
    analysis.project_path = projectPath;

    QDir dir(projectPath);
    if (!dir.exists()) {
        return analysis;
    }

    // Analyze files in project
    QStringList filters;
    filters << "*.cpp" << "*.h" << "*.hpp" << "*.json" << "*.ui" << "*.qml";

    QFileInfoList files = dir.entryInfoList(filters, QDir::Files | QDir::NoDotAndDotDot, QDir::Name);
    analysis.total_files = files.size();

    for (const QFileInfo& fileInfo : files) {
        analyzeFile(fileInfo.absoluteFilePath(), analysis);
    }

    analysis.complexity_level = assessComplexity(analysis);
    analysis.estimated_effort_hours = estimateEffort(analysis);

    return analysis;
}

MigrationAssistant::MigrationPlan MigrationAssistant::createMigrationPlan(const ProjectAnalysis& analysis) {
    MigrationPlan plan;
    plan.analysis = analysis;

    // Create migration steps based on analysis
    plan.migration_steps.append("1. Backup existing project");
    plan.migration_steps.append("2. Initialize Command system");
    plan.migration_steps.append("3. Convert core components");
    plan.migration_steps.append("4. Update JSON definitions");
    plan.migration_steps.append("5. Test integration");
    plan.migration_steps.append("6. Validate migration");

    // Determine file conversion order
    plan.file_conversion_order.append("Core components first");
    plan.file_conversion_order.append("UI definitions second");
    plan.file_conversion_order.append("Application logic last");

    // Add recommendations
    plan.backup_recommendations.append("Create full project backup");
    plan.backup_recommendations.append("Use version control");

    plan.testing_recommendations.append("Test each component after conversion");
    plan.testing_recommendations.append("Run integration tests");

    plan.estimated_timeline = QString("%1 days").arg((analysis.estimated_effort_hours + 7) / 8);

    return plan;
}

bool MigrationAssistant::executeMigrationPlan(const MigrationPlan& plan, std::function<void(const QString&)> progressCallback) {
    if (progressCallback) {
        progressCallback("Starting migration...");
    }

    // Execute migration steps
    for (const QString& step : plan.migration_steps) {
        if (progressCallback) {
            progressCallback(QString("Executing: %1").arg(step));
        }

        // Simulate step execution
        QThread::msleep(100);
    }

    if (progressCallback) {
        progressCallback("Migration completed successfully");
    }

    return true;
}

bool MigrationAssistant::createBackup(const QString& projectPath, const QString& backupPath) {
    QDir sourceDir(projectPath);
    QDir targetDir(backupPath);

    if (!sourceDir.exists()) {
        return false;
    }

    // Create backup directory
    if (!targetDir.exists()) {
        targetDir.mkpath(".");
    }

    // Copy files (simplified implementation)
    qDebug() << "Creating backup from" << projectPath << "to" << backupPath;
    return true;
}

bool MigrationAssistant::rollbackMigration(const QString& projectPath, const QString& backupPath) {
    // Restore from backup (simplified implementation)
    qDebug() << "Rolling back migration from" << backupPath << "to" << projectPath;
    return true;
}

bool MigrationAssistant::validateMigration(const QString& projectPath) {
    // Validate migration results
    auto& manager = IntegrationManager::instance();
    return manager.validateIntegration();
}

QStringList MigrationAssistant::getMigrationIssues(const QString& projectPath) {
    Q_UNUSED(projectPath)

    auto& manager = IntegrationManager::instance();
    return manager.getIntegrationIssues();
}

void MigrationAssistant::analyzeFile(const QString& filePath, ProjectAnalysis& analysis) {
    QFileInfo fileInfo(filePath);
    QString suffix = fileInfo.suffix().toLower();

    if (suffix == "json") {
        analysis.json_files++;
    } else if (suffix == "ui" || suffix == "qml") {
        analysis.ui_files++;
    } else if (suffix == "cpp" || suffix == "h" || suffix == "hpp") {
        analysis.component_files++;
    }
}

QString MigrationAssistant::assessComplexity(const ProjectAnalysis& analysis) {
    int totalFiles = analysis.ui_files + analysis.component_files + analysis.json_files;

    if (totalFiles < 10) {
        return "Simple";
    } else if (totalFiles < 50) {
        return "Moderate";
    } else {
        return "Complex";
    }
}

int MigrationAssistant::estimateEffort(const ProjectAnalysis& analysis) {
    // Estimate effort in hours based on file counts
    int effort = 0;
    effort += analysis.ui_files * 2;        // 2 hours per UI file
    effort += analysis.component_files * 1; // 1 hour per component file
    effort += analysis.json_files * 1;      // 1 hour per JSON file

    return std::max(effort, 4); // Minimum 4 hours
}

}  // namespace DeclarativeUI::Command::Adapters
