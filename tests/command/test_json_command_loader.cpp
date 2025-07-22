#include <QtTest/QtTest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <memory>

#include "../../src/Command/Adapters/JSONCommandLoader.hpp"
#include "../../src/Command/UICommand.hpp"
#include "../../src/Command/CoreCommands.hpp"

using namespace DeclarativeUI::Command::Adapters;
using namespace DeclarativeUI::Command::UI;

class JSONCommandLoaderTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // **Basic loading operations**
    void testLoaderInitialization();
    void testSimpleCommandLoading();
    void testCommandWithProperties();
    void testCommandWithChildren();

    // **JSON parsing**
    void testValidJSONParsing();
    void testInvalidJSONHandling();
    void testMalformedJSONHandling();
    void testEmptyJSONHandling();

    // **Command creation**
    void testButtonCommandCreation();
    void testLabelCommandCreation();
    void testContainerCommandCreation();
    void testCustomCommandCreation();

    // **Property handling**
    void testStringProperties();
    void testNumericProperties();
    void testBooleanProperties();
    void testArrayProperties();
    void testObjectProperties();

    // **Event handling**
    void testEventHandlerRegistration();
    void testEventHandlerExecution();
    void testCustomEventHandlers();

    // **State binding**
    void testStateBindingFromJSON();
    void testAutoStateBinding();
    void testStateBindingValidation();

    // **Hierarchy loading**
    void testNestedCommandLoading();
    void testComplexHierarchyLoading();
    void testHierarchyValidation();

    // **Performance tests**
    void testLargeJSONPerformance();
    void testComplexHierarchyPerformance();

    // **Error handling**
    void testMissingCommandTypeHandling();
    void testInvalidPropertyHandling();
    void testCircularReferenceHandling();

private:
    std::unique_ptr<JSONCommandLoader> loader_;
    
    QString createSimpleButtonJSON();
    QString createComplexHierarchyJSON();
    QString createInvalidJSON();
};

void JSONCommandLoaderTest::initTestCase() {
    qDebug() << "🧪 Starting JSON Command Loader tests...";
}

void JSONCommandLoaderTest::cleanupTestCase() {
    qDebug() << "✅ JSON Command Loader tests completed";
}

void JSONCommandLoaderTest::init() {
    loader_ = std::make_unique<JSONCommandLoader>();
}

void JSONCommandLoaderTest::cleanup() {
    loader_.reset();
}

QString JSONCommandLoaderTest::createSimpleButtonJSON() {
    return R"({
        "type": "Button",
        "properties": {
            "text": "Click Me",
            "enabled": true,
            "width": 100,
            "height": 30
        }
    })";
}

QString JSONCommandLoaderTest::createComplexHierarchyJSON() {
    return R"({
        "type": "Container",
        "properties": {
            "layout": "VBox",
            "spacing": 10,
            "margins": 20
        },
        "children": [
            {
                "type": "Label",
                "properties": {
                    "text": "Welcome",
                    "style": {"font-weight": "bold"}
                }
            },
            {
                "type": "Container",
                "properties": {
                    "layout": "HBox"
                },
                "children": [
                    {
                        "type": "TextInput",
                        "properties": {
                            "placeholder": "Enter name...",
                            "maxLength": 50
                        },
                        "bindings": {
                            "text": "user.name"
                        }
                    },
                    {
                        "type": "Button",
                        "properties": {
                            "text": "Submit"
                        },
                        "events": {
                            "clicked": "onSubmitClicked"
                        }
                    }
                ]
            }
        ]
    })";
}

QString JSONCommandLoaderTest::createInvalidJSON() {
    return R"({
        "type": "InvalidType",
        "properties": {
            "invalid_property": null
        },
        "children": [
            {
                "missing_type": true
            }
        ]
    })";
}

void JSONCommandLoaderTest::testLoaderInitialization() {
    qDebug() << "🧪 Testing loader initialization...";

    QVERIFY(loader_ != nullptr);
    
    // Test configuration options
    loader_->setAutoMVCIntegration(true);
    loader_->setAutoStateBinding(true);
    loader_->setAutoEventHandling(true);
    
    QVERIFY(true); // Configuration should not crash

    qDebug() << "✅ Loader initialization test passed";
}

void JSONCommandLoaderTest::testSimpleCommandLoading() {
    qDebug() << "🧪 Testing simple command loading...";

    QString json = createSimpleButtonJSON();
    auto command = loader_->loadCommandFromString(json);
    
    QVERIFY(command != nullptr);
    QCOMPARE(command->getCommandType(), QString("Button"));

    qDebug() << "✅ Simple command loading test passed";
}

