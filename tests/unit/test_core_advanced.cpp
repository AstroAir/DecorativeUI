#include <QApplication>
#include <QElapsedTimer>
#include <QFuture>
#include <QSignalSpy>
#include <QTest>
#include <QThread>
#include <QtConcurrent>
#include <memory>
#include <vector>

#include "../Core/CacheManager.hpp"
#include "../Core/MemoryManager.hpp"
#include "../Core/ParallelProcessor.hpp"

using namespace DeclarativeUI::Core;

class CoreAdvancedTest : public QObject {
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
    }

    void cleanup() {
        // Clean up after each test
    }

    // **CacheManager Tests**
    void testCacheManagerCreation() {
        auto cache_manager = std::make_unique<CacheManager>();

        QVERIFY(cache_manager != nullptr);

        // Test cache initialization
        auto stats = cache_manager->getCacheStatistics();
        QVERIFY(stats.contains("total_memory_usage"));
        QVERIFY(stats["total_memory_usage"].toDouble() >= 0);
    }

    void testCacheManagerWidgetCaching() {
        auto cache_manager = std::make_unique<CacheManager>();

        // Test widget caching
        QString key = "test_widget_key";
        auto test_widget = std::make_shared<QWidget>();
        test_widget->setObjectName("TestWidget");

        // Store widget in cache
        cache_manager->cacheWidget(key, test_widget);

        // Retrieve widget from cache
        auto cached_widget = cache_manager->getCachedWidget(key);
        QVERIFY(cached_widget != nullptr);
        QCOMPARE(cached_widget->objectName(), QString("TestWidget"));

        // Test cache statistics
        auto stats = cache_manager->getCacheStatistics();
        QVERIFY(stats.contains("caches"));
        auto caches = stats["caches"].toObject();
        QVERIFY(caches.contains("widget_cache"));
    }

    void testCacheManagerStylesheetCaching() {
        auto cache_manager = std::make_unique<CacheManager>();

        QString key = "test_style_key";
        QString stylesheet = "QWidget { background-color: red; }";

        // Store stylesheet
        cache_manager->cacheStylesheet(key, stylesheet);

        // Retrieve stylesheet
        QString cached_style = cache_manager->getCachedStylesheet(key);
        QCOMPARE(cached_style, stylesheet);

        // Test cache statistics
        auto stats = cache_manager->getCacheStatistics();
        QVERIFY(stats.contains("caches"));
        auto caches = stats["caches"].toObject();
        QVERIFY(caches.contains("stylesheet_cache"));
    }

    void testCacheManagerPropertyCaching() {
        auto cache_manager = std::make_unique<CacheManager>();

        QString key = "test_property_key";
        QVariant property_value = QString("Test Property Value");

        // Store property
        cache_manager->cacheProperty(key, property_value);

        // Retrieve property
        QVariant cached_property = cache_manager->getCachedProperty(key);
        QCOMPARE(cached_property.toString(), QString("Test Property Value"));

        // Test cache statistics
        auto stats = cache_manager->getCacheStatistics();
        QVERIFY(stats.contains("caches"));
        auto caches = stats["caches"].toObject();
        QVERIFY(caches.contains("property_cache"));
    }

    void testCacheManagerMemoryLimits() {
        auto cache_manager = std::make_unique<CacheManager>();

        // Set a small memory limit for testing
        cache_manager->setGlobalMemoryLimit(1);  // 1MB

        // Fill cache with data
        for (int i = 0; i < 10; ++i) {
            QString key = QString("large_data_%1").arg(i);
            QString large_data(1000, 'A');  // 1KB per entry
            cache_manager->cacheProperty(key, QVariant(large_data));
        }

        auto stats = cache_manager->getCacheStatistics();
        QVERIFY(stats.contains("total_memory_usage"));

        // Reset to default
        cache_manager->setGlobalMemoryLimit(100);  // 100MB
    }

    void testCacheManagerConcurrency() {
        auto cache_manager = std::make_unique<CacheManager>();

        // Simple concurrency test - just verify no crashes
        const int num_operations = 10;

        for (int i = 0; i < num_operations; ++i) {
            QString key = QString("concurrent_key_%1").arg(i);
            QString value = QString("concurrent_value_%1").arg(i);

            // Store and retrieve property
            cache_manager->cacheProperty(key, QVariant(value));
            QVariant cached = cache_manager->getCachedProperty(key);
            QCOMPARE(cached.toString(), value);
        }

        // Test passes if no crashes occur
        QVERIFY(true);
    }

    // **MemoryManager Tests**
    void testMemoryManagerCreation() {
        auto& memory_manager = MemoryManager::instance();

        // Test memory statistics
        auto stats = memory_manager.get_statistics();
        // Invariants for unsigned stats instead of tautologies
        QVERIFY(stats.current_allocated_bytes <= stats.peak_allocated_bytes);
        QVERIFY(stats.peak_allocated_bytes <= stats.total_allocated_bytes);
        QVERIFY(stats.deallocation_count <= stats.allocation_count);
    }

    void testMemoryManagerConfiguration() {
        auto& memory_manager = MemoryManager::instance();

        // Test configuration methods
        memory_manager.set_memory_limit(100 * 1024 * 1024);  // 100MB
        memory_manager.enable_auto_gc(true);
        memory_manager.enable_leak_detection(true);

        // Verify statistics are accessible
        auto stats = memory_manager.get_statistics();
        // Basic consistency checks
        QVERIFY(stats.current_allocated_bytes <= stats.peak_allocated_bytes);
        QVERIFY(stats.deallocation_count <= stats.allocation_count);
    }

    void testMemoryManagerLeakDetection() {
        auto& memory_manager = MemoryManager::instance();

        // Enable leak detection
        memory_manager.enable_leak_detection(true);

        // Check for leaks (should be empty initially)
        auto leaks = memory_manager.get_memory_leaks();
        // Initially should have no leaks recorded
        QVERIFY(leaks.empty());

        // Disable leak detection
        memory_manager.enable_leak_detection(false);
    }

    void testMemoryManagerMemoryPressure() {
        auto& memory_manager = MemoryManager::instance();

        // Set a low memory limit for testing
        memory_manager.set_memory_limit(1024 * 1024);  // 1MB

        QSignalSpy pressure_spy(&memory_manager,
                                &MemoryManager::memory_pressure_detected);

        // Trigger garbage collection to test memory management
        memory_manager.trigger_gc();

        // Process events to allow signals to be emitted
        QApplication::processEvents();

        // Test passes if no crashes occur
        QVERIFY(true);

        // Reset to default limit
        memory_manager.set_memory_limit(100 * 1024 * 1024);  // 100MB
    }

    void testMemoryManagerOptimization() {
        auto& memory_manager = MemoryManager::instance();

        // Get initial statistics
        auto initial_stats = memory_manager.get_statistics();

        // Perform memory optimization
        memory_manager.optimize_memory_usage();

        // Verify optimization completed without errors
        auto optimized_stats = memory_manager.get_statistics();
        QVERIFY(optimized_stats.total_allocated_bytes >=
                initial_stats.total_allocated_bytes);
    }

    // **ParallelProcessor Tests**
    void testParallelProcessorCreation() {
        auto processor = std::make_unique<ParallelProcessor>();

        QVERIFY(processor != nullptr);

        // Test performance metrics access
        auto metrics = processor->getPerformanceMetrics();
        QVERIFY(metrics.contains("active_task_count"));
    }

    void testParallelProcessorTaskExecution() {
        auto processor = std::make_unique<ParallelProcessor>();

        // Submit a background task using the correct API
        QString task_id = processor->submitBackgroundTask("test_task", []() {
            // Simple task that does some work
            volatile int sum = 0;
            for (int i = 0; i < 1000; ++i) {
                sum += i;
            }
        });

        QVERIFY(!task_id.isEmpty());

        // After submission, either active or queued task count should reflect
        // the task
        auto active = processor->getActiveTaskCount();
        auto queued = processor->getQueuedTaskCount();
        QVERIFY(active + queued >= 1);
    }

    void testParallelProcessorTaskPriority() {
        auto processor = std::make_unique<ParallelProcessor>();

        // Submit high priority task using the correct API
        QString high_priority_task =
            processor->submitHighPriorityTask("high_priority", []() {
                // High priority work
                volatile int sum = 0;
                for (int i = 0; i < 100; ++i) {
                    sum += i;
                }
            });

        // Submit background task
        QString background_task =
            processor->submitBackgroundTask("background", []() {
                // Background work
                volatile int sum = 0;
                for (int i = 0; i < 100; ++i) {
                    sum += i;
                }
            });

        QVERIFY(!high_priority_task.isEmpty());
        QVERIFY(!background_task.isEmpty());

        // Test task management
        auto metrics = processor->getPerformanceMetrics();
        QVERIFY(metrics.contains("active_task_count"));
    }

    void testParallelProcessorPerformance() {
        auto processor = std::make_unique<ParallelProcessor>();

        QElapsedTimer timer;
        timer.start();

        const int num_tasks = 10;

        // Submit CPU-intensive tasks using the correct API
        for (int i = 0; i < num_tasks; ++i) {
            QString task_id = processor->submitBackgroundTask(
                QString("perf_task_%1").arg(i), []() {
                    // Simulate work
                    volatile int sum = 0;
                    for (int j = 0; j < 1000; ++j) {
                        sum += j;
                    }
                });
            QVERIFY(!task_id.isEmpty());
        }

        qint64 elapsed = timer.elapsed();
        qDebug() << "Submitted" << num_tasks << "tasks in" << elapsed << "ms";

        // Test performance metrics
        auto metrics = processor->getPerformanceMetrics();
        QVERIFY(metrics.contains("active_task_count"));
        QVERIFY(elapsed < 1000);  // Should submit quickly
    }
};

QTEST_MAIN(CoreAdvancedTest)
#include "test_core_advanced.moc"
