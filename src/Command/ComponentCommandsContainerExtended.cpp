#include "ComponentCommands.hpp"
#include <QApplication>
#include <QDebug>
#include <QSplitter>
#include <QDockWidget>

namespace DeclarativeUI {
namespace Command {
namespace ComponentCommands {

// ============================================================================
// SPLITTER COMPONENTS
// ============================================================================

SplitterCommand::SplitterCommand(const CommandContext& context)
    : ICommand(nullptr) {}

CommandResult<QVariant> SplitterCommand::execute(const CommandContext& context) {
    auto widget_name = context.getParameter<QString>("widget");
    auto operation = context.getParameter<QString>("operation");

    if (!context.hasParameter("widget")) {
        return CommandResult<QVariant>(QString("Missing required parameter: widget"));
    }

    auto* splitter = findSplitter(widget_name);
    if (!splitter) {
        return CommandResult<QVariant>(QString("Splitter '%1' not found").arg(widget_name));
    }

    widget_name_ = widget_name;
    old_sizes_ = splitter->sizes();
    operation_ = operation;

    if (operation == "setSizes" || operation.isEmpty()) {
        auto sizes = context.getParameter<QList<int>>("sizes");
        if (context.hasParameter("sizes")) {
            new_sizes_ = sizes;
            splitter->setSizes(sizes);
            return CommandResult<QVariant>(QString("Splitter sizes set successfully"));
        }
        return CommandResult<QVariant>(QString("Missing sizes parameter for setSizes operation"));
    } else if (operation == "setOrientation") {
        auto orientation = context.getParameter<int>("orientation");
        if (context.hasParameter("orientation")) {
            splitter->setOrientation(static_cast<Qt::Orientation>(orientation));
            return CommandResult<QVariant>(QString("Splitter orientation set successfully"));
        }
        return CommandResult<QVariant>(QString("Missing orientation parameter for setOrientation operation"));
    } else if (operation == "setChildrenCollapsible") {
        auto collapsible = context.getParameter<bool>("collapsible");
        if (context.hasParameter("collapsible")) {
            splitter->setChildrenCollapsible(collapsible);
            return CommandResult<QVariant>(QString("Splitter children collapsible set successfully"));
        }
        return CommandResult<QVariant>(QString("Missing collapsible parameter for setChildrenCollapsible operation"));
    }

    return CommandResult<QVariant>(QString("Unknown operation: %1").arg(operation));
}

CommandResult<QVariant> SplitterCommand::undo(const CommandContext& context) {
    auto* splitter = findSplitter(widget_name_);
    if (!splitter) {
        return CommandResult<QVariant>(QString("Splitter '%1' not found for undo").arg(widget_name_));
    }

    if (operation_ == "setSizes") {
        splitter->setSizes(old_sizes_);
    }
    return CommandResult<QVariant>(QString("Splitter undo successful"));
}

bool SplitterCommand::canUndo(const CommandContext& context) const {
    return !widget_name_.isEmpty() && operation_ == "setSizes";
}

CommandMetadata SplitterCommand::getMetadata() const {
    return CommandMetadata("SplitterCommand", "Specialized command for Splitter components");
}

QSplitter* SplitterCommand::findSplitter(const QString& name) {
    for (auto* widget : QApplication::allWidgets()) {
        if (auto* splitter = qobject_cast<QSplitter*>(widget)) {
            if (splitter->objectName() == name) {
                return splitter;
            }
        }
    }
    return nullptr;
}

// ============================================================================
// DOCK WIDGET COMPONENTS
// ============================================================================

DockWidgetCommand::DockWidgetCommand(const CommandContext& context)
    : ICommand(nullptr) {}

CommandResult<QVariant> DockWidgetCommand::execute(const CommandContext& context) {
    auto widget_name = context.getParameter<QString>("widget");
    auto operation = context.getParameter<QString>("operation");

    if (!context.hasParameter("widget")) {
        return CommandResult<QVariant>(QString("Missing required parameter: widget"));
    }

    auto* dockWidget = findDockWidget(widget_name);
    if (!dockWidget) {
        return CommandResult<QVariant>(QString("DockWidget '%1' not found").arg(widget_name));
    }

    widget_name_ = widget_name;
    old_floating_ = dockWidget->isFloating();
    operation_ = operation;

    if (operation == "setFloating" || operation.isEmpty()) {
        auto floating = context.getParameter<bool>("floating");
        if (context.hasParameter("floating")) {
            dockWidget->setFloating(floating);
            return CommandResult<QVariant>(QString("DockWidget floating state set successfully"));
        }
        return CommandResult<QVariant>(QString("Missing floating parameter for setFloating operation"));
    } else if (operation == "setAllowedAreas") {
        auto areas = context.getParameter<int>("areas");
        if (context.hasParameter("areas")) {
            dockWidget->setAllowedAreas(static_cast<Qt::DockWidgetAreas>(areas));
            return CommandResult<QVariant>(QString("DockWidget allowed areas set successfully"));
        }
        return CommandResult<QVariant>(QString("Missing areas parameter for setAllowedAreas operation"));
    } else if (operation == "setFeatures") {
        auto features = context.getParameter<int>("features");
        if (context.hasParameter("features")) {
            dockWidget->setFeatures(static_cast<QDockWidget::DockWidgetFeatures>(features));
            return CommandResult<QVariant>(QString("DockWidget features set successfully"));
        }
        return CommandResult<QVariant>(QString("Missing features parameter for setFeatures operation"));
    }

    return CommandResult<QVariant>(QString("Unknown operation: %1").arg(operation));
}

CommandResult<QVariant> DockWidgetCommand::undo(const CommandContext& context) {
    auto* dockWidget = findDockWidget(widget_name_);
    if (!dockWidget) {
        return CommandResult<QVariant>(QString("DockWidget '%1' not found for undo").arg(widget_name_));
    }

    if (operation_ == "setFloating") {
        dockWidget->setFloating(old_floating_);
    }
    return CommandResult<QVariant>(QString("DockWidget undo successful"));
}

bool DockWidgetCommand::canUndo(const CommandContext& context) const {
    return !widget_name_.isEmpty() && operation_ == "setFloating";
}

CommandMetadata DockWidgetCommand::getMetadata() const {
    return CommandMetadata("DockWidgetCommand", "Specialized command for DockWidget components");
}

QDockWidget* DockWidgetCommand::findDockWidget(const QString& name) {
    for (auto* widget : QApplication::allWidgets()) {
        if (auto* dockWidget = qobject_cast<QDockWidget*>(widget)) {
            if (dockWidget->objectName() == name) {
                return dockWidget;
            }
        }
    }
    return nullptr;
}

}  // namespace ComponentCommands
}  // namespace Command
}  // namespace DeclarativeUI
