/**
 * @file ButtonEnhanced.cpp
 * @brief Implementation of enhanced Button component
 */

#include "ButtonEnhanced.hpp"
#include "../../Exceptions/UIExceptions.hpp"
#include <QApplication>
#include <QTimer>
#include <QMenu>
#include <QDebug>

namespace DeclarativeUI::Components::Enhanced {

ButtonEnhanced::ButtonEnhanced(QObject* parent)
    : Components::Button(parent)
    , icon_position_(Qt::ToolButtonTextBesideIcon)
    , icon_size_(16, 16)
    , auto_repeat_enabled_(false)
    , auto_repeat_initial_delay_(300)
    , auto_repeat_delay_(100)
    , checkable_(false)
    , checked_(false)
    , flat_(false)
    , menu_(nullptr)
    , drop_shadow_enabled_(false)
    , shadow_color_(QColor(0, 0, 0, 80))
    , hover_effect_enabled_(true)
    , press_animation_enabled_(true)
    , border_radius_(4)
    , gradient_start_(QColor())
    , gradient_end_(QColor())
    , required_(false)
    , loading_state_(false)
    , disabled_state_(false)
    , tab_index_(-1)
    , progress_percentage_(0)
    , current_state_(0)
{
    // Initialize with default accessibility role
    aria_role_ = "button";
}

ButtonEnhanced& ButtonEnhanced::tooltip(const QString& tooltip_text) {
    tooltip_text_ = tooltip_text;
    return *this;
}

ButtonEnhanced& ButtonEnhanced::accessibleName(const QString& name) {
    accessible_name_ = name;
    return *this;
}

ButtonEnhanced& ButtonEnhanced::accessibleDescription(const QString& description) {
    accessible_description_ = description;
    return *this;
}

ButtonEnhanced& ButtonEnhanced::shortcut(const QKeySequence& shortcut) {
    shortcut_ = shortcut;
    return *this;
}

ButtonEnhanced& ButtonEnhanced::iconPosition(Qt::ToolButtonStyle position) {
    icon_position_ = position;
    return *this;
}

ButtonEnhanced& ButtonEnhanced::iconSize(const QSize& size) {
    icon_size_ = size;
    return *this;
}

ButtonEnhanced& ButtonEnhanced::autoRepeat(bool enabled, int initial_delay, int repeat_delay) {
    auto_repeat_enabled_ = enabled;
    auto_repeat_initial_delay_ = initial_delay;
    auto_repeat_delay_ = repeat_delay;
    return *this;
}

ButtonEnhanced& ButtonEnhanced::checkable(bool checkable) {
    checkable_ = checkable;
    return *this;
}

ButtonEnhanced& ButtonEnhanced::checked(bool checked) {
    checked_ = checked;
    return *this;
}

ButtonEnhanced& ButtonEnhanced::flat(bool flat) {
    flat_ = flat;
    return *this;
}

ButtonEnhanced& ButtonEnhanced::menu(QMenu* menu) {
    menu_ = menu;
    return *this;
}

ButtonEnhanced& ButtonEnhanced::dropShadow(bool enabled, const QColor& color) {
    drop_shadow_enabled_ = enabled;
    shadow_color_ = color;
    return *this;
}

ButtonEnhanced& ButtonEnhanced::hoverEffect(bool enabled) {
    hover_effect_enabled_ = enabled;
    return *this;
}

ButtonEnhanced& ButtonEnhanced::pressAnimation(bool enabled) {
    press_animation_enabled_ = enabled;
    return *this;
}

ButtonEnhanced& ButtonEnhanced::borderRadius(int radius) {
    border_radius_ = radius;
    return *this;
}

ButtonEnhanced& ButtonEnhanced::gradient(const QColor& start, const QColor& end) {
    gradient_start_ = start;
    gradient_end_ = end;
    return *this;
}

ButtonEnhanced& ButtonEnhanced::validator(std::function<bool()> validation_func) {
    validation_func_ = std::move(validation_func);
    return *this;
}

ButtonEnhanced& ButtonEnhanced::onValidationFailed(std::function<void(const QString&)> error_handler) {
    error_handler_ = std::move(error_handler);
    return *this;
}

ButtonEnhanced& ButtonEnhanced::required(bool required) {
    required_ = required;
    return *this;
}

ButtonEnhanced& ButtonEnhanced::loading(bool loading_state) {
    loading_state_ = loading_state;
    return *this;
}

ButtonEnhanced& ButtonEnhanced::loadingText(const QString& text) {
    loading_text_ = text;
    return *this;
}

ButtonEnhanced& ButtonEnhanced::disabled(bool disabled, const QString& reason) {
    disabled_state_ = disabled;
    disabled_reason_ = reason;
    return *this;
}

ButtonEnhanced& ButtonEnhanced::onHover(std::function<void(bool)> hover_handler) {
    hover_handler_ = std::move(hover_handler);
    return *this;
}

ButtonEnhanced& ButtonEnhanced::onFocus(std::function<void(bool)> focus_handler) {
    focus_handler_ = std::move(focus_handler);
    return *this;
}

ButtonEnhanced& ButtonEnhanced::onDoubleClick(std::function<void()> double_click_handler) {
    double_click_handler_ = std::move(double_click_handler);
    return *this;
}

ButtonEnhanced& ButtonEnhanced::onRightClick(std::function<void()> right_click_handler) {
    right_click_handler_ = std::move(right_click_handler);
    return *this;
}

ButtonEnhanced& ButtonEnhanced::role(const QString& aria_role) {
    aria_role_ = aria_role;
    return *this;
}

ButtonEnhanced& ButtonEnhanced::tabIndex(int index) {
    tab_index_ = index;
    return *this;
}

ButtonEnhanced& ButtonEnhanced::describedBy(const QString& element_id) {
    described_by_ = element_id;
    return *this;
}

ButtonEnhanced& ButtonEnhanced::badge(const QString& badge_text, const QColor& badge_color) {
    badge_text_ = badge_text;
    badge_color_ = badge_color;
    return *this;
}

ButtonEnhanced& ButtonEnhanced::progress(int percentage) {
    progress_percentage_ = qBound(0, percentage, 100);
    return *this;
}

ButtonEnhanced& ButtonEnhanced::multiState(const QStringList& states, int current_state) {
    multi_states_ = states;
    current_state_ = qBound(0, current_state, states.size() - 1);
    return *this;
}

void ButtonEnhanced::initialize() {
    // Call parent initialization first
    Components::Button::initialize();
    
    try {
        setupAccessibility();
        setupVisualEffects();
        setupEventHandlers();
        setupValidation();
        updateButtonState();
        
        qDebug() << "Enhanced button initialized successfully";
        
    } catch (const std::exception& e) {
        throw Exceptions::ComponentCreationException(
            QString("Failed to initialize enhanced button: %1").arg(e.what())
        );
    }
}

bool ButtonEnhanced::isValid() const {
    if (validation_func_) {
        return validation_func_();
    }
    
    // Default validation: required buttons must have text or icon
    if (required_) {
        auto* widget = getWidget();
        if (auto* button = qobject_cast<QPushButton*>(widget)) {
            return !button->text().isEmpty() || !button->icon().isNull();
        }
    }
    
    return true;
}

void ButtonEnhanced::setupAccessibility() {
    auto* widget = getWidget();
    if (!widget) return;
    
    // Set up accessibility using the existing AccessibilityManager
    auto accessibility = Core::accessibilityFor()
                        .name(accessible_name_.isEmpty() ? tooltip_text_ : accessible_name_)
                        .description(accessible_description_)
                        .helpText(tooltip_text_)
                        .role(Core::AccessibilityRole::Button)
                        .enabled(!disabled_state_)
                        .required(required_);
    
    if (tab_index_ >= 0) {
        accessibility.tabIndex(tab_index_);
    }
    
    accessibility.applyTo(widget);
    
    // Set tooltip
    if (!tooltip_text_.isEmpty()) {
        widget->setToolTip(tooltip_text_);
    }
    
    // Set up keyboard shortcut
    if (!shortcut_.isEmpty()) {
        shortcut_obj_ = std::make_unique<QShortcut>(shortcut_, widget);
        connect(shortcut_obj_.get(), &QShortcut::activated, [this]() {
            if (auto* button = qobject_cast<QPushButton*>(getWidget())) {
                if (button->isEnabled()) {
                    button->click();
                }
            }
        });
    }
}

void ButtonEnhanced::setupVisualEffects() {
    auto* widget = getWidget();
    if (!widget) return;
    
    auto* button = qobject_cast<QPushButton*>(widget);
    if (!button) return;
    
    // Set up drop shadow
    if (drop_shadow_enabled_) {
        shadow_effect_ = std::make_unique<QGraphicsDropShadowEffect>();
        shadow_effect_->setBlurRadius(10);
        shadow_effect_->setColor(shadow_color_);
        shadow_effect_->setOffset(0, 2);
        button->setGraphicsEffect(shadow_effect_.get());
    }
    
    // Set up press animation
    if (press_animation_enabled_) {
        press_animation_ = std::make_unique<QPropertyAnimation>(button, "geometry");
        press_animation_->setDuration(100);
        press_animation_->setEasingCurve(QEasingCurve::OutCubic);
    }
    
    // Apply visual properties
    button->setCheckable(checkable_);
    button->setChecked(checked_);
    button->setFlat(flat_);
    button->setAutoRepeat(auto_repeat_enabled_);
    button->setAutoRepeatDelay(auto_repeat_initial_delay_);
    button->setAutoRepeatInterval(auto_repeat_delay_);
    
    if (menu_) {
        button->setMenu(menu_);
    }
    
    // Apply custom styling
    QString style_sheet = button->styleSheet();
    
    if (border_radius_ > 0) {
        style_sheet += QString("QPushButton { border-radius: %1px; }").arg(border_radius_);
    }
    
    if (gradient_start_.isValid() && gradient_end_.isValid()) {
        style_sheet += QString(
            "QPushButton { "
            "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, "
            "stop: 0 %1, stop: 1 %2); "
            "}"
        ).arg(gradient_start_.name(), gradient_end_.name());
    }
    
    if (!style_sheet.isEmpty()) {
        button->setStyleSheet(style_sheet);
    }
}

void ButtonEnhanced::setupEventHandlers() {
    auto* widget = getWidget();
    if (!widget) return;
    
    auto* button = qobject_cast<QPushButton*>(widget);
    if (!button) return;
    
    // Install event filter for hover and other events
    button->installEventFilter(this);
    
    // Connect double click if handler is set
    if (double_click_handler_) {
        connect(button, &QPushButton::clicked, [this]() {
            static QTime last_click;
            QTime current_time = QTime::currentTime();
            
            if (last_click.isValid() && last_click.msecsTo(current_time) < 500) {
                double_click_handler_();
            }
            last_click = current_time;
        });
    }
}

void ButtonEnhanced::setupValidation() {
    if (validation_func_) {
        validation_timer_ = std::make_unique<QTimer>();
        validation_timer_->setSingleShot(true);
        validation_timer_->setInterval(500); // Validate 500ms after last change
        
        connect(validation_timer_.get(), &QTimer::timeout, this, &ButtonEnhanced::onValidationCheck);
    }
}

void ButtonEnhanced::updateButtonState() {
    auto* widget = getWidget();
    if (!widget) return;
    
    auto* button = qobject_cast<QPushButton*>(widget);
    if (!button) return;
    
    // Update loading state
    updateLoadingState();
    
    // Update multi-state
    updateMultiState();
    
    // Update disabled state
    button->setEnabled(!disabled_state_ && !loading_state_);
    
    if (disabled_state_ && !disabled_reason_.isEmpty()) {
        button->setToolTip(disabled_reason_);
    }
}

void ButtonEnhanced::updateLoadingState() {
    auto* button = qobject_cast<QPushButton*>(getWidget());
    if (!button) return;
    
    if (loading_state_) {
        if (original_text_.isEmpty()) {
            original_text_ = button->text();
        }
        
        QString display_text = loading_text_.isEmpty() ? "Loading..." : loading_text_;
        button->setText(display_text);
        button->setEnabled(false);
    } else {
        if (!original_text_.isEmpty()) {
            button->setText(original_text_);
            original_text_.clear();
        }
        button->setEnabled(!disabled_state_);
    }
    
    emit loadingStateChanged(loading_state_);
}

void ButtonEnhanced::updateMultiState() {
    if (multi_states_.isEmpty()) return;
    
    auto* button = qobject_cast<QPushButton*>(getWidget());
    if (!button) return;
    
    if (current_state_ >= 0 && current_state_ < multi_states_.size()) {
        button->setText(multi_states_[current_state_]);
        emit stateChanged(current_state_);
    }
}

void ButtonEnhanced::showValidationError(const QString& error) {
    validation_error_ = error;
    
    if (error_handler_) {
        error_handler_(error);
    } else {
        // Default error display
        auto* widget = getWidget();
        if (widget) {
            QToolTip::showText(widget->mapToGlobal(QPoint(0, widget->height())), error, widget);
        }
    }
    
    emit validationChanged(false);
}

void ButtonEnhanced::onValidationCheck() {
    bool valid = isValid();
    
    if (!valid && required_) {
        showValidationError("This field is required");
    } else {
        validation_error_.clear();
        emit validationChanged(true);
    }
}

} // namespace DeclarativeUI::Components::Enhanced

#include "ButtonEnhanced.moc"
