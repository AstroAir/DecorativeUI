#pragma once

#include <QAtomicInteger>
#include <QDateTime>
#include <QFuture>
#include <QFutureWatcher>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMetaType>
#include <QMutex>
#include <QObject>
#include <QReadWriteLock>
#include <QThread>
#include <QThreadPool>
#include <QTimer>
#include <QUuid>
#include <QVariant>
#include <QtConcurrent/QtConcurrent>

#include <chrono>
#include <functional>
#include <map>
#include <memory>
#include <queue>
#include <type_traits>
#include <unordered_map>
#include <vector>

// **C++20 compatible expected implementation**
namespace std {
template <typename T, typename E>
class expected {
private:
    union {
        T value_;
        E error_;
    };
    bool has_value_;

public:
    expected(const T& value) : value_(value), has_value_(true) {}
    expected(const E& error) : error_(error), has_value_(false) {}

    ~expected() {
        if (has_value_) {
            value_.~T();
        } else {
            error_.~E();
        }
    }

    expected(const expected& other) : has_value_(other.has_value_) {
        if (has_value_) {
            new (&value_) T(other.value_);
        } else {
            new (&error_) E(other.error_);
        }
    }

    expected& operator=(const expected& other) {
        if (this != &other) {
            this->~expected();
            new (this) expected(other);
        }
        return *this;
    }

    bool has_value() const noexcept { return has_value_; }
    const T& value() const { return value_; }
    T& value() { return value_; }
    const E& error() const { return error_; }
    E& error() { return error_; }

    const T& value_or(const T& default_value) const {
        return has_value_ ? value_ : default_value;
    }

    template <typename F>
    auto and_then(F&& f) const {
        if (has_value_) {
            return f(value_);
        } else {
            return expected<typename std::invoke_result_t<F, T>::value_type, E>(
                error_);
        }
    }

    template <typename F>
    auto transform(F&& f) const {
        if (has_value_) {
            return expected<std::invoke_result_t<F, T>, E>(f(value_));
        } else {
            return expected<std::invoke_result_t<F, T>, E>(error_);
        }
    }
};

template <typename T>
expected<T, std::string> unexpected(const std::string& error) {
    return expected<T, std::string>(error);
}
}  // namespace std

namespace DeclarativeUI::Command {

// **Modern C++ concepts for type safety**
template <typename T>
concept CommandType = requires {
    typename T;
    requires std::is_default_constructible_v<T>;
    requires std::is_move_constructible_v<T>;
};

template <typename T>
concept CommandResultType =
    std::is_copy_constructible_v<T> && std::is_move_constructible_v<T>;

template <typename T>
concept CommandContextType = std::is_copy_constructible_v<T>;

// **Command execution priority levels**
enum class CommandPriority : int {
    Critical = 0,
    High = 1,
    Normal = 2,
    Low = 3,
    Background = 4
};

// **Command execution mode**
enum class ExecutionMode { Synchronous, Asynchronous, Deferred, Batched };

// **Command state tracking**
enum class CommandState {
    Created,
    Queued,
    Executing,
    Completed,
    Failed,
    Cancelled,
    Timeout
};

// **Command metadata for advanced features**
struct CommandMetadata {
    QUuid id = QUuid::createUuid();
    QString name;
    QString description;
    CommandPriority priority = CommandPriority::Normal;
    ExecutionMode mode = ExecutionMode::Synchronous;
    std::chrono::milliseconds timeout{5000};
    bool retryable = false;
    int max_retry_attempts = 3;
    std::chrono::milliseconds retry_delay{1000};
    bool auditable = true;
    QStringList tags;
    QJsonObject custom_data;

    CommandMetadata();
    CommandMetadata(const QString& cmd_name, const QString& cmd_desc = {});
};

// **Command execution context with dependency injection**
class CommandContext {
public:
    CommandContext();
    explicit CommandContext(const QJsonObject& data);

