// Components/LineEdit.hpp
#pragma once
#include <QCompleter>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPropertyAnimation>
#include <QRegularExpressionValidator>
#include <QTimer>
#include <QValidator>
#include <functional>
#include <memory>

#include "../Core/Theme.hpp"
#include "../Core/UIElement.hpp"

namespace DeclarativeUI::Components {

/**
 * @brief Validation result structure
 */
struct ValidationResult {
    bool is_valid;
    QString error_message;
    QString suggestion;

    ValidationResult(bool valid = true, const QString& error = "",
                     const QString& hint = "")
        : is_valid(valid), error_message(error), suggestion(hint) {}
};

class LineEdit : public Core::UIElement {
    Q_OBJECT

public:
    explicit LineEdit(QObject* parent = nullptr);

    // **Basic fluent interface (backward compatible)**
    LineEdit& text(const QString& text);
    LineEdit& placeholder(const QString& placeholder);
    LineEdit& readOnly(bool readonly);
    LineEdit& maxLength(int length);
    LineEdit& validator(QValidator* validator);
    LineEdit& echoMode(QLineEdit::EchoMode mode);
    LineEdit& onTextChanged(std::function<void(const QString&)> handler);
    LineEdit& onTextEdited(std::function<void(const QString&)> handler);
    LineEdit& onReturnPressed(std::function<void()> handler);
    LineEdit& style(const QString& stylesheet);

    // **Enhanced fluent interface**
    LineEdit& tooltip(const QString& tooltip_text);
    LineEdit& accessibleName(const QString& name);
    LineEdit& accessibleDescription(const QString& description);
    LineEdit& label(const QString& label_text);
    LineEdit& helpText(const QString& help_text);
    LineEdit& errorText(const QString& error_text);

    // **Validation**
    LineEdit& required(bool required = true);
    LineEdit& minLength(int min_length);
    LineEdit& pattern(const QString& regex_pattern,
                      const QString& error_message = "");
    LineEdit& email(bool validate_email = true);
    LineEdit& url(bool validate_url = true);
    LineEdit& numeric(bool integers_only = false);
    LineEdit& customValidator(
        std::function<ValidationResult(const QString&)> validator);
    LineEdit& validateOnType(bool validate_while_typing = true);
    LineEdit& validateOnFocus(bool validate_on_focus_lost = true);

    // **Auto-completion and suggestions**
    LineEdit& autoComplete(const QStringList& completions);
    LineEdit& autoCompleteMode(QCompleter::CompletionMode mode);
    LineEdit& suggestions(const QStringList& suggestions);
    LineEdit& dynamicSuggestions(
        std::function<QStringList(const QString&)> provider);

    // **Visual enhancements**
    LineEdit& icon(const QIcon& icon, bool leading = true);
    LineEdit& clearButton(bool enabled = true);
    LineEdit& showPasswordToggle(bool enabled = true);  // For password fields
    LineEdit& borderColor(const QColor& color);
    LineEdit& focusColor(const QColor& color);
    LineEdit& errorColor(const QColor& color);
    LineEdit& successColor(const QColor& color);
    LineEdit& borderRadius(int radius);
    LineEdit& padding(int padding);

    // **Input formatting**
    LineEdit& inputMask(const QString& mask);
    LineEdit& formatAsPhone(bool enabled = true);
    LineEdit& formatAsCurrency(bool enabled = true,
                               const QString& currency = "$");
    LineEdit& formatAsDate(bool enabled = true,
                           const QString& format = "yyyy-MM-dd");
    LineEdit& upperCase(bool enabled = true);
    LineEdit& lowerCase(bool enabled = true);
    LineEdit& titleCase(bool enabled = true);

    // **Behavior**
    LineEdit& selectAllOnFocus(bool enabled = true);
    LineEdit& clearOnEscape(bool enabled = true);
    LineEdit& submitOnEnter(bool enabled = true);
    LineEdit& debounceDelay(int milliseconds);
    LineEdit& undoRedo(bool enabled = true);

