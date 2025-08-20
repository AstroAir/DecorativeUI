// Components/StatusBar.cpp
#include "StatusBar.hpp"

namespace DeclarativeUI::Components {

// **Implementation**
StatusBar::StatusBar(QObject* parent)
    : UIElement(parent), status_bar_widget_(nullptr) {}

StatusBar& StatusBar::showMessage(const QString& message, int timeout) {
    if (status_bar_widget_) {
        status_bar_widget_->showMessage(message, timeout);
    }
    return *this;
}

StatusBar& StatusBar::clearMessage() {
    if (status_bar_widget_) {
        status_bar_widget_->clearMessage();
    }
    return *this;
}

StatusBar& StatusBar::addWidget(QWidget* widget, int stretch) {
    if (status_bar_widget_ && widget) {
        status_bar_widget_->addWidget(widget, stretch);
    }
    return *this;
}

StatusBar& StatusBar::addPermanentWidget(QWidget* widget, int stretch) {
    if (status_bar_widget_ && widget) {
        status_bar_widget_->addPermanentWidget(widget, stretch);
    }
    return *this;
}

StatusBar& StatusBar::insertWidget(int index, QWidget* widget, int stretch) {
    if (status_bar_widget_ && widget) {
        status_bar_widget_->insertWidget(index, widget, stretch);
    }
    return *this;
}

StatusBar& StatusBar::insertPermanentWidget(int index, QWidget* widget,
                                            int stretch) {
    if (status_bar_widget_ && widget) {
        status_bar_widget_->insertPermanentWidget(index, widget, stretch);
    }
    return *this;
}

StatusBar& StatusBar::removeWidget(QWidget* widget) {
    if (status_bar_widget_ && widget) {
        status_bar_widget_->removeWidget(widget);
    }
    return *this;
}

StatusBar& StatusBar::setSizeGripEnabled(bool enabled) {
    return static_cast<StatusBar&>(setProperty("sizeGripEnabled", enabled));
}

StatusBar& StatusBar::onMessageChanged(
    std::function<void(const QString&)> handler) {
    message_changed_handler_ = std::move(handler);
    return *this;
}

StatusBar& StatusBar::style(const QString& stylesheet) {
    return static_cast<StatusBar&>(setProperty("styleSheet", stylesheet));
}

void StatusBar::initialize() {
    if (!status_bar_widget_) {
        status_bar_widget_ = new QStatusBar();
        setWidget(status_bar_widget_);

        // Connect signals
        if (message_changed_handler_) {
            connect(status_bar_widget_, &QStatusBar::messageChanged, this,
                    [this](const QString& message) {
                        message_changed_handler_(message);
                    });
        }
    }
}

QString StatusBar::currentMessage() const {
    return status_bar_widget_ ? status_bar_widget_->currentMessage()
                              : QString();
}

bool StatusBar::isSizeGripEnabled() const {
    return status_bar_widget_ ? status_bar_widget_->isSizeGripEnabled() : true;
}

void StatusBar::reformat() {
    if (status_bar_widget_) {
        // reformat() is protected, so we trigger a repaint instead
        status_bar_widget_->repaint();
    }
}

}  // namespace DeclarativeUI::Components
