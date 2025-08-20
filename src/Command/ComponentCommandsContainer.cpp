/**
 * @file ComponentCommandsContainer.cpp
 * @brief Implementation of container component commands with reduced
 * complexity.
 *
 * This file contains refactored command implementations for container widgets:
 * - GroupBoxCommand: Handles GroupBox operations (setTitle, setCheckable,
 * setChecked)
 * - FrameCommand: Handles Frame operations (setFrameStyle, setLineWidth,
 * setMidLineWidth)
 *
 * All commands follow the improved pattern with:
 * - Reduced cyclomatic complexity (< 10 per function)
 * - Extracted operation handlers for better maintainability
 * - Standardized error handling and success reporting
 *
 * @author DeclarativeUI Team
 * @version 2.0
 * @date 2024
 */

#include <QApplication>
#include <QDebug>
#include <QDockWidget>
#include <QFrame>
#include <QGroupBox>
#include <QScrollArea>
#include <QSplitter>
#include "ComponentCommands.hpp"

namespace DeclarativeUI {
namespace Command {
namespace ComponentCommands {

// ============================================================================
// GROUP BOX COMPONENTS
// ============================================================================

/**
 * @brief Constructs a GroupBoxCommand with the given context.
 * @param context The command execution context
 */
GroupBoxCommand::GroupBoxCommand(const CommandContext& context)
    : ICommand(nullptr) {}

CommandResult<QVariant> GroupBoxCommand::execute(
    const CommandContext& context) {
    // Validate required parameters
    if (!context.hasParameter("widget")) {
        return CommandResult<QVariant>(
            QString("Missing required parameter: widget"));
    }

    auto widget_name = context.getParameter<QString>("widget");
    auto operation = context.getParameter<QString>("operation");

    // Find and validate widget
    auto* groupBox = findGroupBox(widget_name);
    if (!groupBox) {
        return createWidgetNotFoundError("GroupBox", widget_name);
    }

    // Store state for undo functionality
    widget_name_ = widget_name;
    old_title_ = groupBox->title();
    old_checked_ = groupBox->isChecked();
    operation_ = operation;

    // Route to appropriate operation handler
    if (operation == "setTitle" || operation.isEmpty()) {
        return handleSetTitle(context, groupBox);
    } else if (operation == "setCheckable") {
        return handleSetCheckable(context, groupBox);
    } else if (operation == "setChecked") {
        return handleSetChecked(context, groupBox);
    }

    return CommandResult<QVariant>(
        QString("Unknown operation: %1").arg(operation));
}

CommandResult<QVariant> GroupBoxCommand::undo(const CommandContext& context) {
    auto* groupBox = findGroupBox(widget_name_);
    if (!groupBox) {
        return CommandResult<QVariant>(
            QString("GroupBox '%1' not found for undo").arg(widget_name_));
    }

    if (operation_ == "setTitle") {
        groupBox->setTitle(old_title_);
    } else if (operation_ == "setChecked") {
        groupBox->setChecked(old_checked_);
    }
    return CommandResult<QVariant>(QString("GroupBox undo successful"));
}

bool GroupBoxCommand::canUndo(const CommandContext& context) const {
    return !widget_name_.isEmpty() &&
           (operation_ == "setTitle" || operation_ == "setChecked");
}

CommandMetadata GroupBoxCommand::getMetadata() const {
    return CommandMetadata("GroupBoxCommand",
                           "Specialized command for GroupBox components");
}

QGroupBox* GroupBoxCommand::findGroupBox(const QString& name) {
    for (auto* widget : QApplication::allWidgets()) {
        if (auto* groupBox = qobject_cast<QGroupBox*>(widget)) {
            if (groupBox->objectName() == name) {
                return groupBox;
            }
        }
    }
    return nullptr;
}

CommandResult<QVariant> GroupBoxCommand::handleSetTitle(
    const CommandContext& context, QGroupBox* widget) {
    if (!context.hasParameter("title")) {
        return CommandResult<QVariant>(
            QString("Missing title parameter for setTitle operation"));
    }

    auto title = context.getParameter<QString>("title");
    widget->setTitle(title);
    return createSuccessResult("GroupBox", "title set");
}

CommandResult<QVariant> GroupBoxCommand::handleSetCheckable(
    const CommandContext& context, QGroupBox* widget) {
    if (!context.hasParameter("checkable")) {
        return CommandResult<QVariant>(
            QString("Missing checkable parameter for setCheckable operation"));
    }

    auto checkable = context.getParameter<bool>("checkable");
    widget->setCheckable(checkable);
    return createSuccessResult("GroupBox", "checkable state set");
}

CommandResult<QVariant> GroupBoxCommand::handleSetChecked(
    const CommandContext& context, QGroupBox* widget) {
    if (!context.hasParameter("checked")) {
        return CommandResult<QVariant>(
            QString("Missing checked parameter for setChecked operation"));
    }

    auto checked = context.getParameter<bool>("checked");
    widget->setChecked(checked);
    return createSuccessResult("GroupBox", "checked state set");
}

// ============================================================================
// FRAME COMPONENTS
// ============================================================================

FrameCommand::FrameCommand(const CommandContext& context) : ICommand(nullptr) {}

CommandResult<QVariant> FrameCommand::execute(const CommandContext& context) {
    // Validate required parameters
    if (!context.hasParameter("widget")) {
        return CommandResult<QVariant>(
            QString("Missing required parameter: widget"));
    }

    auto widget_name = context.getParameter<QString>("widget");
    auto operation = context.getParameter<QString>("operation");

    // Find and validate widget
    auto* frame = findFrame(widget_name);
    if (!frame) {
        return createWidgetNotFoundError("Frame", widget_name);
    }

    // Route to appropriate operation handler
    if (operation == "setFrameStyle" || operation.isEmpty()) {
        return handleSetFrameStyle(context, frame);
    } else if (operation == "setLineWidth") {
        return handleSetLineWidth(context, frame);
    } else if (operation == "setMidLineWidth") {
        return handleSetMidLineWidth(context, frame);
    }

    return CommandResult<QVariant>(
        QString("Unknown operation: %1").arg(operation));
}

CommandMetadata FrameCommand::getMetadata() const {
    return CommandMetadata("FrameCommand",
                           "Specialized command for Frame components");
}

QFrame* FrameCommand::findFrame(const QString& name) {
    for (auto* widget : QApplication::allWidgets()) {
        if (auto* frame = qobject_cast<QFrame*>(widget)) {
            if (frame->objectName() == name) {
                return frame;
            }
        }
    }
    return nullptr;
}

CommandResult<QVariant> FrameCommand::handleSetFrameStyle(
    const CommandContext& context, QFrame* widget) {
    if (!context.hasParameter("style")) {
        return CommandResult<QVariant>(
            QString("Missing style parameter for setFrameStyle operation"));
    }

    auto style = context.getParameter<int>("style");
    widget->setFrameStyle(style);
    return createSuccessResult("Frame", "style set");
}

CommandResult<QVariant> FrameCommand::handleSetLineWidth(
    const CommandContext& context, QFrame* widget) {
    if (!context.hasParameter("width")) {
        return CommandResult<QVariant>(
            QString("Missing width parameter for setLineWidth operation"));
    }

    auto width = context.getParameter<int>("width");
    widget->setLineWidth(width);
    return createSuccessResult("Frame", "line width set");
}

CommandResult<QVariant> FrameCommand::handleSetMidLineWidth(
    const CommandContext& context, QFrame* widget) {
    if (!context.hasParameter("width")) {
        return CommandResult<QVariant>(
            QString("Missing width parameter for setMidLineWidth operation"));
    }

    auto width = context.getParameter<int>("width");
    widget->setMidLineWidth(width);
    return createSuccessResult("Frame", "mid line width set");
}

// ============================================================================
// SCROLL AREA COMPONENTS
// ============================================================================

ScrollAreaCommand::ScrollAreaCommand(const CommandContext& context)
    : ICommand(nullptr) {}

CommandResult<QVariant> ScrollAreaCommand::execute(
    const CommandContext& context) {
    auto widget_name = context.getParameter<QString>("widget");
    auto operation = context.getParameter<QString>("operation");

    if (!context.hasParameter("widget")) {
        return CommandResult<QVariant>(
            QString("Missing required parameter: widget"));
    }

    auto* scrollArea = findScrollArea(widget_name);
    if (!scrollArea) {
        return CommandResult<QVariant>(
            QString("ScrollArea '%1' not found").arg(widget_name));
    }

    if (operation == "setWidgetResizable" || operation.isEmpty()) {
        auto resizable = context.getParameter<bool>("resizable");
        if (context.hasParameter("resizable")) {
            scrollArea->setWidgetResizable(resizable);
            return CommandResult<QVariant>(
                QString("ScrollArea widget resizable set successfully"));
        }
        return CommandResult<QVariant>(QString(
            "Missing resizable parameter for setWidgetResizable operation"));
    } else if (operation == "setScrollBarPolicy") {
        auto hPolicy = context.getParameter<int>("horizontalPolicy");
        auto vPolicy = context.getParameter<int>("verticalPolicy");
        if (context.hasParameter("horizontalPolicy")) {
            scrollArea->setHorizontalScrollBarPolicy(
                static_cast<Qt::ScrollBarPolicy>(hPolicy));
        }
        if (context.hasParameter("verticalPolicy")) {
            scrollArea->setVerticalScrollBarPolicy(
                static_cast<Qt::ScrollBarPolicy>(vPolicy));
        }
        return CommandResult<QVariant>(
            QString("ScrollArea scroll bar policy set successfully"));
    }

    return CommandResult<QVariant>(
        QString("Unknown operation: %1").arg(operation));
}

CommandMetadata ScrollAreaCommand::getMetadata() const {
    return CommandMetadata("ScrollAreaCommand",
                           "Specialized command for ScrollArea components");
}

QScrollArea* ScrollAreaCommand::findScrollArea(const QString& name) {
    for (auto* widget : QApplication::allWidgets()) {
        if (auto* scrollArea = qobject_cast<QScrollArea*>(widget)) {
            if (scrollArea->objectName() == name) {
                return scrollArea;
            }
        }
    }
    return nullptr;
}

}  // namespace ComponentCommands
}  // namespace Command
}  // namespace DeclarativeUI
