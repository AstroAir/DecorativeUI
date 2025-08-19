// Components/Slider.hpp
#pragma once
#include <QGraphicsDropShadowEffect>
#include <QLabel>
#include <QPropertyAnimation>
#include <QSlider>
#include <QTimer>
#include <QToolTip>
#include <functional>
#include <memory>

#include "../Core/UIElement.hpp"

namespace DeclarativeUI::Components {

/**
 * @brief Validation result for slider values
 */
struct SliderValidationResult {
    bool is_valid;
    QString error_message;
    QString suggestion;
    int corrected_value = -1;

    SliderValidationResult(bool valid = true, const QString& error = "",
                           const QString& hint = "", int value = -1)
        : is_valid(valid),
          error_message(error),
          suggestion(hint),
          corrected_value(value) {}
};

/**
 * @brief Custom tick mark configuration
 */
struct TickMark {
    int value;
    QString label;
    QColor color;
    bool major = true;

    TickMark(int val, const QString& lbl = "", const QColor& col = QColor(),
             bool maj = true)
        : value(val), label(lbl), color(col), major(maj) {}
};

class Slider : public Core::UIElement {
    Q_OBJECT

public:
    explicit Slider(QObject* parent = nullptr);

    // **Basic fluent interface (backward compatible)**
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

    // **Enhanced fluent interface**
    Slider& tooltip(const QString& tooltip_text);
    Slider& accessibleName(const QString& name);
    Slider& accessibleDescription(const QString& description);
    Slider& shortcut(const QKeySequence& shortcut);

    // **Validation**
    Slider& required(bool required = true);
    Slider& validator(
        std::function<SliderValidationResult(int)> validation_func);
    Slider& onValidationFailed(
        std::function<void(const QString&, int)> error_handler);
    Slider& validateOnChange(bool validate_on_change = true);
    Slider& range(int min, int max);
    Slider& step(int single_step, int page_step = -1);
    Slider& snapToTicks(bool enabled = true);

    // **Visual enhancements**
    Slider& dropShadow(bool enabled = true,
                       const QColor& color = QColor(0, 0, 0, 80));
    Slider& hoverEffect(bool enabled = true);
    Slider& valueAnimation(bool enabled = true);
    Slider& customColors(const QColor& handle, const QColor& groove = QColor(),
                         const QColor& fill = QColor());
    Slider& handleSize(const QSize& size);
    Slider& grooveHeight(int height);
    Slider& borderRadius(int radius);

    // **Value display and formatting**
    Slider& showValue(bool enabled = true);
    Slider& valueFormat(
        const QString& format = "%1");  // e.g., "%1%", "$%1", "%1 units"
    Slider& valuePosition(Qt::Alignment position = Qt::AlignTop);
    Slider& valuePrefix(const QString& prefix);
    Slider& valueSuffix(const QString& suffix);
    Slider& precision(int decimal_places = 0);

    // **Custom tick marks**
    Slider& customTicks(const QList<TickMark>& tick_marks);
    Slider& addTick(int value, const QString& label = "",
                    const QColor& color = QColor(), bool major = true);
    Slider& tickLabels(bool enabled = true);
    Slider& tickLabelRotation(int degrees = 0);
    Slider& tickLabelFont(const QFont& font);

    // **Interactive features**
    Slider& doubleClickReset(bool enabled = true, int reset_value = 0);
    Slider& mouseWheel(bool enabled = true);
    Slider& keyboardNavigation(bool enabled = true);
    Slider& contextMenu(QMenu* menu);
    Slider& draggable(bool enabled = true);

    // **Advanced behavior**
    Slider& invertedAppearance(bool inverted = true);
    Slider& invertedControls(bool inverted = true);
    Slider& tracking(bool enabled = true);
    Slider& autoRepeat(bool enabled = true);
    Slider& logarithmic(bool enabled = true);
    Slider& exponential(bool enabled = true, double base = 2.0);

    // **Event handlers**
    Slider& onHover(std::function<void(bool)> hover_handler);
    Slider& onFocus(std::function<void(bool)> focus_handler);
    Slider& onDoubleClick(std::function<void(int)> double_click_handler);
    Slider& onRightClick(std::function<void(int)> right_click_handler);
    Slider& onValidationChanged(
        std::function<void(bool, const QString&)> validation_handler);
    Slider& onRangeChanged(std::function<void(int, int)> range_handler);

    // **Accessibility**
    Slider& role(const QString& aria_role);
    Slider& tabIndex(int index);
    Slider& describedBy(const QString& element_id);
    Slider& labelledBy(const QString& element_id);

    // **Multi-handle support**
    Slider& rangeSlider(bool enabled = true);
    Slider& lowerValue(int value);
    Slider& upperValue(int value);
    Slider& onRangeValueChanged(std::function<void(int, int)> handler);

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
