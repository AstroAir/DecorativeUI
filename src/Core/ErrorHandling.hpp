// Core/ErrorHandling.hpp
#pragma once

/**
 * @file ErrorHandling.hpp
 * @brief Rich error and diagnostic utilities for DeclarativeUI core components.
 *
 * This header provides:
 *  - typed severity and category enums for classifying problems,
 *  - an ErrorContext type that carries source location, timestamp and metadata,
 *  - UIException base class and several specialized exception types,
 *  - a lightweight Result alias for non-exception error returns,
 *  - an IErrorHandler interface and two built-in handlers (console + file),
 *  - a singleton ErrorManager coordinating handlers and providing safe-execute
 *    wrappers, assertions, and simple performance measurement helpers,
 *  - an RAII ErrorContextScope to temporarily set contextual information,
 *  - a set of convenience macros for logging, assertions and safe execution.
 *
 * All public declarations include detailed doxygen comments describing intended
 * usage, threading considerations and examples where appropriate.
 */

#include <QDebug>
#include <QLoggingCategory>
#include <QString>

#include <chrono>
#include <concepts>
#include <exception>
#include <format>
#include <fstream>
#include <functional>
#include <memory>
#include <mutex>
#include <source_location>
#include <string>
#include <vector>

namespace DeclarativeUI::Core::ErrorHandling {

/**
 * @enum ErrorSeverity
 * @brief Severity levels used to classify logged errors and diagnostics.
 *
 * Typical usage:
 *  - Debug: verbose diagnostics for development only.
 *  - Info: normal runtime events worth recording.
 *  - Warning: recoverable issues that may indicate misconfiguration.
 *  - Error: definite problems that prevent an operation from completing.
 *  - Critical: serious conditions that require immediate attention.
 *  - Fatal: unrecoverable conditions where termination is expected.
 */
enum class ErrorSeverity { Debug, Info, Warning, Error, Critical, Fatal };

/**
 * @enum ErrorCategory
 * @brief Logical categories to group errors by subsystem.
 *
 * Categories help route, filter and aggregate reported problems. Add new
 * categories as system components grow.
 */
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

/**
 * @struct ErrorContext
 * @brief Contextual metadata attached to an error or log entry.
 *
 * ErrorContext bundles:
 *  - source location (file/line/function) captured via std::source_location,
 *  - timestamp captured at construction,
 *  - a human-readable component name (e.g. "ThemeEngine"),
 *  - an operation string describing the action being performed,
 *  - an extensible vector of key/value pairs for additional diagnostic data.
 *
 * Construct with default parameter to automatically capture the call site:
 * @code
 * ErrorContext ctx = ErrorContext{};
 * @endcode
 *
 * Thread-safety:
 *  - Instances are plain value types. When used across threads synchronize
 *    access externally.
 */
struct ErrorContext {
    std::source_location
        location;  ///< Source file/line/function of error origin.
    std::chrono::system_clock::time_point
        timestamp;           ///< Time when context was created.
    QString component_name;  ///< Logical component that raised the error.
    QString operation;  ///< Short description of the operation being performed.
    std::vector<std::pair<QString, QString>>
        additional_data;  ///< Arbitrary key/value diagnostic pairs.

