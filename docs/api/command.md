# Command System API Reference

The Command module provides a comprehensive command execution system with support for synchronous/asynchronous execution, undo/redo functionality, command composition, and UI integration.

## Core Concepts

### CommandResult Template

A type-safe result wrapper for command execution.

```cpp
template<typename T>
class CommandResult {
public:
    static CommandResult<T> success(T&& result);
    static CommandResult<T> error(const QString& error_message);
    
    bool isSuccess() const;
    bool isError() const;
    T getResult() const;
    QString getError() const;
};
```

### CommandContext Structure

Context information passed to commands.

```cpp
struct CommandContext {
    QVariantMap parameters;
    QObject* sender = nullptr;
    QWidget* target_widget = nullptr;
    QString source_file;
    QDateTime timestamp;
    QUuid command_id;
    
    // Convenience methods
    template<typename T>
    T getParameter(const QString& key, const T& default_value = T{}) const;
    
    void setParameter(const QString& key, const QVariant& value);
    bool hasParameter(const QString& key) const;
};
```

### CommandMetadata Structure

Metadata describing a command.

```cpp
struct CommandMetadata {
    QUuid id;
    QString name;
    QString description;
    QString category;
    QStringList required_parameters;
    QStringList optional_parameters;
    bool supports_undo = false;
    bool is_async = false;
    int priority = 0;
};
```

## ICommand Interface

Base interface for all commands.

### Header
```cpp
#include "Command/CommandSystem.hpp"
```

### Class Declaration
```cpp
class ICommand : public QObject {
    Q_OBJECT
    
public:
    explicit ICommand(QObject* parent = nullptr);
    virtual ~ICommand();
    
    // Core interface methods...
};
```

### Pure Virtual Methods

##### `virtual CommandResult<QVariant> execute(const CommandContext& context) = 0`
Executes the command with the given context.

**Parameters:**
- `context`: Command execution context

**Returns:** Command result with success/error status

##### `virtual CommandMetadata getMetadata() const = 0`
Returns metadata describing the command.

**Returns:** Command metadata structure

### Virtual Methods

##### `virtual CommandResult<QVariant> undo(const CommandContext& context)`
Undoes the command execution.

**Returns:** Undo result

##### `virtual CommandResult<QVariant> redo(const CommandContext& context)`
Redoes a previously undone command.

**Returns:** Redo result

##### `virtual bool canExecute(const CommandContext& context) const`
Checks if the command can be executed with the given context.

##### `virtual bool canUndo(const CommandContext& context) const`
Checks if the command can be undone.

##### `virtual bool canRedo(const CommandContext& context) const`
Checks if the command can be redone.

### Lifecycle Hooks

##### `virtual void onBeforeExecute(const CommandContext& context)`
Called before command execution.

##### `virtual void onAfterExecute(const CommandContext& context, const CommandResult<QVariant>& result)`
Called after command execution.

##### `virtual void onError(const CommandContext& context, const CommandError& error)`
Called when command execution fails.

### State Management

##### `CommandState getState() const`
Gets the current command state.

##### `void setState(CommandState state)`
Sets the command state.

### Signals

```cpp
signals:
    void stateChanged(CommandState state);
    void progressUpdated(int percentage);
    void messageUpdated(const QString& message);
```

## Specialized Command Classes

### AsyncCommand

Base class for asynchronous commands.

```cpp
class AsyncCommand : public ICommand {
    Q_OBJECT
    
public:
    virtual QFuture<CommandResult<QVariant>> executeAsync(const CommandContext& context) = 0;
    
    // ICommand interface implemented to delegate to executeAsync
    CommandResult<QVariant> execute(const CommandContext& context) override final;
};
```

### TransactionalCommand

Base class for transactional commands with rollback support.

```cpp
class TransactionalCommand : public ICommand {
    Q_OBJECT
    
public:
    virtual void beginTransaction(const CommandContext& context) = 0;
    virtual void commitTransaction(const CommandContext& context) = 0;
    virtual void rollbackTransaction(const CommandContext& context) = 0;
    
protected:
    virtual CommandResult<QVariant> executeTransactional(const CommandContext& context) = 0;
};
```

### CompositeCommand

Command that executes multiple sub-commands.

```cpp
class CompositeCommand : public ICommand {
    Q_OBJECT
    
public:
    void addCommand(std::unique_ptr<ICommand> command);
    
    CommandResult<QVariant> execute(const CommandContext& context) override;
    CommandResult<QVariant> undo(const CommandContext& context) override;
};
```

