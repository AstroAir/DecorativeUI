// Core/Validation.hpp
#pragma once

#include <concepts>
#include <functional>
#include <string>
#include <vector>
#include <ranges>
#include <algorithm>
#include <type_traits>
#include <QString>
#include <QRegularExpression>

#include "Concepts.hpp"

namespace DeclarativeUI::Core::Validation {

// **Validation result types**
enum class ValidationSeverity {
    Info,
    Warning,
    Error,
    Critical
};

struct ValidationMessage {
    QString message;
    ValidationSeverity severity{ValidationSeverity::Error};
    QString field_name;

    // **Designated initializer support**
    ValidationMessage(QString msg,
                     ValidationSeverity sev = ValidationSeverity::Error,
                     QString field = {})
        : message(std::move(msg)), severity(sev), field_name(std::move(field)) {}
};

class ValidationResult {
public:
    ValidationResult() = default;

    explicit ValidationResult(bool valid) : is_valid_(valid) {}

    ValidationResult(bool valid, std::vector<ValidationMessage> messages)
        : is_valid_(valid), messages_(std::move(messages)) {}

    // **Query methods**
    [[nodiscard]] constexpr bool isValid() const noexcept { return is_valid_; }
    [[nodiscard]] constexpr bool hasErrors() const noexcept { return !is_valid_; }
    [[nodiscard]] constexpr bool hasWarnings() const noexcept {
        return std::ranges::any_of(messages_, [](const auto& msg) {
            return msg.severity == ValidationSeverity::Warning;
        });
    }

    [[nodiscard]] const std::vector<ValidationMessage>& getMessages() const noexcept {
        return messages_;
    }

    // **Get messages by severity**
    [[nodiscard]] auto getErrors() const {
        return messages_ | std::views::filter([](const auto& msg) {
            return msg.severity == ValidationSeverity::Error ||
                   msg.severity == ValidationSeverity::Critical;
        });
    }

    [[nodiscard]] auto getWarnings() const {
        return messages_ | std::views::filter([](const auto& msg) {
            return msg.severity == ValidationSeverity::Warning;
        });
    }

    // **Add messages**
    void addMessage(ValidationMessage message) {
        messages_.push_back(std::move(message));
        if (message.severity == ValidationSeverity::Error ||
            message.severity == ValidationSeverity::Critical) {
            is_valid_ = false;
        }
    }

    // **Combine results**
    ValidationResult& operator+=(const ValidationResult& other) {
        is_valid_ = is_valid_ && other.is_valid_;
        messages_.insert(messages_.end(), other.messages_.begin(), other.messages_.end());
        return *this;
    }

    ValidationResult operator+(const ValidationResult& other) const {
        auto result = *this;
        result += other;
        return result;
    }

private:
    bool is_valid_{true};
    std::vector<ValidationMessage> messages_;
};

// **Validator concept**
template<typename F, typename T>
concept Validator = std::invocable<F, const T&> &&
                   std::convertible_to<std::invoke_result_t<F, const T&>, ValidationResult>;

// **Basic validators**
namespace Validators {

// **Required field validator**
template<typename T>
struct Required {
    QString message{"Field is required"};

    [[nodiscard]] ValidationResult operator()(const T& value) const {
        if constexpr (std::same_as<T, QString>) {
            return value.isEmpty() ?
                ValidationResult{false, {ValidationMessage{message}}} :
                ValidationResult{true};
        } else if constexpr (std::same_as<T, std::string>) {
            return value.empty() ?
                ValidationResult{false, {ValidationMessage{message}}} :
                ValidationResult{true};
        } else if constexpr (requires { value.has_value(); }) {
            return value.has_value() ?
                ValidationResult{true} :
                ValidationResult{false, {ValidationMessage{message}}};
        } else {
            return ValidationResult{true}; // Assume non-empty for other types
        }
    }
};

// **Length validators**
template<typename T>
struct MinLength {
    std::size_t min_length;
    QString message;

    MinLength(std::size_t min_len, QString msg = {})
        : min_length(min_len),
          message(msg.isEmpty() ? QString("Minimum length is %1").arg(min_len) : std::move(msg)) {}

    [[nodiscard]] constexpr ValidationResult operator()(const T& value) const {
        std::size_t length = 0;
        if constexpr (std::same_as<T, QString>) {
            length = value.length();
        } else if constexpr (requires { value.size(); }) {
            length = value.size();
        } else if constexpr (requires { value.length(); }) {
            length = value.length();
        }

        return length >= min_length ?
            ValidationResult{true} :
            ValidationResult{false, {{message}}};
    }
};

template<typename T>
struct MaxLength {
    std::size_t max_length;
    QString message;

    MaxLength(std::size_t max_len, QString msg = {})
        : max_length(max_len),
          message(msg.isEmpty() ? QString("Maximum length is %1").arg(max_len) : std::move(msg)) {}

    [[nodiscard]] ValidationResult operator()(const T& value) const {
        std::size_t length = 0;
        if constexpr (std::same_as<T, QString>) {
            length = value.length();
        } else if constexpr (requires { value.size(); }) {
            length = value.size();
        } else if constexpr (requires { value.length(); }) {
            length = value.length();
        }

        return length <= max_length ?
            ValidationResult{true} :
            ValidationResult{false, {{message}}};
    }
};

// **Range validators**
template<Concepts::Numeric T>
struct Range {
    T min_value;
    T max_value;
    QString message;

    Range(T min_val, T max_val, QString msg = {})
        : min_value(min_val), max_value(max_val),
          message(msg.isEmpty() ? QString("Value must be between %1 and %2").arg(min_val).arg(max_val) : std::move(msg)) {}

