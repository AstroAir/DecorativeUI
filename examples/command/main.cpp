#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QStyleFactory>

#include "Binding/StateManager.hpp"
#include "Command/MVCIntegration.hpp"
#include "Command/UICommandFactory.hpp"
#include "Command/WidgetMapper.hpp"
#include "CommandUIExample.hpp"

using namespace DeclarativeUI::Command::UI::Examples;

int main(int argc, char* argv[]) {
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
        [[maybe_unused]] auto& factory =
            DeclarativeUI::Command::UI::UICommandFactory::instance();
        [[maybe_unused]] auto& mapper =
            DeclarativeUI::Command::UI::WidgetMapper::instance();
        [[maybe_unused]] auto& bridge =
            DeclarativeUI::Command::UI::MVCIntegrationBridge::instance();
        [[maybe_unused]] auto& stateManager =
            DeclarativeUI::Binding::StateManager::instance();

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
