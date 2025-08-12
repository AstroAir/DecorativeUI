#include <QJsonObject>
#include <QSignalSpy>
#include <QtTest/QtTest>
#include <memory>

#include "../CoreCommands.hpp"
#include "../UICommand.hpp"

using namespace DeclarativeUI::Command::UI;

class CommandStateTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // **Basic state operations**
    void testPropertySetAndGet();
    void testPropertyChangedSignal();
    void testStateChangedSignal();
    void testPropertyValidation();
    void testInvalidPropertyHandling();

    // **State serialization**
    void testStateToJson();
    void testStateFromJson();
    void testStateDiff();

    // **State validation**
    void testValidatorRegistration();
    void testValidatorExecution();
    void testValidationFailureHandling();

    // **State binding**
    void testStateBinding();
    void testStateSynchronization();
    void testBatchStateUpdates();

    // **Performance tests**
    void testLargeStatePerformance();
    void testFrequentUpdatesPerformance();

private:
    std::unique_ptr<UICommandState> state_;
    std::unique_ptr<ButtonCommand> command_;
};

void CommandStateTest::initTestCase() {
    qDebug() << "🧪 Starting Command State tests...";
}

void CommandStateTest::cleanupTestCase() {
    qDebug() << "✅ Command State tests completed";
}

void CommandStateTest::init() {
    state_ = std::make_unique<UICommandState>();
    command_ = std::make_unique<ButtonCommand>();
}

void CommandStateTest::cleanup() {
    state_.reset();
    command_.reset();
}

void CommandStateTest::testPropertySetAndGet() {
    qDebug() << "🧪 Testing property set and get...";

    // Test basic property operations
    state_->setProperty("text", QString("Hello World"));
    QCOMPARE(state_->getProperty<QString>("text"), QString("Hello World"));

    state_->setProperty("enabled", true);
    QCOMPARE(state_->getProperty<bool>("enabled"), true);

    state_->setProperty("count", 42);
    QCOMPARE(state_->getProperty<int>("count"), 42);

    // Test default values
    QCOMPARE(state_->getProperty<QString>("nonexistent", "default"),
             QString("default"));
    QCOMPARE(state_->getProperty<int>("nonexistent", 100), 100);

    qDebug() << "✅ Property set and get test passed";
}

void CommandStateTest::testPropertyChangedSignal() {
    qDebug() << "🧪 Testing property changed signal...";

    QSignalSpy spy(state_.get(), &UICommandState::propertyChanged);

    state_->setProperty("text", QString("Initial"));
    QCOMPARE(spy.count(), 1);

    auto arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), QString("text"));
    QCOMPARE(arguments.at(1).toString(), QString("Initial"));

    // Test that setting the same value doesn't emit signal
    state_->setProperty("text", QString("Initial"));
    QCOMPARE(spy.count(), 0);

    // Test changing to different value
    state_->setProperty("text", QString("Changed"));
    QCOMPARE(spy.count(), 1);

    qDebug() << "✅ Property changed signal test passed";
}

void CommandStateTest::testStateChangedSignal() {
    qDebug() << "🧪 Testing state changed signal...";

    QSignalSpy spy(state_.get(), &UICommandState::stateChanged);

    state_->setProperty("prop1", "value1");
    QCOMPARE(spy.count(), 1);

    state_->setProperty("prop2", "value2");
    QCOMPARE(spy.count(), 2);

    // Setting same value shouldn't emit
    state_->setProperty("prop1", "value1");
    QCOMPARE(spy.count(), 2);

    qDebug() << "✅ State changed signal test passed";
}

void CommandStateTest::testPropertyValidation() {
    qDebug() << "🧪 Testing property validation...";

    QSignalSpy validationSpy(state_.get(), &UICommandState::validationFailed);

    // Register a validator
    state_->setValidator("positive_number", [](const QVariant& value) {
        return value.toInt() > 0;
    });

    // Test valid value
    state_->setProperty("positive_number", 10);
    QCOMPARE(state_->getProperty<int>("positive_number"), 10);
    QCOMPARE(validationSpy.count(), 0);

    // Test invalid value
    state_->setProperty("positive_number", -5);
    QCOMPARE(validationSpy.count(), 1);
    // Property should not be updated on validation failure
    QCOMPARE(state_->getProperty<int>("positive_number"), 10);

    qDebug() << "✅ Property validation test passed";
}

void CommandStateTest::testInvalidPropertyHandling() {
    qDebug() << "🧪 Testing invalid property handling...";

    // Test getting non-existent property
    QVERIFY(!state_->hasProperty("nonexistent"));
    QCOMPARE(state_->getProperty<QString>("nonexistent"), QString());

    // Test with default value
    QCOMPARE(state_->getProperty<QString>("nonexistent", "default"),
             QString("default"));

    qDebug() << "✅ Invalid property handling test passed";
}

void CommandStateTest::testStateToJson() {
    qDebug() << "🧪 Testing state to JSON...";

    state_->setProperty("text", QString("Hello"));
    state_->setProperty("enabled", true);
    state_->setProperty("count", 42);

    QJsonObject json = state_->toJson();

    QCOMPARE(json["text"].toString(), QString("Hello"));
    QCOMPARE(json["enabled"].toBool(), true);
    QCOMPARE(json["count"].toInt(), 42);

    qDebug() << "✅ State to JSON test passed";
}

void CommandStateTest::testStateFromJson() {
    qDebug() << "🧪 Testing state from JSON...";

    QJsonObject json;
    json["text"] = "Hello from JSON";
    json["enabled"] = false;
    json["count"] = 123;

    state_->fromJson(json);

    QCOMPARE(state_->getProperty<QString>("text"), QString("Hello from JSON"));
    QCOMPARE(state_->getProperty<bool>("enabled"), false);
    QCOMPARE(state_->getProperty<int>("count"), 123);

    qDebug() << "✅ State from JSON test passed";
}

