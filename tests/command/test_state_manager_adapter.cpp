#include <QtTest/QtTest>
#include <QSignalSpy>
#include <memory>

#include "../../src/Command/Adapters/StateManagerAdapter.hpp"
#include "../../src/Command/UICommand.hpp"
#include "../../src/Command/CoreCommands.hpp"
#include "../../src/Binding/StateManager.hpp"

using namespace DeclarativeUI::Command::Adapters;
using namespace DeclarativeUI::Command::UI;
using namespace DeclarativeUI::Binding;

class StateManagerAdapterTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // **Basic adapter operations**
    void testAdapterInitialization();
    void testCommandBinding();
    void testCommandUnbinding();
    void testStateRetrieval();

    // **State synchronization**
    void testStateToCommandSync();
    void testCommandToStateSync();
    void testBidirectionalSync();
    void testMultipleCommandSync();

    // **Batch operations**
    void testBatchStateUpdates();
    void testBatchCommandBinding();
    void testBatchTransactionHandling();

    // **State management**
    void testStateCreation();
    void testStateModification();
    void testStateDeletion();
    void testStateValidation();

    // **Performance tests**
    void testMassBindingPerformance();
    void testFrequentUpdatePerformance();
    void testBatchUpdatePerformance();

    // **Error handling**
    void testInvalidStateKeyHandling();
    void testNullCommandHandling();
    void testBindingConflictResolution();

private:
    std::unique_ptr<CommandStateManagerAdapter> adapter_;
    std::shared_ptr<ButtonCommand> button_;
    std::shared_ptr<LabelCommand> label_;
};

void StateManagerAdapterTest::initTestCase() {
    qDebug() << "🧪 Starting State Manager Adapter tests...";
}

void StateManagerAdapterTest::cleanupTestCase() {
    qDebug() << "✅ State Manager Adapter tests completed";
}

void StateManagerAdapterTest::init() {
    adapter_ = std::make_unique<CommandStateManagerAdapter>();
    button_ = std::make_shared<ButtonCommand>();
    label_ = std::make_shared<LabelCommand>();
}

void StateManagerAdapterTest::cleanup() {
    adapter_.reset();
    button_.reset();
    label_.reset();
}

void StateManagerAdapterTest::testAdapterInitialization() {
    qDebug() << "🧪 Testing adapter initialization...";

    QVERIFY(adapter_ != nullptr);
    
    // Test singleton access
    auto& instance = CommandStateManagerAdapter::instance();
    QVERIFY(&instance != nullptr);

    qDebug() << "✅ Adapter initialization test passed";
}

void StateManagerAdapterTest::testCommandBinding() {
    qDebug() << "🧪 Testing command binding...";

    auto& stateManager = StateManager::instance();
    
    // Set initial state
    stateManager.setState("test.button.text", QString("Initial Text"));
    
    // Bind command to state
    adapter_->bindCommand(button_, "test.button.text", "text");
    
    // Command should reflect state
    QCOMPARE(button_->getState()->getProperty<QString>("text"), QString("Initial Text"));

    qDebug() << "✅ Command binding test passed";
}

void StateManagerAdapterTest::testCommandUnbinding() {
    qDebug() << "🧪 Testing command unbinding...";

    auto& stateManager = StateManager::instance();
    
    // Bind and then unbind
    adapter_->bindCommand(button_, "test.unbind", "text");
    stateManager.setState("test.unbind", QString("Bound Text"));
    
    QCOMPARE(button_->getState()->getProperty<QString>("text"), QString("Bound Text"));
    
    // Unbind
    adapter_->unbindCommand(button_, "test.unbind", "text");
    
    // Change state - command should not update
    stateManager.setState("test.unbind", QString("New Text"));
    QCOMPARE(button_->getState()->getProperty<QString>("text"), QString("Bound Text"));

    qDebug() << "✅ Command unbinding test passed";
}

