#include <QApplication>
#include <QCalendarWidget>
#include <QDebug>
#include <QLCDNumber>
#include "ComponentCommands.hpp"

namespace DeclarativeUI {
namespace Command {
namespace ComponentCommands {

// ============================================================================
// LCD NUMBER COMPONENTS
// ============================================================================

LCDNumberCommand::LCDNumberCommand(const CommandContext& context)
    : ICommand(nullptr) {}

CommandResult<QVariant> LCDNumberCommand::execute(
    const CommandContext& context) {
    // Validate required parameters
    auto validationResult = validateRequiredParameters(context, {"widget"});
    if (!validationResult.isSuccess()) {
        return validationResult;
    }

    auto widget_name = context.getParameter<QString>("widget");
    auto operation = context.getParameter<QString>("operation");

    auto* lcdNumber = findLCDNumber(widget_name);
    if (!lcdNumber) {
        return createWidgetNotFoundError("LCDNumber", widget_name);
    }

    // Store state for undo functionality
    widget_name_ = widget_name;
    old_value_ = lcdNumber->value();
    operation_ = operation;

    // Route to appropriate operation handler
    if (operation == "display" || operation.isEmpty()) {
        return handleDisplay(context, lcdNumber);
    } else if (operation == "setDigitCount") {
        return handleSetDigitCount(context, lcdNumber);
    } else if (operation == "setMode") {
        return handleSetMode(context, lcdNumber);
    } else if (operation == "setSegmentStyle") {
        return handleSetSegmentStyle(context, lcdNumber);
    }

    return CommandResult<QVariant>(
        QString("Unknown operation: %1").arg(operation));
}

CommandResult<QVariant> LCDNumberCommand::undo(const CommandContext& context) {
    auto* lcdNumber = findLCDNumber(widget_name_);
    if (!lcdNumber) {
        return CommandResult<QVariant>(
            QString("LCDNumber '%1' not found for undo").arg(widget_name_));
    }

    lcdNumber->display(old_value_);
    return CommandResult<QVariant>(QString("LCDNumber undo successful"));
}

bool LCDNumberCommand::canUndo(const CommandContext& context) const {
    return !widget_name_.isEmpty();
}

CommandMetadata LCDNumberCommand::getMetadata() const {
    return CommandMetadata("LCDNumberCommand",
                           "Specialized command for LCDNumber components");
}

QLCDNumber* LCDNumberCommand::findLCDNumber(const QString& name) {
    return findWidget<QLCDNumber>(name);
}

CommandResult<QVariant> LCDNumberCommand::handleDisplay(
    const CommandContext& context, QLCDNumber* widget) {
    if (!context.hasParameter("value")) {
        return CommandResult<QVariant>(
            QString("Missing value parameter for display operation"));
    }

    auto value = context.getParameter<double>("value");
    new_value_ = value;
    widget->display(value);
    return createSuccessResult("LCDNumber", "value displayed");
}

CommandResult<QVariant> LCDNumberCommand::handleSetDigitCount(
    const CommandContext& context, QLCDNumber* widget) {
    if (!context.hasParameter("count")) {
        return CommandResult<QVariant>(
            QString("Missing count parameter for setDigitCount operation"));
    }

    auto count = context.getParameter<int>("count");
    widget->setDigitCount(count);
    return createSuccessResult("LCDNumber", "digit count set");
}

CommandResult<QVariant> LCDNumberCommand::handleSetMode(
    const CommandContext& context, QLCDNumber* widget) {
    if (!context.hasParameter("mode")) {
        return CommandResult<QVariant>(
            QString("Missing mode parameter for setMode operation"));
    }

    auto mode = context.getParameter<int>("mode");
    widget->setMode(static_cast<QLCDNumber::Mode>(mode));
    return createSuccessResult("LCDNumber", "mode set");
}

CommandResult<QVariant> LCDNumberCommand::handleSetSegmentStyle(
    const CommandContext& context, QLCDNumber* widget) {
    if (!context.hasParameter("style")) {
        return CommandResult<QVariant>(
            QString("Missing style parameter for setSegmentStyle operation"));
    }

    auto style = context.getParameter<int>("style");
    widget->setSegmentStyle(static_cast<QLCDNumber::SegmentStyle>(style));
    return createSuccessResult("LCDNumber", "segment style set");
}

// ============================================================================
// CALENDAR COMPONENTS
// ============================================================================

CalendarCommand::CalendarCommand(const CommandContext& context)
    : ICommand(nullptr) {}

CommandResult<QVariant> CalendarCommand::execute(
    const CommandContext& context) {
    // Validate required parameters
    auto validationResult = validateRequiredParameters(context, {"widget"});
    if (!validationResult.isSuccess()) {
        return validationResult;
    }

    auto widget_name = context.getParameter<QString>("widget");
    auto operation = context.getParameter<QString>("operation");

    auto* calendar = findCalendar(widget_name);
    if (!calendar) {
        return createWidgetNotFoundError("Calendar", widget_name);
    }

    // Store state for undo functionality
    widget_name_ = widget_name;
    old_date_ = calendar->selectedDate();
    operation_ = operation;

    // Route to appropriate operation handler
    if (operation == "setSelectedDate" || operation.isEmpty()) {
        return handleSetSelectedDate(context, calendar);
    } else if (operation == "setDateRange") {
        return handleSetDateRange(context, calendar);
    } else if (operation == "setGridVisible") {
        return handleSetGridVisible(context, calendar);
    }

    return CommandResult<QVariant>(
        QString("Unknown operation: %1").arg(operation));
}

CommandResult<QVariant> CalendarCommand::undo(const CommandContext& context) {
    auto* calendar = findCalendar(widget_name_);
    if (!calendar) {
        return CommandResult<QVariant>(
            QString("Calendar '%1' not found for undo").arg(widget_name_));
    }

    calendar->setSelectedDate(old_date_);
    return CommandResult<QVariant>(QString("Calendar undo successful"));
}

bool CalendarCommand::canUndo(const CommandContext& context) const {
    return !widget_name_.isEmpty();
}

CommandMetadata CalendarCommand::getMetadata() const {
    return CommandMetadata("CalendarCommand",
                           "Specialized command for Calendar components");
}

QCalendarWidget* CalendarCommand::findCalendar(const QString& name) {
    return findWidget<QCalendarWidget>(name);
}

CommandResult<QVariant> CalendarCommand::handleSetSelectedDate(
    const CommandContext& context, QCalendarWidget* widget) {
    if (!context.hasParameter("date")) {
        return CommandResult<QVariant>(
            QString("Missing date parameter for setSelectedDate operation"));
    }

    auto date = context.getParameter<QDate>("date");
    new_date_ = date;
    widget->setSelectedDate(date);
    return createSuccessResult("Calendar", "selected date set");
}

CommandResult<QVariant> CalendarCommand::handleSetDateRange(
    const CommandContext& context, QCalendarWidget* widget) {
    auto validationResult =
        validateRequiredParameters(context, {"minDate", "maxDate"});
    if (!validationResult.isSuccess()) {
        return validationResult;
    }

    auto minDate = context.getParameter<QDate>("minDate");
    auto maxDate = context.getParameter<QDate>("maxDate");
    widget->setDateRange(minDate, maxDate);
    return createSuccessResult("Calendar", "date range set");
}

CommandResult<QVariant> CalendarCommand::handleSetGridVisible(
    const CommandContext& context, QCalendarWidget* widget) {
    if (!context.hasParameter("visible")) {
        return CommandResult<QVariant>(
            QString("Missing visible parameter for setGridVisible operation"));
    }

    auto visible = context.getParameter<bool>("visible");
    widget->setGridVisible(visible);
    return createSuccessResult("Calendar", "grid visibility set");
}

}  // namespace ComponentCommands
}  // namespace Command
}  // namespace DeclarativeUI
