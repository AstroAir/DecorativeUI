// Components/CheckBox.hpp
#pragma once
#include <QCheckBox>
#include <QButtonGroup>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>
#include <QTimer>
#include <memory>
#include <functional>

#include "../Core/UIElement.hpp"

namespace DeclarativeUI::Components {

/**
 * @brief Validation result for checkbox groups
 */
struct CheckBoxValidationResult {
    bool is_valid;
    QString error_message;
    QString suggestion;

    CheckBoxValidationResult(bool valid = true, const QString& error = "", const QString& hint = "")
        : is_valid(valid), error_message(error), suggestion(hint) {}
};

class CheckBox : public Core::UIElement {
    Q_OBJECT

public:
    explicit CheckBox(QObject *parent = nullptr);

    // **Basic fluent interface (backward compatible)**
    CheckBox &text(const QString &text);
    CheckBox &checked(bool checked);
    CheckBox &tristate(bool tristate);
    CheckBox &onStateChanged(std::function<void(int)> handler);
    CheckBox &onToggled(std::function<void(bool)> handler);
    CheckBox &style(const QString &stylesheet);

    // **Enhanced fluent interface**
    CheckBox &tooltip(const QString& tooltip_text);
    CheckBox &accessibleName(const QString& name);
    CheckBox &accessibleDescription(const QString& description);
    CheckBox &shortcut(const QKeySequence& shortcut);
    CheckBox &group(QButtonGroup* group);
    CheckBox &groupName(const QString& group_name);

    // **Validation**
    CheckBox &required(bool required = true);
    CheckBox &validator(std::function<CheckBoxValidationResult(Qt::CheckState)> validation_func);
    CheckBox &onValidationFailed(std::function<void(const QString&)> error_handler);
    CheckBox &validateOnChange(bool validate_on_change = true);

    // **Visual enhancements**
    CheckBox &dropShadow(bool enabled = true, const QColor& color = QColor(0, 0, 0, 80));
    CheckBox &hoverEffect(bool enabled = true);
    CheckBox &checkAnimation(bool enabled = true);
    CheckBox &borderRadius(int radius);
    CheckBox &customColors(const QColor& checked_color, const QColor& unchecked_color = QColor());
    CheckBox &size(const QSize& size);

    // **State management**
    CheckBox &disabled(bool disabled, const QString& reason = "");
    CheckBox &readOnly(bool readonly = true);

    // **Event handlers**
    CheckBox &onHover(std::function<void(bool)> hover_handler);
    CheckBox &onFocus(std::function<void(bool)> focus_handler);
    CheckBox &onDoubleClick(std::function<void()> double_click_handler);
    CheckBox &onRightClick(std::function<void()> right_click_handler);
    CheckBox &onValidationChanged(std::function<void(bool, const QString&)> validation_handler);

    // **Accessibility**
    CheckBox &role(const QString& aria_role);
    CheckBox &tabIndex(int index);
    CheckBox &describedBy(const QString& element_id);
    CheckBox &labelledBy(const QString& element_id);

    // **Group management**
    CheckBox &exclusiveGroup(bool exclusive = true);
    CheckBox &groupValidation(std::function<CheckBoxValidationResult(const QList<CheckBox*>&)> group_validator);

    void initialize() override;
    bool isChecked() const;
    void setChecked(bool checked);
    Qt::CheckState checkState() const;
    void setCheckState(Qt::CheckState state);

    // **State queries**
    bool isValid() const { return is_valid_; }
    QString getValidationError() const { return validation_error_; }
    QButtonGroup* getGroup() const { return button_group_; }
    QString getGroupName() const { return group_name_; }

signals:
    void validationChanged(bool is_valid, const QString& error_message);
    void groupStateChanged(const QString& group_name, const QList<CheckBox*>& checked_items);

protected:
    void setupAccessibility();
    void setupVisualEffects();
    void setupEventHandlers();
    void setupValidation();
    void setupGroupManagement();
    void updateCheckBoxState();
    CheckBoxValidationResult validateState(Qt::CheckState state) const;
    void showValidationError(const QString& error);
    void animateCheck(bool checked);

private slots:
    void onStateChangedInternal(int state);
    void onValidationTimer();
    void onGroupValidation();

private:
    QCheckBox *checkbox_widget_;
    std::function<void(int)> state_changed_handler_;
    std::function<void(bool)> toggled_handler_;

    // **Enhanced properties**
    QString tooltip_text_;
    QString accessible_name_;
    QString accessible_description_;
    QKeySequence shortcut_;
    QButtonGroup* button_group_;
    QString group_name_;

    // **Validation**
    bool required_;
    std::function<CheckBoxValidationResult(Qt::CheckState)> validation_func_;
    std::function<void(const QString&)> error_handler_;
    bool validate_on_change_;
    QString validation_error_;
    bool is_valid_;

    // **Visual effects**
    bool drop_shadow_enabled_;
    QColor shadow_color_;
    bool hover_effect_enabled_;
    bool check_animation_enabled_;
    int border_radius_;
    QColor checked_color_;
    QColor unchecked_color_;
    QSize custom_size_;

    // **State management**
    bool disabled_state_;
    QString disabled_reason_;
    bool read_only_;

    // **Event handlers**
    std::function<void(bool)> hover_handler_;
    std::function<void(bool)> focus_handler_;
    std::function<void()> double_click_handler_;
    std::function<void()> right_click_handler_;
    std::function<void(bool, const QString&)> validation_handler_;

    // **Accessibility**
    QString aria_role_;
    int tab_index_;
    QString described_by_;
    QString labelled_by_;

    // **Group management**
    bool exclusive_group_;
    std::function<CheckBoxValidationResult(const QList<CheckBox*>&)> group_validator_;

    // **Internal components**
    std::unique_ptr<QGraphicsDropShadowEffect> shadow_effect_;
    std::unique_ptr<QPropertyAnimation> check_animation_;
    std::unique_ptr<QTimer> validation_timer_;

    // **Static group management**
    static QMap<QString, QList<CheckBox*>> named_groups_;
    static QMap<QString, std::function<CheckBoxValidationResult(const QList<CheckBox*>&)>> group_validators_;
};

}  // namespace DeclarativeUI::Components
