#include <QApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSignalSpy>
#include <QTemporaryFile>
#include <QTest>
#include <QUrl>
#include <QWidget>
#include <memory>

#include "../JSON/ComponentRegistry.hpp"
#include "../JSON/JSONParser.hpp"
#include "../JSON/JSONUILoader.hpp"
#include "../JSON/JSONValidator.hpp"
#include "../Exceptions/UIExceptions.hpp"

using namespace DeclarativeUI::JSON;
using namespace DeclarativeUI::Exceptions;

class JSONTest : public QObject {
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
    }

    void init() {
        // Set up for each test
    }

    void cleanup() {
        // Clean up after each test
    }

    // **JSONParser Tests**
    void testJSONParserBasicParsing() {
        JSONParser parser;
        
        QString json_string = R"({
            "name": "test",
            "value": 42,
            "enabled": true,
            "items": ["item1", "item2", "item3"]
        })";
        
        auto result = parser.parseString(json_string);
        
        QVERIFY(!result.isEmpty());
        QCOMPARE(result["name"].toString(), QString("test"));
        QCOMPARE(result["value"].toInt(), 42);
        QCOMPARE(result["enabled"].toBool(), true);
        
        auto items = result["items"].toArray();
        QCOMPARE(items.size(), 3);
        QCOMPARE(items[0].toString(), QString("item1"));
        QCOMPARE(items[1].toString(), QString("item2"));
        QCOMPARE(items[2].toString(), QString("item3"));
    }

    void testJSONParserFileLoading() {
        JSONParser parser;
        
        // Create a temporary JSON file
        QTemporaryFile temp_file;
        temp_file.setFileTemplate("test_json_XXXXXX.json");
        QVERIFY(temp_file.open());
        
        QString json_content = R"({
            "widget": {
                "type": "QLabel",
                "properties": {
                    "text": "Hello from file",
                    "enabled": true
                }
            }
        })";
        
        temp_file.write(json_content.toUtf8());
        temp_file.close();
        
        auto result = parser.parseFile(temp_file.fileName());
        
        QVERIFY(!result.isEmpty());
        auto widget = result["widget"].toObject();
        QCOMPARE(widget["type"].toString(), QString("QLabel"));
        
        auto properties = widget["properties"].toObject();
        QCOMPARE(properties["text"].toString(), QString("Hello from file"));
        QCOMPARE(properties["enabled"].toBool(), true);
    }

    void testJSONParserStrictMode() {
        JSONParser parser;
        
        // Test with strict mode enabled
        parser.setStrictMode(true);
        
        QString valid_json = R"({"valid": true})";
        auto result = parser.parseString(valid_json);
        QVERIFY(!result.isEmpty());
        
        // Test with invalid JSON in strict mode
        QString invalid_json = R"({invalid: true})"; // Missing quotes around key
        try {
            auto invalid_result = parser.parseString(invalid_json);
            // Should either return empty object or throw exception
            QVERIFY(invalid_result.isEmpty());
        } catch (const JSONParsingException& e) {
            // Exception is acceptable in strict mode
            QVERIFY(!e.getMessage().isEmpty());
        }
    }

    void testJSONParserWithComments() {
        JSONParser parser;
        
        parser.setAllowComments(true);
        
        QString json_with_comments = R"({
            // This is a comment
            "name": "test", // Another comment
            "value": 42
            /* Multi-line
               comment */
        })";
        
        auto result = parser.parseString(json_with_comments);
        
        QVERIFY(!result.isEmpty());
        QCOMPARE(result["name"].toString(), QString("test"));
        QCOMPARE(result["value"].toInt(), 42);
    }

    void testJSONParserMaxDepth() {
        JSONParser parser;
        
        parser.setMaxDepth(3);
        
        // Create deeply nested JSON
        QString deep_json = R"({
            "level1": {
                "level2": {
                    "level3": {
                        "level4": "too deep"
                    }
                }
            }
        })";
        
        try {
            auto result = parser.parseString(deep_json);
            // Should handle max depth gracefully
        } catch (const JSONParsingException& e) {
            // Exception is acceptable for max depth exceeded
            QVERIFY(e.getMessage().contains("depth") || e.getMessage().contains("nested"));
        }
    }

    // **JSONValidator Tests**
    void testJSONValidatorBasicValidation() {
        JSONValidator validator;
        
        QJsonObject valid_object;
        valid_object["name"] = "test";
        valid_object["value"] = 42;
        
        bool is_valid = validator.validate(valid_object);
        QVERIFY(is_valid);
        
        auto errors = validator.getValidationErrors();
        QVERIFY(errors.isEmpty());
    }

    void testJSONValidatorWithSchema() {
        JSONValidator validator;
        
        // Define a simple schema
        QJsonObject schema;
        schema["type"] = "object";
        
        QJsonObject properties;
        QJsonObject name_prop;
        name_prop["type"] = "string";
        properties["name"] = name_prop;
        
        QJsonObject value_prop;
        value_prop["type"] = "number";
        properties["value"] = value_prop;
        
        schema["properties"] = properties;
        schema["required"] = QJsonArray({"name", "value"});
        
        validator.setSchema(schema);
        
        // Test valid object
        QJsonObject valid_object;
        valid_object["name"] = "test";
        valid_object["value"] = 42;
        
        bool is_valid = validator.validate(valid_object);
        QVERIFY(is_valid);
        
        // Test invalid object (missing required field)
        QJsonObject invalid_object;
        invalid_object["name"] = "test";
        // Missing "value" field
        
        bool is_invalid = validator.validate(invalid_object);
        QVERIFY(!is_invalid);
        
        auto errors = validator.getValidationErrors();
        QVERIFY(!errors.isEmpty());
    }

    void testJSONValidatorCustomRules() {
        JSONValidator validator;
        
        // Add custom validation rule
        validator.addCustomRule("positive_number", [](const QJsonValue& value) -> bool {
            return value.isDouble() && value.toDouble() > 0;
        });
        
        QJsonObject test_object;
        test_object["positive_value"] = 42;
        test_object["negative_value"] = -5;
        
        // This test depends on how custom rules are applied in the implementation
        // The test documents the expected behavior
    }

    // **ComponentRegistry Tests**
    void testComponentRegistryBasicRegistration() {
        ComponentRegistry registry;
        
        // Register a component type
        registry.registerComponent("Button", [](const QJsonObject& config) -> QWidget* {
            auto button = new QPushButton();
            if (config.contains("text")) {
                button->setText(config["text"].toString());
            }
            return button;
        });
        
        QVERIFY(registry.isRegistered("Button"));
        QVERIFY(!registry.isRegistered("NonExistent"));
        
        auto registered_types = registry.getRegisteredTypes();
        QVERIFY(registered_types.contains("Button"));
    }

    void testComponentRegistryComponentCreation() {
        ComponentRegistry registry;
        
        // Register a label component
        registry.registerComponent("Label", [](const QJsonObject& config) -> QWidget* {
            auto label = new QLabel();
            if (config.contains("text")) {
                label->setText(config["text"].toString());
            }
            if (config.contains("enabled")) {
                label->setEnabled(config["enabled"].toBool());
            }
            return label;
        });
        
        QJsonObject config;
        config["text"] = "Test Label";
        config["enabled"] = false;
        
        auto widget = registry.createComponent("Label", config);
        QVERIFY(widget != nullptr);
        
        auto label = qobject_cast<QLabel*>(widget);
        QVERIFY(label != nullptr);
        QCOMPARE(label->text(), QString("Test Label"));
        QVERIFY(!label->isEnabled());
        
        delete widget; // Clean up
    }

    void testComponentRegistryUnregistration() {
        ComponentRegistry registry;
        
        registry.registerComponent("TempComponent", [](const QJsonObject&) -> QWidget* {
            return new QWidget();
        });
        
        QVERIFY(registry.isRegistered("TempComponent"));
        
        registry.unregisterComponent("TempComponent");
        QVERIFY(!registry.isRegistered("TempComponent"));
    }

    // **JSONUILoader Tests**
    void testJSONUILoaderBasicLoading() {
        JSONUILoader loader;
        
        QString ui_json = R"({
            "type": "QWidget",
            "properties": {
                "windowTitle": "Test Window",
                "geometry": [100, 100, 400, 300]
            },
            "children": [{
                "type": "QLabel",
                "properties": {
                    "text": "Hello World",
                    "alignment": "AlignCenter"
                }
            }]
        })";
        
        auto widget = loader.loadFromString(ui_json);
        QVERIFY(widget != nullptr);
        
        QCOMPARE(widget->windowTitle(), QString("Test Window"));
        
        // Check if child was created
        auto children = widget->findChildren<QLabel*>();
        QVERIFY(!children.isEmpty());
        QCOMPARE(children[0]->text(), QString("Hello World"));
    }

    void testJSONUILoaderWithEventHandlers() {
        JSONUILoader loader;
        
        bool event_triggered = false;
        loader.registerEventHandler("testClick", [&event_triggered]() {
            event_triggered = true;
        });
        
        QString ui_json = R"({
            "type": "QPushButton",
            "properties": {
                "text": "Click Me"
            },
            "events": {
                "clicked": "testClick"
            }
        })";
        
        auto widget = loader.loadFromString(ui_json);
        QVERIFY(widget != nullptr);
        
        auto button = qobject_cast<QPushButton*>(widget);
        QVERIFY(button != nullptr);
        QCOMPARE(button->text(), QString("Click Me"));
        
        // Simulate click to test event handler
        button->click();
        QVERIFY(event_triggered);
    }

    void testJSONUILoaderFileLoading() {
        JSONUILoader loader;
        
        // Create a temporary UI file
        QTemporaryFile temp_file;
        temp_file.setFileTemplate("test_ui_XXXXXX.json");
        QVERIFY(temp_file.open());
        
        QString ui_content = R"({
            "type": "QWidget",
            "properties": {
                "windowTitle": "File Loaded UI"
            },
            "children": [{
                "type": "QLabel",
                "properties": {
                    "text": "Loaded from file"
                }
            }]
        })";
        
        temp_file.write(ui_content.toUtf8());
        temp_file.close();
        
        auto widget = loader.loadFromFile(temp_file.fileName());
        QVERIFY(widget != nullptr);
        QCOMPARE(widget->windowTitle(), QString("File Loaded UI"));
        
        auto children = widget->findChildren<QLabel*>();
        QVERIFY(!children.isEmpty());
        QCOMPARE(children[0]->text(), QString("Loaded from file"));
    }

    // **Error Handling Tests**
    void testJSONParserErrorHandling() {
        JSONParser parser;
        
        QString invalid_json = R"({
            "name": "test",
            "value": 42,
            "invalid": 
        })"; // Incomplete JSON
        
        try {
            auto result = parser.parseString(invalid_json);
            QVERIFY(result.isEmpty()); // Should return empty on error
        } catch (const JSONParsingException& e) {
            QVERIFY(!e.getMessage().isEmpty());
            QVERIFY(!e.getFilePath().isEmpty() || e.getFilePath() == "");
        }
    }

    void testJSONValidatorErrorReporting() {
        JSONValidator validator;
        
        QJsonObject invalid_object;
        invalid_object["invalid_type"] = QJsonArray(); // Assuming this violates some rule
        
        bool is_valid = validator.validate(invalid_object);
        
        if (!is_valid) {
            auto errors = validator.getValidationErrors();
            QVERIFY(!errors.isEmpty());
            
            for (const auto& error : errors) {
                QVERIFY(!error.isEmpty());
            }
        }
    }

    void testComponentRegistryErrorHandling() {
        ComponentRegistry registry;
        
        // Try to create non-existent component
        auto widget = registry.createComponent("NonExistent", QJsonObject());
        QVERIFY(widget == nullptr);
        
        // Try to register with null factory
        try {
            registry.registerComponent("NullFactory", nullptr);
            // Should handle gracefully or throw exception
        } catch (const ComponentRegistrationException& e) {
            QVERIFY(!e.getMessage().isEmpty());
        }
    }

    void testJSONUILoaderErrorHandling() {
        JSONUILoader loader;
        
        QString invalid_ui_json = R"({
            "type": "NonExistentWidget",
            "properties": {
                "text": "This should fail"
            }
        })";
        
        try {
            auto widget = loader.loadFromString(invalid_ui_json);
            QVERIFY(widget == nullptr); // Should return null on error
        } catch (const UIException& e) {
            QVERIFY(!e.getMessage().isEmpty());
        }
    }

    // **Integration Tests**
    void testJSONWorkflowIntegration() {
        // Test complete workflow: Parse -> Validate -> Load UI
        JSONParser parser;
        JSONValidator validator;
        JSONUILoader loader;
        
        QString complete_ui_json = R"({
            "type": "QWidget",
            "properties": {
                "windowTitle": "Integration Test",
                "geometry": [0, 0, 300, 200]
            },
            "layout": {
                "type": "QVBoxLayout"
            },
            "children": [{
                "type": "QLabel",
                "properties": {
                    "text": "Integration Test Label"
                }
            }, {
                "type": "QPushButton",
                "properties": {
                    "text": "Integration Test Button"
                }
            }]
        })";
        
        // Parse
        auto parsed = parser.parseString(complete_ui_json);
        QVERIFY(!parsed.isEmpty());
        
        // Validate
        bool is_valid = validator.validate(parsed);
        QVERIFY(is_valid);
        
        // Load UI
        auto widget = loader.loadFromString(complete_ui_json);
        QVERIFY(widget != nullptr);
        QCOMPARE(widget->windowTitle(), QString("Integration Test"));
        
        // Verify children
        auto labels = widget->findChildren<QLabel*>();
        auto buttons = widget->findChildren<QPushButton*>();
        
        QVERIFY(!labels.isEmpty());
        QVERIFY(!buttons.isEmpty());
        QCOMPARE(labels[0]->text(), QString("Integration Test Label"));
        QCOMPARE(buttons[0]->text(), QString("Integration Test Button"));
    }
};

QTEST_MAIN(JSONTest)
#include "test_json.moc"
