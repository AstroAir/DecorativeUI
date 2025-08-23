// Components/PlainTextEdit.cpp
#include "PlainTextEdit.hpp"
#include <QPlainTextEdit>

namespace DeclarativeUI::Components {

PlainTextEdit::PlainTextEdit(QObject* parent)
    : UIElement(parent), plain_text_edit_widget_(nullptr) {}

void PlainTextEdit::initialize() {
    if (!plain_text_edit_widget_) {
        plain_text_edit_widget_ = new QPlainTextEdit();
        setWidget(plain_text_edit_widget_);

        // Connect signals
        connect(plain_text_edit_widget_, &QPlainTextEdit::textChanged, this,
                [this]() {
                    if (text_changed_handler_) {
                        text_changed_handler_();
                    }
                });

        connect(plain_text_edit_widget_, &QPlainTextEdit::selectionChanged,
                this, [this]() {
                    if (selection_changed_handler_) {
                        selection_changed_handler_();
                    }
                });
    }
}

PlainTextEdit& PlainTextEdit::plainText(const QString& text) {
    return static_cast<PlainTextEdit&>(setProperty("plainText", text));
}

PlainTextEdit& PlainTextEdit::placeholderText(const QString& placeholder) {
    return static_cast<PlainTextEdit&>(
        setProperty("placeholderText", placeholder));
}

PlainTextEdit& PlainTextEdit::readOnly(bool readOnly) {
    return static_cast<PlainTextEdit&>(setProperty("readOnly", readOnly));
}

PlainTextEdit& PlainTextEdit::lineWrapMode(QPlainTextEdit::LineWrapMode mode) {
    return static_cast<PlainTextEdit&>(
        setProperty("lineWrapMode", static_cast<int>(mode)));
}

PlainTextEdit& PlainTextEdit::maximumBlockCount(int maximum) {
    return static_cast<PlainTextEdit&>(
        setProperty("maximumBlockCount", maximum));
}

PlainTextEdit& PlainTextEdit::onTextChanged(std::function<void()> handler) {
    text_changed_handler_ = handler;
    return *this;
}

PlainTextEdit& PlainTextEdit::onSelectionChanged(
    std::function<void()> handler) {
    selection_changed_handler_ = handler;
    return *this;
}

QString PlainTextEdit::getPlainText() const {
    return plain_text_edit_widget_ ? plain_text_edit_widget_->toPlainText()
                                   : QString();
}

QString PlainTextEdit::getPlaceholderText() const {
    return plain_text_edit_widget_ ? plain_text_edit_widget_->placeholderText()
                                   : QString();
}

bool PlainTextEdit::isReadOnly() const {
    return plain_text_edit_widget_ ? plain_text_edit_widget_->isReadOnly()
                                   : false;
}

QPlainTextEdit::LineWrapMode PlainTextEdit::getLineWrapMode() const {
    return plain_text_edit_widget_ ? plain_text_edit_widget_->lineWrapMode()
                                   : QPlainTextEdit::WidgetWidth;
}

int PlainTextEdit::getMaximumBlockCount() const {
    return plain_text_edit_widget_
               ? plain_text_edit_widget_->maximumBlockCount()
               : 0;
}

}  // namespace DeclarativeUI::Components
