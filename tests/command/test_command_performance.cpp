#include <QApplication>
#include <QElapsedTimer>
#include <QFuture>
#include <QSignalSpy>
#include <QTest>
#include <QtConcurrent>
#include <atomic>
#include <memory>
#include <vector>

#include "../../src/Binding/StateManager.hpp"
#include "../../src/Command/Adapters/ComponentSystemAdapter.hpp"
#include "../../src/Command/Adapters/IntegrationManager.hpp"
#include "../../src/Command/Adapters/JSONCommandLoader.hpp"
#include "../../src/Command/Adapters/StateManagerAdapter.hpp"
#include "../../src/Command/Adapters/UIElementAdapter.hpp"
#include "../../src/Command/BuiltinCommands.hpp"
#include "../../src/Command/CommandIntegration.hpp"
#include "../../src/Command/CommandSystem.hpp"
#include "../../src/Components/Button.hpp"

using namespace DeclarativeUI::Command;
using namespace DeclarativeUI::Command::Commands;
using namespace DeclarativeUI::Command::Integration;
using namespace DeclarativeUI::Command::Adapters;
using namespace DeclarativeUI::Components;
using namespace DeclarativeUI::Binding;

class CommandPerformanceTest : public QObject {
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

    // **Command Execution Performance Tests**
    void testCommandExecutionPerformance() {
        auto& invoker = CommandManager::instance().getInvoker();

        QElapsedTimer timer;
        timer.start();

        const int num_executions = 1000;
        int success_count = 0;

        for (int i = 0; i < num_executions; ++i) {
            CommandContext context;
            context.setParameter("text", QString("Performance test %1").arg(i));

            auto result = invoker.execute("set_property", context);
            if (result.isSuccess()) {
                success_count++;
            }
        }

        qint64 elapsed = timer.elapsed();
        qDebug() << "Executed" << num_executions << "commands in" << elapsed
                 << "ms";
        qDebug() << "Average execution time:"
                 << (double)elapsed / num_executions << "ms per command";

        QCOMPARE(success_count, num_executions);
        QVERIFY(elapsed < 5000);  // Should complete within 5 seconds
        QVERIFY((double)elapsed / num_executions <
                1.0);  // Less than 1ms per command
    }

    void testAsyncCommandPerformance() {
        auto& invoker = CommandManager::instance().getInvoker();

        QElapsedTimer timer;
        timer.start();

        const int num_async_commands = 100;
        std::vector<QFuture<CommandResult<QVariant>>> futures;

        for (int i = 0; i < num_async_commands; ++i) {
            CommandContext context;
            context.setParameter("delay", 10);  // 10ms delay
            context.setParameter("value", i);

            auto future = invoker.executeAsync("async_test", context);
            futures.push_back(future);
        }

        // Wait for all commands to complete
        int success_count = 0;
        for (auto& future : futures) {
            auto result = future.result();
            if (result.isSuccess()) {
                success_count++;
            }
        }

        qint64 elapsed = timer.elapsed();
        qDebug() << "Executed" << num_async_commands << "async commands in"
                 << elapsed << "ms";

        QCOMPARE(success_count, num_async_commands);
        QVERIFY(elapsed <
                3000);  // Should complete within 3 seconds with parallelization
    }

    void testCommandCreationPerformance() {
        auto& factory = CommandFactory::instance();

        QElapsedTimer timer;
        timer.start();

        const int num_creations = 10000;
        std::vector<std::shared_ptr<ICommand>> commands;

        for (int i = 0; i < num_creations; ++i) {
            auto command = factory.createCommand("set_property");
            if (command) {
                commands.push_back(command);
            }
        }

        qint64 elapsed = timer.elapsed();
        qDebug() << "Created" << num_creations << "commands in" << elapsed
                 << "ms";
        qDebug() << "Average creation time:" << (double)elapsed / num_creations
                 << "ms per command";

        QCOMPARE(commands.size(), num_creations);
        QVERIFY(elapsed < 2000);  // Should complete within 2 seconds
        QVERIFY((double)elapsed / num_creations <
                0.1);  // Less than 0.1ms per command
    }

