// Components/Button.cpp
#include "Button.hpp"
#include <QApplication>
#include <QDebug>
#include <QTime>
#include "../Exceptions/UIExceptions.hpp"

namespace DeclarativeUI::Components {

// **Implementation with exception safety and enhanced features**
Button::Button(QObject* parent)
    : UIElement(parent),
      button_widget_(nullptr),
      icon_position_(Qt::ToolButtonTextBesideIcon),
      icon_size_(16, 16),
      auto_repeat_enabled_(false),
      auto_repeat_initial_delay_(300),
      auto_repeat_delay_(100),
      checkable_(false),
      checked_(false),
      flat_(false),
      menu_(nullptr),
      drop_shadow_enabled_(false),
      shadow_color_(QColor(0, 0, 0, 80)),
      hover_effect_enabled_(true),
      press_animation_enabled_(true),
      border_radius_(4),
      gradient_start_(QColor()),
      gradient_end_(QColor()),
      required_(false),
      loading_state_(false),
      disabled_state_(false),
      tab_index_(-1),
      progress_percentage_(0),
      current_state_(0) {
    // Initialize with default accessibility role
    aria_role_ = "button";
}

// **Basic fluent interface (backward compatible)**
Button& Button::text(const QString& text) {
    return static_cast<Button&>(setProperty("text", text));
}

Button& Button::icon(const QIcon& icon) {
    return static_cast<Button&>(setProperty("icon", icon));
}

Button& Button::onClick(std::function<void()> handler) {
    return static_cast<Button&>(onEvent("clicked", std::move(handler)));
}

Button& Button::enabled(bool enabled) {
    return static_cast<Button&>(setProperty("enabled", enabled));
}

Button& Button::style(const QString& stylesheet) {
    return static_cast<Button&>(setProperty("styleSheet", stylesheet));
}

// **Enhanced fluent interface**
Button& Button::tooltip(const QString& tooltip_text) {
    tooltip_text_ = tooltip_text;
    return *this;
}

Button& Button::accessibleName(const QString& name) {
    accessible_name_ = name;
    return *this;
}

Button& Button::accessibleDescription(const QString& description) {
    accessible_description_ = description;
    return *this;
}

Button& Button::shortcut(const QKeySequence& shortcut) {
    shortcut_ = shortcut;
    return *this;
}

Button& Button::iconPosition(Qt::ToolButtonStyle position) {
    icon_position_ = position;
    return *this;
}

Button& Button::iconSize(const QSize& size) {
    icon_size_ = size;
    return *this;
}

Button& Button::autoRepeat(bool enabled, int initial_delay, int repeat_delay) {
    auto_repeat_enabled_ = enabled;
    auto_repeat_initial_delay_ = initial_delay;
    auto_repeat_delay_ = repeat_delay;
    return *this;
}

Button& Button::checkable(bool checkable) {
    checkable_ = checkable;
    return *this;
}

Button& Button::checked(bool checked) {
    checked_ = checked;
    return *this;
}

Button& Button::flat(bool flat) {
    flat_ = flat;
    return *this;
}

Button& Button::menu(QMenu* menu) {
    menu_ = menu;
    return *this;
}

// **Visual enhancements**
Button& Button::dropShadow(bool enabled, const QColor& color) {
    drop_shadow_enabled_ = enabled;
    shadow_color_ = color;
    return *this;
}

Button& Button::hoverEffect(bool enabled) {
    hover_effect_enabled_ = enabled;
    return *this;
}

Button& Button::pressAnimation(bool enabled) {
    press_animation_enabled_ = enabled;
    return *this;
}

Button& Button::borderRadius(int radius) {
    border_radius_ = radius;
    return *this;
}

Button& Button::gradient(const QColor& start, const QColor& end) {
    gradient_start_ = start;
    gradient_end_ = end;
    return *this;
}

// **Validation and error handling**
Button& Button::validator(std::function<bool()> validation_func) {
    validation_func_ = std::move(validation_func);
    return *this;
}

Button& Button::onValidationFailed(
    std::function<void(const QString&)> error_handler) {
    error_handler_ = std::move(error_handler);
    return *this;
}

Button& Button::required(bool required) {
    required_ = required;
    return *this;
}

// **State management**
Button& Button::loading(bool loading_state) {
    loading_state_ = loading_state;
    return *this;
}

Button& Button::loadingText(const QString& text) {
    loading_text_ = text;
    return *this;
}

Button& Button::disabled(bool disabled, const QString& reason) {
    disabled_state_ = disabled;
    disabled_reason_ = reason;
    return *this;
}

// **Event handlers**
Button& Button::onHover(std::function<void(bool)> hover_handler) {
    hover_handler_ = std::move(hover_handler);
    return *this;
}

Button& Button::onFocus(std::function<void(bool)> focus_handler) {
    focus_handler_ = std::move(focus_handler);
    return *this;
}

Button& Button::onDoubleClick(std::function<void()> double_click_handler) {
    double_click_handler_ = std::move(double_click_handler);
    return *this;
}

Button& Button::onRightClick(std::function<void()> right_click_handler) {
    right_click_handler_ = std::move(right_click_handler);
    return *this;
}

// **Accessibility helpers**
Button& Button::role(const QString& aria_role) {
    aria_role_ = aria_role;
    return *this;
}

Button& Button::tabIndex(int index) {
    tab_index_ = index;
    return *this;
}

Button& Button::describedBy(const QString& element_id) {
    described_by_ = element_id;
    return *this;
}

// **Advanced features**
Button& Button::badge(const QString& badge_text, const QColor& badge_color) {
    badge_text_ = badge_text;
    badge_color_ = badge_color;
    return *this;
}

Button& Button::progress(int percentage) {
    progress_percentage_ = qBound(0, percentage, 100);
    return *this;
}

Button& Button::multiState(const QStringList& states, int current_state) {
    multi_states_ = states;
    current_state_ = qBound(0, current_state, states.size() - 1);
    return *this;
}

void Button::initialize() {
    if (!button_widget_) {
        try {
            button_widget_ = new QPushButton();
            setWidget(button_widget_);

            // Connect click signal if handler is set
            auto click_handler = event_handlers_.find("clicked");
            if (click_handler != event_handlers_.end()) {
                connect(button_widget_, &QPushButton::clicked, this,
                        [handler = click_handler->second]() { handler(); });
            }

            // Initialize enhanced features
            setupAccessibility();
            setupVisualEffects();
            setupEventHandlers();
            setupValidation();
            updateButtonState();

            qDebug() << "Enhanced button initialized successfully";

        } catch (const std::exception& e) {
            throw Exceptions::ComponentCreationException("Button: " +
                                                         std::string(e.what()));
        }
    }
}

bool Button::isValid() const {
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

void Button::setupAccessibility() {
    auto* widget = getWidget();
    if (!widget)
        return;

    // Set up accessibility using the existing AccessibilityManager
    auto accessibility =
        Core::Accessibility::accessibilityFor()
            .name(accessible_name_.isEmpty() ? tooltip_text_ : accessible_name_)
            .description(accessible_description_)
            .helpText(tooltip_text_)
            .role(Core::Accessibility::AccessibilityRole::Button)
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

void Button::setupVisualEffects() {
    auto* widget = getWidget();
    if (!widget)
        return;

    auto* button = qobject_cast<QPushButton*>(widget);
    if (!button)
        return;

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
        press_animation_ =
            std::make_unique<QPropertyAnimation>(button, "geometry");
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
        style_sheet +=
            QString("QPushButton { border-radius: %1px; }").arg(border_radius_);
    }

    if (gradient_start_.isValid() && gradient_end_.isValid()) {
        style_sheet +=
            QString(
                "QPushButton { "
                "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, "
                "stop: 0 %1, stop: 1 %2); "
                "}")
                .arg(gradient_start_.name(), gradient_end_.name());
    }

    if (!style_sheet.isEmpty()) {
        button->setStyleSheet(style_sheet);
    }
}

void Button::setupEventHandlers() {
    auto* widget = getWidget();
    if (!widget)
        return;

    auto* button = qobject_cast<QPushButton*>(widget);
    if (!button)
        return;

    // Install event filter for hover and other events
    button->installEventFilter(this);

    // Connect double click if handler is set
    if (double_click_handler_) {
        connect(button, &QPushButton::clicked, [this]() {
            static QTime last_click;
            QTime current_time = QTime::currentTime();

            if (last_click.isValid() &&
                last_click.msecsTo(current_time) < 500) {
                double_click_handler_();
            }
            last_click = current_time;
        });
    }
}

void Button::setupValidation() {
    if (validation_func_) {
        validation_timer_ = std::make_unique<QTimer>();
        validation_timer_->setSingleShot(true);
        validation_timer_->setInterval(
            500);  // Validate 500ms after last change

        connect(validation_timer_.get(), &QTimer::timeout, this,
                &Button::onValidationCheck);
    }
}

void Button::updateButtonState() {
    auto* widget = getWidget();
    if (!widget)
        return;

    auto* button = qobject_cast<QPushButton*>(widget);
    if (!button)
        return;

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

void Button::updateLoadingState() {
    auto* button = qobject_cast<QPushButton*>(getWidget());
    if (!button)
        return;

    if (loading_state_) {
        if (original_text_.isEmpty()) {
            original_text_ = button->text();
        }

        QString display_text =
            loading_text_.isEmpty() ? "Loading..." : loading_text_;
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

void Button::updateMultiState() {
    if (multi_states_.isEmpty())
        return;

    auto* button = qobject_cast<QPushButton*>(getWidget());
    if (!button)
        return;

    if (current_state_ >= 0 && current_state_ < multi_states_.size()) {
        button->setText(multi_states_[current_state_]);
        emit stateChanged(current_state_);
    }
}

void Button::showValidationError(const QString& error) {
    validation_error_ = error;

    if (error_handler_) {
        error_handler_(error);
    } else {
        // Default error display
        auto* widget = getWidget();
        if (widget) {
            QToolTip::showText(widget->mapToGlobal(QPoint(0, widget->height())),
                               error, widget);
        }
    }

    emit validationChanged(false);
}

void Button::onValidationCheck() {
    bool valid = isValid();

    if (!valid && required_) {
        showValidationError("This field is required");
    } else {
        validation_error_.clear();
        emit validationChanged(true);
    }
}

void Button::onButtonHovered() {
    if (hover_handler_) {
        hover_handler_(true);
    }
}

void Button::onButtonPressed() {
    // Handle press animation or other press effects
}

void Button::onButtonReleased() {
    // Handle release effects
}

}  // namespace DeclarativeUI::Components
