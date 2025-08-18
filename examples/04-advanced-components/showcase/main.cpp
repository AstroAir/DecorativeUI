/**
 * @file main.cpp
 * @brief Main entry point for the Advanced Components Showcase
 */

#include <QApplication>
#include <QStyleFactory>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>

#include "AdvancedComponentsShowcase.hpp"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    
    // Set application properties
    app.setApplicationName("Advanced Components Showcase");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("DeclarativeUI");
    app.setOrganizationDomain("declarativeui.org");
    
    qDebug() << "🚀 Starting Advanced Components Showcase";
    qDebug() << "📱 Application:" << app.applicationName() << app.applicationVersion();
    qDebug() << "🎨 Available styles:" << QStyleFactory::keys();
    
    // Set a modern style
    if (QStyleFactory::keys().contains("Fusion")) {
        app.setStyle("Fusion");
        qDebug() << "✨ Using Fusion style";
    }
    
    // Apply modern dark theme
    app.setStyleSheet(R"(
        QMainWindow {
            background-color: #f5f5f5;
        }
        
        QTabWidget::pane {
            border: 1px solid #cccccc;
            background-color: white;
        }
        
        QTabWidget::tab-bar {
            alignment: center;
        }
        
        QTabBar::tab {
            background-color: #e0e0e0;
            border: 1px solid #cccccc;
            border-bottom: none;
            padding: 8px 16px;
            margin-right: 2px;
            border-top-left-radius: 4px;
            border-top-right-radius: 4px;
        }
        
        QTabBar::tab:selected {
            background-color: white;
            border-bottom: 1px solid white;
        }
        
        QTabBar::tab:hover {
            background-color: #f0f0f0;
        }
        
        QGroupBox {
            font-weight: bold;
            border: 2px solid #cccccc;
            border-radius: 5px;
            margin-top: 10px;
            padding-top: 10px;
        }
        
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px 0 5px;
        }
        
        QPushButton {
            background-color: #4CAF50;
            border: none;
            color: white;
            padding: 8px 16px;
            border-radius: 4px;
            font-weight: bold;
        }
        
        QPushButton:hover {
            background-color: #45a049;
        }
        
        QPushButton:pressed {
            background-color: #3d8b40;
        }
        
        QLineEdit {
            border: 2px solid #cccccc;
            border-radius: 4px;
            padding: 8px;
            font-size: 14px;
        }
        
        QLineEdit:focus {
            border-color: #4CAF50;
        }
        
        QTextEdit {
            border: 1px solid #cccccc;
            border-radius: 4px;
            background-color: white;
        }
        
        QListWidget {
            border: 1px solid #cccccc;
            border-radius: 4px;
            background-color: white;
            alternate-background-color: #f8f9fa;
        }
        
        QListWidget::item {
            padding: 8px;
            border-bottom: 1px solid #eeeeee;
        }
        
        QListWidget::item:selected {
            background-color: #e3f2fd;
            color: #1976d2;
        }
        
        QTreeWidget {
            border: 1px solid #cccccc;
            border-radius: 4px;
            background-color: white;
            alternate-background-color: #f8f9fa;
        }
        
        QTreeWidget::item {
            padding: 4px;
        }
        
        QTreeWidget::item:selected {
            background-color: #e3f2fd;
            color: #1976d2;
        }
        
        QDockWidget {
            titlebar-close-icon: url(:/icons/close.png);
            titlebar-normal-icon: url(:/icons/undock.png);
        }
        
        QDockWidget::title {
            background-color: #e0e0e0;
            padding: 8px;
            border-bottom: 1px solid #cccccc;
        }
        
        QStatusBar {
            background-color: #f0f0f0;
            border-top: 1px solid #cccccc;
        }
        
        QMenuBar {
            background-color: #f8f9fa;
            border-bottom: 1px solid #dee2e6;
        }
        
        QMenuBar::item {
            padding: 8px 12px;
            background-color: transparent;
        }
        
        QMenuBar::item:selected {
            background-color: #e9ecef;
        }
        
        QToolBar {
            background-color: #f8f9fa;
            border-bottom: 1px solid #dee2e6;
            spacing: 4px;
        }
        
        QProgressBar {
            border: 1px solid #cccccc;
            border-radius: 4px;
            text-align: center;
        }
        
        QProgressBar::chunk {
            background-color: #4CAF50;
            border-radius: 3px;
        }
    )");
    
    try {
        // Create and show the main window
        AdvancedComponentsShowcase window;
        window.show();
        
        qDebug() << "🎪 Advanced Components Showcase window created and shown";
        qDebug() << "📏 Window size:" << window.size();
        qDebug() << "🖥️ Screen geometry:" << app.primaryScreen()->geometry();
        
        // Center the window on screen
        QRect screen_geometry = app.primaryScreen()->geometry();
        int x = (screen_geometry.width() - window.width()) / 2;
        int y = (screen_geometry.height() - window.height()) / 2;
        window.move(x, y);
        
        qDebug() << "📍 Window centered at:" << window.pos();
        qDebug() << "✅ Application ready - entering event loop";
        
        return app.exec();
        
    } catch (const std::exception& e) {
        qCritical() << "❌ Fatal error:" << e.what();
        return 1;
    } catch (...) {
        qCritical() << "❌ Unknown fatal error occurred";
        return 1;
    }
}
