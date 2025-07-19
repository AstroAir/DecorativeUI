// Components/MessageBox.cpp
#include "MessageBox.hpp"

namespace DeclarativeUI::Components {

// **Implementation**
MessageBox::MessageBox(QObject* parent)
    : UIElement(parent), message_box_widget_(nullptr) {}

MessageBox& MessageBox::icon(QMessageBox::Icon icon) {
    return static_cast<MessageBox&>(setProperty("icon", static_cast<int>(icon)));
}

MessageBox& MessageBox::text(const QString& text) {
    return static_cast<MessageBox&>(setProperty("text", text));
}

MessageBox& MessageBox::informativeText(const QString& text) {
    return static_cast<MessageBox&>(setProperty("informativeText", text));
}

MessageBox& MessageBox::detailedText(const QString& text) {
    return static_cast<MessageBox&>(setProperty("detailedText", text));
}

MessageBox& MessageBox::windowTitle(const QString& title) {
    return static_cast<MessageBox&>(setProperty("windowTitle", title));
}

MessageBox& MessageBox::standardButtons(QMessageBox::StandardButtons buttons) {
    return static_cast<MessageBox&>(setProperty("standardButtons", static_cast<int>(buttons)));
}

MessageBox& MessageBox::defaultButton(QMessageBox::StandardButton button) {
    if (message_box_widget_) {
        message_box_widget_->setDefaultButton(button);
    }
    return *this;
}

MessageBox& MessageBox::escapeButton(QMessageBox::StandardButton button) {
    if (message_box_widget_) {
        message_box_widget_->setEscapeButton(button);
    }
    return *this;
}

MessageBox& MessageBox::textFormat(Qt::TextFormat format) {
    return static_cast<MessageBox&>(setProperty("textFormat", static_cast<int>(format)));
}

MessageBox& MessageBox::onButtonClicked(std::function<void(QAbstractButton*)> handler) {
    button_clicked_handler_ = std::move(handler);
    return *this;
}

MessageBox& MessageBox::onFinished(std::function<void(int)> handler) {
    finished_handler_ = std::move(handler);
    return *this;
}

void MessageBox::initialize() {
    if (!message_box_widget_) {
        message_box_widget_ = new QMessageBox();
        setWidget(message_box_widget_);

        // Connect signals
        if (button_clicked_handler_) {
            connect(message_box_widget_, &QMessageBox::buttonClicked, this,
                    [this](QAbstractButton* button) { button_clicked_handler_(button); });
        }

        if (finished_handler_) {
            connect(message_box_widget_, QOverload<int>::of(&QMessageBox::finished), this,
                    [this](int result) { finished_handler_(result); });
        }
    }
}

int MessageBox::exec() {
    return message_box_widget_ ? message_box_widget_->exec() : QMessageBox::Cancel;
}

void MessageBox::show() {
    if (message_box_widget_) {
        message_box_widget_->show();
    }
}

void MessageBox::accept() {
    if (message_box_widget_) {
        message_box_widget_->accept();
    }
}

void MessageBox::reject() {
    if (message_box_widget_) {
        message_box_widget_->reject();
    }
}

QMessageBox::StandardButton MessageBox::getStandardButton(QAbstractButton* button) const {
    return message_box_widget_ ? message_box_widget_->standardButton(button) : QMessageBox::NoButton;
}

QAbstractButton* MessageBox::getButton(QMessageBox::StandardButton which) const {
    return message_box_widget_ ? message_box_widget_->button(which) : nullptr;
}

// **Static convenience methods**
QMessageBox::StandardButton MessageBox::information(QWidget* parent, const QString& title, 
                                                   const QString& text, 
                                                   QMessageBox::StandardButtons buttons,
                                                   QMessageBox::StandardButton defaultButton) {
    return QMessageBox::information(parent, title, text, buttons, defaultButton);
}

QMessageBox::StandardButton MessageBox::question(QWidget* parent, const QString& title, 
                                               const QString& text, 
                                               QMessageBox::StandardButtons buttons,
                                               QMessageBox::StandardButton defaultButton) {
    return QMessageBox::question(parent, title, text, buttons, defaultButton);
}

QMessageBox::StandardButton MessageBox::warning(QWidget* parent, const QString& title, 
                                              const QString& text, 
                                              QMessageBox::StandardButtons buttons,
                                              QMessageBox::StandardButton defaultButton) {
    return QMessageBox::warning(parent, title, text, buttons, defaultButton);
}

QMessageBox::StandardButton MessageBox::critical(QWidget* parent, const QString& title, 
                                               const QString& text, 
                                               QMessageBox::StandardButtons buttons,
                                               QMessageBox::StandardButton defaultButton) {
    return QMessageBox::critical(parent, title, text, buttons, defaultButton);
}

}  // namespace DeclarativeUI::Components
