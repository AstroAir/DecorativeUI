// Core/ErrorHandling.hpp
#pragma once

#include <exception>
#include <stdexcept>
#include <string>
#include <string_view>
#include <source_location>
#include <format>
#include <functional>
#include <vector>
#include <memory>
#include <chrono>
#include <fstream>
#include <mutex>
#include <concepts>
#include <QString>
#include <QDebug>
#include <QLoggingCategory>

#include "Concepts.hpp"

namespace DeclarativeUI::Core::ErrorHandling {

// **Error severity levels**
enum class ErrorSeverity {
    Debug,
    Info,
    Warning,
    Error,
    Critical,
    Fatal
};

// **Error categories**
enum class ErrorCategory {
    General,
    UI,
    Layout,
    Animation,
    Theme,
    Validation,
    Memory,
    IO,
    Network,
    Configuration
};

// **Error context information**
struct ErrorContext {
    std::source_location location;
    std::chrono::system_clock::time_point timestamp;
    QString component_name;
    QString operation;
    std::vector<std::pair<QString, QString>> additional_data;

    ErrorContext(std::source_location loc = std::source_location::current())
        : location(loc), timestamp(std::chrono::system_clock::now()) {}
};

// **Base exception class with rich context**
class UIException : public std::exception {
public:
    UIException(QString message,
                ErrorSeverity severity = ErrorSeverity::Error,
                ErrorCategory category = ErrorCategory::General,
                ErrorContext context = ErrorContext{})
        : message_(std::move(message)),
          severity_(severity),
          category_(category),
          context_(std::move(context)) {}

    [[nodiscard]] const char* what() const noexcept override {
        if (what_cache_.empty()) {
            what_cache_ = message_.toStdString();
        }
        return what_cache_.c_str();
    }

    [[nodiscard]] const QString& getMessage() const noexcept { return message_; }
    [[nodiscard]] ErrorSeverity getSeverity() const noexcept { return severity_; }
    [[nodiscard]] ErrorCategory getCategory() const noexcept { return category_; }
    [[nodiscard]] const ErrorContext& getContext() const noexcept { return context_; }

    // **Formatted error message with context**
    [[nodiscard]] QString getFormattedMessage() const {
        return QString("Error in %1:%2 (%3): %4")
            .arg(QString::fromStdString(context_.location.file_name()))
            .arg(context_.location.line())
            .arg(QString::fromStdString(context_.location.function_name()))
            .arg(message_);
    }

private:
    QString message_;
    ErrorSeverity severity_;
    ErrorCategory category_;
    ErrorContext context_;
    mutable std::string what_cache_;
};

// **Specific exception types**
class ValidationException : public UIException {
public:
    ValidationException(QString message, ErrorContext context = ErrorContext{})
        : UIException(std::move(message), ErrorSeverity::Error, ErrorCategory::Validation, std::move(context)) {}
};

class LayoutException : public UIException {
public:
    LayoutException(QString message, ErrorContext context = ErrorContext{})
        : UIException(std::move(message), ErrorSeverity::Error, ErrorCategory::Layout, std::move(context)) {}
};

class AnimationException : public UIException {
public:
    AnimationException(QString message, ErrorContext context = ErrorContext{})
        : UIException(std::move(message), ErrorSeverity::Error, ErrorCategory::Animation, std::move(context)) {}
};

class ThemeException : public UIException {
public:
    ThemeException(QString message, ErrorContext context = ErrorContext{})
        : UIException(std::move(message), ErrorSeverity::Error, ErrorCategory::Theme, std::move(context)) {}
};

// **Result type for error handling without exceptions**
template<typename T, typename E = UIException>
using Result = std::optional<T>;

// **Error handler interface**
class IErrorHandler {
public:
    virtual ~IErrorHandler() = default;
    virtual void handleError(const UIException& error) = 0;
    virtual void handleError(ErrorSeverity severity, const QString& message, const ErrorContext& context) = 0;
};

// **Console error handler**
class ConsoleErrorHandler : public IErrorHandler {
public:
    void handleError(const UIException& error) override {
        qDebug() << "[" << severityToString(error.getSeverity()) << "]"
                 << categoryToString(error.getCategory()) << ":"
                 << error.getFormattedMessage();
    }

