#include <QAction>
#include <QApplication>
#include <QDebug>
#include <QToolBar>
#include <QToolButton>
#include "ComponentCommands.hpp"

namespace DeclarativeUI {
namespace Command {
namespace ComponentCommands {

// ============================================================================
// TOOL BAR COMPONENTS
// ============================================================================

ToolBarCommand::ToolBarCommand(const CommandContext& context)
    : ICommand(nullptr) {}

/**
 * @brief Execute a toolbar command operation
 * @param context Command execution context containing parameters
 * @return CommandResult indicating success or failure
 *
 * This function handles various toolbar operations by delegating to specific
 * operation handlers to reduce complexity and improve maintainability.
 */
CommandResult<QVariant> ToolBarCommand::execute(const CommandContext& context) {
    // Extract and validate basic parameters
    auto widget_name = context.getParameter<QString>("widget");
    auto operation = context.getParameter<QString>("operation");

    // Validate required parameters
    auto validationResult = validateRequiredParameters(context, {"widget"});
    if (!validationResult.isSuccess()) {
        return validationResult;
    }

    // Find and validate toolbar widget
    auto* toolBar = findToolBar(widget_name);
    if (!toolBar) {
        return createWidgetNotFoundError("ToolBar", widget_name);
    }

    // Store state for undo functionality
    widget_name_ = widget_name;
    old_orientation_ = toolBar->orientation();
    operation_ = operation;

    // Route to appropriate operation handler
    if (operation == "addAction" || operation.isEmpty()) {
        return handleAddAction(context, toolBar);
    } else if (operation == "removeAction") {
        return handleRemoveAction(context, toolBar);
    } else if (operation == "setOrientation") {
        return handleSetOrientation(context, toolBar);
    } else if (operation == "setMovable") {
        return handleSetMovable(context, toolBar);
    }

    return CommandResult<QVariant>(
        QString("Unknown operation: %1").arg(operation));
}

CommandResult<QVariant> ToolBarCommand::undo(const CommandContext& context) {
    auto* toolBar = findToolBar(widget_name_);
    if (!toolBar) {
        return CommandResult<QVariant>(
            QString("ToolBar '%1' not found for undo").arg(widget_name_));
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
    return CommandMetadata("ToolBarCommand",
                           "Specialized command for ToolBar components");
}

QToolBar* ToolBarCommand::findToolBar(const QString& name) {
    return findWidget<QToolBar>(name);
}

/**
 * @brief Handle adding an action to the toolbar
 * @param context Command context containing action parameters
 * @param toolBar Target toolbar widget
 * @return CommandResult indicating success or failure
 */
CommandResult<QVariant> ToolBarCommand::handleAddAction(
    const CommandContext& context, QToolBar* toolBar) {
    auto text = context.getParameter<QString>("text");
    if (!context.hasParameter("text")) {
        return CommandResult<QVariant>(
            QString("Missing text parameter for addAction operation"));
    }

    [[maybe_unused]] auto action = toolBar->addAction(text);
    return CommandResult<QVariant>(
        QString("ToolBar action added successfully"));
}

/**
 * @brief Handle removing an action from the toolbar
 * @param context Command context containing action parameters
 * @param toolBar Target toolbar widget
 * @return CommandResult indicating success or failure
 */
CommandResult<QVariant> ToolBarCommand::handleRemoveAction(
    const CommandContext& context, QToolBar* toolBar) {
    auto text = context.getParameter<QString>("text");
    if (!context.hasParameter("text")) {
        return CommandResult<QVariant>(
            QString("Missing text parameter for removeAction operation"));
    }

    // Find and remove action by text
    for (auto* action : toolBar->actions()) {
        if (action->text() == text) {
            toolBar->removeAction(action);
            return CommandResult<QVariant>(
                QString("ToolBar action removed successfully"));
        }
    }
    return CommandResult<QVariant>(
        QString("Action with text '%1' not found").arg(text));
}

/**
 * @brief Handle setting toolbar orientation
 * @param context Command context containing orientation parameter
 * @param toolBar Target toolbar widget
 * @return CommandResult indicating success or failure
 */
CommandResult<QVariant> ToolBarCommand::handleSetOrientation(
    const CommandContext& context, QToolBar* toolBar) {
    auto orientation = context.getParameter<int>("orientation");
    if (!context.hasParameter("orientation")) {
        return CommandResult<QVariant>(QString(
            "Missing orientation parameter for setOrientation operation"));
    }

    toolBar->setOrientation(static_cast<Qt::Orientation>(orientation));
    return CommandResult<QVariant>(
        QString("ToolBar orientation set successfully"));
}

/**
 * @brief Handle setting toolbar movable state
 * @param context Command context containing movable parameter
 * @param toolBar Target toolbar widget
 * @return CommandResult indicating success or failure
 */
CommandResult<QVariant> ToolBarCommand::handleSetMovable(
    const CommandContext& context, QToolBar* toolBar) {
    auto movable = context.getParameter<bool>("movable");
    if (!context.hasParameter("movable")) {
        return CommandResult<QVariant>(
            QString("Missing movable parameter for setMovable operation"));
    }

    toolBar->setMovable(movable);
    return CommandResult<QVariant>(
        QString("ToolBar movable state set successfully"));
}

// ============================================================================
// TOOL BUTTON COMPONENTS
// ============================================================================

ToolButtonCommand::ToolButtonCommand(const CommandContext& context)
    : ICommand(nullptr) {}

CommandResult<QVariant> ToolButtonCommand::execute(
    const CommandContext& context) {
    auto widget_name = context.getParameter<QString>("widget");
    auto operation = context.getParameter<QString>("operation");

    // Validate required parameters
    auto validationResult = validateRequiredParameters(context, {"widget"});
    if (!validationResult.isSuccess()) {
        return validationResult;
    }

    auto* toolButton = findToolButton(widget_name);
    if (!toolButton) {
        return createWidgetNotFoundError("ToolButton", widget_name);
    }

    widget_name_ = widget_name;
    old_popup_mode_ = toolButton->popupMode();
    old_style_ = toolButton->toolButtonStyle();
    operation_ = operation;

    if (operation == "setDefaultAction" || operation.isEmpty()) {
        // Note: Setting default action requires QAction instance
        return CommandResult<QVariant>(QString(
            "setDefaultAction operation requires QAction implementation"));
    } else if (operation == "setPopupMode") {
        auto mode = context.getParameter<int>("mode");
        if (context.hasParameter("mode")) {
            toolButton->setPopupMode(
                static_cast<QToolButton::ToolButtonPopupMode>(mode));
            return CommandResult<QVariant>(
                QString("ToolButton popup mode set successfully"));
        }
        return CommandResult<QVariant>(
            QString("Missing mode parameter for setPopupMode operation"));
    } else if (operation == "setToolButtonStyle") {
        auto style = context.getParameter<int>("style");
        if (context.hasParameter("style")) {
            toolButton->setToolButtonStyle(
                static_cast<Qt::ToolButtonStyle>(style));
            return CommandResult<QVariant>(
                QString("ToolButton style set successfully"));
        }
        return CommandResult<QVariant>(QString(
            "Missing style parameter for setToolButtonStyle operation"));
    }

    return CommandResult<QVariant>(
        QString("Unknown operation: %1").arg(operation));
}

CommandResult<QVariant> ToolButtonCommand::undo(const CommandContext& context) {
    auto* toolButton = findToolButton(widget_name_);
    if (!toolButton) {
        return CommandResult<QVariant>(
            QString("ToolButton '%1' not found for undo").arg(widget_name_));
    }

    if (operation_ == "setPopupMode") {
        toolButton->setPopupMode(old_popup_mode_);
    } else if (operation_ == "setToolButtonStyle") {
        toolButton->setToolButtonStyle(old_style_);
    }
    return CommandResult<QVariant>(QString("ToolButton undo successful"));
}

bool ToolButtonCommand::canUndo(const CommandContext& context) const {
    return !widget_name_.isEmpty() &&
           (operation_ == "setPopupMode" || operation_ == "setToolButtonStyle");
}

CommandMetadata ToolButtonCommand::getMetadata() const {
    return CommandMetadata("ToolButtonCommand",
                           "Specialized command for ToolButton components");
}

QToolButton* ToolButtonCommand::findToolButton(const QString& name) {
    return findWidget<QToolButton>(name);
}

}  // namespace ComponentCommands
}  // namespace Command
}  // namespace DeclarativeUI
