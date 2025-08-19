// Components/CheckBox.cpp
#include "CheckBox.hpp"
#include "../Core/Theme.hpp"
#include <QShortcut>
#include <QToolTip>
#include <QDebug>

namespace DeclarativeUI::Components {

// **Static group management**
QMap<QString, QList<CheckBox*>> CheckBox::named_groups_;
QMap<QString, std::function<CheckBoxValidationResult(const QList<CheckBox*>&)>> CheckBox::group_validators_;

// **Enhanced implementation**
CheckBox::CheckBox(QObject *parent)
    : UIElement(parent)
    , checkbox_widget_(nullptr)
    , button_group_(nullptr)
    , required_(false)
    , validate_on_change_(true)
    , is_valid_(true)
    , drop_shadow_enabled_(false)
    , shadow_color_(QColor(0, 0, 0, 80))
    , hover_effect_enabled_(true)
    , check_animation_enabled_(true)
    , border_radius_(4)
    , disabled_state_(false)
    , read_only_(false)
    , tab_index_(-1)
    , exclusive_group_(false)
{
    // Initialize with default accessibility role
    aria_role_ = "checkbox";
}

// **Basic fluent interface (backward compatible)**
CheckBox &CheckBox::text(const QString &text) {
    return static_cast<CheckBox &>(setProperty("text", text));
}

CheckBox &CheckBox::checked(bool checked) {
    return static_cast<CheckBox &>(setProperty("checked", checked));
}

CheckBox &CheckBox::tristate(bool tristate) {
    return static_cast<CheckBox &>(setProperty("tristate", tristate));
}

CheckBox &CheckBox::onStateChanged(std::function<void(int)> handler) {
    state_changed_handler_ = std::move(handler);
    return *this;
}

CheckBox &CheckBox::onToggled(std::function<void(bool)> handler) {
    toggled_handler_ = std::move(handler);
    return *this;
}

CheckBox &CheckBox::style(const QString &stylesheet) {
    return static_cast<CheckBox &>(setProperty("styleSheet", stylesheet));
}

// **Enhanced fluent interface**
CheckBox &CheckBox::tooltip(const QString& tooltip_text) {
    tooltip_text_ = tooltip_text;
    return *this;
}

CheckBox &CheckBox::accessibleName(const QString& name) {
    accessible_name_ = name;
    return *this;
}

CheckBox &CheckBox::accessibleDescription(const QString& description) {
    accessible_description_ = description;
    return *this;
}

CheckBox &CheckBox::shortcut(const QKeySequence& shortcut) {
    shortcut_ = shortcut;
    return *this;
}

CheckBox &CheckBox::group(QButtonGroup* group) {
    button_group_ = group;
    return *this;
}

CheckBox &CheckBox::groupName(const QString& group_name) {
    group_name_ = group_name;
    return *this;
}

// **Validation**
CheckBox &CheckBox::required(bool required) {
    required_ = required;
    return *this;
}

CheckBox &CheckBox::validator(std::function<CheckBoxValidationResult(Qt::CheckState)> validation_func) {
    validation_func_ = std::move(validation_func);
    return *this;
}

CheckBox &CheckBox::onValidationFailed(std::function<void(const QString&)> error_handler) {
    error_handler_ = std::move(error_handler);
    return *this;
}

CheckBox &CheckBox::validateOnChange(bool validate_on_change) {
    validate_on_change_ = validate_on_change;
    return *this;
}

// **Visual enhancements**
CheckBox &CheckBox::dropShadow(bool enabled, const QColor& color) {
    drop_shadow_enabled_ = enabled;
    shadow_color_ = color;
    return *this;
}

CheckBox &CheckBox::hoverEffect(bool enabled) {
    hover_effect_enabled_ = enabled;
    return *this;
}

CheckBox &CheckBox::checkAnimation(bool enabled) {
    check_animation_enabled_ = enabled;
    return *this;
}

CheckBox &CheckBox::borderRadius(int radius) {
    border_radius_ = radius;
    return *this;
}

CheckBox &CheckBox::customColors(const QColor& checked_color, const QColor& unchecked_color) {
    checked_color_ = checked_color;
    unchecked_color_ = unchecked_color;
    return *this;
}

CheckBox &CheckBox::size(const QSize& size) {
    custom_size_ = size;
    return *this;
}

// **State management**
CheckBox &CheckBox::disabled(bool disabled, const QString& reason) {
    disabled_state_ = disabled;
    disabled_reason_ = reason;
    return *this;
}

CheckBox &CheckBox::readOnly(bool readonly) {
    read_only_ = readonly;
    return *this;
}

// **Event handlers**
CheckBox &CheckBox::onHover(std::function<void(bool)> hover_handler) {
    hover_handler_ = std::move(hover_handler);
    return *this;
}

CheckBox &CheckBox::onFocus(std::function<void(bool)> focus_handler) {
    focus_handler_ = std::move(focus_handler);
    return *this;
}

CheckBox &CheckBox::onDoubleClick(std::function<void()> double_click_handler) {
    double_click_handler_ = std::move(double_click_handler);
    return *this;
}

CheckBox &CheckBox::onRightClick(std::function<void()> right_click_handler) {
    right_click_handler_ = std::move(right_click_handler);
    return *this;
}

CheckBox &CheckBox::onValidationChanged(std::function<void(bool, const QString&)> validation_handler) {
    validation_handler_ = std::move(validation_handler);
    return *this;
}

// **Accessibility**
CheckBox &CheckBox::role(const QString& aria_role) {
    aria_role_ = aria_role;
    return *this;
}

CheckBox &CheckBox::tabIndex(int index) {
    tab_index_ = index;
    return *this;
}

CheckBox &CheckBox::describedBy(const QString& element_id) {
    described_by_ = element_id;
    return *this;
}

CheckBox &CheckBox::labelledBy(const QString& element_id) {
    labelled_by_ = element_id;
    return *this;
}

// **Group management**
CheckBox &CheckBox::exclusiveGroup(bool exclusive) {
    exclusive_group_ = exclusive;
    return *this;
}

CheckBox &CheckBox::groupValidation(std::function<CheckBoxValidationResult(const QList<CheckBox*>&)> group_validator) {
    group_validator_ = std::move(group_validator);
    return *this;
}

void CheckBox::initialize() {
    if (!checkbox_widget_) {
        try {
            checkbox_widget_ = new QCheckBox();
            setWidget(checkbox_widget_);

            // Connect basic signals
            if (state_changed_handler_) {
                connect(checkbox_widget_, &QCheckBox::checkStateChanged, this,
                        [this](Qt::CheckState state) {
                            state_changed_handler_(static_cast<int>(state));
                        });
            }

            if (toggled_handler_) {
                connect(checkbox_widget_, &QCheckBox::toggled, this,
                        [this](bool checked) { toggled_handler_(checked); });
            }

            // Initialize enhanced features
            setupAccessibility();
            setupVisualEffects();
            setupEventHandlers();
            setupValidation();
            setupGroupManagement();
            updateCheckBoxState();

            qDebug() << "Enhanced checkbox initialized successfully";

        } catch (const std::exception& e) {
            qWarning() << "Failed to initialize enhanced checkbox:" << e.what();
        }
    }
}

bool CheckBox::isChecked() const {
    return checkbox_widget_ ? checkbox_widget_->isChecked() : false;
}

void CheckBox::setChecked(bool checked) {
    if (checkbox_widget_) {
        checkbox_widget_->setChecked(checked);
    }
}

Qt::CheckState CheckBox::checkState() const {
    return checkbox_widget_ ? checkbox_widget_->checkState() : Qt::Unchecked;
}

void CheckBox::setCheckState(Qt::CheckState state) {
    if (checkbox_widget_) {
        checkbox_widget_->setCheckState(state);
    }
}

void CheckBox::setupAccessibility() {
    auto* widget = getWidget();
    if (!widget) return;

    // Set up accessibility using the existing AccessibilityManager
    auto accessibility = Core::accessibilityFor()
                        .name(accessible_name_.isEmpty() ? tooltip_text_ : accessible_name_)
                        .description(accessible_description_)
                        .helpText(tooltip_text_)
                        .role(Core::AccessibilityRole::CheckBox)
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
        auto* shortcut_obj = new QShortcut(shortcut_, widget);
        connect(shortcut_obj, &QShortcut::activated, [this]() {
            if (auto* checkbox = qobject_cast<QCheckBox*>(getWidget())) {
                if (checkbox->isEnabled() && !read_only_) {
                    checkbox->toggle();
                }
            }
        });
    }
}