    void handleError(ErrorSeverity severity, const QString& message, const ErrorContext& context) override {
        qDebug() << "[" << severityToString(severity) << "]" << message;
    }

private:
    QString severityToString(ErrorSeverity severity) const {
        switch (severity) {
            case ErrorSeverity::Debug: return "DEBUG";
            case ErrorSeverity::Info: return "INFO";
            case ErrorSeverity::Warning: return "WARNING";
            case ErrorSeverity::Error: return "ERROR";
            case ErrorSeverity::Critical: return "CRITICAL";
            case ErrorSeverity::Fatal: return "FATAL";
        }
        return "UNKNOWN";
    }

    QString categoryToString(ErrorCategory category) const {
        switch (category) {
            case ErrorCategory::General: return "GENERAL";
            case ErrorCategory::UI: return "UI";
            case ErrorCategory::Layout: return "LAYOUT";
            case ErrorCategory::Animation: return "ANIMATION";
            case ErrorCategory::Theme: return "THEME";
            case ErrorCategory::Validation: return "VALIDATION";
            case ErrorCategory::Memory: return "MEMORY";
            case ErrorCategory::IO: return "IO";
            case ErrorCategory::Network: return "NETWORK";
            case ErrorCategory::Configuration: return "CONFIG";
        }
        return "UNKNOWN";
    }
};

// **File error handler**
class FileErrorHandler : public IErrorHandler {
public:
    explicit FileErrorHandler(const QString& filename) : filename_(filename) {}

    void handleError(const UIException& error) override {
        writeToFile(error.getFormattedMessage());
    }

    void handleError(ErrorSeverity severity, const QString& message, const ErrorContext& context) override {
        writeToFile(QString("[%1] %2").arg(severityToString(severity)).arg(message));
    }

private:
    QString filename_;
    std::mutex file_mutex_;

    void writeToFile(const QString& message) {
        std::lock_guard<std::mutex> lock(file_mutex_);
        std::ofstream file(filename_.toStdString(), std::ios::app);
        if (file.is_open()) {
            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);
            file << std::format("[{}] {}\n", std::ctime(&time_t), message.toStdString());
        }
    }

    QString severityToString(ErrorSeverity severity) const {
        switch (severity) {
            case ErrorSeverity::Debug: return "DEBUG";
            case ErrorSeverity::Info: return "INFO";
            case ErrorSeverity::Warning: return "WARNING";
            case ErrorSeverity::Error: return "ERROR";
            case ErrorSeverity::Critical: return "CRITICAL";
            case ErrorSeverity::Fatal: return "FATAL";
        }
        return "UNKNOWN";
    }
};

// **Error manager**
class ErrorManager {
public:
    static ErrorManager& instance() {
        static ErrorManager instance;
        return instance;
    }

    // **Add error handlers**
    void addHandler(std::unique_ptr<IErrorHandler> handler) {
        handlers_.push_back(std::move(handler));
    }

    // **Handle errors**
    void handleError(const UIException& error) {
        for (const auto& handler : handlers_) {
            handler->handleError(error);
        }
    }

    void handleError(ErrorSeverity severity, const QString& message,
                    const ErrorContext& context = ErrorContext{}) {
        for (const auto& handler : handlers_) {
            handler->handleError(severity, message, context);
        }
    }

    // **Log messages with different severity levels**
    void debug(const QString& message, const ErrorContext& context = ErrorContext{}) {
        handleError(ErrorSeverity::Debug, message, context);
    }

    void info(const QString& message, const ErrorContext& context = ErrorContext{}) {
        handleError(ErrorSeverity::Info, message, context);
    }

    void warning(const QString& message, const ErrorContext& context = ErrorContext{}) {
        handleError(ErrorSeverity::Warning, message, context);
    }

    void error(const QString& message, const ErrorContext& context = ErrorContext{}) {
        handleError(ErrorSeverity::Error, message, context);
    }

    void critical(const QString& message, const ErrorContext& context = ErrorContext{}) {
        handleError(ErrorSeverity::Critical, message, context);
    }

    void fatal(const QString& message, const ErrorContext& context = ErrorContext{}) {
        handleError(ErrorSeverity::Fatal, message, context);
    }

    // **Exception handling utilities**
    template<typename F>
    requires std::invocable<F> && (!std::is_void_v<std::invoke_result_t<F>>)
    auto safeExecute(F&& func, const QString& operation_name = "Unknown operation")
        -> Result<std::invoke_result_t<F>, UIException> {
        try {
            return std::invoke(std::forward<F>(func));
        } catch (const UIException& e) {
            handleError(e);
            return std::nullopt;
        } catch (const std::exception& e) {
            ErrorContext context;
            context.operation = operation_name;
            UIException ui_error(QString::fromStdString(e.what()), ErrorSeverity::Error, ErrorCategory::General, context);
            handleError(ui_error);
            return std::nullopt;
        } catch (...) {
            ErrorContext context;
            context.operation = operation_name;
            UIException ui_error("Unknown exception occurred", ErrorSeverity::Critical, ErrorCategory::General, context);
            handleError(ui_error);
            return std::nullopt;
        }
    }

