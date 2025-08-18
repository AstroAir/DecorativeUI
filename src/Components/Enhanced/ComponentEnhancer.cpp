/**
 * @file ComponentEnhancer.cpp
 * @brief Implementation of component enhancement system
 */

#include "ComponentEnhancer.hpp"
#include <QPushButton>
#include <QLineEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QApplication>
#include <QDebug>

namespace DeclarativeUI::Components::Enhanced {

// Static member initialization
std::map<QWidget*, EnhancementConfig> ComponentEnhancer::enhanced_widgets_;
std::map<QWidget*, std::vector<QObject*>> ComponentEnhancer::enhancement_objects_;

ComponentEnhancer::ComponentEnhancer(QObject* parent)
    : QObject(parent)
{
}

void ComponentEnhancer::enhance(QWidget* widget, const EnhancementConfig& config) {
    if (!widget) {
        qWarning() << "Cannot enhance null widget";
        return;
    }
    
    try {
        // Store the configuration
        enhanced_widgets_[widget] = config;
        
        // Apply enhancements
        setupAccessibility(widget, config);
        setupVisualEffects(widget, config);
        setupValidation(widget, config);
        setupKeyboardHandling(widget, config);
        setupBehavior(widget, config);
        setupEventHandlers(widget, config);
        
        qDebug() << "Enhanced widget:" << widget->metaObject()->className();
        
    } catch (const std::exception& e) {
        qWarning() << "Failed to enhance widget:" << e.what();
    }
}

void ComponentEnhancer::enhanceAccessibility(QWidget* widget, const EnhancementConfig& config) {
    setupAccessibility(widget, config);
}

void ComponentEnhancer::enhanceVisuals(QWidget* widget, const EnhancementConfig& config) {
    setupVisualEffects(widget, config);
}

void ComponentEnhancer::enhanceValidation(QWidget* widget, const EnhancementConfig& config) {
    setupValidation(widget, config);
}

void ComponentEnhancer::enhanceKeyboard(QWidget* widget, const EnhancementConfig& config) {
    setupKeyboardHandling(widget, config);
}

void ComponentEnhancer::enhanceBehavior(QWidget* widget, const EnhancementConfig& config) {
    setupBehavior(widget, config);
}

EnhancementConfig ComponentEnhancer::configure() {
    return EnhancementConfig{};
}

void ComponentEnhancer::addTooltip(QWidget* widget, const QString& tooltip) {
    if (widget && !tooltip.isEmpty()) {
        widget->setToolTip(tooltip);
    }
}

void ComponentEnhancer::addDropShadow(QWidget* widget, const QColor& color) {
    if (!widget) return;
    
    auto* shadow = new QGraphicsDropShadowEffect(widget);
    shadow->setBlurRadius(10);
    shadow->setColor(color);
    shadow->setOffset(0, 2);
    widget->setGraphicsEffect(shadow);
    
    // Track the effect for cleanup
    enhancement_objects_[widget].push_back(shadow);
}

void ComponentEnhancer::addHoverEffect(QWidget* widget) {
    if (!widget) return;
    
    widget->setAttribute(Qt::WA_Hover, true);
    
    QString current_style = widget->styleSheet();
    QString hover_style = current_style + 
        "QWidget:hover { background-color: rgba(52, 152, 219, 0.1); }";
    widget->setStyleSheet(hover_style);
}

void ComponentEnhancer::addFocusEffect(QWidget* widget, const QColor& color) {
    if (!widget) return;
    
    QString current_style = widget->styleSheet();
    QString focus_style = current_style + 
        QString("QWidget:focus { border: 2px solid %1; }").arg(color.name());
    widget->setStyleSheet(focus_style);
}

void ComponentEnhancer::addValidation(QWidget* widget, std::function<bool(QWidget*)> validator, const QString& error_message) {
    if (!widget || !validator) return;
    
    // Create validation timer
    auto* timer = new QTimer(widget);
    timer->setSingleShot(true);
    timer->setInterval(500);
    
    // Connect validation logic
    QObject::connect(timer, &QTimer::timeout, [widget, validator, error_message]() {
        bool is_valid = validator(widget);
        
        if (!is_valid) {
            // Show error styling
            QString error_style = widget->styleSheet() + 
                "QWidget { border: 2px solid #e74c3c; }";
            widget->setStyleSheet(error_style);
            
            if (!error_message.isEmpty()) {
                QToolTip::showText(widget->mapToGlobal(QPoint(0, widget->height())), 
                                 error_message, widget);
            }
        } else {
            // Show success styling
            QString success_style = widget->styleSheet() + 
                "QWidget { border: 2px solid #27ae60; }";
            widget->setStyleSheet(success_style);
        }
    });
    
    // Trigger validation on text change for input widgets
    if (auto* line_edit = qobject_cast<QLineEdit*>(widget)) {
        QObject::connect(line_edit, &QLineEdit::textChanged, [timer]() {
            timer->start();
        });
    }
    
    enhancement_objects_[widget].push_back(timer);
}

void ComponentEnhancer::addShortcut(QWidget* widget, const QKeySequence& shortcut) {
    if (!widget || shortcut.isEmpty()) return;
    
    auto* shortcut_obj = new QShortcut(shortcut, widget);
    
    // Connect to appropriate action based on widget type
    if (auto* button = qobject_cast<QPushButton*>(widget)) {
        QObject::connect(shortcut_obj, &QShortcut::activated, button, &QPushButton::click);
    } else if (auto* line_edit = qobject_cast<QLineEdit*>(widget)) {
        QObject::connect(shortcut_obj, &QShortcut::activated, line_edit, 
                        QOverload<>::of(&QLineEdit::setFocus));
    }
    
    enhancement_objects_[widget].push_back(shortcut_obj);
}

void ComponentEnhancer::addAutoComplete(QWidget* widget, const QStringList& completions) {
    auto* line_edit = qobject_cast<QLineEdit*>(widget);
    if (!line_edit || completions.isEmpty()) return;
    
    auto* completer = new QCompleter(completions, line_edit);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    line_edit->setCompleter(completer);
    
    enhancement_objects_[widget].push_back(completer);
}

void ComponentEnhancer::enhanceButton(QWidget* button, const EnhancementConfig& config) {
    auto* push_button = qobject_cast<QPushButton*>(button);
    if (!push_button) return;
    
    enhance(button, config);
    
    // Button-specific enhancements
    if (!config.shortcut.isEmpty()) {
        push_button->setShortcut(config.shortcut);
    }
}

void ComponentEnhancer::enhanceLineEdit(QWidget* line_edit, const EnhancementConfig& config) {
    auto* edit = qobject_cast<QLineEdit*>(line_edit);
    if (!edit) return;
    
    enhance(line_edit, config);
    
    // LineEdit-specific enhancements
    if (!config.input_mask.isEmpty()) {
        edit->setInputMask(config.input_mask);
    }
    
    if (config.custom_validator) {
        edit->setValidator(config.custom_validator);
    }
    
    if (!config.completions.isEmpty()) {
        addAutoComplete(line_edit, config.completions);
    }
    
    if (config.select_all_on_focus) {
        QObject::connect(edit, &QLineEdit::focusInEvent, [edit]() {
            edit->selectAll();
        });
    }
}

void ComponentEnhancer::enhanceCheckBox(QWidget* checkbox, const EnhancementConfig& config) {
    enhance(checkbox, config);
}

void ComponentEnhancer::enhanceComboBox(QWidget* combobox, const EnhancementConfig& config) {
    enhance(combobox, config);
}

void ComponentEnhancer::enhanceLabel(QWidget* label, const EnhancementConfig& config) {
    enhance(label, config);
}

void ComponentEnhancer::enhanceContainer(QWidget* container, const EnhancementConfig& default_config) {
    if (!container) return;
    
    // Enhance the container itself
    enhance(container, default_config);
    
    // Enhance all child widgets
    for (auto* child : container->findChildren<QWidget*>()) {
        enhance(child, default_config);
    }
}

void ComponentEnhancer::enhanceForm(QWidget* form, const std::map<QString, EnhancementConfig>& field_configs) {
    if (!form) return;
    
    for (const auto& [object_name, config] : field_configs) {
        auto* widget = form->findChild<QWidget*>(object_name);
        if (widget) {
            enhance(widget, config);
        }
    }
}

void ComponentEnhancer::removeEnhancements(QWidget* widget) {
    if (!widget) return;
    
    // Clean up enhancement objects
    auto it = enhancement_objects_.find(widget);
    if (it != enhancement_objects_.end()) {
        for (auto* obj : it->second) {
            obj->deleteLater();
        }
        enhancement_objects_.erase(it);
    }
    
    // Remove from tracking
    enhanced_widgets_.erase(widget);
    
    // Reset widget properties
    widget->setGraphicsEffect(nullptr);
    widget->setToolTip("");
    // Note: StyleSheet reset would need to be more sophisticated in a real implementation
}

void ComponentEnhancer::setupAccessibility(QWidget* widget, const EnhancementConfig& config) {
    if (!widget) return;
    
    // Set up accessibility using the existing AccessibilityManager
    auto accessibility = Core::accessibilityFor();
    
    if (!config.accessible_name.isEmpty()) {
        accessibility.name(config.accessible_name);
    }
    
    if (!config.accessible_description.isEmpty()) {
        accessibility.description(config.accessible_description);
    }
    
    if (!config.help_text.isEmpty()) {
        accessibility.helpText(config.help_text);
    }
    
    accessibility.role(config.accessibility_role);
    
    if (config.tab_index >= 0) {
        accessibility.tabIndex(config.tab_index);
    }
    
    accessibility.applyTo(widget);
    
    // Set tooltip
    if (!config.tooltip_text.isEmpty()) {
        widget->setToolTip(config.tooltip_text);
    }
}

void ComponentEnhancer::setupVisualEffects(QWidget* widget, const EnhancementConfig& config) {
    if (!widget) return;
    
    // Drop shadow
    if (config.drop_shadow) {
        addDropShadow(widget, config.shadow_color);
    }
    
    // Hover effect
    if (config.hover_effect) {
        addHoverEffect(widget);
    }
    
    // Focus effect
    if (config.focus_effect && config.focus_color.isValid()) {
        addFocusEffect(widget, config.focus_color);
    }
    
    // Border styling
    QString style_additions;
    
    if (config.border_radius > 0) {
        style_additions += QString("border-radius: %1px; ").arg(config.border_radius);
    }
    
    if (config.border_color.isValid()) {
        style_additions += QString("border: 1px solid %1; ").arg(config.border_color.name());
    }
    
    if (!style_additions.isEmpty()) {
        applyStyleSheet(widget, style_additions);
    }
}

void ComponentEnhancer::setupValidation(QWidget* widget, const EnhancementConfig& config) {
    if (!widget || !config.validator) return;
    
    addValidation(widget, config.validator, config.validation_error_message);
}

void ComponentEnhancer::setupKeyboardHandling(QWidget* widget, const EnhancementConfig& config) {
    if (!widget) return;
    
    if (!config.shortcut.isEmpty()) {
        addShortcut(widget, config.shortcut);
    }
}

void ComponentEnhancer::setupBehavior(QWidget* widget, const EnhancementConfig& config) {
    if (!widget) return;
    
    // Auto-completion
    if (!config.completions.isEmpty()) {
        addAutoComplete(widget, config.completions);
    }
    
    // Input mask
    if (auto* line_edit = qobject_cast<QLineEdit*>(widget)) {
        if (!config.input_mask.isEmpty()) {
            line_edit->setInputMask(config.input_mask);
        }
        
        if (config.custom_validator) {
            line_edit->setValidator(config.custom_validator);
        }
    }
}

void ComponentEnhancer::setupEventHandlers(QWidget* widget, const EnhancementConfig& config) {
    if (!widget) return;
    
    // Focus change handler
    if (config.on_focus_changed) {
        // This would need a custom event filter implementation
        // For now, just log that it's configured
        qDebug() << "Focus change handler configured for widget";
    }
    
    // Hover handlers
    if (config.on_hover_enter || config.on_hover_leave) {
        widget->setAttribute(Qt::WA_Hover, true);
        // This would need a custom event filter implementation
        qDebug() << "Hover handlers configured for widget";
    }
}

void ComponentEnhancer::applyStyleSheet(QWidget* widget, const QString& additional_styles) {
    if (!widget) return;
    
    QString current_style = widget->styleSheet();
    QString widget_class = widget->metaObject()->className();
    
    QString new_style = current_style + QString("%1 { %2 }").arg(widget_class, additional_styles);
    widget->setStyleSheet(new_style);
}

// EnhancementConfigBuilder implementation
EnhancementConfigBuilder& EnhancementConfigBuilder::accessibleName(const QString& name) {
    config_.accessible_name = name;
    return *this;
}

EnhancementConfigBuilder& EnhancementConfigBuilder::accessibleDescription(const QString& description) {
    config_.accessible_description = description;
    return *this;
}

EnhancementConfigBuilder& EnhancementConfigBuilder::tooltip(const QString& tooltip) {
    config_.tooltip_text = tooltip;
    return *this;
}

EnhancementConfigBuilder& EnhancementConfigBuilder::helpText(const QString& help) {
    config_.help_text = help;
    return *this;
}

EnhancementConfigBuilder& EnhancementConfigBuilder::role(Core::AccessibilityRole role) {
    config_.accessibility_role = role;
    return *this;
}

EnhancementConfigBuilder& EnhancementConfigBuilder::tabIndex(int index) {
    config_.tab_index = index;
    return *this;
}

EnhancementConfigBuilder& EnhancementConfigBuilder::dropShadow(bool enabled, const QColor& color) {
    config_.drop_shadow = enabled;
    config_.shadow_color = color;
    return *this;
}

EnhancementConfigBuilder& EnhancementConfigBuilder::hoverEffect(bool enabled) {
    config_.hover_effect = enabled;
    return *this;
}

EnhancementConfigBuilder& EnhancementConfigBuilder::focusEffect(bool enabled) {
    config_.focus_effect = enabled;
    return *this;
}

EnhancementConfigBuilder& EnhancementConfigBuilder::borderRadius(int radius) {
    config_.border_radius = radius;
    return *this;
}

EnhancementConfigBuilder& EnhancementConfigBuilder::borderColor(const QColor& color) {
    config_.border_color = color;
    return *this;
}

EnhancementConfigBuilder& EnhancementConfigBuilder::focusColor(const QColor& color) {
    config_.focus_color = color;
    return *this;
}

EnhancementConfigBuilder& EnhancementConfigBuilder::required(bool required) {
    config_.required = required;
    return *this;
}

EnhancementConfigBuilder& EnhancementConfigBuilder::validator(std::function<bool(QWidget*)> validator) {
    config_.validator = std::move(validator);
    return *this;
}

EnhancementConfigBuilder& EnhancementConfigBuilder::validationError(const QString& error) {
    config_.validation_error_message = error;
    return *this;
}

EnhancementConfigBuilder& EnhancementConfigBuilder::validateOnChange(bool enabled) {
    config_.validate_on_change = enabled;
    return *this;
}

EnhancementConfigBuilder& EnhancementConfigBuilder::validateOnFocusLost(bool enabled) {
    config_.validate_on_focus_lost = enabled;
    return *this;
}

EnhancementConfigBuilder& EnhancementConfigBuilder::shortcut(const QKeySequence& shortcut) {
    config_.shortcut = shortcut;
    return *this;
}

EnhancementConfigBuilder& EnhancementConfigBuilder::autoComplete(const QStringList& completions) {
    config_.completions = completions;
    return *this;
}

EnhancementConfigBuilder& EnhancementConfigBuilder::inputMask(const QString& mask) {
    config_.input_mask = mask;
    return *this;
}

EnhancementConfigBuilder& EnhancementConfigBuilder::customValidator(QValidator* validator) {
    config_.custom_validator = validator;
    return *this;
}

EnhancementConfigBuilder& EnhancementConfigBuilder::selectAllOnFocus(bool enabled) {
    config_.select_all_on_focus = enabled;
    return *this;
}

EnhancementConfigBuilder& EnhancementConfigBuilder::clearOnEscape(bool enabled) {
    config_.clear_on_escape = enabled;
    return *this;
}

EnhancementConfigBuilder& EnhancementConfigBuilder::debounceDelay(int milliseconds) {
    config_.debounce_delay = milliseconds;
    return *this;
}

EnhancementConfigBuilder& EnhancementConfigBuilder::onValidationChanged(std::function<void(bool)> handler) {
    config_.on_validation_changed = std::move(handler);
    return *this;
}

EnhancementConfigBuilder& EnhancementConfigBuilder::onFocusChanged(std::function<void(bool)> handler) {
    config_.on_focus_changed = std::move(handler);
    return *this;
}

EnhancementConfigBuilder& EnhancementConfigBuilder::onHoverEnter(std::function<void()> handler) {
    config_.on_hover_enter = std::move(handler);
    return *this;
}

EnhancementConfigBuilder& EnhancementConfigBuilder::onHoverLeave(std::function<void()> handler) {
    config_.on_hover_leave = std::move(handler);
    return *this;
}

EnhancementConfig EnhancementConfigBuilder::build() const {
    return config_;
}

void EnhancementConfigBuilder::applyTo(QWidget* widget) const {
    ComponentEnhancer::enhance(widget, config_);
}

} // namespace DeclarativeUI::Components::Enhanced

#include "ComponentEnhancer.moc"
