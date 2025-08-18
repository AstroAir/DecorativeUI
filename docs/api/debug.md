# Debug API Reference

The Debug module provides comprehensive debugging and performance monitoring tools for DeclarativeUI applications, including interactive widgets for diagnostics, profiling, and development debugging.

## ProfilerDashboard Class

Comprehensive performance monitoring dashboard with real-time metrics and analysis.

### Header

```cpp
#include "Debug/ProfilerDashboard.hpp"
```

### Class Declaration

```cpp
namespace DeclarativeUI::Debug {
    class ProfilerDashboard : public QMainWindow {
        Q_OBJECT
    public:
        explicit ProfilerDashboard(QWidget* parent = nullptr);
        
        // Dashboard management methods...
    };
}
```

### Methods

#### Data Updates

```cpp
void updatePerformanceData(const PerformanceDataPoint& data);
```
Updates the dashboard with new performance metrics.

```cpp
void addBottleneck(const BottleneckInfo& bottleneck);
```
Adds a detected performance bottleneck to the dashboard.

#### Configuration

```cpp
void setPerformanceThresholds(double cpu_threshold, size_t memory_threshold_mb, double frame_rate_threshold);
```
Sets performance alert thresholds.

#### Export Functionality

```cpp
void exportReport(const QString& format, const QString& filename) const;
```
Exports performance report in specified format (JSON, text).

### Signals

```cpp
void performanceAlert(const QString& metric, double value);
void bottleneckDetected(const BottleneckInfo& bottleneck);
void reportExported(const QString& filename);
```

## DebuggingConsole Class

Interactive debugging console for live log viewing and filtering.

### Header

```cpp
#include "Debug/DebuggingConsole.hpp"
```

### Methods

#### Log Management

```cpp
void appendLogLine(const QString& text);
```
Adds a single log line with automatic timestamp.

```cpp
void setLogText(const QString& full_text);
```
Sets the complete log content, replacing existing content.

```cpp
void addLogMessage(const QString& level, const QString& component, const QString& message);
```
Adds a structured log message with level and component information.

```cpp
void clearLog();
```
Clears all log content.

#### Filtering

```cpp
void setFilter(const QString& filter);
```
Sets the current log level filter ("All", "Error", "Warning", "Info", "Debug").

### Signals

```cpp
void exportRequested();
void filterChanged(const QString& filter);
```

## BottleneckDetectorWidget Class

Interactive widget for displaying and resolving performance bottlenecks.

### Header

```cpp
#include "Debug/BottleneckDetectorWidget.hpp"
```

### Methods

#### Bottleneck Management

```cpp
void addBottleneck(const QString& description, const QString& severity);
```
Adds a bottleneck with description and severity level.

```cpp
void clearBottlenecks();
```
Removes all bottlenecks from the display.

```cpp
void refreshBottlenecks();
```
Triggers a refresh of bottleneck detection.

#### Resolution

```cpp
void resolveSelectedBottleneck();
```
Attempts to resolve the currently selected bottleneck.

### Signals

```cpp
void bottleneckSelected(const QString& description);
void resolutionRequested(const QString& bottleneck);
void refreshRequested();
```

## PerformanceProfilerWidget Class

Control panel for managing performance profiling sessions.

### Header

```cpp
#include "Debug/PerformanceProfilerWidget.hpp"
```

### Methods

#### Profiling Control

```cpp
void startProfiling();
```
Starts a new profiling session.

```cpp
void stopProfiling();
```
Stops the current profiling session.

```cpp
void resetProfiling();
```
Resets profiling data and state.

#### Data Export

```cpp
void exportProfile(const QString& fileName);
```
Exports collected profile data to JSON file.

#### Status Queries

```cpp
bool isProfiling() const;
```
Returns true if profiling is currently active.

```cpp
qint64 getProfilingDuration() const;
```
Returns the duration of the current/last profiling session in milliseconds.

### Signals

```cpp
void profilingStarted();
void profilingStopped();
void profileExported(const QString& fileName);
void progressUpdated(int percentage);
```