    // **Specialization for void functions**
    template<typename F>
    requires std::invocable<F> && std::is_void_v<std::invoke_result_t<F>>
    bool safeExecute(F&& func, const QString& operation_name = "Unknown operation") {
        try {
            std::invoke(std::forward<F>(func));
            return true;
        } catch (const UIException& e) {
            handleError(e);
            return false;
        } catch (const std::exception& e) {
            ErrorContext context;
            context.operation = operation_name;
            UIException ui_error(QString::fromStdString(e.what()), ErrorSeverity::Error, ErrorCategory::General, context);
            handleError(ui_error);
            return false;
        } catch (...) {
            ErrorContext context;
            context.operation = operation_name;
            UIException ui_error("Unknown exception occurred", ErrorSeverity::Critical, ErrorCategory::General, context);
            handleError(ui_error);
            return false;
        }
    }

    // **Assertion with custom error handling**
    void uiAssert(bool condition, const QString& message,
                 ErrorSeverity severity = ErrorSeverity::Error,
                 std::source_location location = std::source_location::current()) {
        if (!condition) {
            ErrorContext context(location);
            handleError(severity, QString("Assertion failed: %1").arg(message), context);

            if (severity == ErrorSeverity::Fatal) {
                std::terminate();
            }
        }
    }

    // **Performance monitoring**
    template<typename F>
    requires std::invocable<F>
    auto measurePerformance(F&& func, const QString& operation_name) {
        auto start = std::chrono::high_resolution_clock::now();

        auto result = safeExecute(std::forward<F>(func), operation_name);

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        info(QString("Operation '%1' completed in %2ms").arg(operation_name).arg(duration.count()));

        return result;
    }

private:
    std::vector<std::unique_ptr<IErrorHandler>> handlers_;

    ErrorManager() {
        // Add default console handler
        addHandler(std::make_unique<ConsoleErrorHandler>());
    }
};

// **Utility functions and macros**
[[nodiscard]] inline ErrorManager& errorManager() {
    return ErrorManager::instance();
}

// **RAII error context setter**
class ErrorContextScope {
public:
    ErrorContextScope(const QString& component_name, const QString& operation)
        : previous_context_(current_context_) {
        current_context_.component_name = component_name;
        current_context_.operation = operation;
    }

    ~ErrorContextScope() {
        current_context_ = previous_context_;
    }

    static ErrorContext getCurrentContext() {
        return current_context_;
    }

private:
    static thread_local ErrorContext current_context_;
    ErrorContext previous_context_;
};

// thread_local variable defined in ErrorHandling.cpp

// **Convenience macros**
#define UI_ASSERT(condition, message) \
    DeclarativeUI::Core::ErrorHandling::errorManager().uiAssert(condition, message, \
        DeclarativeUI::Core::ErrorHandling::ErrorSeverity::Error)

#define UI_ASSERT_CRITICAL(condition, message) \
    DeclarativeUI::Core::ErrorHandling::errorManager().uiAssert(condition, message, \
        DeclarativeUI::Core::ErrorHandling::ErrorSeverity::Critical)

#define UI_LOG_DEBUG(message) \
    DeclarativeUI::Core::ErrorHandling::errorManager().debug(message)

#define UI_LOG_INFO(message) \
    DeclarativeUI::Core::ErrorHandling::errorManager().info(message)

#define UI_LOG_WARNING(message) \
    DeclarativeUI::Core::ErrorHandling::errorManager().warning(message)

#define UI_LOG_ERROR(message) \
    DeclarativeUI::Core::ErrorHandling::errorManager().error(message)

#define UI_SAFE_EXECUTE(func, operation) \
    DeclarativeUI::Core::ErrorHandling::errorManager().safeExecute(func, operation)

#define UI_MEASURE_PERFORMANCE(func, operation) \
    DeclarativeUI::Core::ErrorHandling::errorManager().measurePerformance(func, operation)

#define UI_ERROR_SCOPE(component, operation) \
    DeclarativeUI::Core::ErrorHandling::ErrorContextScope _error_scope(component, operation)

}  // namespace DeclarativeUI::Core::ErrorHandling
