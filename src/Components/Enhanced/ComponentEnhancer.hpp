/**
 * @file ComponentEnhancer.hpp
 * @brief System for enhancing existing DeclarativeUI components with
 * accessibility, tooltips, and advanced features
 *
 * This system provides a non-intrusive way to add enhanced features to existing
 * components without modifying their core implementation. It uses composition
 * and decoration patterns to layer additional functionality on top of existing
 * components.
 */

#pragma once

#include <QCompleter>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QShortcut>
#include <QTimer>
#include <QToolTip>
#include <QValidator>
#include <QWidget>
#include <functional>
#include <map>
#include <memory>
#include "../../Core/Theme.hpp"
#include "../../Core/UIElement.hpp"

namespace DeclarativeUI::Components::Enhanced {

/**
 * @brief Enhancement configuration structure
 */
struct EnhancementConfig {
    // Accessibility
    QString accessible_name;
    QString accessible_description;
    QString tooltip_text;
    QString help_text;
    Core::AccessibilityRole accessibility_role =
        Core::AccessibilityRole::Generic;
    int tab_index = -1;

    // Visual effects
    bool drop_shadow = false;
    QColor shadow_color = QColor(0, 0, 0, 80);
    bool hover_effect = true;
    bool focus_effect = true;
    int border_radius = 0;
    QColor border_color;
    QColor focus_color;

    // Validation
    bool required = false;
    std::function<bool(QWidget*)> validator;
    QString validation_error_message;
    bool validate_on_change = true;
    bool validate_on_focus_lost = true;

    // Keyboard shortcuts
    QKeySequence shortcut;

    // Auto-completion (for input widgets)
    QStringList completions;

    // Formatting (for input widgets)
    QString input_mask;
    QValidator* custom_validator = nullptr;

    // Behavior
    bool select_all_on_focus = false;
    bool clear_on_escape = false;
    int debounce_delay = 0;

    // Event handlers
    std::function<void(bool)> on_validation_changed;
    std::function<void(bool)> on_focus_changed;
    std::function<void()> on_hover_enter;
    std::function<void()> on_hover_leave;
};

/**
 * @brief Component enhancer that adds features to existing widgets
 */
class ComponentEnhancer : public QObject {
    Q_OBJECT

public:
    explicit ComponentEnhancer(QObject* parent = nullptr);
    ~ComponentEnhancer() override = default;

    // Static enhancement methods
    static void enhance(QWidget* widget, const EnhancementConfig& config);
    static void enhanceAccessibility(QWidget* widget,
                                     const EnhancementConfig& config);
    static void enhanceVisuals(QWidget* widget,
                               const EnhancementConfig& config);
    static void enhanceValidation(QWidget* widget,
                                  const EnhancementConfig& config);
    static void enhanceKeyboard(QWidget* widget,
                                const EnhancementConfig& config);
    static void enhanceBehavior(QWidget* widget,
                                const EnhancementConfig& config);

    // Fluent interface for configuration
    static EnhancementConfig configure();

    // Utility methods
    static void addTooltip(QWidget* widget, const QString& tooltip);
    static void addDropShadow(QWidget* widget,
                              const QColor& color = QColor(0, 0, 0, 80));
    static void addHoverEffect(QWidget* widget);
    static void addFocusEffect(QWidget* widget,
                               const QColor& color = QColor("#3498db"));
    static void addValidation(QWidget* widget,
                              std::function<bool(QWidget*)> validator,
                              const QString& error_message = "");
    static void addShortcut(QWidget* widget, const QKeySequence& shortcut);
    static void addAutoComplete(QWidget* widget,
                                const QStringList& completions);

    // Component-specific enhancements
    static void enhanceButton(QWidget* button, const EnhancementConfig& config);
    static void enhanceLineEdit(QWidget* line_edit,
                                const EnhancementConfig& config);
    static void enhanceCheckBox(QWidget* checkbox,
                                const EnhancementConfig& config);
    static void enhanceComboBox(QWidget* combobox,
                                const EnhancementConfig& config);
    static void enhanceLabel(QWidget* label, const EnhancementConfig& config);

    // Batch enhancement
    static void enhanceContainer(QWidget* container,
                                 const EnhancementConfig& default_config);
    static void enhanceForm(
        QWidget* form,
        const std::map<QString, EnhancementConfig>& field_configs);

