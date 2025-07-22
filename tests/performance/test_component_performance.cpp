#include <QApplication>
#include <QTest>
#include <QElapsedTimer>
#include <QThread>
#include <QFuture>
#include <QtConcurrent>
#include <memory>
#include <vector>
#include <atomic>
#include <chrono>

#include "../Components/Button.hpp"
#include "../Components/LineEdit.hpp"
#include "../Components/Label.hpp"
#include "../Components/ProgressBar.hpp"
#include "../Components/Slider.hpp"
#include "../Components/ComboBox.hpp"
#include "../Components/CheckBox.hpp"
#include "../Components/RadioButton.hpp"
#include "../Components/Dial.hpp"
#include "../Components/LCDNumber.hpp"
#include "../Core/DeclarativeBuilder.hpp"
#include "../Core/CacheManager.hpp"
#include "../Core/MemoryManager.hpp"
#include "../Core/ParallelProcessor.hpp"
#include "../Binding/StateManager.hpp"

using namespace DeclarativeUI::Components;
using namespace DeclarativeUI::Core;
using namespace DeclarativeUI::Binding;

class ComponentPerformanceTest : public QObject {
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

    // **Component Creation Performance**
    void benchmarkButtonCreation() {
        QBENCHMARK {
            auto button = std::make_unique<Button>();
            button->text("Benchmark Button")
                  .enabled(true)
                  .style("QPushButton { background-color: blue; }");
            button->initialize();
        }
    }

    void benchmarkComplexComponentCreation() {
        QBENCHMARK {
            auto line_edit = std::make_unique<LineEdit>();
            line_edit->text("Benchmark Text")
                     .placeholder("Enter text here")
                     .maxLength(100)
                     .echoMode(QLineEdit::Normal)
                     .readOnly(false);
            line_edit->initialize();
        }
    }

    void benchmarkMultipleComponentCreation() {
        const int num_components = 100;
        
        QBENCHMARK {
            std::vector<std::unique_ptr<Button>> buttons;
            buttons.reserve(num_components);
            
            for (int i = 0; i < num_components; ++i) {
                auto button = std::make_unique<Button>();
                button->text(QString("Button %1").arg(i));
                button->initialize();
                buttons.push_back(std::move(button));
            }
        }
    }

    void testComponentCreationScaling() {
        std::vector<int> sizes = {10, 50, 100, 500, 1000};
        
        for (int size : sizes) {
            QElapsedTimer timer;
            timer.start();
            
            std::vector<std::unique_ptr<Button>> buttons;
            buttons.reserve(size);
            
            for (int i = 0; i < size; ++i) {
                auto button = std::make_unique<Button>();
                button->text(QString("Scale Test %1").arg(i));
                button->initialize();
                buttons.push_back(std::move(button));
            }
            
            qint64 elapsed = timer.elapsed();
            double avg_time = (double)elapsed / size;
            
            qDebug() << "Created" << size << "buttons in" << elapsed << "ms"
                     << "(avg:" << avg_time << "ms per button)";
            
            // Performance should scale linearly
            QVERIFY(avg_time < 2.0); // Less than 2ms per button
        }
    }

    // **DeclarativeBuilder Performance**
    void benchmarkDeclarativeBuilderSimple() {
        QBENCHMARK {
            auto widget = create<QLabel>()
                .property("text", QString("Benchmark Label"))
                .property("alignment", Qt::AlignCenter)
                .build();
        }
    }

    void benchmarkDeclarativeBuilderComplex() {
        QBENCHMARK {
            auto widget = create<QWidget>()
                .property("windowTitle", QString("Complex Widget"))
                .layout<QVBoxLayout>()
                .child<QLabel>([](auto& label) {
                    label.property("text", QString("Header"));
                })
                .child<QLineEdit>([](auto& edit) {
                    edit.property("placeholderText", QString("Input"));
                })
                .child<QPushButton>([](auto& btn) {
                    btn.property("text", QString("Submit"));
                })
                .build();
        }
    }

    void testDeclarativeBuilderNesting() {
        const int nesting_levels = 10;
        
        QElapsedTimer timer;
        timer.start();
        
        // Create deeply nested structure
        auto widget = create<QWidget>()
            .property("windowTitle", QString("Nested Test"))
            .layout<QVBoxLayout>();
        
        // Add nested children
        for (int i = 0; i < nesting_levels; ++i) {
            widget.child<QWidget>([i](auto& child) {
                child.layout<QHBoxLayout>()
                     .child<QLabel>([i](auto& label) {
                         label.property("text", QString("Level %1").arg(i));
                     })
                     .child<QPushButton>([i](auto& btn) {
                         btn.property("text", QString("Button %1").arg(i));
                     });
            });
        }
        
        auto result = widget.build();
        
        qint64 elapsed = timer.elapsed();
        qDebug() << "Created" << nesting_levels << "nested levels in" << elapsed << "ms";
        
        QVERIFY(result != nullptr);
        QVERIFY(elapsed < 100); // Should complete quickly
    }