    // **Template-based parameter access**
    template <typename T>
    T getParameter(const QString& key, const T& default_value = T{}) const {
        if (!data_.contains(key)) {
            return default_value;
        }

        const auto value = data_[key];
        if constexpr (std::is_same_v<T, QString>) {
            return value.toString();
        } else if constexpr (std::is_same_v<T, int>) {
            return value.toInt();
        } else if constexpr (std::is_same_v<T, double>) {
            return value.toDouble();
        } else if constexpr (std::is_same_v<T, bool>) {
            return value.toBool();
        } else if constexpr (std::is_same_v<T, QVariant>) {
            return value.toVariant();
        } else {
            return default_value;
        }
    }

    bool hasParameter(const QString& key) const;

    template <typename T>
    CommandContext& setParameter(const QString& key, const T& value) {
        if constexpr (std::is_same_v<T, QString>) {
            data_[key] = value;
        } else if constexpr (std::is_same_v<T, int>) {
            data_[key] = value;
        } else if constexpr (std::is_same_v<T, double>) {
            data_[key] = value;
        } else if constexpr (std::is_same_v<T, bool>) {
            data_[key] = value;
        } else {
            data_[key] = QJsonValue::fromVariant(QVariant::fromValue(value));
        }
        return *this;
    }

    // **Service locator pattern for dependency injection**
    template <typename T>
    void registerService(std::shared_ptr<T> service) {
        services_[typeid(T).name()] = service;
    }

    template <typename T>
    std::shared_ptr<T> getService() const {
        auto it = services_.find(typeid(T).name());
        if (it != services_.end()) {
            return std::static_pointer_cast<T>(it->second);
        }
        return nullptr;
    }

    QJsonObject getData() const;
    void setData(const QJsonObject& data);

private:
    QJsonObject data_;
    std::unordered_map<std::string, std::shared_ptr<void>> services_;
};

// **Command result with comprehensive error handling**
template <typename T = QVariant>
class CommandResult {
public:
    CommandResult();
    explicit CommandResult(const T& result);
    explicit CommandResult(const QString& error);

    bool isSuccess() const;
    bool isError() const;

    const T& getResult() const;
    const QString& getError() const;

    // **Monadic operations**
    template <typename F>
    auto map(F&& func) const -> CommandResult<std::invoke_result_t<F, T>> {
        if (success_) {
            return CommandResult<std::invoke_result_t<F, T>>(func(result_));
        }
        return CommandResult<std::invoke_result_t<F, T>>(error_);
    }

    template <typename F>
    auto flatMap(F&& func) const -> std::invoke_result_t<F, T> {
        if (success_) {
            return func(result_);
        }
        return std::invoke_result_t<F, T>(error_);
    }

    QJsonObject toJson() const;

private:
    T result_;
    QString error_;
    bool success_ = false;
};

// **Command error with stack trace and context**
class CommandError : public std::exception {
public:
    CommandError(const QString& message, const QString& code = {});

    const char* what() const noexcept override;

    const QString& getMessage() const;
    const QString& getCode() const;
    const QDateTime& getTimestamp() const;

    CommandError& setContext(const CommandContext& context);

    CommandError& setStackTrace(const QStringList& trace);

    QJsonObject toJson() const;

private:
    QString message_;
    QString code_;
    QDateTime timestamp_;
    CommandContext context_;
    QStringList stack_trace_;
};

// **Base command interface with CRTP for performance**
class ICommand : public QObject {
    Q_OBJECT

public:
    explicit ICommand(QObject* parent = nullptr);
    virtual ~ICommand();

    // **Core execution interface**
    virtual CommandResult<QVariant> execute(const CommandContext& context) = 0;
    virtual CommandResult<QVariant> undo(const CommandContext& context);
    virtual CommandResult<QVariant> redo(const CommandContext& context);

    // **Command validation**
    virtual bool canExecute(const CommandContext& context) const;
    virtual bool canUndo(const CommandContext& context) const;
    virtual bool canRedo(const CommandContext& context) const;

    // **Command metadata**
    virtual CommandMetadata getMetadata() const = 0;

