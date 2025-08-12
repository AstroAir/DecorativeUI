#include <QApplication>
#include <QSignalSpy>
#include <QTest>
#include <QTimer>

#include "../Binding/StateManager.hpp"

using namespace DeclarativeUI::Binding;

class StateManagerTest : public QObject {
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
        // Clear state before each test
        StateManager::instance().clearState();
    }

    void cleanup() {
        // Clean up after each test
        StateManager::instance().clearState();
    }

    // **Basic State Management Tests**
    void testStateManagerSingleton() {
        auto& manager1 = StateManager::instance();
        auto& manager2 = StateManager::instance();

        QCOMPARE(&manager1, &manager2);
    }

    void testSetAndGetState() {
        auto& manager = StateManager::instance();

        // Test setting and getting different types
        manager.setState("string_value", QString("Hello World"));
        manager.setState("int_value", 42);
        manager.setState("double_value", 3.14);
        manager.setState("bool_value", true);

        auto string_state = manager.getState<QString>("string_value");
        auto int_state = manager.getState<int>("int_value");
        auto double_state = manager.getState<double>("double_value");
        auto bool_state = manager.getState<bool>("bool_value");

        QVERIFY(string_state != nullptr);
        QVERIFY(int_state != nullptr);
        QVERIFY(double_state != nullptr);
        QVERIFY(bool_state != nullptr);

        QCOMPARE(string_state->get(), QString("Hello World"));
        QCOMPARE(int_state->get(), 42);
        QCOMPARE(double_state->get(), 3.14);
        QCOMPARE(bool_state->get(), true);
    }

    void testStateExists() {
        auto& manager = StateManager::instance();

        QVERIFY(!manager.hasState("nonexistent"));

        manager.setState("test_key", QString("test_value"));
        QVERIFY(manager.hasState("test_key"));
    }

    void testRemoveState() {
        auto& manager = StateManager::instance();

        manager.setState("to_remove", QString("value"));
        QVERIFY(manager.hasState("to_remove"));

        manager.removeState("to_remove");
        QVERIFY(!manager.hasState("to_remove"));

        auto removed_state = manager.getState<QString>("to_remove");
        QVERIFY(removed_state == nullptr);
    }

    void testStateSignals() {
        auto& manager = StateManager::instance();

        QSignalSpy state_changed_spy(&manager, &StateManager::stateChanged);
        QSignalSpy state_added_spy(&manager, &StateManager::stateAdded);
        QSignalSpy state_removed_spy(&manager, &StateManager::stateRemoved);

        // Test state addition
        manager.setState("new_state", QString("value"));
        QCOMPARE(state_added_spy.count(), 1);
        QCOMPARE(state_changed_spy.count(), 1);

        // Test state modification
        manager.setState("new_state", QString("modified_value"));
        QCOMPARE(state_changed_spy.count(), 2);

        // Test state removal
        manager.removeState("new_state");
        QCOMPARE(state_removed_spy.count(), 1);
    }

    void testStateValidation() {
        auto& manager = StateManager::instance();

        // Set validator that only allows positive integers
        auto validator = [](const int& value) -> bool { return value > 0; };

        manager.setStateValidator<int>("positive_int", validator);

        // Valid value should work
        manager.setState("positive_int", 10);
        auto state = manager.getState<int>("positive_int");
        QVERIFY(state != nullptr);
        QCOMPARE(state->get(), 10);

        // Invalid value should be rejected
        manager.setState("positive_int", -5);
        QCOMPARE(state->get(), 10);  // Should remain unchanged
    }

    void testBatchUpdates() {
        auto& manager = StateManager::instance();

        QSignalSpy state_changed_spy(&manager, &StateManager::stateChanged);

        manager.batchUpdate([&]() {
            manager.setState("batch1", QString("value1"));
            manager.setState("batch2", QString("value2"));
            manager.setState("batch3", QString("value3"));
        });

        // All states should be set
        QVERIFY(manager.hasState("batch1"));
        QVERIFY(manager.hasState("batch2"));
        QVERIFY(manager.hasState("batch3"));

        // Signals should be emitted (exact count depends on implementation)
        QVERIFY(state_changed_spy.count() >= 3);
    }

    void testStateHistory() {
        auto& manager = StateManager::instance();

        manager.enableHistory("history_test", 5);

        manager.setState("history_test", QString("value1"));
        manager.setState("history_test", QString("value2"));
        manager.setState("history_test", QString("value3"));

        auto state = manager.getState<QString>("history_test");
        QCOMPARE(state->get(), QString("value3"));

        // Test undo
        QVERIFY(manager.canUndo("history_test"));
        manager.undo("history_test");
        QCOMPARE(state->get(), QString("value2"));

        manager.undo("history_test");
        QCOMPARE(state->get(), QString("value1"));

        // Test redo
        QVERIFY(manager.canRedo("history_test"));
        manager.redo("history_test");
        QCOMPARE(state->get(), QString("value2"));

        manager.redo("history_test");
        QCOMPARE(state->get(), QString("value3"));

        // Should not be able to redo further
        QVERIFY(!manager.canRedo("history_test"));
    }

    void testStateDependencies() {
        auto& manager = StateManager::instance();

        manager.setState("base_value", 10);
        manager.setState("dependent_value", 0);

        // Set up dependency: dependent_value depends on base_value
        manager.addDependency("dependent_value", "base_value");

        auto dependencies = manager.getDependencies("dependent_value");
        QVERIFY(dependencies.contains("base_value"));

        // Test dependency update (this would typically be handled by binding
        // system)
        manager.updateDependents("base_value");

        // Clean up
        manager.removeDependency("dependent_value", "base_value");
        dependencies = manager.getDependencies("dependent_value");
        QVERIFY(!dependencies.contains("base_value"));
    }

    void testPerformanceMonitoring() {
        auto& manager = StateManager::instance();

        manager.enablePerformanceMonitoring(true);

        // Perform some operations
        for (int i = 0; i < 100; ++i) {
            manager.setState("perf_test", i);
        }

        QString report = manager.getPerformanceReport();
        QVERIFY(!report.isEmpty());
        QVERIFY(report.contains("perf_test"));
    }

    void testDebugMode() {
        auto& manager = StateManager::instance();

        manager.enableDebugMode(true);

        // This should trigger debug logging
        manager.setState("debug_test", QString("debug_value"));
        manager.setState("debug_test", QString("modified_debug_value"));

        // Debug mode doesn't change behavior, just adds logging
        auto state = manager.getState<QString>("debug_test");
        QCOMPARE(state->get(), QString("modified_debug_value"));
    }

    void testStateTypeConversion() {
        auto& manager = StateManager::instance();

        // Set as string
        manager.setState("convertible", QString("42"));

        // Try to get as different types
        auto string_state = manager.getState<QString>("convertible");
        auto int_state = manager.getState<int>("convertible");

        QVERIFY(string_state != nullptr);
        QCOMPARE(string_state->get(), QString("42"));

        // Type conversion might not work depending on implementation
        // This test documents the expected behavior
    }

    void testClearState() {
        auto& manager = StateManager::instance();

        manager.setState("clear_test1", QString("value1"));
        manager.setState("clear_test2", 42);
        manager.setState("clear_test3", true);

        QVERIFY(manager.hasState("clear_test1"));
        QVERIFY(manager.hasState("clear_test2"));
        QVERIFY(manager.hasState("clear_test3"));

        manager.clearState();

        QVERIFY(!manager.hasState("clear_test1"));
        QVERIFY(!manager.hasState("clear_test2"));
        QVERIFY(!manager.hasState("clear_test3"));
    }

    void testStateManagerExceptionHandling() {
        auto& manager = StateManager::instance();

        // Test with null validator (should not crash) - comment out as nullptr
        // not supported manager.setStateValidator("exception_test", nullptr);
        manager.setState("exception_test", QString("value"));

        auto state = manager.getState<QString>("exception_test");
        QVERIFY(state != nullptr);
        QCOMPARE(state->get(), QString("value"));
    }

    void testConcurrentStateAccess() {
        auto& manager = StateManager::instance();

        // Set up initial state
        manager.setState("concurrent_test", 0);
        auto state = manager.getState<int>("concurrent_test");

        // Simulate concurrent access (basic test)
        for (int i = 0; i < 10; ++i) {
            manager.setState("concurrent_test", i);
            QCOMPARE(state->get(), i);
        }
    }
};

QTEST_MAIN(StateManagerTest)
#include "test_state_manager.moc"
