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
// COMMON HELPER FUNCTION IMPLEMENTATIONS
// ============================================================================

CommandResult<QVariant> validateRequiredParameters(const CommandContext& context,
                                                   const QStringList& requiredParams) {
    for (const QString& param : requiredParams) {
        if (!context.hasParameter(param)) {
            return CommandResult<QVariant>(QString("Missing required parameter: %1").arg(param));
        }
    }
    return CommandResult<QVariant>(); // Empty result indicates success
}

CommandResult<QVariant> createWidgetNotFoundError(const QString& widgetType,
                                                  const QString& widgetName) {
    return CommandResult<QVariant>(QString("%1 '%2' not found").arg(widgetType, widgetName));
}

CommandResult<QVariant> createSuccessResult(const QString& widgetType,
                                           const QString& operation) {
    return CommandResult<QVariant>(QString("%1 %2 successful").arg(widgetType, operation));
}

// ============================================================================
// DOUBLE SPINBOX COMPONENTS
// ============================================================================

DoubleSpinBoxCommand::DoubleSpinBoxCommand(const CommandContext& context)
    : ICommand(nullptr) {}

CommandResult<QVariant> DoubleSpinBoxCommand::execute(const CommandContext& context) {
    // Validate required parameters
    auto validationResult = validateRequiredParameters(context, {"widget"});
    if (!validationResult.isSuccess()) {
        return validationResult;
    }

    auto widget_name = context.getParameter<QString>("widget");
    auto operation = context.getParameter<QString>("operation");

    auto* doubleSpinBox = findDoubleSpinBox(widget_name);
    if (!doubleSpinBox) {
        return createWidgetNotFoundError("DoubleSpinBox", widget_name);
    }

    // Store state for undo functionality
    widget_name_ = widget_name;
    old_value_ = doubleSpinBox->value();
    operation_ = operation;

    // Route to appropriate operation handler
    if (operation == "setValue" || operation.isEmpty()) {
        return handleSetValue(context, doubleSpinBox);
    } else if (operation == "stepUp") {
        return handleStepUp(context, doubleSpinBox);
    } else if (operation == "stepDown") {
        return handleStepDown(context, doubleSpinBox);
    } else if (operation == "setRange") {
        return handleSetRange(context, doubleSpinBox);
    } else if (operation == "setDecimals") {
        return handleSetDecimals(context, doubleSpinBox);
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
    return findWidget<QDoubleSpinBox>(name);
}

CommandResult<QVariant> DoubleSpinBoxCommand::handleSetValue(const CommandContext& context, QDoubleSpinBox* widget) {
    if (!context.hasParameter("value")) {
        return CommandResult<QVariant>(QString("Missing value parameter for setValue operation"));
    }

    auto value = context.getParameter<double>("value");
    new_value_ = value;
    widget->setValue(value);
    return createSuccessResult("DoubleSpinBox", "value set");
}

CommandResult<QVariant> DoubleSpinBoxCommand::handleStepUp(const CommandContext& context, QDoubleSpinBox* widget) {
    widget->stepUp();
    new_value_ = widget->value();
    return createSuccessResult("DoubleSpinBox", "stepped up");
}

CommandResult<QVariant> DoubleSpinBoxCommand::handleStepDown(const CommandContext& context, QDoubleSpinBox* widget) {
    widget->stepDown();
    new_value_ = widget->value();
    return createSuccessResult("DoubleSpinBox", "stepped down");
}

CommandResult<QVariant> DoubleSpinBoxCommand::handleSetRange(const CommandContext& context, QDoubleSpinBox* widget) {
    auto validationResult = validateRequiredParameters(context, {"min", "max"});
    if (!validationResult.isSuccess()) {
        return validationResult;
    }

    auto min = context.getParameter<double>("min");
    auto max = context.getParameter<double>("max");
    widget->setRange(min, max);
    return createSuccessResult("DoubleSpinBox", "range set");
}

CommandResult<QVariant> DoubleSpinBoxCommand::handleSetDecimals(const CommandContext& context, QDoubleSpinBox* widget) {
    if (!context.hasParameter("decimals")) {
        return CommandResult<QVariant>(QString("Missing decimals parameter for setDecimals operation"));
    }

    auto decimals = context.getParameter<int>("decimals");
    widget->setDecimals(decimals);
    return createSuccessResult("DoubleSpinBox", "decimals set");
}

// ============================================================================
// DIAL COMPONENTS
// ============================================================================

DialCommand::DialCommand(const CommandContext& context)
    : ICommand(nullptr) {}

CommandResult<QVariant> DialCommand::execute(const CommandContext& context) {
    // Validate required parameters
    auto validationResult = validateRequiredParameters(context, {"widget"});
    if (!validationResult.isSuccess()) {
        return validationResult;
    }

    auto widget_name = context.getParameter<QString>("widget");
    auto operation = context.getParameter<QString>("operation");

    auto* dial = findDial(widget_name);
    if (!dial) {
        return createWidgetNotFoundError("Dial", widget_name);
    }

    // Store state for undo functionality
    widget_name_ = widget_name;
    old_value_ = dial->value();
    operation_ = operation;

    // Route to appropriate operation handler
    if (operation == "setValue" || operation.isEmpty()) {
        return handleSetValue(context, dial);
    } else if (operation == "setRange") {
        return handleSetRange(context, dial);
    } else if (operation == "setNotchesVisible") {
        return handleSetNotchesVisible(context, dial);
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
    return findWidget<QDial>(name);
}

CommandResult<QVariant> DialCommand::handleSetValue(const CommandContext& context, QDial* widget) {
    if (!context.hasParameter("value")) {
        return CommandResult<QVariant>(QString("Missing value parameter for setValue operation"));
    }

    auto value = context.getParameter<int>("value");
    new_value_ = value;
    widget->setValue(value);
    return createSuccessResult("Dial", "value set");
}

CommandResult<QVariant> DialCommand::handleSetRange(const CommandContext& context, QDial* widget) {
    auto validationResult = validateRequiredParameters(context, {"min", "max"});
    if (!validationResult.isSuccess()) {
        return validationResult;
    }

    auto min = context.getParameter<int>("min");
    auto max = context.getParameter<int>("max");
    widget->setRange(min, max);
    return createSuccessResult("Dial", "range set");
}

CommandResult<QVariant> DialCommand::handleSetNotchesVisible(const CommandContext& context, QDial* widget) {
    if (!context.hasParameter("visible")) {
        return CommandResult<QVariant>(QString("Missing visible parameter for setNotchesVisible operation"));
    }

    auto visible = context.getParameter<bool>("visible");
    widget->setNotchesVisible(visible);
    return createSuccessResult("Dial", "notches visibility set");
}

// ============================================================================
// DATETIME EDIT COMPONENTS
// ============================================================================

DateTimeEditCommand::DateTimeEditCommand(const CommandContext& context)
    : ICommand(nullptr) {}

CommandResult<QVariant> DateTimeEditCommand::execute(const CommandContext& context) {
    // Validate required parameters
    auto validationResult = validateRequiredParameters(context, {"widget"});
    if (!validationResult.isSuccess()) {
        return validationResult;
    }

    auto widget_name = context.getParameter<QString>("widget");
    auto operation = context.getParameter<QString>("operation");

    auto* dateTimeEdit = findDateTimeEdit(widget_name);
    if (!dateTimeEdit) {
        return createWidgetNotFoundError("DateTimeEdit", widget_name);
    }

    // Store state for undo functionality
    widget_name_ = widget_name;
    old_datetime_ = dateTimeEdit->dateTime();
    operation_ = operation;

    // Route to appropriate operation handler
    if (operation == "setDateTime" || operation.isEmpty()) {
        return handleSetDateTime(context, dateTimeEdit);
    } else if (operation == "setDateRange") {
        return handleSetDateRange(context, dateTimeEdit);
    } else if (operation == "setTimeRange") {
        return handleSetTimeRange(context, dateTimeEdit);
    } else if (operation == "setDisplayFormat") {
        return handleSetDisplayFormat(context, dateTimeEdit);
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
    return findWidget<QDateTimeEdit>(name);
}

CommandResult<QVariant> DateTimeEditCommand::handleSetDateTime(const CommandContext& context, QDateTimeEdit* widget) {
    if (!context.hasParameter("datetime")) {
        return CommandResult<QVariant>(QString("Missing datetime parameter for setDateTime operation"));
    }

    auto datetime = context.getParameter<QDateTime>("datetime");
    new_datetime_ = datetime;
    widget->setDateTime(datetime);
    return createSuccessResult("DateTimeEdit", "datetime set");
}

CommandResult<QVariant> DateTimeEditCommand::handleSetDateRange(const CommandContext& context, QDateTimeEdit* widget) {
    auto validationResult = validateRequiredParameters(context, {"minDate", "maxDate"});
    if (!validationResult.isSuccess()) {
        return validationResult;
    }

    auto minDate = context.getParameter<QDate>("minDate");
    auto maxDate = context.getParameter<QDate>("maxDate");
    widget->setDateRange(minDate, maxDate);
    return createSuccessResult("DateTimeEdit", "date range set");
}

CommandResult<QVariant> DateTimeEditCommand::handleSetTimeRange(const CommandContext& context, QDateTimeEdit* widget) {
    auto validationResult = validateRequiredParameters(context, {"minTime", "maxTime"});
    if (!validationResult.isSuccess()) {
        return validationResult;
    }

    auto minTime = context.getParameter<QTime>("minTime");
    auto maxTime = context.getParameter<QTime>("maxTime");
    widget->setTimeRange(minTime, maxTime);
    return createSuccessResult("DateTimeEdit", "time range set");
}

CommandResult<QVariant> DateTimeEditCommand::handleSetDisplayFormat(const CommandContext& context, QDateTimeEdit* widget) {
    if (!context.hasParameter("format")) {
        return CommandResult<QVariant>(QString("Missing format parameter for setDisplayFormat operation"));
    }

    auto format = context.getParameter<QString>("format");
    widget->setDisplayFormat(format);
    return createSuccessResult("DateTimeEdit", "display format set");
}

// ============================================================================
// PROGRESS BAR COMPONENTS
// ============================================================================

ProgressBarCommand::ProgressBarCommand(const CommandContext& context)
    : ICommand(nullptr) {}

CommandResult<QVariant> ProgressBarCommand::execute(const CommandContext& context) {
    // Validate required parameters
    auto validationResult = validateRequiredParameters(context, {"widget"});
    if (!validationResult.isSuccess()) {
        return validationResult;
    }

    auto widget_name = context.getParameter<QString>("widget");
    auto operation = context.getParameter<QString>("operation");

    auto* progressBar = findProgressBar(widget_name);
    if (!progressBar) {
        return createWidgetNotFoundError("ProgressBar", widget_name);
    }

    // Store state for undo functionality
    widget_name_ = widget_name;
    old_value_ = progressBar->value();
    operation_ = operation;

    // Route to appropriate operation handler
    if (operation == "setValue" || operation.isEmpty()) {
        return handleSetValue(context, progressBar);
    } else if (operation == "setRange") {
        return handleSetRange(context, progressBar);
    } else if (operation == "setTextVisible") {
        return handleSetTextVisible(context, progressBar);
    } else if (operation == "reset") {
        return handleReset(context, progressBar);
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
    return findWidget<QProgressBar>(name);
}

CommandResult<QVariant> ProgressBarCommand::handleSetValue(const CommandContext& context, QProgressBar* widget) {
    if (!context.hasParameter("value")) {
        return CommandResult<QVariant>(QString("Missing value parameter for setValue operation"));
    }

    auto value = context.getParameter<int>("value");
    new_value_ = value;
    widget->setValue(value);
    return createSuccessResult("ProgressBar", "value set");
}

CommandResult<QVariant> ProgressBarCommand::handleSetRange(const CommandContext& context, QProgressBar* widget) {
    auto validationResult = validateRequiredParameters(context, {"min", "max"});
    if (!validationResult.isSuccess()) {
        return validationResult;
    }

    auto min = context.getParameter<int>("min");
    auto max = context.getParameter<int>("max");
    widget->setRange(min, max);
    return createSuccessResult("ProgressBar", "range set");
}

CommandResult<QVariant> ProgressBarCommand::handleSetTextVisible(const CommandContext& context, QProgressBar* widget) {
    if (!context.hasParameter("visible")) {
        return CommandResult<QVariant>(QString("Missing visible parameter for setTextVisible operation"));
    }

    auto visible = context.getParameter<bool>("visible");
    widget->setTextVisible(visible);
    return createSuccessResult("ProgressBar", "text visibility set");
}

CommandResult<QVariant> ProgressBarCommand::handleReset(const CommandContext& context, QProgressBar* widget) {
    widget->reset();
    new_value_ = widget->value();
    return createSuccessResult("ProgressBar", "reset");
}

}  // namespace ComponentCommands
}  // namespace Command
}  // namespace DeclarativeUI
