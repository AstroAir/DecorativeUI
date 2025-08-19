// Components/LineEdit.cpp
#include "LineEdit.hpp"
#include <QDebug>
#include <QRegularExpression>
#include <QStringListModel>

namespace DeclarativeUI::Components {

// **Implementation with enhanced features**
LineEdit::LineEdit(QObject* parent)
    : UIElement(parent),
      line_edit_widget_(nullptr),
      required_(false),
      min_length_(0),
      validate_email_(false),
      validate_url_(false),
      validate_numeric_(false),
      integers_only_(false),
      validate_on_type_(true),
      validate_on_focus_(true),
      is_valid_(true),
      completion_mode_(QCompleter::PopupCompletion),
      icon_leading_(true),
      clear_button_enabled_(false),
      password_toggle_enabled_(false),
      border_radius_(4),
      padding_(8),
      format_phone_(false),
      format_currency_(false),
      currency_symbol_("$"),
      format_date_(false),
      date_format_("yyyy-MM-dd"),
      upper_case_(false),
      lower_case_(false),
      title_case_(false),
      select_all_on_focus_(false),
      clear_on_escape_(false),
      submit_on_enter_(false),
      debounce_delay_(0),
      undo_redo_enabled_(true),
      tab_index_(-1) {
    // Initialize with default accessibility role
    aria_role_ = "textbox";
}

// **Basic fluent interface (backward compatible)**
LineEdit& LineEdit::text(const QString& text) {
    return static_cast<LineEdit&>(setProperty("text", text));
}

LineEdit& LineEdit::placeholder(const QString& placeholder) {
    return static_cast<LineEdit&>(setProperty("placeholderText", placeholder));
}

LineEdit& LineEdit::readOnly(bool readonly) {
    return static_cast<LineEdit&>(setProperty("readOnly", readonly));
}

LineEdit& LineEdit::maxLength(int length) {
    return static_cast<LineEdit&>(setProperty("maxLength", length));
}

LineEdit& LineEdit::validator(QValidator* validator) {
    return static_cast<LineEdit&>(
        setProperty("validator", QVariant::fromValue(validator)));
}

LineEdit& LineEdit::echoMode(QLineEdit::EchoMode mode) {
    return static_cast<LineEdit&>(
        setProperty("echoMode", static_cast<int>(mode)));
}

LineEdit& LineEdit::onTextChanged(std::function<void(const QString&)> handler) {
    text_changed_handler_ = std::move(handler);
    return *this;
}

LineEdit& LineEdit::onTextEdited(std::function<void(const QString&)> handler) {
    text_edited_handler_ = std::move(handler);
    return *this;
}

LineEdit& LineEdit::onReturnPressed(std::function<void()> handler) {
    return_pressed_handler_ = std::move(handler);
    return *this;
}

LineEdit& LineEdit::style(const QString& stylesheet) {
    return static_cast<LineEdit&>(setProperty("styleSheet", stylesheet));
}

// **Enhanced fluent interface**
LineEdit& LineEdit::tooltip(const QString& tooltip_text) {
    tooltip_text_ = tooltip_text;
    return *this;
}

LineEdit& LineEdit::accessibleName(const QString& name) {
    accessible_name_ = name;
    return *this;
}

LineEdit& LineEdit::accessibleDescription(const QString& description) {
    accessible_description_ = description;
    return *this;
}

LineEdit& LineEdit::label(const QString& label_text) {
    label_text_ = label_text;
    return *this;
}

LineEdit& LineEdit::helpText(const QString& help_text) {
    help_text_ = help_text;
    return *this;
}

LineEdit& LineEdit::errorText(const QString& error_text) {
    error_text_ = error_text;
    return *this;
}

// **Validation**
LineEdit& LineEdit::required(bool required) {
    required_ = required;
    return *this;
}

LineEdit& LineEdit::minLength(int min_length) {
    min_length_ = min_length;
    return *this;
}

LineEdit& LineEdit::pattern(const QString& regex_pattern,
                            const QString& error_message) {
    regex_pattern_ = regex_pattern;
    pattern_error_ = error_message;
    return *this;
}

LineEdit& LineEdit::email(bool validate_email) {
    validate_email_ = validate_email;
    return *this;
}

LineEdit& LineEdit::url(bool validate_url) {
    validate_url_ = validate_url;
    return *this;
}

LineEdit& LineEdit::numeric(bool integers_only) {
    validate_numeric_ = true;
    integers_only_ = integers_only;
    return *this;
}

LineEdit& LineEdit::customValidator(
    std::function<ValidationResult(const QString&)> validator) {
    custom_validator_ = std::move(validator);
    return *this;
}

LineEdit& LineEdit::validateOnType(bool validate_while_typing) {
    validate_on_type_ = validate_while_typing;
    return *this;
}

LineEdit& LineEdit::validateOnFocus(bool validate_on_focus_lost) {
    validate_on_focus_ = validate_on_focus_lost;
    return *this;
}

// **Auto-completion and suggestions**
LineEdit& LineEdit::autoComplete(const QStringList& completions) {
    completions_ = completions;
    return *this;
}

LineEdit& LineEdit::autoCompleteMode(QCompleter::CompletionMode mode) {
    completion_mode_ = mode;
    return *this;
}

LineEdit& LineEdit::suggestions(const QStringList& suggestions) {
    suggestions_ = suggestions;
    return *this;
}

LineEdit& LineEdit::dynamicSuggestions(
    std::function<QStringList(const QString&)> provider) {
    dynamic_suggestions_ = std::move(provider);
    return *this;
}

// **Visual enhancements**
LineEdit& LineEdit::icon(const QIcon& icon, bool leading) {
    icon_ = icon;
    icon_leading_ = leading;
    return *this;
}

LineEdit& LineEdit::clearButton(bool enabled) {
    clear_button_enabled_ = enabled;
    return *this;
}

LineEdit& LineEdit::showPasswordToggle(bool enabled) {
    password_toggle_enabled_ = enabled;
    return *this;
}

LineEdit& LineEdit::borderColor(const QColor& color) {
    border_color_ = color;
    return *this;
}

LineEdit& LineEdit::focusColor(const QColor& color) {
    focus_color_ = color;
    return *this;
}

LineEdit& LineEdit::errorColor(const QColor& color) {
    error_color_ = color;
    return *this;
}

LineEdit& LineEdit::successColor(const QColor& color) {
    success_color_ = color;
    return *this;
}

LineEdit& LineEdit::borderRadius(int radius) {
    border_radius_ = radius;
    return *this;
}

LineEdit& LineEdit::padding(int padding) {
    padding_ = padding;
    return *this;
}

// **Input formatting**
LineEdit& LineEdit::inputMask(const QString& mask) {
    input_mask_ = mask;
    return *this;
}

LineEdit& LineEdit::formatAsPhone(bool enabled) {
    format_phone_ = enabled;
    return *this;
}

LineEdit& LineEdit::formatAsCurrency(bool enabled, const QString& currency) {
    format_currency_ = enabled;
    currency_symbol_ = currency;
    return *this;
}

LineEdit& LineEdit::formatAsDate(bool enabled, const QString& format) {
    format_date_ = enabled;
    date_format_ = format;
    return *this;
}

LineEdit& LineEdit::upperCase(bool enabled) {
    upper_case_ = enabled;
    return *this;
}

LineEdit& LineEdit::lowerCase(bool enabled) {
    lower_case_ = enabled;
    return *this;
}

LineEdit& LineEdit::titleCase(bool enabled) {
    title_case_ = enabled;
    return *this;
}

// **Behavior**
LineEdit& LineEdit::selectAllOnFocus(bool enabled) {
    select_all_on_focus_ = enabled;
    return *this;
}

LineEdit& LineEdit::clearOnEscape(bool enabled) {
    clear_on_escape_ = enabled;
    return *this;
}

LineEdit& LineEdit::submitOnEnter(bool enabled) {
    submit_on_enter_ = enabled;
    return *this;
}

LineEdit& LineEdit::debounceDelay(int milliseconds) {
    debounce_delay_ = milliseconds;
    return *this;
}

LineEdit& LineEdit::undoRedo(bool enabled) {
    undo_redo_enabled_ = enabled;
    return *this;
}

// **Event handlers**
LineEdit& LineEdit::onValidationChanged(
    std::function<void(bool, const QString&)> handler) {
    validation_handler_ = std::move(handler);
    return *this;
}

LineEdit& LineEdit::onSubmit(std::function<void(const QString&)> handler) {
    submit_handler_ = std::move(handler);
    return *this;
}

LineEdit& LineEdit::onFocus(std::function<void(bool)> handler) {
    focus_handler_ = std::move(handler);
    return *this;
}

LineEdit& LineEdit::onTextFormatted(
    std::function<void(const QString&)> handler) {
    formatting_handler_ = std::move(handler);
    return *this;
}

// **Accessibility**
LineEdit& LineEdit::role(const QString& aria_role) {
    aria_role_ = aria_role;
    return *this;
}

LineEdit& LineEdit::tabIndex(int index) {
    tab_index_ = index;
    return *this;
}

LineEdit& LineEdit::describedBy(const QString& element_id) {
    described_by_ = element_id;
    return *this;
}

LineEdit& LineEdit::labelledBy(const QString& element_id) {
    labelled_by_ = element_id;
    return *this;
}

void LineEdit::initialize() {
    if (!line_edit_widget_) {
        try {
            line_edit_widget_ = new QLineEdit();
            setWidget(line_edit_widget_);

            // Connect basic signals
            if (text_changed_handler_) {
                connect(line_edit_widget_, &QLineEdit::textChanged, this,
                        [this](const QString& text) {
                            text_changed_handler_(text);
                        });
            }

            if (text_edited_handler_) {
                connect(line_edit_widget_, &QLineEdit::textEdited, this,
                        [this](const QString& text) {
                            text_edited_handler_(text);
                        });
            }

            if (return_pressed_handler_) {
                connect(line_edit_widget_, &QLineEdit::returnPressed, this,
                        [this]() { return_pressed_handler_(); });
            }

            // Initialize enhanced features
            setupValidation();
            setupAutoCompletion();
            setupVisualEffects();
            setupEventHandlers();
            setupAccessibility();
            setupFormatting();

            qDebug() << "Enhanced line edit initialized successfully";

        } catch (const std::exception& e) {
            qWarning() << "Failed to initialize enhanced line edit:"
                       << e.what();
        }
    }
}

QString LineEdit::getText() const {
    return line_edit_widget_ ? line_edit_widget_->text() : QString();
}

void LineEdit::setText(const QString& text) {
    if (line_edit_widget_) {
        line_edit_widget_->setText(text);
    }
}

QString LineEdit::getFormattedText() const {
    return applyFormatting(getText());
}

QStringList LineEdit::getCurrentSuggestions() const {
    if (dynamic_suggestions_) {
        return dynamic_suggestions_(getText());
    }
    return suggestions_;
}

bool LineEdit::isValid() const { return is_valid_; }

void LineEdit::setupValidation() {
    if (validate_on_type_ || validate_on_focus_ || custom_validator_ ||
        required_) {
        validation_timer_ = std::make_unique<QTimer>();
        validation_timer_->setSingleShot(true);
        validation_timer_->setInterval(
            300);  // Validate 300ms after last change

        connect(validation_timer_.get(), &QTimer::timeout, this,
                &LineEdit::onValidationTimer);
    }
}

void LineEdit::setupAutoCompletion() {
    if (!completions_.isEmpty()) {
        completer_ = std::make_unique<QCompleter>(completions_);
        completer_->setCompletionMode(completion_mode_);
        completer_->setCaseSensitivity(Qt::CaseInsensitive);
        completer_->setFilterMode(Qt::MatchContains);

        if (line_edit_widget_) {
            line_edit_widget_->setCompleter(completer_.get());
            connect(completer_.get(),
                    QOverload<const QString&>::of(&QCompleter::activated), this,
                    &LineEdit::onSuggestionActivated);
        }
    }
}

void LineEdit::setupVisualEffects() {
    if (!line_edit_widget_)
        return;

    // Set up clear button
    if (clear_button_enabled_) {
        line_edit_widget_->setClearButtonEnabled(true);
    }

    // Apply visual styling
    QString style_sheet = line_edit_widget_->styleSheet();

    if (border_radius_ > 0) {
        style_sheet +=
            QString("QLineEdit { border-radius: %1px; }").arg(border_radius_);
    }

    if (padding_ > 0) {
        style_sheet += QString("QLineEdit { padding: %1px; }").arg(padding_);
    }

    if (border_color_.isValid()) {
        style_sheet += QString("QLineEdit { border: 1px solid %1; }")
                           .arg(border_color_.name());
    }

    if (focus_color_.isValid()) {
        style_sheet += QString("QLineEdit:focus { border-color: %1; }")
                           .arg(focus_color_.name());
    }

    if (!style_sheet.isEmpty()) {
        line_edit_widget_->setStyleSheet(style_sheet);
    }

    // Set up border animation
    if (focus_color_.isValid()) {
        border_animation_ = std::make_unique<QPropertyAnimation>(
            line_edit_widget_, "styleSheet");
        border_animation_->setDuration(200);
        border_animation_->setEasingCurve(QEasingCurve::OutCubic);
    }
}

void LineEdit::setupEventHandlers() {
    if (!line_edit_widget_)
        return;

    // Connect enhanced text change handler
    connect(line_edit_widget_, &QLineEdit::textChanged, this,
            &LineEdit::onTextChangedInternal);

    // Install event filter for focus events
    line_edit_widget_->installEventFilter(this);

    // Set up debounce timer if needed
    if (debounce_delay_ > 0) {
        debounce_timer_ = std::make_unique<QTimer>();
        debounce_timer_->setSingleShot(true);
        debounce_timer_->setInterval(debounce_delay_);
    }
}

void LineEdit::setupAccessibility() {
    auto* widget = getWidget();
    if (!widget)
        return;

    // Set up accessibility using the existing AccessibilityManager
    auto accessibility =
        Core::Accessibility::accessibilityFor()
            .name(accessible_name_.isEmpty() ? label_text_ : accessible_name_)
            .description(accessible_description_)
            .helpText(help_text_.isEmpty() ? tooltip_text_ : help_text_)
            .role(Core::Accessibility::AccessibilityRole::TextEdit)
            .required(required_);

    if (tab_index_ >= 0) {
        accessibility.tabIndex(tab_index_);
    }

    accessibility.applyTo(widget);

    // Set tooltip
    if (!tooltip_text_.isEmpty()) {
        widget->setToolTip(tooltip_text_);
    }
}

void LineEdit::setupFormatting() {
    if (!line_edit_widget_)
        return;

    // Set input mask if specified
    if (!input_mask_.isEmpty()) {
        line_edit_widget_->setInputMask(input_mask_);
    }

    // Set up undo/redo
    if (line_edit_widget_) {
        line_edit_widget_->setProperty("undoRedoEnabled", undo_redo_enabled_);
    }
}

ValidationResult LineEdit::validateText(const QString& text) const {
    // Check if required
    if (required_ && text.isEmpty()) {
        return ValidationResult(false, "This field is required");
    }

    // Check minimum length
    if (min_length_ > 0 && text.length() < min_length_) {
        return ValidationResult(
            false, QString("Minimum length is %1 characters").arg(min_length_));
    }

    // Check pattern
    if (!regex_pattern_.isEmpty()) {
        QRegularExpression regex(regex_pattern_);
        if (!regex.match(text).hasMatch()) {
            return ValidationResult(false, pattern_error_.isEmpty()
                                               ? "Invalid format"
                                               : pattern_error_);
        }
    }

    // Check email
    if (validate_email_) {
        QRegularExpression email_regex(
            R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
        if (!email_regex.match(text).hasMatch()) {
            return ValidationResult(false,
                                    "Please enter a valid email address");
        }
    }

    // Check URL
    if (validate_url_) {
        QRegularExpression url_regex(R"(^https?://[^\s/$.?#].[^\s]*$)");
        if (!url_regex.match(text).hasMatch()) {
            return ValidationResult(false, "Please enter a valid URL");
        }
    }

    // Check numeric
    if (validate_numeric_) {
        if (integers_only_) {
            bool ok;
            text.toInt(&ok);
            if (!ok) {
                return ValidationResult(false, "Please enter a valid integer");
            }
        } else {
            bool ok;
            text.toDouble(&ok);
            if (!ok) {
                return ValidationResult(false, "Please enter a valid number");
            }
        }
    }

    // Custom validation
    if (custom_validator_) {
        return custom_validator_(text);
    }

    return ValidationResult(true);
}

void LineEdit::showValidationState(bool is_valid, const QString& error) {
    is_valid_ = is_valid;
    current_error_ = error;

    if (line_edit_widget_) {
        // Update visual state
        QString style_sheet = line_edit_widget_->styleSheet();

        if (!is_valid && error_color_.isValid()) {
            animateBorder(error_color_);
        } else if (is_valid && success_color_.isValid()) {
            animateBorder(success_color_);
        }
    }

    // Call validation handler
    if (validation_handler_) {
        validation_handler_(is_valid, error);
    }

    emit validationChanged(is_valid, error);
}

void LineEdit::updateSuggestions(const QString& text) {
    if (dynamic_suggestions_) {
        suggestions_ = dynamic_suggestions_(text);

        if (completer_) {
            completer_->setModel(
                new QStringListModel(suggestions_, completer_.get()));
        }
    }
}

QString LineEdit::applyFormatting(const QString& text) const {
    QString formatted = text;

    if (upper_case_) {
        formatted = formatted.toUpper();
    } else if (lower_case_) {
        formatted = formatted.toLower();
    } else if (title_case_) {
        formatted = formatted.toLower();
        if (!formatted.isEmpty()) {
            formatted[0] = formatted[0].toUpper();
        }
    }

    // Apply specific formatting
    if (format_phone_) {
        // Simple phone formatting (XXX) XXX-XXXX
        QString digits = formatted;
        digits.remove(QRegularExpression("[^0-9]"));
        if (digits.length() >= 10) {
            formatted = QString("(%1) %2-%3")
                            .arg(digits.mid(0, 3))
                            .arg(digits.mid(3, 3))
                            .arg(digits.mid(6, 4));
        }
    }

    if (format_currency_) {
        bool ok;
        double value = formatted.toDouble(&ok);
        if (ok) {
            formatted =
                QString("%1%2").arg(currency_symbol_).arg(value, 0, 'f', 2);
        }
    }

    return formatted;
}

void LineEdit::animateBorder(const QColor& color) {
    if (border_animation_ && line_edit_widget_) {
        QString current_style = line_edit_widget_->styleSheet();
        QString target_style = current_style;

        // Update border color in stylesheet
        QRegularExpression border_regex("border-color:\\s*[^;]+;");
        if (border_regex.match(target_style).hasMatch()) {
            target_style.replace(
                border_regex, QString("border-color: %1;").arg(color.name()));
        } else {
            target_style += QString("border-color: %1;").arg(color.name());
        }

        border_animation_->setStartValue(current_style);
        border_animation_->setEndValue(target_style);
        border_animation_->start();
    }
}

void LineEdit::onTextChangedInternal() {
    QString text = getText();

    // Apply formatting
    QString formatted = applyFormatting(text);
    if (formatted != text && formatting_handler_) {
        formatting_handler_(formatted);
        emit formattingApplied(formatted);
    }

    // Update suggestions
    updateSuggestions(text);

    // Trigger validation if enabled
    if (validate_on_type_ && validation_timer_) {
        validation_timer_->start();
    }

    // Handle debouncing
    if (debounce_timer_) {
        debounce_timer_->start();
    }
}

void LineEdit::onFocusChangedInternal(bool has_focus) {
    if (focus_handler_) {
        focus_handler_(has_focus);
    }

    if (has_focus) {
        if (select_all_on_focus_ && line_edit_widget_) {
            line_edit_widget_->selectAll();
        }
    } else {
        // Validate on focus lost
        if (validate_on_focus_ && validation_timer_) {
            validation_timer_->start();
        }
    }
}

void LineEdit::onValidationTimer() {
    ValidationResult result = validateText(getText());
    showValidationState(result.is_valid, result.error_message);
}

void LineEdit::onSuggestionActivated(const QString& suggestion) {
    emit suggestionSelected(suggestion);
}

}  // namespace DeclarativeUI::Components