    /**
     * @brief Construct an ErrorContext capturing the current source location.
     * @param loc source location (default: std::source_location::current()).
     */
    ErrorContext(std::source_location loc = std::source_location::current())
        : location(loc), timestamp(std::chrono::system_clock::now()) {}
};

/**
 * @class UIException
 * @brief Root exception type used by DeclarativeUI to carry rich diagnostic
 * info.
 *
 * UIException stores:
 *  - a QString message,
 *  - an ErrorSeverity and ErrorCategory,
 *  - an ErrorContext describing where and when the error occurred.
 *
 * Use derived types (ValidationException, LayoutException, etc.) for clearer
 * intent when throwing. The what() method satisfies std::exception for
 * interoperability with libraries that catch std::exception.
 *
 * Example:
 * @code
 * throw UIException("Failed to load theme", ErrorSeverity::Error,
 * ErrorCategory::Theme);
 * @endcode
 *
 * Thread-safety:
 *  - Exception objects are typically copied when thrown; members are value
 *    types (QString is implicitly shared) and safe to copy between threads.
 */
class UIException : public std::exception {
public:
    /**
     * @brief Construct a UIException with message, severity, category and
     * context.
     * @param message human-readable error message (QString).
     * @param severity severity classification (default: Error).
     * @param category logical error category (default: General).
     * @param context additional contextual information (default: captured
     * location).
     */
    UIException(QString message, ErrorSeverity severity = ErrorSeverity::Error,
                ErrorCategory category = ErrorCategory::General,
                ErrorContext context = ErrorContext{})
        : message_(std::move(message)),
          severity_(severity),
          category_(category),
          context_(std::move(context)) {}

    /**
     * @brief Return a C-string description for compatibility with
     * std::exception.
     * @note The returned pointer remains valid for the lifetime of the
     * exception object.
     */
    [[nodiscard]] const char* what() const noexcept override {
        if (what_cache_.empty()) {
            what_cache_ = message_.toStdString();
        }
        return what_cache_.c_str();
    }

    [[nodiscard]] const QString& getMessage() const noexcept {
        return message_;
    }
    [[nodiscard]] ErrorSeverity getSeverity() const noexcept {
        return severity_;
    }
    [[nodiscard]] ErrorCategory getCategory() const noexcept {
        return category_;
    }
    [[nodiscard]] const ErrorContext& getContext() const noexcept {
        return context_;
    }

    /**
     * @brief Produce a short formatted message containing file/line/function
     * and text.
     * @return QString containing location and message suitable for logging.
     */
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
    mutable std::string what_cache_;  ///< Cached what() conversion.
};

/**
 * @class ValidationException
 * @brief Specialized exception representing validation failures.
 *
 * Use when input or configuration validation fails. Semantically maps to
 * ErrorCategory::Validation and ErrorSeverity::Error.
 */
class ValidationException : public UIException {
public:
    ValidationException(QString message, ErrorContext context = ErrorContext{})
        : UIException(std::move(message), ErrorSeverity::Error,
                      ErrorCategory::Validation, std::move(context)) {}
};

/**
 * @class LayoutException
 * @brief Exception to indicate layout computation or constraint failures.
 *
 * Use to signal problems encountered during UI layout processing.
 */
class LayoutException : public UIException {
public:
    LayoutException(QString message, ErrorContext context = ErrorContext{})
        : UIException(std::move(message), ErrorSeverity::Error,
                      ErrorCategory::Layout, std::move(context)) {}
};

/**
 * @class AnimationException
 * @brief Exception thrown when animation subsystem encounters an error.
 */
class AnimationException : public UIException {
public:
    AnimationException(QString message, ErrorContext context = ErrorContext{})
        : UIException(std::move(message), ErrorSeverity::Error,
                      ErrorCategory::Animation, std::move(context)) {}
};

/**
 * @class ThemeException
 * @brief Exception used for theme loading/processing failures.
 */
class ThemeException : public UIException {
public:
    ThemeException(QString message, ErrorContext context = ErrorContext{})
        : UIException(std::move(message), ErrorSeverity::Error,
                      ErrorCategory::Theme, std::move(context)) {}
};

/**
 * @brief Result alias for functions that prefer optional return semantics.
 *
 * Template parameter T represents success value. When std::nullopt is
 * returned an error was reported via ErrorManager (or handled separately).
 *
 * E parameter reserved for future typed error payloads (currently unused).
 */
template <typename T, typename E = UIException>
using Result = std::optional<T>;

/**
 * @interface IErrorHandler
 * @brief Abstract interface for pluggable error handlers.
 *
 * Implementations receive UIException instances or simple
 * severity/message/context triples. Handlers are owned by ErrorManager and
 * invoked sequentially on the thread where the error is reported.
 *
 * Implementations should be careful about reentrancy: handlers may be invoked
 * while other subsystems are operating. Use internal synchronization where
 * necessary (e.g. file writes).
 */
class IErrorHandler {
public:
    virtual ~IErrorHandler() = default;