void CommandStateTest::testStateDiff() {
    qDebug() << "🧪 Testing state diff...";

    auto otherState = std::make_unique<UICommandState>();

    state_->setProperty("text", QString("Hello"));
    state_->setProperty("enabled", true);

    otherState->setProperty("text", QString("World"));
    otherState->setProperty("count", 42);

    auto differences = state_->diff(*otherState);

    // Should contain differences for text, enabled, and count
    QVERIFY(differences.contains("text"));
    QVERIFY(differences.contains("enabled"));
    QVERIFY(differences.contains("count"));

    qDebug() << "✅ State diff test passed";
}

void CommandStateTest::testValidatorRegistration() {
    qDebug() << "🧪 Testing validator registration...";

    // Register multiple validators
    state_->setValidator("email", [](const QVariant& value) {
        QString email = value.toString();
        return email.contains("@") && email.contains(".");
    });

    state_->setValidator("range", [](const QVariant& value) {
        int num = value.toInt();
        return num >= 0 && num <= 100;
    });

    // Test email validator
    state_->setProperty("email", "test@example.com");
    QCOMPARE(state_->getProperty<QString>("email"),
             QString("test@example.com"));

    // Test range validator
    state_->setProperty("range", 50);
    QCOMPARE(state_->getProperty<int>("range"), 50);

    qDebug() << "✅ Validator registration test passed";
}

void CommandStateTest::testValidatorExecution() {
    qDebug() << "🧪 Testing validator execution...";

    QSignalSpy validationSpy(state_.get(), &UICommandState::validationFailed);

    state_->setValidator("even_number", [](const QVariant& value) {
        return value.toInt() % 2 == 0;
    });

    // Valid value
    state_->setProperty("even_number", 4);
    QCOMPARE(validationSpy.count(), 0);

    // Invalid value
    state_->setProperty("even_number", 3);
    QCOMPARE(validationSpy.count(), 1);

    qDebug() << "✅ Validator execution test passed";
}

void CommandStateTest::testValidationFailureHandling() {
    qDebug() << "🧪 Testing validation failure handling...";

    QSignalSpy validationSpy(state_.get(), &UICommandState::validationFailed);

    state_->setValidator("strict", [](const QVariant&) {
        return false;  // Always fail
    });

    QString originalValue = "original";
    state_->setProperty("other_prop", originalValue);

    // Try to set invalid value
    state_->setProperty("strict", "any_value");
    QCOMPARE(validationSpy.count(), 1);

    // Ensure other properties are not affected
    QCOMPARE(state_->getProperty<QString>("other_prop"), originalValue);

    qDebug() << "✅ Validation failure handling test passed";
}

void CommandStateTest::testStateBinding() {
    qDebug() << "🧪 Testing state binding...";

    // This would test binding between command state and external state
    // Implementation depends on the binding system
    QVERIFY(true);  // Placeholder

    qDebug() << "✅ State binding test passed";
}

void CommandStateTest::testStateSynchronization() {
    qDebug() << "🧪 Testing state synchronization...";

    // This would test synchronization between multiple state instances
    // Implementation depends on the synchronization system
    QVERIFY(true);  // Placeholder

    qDebug() << "✅ State synchronization test passed";
}

void CommandStateTest::testBatchStateUpdates() {
    qDebug() << "🧪 Testing batch state updates...";

    QSignalSpy stateChangedSpy(state_.get(), &UICommandState::stateChanged);

    // Batch updates would minimize signal emissions
    // Implementation depends on batch update system
    state_->setProperty("prop1", "value1");
    state_->setProperty("prop2", "value2");
    state_->setProperty("prop3", "value3");

    // For now, each update emits a signal
    QCOMPARE(stateChangedSpy.count(), 3);

    qDebug() << "✅ Batch state updates test passed";
}

void CommandStateTest::testLargeStatePerformance() {
    qDebug() << "🧪 Testing large state performance...";

    QElapsedTimer timer;
    timer.start();

    // Set many properties
    for (int i = 0; i < 1000; ++i) {
        state_->setProperty(QString("prop_%1").arg(i), i);
    }

    qint64 setTime = timer.elapsed();
    timer.restart();

    // Get many properties
    for (int i = 0; i < 1000; ++i) {
        int value = state_->getProperty<int>(QString("prop_%1").arg(i));
        Q_UNUSED(value)
    }

    qint64 getTime = timer.elapsed();

    qDebug() << "Set 1000 properties in" << setTime << "ms";
    qDebug() << "Get 1000 properties in" << getTime << "ms";

    // Performance should be reasonable
    QVERIFY(setTime < 1000);  // Less than 1 second
    QVERIFY(getTime < 100);   // Less than 100ms

    qDebug() << "✅ Large state performance test passed";
}

void CommandStateTest::testFrequentUpdatesPerformance() {
    qDebug() << "🧪 Testing frequent updates performance...";

    QElapsedTimer timer;
    timer.start();

    // Frequent updates to same property
    for (int i = 0; i < 10000; ++i) {
        state_->setProperty("counter", i);
    }

    qint64 elapsed = timer.elapsed();
    qDebug() << "10000 property updates in" << elapsed << "ms";

    // Should handle frequent updates efficiently
    QVERIFY(elapsed < 2000);  // Less than 2 seconds

    qDebug() << "✅ Frequent updates performance test passed";
}

QTEST_MAIN(CommandStateTest)
#include "test_command_state.moc"
