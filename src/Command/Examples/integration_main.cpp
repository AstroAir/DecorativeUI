#include <QApplication>
#include <QDebug>
#include <QTimer>

#include "IntegrationExample.hpp"
#include "../UICommandFactory.hpp"
#include "../WidgetMapper.hpp"
#include "../MVCIntegration.hpp"
#include "../Adapters/IntegrationManager.hpp"
#include "../../Binding/StateManager.hpp"

using namespace DeclarativeUI::Command::Examples;

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Set application properties
    app.setApplicationName("Integration Example");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("DeclarativeUI");
    app.setOrganizationDomain("declarativeui.org");
    
    qDebug() << "🚀 Starting Integration Example Application";
    qDebug() << "This example demonstrates integration between legacy Components and new Commands";
    
    try {
        // Initialize the Command system and adapters
        qDebug() << "🔧 Initializing systems...";
        
        [[maybe_unused]] auto& factory = DeclarativeUI::Command::UI::UICommandFactory::instance();
        [[maybe_unused]] auto& mapper = DeclarativeUI::Command::UI::WidgetMapper::instance();
        [[maybe_unused]] auto& bridge = DeclarativeUI::Command::UI::MVCIntegrationBridge::instance();
        [[maybe_unused]] auto& stateManager = DeclarativeUI::Binding::StateManager::instance();
        [[maybe_unused]] auto& integrationManager = DeclarativeUI::Command::Adapters::IntegrationManager::instance();
        
        qDebug() << "✅ All systems initialized";
        
        // Create and show the main window
        qDebug() << "🏗️ Creating integration example window...";
        IntegrationExample window;
        window.setWindowTitle("Integration Example - Legacy ↔ Command Systems");
        window.setMinimumSize(900, 700);
        window.resize(1100, 800);
        
        window.show();
        
        // Run demonstrations after a short delay
        QTimer::singleShot(1000, &window, [&window]() {
            qDebug() << "🎬 Starting integration demonstrations...";
            
            window.demonstrateComponentToCommandConversion();
            
            QTimer::singleShot(500, &window, [&window]() {
                window.demonstrateCommandToComponentConversion();
            });
            
            QTimer::singleShot(1000, &window, [&window]() {
                window.demonstrateBidirectionalSync();
            });
            
            QTimer::singleShot(1500, &window, [&window]() {
                window.demonstrateHybridContainer();
            });
            
            QTimer::singleShot(2000, &window, [&window]() {
                window.demonstrateJSONLoading();
            });
            
            QTimer::singleShot(2500, &window, [&window]() {
                window.demonstrateMigrationScenario();
            });
            
            QTimer::singleShot(3000, &window, [&window]() {
                window.demonstrateStateIntegration();
                qDebug() << "✅ All integration demonstrations completed";
            });
        });
        
        qDebug() << "✅ Integration example window created and shown";
        qDebug() << "🎯 Application ready - starting event loop";
        
        // Start the application event loop
        int result = app.exec();
        
        qDebug() << "🏁 Integration example finished with exit code:" << result;
        return result;
        
    } catch (const std::exception& e) {
        qCritical() << "❌ Fatal error in integration example:" << e.what();
        return -1;
    } catch (...) {
        qCritical() << "❌ Unknown fatal error occurred in integration example";
        return -2;
    }
}
