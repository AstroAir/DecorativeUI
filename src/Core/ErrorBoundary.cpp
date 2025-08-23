/**
 * @file ErrorBoundary.cpp
 * @brief Implementation of error boundary system for DeclarativeUI
 */

#include "ErrorBoundary.hpp"
#include "../Exceptions/UIExceptions.hpp"

#include <QApplication>
#include <QDebug>
#include <QHBoxLayout>
#include <QIcon>
#include <QMessageBox>
#include <QScrollArea>
#include <QStyle>
#include <QTextEdit>
#include <QVBoxLayout>
#include <algorithm>

namespace DeclarativeUI::Core {

ErrorBoundary::ErrorBoundary(QObject* parent) : UIElement(parent) {
    // Initialize retry timer
    retry_timer_ = new QTimer(this);
    retry_timer_->setSingleShot(true);
    connect(retry_timer_, &QTimer::timeout, this, &ErrorBoundary::onRetryTimer);

    // Set default configuration
    config_.fallback_factory =
        [](const ErrorInfo& error) -> std::unique_ptr<QWidget> {
        auto widget = std::make_unique<QWidget>();
        auto layout = new QVBoxLayout(widget.get());

        auto icon_label = new QLabel();
        icon_label->setPixmap(QApplication::style()
                                  ->standardIcon(QStyle::SP_MessageBoxCritical)
                                  .pixmap(48, 48));
        icon_label->setAlignment(Qt::AlignCenter);

        auto title_label = new QLabel("An error occurred");
        title_label->setStyleSheet(
            "font-weight: bold; font-size: 14px; color: #d32f2f;");
        title_label->setAlignment(Qt::AlignCenter);

        auto message_label = new QLabel(error.message);
        message_label->setWordWrap(true);
        message_label->setAlignment(Qt::AlignCenter);
        message_label->setStyleSheet("color: #666; margin: 10px;");

        layout->addWidget(icon_label);
        layout->addWidget(title_label);
        layout->addWidget(message_label);
        layout->setAlignment(Qt::AlignCenter);

        return widget;
    };

    qDebug() << "🛡️ ErrorBoundary created";
}

void ErrorBoundary::setConfig(const ErrorBoundaryConfig& config) {
    config_ = config;
    qDebug() << "🛡️ ErrorBoundary configuration updated";
}

void ErrorBoundary::catchError(const std::exception& e,
                               const QString& component_name) {
    ErrorInfo error(QString::fromStdString(e.what()), component_name);
    error.exception_ptr = std::current_exception();
    handleError(error);
}

void ErrorBoundary::catchError(const QString& error_message,
                               const QString& component_name) {
    ErrorInfo error(error_message, component_name);
    handleError(error);
}

void ErrorBoundary::catchError(std::exception_ptr eptr,
                               const QString& component_name) {
    try {
        if (eptr) {
            std::rethrow_exception(eptr);
        }
    } catch (const std::exception& e) {
        ErrorInfo error(QString::fromStdString(e.what()), component_name);
        error.exception_ptr = eptr;
        error.stack_trace = extractStackTrace(eptr);
        handleError(error);
    } catch (...) {
        ErrorInfo error("Unknown error", component_name);
        error.exception_ptr = eptr;
        handleError(error);
    }
}

void ErrorBoundary::retry() {
    if (!has_error_) {
        qDebug() << "🛡️ No error to retry";
        return;
    }

    if (current_retry_attempt_ >= config_.max_retry_attempts) {
        qWarning() << "🛡️ Maximum retry attempts reached";
        return;
    }

    current_retry_attempt_++;
    stats_.retry_attempts++;

    qDebug() << "🛡️ Retrying error recovery, attempt" << current_retry_attempt_;
    emit retryAttempted(current_retry_attempt_);

    try {
        if (child_factory_) {
            auto new_child = child_factory_();
            setChild(std::move(new_child));
            reset();
            stats_.recovered_errors++;
            emit errorRecovered();
        }
    } catch (const std::exception& e) {
        qWarning() << "🛡️ Retry failed:" << e.what();
        // Schedule another retry if within limits
        if (current_retry_attempt_ < config_.max_retry_attempts) {
            retry_timer_->start(config_.retry_delay);
        }
    }
}

void ErrorBoundary::reset() {
    has_error_ = false;
    current_retry_attempt_ = 0;
    retry_timer_->stop();

    if (child_widget_) {
        switchToChild();
    }

    qDebug() << "🛡️ ErrorBoundary reset";
}

void ErrorBoundary::showFallback() {
    if (!has_error_) {
        qDebug() << "🛡️ No error to show fallback for";
        return;
    }

    createFallbackWidget(last_error_);
    switchToFallback();
    stats_.fallback_displays++;
    emit fallbackShown();

    qDebug() << "🛡️ Fallback UI displayed";
}

void ErrorBoundary::setChild(std::unique_ptr<QWidget> child) {
    if (child_widget_) {
        disconnect(child_widget_, &QWidget::destroyed, this,
                   &ErrorBoundary::onChildDestroyed);
    }

    child_widget_ = child.release();

    if (child_widget_) {
        connect(child_widget_, &QWidget::destroyed, this,
                &ErrorBoundary::onChildDestroyed);
        child_widget_->setParent(getWidget());

        if (!has_error_) {
            switchToChild();
        }
    }
}

void ErrorBoundary::setChildFactory(
    std::function<std::unique_ptr<QWidget>()> factory) {
    child_factory_ = std::move(factory);
}

void ErrorBoundary::initialize() {
    auto container = std::make_unique<QWidget>();
    auto layout = new QVBoxLayout(container.get());
    layout->setContentsMargins(0, 0, 0, 0);

    setWidget(container.release());

    if (child_widget_) {
        switchToChild();
    }
}

void ErrorBoundary::onRetryTimer() { retry(); }

void ErrorBoundary::onChildDestroyed() {
    child_widget_ = nullptr;
    qDebug() << "🛡️ Child widget destroyed";
}

void ErrorBoundary::handleError(const ErrorInfo& error) {
    has_error_ = true;
    last_error_ = error;
    error_history_.push_back(error);

    // Limit error history size
    if (error_history_.size() > 100) {
        error_history_.erase(error_history_.begin());
    }

    updateStats(error);
    logError(error);

    emit errorCaught(error);

    // Report to global error handler
    ErrorBoundaryManager::instance().reportError(error);

    executeRecoveryStrategy(error);
}

void ErrorBoundary::executeRecoveryStrategy(const ErrorInfo& error) {
    switch (config_.strategy) {
        case ErrorRecoveryStrategy::ShowFallback:
            showFallback();
            break;

        case ErrorRecoveryStrategy::Retry:
            if (current_retry_attempt_ < config_.max_retry_attempts) {
                retry_timer_->start(config_.retry_delay);
            } else {
                showFallback();
            }
            break;

        case ErrorRecoveryStrategy::Ignore:
            qDebug() << "🛡️ Ignoring error as per strategy";
            break;

        case ErrorRecoveryStrategy::Propagate:
            qDebug() << "🛡️ Propagating error to parent";
            // In a real implementation, this would propagate to parent error
            // boundary
            throw Exceptions::UIException(error.message.toStdString());
            break;

        case ErrorRecoveryStrategy::Restart:
            reset();
            if (child_factory_) {
                try {
                    auto new_child = child_factory_();
                    setChild(std::move(new_child));
                } catch (const std::exception& e) {
                    qWarning() << "🛡️ Restart failed:" << e.what();
                    showFallback();
                }
            }
            break;
    }
}

void ErrorBoundary::createFallbackWidget(const ErrorInfo& error) {
    if (config_.fallback_factory) {
        fallback_widget_ = config_.fallback_factory(error);
    } else {
        createDefaultFallback(error);
    }

    if (fallback_widget_) {
        fallback_widget_->setParent(getWidget());
    }
}

void ErrorBoundary::createDefaultFallback(const ErrorInfo& error) {
    auto widget = std::make_unique<QWidget>();
    auto layout = new QVBoxLayout(widget.get());

    auto title = new QLabel("Something went wrong");
    title->setStyleSheet("font-weight: bold; color: red; font-size: 16px;");
    title->setAlignment(Qt::AlignCenter);

    auto message = new QLabel(error.message);
    message->setWordWrap(true);
    message->setAlignment(Qt::AlignCenter);

    auto retry_button = new QPushButton("Try Again");
    connect(retry_button, &QPushButton::clicked, this, &ErrorBoundary::retry);

    layout->addWidget(title);
    layout->addWidget(message);
    layout->addWidget(retry_button);
    layout->setAlignment(Qt::AlignCenter);

    fallback_widget_ = std::move(widget);
}

void ErrorBoundary::switchToFallback() {
    if (!getWidget() || !fallback_widget_)
        return;

    auto layout = getWidget()->layout();
    if (!layout)
        return;

    // Hide child widget
    if (child_widget_) {
        child_widget_->hide();
    }

    // Show fallback widget
    layout->addWidget(fallback_widget_.get());
    fallback_widget_->show();
}

void ErrorBoundary::switchToChild() {
    if (!getWidget() || !child_widget_)
        return;

    auto layout = getWidget()->layout();
    if (!layout)
        return;

    // Hide fallback widget
    if (fallback_widget_) {
        fallback_widget_->hide();
        layout->removeWidget(fallback_widget_.get());
    }

    // Show child widget
    layout->addWidget(child_widget_);
    child_widget_->show();
}

void ErrorBoundary::logError(const ErrorInfo& error) {
    if (!config_.log_errors)
        return;

    qCritical() << "🛡️ ErrorBoundary caught error in" << error.component_name
                << ":" << error.message;

    if (config_.show_error_details && !error.stack_trace.isEmpty()) {
        qDebug() << "🛡️ Stack trace:" << error.stack_trace;
    }

    if (config_.error_reporter) {
        config_.error_reporter(error);
    }
}

void ErrorBoundary::updateStats(const ErrorInfo& error) {
    stats_.total_errors++;
    stats_.last_error_time = error.timestamp;
}

QString ErrorBoundary::extractStackTrace(std::exception_ptr eptr) {
    // In a real implementation, this would extract stack trace information
    // For now, return a placeholder
    return "Stack trace not available";
}

// ErrorBoundaryBuilder implementation
ErrorBoundaryBuilder::ErrorBoundaryBuilder() {
    // Set default configuration
    config_.strategy = ErrorRecoveryStrategy::ShowFallback;
    config_.retry_delay = std::chrono::milliseconds(1000);
    config_.max_retry_attempts = 3;
    config_.log_errors = true;
    config_.show_error_details = false;
}

ErrorBoundaryBuilder& ErrorBoundaryBuilder::strategy(
    ErrorRecoveryStrategy strategy) {
    config_.strategy = strategy;
    return *this;
}

ErrorBoundaryBuilder& ErrorBoundaryBuilder::retryDelay(
    std::chrono::milliseconds delay) {
    config_.retry_delay = delay;
    return *this;
}

ErrorBoundaryBuilder& ErrorBoundaryBuilder::maxRetries(int max_attempts) {
    config_.max_retry_attempts = max_attempts;
    return *this;
}

ErrorBoundaryBuilder& ErrorBoundaryBuilder::logErrors(bool enable) {
    config_.log_errors = enable;
    return *this;
}

ErrorBoundaryBuilder& ErrorBoundaryBuilder::showErrorDetails(bool show) {
    config_.show_error_details = show;
    return *this;
}

ErrorBoundaryBuilder& ErrorBoundaryBuilder::fallback(
    std::function<std::unique_ptr<QWidget>(const ErrorInfo&)> factory) {
    config_.fallback_factory = std::move(factory);
    return *this;
}

ErrorBoundaryBuilder& ErrorBoundaryBuilder::simpleFallback(
    const QString& message) {
    config_.fallback_factory =
        [message](const ErrorInfo&) -> std::unique_ptr<QWidget> {
        auto widget = std::make_unique<QLabel>(message);
        widget->setAlignment(Qt::AlignCenter);
        widget->setStyleSheet("color: red; font-weight: bold; padding: 20px;");
        return widget;
    };
    return *this;
}

ErrorBoundaryBuilder& ErrorBoundaryBuilder::customFallback(
    std::unique_ptr<QWidget> widget) {
    auto captured_widget = widget.release();
    config_.fallback_factory =
        [captured_widget](const ErrorInfo&) -> std::unique_ptr<QWidget> {
        return std::unique_ptr<QWidget>(captured_widget);
    };
    return *this;
}

ErrorBoundaryBuilder& ErrorBoundaryBuilder::onError(
    std::function<void(const ErrorInfo&)> reporter) {
    config_.error_reporter = std::move(reporter);
    return *this;
}

ErrorBoundaryBuilder& ErrorBoundaryBuilder::child(
    std::function<std::unique_ptr<QWidget>()> factory) {
    child_factory_ = std::move(factory);
    return *this;
}

ErrorBoundaryBuilder& ErrorBoundaryBuilder::child(
    std::unique_ptr<QWidget> widget) {
    child_widget_ = std::move(widget);
    return *this;
}

std::unique_ptr<ErrorBoundary> ErrorBoundaryBuilder::build() {
    auto boundary = std::make_unique<ErrorBoundary>();
    boundary->setConfig(config_);

    if (child_factory_) {
        boundary->setChildFactory(child_factory_);
        try {
            auto child = child_factory_();
            boundary->setChild(std::move(child));
        } catch (const std::exception& e) {
            qWarning() << "🛡️ Failed to create child during build:" << e.what();
        }
    } else if (child_widget_) {
        boundary->setChild(std::move(child_widget_));
    }

    return boundary;
}

// ErrorBoundaryManager implementation
ErrorBoundaryManager& ErrorBoundaryManager::instance() {
    static ErrorBoundaryManager instance;
    return instance;
}

ErrorBoundaryManager::ErrorBoundaryManager(QObject* parent) : QObject(parent) {
    qDebug() << "🛡️ ErrorBoundaryManager initialized";
}

void ErrorBoundaryManager::setGlobalErrorHandler(
    std::function<void(const ErrorInfo&)> handler) {
    global_error_handler_ = std::move(handler);
}

void ErrorBoundaryManager::reportError(const ErrorInfo& error) {
    global_stats_.total_errors_caught++;
    global_stats_.last_error_time = error.timestamp;

    if (global_error_handler_) {
        global_error_handler_(error);
    }

    emit globalErrorReported(error);
}

void ErrorBoundaryManager::registerErrorBoundary(ErrorBoundary* boundary) {
    if (!boundary)
        return;

    auto it = std::find(registered_boundaries_.begin(),
                        registered_boundaries_.end(), boundary);
    if (it == registered_boundaries_.end()) {
        registered_boundaries_.push_back(boundary);
        global_stats_.total_boundaries++;
        global_stats_.active_boundaries++;

        emit errorBoundaryRegistered(boundary);
        qDebug() << "🛡️ ErrorBoundary registered, total:"
                 << global_stats_.active_boundaries;
    }
}

void ErrorBoundaryManager::unregisterErrorBoundary(ErrorBoundary* boundary) {
    auto it = std::find(registered_boundaries_.begin(),
                        registered_boundaries_.end(), boundary);
    if (it != registered_boundaries_.end()) {
        registered_boundaries_.erase(it);
        global_stats_.active_boundaries--;

        emit errorBoundaryUnregistered(boundary);
        qDebug() << "🛡️ ErrorBoundary unregistered, remaining:"
                 << global_stats_.active_boundaries;
    }
}

void ErrorBoundaryManager::setDefaultConfig(const ErrorBoundaryConfig& config) {
    default_config_ = config;
}

// ErrorBoundaries namespace implementation
namespace ErrorBoundaries {

std::unique_ptr<ErrorBoundary> create(std::unique_ptr<QWidget> child) {
    return ErrorBoundaryBuilder().child(std::move(child)).build();
}

std::unique_ptr<ErrorBoundary> createWithFallback(
    std::unique_ptr<QWidget> child, const QString& fallback_message) {
    return ErrorBoundaryBuilder()
        .child(std::move(child))
        .simpleFallback(fallback_message)
        .build();
}

std::unique_ptr<QWidget> wrap(std::unique_ptr<QWidget> widget,
                              const ErrorBoundaryConfig& config) {
    auto boundary = std::make_unique<ErrorBoundary>();
    boundary->setConfig(config);
    boundary->setChild(std::move(widget));
    boundary->initialize();

    // Cast ErrorBoundary to QWidget since ErrorBoundary inherits from UIElement
    // which manages QWidget
    return std::unique_ptr<QWidget>(
        static_cast<QWidget*>(boundary->getWidget()));
}

}  // namespace ErrorBoundaries

}  // namespace DeclarativeUI::Core
