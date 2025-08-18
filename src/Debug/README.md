# Debug Module

The Debug module provides comprehensive debugging and performance monitoring tools for DeclarativeUI applications. It includes interactive widgets for real-time diagnostics, performance profiling, bottleneck detection, and development debugging.

## Overview

This directory contains debugging and profiling tools designed for developers:

- **ProfilerDashboard**: Comprehensive performance monitoring dashboard
- **DebuggingConsole**: Interactive logging and debugging console
- **BottleneckDetectorWidget**: Performance bottleneck identification and resolution
- **PerformanceProfilerWidget**: Real-time performance profiling controls

## Key Features

### 📊 **Performance Monitoring**
- Real-time CPU, memory, and frame rate tracking
- Performance history and trend analysis
- Bottleneck detection with automated suggestions
- Export capabilities for detailed analysis

### 🐛 **Interactive Debugging**
- Live log viewing with filtering capabilities
- Component inspection and state monitoring
- Error tracking and exception handling
- Export logs for external analysis

### 🔍 **Bottleneck Detection**
- Automatic performance issue identification
- Suggested optimizations and resolutions
- Interactive resolution attempts
- Performance impact measurement

### ⚡ **Profiling Tools**
- Session-based performance profiling
- Detailed metrics collection
- JSON export for external tools
- Progress tracking and status updates

## Components

### ProfilerDashboard (`ProfilerDashboard.hpp/.cpp`)

Comprehensive performance monitoring dashboard with multiple tabs and real-time metrics.

**Key Features:**
- Multi-tab interface for different monitoring aspects
- Real-time performance data visualization
- Memory usage tracking and leak detection
- Bottleneck detection and resolution
- Export capabilities (JSON, text reports)

**Dashboard Tabs:**
- **Performance**: CPU, memory, frame rate monitoring
- **Memory**: Memory allocation tracking and leak detection
- **Bottlenecks**: Performance issue identification
- **Profiler**: Session-based profiling controls
- **Debug**: Live debugging console

**Core Methods:**
```cpp
class ProfilerDashboard : public QMainWindow {
public:
    explicit ProfilerDashboard(QWidget* parent = nullptr);
    
    // Data updates
    void updatePerformanceData(const PerformanceDataPoint& data);
    void addBottleneck(const BottleneckInfo& bottleneck);
    
    // Export functionality
    void exportReport(const QString& format, const QString& filename);
    
    // Configuration
    void setPerformanceThresholds(double cpu, size_t memory, double frameRate);
};
```

### DebuggingConsole (`DebuggingConsole.hpp/.cpp`)

Interactive debugging console for live log viewing and filtering.

**Key Features:**
- Real-time log message display with timestamps
- Filtering by log level (Error, Warning, Info, Debug)
- Automatic scrolling and memory management
- Export logs to file
- Thread-safe log message handling

**Core Methods:**
```cpp
class DebuggingConsole : public QWidget {
public:
    explicit DebuggingConsole(QWidget* parent = nullptr);
    
    // Log management
    void appendLogLine(const QString& text);
    void setLogText(const QString& full_text);
    void addLogMessage(const QString& level, const QString& component, const QString& message);
    void clearLog();
    
    // Filtering
    void setFilter(const QString& filter);
    
signals:
    void exportRequested();
    void filterChanged(const QString& filter);
};
```

### BottleneckDetectorWidget (`BottleneckDetectorWidget.hpp/.cpp`)

Interactive widget for displaying and resolving performance bottlenecks.

**Key Features:**
- List of detected bottlenecks with descriptions
- Interactive selection and detailed information
- Automated resolution attempts
- Refresh capability for updated detection
- Visual indicators for resolved issues

**Core Methods:**
```cpp
class BottleneckDetectorWidget : public QWidget {
public:
    explicit BottleneckDetectorWidget(QWidget* parent = nullptr);
    
    // Bottleneck management
    void addBottleneck(const QString& description, const QString& severity);
    void clearBottlenecks();
    void refreshBottlenecks();
    
    // Resolution
    void resolveSelectedBottleneck();
    
signals:
    void bottleneckSelected(const QString& description);
    void resolutionRequested(const QString& bottleneck);
};
```