## CommandFactory Class

Factory for creating command instances.

### Header
```cpp
#include "Command/CommandSystem.hpp"
```

### Public Methods

#### Singleton Access

##### `static CommandFactory& instance()`
Gets the global command factory instance.

#### Command Registration

##### `registerCommand(const QString& command_name, std::function<std::unique_ptr<ICommand>(const CommandContext&)> creator)`
Registers a command creator function.

**Parameters:**
- `command_name`: Name to register the command under
- `creator`: Function that creates command instances

**Example:**
```cpp
CommandFactory::instance().registerCommand(
    "my_command",
    [](const CommandContext& context) -> std::unique_ptr<ICommand> {
        return std::make_unique<MyCommand>(context);
    }
);
```

##### `unregisterCommand(const QString& command_name)`
Unregisters a command.

##### `getRegisteredCommands() const -> std::vector<QString>`
Gets all registered command names.

#### Command Creation

##### `createCommand(const QString& command_name, const CommandContext& context) -> std::unique_ptr<ICommand>`
Creates a command instance.

**Parameters:**
- `command_name`: Name of the command to create
- `context`: Context for command creation

**Returns:** Unique pointer to the created command, or nullptr if not found

## CommandInvoker Class

Handles command execution with various modes.

### Public Methods

#### Synchronous Execution

##### `execute(const QString& command_name, const CommandContext& context = {}) -> CommandResult<QVariant>`
Executes a command synchronously.

**Example:**
```cpp
CommandInvoker invoker;
CommandContext context;
context.setParameter("target", "button1");
context.setParameter("property", "text");
context.setParameter("value", "New Text");

auto result = invoker.execute("set_property", context);
if (result.isSuccess()) {
    qDebug() << "Command executed successfully";
}
```

#### Asynchronous Execution

##### `executeAsync(const QString& command_name, const CommandContext& context = {}) -> QFuture<CommandResult<QVariant>>`
Executes a command asynchronously.

**Example:**
```cpp
auto future = invoker.executeAsync("long_running_command", context);
auto watcher = new QFutureWatcher<CommandResult<QVariant>>();
connect(watcher, &QFutureWatcher<CommandResult<QVariant>>::finished,
        [watcher]() {
            auto result = watcher->result();
            // Handle result...
            watcher->deleteLater();
        });
watcher->setFuture(future);
```

#### Deferred Execution

##### `executeDeferred(const QString& command_name, const CommandContext& context = {}, std::chrono::milliseconds delay = std::chrono::milliseconds{0})`
Executes a command after a delay.

#### Batch Execution

##### `executeBatch(const std::vector<QString>& command_names, const CommandContext& context = {})`
Executes multiple commands in batch.

#### Command Management

##### `cancelCommand(const QUuid& command_id)`
Cancels a running command.

##### `getRunningCommands() const -> std::vector<std::pair<QUuid, CommandState>>`
Gets information about currently running commands.

### Signals

```cpp
signals:
    void commandExecuted(const QString& command_name, const CommandResult<QVariant>& result);
    void commandFailed(const QString& command_name, const CommandError& error);
    void commandProgress(const QUuid& command_id, int percentage);
```

## CommandManager Class

High-level command management with history and audit trail.

### Header
```cpp
#include "Command/CommandSystem.hpp"
```

### Public Methods

#### Singleton Access

##### `static CommandManager& instance()`
Gets the global command manager instance.

#### Command Execution

##### `getInvoker() -> CommandInvoker&`
Gets the command invoker for direct access.

#### History Management

##### `enableCommandHistory(bool enabled)`
Enables or disables command history.

##### `canUndo() const -> bool`
Checks if undo is available.

##### `canRedo() const -> bool`
Checks if redo is available.

##### `undo()`
Undoes the last command.

##### `redo()`
Redoes the last undone command.

#### Audit Trail

##### `enableAuditTrail(bool enabled)`
Enables audit trail logging.

##### `getAuditTrail() const -> QJsonArray`
Gets the audit trail as JSON.

#### Interceptors

##### `addInterceptor(std::unique_ptr<CommandInterceptor> interceptor)`
Adds a command interceptor for logging, validation, etc.

### Usage Example

```cpp
// Enable features
CommandManager::instance().enableCommandHistory(true);
CommandManager::instance().enableAuditTrail(true);

// Execute command through manager
auto& invoker = CommandManager::instance().getInvoker();
auto result = invoker.execute("my_command", context);

// Use history
if (CommandManager::instance().canUndo()) {
    CommandManager::instance().undo();
}
```

