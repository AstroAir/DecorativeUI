#include <QApplication>
#include <QJsonDocument>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTemporaryFile>
#include <QTest>
#include <QTimer>
#include <memory>

#include "../../src/Exceptions/UIExceptions.hpp"
#include "../../src/HotReload/PerformanceMonitor.hpp"

using namespace DeclarativeUI::HotReload;
using namespace DeclarativeUI::Exceptions;

/**
 * @brief Comprehensive tests for newly implemented PerformanceMonitor
 * functionality
 *
 * This test suite focuses on testing all the newly implemented methods in
 * PerformanceMonitor that were previously missing or incomplete.
 */
class PerformanceMonitorAdvancedTest : public QObject {
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

    // **Test Monitoring Lifecycle Control**
    void testMonitoringLifecycle() {
        auto monitor = std::make_unique<PerformanceMonitor>();

        // Test initial state
        QVERIFY(!monitor->isMonitoring());

        // Test start monitoring
        monitor->startMonitoring();
        QVERIFY(monitor->isMonitoring());

        // Test pause monitoring
        monitor->pauseMonitoring();
        QVERIFY(!monitor->isMonitoring());

        // Test resume monitoring
        monitor->resumeMonitoring();
        QVERIFY(monitor->isMonitoring());

        // Test stop monitoring
        monitor->stopMonitoring();
        QVERIFY(!monitor->isMonitoring());
    }

    // **Test Resource Usage Recording**
    void testResourceUsageRecording() {
        auto monitor = std::make_unique<PerformanceMonitor>();
        monitor->startMonitoring();

        // Test memory usage recording
        monitor->recordMemoryUsage(100);
        monitor->recordMemoryUsage(150);
        monitor->recordMemoryUsage(200);

        // Test CPU usage recording
        monitor->recordCPUUsage(25.5);
        monitor->recordCPUUsage(45.0);
        monitor->recordCPUUsage(30.2);

        // Verify monitoring is still active
        QVERIFY(monitor->isMonitoring());

        monitor->stopMonitoring();
    }

    // **Test Signal Emission for Warnings**
    void testWarningSignals() {
        auto monitor = std::make_unique<PerformanceMonitor>();
        monitor->startMonitoring();

        // Set low thresholds to trigger warnings
        monitor->setMemoryWarningThreshold(50);
        monitor->setCPUWarningThreshold(20.0);

        // Set up signal spies
        QSignalSpy memory_warning_spy(monitor.get(),
                                      &PerformanceMonitor::memoryWarning);
        QSignalSpy cpu_warning_spy(monitor.get(),
                                   &PerformanceMonitor::cpuWarning);

        // Trigger memory warning
        monitor->recordMemoryUsage(100);  // Above threshold of 50
        QVERIFY(memory_warning_spy.count() >= 1);

        // Trigger CPU warning
        monitor->recordCPUUsage(50.0);  // Above threshold of 20.0
        QVERIFY(cpu_warning_spy.count() >= 1);

        monitor->stopMonitoring();
    }

    // **Test Real-time Analytics**
    void testRealTimeAnalytics() {
        auto monitor = std::make_unique<PerformanceMonitor>();
        monitor->startMonitoring();
        monitor->enableRealTimeAnalytics(true);

        // Create test metrics
        AdvancedPerformanceMetrics metrics;
        metrics.total_time_ms = 150;
        metrics.memory_peak_mb = 200;
        metrics.cpu_usage_percent = 45.0;
        metrics.file_path = "test.json";
        metrics.operation_type = "reload";
        metrics.timestamp = QDateTime::currentDateTime();

        // Record metrics multiple times
        for (int i = 0; i < 5; ++i) {
            metrics.total_time_ms = 100 + i * 10;
            metrics.memory_peak_mb = 150 + i * 20;
            monitor->recordReloadMetrics("test.json", metrics);
        }

        // Test analytics dashboard
        QJsonObject dashboard = monitor->getAnalyticsDashboard();
        QVERIFY(!dashboard.isEmpty());
        QVERIFY(dashboard.contains("real_time_analytics_enabled"));
        QVERIFY(dashboard["real_time_analytics_enabled"].toBool());

        // Test real-time analytics data
        AnalyticsData analytics = monitor->getRealTimeAnalytics();
        QVERIFY(analytics.total_operations.load() > 0);

        monitor->stopMonitoring();
    }

