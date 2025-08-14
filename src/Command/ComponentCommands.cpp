#include "ComponentCommands.hpp"
#include <QApplication>
#include <QDebug>

namespace DeclarativeUI {
namespace Command {
namespace ComponentCommands {

// ============================================================================
// BUTTON COMPONENTS
// ============================================================================

ButtonCommand::ButtonCommand(const CommandContext& context)
    : ICommand(nullptr) {}

CommandResult<QVariant> ButtonCommand::execute(const CommandContext& context) {
    auto widget_name = context.getParameter<QString>("widget");
    auto operation = context.getParameter<QString>("operation");

    if (!context.hasParameter("widget")) {
        return CommandResult<QVariant>(QString("Missing required parameter: widget"));
    }

    auto* button = findButton(widget_name);
    if (!button) {
        return CommandResult<QVariant>(QString("Button '%1' not found").arg(widget_name));
    }

    if (operation == "click" || operation.isEmpty()) {
        button->click();
        return CommandResult<QVariant>(QString("Button clicked successfully"));
    } else if (operation == "setText") {
        auto text = context.getParameter<QString>("text");
        if (context.hasParameter("text")) {
            button->setText(text);
            return CommandResult<QVariant>(QString("Button text set successfully"));
        }
        return CommandResult<QVariant>(QString("Missing text parameter for setText operation"));
    } else if (operation == "setEnabled") {
        auto enabled = context.getParameter<bool>("enabled");
        if (context.hasParameter("enabled")) {
            button->setEnabled(enabled);
            return CommandResult<QVariant>(QString("Button enabled state set successfully"));
        }
        return CommandResult<QVariant>(QString("Missing enabled parameter for setEnabled operation"));
    }

    return CommandResult<QVariant>(QString("Unknown operation: %1").arg(operation));
}

CommandMetadata ButtonCommand::getMetadata() const {
    return CommandMetadata("ButtonCommand", "Specialized command for Button components");
}

QPushButton* ButtonCommand::findButton(const QString& name) {
    for (auto* widget : QApplication::allWidgets()) {
        if (auto* button = qobject_cast<QPushButton*>(widget)) {
            if (button->objectName() == name) {
                return button;
            }
        }
    }
    return nullptr;
}

// ============================================================================
// CHECKBOX COMPONENTS
// ============================================================================

CheckBoxCommand::CheckBoxCommand(const CommandContext& context)
    : ICommand(nullptr) {}

CommandResult<QVariant> CheckBoxCommand::execute(const CommandContext& context) {
    auto widget_name = context.getParameter<QString>("widget");
    auto operation = context.getParameter<QString>("operation");

    if (!context.hasParameter("widget")) {
        return CommandResult<QVariant>(QString("Missing required parameter: widget"));
    }

    auto* checkbox = findCheckBox(widget_name);
    if (!checkbox) {
        return CommandResult<QVariant>(QString("CheckBox '%1' not found").arg(widget_name));
    }

    widget_name_ = widget_name;
    old_state_ = checkbox->isChecked();

    if (operation == "toggle" || operation.isEmpty()) {
        new_state_ = !old_state_;
        checkbox->setChecked(new_state_);
        return CommandResult<QVariant>(QString("CheckBox toggled successfully"));
    } else if (operation == "setChecked") {
        auto checked = context.getParameter<bool>("checked");
        if (context.hasParameter("checked")) {
            new_state_ = checked;
            checkbox->setChecked(new_state_);
            return CommandResult<QVariant>(QString("CheckBox state set successfully"));
        }
        return CommandResult<QVariant>(QString("Missing checked parameter for setChecked operation"));
    }

    return CommandResult<QVariant>(QString("Unknown operation: %1").arg(operation));
}

CommandResult<QVariant> CheckBoxCommand::undo(const CommandContext& context) {
    auto* checkbox = findCheckBox(widget_name_);
    if (!checkbox) {
        return CommandResult<QVariant>(QString("CheckBox '%1' not found for undo").arg(widget_name_));
    }

    checkbox->setChecked(old_state_);
    return CommandResult<QVariant>(QString("CheckBox undo successful"));
}

bool CheckBoxCommand::canUndo(const CommandContext& context) const {
    return !widget_name_.isEmpty();
}

CommandMetadata CheckBoxCommand::getMetadata() const {
    return CommandMetadata("CheckBoxCommand", "Specialized command for CheckBox components");
}

QCheckBox* CheckBoxCommand::findCheckBox(const QString& name) {
    for (auto* widget : QApplication::allWidgets()) {
        if (auto* checkbox = qobject_cast<QCheckBox*>(widget)) {
            if (checkbox->objectName() == name) {
                return checkbox;
            }
        }
    }
    return nullptr;
}

// ============================================================================
// RADIO BUTTON COMPONENTS
// ============================================================================

RadioButtonCommand::RadioButtonCommand(const CommandContext& context)
    : ICommand(nullptr) {}

CommandResult<QVariant> RadioButtonCommand::execute(const CommandContext& context) {
    auto widget_name = context.getParameter<QString>("widget");
    auto operation = context.getParameter<QString>("operation");

    if (!context.hasParameter("widget")) {
        return CommandResult<QVariant>(QString("Missing required parameter: widget"));
    }

    auto* radioButton = findRadioButton(widget_name);
    if (!radioButton) {
        return CommandResult<QVariant>(QString("RadioButton '%1' not found").arg(widget_name));
    }

    widget_name_ = widget_name;
    old_state_ = radioButton->isChecked();

    if (operation == "select" || operation.isEmpty()) {
        radioButton->setChecked(true);
        return CommandResult<QVariant>(QString("RadioButton selected successfully"));
    } else if (operation == "setChecked") {
        auto checked = context.getParameter<bool>("checked");
        if (context.hasParameter("checked")) {
            radioButton->setChecked(checked);
            return CommandResult<QVariant>(QString("RadioButton state set successfully"));
        }
        return CommandResult<QVariant>(QString("Missing checked parameter for setChecked operation"));
    }

    return CommandResult<QVariant>(QString("Unknown operation: %1").arg(operation));
}

CommandResult<QVariant> RadioButtonCommand::undo(const CommandContext& context) {
    auto* radioButton = findRadioButton(widget_name_);
    if (!radioButton) {
        return CommandResult<QVariant>(QString("RadioButton '%1' not found for undo").arg(widget_name_));
    }

    radioButton->setChecked(old_state_);
    return CommandResult<QVariant>(QString("RadioButton undo successful"));
}

bool RadioButtonCommand::canUndo(const CommandContext& context) const {
    return !widget_name_.isEmpty();
}

CommandMetadata RadioButtonCommand::getMetadata() const {
    return CommandMetadata("RadioButtonCommand", "Specialized command for RadioButton components");
}

QRadioButton* RadioButtonCommand::findRadioButton(const QString& name) {
    for (auto* widget : QApplication::allWidgets()) {
        if (auto* radioButton = qobject_cast<QRadioButton*>(widget)) {
            if (radioButton->objectName() == name) {
                return radioButton;
            }
        }
    }
    return nullptr;
}

// ============================================================================
// TEXT COMPONENTS
// ============================================================================

TextEditCommand::TextEditCommand(const CommandContext& context)
    : ICommand(nullptr) {}

CommandResult<QVariant> TextEditCommand::execute(const CommandContext& context) {
    auto widget_name = context.getParameter<QString>("widget");
    auto operation = context.getParameter<QString>("operation");

    if (!context.hasParameter("widget")) {
        return CommandResult<QVariant>(QString("Missing required parameter: widget"));
    }

    auto* textEdit = findTextEdit(widget_name);
    if (!textEdit) {
        return CommandResult<QVariant>(QString("TextEdit '%1' not found").arg(widget_name));
    }

    widget_name_ = widget_name;
    old_text_ = textEdit->toPlainText();
    operation_ = operation;

    if (operation == "setText" || operation.isEmpty()) {
        auto text = context.getParameter<QString>("text");
        if (context.hasParameter("text")) {
            new_text_ = text;
            textEdit->setPlainText(text);
            return CommandResult<QVariant>(QString("TextEdit text set successfully"));
        }
        return CommandResult<QVariant>(QString("Missing text parameter for setText operation"));
    } else if (operation == "append") {
        auto text = context.getParameter<QString>("text");
        if (context.hasParameter("text")) {
            textEdit->append(text);
            new_text_ = textEdit->toPlainText();
            return CommandResult<QVariant>(QString("Text appended successfully"));
        }
        return CommandResult<QVariant>(QString("Missing text parameter for append operation"));
    } else if (operation == "clear") {
        textEdit->clear();
        new_text_ = "";
        return CommandResult<QVariant>(QString("TextEdit cleared successfully"));
    }

    return CommandResult<QVariant>(QString("Unknown operation: %1").arg(operation));
}

CommandResult<QVariant> TextEditCommand::undo(const CommandContext& context) {
    auto* textEdit = findTextEdit(widget_name_);
    if (!textEdit) {
        return CommandResult<QVariant>(QString("TextEdit '%1' not found for undo").arg(widget_name_));
    }

    textEdit->setPlainText(old_text_);
    return CommandResult<QVariant>(QString("TextEdit undo successful"));
}

bool TextEditCommand::canUndo(const CommandContext& context) const {
    return !widget_name_.isEmpty();
}

CommandMetadata TextEditCommand::getMetadata() const {
    return CommandMetadata("TextEditCommand", "Specialized command for TextEdit components");
}

QTextEdit* TextEditCommand::findTextEdit(const QString& name) {
    for (auto* widget : QApplication::allWidgets()) {
        if (auto* textEdit = qobject_cast<QTextEdit*>(widget)) {
            if (textEdit->objectName() == name) {
                return textEdit;
            }
        }
    }
    return nullptr;
}

// ============================================================================
// LINE EDIT COMPONENTS
// ============================================================================

LineEditCommand::LineEditCommand(const CommandContext& context)
    : ICommand(nullptr) {}

CommandResult<QVariant> LineEditCommand::execute(const CommandContext& context) {
    auto widget_name = context.getParameter<QString>("widget");
    auto operation = context.getParameter<QString>("operation");

    if (!context.hasParameter("widget")) {
        return CommandResult<QVariant>(QString("Missing required parameter: widget"));
    }

    auto* lineEdit = findLineEdit(widget_name);
    if (!lineEdit) {
        return CommandResult<QVariant>(QString("LineEdit '%1' not found").arg(widget_name));
    }

    widget_name_ = widget_name;
    old_text_ = lineEdit->text();
    operation_ = operation;

    if (operation == "setText" || operation.isEmpty()) {
        auto text = context.getParameter<QString>("text");
        if (context.hasParameter("text")) {
            new_text_ = text;
            lineEdit->setText(text);
            return CommandResult<QVariant>(QString("LineEdit text set successfully"));
        }
        return CommandResult<QVariant>(QString("Missing text parameter for setText operation"));
    } else if (operation == "clear") {
        lineEdit->clear();
        new_text_ = "";
        return CommandResult<QVariant>(QString("LineEdit cleared successfully"));
    } else if (operation == "selectAll") {
        lineEdit->selectAll();
        return CommandResult<QVariant>(QString("LineEdit text selected successfully"));
    }

    return CommandResult<QVariant>(QString("Unknown operation: %1").arg(operation));
}

CommandResult<QVariant> LineEditCommand::undo(const CommandContext& context) {
    auto* lineEdit = findLineEdit(widget_name_);
    if (!lineEdit) {
        return CommandResult<QVariant>(QString("LineEdit '%1' not found for undo").arg(widget_name_));
    }

    lineEdit->setText(old_text_);
    return CommandResult<QVariant>(QString("LineEdit undo successful"));
}

bool LineEditCommand::canUndo(const CommandContext& context) const {
    return !widget_name_.isEmpty();
}

CommandMetadata LineEditCommand::getMetadata() const {
    return CommandMetadata("LineEditCommand", "Specialized command for LineEdit components");
}

QLineEdit* LineEditCommand::findLineEdit(const QString& name) {
    for (auto* widget : QApplication::allWidgets()) {
        if (auto* lineEdit = qobject_cast<QLineEdit*>(widget)) {
            if (lineEdit->objectName() == name) {
                return lineEdit;
            }
        }
    }
    return nullptr;
}

// ============================================================================
// LABEL COMPONENTS
// ============================================================================

LabelCommand::LabelCommand(const CommandContext& context)
    : ICommand(nullptr) {}

CommandResult<QVariant> LabelCommand::execute(const CommandContext& context) {
    auto widget_name = context.getParameter<QString>("widget");
    auto operation = context.getParameter<QString>("operation");

    if (!context.hasParameter("widget")) {
        return CommandResult<QVariant>(QString("Missing required parameter: widget"));
    }

    auto* label = findLabel(widget_name);
    if (!label) {
        return CommandResult<QVariant>(QString("Label '%1' not found").arg(widget_name));
    }

    widget_name_ = widget_name;
    old_text_ = label->text();

    if (operation == "setText" || operation.isEmpty()) {
        auto text = context.getParameter<QString>("text");
        if (context.hasParameter("text")) {
            new_text_ = text;
            label->setText(text);
            return CommandResult<QVariant>(QString("Label text set successfully"));
        }
        return CommandResult<QVariant>(QString("Missing text parameter for setText operation"));
    } else if (operation == "clear") {
        label->clear();
        new_text_ = "";
        return CommandResult<QVariant>(QString("Label cleared successfully"));
    }

    return CommandResult<QVariant>(QString("Unknown operation: %1").arg(operation));
}

CommandResult<QVariant> LabelCommand::undo(const CommandContext& context) {
    auto* label = findLabel(widget_name_);
    if (!label) {
        return CommandResult<QVariant>(QString("Label '%1' not found for undo").arg(widget_name_));
    }

    label->setText(old_text_);
    return CommandResult<QVariant>(QString("Label undo successful"));
}

bool LabelCommand::canUndo(const CommandContext& context) const {
    return !widget_name_.isEmpty();
}

CommandMetadata LabelCommand::getMetadata() const {
    return CommandMetadata("LabelCommand", "Specialized command for Label components");
}

QLabel* LabelCommand::findLabel(const QString& name) {
    for (auto* widget : QApplication::allWidgets()) {
        if (auto* label = qobject_cast<QLabel*>(widget)) {
            if (label->objectName() == name) {
                return label;
            }
        }
    }
    return nullptr;
}

// ============================================================================
// REGISTRATION FUNCTION
// ============================================================================

void registerComponentCommands() {
    // This will be implemented when we integrate with the command system
    qDebug() << "🔧 Component commands registered";
}

}  // namespace ComponentCommands
}  // namespace Command
}  // namespace DeclarativeUI
