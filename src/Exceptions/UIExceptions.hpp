// Exceptions/UIExceptions.hpp
#pragma once

#include <QString>
#include <QStringList>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace DeclarativeUI::Exceptions {

// **Base exception class for all UI-related exceptions**
class UIException : public std::exception {
public:
    explicit UIException(std::string message) : message_(std::move(message)) {}

    UIException(std::string message, std::string context)
        : message_(std::move(message)), context_(std::move(context)) {}

    UIException(std::string message, std::string context,
                std::string suggestion)
        : message_(std::move(message)),
          context_(std::move(context)),
          suggestion_(std::move(suggestion)) {}

    [[nodiscard]] const char *what() const noexcept override {
        if (formatted_message_.empty()) {
            formatted_message_ = formatMessage();
        }
        return formatted_message_.c_str();
    }

    [[nodiscard]] const std::string &getMessage() const noexcept {
        return message_;
    }
    [[nodiscard]] const std::string &getContext() const noexcept {
        return context_;
    }
    [[nodiscard]] const std::string &getSuggestion() const noexcept {
        return suggestion_;
    }

    // **Add additional context information**
    UIException &withContext(const std::string &context) {
        context_ = context;
        formatted_message_.clear();  // Clear cached message
        return *this;
    }

    UIException &withSuggestion(const std::string &suggestion) {
        suggestion_ = suggestion;
        formatted_message_.clear();  // Clear cached message
        return *this;
    }

protected:
    std::string message_;
    std::string context_;
    std::string suggestion_;
    mutable std::string formatted_message_;  // Cached formatted message

    [[nodiscard]] virtual std::string formatMessage() const {
        std::string result = message_;

        if (!context_.empty()) {
            result += " [Context: " + context_ + "]";
        }

        if (!suggestion_.empty()) {
            result += " [Suggestion: " + suggestion_ + "]";
        }

        return result;
    }
};

// **Component creation and lifecycle exceptions**
class ComponentCreationException : public UIException {
public:
    explicit ComponentCreationException(const std::string &component_type)
        : UIException("Failed to create component",
                      "Component type: " + component_type),
          component_type_(component_type) {}

    ComponentCreationException(const std::string &component_type,
                               const std::string &reason)
        : UIException("Failed to create component: " + reason,
                      "Component type: " + component_type),
          component_type_(component_type) {}

    [[nodiscard]] const std::string &getComponentType() const noexcept {
        return component_type_;
    }

private:
    std::string component_type_;
};

class ComponentRegistrationException : public UIException {
public:
    explicit ComponentRegistrationException(const std::string &component_name)
        : UIException("Component registration failed",
                      "Component: " + component_name),
          component_name_(component_name) {}

    ComponentRegistrationException(const std::string &component_name,
                                   const std::string &reason)
        : UIException("Component registration failed: " + reason,
                      "Component: " + component_name),
          component_name_(component_name) {}

    [[nodiscard]] const std::string &getComponentName() const noexcept {
        return component_name_;
    }

private:
    std::string component_name_;
};

// **Property binding and state management exceptions**
class PropertyBindingException : public UIException {
public:
    explicit PropertyBindingException(const std::string &property_name)
        : UIException("Property binding failed", "Property: " + property_name),
          property_name_(property_name) {}

    PropertyBindingException(const std::string &property_name,
                             const std::string &reason)
        : UIException("Property binding failed: " + reason,
                      "Property: " + property_name),
          property_name_(property_name) {}

    [[nodiscard]] const std::string &getPropertyName() const noexcept {
        return property_name_;
    }

private:
    std::string property_name_;
};

class StateManagementException : public UIException {
public:
    explicit StateManagementException(const std::string &state_key)
        : UIException("State management error", "State key: " + state_key),
          state_key_(state_key) {}

    StateManagementException(const std::string &state_key,
                             const std::string &reason)
        : UIException("State management error: " + reason,
                      "State key: " + state_key),
          state_key_(state_key) {}

    [[nodiscard]] const std::string &getStateKey() const noexcept {
        return state_key_;
    }

private:
    std::string state_key_;
};

// **Layout and positioning exceptions**
class LayoutException : public UIException {
public:
    explicit LayoutException(const std::string &layout_error)
        : UIException("Layout error: " + layout_error) {}

    LayoutException(const std::string &layout_type,
                    const std::string &error_details)
        : UIException("Layout error: " + error_details,
                      "Layout type: " + layout_type),
          layout_type_(layout_type) {}

    [[nodiscard]] const std::string &getLayoutType() const noexcept {
        return layout_type_;
    }

private:
    std::string layout_type_;
};