## Built-in Commands

The system includes several built-in commands for common operations.

### SetPropertyCommand

Sets a property on a QObject.

**Required Parameters:**
- `target`: Target object (QObject*)
- `property`: Property name (QString)
- `value`: Property value (QVariant)

**Example:**
```cpp
CommandContext context;
context.setParameter("target", my_widget);
context.setParameter("property", "text");
context.setParameter("value", "New Text");

invoker.execute("set_property", context);
```

### UpdateStateCommand

Updates a value in the StateManager.

**Required Parameters:**
- `key`: State key (QString)
- `value`: New value (QVariant)

### SaveFileCommand / LoadFileCommand

File operations with dialog support.

**Parameters:**
- `file_path`: File path (QString, optional - shows dialog if not provided)
- `content`: Content to save (QByteArray, for save command)

### CopyToClipboardCommand / PasteFromClipboardCommand

Clipboard operations.

### ShowMessageCommand

Shows a message box.

**Required Parameters:**
- `title`: Message title (QString)
- `message`: Message text (QString)
- `type`: Message type (QString: "information", "warning", "critical")

## UI Integration

### CommandableUIElement

Extends UIElement with command capabilities.

```cpp
class CommandableUIElement : public Core::UIElement {
    Q_OBJECT
    
public:
    CommandableUIElement& onCommand(const QString& command_name, const CommandContext& context = {});
    CommandableUIElement& onShortcut(const QKeySequence& shortcut, const QString& command_name, const CommandContext& context = {});
    
    virtual CommandResult<QVariant> executeCommand(const QString& command_name, const CommandContext& context = {});
    virtual QFuture<CommandResult<QVariant>> executeCommandAsync(const QString& command_name, const CommandContext& context = {});
};
```

### Usage Example

```cpp
auto button = create<CommandableUIElement>()
    .onCommand("save_file")
    .onShortcut(QKeySequence::Save, "save_file")
    .build();
```

## Custom Command Implementation

### Basic Command

```cpp
class MyCommand : public ICommand {
    Q_OBJECT
    
public:
    explicit MyCommand(const CommandContext& context) : ICommand() {
        // Initialize from context
    }
    
    CommandResult<QVariant> execute(const CommandContext& context) override {
        try {
            // Perform command logic
            QString result = performOperation(context);
            return CommandResult<QVariant>::success(QVariant(result));
        } catch (const std::exception& e) {
            return CommandResult<QVariant>::error(QString::fromStdString(e.what()));
        }
    }
    
    CommandMetadata getMetadata() const override {
        CommandMetadata metadata;
        metadata.name = "my_command";
        metadata.description = "My custom command";
        metadata.category = "Custom";
        metadata.required_parameters = {"param1", "param2"};
        metadata.supports_undo = true;
        return metadata;
    }
    
    CommandResult<QVariant> undo(const CommandContext& context) override {
        // Implement undo logic
        return CommandResult<QVariant>::success(QVariant());
    }
    
private:
    QString performOperation(const CommandContext& context) {
        // Implementation
        return "Operation completed";
    }
};
```

### Async Command

```cpp
class MyAsyncCommand : public AsyncCommand {
    Q_OBJECT
    
public:
    QFuture<CommandResult<QVariant>> executeAsync(const CommandContext& context) override {
        return QtConcurrent::run([this, context]() -> CommandResult<QVariant> {
            // Long-running operation
            QThread::sleep(2); // Simulate work
            return CommandResult<QVariant>::success(QVariant("Async result"));
        });
    }
    
    CommandMetadata getMetadata() const override {
        CommandMetadata metadata;
        metadata.name = "my_async_command";
        metadata.is_async = true;
        return metadata;
    }
};
```

## Best Practices

### Command Design
- Keep commands focused on single operations
- Use meaningful command names and descriptions
- Implement proper error handling
- Support undo when possible
- Use appropriate command types (sync/async/transactional)

### Performance
- Use async commands for long-running operations
- Implement proper progress reporting
- Use batch execution for multiple related commands
- Monitor command execution times

### Error Handling
- Always return proper CommandResult objects
- Provide meaningful error messages
- Implement proper exception handling
- Use command interceptors for logging

### Testing
- Test command execution in isolation
- Test undo/redo functionality
- Test error conditions
- Use mock contexts for unit testing
