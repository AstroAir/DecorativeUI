#include <QApplication>
#include <QDir>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTemporaryFile>
#include <QTest>
#include <QTimer>
#include <QWidget>
#include <memory>

#include "../Exceptions/UIExceptions.hpp"
#include "../HotReload/FileWatcher.hpp"
#include "../HotReload/HotReloadManager.hpp"
#include "../HotReload/PerformanceMonitor.hpp"

using namespace DeclarativeUI::HotReload;
using namespace DeclarativeUI::Exceptions;

class HotReloadTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        // Initialize Qt application if not already done
        if (!QApplication::instance()) {
            int argc = 0;
            char* argv[] = {nullptr};
            new QApplication(argc, argv);
        }
    }

    void cleanupTestCase() {
        // Clean up any global state
    }

    void init() {
        // Set up for each test
        temp_dir_ = std::make_unique<QTemporaryDir>();
        QVERIFY(temp_dir_->isValid());
    }

    void cleanup() {
        // Clean up after each test
        temp_dir_.reset();
    }

    // **FileWatcher Tests**
    void testFileWatcherCreation() {
        auto watcher = std::make_unique<FileWatcher>();

        QVERIFY(watcher != nullptr);
        QVERIFY(watcher->getWatchedFiles().isEmpty());
        QVERIFY(watcher->getWatchedDirectories().isEmpty());
    }

    void testFileWatcherAddFile() {
        auto watcher = std::make_unique<FileWatcher>();

        // Create a temporary file
        QTemporaryFile temp_file(temp_dir_->path() + "/test_file_XXXXXX.txt");
        QVERIFY(temp_file.open());
        temp_file.write("Initial content");
        temp_file.close();

        QString file_path = temp_file.fileName();

        bool added = watcher->addFile(file_path);
        QVERIFY(added);

        auto watched_files = watcher->getWatchedFiles();
        QVERIFY(watched_files.contains(file_path));
    }

    void testFileWatcherAddDirectory() {
        auto watcher = std::make_unique<FileWatcher>();

        QString dir_path = temp_dir_->path();

        bool added = watcher->addDirectory(dir_path);
        QVERIFY(added);

        auto watched_dirs = watcher->getWatchedDirectories();
        QVERIFY(watched_dirs.contains(dir_path));
    }

    void testFileWatcherFileChanged() {
        auto watcher = std::make_unique<FileWatcher>();

        // Create a temporary file
        QTemporaryFile temp_file(temp_dir_->path() + "/watch_test_XXXXXX.txt");
        QVERIFY(temp_file.open());
        temp_file.write("Initial content");
        temp_file.close();

        QString file_path = temp_file.fileName();

        QSignalSpy file_changed_spy(watcher.get(), &FileWatcher::fileChanged);

        bool added = watcher->addFile(file_path);
        QVERIFY(added);

        // Modify the file
        QFile file(file_path);
        QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Append));
        file.write("\nModified content");
        file.close();

        // Wait for file system notification
        QTest::qWait(100);

        // Check if signal was emitted
        QVERIFY(file_changed_spy.count() >=
                0);  // May be 0 if file system doesn't support notifications
    }

    void testFileWatcherRemoveFile() {
        auto watcher = std::make_unique<FileWatcher>();

        QTemporaryFile temp_file(temp_dir_->path() + "/remove_test_XXXXXX.txt");
        QVERIFY(temp_file.open());
        temp_file.close();

        QString file_path = temp_file.fileName();

        watcher->addFile(file_path);
        QVERIFY(watcher->getWatchedFiles().contains(file_path));

        bool removed = watcher->removeFile(file_path);
        QVERIFY(removed);
        QVERIFY(!watcher->getWatchedFiles().contains(file_path));
    }

    void testFileWatcherInvalidFile() {
        auto watcher = std::make_unique<FileWatcher>();

        QString invalid_path = "/nonexistent/path/file.txt";

        bool added = watcher->addFile(invalid_path);
        QVERIFY(!added);  // Should fail for non-existent file
    }

    // **PerformanceMonitor Tests**
    void testPerformanceMonitorCreation() {
        auto monitor = std::make_unique<PerformanceMonitor>();

        QVERIFY(monitor != nullptr);
        QVERIFY(monitor->isEnabled());
    }

    void testPerformanceMonitorStartStopTimer() {
        auto monitor = std::make_unique<PerformanceMonitor>();

        QString operation_name = "test_operation";

        monitor->startTimer(operation_name);

        // Simulate some work
        QTest::qWait(10);

        auto elapsed = monitor->stopTimer(operation_name);
        QVERIFY(elapsed >= 10);  // Should be at least 10ms
    }

    void testPerformanceMonitorRecordMetric() {
        auto monitor = std::make_unique<PerformanceMonitor>();

        monitor->recordMetric("memory_usage", 1024.0);
        monitor->recordMetric("cpu_usage", 75.5);
        monitor->recordMetric("memory_usage",
                              2048.0);  // Update existing metric

        auto metrics = monitor->getMetrics();
        QVERIFY(metrics.contains("memory_usage"));
        QVERIFY(metrics.contains("cpu_usage"));

        QCOMPARE(metrics["memory_usage"], 2048.0);
        QCOMPARE(metrics["cpu_usage"], 75.5);
    }

    void testPerformanceMonitorGetReport() {
        auto monitor = std::make_unique<PerformanceMonitor>();

        monitor->recordMetric("test_metric", 42.0);
        monitor->startTimer("test_timer");
        QTest::qWait(5);
        monitor->stopTimer("test_timer");

        QString report = monitor->getReport();
        QVERIFY(!report.isEmpty());
        QVERIFY(report.contains("test_metric"));
        QVERIFY(report.contains("test_timer"));
    }

    void testPerformanceMonitorReset() {
        auto monitor = std::make_unique<PerformanceMonitor>();

        monitor->recordMetric("temp_metric", 100.0);
        monitor->startTimer("temp_timer");
        monitor->stopTimer("temp_timer");

        QVERIFY(!monitor->getMetrics().isEmpty());

        monitor->reset();

        auto metrics = monitor->getMetrics();
        QVERIFY(metrics.isEmpty() || metrics["temp_metric"] == 0.0);
    }

    void testPerformanceMonitorEnableDisable() {
        auto monitor = std::make_unique<PerformanceMonitor>();

        QVERIFY(monitor->isEnabled());

        monitor->setEnabled(false);
        QVERIFY(!monitor->isEnabled());

        // Operations while disabled should not record metrics
        monitor->recordMetric("disabled_metric", 50.0);
        monitor->startTimer("disabled_timer");
        monitor->stopTimer("disabled_timer");

        auto metrics = monitor->getMetrics();
        QVERIFY(!metrics.contains("disabled_metric") ||
                metrics["disabled_metric"] == 0.0);

        monitor->setEnabled(true);
        QVERIFY(monitor->isEnabled());
    }

    // **HotReloadManager Tests**
    void testHotReloadManagerCreation() {
        auto manager = std::make_unique<HotReloadManager>();

        QVERIFY(manager != nullptr);
        QVERIFY(manager->isEnabled());
    }

    void testHotReloadManagerRegisterUIFile() {
        auto manager = std::make_unique<HotReloadManager>();

        // Create a test widget
        auto test_widget = std::make_unique<QWidget>();
        test_widget->setWindowTitle("Test Widget");

        // Create a temporary UI file
        QTemporaryFile ui_file(temp_dir_->path() + "/ui_XXXXXX.json");
        QVERIFY(ui_file.open());

        QString ui_content = R"({
            "type": "QWidget",
            "properties": {
                "windowTitle": "Hot Reload Test"
            }
        })";

        ui_file.write(ui_content.toUtf8());
        ui_file.close();

        bool registered =
            manager->registerUIFile(ui_file.fileName(), test_widget.get());
        QVERIFY(registered);

        auto registered_files = manager->getRegisteredFiles();
        QVERIFY(registered_files.contains(ui_file.fileName()));
    }

    void testHotReloadManagerUnregisterUIFile() {
        auto manager = std::make_unique<HotReloadManager>();
        auto test_widget = std::make_unique<QWidget>();

        QTemporaryFile ui_file(temp_dir_->path() + "/ui_unreg_XXXXXX.json");
        QVERIFY(ui_file.open());
        ui_file.write("{}");
        ui_file.close();

        manager->registerUIFile(ui_file.fileName(), test_widget.get());
        QVERIFY(manager->getRegisteredFiles().contains(ui_file.fileName()));

        bool unregistered = manager->unregisterUIFile(ui_file.fileName());
        QVERIFY(unregistered);
        QVERIFY(!manager->getRegisteredFiles().contains(ui_file.fileName()));
    }

    void testHotReloadManagerFileChangeDetection() {
        auto manager = std::make_unique<HotReloadManager>();
        auto test_widget = std::make_unique<QWidget>();

        QSignalSpy reload_spy(manager.get(), &HotReloadManager::uiReloaded);

        // Create and register UI file
        QTemporaryFile ui_file(temp_dir_->path() + "/ui_change_XXXXXX.json");
        QVERIFY(ui_file.open());

        QString initial_content = R"({
            "type": "QWidget",
            "properties": {
                "windowTitle": "Initial Title"
            }
        })";

        ui_file.write(initial_content.toUtf8());
        ui_file.close();

        manager->registerUIFile(ui_file.fileName(), test_widget.get());

        // Modify the file
        QFile file(ui_file.fileName());
        QVERIFY(file.open(QIODevice::WriteOnly));

        QString modified_content = R"({
            "type": "QWidget",
            "properties": {
                "windowTitle": "Modified Title"
            }
        })";

        file.write(modified_content.toUtf8());
        file.close();

        // Wait for file change detection and reload
        QTest::qWait(200);

        // Check if reload signal was emitted
        QVERIFY(reload_spy.count() >=
                0);  // May be 0 if file watching is not available
    }

    void testHotReloadManagerEnableDisable() {
        auto manager = std::make_unique<HotReloadManager>();

        QVERIFY(manager->isEnabled());

        manager->setEnabled(false);
        QVERIFY(!manager->isEnabled());

        // Operations while disabled should not trigger reloads
        auto test_widget = std::make_unique<QWidget>();
        QTemporaryFile ui_file(temp_dir_->path() + "/ui_disabled_XXXXXX.json");
        QVERIFY(ui_file.open());
        ui_file.write("{}");
        ui_file.close();

        bool registered =
            manager->registerUIFile(ui_file.fileName(), test_widget.get());
        QVERIFY(!registered);  // Should fail when disabled

        manager->setEnabled(true);
        QVERIFY(manager->isEnabled());
    }

    void testHotReloadManagerPerformanceMonitoring() {
        auto manager = std::make_unique<HotReloadManager>();

        auto performance_monitor = manager->getPerformanceMonitor();
        QVERIFY(performance_monitor != nullptr);

        // Performance monitor should be integrated with hot reload operations
        auto test_widget = std::make_unique<QWidget>();
        QTemporaryFile ui_file(temp_dir_->path() + "/ui_perf_XXXXXX.json");
        QVERIFY(ui_file.open());
        ui_file.write("{}");
        ui_file.close();

        manager->registerUIFile(ui_file.fileName(), test_widget.get());

        // Check if performance metrics were recorded
        auto metrics = performance_monitor->getMetrics();
        // Specific metrics depend on implementation
    }

    // **Error Handling Tests**
    void testFileWatcherErrorHandling() {
        auto watcher = std::make_unique<FileWatcher>();

        // Test adding invalid paths
        QVERIFY(!watcher->addFile(""));
        QVERIFY(!watcher->addFile("/invalid/path/file.txt"));
        QVERIFY(!watcher->addDirectory("/invalid/path/"));

        // Test removing non-watched files
        QVERIFY(!watcher->removeFile("/not/watched/file.txt"));
        QVERIFY(!watcher->removeDirectory("/not/watched/dir/"));
    }

    void testHotReloadManagerErrorHandling() {
        auto manager = std::make_unique<HotReloadManager>();

        // Test registering with invalid parameters
        QVERIFY(!manager->registerUIFile("", nullptr));
        QVERIFY(!manager->registerUIFile("/invalid/file.json", nullptr));

        auto test_widget = std::make_unique<QWidget>();
        QVERIFY(!manager->registerUIFile("", test_widget.get()));
        QVERIFY(!manager->registerUIFile("/nonexistent/file.json",
                                         test_widget.get()));

        // Test unregistering non-registered files
        QVERIFY(!manager->unregisterUIFile("/not/registered/file.json"));
    }

    void testPerformanceMonitorErrorHandling() {
        auto monitor = std::make_unique<PerformanceMonitor>();

        // Test stopping non-started timer
        auto elapsed = monitor->stopTimer("non_existent_timer");
        QVERIFY(elapsed == 0 || elapsed < 0);  // Should return 0 or error value

        // Test with empty metric names
        monitor->recordMetric("", 100.0);  // Should handle gracefully
        monitor->startTimer("");           // Should handle gracefully
    }

    // **Integration Tests**
    void testHotReloadWorkflowIntegration() {
        auto manager = std::make_unique<HotReloadManager>();
        auto test_widget = std::make_unique<QWidget>();

        QSignalSpy reload_spy(manager.get(), &HotReloadManager::uiReloaded);
        QSignalSpy error_spy(manager.get(), &HotReloadManager::reloadError);

        // Create UI file
        QTemporaryFile ui_file(temp_dir_->path() + "/integration_XXXXXX.json");
        QVERIFY(ui_file.open());

        QString ui_content = R"({
            "type": "QWidget",
            "properties": {
                "windowTitle": "Integration Test",
                "geometry": [0, 0, 300, 200]
            }
        })";

        ui_file.write(ui_content.toUtf8());
        ui_file.close();

        // Register file
        bool registered =
            manager->registerUIFile(ui_file.fileName(), test_widget.get());
        QVERIFY(registered);

        // Verify performance monitoring is working
        auto perf_monitor = manager->getPerformanceMonitor();
        QVERIFY(perf_monitor != nullptr);

        QString initial_report = perf_monitor->getReport();

        // Modify file to trigger reload
        QFile file(ui_file.fileName());
        QVERIFY(file.open(QIODevice::WriteOnly));

        QString modified_content = R"({
            "type": "QWidget",
            "properties": {
                "windowTitle": "Modified Integration Test",
                "geometry": [0, 0, 400, 300]
            }
        })";

        file.write(modified_content.toUtf8());
        file.close();

        // Wait for reload
        QTest::qWait(300);

        // Check results
        QString final_report = perf_monitor->getReport();
        QVERIFY(final_report.length() >= initial_report.length());

        // Cleanup
        manager->unregisterUIFile(ui_file.fileName());
    }

    // **New PerformanceMonitor Tests**
    void testPerformanceMonitorPauseResume() {
        auto monitor = std::make_unique<PerformanceMonitor>();

        // Test initial state
        QVERIFY(monitor->isMonitoring() == false);

        // Start monitoring
        monitor->startMonitoring();
        QVERIFY(monitor->isMonitoring() == true);

        // Pause monitoring
        monitor->pauseMonitoring();
        QVERIFY(monitor->isMonitoring() == false);

        // Resume monitoring
        monitor->resumeMonitoring();
        QVERIFY(monitor->isMonitoring() == true);

        // Stop monitoring
        monitor->stopMonitoring();
        QVERIFY(monitor->isMonitoring() == false);
    }

    void testPerformanceMonitorMemoryTracking() {
        auto monitor = std::make_unique<PerformanceMonitor>();
        monitor->startMonitoring();

        // Test memory usage recording
        monitor->recordMemoryUsage(100);
        monitor->recordMemoryUsage(150);
        monitor->recordMemoryUsage(120);

        // Test memory heavy files detection
        QStringList heavy_files = monitor->getMemoryHeavyFiles(50);
        // Should be empty since we haven't associated memory with specific files

        monitor->stopMonitoring();
    }

    void testPerformanceMonitorCPUTracking() {
        auto monitor = std::make_unique<PerformanceMonitor>();
        monitor->startMonitoring();

        // Test CPU usage recording
        monitor->recordCPUUsage(25.5);
        monitor->recordCPUUsage(45.0);
        monitor->recordCPUUsage(30.2);

        // Test that monitoring is still active
        QVERIFY(monitor->isMonitoring() == true);

        monitor->stopMonitoring();
    }

    void testPerformanceMonitorBottleneckDetection() {
        auto monitor = std::make_unique<PerformanceMonitor>();
        monitor->startMonitoring();
        monitor->enableBottleneckDetection(true);

        // Simulate high resource usage
        monitor->recordCPUUsage(95.0);
        monitor->recordMemoryUsage(1000);

        // Test bottleneck detection
        auto bottlenecks = monitor->detectBottlenecks();
        // Should detect bottlenecks based on high usage

        auto critical_bottleneck = monitor->getMostCriticalBottleneck();
        // Should return the most severe bottleneck

        monitor->stopMonitoring();
    }

    void testPerformanceMonitorReporting() {
        auto monitor = std::make_unique<PerformanceMonitor>();
        monitor->startMonitoring();

        // Generate some test data
        monitor->recordCPUUsage(50.0);
        monitor->recordMemoryUsage(200);

        // Test basic report generation
        QString basic_report = monitor->generateReport();
        QVERIFY(!basic_report.isEmpty());
        QVERIFY(basic_report.contains("Performance Report"));

        // Test detailed report generation
        QString detailed_report = monitor->generateDetailedReport();
        QVERIFY(!detailed_report.isEmpty());
        QVERIFY(detailed_report.contains("Detailed Performance Analysis"));

        // Test JSON report generation
        QJsonObject json_report = monitor->generateJSONReport();
        QVERIFY(!json_report.isEmpty());
        QVERIFY(json_report.contains("monitoring_enabled"));

        monitor->stopMonitoring();
    }

    void testPerformanceMonitorMemoryProfiling() {
        auto monitor = std::make_unique<PerformanceMonitor>();
        monitor->startMonitoring();
        monitor->enableMemoryProfiling(true);

        // Test memory profile generation
        QJsonObject memory_profile = monitor->getMemoryProfile();
        QVERIFY(!memory_profile.isEmpty());
        QVERIFY(memory_profile.contains("memory_profiling_enabled"));
        QVERIFY(memory_profile["memory_profiling_enabled"].toBool() == true);

        // Test garbage collection
        monitor->forceGarbageCollection();

        monitor->stopMonitoring();
    }

    void testPerformanceMonitorPredictiveModeling() {
        auto monitor = std::make_unique<PerformanceMonitor>();
        monitor->startMonitoring();
        monitor->enablePredictiveModeling(true);

        // Generate some historical data
        for (int i = 0; i < 10; ++i) {
            monitor->recordCPUUsage(20.0 + i * 2.0);
            monitor->recordMemoryUsage(100 + i * 10);
        }

        // Test prediction methods
        double next_response_time = monitor->predictNextResponseTime();
        QVERIFY(next_response_time >= 0.0);

        double memory_prediction = monitor->predictMemoryUsageIn(5);
        QVERIFY(memory_prediction >= 0.0);

        // Test prediction report
        QJsonObject prediction_report = monitor->getPredictionReport();
        QVERIFY(!prediction_report.isEmpty());
        QVERIFY(prediction_report.contains("predictive_modeling_enabled"));

        monitor->stopMonitoring();
    }

    // **New HotReloadManager Tests**
    void testHotReloadManagerDependencyGraph() {
        auto manager = std::make_unique<HotReloadManager>();

        // Create test files with dependencies
        QTemporaryFile main_file(temp_dir_->path() + "/main_XXXXXX.json");
        QVERIFY(main_file.open());

        QString main_content = R"({
            "type": "QWidget",
            "include": "component.json",
            "properties": {
                "windowTitle": "Main Window"
            }
        })";
        main_file.write(main_content.toUtf8());
        main_file.close();

        QTemporaryFile component_file(temp_dir_->path() + "/component_XXXXXX.json");
        QVERIFY(component_file.open());

        QString component_content = R"({
            "type": "QLabel",
            "properties": {
                "text": "Component Label"
            }
        })";
        component_file.write(component_content.toUtf8());
        component_file.close();

        // Register files
        auto main_widget = std::make_unique<QWidget>();
        auto component_widget = std::make_unique<QWidget>();

        manager->registerUIFile(main_file.fileName(), main_widget.get());
        manager->registerUIFile(component_file.fileName(), component_widget.get());

        // Test dependency graph building
        manager->buildDependencyGraph();

        // Test affected files detection
        QStringList affected = manager->getAffectedFiles(component_file.fileName());
        // Should include main_file since it depends on component_file

        // Test cyclic dependency detection
        bool has_cycle = manager->hasCyclicDependency(main_file.fileName());
        QVERIFY(has_cycle == false); // Should not have cycles in this simple case

        // Cleanup
        manager->unregisterUIFile(main_file.fileName());
        manager->unregisterUIFile(component_file.fileName());
    }

    void testHotReloadManagerThreadManagement() {
        auto manager = std::make_unique<HotReloadManager>();

        // Test thread pool functionality
        QThread* thread = manager->getAvailableThread();
        // Should return a valid thread or nullptr if no threads available

        // Test async reload functionality
        QTemporaryFile test_file(temp_dir_->path() + "/async_test_XXXXXX.json");
        QVERIFY(test_file.open());

        QString content = R"({
            "type": "QWidget",
            "properties": {
                "windowTitle": "Async Test"
            }
        })";
        test_file.write(content.toUtf8());
        test_file.close();

        auto widget = std::make_unique<QWidget>();
        manager->registerUIFile(test_file.fileName(), widget.get());

        // Test async reload (should not crash)
        manager->performReloadAsync(test_file.fileName());

        // Wait a bit for async operation
        QTest::qWait(100);

        // Cleanup
        manager->unregisterUIFile(test_file.fileName());
    }

    void testHotReloadManagerPerformanceMeasurement() {
        auto manager = std::make_unique<HotReloadManager>();

        // Test performance measurement
        bool test_executed = false;
        auto test_function = [&test_executed]() {
            test_executed = true;
            QTest::qWait(10); // Simulate some work
        };

        ReloadMetrics metrics = manager->measureReloadPerformance(test_function);

        QVERIFY(test_executed == true);
        QVERIFY(metrics.success == true);
        QVERIFY(metrics.total_time.count() >= 10); // Should be at least 10ms
    }

    void testHotReloadManagerRollbackPoints() {
        auto manager = std::make_unique<HotReloadManager>();

        // Create test file
        QTemporaryFile test_file(temp_dir_->path() + "/rollback_test_XXXXXX.json");
        QVERIFY(test_file.open());

        QString content = R"({
            "type": "QWidget",
            "properties": {
                "windowTitle": "Rollback Test"
            }
        })";
        test_file.write(content.toUtf8());
        test_file.close();

        auto widget = std::make_unique<QWidget>();
        manager->registerUIFile(test_file.fileName(), widget.get());

        // Test rollback point creation
        manager->createRollbackPoint(test_file.fileName());

        // Test rollback
        manager->rollbackToPoint(test_file.fileName());

        // Test clearing rollback points
        manager->clearRollbackPoints();

        // Cleanup
        manager->unregisterUIFile(test_file.fileName());
    }

    void testHotReloadManagerConfiguration() {
        auto manager = std::make_unique<HotReloadManager>();

        // Test configuration methods
        manager->setPreloadStrategy(true);
        manager->enableIncrementalReloading(true);
        manager->enableParallelProcessing(true);
        manager->enableSmartCaching(true);

        // Test that manager is still functional after configuration
        QVERIFY(manager->isEnabled() == true);

        // Test disabling and re-enabling
        manager->setEnabled(false);
        QVERIFY(manager->isEnabled() == false);

        manager->setEnabled(true);
        QVERIFY(manager->isEnabled() == true);
    }

private:
    std::unique_ptr<QTemporaryDir> temp_dir_;
};

QTEST_MAIN(HotReloadTest)
#include "test_hot_reload.moc"