// **JSON parsing and processing exceptions**
class JSONParsingException : public UIException {
public:
    explicit JSONParsingException(const std::string &file_path,
                                  const std::string &error)
        : UIException("JSON parsing error: " + error, "File: " + file_path),
          file_path_(file_path),
          line_number_(-1),
          column_number_(-1) {}

    JSONParsingException(const std::string &file_path, const std::string &error,
                         int line, int column)
        : UIException("JSON parsing error: " + error,
                      "File: " + file_path + ", Line: " + std::to_string(line) +
                          ", Column: " + std::to_string(column)),
          file_path_(file_path),
          line_number_(line),
          column_number_(column) {}

    [[nodiscard]] const std::string &getFilePath() const noexcept {
        return file_path_;
    }
    [[nodiscard]] int getLineNumber() const noexcept { return line_number_; }
    [[nodiscard]] int getColumnNumber() const noexcept {
        return column_number_;
    }
    [[nodiscard]] bool hasLineInfo() const noexcept {
        return line_number_ >= 0;
    }

private:
    std::string file_path_;
    int line_number_;
    int column_number_;
};

class JSONValidationException : public UIException {
public:
    explicit JSONValidationException(const std::string &validation_error)
        : UIException("JSON validation failed: " + validation_error) {}

    JSONValidationException(const std::string &validation_error,
                            const std::string &json_path)
        : UIException("JSON validation failed: " + validation_error,
                      "Path: " + json_path),
          json_path_(json_path) {}

    JSONValidationException(const std::vector<std::string> &validation_errors)
        : UIException("JSON validation failed with multiple errors"),
          validation_errors_(validation_errors) {}

    [[nodiscard]] const std::string &getJsonPath() const noexcept {
        return json_path_;
    }
    [[nodiscard]] const std::vector<std::string> &getValidationErrors()
        const noexcept {
        return validation_errors_;
    }
    [[nodiscard]] bool hasMultipleErrors() const noexcept {
        return !validation_errors_.empty();
    }

protected:
    [[nodiscard]] std::string formatMessage() const override {
        std::string result = message_;

        if (!context_.empty()) {
            result += " [" + context_ + "]";
        }

        if (hasMultipleErrors()) {
            result += "\nValidation errors:";
            for (size_t i = 0; i < validation_errors_.size(); ++i) {
                result += "\n  " + std::to_string(i + 1) + ". " +
                          validation_errors_[i];
            }
        }

        if (!suggestion_.empty()) {
            result += "\nSuggestion: " + suggestion_;
        }

        return result;
    }

private:
    std::string json_path_;
    std::vector<std::string> validation_errors_;
};

// **Hot reload system exceptions**
class HotReloadException : public UIException {
public:
    explicit HotReloadException(const std::string &reload_error)
        : UIException("Hot reload error: " + reload_error) {}

    HotReloadException(const std::string &file_path,
                       const std::string &reload_error)
        : UIException("Hot reload error: " + reload_error,
                      "File: " + file_path),
          file_path_(file_path) {}

    [[nodiscard]] const std::string &getFilePath() const noexcept {
        return file_path_;
    }

private:
    std::string file_path_;
};

class FileWatchException : public UIException {
public:
    explicit FileWatchException(const std::string &file_path)
        : UIException("File watch setup failed", "File: " + file_path),
          file_path_(file_path) {}

    FileWatchException(const std::string &file_path, const std::string &reason)
        : UIException("File watch setup failed: " + reason,
                      "File: " + file_path),
          file_path_(file_path) {}

    [[nodiscard]] const std::string &getFilePath() const noexcept {
        return file_path_;
    }

private:
    std::string file_path_;
};

// **Resource and asset loading exceptions**
class ResourceLoadException : public UIException {
public:
    explicit ResourceLoadException(const std::string &resource_path)
        : UIException("Resource loading failed", "Resource: " + resource_path),
          resource_path_(resource_path) {}

    ResourceLoadException(const std::string &resource_path,
                          const std::string &reason)
        : UIException("Resource loading failed: " + reason,
                      "Resource: " + resource_path),
          resource_path_(resource_path) {}

    [[nodiscard]] const std::string &getResourcePath() const noexcept {
        return resource_path_;
    }

private:
    std::string resource_path_;
};

// **Theme and styling exceptions**
class StyleException : public UIException {
public:
    explicit StyleException(const std::string &style_error)
        : UIException("Style application error: " + style_error) {}

    StyleException(const std::string &selector, const std::string &style_error)
        : UIException("Style application error: " + style_error,
                      "Selector: " + selector),
          selector_(selector) {}

    [[nodiscard]] const std::string &getSelector() const noexcept {
        return selector_;
    }

private:
    std::string selector_;
};

// **Event handling exceptions**
class EventHandlingException : public UIException {
public:
    explicit EventHandlingException(const std::string &event_name)
        : UIException("Event handling failed", "Event: " + event_name),
          event_name_(event_name) {}

