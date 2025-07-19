// Components/LineEdit.cpp
#include "LineEdit.hpp"

namespace DeclarativeUI::Components {

// **Implementation**
LineEdit::LineEdit(QObject* parent)
    : UIElement(parent), line_edit_widget_(nullptr) {}

LineEdit& LineEdit::text(const QString& text) {
    return static_cast<LineEdit&>(setProperty("text", text));
}

LineEdit& LineEdit::placeholder(const QString& placeholder) {
    return static_cast<LineEdit&>(setProperty("placeholderText", placeholder));
}

LineEdit& LineEdit::readOnly(bool readonly) {
    return static_cast<LineEdit&>(setProperty("readOnly", readonly));
}

LineEdit& LineEdit::maxLength(int length) {
    return static_cast<LineEdit&>(setProperty("maxLength", length));
}

LineEdit& LineEdit::validator(QValidator* validator) {
    return static_cast<LineEdit&>(
        setProperty("validator", QVariant::fromValue(validator)));
}

LineEdit& LineEdit::echoMode(QLineEdit::EchoMode mode) {
    return static_cast<LineEdit&>(
        setProperty("echoMode", static_cast<int>(mode)));
}

LineEdit& LineEdit::onTextChanged(
    std::function<void(const QString&)> handler) {
    text_changed_handler_ = std::move(handler);
    return *this;
}

LineEdit& LineEdit::onTextEdited(
    std::function<void(const QString&)> handler) {
    text_edited_handler_ = std::move(handler);
    return *this;
}

LineEdit& LineEdit::onReturnPressed(std::function<void()> handler) {
    return_pressed_handler_ = std::move(handler);
    return *this;
}

LineEdit& LineEdit::style(const QString& stylesheet) {
    return static_cast<LineEdit&>(setProperty("styleSheet", stylesheet));
}

void LineEdit::initialize() {
    if (!line_edit_widget_) {
        line_edit_widget_ = new QLineEdit();
        setWidget(line_edit_widget_);

        // Connect signals
        if (text_changed_handler_) {
            connect(
                line_edit_widget_, &QLineEdit::textChanged, this,
                [this](const QString& text) { text_changed_handler_(text); });
        }

        if (text_edited_handler_) {
            connect(
                line_edit_widget_, &QLineEdit::textEdited, this,
                [this](const QString& text) { text_edited_handler_(text); });
        }

        if (return_pressed_handler_) {
            connect(line_edit_widget_, &QLineEdit::returnPressed, this,
                    [this]() { return_pressed_handler_(); });
        }
    }
}

QString LineEdit::getText() const {
    return line_edit_widget_ ? line_edit_widget_->text() : QString();
}

void LineEdit::setText(const QString& text) {
    if (line_edit_widget_) {
        line_edit_widget_->setText(text);
    }
}

}  // namespace DeclarativeUI::Components