## Data Structures

### PerformanceDataPoint

```cpp
struct PerformanceDataPoint {
    double cpu_usage = 0.0;           // CPU usage percentage (0-100)
    size_t memory_usage_mb = 0;       // Memory usage in megabytes
    double frame_rate = 0.0;          // Frames per second
    QDateTime timestamp;              // When the measurement was taken
    
    // Additional metrics
    double render_time_ms = 0.0;      // Rendering time in milliseconds
    int active_animations = 0;        // Number of active animations
    size_t widget_count = 0;          // Total widget count
    
    // I/O metrics
    size_t disk_read_mb = 0;          // Disk read in MB
    size_t disk_write_mb = 0;         // Disk write in MB
    size_t network_in_kb = 0;         // Network input in KB
    size_t network_out_kb = 0;        // Network output in KB
};
```

### BottleneckInfo

```cpp
struct BottleneckInfo {
    QString description;              // Human-readable description
    QString severity;                 // "HIGH", "MEDIUM", "LOW"
    QString category;                 // "CPU", "MEMORY", "IO", "RENDER"
    QString suggestion;               // Suggested resolution
    QDateTime detected_at;            // When it was detected
    bool resolved = false;            // Resolution status
    
    // Additional context
    QString component;                // Component that caused the bottleneck
    QVariantMap metrics;              // Associated performance metrics
    QString resolution_action;        // Action taken to resolve (if any)
};
```

### MemoryAllocationInfo

```cpp
struct MemoryAllocationInfo {
    QString component;                // Component that allocated memory
    size_t size_bytes;               // Allocation size in bytes
    QDateTime allocated_at;          // When it was allocated
    QString stack_trace;             // Stack trace (if available)
    bool is_leaked = false;          // Whether this is a potential leak
};
```

## Usage Examples

### Basic Dashboard Setup

```cpp
#include "Debug/ProfilerDashboard.hpp"

auto dashboard = std::make_unique<ProfilerDashboard>();

// Configure thresholds
dashboard->setPerformanceThresholds(
    80.0,   // CPU threshold (80%)
    512,    // Memory threshold (512 MB)
    30.0    // Frame rate threshold (30 FPS)
);

// Connect to performance alerts
connect(dashboard.get(), &ProfilerDashboard::performanceAlert,
        [](const QString& metric, double value) {
            qWarning() << "Performance alert:" << metric << "=" << value;
        });

dashboard->show();
```

### Real-time Performance Monitoring

```cpp
// Create performance monitoring timer
auto timer = new QTimer(this);
connect(timer, &QTimer::timeout, [dashboard]() {
    PerformanceDataPoint data;
    data.cpu_usage = getCurrentCPUUsage();
    data.memory_usage_mb = getCurrentMemoryUsage();
    data.frame_rate = getCurrentFrameRate();
    data.timestamp = QDateTime::currentDateTime();
    
    dashboard->updatePerformanceData(data);
});

timer->start(1000);  // Update every second
```

### Debugging Console Integration

```cpp
#include "Debug/DebuggingConsole.hpp"

auto console = std::make_unique<DebuggingConsole>();

// Connect to Qt logging system
qInstallMessageHandler([](QtMsgType type, const QMessageLogContext& context, const QString& msg) {
    QString level;
    switch (type) {
        case QtDebugMsg: level = "DEBUG"; break;
        case QtWarningMsg: level = "WARNING"; break;
        case QtCriticalMsg: level = "ERROR"; break;
        case QtFatalMsg: level = "FATAL"; break;
    }
    
    console->addLogMessage(level, context.category, msg);
});

// Add to main window
mainWindow->addDockWidget(Qt::BottomDockWidgetArea, console.get());
```

### Bottleneck Detection

