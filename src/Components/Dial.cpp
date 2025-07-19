// Components/Dial.cpp
#include "Dial.hpp"

namespace DeclarativeUI::Components {

// **Implementation**
Dial::Dial(QObject* parent)
    : UIElement(parent), dial_widget_(nullptr) {}

Dial& Dial::minimum(int min) {
    return static_cast<Dial&>(setProperty("minimum", min));
}

Dial& Dial::maximum(int max) {
    return static_cast<Dial&>(setProperty("maximum", max));
}

Dial& Dial::value(int value) {
    return static_cast<Dial&>(setProperty("value", value));
}

Dial& Dial::singleStep(int step) {
    return static_cast<Dial&>(setProperty("singleStep", step));
}

Dial& Dial::pageStep(int step) {
    return static_cast<Dial&>(setProperty("pageStep", step));
}

Dial& Dial::notchesVisible(bool visible) {
    return static_cast<Dial&>(setProperty("notchesVisible", visible));
}

Dial& Dial::notchTarget(double target) {
    return static_cast<Dial&>(setProperty("notchTarget", target));
}

Dial& Dial::wrapping(bool wrapping) {
    return static_cast<Dial&>(setProperty("wrapping", wrapping));
}

Dial& Dial::onValueChanged(std::function<void(int)> handler) {
    value_changed_handler_ = std::move(handler);
    return *this;
}

Dial& Dial::onSliderPressed(std::function<void()> handler) {
    slider_pressed_handler_ = std::move(handler);
    return *this;
}

Dial& Dial::onSliderReleased(std::function<void()> handler) {
    slider_released_handler_ = std::move(handler);
    return *this;
}

Dial& Dial::onSliderMoved(std::function<void(int)> handler) {
    slider_moved_handler_ = std::move(handler);
    return *this;
}

Dial& Dial::style(const QString& stylesheet) {
    return static_cast<Dial&>(setProperty("styleSheet", stylesheet));
}

void Dial::initialize() {
    if (!dial_widget_) {
        dial_widget_ = new QDial();
        setWidget(dial_widget_);

        // Connect signals
        if (value_changed_handler_) {
            connect(dial_widget_, &QDial::valueChanged, this,
                    [this](int value) { value_changed_handler_(value); });
        }

        if (slider_pressed_handler_) {
            connect(dial_widget_, &QDial::sliderPressed, this,
                    [this]() { slider_pressed_handler_(); });
        }

        if (slider_released_handler_) {
            connect(dial_widget_, &QDial::sliderReleased, this,
                    [this]() { slider_released_handler_(); });
        }

        if (slider_moved_handler_) {
            connect(dial_widget_, &QDial::sliderMoved, this,
                    [this](int value) { slider_moved_handler_(value); });
        }
    }
}

int Dial::getValue() const {
    return dial_widget_ ? dial_widget_->value() : 0;
}

void Dial::setValue(int value) {
    if (dial_widget_) {
        dial_widget_->setValue(value);
    }
}

int Dial::getMinimum() const {
    return dial_widget_ ? dial_widget_->minimum() : 0;
}

int Dial::getMaximum() const {
    return dial_widget_ ? dial_widget_->maximum() : 100;
}

bool Dial::isWrapping() const {
    return dial_widget_ ? dial_widget_->wrapping() : false;
}

}  // namespace DeclarativeUI::Components
