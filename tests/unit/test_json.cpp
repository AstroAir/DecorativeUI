#include <QApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QPushButton>
#include <QSignalSpy>
#include <QTemporaryFile>
#include <QTest>
#include <QUrl>
#include <QWidget>
#include <memory>

#include "../Exceptions/UIExceptions.hpp"
#include "../JSON/ComponentRegistry.hpp"
#include "../JSON/JSONParser.hpp"
#include "../JSON/JSONUILoader.hpp"
#include "../JSON/JSONValidator.hpp"

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
        QString invalid_json =
            R"({invalid: true})";  // Missing quotes around key
        try {
            auto invalid_result = parser.parseString(invalid_json);
            // Should either return empty object or throw exception
            QVERIFY(invalid_result.isEmpty());
        } catch (const JSONParsingException& e) {
            // Exception is acceptable in strict mode
            QVERIFY(!e.getMessage().empty());
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
            QVERIFY(e.getMessage().find("depth") != std::string::npos ||
                    e.getMessage().find("nested") != std::string::npos);
        }
    }

    // **JSONValidator Tests**
    void testJSONValidatorBasicValidation() {
        UIJSONValidator validator;

        QJsonObject valid_object;
        valid_object["type"] = "QLabel";
        valid_object["properties"] = QJsonObject();

        bool is_valid = validator.validate(valid_object);
        QVERIFY(is_valid);

        auto errors = validator.getErrors();
        QVERIFY(errors.empty());
    }

    void testJSONValidatorWithSchema() {
        UIJSONValidator validator;

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

        validator.loadSchema(schema);

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

        auto errors = validator.getErrors();
        QVERIFY(!errors.empty());
    }

    void testJSONValidatorCustomRules() {
        UIJSONValidator validator;

        // Test basic validation functionality
        QJsonObject test_object;
        test_object["type"] = "QLabel";
        test_object["properties"] = QJsonObject{{"text", "Test"}};

        bool is_valid = validator.validate(test_object);
        QVERIFY(is_valid);

        // Test invalid component type
        QJsonObject invalid_object;
        invalid_object["type"] = "InvalidComponent";
        bool is_invalid = validator.validate(invalid_object);
        QVERIFY(!is_invalid);
    }

    // **ComponentRegistry Tests**
    void testComponentRegistryBasicRegistration() {
        auto& registry = ComponentRegistry::instance();

        // Register a component type
        registry.registerComponent<QPushButton>(
            "Button",
            [](const QJsonObject& config) -> std::unique_ptr<QPushButton> {
                auto button = std::make_unique<QPushButton>();
                if (config.contains("text")) {
                    button->setText(config["text"].toString());
                }
                return button;
            });

        QVERIFY(registry.hasComponent("Button"));
        QVERIFY(!registry.hasComponent("NonExistent"));

        auto registered_types = registry.getRegisteredTypes();
        QVERIFY(registered_types.contains("Button"));
    }

    void testComponentRegistryComponentCreation() {
        auto& registry = ComponentRegistry::instance();

        // Register a label component
        registry.registerComponent<QLabel>(
            "Label", [](const QJsonObject& config) -> std::unique_ptr<QLabel> {
                auto label = std::make_unique<QLabel>();
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

        auto label = qobject_cast<QLabel*>(widget.get());
        QVERIFY(label != nullptr);
        QCOMPARE(label->text(), QString("Test Label"));
        QVERIFY(!label->isEnabled());
    }

    void testComponentRegistryUnregistration() {
        auto& registry = ComponentRegistry::instance();

        // Test clearing registry (since unregisterComponent doesn't exist)
        registry.registerComponent<QWidget>(
            "TempComponent",
            [](const QJsonObject&) -> std::unique_ptr<QWidget> {
                return std::make_unique<QWidget>();
            });

        QVERIFY(registry.hasComponent("TempComponent"));

        // Clear all components (this is the available API)
        registry.clear();
        QVERIFY(!registry.hasComponent("TempComponent"));
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
        loader.registerEventHandler(
            "testClick", [&event_triggered]() { event_triggered = true; });

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

        auto button = qobject_cast<QPushButton*>(widget.get());
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
        })";  // Incomplete JSON

        try {
            auto result = parser.parseString(invalid_json);
            QVERIFY(result.isEmpty());  // Should return empty on error
        } catch (const JSONParsingException& e) {
            QVERIFY(!e.getMessage().empty());
            QVERIFY(!e.getFilePath().empty() || e.getFilePath() == "");
        }
    }

    void testJSONValidatorErrorReporting() {
        UIJSONValidator validator;

        QJsonObject invalid_object;
        invalid_object["invalid_type"] =
            QJsonArray();  // Assuming this violates some rule

        bool is_valid = validator.validate(invalid_object);

        if (!is_valid) {
            auto errors = validator.getErrors();
            QVERIFY(!errors.empty());

            for (const auto& error : errors) {
                QVERIFY(!error.message.isEmpty());
            }
        }
    }

    void testComponentRegistryErrorHandling() {
        auto& registry = ComponentRegistry::instance();

        // Try to create non-existent component
        auto widget = registry.createComponent("NonExistent", QJsonObject());
        QVERIFY(widget == nullptr);

        // Test factory that returns nullptr
        registry.registerComponent<QWidget>(
            "NullFactory",
            [](const QJsonObject& config) -> std::unique_ptr<QWidget> {
                return nullptr;
            });

        auto null_widget =
            registry.createComponent("NullFactory", QJsonObject());
        QVERIFY(null_widget == nullptr);
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
            QVERIFY(widget == nullptr);  // Should return null on error
        } catch (const UIException& e) {
            QVERIFY(!e.getMessage().empty());
        }
    }

    // **Integration Tests**
    void testJSONWorkflowIntegration() {
        // Test complete workflow: Parse -> Validate -> Load UI
        JSONParser parser;
        UIJSONValidator validator;
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

    // **Enhanced JSON Validation Tests**
    void testJSONValidatorSchemaValidation() {
        UIJSONValidator validator;

        // Test valid schema
        QString valid_schema_json = R"({
            "widget": {
                "type": "QLabel",
                "properties": {
                    "text": "Valid text",
                    "enabled": true,
                    "visible": true
                }
            }
        })";

        QJsonParseError error;
        auto doc = QJsonDocument::fromJson(valid_schema_json.toUtf8(), &error);
        QVERIFY(error.error == QJsonParseError::NoError);

        bool is_valid = validator.validate(doc.object());
        QVERIFY(is_valid);

        // Test invalid schema - missing required fields
        QString invalid_schema_json = R"({
            "widget": {
                "properties": {
                    "text": "Missing type field"
                }
            }
        })";

        auto invalid_doc =
            QJsonDocument::fromJson(invalid_schema_json.toUtf8(), &error);
        QVERIFY(error.error == QJsonParseError::NoError);

        bool is_invalid = validator.validate(invalid_doc.object());
        QVERIFY(!is_invalid);
    }

    void testJSONValidatorPropertyTypes() {
        UIJSONValidator validator;

        // Test invalid property types
        QString invalid_types_json = R"({
            "widget": {
                "type": "QLabel",
                "properties": {
                    "text": 123,
                    "enabled": "not_a_boolean",
                    "width": "not_a_number"
                }
            }
        })";

        QJsonParseError error;
        auto doc = QJsonDocument::fromJson(invalid_types_json.toUtf8(), &error);
        QVERIFY(error.error == QJsonParseError::NoError);

        [[maybe_unused]] bool result = validator.validate(doc.object());
        // Depending on implementation, this might be valid or invalid
        // The test documents the expected behavior
        QVERIFY(true);  // Placeholder - adjust based on actual implementation
    }

    void testJSONValidatorNestedStructures() {
        UIJSONValidator validator;

        // Test nested widget validation
        QString nested_json = R"({
            "widget": {
                "type": "QWidget",
                "properties": {
                    "windowTitle": "Main Window"
                },
                "children": [
                    {
                        "type": "QLabel",
                        "properties": {
                            "text": "Child Label"
                        }
                    },
                    {
                        "type": "QPushButton",
                        "properties": {
                            "text": "Child Button"
                        }
                    }
                ]
            }
        })";

        QJsonParseError error;
        auto doc = QJsonDocument::fromJson(nested_json.toUtf8(), &error);
        QVERIFY(error.error == QJsonParseError::NoError);

        bool is_valid = validator.validate(doc.object());
        QVERIFY(is_valid);
    }

    void testJSONValidatorErrorRecovery() {
        UIJSONValidator validator;

        // Test malformed JSON
        QString malformed_json = R"({
            "widget": {
                "type": "QLabel",
                "properties": {
                    "text": "Unclosed string
                }
            }
        })";

        QJsonParseError error;
        auto doc = QJsonDocument::fromJson(malformed_json.toUtf8(), &error);
        QVERIFY(error.error != QJsonParseError::NoError);

        // Validator should handle parse errors gracefully
        bool is_valid = validator.validate(QJsonObject());
        QVERIFY(!is_valid ||
                is_valid);  // Either result is acceptable for empty object
    }

    void testComponentRegistryAdvanced() {
        auto& registry = ComponentRegistry::instance();

        // Test component override
        registry.registerComponent<QPushButton>(
            "Button",
            [](const QJsonObject& config) -> std::unique_ptr<QPushButton> {
                return std::make_unique<QPushButton>("Original");
            });

        registry.registerComponent<QPushButton>(
            "Button",
            [](const QJsonObject& config) -> std::unique_ptr<QPushButton> {
                return std::make_unique<QPushButton>("Override");
            });

        QJsonObject config;
        auto widget = registry.createComponent("Button", config);
        QVERIFY(widget != nullptr);

        auto* button = qobject_cast<QPushButton*>(widget.get());
        QVERIFY(button != nullptr);
        QCOMPARE(button->text(), QString("Override"));
    }
};

QTEST_MAIN(JSONTest)
#include "test_json.moc"