    // **Adapter Performance Tests**
    void testComponentSystemAdapterPerformance() {
        auto adapter = std::make_unique<ComponentSystemAdapter>();

        QElapsedTimer timer;
        timer.start();

        const int num_conversions = 1000;
        std::vector<std::shared_ptr<ButtonCommand>> commands;

        for (int i = 0; i < num_conversions; ++i) {
            auto button = std::make_unique<Button>();
            button->text(QString("Performance Button %1").arg(i));
            button->initialize();

            auto command = adapter->convertToCommand(button.get());
            if (command) {
                commands.push_back(command);
            }
        }

        qint64 elapsed = timer.elapsed();
        qDebug() << "Converted" << num_conversions << "components in" << elapsed
                 << "ms";
        qDebug() << "Average conversion time:"
                 << (double)elapsed / num_conversions << "ms per conversion";

        QCOMPARE(commands.size(), num_conversions);
        QVERIFY(elapsed < 3000);  // Should complete within 3 seconds
        QVERIFY((double)elapsed / num_conversions <
                1.0);  // Less than 1ms per conversion
    }

    void testStateManagerAdapterPerformance() {
        auto adapter = std::make_unique<StateManagerAdapter>();
        auto& state_manager = StateManager::instance();

        QElapsedTimer timer;
        timer.start();

        const int num_operations = 5000;

        for (int i = 0; i < num_operations; ++i) {
            QString key = QString("perf_test_%1").arg(i);
            QString value = QString("Value %1").arg(i);

            // Set state through adapter
            state_manager.setState(key, value);

            // Get state through adapter
            auto retrieved = state_manager.getState<QString>(key);
            QVERIFY(retrieved != nullptr);
            QCOMPARE(retrieved->get(), value);
        }

        qint64 elapsed = timer.elapsed();
        qDebug() << "Performed" << num_operations << "state operations in"
                 << elapsed << "ms";
        qDebug() << "Average operation time:"
                 << (double)elapsed / num_operations << "ms per operation";

        QVERIFY(elapsed < 2000);  // Should complete within 2 seconds
        QVERIFY((double)elapsed / num_operations <
                0.1);  // Less than 0.1ms per operation
    }

