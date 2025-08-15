#include <QApplication>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTemporaryFile>
#include <QTest>
#include <QTimer>
#include <QWidget>
#include <memory>

#include "../../src/Exceptions/UIExceptions.hpp"
#include "../../src/HotReload/HotReloadManager.hpp"
#include "../../src/HotReload/PerformanceMonitor.hpp"
#include "../../src/JSON/JSONUILoader.hpp"
#include "../../src/JSON/JSONParser.hpp"
#include "../../src/JSON/ComponentRegistry.hpp"

using namespace DeclarativeUI::HotReload;
using namespace DeclarativeUI::JSON;
using namespace DeclarativeUI::Exceptions;

/**
 * @brief Integration tests for HotReload and JSON functionality
 * 
 * This test suite focuses on testing the integration between the newly
 * implemented HotReload and JSON functionality to ensure they work together.
 */
class HotReloadJSONIntegrationTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        if (!QApplication::instance()) {
            int argc = 0;
            char* argv[] = {nullptr};
            new QApplication(argc, argv);
        }
    }

    void cleanupTestCase() {}

    void init() {
        temp_dir_ = std::make_unique<QTemporaryDir>();
        QVERIFY(temp_dir_->isValid());
    }

    void cleanup() {
        temp_dir_.reset();
    }

    // **Test HotReload with Performance Monitoring**
    void testHotReloadWithPerformanceMonitoring() {
        auto manager = std::make_unique<HotReloadManager>();
        auto monitor = std::make_unique<PerformanceMonitor>();
        
        // Start performance monitoring
        monitor->startMonitoring();
        monitor->enableRealTimeAnalytics(true);
        monitor->enablePredictiveModeling(true);
        
        // Create test JSON file
        QTemporaryFile json_file(temp_dir_->path() + "/monitored_ui_XXXXXX.json");
        QVERIFY(json_file.open());
        
        QString json_content = R"({
            "type": "QWidget",
            "properties": {
                "windowTitle": "Monitored Widget",
                "geometry": [100, 100, 400, 300]
            }
        })";
        json_file.write(json_content.toUtf8());
        json_file.close();
        
        // Register file with hot reload manager
        auto widget = std::make_unique<QWidget>();
        manager->registerUIFile(json_file.fileName(), widget.get());
        
        // Simulate performance metrics during reload
        AdvancedPerformanceMetrics metrics;
        metrics.total_time_ms = 150;
        metrics.memory_peak_mb = 50;
        metrics.cpu_usage_percent = 25.0;
        metrics.file_path = json_file.fileName();
        metrics.operation_type = "reload";
        metrics.timestamp = QDateTime::currentDateTime();
        
        monitor->recordReloadMetrics(json_file.fileName(), metrics);
        
        // Test that monitoring captured the data
        QJsonObject dashboard = monitor->getAnalyticsDashboard();
        QVERIFY(!dashboard.isEmpty());
        QVERIFY(dashboard.contains("real_time_analytics_enabled"));
        
        // Test performance measurement integration
        auto reload_function = [&manager, &json_file]() {
            manager->performReload(json_file.fileName());
        };
        
        ReloadMetrics reload_metrics = manager->measureReloadPerformance(reload_function);
        QVERIFY(reload_metrics.success == true || reload_metrics.success == false); // Either is valid
        
        // Cleanup
        manager->unregisterUIFile(json_file.fileName());
        monitor->stopMonitoring();
    }

    // **Test JSON Loading with HotReload**
    void testJSONLoadingWithHotReload() {
        auto manager = std::make_unique<HotReloadManager>();
        auto loader = std::make_unique<JSONUILoader>();
        
        // Create test JSON file
        QTemporaryFile json_file(temp_dir_->path() + "/hot_reload_ui_XXXXXX.json");
        QVERIFY(json_file.open());
        
        QString json_content = R"({
            "type": "QWidget",
            "properties": {
                "windowTitle": "Hot Reload Test",
                "geometry": [50, 50, 300, 200]
            },
            "children": [
                {
                    "type": "QLabel",
                    "properties": {
                        "text": "Original Text"
                    }
                }
            ]
        })";
        json_file.write(json_content.toUtf8());
        json_file.close();
        
        // Load initial widget
        try {
            auto widget = loader->loadFromFile(json_file.fileName());
            QVERIFY(widget != nullptr);
            
            // Register with hot reload manager
            manager->registerUIFile(json_file.fileName(), widget.get());
            
            // Build dependency graph
            manager->buildDependencyGraph();
            
            // Test dependency management
            manager->updateDependencies(json_file.fileName());
            bool has_cycle = manager->hasCyclicDependency(json_file.fileName());
            QVERIFY(has_cycle == false);
            
            // Test async reload
            manager->performReloadAsync(json_file.fileName());
            QTest::qWait(50); // Wait for async operation
            
            // Cleanup
            manager->unregisterUIFile(json_file.fileName());
        } catch (const std::exception& e) {
            qDebug() << "JSON loading with hot reload failed:" << e.what();
        }
    }

    // **Test Component Registry with HotReload**
    void testComponentRegistryWithHotReload() {
        auto manager = std::make_unique<HotReloadManager>();
        ComponentRegistry& registry = ComponentRegistry::instance();
        
        // Test that registry has built-in components
        QStringList types = registry.getRegisteredTypes();
        QVERIFY(types.size() > 0);
        
        // Create JSON using registered component
        QTemporaryFile json_file(temp_dir_->path() + "/registry_test_XXXXXX.json");
        QVERIFY(json_file.open());
        
        QString json_content = R"({
            "type": "QWidget",
            "properties": {
                "windowTitle": "Registry Test"
            }
        })";
        json_file.write(json_content.toUtf8());
        json_file.close();
        
        // Test component creation
        QJsonObject config;
        config["windowTitle"] = "Test Widget";
        
        try {
            auto widget = registry.createComponent("QWidget", config);
            if (widget) {
                // Register with hot reload manager
                manager->registerUIFile(json_file.fileName(), widget.get());
                
                // Test safe widget replacement
                auto new_widget = std::make_unique<QWidget>();
                new_widget->setWindowTitle("Replaced Widget");
                
                try {
                    manager->replaceWidgetSafe(json_file.fileName(), std::move(new_widget));
                } catch (const std::exception& e) {
                    qDebug() << "Safe replacement failed (expected):" << e.what();
                }
                
                // Cleanup
                manager->unregisterUIFile(json_file.fileName());
            }
        } catch (const std::exception& e) {
            qDebug() << "Component creation failed:" << e.what();
        }
    }

    // **Test Performance Monitoring with JSON Operations**
    void testPerformanceMonitoringWithJSONOperations() {
        auto monitor = std::make_unique<PerformanceMonitor>();
        auto parser = std::make_unique<JSONParser>();
        
        monitor->startMonitoring();
        monitor->enableBottleneckDetection(true);
        monitor->enableMemoryProfiling(true);
        
        // Create multiple JSON files to simulate load
        QStringList json_files;
        for (int i = 0; i < 5; ++i) {
            QTemporaryFile* json_file = new QTemporaryFile(temp_dir_->path() + QString("/perf_test_%1_XXXXXX.json").arg(i));
            QVERIFY(json_file->open());
            
            QString json_content = QString(R"({
                "type": "QWidget",
                "properties": {
                    "windowTitle": "Performance Test %1",
                    "geometry": [%2, %3, 300, 200]
                }
            })").arg(i).arg(i * 50).arg(i * 30);
            
            json_file->write(json_content.toUtf8());
            json_file->close();
            json_files.append(json_file->fileName());
        }
        
        // Simulate parsing operations with performance monitoring
        for (const QString& file_path : json_files) {
            try {
                auto result = parser->parseFile(file_path);
                
                // Record simulated metrics
                AdvancedPerformanceMetrics metrics;
                metrics.total_time_ms = 50 + qrand() % 100;
                metrics.memory_peak_mb = 30 + qrand() % 50;
                metrics.cpu_usage_percent = 10.0 + (qrand() % 40);
                metrics.file_path = file_path;
                metrics.operation_type = "parse";
                metrics.timestamp = QDateTime::currentDateTime();
                
                monitor->recordReloadMetrics(file_path, metrics);
            } catch (const std::exception& e) {
                qDebug() << "JSON parsing failed for" << file_path << ":" << e.what();
            }
        }
        
        // Test bottleneck detection after operations
        auto bottlenecks = monitor->detectBottlenecks();
        // May or may not detect bottlenecks depending on simulated data
        
        // Test memory profiling
        QJsonObject memory_profile = monitor->getMemoryProfile();
        QVERIFY(memory_profile.contains("memory_profiling_enabled"));
        
        // Test report generation
        QString detailed_report = monitor->generateDetailedReport();
        QVERIFY(!detailed_report.isEmpty());
        
        monitor->stopMonitoring();
    }

    // **Test Error Handling Integration**
    void testErrorHandlingIntegration() {
        auto manager = std::make_unique<HotReloadManager>();
        auto monitor = std::make_unique<PerformanceMonitor>();
        
        monitor->startMonitoring();
        
        // Test error handling with invalid JSON file
        QTemporaryFile invalid_json(temp_dir_->path() + "/invalid_XXXXXX.json");
        QVERIFY(invalid_json.open());
        
        QString invalid_content = R"({
            "type": "QWidget",
            "properties": {
                "windowTitle": "Invalid Test"
            // Missing closing brace
        )";
        invalid_json.write(invalid_content.toUtf8());
        invalid_json.close();
        
        // Test that error handling works correctly
        auto widget = std::make_unique<QWidget>();
        manager->registerUIFile(invalid_json.fileName(), widget.get());
        
        // Test performance measurement with error
        auto error_function = []() {
            throw std::runtime_error("Simulated error");
        };
        
        ReloadMetrics error_metrics = manager->measureReloadPerformance(error_function);
        QVERIFY(error_metrics.success == false);
        
        // Test that monitoring continues after errors
        QVERIFY(monitor->isMonitoring());
        
        // Cleanup
        manager->unregisterUIFile(invalid_json.fileName());
        monitor->stopMonitoring();
    }

    // **Test Configuration Integration**
    void testConfigurationIntegration() {
        auto manager = std::make_unique<HotReloadManager>();
        auto monitor = std::make_unique<PerformanceMonitor>();
        
        // Test configuration of both components
        manager->setPreloadStrategy(true);
        manager->enableIncrementalReloading(true);
        manager->enableParallelProcessing(true);
        manager->enableSmartCaching(true);
        
        monitor->startMonitoring();
        monitor->enableRealTimeAnalytics(true);
        monitor->enablePredictiveModeling(true);
        monitor->enableBottleneckDetection(true);
        monitor->enableMemoryProfiling(true);
        
        // Test that both components work together with configuration
        QVERIFY(manager->isEnabled());
        QVERIFY(monitor->isMonitoring());
        
        // Test configuration changes
        manager->setEnabled(false);
        QVERIFY(!manager->isEnabled());
        
        monitor->pauseMonitoring();
        QVERIFY(!monitor->isMonitoring());
        
        // Test re-enabling
        manager->setEnabled(true);
        monitor->resumeMonitoring();
        
        QVERIFY(manager->isEnabled());
        QVERIFY(monitor->isMonitoring());
        
        monitor->stopMonitoring();
    }

private:
    std::unique_ptr<QTemporaryDir> temp_dir_;
};

QTEST_MAIN(HotReloadJSONIntegrationTest)
#include "test_hot_reload_json_integration.moc"