    /**
     * @brief Handle a fully-formed UIException.
     * @param error reference to the exception being reported.
     */
    virtual void handleError(const UIException& error) = 0;

    /**
     * @brief Handle a simple severity/message/context triple.
     * @param severity classification level.
     * @param message textual message to log.
     * @param context optional contextual information.
     */
    virtual void handleError(ErrorSeverity severity, const QString& message,
                             const ErrorContext& context) = 0;
};

/**
 * @class ConsoleErrorHandler
 * @brief Simple handler that writes formatted messages to qDebug().
 *
 * Intended as the default development-time handler. Not suitable for heavy
 * production logging (use FileErrorHandler or integrate with the platform
 * logging subsystem instead).
 */
class ConsoleErrorHandler : public IErrorHandler {
public:
    void handleError(const UIException& error) override {
        qDebug() << "[" << severityToString(error.getSeverity()) << "]"
                 << categoryToString(error.getCategory()) << ":"
                 << error.getFormattedMessage();
    }

    void handleError(ErrorSeverity severity, const QString& message,
                     const ErrorContext& /*context*/) override {
        qDebug() << "[" << severityToString(severity) << "]" << message;
    }

private:
    /**
     * @brief Convert ErrorSeverity to an uppercase textual label.
     * @param severity severity enum value.
     * @return QString textual label.
     */
    QString severityToString(ErrorSeverity severity) const {
        switch (severity) {
            case ErrorSeverity::Debug:
                return "DEBUG";
            case ErrorSeverity::Info:
                return "INFO";
            case ErrorSeverity::Warning:
                return "WARNING";
            case ErrorSeverity::Error:
                return "ERROR";
            case ErrorSeverity::Critical:
                return "CRITICAL";
            case ErrorSeverity::Fatal:
                return "FATAL";
        }
        return "UNKNOWN";
    }

    /**
     * @brief Convert ErrorCategory to an uppercase textual label.
     * @param category category enum value.
     * @return QString textual label.
     */
    QString categoryToString(ErrorCategory category) const {
        switch (category) {
            case ErrorCategory::General:
                return "GENERAL";
            case ErrorCategory::UI:
                return "UI";
            case ErrorCategory::Layout:
                return "LAYOUT";
            case ErrorCategory::Animation:
                return "ANIMATION";
            case ErrorCategory::Theme:
                return "THEME";
            case ErrorCategory::Validation:
                return "VALIDATION";
            case ErrorCategory::Memory:
                return "MEMORY";
            case ErrorCategory::IO:
                return "IO";
            case ErrorCategory::Network:
                return "NETWORK";
            case ErrorCategory::Configuration:
                return "CONFIG";
        }
        return "UNKNOWN";
    }
};

/**
 * @class FileErrorHandler
 * @brief Handler that appends formatted messages to a file.
 *
 * The handler uses a mutex to protect concurrent file access. Messages are
 * appended using std::ofstream. Timestamps are written using std::ctime for
 * human readability.
 *
 * Construction:
 * @code
 * auto fileHandler = std::make_unique<FileErrorHandler>("errors.log");
 * errorManager().addHandler(std::move(fileHandler));
 * @endcode
 */
class FileErrorHandler : public IErrorHandler {
public:
    /**
     * @brief Construct a FileErrorHandler writing to the given file path.
     * @param filename path of the file to append logs to (QString).
     */
    explicit FileErrorHandler(const QString& filename) : filename_(filename) {}

    void handleError(const UIException& error) override {
        writeToFile(error.getFormattedMessage());
    }

    void handleError(ErrorSeverity severity, const QString& message,
                     const ErrorContext& /*context*/) override {
        writeToFile(
            QString("[%1] %2").arg(severityToString(severity)).arg(message));
    }

private:
    QString filename_;
    std::mutex file_mutex_;