```cpp
#include "Debug/BottleneckDetectorWidget.hpp"

auto detector = std::make_unique<BottleneckDetectorWidget>();

// Connect to resolution requests
connect(detector.get(), &BottleneckDetectorWidget::resolutionRequested,
        [](const QString& bottleneck) {
            // Implement resolution logic based on bottleneck type
            if (bottleneck.contains("memory")) {
                performMemoryCleanup();
            } else if (bottleneck.contains("CPU")) {
                reduceAnimationQuality();
            }
        });

// Add bottlenecks programmatically
detector->addBottleneck("High CPU usage in animation loop", "HIGH");
detector->addBottleneck("Memory leak in component creation", "MEDIUM");
```

### Performance Profiling Session

```cpp
#include "Debug/PerformanceProfilerWidget.hpp"

auto profiler = std::make_unique<PerformanceProfilerWidget>();

// Start profiling when needed
connect(startButton, &QPushButton::clicked, [profiler]() {
    profiler->startProfiling();
});

// Stop and export after duration
connect(profiler.get(), &PerformanceProfilerWidget::profilingStarted, []() {
    QTimer::singleShot(30000, [profiler]() {  // Profile for 30 seconds
        profiler->stopProfiling();
        profiler->exportProfile("performance_profile.json");
    });
});
```

### Memory Monitoring

```cpp
// Monitor memory allocations
void trackMemoryAllocation(const QString& component, size_t size) {
    MemoryAllocationInfo info;
    info.component = component;
    info.size_bytes = size;
    info.allocated_at = QDateTime::currentDateTime();
    
    // Add to dashboard memory tracking
    dashboard->addMemoryAllocation(info);
}

// Detect memory leaks
void detectMemoryLeaks() {
    auto leaks = dashboard->detectMemoryLeaks();
    for (const auto& leak : leaks) {
        BottleneckInfo bottleneck;
        bottleneck.description = QString("Memory leak in %1").arg(leak.component);
        bottleneck.severity = "HIGH";
        bottleneck.category = "MEMORY";
        bottleneck.suggestion = "Review component lifecycle management";
        
        dashboard->addBottleneck(bottleneck);
    }
}
```

## Integration with Other Modules

### Animation Performance Monitoring

```cpp
// Monitor animation performance
connect(&AnimationEngine::instance(), &AnimationEngine::performanceAlert,
        [dashboard](const QString& metric, double value) {
            BottleneckInfo bottleneck;
            bottleneck.description = QString("Animation performance: %1 = %2").arg(metric).arg(value);
            bottleneck.severity = value > 50 ? "HIGH" : "MEDIUM";
            bottleneck.category = "RENDER";
            dashboard->addBottleneck(bottleneck);
        });
```

### Hot Reload Integration

```cpp
// Log hot reload events
connect(&HotReloadManager::instance(), &HotReloadManager::reloadCompleted,
        [console](const QString& file) {
            console->addLogMessage("INFO", "HotReload", 
                                 QString("Successfully reloaded: %1").arg(file));
        });

connect(&HotReloadManager::instance(), &HotReloadManager::reloadFailed,
        [console](const QString& file, const QString& error) {
            console->addLogMessage("ERROR", "HotReload", 
                                 QString("Failed to reload %1: %2").arg(file, error));
        });
```

### State Management Debugging

```cpp
// Monitor state changes
connect(&StateManager::instance(), &StateManager::stateChanged,
        [console](const QString& key, const QVariant& value) {
            console->addLogMessage("DEBUG", "StateManager", 
                                 QString("State changed: %1 = %2").arg(key, value.toString()));
        });
```

## Build Configuration

Debug tools can be conditionally compiled:

```cmake
option(BUILD_DEBUG_TOOLS "Build debugging and profiling tools" ON)

if(BUILD_DEBUG_TOOLS)
    target_compile_definitions(DeclarativeUI PUBLIC DECLARATIVE_UI_DEBUG_TOOLS)
endif()
```

Use in code:

```cpp
#ifdef DECLARATIVE_UI_DEBUG_TOOLS
    auto dashboard = std::make_unique<ProfilerDashboard>();
    dashboard->show();
#endif
```