    [[nodiscard]] ValidationResult operator()(const T& value) const {
        return (value >= min_value && value <= max_value) ?
            ValidationResult{true} :
            ValidationResult{false, {{message}}};
    }
};

template<Concepts::Numeric T>
struct Min {
    T min_value;
    QString message;

    Min(T min_val, QString msg = {})
        : min_value(min_val),
          message(msg.isEmpty() ? QString("Minimum value is %1").arg(min_val) : std::move(msg)) {}

    [[nodiscard]] ValidationResult operator()(const T& value) const {
        return value >= min_value ?
            ValidationResult{true} :
            ValidationResult{false, {{message}}};
    }
};

template<Concepts::Numeric T>
struct Max {
    T max_value;
    QString message;

    Max(T max_val, QString msg = {})
        : max_value(max_val),
          message(msg.isEmpty() ? QString("Maximum value is %1").arg(max_val) : std::move(msg)) {}

    [[nodiscard]] ValidationResult operator()(const T& value) const {
        return value <= max_value ?
            ValidationResult{true} :
            ValidationResult{false, {{message}}};
    }
};

// **Pattern validators**
struct Pattern {
    QRegularExpression regex;
    QString message;

    Pattern(const QString& pattern, QString msg = "Invalid format")
        : regex(pattern), message(std::move(msg)) {}

    [[nodiscard]] ValidationResult operator()(const QString& value) const {
        return regex.match(value).hasMatch() ?
            ValidationResult{true} :
            ValidationResult{false, {{message}}};
    }
};

// **Email validator**
struct Email {
    QString message{"Invalid email format"};

    [[nodiscard]] ValidationResult operator()(const QString& value) const {
        static const QRegularExpression emailRegex(
            R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)"
        );
        return emailRegex.match(value).hasMatch() ?
            ValidationResult{true} :
            ValidationResult{false, {{message}}};
    }
};

// **URL validator**
struct Url {
    QString message{"Invalid URL format"};

    [[nodiscard]] ValidationResult operator()(const QString& value) const {
        static const QRegularExpression urlRegex(
            R"(^https?:\/\/(www\.)?[-a-zA-Z0-9@:%._\+~#=]{1,256}\.[a-zA-Z0-9()]{1,6}\b([-a-zA-Z0-9()@:%_\+.~#?&//=]*)$)"
        );
        return urlRegex.match(value).hasMatch() ?
            ValidationResult{true} :
            ValidationResult{false, {{message}}};
    }
};

// **Custom validator wrapper**
template<typename F>
struct Custom {
    F validator_func;
    QString message;

    template<typename Func>
    Custom(Func&& func, QString msg = "Validation failed")
        : validator_func(std::forward<Func>(func)), message(std::move(msg)) {}

    template<typename T>
    [[nodiscard]] ValidationResult operator()(const T& value) const {
        if constexpr (std::same_as<std::invoke_result_t<F, const T&>, bool>) {
            return validator_func(value) ?
                ValidationResult{true} :
                ValidationResult{false, {{message}}};
        } else {
            return validator_func(value);
        }
    }
};

}  // namespace Validators

// **Validation chain builder**
template<typename T>
class ValidationChain {
public:
    ValidationChain() = default;

    // **Add validators using perfect forwarding**
    template<typename ValidatorType>
    requires Validator<ValidatorType, T>
    ValidationChain& add(ValidatorType&& validator) {
        validators_.emplace_back(std::forward<ValidatorType>(validator));
        return *this;
    }

    // **Convenience methods for common validators**
    ValidationChain& required(QString message = "Field is required") {
        return add(Validators::Required<T>{std::move(message)});
    }

    template<typename LengthType = std::size_t>
    ValidationChain& minLength(LengthType min_len, QString message = {}) {
    return add(Validators::MinLength<T>{static_cast<std::size_t>(min_len), std::move(message)});
    }

    template<typename LengthType = std::size_t>
    ValidationChain& maxLength(LengthType max_len, QString message = {}) {
    return add(Validators::MaxLength<T>{static_cast<std::size_t>(max_len), std::move(message)});
    }

    template<typename NumericType>
    requires Concepts::Numeric<NumericType>
    ValidationChain& range(NumericType min_val, NumericType max_val, QString message = {}) {
        return add(Validators::Range<NumericType>{min_val, max_val, std::move(message)});
    }

    ValidationChain& pattern(const QString& regex_pattern, QString message = "Invalid format") {
        return add(Validators::Pattern{regex_pattern, std::move(message)});
    }

    ValidationChain& email(QString message = "Invalid email format") {
        return add(Validators::Email{std::move(message)});
    }

    ValidationChain& url(QString message = "Invalid URL format") {
        return add(Validators::Url{std::move(message)});
    }

    template<typename F>
    ValidationChain& custom(F&& func, QString message = "Validation failed") {
        return add(Validators::Custom{std::forward<F>(func), std::move(message)});
    }

    // **Validate value**
    [[nodiscard]] ValidationResult validate(const T& value) const {
        ValidationResult result{true};

        for (const auto& validator : validators_) {
            auto validator_result = validator(value);

            result += validator_result;

            // Stop on first error if configured to do so
            if (stop_on_first_error_ && !validator_result.isValid()) {
                break;
            }
        }

        return result;
    }

    // **Configuration**
    ValidationChain& stopOnFirstError(bool stop = true) {
        stop_on_first_error_ = stop;
        return *this;
    }

private:
    std::vector<std::function<ValidationResult(const T&)>> validators_;
    bool stop_on_first_error_{false};
};

// **Factory function for creating validation chains**
template<typename T>
[[nodiscard]] constexpr auto validate() -> ValidationChain<T> {
    return ValidationChain<T>{};
}

}  // namespace DeclarativeUI::Core::Validation
