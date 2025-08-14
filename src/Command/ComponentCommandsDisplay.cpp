#include "ComponentCommands.hpp"
#include <QApplication>
#include <QDebug>
#include <QLCDNumber>
#include <QCalendarWidget>

namespace DeclarativeUI {
namespace Command {
namespace ComponentCommands {

// ============================================================================
// LCD NUMBER COMPONENTS
// ============================================================================

LCDNumberCommand::LCDNumberCommand(const CommandContext& context)
    : ICommand(nullptr) {}

CommandResult<QVariant> LCDNumberCommand::execute(const CommandContext& context) {
    auto widget_name = context.getParameter<QString>("widget");
    auto operation = context.getParameter<QString>("operation");

    if (!context.hasParameter("widget")) {
        return CommandResult<QVariant>(QString("Missing required parameter: widget"));
    }

    auto* lcdNumber = findLCDNumber(widget_name);
    if (!lcdNumber) {
        return CommandResult<QVariant>(QString("LCDNumber '%1' not found").arg(widget_name));
    }

    widget_name_ = widget_name;
    old_value_ = lcdNumber->value();
    operation_ = operation;

    if (operation == "display" || operation.isEmpty()) {
        auto value = context.getParameter<double>("value");
        if (context.hasParameter("value")) {
            new_value_ = value;
            lcdNumber->display(value);
            return CommandResult<QVariant>(QString("LCDNumber value displayed successfully"));
        }
        return CommandResult<QVariant>(QString("Missing value parameter for display operation"));
    } else if (operation == "setDigitCount") {
        auto count = context.getParameter<int>("count");
        if (context.hasParameter("count")) {
            lcdNumber->setDigitCount(count);
            return CommandResult<QVariant>(QString("LCDNumber digit count set successfully"));
        }
        return CommandResult<QVariant>(QString("Missing count parameter for setDigitCount operation"));
    } else if (operation == "setMode") {
        auto mode = context.getParameter<int>("mode");
        if (context.hasParameter("mode")) {
            lcdNumber->setMode(static_cast<QLCDNumber::Mode>(mode));
            return CommandResult<QVariant>(QString("LCDNumber mode set successfully"));
        }
        return CommandResult<QVariant>(QString("Missing mode parameter for setMode operation"));
    } else if (operation == "setSegmentStyle") {
        auto style = context.getParameter<int>("style");
        if (context.hasParameter("style")) {
            lcdNumber->setSegmentStyle(static_cast<QLCDNumber::SegmentStyle>(style));
            return CommandResult<QVariant>(QString("LCDNumber segment style set successfully"));
        }
        return CommandResult<QVariant>(QString("Missing style parameter for setSegmentStyle operation"));
    }

    return CommandResult<QVariant>(QString("Unknown operation: %1").arg(operation));
}

CommandResult<QVariant> LCDNumberCommand::undo(const CommandContext& context) {
    auto* lcdNumber = findLCDNumber(widget_name_);
    if (!lcdNumber) {
        return CommandResult<QVariant>(QString("LCDNumber '%1' not found for undo").arg(widget_name_));
    }

    lcdNumber->display(old_value_);
    return CommandResult<QVariant>(QString("LCDNumber undo successful"));
}

bool LCDNumberCommand::canUndo(const CommandContext& context) const {
    return !widget_name_.isEmpty();
}

CommandMetadata LCDNumberCommand::getMetadata() const {
    return CommandMetadata("LCDNumberCommand", "Specialized command for LCDNumber components");
}

QLCDNumber* LCDNumberCommand::findLCDNumber(const QString& name) {
    for (auto* widget : QApplication::allWidgets()) {
        if (auto* lcdNumber = qobject_cast<QLCDNumber*>(widget)) {
            if (lcdNumber->objectName() == name) {
                return lcdNumber;
            }
        }
    }
    return nullptr;
}

// ============================================================================
// CALENDAR COMPONENTS
// ============================================================================

CalendarCommand::CalendarCommand(const CommandContext& context)
    : ICommand(nullptr) {}

CommandResult<QVariant> CalendarCommand::execute(const CommandContext& context) {
    auto widget_name = context.getParameter<QString>("widget");
    auto operation = context.getParameter<QString>("operation");

    if (!context.hasParameter("widget")) {
        return CommandResult<QVariant>(QString("Missing required parameter: widget"));
    }

    auto* calendar = findCalendar(widget_name);
    if (!calendar) {
        return CommandResult<QVariant>(QString("Calendar '%1' not found").arg(widget_name));
    }

    widget_name_ = widget_name;
    old_date_ = calendar->selectedDate();
    operation_ = operation;

    if (operation == "setSelectedDate" || operation.isEmpty()) {
        auto date = context.getParameter<QDate>("date");
        if (context.hasParameter("date")) {
            new_date_ = date;
            calendar->setSelectedDate(date);
            return CommandResult<QVariant>(QString("Calendar selected date set successfully"));
        }
        return CommandResult<QVariant>(QString("Missing date parameter for setSelectedDate operation"));
    } else if (operation == "setDateRange") {
        auto minDate = context.getParameter<QDate>("minDate");
        auto maxDate = context.getParameter<QDate>("maxDate");
        if (context.hasParameter("minDate") && context.hasParameter("maxDate")) {
            calendar->setDateRange(minDate, maxDate);
            return CommandResult<QVariant>(QString("Calendar date range set successfully"));
        }
        return CommandResult<QVariant>(QString("Missing minDate/maxDate parameters for setDateRange operation"));
    } else if (operation == "setGridVisible") {
        auto visible = context.getParameter<bool>("visible");
        if (context.hasParameter("visible")) {
            calendar->setGridVisible(visible);
            return CommandResult<QVariant>(QString("Calendar grid visibility set successfully"));
        }
        return CommandResult<QVariant>(QString("Missing visible parameter for setGridVisible operation"));
    }

    return CommandResult<QVariant>(QString("Unknown operation: %1").arg(operation));
}

CommandResult<QVariant> CalendarCommand::undo(const CommandContext& context) {
    auto* calendar = findCalendar(widget_name_);
    if (!calendar) {
        return CommandResult<QVariant>(QString("Calendar '%1' not found for undo").arg(widget_name_));
    }

    calendar->setSelectedDate(old_date_);
    return CommandResult<QVariant>(QString("Calendar undo successful"));
}

bool CalendarCommand::canUndo(const CommandContext& context) const {
    return !widget_name_.isEmpty();
}

CommandMetadata CalendarCommand::getMetadata() const {
    return CommandMetadata("CalendarCommand", "Specialized command for Calendar components");
}

QCalendarWidget* CalendarCommand::findCalendar(const QString& name) {
    for (auto* widget : QApplication::allWidgets()) {
        if (auto* calendar = qobject_cast<QCalendarWidget*>(widget)) {
            if (calendar->objectName() == name) {
                return calendar;
            }
        }
    }
    return nullptr;
}

}  // namespace ComponentCommands
}  // namespace Command
}  // namespace DeclarativeUI
