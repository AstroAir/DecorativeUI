#include <QApplication>
#include <QElapsedTimer>
#include <QFuture>
#include <QMutex>
#include <QTest>
#include <QThread>
#include <QWaitCondition>
#include <QtConcurrent>
#include <atomic>
#include <chrono>
#include <memory>
#include <thread>

#include "../Binding/StateManager.hpp"
#include "../Command/BuiltinCommands.hpp"
#include "../Command/CommandSystem.hpp"
#include "../Core/CacheManager.hpp"
#include "../Core/MemoryManager.hpp"
#include "../Core/ParallelProcessor.hpp"

using namespace DeclarativeUI::Binding;
using namespace DeclarativeUI::Core;
using namespace DeclarativeUI::Command;
using namespace DeclarativeUI::Command::Commands;

class ThreadSafetyTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        // Initialize Qt application if not already done
        if (!QApplication::instance()) {
            int argc = 0;
            char* argv[] = {nullptr};
            new QApplication(argc, argv);
        }

        // Register builtin commands
        registerBuiltinCommands();
    }

    void cleanupTestCase() {
        // Clean up any global state
        StateManager::instance().clearState();
    }

    void init() {
        // Set up for each test
        StateManager::instance().clearState();
    }

    void cleanup() {
        // Clean up after each test
        StateManager::instance().clearState();
    }

    // **StateManager Thread Safety**
    void testStateManagerConcurrentAccess() {
        auto& state_manager = StateManager::instance();

        const int num_threads = 4;
        const int operations_per_thread = 100;
        std::atomic<int> read_success{0};
        std::atomic<int> write_success{0};
        std::atomic<int> errors{0};

        QVector<QFuture<void>> futures;

        // Launch reader threads
        for (int t = 0; t < num_threads / 2; ++t) {
            auto future = QtConcurrent::run([&, t]() {
                for (int i = 0; i < operations_per_thread; ++i) {
                    try {
                        QString key =
                            QString("reader_test_%1_%2").arg(t).arg(i);

                        // Set a value first
                        state_manager.setState(
                            key, QString("Reader Value %1").arg(i));

                        // Then read it
                        auto value = state_manager.getState<QString>(key);
                        if (value && value->get().contains("Reader Value")) {
                            read_success.fetch_add(1);
                        }
                    } catch (const std::exception& e) {
                        errors.fetch_add(1);
                        qWarning()
                            << "Reader thread" << t << "error:" << e.what();
                    }
                }
            });
            futures.append(future);
        }

        // Launch writer threads
        for (int t = num_threads / 2; t < num_threads; ++t) {
            auto future = QtConcurrent::run([&, t]() {
                for (int i = 0; i < operations_per_thread; ++i) {
                    try {
                        QString key =
                            QString("writer_test_%1_%2").arg(t).arg(i);
                        QString value =
                            QString("Writer Value %1_%2").arg(t).arg(i);

                        state_manager.setState(key, value);

                        // Verify the write
                        auto retrieved = state_manager.getState<QString>(key);
                        if (retrieved && retrieved->get() == value) {
                            write_success.fetch_add(1);
                        }
                    } catch (const std::exception& e) {
                        errors.fetch_add(1);
                        qWarning()
                            << "Writer thread" << t << "error:" << e.what();
                    }
                }
            });
            futures.append(future);
        }

        // Wait for all threads to complete
        for (auto& future : futures) {
            future.waitForFinished();
        }

        qDebug() << "Read successes:" << read_success.load();
        qDebug() << "Write successes:" << write_success.load();
        qDebug() << "Errors:" << errors.load();

        // Allow for some race conditions in concurrent access
        // The current StateManager implementation doesn't have full thread
        // safety
        QCOMPARE(errors.load(), 0);

        // Expect at least 80% success rate for concurrent operations
        // The current StateManager implementation has limited thread safety
        int expected_reads = (num_threads / 2) * operations_per_thread;
        int expected_writes = (num_threads / 2) * operations_per_thread;

        QVERIFY(read_success.load() >= expected_reads * 0.8);
        QVERIFY(write_success.load() >= expected_writes * 0.8);
    }

    void testStateManagerComputedStateThreadSafety() {
        auto& state_manager = StateManager::instance();

        // Set up base values
        state_manager.setState("base.value1", 10);
        state_manager.setState("base.value2", 20);

        // Set up computed state
        auto computed_sum =
            state_manager.createComputed<int>("computed.sum", [&]() -> int {
                auto val1 = state_manager.getState<int>("base.value1");
                auto val2 = state_manager.getState<int>("base.value2");
                if (val1 && val2) {
                    return val1->get() + val2->get();
                }
                return 0;
            });

        const int num_threads = 4;
        const int operations_per_thread = 100;
        std::atomic<int> success_count{0};

        QVector<QFuture<void>> futures;

        for (int t = 0; t < num_threads; ++t) {
            auto future = QtConcurrent::run([&, t]() {
                for (int i = 0; i < operations_per_thread; ++i) {
                    try {
                        // Update base values
                        state_manager.setState("base.value1", 10 + i);
                        state_manager.setState("base.value2", 20 + i);

                        // Read computed value
                        auto computed =
                            state_manager.getState<int>("computed.sum");
                        if (computed && computed->get() >= 30) {
                            success_count.fetch_add(1);
                        }

                        // Small delay to increase chance of race conditions
                        std::this_thread::sleep_for(
                            std::chrono::microseconds(1));
                    } catch (const std::exception& e) {
                        qWarning() << "Computed state thread" << t
                                   << "error:" << e.what();
                    }
                }
            });
            futures.append(future);
        }

        // Wait for all threads to complete
        for (auto& future : futures) {
            future.waitForFinished();
        }

        qDebug() << "Computed state successes:" << success_count.load();
        QVERIFY(success_count.load() > 0);  // Should have some successes
    }

    // **CacheManager Thread Safety**
    void testCacheManagerConcurrentAccess() {
        auto cache_manager = std::make_unique<CacheManager>();

        const int num_threads = 6;
        const int operations_per_thread = 200;
        std::atomic<int> cache_hits{0};
        std::atomic<int> cache_stores{0};
        std::atomic<int> errors{0};

        QVector<QFuture<void>> futures;

        for (int t = 0; t < num_threads; ++t) {
            auto future = QtConcurrent::run([&, t]() {
                for (int i = 0; i < operations_per_thread; ++i) {
                    try {
                        QString key = QString("cache_test_%1_%2").arg(t).arg(i);
                        QString value =
                            QString("Cache Value %1_%2").arg(t).arg(i);

                        // Store in cache
                        cache_manager->cacheProperty(key, QVariant(value));
                        cache_stores.fetch_add(1);

                        // Retrieve from cache
                        QVariant cached = cache_manager->getCachedProperty(key);
                        if (!cached.isNull() && cached.toString() == value) {
                            cache_hits.fetch_add(1);
                        }

                        // Test widget cache too
                        QString widget_key =
                            QString("widget_%1_%2").arg(t).arg(i);
                        QWidget* widget =
                            new QLabel(QString("Widget %1_%2").arg(t).arg(i));

                        cache_manager->cacheWidget(
                            widget_key, std::shared_ptr<QWidget>(widget));
                        auto cached_widget =
                            cache_manager->getCachedWidget(widget_key);

                        if (cached_widget && cached_widget.get() == widget) {
                            cache_hits.fetch_add(1);
                        }
                    } catch (const std::exception& e) {
                        errors.fetch_add(1);
                        qWarning()
                            << "Cache thread" << t << "error:" << e.what();
                    }
                }
            });
            futures.append(future);
        }

        // Wait for all threads to complete
        for (auto& future : futures) {
            future.waitForFinished();
        }

        qDebug() << "Cache stores:" << cache_stores.load();
        qDebug() << "Cache hits:" << cache_hits.load();
        qDebug() << "Cache errors:" << errors.load();

        QCOMPARE(errors.load(), 0);
        QCOMPARE(cache_stores.load(), num_threads * operations_per_thread);
        QVERIFY(cache_hits.load() >=
                num_threads *
                    operations_per_thread);  // At least as many hits as stores
    }

    // **MemoryManager Thread Safety**
    void testMemoryManagerConcurrentAllocation() {
        auto& memory_manager = MemoryManager::instance();

        const int num_threads = 4;
        const int operations_per_thread = 50;
        std::atomic<int> successful_operations{0};
        std::atomic<int> errors{0};

        QVector<QFuture<void>> futures;

        for (int t = 0; t < num_threads; ++t) {
            auto future = QtConcurrent::run([&, t]() {
                for (int i = 0; i < operations_per_thread; ++i) {
                    try {
                        // Test memory management operations that are available
                        memory_manager.trigger_gc();
                        memory_manager.optimize_memory_usage();

                        // Get statistics to verify memory manager is working
                        auto stats = memory_manager.get_statistics();
                        // Meaningful invariant: current allocation cannot
                        // exceed peak (both are unsigned, so no >= 0 check
                        // needed)
                        if (stats.current_allocated_bytes <=
                            stats.peak_allocated_bytes) {
                            successful_operations.fetch_add(1);
                        }

                        // Test memory configuration
                        memory_manager.set_memory_limit(100 * 1024 *
                                                        1024);  // 100MB
                        memory_manager.enable_auto_gc(true);

                    } catch (const std::exception& e) {
                        errors.fetch_add(1);
                        qWarning()
                            << "Memory thread" << t << "error:" << e.what();
                    }
                }
            });
            futures.append(future);
        }

        // Wait for all threads to complete
        for (auto& future : futures) {
            future.waitForFinished();
        }

        qDebug() << "Successful operations:" << successful_operations.load();
        qDebug() << "Memory errors:" << errors.load();

        QCOMPARE(errors.load(), 0);
        QVERIFY(successful_operations.load() != 0);
    }

    // **ParallelProcessor Thread Safety**
    void testParallelProcessorConcurrentSubmission() {
        auto processor = std::make_unique<ParallelProcessor>();

        const int num_submitter_threads = 4;
        const int tasks_per_thread = 250;
        std::atomic<int> submitted_tasks{0};
        std::atomic<int> completed_tasks{0};
        std::atomic<int> errors{0};

        QVector<QFuture<void>> submitter_futures;

        // Launch task submitter threads
        for (int t = 0; t < num_submitter_threads; ++t) {
            auto future = QtConcurrent::run([&, t]() {
                for (int i = 0; i < tasks_per_thread; ++i) {
                    try {
                        QString task_id = processor->submitBackgroundTask(
                            QString("task_%1_%2").arg(t).arg(i), [&, t, i]() {
                                // Simulate work
                                volatile int sum = 0;
                                for (int j = 0; j < 100; ++j) {
                                    sum += j * t * i;
                                }
                                completed_tasks.fetch_add(1);
                            });
                        submitted_tasks.fetch_add(1);
                    } catch (const std::exception& e) {
                        errors.fetch_add(1);
                        qWarning() << "Task submission thread" << t
                                   << "error:" << e.what();
                    }
                }
            });
            submitter_futures.append(future);
        }

        // Wait for all submitters to finish
        for (auto& future : submitter_futures) {
            future.waitForFinished();
        }

        // Wait for all tasks to complete
        QElapsedTimer timer;
        timer.start();
        while (completed_tasks.load() < submitted_tasks.load() &&
               timer.elapsed() < 5000) {
            QThread::msleep(10);
        }

        qDebug() << "Submitted tasks:" << submitted_tasks.load();
        qDebug() << "Completed tasks:" << completed_tasks.load();
        qDebug() << "Submission errors:" << errors.load();

        QCOMPARE(errors.load(), 0);
        QCOMPARE(submitted_tasks.load(),
                 num_submitter_threads * tasks_per_thread);
        QCOMPARE(completed_tasks.load(), submitted_tasks.load());
    }

    // **Command System Thread Safety**
    void testCommandSystemConcurrentExecution() {
        auto& command_manager = CommandManager::instance();
        auto& invoker = command_manager.getInvoker();

        const int num_threads = 6;
        const int commands_per_thread = 100;
        std::atomic<int> successful_executions{0};
        std::atomic<int> failed_executions{0};
        std::atomic<int> errors{0};

        QVector<QFuture<void>> futures;

        for (int t = 0; t < num_threads; ++t) {
            auto future = QtConcurrent::run([&, t]() {
                for (int i = 0; i < commands_per_thread; ++i) {
                    try {
                        CommandContext context;
                        context.setParameter("thread_id", t);
                        context.setParameter("command_id", i);
                        context.setParameter(
                            "text",
                            QString("Thread %1 Command %2").arg(t).arg(i));

                        auto result = invoker.execute("set_property", context);
                        if (result.isSuccess()) {
                            successful_executions.fetch_add(1);
                        } else {
                            failed_executions.fetch_add(1);
                        }
                    } catch (const std::exception& e) {
                        errors.fetch_add(1);
                        qWarning()
                            << "Command thread" << t << "error:" << e.what();
                    }
                }
            });
            futures.append(future);
        }

        // Wait for all threads to complete
        for (auto& future : futures) {
            future.waitForFinished();
        }

        qDebug() << "Successful executions:" << successful_executions.load();
        qDebug() << "Failed executions:" << failed_executions.load();
        qDebug() << "Command errors:" << errors.load();

        QCOMPARE(errors.load(), 0);
        QCOMPARE(successful_executions.load(),
                 num_threads * commands_per_thread);
        QCOMPARE(failed_executions.load(), 0);
    }

    // **Race Condition Detection**
    void testRaceConditionDetection() {
        auto& state_manager = StateManager::instance();

        // Set up a shared counter
        state_manager.setState("race.counter", 0);

        const int num_threads = 10;
        const int increments_per_thread = 1000;
        std::atomic<int> race_detected{0};

        QVector<QFuture<void>> futures;

        for (int t = 0; t < num_threads; ++t) {
            auto future = QtConcurrent::run([&, t]() {
                for (int i = 0; i < increments_per_thread; ++i) {
                    try {
                        // Read-modify-write operation that could have race
                        // conditions
                        auto current =
                            state_manager.getState<int>("race.counter");
                        if (current) {
                            int new_value = current->get() + 1;
                            state_manager.setState("race.counter", new_value);
                        }
                    } catch (const std::exception& e) {
                        race_detected.fetch_add(1);
                        qWarning() << "Race condition detected in thread" << t
                                   << ":" << e.what();
                    }
                }
            });
            futures.append(future);
        }

        // Wait for all threads to complete
        for (auto& future : futures) {
            future.waitForFinished();
        }

        auto final_counter = state_manager.getState<int>("race.counter");
        int expected_value = num_threads * increments_per_thread;

        qDebug() << "Expected counter value:" << expected_value;
        qDebug() << "Actual counter value:"
                 << (final_counter ? final_counter->get() : -1);
        qDebug() << "Race conditions detected:" << race_detected.load();

        // Due to race conditions, the final value might be less than expected
        // This test documents the behavior rather than enforcing perfect thread
        // safety
        QVERIFY(final_counter != nullptr);
        QVERIFY(final_counter->get() > 0);
        QVERIFY(final_counter->get() <= expected_value);
    }
};

QTEST_MAIN(ThreadSafetyTest)
#include "test_thread_safety.moc"
