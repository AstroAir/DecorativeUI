#include "ComponentCommands.hpp"
#include <QApplication>
#include <QDebug>
#include <QGroupBox>
#include <QFrame>
#include <QScrollArea>
#include <QSplitter>
#include <QDockWidget>

namespace DeclarativeUI {
namespace Command {
namespace ComponentCommands {

// ============================================================================
// GROUP BOX COMPONENTS
// ============================================================================

GroupBoxCommand::GroupBoxCommand(const CommandContext& context)
    : ICommand(nullptr) {}

CommandResult<QVariant> GroupBoxCommand::execute(const CommandContext& context) {
    auto widget_name = context.getParameter<QString>("widget");
    auto operation = context.getParameter<QString>("operation");

    if (!context.hasParameter("widget")) {
        return CommandResult<QVariant>(QString("Missing required parameter: widget"));
    }

    auto* groupBox = findGroupBox(widget_name);
    if (!groupBox) {
        return CommandResult<QVariant>(QString("GroupBox '%1' not found").arg(widget_name));
    }

    widget_name_ = widget_name;
    old_title_ = groupBox->title();
    old_checked_ = groupBox->isChecked();
    operation_ = operation;

    if (operation == "setTitle" || operation.isEmpty()) {
        auto title = context.getParameter<QString>("title");
        if (context.hasParameter("title")) {
            groupBox->setTitle(title);
            return CommandResult<QVariant>(QString("GroupBox title set successfully"));
        }
        return CommandResult<QVariant>(QString("Missing title parameter for setTitle operation"));
    } else if (operation == "setCheckable") {
        auto checkable = context.getParameter<bool>("checkable");
        if (context.hasParameter("checkable")) {
            groupBox->setCheckable(checkable);
            return CommandResult<QVariant>(QString("GroupBox checkable state set successfully"));
        }
        return CommandResult<QVariant>(QString("Missing checkable parameter for setCheckable operation"));
    } else if (operation == "setChecked") {
        auto checked = context.getParameter<bool>("checked");
        if (context.hasParameter("checked")) {
            groupBox->setChecked(checked);
            return CommandResult<QVariant>(QString("GroupBox checked state set successfully"));
        }
        return CommandResult<QVariant>(QString("Missing checked parameter for setChecked operation"));
    }

    return CommandResult<QVariant>(QString("Unknown operation: %1").arg(operation));
}

CommandResult<QVariant> GroupBoxCommand::undo(const CommandContext& context) {
    auto* groupBox = findGroupBox(widget_name_);
    if (!groupBox) {
        return CommandResult<QVariant>(QString("GroupBox '%1' not found for undo").arg(widget_name_));
    }

    if (operation_ == "setTitle") {
        groupBox->setTitle(old_title_);
    } else if (operation_ == "setChecked") {
        groupBox->setChecked(old_checked_);
    }
    return CommandResult<QVariant>(QString("GroupBox undo successful"));
}

bool GroupBoxCommand::canUndo(const CommandContext& context) const {
    return !widget_name_.isEmpty() && (operation_ == "setTitle" || operation_ == "setChecked");
}

CommandMetadata GroupBoxCommand::getMetadata() const {
    return CommandMetadata("GroupBoxCommand", "Specialized command for GroupBox components");
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

// ============================================================================
// FRAME COMPONENTS
// ============================================================================

FrameCommand::FrameCommand(const CommandContext& context)
    : ICommand(nullptr) {}

CommandResult<QVariant> FrameCommand::execute(const CommandContext& context) {
    auto widget_name = context.getParameter<QString>("widget");
    auto operation = context.getParameter<QString>("operation");

    if (!context.hasParameter("widget")) {
        return CommandResult<QVariant>(QString("Missing required parameter: widget"));
    }

    auto* frame = findFrame(widget_name);
    if (!frame) {
        return CommandResult<QVariant>(QString("Frame '%1' not found").arg(widget_name));
    }

    if (operation == "setFrameStyle" || operation.isEmpty()) {
        auto style = context.getParameter<int>("style");
        if (context.hasParameter("style")) {
            frame->setFrameStyle(style);
            return CommandResult<QVariant>(QString("Frame style set successfully"));
        }
        return CommandResult<QVariant>(QString("Missing style parameter for setFrameStyle operation"));
    } else if (operation == "setLineWidth") {
        auto width = context.getParameter<int>("width");
        if (context.hasParameter("width")) {
            frame->setLineWidth(width);
            return CommandResult<QVariant>(QString("Frame line width set successfully"));
        }
        return CommandResult<QVariant>(QString("Missing width parameter for setLineWidth operation"));
    } else if (operation == "setMidLineWidth") {
        auto width = context.getParameter<int>("width");
        if (context.hasParameter("width")) {
            frame->setMidLineWidth(width);
            return CommandResult<QVariant>(QString("Frame mid line width set successfully"));
        }
        return CommandResult<QVariant>(QString("Missing width parameter for setMidLineWidth operation"));
    }

    return CommandResult<QVariant>(QString("Unknown operation: %1").arg(operation));
}

CommandMetadata FrameCommand::getMetadata() const {
    return CommandMetadata("FrameCommand", "Specialized command for Frame components");
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

// ============================================================================
// SCROLL AREA COMPONENTS
// ============================================================================

ScrollAreaCommand::ScrollAreaCommand(const CommandContext& context)
    : ICommand(nullptr) {}

CommandResult<QVariant> ScrollAreaCommand::execute(const CommandContext& context) {
    auto widget_name = context.getParameter<QString>("widget");
    auto operation = context.getParameter<QString>("operation");

    if (!context.hasParameter("widget")) {
        return CommandResult<QVariant>(QString("Missing required parameter: widget"));
    }

    auto* scrollArea = findScrollArea(widget_name);
    if (!scrollArea) {
        return CommandResult<QVariant>(QString("ScrollArea '%1' not found").arg(widget_name));
    }

    if (operation == "setWidgetResizable" || operation.isEmpty()) {
        auto resizable = context.getParameter<bool>("resizable");
        if (context.hasParameter("resizable")) {
            scrollArea->setWidgetResizable(resizable);
            return CommandResult<QVariant>(QString("ScrollArea widget resizable set successfully"));
        }
        return CommandResult<QVariant>(QString("Missing resizable parameter for setWidgetResizable operation"));
    } else if (operation == "setScrollBarPolicy") {
        auto hPolicy = context.getParameter<int>("horizontalPolicy");
        auto vPolicy = context.getParameter<int>("verticalPolicy");
        if (context.hasParameter("horizontalPolicy")) {
            scrollArea->setHorizontalScrollBarPolicy(static_cast<Qt::ScrollBarPolicy>(hPolicy));
        }
        if (context.hasParameter("verticalPolicy")) {
            scrollArea->setVerticalScrollBarPolicy(static_cast<Qt::ScrollBarPolicy>(vPolicy));
        }
        return CommandResult<QVariant>(QString("ScrollArea scroll bar policy set successfully"));
    }

    return CommandResult<QVariant>(QString("Unknown operation: %1").arg(operation));
}

CommandMetadata ScrollAreaCommand::getMetadata() const {
    return CommandMetadata("ScrollAreaCommand", "Specialized command for ScrollArea components");
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