    /**
     * @brief Append a message to the configured file in a thread-safe manner.
     * @param message text to append (no newline appended by caller).
     */
    void writeToFile(const QString& message) {
        std::lock_guard<std::mutex> lock(file_mutex_);
        std::ofstream file(filename_.toStdString(), std::ios::app);
        if (file.is_open()) {
            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);
            // std::ctime returns a newline-terminated string; format using
            // std::format
            file << std::format("[{}] {}\n", std::ctime(&time_t),
                                message.toStdString());
        }
    }

    QString severityToString(ErrorSeverity severity) const {
        switch (severity) {
            case ErrorSeverity::Debug:
                return "DEBUG";
            case ErrorSeverity::Info:
                return "INFO";
            case ErrorSeverity::Warning:
                return "WARNING";
            case ErrorSeverity::Error:
                return "ERROR";
            case ErrorSeverity::Critical:
                return "CRITICAL";
            case ErrorSeverity::Fatal:
                return "FATAL";
        }
        return "UNKNOWN";
    }
};

/**
 * @class ErrorManager
 * @brief Singleton coordinating error handlers, logging and safe execution.
 *
 * Responsibilities:
 *  - Maintain a list of IErrorHandler instances.
 *  - Provide convenience logging methods (debug/info/warning/error/etc.).
 *  - Offer safeExecute wrappers which catch exceptions, forward them to
 *    handlers and return Result or boolean success.
 *  - Provide uiAssert() to assert runtime conditions and optionally terminate.
 *  - Provide measurePerformance() that times an operation and logs duration.
 *
 * Thread-safety:
 *  - The internal handlers_ vector is mutated only via addHandler() which is
 *    typically called during initialization. If you add handlers at runtime
 *    from multiple threads, synchronize externally or extend this class with
 *    internal synchronization.
 */
class ErrorManager {
public:
    /**
     * @brief Access the global ErrorManager singleton.
     * @return reference to the singleton instance.
     */
    static ErrorManager& instance() {
        static ErrorManager instance;
        return instance;
    }

    /**
     * @brief Add a custom error handler.
     * @param handler unique_ptr to an IErrorHandler implementation.
     */
    void addHandler(std::unique_ptr<IErrorHandler> handler) {
        handlers_.push_back(std::move(handler));
    }

    /**
     * @brief Dispatch a full UIException to all registered handlers.
     * @param error exception to report.
     */
    void handleError(const UIException& error) {
        for (const auto& handler : handlers_) {
            handler->handleError(error);
        }
    }

    /**
     * @brief Dispatch a simple severity/message/context triple to handlers.
     * @param severity severity level.
     * @param message message text.
     * @param context optional context (default: captured location/time).
     */
    void handleError(ErrorSeverity severity, const QString& message,
                     const ErrorContext& context = ErrorContext{}) {
        for (const auto& handler : handlers_) {
            handler->handleError(severity, message, context);
        }
    }

    /* Convenience logging helpers forwarding to handleError(...) */

    void debug(const QString& message,
               const ErrorContext& context = ErrorContext{}) {
        handleError(ErrorSeverity::Debug, message, context);
    }

    void info(const QString& message,
              const ErrorContext& context = ErrorContext{}) {
        handleError(ErrorSeverity::Info, message, context);
    }

    void warning(const QString& message,
                 const ErrorContext& context = ErrorContext{}) {
        handleError(ErrorSeverity::Warning, message, context);
    }

    void error(const QString& message,
               const ErrorContext& context = ErrorContext{}) {
        handleError(ErrorSeverity::Error, message, context);
    }

    void critical(const QString& message,
                  const ErrorContext& context = ErrorContext{}) {
        handleError(ErrorSeverity::Critical, message, context);
    }

    void fatal(const QString& message,
               const ErrorContext& context = ErrorContext{}) {
        handleError(ErrorSeverity::Fatal, message, context);
    }