    // **Memory Performance**
    void testMemoryUsageScaling() {
        auto& memory_manager = MemoryManager::instance();
        
        // Get initial memory usage
        auto initial_stats = memory_manager.get_statistics();
        qDebug() << "Initial memory usage:" << initial_stats.current_usage << "bytes";
        
        std::vector<std::unique_ptr<Button>> buttons;
        const int num_buttons = 1000;
        
        for (int i = 0; i < num_buttons; ++i) {
            auto button = std::make_unique<Button>();
            button->text(QString("Memory Test %1").arg(i));
            button->initialize();
            buttons.push_back(std::move(button));
            
            // Check memory every 100 components
            if (i % 100 == 99) {
                auto current_stats = memory_manager.get_statistics();
                size_t memory_per_component = 
                    (current_stats.current_usage - initial_stats.current_usage) / (i + 1);
                
                qDebug() << "After" << (i + 1) << "components:"
                         << current_stats.current_usage << "bytes total,"
                         << memory_per_component << "bytes per component";
                
                // Memory per component should be reasonable
                QVERIFY(memory_per_component < 10000); // Less than 10KB per component
            }
        }
        
        // Clean up and verify memory is freed
        buttons.clear();
        
        // Force garbage collection if available
        memory_manager.optimize_memory_usage();
        
        auto final_stats = memory_manager.get_statistics();
        qDebug() << "Final memory usage:" << final_stats.current_usage << "bytes";
        
        // Memory should be mostly freed (allow some overhead)
        size_t memory_increase = final_stats.current_usage - initial_stats.current_usage;
        QVERIFY(memory_increase < initial_stats.current_usage / 2); // Less than 50% increase
    }

    void benchmarkCacheManagerOperations() {
        auto& cache_manager = CacheManager::instance();
        
        // Benchmark widget caching
        QBENCHMARK {
            QString key = QString("benchmark_widget_%1").arg(qrand());
            QWidget* widget = new QLabel("Benchmark Widget");
            
            cache_manager.storeWidget(key, widget);
            auto* retrieved = cache_manager.getWidget(key);
            Q_UNUSED(retrieved);
            
            cache_manager.removeWidget(key);
        }
    }

    void benchmarkStateManagerOperations() {
        auto& state_manager = StateManager::instance();
        
        // Benchmark state operations
        QBENCHMARK {
            QString key = QString("benchmark_state_%1").arg(qrand());
            QString value = QString("Benchmark Value %1").arg(qrand());
            
            state_manager.setState(key, value);
            auto retrieved = state_manager.getState<QString>(key);
            Q_UNUSED(retrieved);
            
            state_manager.removeState(key);
        }
    }

    // **Thread Safety Performance**
    void testConcurrentComponentCreation() {
        const int num_threads = 4;
        const int components_per_thread = 100;
        std::atomic<int> success_count{0};
        
        QElapsedTimer timer;
        timer.start();
        
        QVector<QFuture<void>> futures;
        
        for (int t = 0; t < num_threads; ++t) {
            auto future = QtConcurrent::run([&, t]() {
                for (int i = 0; i < components_per_thread; ++i) {
                    try {
                        auto button = std::make_unique<Button>();
                        button->text(QString("Thread %1 Button %2").arg(t).arg(i));
                        button->initialize();
                        
                        if (button->getWidget() != nullptr) {
                            success_count.fetch_add(1);
                        }
                    } catch (const std::exception& e) {
                        qWarning() << "Exception in thread" << t << ":" << e.what();
                    }
                }
            });
            futures.append(future);
        }
        
        // Wait for all threads to complete
        for (auto& future : futures) {
            future.waitForFinished();
        }
        
        qint64 elapsed = timer.elapsed();
        qDebug() << "Created" << (num_threads * components_per_thread) 
                 << "components concurrently in" << elapsed << "ms";
        qDebug() << "Success count:" << success_count.load();
        
        QCOMPARE(success_count.load(), num_threads * components_per_thread);
        QVERIFY(elapsed < 5000); // Should complete within 5 seconds
    }