### PerformanceProfilerWidget (`PerformanceProfilerWidget.hpp/.cpp`)

Control panel for managing performance profiling sessions.

**Key Features:**
- Start/stop profiling session controls
- Real-time progress tracking
- Session duration monitoring
- Profile data export to JSON
- Reset and clear functionality

**Core Methods:**
```cpp
class PerformanceProfilerWidget : public QWidget {
public:
    explicit PerformanceProfilerWidget(QWidget* parent = nullptr);
    
    // Profiling control
    void startProfiling();
    void stopProfiling();
    void resetProfiling();
    
    // Data export
    void exportProfile(const QString& fileName);
    
    // Status
    bool isProfiling() const;
    qint64 getProfilingDuration() const;
    
signals:
    void profilingStarted();
    void profilingStopped();
    void profileExported(const QString& fileName);
};
```

## Usage Examples

### Basic Dashboard Setup

```cpp
#include "Debug/ProfilerDashboard.hpp"

// Create and configure dashboard
auto dashboard = std::make_unique<ProfilerDashboard>();
dashboard->setPerformanceThresholds(80.0, 512, 30.0);  // CPU%, Memory MB, FPS
dashboard->show();

// Update with performance data
PerformanceDataPoint data;
data.cpu_usage = 45.2;
data.memory_usage_mb = 256;
data.frame_rate = 60.0;
data.timestamp = QDateTime::currentDateTime();

dashboard->updatePerformanceData(data);
```

### Debugging Console Integration

```cpp
#include "Debug/DebuggingConsole.hpp"

// Create debugging console
auto console = std::make_unique<DebuggingConsole>();

// Connect to application logging
connect(qApp, &QApplication::aboutToQuit, [&console]() {
    console->addLogMessage("INFO", "Application", "Shutting down");
});

// Add to main window or dock
mainWindow->addDockWidget(Qt::BottomDockWidgetArea, 
                         new QDockWidget("Debug Console", console.get()));

// Log messages from anywhere in the application
console->addLogMessage("ERROR", "Network", "Connection failed");
console->addLogMessage("WARNING", "UI", "Component not found");
console->addLogMessage("INFO", "Core", "Initialization complete");
```

### Bottleneck Detection

```cpp
#include "Debug/BottleneckDetectorWidget.hpp"

auto detector = std::make_unique<BottleneckDetectorWidget>();

// Connect to resolution requests
connect(detector.get(), &BottleneckDetectorWidget::resolutionRequested,
        [](const QString& bottleneck) {
            qDebug() << "Attempting to resolve:" << bottleneck;
            // Implement resolution logic
        });

// Add detected bottlenecks
detector->addBottleneck("High CPU usage in animation loop", "HIGH");
detector->addBottleneck("Memory leak in component creation", "MEDIUM");
detector->addBottleneck("Slow JSON parsing", "LOW");
```

### Performance Profiling Session

```cpp
#include "Debug/PerformanceProfilerWidget.hpp"

auto profiler = std::make_unique<PerformanceProfilerWidget>();

// Connect to profiling events
connect(profiler.get(), &PerformanceProfilerWidget::profilingStarted,
        []() {
            qDebug() << "Profiling session started";
            // Begin detailed performance monitoring
        });

connect(profiler.get(), &PerformanceProfilerWidget::profilingStopped,
        []() {
            qDebug() << "Profiling session stopped";
            // Process collected data
        });

// Start profiling session
profiler->startProfiling();

// Later, stop and export
QTimer::singleShot(30000, [&profiler]() {  // Profile for 30 seconds
    profiler->stopProfiling();
    profiler->exportProfile("profile_session.json");
});
```

### Integration with Main Application