    /**
     * @brief Execute a callable safely and capture exceptions.
     *
     * This overload is for non-void returning callables. If the callable
     * completes successfully its return value is wrapped in std::optional and
     * returned. If an exception is thrown, it is forwarded to handlers and
     * std::nullopt is returned.
     *
     * @tparam F callable type returning a non-void result.
     * @param func callable to invoke.
     * @param operation_name textual name used in contextual messages.
     * @return Result containing the return value or std::nullopt on error.
     */
    template <typename F>
        requires std::invocable<F> && (!std::is_void_v<std::invoke_result_t<F>>)
    auto safeExecute(F&& func,
                     const QString& operation_name = "Unknown operation")
        -> Result<std::invoke_result_t<F>, UIException> {
        try {
            return std::invoke(std::forward<F>(func));
        } catch (const UIException& e) {
            handleError(e);
            return std::nullopt;
        } catch (const std::exception& e) {
            ErrorContext context;
            context.operation = operation_name;
            UIException ui_error(QString::fromStdString(e.what()),
                                 ErrorSeverity::Error, ErrorCategory::General,
                                 context);
            handleError(ui_error);
            return std::nullopt;
        } catch (...) {
            ErrorContext context;
            context.operation = operation_name;
            UIException ui_error("Unknown exception occurred",
                                 ErrorSeverity::Critical,
                                 ErrorCategory::General, context);
            handleError(ui_error);
            return std::nullopt;
        }
    }

    /**
     * @brief Execute a void callable safely and report success as bool.
     *
     * Returns true on success; on exception handlers are invoked and false is
     * returned.
     *
     * @tparam F callable type returning void.
     * @param func callable to invoke.
     * @param operation_name textual operation name for context.
     * @return true if func completed successfully, false otherwise.
     */
    template <typename F>
        requires std::invocable<F> && std::is_void_v<std::invoke_result_t<F>>
    bool safeExecute(F&& func,
                     const QString& operation_name = "Unknown operation") {
        try {
            std::invoke(std::forward<F>(func));
            return true;
        } catch (const UIException& e) {
            handleError(e);
            return false;
        } catch (const std::exception& e) {
            ErrorContext context;
            context.operation = operation_name;
            UIException ui_error(QString::fromStdString(e.what()),
                                 ErrorSeverity::Error, ErrorCategory::General,
                                 context);
            handleError(ui_error);
            return false;
        } catch (...) {
            ErrorContext context;
            context.operation = operation_name;
            UIException ui_error("Unknown exception occurred",
                                 ErrorSeverity::Critical,
                                 ErrorCategory::General, context);
            handleError(ui_error);
            return false;
        }
    }

    /**
     * @brief Runtime assertion helper which reports via the error subsystem.
     *
     * If condition is false an error is logged with the provided severity and
     * optionally the process is terminated if severity == Fatal.
     *
     * @param condition boolean expression expected to be true.
     * @param message human-readable assertion message.
     * @param severity severity to report when assertion fails (default: Error).
     * @param location captured source location (default: current call site).
     */
    void uiAssert(
        bool condition, const QString& message,
        ErrorSeverity severity = ErrorSeverity::Error,
        std::source_location location = std::source_location::current()) {
        if (!condition) {
            ErrorContext context(location);
            handleError(severity, QString("Assertion failed: %1").arg(message),
                        context);

            if (severity == ErrorSeverity::Fatal) {
                std::terminate();
            }
        }
    }

    /**
     * @brief Measure execution time of a callable and log the duration.
     *
     * The callable is executed via safeExecute so exceptions will be handled
     * by the configured handlers. The method logs a human-readable duration
     * message via info().
     *
     * @tparam F callable type.
     * @param func callable to measure.
     * @param operation_name textual name used in logs.
     * @return The result of safeExecute(func, operation_name).
     */
    template <typename F>
        requires std::invocable<F>
    auto measurePerformance(F&& func, const QString& operation_name) {
        auto start = std::chrono::high_resolution_clock::now();

        auto result = safeExecute(std::forward<F>(func), operation_name);

        auto end = std::chrono::high_resolution_clock::now();
        auto duration =
            std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        info(QString("Operation '%1' completed in %2ms")
                 .arg(operation_name)
                 .arg(duration.count()));

        return result;
    }

private:
    std::vector<std::unique_ptr<IErrorHandler>> handlers_;