    EventHandlingException(const std::string &event_name,
                           const std::string &reason)
        : UIException("Event handling failed: " + reason,
                      "Event: " + event_name),
          event_name_(event_name) {}

    [[nodiscard]] const std::string &getEventName() const noexcept {
        return event_name_;
    }

private:
    std::string event_name_;
};

// **Configuration and initialization exceptions**
class ConfigurationException : public UIException {
public:
    explicit ConfigurationException(const std::string &config_error)
        : UIException("Configuration error: " + config_error) {}

    ConfigurationException(const std::string &config_key,
                           const std::string &config_error)
        : UIException("Configuration error: " + config_error,
                      "Key: " + config_key),
          config_key_(config_key) {}

    [[nodiscard]] const std::string &getConfigKey() const noexcept {
        return config_key_;
    }

private:
    std::string config_key_;
};

class InitializationException : public UIException {
public:
    explicit InitializationException(const std::string &component_name)
        : UIException("Initialization failed", "Component: " + component_name),
          component_name_(component_name) {}

    InitializationException(const std::string &component_name,
                            const std::string &reason)
        : UIException("Initialization failed: " + reason,
                      "Component: " + component_name),
          component_name_(component_name) {}

    [[nodiscard]] const std::string &getComponentName() const noexcept {
        return component_name_;
    }

private:
    std::string component_name_;
};

// **Validation and schema exceptions**
class SchemaValidationException : public UIException {
public:
    explicit SchemaValidationException(const std::string &validation_error)
        : UIException("Schema validation failed: " + validation_error) {}

    SchemaValidationException(const std::string &schema_path,
                              const std::string &validation_error)
        : UIException("Schema validation failed: " + validation_error,
                      "Schema: " + schema_path),
          schema_path_(schema_path) {}

    [[nodiscard]] const std::string &getSchemaPath() const noexcept {
        return schema_path_;
    }

private:
    std::string schema_path_;
};

// **Runtime and execution exceptions**
class RuntimeException : public UIException {
public:
    explicit RuntimeException(const std::string &runtime_error)
        : UIException("Runtime error: " + runtime_error) {}

    RuntimeException(const std::string &operation,
                     const std::string &runtime_error)
        : UIException("Runtime error: " + runtime_error,
                      "Operation: " + operation),
          operation_(operation) {}

    [[nodiscard]] const std::string &getOperation() const noexcept {
        return operation_;
    }

private:
    std::string operation_;
};

// **Memory and resource management exceptions**
class ResourceManagementException : public UIException {
public:
    explicit ResourceManagementException(const std::string &resource_error)
        : UIException("Resource management error: " + resource_error) {}

    ResourceManagementException(const std::string &resource_type,
                                const std::string &resource_error)
        : UIException("Resource management error: " + resource_error,
                      "Resource type: " + resource_type),
          resource_type_(resource_type) {}

    [[nodiscard]] const std::string &getResourceType() const noexcept {
        return resource_type_;
    }

private:
    std::string resource_type_;
};

// **Threading and concurrency exceptions**
class ConcurrencyException : public UIException {
public:
    explicit ConcurrencyException(const std::string &concurrency_error)
        : UIException("Concurrency error: " + concurrency_error) {}

    ConcurrencyException(const std::string &thread_context,
                         const std::string &concurrency_error)
        : UIException("Concurrency error: " + concurrency_error,
                      "Thread context: " + thread_context),
          thread_context_(thread_context) {}

    [[nodiscard]] const std::string &getThreadContext() const noexcept {
        return thread_context_;
    }

private:
    std::string thread_context_;
};

// **Network and external resource exceptions**
class NetworkException : public UIException {
public:
    explicit NetworkException(const std::string &network_error)
        : UIException("Network error: " + network_error) {}

    NetworkException(const std::string &url, const std::string &network_error)
        : UIException("Network error: " + network_error, "URL: " + url),
          url_(url) {}

    NetworkException(const std::string &url, int status_code,
                     const std::string &network_error)
        : UIException(
              "Network error: " + network_error,
              "URL: " + url + ", Status: " + std::to_string(status_code)),
          url_(url),
          status_code_(status_code) {}

    [[nodiscard]] const std::string &getUrl() const noexcept { return url_; }
    [[nodiscard]] int getStatusCode() const noexcept { return status_code_; }
    [[nodiscard]] bool hasStatusCode() const noexcept {
        return status_code_ >= 0;
    }

private:
    std::string url_;
    int status_code_ = -1;
};

// **Plugin and extension exceptions**
class PluginException : public UIException {
public:
    explicit PluginException(const std::string &plugin_name)
        : UIException("Plugin error", "Plugin: " + plugin_name),
          plugin_name_(plugin_name) {}

