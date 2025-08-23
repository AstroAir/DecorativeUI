/**
 * @file ComponentCommandsExtended.cpp
 * @brief Implementation of extended component commands with reduced cyclomatic
 * complexity.
 *
 * This file contains refactored command implementations that follow best
 * practices:
 * - Reduced cyclomatic complexity (< 10 per function)
 * - Extracted operation handlers for better maintainability
 * - Standardized error handling and success reporting
 * - Comprehensive parameter validation
 *
 * @author DeclarativeUI Team
 * @version 2.0
 * @date 2024
 */

#include <QApplication>
#include <QDebug>
#include "ComponentCommands.hpp"

namespace DeclarativeUI {
namespace Command {
namespace ComponentCommands {

// ============================================================================
// INPUT COMPONENTS
// ============================================================================

/**
 * @brief Constructs a SpinBoxCommand with the given context.
 * @param context The command execution context
 */
SpinBoxCommand::SpinBoxCommand(const CommandContext& context)
    : ICommand(nullptr) {}

/**
 * @brief Executes a SpinBox command with the specified operation.
 *
 * Supported operations:
 * - setValue: Sets the spinbox value (requires 'value' parameter)
 * - stepUp: Increments the spinbox value by one step
 * - stepDown: Decrements the spinbox value by one step
 * - setRange: Sets the min/max range (requires 'min' and 'max' parameters)
 *
 * @param context Command context containing widget name, operation, and
 * parameters
 * @return CommandResult indicating success or failure with descriptive message
 */
CommandResult<QVariant> SpinBoxCommand::execute(const CommandContext& context) {
    // Validate required parameters
    if (!context.hasParameter("widget")) {
        return CommandResult<QVariant>(
            QString("Missing required parameter: widget"));
    }

    auto widget_name = context.getParameter<QString>("widget");
    auto operation = context.getParameter<QString>("operation");

    // Find and validate widget
    auto* spinBox = findSpinBox(widget_name);
    if (!spinBox) {
        return createWidgetNotFoundError("SpinBox", widget_name);
    }

    // Store state for undo functionality
    widget_name_ = widget_name;
    old_value_ = spinBox->value();
    operation_ = operation;

    // Route to appropriate operation handler
    if (operation == "setValue" || operation.isEmpty()) {
        return handleSetValue(context, spinBox);
    } else if (operation == "stepUp") {
        return handleStepUp(context, spinBox);
    } else if (operation == "stepDown") {
        return handleStepDown(context, spinBox);
    } else if (operation == "setRange") {
        return handleSetRange(context, spinBox);
    }

    return CommandResult<QVariant>(
        QString("Unknown operation: %1").arg(operation));
}

CommandResult<QVariant> SpinBoxCommand::undo(const CommandContext& context) {
    auto* spinBox = findSpinBox(widget_name_);
    if (!spinBox) {
        return CommandResult<QVariant>(
            QString("SpinBox '%1' not found for undo").arg(widget_name_));
    }

    spinBox->setValue(old_value_);
    return CommandResult<QVariant>(QString("SpinBox undo successful"));
}

bool SpinBoxCommand::canUndo(const CommandContext& context) const {
    return !widget_name_.isEmpty();
}

CommandMetadata SpinBoxCommand::getMetadata() const {
    return CommandMetadata("SpinBoxCommand",
                           "Specialized command for SpinBox components");
}

QSpinBox* SpinBoxCommand::findSpinBox(const QString& name) {
    for (auto* widget : QApplication::allWidgets()) {
        if (auto* spinBox = qobject_cast<QSpinBox*>(widget)) {
            if (spinBox->objectName() == name) {
                return spinBox;
            }
        }
    }
    return nullptr;
}

/**
 * @brief Handles the setValue operation for SpinBox widgets.
 * @param context Command context containing the 'value' parameter
 * @param widget Pointer to the target SpinBox widget
 * @return CommandResult indicating success or failure
 */
CommandResult<QVariant> SpinBoxCommand::handleSetValue(
    const CommandContext& context, QSpinBox* widget) {
    if (!context.hasParameter("value")) {
        return CommandResult<QVariant>(
            QString("Missing value parameter for setValue operation"));
    }

    auto value = context.getParameter<int>("value");
    new_value_ = value;
    widget->setValue(value);
    return createSuccessResult("SpinBox", "value set");
}

/**
 * @brief Handles the stepUp operation for SpinBox widgets.
 * @param context Command context (unused for this operation)
 * @param widget Pointer to the target SpinBox widget
 * @return CommandResult indicating success
 */
CommandResult<QVariant> SpinBoxCommand::handleStepUp(
    const CommandContext& context, QSpinBox* widget) {
    Q_UNUSED(context)
    widget->stepUp();
    new_value_ = widget->value();
    return createSuccessResult("SpinBox", "stepped up");
}

/**
 * @brief Handles the stepDown operation for SpinBox widgets.
 * @param context Command context (unused for this operation)
 * @param widget Pointer to the target SpinBox widget
 * @return CommandResult indicating success
 */
CommandResult<QVariant> SpinBoxCommand::handleStepDown(
    const CommandContext& context, QSpinBox* widget) {
    Q_UNUSED(context)
    widget->stepDown();
    new_value_ = widget->value();
    return createSuccessResult("SpinBox", "stepped down");
}

/**
 * @brief Handles the setRange operation for SpinBox widgets.
 * @param context Command context containing 'min' and 'max' parameters
 * @param widget Pointer to the target SpinBox widget
 * @return CommandResult indicating success or failure
 */
CommandResult<QVariant> SpinBoxCommand::handleSetRange(
    const CommandContext& context, QSpinBox* widget) {
    auto validationResult = validateRequiredParameters(context, {"min", "max"});
    if (!validationResult.isSuccess()) {
        return validationResult;
    }

    auto min = context.getParameter<int>("min");
    auto max = context.getParameter<int>("max");
    widget->setRange(min, max);
    return createSuccessResult("SpinBox", "range set");
}

// ============================================================================
// SLIDER COMPONENTS
// ============================================================================

/**
 * @brief Constructs a SliderCommand with the given context.
 * @param context The command execution context
 */
SliderCommand::SliderCommand(const CommandContext& context)
    : ICommand(nullptr) {}

/**
 * @brief Executes a Slider command with the specified operation.
 *
 * Supported operations:
 * - setValue: Sets the slider value (requires 'value' parameter)
 * - setRange: Sets the min/max range (requires 'min' and 'max' parameters)
 *
 * @param context Command context containing widget name, operation, and
 * parameters
 * @return CommandResult indicating success or failure with descriptive message
 */
CommandResult<QVariant> SliderCommand::execute(const CommandContext& context) {
    // Validate required parameters
    if (!context.hasParameter("widget")) {
        return CommandResult<QVariant>(
            QString("Missing required parameter: widget"));
    }

    auto widget_name = context.getParameter<QString>("widget");
    auto operation = context.getParameter<QString>("operation");

    // Find and validate widget
    auto* slider = findSlider(widget_name);
    if (!slider) {
        return createWidgetNotFoundError("Slider", widget_name);
    }

    // Store state for undo functionality
    widget_name_ = widget_name;
    old_value_ = slider->value();
    operation_ = operation;

    // Route to appropriate operation handler
    if (operation == "setValue" || operation.isEmpty()) {
        return handleSetValue(context, slider);
    } else if (operation == "setRange") {
        return handleSetRange(context, slider);
    }

    return CommandResult<QVariant>(
        QString("Unknown operation: %1").arg(operation));
}

CommandResult<QVariant> SliderCommand::undo(const CommandContext& context) {
    auto* slider = findSlider(widget_name_);
    if (!slider) {
        return CommandResult<QVariant>(
            QString("Slider '%1' not found for undo").arg(widget_name_));
    }

    slider->setValue(old_value_);
    return CommandResult<QVariant>(QString("Slider undo successful"));
}

bool SliderCommand::canUndo(const CommandContext& context) const {
    return !widget_name_.isEmpty();
}

CommandMetadata SliderCommand::getMetadata() const {
    return CommandMetadata("SliderCommand",
                           "Specialized command for Slider components");
}

QSlider* SliderCommand::findSlider(const QString& name) {
    for (auto* widget : QApplication::allWidgets()) {
        if (auto* slider = qobject_cast<QSlider*>(widget)) {
            if (slider->objectName() == name) {
                return slider;
            }
        }
    }
    return nullptr;
}

/**
 * @brief Handles the setValue operation for Slider widgets.
 * @param context Command context containing the 'value' parameter
 * @param widget Pointer to the target Slider widget
 * @return CommandResult indicating success or failure
 */
CommandResult<QVariant> SliderCommand::handleSetValue(
    const CommandContext& context, QSlider* widget) {
    if (!context.hasParameter("value")) {
        return CommandResult<QVariant>(
            QString("Missing value parameter for setValue operation"));
    }

    auto value = context.getParameter<int>("value");
    new_value_ = value;
    widget->setValue(value);
    return createSuccessResult("Slider", "value set");
}

/**
 * @brief Handles the setRange operation for Slider widgets.
 * @param context Command context containing 'min' and 'max' parameters
 * @param widget Pointer to the target Slider widget
 * @return CommandResult indicating success or failure
 */
CommandResult<QVariant> SliderCommand::handleSetRange(
    const CommandContext& context, QSlider* widget) {
    auto validationResult = validateRequiredParameters(context, {"min", "max"});
    if (!validationResult.isSuccess()) {
        return validationResult;
    }

    auto min = context.getParameter<int>("min");
    auto max = context.getParameter<int>("max");
    widget->setRange(min, max);
    return createSuccessResult("Slider", "range set");
}

// ============================================================================
// CONTAINER COMPONENTS
// ============================================================================

/**
 * @brief Constructs a TabWidgetCommand with the given context.
 * @param context The command execution context
 */
TabWidgetCommand::TabWidgetCommand(const CommandContext& context)
    : ICommand(nullptr) {}

CommandResult<QVariant> TabWidgetCommand::execute(
    const CommandContext& context) {
    // Validate required parameters
    if (!context.hasParameter("widget")) {
        return CommandResult<QVariant>(
            QString("Missing required parameter: widget"));
    }

    auto widget_name = context.getParameter<QString>("widget");
    auto operation = context.getParameter<QString>("operation");

    // Find and validate widget
    auto* tabWidget = findTabWidget(widget_name);
    if (!tabWidget) {
        return createWidgetNotFoundError("TabWidget", widget_name);
    }

    // Store state for undo functionality
    widget_name_ = widget_name;
    old_index_ = tabWidget->currentIndex();
    operation_ = operation;

    // Route to appropriate operation handler
    if (operation == "setCurrentIndex" || operation.isEmpty()) {
        return handleSetCurrentIndex(context, tabWidget);
    } else if (operation == "setTabText") {
        return handleSetTabText(context, tabWidget);
    }

    return CommandResult<QVariant>(
        QString("Unknown operation: %1").arg(operation));
}

CommandResult<QVariant> TabWidgetCommand::undo(const CommandContext& context) {
    auto* tabWidget = findTabWidget(widget_name_);
    if (!tabWidget) {
        return CommandResult<QVariant>(
            QString("TabWidget '%1' not found for undo").arg(widget_name_));
    }

    tabWidget->setCurrentIndex(old_index_);
    return CommandResult<QVariant>(QString("TabWidget undo successful"));
}

bool TabWidgetCommand::canUndo(const CommandContext& context) const {
    return !widget_name_.isEmpty();
}

CommandMetadata TabWidgetCommand::getMetadata() const {
    return CommandMetadata("TabWidgetCommand",
                           "Specialized command for TabWidget components");
}

QTabWidget* TabWidgetCommand::findTabWidget(const QString& name) {
    for (auto* widget : QApplication::allWidgets()) {
        if (auto* tabWidget = qobject_cast<QTabWidget*>(widget)) {
            if (tabWidget->objectName() == name) {
                return tabWidget;
            }
        }
    }
    return nullptr;
}

CommandResult<QVariant> TabWidgetCommand::handleSetCurrentIndex(
    const CommandContext& context, QTabWidget* widget) {
    if (!context.hasParameter("index")) {
        return CommandResult<QVariant>(
            QString("Missing index parameter for setCurrentIndex operation"));
    }

    auto index = context.getParameter<int>("index");
    new_index_ = index;
    widget->setCurrentIndex(index);
    return createSuccessResult("TabWidget", "current index set");
}

CommandResult<QVariant> TabWidgetCommand::handleSetTabText(
    const CommandContext& context, QTabWidget* widget) {
    auto validationResult =
        validateRequiredParameters(context, {"index", "text"});
    if (!validationResult.isSuccess()) {
        return validationResult;
    }

    auto index = context.getParameter<int>("index");
    auto text = context.getParameter<QString>("text");
    widget->setTabText(index, text);
    return createSuccessResult("TabWidget", "tab text set");
}

// ============================================================================
// COMBOBOX COMPONENTS
// ============================================================================

/**
 * @brief Constructs a ComboBoxCommand with the given context.
 * @param context The command execution context
 */
ComboBoxCommand::ComboBoxCommand(const CommandContext& context)
    : ICommand(nullptr) {}

CommandResult<QVariant> ComboBoxCommand::execute(
    const CommandContext& context) {
    // Validate required parameters
    if (!context.hasParameter("widget")) {
        return CommandResult<QVariant>(
            QString("Missing required parameter: widget"));
    }

    auto widget_name = context.getParameter<QString>("widget");
    auto operation = context.getParameter<QString>("operation");

    // Find and validate widget
    auto* comboBox = findComboBox(widget_name);
    if (!comboBox) {
        return createWidgetNotFoundError("ComboBox", widget_name);
    }

    // Store state for undo functionality
    widget_name_ = widget_name;
    old_index_ = comboBox->currentIndex();
    old_text_ = comboBox->currentText();
    operation_ = operation;

    // Route to appropriate operation handler
    if (operation == "setCurrentIndex" || operation.isEmpty()) {
        return handleSetCurrentIndex(context, comboBox);
    } else if (operation == "setCurrentText") {
        return handleSetCurrentText(context, comboBox);
    } else if (operation == "addItem") {
        return handleAddItem(context, comboBox);
    } else if (operation == "clear") {
        return handleClear(context, comboBox);
    }

    return CommandResult<QVariant>(
        QString("Unknown operation: %1").arg(operation));
}

CommandResult<QVariant> ComboBoxCommand::undo(const CommandContext& context) {
    auto* comboBox = findComboBox(widget_name_);
    if (!comboBox) {
        return CommandResult<QVariant>(
            QString("ComboBox '%1' not found for undo").arg(widget_name_));
    }

    comboBox->setCurrentIndex(old_index_);
    return CommandResult<QVariant>(QString("ComboBox undo successful"));
}

bool ComboBoxCommand::canUndo(const CommandContext& context) const {
    return !widget_name_.isEmpty();
}

CommandMetadata ComboBoxCommand::getMetadata() const {
    return CommandMetadata("ComboBoxCommand",
                           "Specialized command for ComboBox components");
}

QComboBox* ComboBoxCommand::findComboBox(const QString& name) {
    for (auto* widget : QApplication::allWidgets()) {
        if (auto* comboBox = qobject_cast<QComboBox*>(widget)) {
            if (comboBox->objectName() == name) {
                return comboBox;
            }
        }
    }
    return nullptr;
}

CommandResult<QVariant> ComboBoxCommand::handleSetCurrentIndex(
    const CommandContext& context, QComboBox* widget) {
    if (!context.hasParameter("index")) {
        return CommandResult<QVariant>(
            QString("Missing index parameter for setCurrentIndex operation"));
    }

    auto index = context.getParameter<int>("index");
    widget->setCurrentIndex(index);
    return createSuccessResult("ComboBox", "current index set");
}

CommandResult<QVariant> ComboBoxCommand::handleSetCurrentText(
    const CommandContext& context, QComboBox* widget) {
    if (!context.hasParameter("text")) {
        return CommandResult<QVariant>(
            QString("Missing text parameter for setCurrentText operation"));
    }

    auto text = context.getParameter<QString>("text");
    widget->setCurrentText(text);
    return createSuccessResult("ComboBox", "current text set");
}

CommandResult<QVariant> ComboBoxCommand::handleAddItem(
    const CommandContext& context, QComboBox* widget) {
    if (!context.hasParameter("text")) {
        return CommandResult<QVariant>(
            QString("Missing text parameter for addItem operation"));
    }

    auto text = context.getParameter<QString>("text");
    widget->addItem(text);
    return createSuccessResult("ComboBox", "item added");
}

CommandResult<QVariant> ComboBoxCommand::handleClear(
    const CommandContext& context, QComboBox* widget) {
    Q_UNUSED(context)
    widget->clear();
    return createSuccessResult("ComboBox", "cleared");
}

}  // namespace ComponentCommands
}  // namespace Command
}  // namespace DeclarativeUI