    // Enhancement removal
    static void removeEnhancements(QWidget* widget);
    static void removeAccessibilityEnhancements(QWidget* widget);
    static void removeVisualEnhancements(QWidget* widget);
    static void removeValidationEnhancements(QWidget* widget);

signals:
    void enhancementApplied(QWidget* widget, const QString& enhancement_type);
    void validationStateChanged(QWidget* widget, bool is_valid,
                                const QString& error);

private:
    static void setupAccessibility(QWidget* widget,
                                   const EnhancementConfig& config);
    static void setupVisualEffects(QWidget* widget,
                                   const EnhancementConfig& config);
    static void setupValidation(QWidget* widget,
                                const EnhancementConfig& config);
    static void setupKeyboardHandling(QWidget* widget,
                                      const EnhancementConfig& config);
    static void setupBehavior(QWidget* widget, const EnhancementConfig& config);
    static void setupEventHandlers(QWidget* widget,
                                   const EnhancementConfig& config);

    static QWidget* findInputWidget(QWidget* widget);
    static QString getWidgetType(QWidget* widget);
    static void applyStyleSheet(QWidget* widget,
                                const QString& additional_styles);

    // Enhancement tracking
    static std::map<QWidget*, EnhancementConfig> enhanced_widgets_;
    static std::map<QWidget*, std::vector<QObject*>> enhancement_objects_;
};

/**
 * @brief Fluent configuration builder for enhancements
 */
class EnhancementConfigBuilder {
public:
    EnhancementConfigBuilder() = default;

    // Accessibility
    EnhancementConfigBuilder& accessibleName(const QString& name);
    EnhancementConfigBuilder& accessibleDescription(const QString& description);
    EnhancementConfigBuilder& tooltip(const QString& tooltip);
    EnhancementConfigBuilder& helpText(const QString& help);
    EnhancementConfigBuilder& role(Core::AccessibilityRole role);
    EnhancementConfigBuilder& tabIndex(int index);

    // Visual effects
    EnhancementConfigBuilder& dropShadow(bool enabled = true,
                                         const QColor& color = QColor(0, 0, 0,
                                                                      80));
    EnhancementConfigBuilder& hoverEffect(bool enabled = true);
    EnhancementConfigBuilder& focusEffect(bool enabled = true);
    EnhancementConfigBuilder& borderRadius(int radius);
    EnhancementConfigBuilder& borderColor(const QColor& color);
    EnhancementConfigBuilder& focusColor(const QColor& color);

    // Validation
    EnhancementConfigBuilder& required(bool required = true);
    EnhancementConfigBuilder& validator(
        std::function<bool(QWidget*)> validator);
    EnhancementConfigBuilder& validationError(const QString& error);
    EnhancementConfigBuilder& validateOnChange(bool enabled = true);
    EnhancementConfigBuilder& validateOnFocusLost(bool enabled = true);

    // Keyboard
    EnhancementConfigBuilder& shortcut(const QKeySequence& shortcut);

    // Auto-completion
    EnhancementConfigBuilder& autoComplete(const QStringList& completions);

    // Formatting
    EnhancementConfigBuilder& inputMask(const QString& mask);
    EnhancementConfigBuilder& customValidator(QValidator* validator);

    // Behavior
    EnhancementConfigBuilder& selectAllOnFocus(bool enabled = true);
    EnhancementConfigBuilder& clearOnEscape(bool enabled = true);
    EnhancementConfigBuilder& debounceDelay(int milliseconds);

    // Event handlers
    EnhancementConfigBuilder& onValidationChanged(
        std::function<void(bool)> handler);
    EnhancementConfigBuilder& onFocusChanged(std::function<void(bool)> handler);
    EnhancementConfigBuilder& onHoverEnter(std::function<void()> handler);
    EnhancementConfigBuilder& onHoverLeave(std::function<void()> handler);

    // Build the configuration
    EnhancementConfig build() const;

    // Apply to widget
    void applyTo(QWidget* widget) const;

private:
    EnhancementConfig config_;
};

/**
 * @brief Factory function for creating enhancement configurations
 */
inline EnhancementConfigBuilder enhance() { return EnhancementConfigBuilder{}; }

/**
 * @brief Convenience macros for common enhancements
 */
#define ENHANCE_ACCESSIBILITY(widget, name, description)                       \
    ComponentEnhancer::enhance(widget, enhance()                               \
                                           .accessibleName(name)               \
                                           .accessibleDescription(description) \
                                           .build())

#define ENHANCE_VALIDATION(widget, validator_func, error_msg)          \
    ComponentEnhancer::enhance(widget, enhance()                       \
                                           .validator(validator_func)  \
                                           .validationError(error_msg) \
                                           .build())

#define ENHANCE_VISUAL(widget)  \
    ComponentEnhancer::enhance( \
        widget, enhance().dropShadow().hoverEffect().focusEffect().build())

#define ENHANCE_TOOLTIP(widget, tooltip_text) \
    ComponentEnhancer::addTooltip(widget, tooltip_text)

}  // namespace DeclarativeUI::Components::Enhanced
