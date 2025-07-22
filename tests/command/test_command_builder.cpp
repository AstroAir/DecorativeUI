#include <QtTest/QtTest>
#include <QObject>
#include <QDebug>

#ifdef DECLARATIVE_UI_COMMAND_SYSTEM_ENABLED

#include "Command/CommandBuilder.hpp"
#include "../../Command/UICommand.hpp"

using namespace DeclarativeUI::Command;

class CommandBuilderTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        qDebug() << "🧪 Starting Command Builder Tests";
    }
    
    void cleanupTestCase() {
        qDebug() << "✅ Command Builder Tests completed";
    }
    
    void testBasicCommandBuilder() {
        try {
            auto command = UI::CommandBuilder("Button")
                .text("Test Button")
                .enabled(true)
                .build();
            
            QVERIFY(command != nullptr);
            QCOMPARE(command->getCommandType(), QString("Button"));
            
            // Test properties were set
            auto state = command->getState();
            QCOMPARE(state->getProperty<QString>("text"), QString("Test Button"));
            QCOMPARE(state->getProperty<bool>("enabled"), true);
            
        } catch (const std::exception& e) {
            QFAIL(QString("CommandBuilder test failed: %1").arg(e.what()).toUtf8().constData());
        }
    }
    
    void testCommandBuilderChaining() {
        try {
            auto command = UI::CommandBuilder("TextInput")
                .placeholder("Enter text...")
                .maxLength(100)
                .enabled(true)
                .visible(true)
                .build();
            
            QVERIFY(command != nullptr);
            
            auto state = command->getState();
            QCOMPARE(state->getProperty<QString>("placeholder"), QString("Enter text..."));
            QCOMPARE(state->getProperty<int>("maxLength"), 100);
            QCOMPARE(state->getProperty<bool>("enabled"), true);
            QCOMPARE(state->getProperty<bool>("visible"), true);
            
        } catch (const std::exception& e) {
            QFAIL(QString("CommandBuilder chaining test failed: %1").arg(e.what()).toUtf8().constData());
        }
    }
    
    void testCommandBuilderWithCustomProperties() {
        try {
            auto command = UI::CommandBuilder("CustomComponent")
                .property("customProp1", "value1")
                .property("customProp2", 42)
                .property("customProp3", true)
                .build();
            
            QVERIFY(command != nullptr);
            
            auto state = command->getState();
            QCOMPARE(state->getProperty<QString>("customProp1"), QString("value1"));
            QCOMPARE(state->getProperty<int>("customProp2"), 42);
            QCOMPARE(state->getProperty<bool>("customProp3"), true);
            
        } catch (const std::exception& e) {
            QFAIL(QString("CommandBuilder custom properties test failed: %1").arg(e.what()).toUtf8().constData());
        }
    }
    
    void testCommandBuilderEventHandlers() {
        try {
            bool eventHandled = false;
            
            auto command = UI::CommandBuilder("Button")
                .text("Event Test")
                .onClick([&eventHandled]() {
                    eventHandled = true;
                })
                .build();
            
            QVERIFY(command != nullptr);
            
            // Simulate click event (this would depend on the actual event system implementation)
            // For now, just verify the command was created successfully
            QCOMPARE(command->getState()->getProperty<QString>("text"), QString("Event Test"));
            
        } catch (const std::exception& e) {
            QFAIL(QString("CommandBuilder event handlers test failed: %1").arg(e.what()).toUtf8().constData());
        }
    }
    
    void testCommandHierarchyBuilder() {
        try {
            auto container = UI::CommandHierarchyBuilder("Container")
                .layout("VBox")
                .spacing(10)
                .addChild("Label", [](UI::CommandBuilder& label) {
                    label.text("Child Label");
                })
                .addChild("Button", [](UI::CommandBuilder& button) {
                    button.text("Child Button")
                          .enabled(true);
                })
                .build();
            
            QVERIFY(container != nullptr);
            QCOMPARE(container->getCommandType(), QString("Container"));
            
            auto state = container->getState();
            QCOMPARE(state->getProperty<QString>("layout"), QString("VBox"));
            QCOMPARE(state->getProperty<int>("spacing"), 10);
            
            // Test children
            auto children = container->getChildren();
            QCOMPARE(children.size(), 2);
            
            if (children.size() >= 2) {
                QCOMPARE(children[0]->getCommandType(), QString("Label"));
                QCOMPARE(children[0]->getState()->getProperty<QString>("text"), QString("Child Label"));
                
                QCOMPARE(children[1]->getCommandType(), QString("Button"));
                QCOMPARE(children[1]->getState()->getProperty<QString>("text"), QString("Child Button"));
                QCOMPARE(children[1]->getState()->getProperty<bool>("enabled"), true);
            }
            
        } catch (const std::exception& e) {
            QFAIL(QString("CommandHierarchyBuilder test failed: %1").arg(e.what()).toUtf8().constData());
        }
    }
    
    void testCommandBuilderStateBinding() {
        try {
            auto command = UI::CommandBuilder("TextInput")
                .placeholder("Bound Input")
                .bindToState("test.input.value")
                .build();
            
            QVERIFY(command != nullptr);
            QCOMPARE(command->getState()->getProperty<QString>("placeholder"), QString("Bound Input"));
            
            // Note: Actual state binding testing would require the state management system
            // For now, just verify the command was created successfully
            
        } catch (const std::exception& e) {
            QFAIL(QString("CommandBuilder state binding test failed: %1").arg(e.what()).toUtf8().constData());
        }
    }
    
    void testCommandBuilderValidation() {
        try {
            auto command = UI::CommandBuilder("TextInput")
                .placeholder("Validated Input")
                .validator([](const QVariant& value) {
                    return !value.toString().isEmpty();
                })
                .build();
            
            QVERIFY(command != nullptr);
            
            // Note: Actual validation testing would require the validation system
            // For now, just verify the command was created successfully
            
        } catch (const std::exception& e) {
            QFAIL(QString("CommandBuilder validation test failed: %1").arg(e.what()).toUtf8().constData());
        }
    }
    
    void testCommandBuilderStyling() {
        try {
            auto command = UI::CommandBuilder("Label")
                .text("Styled Label")
                .style("color: red; font-weight: bold;")
                .build();
            
            QVERIFY(command != nullptr);
            
            auto state = command->getState();
            QCOMPARE(state->getProperty<QString>("text"), QString("Styled Label"));
            QCOMPARE(state->getProperty<QString>("style"), QString("color: red; font-weight: bold;"));
            
        } catch (const std::exception& e) {
            QFAIL(QString("CommandBuilder styling test failed: %1").arg(e.what()).toUtf8().constData());
        }
    }
    
    void testNestedCommandHierarchy() {
        try {
            auto mainContainer = UI::CommandHierarchyBuilder("Container")
                .layout("VBox")
                .addChild("Label", [](UI::CommandBuilder& label) {
                    label.text("Main Title");
                })
                .addChild("Container", [](UI::CommandBuilder& subContainer) {
                    // For nested containers with children, we need to use CommandHierarchyBuilder
                    // or add children using the child() method on CommandBuilder
                    subContainer.layout("HBox")
                               .spacing(5)
                               .child(UI::CommandBuilder("Button").text("Button 1"))
                               .child(UI::CommandBuilder("Button").text("Button 2"));
                })
                .build();
            
            QVERIFY(mainContainer != nullptr);
            
            auto children = mainContainer->getChildren();
            QCOMPARE(children.size(), 2);
            
            if (children.size() >= 2) {
                // Check first child (Label)
                QCOMPARE(children[0]->getCommandType(), QString("Label"));
                
                // Check second child (Container)
                QCOMPARE(children[1]->getCommandType(), QString("Container"));
                
                auto subChildren = children[1]->getChildren();
                QCOMPARE(subChildren.size(), 2);
                
                if (subChildren.size() >= 2) {
                    QCOMPARE(subChildren[0]->getCommandType(), QString("Button"));
                    QCOMPARE(subChildren[0]->getState()->getProperty<QString>("text"), QString("Button 1"));
                    
                    QCOMPARE(subChildren[1]->getCommandType(), QString("Button"));
                    QCOMPARE(subChildren[1]->getState()->getProperty<QString>("text"), QString("Button 2"));
                }
            }
            
        } catch (const std::exception& e) {
            QFAIL(QString("Nested CommandHierarchy test failed: %1").arg(e.what()).toUtf8().constData());
        }
    }
};

QTEST_MAIN(CommandBuilderTest)

#include "test_command_builder.moc"

#else

// Dummy test when Command system is not enabled
class DummyCommandBuilderTest : public QObject {
    Q_OBJECT

private slots:
    void testCommandSystemDisabled() {
        QSKIP("Command system not enabled");
    }
};

QTEST_MAIN(DummyCommandBuilderTest)

#include "test_command_builder.moc"

#endif // DECLARATIVE_UI_COMMAND_SYSTEM_ENABLED