    void testConcurrentStateOperations() {
        auto& state_manager = StateManager::instance();
        
        const int num_threads = 8;
        const int operations_per_thread = 500;
        std::atomic<int> success_count{0};
        
        QElapsedTimer timer;
        timer.start();
        
        QVector<QFuture<void>> futures;
        
        for (int t = 0; t < num_threads; ++t) {
            auto future = QtConcurrent::run([&, t]() {
                for (int i = 0; i < operations_per_thread; ++i) {
                    try {
                        QString key = QString("concurrent_%1_%2").arg(t).arg(i);
                        QString value = QString("Value_%1_%2").arg(t).arg(i);
                        
                        // Set state
                        state_manager.setState(key, value);
                        
                        // Get state
                        auto retrieved = state_manager.getState<QString>(key);
                        if (retrieved && retrieved->get() == value) {
                            success_count.fetch_add(1);
                        }
                    } catch (const std::exception& e) {
                        qWarning() << "Exception in thread" << t << ":" << e.what();
                    }
                }
            });
            futures.append(future);
        }
        
        // Wait for all threads to complete
        for (auto& future : futures) {
            future.waitForFinished();
        }
        
        qint64 elapsed = timer.elapsed();
        qDebug() << "Performed" << (num_threads * operations_per_thread)
                 << "state operations concurrently in" << elapsed << "ms";
        qDebug() << "Success count:" << success_count.load();
        
        QCOMPARE(success_count.load(), num_threads * operations_per_thread);
        QVERIFY(elapsed < 3000); // Should complete within 3 seconds
    }

    void testParallelProcessorPerformance() {
        auto processor = std::make_unique<ParallelProcessor>();
        
        const int num_tasks = 1000;
        std::atomic<int> completed_tasks{0};
        
        QElapsedTimer timer;
        timer.start();
        
        // Submit CPU-intensive tasks
        for (int i = 0; i < num_tasks; ++i) {
            processor->submitTask([&completed_tasks, i]() {
                // Simulate work
                volatile int sum = 0;
                for (int j = 0; j < 1000; ++j) {
                    sum += j * i;
                }
                completed_tasks.fetch_add(1);
            });
        }
        
        // Wait for all tasks to complete
        processor->waitForAll();
        
        qint64 elapsed = timer.elapsed();
        qDebug() << "Completed" << num_tasks << "parallel tasks in" << elapsed << "ms";
        qDebug() << "Completed tasks:" << completed_tasks.load();
        
        QCOMPARE(completed_tasks.load(), num_tasks);
        QVERIFY(elapsed < 5000); // Should complete within 5 seconds
    }

    // **Stress Testing**
    void testComponentStressTest() {
        const int stress_iterations = 10;
        const int components_per_iteration = 500;
        
        QElapsedTimer total_timer;
        total_timer.start();
        
        for (int iteration = 0; iteration < stress_iterations; ++iteration) {
            QElapsedTimer iteration_timer;
            iteration_timer.start();
            
            std::vector<std::unique_ptr<Button>> buttons;
            std::vector<std::unique_ptr<LineEdit>> line_edits;
            std::vector<std::unique_ptr<Label>> labels;
            
            // Create mixed components
            for (int i = 0; i < components_per_iteration; ++i) {
                if (i % 3 == 0) {
                    auto button = std::make_unique<Button>();
                    button->text(QString("Stress Button %1").arg(i));
                    button->initialize();
                    buttons.push_back(std::move(button));
                } else if (i % 3 == 1) {
                    auto edit = std::make_unique<LineEdit>();
                    edit->text(QString("Stress Text %1").arg(i));
                    edit->initialize();
                    line_edits.push_back(std::move(edit));
                } else {
                    auto label = std::make_unique<Label>();
                    label->text(QString("Stress Label %1").arg(i));
                    label->initialize();
                    labels.push_back(std::move(label));
                }
            }
            
            qint64 iteration_elapsed = iteration_timer.elapsed();
            qDebug() << "Stress iteration" << iteration << "completed in" 
                     << iteration_elapsed << "ms";
            
            QVERIFY(iteration_elapsed < 2000); // Each iteration should be fast
            
            // Components will be destroyed at end of iteration
        }
        
        qint64 total_elapsed = total_timer.elapsed();
        qDebug() << "Stress test completed in" << total_elapsed << "ms";
        qDebug() << "Total components created:" << (stress_iterations * components_per_iteration);
        
        QVERIFY(total_elapsed < 20000); // Total should complete within 20 seconds
    }
};

QTEST_MAIN(ComponentPerformanceTest)
#include "test_component_performance.moc"
