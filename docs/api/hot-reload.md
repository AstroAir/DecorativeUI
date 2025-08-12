# Hot Reload API Reference

The Hot Reload module provides real-time UI updates without application restart, enabling rapid development workflows. It consists of file monitoring, reload management, and performance tracking components.

## HotReloadManager Class

The `HotReloadManager` class coordinates the hot reload system, managing file registration, reload triggers, and widget replacement.

### Header

```cpp
#include "HotReload/HotReloadManager.hpp"
```

### Class Declaration

```cpp
namespace DeclarativeUI::HotReload {
    class HotReloadManager : public QObject {
        Q_OBJECT
    public:
        explicit HotReloadManager(QObject* parent = nullptr);
        ~HotReloadManager() override = default;

        // Registration and management methods...
    };
}
```

### Public Methods

#### File Registration

##### `registerUIFile(const QString& file_path, QWidget* target_widget)`

Registers a UI file for hot reloading with a target widget.

**Parameters:**

- `file_path`: Path to the UI definition file
- `target_widget`: Widget to replace when file changes

**Throws:** `HotReloadException` if file doesn't exist or registration fails

**Example:**

```cpp
auto manager = std::make_unique<HotReloadManager>();
manager->registerUIFile("ui/main_window.json", main_widget);
```

##### `registerUIDirectory(const QString& directory_path, bool recursive = true)`

Registers an entire directory for hot reloading.

**Parameters:**

- `directory_path`: Path to the directory containing UI files
- `recursive`: Whether to watch subdirectories

**Example:**

```cpp
manager->registerUIDirectory("Resources/ui_definitions", true);
```

#### Control Methods

##### `setEnabled(bool enabled)`

Enables or disables hot reloading globally.

**Parameters:**

- `enabled`: Whether hot reloading should be active

##### `isEnabled() const noexcept -> bool`

Checks if hot reloading is currently enabled.

**Returns:** `true` if enabled, `false` otherwise

#### Configuration

##### `setReloadDelay(int milliseconds)`

Sets the debounce delay for file change events.

**Parameters:**

- `milliseconds`: Delay in milliseconds (default: 100ms)

**Example:**

```cpp
manager->setReloadDelay(250); // 250ms delay
```

##### `setFileFilters(const QStringList& filters)`

Sets file extension filters for monitoring.

**Parameters:**

- `filters`: List of file extensions to monitor (e.g., "_.json", "_.ui")

#### Manual Operations

##### `reloadFile(const QString& file_path)`

Manually triggers reload for a specific file.

**Parameters:**

- `file_path`: Path to the file to reload

##### `reloadAll()`

Manually triggers reload for all registered files.

#### Cleanup

##### `unregisterUIFile(const QString& file_path)`

Stops monitoring a specific file.

**Parameters:**

- `file_path`: Path to the file to stop monitoring

##### `unregisterAll()`

Stops monitoring all registered files.

#### Error Handling

##### `setErrorHandler(std::function<void(const QString&, const QString&)> handler)`

Sets a custom error handler for reload failures.

**Parameters:**

- `handler`: Function to handle errors (file_path, error_message)

**Example:**

```cpp
manager->setErrorHandler([](const QString& file, const QString& error) {
    QMessageBox::warning(nullptr, "Hot Reload Error",
                        QString("Failed to reload %1: %2").arg(file, error));
});
```

### Signals

```cpp
signals:
    void reloadStarted(const QString& file_path);
    void reloadCompleted(const QString& file_path);
    void reloadFailed(const QString& file_path, const QString& error);
    void hotReloadEnabled(bool enabled);
```

### Usage Example

