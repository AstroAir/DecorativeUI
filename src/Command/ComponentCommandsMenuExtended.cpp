#include "ComponentCommands.hpp"
#include <QApplication>
#include <QDebug>
#include <QToolBar>
#include <QToolButton>
#include <QAction>

namespace DeclarativeUI {
namespace Command {
namespace ComponentCommands {

// ============================================================================
// TOOL BAR COMPONENTS
// ============================================================================

ToolBarCommand::ToolBarCommand(const CommandContext& context)
    : ICommand(nullptr) {}

CommandResult<QVariant> ToolBarCommand::execute(const CommandContext& context) {
    auto widget_name = context.getParameter<QString>("widget");
    auto operation = context.getParameter<QString>("operation");

    if (!context.hasParameter("widget")) {
        return CommandResult<QVariant>(QString("Missing required parameter: widget"));
    }

    auto* toolBar = findToolBar(widget_name);
    if (!toolBar) {
        return CommandResult<QVariant>(QString("ToolBar '%1' not found").arg(widget_name));
    }

    widget_name_ = widget_name;
    old_orientation_ = toolBar->orientation();
    operation_ = operation;

    if (operation == "addAction" || operation.isEmpty()) {
        auto text = context.getParameter<QString>("text");
        if (context.hasParameter("text")) {
            auto action = toolBar->addAction(text);
            return CommandResult<QVariant>(QString("ToolBar action added successfully"));
        }
        return CommandResult<QVariant>(QString("Missing text parameter for addAction operation"));
    } else if (operation == "removeAction") {
        auto text = context.getParameter<QString>("text");
        if (context.hasParameter("text")) {
            // Find and remove action by text
            for (auto* action : toolBar->actions()) {
                if (action->text() == text) {
                    toolBar->removeAction(action);
                    return CommandResult<QVariant>(QString("ToolBar action removed successfully"));
                }
            }
            return CommandResult<QVariant>(QString("Action with text '%1' not found").arg(text));
        }
        return CommandResult<QVariant>(QString("Missing text parameter for removeAction operation"));
    } else if (operation == "setOrientation") {
        auto orientation = context.getParameter<int>("orientation");
        if (context.hasParameter("orientation")) {
            toolBar->setOrientation(static_cast<Qt::Orientation>(orientation));
            return CommandResult<QVariant>(QString("ToolBar orientation set successfully"));
        }
        return CommandResult<QVariant>(QString("Missing orientation parameter for setOrientation operation"));
    } else if (operation == "setMovable") {
        auto movable = context.getParameter<bool>("movable");
        if (context.hasParameter("movable")) {
            toolBar->setMovable(movable);
            return CommandResult<QVariant>(QString("ToolBar movable state set successfully"));
        }
        return CommandResult<QVariant>(QString("Missing movable parameter for setMovable operation"));
    }

    return CommandResult<QVariant>(QString("Unknown operation: %1").arg(operation));
}

CommandResult<QVariant> ToolBarCommand::undo(const CommandContext& context) {
    auto* toolBar = findToolBar(widget_name_);
    if (!toolBar) {
        return CommandResult<QVariant>(QString("ToolBar '%1' not found for undo").arg(widget_name_));
    }

    if (operation_ == "setOrientation") {
        toolBar->setOrientation(old_orientation_);
    }
    return CommandResult<QVariant>(QString("ToolBar undo successful"));
}

bool ToolBarCommand::canUndo(const CommandContext& context) const {
    return !widget_name_.isEmpty() && operation_ == "setOrientation";
}

CommandMetadata ToolBarCommand::getMetadata() const {
    return CommandMetadata("ToolBarCommand", "Specialized command for ToolBar components");
}

QToolBar* ToolBarCommand::findToolBar(const QString& name) {
    for (auto* widget : QApplication::allWidgets()) {
        if (auto* toolBar = qobject_cast<QToolBar*>(widget)) {
            if (toolBar->objectName() == name) {
                return toolBar;
            }
        }
    }
    return nullptr;
}

// ============================================================================
// TOOL BUTTON COMPONENTS
// ============================================================================

ToolButtonCommand::ToolButtonCommand(const CommandContext& context)
    : ICommand(nullptr) {}

CommandResult<QVariant> ToolButtonCommand::execute(const CommandContext& context) {
    auto widget_name = context.getParameter<QString>("widget");
    auto operation = context.getParameter<QString>("operation");

    if (!context.hasParameter("widget")) {
        return CommandResult<QVariant>(QString("Missing required parameter: widget"));
    }

    auto* toolButton = findToolButton(widget_name);
    if (!toolButton) {
        return CommandResult<QVariant>(QString("ToolButton '%1' not found").arg(widget_name));
    }

    widget_name_ = widget_name;
    old_popup_mode_ = toolButton->popupMode();
    old_style_ = toolButton->toolButtonStyle();
    operation_ = operation;

    if (operation == "setDefaultAction" || operation.isEmpty()) {
        // Note: Setting default action requires QAction instance
        return CommandResult<QVariant>(QString("setDefaultAction operation requires QAction implementation"));
    } else if (operation == "setPopupMode") {
        auto mode = context.getParameter<int>("mode");
        if (context.hasParameter("mode")) {
            toolButton->setPopupMode(static_cast<QToolButton::ToolButtonPopupMode>(mode));
            return CommandResult<QVariant>(QString("ToolButton popup mode set successfully"));
        }
        return CommandResult<QVariant>(QString("Missing mode parameter for setPopupMode operation"));
    } else if (operation == "setToolButtonStyle") {
        auto style = context.getParameter<int>("style");
        if (context.hasParameter("style")) {
            toolButton->setToolButtonStyle(static_cast<Qt::ToolButtonStyle>(style));
            return CommandResult<QVariant>(QString("ToolButton style set successfully"));
        }
        return CommandResult<QVariant>(QString("Missing style parameter for setToolButtonStyle operation"));
    }

    return CommandResult<QVariant>(QString("Unknown operation: %1").arg(operation));
}

CommandResult<QVariant> ToolButtonCommand::undo(const CommandContext& context) {
    auto* toolButton = findToolButton(widget_name_);
    if (!toolButton) {
        return CommandResult<QVariant>(QString("ToolButton '%1' not found for undo").arg(widget_name_));
    }

    if (operation_ == "setPopupMode") {
        toolButton->setPopupMode(old_popup_mode_);
    } else if (operation_ == "setToolButtonStyle") {
        toolButton->setToolButtonStyle(old_style_);
    }
    return CommandResult<QVariant>(QString("ToolButton undo successful"));
}

bool ToolButtonCommand::canUndo(const CommandContext& context) const {
    return !widget_name_.isEmpty() && (operation_ == "setPopupMode" || operation_ == "setToolButtonStyle");
}

CommandMetadata ToolButtonCommand::getMetadata() const {
    return CommandMetadata("ToolButtonCommand", "Specialized command for ToolButton components");
}

QToolButton* ToolButtonCommand::findToolButton(const QString& name) {
    for (auto* widget : QApplication::allWidgets()) {
        if (auto* toolButton = qobject_cast<QToolButton*>(widget)) {
            if (toolButton->objectName() == name) {
                return toolButton;
            }
        }
    }
    return nullptr;
}

}  // namespace ComponentCommands
}  // namespace Command
}  // namespace DeclarativeUI