    /**
     * @brief Private constructor sets up default handlers.
     *
     * By default a ConsoleErrorHandler is registered. Additional handlers
     * (for example a FileErrorHandler) can be added during initialization.
     */
    ErrorManager() {
        // Add default console handler
        addHandler(std::make_unique<ConsoleErrorHandler>());
    }
};

/**
 * @brief Convenience accessor to the global ErrorManager.
 * @return reference to the singleton ErrorManager.
 */
[[nodiscard]] inline ErrorManager& errorManager() {
    return ErrorManager::instance();
}

/**
 * @class ErrorContextScope
 * @brief RAII helper to temporarily set global per-thread ErrorContext fields.
 *
 * Use this scope to attach component and operation information to all calls
 * that consult ErrorContextScope::getCurrentContext() while the scope is
 * active. The previous context is restored when the scope is destroyed.
 *
 * Example:
 * @code
 * {
 *   UI_ERROR_SCOPE("ThemeEngine", "LoadTheme");
 *   // errors logged here will include component=ThemeEngine,
 * operation=LoadTheme
 * }
 * @endcode
 *
 * Note:
 *  - The current context is thread_local; each thread has its own context.
 *  - The actual thread_local variable is declared in ErrorHandling.cpp.
 */
class ErrorContextScope {
public:
    /**
     * @brief Create a scope setting component_name and operation.
     * @param component_name logical component identifier.
     * @param operation short operation description.
     */
    ErrorContextScope(const QString& component_name, const QString& operation)
        : previous_context_(current_context_) {
        current_context_.component_name = component_name;
        current_context_.operation = operation;
    }

    /**
     * @brief Restore the previously active context.
     */
    ~ErrorContextScope() { current_context_ = previous_context_; }

    /**
     * @brief Retrieve the current thread-local ErrorContext.
     * @return copy of the current ErrorContext.
     */
    static ErrorContext getCurrentContext() { return current_context_; }

private:
    static thread_local ErrorContext current_context_;
    ErrorContext previous_context_;
};

// thread_local variable defined in ErrorHandling.cpp

/* Convenience macros providing terse call-sites for common operations.
 *
 * These macros are intended for use in application code; they forward to the
 * global ErrorManager instance. Use them sparingly to keep call-sites explicit.
 */

#define UI_ASSERT(condition, message)                            \
    DeclarativeUI::Core::ErrorHandling::errorManager().uiAssert( \
        condition, message,                                      \
        DeclarativeUI::Core::ErrorHandling::ErrorSeverity::Error)

#define UI_ASSERT_CRITICAL(condition, message)                   \
    DeclarativeUI::Core::ErrorHandling::errorManager().uiAssert( \
        condition, message,                                      \
        DeclarativeUI::Core::ErrorHandling::ErrorSeverity::Critical)

#define UI_LOG_DEBUG(message) \
    DeclarativeUI::Core::ErrorHandling::errorManager().debug(message)

#define UI_LOG_INFO(message) \
    DeclarativeUI::Core::ErrorHandling::errorManager().info(message)

#define UI_LOG_WARNING(message) \
    DeclarativeUI::Core::ErrorHandling::errorManager().warning(message)

#define UI_LOG_ERROR(message) \
    DeclarativeUI::Core::ErrorHandling::errorManager().error(message)

#define UI_SAFE_EXECUTE(func, operation)                                 \
    DeclarativeUI::Core::ErrorHandling::errorManager().safeExecute(func, \
                                                                   operation)

#define UI_MEASURE_PERFORMANCE(func, operation)                            \
    DeclarativeUI::Core::ErrorHandling::errorManager().measurePerformance( \
        func, operation)

#define UI_ERROR_SCOPE(component, operation)                            \
    DeclarativeUI::Core::ErrorHandling::ErrorContextScope _error_scope( \
        component, operation)

}  // namespace DeclarativeUI::Core::ErrorHandling