```cpp
#include "HotReload/HotReloadManager.hpp"

class MyApplication : public QObject {
    Q_OBJECT

public:
    MyApplication() {
        setupHotReload();
    }

private:
    void setupHotReload() {
        hot_reload_manager_ = std::make_unique<HotReloadManager>();

        // Register UI file
        hot_reload_manager_->registerUIFile(
            "Resources/ui_definitions/main.json",
            main_widget_.get()
        );

        // Set up error handling
        hot_reload_manager_->setErrorHandler(
            [this](const QString& file, const QString& error) {
                qWarning() << "Hot reload failed:" << file << error;
            }
        );

        // Connect signals
        connect(hot_reload_manager_.get(),
                &HotReloadManager::reloadCompleted,
                this, &MyApplication::onReloadCompleted);
    }

private slots:
    void onReloadCompleted(const QString& file_path) {
        qDebug() << "Successfully reloaded:" << file_path;
        // Update UI state if needed
    }

private:
    std::unique_ptr<HotReloadManager> hot_reload_manager_;
    std::unique_ptr<QWidget> main_widget_;
};
```

## FileWatcher Class

The `FileWatcher` class provides low-level file system monitoring with debouncing and filtering capabilities.

### Header

```cpp
#include "HotReload/FileWatcher.hpp"
```

### Public Methods

#### Watching Operations

##### `watchFile(const QString &file_path)`

Starts watching a specific file for changes.

**Parameters:**

- `file_path`: Path to the file to watch

**Throws:** `FileWatchException` if file doesn't exist or watch setup fails

##### `watchDirectory(const QString &directory_path, bool recursive = false)`

Starts watching a directory for changes.

**Parameters:**

- `directory_path`: Path to the directory to watch
- `recursive`: Whether to watch subdirectories

##### `unwatchFile(const QString &file_path)`

Stops watching a specific file.

##### `unwatchDirectory(const QString &directory_path)`

Stops watching a specific directory.

##### `unwatchAll()`

Stops watching all files and directories.

#### Configuration

##### `setDebounceInterval(int milliseconds)`

Sets the debounce interval for file change events.

**Parameters:**

- `milliseconds`: Debounce interval (default: 100ms)

##### `setFileFilters(const QStringList &filters)`

Sets file extension filters.

**Parameters:**

- `filters`: List of file patterns to monitor

#### Status Queries

##### `isWatching(const QString &path) const -> bool`

Checks if a path is currently being watched.

##### `watchedFiles() const -> QStringList`

Returns list of all watched files.

##### `watchedDirectories() const -> QStringList`

Returns list of all watched directories.

### Signals

```cpp
signals:
    void fileChanged(const QString &file_path);
    void fileAdded(const QString &file_path);
    void fileRemoved(const QString &file_path);
    void directoryChanged(const QString &directory_path);
    void watchingStarted(const QString &path);
    void watchingStopped(const QString &path);
    void watchingFailed(const QString &path, const QString &error);
```

### Usage Example

```cpp
auto watcher = std::make_unique<FileWatcher>();

// Set up file filters
watcher->setFileFilters({"*.json", "*.ui"});
watcher->setDebounceInterval(200);

// Connect signals
connect(watcher.get(), &FileWatcher::fileChanged,
        [](const QString& file) {
            qDebug() << "File changed:" << file;
        });

// Start watching
watcher->watchDirectory("ui_files", true);
```

## PerformanceMonitor Class

The `PerformanceMonitor` class tracks and analyzes hot reload performance metrics.

### Header

```cpp
#include "HotReload/PerformanceMonitor.hpp"
```

### Performance Metrics Structure

```cpp
struct PerformanceMetrics {
    QString file_path;
    qint64 total_time_ms;
    qint64 parse_time_ms;
    qint64 validation_time_ms;
    qint64 creation_time_ms;
    qint64 replacement_time_ms;
    QDateTime timestamp;
    bool success;
    QString error_message;
};
```

### Public Methods

#### Monitoring Control

##### `setEnabled(bool enabled)`

Enables or disables performance monitoring.

##### `isEnabled() const -> bool`

Checks if monitoring is enabled.

#### Operation Tracking

##### `startOperation(const QString &operation_name)`

Starts timing an operation.

**Parameters:**

- `operation_name`: Name of the operation to track

##### `endOperation(const QString &operation_name) -> qint64`

Ends timing an operation and returns elapsed time.

**Returns:** Elapsed time in milliseconds

##### `recordReloadMetrics(const QString &file_path, const PerformanceMetrics &metrics)`

Records performance metrics for a reload operation.

#### Statistics

##### `getAverageMetrics() const -> PerformanceMetrics`