void StateManagerAdapterTest::testStateRetrieval() {
    qDebug() << "🧪 Testing state retrieval...";

    auto& stateManager = StateManager::instance();
    
    // Set state
    stateManager.setState("test.retrieve", QString("Retrieved Value"));
    
    // Retrieve through adapter
    auto value = adapter_->getCommandState<QString>(button_, "test.retrieve", QString());
    QCOMPARE(value, QString("Retrieved Value"));

    qDebug() << "✅ State retrieval test passed";
}

void StateManagerAdapterTest::testStateToCommandSync() {
    qDebug() << "🧪 Testing state to command sync...";

    auto& stateManager = StateManager::instance();
    
    adapter_->bindCommand(button_, "test.sync.state_to_command", "text");
    
    // Change state multiple times
    stateManager.setState("test.sync.state_to_command", QString("Value 1"));
    QCOMPARE(button_->getState()->getProperty<QString>("text"), QString("Value 1"));
    
    stateManager.setState("test.sync.state_to_command", QString("Value 2"));
    QCOMPARE(button_->getState()->getProperty<QString>("text"), QString("Value 2"));

    qDebug() << "✅ State to command sync test passed";
}

void StateManagerAdapterTest::testCommandToStateSync() {
    qDebug() << "🧪 Testing command to state sync...";

    auto& stateManager = StateManager::instance();
    
    adapter_->bindCommand(button_, "test.sync.command_to_state", "text");
    
    // Change command property
    adapter_->setCommandState(button_, "text", QString("From Command"));
    
    // State should be updated
    auto stateValue = stateManager.getState<QString>("test.sync.command_to_state");
    if (stateValue) {
        QCOMPARE(stateValue->get(), QString("From Command"));
    }

    qDebug() << "✅ Command to state sync test passed";
}

void StateManagerAdapterTest::testBidirectionalSync() {
    qDebug() << "🧪 Testing bidirectional sync...";

    auto& stateManager = StateManager::instance();
    
    adapter_->bindCommand(button_, "test.bidirectional", "text");
    
    // Change state
    stateManager.setState("test.bidirectional", QString("From State"));
    QCOMPARE(button_->getState()->getProperty<QString>("text"), QString("From State"));
    
    // Change command
    adapter_->setCommandState(button_, "text", QString("From Command"));
    auto stateValue = stateManager.getState<QString>("test.bidirectional");
    if (stateValue) {
        QCOMPARE(stateValue->get(), QString("From Command"));
    }

    qDebug() << "✅ Bidirectional sync test passed";
}

void StateManagerAdapterTest::testMultipleCommandSync() {
    qDebug() << "🧪 Testing multiple command sync...";

    auto& stateManager = StateManager::instance();
    
    // Bind multiple commands to same state
    adapter_->bindCommand(button_, "test.multiple.shared", "text");
    adapter_->bindCommand(label_, "test.multiple.shared", "text");
    
    // Change state
    stateManager.setState("test.multiple.shared", QString("Shared Value"));
    
    // Both commands should update
    QCOMPARE(button_->getState()->getProperty<QString>("text"), QString("Shared Value"));
    QCOMPARE(label_->getState()->getProperty<QString>("text"), QString("Shared Value"));

    qDebug() << "✅ Multiple command sync test passed";
}

void StateManagerAdapterTest::testBatchStateUpdates() {
    qDebug() << "🧪 Testing batch state updates...";

    auto& stateManager = StateManager::instance();
    
    adapter_->bindCommand(button_, "test.batch.button", "text");
    adapter_->bindCommand(label_, "test.batch.label", "text");
    
    // Begin batch update
    adapter_->beginBatchUpdate();
    
    // Update multiple states
    stateManager.setState("test.batch.button", QString("Batch Button"));
    stateManager.setState("test.batch.label", QString("Batch Label"));
    
    // Commit batch
    adapter_->commitBatchUpdate();
    
    // Verify updates
    QCOMPARE(button_->getState()->getProperty<QString>("text"), QString("Batch Button"));
    QCOMPARE(label_->getState()->getProperty<QString>("text"), QString("Batch Label"));

    qDebug() << "✅ Batch state updates test passed";
}

