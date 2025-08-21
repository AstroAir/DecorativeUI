/**
 * @file main.cpp
 * @brief DeclarativeUI Comprehensive Showcase Application
 *
 * This application demonstrates all features and capabilities of the
 * DeclarativeUI framework in a single, interactive showcase. It serves as both
 * a learning resource and a reference implementation for best practices.
 *
 * Features demonstrated:
 * - All 30+ UI components with live examples
 * - State management patterns and reactive programming
 * - Animation system with smooth transitions
 * - Command system with undo/redo functionality
 * - JSON UI loading and dynamic interfaces
 * - Hot reload development workflow
 * - Theme system with dark/light modes
 * - Performance monitoring and optimization
 * - Advanced component composition
 * - Real-world application patterns
 */

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QStandardPaths>
#include <QStyleFactory>
#include <QTimer>

// DeclarativeUI Core
#include "Animation/AnimationEngine.hpp"
#include "Binding/StateManager.hpp"
#include "Core/UIElement.hpp"
#include "HotReload/HotReloadManager.hpp"

// Showcase Application
#include "showcase/ShowcaseApp.hpp"
#include "utils/ExampleHelpers.hpp"

using namespace DeclarativeUI;

/**
 * @brief Configure application settings and environment
 */
void configureApplication(QApplication& app) {
    // Application metadata
    app.setApplicationName("DeclarativeUI Showcase");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("DeclarativeUI");
    app.setOrganizationDomain("declarativeui.org");

    // Set application icon
    app.setWindowIcon(QIcon(":/assets/icons/showcase.png"));

    // Configure high DPI support
    app.setAttribute(Qt::AA_EnableHighDpiScaling);
    app.setAttribute(Qt::AA_UseHighDpiPixmaps);

    qDebug() << "🚀 DeclarativeUI Showcase v" << app.applicationVersion();
    qDebug() << "📁 Working directory:" << QDir::currentPath();
}

/**
 * @brief Initialize DeclarativeUI framework components
 */
void initializeFramework() {
    try {
        // Initialize state manager
        auto& stateManager = Binding::StateManager::instance();
        qDebug() << "✅ State Manager initialized";

        // Initialize animation engine
        auto& animationEngine = Animation::AnimationEngine::instance();
        animationEngine.setGlobalGPUAcceleration(true);
        qDebug() << "✅ Animation Engine initialized with GPU acceleration";

        // Set up global application state
        stateManager.setState("app.theme", "light");
        stateManager.setState("app.language", "en");
        stateManager.setState("app.performance_monitoring", true);
        stateManager.setState("app.hot_reload", true);

        qDebug() << "✅ Framework initialization complete";

    } catch (const std::exception& e) {
        qCritical() << "❌ Framework initialization failed:" << e.what();
        throw;
    }
}

/**
 * @brief Set up resource paths and data directories
 */
void setupResourcePaths() {
    // Ensure resource directories exist
    QStringList resourceDirs = {"resources/ui", "resources/themes",
                                "resources/data", "resources/assets/icons",
                                "resources/assets/images"};

    for (const QString& dir : resourceDirs) {
        QDir().mkpath(dir);
    }

    // Set up data directory for user settings
    QString dataDir =
        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataDir);

    qDebug() << "📁 Resource paths configured";
    qDebug() << "💾 Data directory:" << dataDir;
}

/**
 * @brief Configure logging and debugging
 */
void configureLogging() {
    // Set up custom message handler for better debugging
    qSetMessagePattern(
        "[%{time hh:mm:ss.zzz}] %{if-category}%{category}: %{endif}%{message}");

    // Enable Qt logging categories
    QLoggingCategory::setFilterRules(
        "qt.qml.debug=true\n"
        "declarativeui.*=true");

    qDebug() << "📝 Logging configured";
}

/**
 * @brief Main application entry point
 */
int main(int argc, char* argv[]) {
    // Enable high DPI support before QApplication creation
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QApplication app(argc, argv);

    try {
        // Configure application
        configureApplication(app);
        configureLogging();
        setupResourcePaths();

        // Initialize DeclarativeUI framework
        initializeFramework();

        // Create and show main showcase window
        ShowcaseApp showcase;
        showcase.show();

        // Center window on screen
        showcase.move(QApplication::primaryScreen()->geometry().center() -
                      showcase.rect().center());

        qDebug() << "🎨 Showcase application started successfully";
        qDebug() << "💡 Explore all DeclarativeUI features in the interactive "
                    "showcase!";
        qDebug() << "";
        qDebug() << "📚 Features demonstrated:";
        qDebug() << "   • All 30+ UI components with live examples";
        qDebug() << "   • State management and reactive programming";
        qDebug() << "   • Animation system with smooth transitions";
        qDebug() << "   • Command system with undo/redo";
        qDebug() << "   • JSON UI loading and hot reload";
        qDebug() << "   • Theme system and performance monitoring";
        qDebug() << "";

        // Start the application event loop
        int result = app.exec();

        qDebug() << "👋 Showcase application finished with code:" << result;
        return result;

    } catch (const std::exception& e) {
        qCritical() << "❌ Application error:" << e.what();

        // Show error dialog to user
        QMessageBox::critical(
            nullptr, "DeclarativeUI Showcase Error",
            QString(
                "An error occurred while starting the application:\n\n%1\n\n"
                "Please check the console output for more details.")
                .arg(e.what()));

        return -1;
    }
}
