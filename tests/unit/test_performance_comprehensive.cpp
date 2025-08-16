#include <QTest>
#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QProgressBar>
#include <QElapsedTimer>
#include <QApplication>
#include <QThread>
#include <QTimer>
#include <QSignalSpy>
#include <atomic>
#include <vector>
#include <memory>

class PerformanceComprehensiveTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Widget Creation Performance
    void testWidgetCreationPerformance();
    void testLayoutPerformance();
    void testSignalSlotPerformance();
    void testPropertyAccessPerformance();

    // Memory Performance
    void testMemoryAllocationPerformance();

    // Threading Performance
    void testThreadCreationPerformance();
    void testConcurrentOperations();

    // UI Update Performance
    void testUIUpdatePerformance();

    // Stress Tests
    void testStressWidgetCreation();
    void testStressSignalEmission();
    void testStressMemoryOperations();

private:
    std::unique_ptr<QWidget> test_widget_;
    QElapsedTimer timer_;
};

void PerformanceComprehensiveTest::initTestCase() {
    // Ensure QApplication exists
    if (!QApplication::instance()) {
        int argc = 0;
        char* argv[] = {nullptr};
        new QApplication(argc, argv);
    }
}

void PerformanceComprehensiveTest::cleanupTestCase() {
    // Cleanup handled by QApplication
}

void PerformanceComprehensiveTest::init() {
    test_widget_ = std::make_unique<QWidget>();
    test_widget_->resize(800, 600);
    test_widget_->show();
}

void PerformanceComprehensiveTest::cleanup() {
    test_widget_.reset();
}

void PerformanceComprehensiveTest::testWidgetCreationPerformance() {
    const int num_widgets = 1000;
    std::vector<std::unique_ptr<QWidget>> widgets;
    widgets.reserve(num_widgets);
    
    timer_.start();
    
    // Test widget creation
    for (int i = 0; i < num_widgets; ++i) {
        auto widget = std::make_unique<QWidget>();
        widget->setObjectName(QString("Widget_%1").arg(i));
        widget->resize(100, 50);
        widgets.push_back(std::move(widget));
    }
    
    qint64 creation_time = timer_.elapsed();
    
    // Test widget destruction
    timer_.restart();
    widgets.clear();
    qint64 destruction_time = timer_.elapsed();
    
    qDebug() << "Widget Performance:";
    qDebug() << "Created" << num_widgets << "widgets in" << creation_time << "ms";
    qDebug() << "Destroyed" << num_widgets << "widgets in" << destruction_time << "ms";
    qDebug() << "Average creation time:" << (double)creation_time / num_widgets << "ms per widget";
    
    // Performance assertions
    QVERIFY(creation_time < 1000); // Should create within 1 second
    QVERIFY(destruction_time < 500); // Should destroy within 0.5 seconds
    QVERIFY((double)creation_time / num_widgets < 1.0); // Less than 1ms per widget
}

void PerformanceComprehensiveTest::testLayoutPerformance() {
    const int num_widgets = 500;
    auto layout = new QVBoxLayout(test_widget_.get());
    
    std::vector<std::unique_ptr<QLabel>> labels;
    labels.reserve(num_widgets);
    
    timer_.start();
    
    // Create and add widgets to layout
    for (int i = 0; i < num_widgets; ++i) {
        auto label = std::make_unique<QLabel>(QString("Label %1").arg(i));
        layout->addWidget(label.get());
        labels.push_back(std::move(label));
    }
    
    qint64 layout_time = timer_.elapsed();
    
    // Test layout update
    timer_.restart();
    test_widget_->updateGeometry();
    test_widget_->update();
    qint64 update_time = timer_.elapsed();
    
    qDebug() << "Layout Performance:";
    qDebug() << "Added" << num_widgets << "widgets to layout in" << layout_time << "ms";
    qDebug() << "Updated layout in" << update_time << "ms";
    
    // Performance assertions
    QVERIFY(layout_time < 1000); // Should layout within 1 second
    QVERIFY(update_time < 500); // Should update within 0.5 seconds
    
    // Cleanup
    for (auto& label : labels) {
        layout->removeWidget(label.get());
        (void)label.release(); // Let Qt handle deletion
    }
}

