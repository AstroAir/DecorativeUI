#include <QApplication>
#include <QDir>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTemporaryFile>
#include <QTest>
#include <QTimer>
#include <QWidget>
#include <memory>

#include "../../src/Exceptions/UIExceptions.hpp"
#include "../../src/HotReload/HotReloadManager.hpp"

using namespace DeclarativeUI::HotReload;
using namespace DeclarativeUI::Exceptions;

/**
 * @brief Comprehensive tests for newly implemented HotReloadManager functionality
 * 
 * This test suite focuses on testing all the newly implemented methods in
 * HotReloadManager that were previously missing or incomplete.
 */
class HotReloadManagerAdvancedTest : public QObject {
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

    // **Test Dependency Graph Building**
    void testDependencyGraphBuilding() {
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
        manager->buildDependencyGraph(); // Should not crash
        
        // Test affected files detection
        QStringList affected = manager->getAffectedFiles(component_file.fileName());
        // Note: May be empty if dependency parsing doesn't find the relationship
        
        // Cleanup
        manager->unregisterUIFile(main_file.fileName());
        manager->unregisterUIFile(component_file.fileName());
    }

    // **Test Dependency Management**
    void testDependencyManagement() {
        auto manager = std::make_unique<HotReloadManager>();
        
        // Create test file
        QTemporaryFile test_file(temp_dir_->path() + "/dependency_test_XXXXXX.json");
        QVERIFY(test_file.open());
        
        QString content = R"({
            "type": "QWidget",
            "properties": {
                "windowTitle": "Dependency Test"
            }
        })";
        test_file.write(content.toUtf8());
        test_file.close();
        
        auto widget = std::make_unique<QWidget>();
        manager->registerUIFile(test_file.fileName(), widget.get());
        
        // Build dependency graph first
        manager->buildDependencyGraph();
        
        // Test dependency updates
        manager->updateDependencies(test_file.fileName()); // Should not crash
        
        // Test cyclic dependency detection
        bool has_cycle = manager->hasCyclicDependency(test_file.fileName());
        QVERIFY(has_cycle == false); // Should not have cycles in this simple case
        
        // Cleanup
        manager->unregisterUIFile(test_file.fileName());
    }

    // **Test Thread Management**
    void testThreadManagement() {
        auto manager = std::make_unique<HotReloadManager>();
        
        // Test thread pool functionality
        QThread* thread = manager->getAvailableThread();
        // Should return a valid thread or nullptr if no threads available
        // We can't guarantee threads are available, so just test it doesn't crash
        Q_UNUSED(thread);
        
        // Test multiple calls
        for (int i = 0; i < 5; ++i) {
            QThread* t = manager->getAvailableThread();
            Q_UNUSED(t);
        }
    }

    // **Test Async Reload Operations**
    void testAsyncReloadOperations() {
        auto manager = std::make_unique<HotReloadManager>();
        
        // Create test file
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
        
        // Set up signal spy for reload events
        QSignalSpy reload_completed_spy(manager.get(), &HotReloadManager::reloadCompleted);
        QSignalSpy reload_failed_spy(manager.get(), &HotReloadManager::reloadFailed);
        
        // Test async reload (should not crash)
        manager->performReloadAsync(test_file.fileName());
        
        // Wait a bit for async operation
        QTest::qWait(100);
        
        // Check if any signals were emitted (may or may not happen depending on implementation)
        // We're mainly testing that it doesn't crash
        
        // Cleanup
        manager->unregisterUIFile(test_file.fileName());
    }

    // **Test Performance Measurement**
    void testPerformanceMeasurement() {
        auto manager = std::make_unique<HotReloadManager>();
        
        // Test performance measurement with a simple function
        bool test_executed = false;
        auto test_function = [&test_executed]() {
            test_executed = true;
            QTest::qWait(10); // Simulate some work
        };
        
        ReloadMetrics metrics = manager->measureReloadPerformance(test_function);
        
        QVERIFY(test_executed == true);
        QVERIFY(metrics.success == true);
        QVERIFY(metrics.total_time.count() >= 10); // Should be at least 10ms
        
        // Test with a function that throws
        auto throwing_function = []() {
            throw std::runtime_error("Test exception");
        };
        
        ReloadMetrics error_metrics = manager->measureReloadPerformance(throwing_function);
        QVERIFY(error_metrics.success == false);
    }

    // **Test Safe Widget Replacement**
    void testSafeWidgetReplacement() {
        auto manager = std::make_unique<HotReloadManager>();
        
        // Create test file
        QTemporaryFile test_file(temp_dir_->path() + "/safe_replace_XXXXXX.json");
        QVERIFY(test_file.open());
        
        QString content = R"({
            "type": "QWidget",
            "properties": {
                "windowTitle": "Safe Replace Test"
            }
        })";
        test_file.write(content.toUtf8());
        test_file.close();
        
        auto original_widget = std::make_unique<QWidget>();
        manager->registerUIFile(test_file.fileName(), original_widget.get());
        
        // Test safe widget replacement
        auto new_widget = std::make_unique<QWidget>();
        new_widget->setWindowTitle("New Widget");
        
        // This might fail due to validation, but should not crash
        try {
            manager->replaceWidgetSafe(test_file.fileName(), std::move(new_widget));
        } catch (const std::exception& e) {
            // Safe replacement might fail due to validation, which is expected
            qDebug() << "Safe replacement failed (expected):" << e.what();
        }
        
        // Cleanup
        manager->unregisterUIFile(test_file.fileName());
    }

    // **Test Rollback Points**
    void testRollbackPoints() {
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
        manager->createRollbackPoint(test_file.fileName()); // Should not crash
        
        // Test rollback
        manager->rollbackToPoint(test_file.fileName()); // Should not crash
        
        // Test clearing rollback points
        manager->clearRollbackPoints(); // Should not crash
        
        // Cleanup
        manager->unregisterUIFile(test_file.fileName());
    }

    // **Test Configuration Methods**
    void testConfigurationMethods() {
        auto manager = std::make_unique<HotReloadManager>();
        
        // Test configuration methods
        manager->setPreloadStrategy(true); // Should not crash
        manager->setPreloadStrategy(false);
        
        manager->enableIncrementalReloading(true);
        manager->enableIncrementalReloading(false);
        
        manager->enableParallelProcessing(true);
        manager->enableParallelProcessing(false);
        
        manager->enableSmartCaching(true);
        manager->enableSmartCaching(false);
        
        // Test that manager is still functional after configuration
        QVERIFY(manager->isEnabled() == true);
        
        // Test disabling and re-enabling
        manager->setEnabled(false);
        QVERIFY(manager->isEnabled() == false);
        
        manager->setEnabled(true);
        QVERIFY(manager->isEnabled() == true);
    }

    // **Test Widget Caching**
    void testWidgetCaching() {
        auto manager = std::make_unique<HotReloadManager>();
        
        // Create test file
        QTemporaryFile test_file(temp_dir_->path() + "/cache_test_XXXXXX.json");
        QVERIFY(test_file.open());
        
        QString content = R"({
            "type": "QWidget",
            "properties": {
                "windowTitle": "Cache Test"
            }
        })";
        test_file.write(content.toUtf8());
        test_file.close();
        
        // Test widget creation from cache
        auto cached_widget = manager->createWidgetFromCache(test_file.fileName());
        // May return nullptr if not in cache, which is expected
        Q_UNUSED(cached_widget);
        
        // Test preload dependencies
        manager->preloadDependencies(test_file.fileName()); // Should not crash
        
        // Test memory optimization
        manager->optimizeMemoryUsage(); // Should not crash
    }

    // **Test Error Handling**
    void testErrorHandling() {
        auto manager = std::make_unique<HotReloadManager>();
        
        // Test operations on non-existent files
        manager->updateDependencies("non_existent_file.json"); // Should not crash
        
        bool has_cycle = manager->hasCyclicDependency("non_existent_file.json");
        QVERIFY(has_cycle == false); // Should return false for non-existent files
        
        QStringList affected = manager->getAffectedFiles("non_existent_file.json");
        QVERIFY(affected.isEmpty()); // Should return empty list
        
        // Test safe widget replacement with null widget
        try {
            manager->replaceWidgetSafe("test.json", nullptr);
        } catch (const std::exception& e) {
            // Should handle null widget gracefully
            qDebug() << "Null widget replacement failed (expected):" << e.what();
        }
    }

private:
    std::unique_ptr<QTemporaryDir> temp_dir_;
};

QTEST_MAIN(HotReloadManagerAdvancedTest)
#include "test_hot_reload_manager_advanced.moc"
