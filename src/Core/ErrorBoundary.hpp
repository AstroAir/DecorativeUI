#pragma once

/**
 * @file ErrorBoundary.hpp
 * @brief Error boundary system for DeclarativeUI components
 *
 * This header provides a comprehensive error boundary system that can catch
 * and handle component errors gracefully, preventing application crashes and
 * providing fallback UI. The system integrates with the lifecycle management
 * and provides React-like error boundary functionality.
 */

#include <QLabel>
#include <QObject>
#include <QPushButton>
#include <QString>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <chrono>
#include <exception>
#include <functional>
#include <memory>
#include <vector>

#include "Lifecycle.hpp"
#include "UIElement.hpp"

namespace DeclarativeUI::Core {

// Forward declarations
template <typename WidgetType>
class DeclarativeBuilder;

/**
 * @brief Error information structure
 */
struct ErrorInfo {
    QString message;
    QString component_name;
    QString stack_trace;
    std::chrono::steady_clock::time_point timestamp;
    std::exception_ptr exception_ptr;

    ErrorInfo(const QString& msg, const QString& component = "Unknown")
        : message(msg),
          component_name(component),
          timestamp(std::chrono::steady_clock::now()) {}
};

/**
 * @brief Error recovery strategy enumeration
 */
enum class ErrorRecoveryStrategy {
    ShowFallback,  ///< Show fallback UI
    Retry,         ///< Attempt to retry the operation
    Ignore,        ///< Ignore the error and continue
    Propagate,     ///< Propagate error to parent boundary
    Restart        ///< Restart the component
};

/**
 * @brief Error boundary configuration
 */
struct ErrorBoundaryConfig {
    ErrorRecoveryStrategy strategy = ErrorRecoveryStrategy::ShowFallback;
    std::chrono::milliseconds retry_delay{1000};
    int max_retry_attempts = 3;
    bool log_errors = true;
    bool show_error_details = false;  // Only in debug builds
    std::function<std::unique_ptr<QWidget>(const ErrorInfo&)> fallback_factory;
    std::function<void(const ErrorInfo&)> error_reporter;
};

/**
 * @brief Error boundary component that catches and handles errors
 *
 * ErrorBoundary provides a safety net for component hierarchies, catching
 * errors that occur during rendering, lifecycle methods, or event handlers.
 * When an error is caught, it can display fallback UI, attempt recovery,
 * or propagate the error up the component tree.
 */
class ErrorBoundary : public UIElement {
    Q_OBJECT

public:
    explicit ErrorBoundary(QObject* parent = nullptr);
    ~ErrorBoundary() override = default;

    // **Configuration**
    void setConfig(const ErrorBoundaryConfig& config);
    const ErrorBoundaryConfig& getConfig() const { return config_; }

    // **Error handling**
    void catchError(const std::exception& e,
                    const QString& component_name = "Unknown");
    void catchError(const QString& error_message,
                    const QString& component_name = "Unknown");
    void catchError(std::exception_ptr eptr,
                    const QString& component_name = "Unknown");

    // **Recovery methods**
    void retry();
    void reset();
    void showFallback();

    // **Child management**
    void setChild(std::unique_ptr<QWidget> child);
    void setChildFactory(std::function<std::unique_ptr<QWidget>()> factory);
    QWidget* getChild() const { return child_widget_; }

    // **Error state queries**
    bool hasError() const { return has_error_; }
    const ErrorInfo& getLastError() const { return last_error_; }
    const std::vector<ErrorInfo>& getErrorHistory() const {
        return error_history_;
    }

    // **Statistics**
    struct ErrorStats {
        int total_errors = 0;
        int recovered_errors = 0;
        int fallback_displays = 0;
        int retry_attempts = 0;
        std::chrono::steady_clock::time_point last_error_time;
    };

    const ErrorStats& getStats() const { return stats_; }

    // **UIElement implementation**
    void initialize() override;

signals:
    void errorCaught(const ErrorInfo& error);
    void errorRecovered();
    void fallbackShown();
    void retryAttempted(int attempt);

private slots:
    void onRetryTimer();
    void onChildDestroyed();

private:
    // **Configuration**
    ErrorBoundaryConfig config_;

    // **Error state**
    bool has_error_ = false;
    ErrorInfo last_error_{"No error", "ErrorBoundary"};
    std::vector<ErrorInfo> error_history_;

    // **Child management**
    QWidget* child_widget_ = nullptr;
    std::unique_ptr<QWidget> fallback_widget_;
    std::function<std::unique_ptr<QWidget>()> child_factory_;

    // **Recovery state**
    int current_retry_attempt_ = 0;
    QTimer* retry_timer_ = nullptr;

    // **Statistics**
    ErrorStats stats_;

