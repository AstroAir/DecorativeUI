#include <QApplication>
#include <QDebug>
#include <QDockWidget>
#include <QSplitter>
#include "ComponentCommands.hpp"

namespace DeclarativeUI {
namespace Command {
namespace ComponentCommands {

// ============================================================================
// SPLITTER COMPONENTS
// ============================================================================

SplitterCommand::SplitterCommand(const CommandContext& context)
    : ICommand(nullptr) {}

CommandResult<QVariant> SplitterCommand::execute(
    const CommandContext& context) {
    // Validate required parameters
    auto validationResult = validateRequiredParameters(context, {"widget"});
    if (!validationResult.isSuccess()) {
        return validationResult;
    }

    auto widget_name = context.getParameter<QString>("widget");
    auto operation = context.getParameter<QString>("operation");

    auto* splitter = findSplitter(widget_name);
    if (!splitter) {
        return createWidgetNotFoundError("Splitter", widget_name);
    }

    // Store state for undo functionality
    widget_name_ = widget_name;
    old_sizes_ = splitter->sizes();
    operation_ = operation;

    // Route to appropriate operation handler
    if (operation == "setSizes" || operation.isEmpty()) {
        return handleSetSizes(context, splitter);
    } else if (operation == "setOrientation") {
        return handleSetOrientation(context, splitter);
    } else if (operation == "setChildrenCollapsible") {
        return handleSetChildrenCollapsible(context, splitter);
    }

    return CommandResult<QVariant>(
        QString("Unknown operation: %1").arg(operation));
}

CommandResult<QVariant> SplitterCommand::undo(const CommandContext& context) {
    auto* splitter = findSplitter(widget_name_);
    if (!splitter) {
        return CommandResult<QVariant>(
            QString("Splitter '%1' not found for undo").arg(widget_name_));
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
    return CommandMetadata("SplitterCommand",
                           "Specialized command for Splitter components");
}

QSplitter* SplitterCommand::findSplitter(const QString& name) {
    return findWidget<QSplitter>(name);
}

CommandResult<QVariant> SplitterCommand::handleSetSizes(
    const CommandContext& context, QSplitter* widget) {
    if (!context.hasParameter("sizes")) {
        return CommandResult<QVariant>(
            QString("Missing sizes parameter for setSizes operation"));
    }

    auto sizes = context.getParameter<QList<int>>("sizes");
    new_sizes_ = sizes;
    widget->setSizes(sizes);
    return createSuccessResult("Splitter", "sizes set");
}

CommandResult<QVariant> SplitterCommand::handleSetOrientation(
    const CommandContext& context, QSplitter* widget) {
    if (!context.hasParameter("orientation")) {
        return CommandResult<QVariant>(QString(
            "Missing orientation parameter for setOrientation operation"));
    }

    auto orientation = context.getParameter<int>("orientation");
    widget->setOrientation(static_cast<Qt::Orientation>(orientation));
    return createSuccessResult("Splitter", "orientation set");
}

CommandResult<QVariant> SplitterCommand::handleSetChildrenCollapsible(
    const CommandContext& context, QSplitter* widget) {
    if (!context.hasParameter("collapsible")) {
        return CommandResult<QVariant>(
            QString("Missing collapsible parameter for setChildrenCollapsible "
                    "operation"));
    }

    auto collapsible = context.getParameter<bool>("collapsible");
    widget->setChildrenCollapsible(collapsible);
    return createSuccessResult("Splitter", "children collapsible set");
}

// ============================================================================
// DOCK WIDGET COMPONENTS
// ============================================================================

DockWidgetCommand::DockWidgetCommand(const CommandContext& context)
    : ICommand(nullptr) {}

CommandResult<QVariant> DockWidgetCommand::execute(
    const CommandContext& context) {
    // Validate required parameters
    auto validationResult = validateRequiredParameters(context, {"widget"});
    if (!validationResult.isSuccess()) {
        return validationResult;
    }

    auto widget_name = context.getParameter<QString>("widget");
    auto operation = context.getParameter<QString>("operation");

    auto* dockWidget = findDockWidget(widget_name);
    if (!dockWidget) {
        return createWidgetNotFoundError("DockWidget", widget_name);
    }

    // Store state for undo functionality
    widget_name_ = widget_name;
    old_floating_ = dockWidget->isFloating();
    operation_ = operation;

    // Route to appropriate operation handler
    if (operation == "setFloating" || operation.isEmpty()) {
        return handleSetFloating(context, dockWidget);
    } else if (operation == "setAllowedAreas") {
        return handleSetAllowedAreas(context, dockWidget);
    } else if (operation == "setFeatures") {
        return handleSetFeatures(context, dockWidget);
    }

    return CommandResult<QVariant>(
        QString("Unknown operation: %1").arg(operation));
}

CommandResult<QVariant> DockWidgetCommand::undo(const CommandContext& context) {
    auto* dockWidget = findDockWidget(widget_name_);
    if (!dockWidget) {
        return CommandResult<QVariant>(
            QString("DockWidget '%1' not found for undo").arg(widget_name_));
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
    return CommandMetadata("DockWidgetCommand",
                           "Specialized command for DockWidget components");
}

QDockWidget* DockWidgetCommand::findDockWidget(const QString& name) {
    return findWidget<QDockWidget>(name);
}

CommandResult<QVariant> DockWidgetCommand::handleSetFloating(
    const CommandContext& context, QDockWidget* widget) {
    if (!context.hasParameter("floating")) {
        return CommandResult<QVariant>(
            QString("Missing floating parameter for setFloating operation"));
    }

    auto floating = context.getParameter<bool>("floating");
    widget->setFloating(floating);
    return createSuccessResult("DockWidget", "floating state set");
}

CommandResult<QVariant> DockWidgetCommand::handleSetAllowedAreas(
    const CommandContext& context, QDockWidget* widget) {
    if (!context.hasParameter("areas")) {
        return CommandResult<QVariant>(
            QString("Missing areas parameter for setAllowedAreas operation"));
    }

    auto areas = context.getParameter<int>("areas");
    widget->setAllowedAreas(static_cast<Qt::DockWidgetAreas>(areas));
    return createSuccessResult("DockWidget", "allowed areas set");
}

CommandResult<QVariant> DockWidgetCommand::handleSetFeatures(
    const CommandContext& context, QDockWidget* widget) {
    if (!context.hasParameter("features")) {
        return CommandResult<QVariant>(
            QString("Missing features parameter for setFeatures operation"));
    }

    auto features = context.getParameter<int>("features");
    widget->setFeatures(static_cast<QDockWidget::DockWidgetFeatures>(features));
    return createSuccessResult("DockWidget", "features set");
}

}  // namespace ComponentCommands
}  // namespace Command
}  // namespace DeclarativeUI
