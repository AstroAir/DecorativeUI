#include <QApplication>
#include <QDebug>
#include <QStyleFactory>
#include <QDir>

#include "CommandUIExample.hpp"
#include "../UICommandFactory.hpp"
#include "../WidgetMapper.hpp"
#include "../MVCIntegration.hpp"
#include "../../Binding/StateManager.hpp"

using namespace DeclarativeUI::Command::UI::Examples;

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Set application properties
    app.setApplicationName("Command UI Example");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("DeclarativeUI");
    app.setOrganizationDomain("declarativeui.org");
    
    qDebug() << "🚀 Starting Command UI Example Application";
    qDebug() << "Qt Version:" << QT_VERSION_STR;
    qDebug() << "Available styles:" << QStyleFactory::keys();
    
    try {
        // Initialize the Command system
        qDebug() << "🔧 Initializing Command system...";
        
        // Initialize core components
        auto& factory = DeclarativeUI::Command::UI::UICommandFactory::instance();
        auto& mapper = DeclarativeUI::Command::UI::WidgetMapper::instance();
        auto& bridge = DeclarativeUI::Command::UI::MVCIntegrationBridge::instance();
        auto& stateManager = DeclarativeUI::Binding::StateManager::instance();
        
        qDebug() << "✅ Command system initialized";
        
        // Create and show the main window
        qDebug() << "🏗️ Creating main window...";
        CommandUIExample window;
        window.setWindowTitle("Command-based UI Architecture Examples");
        window.setMinimumSize(800, 600);
        window.resize(1000, 700);
        
        // Center the window
        window.show();
        
        qDebug() << "✅ Main window created and shown";
        qDebug() << "🎯 Application ready - starting event loop";
        
        // Start the application event loop
        int result = app.exec();
        
        qDebug() << "🏁 Application finished with exit code:" << result;
        return result;
        
    } catch (const std::exception& e) {
        qCritical() << "❌ Fatal error:" << e.what();
        return -1;
    } catch (...) {
        qCritical() << "❌ Unknown fatal error occurred";
        return -2;
    }
}