    PluginException(const std::string &plugin_name,
                    const std::string &plugin_error)
        : UIException("Plugin error: " + plugin_error,
                      "Plugin: " + plugin_name),
          plugin_name_(plugin_name) {}

    [[nodiscard]] const std::string &getPluginName() const noexcept {
        return plugin_name_;
    }

private:
    std::string plugin_name_;
};

// **Utility exception handling functions**
namespace ExceptionUtils {

// **Exception chaining and aggregation**
class ExceptionChain : public UIException {
public:
    explicit ExceptionChain(const std::string &primary_message)
        : UIException(primary_message) {}

    ExceptionChain &addException(std::unique_ptr<UIException> exception) {
        chained_exceptions_.push_back(std::move(exception));
        formatted_message_.clear();  // Clear cached message
        return *this;
    }

    ExceptionChain &addException(const UIException &exception) {
        chained_exceptions_.push_back(std::make_unique<UIException>(exception));
        formatted_message_.clear();  // Clear cached message
        return *this;
    }

    [[nodiscard]] const std::vector<std::unique_ptr<UIException>> &
    getChainedExceptions() const noexcept {
        return chained_exceptions_;
    }

    [[nodiscard]] size_t getExceptionCount() const noexcept {
        return chained_exceptions_.size();
    }

protected:
    [[nodiscard]] std::string formatMessage() const override {
        std::string result = UIException::formatMessage();

        if (!chained_exceptions_.empty()) {
            result += "\nChained exceptions:";
            for (size_t i = 0; i < chained_exceptions_.size(); ++i) {
                result += "\n  " + std::to_string(i + 1) + ". " +
                          chained_exceptions_[i]->what();
            }
        }

        return result;
    }

private:
    std::vector<std::unique_ptr<UIException>> chained_exceptions_;
};

// **Exception creation helpers**
template <typename ExceptionType, typename... Args>
[[nodiscard]] std::unique_ptr<ExceptionType> createException(Args &&...args) {
    static_assert(std::is_base_of_v<UIException, ExceptionType>,
                  "ExceptionType must derive from UIException");
    return std::make_unique<ExceptionType>(std::forward<Args>(args)...);
}

// **Exception handling utilities**
[[nodiscard]] std::string formatExceptionDetails(const UIException &exception);
[[nodiscard]] std::string getExceptionHierarchy(
    const std::exception &exception);
[[nodiscard]] QStringList getExceptionMessages(const UIException &exception);

// **Exception logging and reporting**
void logException(const UIException &exception,
                  const std::string &severity = "ERROR");
void reportException(const UIException &exception,
                     const std::string &context = "");

// **Exception recovery and fallback**
template <typename T>
[[nodiscard]] std::optional<T> safeExecute(
    std::function<T()> operation,
    std::function<void(const UIException &)> error_handler = nullptr) {
    try {
        return operation();
    } catch (const UIException &e) {
        if (error_handler) {
            error_handler(e);
        }
        return std::nullopt;
    } catch (const std::exception &e) {
        UIException ui_exception("Unexpected exception: " +
                                 std::string(e.what()));
        if (error_handler) {
            error_handler(ui_exception);
        }
        return std::nullopt;
    }
}

// **Exception validation**
[[nodiscard]] bool isRecoverableException(
    const UIException &exception) noexcept;
[[nodiscard]] bool isCriticalException(const UIException &exception) noexcept;
[[nodiscard]] std::string getExceptionCategory(
    const UIException &exception) noexcept;

}  // namespace ExceptionUtils

// **Exception macros for convenient throwing with context**
#define THROW_UI_EXCEPTION(ExceptionType, ...) \
    throw DeclarativeUI::Exceptions::ExceptionType(__VA_ARGS__)

#define THROW_UI_EXCEPTION_WITH_CONTEXT(ExceptionType, context, ...) \
    throw DeclarativeUI::Exceptions::ExceptionType(__VA_ARGS__)      \
        .withContext(context)

#define THROW_UI_EXCEPTION_WITH_SUGGESTION(ExceptionType, suggestion, ...) \
    throw DeclarativeUI::Exceptions::ExceptionType(__VA_ARGS__)            \
        .withSuggestion(suggestion)

#define SAFE_UI_EXECUTE(operation, error_handler)                     \
    DeclarativeUI::Exceptions::ExceptionUtils::safeExecute(operation, \
                                                           error_handler)

// **Exception handling in Qt context**
#define HANDLE_UI_EXCEPTION_QT(exception)                             \
    do {                                                              \
        QString error_msg = QString::fromStdString(exception.what()); \
        qCritical() << "UIException:" << error_msg;                   \
    } while (0)

}  // namespace DeclarativeUI::Exceptions