    // **Test Predictive Modeling**
    void testPredictiveModeling() {
        auto monitor = std::make_unique<PerformanceMonitor>();
        monitor->startMonitoring();
        monitor->enablePredictiveModeling(true);

        // Generate historical data for prediction
        AdvancedPerformanceMetrics metrics;
        for (int i = 0; i < 15; ++i) {
            metrics.total_time_ms = 100 + i * 5;  // Increasing trend
            metrics.memory_peak_mb = 150 + i * 10;
            metrics.cpu_usage_percent = 20.0 + i * 2.0;
            metrics.file_path = "test.json";
            monitor->recordReloadMetrics("test.json", metrics);
        }

        // Test prediction methods
        double next_response_time = monitor->predictNextResponseTime();
        QVERIFY(next_response_time >= 0.0);

        double memory_prediction_5min = monitor->predictMemoryUsageIn(5);
        QVERIFY(memory_prediction_5min >= 0.0);

        double memory_prediction_15min = monitor->predictMemoryUsageIn(15);
        QVERIFY(memory_prediction_15min >= 0.0);

        // Test prediction report
        QJsonObject prediction_report = monitor->getPredictionReport();
        QVERIFY(!prediction_report.isEmpty());
        QVERIFY(prediction_report.contains("predictive_modeling_enabled"));
        QVERIFY(prediction_report["predictive_modeling_enabled"].toBool());
        QVERIFY(prediction_report.contains("next_response_time_prediction"));
        QVERIFY(prediction_report.contains("memory_usage_prediction_5min"));

        monitor->stopMonitoring();
    }

    // **Test Bottleneck Detection**
    void testBottleneckDetection() {
        auto monitor = std::make_unique<PerformanceMonitor>();
        monitor->startMonitoring();
        monitor->enableBottleneckDetection(true);

        // Set thresholds to trigger bottleneck detection
        monitor->setCPUWarningThreshold(50.0);
        monitor->setMemoryWarningThreshold(100);

        // Simulate high resource usage to trigger bottlenecks
        monitor->recordCPUUsage(95.0);    // High CPU usage
        monitor->recordMemoryUsage(500);  // High memory usage

        // Create metrics with slow I/O
        AdvancedPerformanceMetrics metrics;
        metrics.file_load_time_ms = 1000;  // Slow file loading
        metrics.total_time_ms = 1200;
        metrics.file_path = "slow_file.json";
        monitor->recordReloadMetrics("slow_file.json", metrics);

        // Test bottleneck detection
        auto bottlenecks = monitor->detectBottlenecks();
        QVERIFY(bottlenecks.size() >
                0);  // Should detect at least one bottleneck

        // Test most critical bottleneck
        auto critical_bottleneck = monitor->getMostCriticalBottleneck();
        QVERIFY(!critical_bottleneck.bottleneck_type.isEmpty());
        QVERIFY(critical_bottleneck.severity_score > 0.0);
        QVERIFY(!critical_bottleneck.description.isEmpty());
        QVERIFY(!critical_bottleneck.recommendations.isEmpty());

        monitor->stopMonitoring();
    }

    // **Test Memory Profiling**
    void testMemoryProfiling() {
        auto monitor = std::make_unique<PerformanceMonitor>();
        monitor->startMonitoring();
        monitor->enableMemoryProfiling(true);

        // Record some memory usage
        monitor->recordMemoryUsage(100);
        monitor->recordMemoryUsage(150);
        monitor->recordMemoryUsage(200);

        // Test memory profile generation
        QJsonObject memory_profile = monitor->getMemoryProfile();
        QVERIFY(!memory_profile.isEmpty());
        QVERIFY(memory_profile.contains("memory_profiling_enabled"));
        QVERIFY(memory_profile["memory_profiling_enabled"].toBool());
        QVERIFY(memory_profile.contains("current_memory_usage_mb"));
        QVERIFY(memory_profile.contains("baseline_memory_usage_mb"));
        QVERIFY(memory_profile.contains("peak_memory_usage_mb"));
        QVERIFY(memory_profile.contains("memory_snapshots"));

        // Test garbage collection
        monitor->forceGarbageCollection();  // Should not crash

        monitor->stopMonitoring();
    }

