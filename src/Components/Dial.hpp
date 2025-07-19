// Components/Dial.hpp
#pragma once
#include <QDial>

#include "../Core/UIElement.hpp"

namespace DeclarativeUI::Components {

class Dial : public Core::UIElement {
    Q_OBJECT

public:
    explicit Dial(QObject* parent = nullptr);

    // **Fluent interface for dial**
    Dial& minimum(int min);
    Dial& maximum(int max);
    Dial& value(int value);
    Dial& singleStep(int step);
    Dial& pageStep(int step);
    Dial& notchesVisible(bool visible);
    Dial& notchTarget(double target);
    Dial& wrapping(bool wrapping);
    Dial& onValueChanged(std::function<void(int)> handler);
    Dial& onSliderPressed(std::function<void()> handler);
    Dial& onSliderReleased(std::function<void()> handler);
    Dial& onSliderMoved(std::function<void(int)> handler);
    Dial& style(const QString& stylesheet);

    void initialize() override;
    int getValue() const;
    void setValue(int value);
    int getMinimum() const;
    int getMaximum() const;
    bool isWrapping() const;

private:
    QDial* dial_widget_;
    std::function<void(int)> value_changed_handler_;
    std::function<void()> slider_pressed_handler_;
    std::function<void()> slider_released_handler_;
    std::function<void(int)> slider_moved_handler_;
};



}  // namespace DeclarativeUI::Components
