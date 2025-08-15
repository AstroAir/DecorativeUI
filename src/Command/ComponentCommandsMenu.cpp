#include "ComponentCommands.hpp"
#include <QApplication>
#include <QDebug>
#include <QMenuBar>
#include <QStatusBar>
#include <QToolBar>
#include <QToolButton>
#include <QMenu>
#include <QAction>

namespace DeclarativeUI {
namespace Command {
namespace ComponentCommands {

// ============================================================================
// MENU BAR COMPONENTS
// ============================================================================

MenuBarCommand::MenuBarCommand(const CommandContext& context)
    : ICommand(nullptr) {}

CommandResult<QVariant> MenuBarCommand::execute(const CommandContext& context) {
    auto widget_name = context.getParameter<QString>("widget");
    auto operation = context.getParameter<QString>("operation");

    if (!context.hasParameter("widget")) {
        return CommandResult<QVariant>(QString("Missing required parameter: widget"));
    }

    auto* menuBar = findMenuBar(widget_name);
    if (!menuBar) {
        return CommandResult<QVariant>(QString("MenuBar '%1' not found").arg(widget_name));
    }

    if (operation == "addMenu" || operation.isEmpty()) {
        auto title = context.getParameter<QString>("title");
        if (context.hasParameter("title")) {
            auto menu = menuBar->addMenu(title);
            return CommandResult<QVariant>(QString("MenuBar menu added successfully"));
        }
        return CommandResult<QVariant>(QString("Missing title parameter for addMenu operation"));
    } else if (operation == "removeMenu") {
        auto title = context.getParameter<QString>("title");
        if (context.hasParameter("title")) {
            // Find and remove menu by title
            for (auto* action : menuBar->actions()) {
                if (action->text() == title) {
                    menuBar->removeAction(action);
                    return CommandResult<QVariant>(QString("MenuBar menu removed successfully"));
                }
            }
            return CommandResult<QVariant>(QString("Menu with title '%1' not found").arg(title));
        }
        return CommandResult<QVariant>(QString("Missing title parameter for removeMenu operation"));
    } else if (operation == "setNativeMenuBar") {
        auto native = context.getParameter<bool>("native");
        if (context.hasParameter("native")) {
            menuBar->setNativeMenuBar(native);
            return CommandResult<QVariant>(QString("MenuBar native menu bar set successfully"));
        }
        return CommandResult<QVariant>(QString("Missing native parameter for setNativeMenuBar operation"));
    }

    return CommandResult<QVariant>(QString("Unknown operation: %1").arg(operation));
}

CommandMetadata MenuBarCommand::getMetadata() const {
    return CommandMetadata("MenuBarCommand", "Specialized command for MenuBar components");
}

QMenuBar* MenuBarCommand::findMenuBar(const QString& name) {
    for (auto* widget : QApplication::allWidgets()) {
        if (auto* menuBar = qobject_cast<QMenuBar*>(widget)) {
            if (menuBar->objectName() == name) {
                return menuBar;
            }
        }
    }
    return nullptr;
}

// ============================================================================
// STATUS BAR COMPONENTS
// ============================================================================

StatusBarCommand::StatusBarCommand(const CommandContext& context)
    : ICommand(nullptr) {}

CommandResult<QVariant> StatusBarCommand::execute(const CommandContext& context) {
    auto widget_name = context.getParameter<QString>("widget");
    auto operation = context.getParameter<QString>("operation");

    if (!context.hasParameter("widget")) {
        return CommandResult<QVariant>(QString("Missing required parameter: widget"));
    }

    auto* statusBar = findStatusBar(widget_name);
    if (!statusBar) {
        return CommandResult<QVariant>(QString("StatusBar '%1' not found").arg(widget_name));
    }

    if (operation == "showMessage" || operation.isEmpty()) {
        auto message = context.getParameter<QString>("message");
        auto timeout = context.getParameter<int>("timeout");
        if (context.hasParameter("message")) {
            int timeoutMs = context.hasParameter("timeout") ? timeout : 0;
            statusBar->showMessage(message, timeoutMs);
            return CommandResult<QVariant>(QString("StatusBar message shown successfully"));
        }
        return CommandResult<QVariant>(QString("Missing message parameter for showMessage operation"));
    } else if (operation == "clearMessage") {
        statusBar->clearMessage();
        return CommandResult<QVariant>(QString("StatusBar message cleared successfully"));
    } else if (operation == "addWidget") {
        auto widgetName = context.getParameter<QString>("widgetName");
        auto stretch = context.getParameter<int>("stretch");
        if (context.hasParameter("widgetName")) {
            // Find the widget to add by name
            QWidget* widgetToAdd = nullptr;
            for (auto* widget : QApplication::allWidgets()) {
                if (widget->objectName() == widgetName) {
                    widgetToAdd = widget;
                    break;
                }
            }

            if (widgetToAdd) {
                int stretchValue = context.hasParameter("stretch") ? stretch : 0;
                statusBar->addWidget(widgetToAdd, stretchValue);
                return CommandResult<QVariant>(QString("Widget '%1' added to StatusBar successfully").arg(widgetName));
            } else {
                return CommandResult<QVariant>(QString("Widget '%1' not found for addWidget operation").arg(widgetName));
            }
        }
        return CommandResult<QVariant>(QString("Missing widgetName parameter for addWidget operation"));
    } else if (operation == "removeWidget") {
        auto widgetName = context.getParameter<QString>("widgetName");
        if (context.hasParameter("widgetName")) {
            // Find the widget to remove by name
            QWidget* widgetToRemove = nullptr;
            for (auto* widget : QApplication::allWidgets()) {
                if (widget->objectName() == widgetName && widget->parent() == statusBar) {
                    widgetToRemove = widget;
                    break;
                }
            }

            if (widgetToRemove) {
                statusBar->removeWidget(widgetToRemove);
                return CommandResult<QVariant>(QString("Widget '%1' removed from StatusBar successfully").arg(widgetName));
            } else {
                return CommandResult<QVariant>(QString("Widget '%1' not found in StatusBar for removeWidget operation").arg(widgetName));
            }
        }
        return CommandResult<QVariant>(QString("Missing widgetName parameter for removeWidget operation"));
    }

    return CommandResult<QVariant>(QString("Unknown operation: %1").arg(operation));
}

CommandMetadata StatusBarCommand::getMetadata() const {
    return CommandMetadata("StatusBarCommand", "Specialized command for StatusBar components");
}

QStatusBar* StatusBarCommand::findStatusBar(const QString& name) {
    for (auto* widget : QApplication::allWidgets()) {
        if (auto* statusBar = qobject_cast<QStatusBar*>(widget)) {
            if (statusBar->objectName() == name) {
                return statusBar;
            }
        }
    }
    return nullptr;
}

}  // namespace ComponentCommands
}  // namespace Command
}  // namespace DeclarativeUI
