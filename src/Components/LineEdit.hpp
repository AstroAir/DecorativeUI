// Components/LineEdit.hpp
#pragma once
#include <QLineEdit>
#include <QValidator>

#include "../Core/UIElement.hpp"

namespace DeclarativeUI::Components {

class LineEdit : public Core::UIElement {
    Q_OBJECT

public:
    explicit LineEdit(QObject* parent = nullptr);

    // **Fluent interface for line edit**
    LineEdit& text(const QString& text);
    LineEdit& placeholder(const QString& placeholder);
    LineEdit& readOnly(bool readonly);
    LineEdit& maxLength(int length);
    LineEdit& validator(QValidator* validator);
    LineEdit& echoMode(QLineEdit::EchoMode mode);
    LineEdit& onTextChanged(std::function<void(const QString&)> handler);
    LineEdit& onTextEdited(std::function<void(const QString&)> handler);
    LineEdit& onReturnPressed(std::function<void()> handler);
    LineEdit& style(const QString& stylesheet);

    void initialize() override;
    QString getText() const;
    void setText(const QString& text);

private:
    QLineEdit* line_edit_widget_;
    std::function<void(const QString&)> text_changed_handler_;
    std::function<void(const QString&)> text_edited_handler_;
    std::function<void()> return_pressed_handler_;
};

// **Implementation**
inline LineEdit::LineEdit(QObject* parent)
    : UIElement(parent), line_edit_widget_(nullptr) {}

inline LineEdit& LineEdit::text(const QString& text) {
    return static_cast<LineEdit&>(setProperty("text", text));
}

inline LineEdit& LineEdit::placeholder(const QString& placeholder) {
    return static_cast<LineEdit&>(setProperty("placeholderText", placeholder));
}

inline LineEdit& LineEdit::readOnly(bool readonly) {
    return static_cast<LineEdit&>(setProperty("readOnly", readonly));
}

inline LineEdit& LineEdit::maxLength(int length) {
    return static_cast<LineEdit&>(setProperty("maxLength", length));
}

inline LineEdit& LineEdit::validator(QValidator* validator) {
    return static_cast<LineEdit&>(
        setProperty("validator", QVariant::fromValue(validator)));
}

inline LineEdit& LineEdit::echoMode(QLineEdit::EchoMode mode) {
    return static_cast<LineEdit&>(
        setProperty("echoMode", static_cast<int>(mode)));
}

inline LineEdit& LineEdit::onTextChanged(
    std::function<void(const QString&)> handler) {
    text_changed_handler_ = std::move(handler);
    return *this;
}

inline LineEdit& LineEdit::onTextEdited(
    std::function<void(const QString&)> handler) {
    text_edited_handler_ = std::move(handler);
    return *this;
}

inline LineEdit& LineEdit::onReturnPressed(std::function<void()> handler) {
    return_pressed_handler_ = std::move(handler);
    return *this;
}

inline LineEdit& LineEdit::style(const QString& stylesheet) {
    return static_cast<LineEdit&>(setProperty("styleSheet", stylesheet));
}

inline void LineEdit::initialize() {
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

inline QString LineEdit::getText() const {
    return line_edit_widget_ ? line_edit_widget_->text() : QString();
}

inline void LineEdit::setText(const QString& text) {
    if (line_edit_widget_) {
        line_edit_widget_->setText(text);
    }
}

}  // namespace DeclarativeUI::Components
