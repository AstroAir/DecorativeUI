/**
 * @file ButtonEnhanced.hpp
 * @brief Enhanced Button component with accessibility, tooltips, and advanced features
 * 
 * This enhanced version of the Button component adds:
 * - Comprehensive accessibility support
 * - Tooltip management
 * - Icon positioning and styling
 * - Validation and error handling
 * - Advanced styling options
 * - Keyboard navigation support
 */

#pragma once

#include "../Button.hpp"
#include "../../Core/Theme.hpp"
#include <QToolTip>
#include <QShortcut>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <memory>

namespace DeclarativeUI::Components::Enhanced {

/**
 * @brief Enhanced button component with advanced features
 */
class ButtonEnhanced : public Components::Button {
    Q_OBJECT

public:
    explicit ButtonEnhanced(QObject* parent = nullptr);
    ~ButtonEnhanced() override = default;

    // Enhanced fluent interface
    ButtonEnhanced& tooltip(const QString& tooltip_text);
    ButtonEnhanced& accessibleName(const QString& name);
    ButtonEnhanced& accessibleDescription(const QString& description);
    ButtonEnhanced& shortcut(const QKeySequence& shortcut);
    ButtonEnhanced& iconPosition(Qt::ToolButtonStyle position);
    ButtonEnhanced& iconSize(const QSize& size);
    ButtonEnhanced& autoRepeat(bool enabled, int initial_delay = 300, int repeat_delay = 100);
    ButtonEnhanced& checkable(bool checkable);
    ButtonEnhanced& checked(bool checked);
    ButtonEnhanced& flat(bool flat);
    ButtonEnhanced& menu(QMenu* menu);
    
    // Visual enhancements
    ButtonEnhanced& dropShadow(bool enabled = true, const QColor& color = QColor(0, 0, 0, 80));
    ButtonEnhanced& hoverEffect(bool enabled = true);
    ButtonEnhanced& pressAnimation(bool enabled = true);
    ButtonEnhanced& borderRadius(int radius);
    ButtonEnhanced& gradient(const QColor& start, const QColor& end);
    
    // Validation and error handling
    ButtonEnhanced& validator(std::function<bool()> validation_func);
    ButtonEnhanced& onValidationFailed(std::function<void(const QString&)> error_handler);
    ButtonEnhanced& required(bool required = true);
    
    // State management
    ButtonEnhanced& loading(bool loading_state);
    ButtonEnhanced& loadingText(const QString& text);
    ButtonEnhanced& disabled(bool disabled, const QString& reason = "");
    
    // Event handlers
    ButtonEnhanced& onHover(std::function<void(bool)> hover_handler);
    ButtonEnhanced& onFocus(std::function<void(bool)> focus_handler);
    ButtonEnhanced& onDoubleClick(std::function<void()> double_click_handler);
    ButtonEnhanced& onRightClick(std::function<void()> right_click_handler);
    
    // Accessibility helpers
    ButtonEnhanced& role(const QString& aria_role);
    ButtonEnhanced& tabIndex(int index);
    ButtonEnhanced& describedBy(const QString& element_id);
    
    // Advanced features
    ButtonEnhanced& badge(const QString& badge_text, const QColor& badge_color = QColor("#e74c3c"));
    ButtonEnhanced& progress(int percentage); // For progress buttons
    ButtonEnhanced& multiState(const QStringList& states, int current_state = 0);
    
    void initialize() override;
    
    // State queries
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
    // Enhanced properties
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
    
    // Visual effects
    bool drop_shadow_enabled_;
    QColor shadow_color_;
    bool hover_effect_enabled_;
    bool press_animation_enabled_;
    int border_radius_;
    QColor gradient_start_;
    QColor gradient_end_;
    
    // Validation
    std::function<bool()> validation_func_;
    std::function<void(const QString&)> error_handler_;
    bool required_;
    QString validation_error_;
    
    // State management
    bool loading_state_;
    QString loading_text_;
    QString original_text_;
    bool disabled_state_;
    QString disabled_reason_;
    
    // Event handlers
    std::function<void(bool)> hover_handler_;
    std::function<void(bool)> focus_handler_;
    std::function<void()> double_click_handler_;
    std::function<void()> right_click_handler_;
    
    // Accessibility
    QString aria_role_;
    int tab_index_;
    QString described_by_;
    
    // Advanced features
    QString badge_text_;
    QColor badge_color_;
    int progress_percentage_;
    QStringList multi_states_;
    int current_state_;
    
    // Internal components
    std::unique_ptr<QShortcut> shortcut_obj_;
    std::unique_ptr<QGraphicsDropShadowEffect> shadow_effect_;
    std::unique_ptr<QPropertyAnimation> press_animation_;
    std::unique_ptr<QTimer> validation_timer_;
};

/**
 * @brief Factory function for creating enhanced buttons
 */
inline std::unique_ptr<ButtonEnhanced> createEnhancedButton() {
    return std::make_unique<ButtonEnhanced>();
}

} // namespace DeclarativeUI::Components::Enhanced
