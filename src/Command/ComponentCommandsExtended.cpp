#include "ComponentCommands.hpp"
#include <QApplication>
#include <QDebug>

namespace DeclarativeUI {
namespace Command {
namespace ComponentCommands {

// ============================================================================
// INPUT COMPONENTS
// ============================================================================

SpinBoxCommand::SpinBoxCommand(const CommandContext& context)
    : ICommand(nullptr) {}

CommandResult<QVariant> SpinBoxCommand::execute(const CommandContext& context) {
    auto widget_name = context.getParameter<QString>("widget");
    auto operation = context.getParameter<QString>("operation");

    if (!context.hasParameter("widget")) {
        return CommandResult<QVariant>(QString("Missing required parameter: widget"));
    }

    auto* spinBox = findSpinBox(widget_name);
    if (!spinBox) {
        return CommandResult<QVariant>(QString("SpinBox '%1' not found").arg(widget_name));
    }

    widget_name_ = widget_name;
    old_value_ = spinBox->value();
    operation_ = operation;

    if (operation == "setValue" || operation.isEmpty()) {
        auto value = context.getParameter<int>("value");
        if (context.hasParameter("value")) {
            new_value_ = value;
            spinBox->setValue(value);
            return CommandResult<QVariant>(QString("SpinBox value set successfully"));
        }
        return CommandResult<QVariant>(QString("Missing value parameter for setValue operation"));
    } else if (operation == "stepUp") {
        spinBox->stepUp();
        new_value_ = spinBox->value();
        return CommandResult<QVariant>(QString("SpinBox stepped up successfully"));
    } else if (operation == "stepDown") {
        spinBox->stepDown();
        new_value_ = spinBox->value();
        return CommandResult<QVariant>(QString("SpinBox stepped down successfully"));
    } else if (operation == "setRange") {
        auto min = context.getParameter<int>("min");
        auto max = context.getParameter<int>("max");
        if (context.hasParameter("min") && context.hasParameter("max")) {
            spinBox->setRange(min, max);
            return CommandResult<QVariant>(QString("SpinBox range set successfully"));
        }
        return CommandResult<QVariant>(QString("Missing min/max parameters for setRange operation"));
    }

    return CommandResult<QVariant>(QString("Unknown operation: %1").arg(operation));
}

CommandResult<QVariant> SpinBoxCommand::undo(const CommandContext& context) {
    auto* spinBox = findSpinBox(widget_name_);
    if (!spinBox) {
        return CommandResult<QVariant>(QString("SpinBox '%1' not found for undo").arg(widget_name_));
    }

    spinBox->setValue(old_value_);
    return CommandResult<QVariant>(QString("SpinBox undo successful"));
}

bool SpinBoxCommand::canUndo(const CommandContext& context) const {
    return !widget_name_.isEmpty();
}

CommandMetadata SpinBoxCommand::getMetadata() const {
    return CommandMetadata("SpinBoxCommand", "Specialized command for SpinBox components");
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

// ============================================================================
// SLIDER COMPONENTS
// ============================================================================

SliderCommand::SliderCommand(const CommandContext& context)
    : ICommand(nullptr) {}

CommandResult<QVariant> SliderCommand::execute(const CommandContext& context) {
    auto widget_name = context.getParameter<QString>("widget");
    auto operation = context.getParameter<QString>("operation");

    if (!context.hasParameter("widget")) {
        return CommandResult<QVariant>(QString("Missing required parameter: widget"));
    }

    auto* slider = findSlider(widget_name);
    if (!slider) {
        return CommandResult<QVariant>(QString("Slider '%1' not found").arg(widget_name));
    }

    widget_name_ = widget_name;
    old_value_ = slider->value();
    operation_ = operation;

    if (operation == "setValue" || operation.isEmpty()) {
        auto value = context.getParameter<int>("value");
        if (context.hasParameter("value")) {
            new_value_ = value;
            slider->setValue(value);
            return CommandResult<QVariant>(QString("Slider value set successfully"));
        }
        return CommandResult<QVariant>(QString("Missing value parameter for setValue operation"));
    } else if (operation == "setRange") {
        auto min = context.getParameter<int>("min");
        auto max = context.getParameter<int>("max");
        if (context.hasParameter("min") && context.hasParameter("max")) {
            slider->setRange(min, max);
            return CommandResult<QVariant>(QString("Slider range set successfully"));
        }
        return CommandResult<QVariant>(QString("Missing min/max parameters for setRange operation"));
    }

    return CommandResult<QVariant>(QString("Unknown operation: %1").arg(operation));
}

CommandResult<QVariant> SliderCommand::undo(const CommandContext& context) {
    auto* slider = findSlider(widget_name_);
    if (!slider) {
        return CommandResult<QVariant>(QString("Slider '%1' not found for undo").arg(widget_name_));
    }

    slider->setValue(old_value_);
    return CommandResult<QVariant>(QString("Slider undo successful"));
}

bool SliderCommand::canUndo(const CommandContext& context) const {
    return !widget_name_.isEmpty();
}

CommandMetadata SliderCommand::getMetadata() const {
    return CommandMetadata("SliderCommand", "Specialized command for Slider components");
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

// ============================================================================
// CONTAINER COMPONENTS
// ============================================================================

TabWidgetCommand::TabWidgetCommand(const CommandContext& context)
    : ICommand(nullptr) {}

CommandResult<QVariant> TabWidgetCommand::execute(const CommandContext& context) {
    auto widget_name = context.getParameter<QString>("widget");
    auto operation = context.getParameter<QString>("operation");

    if (!context.hasParameter("widget")) {
        return CommandResult<QVariant>(QString("Missing required parameter: widget"));
    }

    auto* tabWidget = findTabWidget(widget_name);
    if (!tabWidget) {
        return CommandResult<QVariant>(QString("TabWidget '%1' not found").arg(widget_name));
    }

    widget_name_ = widget_name;
    old_index_ = tabWidget->currentIndex();
    operation_ = operation;

    if (operation == "setCurrentIndex" || operation.isEmpty()) {
        auto index = context.getParameter<int>("index");
        if (context.hasParameter("index")) {
            new_index_ = index;
            tabWidget->setCurrentIndex(index);
            return CommandResult<QVariant>(QString("TabWidget current index set successfully"));
        }
        return CommandResult<QVariant>(QString("Missing index parameter for setCurrentIndex operation"));
    } else if (operation == "setTabText") {
        auto index = context.getParameter<int>("index");
        auto text = context.getParameter<QString>("text");
        if (context.hasParameter("index") && context.hasParameter("text")) {
            tabWidget->setTabText(index, text);
            return CommandResult<QVariant>(QString("TabWidget tab text set successfully"));
        }
        return CommandResult<QVariant>(QString("Missing index/text parameters for setTabText operation"));
    }

    return CommandResult<QVariant>(QString("Unknown operation: %1").arg(operation));
}

CommandResult<QVariant> TabWidgetCommand::undo(const CommandContext& context) {
    auto* tabWidget = findTabWidget(widget_name_);
    if (!tabWidget) {
        return CommandResult<QVariant>(QString("TabWidget '%1' not found for undo").arg(widget_name_));
    }

    tabWidget->setCurrentIndex(old_index_);
    return CommandResult<QVariant>(QString("TabWidget undo successful"));
}

bool TabWidgetCommand::canUndo(const CommandContext& context) const {
    return !widget_name_.isEmpty();
}

CommandMetadata TabWidgetCommand::getMetadata() const {
    return CommandMetadata("TabWidgetCommand", "Specialized command for TabWidget components");
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

// ============================================================================
// COMBOBOX COMPONENTS
// ============================================================================

ComboBoxCommand::ComboBoxCommand(const CommandContext& context)
    : ICommand(nullptr) {}

CommandResult<QVariant> ComboBoxCommand::execute(const CommandContext& context) {
    auto widget_name = context.getParameter<QString>("widget");
    auto operation = context.getParameter<QString>("operation");

    if (!context.hasParameter("widget")) {
        return CommandResult<QVariant>(QString("Missing required parameter: widget"));
    }

    auto* comboBox = findComboBox(widget_name);
    if (!comboBox) {
        return CommandResult<QVariant>(QString("ComboBox '%1' not found").arg(widget_name));
    }

    widget_name_ = widget_name;
    old_index_ = comboBox->currentIndex();
    old_text_ = comboBox->currentText();
    operation_ = operation;

    if (operation == "setCurrentIndex" || operation.isEmpty()) {
        auto index = context.getParameter<int>("index");
        if (context.hasParameter("index")) {
            comboBox->setCurrentIndex(index);
            return CommandResult<QVariant>(QString("ComboBox current index set successfully"));
        }
        return CommandResult<QVariant>(QString("Missing index parameter for setCurrentIndex operation"));
    } else if (operation == "setCurrentText") {
        auto text = context.getParameter<QString>("text");
        if (context.hasParameter("text")) {
            comboBox->setCurrentText(text);
            return CommandResult<QVariant>(QString("ComboBox current text set successfully"));
        }
        return CommandResult<QVariant>(QString("Missing text parameter for setCurrentText operation"));
    } else if (operation == "addItem") {
        auto text = context.getParameter<QString>("text");
        if (context.hasParameter("text")) {
            comboBox->addItem(text);
            return CommandResult<QVariant>(QString("ComboBox item added successfully"));
        }
        return CommandResult<QVariant>(QString("Missing text parameter for addItem operation"));
    } else if (operation == "clear") {
        comboBox->clear();
        return CommandResult<QVariant>(QString("ComboBox cleared successfully"));
    }

    return CommandResult<QVariant>(QString("Unknown operation: %1").arg(operation));
}

CommandResult<QVariant> ComboBoxCommand::undo(const CommandContext& context) {
    auto* comboBox = findComboBox(widget_name_);
    if (!comboBox) {
        return CommandResult<QVariant>(QString("ComboBox '%1' not found for undo").arg(widget_name_));
    }

    comboBox->setCurrentIndex(old_index_);
    return CommandResult<QVariant>(QString("ComboBox undo successful"));
}

bool ComboBoxCommand::canUndo(const CommandContext& context) const {
    return !widget_name_.isEmpty();
}

CommandMetadata ComboBoxCommand::getMetadata() const {
    return CommandMetadata("ComboBoxCommand", "Specialized command for ComboBox components");
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

}  // namespace ComponentCommands
}  // namespace Command
}  // namespace DeclarativeUI