void PerformanceComprehensiveTest::testSignalSlotPerformance() {
    const int num_signals = 10000;
    
    auto button = new QPushButton("Test Button", test_widget_.get());
    std::atomic<int> signal_count{0};
    
    // Connect signal to lambda
    QObject::connect(button, &QPushButton::clicked, [&signal_count]() {
        signal_count++;
    });
    
    timer_.start();
    
    // Emit many signals
    for (int i = 0; i < num_signals; ++i) {
        emit button->clicked();
    }
    
    qint64 signal_time = timer_.elapsed();
    
    qDebug() << "Signal/Slot Performance:";
    qDebug() << "Emitted" << num_signals << "signals in" << signal_time << "ms";
    qDebug() << "Average signal time:" << (double)signal_time / num_signals << "ms per signal";
    
    QVERIFY(signal_count == num_signals);
    QVERIFY(signal_time < 1000); // Should complete within 1 second
    QVERIFY((double)signal_time / num_signals < 0.1); // Less than 0.1ms per signal
}

void PerformanceComprehensiveTest::testPropertyAccessPerformance() {
    const int num_operations = 100000;
    
    auto label = new QLabel("Test Label", test_widget_.get());
    
    // Test property setting performance
    timer_.start();
    
    for (int i = 0; i < num_operations; ++i) {
        label->setText(QString("Text %1").arg(i % 100));
    }
    
    qint64 set_time = timer_.elapsed();
    
    // Test property getting performance
    timer_.restart();
    
    QString text;
    for (int i = 0; i < num_operations; ++i) {
        text = label->text();
    }
    
    qint64 get_time = timer_.elapsed();
    
    qDebug() << "Property Access Performance:";
    qDebug() << "Set property" << num_operations << "times in" << set_time << "ms";
    qDebug() << "Get property" << num_operations << "times in" << get_time << "ms";
    
    // Performance assertions
    QVERIFY(set_time < 2000); // Should set within 2 seconds
    QVERIFY(get_time < 1000); // Should get within 1 second
    QVERIFY(!text.isEmpty()); // Ensure we actually got the text
}

void PerformanceComprehensiveTest::testMemoryAllocationPerformance() {
    const int num_allocations = 10000;
    const size_t allocation_size = 1024; // 1KB per allocation
    
    std::vector<std::unique_ptr<char[]>> allocations;
    allocations.reserve(num_allocations);
    
    timer_.start();
    
    // Test allocation performance
    for (int i = 0; i < num_allocations; ++i) {
        auto ptr = std::make_unique<char[]>(allocation_size);
        // Write to memory to ensure it's actually allocated
        ptr[0] = static_cast<char>(i % 256);
        ptr[allocation_size - 1] = static_cast<char>(i % 256);
        allocations.push_back(std::move(ptr));
    }
    
    qint64 allocation_time = timer_.elapsed();
    
    // Test deallocation performance
    timer_.restart();
    allocations.clear();
    qint64 deallocation_time = timer_.elapsed();
    
    qDebug() << "Memory Allocation Performance:";
    qDebug() << "Allocated" << num_allocations << "blocks (" << (num_allocations * allocation_size / 1024) << "KB total) in" << allocation_time << "ms";
    qDebug() << "Deallocated in" << deallocation_time << "ms";
    
    // Performance assertions
    QVERIFY(allocation_time < 1000); // Should allocate within 1 second
    QVERIFY(deallocation_time < 500); // Should deallocate within 0.5 seconds
}

