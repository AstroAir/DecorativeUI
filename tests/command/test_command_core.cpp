#include <QtTest/QtTest>
#include <QObject>
#include <QDebug>

#ifdef DECLARATIVE_UI_COMMAND_SYSTEM_ENABLED

#include "../../Command/UICommand.hpp"
#include "../../Command/UICommandFactory.hpp"

using namespace DeclarativeUI::Command;

class CommandCoreTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        qDebug() << "🧪 Starting Command Core Tests";
    }
    
    void cleanupTestCase() {
        qDebug() << "✅ Command Core Tests completed";
    }
    
    void testUICommandStateCreation() {
        UI::UICommandState state;
        
        // Test initial state
        QVERIFY(state.getPropertyNames().isEmpty());
        QVERIFY(!state.hasProperty("test"));
    }
    
    void testUICommandStateProperties() {
        UI::UICommandState state;
        
        // Test setting and getting properties
        state.setProperty("text", "Hello World");
        state.setProperty("enabled", true);
        state.setProperty("count", 42);
        
        QCOMPARE(state.getProperty<QString>("text"), QString("Hello World"));
        QCOMPARE(state.getProperty<bool>("enabled"), true);
        QCOMPARE(state.getProperty<int>("count"), 42);
        
        // Test property existence
        QVERIFY(state.hasProperty("text"));
        QVERIFY(state.hasProperty("enabled"));
        QVERIFY(state.hasProperty("count"));
        QVERIFY(!state.hasProperty("nonexistent"));
        
        // Test property names
        QStringList propertyNames = state.getPropertyNames();
        QCOMPARE(propertyNames.size(), 3);
        QVERIFY(propertyNames.contains("text"));
        QVERIFY(propertyNames.contains("enabled"));
        QVERIFY(propertyNames.contains("count"));
    }
    
    void testUICommandStateValidation() {
        UI::UICommandState state;
        
        // Set up validator
        state.setValidator("count", [](const QVariant& value) {
            return value.toInt() >= 0;
        });
        
        // Test valid value
        state.setProperty("count", 10);
        QVERIFY(state.validateProperty("count", 10));
        QCOMPARE(state.getProperty<int>("count"), 10);
        
        // Test invalid value
        QVERIFY(!state.validateProperty("count", -5));
        
        // Test validation of all properties
        state.setProperty("text", "valid");
        QVERIFY(state.validateAllProperties());
    }
    
    void testUICommandStateSignals() {
        UI::UICommandState state;
        
        QSignalSpy propertyChangedSpy(&state, &UI::UICommandState::propertyChanged);
        QSignalSpy stateChangedSpy(&state, &UI::UICommandState::stateChanged);
        
        // Test property change signals
        state.setProperty("test", "value1");
        QCOMPARE(propertyChangedSpy.count(), 1);
        QCOMPARE(stateChangedSpy.count(), 1);
        
        // Test that setting the same value doesn't emit signals
        state.setProperty("test", "value1");
        QCOMPARE(propertyChangedSpy.count(), 1);
        QCOMPARE(stateChangedSpy.count(), 1);
        
        // Test changing to different value
        state.setProperty("test", "value2");
        QCOMPARE(propertyChangedSpy.count(), 2);
        QCOMPARE(stateChangedSpy.count(), 2);
    }
    
    void testUICommandStateJSON() {
        UI::UICommandState state;
        
        // Set up test data
        state.setProperty("text", "Hello");
        state.setProperty("enabled", true);
        state.setProperty("count", 42);
        
        // Test JSON export
        QJsonObject json = state.toJson();
        QCOMPARE(json["text"].toString(), QString("Hello"));
        QCOMPARE(json["enabled"].toBool(), true);
        QCOMPARE(json["count"].toInt(), 42);
        
        // Test JSON import
        UI::UICommandState newState;
        newState.fromJson(json);
        
        QCOMPARE(newState.getProperty<QString>("text"), QString("Hello"));
        QCOMPARE(newState.getProperty<bool>("enabled"), true);
        QCOMPARE(newState.getProperty<int>("count"), 42);
    }
    
    void testUICommandFactory() {
        auto& factory = UI::UICommandFactory::instance();
        
        // Test factory singleton
        auto& factory2 = UI::UICommandFactory::instance();
        QCOMPARE(&factory, &factory2);
        
        // Test command creation (basic test)
        // Note: This would require actual command types to be registered
        // For now, just test that the factory exists
        QVERIFY(&factory != nullptr);
    }
    
    void testUICommandStateRemoval() {
        UI::UICommandState state;
        
        // Add properties
        state.setProperty("prop1", "value1");
        state.setProperty("prop2", "value2");
        QCOMPARE(state.getPropertyNames().size(), 2);
        
        // Remove property
        QSignalSpy removedSpy(&state, &UI::UICommandState::propertyRemoved);
        state.removeProperty("prop1");
        
        QCOMPARE(state.getPropertyNames().size(), 1);
        QVERIFY(!state.hasProperty("prop1"));
        QVERIFY(state.hasProperty("prop2"));
        QCOMPARE(removedSpy.count(), 1);
        
        // Clear all properties
        state.clearProperties();
        QCOMPARE(state.getPropertyNames().size(), 0);
    }
    
    void testUICommandStateDefaultValues() {
        UI::UICommandState state;
        
        // Test default values for non-existent properties
        QCOMPARE(state.getProperty<QString>("nonexistent", "default"), QString("default"));
        QCOMPARE(state.getProperty<int>("nonexistent", 99), 99);
        QCOMPARE(state.getProperty<bool>("nonexistent", true), true);
        
        // Test that default values don't affect actual properties
        state.setProperty("test", "actual");
        QCOMPARE(state.getProperty<QString>("test", "default"), QString("actual"));
    }
};

QTEST_MAIN(CommandCoreTest)

#include "test_command_core.moc"

#else

// Dummy test when Command system is not enabled
class DummyCommandCoreTest : public QObject {
    Q_OBJECT

private slots:
    void testCommandSystemDisabled() {
        QSKIP("Command system not enabled");
    }
};

QTEST_MAIN(DummyCommandCoreTest)

#include "test_command_core.moc"

#endif // DECLARATIVE_UI_COMMAND_SYSTEM_ENABLED