    // **Event handlers**
    LineEdit& onValidationChanged(
        std::function<void(bool, const QString&)> handler);
    LineEdit& onSubmit(std::function<void(const QString&)> handler);
    LineEdit& onFocus(std::function<void(bool)> handler);
    LineEdit& onTextFormatted(std::function<void(const QString&)> handler);

    // **Accessibility**
    LineEdit& role(const QString& aria_role);
    LineEdit& tabIndex(int index);
    LineEdit& describedBy(const QString& element_id);
    LineEdit& labelledBy(const QString& element_id);

    void initialize() override;
    QString getText() const;
    void setText(const QString& text);

    // **State queries**
    bool isValid() const;
    QString getValidationError() const { return current_error_; }
    QString getFormattedText() const;
    QStringList getCurrentSuggestions() const;

signals:
    void validationChanged(bool is_valid, const QString& error_message);
    void textSubmitted(const QString& text);
    void suggestionSelected(const QString& suggestion);
    void formattingApplied(const QString& formatted_text);

protected:
    void setupValidation();
    void setupAutoCompletion();
    void setupVisualEffects();
    void setupEventHandlers();
    void setupAccessibility();
    void setupFormatting();

    ValidationResult validateText(const QString& text) const;
    void showValidationState(bool is_valid, const QString& error = "");
    void updateSuggestions(const QString& text);
    QString applyFormatting(const QString& text) const;
    void animateBorder(const QColor& color);

private slots:
    void onTextChangedInternal();
    void onFocusChangedInternal(bool has_focus);
    void onValidationTimer();
    void onSuggestionActivated(const QString& suggestion);

private:
    QLineEdit* line_edit_widget_;
    std::function<void(const QString&)> text_changed_handler_;
    std::function<void(const QString&)> text_edited_handler_;
    std::function<void()> return_pressed_handler_;

    // **Enhanced properties**
    QString tooltip_text_;
    QString accessible_name_;
    QString accessible_description_;
    QString label_text_;
    QString help_text_;
    QString error_text_;

    // **Validation**
    bool required_;
    int min_length_;
    QString regex_pattern_;
    QString pattern_error_;
    bool validate_email_;
    bool validate_url_;
    bool validate_numeric_;
    bool integers_only_;
    std::function<ValidationResult(const QString&)> custom_validator_;
    bool validate_on_type_;
    bool validate_on_focus_;
    QString current_error_;
    bool is_valid_;

    // **Auto-completion**
    QStringList completions_;
    QCompleter::CompletionMode completion_mode_;
    QStringList suggestions_;
    std::function<QStringList(const QString&)> dynamic_suggestions_;
    std::unique_ptr<QCompleter> completer_;

    // **Visual**
    QIcon icon_;
    bool icon_leading_;
    bool clear_button_enabled_;
    bool password_toggle_enabled_;
    QColor border_color_;
    QColor focus_color_;
    QColor error_color_;
    QColor success_color_;
    int border_radius_;
    int padding_;

    // **Formatting**
    QString input_mask_;
    bool format_phone_;
    bool format_currency_;
    QString currency_symbol_;
    bool format_date_;
    QString date_format_;
    bool upper_case_;
    bool lower_case_;
    bool title_case_;

    // **Behavior**
    bool select_all_on_focus_;
    bool clear_on_escape_;
    bool submit_on_enter_;
    int debounce_delay_;
    bool undo_redo_enabled_;

    // **Event handlers**
    std::function<void(bool, const QString&)> validation_handler_;
    std::function<void(const QString&)> submit_handler_;
    std::function<void(bool)> focus_handler_;
    std::function<void(const QString&)> formatting_handler_;

    // **Accessibility**
    QString aria_role_;
    int tab_index_;
    QString described_by_;
    QString labelled_by_;

    // **Internal components**
    std::unique_ptr<QTimer> validation_timer_;
    std::unique_ptr<QTimer> debounce_timer_;
    std::unique_ptr<QPropertyAnimation> border_animation_;
    std::unique_ptr<QLabel> label_widget_;
    std::unique_ptr<QLabel> help_widget_;
    std::unique_ptr<QLabel> error_widget_;
};

}  // namespace DeclarativeUI::Components