void PerformanceComprehensiveTest::testThreadCreationPerformance() {
    const int num_threads = 100;
    std::vector<std::unique_ptr<QThread>> threads;
    threads.reserve(num_threads);
    
    timer_.start();
    
    // Create threads
    for (int i = 0; i < num_threads; ++i) {
        auto thread = std::make_unique<QThread>();
        thread->start();
        threads.push_back(std::move(thread));
    }
    
    qint64 creation_time = timer_.elapsed();
    
    // Wait for all threads to finish
    for (auto& thread : threads) {
        thread->quit();
        QVERIFY(thread->wait(1000)); // Wait up to 1 second for each thread
    }
    
    qint64 total_time = timer_.elapsed();
    
    qDebug() << "Thread Performance:";
    qDebug() << "Created and started" << num_threads << "threads in" << creation_time << "ms";
    qDebug() << "Total time including wait:" << total_time << "ms";
    
    // Performance assertions
    QVERIFY(creation_time < 2000); // Should create within 2 seconds
    QVERIFY(total_time < 5000); // Should complete within 5 seconds
}

void PerformanceComprehensiveTest::testConcurrentOperations() {
    const int num_threads = 4;
    const int operations_per_thread = 1000;
    
    std::atomic<int> completed_operations{0};
    std::vector<std::thread> threads;
    
    timer_.start();
    
    // Create worker threads
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([&completed_operations, operations_per_thread]() {
            for (int i = 0; i < operations_per_thread; ++i) {
                // Simulate some work
                volatile int dummy = 0;
                for (int j = 0; j < 1000; ++j) {
                    dummy += j;
                }
                completed_operations++;
            }
        });
    }
    
    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }
    
    qint64 concurrent_time = timer_.elapsed();
    
    qDebug() << "Concurrent Operations Performance:";
    qDebug() << "Completed" << (num_threads * operations_per_thread) << "operations across" << num_threads << "threads in" << concurrent_time << "ms";
    qDebug() << "Operations per second:" << (double)(num_threads * operations_per_thread) * 1000 / concurrent_time;
    
    QVERIFY(completed_operations == num_threads * operations_per_thread);
    QVERIFY(concurrent_time < 5000); // Should complete within 5 seconds
}

void PerformanceComprehensiveTest::testUIUpdatePerformance() {
    const int num_updates = 1000;
    
    auto label = new QLabel("Initial Text", test_widget_.get());
    auto progress_bar = new QProgressBar(test_widget_.get());
    progress_bar->setRange(0, 100);
    
    auto layout = new QVBoxLayout(test_widget_.get());
    layout->addWidget(label);
    layout->addWidget(progress_bar);
    
    timer_.start();
    
    // Perform many UI updates
    for (int i = 0; i < num_updates; ++i) {
        label->setText(QString("Update %1").arg(i));
        progress_bar->setValue(i % 101);
        
        // Force immediate update
        QApplication::processEvents();
    }
    
    qint64 update_time = timer_.elapsed();
    
    qDebug() << "UI Update Performance:";
    qDebug() << "Performed" << num_updates << "UI updates in" << update_time << "ms";
    qDebug() << "Average update time:" << (double)update_time / num_updates << "ms per update";
    
    // Performance assertions
    QVERIFY(update_time < 5000); // Should update within 5 seconds
    QVERIFY((double)update_time / num_updates < 5.0); // Less than 5ms per update
}

void PerformanceComprehensiveTest::testStressWidgetCreation() {
    const int stress_widgets = 5000;
    std::vector<std::unique_ptr<QWidget>> stress_widgets_vec;
    stress_widgets_vec.reserve(stress_widgets);
    
    timer_.start();
    
    // Stress test widget creation
    for (int i = 0; i < stress_widgets; ++i) {
        auto widget = std::make_unique<QLabel>(QString("Stress Widget %1").arg(i));
        widget->setStyleSheet("color: blue; font-weight: bold;");
        widget->setToolTip(QString("Tooltip for widget %1").arg(i));
        widget->resize(200, 30);
        stress_widgets_vec.push_back(std::move(widget));
        
        // Periodically process events to prevent UI freeze
        if (i % 100 == 0) {
            QApplication::processEvents();
        }
    }
    
    qint64 stress_time = timer_.elapsed();
    
    qDebug() << "Stress Test Results:";
    qDebug() << "Created" << stress_widgets << "complex widgets in" << stress_time << "ms";
    qDebug() << "Memory usage (approximate):" << (stress_widgets * sizeof(QLabel)) / 1024 << "KB";
    
    // Performance assertions for stress test
    QVERIFY(stress_time < 10000); // Should complete within 10 seconds
    QVERIFY(stress_widgets_vec.size() == stress_widgets);
    
    // Cleanup stress test widgets
    timer_.restart();
    stress_widgets_vec.clear();
    qint64 cleanup_time = timer_.elapsed();
    
    qDebug() << "Cleaned up stress widgets in" << cleanup_time << "ms";
    QVERIFY(cleanup_time < 2000); // Should cleanup within 2 seconds
}

