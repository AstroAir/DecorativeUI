// Components/RadioButton.hpp
#pragma once
#include <QRadioButton>
#include <QButtonGroup>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>
#include <QTimer>
#include <QShortcut>
#include <memory>
#include <functional>

#include "../Core/UIElement.hpp"

namespace DeclarativeUI::Components {

/**
 * @brief Validation result for radio button groups
 */
struct RadioButtonValidationResult {
    bool is_valid;
    QString error_message;
    QString suggestion;

    RadioButtonValidationResult(bool valid = true, const QString& error = "", const QString& hint = "")
        : is_valid(valid), error_message(error), suggestion(hint) {}
};

class RadioButton : public Core::UIElement {
    Q_OBJECT

public:
    explicit RadioButton(QObject* parent = nullptr);

    // **Basic fluent interface (backward compatible)**
    RadioButton& text(const QString& text);
    RadioButton& checked(bool checked);
    RadioButton& autoExclusive(bool exclusive);
    RadioButton& onToggled(std::function<void(bool)> handler);
    RadioButton& onClicked(std::function<void()> handler);
    RadioButton& style(const QString& stylesheet);

    // **Enhanced fluent interface**
    RadioButton& tooltip(const QString& tooltip_text);
    RadioButton& accessibleName(const QString& name);
    RadioButton& accessibleDescription(const QString& description);
    RadioButton& shortcut(const QKeySequence& shortcut);
    RadioButton& group(const QString& group_name);
    RadioButton& value(const QVariant& value);

    // **Validation**
    RadioButton& required(bool required = true);
    RadioButton& validator(std::function<RadioButtonValidationResult(bool, const QString&)> validation_func);
    RadioButton& onValidationFailed(std::function<void(const QString&)> error_handler);
    RadioButton& validateOnChange(bool validate_on_change = true);

    // **Visual enhancements**
    RadioButton& dropShadow(bool enabled = true, const QColor& color = QColor(0, 0, 0, 80));
    RadioButton& hoverEffect(bool enabled = true);
    RadioButton& checkAnimation(bool enabled = true);
    RadioButton& borderRadius(int radius);
    RadioButton& customColors(const QColor& checked, const QColor& unchecked = QColor(), const QColor& text = QColor());
    RadioButton& customSize(const QSize& size);
    RadioButton& indicatorSize(const QSize& size);

    // **State management**
    RadioButton& disabled(bool disabled, const QString& reason = "");
    RadioButton& readOnly(bool readonly = true);
    RadioButton& exclusive(bool exclusive = true);

    // **Event handlers**
    RadioButton& onHover(std::function<void(bool)> hover_handler);
    RadioButton& onFocus(std::function<void(bool)> focus_handler);
    RadioButton& onDoubleClick(std::function<void()> double_click_handler);
    RadioButton& onRightClick(std::function<void()> right_click_handler);
    RadioButton& onValidationChanged(std::function<void(bool, const QString&)> validation_handler);
    RadioButton& onGroupChanged(std::function<void(const QString&)> group_handler);

    // **Accessibility**
    RadioButton& role(const QString& aria_role);
    RadioButton& tabIndex(int index);
    RadioButton& describedBy(const QString& element_id);
    RadioButton& labelledBy(const QString& element_id);

    // **Group management**
    RadioButton& groupValidator(std::function<bool(const QString&)> group_validation_func);
    RadioButton& groupRequired(bool required = true);
    RadioButton& onGroupValidationChanged(std::function<void(bool, const QString&)> group_validation_handler);

    void initialize() override;
    bool isChecked() const;
    void setChecked(bool checked);

private:
    QRadioButton* radio_button_widget_;
    std::function<void(bool)> toggled_handler_;
    std::function<void()> clicked_handler_;
};

class ButtonGroup : public Core::UIElement {
    Q_OBJECT

public:
    explicit ButtonGroup(QObject* parent = nullptr);

    // **Fluent interface for button group**
    ButtonGroup& addButton(QAbstractButton* button, int id = -1);
    ButtonGroup& removeButton(QAbstractButton* button);
    ButtonGroup& exclusive(bool exclusive);
    ButtonGroup& onButtonClicked(std::function<void(QAbstractButton*)> handler);
    ButtonGroup& onButtonToggled(std::function<void(QAbstractButton*, bool)> handler);
    ButtonGroup& onIdClicked(std::function<void(int)> handler);

    void initialize() override;
    QAbstractButton* checkedButton() const;
    int checkedId() const;
    QAbstractButton* button(int id) const;
    void setId(QAbstractButton* button, int id);

private:
    QButtonGroup* button_group_;
    std::function<void(QAbstractButton*)> button_clicked_handler_;
    std::function<void(QAbstractButton*, bool)> button_toggled_handler_;
    std::function<void(int)> id_clicked_handler_;
};



}  // namespace DeclarativeUI::Components