    // **Command lifecycle hooks**
    virtual void onBeforeExecute(const CommandContext& context);
    virtual void onAfterExecute(const CommandContext& context,
                                const CommandResult<QVariant>& result);
    virtual void onError(const CommandContext& context,
                         const CommandError& error);

    // **Command state management**
    CommandState getState() const;
    void setState(CommandState state);

    // **Command execution statistics**
    struct ExecutionStats {
        std::chrono::milliseconds duration{0};
        int retry_count = 0;
        QDateTime start_time;
        QDateTime end_time;
        QString error_message;
    };

    const ExecutionStats& getStats() const;

signals:
    void stateChanged(CommandState state);
    void progressUpdated(int percentage);
    void messageUpdated(const QString& message);

protected:
    CommandState state_ = CommandState::Created;
    ExecutionStats stats_;

    void updateProgress(int percentage);

    void updateMessage(const QString& message);
};

// **Async command with simplified interface**
class AsyncCommand : public ICommand {
    Q_OBJECT

public:
    explicit AsyncCommand(QObject* parent = nullptr);

    // **Async execution using Qt's future system**
    virtual QFuture<CommandResult<QVariant>> executeAsync(
        const CommandContext& context) = 0;

    CommandResult<QVariant> execute(
        const CommandContext& context) override final ;
};

// **Command with transactional support**
class TransactionalCommand : public ICommand {
    Q_OBJECT

public:
    explicit TransactionalCommand(QObject* parent = nullptr);

    // **Transaction management**
    virtual void beginTransaction(const CommandContext& context) = 0;
    virtual void commitTransaction(const CommandContext& context) = 0;
    virtual void rollbackTransaction(const CommandContext& context) = 0;

    CommandResult<QVariant> execute(
        const CommandContext& context) override final;

protected:
    virtual CommandResult<QVariant> executeTransactional(
        const CommandContext& context) = 0;
};

// **Composite command for complex operations**
class CompositeCommand : public ICommand {
    Q_OBJECT

public:
    explicit CompositeCommand(QObject* parent = nullptr);

    void addCommand(std::unique_ptr<ICommand> command);

    CommandResult<QVariant> execute(const CommandContext& context) override;

    CommandResult<QVariant> undo(const CommandContext& context) override ;

    CommandMetadata getMetadata() const override ;

protected:
    std::vector<std::unique_ptr<ICommand>> commands_;

    CommandResult<QVariant> rollbackCommands(const CommandContext& context) ;
};

// **Command factory with modern registration system**
class CommandFactory {
public:
    static CommandFactory& instance() ;

    std::unique_ptr<ICommand> createCommand(
        const QString& command_name, const CommandContext& context = {}) ;

    void registerCommand(
        const QString& command_name,
        std::function<std::unique_ptr<ICommand>(const CommandContext&)>
            creator);

    void unregisterCommand(const QString& command_name);

    std::vector<QString> getRegisteredCommands() const;

private:
    CommandFactory();
    std::unordered_map<QString, std::function<std::unique_ptr<ICommand>(
                                    const CommandContext&)>>
        creators_;
};

// **Command invoker with advanced execution modes**
class CommandInvoker : public QObject {
    Q_OBJECT

public:
    explicit CommandInvoker(QObject* parent = nullptr);

    // **Synchronous execution**
    CommandResult<QVariant> execute(const QString& command_name,
                                    const CommandContext& context = {});

    // **Asynchronous execution with future support**
    QFuture<CommandResult<QVariant>> executeAsync(
        const QString& command_name, const CommandContext& context = {});

    // **Deferred execution**
    void executeDeferred(
        const QString& command_name, const CommandContext& context = {},
        std::chrono::milliseconds delay = std::chrono::milliseconds{0});

    // **Batch execution**
    void executeBatch(const std::vector<QString>& command_names,
                      const CommandContext& context = {});

    // **Command cancellation**
    void cancelCommand(const QUuid& command_id);

