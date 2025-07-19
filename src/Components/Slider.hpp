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



}  // namespace DeclarativeUI::Components