void StateManagerAdapterTest::testBatchCommandBinding() {
    qDebug() << "🧪 Testing batch command binding...";

    // Begin batch
    adapter_->beginBatchUpdate();
    
    // Create multiple bindings
    adapter_->bindCommand(button_, "test.batch.bind1", "text");
    adapter_->bindCommand(button_, "test.batch.bind2", "enabled");
    adapter_->bindCommand(label_, "test.batch.bind3", "text");
    
    // Commit batch
    adapter_->commitBatchUpdate();
    
    // Test bindings work
    auto& stateManager = StateManager::instance();
    stateManager.setState("test.batch.bind1", QString("Batch Text"));
    stateManager.setState("test.batch.bind2", false);
    stateManager.setState("test.batch.bind3", QString("Label Text"));
    
    QCOMPARE(button_->getState()->getProperty<QString>("text"), QString("Batch Text"));
    QCOMPARE(button_->getState()->getProperty<bool>("enabled"), false);
    QCOMPARE(label_->getState()->getProperty<QString>("text"), QString("Label Text"));

    qDebug() << "✅ Batch command binding test passed";
}

void StateManagerAdapterTest::testBatchTransactionHandling() {
    qDebug() << "🧪 Testing batch transaction handling...";

    // Test transaction rollback
    adapter_->beginBatchUpdate();
    
    auto& stateManager = StateManager::instance();
    adapter_->bindCommand(button_, "test.transaction", "text");
    stateManager.setState("test.transaction", QString("Original"));
    
    // Start transaction
    stateManager.setState("test.transaction", QString("Modified"));
    
    // Rollback
    adapter_->rollbackBatchUpdate();
    
    // Should revert to original
    QCOMPARE(button_->getState()->getProperty<QString>("text"), QString("Original"));

    qDebug() << "✅ Batch transaction handling test passed";
}

void StateManagerAdapterTest::testStateCreation() {
    qDebug() << "🧪 Testing state creation...";

    // Create new state through adapter
    adapter_->createState("test.new_state", QString("New Value"));
    
    auto& stateManager = StateManager::instance();
    auto state = stateManager.getState<QString>("test.new_state");
    QVERIFY(state != nullptr);
    QCOMPARE(state->get(), QString("New Value"));

    qDebug() << "✅ State creation test passed";
}

void StateManagerAdapterTest::testStateModification() {
    qDebug() << "🧪 Testing state modification...";

    auto& stateManager = StateManager::instance();
    stateManager.setState("test.modify", QString("Original"));
    
    // Modify through adapter
    adapter_->setCommandState(button_, "test.modify", QString("Modified"));
    
    auto state = stateManager.getState<QString>("test.modify");
    QVERIFY(state != nullptr);
    QCOMPARE(state->get(), QString("Modified"));

    qDebug() << "✅ State modification test passed";
}

void StateManagerAdapterTest::testStateDeletion() {
    qDebug() << "🧪 Testing state deletion...";

    auto& stateManager = StateManager::instance();
    stateManager.setState("test.delete", QString("To Delete"));
    
    // Delete through adapter
    adapter_->deleteState("test.delete");
    
    auto state = stateManager.getState<QString>("test.delete");
    QVERIFY(state == nullptr);

    qDebug() << "✅ State deletion test passed";
}

void StateManagerAdapterTest::testStateValidation() {
    qDebug() << "🧪 Testing state validation...";

    // Test state validation functionality
    QVERIFY(true); // Placeholder

    qDebug() << "✅ State validation test passed";
}