void CheckBox::setupVisualEffects() {
    auto* widget = getWidget();
    if (!widget) return;

    auto* checkbox = qobject_cast<QCheckBox*>(widget);
    if (!checkbox) return;

    // Set up drop shadow
    if (drop_shadow_enabled_) {
        shadow_effect_ = std::make_unique<QGraphicsDropShadowEffect>();
        shadow_effect_->setBlurRadius(8);
        shadow_effect_->setColor(shadow_color_);
        shadow_effect_->setOffset(0, 2);
        checkbox->setGraphicsEffect(shadow_effect_.get());
    }

    // Set up check animation
    if (check_animation_enabled_) {
        check_animation_ = std::make_unique<QPropertyAnimation>(checkbox, "geometry");
        check_animation_->setDuration(150);
        check_animation_->setEasingCurve(QEasingCurve::OutCubic);
    }

    // Apply custom styling
    QString style_sheet = checkbox->styleSheet();

    if (border_radius_ > 0) {
        style_sheet += QString("QCheckBox::indicator { border-radius: %1px; }").arg(border_radius_);
    }

    if (checked_color_.isValid()) {
        style_sheet += QString(
            "QCheckBox::indicator:checked { "
            "background-color: %1; "
            "border: 2px solid %1; "
            "}"
        ).arg(checked_color_.name());
    }

    if (unchecked_color_.isValid()) {
        style_sheet += QString(
            "QCheckBox::indicator:unchecked { "
            "background-color: %1; "
            "border: 2px solid %1; "
            "}"
        ).arg(unchecked_color_.name());
    }

    if (custom_size_.isValid()) {
        style_sheet += QString(
            "QCheckBox::indicator { "
            "width: %1px; "
            "height: %2px; "
            "}"
        ).arg(custom_size_.width()).arg(custom_size_.height());
    }

    if (!style_sheet.isEmpty()) {
        checkbox->setStyleSheet(style_sheet);
    }
}