    // **Test Performance Optimization**
    void testPerformanceOptimization() {
        auto monitor = std::make_unique<PerformanceMonitor>();
        monitor->startMonitoring();

        // Test optimization recommendations
        QStringList recommendations = monitor->getOptimizationRecommendations();
        QVERIFY(recommendations.size() >=
                0);  // Should return a list (may be empty)

        // Test performance optimization
        monitor->optimizePerformance();  // Should not crash

        // Test automatic optimizations
        monitor->applyAutomaticOptimizations(true);
        monitor->applyAutomaticOptimizations(false);

        monitor->stopMonitoring();
    }

    // **Test Public API Methods**
    void testPublicAPIMethods() {
        auto monitor = std::make_unique<PerformanceMonitor>();
        monitor->startMonitoring();

        // Test threshold setters/getters
        monitor->setMemoryWarningThreshold(100);
        monitor->setCPUWarningThreshold(50.0);

        // Test enable/disable methods
        monitor->enableRealTimeAnalytics(true);
        monitor->enablePredictiveModeling(true);
        monitor->enableBottleneckDetection(true);
        monitor->enableMemoryProfiling(true);

        // Test that these don't crash
        monitor->enableRealTimeAnalytics(false);
        monitor->enablePredictiveModeling(false);
        monitor->enableBottleneckDetection(false);
        monitor->enableMemoryProfiling(false);

        monitor->stopMonitoring();
    }

    // **Test Report Generation and Export**
    void testReportGenerationAndExport() {
        auto monitor = std::make_unique<PerformanceMonitor>();
        monitor->startMonitoring();

        // Generate some test data
        monitor->recordCPUUsage(30.0);
        monitor->recordMemoryUsage(150);

        AdvancedPerformanceMetrics metrics;
        metrics.total_time_ms = 200;
        metrics.memory_peak_mb = 100;
        metrics.file_path = "test.json";
        monitor->recordReloadMetrics("test.json", metrics);

        // Test basic report generation
        QString basic_report = monitor->generateReport();
        QVERIFY(!basic_report.isEmpty());
        QVERIFY(basic_report.contains("Performance Report"));

        // Test detailed report generation
        QString detailed_report = monitor->generateDetailedReport();
        QVERIFY(!detailed_report.isEmpty());
        QVERIFY(detailed_report.contains("Detailed Performance Analysis"));
        QVERIFY(detailed_report.contains("Memory Analysis"));
        QVERIFY(detailed_report.contains("CPU Analysis"));

        // Test JSON report generation
        QJsonObject json_report = monitor->generateJSONReport();
        QVERIFY(!json_report.isEmpty());
        QVERIFY(json_report.contains("monitoring_enabled"));

        // Test export to text file
        QString text_file_path = temp_dir_->path() + "/performance_report.txt";
        monitor->exportReportToFile(text_file_path);

        QFile text_file(text_file_path);
        QVERIFY(text_file.exists());
        QVERIFY(text_file.open(QIODevice::ReadOnly));
        QString content = text_file.readAll();
        QVERIFY(!content.isEmpty());
        QVERIFY(content.contains("Performance Report"));
        text_file.close();

        // Test export to JSON file
        QString json_file_path = temp_dir_->path() + "/performance_report.json";
        monitor->exportReportToFile(json_file_path);

        QFile json_file(json_file_path);
        QVERIFY(json_file.exists());
        QVERIFY(json_file.open(QIODevice::ReadOnly));
        QByteArray json_content = json_file.readAll();
        QVERIFY(!json_content.isEmpty());

        // Verify it's valid JSON
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(json_content, &error);
        QVERIFY(error.error == QJsonParseError::NoError);
        json_file.close();

        monitor->stopMonitoring();
    }

    // **Test Memory Profiling Integration**
    void testMemoryProfilingIntegration() {
        auto monitor = std::make_unique<PerformanceMonitor>();
        monitor->startMonitoring();
        monitor->enableMemoryProfiling(true);

        // Simulate memory growth pattern
        for (int i = 0; i < 15; ++i) {
            monitor->recordMemoryUsage(100 +
                                       i * 20);  // Steadily increasing memory
        }

        // Test that memory profiling data is captured
        QJsonObject memory_profile = monitor->getMemoryProfile();
        QVERIFY(memory_profile.contains("memory_profiling_enabled"));
        QVERIFY(memory_profile["memory_profiling_enabled"].toBool());

        monitor->stopMonitoring();
    }

private:
    std::unique_ptr<QTemporaryDir> temp_dir_;
};

QTEST_MAIN(PerformanceMonitorAdvancedTest)
#include "test_performance_monitor_advanced.moc"