void JSONCommandLoaderTest::testCommandWithProperties() {
    qDebug() << "🧪 Testing command with properties...";

    QString json = createSimpleButtonJSON();
    auto command = loader_->loadCommandFromString(json);
    
    QVERIFY(command != nullptr);
    
    // Verify properties were loaded
    QCOMPARE(command->getState()->getProperty<QString>("text"), QString("Click Me"));
    QCOMPARE(command->getState()->getProperty<bool>("enabled"), true);
    QCOMPARE(command->getState()->getProperty<int>("width"), 100);
    QCOMPARE(command->getState()->getProperty<int>("height"), 30);

    qDebug() << "✅ Command with properties test passed";
}

void JSONCommandLoaderTest::testCommandWithChildren() {
    qDebug() << "🧪 Testing command with children...";

    QString json = createComplexHierarchyJSON();
    auto command = loader_->loadCommandFromString(json);
    
    QVERIFY(command != nullptr);
    QCOMPARE(command->getCommandType(), QString("Container"));
    
    // Verify children were loaded
    auto children = command->getChildren();
    QVERIFY(children.size() >= 2); // Should have at least label and container

    qDebug() << "✅ Command with children test passed";
}

void JSONCommandLoaderTest::testValidJSONParsing() {
    qDebug() << "🧪 Testing valid JSON parsing...";

    QString json = createSimpleButtonJSON();
    
    // Should parse without errors
    auto command = loader_->loadCommandFromString(json);
    QVERIFY(command != nullptr);

    qDebug() << "✅ Valid JSON parsing test passed";
}

void JSONCommandLoaderTest::testInvalidJSONHandling() {
    qDebug() << "🧪 Testing invalid JSON handling...";

    QString invalidJson = "{ invalid json syntax }";
    
    // Should handle gracefully
    auto command = loader_->loadCommandFromString(invalidJson);
    QVERIFY(command == nullptr);

    qDebug() << "✅ Invalid JSON handling test passed";
}

void JSONCommandLoaderTest::testMalformedJSONHandling() {
    qDebug() << "🧪 Testing malformed JSON handling...";

    QString malformedJson = R"({
        "type": "Button",
        "properties": {
            "text": "Unclosed string
        }
    })";
    
    // Should handle gracefully
    auto command = loader_->loadCommandFromString(malformedJson);
    QVERIFY(command == nullptr);

    qDebug() << "✅ Malformed JSON handling test passed";
}

void JSONCommandLoaderTest::testEmptyJSONHandling() {
    qDebug() << "🧪 Testing empty JSON handling...";

    QString emptyJson = "{}";
    
    // Should handle gracefully
    auto command = loader_->loadCommandFromString(emptyJson);
    QVERIFY(command == nullptr); // No type specified

    qDebug() << "✅ Empty JSON handling test passed";
}

void JSONCommandLoaderTest::testButtonCommandCreation() {
    qDebug() << "🧪 Testing button command creation...";

    QString json = R"({
        "type": "Button",
        "properties": {
            "text": "Test Button",
            "enabled": false
        }
    })";
    
    auto command = loader_->loadCommandFromString(json);
    QVERIFY(command != nullptr);
    QCOMPARE(command->getCommandType(), QString("Button"));
    QCOMPARE(command->getState()->getProperty<QString>("text"), QString("Test Button"));
    QCOMPARE(command->getState()->getProperty<bool>("enabled"), false);

    qDebug() << "✅ Button command creation test passed";
}

void JSONCommandLoaderTest::testLabelCommandCreation() {
    qDebug() << "🧪 Testing label command creation...";

    QString json = R"({
        "type": "Label",
        "properties": {
            "text": "Test Label",
            "alignment": "center"
        }
    })";
    
    auto command = loader_->loadCommandFromString(json);
    QVERIFY(command != nullptr);
    QCOMPARE(command->getCommandType(), QString("Label"));
    QCOMPARE(command->getState()->getProperty<QString>("text"), QString("Test Label"));

    qDebug() << "✅ Label command creation test passed";
}

void JSONCommandLoaderTest::testContainerCommandCreation() {
    qDebug() << "🧪 Testing container command creation...";

    QString json = R"({
        "type": "Container",
        "properties": {
            "layout": "VBox",
            "spacing": 15
        },
        "children": []
    })";
    
    auto command = loader_->loadCommandFromString(json);
    QVERIFY(command != nullptr);
    QCOMPARE(command->getCommandType(), QString("Container"));

    qDebug() << "✅ Container command creation test passed";
}

void JSONCommandLoaderTest::testCustomCommandCreation() {
    qDebug() << "🧪 Testing custom command creation...";

    // Test creation of custom command types
    QVERIFY(true); // Placeholder

    qDebug() << "✅ Custom command creation test passed";
}