Returns average performance metrics across all operations.

##### `getMetricsForFile(const QString &file_path) const -> PerformanceMetrics`

Returns metrics for a specific file.

##### `getSlowFiles(int threshold_ms = 1000) const -> QStringList`

Returns list of files that reload slowly.

##### `getSuccessRate() const -> double`

Returns the success rate as a percentage (0.0 to 1.0).

#### Configuration

##### `setMaxHistorySize(int size)`

Sets maximum number of metrics to keep in history.

##### `setWarningThreshold(int threshold_ms)`

Sets threshold for performance warnings.

##### `setPerformanceCallback(std::function<void(const QString &, const PerformanceMetrics &)> callback)`

Sets callback for performance events.

#### Reporting

##### `generateReport() const -> QString`

Generates a detailed performance report.

##### `clearHistory()`

Clears all performance history.

### Signals

```cpp
signals:
    void performanceWarning(const QString &file_path, qint64 time_ms);
    void slowOperationDetected(const QString &operation, qint64 time_ms);
```

### Usage Example

```cpp
auto monitor = std::make_unique<PerformanceMonitor>();
monitor->setEnabled(true);
monitor->setWarningThreshold(500); // 500ms threshold

// Connect performance warnings
connect(monitor.get(), &PerformanceMonitor::performanceWarning,
        [](const QString& file, qint64 time) {
            qWarning() << "Slow reload detected:" << file << time << "ms";
        });

// Track an operation
monitor->startOperation("ui_reload");
// ... perform reload operation ...
qint64 elapsed = monitor->endOperation("ui_reload");

// Generate report
QString report = monitor->generateReport();
qDebug() << report;
```

## Integration Example

Here's a complete example showing how to integrate all hot reload components:

```cpp
class HotReloadApplication : public QApplication {
    Q_OBJECT

public:
    HotReloadApplication(int argc, char* argv[]) : QApplication(argc, argv) {
        setupHotReload();
    }

private:
    void setupHotReload() {
        // Create components
        hot_reload_manager_ = std::make_unique<HotReloadManager>();
        performance_monitor_ = std::make_unique<PerformanceMonitor>();

        // Configure performance monitoring
        performance_monitor_->setEnabled(true);
        performance_monitor_->setWarningThreshold(1000);

        // Set up hot reload manager
        hot_reload_manager_->setReloadDelay(100);
        hot_reload_manager_->setErrorHandler(
            [this](const QString& file, const QString& error) {
                handleReloadError(file, error);
            }
        );

        // Register UI files
        hot_reload_manager_->registerUIDirectory("ui", true);

        // Connect signals
        connect(hot_reload_manager_.get(),
                &HotReloadManager::reloadStarted,
                this, &HotReloadApplication::onReloadStarted);

        connect(hot_reload_manager_.get(),
                &HotReloadManager::reloadCompleted,
                this, &HotReloadApplication::onReloadCompleted);
    }

private slots:
    void onReloadStarted(const QString& file_path) {
        performance_monitor_->startOperation("reload_" + file_path);
    }

    void onReloadCompleted(const QString& file_path) {
        qint64 elapsed = performance_monitor_->endOperation("reload_" + file_path);
        qDebug() << "Reload completed in" << elapsed << "ms";
    }

    void handleReloadError(const QString& file, const QString& error) {
        qCritical() << "Hot reload failed for" << file << ":" << error;
    }

private:
    std::unique_ptr<HotReloadManager> hot_reload_manager_;
    std::unique_ptr<PerformanceMonitor> performance_monitor_;
};
```

## Best Practices

### Performance Optimization

- Use appropriate debounce intervals (100-250ms)
- Monitor performance metrics regularly
- Set reasonable warning thresholds
- Clear performance history periodically

### Error Handling

- Always set custom error handlers
- Validate UI files before deployment
- Implement fallback mechanisms
- Log errors for debugging

### File Organization

- Use consistent directory structures
- Group related UI files together
- Use meaningful file names
- Implement proper file filters

### Development Workflow

- Enable hot reload during development
- Disable in production builds
- Use performance monitoring to optimize
- Test reload scenarios thoroughly
