// Components/Slider.cpp
#include "Slider.hpp"

namespace DeclarativeUI::Components {

// **Implementation**
Slider::Slider(QObject* parent)
    : UIElement(parent), slider_widget_(nullptr) {}

Slider& Slider::orientation(Qt::Orientation orientation) {
    return static_cast<Slider&>(
        setProperty("orientation", static_cast<int>(orientation)));
}

Slider& Slider::minimum(int min) {
    return static_cast<Slider&>(setProperty("minimum", min));
}

Slider& Slider::maximum(int max) {
    return static_cast<Slider&>(setProperty("maximum", max));
}

Slider& Slider::value(int value) {
    return static_cast<Slider&>(setProperty("value", value));
}

Slider& Slider::singleStep(int step) {
    return static_cast<Slider&>(setProperty("singleStep", step));
}

Slider& Slider::pageStep(int step) {
    return static_cast<Slider&>(setProperty("pageStep", step));
}

Slider& Slider::tickPosition(QSlider::TickPosition position) {
    return static_cast<Slider&>(
        setProperty("tickPosition", static_cast<int>(position)));
}

Slider& Slider::tickInterval(int interval) {
    return static_cast<Slider&>(setProperty("tickInterval", interval));
}

Slider& Slider::onValueChanged(std::function<void(int)> handler) {
    value_changed_handler_ = std::move(handler);
    return *this;
}

Slider& Slider::onSliderPressed(std::function<void()> handler) {
    slider_pressed_handler_ = std::move(handler);
    return *this;
}

Slider& Slider::onSliderReleased(std::function<void()> handler) {
    slider_released_handler_ = std::move(handler);
    return *this;
}

Slider& Slider::style(const QString& stylesheet) {
    return static_cast<Slider&>(setProperty("styleSheet", stylesheet));
}

void Slider::initialize() {
    if (!slider_widget_) {
        slider_widget_ = new QSlider();
        setWidget(slider_widget_);

        // Connect signals
        if (value_changed_handler_) {
            connect(slider_widget_, &QSlider::valueChanged, this,
                    [this](int value) { value_changed_handler_(value); });
        }

        if (slider_pressed_handler_) {
            connect(slider_widget_, &QSlider::sliderPressed, this,
                    [this]() { slider_pressed_handler_(); });
        }

        if (slider_released_handler_) {
            connect(slider_widget_, &QSlider::sliderReleased, this,
                    [this]() { slider_released_handler_(); });
        }
    }
}

int Slider::getValue() const {
    return slider_widget_ ? slider_widget_->value() : 0;
}

void Slider::setValue(int value) {
    if (slider_widget_) {
        slider_widget_->setValue(value);
    }
}

int Slider::getMinimum() const {
    return slider_widget_ ? slider_widget_->minimum() : 0;
}

int Slider::getMaximum() const {
    return slider_widget_ ? slider_widget_->maximum() : 100;
}

}  // namespace DeclarativeUI::Components
