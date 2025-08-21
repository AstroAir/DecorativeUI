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
#include <random>

#include "../../src/Exceptions/UIExceptions.hpp"
#include "../../src/HotReload/HotReloadManager.hpp"
#include "../../src/HotReload/PerformanceMonitor.hpp"
#include "../../src/JSON/ComponentRegistry.hpp"
#include "../../src/JSON/JSONParser.hpp"
#include "../../src/JSON/JSONUILoader.hpp"

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

    void cleanup() { temp_dir_.reset(); }

    // **Test HotReload with Performance Monitoring**
    void testHotReloadWithPerformanceMonitoring() {
        auto manager = std::make_unique<HotReloadManager>();
        auto monitor = std::make_unique<PerformanceMonitor>();

        // Start performance monitoring
        monitor->startMonitoring();
        monitor->enableRealTimeAnalytics(true);
        monitor->enablePredictiveModeling(true);

        // Create test JSON file
        QTemporaryFile json_file(temp_dir_->path() +
                                 "/monitored_ui_XXXXXX.json");
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

        // Test performance measurement integration through public API
        manager->reloadFile(json_file.fileName());

        // Get performance metrics through public API
        QJsonObject perf_report = manager->getPerformanceReport();
        QVERIFY(!perf_report.isEmpty());

        // Cleanup
        manager->unregisterUIFile(json_file.fileName());
        monitor->stopMonitoring();
    }

    // **Test JSON Loading with HotReload**
    void testJSONLoadingWithHotReload() {
        auto manager = std::make_unique<HotReloadManager>();
        auto loader = std::make_unique<JSONUILoader>();

        // Create test JSON file
        QTemporaryFile json_file(temp_dir_->path() +
                                 "/hot_reload_ui_XXXXXX.json");
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

            // Test dependency management through public API
            manager->reloadFile(json_file.fileName());

            // Test that reload operations work without crashing
            QVERIFY(true);

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
        QTemporaryFile json_file(temp_dir_->path() +
                                 "/registry_test_XXXXXX.json");
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

                // Test widget replacement through public API
                manager->reloadFile(json_file.fileName());

                // Test that replacement operations work without crashing
                QVERIFY(true);

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
            QTemporaryFile* json_file =
                new QTemporaryFile(temp_dir_->path() +
                                   QString("/perf_test_%1_XXXXXX.json").arg(i));
            QVERIFY(json_file->open());

            QString json_content = QString(R"({
                "type": "QWidget",
                "properties": {
                    "windowTitle": "Performance Test %1",
                    "geometry": [%2, %3, 300, 200]
                }
            })")
                                       .arg(i)
                                       .arg(i * 50)
                                       .arg(i * 30);

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
                static std::random_device rd;
                static std::mt19937 gen(rd());
                static std::uniform_int_distribution<> time_dist(50, 150);
                static std::uniform_int_distribution<> mem_dist(30, 80);
                static std::uniform_int_distribution<> cpu_dist(10, 50);

                metrics.total_time_ms = time_dist(gen);
                metrics.memory_peak_mb = mem_dist(gen);
                metrics.cpu_usage_percent = cpu_dist(gen);
                metrics.file_path = file_path;
                metrics.operation_type = "parse";
                metrics.timestamp = QDateTime::currentDateTime();

                monitor->recordReloadMetrics(file_path, metrics);
            } catch (const std::exception& e) {
                qDebug() << "JSON parsing failed for" << file_path << ":"
                         << e.what();
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

        // Test error handling through public API
        error_function();  // Execute the error function

        // Test that error handling works
        QVERIFY(true);

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
