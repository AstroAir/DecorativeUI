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
    // Validate required widget parameter
    auto validationResult = validateRequiredParameter(context, "widget");
    if (!validationResult.isSuccess()) {
        return validationResult;
    }

    auto widget_name = context.getParameter<QString>("widget");
    auto* menuBar = findMenuBar(widget_name);
    if (!menuBar) {
        return CommandResult<QVariant>(QString("MenuBar '%1' not found").arg(widget_name));
    }

    auto operation = context.getParameter<QString>("operation");

    // Route to appropriate operation handler
    if (operation == "addMenu" || operation.isEmpty()) {
        return handleAddMenu(context, menuBar);
    } else if (operation == "removeMenu") {
        return handleRemoveMenu(context, menuBar);
    } else if (operation == "setNativeMenuBar") {
        return handleSetNativeMenuBar(context, menuBar);
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

CommandResult<QVariant> MenuBarCommand::validateRequiredParameter(const CommandContext& context, const QString& paramName) {
    if (!context.hasParameter(paramName)) {
        return CommandResult<QVariant>(QString("Missing required parameter: %1").arg(paramName));
    }
    return CommandResult<QVariant>(QVariant()); // Success
}

CommandResult<QVariant> MenuBarCommand::handleAddMenu(const CommandContext& context, QMenuBar* menuBar) {
    auto title = context.getParameter<QString>("title");
    if (!context.hasParameter("title")) {
        return CommandResult<QVariant>(QString("Missing title parameter for addMenu operation"));
    }

    auto menu = menuBar->addMenu(title);
    return CommandResult<QVariant>(QString("MenuBar menu added successfully"));
}

CommandResult<QVariant> MenuBarCommand::handleRemoveMenu(const CommandContext& context, QMenuBar* menuBar) {
    auto title = context.getParameter<QString>("title");
    if (!context.hasParameter("title")) {
        return CommandResult<QVariant>(QString("Missing title parameter for removeMenu operation"));
    }

    // Find and remove menu by title
    for (auto* action : menuBar->actions()) {
        if (action->text() == title) {
            menuBar->removeAction(action);
            return CommandResult<QVariant>(QString("MenuBar menu removed successfully"));
        }
    }
    return CommandResult<QVariant>(QString("Menu with title '%1' not found").arg(title));
}

CommandResult<QVariant> MenuBarCommand::handleSetNativeMenuBar(const CommandContext& context, QMenuBar* menuBar) {
    auto native = context.getParameter<bool>("native");
    if (!context.hasParameter("native")) {
        return CommandResult<QVariant>(QString("Missing native parameter for setNativeMenuBar operation"));
    }

    menuBar->setNativeMenuBar(native);
    return CommandResult<QVariant>(QString("MenuBar native menu bar set successfully"));
}

// ============================================================================
// STATUS BAR COMPONENTS
// ============================================================================

StatusBarCommand::StatusBarCommand(const CommandContext& context)
    : ICommand(nullptr) {}

CommandResult<QVariant> StatusBarCommand::execute(const CommandContext& context) {
    // Validate required widget parameter
    auto validationResult = validateRequiredParameter(context, "widget");
    if (!validationResult.isSuccess()) {
        return validationResult;
    }

    auto widget_name = context.getParameter<QString>("widget");
    auto* statusBar = findStatusBar(widget_name);
    if (!statusBar) {
        return CommandResult<QVariant>(QString("StatusBar '%1' not found").arg(widget_name));
    }

    auto operation = context.getParameter<QString>("operation");

    // Route to appropriate operation handler
    if (operation == "showMessage" || operation.isEmpty()) {
        return handleShowMessage(context, statusBar);
    } else if (operation == "clearMessage") {
        return handleClearMessage(context, statusBar);
    } else if (operation == "addWidget") {
        return handleAddWidget(context, statusBar);
    } else if (operation == "removeWidget") {
        return handleRemoveWidget(context, statusBar);
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

QWidget* StatusBarCommand::findWidgetByName(const QString& name, QWidget* parent) {
    for (auto* widget : QApplication::allWidgets()) {
        if (widget->objectName() == name) {
            // If parent is specified, check if widget is child of parent
            if (parent && widget->parent() != parent) {
                continue;
            }
            return widget;
        }
    }
    return nullptr;
}

CommandResult<QVariant> StatusBarCommand::validateRequiredParameter(const CommandContext& context, const QString& paramName) {
    if (!context.hasParameter(paramName)) {
        return CommandResult<QVariant>(QString("Missing required parameter: %1").arg(paramName));
    }
    return CommandResult<QVariant>(QVariant()); // Success
}

CommandResult<QVariant> StatusBarCommand::handleShowMessage(const CommandContext& context, QStatusBar* statusBar) {
    auto message = context.getParameter<QString>("message");
    if (!context.hasParameter("message")) {
        return CommandResult<QVariant>(QString("Missing message parameter for showMessage operation"));
    }

    auto timeout = context.getParameter<int>("timeout");
    int timeoutMs = context.hasParameter("timeout") ? timeout : 0;
    statusBar->showMessage(message, timeoutMs);
    return CommandResult<QVariant>(QString("StatusBar message shown successfully"));
}

CommandResult<QVariant> StatusBarCommand::handleClearMessage(const CommandContext& context, QStatusBar* statusBar) {
    Q_UNUSED(context) // clearMessage doesn't need additional parameters
    statusBar->clearMessage();
    return CommandResult<QVariant>(QString("StatusBar message cleared successfully"));
}

CommandResult<QVariant> StatusBarCommand::handleAddWidget(const CommandContext& context, QStatusBar* statusBar) {
    auto widgetName = context.getParameter<QString>("widgetName");
    if (!context.hasParameter("widgetName")) {
        return CommandResult<QVariant>(QString("Missing widgetName parameter for addWidget operation"));
    }

    // Find the widget to add by name
    QWidget* widgetToAdd = findWidgetByName(widgetName);
    if (!widgetToAdd) {
        return CommandResult<QVariant>(QString("Widget '%1' not found for addWidget operation").arg(widgetName));
    }

    auto stretch = context.getParameter<int>("stretch");
    int stretchValue = context.hasParameter("stretch") ? stretch : 0;
    statusBar->addWidget(widgetToAdd, stretchValue);
    return CommandResult<QVariant>(QString("Widget '%1' added to StatusBar successfully").arg(widgetName));
}

CommandResult<QVariant> StatusBarCommand::handleRemoveWidget(const CommandContext& context, QStatusBar* statusBar) {
    auto widgetName = context.getParameter<QString>("widgetName");
    if (!context.hasParameter("widgetName")) {
        return CommandResult<QVariant>(QString("Missing widgetName parameter for removeWidget operation"));
    }

    // Find the widget to remove by name (must be child of statusBar)
    QWidget* widgetToRemove = findWidgetByName(widgetName, statusBar);
    if (!widgetToRemove) {
        return CommandResult<QVariant>(QString("Widget '%1' not found in StatusBar for removeWidget operation").arg(widgetName));
    }

    statusBar->removeWidget(widgetToRemove);
    return CommandResult<QVariant>(QString("Widget '%1' removed from StatusBar successfully").arg(widgetName));
}

}  // namespace ComponentCommands
}  // namespace Command
}  // namespace DeclarativeUI