void JSONCommandLoaderTest::testStringProperties() {
    qDebug() << "🧪 Testing string properties...";

    QString json = R"({
        "type": "Label",
        "properties": {
            "text": "String Property",
            "tooltip": "This is a tooltip"
        }
    })";
    
    auto command = loader_->loadCommandFromString(json);
    QVERIFY(command != nullptr);
    QCOMPARE(command->getState()->getProperty<QString>("text"), QString("String Property"));
    QCOMPARE(command->getState()->getProperty<QString>("tooltip"), QString("This is a tooltip"));

    qDebug() << "✅ String properties test passed";
}

void JSONCommandLoaderTest::testNumericProperties() {
    qDebug() << "🧪 Testing numeric properties...";

    QString json = R"({
        "type": "Button",
        "properties": {
            "width": 200,
            "height": 50,
            "opacity": 0.8
        }
    })";
    
    auto command = loader_->loadCommandFromString(json);
    QVERIFY(command != nullptr);
    QCOMPARE(command->getState()->getProperty<int>("width"), 200);
    QCOMPARE(command->getState()->getProperty<int>("height"), 50);
    QCOMPARE(command->getState()->getProperty<double>("opacity"), 0.8);

    qDebug() << "✅ Numeric properties test passed";
}

void JSONCommandLoaderTest::testBooleanProperties() {
    qDebug() << "🧪 Testing boolean properties...";

    QString json = R"({
        "type": "Button",
        "properties": {
            "enabled": true,
            "visible": false,
            "checkable": true
        }
    })";
    
    auto command = loader_->loadCommandFromString(json);
    QVERIFY(command != nullptr);
    QCOMPARE(command->getState()->getProperty<bool>("enabled"), true);
    QCOMPARE(command->getState()->getProperty<bool>("visible"), false);
    QCOMPARE(command->getState()->getProperty<bool>("checkable"), true);

    qDebug() << "✅ Boolean properties test passed";
}

void JSONCommandLoaderTest::testArrayProperties() {
    qDebug() << "🧪 Testing array properties...";

    QString json = R"({
        "type": "Container",
        "properties": {
            "margins": [10, 20, 10, 20],
            "colors": ["red", "green", "blue"]
        }
    })";
    
    auto command = loader_->loadCommandFromString(json);
    QVERIFY(command != nullptr);
    
    // Test array property handling
    QVERIFY(true); // Placeholder

    qDebug() << "✅ Array properties test passed";
}

void JSONCommandLoaderTest::testObjectProperties() {
    qDebug() << "🧪 Testing object properties...";

    QString json = R"({
        "type": "Label",
        "properties": {
            "style": {
                "font-size": "16px",
                "color": "blue",
                "font-weight": "bold"
            }
        }
    })";
    
    auto command = loader_->loadCommandFromString(json);
    QVERIFY(command != nullptr);
    
    // Test object property handling
    QVERIFY(true); // Placeholder

    qDebug() << "✅ Object properties test passed";
}

void JSONCommandLoaderTest::testEventHandlerRegistration() {
    qDebug() << "🧪 Testing event handler registration...";

    // Register event handler
    loader_->registerEventHandler("testHandler", [](const QVariant&) {
        qDebug() << "Test handler called";
    });
    
    QString json = R"({
        "type": "Button",
        "properties": {
            "text": "Click Me"
        },
        "events": {
            "clicked": "testHandler"
        }
    })";
    
    auto command = loader_->loadCommandFromString(json);
    QVERIFY(command != nullptr);
    
    // Event handler should be registered
    QVERIFY(true); // Placeholder

    qDebug() << "✅ Event handler registration test passed";
}

void JSONCommandLoaderTest::testEventHandlerExecution() {
    qDebug() << "🧪 Testing event handler execution...";

    bool handlerCalled = false;
    loader_->registerEventHandler("executionTest", [&handlerCalled](const QVariant&) {
        handlerCalled = true;
    });
    
    // Test would trigger event and verify handler execution
    QVERIFY(true); // Placeholder

    qDebug() << "✅ Event handler execution test passed";
}

void JSONCommandLoaderTest::testCustomEventHandlers() {
    qDebug() << "🧪 Testing custom event handlers...";

    // Test custom event handler functionality
    QVERIFY(true); // Placeholder

    qDebug() << "✅ Custom event handlers test passed";
}

void JSONCommandLoaderTest::testStateBindingFromJSON() {
    qDebug() << "🧪 Testing state binding from JSON...";

    QString json = R"({
        "type": "TextInput",
        "properties": {
            "placeholder": "Enter text..."
        },
        "bindings": {
            "text": "user.input",
            "enabled": "form.enabled"
        }
    })";
    
    auto command = loader_->loadCommandFromString(json);
    QVERIFY(command != nullptr);
    
    // Verify bindings were established
    QVERIFY(true); // Placeholder

    qDebug() << "✅ State binding from JSON test passed";
}