void CheckBox::setupEventHandlers() {
    auto* widget = getWidget();
    if (!widget) return;

    auto* checkbox = qobject_cast<QCheckBox*>(widget);
    if (!checkbox) return;

    // Connect enhanced state change handler
    connect(checkbox, &QCheckBox::checkStateChanged, this, &CheckBox::onStateChangedInternal);

    // Install event filter for hover and other events
    checkbox->installEventFilter(this);
}

void CheckBox::setupValidation() {
    if (validation_func_ || required_ || group_validator_) {
        validation_timer_ = std::make_unique<QTimer>();
        validation_timer_->setSingleShot(true);
        validation_timer_->setInterval(300); // Validate 300ms after last change

        connect(validation_timer_.get(), &QTimer::timeout, this, &CheckBox::onValidationTimer);
    }
}

void CheckBox::setupGroupManagement() {
    // Add to button group if specified
    if (button_group_) {
        button_group_->addButton(checkbox_widget_);
    }

    // Add to named group if specified
    if (!group_name_.isEmpty()) {
        named_groups_[group_name_].append(this);

        // Set up group validator if provided
        if (group_validator_) {
            group_validators_[group_name_] = group_validator_;
        }
    }
}

void CheckBox::updateCheckBoxState() {
    auto* widget = getWidget();
    if (!widget) return;

    auto* checkbox = qobject_cast<QCheckBox*>(widget);
    if (!checkbox) return;

    // Update disabled state
    checkbox->setEnabled(!disabled_state_);

    if (disabled_state_ && !disabled_reason_.isEmpty()) {
        checkbox->setToolTip(disabled_reason_);
    }

    // Update read-only state
    if (read_only_) {
        checkbox->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    }
}

CheckBoxValidationResult CheckBox::validateState(Qt::CheckState state) const {
    // Check if required
    if (required_ && state == Qt::Unchecked) {
        return CheckBoxValidationResult(false, "This option is required");
    }

    // Custom validation
    if (validation_func_) {
        return validation_func_(state);
    }

    return CheckBoxValidationResult(true);
}

void CheckBox::showValidationError(const QString& error) {
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

    emit validationChanged(false, error);
}

void CheckBox::animateCheck(bool checked) {
    if (check_animation_ && checkbox_widget_) {
        // Simple scale animation for check/uncheck
        QRect current_geometry = checkbox_widget_->geometry();
        QRect target_geometry = current_geometry;

        if (checked) {
            target_geometry.setSize(current_geometry.size() * 1.1);
        }

        check_animation_->setStartValue(current_geometry);
        check_animation_->setEndValue(target_geometry);
        check_animation_->start();
    }
}

void CheckBox::onStateChangedInternal(int state) {
    Qt::CheckState check_state = static_cast<Qt::CheckState>(state);

    // Animate check/uncheck
    if (check_animation_enabled_) {
        animateCheck(check_state == Qt::Checked);
    }

    // Trigger validation if enabled
    if (validate_on_change_ && validation_timer_) {
        validation_timer_->start();
    }

    // Trigger group validation
    if (!group_name_.isEmpty()) {
        QTimer::singleShot(0, this, &CheckBox::onGroupValidation);
    }
}

void CheckBox::onValidationTimer() {
    CheckBoxValidationResult result = validateState(checkState());
    is_valid_ = result.is_valid;

    if (!result.is_valid) {
        showValidationError(result.error_message);
    } else {
        validation_error_.clear();
        emit validationChanged(true, "");
    }
}

void CheckBox::onGroupValidation() {
    if (group_name_.isEmpty() || !group_validators_.contains(group_name_)) {
        return;
    }

    auto group_items = named_groups_[group_name_];
    auto group_validator = group_validators_[group_name_];

    CheckBoxValidationResult result = group_validator(group_items);

    // Apply validation result to all items in group
    for (auto* item : group_items) {
        item->is_valid_ = result.is_valid;
        if (!result.is_valid) {
            item->showValidationError(result.error_message);
        } else {
            item->validation_error_.clear();
            emit item->validationChanged(true, "");
        }
    }

    // Emit group state changed signal
    QList<CheckBox*> checked_items;
    for (auto* item : group_items) {
        if (item->isChecked()) {
            checked_items.append(item);
        }
    }
    emit groupStateChanged(group_name_, checked_items);
}

}  // namespace DeclarativeUI::Components