```cpp
#include "Debug/ProfilerDashboard.hpp"

class MainWindow : public QMainWindow {
private:
    std::unique_ptr<ProfilerDashboard> debugDashboard_;
    
public:
    MainWindow() {
        setupDebugTools();
    }
    
private:
    void setupDebugTools() {
        #ifdef QT_DEBUG
        // Only include debug tools in debug builds
        debugDashboard_ = std::make_unique<ProfilerDashboard>();
        
        // Add to menu
        auto debugMenu = menuBar()->addMenu("Debug");
        auto showDashboardAction = debugMenu->addAction("Show Performance Dashboard");
        connect(showDashboardAction, &QAction::triggered,
                [this]() { debugDashboard_->show(); });
        
        // Connect to application performance monitoring
        auto timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &MainWindow::updatePerformanceMetrics);
        timer->start(1000);  // Update every second
        #endif
    }
    
    void updatePerformanceMetrics() {
        PerformanceDataPoint data;
        data.cpu_usage = getCurrentCPUUsage();
        data.memory_usage_mb = getCurrentMemoryUsage();
        data.frame_rate = getCurrentFrameRate();
        data.timestamp = QDateTime::currentDateTime();
        
        debugDashboard_->updatePerformanceData(data);
    }
};
```

## Data Structures

### PerformanceDataPoint

```cpp
struct PerformanceDataPoint {
    double cpu_usage = 0.0;           // CPU usage percentage
    size_t memory_usage_mb = 0;       // Memory usage in MB
    double frame_rate = 0.0;          // Frames per second
    QDateTime timestamp;              // When the measurement was taken
    
    // Additional metrics
    double render_time_ms = 0.0;      // Rendering time
    int active_animations = 0;        // Number of active animations
    size_t widget_count = 0;          // Total widget count
};
```

### BottleneckInfo

```cpp
struct BottleneckInfo {
    QString description;              // Human-readable description
    QString severity;                 // HIGH, MEDIUM, LOW
    QString category;                 // CPU, MEMORY, IO, RENDER
    QString suggestion;               // Suggested resolution
    QDateTime detected_at;            // When it was detected
    bool resolved = false;            // Resolution status
};
```

## Performance Considerations

- **Debug Builds Only**: Most debug tools should only be enabled in debug builds
- **Sampling Rate**: Adjust monitoring frequency based on performance impact
- **Memory Management**: Debug tools automatically manage memory usage
- **Thread Safety**: All components are thread-safe for cross-thread logging

## Integration with Other Modules

### HotReload Integration

```cpp
// Connect debug console to hot reload events
connect(&HotReloadManager::instance(), &HotReloadManager::reloadCompleted,
        [&console](const QString& file) {
            console->addLogMessage("INFO", "HotReload", 
                                 QString("Reloaded: %1").arg(file));
        });
```

### Animation Performance Monitoring

```cpp
// Monitor animation performance
connect(&AnimationEngine::instance(), &AnimationEngine::performanceAlert,
        [&dashboard](const QString& metric, double value) {
            BottleneckInfo bottleneck;
            bottleneck.description = QString("Animation %1: %2").arg(metric).arg(value);
            bottleneck.severity = "MEDIUM";
            bottleneck.category = "RENDER";
            dashboard->addBottleneck(bottleneck);
        });
```

## Dependencies

- **Qt6**: Core, Widgets modules for UI components
- **Internal**: Integration with other DeclarativeUI modules
- **JSON**: For data export and configuration

## Testing

Debug components have specialized tests:

```bash
cd build
ctest -R Debug
```

## Build Configuration

Debug tools can be conditionally compiled:

```cmake
option(BUILD_DEBUG_TOOLS "Build debugging and profiling tools" ON)

if(BUILD_DEBUG_TOOLS)
    target_compile_definitions(DeclarativeUI PUBLIC DECLARATIVE_UI_DEBUG_TOOLS)
endif()
```

## Future Enhancements

- **Remote Debugging**: Network-based debugging capabilities
- **Visual Profiler**: Graphical performance visualization
- **Memory Profiler**: Advanced memory leak detection
- **Network Monitor**: HTTP request/response monitoring