void PerformanceComprehensiveTest::testStressSignalEmission() {
    const int stress_signals = 50000;
    
    auto button = new QPushButton("Stress Test Button", test_widget_.get());
    std::atomic<int> signal_counter{0};
    
    // Connect multiple slots to the same signal
    for (int i = 0; i < 5; ++i) {
        QObject::connect(button, &QPushButton::clicked, [&signal_counter]() {
            signal_counter++;
            // Simulate some processing
            volatile int dummy = 0;
            for (int j = 0; j < 10; ++j) {
                dummy += j;
            }
        });
    }
    
    timer_.start();
    
    // Emit many signals
    for (int i = 0; i < stress_signals; ++i) {
        emit button->clicked();
        
        // Periodically process events
        if (i % 1000 == 0) {
            QApplication::processEvents();
        }
    }
    
    qint64 stress_signal_time = timer_.elapsed();
    
    qDebug() << "Stress Signal Test Results:";
    qDebug() << "Emitted" << stress_signals << "signals (with 5 slots each) in" << stress_signal_time << "ms";
    qDebug() << "Total slot executions:" << signal_counter.load();
    qDebug() << "Signals per second:" << (double)stress_signals * 1000 / stress_signal_time;
    
    QVERIFY(signal_counter == stress_signals * 5); // 5 slots per signal
    QVERIFY(stress_signal_time < 5000); // Should complete within 5 seconds
}

void PerformanceComprehensiveTest::testStressMemoryOperations() {
    const int stress_allocations = 50000;
    const size_t min_size = 64;
    const size_t max_size = 4096;
    
    std::vector<std::unique_ptr<char[]>> stress_memory;
    stress_memory.reserve(stress_allocations);
    
    timer_.start();
    
    // Stress test memory operations with varying sizes
    for (int i = 0; i < stress_allocations; ++i) {
        size_t size = min_size + (i % (max_size - min_size));
        auto ptr = std::make_unique<char[]>(size);
        
        // Write pattern to memory
        for (size_t j = 0; j < size; j += 64) {
            ptr[j] = static_cast<char>(i % 256);
        }
        
        stress_memory.push_back(std::move(ptr));
        
        // Occasionally free some memory to test fragmentation
        if (i > 1000 && i % 100 == 0) {
            // Remove every 10th allocation
            for (size_t k = 0; k < stress_memory.size(); k += 10) {
                if (k < stress_memory.size()) {
                    stress_memory.erase(stress_memory.begin() + k);
                }
            }
        }
    }
    
    qint64 stress_memory_time = timer_.elapsed();
    
    qDebug() << "Stress Memory Test Results:";
    qDebug() << "Performed" << stress_allocations << "variable-size allocations in" << stress_memory_time << "ms";
    qDebug() << "Final allocations remaining:" << stress_memory.size();
    qDebug() << "Allocations per second:" << (double)stress_allocations * 1000 / stress_memory_time;
    
    // Performance assertions
    QVERIFY(stress_memory_time < 5000); // Should complete within 5 seconds
    QVERIFY(stress_memory.size() > 0); // Should have some allocations remaining
    
    // Cleanup
    timer_.restart();
    stress_memory.clear();
    qint64 final_cleanup_time = timer_.elapsed();
    
    qDebug() << "Final cleanup took" << final_cleanup_time << "ms";
    QVERIFY(final_cleanup_time < 1000); // Should cleanup within 1 second
}

QTEST_MAIN(PerformanceComprehensiveTest)
#include "test_performance_comprehensive.moc"
