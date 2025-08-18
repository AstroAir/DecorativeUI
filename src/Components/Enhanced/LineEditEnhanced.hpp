/**
 * @file LineEditEnhanced.hpp
 * @brief Enhanced LineEdit component with validation, accessibility, and advanced features
 */

#pragma once

#include "../LineEdit.hpp"
#include "../../Core/Theme.hpp"
#include <QRegularExpressionValidator>
#include <QCompleter>
#include <QLabel>
#include <QHBoxLayout>
#include <QPropertyAnimation>
#include <QTimer>
#include <memory>

namespace DeclarativeUI::Components::Enhanced {

/**
 * @brief Validation result structure
 */
struct ValidationResult {
    bool is_valid;
    QString error_message;
    QString suggestion;
    
    ValidationResult(bool valid = true, const QString& error = "", const QString& hint = "")
        : is_valid(valid), error_message(error), suggestion(hint) {}
};

/**
 * @brief Enhanced LineEdit with comprehensive validation and accessibility
 */
class LineEditEnhanced : public Components::LineEdit {
    Q_OBJECT

public:
    explicit LineEditEnhanced(QObject* parent = nullptr);
    ~LineEditEnhanced() override = default;

    // Enhanced fluent interface
    LineEditEnhanced& tooltip(const QString& tooltip_text);
    LineEditEnhanced& accessibleName(const QString& name);
    LineEditEnhanced& accessibleDescription(const QString& description);
    LineEditEnhanced& label(const QString& label_text);
    LineEditEnhanced& helpText(const QString& help_text);
    LineEditEnhanced& errorText(const QString& error_text);
    
    // Validation
    LineEditEnhanced& required(bool required = true);
    LineEditEnhanced& minLength(int min_length);
    LineEditEnhanced& maxLength(int max_length);
    LineEditEnhanced& pattern(const QString& regex_pattern, const QString& error_message = "");
    LineEditEnhanced& email(bool validate_email = true);
    LineEditEnhanced& url(bool validate_url = true);
    LineEditEnhanced& numeric(bool integers_only = false);
    LineEditEnhanced& customValidator(std::function<ValidationResult(const QString&)> validator);
    LineEditEnhanced& validateOnType(bool validate_while_typing = true);
    LineEditEnhanced& validateOnFocus(bool validate_on_focus_lost = true);
    
    // Auto-completion and suggestions
    LineEditEnhanced& autoComplete(const QStringList& completions);
    LineEditEnhanced& autoCompleteMode(QCompleter::CompletionMode mode);
    LineEditEnhanced& suggestions(const QStringList& suggestions);
    LineEditEnhanced& dynamicSuggestions(std::function<QStringList(const QString&)> provider);
    
    // Visual enhancements
    LineEditEnhanced& icon(const QIcon& icon, bool leading = true);
    LineEditEnhanced& clearButton(bool enabled = true);
    LineEditEnhanced& showPasswordToggle(bool enabled = true); // For password fields
    LineEditEnhanced& borderColor(const QColor& color);
    LineEditEnhanced& focusColor(const QColor& color);
    LineEditEnhanced& errorColor(const QColor& color);
    LineEditEnhanced& successColor(const QColor& color);
    LineEditEnhanced& borderRadius(int radius);
    LineEditEnhanced& padding(int padding);
    
    // Input formatting
    LineEditEnhanced& inputMask(const QString& mask);
    LineEditEnhanced& formatAsPhone(bool enabled = true);
    LineEditEnhanced& formatAsCurrency(bool enabled = true, const QString& currency = "$");
    LineEditEnhanced& formatAsDate(bool enabled = true, const QString& format = "yyyy-MM-dd");
    LineEditEnhanced& upperCase(bool enabled = true);
    LineEditEnhanced& lowerCase(bool enabled = true);
    LineEditEnhanced& titleCase(bool enabled = true);
    
    // Behavior
    LineEditEnhanced& selectAllOnFocus(bool enabled = true);
    LineEditEnhanced& clearOnEscape(bool enabled = true);
    LineEditEnhanced& submitOnEnter(bool enabled = true);
    LineEditEnhanced& debounceDelay(int milliseconds);
    LineEditEnhanced& undoRedo(bool enabled = true);
    
    // Event handlers
    LineEditEnhanced& onValidationChanged(std::function<void(bool, const QString&)> handler);
    LineEditEnhanced& onSubmit(std::function<void(const QString&)> handler);
    LineEditEnhanced& onFocus(std::function<void(bool)> handler);
    LineEditEnhanced& onTextFormatted(std::function<void(const QString&)> handler);
    
    // Accessibility
    LineEditEnhanced& role(const QString& aria_role);
    LineEditEnhanced& tabIndex(int index);
    LineEditEnhanced& describedBy(const QString& element_id);
    LineEditEnhanced& labelledBy(const QString& element_id);
    
    void initialize() override;
    
    // State queries
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
    // Enhanced properties
    QString tooltip_text_;
    QString accessible_name_;
    QString accessible_description_;
    QString label_text_;
    QString help_text_;
    QString error_text_;
    
    // Validation
    bool required_;
    int min_length_;
    int max_length_;
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
    
    // Auto-completion
    QStringList completions_;
    QCompleter::CompletionMode completion_mode_;
    QStringList suggestions_;
    std::function<QStringList(const QString&)> dynamic_suggestions_;
    std::unique_ptr<QCompleter> completer_;
    
    // Visual
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
    
    // Formatting
    QString input_mask_;
    bool format_phone_;
    bool format_currency_;
    QString currency_symbol_;
    bool format_date_;
    QString date_format_;
    bool upper_case_;
    bool lower_case_;
    bool title_case_;
    
    // Behavior
    bool select_all_on_focus_;
    bool clear_on_escape_;
    bool submit_on_enter_;
    int debounce_delay_;
    bool undo_redo_enabled_;
    
    // Event handlers
    std::function<void(bool, const QString&)> validation_handler_;
    std::function<void(const QString&)> submit_handler_;
    std::function<void(bool)> focus_handler_;
    std::function<void(const QString&)> formatting_handler_;
    
    // Accessibility
    QString aria_role_;
    int tab_index_;
    QString described_by_;
    QString labelled_by_;
    
    // Internal components
    std::unique_ptr<QTimer> validation_timer_;
    std::unique_ptr<QTimer> debounce_timer_;
    std::unique_ptr<QPropertyAnimation> border_animation_;
    std::unique_ptr<QLabel> label_widget_;
    std::unique_ptr<QLabel> help_widget_;
    std::unique_ptr<QLabel> error_widget_;
};

/**
 * @brief Factory function for creating enhanced line edits
 */
inline std::unique_ptr<LineEditEnhanced> createEnhancedLineEdit() {
    return std::make_unique<LineEditEnhanced>();
}

} // namespace DeclarativeUI::Components::Enhanced
