#include "ComponentCommands.hpp"
#include <QApplication>
#include <QDebug>
#include <QDoubleSpinBox>
#include <QDial>
#include <QDateTimeEdit>
#include <QProgressBar>

namespace DeclarativeUI {
namespace Command {
namespace ComponentCommands {

// ============================================================================
// DOUBLE SPINBOX COMPONENTS
// ============================================================================

DoubleSpinBoxCommand::DoubleSpinBoxCommand(const CommandContext& context)
    : ICommand(nullptr) {}

CommandResult<QVariant> DoubleSpinBoxCommand::execute(const CommandContext& context) {
    auto widget_name = context.getParameter<QString>("widget");
    auto operation = context.getParameter<QString>("operation");

    if (!context.hasParameter("widget")) {
        return CommandResult<QVariant>(QString("Missing required parameter: widget"));
    }

    auto* doubleSpinBox = findDoubleSpinBox(widget_name);
    if (!doubleSpinBox) {
        return CommandResult<QVariant>(QString("DoubleSpinBox '%1' not found").arg(widget_name));
    }

    widget_name_ = widget_name;
    old_value_ = doubleSpinBox->value();
    operation_ = operation;

    if (operation == "setValue" || operation.isEmpty()) {
        auto value = context.getParameter<double>("value");
        if (context.hasParameter("value")) {
            new_value_ = value;
            doubleSpinBox->setValue(value);
            return CommandResult<QVariant>(QString("DoubleSpinBox value set successfully"));
        }
        return CommandResult<QVariant>(QString("Missing value parameter for setValue operation"));
    } else if (operation == "stepUp") {
        doubleSpinBox->stepUp();
        new_value_ = doubleSpinBox->value();
        return CommandResult<QVariant>(QString("DoubleSpinBox stepped up successfully"));
    } else if (operation == "stepDown") {
        doubleSpinBox->stepDown();
        new_value_ = doubleSpinBox->value();
        return CommandResult<QVariant>(QString("DoubleSpinBox stepped down successfully"));
    } else if (operation == "setRange") {
        auto min = context.getParameter<double>("min");
        auto max = context.getParameter<double>("max");
        if (context.hasParameter("min") && context.hasParameter("max")) {
            doubleSpinBox->setRange(min, max);
            return CommandResult<QVariant>(QString("DoubleSpinBox range set successfully"));
        }
        return CommandResult<QVariant>(QString("Missing min/max parameters for setRange operation"));
    } else if (operation == "setDecimals") {
        auto decimals = context.getParameter<int>("decimals");
        if (context.hasParameter("decimals")) {
            doubleSpinBox->setDecimals(decimals);
            return CommandResult<QVariant>(QString("DoubleSpinBox decimals set successfully"));
        }
        return CommandResult<QVariant>(QString("Missing decimals parameter for setDecimals operation"));
    }

    return CommandResult<QVariant>(QString("Unknown operation: %1").arg(operation));
}

CommandResult<QVariant> DoubleSpinBoxCommand::undo(const CommandContext& context) {
    auto* doubleSpinBox = findDoubleSpinBox(widget_name_);
    if (!doubleSpinBox) {
        return CommandResult<QVariant>(QString("DoubleSpinBox '%1' not found for undo").arg(widget_name_));
    }

    doubleSpinBox->setValue(old_value_);
    return CommandResult<QVariant>(QString("DoubleSpinBox undo successful"));
}

bool DoubleSpinBoxCommand::canUndo(const CommandContext& context) const {
    return !widget_name_.isEmpty();
}

CommandMetadata DoubleSpinBoxCommand::getMetadata() const {
    return CommandMetadata("DoubleSpinBoxCommand", "Specialized command for DoubleSpinBox components");
}

QDoubleSpinBox* DoubleSpinBoxCommand::findDoubleSpinBox(const QString& name) {
    for (auto* widget : QApplication::allWidgets()) {
        if (auto* doubleSpinBox = qobject_cast<QDoubleSpinBox*>(widget)) {
            if (doubleSpinBox->objectName() == name) {
                return doubleSpinBox;
            }
        }
    }
    return nullptr;
}

// ============================================================================
// DIAL COMPONENTS
// ============================================================================

DialCommand::DialCommand(const CommandContext& context)
    : ICommand(nullptr) {}

CommandResult<QVariant> DialCommand::execute(const CommandContext& context) {
    auto widget_name = context.getParameter<QString>("widget");
    auto operation = context.getParameter<QString>("operation");

    if (!context.hasParameter("widget")) {
        return CommandResult<QVariant>(QString("Missing required parameter: widget"));
    }

    auto* dial = findDial(widget_name);
    if (!dial) {
        return CommandResult<QVariant>(QString("Dial '%1' not found").arg(widget_name));
    }

    widget_name_ = widget_name;
    old_value_ = dial->value();
    operation_ = operation;

    if (operation == "setValue" || operation.isEmpty()) {
        auto value = context.getParameter<int>("value");
        if (context.hasParameter("value")) {
            new_value_ = value;
            dial->setValue(value);
            return CommandResult<QVariant>(QString("Dial value set successfully"));
        }
        return CommandResult<QVariant>(QString("Missing value parameter for setValue operation"));
    } else if (operation == "setRange") {
        auto min = context.getParameter<int>("min");
        auto max = context.getParameter<int>("max");
        if (context.hasParameter("min") && context.hasParameter("max")) {
            dial->setRange(min, max);
            return CommandResult<QVariant>(QString("Dial range set successfully"));
        }
        return CommandResult<QVariant>(QString("Missing min/max parameters for setRange operation"));
    } else if (operation == "setNotchesVisible") {
        auto visible = context.getParameter<bool>("visible");
        if (context.hasParameter("visible")) {
            dial->setNotchesVisible(visible);
            return CommandResult<QVariant>(QString("Dial notches visibility set successfully"));
        }
        return CommandResult<QVariant>(QString("Missing visible parameter for setNotchesVisible operation"));
    }

    return CommandResult<QVariant>(QString("Unknown operation: %1").arg(operation));
}

CommandResult<QVariant> DialCommand::undo(const CommandContext& context) {
    auto* dial = findDial(widget_name_);
    if (!dial) {
        return CommandResult<QVariant>(QString("Dial '%1' not found for undo").arg(widget_name_));
    }

    dial->setValue(old_value_);
    return CommandResult<QVariant>(QString("Dial undo successful"));
}

bool DialCommand::canUndo(const CommandContext& context) const {
    return !widget_name_.isEmpty();
}

CommandMetadata DialCommand::getMetadata() const {
    return CommandMetadata("DialCommand", "Specialized command for Dial components");
}

QDial* DialCommand::findDial(const QString& name) {
    for (auto* widget : QApplication::allWidgets()) {
        if (auto* dial = qobject_cast<QDial*>(widget)) {
            if (dial->objectName() == name) {
                return dial;
            }
        }
    }
    return nullptr;
}

// ============================================================================
// DATETIME EDIT COMPONENTS
// ============================================================================

DateTimeEditCommand::DateTimeEditCommand(const CommandContext& context)
    : ICommand(nullptr) {}

CommandResult<QVariant> DateTimeEditCommand::execute(const CommandContext& context) {
    auto widget_name = context.getParameter<QString>("widget");
    auto operation = context.getParameter<QString>("operation");

    if (!context.hasParameter("widget")) {
        return CommandResult<QVariant>(QString("Missing required parameter: widget"));
    }

    auto* dateTimeEdit = findDateTimeEdit(widget_name);
    if (!dateTimeEdit) {
        return CommandResult<QVariant>(QString("DateTimeEdit '%1' not found").arg(widget_name));
    }

    widget_name_ = widget_name;
    old_datetime_ = dateTimeEdit->dateTime();
    operation_ = operation;

    if (operation == "setDateTime" || operation.isEmpty()) {
        auto datetime = context.getParameter<QDateTime>("datetime");
        if (context.hasParameter("datetime")) {
            new_datetime_ = datetime;
            dateTimeEdit->setDateTime(datetime);
            return CommandResult<QVariant>(QString("DateTimeEdit datetime set successfully"));
        }
        return CommandResult<QVariant>(QString("Missing datetime parameter for setDateTime operation"));
    } else if (operation == "setDateRange") {
        auto minDate = context.getParameter<QDate>("minDate");
        auto maxDate = context.getParameter<QDate>("maxDate");
        if (context.hasParameter("minDate") && context.hasParameter("maxDate")) {
            dateTimeEdit->setDateRange(minDate, maxDate);
            return CommandResult<QVariant>(QString("DateTimeEdit date range set successfully"));
        }
        return CommandResult<QVariant>(QString("Missing minDate/maxDate parameters for setDateRange operation"));
    } else if (operation == "setTimeRange") {
        auto minTime = context.getParameter<QTime>("minTime");
        auto maxTime = context.getParameter<QTime>("maxTime");
        if (context.hasParameter("minTime") && context.hasParameter("maxTime")) {
            dateTimeEdit->setTimeRange(minTime, maxTime);
            return CommandResult<QVariant>(QString("DateTimeEdit time range set successfully"));
        }
        return CommandResult<QVariant>(QString("Missing minTime/maxTime parameters for setTimeRange operation"));
    } else if (operation == "setDisplayFormat") {
        auto format = context.getParameter<QString>("format");
        if (context.hasParameter("format")) {
            dateTimeEdit->setDisplayFormat(format);
            return CommandResult<QVariant>(QString("DateTimeEdit display format set successfully"));
        }
        return CommandResult<QVariant>(QString("Missing format parameter for setDisplayFormat operation"));
    }

    return CommandResult<QVariant>(QString("Unknown operation: %1").arg(operation));
}

CommandResult<QVariant> DateTimeEditCommand::undo(const CommandContext& context) {
    auto* dateTimeEdit = findDateTimeEdit(widget_name_);
    if (!dateTimeEdit) {
        return CommandResult<QVariant>(QString("DateTimeEdit '%1' not found for undo").arg(widget_name_));
    }

    dateTimeEdit->setDateTime(old_datetime_);
    return CommandResult<QVariant>(QString("DateTimeEdit undo successful"));
}

bool DateTimeEditCommand::canUndo(const CommandContext& context) const {
    return !widget_name_.isEmpty();
}

CommandMetadata DateTimeEditCommand::getMetadata() const {
    return CommandMetadata("DateTimeEditCommand", "Specialized command for DateTimeEdit components");
}

QDateTimeEdit* DateTimeEditCommand::findDateTimeEdit(const QString& name) {
    for (auto* widget : QApplication::allWidgets()) {
        if (auto* dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget)) {
            if (dateTimeEdit->objectName() == name) {
                return dateTimeEdit;
            }
        }
    }
    return nullptr;
}

// ============================================================================
// PROGRESS BAR COMPONENTS
// ============================================================================

ProgressBarCommand::ProgressBarCommand(const CommandContext& context)
    : ICommand(nullptr) {}

CommandResult<QVariant> ProgressBarCommand::execute(const CommandContext& context) {
    auto widget_name = context.getParameter<QString>("widget");
    auto operation = context.getParameter<QString>("operation");

    if (!context.hasParameter("widget")) {
        return CommandResult<QVariant>(QString("Missing required parameter: widget"));
    }

    auto* progressBar = findProgressBar(widget_name);
    if (!progressBar) {
        return CommandResult<QVariant>(QString("ProgressBar '%1' not found").arg(widget_name));
    }

    widget_name_ = widget_name;
    old_value_ = progressBar->value();
    operation_ = operation;

    if (operation == "setValue" || operation.isEmpty()) {
        auto value = context.getParameter<int>("value");
        if (context.hasParameter("value")) {
            new_value_ = value;
            progressBar->setValue(value);
            return CommandResult<QVariant>(QString("ProgressBar value set successfully"));
        }
        return CommandResult<QVariant>(QString("Missing value parameter for setValue operation"));
    } else if (operation == "setRange") {
        auto min = context.getParameter<int>("min");
        auto max = context.getParameter<int>("max");
        if (context.hasParameter("min") && context.hasParameter("max")) {
            progressBar->setRange(min, max);
            return CommandResult<QVariant>(QString("ProgressBar range set successfully"));
        }
        return CommandResult<QVariant>(QString("Missing min/max parameters for setRange operation"));
    } else if (operation == "setTextVisible") {
        auto visible = context.getParameter<bool>("visible");
        if (context.hasParameter("visible")) {
            progressBar->setTextVisible(visible);
            return CommandResult<QVariant>(QString("ProgressBar text visibility set successfully"));
        }
        return CommandResult<QVariant>(QString("Missing visible parameter for setTextVisible operation"));
    } else if (operation == "reset") {
        progressBar->reset();
        new_value_ = progressBar->value();
        return CommandResult<QVariant>(QString("ProgressBar reset successfully"));
    }

    return CommandResult<QVariant>(QString("Unknown operation: %1").arg(operation));
}

CommandResult<QVariant> ProgressBarCommand::undo(const CommandContext& context) {
    auto* progressBar = findProgressBar(widget_name_);
    if (!progressBar) {
        return CommandResult<QVariant>(QString("ProgressBar '%1' not found for undo").arg(widget_name_));
    }

    progressBar->setValue(old_value_);
    return CommandResult<QVariant>(QString("ProgressBar undo successful"));
}

bool ProgressBarCommand::canUndo(const CommandContext& context) const {
    return !widget_name_.isEmpty();
}

CommandMetadata ProgressBarCommand::getMetadata() const {
    return CommandMetadata("ProgressBarCommand", "Specialized command for ProgressBar components");
}

QProgressBar* ProgressBarCommand::findProgressBar(const QString& name) {
    for (auto* widget : QApplication::allWidgets()) {
        if (auto* progressBar = qobject_cast<QProgressBar*>(widget)) {
            if (progressBar->objectName() == name) {
                return progressBar;
            }
        }
    }
    return nullptr;
}

}  // namespace ComponentCommands
}  // namespace Command
}  // namespace DeclarativeUI