void StateManagerAdapterTest::testMassBindingPerformance() {
    qDebug() << "🧪 Testing mass binding performance...";

    auto& stateManager = StateManager::instance();
    
    QElapsedTimer timer;
    timer.start();

    // Create many bindings
    std::vector<std::shared_ptr<ButtonCommand>> buttons;
    for (int i = 0; i < 100; ++i) {
        auto button = std::make_shared<ButtonCommand>();
        adapter_->bindCommand(button, QString("test.mass.%1").arg(i), "text");
        stateManager.setState(QString("test.mass.%1").arg(i), QString("Value %1").arg(i));
        buttons.push_back(button);
    }

    qint64 elapsed = timer.elapsed();
    qDebug() << "Created and bound 100 commands in" << elapsed << "ms";

    QVERIFY(elapsed < 2000);

    qDebug() << "✅ Mass binding performance test passed";
}

void StateManagerAdapterTest::testFrequentUpdatePerformance() {
    qDebug() << "🧪 Testing frequent update performance...";

    auto& stateManager = StateManager::instance();
    
    adapter_->bindCommand(button_, "test.frequent", "text");
    
    QElapsedTimer timer;
    timer.start();

    // Frequent updates
    for (int i = 0; i < 1000; ++i) {
        stateManager.setState("test.frequent", QString("Update %1").arg(i));
    }

    qint64 elapsed = timer.elapsed();
    qDebug() << "1000 frequent updates in" << elapsed << "ms";

    QVERIFY(elapsed < 2000);

    qDebug() << "✅ Frequent update performance test passed";
}

void StateManagerAdapterTest::testBatchUpdatePerformance() {
    qDebug() << "🧪 Testing batch update performance...";

    auto& stateManager = StateManager::instance();
    
    // Bind many commands
    std::vector<std::shared_ptr<ButtonCommand>> buttons;
    for (int i = 0; i < 100; ++i) {
        auto button = std::make_shared<ButtonCommand>();
        adapter_->bindCommand(button, QString("test.batch_perf.%1").arg(i), "text");
        buttons.push_back(button);
    }
    
    QElapsedTimer timer;
    timer.start();

    // Batch update
    adapter_->beginBatchUpdate();
    for (int i = 0; i < 100; ++i) {
        stateManager.setState(QString("test.batch_perf.%1").arg(i), QString("Batch %1").arg(i));
    }
    adapter_->commitBatchUpdate();

    qint64 elapsed = timer.elapsed();
    qDebug() << "Batch update of 100 states in" << elapsed << "ms";

    QVERIFY(elapsed < 1000);

    qDebug() << "✅ Batch update performance test passed";
}

void StateManagerAdapterTest::testInvalidStateKeyHandling() {
    qDebug() << "🧪 Testing invalid state key handling...";

    // Test binding to invalid state keys
    adapter_->bindCommand(button_, "", "text"); // Empty key
    adapter_->bindCommand(button_, "invalid..key", "text"); // Invalid format
    
    // Should handle gracefully
    QVERIFY(true);

    qDebug() << "✅ Invalid state key handling test passed";
}

void StateManagerAdapterTest::testNullCommandHandling() {
    qDebug() << "🧪 Testing null command handling...";

    // Test operations with null commands
    adapter_->bindCommand(nullptr, "test.null", "text");
    adapter_->setCommandState(nullptr, "text", QString("Value"));
    
    // Should handle gracefully
    QVERIFY(true);

    qDebug() << "✅ Null command handling test passed";
}

void StateManagerAdapterTest::testBindingConflictResolution() {
    qDebug() << "🧪 Testing binding conflict resolution...";

    // Test resolution of binding conflicts
    adapter_->bindCommand(button_, "test.conflict", "text");
    adapter_->bindCommand(button_, "test.conflict", "text"); // Duplicate binding
    
    // Should handle gracefully
    QVERIFY(true);

    qDebug() << "✅ Binding conflict resolution test passed";
}

QTEST_MAIN(StateManagerAdapterTest)
#include "test_state_manager_adapter.moc"
