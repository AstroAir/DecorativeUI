# HotReload Module

The HotReload module provides comprehensive hot-reload functionality for DeclarativeUI applications, enabling real-time UI updates during development without application restarts.

## Overview

This directory contains the core components for hot-reload functionality:

- **FileWatcher**: Advanced file system monitoring with debouncing and filtering
- **HotReloadManager**: Central orchestration of hot-reload operations
- **PerformanceMonitor**: Comprehensive performance tracking and analytics

## Components

### FileWatcher (`FileWatcher.hpp/.cpp`)

High-performance file and directory change detection with advanced features:

- Efficient monitoring of files and directories (recursive optional)
- Configurable debouncing strategies to coalesce noisy file system events
- Filtering by extension, glob-style patterns, file size and hidden flag
- Batch processing and thread-pool for off-main-thread work
- Adaptive heuristics to tune debounce intervals based on change frequency

**Key Features:**

- Cross-platform file system monitoring
- Adaptive debouncing based on change frequency
- Memory and performance optimization
- Thread-safe operations

### HotReloadManager (`HotReloadManager.hpp/.cpp`)

Central manager coordinating hot-reload lifecycle for JSON-driven UI:

- Register UI files and directories for hot-reload monitoring
- Maintain dependency graph for incremental reloads
- Schedule and execute reloads according to configured strategies
- Provide rollback/backup facilities for recovery after failed reloads
- Expose configuration setters for debounce, concurrency, memory limits

**Key Features:**

- **Dependency Management**: Automatic dependency graph building and cycle detection
- **Thread Management**: Multi-threaded reload operations with thread pool
- **Performance Measurement**: Built-in performance metrics collection
- **Safe Widget Replacement**: Backup and rollback mechanisms
- **Async Operations**: Non-blocking reload operations
- **Smart Caching**: Widget caching and preloading strategies

**Recently Implemented Methods:**

- `buildDependencyGraph()`: Analyzes JSON files for dependencies
- `updateDependencies()`: Updates dependency metadata
- `hasCyclicDependency()`: Detects circular dependencies
- `getAvailableThread()`: Thread pool management
- `measureReloadPerformance()`: Performance measurement wrapper
- `setPreloadStrategy()`: Configure dependency preloading
- `clearRollbackPoints()`: Cleanup rollback data
- `performReloadAsync()`: Asynchronous reload operations
- `replaceWidgetSafe()`: Safe widget replacement with validation
- `preloadDependencies()`: Preload dependent files
- `createWidgetFromCache()`: Create widgets from cache

### PerformanceMonitor (`PerformanceMonitor.hpp/.cpp`)

Comprehensive performance monitoring, analytics and optimization for hot-reload operations:

- Fine-grained breakdown of reload phases (parsing, validation, widget creation)
- Memory and CPU observations with peak and delta reporting
- Sliding-window real-time analytics and lightweight predictive models
- Bottleneck detection and human-friendly recommendations
- Automated optimization hooks and performance callbacks

**Key Features:**

- **Real-time Analytics**: Sliding window analytics with trend calculation
- **Predictive Modeling**: Simple forecasting for response times and memory usage
- **Bottleneck Detection**: Automatic detection of CPU, memory, and I/O bottlenecks
- **Memory Profiling**: Memory leak detection and garbage collection
- **Performance Optimization**: Automated optimization recommendations
- **Comprehensive Reporting**: Text, JSON, and detailed report generation

**Recently Implemented Methods:**

- `pauseMonitoring()` / `resumeMonitoring()`: Monitoring lifecycle control
- `recordMemoryUsage()` / `recordCPUUsage()`: Resource usage tracking
- `getPredictionReport()`: Predictive modeling results
- `detectBottlenecks()` / `getMostCriticalBottleneck()`: Bottleneck analysis
- `getMemoryProfile()`: Memory profiling data
- `forceGarbageCollection()`: Manual memory cleanup
- `generateDetailedReport()`: Comprehensive performance analysis
- `exportReportToFile()`: Export reports to text or JSON files
- `updateRealTimeAnalytics()`: Real-time data processing
- `updatePredictiveModels()`: Machine learning model updates
- Utility methods: `formatDuration()`, `formatMemorySize()`, `metricsToJson()`

## Usage Examples

### Basic Hot Reload Setup

```cpp
#include "HotReload/HotReloadManager.hpp"

auto manager = std::make_unique<HotReloadManager>();

// Register UI file for monitoring
QWidget* target_widget = new QWidget();
manager->registerUIFile("ui/main.json", target_widget);

// Configure reload strategy
manager->setReloadStrategy(ReloadStrategy::Smart);
manager->enableIncrementalReloading(true);

// Connect to reload events
connect(manager.get(), &HotReloadManager::reloadCompleted,
        [](const QString& file_path) {
            qDebug() << "Reload completed:" << file_path;
        });
```

### Performance Monitoring

```cpp
#include "HotReload/PerformanceMonitor.hpp"

auto monitor = std::make_unique<PerformanceMonitor>();

// Start monitoring with advanced features
monitor->startMonitoring();
monitor->enableRealTimeAnalytics(true);
monitor->enablePredictiveModeling(true);
monitor->enableBottleneckDetection(true);

// Record performance metrics
AdvancedPerformanceMetrics metrics;
metrics.total_time_ms = 150;
metrics.memory_peak_mb = 200;
monitor->recordReloadMetrics("ui/main.json", metrics);

// Generate reports
QString report = monitor->generateDetailedReport();
monitor->exportReportToFile("performance_report.json");
```

### Dependency Management

```cpp
// Build dependency graph
manager->buildDependencyGraph();

// Check for circular dependencies
if (manager->hasCyclicDependency("ui/main.json")) {
    qWarning() << "Circular dependency detected!";
}

// Get affected files when a dependency changes
QStringList affected = manager->getAffectedFiles("ui/component.json");
```

## Configuration

### Reload Strategies

- **Immediate**: Reload synchronously as soon as change is detected
- **Batched**: Collect changes and perform single reload for the batch
- **Incremental**: Reload only affected portions using dependency tracking
- **Smart**: Use heuristics to choose the most efficient approach

### Performance Tuning

- Configure debounce intervals to reduce reload frequency
- Set memory limits to prevent excessive resource usage
- Enable parallel processing for faster reloads
- Use smart caching to improve performance

## Thread Safety

All components are designed for concurrent use:

- Internal mutable containers are guarded by shared_mutex
- Atomic primitives are used for counters updated from hot paths
- Signal emission adheres to Qt threading rules

## Dependencies

- Qt6::Core
- Qt6::Widgets
- Qt6::Concurrent (for parallel processing)

## Testing

Comprehensive unit tests are available in `tests/unit/`:

- `test_hot_reload.cpp`: Basic functionality tests
- `test_hot_reload_advanced.cpp`: Advanced feature tests

Run tests with:

```bash
cd build
ctest -R HotReload
```
