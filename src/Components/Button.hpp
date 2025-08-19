// Components/Button.hpp
#pragma once
#include <QIcon>
#include <QPushButton>
#include <QMenu>
#include <QShortcut>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QTimer>
#include <QToolTip>
#include <memory>
#include <functional>

#include "../Core/UIElement.hpp"
#include "../Core/Theme.hpp"

namespace DeclarativeUI::Components {

class Button : public Core::UIElement {
    Q_OBJECT

public:
    explicit Button(QObject* parent = nullptr);

    // **Basic fluent interface (backward compatible)**
    Button& text(const QString& text);
    Button& icon(const QIcon& icon);
    Button& onClick(std::function<void()> handler);
    Button& enabled(bool enabled);
    Button& style(const QString& stylesheet);

    // **Enhanced fluent interface**
    Button& tooltip(const QString& tooltip_text);
    Button& accessibleName(const QString& name);
    Button& accessibleDescription(const QString& description);
    Button& shortcut(const QKeySequence& shortcut);
    Button& iconPosition(Qt::ToolButtonStyle position);
    Button& iconSize(const QSize& size);
    Button& autoRepeat(bool enabled, int initial_delay = 300, int repeat_delay = 100);
    Button& checkable(bool checkable);
    Button& checked(bool checked);
    Button& flat(bool flat);
    Button& menu(QMenu* menu);

    // **Visual enhancements**
    Button& dropShadow(bool enabled = true, const QColor& color = QColor(0, 0, 0, 80));
    Button& hoverEffect(bool enabled = true);
    Button& pressAnimation(bool enabled = true);
    Button& borderRadius(int radius);
    Button& gradient(const QColor& start, const QColor& end);

    // **Validation and error handling**
    Button& validator(std::function<bool()> validation_func);
    Button& onValidationFailed(std::function<void(const QString&)> error_handler);
    Button& required(bool required = true);

    // **State management**
    Button& loading(bool loading_state);
    Button& loadingText(const QString& text);
    Button& disabled(bool disabled, const QString& reason = "");

    // **Event handlers**
    Button& onHover(std::function<void(bool)> hover_handler);
    Button& onFocus(std::function<void(bool)> focus_handler);
    Button& onDoubleClick(std::function<void()> double_click_handler);
    Button& onRightClick(std::function<void()> right_click_handler);

    // **Accessibility helpers**
    Button& role(const QString& aria_role);
    Button& tabIndex(int index);
    Button& describedBy(const QString& element_id);

    // **Advanced features**
    Button& badge(const QString& badge_text, const QColor& badge_color = QColor("#e74c3c"));
    Button& progress(int percentage); // For progress buttons
    Button& multiState(const QStringList& states, int current_state = 0);

    void initialize() override;

    // **State queries**
    bool isLoading() const { return loading_state_; }
    bool isValid() const;
    QString getValidationError() const { return validation_error_; }
    int getCurrentState() const { return current_state_; }

signals:
    void validationChanged(bool is_valid);
    void stateChanged(int new_state);
    void loadingStateChanged(bool loading);

protected:
    void setupAccessibility();
    void setupVisualEffects();
    void setupEventHandlers();
    void setupValidation();
    void updateButtonState();
    void showValidationError(const QString& error);
    void updateLoadingState();
    void updateMultiState();

private slots:
    void onButtonHovered();
    void onButtonPressed();
    void onButtonReleased();
    void onValidationCheck();

private:
    QPushButton* button_widget_;

    // **Enhanced properties**
    QString tooltip_text_;
    QString accessible_name_;
    QString accessible_description_;
    QKeySequence shortcut_;
    Qt::ToolButtonStyle icon_position_;
    QSize icon_size_;
    bool auto_repeat_enabled_;
    int auto_repeat_initial_delay_;
    int auto_repeat_delay_;
    bool checkable_;
    bool checked_;
    bool flat_;
    QMenu* menu_;

    // **Visual effects**
    bool drop_shadow_enabled_;
    QColor shadow_color_;
    bool hover_effect_enabled_;
    bool press_animation_enabled_;
    int border_radius_;
    QColor gradient_start_;
    QColor gradient_end_;

    // **Validation**
    std::function<bool()> validation_func_;
    std::function<void(const QString&)> error_handler_;
    bool required_;
    QString validation_error_;

    // **State management**
    bool loading_state_;
    QString loading_text_;
    QString original_text_;
    bool disabled_state_;
    QString disabled_reason_;

    // **Event handlers**
    std::function<void(bool)> hover_handler_;
    std::function<void(bool)> focus_handler_;
    std::function<void()> double_click_handler_;
    std::function<void()> right_click_handler_;

    // **Accessibility**
    QString aria_role_;
    int tab_index_;
    QString described_by_;

    // **Advanced features**
    QString badge_text_;
    QColor badge_color_;
    int progress_percentage_;
    QStringList multi_states_;
    int current_state_;

    // **Internal components**
    std::unique_ptr<QShortcut> shortcut_obj_;
    std::unique_ptr<QGraphicsDropShadowEffect> shadow_effect_;
    std::unique_ptr<QPropertyAnimation> press_animation_;
    std::unique_ptr<QTimer> validation_timer_;
};

}  // namespace DeclarativeUI::Components