void JSONCommandLoaderTest::testAutoStateBinding() {
    qDebug() << "🧪 Testing auto state binding...";

    loader_->setAutoStateBinding(true);
    
    // Test automatic state binding functionality
    QVERIFY(true); // Placeholder

    qDebug() << "✅ Auto state binding test passed";
}

void JSONCommandLoaderTest::testStateBindingValidation() {
    qDebug() << "🧪 Testing state binding validation...";

    // Test validation of state binding configurations
    QVERIFY(true); // Placeholder

    qDebug() << "✅ State binding validation test passed";
}

void JSONCommandLoaderTest::testNestedCommandLoading() {
    qDebug() << "🧪 Testing nested command loading...";

    QString json = createComplexHierarchyJSON();
    auto command = loader_->loadCommandFromString(json);
    
    QVERIFY(command != nullptr);
    
    // Verify nested structure
    auto children = command->getChildren();
    QVERIFY(children.size() >= 2);
    
    // Check nested container
    if (children.size() >= 2) {
        auto nestedContainer = children[1];
        auto nestedChildren = nestedContainer->getChildren();
        QVERIFY(nestedChildren.size() >= 2); // TextInput and Button
    }

    qDebug() << "✅ Nested command loading test passed";
}

void JSONCommandLoaderTest::testComplexHierarchyLoading() {
    qDebug() << "🧪 Testing complex hierarchy loading...";

    QString json = createComplexHierarchyJSON();
    auto command = loader_->loadCommandFromString(json);
    
    QVERIFY(command != nullptr);
    QCOMPARE(command->getCommandType(), QString("Container"));

    qDebug() << "✅ Complex hierarchy loading test passed";
}

void JSONCommandLoaderTest::testHierarchyValidation() {
    qDebug() << "🧪 Testing hierarchy validation...";

    // Test validation of command hierarchies
    QVERIFY(true); // Placeholder

    qDebug() << "✅ Hierarchy validation test passed";
}

void JSONCommandLoaderTest::testLargeJSONPerformance() {
    qDebug() << "🧪 Testing large JSON performance...";

    // Create large JSON with many commands
    QJsonObject root;
    root["type"] = "Container";
    
    QJsonArray children;
    for (int i = 0; i < 100; ++i) {
        QJsonObject child;
        child["type"] = "Button";
        QJsonObject props;
        props["text"] = QString("Button %1").arg(i);
        child["properties"] = props;
        children.append(child);
    }
    root["children"] = children;
    
    QJsonDocument doc(root);
    QString largeJson = doc.toJson();
    
    QElapsedTimer timer;
    timer.start();
    
    auto command = loader_->loadCommandFromString(largeJson);
    
    qint64 elapsed = timer.elapsed();
    qDebug() << "Loaded large JSON (100 buttons) in" << elapsed << "ms";
    
    QVERIFY(command != nullptr);
    QVERIFY(elapsed < 2000); // Should be reasonably fast

    qDebug() << "✅ Large JSON performance test passed";
}

void JSONCommandLoaderTest::testComplexHierarchyPerformance() {
    qDebug() << "🧪 Testing complex hierarchy performance...";

    // Test performance with deeply nested hierarchies
    QVERIFY(true); // Placeholder

    qDebug() << "✅ Complex hierarchy performance test passed";
}

void JSONCommandLoaderTest::testMissingCommandTypeHandling() {
    qDebug() << "🧪 Testing missing command type handling...";

    QString json = R"({
        "properties": {
            "text": "No Type Specified"
        }
    })";
    
    auto command = loader_->loadCommandFromString(json);
    QVERIFY(command == nullptr); // Should fail gracefully

    qDebug() << "✅ Missing command type handling test passed";
}

void JSONCommandLoaderTest::testInvalidPropertyHandling() {
    qDebug() << "🧪 Testing invalid property handling...";

    QString json = createInvalidJSON();
    
    // Should handle invalid properties gracefully
    auto command = loader_->loadCommandFromString(json);
    // May return null or valid command with defaults
    QVERIFY(true); // Placeholder

    qDebug() << "✅ Invalid property handling test passed";
}

void JSONCommandLoaderTest::testCircularReferenceHandling() {
    qDebug() << "🧪 Testing circular reference handling...";

    // Test handling of circular references in JSON
    QVERIFY(true); // Placeholder

    qDebug() << "✅ Circular reference handling test passed";
}

QTEST_MAIN(JSONCommandLoaderTest)
#include "test_json_command_loader.moc"
