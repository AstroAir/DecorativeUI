// Components/Dial.hpp
#pragma once
#include <QDial>

#include "../Core/UIElement.hpp"

namespace DeclarativeUI::Components {

class Dial : public Core::UIElement {
    Q_OBJECT

public:
    explicit Dial(QObject* parent = nullptr);

    // **Basic fluent interface (backward compatible)**
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

    // **Enhanced fluent interface**
    Dial& tooltip(const QString& tooltip_text);
    Dial& accessibleName(const QString& name);
    Dial& accessibleDescription(const QString& description);
    Dial& shortcut(const QKeySequence& shortcut);

    // **Validation**
    Dial& required(bool required = true);
    Dial& validator(std::function<bool(int)> validation_func);
    Dial& onValidationFailed(std::function<void(const QString&, int)> error_handler);
    Dial& validateOnChange(bool validate_on_change = true);
    Dial& range(int min, int max);
    Dial& step(int single_step, int page_step = -1);
    Dial& snapToNotches(bool enabled = true);

    // **Visual enhancements**
    Dial& dropShadow(bool enabled = true, const QColor& color = QColor(0, 0, 0, 80));
    Dial& hoverEffect(bool enabled = true);
    Dial& rotationAnimation(bool enabled = true);
    Dial& customColors(const QColor& handle, const QColor& background = QColor(), const QColor& notches = QColor());
    Dial& handleSize(int size);
    Dial& dialSize(const QSize& size);
    Dial& borderRadius(int radius);
    Dial& gradient(const QColor& start, const QColor& end);

    // **Value display and formatting**
    Dial& showValue(bool enabled = true);
    Dial& valueFormat(const QString& format = "%1"); // e.g., "%1°", "%1%", "%1 units"
    Dial& valuePosition(Qt::Alignment position = Qt::AlignCenter);
    Dial& valuePrefix(const QString& prefix);
    Dial& valueSuffix(const QString& suffix);
    Dial& precision(int decimal_places = 0);

    // **Custom notches and markers**
    Dial& customNotches(const QList<int>& notch_values);
    Dial& addNotch(int value, const QString& label = "", const QColor& color = QColor());
    Dial& notchLabels(bool enabled = true);
    Dial& notchLabelFont(const QFont& font);
    Dial& majorNotches(const QList<int>& major_values);
    Dial& minorNotches(int interval);

    // **Interactive features**
    Dial& doubleClickReset(bool enabled = true, int reset_value = 0);
    Dial& mouseWheel(bool enabled = true);
    Dial& keyboardNavigation(bool enabled = true);
    Dial& contextMenu(QMenu* menu);
    Dial& draggable(bool enabled = true);
    Dial& sensitivity(double sensitivity = 1.0);

    // **Advanced behavior**
    Dial& invertedAppearance(bool inverted = true);
    Dial& invertedControls(bool inverted = true);
    Dial& tracking(bool enabled = true);
    Dial& autoRepeat(bool enabled = true);
    Dial& logarithmic(bool enabled = true);
    Dial& exponential(bool enabled = true, double base = 2.0);
    Dial& deadZone(int degrees = 0);

    // **Event handlers**
    Dial& onHover(std::function<void(bool)> hover_handler);
    Dial& onFocus(std::function<void(bool)> focus_handler);
    Dial& onDoubleClick(std::function<void(int)> double_click_handler);
    Dial& onRightClick(std::function<void(int)> right_click_handler);
    Dial& onValidationChanged(std::function<void(bool, const QString&)> validation_handler);
    Dial& onRangeChanged(std::function<void(int, int)> range_handler);
    Dial& onRotationStarted(std::function<void()> rotation_started_handler);
    Dial& onRotationFinished(std::function<void()> rotation_finished_handler);

    // **Accessibility**
    Dial& role(const QString& aria_role);
    Dial& tabIndex(int index);
    Dial& describedBy(const QString& element_id);
    Dial& labelledBy(const QString& element_id);

    // **Multi-dial support**
    Dial& concentricDial(bool enabled = true);
    Dial& innerValue(int value);
    Dial& outerValue(int value);
    Dial& onInnerValueChanged(std::function<void(int)> handler);
    Dial& onOuterValueChanged(std::function<void(int)> handler);

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
