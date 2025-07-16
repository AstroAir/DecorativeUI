// Components/Slider.hpp
#pragma once
#include <QSlider>

#include "../Core/UIElement.hpp"

namespace DeclarativeUI::Components {

class Slider : public Core::UIElement {
    Q_OBJECT

public:
    explicit Slider(QObject* parent = nullptr);

    // **Fluent interface for slider**
    Slider& orientation(Qt::Orientation orientation);
    Slider& minimum(int min);
    Slider& maximum(int max);
    Slider& value(int value);
    Slider& singleStep(int step);
    Slider& pageStep(int step);
    Slider& tickPosition(QSlider::TickPosition position);
    Slider& tickInterval(int interval);
    Slider& onValueChanged(std::function<void(int)> handler);
    Slider& onSliderPressed(std::function<void()> handler);
    Slider& onSliderReleased(std::function<void()> handler);
    Slider& style(const QString& stylesheet);

    void initialize() override;
    int getValue() const;
    void setValue(int value);
    int getMinimum() const;
    int getMaximum() const;

private:
    QSlider* slider_widget_;
    std::function<void(int)> value_changed_handler_;
    std::function<void()> slider_pressed_handler_;
    std::function<void()> slider_released_handler_;
};

// **Implementation**
inline Slider::Slider(QObject* parent)
    : UIElement(parent), slider_widget_(nullptr) {}

inline Slider& Slider::orientation(Qt::Orientation orientation) {
    return static_cast<Slider&>(
        setProperty("orientation", static_cast<int>(orientation)));
}

inline Slider& Slider::minimum(int min) {
    return static_cast<Slider&>(setProperty("minimum", min));
}

inline Slider& Slider::maximum(int max) {
    return static_cast<Slider&>(setProperty("maximum", max));
}

inline Slider& Slider::value(int value) {
    return static_cast<Slider&>(setProperty("value", value));
}

inline Slider& Slider::singleStep(int step) {
    return static_cast<Slider&>(setProperty("singleStep", step));
}

inline Slider& Slider::pageStep(int step) {
    return static_cast<Slider&>(setProperty("pageStep", step));
}

inline Slider& Slider::tickPosition(QSlider::TickPosition position) {
    return static_cast<Slider&>(
        setProperty("tickPosition", static_cast<int>(position)));
}

inline Slider& Slider::tickInterval(int interval) {
    return static_cast<Slider&>(setProperty("tickInterval", interval));
}

inline Slider& Slider::onValueChanged(std::function<void(int)> handler) {
    value_changed_handler_ = std::move(handler);
    return *this;
}

inline Slider& Slider::onSliderPressed(std::function<void()> handler) {
    slider_pressed_handler_ = std::move(handler);
    return *this;
}

inline Slider& Slider::onSliderReleased(std::function<void()> handler) {
    slider_released_handler_ = std::move(handler);
    return *this;
}

inline Slider& Slider::style(const QString& stylesheet) {
    return static_cast<Slider&>(setProperty("styleSheet", stylesheet));
}

inline void Slider::initialize() {
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

inline int Slider::getValue() const {
    return slider_widget_ ? slider_widget_->value() : 0;
}

inline void Slider::setValue(int value) {
    if (slider_widget_) {
        slider_widget_->setValue(value);
    }
}

inline int Slider::getMinimum() const {
    return slider_widget_ ? slider_widget_->minimum() : 0;
}

inline int Slider::getMaximum() const {
    return slider_widget_ ? slider_widget_->maximum() : 100;
}

}  // namespace DeclarativeUI::Components