    // **Command monitoring**
    std::vector<std::pair<QUuid, CommandState>> getRunningCommands() const;

signals:
    void commandExecuted(const QString& command_name,
                         const CommandResult<QVariant>& result);
    void commandFailed(const QString& command_name, const CommandError& error);
    void commandProgress(const QUuid& command_id, int percentage);

private slots:
    void processBatch();

private:
    struct BatchItem {
        QString command_name;
        CommandContext context;
    };

    std::queue<BatchItem> batch_queue_;
    QTimer* batch_timer_;
    std::chrono::milliseconds batch_interval_{100};

    std::map<QUuid, std::unique_ptr<ICommand>> running_commands_;
    mutable QReadWriteLock commands_lock_;

    CommandResult<QVariant> executeInternal(const QString& command_name,
                                            const CommandContext& context,
                                            ExecutionMode mode);
};

// **Command interceptor for logging, validation, etc.**
class CommandInterceptor : public QObject {
    Q_OBJECT

public:
    explicit CommandInterceptor(QObject* parent = nullptr);
    virtual ~CommandInterceptor();

    virtual bool beforeExecute(ICommand* command,
                               const CommandContext& context) = 0;
    virtual void afterExecute(ICommand* command, const CommandContext& context,
                              const CommandResult<QVariant>& result) = 0;
    virtual void onError(ICommand* command, const CommandContext& context,
                         const CommandError& error) = 0;
};

// **Command manager for centralized command handling**
class CommandManager : public QObject {
    Q_OBJECT

public:
    static CommandManager& instance();

    // **Get command invoker**
    CommandInvoker& getInvoker();

    // **Add command interceptor**
    void addInterceptor(std::unique_ptr<CommandInterceptor> interceptor);

    // **Command history management**
    void enableCommandHistory(bool enabled);
    bool canUndo() const;
    bool canRedo() const;

    void undo();

    void redo();

    // **Command audit trail**
    void enableAuditTrail(bool enabled);
    QJsonArray getAuditTrail() const;

signals:
    void commandExecuted(const QString& command_name);
    void commandUndone(const QString& command_name);
    void commandRedone(const QString& command_name);

private:
    CommandManager();

    struct CommandHistoryItem {
        std::unique_ptr<ICommand> command;
        CommandContext context;
        QDateTime timestamp;
    };

    CommandInvoker invoker_;
    std::vector<std::unique_ptr<CommandInterceptor>> interceptors_;

    bool history_enabled_ = false;
    std::vector<CommandHistoryItem> command_history_;
    std::vector<CommandHistoryItem> redo_stack_;

    bool audit_enabled_ = false;
    QJsonArray audit_trail_;
};

// **Utility macros for command registration**
#define REGISTER_COMMAND(CommandType, CommandName)                           \
    static bool CommandType##_registered = []() {                            \
        CommandFactory::instance().registerCommand(                          \
            CommandName,                                                     \
            [](const CommandContext& context) -> std::unique_ptr<ICommand> { \
                return std::make_unique<CommandType>(context);               \
            });                                                              \
        return true;                                                         \
    }();

#define REGISTER_COMMAND_WITH_FACTORY(CommandType, CommandName, Factory)  \
    static bool CommandType##_registered = []() {                         \
        CommandFactory::instance().registerCommand(CommandName, Factory); \
        return true;                                                      \
    }();

}  // namespace DeclarativeUI::Command

// **Forward declarations for UI Command system**
namespace DeclarativeUI::Command::UI {
    class BaseUICommand;
    class UICommandMetadata;
    class UICommandState;
    class WidgetMapper;
    class UICommandFactory;
}

#include "UICommand.hpp"
#include "UICommandFactory.hpp"
#include "WidgetMapper.hpp"
#include "CoreCommands.hpp"
#include "SpecializedCommands.hpp"
#include "CommandBuilder.hpp"
#include "CommandBinding.hpp"
#include "MVCIntegration.hpp"
#include "CommandEvents.hpp"

Q_DECLARE_METATYPE(DeclarativeUI::Command::CommandState)
Q_DECLARE_METATYPE(DeclarativeUI::Command::CommandPriority)
Q_DECLARE_METATYPE(DeclarativeUI::Command::ExecutionMode)