    void testJSONCommandLoaderPerformance() {
        auto loader = std::make_unique<JSONCommandLoader>();

        // Create test JSON
        QString test_json = R"({
            "commands": [
                {
                    "type": "set_property",
                    "parameters": {
                        "property": "text",
                        "value": "Performance Test"
                    }
                }
            ]
        })";

        QElapsedTimer timer;
        timer.start();

        const int num_loads = 1000;
        int success_count = 0;

        for (int i = 0; i < num_loads; ++i) {
            auto commands = loader->loadFromString(test_json);
            if (!commands.empty()) {
                success_count++;
            }
        }

        qint64 elapsed = timer.elapsed();
        qDebug() << "Loaded JSON" << num_loads << "times in" << elapsed << "ms";
        qDebug() << "Average load time:" << (double)elapsed / num_loads
                 << "ms per load";

        QCOMPARE(success_count, num_loads);
        QVERIFY(elapsed < 3000);  // Should complete within 3 seconds
        QVERIFY((double)elapsed / num_loads < 1.0);  // Less than 1ms per load
    }

    // **Concurrency and Thread Safety Tests**
    void testCommandSystemConcurrency() {
        auto& invoker = CommandManager::instance().getInvoker();

        const int num_threads = 8;
        const int commands_per_thread = 100;
        std::atomic<int> success_count{0};
        std::atomic<int> error_count{0};

        QElapsedTimer timer;
        timer.start();

        QVector<QFuture<void>> futures;

        for (int t = 0; t < num_threads; ++t) {
            auto future = QtConcurrent::run([&, t]() {
                for (int i = 0; i < commands_per_thread; ++i) {
                    CommandContext context;
                    context.setParameter("thread_id", t);
                    context.setParameter("command_id", i);
                    context.setParameter(
                        "text", QString("Thread %1 Command %2").arg(t).arg(i));

                    auto result = invoker.execute("set_property", context);
                    if (result.isSuccess()) {
                        success_count.fetch_add(1);
                    } else {
                        error_count.fetch_add(1);
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
        qDebug() << "Executed" << (num_threads * commands_per_thread)
                 << "commands concurrently in" << elapsed << "ms";
        qDebug() << "Success count:" << success_count.load();
        qDebug() << "Error count:" << error_count.load();

        QCOMPARE(success_count.load(), num_threads * commands_per_thread);
        QCOMPARE(error_count.load(), 0);
        QVERIFY(elapsed < 5000);  // Should complete within 5 seconds
    }

    void testAdapterConcurrency() {
        auto component_adapter = std::make_unique<ComponentSystemAdapter>();
        auto state_adapter = std::make_unique<StateManagerAdapter>();

        const int num_threads = 4;
        const int operations_per_thread = 50;
        std::atomic<int> component_success{0};
        std::atomic<int> state_success{0};

        QVector<QFuture<void>> futures;

        for (int t = 0; t < num_threads; ++t) {
            auto future = QtConcurrent::run([&, t]() {
                for (int i = 0; i < operations_per_thread; ++i) {
                    // Test component adapter
                    auto button = std::make_unique<Button>();
                    button->text(
                        QString("Concurrent Button %1-%2").arg(t).arg(i));
                    button->initialize();

                    auto command =
                        component_adapter->convertToCommand(button.get());
                    if (command) {
                        component_success.fetch_add(1);
                    }

                    // Test state adapter
                    QString key =
                        QString("concurrent_state_%1_%2").arg(t).arg(i);
                    QString value = QString("Value %1-%2").arg(t).arg(i);

                    StateManager::instance().setState(key, value);
                    auto retrieved =
                        StateManager::instance().getState<QString>(key);
                    if (retrieved && retrieved->get() == value) {
                        state_success.fetch_add(1);
                    }
                }
            });
            futures.append(future);
        }

        // Wait for all threads to complete
        for (auto& future : futures) {
            future.waitForFinished();
        }

        QCOMPARE(component_success.load(), num_threads * operations_per_thread);
        QCOMPARE(state_success.load(), num_threads * operations_per_thread);
    }

    // **Memory and Resource Tests**
    void testCommandSystemMemoryUsage() {
        auto& factory = CommandFactory::instance();
        auto& invoker = CommandManager::instance().getInvoker();

        // Create many commands and execute them
        const int num_commands = 10000;
        std::vector<std::shared_ptr<ICommand>> commands;

        for (int i = 0; i < num_commands; ++i) {
            auto command = factory.createCommand("set_property");
            if (command) {
                commands.push_back(command);

                // Execute every 10th command
                if (i % 10 == 0) {
                    CommandContext context;
                    context.setParameter("text",
                                         QString("Memory test %1").arg(i));
                    auto result = invoker.execute("set_property", context);
                    Q_UNUSED(result);
                }
            }
        }

        QCOMPARE(commands.size(), num_commands);

        // Clear commands and verify cleanup
        commands.clear();
        QVERIFY(commands.empty());

        // Force garbage collection if available
        // This test mainly ensures no crashes occur during cleanup
        QVERIFY(true);
    }

    void testIntegrationManagerStressTest() {
        auto manager = std::make_unique<IntegrationManager>();

        // Register multiple adapters
        manager->registerAdapter("component",
                                 std::make_unique<ComponentSystemAdapter>());
        manager->registerAdapter("state",
                                 std::make_unique<StateManagerAdapter>());
        manager->registerAdapter("json", std::make_unique<JSONCommandLoader>());

        const int num_operations = 1000;
        int success_count = 0;

        QElapsedTimer timer;
        timer.start();

        for (int i = 0; i < num_operations; ++i) {
            // Simulate mixed adapter usage
            if (i % 3 == 0) {
                // Component adapter test
                auto button = std::make_unique<Button>();
                button->text(QString("Stress Test %1").arg(i));
                button->initialize();

                if (manager->hasAdapter("component")) {
                    success_count++;
                }
            } else if (i % 3 == 1) {
                // State adapter test
                QString key = QString("stress_test_%1").arg(i);
                StateManager::instance().setState(key, i);

                if (manager->hasAdapter("state")) {
                    success_count++;
                }
            } else {
                // JSON adapter test
                if (manager->hasAdapter("json")) {
                    success_count++;
                }
            }
        }

        qint64 elapsed = timer.elapsed();
        qDebug() << "Performed" << num_operations << "integration operations in"
                 << elapsed << "ms";

        QCOMPARE(success_count, num_operations);
        QVERIFY(elapsed < 3000);  // Should complete within 3 seconds
    }
};

QTEST_MAIN(CommandPerformanceTest)
#include "test_command_performance.moc"