    // **Internal methods**
    void handleError(const ErrorInfo& error);
    void executeRecoveryStrategy(const ErrorInfo& error);
    void createFallbackWidget(const ErrorInfo& error);
    void createDefaultFallback(const ErrorInfo& error);
    void switchToFallback();
    void switchToChild();
    void logError(const ErrorInfo& error);
    void updateStats(const ErrorInfo& error);
    QString extractStackTrace(std::exception_ptr eptr);
};

/**
 * @brief Error boundary builder for fluent API
 *
 * Provides a fluent interface for creating and configuring error boundaries
 * that integrates seamlessly with DeclarativeBuilder.
 */
class ErrorBoundaryBuilder {
public:
    ErrorBoundaryBuilder();

    // **Configuration methods**
    ErrorBoundaryBuilder& strategy(ErrorRecoveryStrategy strategy);
    ErrorBoundaryBuilder& retryDelay(std::chrono::milliseconds delay);
    ErrorBoundaryBuilder& maxRetries(int max_attempts);
    ErrorBoundaryBuilder& logErrors(bool enable = true);
    ErrorBoundaryBuilder& showErrorDetails(bool show = true);

    // **Fallback UI configuration**
    ErrorBoundaryBuilder& fallback(
        std::function<std::unique_ptr<QWidget>(const ErrorInfo&)> factory);
    ErrorBoundaryBuilder& simpleFallback(const QString& message);
    ErrorBoundaryBuilder& customFallback(std::unique_ptr<QWidget> widget);

    // **Error reporting**
    ErrorBoundaryBuilder& onError(
        std::function<void(const ErrorInfo&)> reporter);

    // **Child configuration**
    ErrorBoundaryBuilder& child(
        std::function<std::unique_ptr<QWidget>()> factory);
    ErrorBoundaryBuilder& child(std::unique_ptr<QWidget> widget);

    template <typename WidgetType>
    ErrorBoundaryBuilder& child(
        std::function<void(DeclarativeBuilder<WidgetType>&)> config);

    // **Build method**
    std::unique_ptr<ErrorBoundary> build();

private:
    ErrorBoundaryConfig config_;
    std::function<std::unique_ptr<QWidget>()> child_factory_;
    std::unique_ptr<QWidget> child_widget_;
};

/**
 * @brief Global error boundary manager
 *
 * Manages global error handling and provides utilities for error boundary
 * integration throughout the application.
 */
class ErrorBoundaryManager : public QObject {
    Q_OBJECT

public:
    static ErrorBoundaryManager& instance();

    // **Global error handling**
    void setGlobalErrorHandler(std::function<void(const ErrorInfo&)> handler);
    void reportError(const ErrorInfo& error);

    // **Error boundary registration**
    void registerErrorBoundary(ErrorBoundary* boundary);
    void unregisterErrorBoundary(ErrorBoundary* boundary);

    // **Global statistics**
    struct GlobalErrorStats {
        int total_boundaries = 0;
        int active_boundaries = 0;
        int total_errors_caught = 0;
        int total_recoveries = 0;
        std::chrono::steady_clock::time_point last_error_time;
    };

    const GlobalErrorStats& getGlobalStats() const { return global_stats_; }

    // **Configuration**
    void setDefaultConfig(const ErrorBoundaryConfig& config);
    const ErrorBoundaryConfig& getDefaultConfig() const {
        return default_config_;
    }

signals:
    void globalErrorReported(const ErrorInfo& error);
    void errorBoundaryRegistered(ErrorBoundary* boundary);
    void errorBoundaryUnregistered(ErrorBoundary* boundary);

private:
    explicit ErrorBoundaryManager(QObject* parent = nullptr);

    std::function<void(const ErrorInfo&)> global_error_handler_;
    std::vector<ErrorBoundary*> registered_boundaries_;
    ErrorBoundaryConfig default_config_;
    GlobalErrorStats global_stats_;
};

/**
 * @brief Convenience functions for creating error boundaries
 */
namespace ErrorBoundaries {

// **Quick error boundary creation**
std::unique_ptr<ErrorBoundary> create(std::unique_ptr<QWidget> child);
std::unique_ptr<ErrorBoundary> createWithFallback(
    std::unique_ptr<QWidget> child, const QString& fallback_message);

// **Error boundary wrapper for existing widgets**
std::unique_ptr<QWidget> wrap(std::unique_ptr<QWidget> widget,
                              const ErrorBoundaryConfig& config = {});

// **Integration with DeclarativeBuilder**
template <typename WidgetType>
ErrorBoundaryBuilder createFor(
    std::function<void(DeclarativeBuilder<WidgetType>&)> config);

}  // namespace ErrorBoundaries

}  // namespace DeclarativeUI::Core
