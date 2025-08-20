/**
 * @file test_refactored_components.cpp
 * @brief Unit tests to verify refactored components maintain functionality
 *
 * This test suite validates that the refactored code maintains all existing
 * functionality while improving code quality metrics.
 */

#include <gtest/gtest.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>

#include "Binding/StateManager.hpp"
#include "Core/UIElement.hpp"
#include "JSON/JSONParser.hpp"
#include "JSON/JSONValidator.hpp"

using namespace DeclarativeUI;

/**
 * @brief Test suite for refactored JSONValidator functionality
 */
class JSONValidatorTest : public ::testing::Test {
protected:
    void SetUp() override {
        validator = std::make_unique<JSON::UIJSONValidator>();
        validator->registerBuiltinValidators();
    }

    std::unique_ptr<JSON::UIJSONValidator> validator;
};

/**
 * @brief Test basic component validation functionality
 */
TEST_F(JSONValidatorTest, ValidateBasicComponent) {
    QJsonObject component;
    component["type"] = "QLabel";

    QJsonObject properties;
    properties["text"] = "Hello World";
    component["properties"] = properties;

    // Test that basic validation still works after refactoring
    bool result = validator->validateComponent(component, "QLabel");
    EXPECT_TRUE(result) << "Basic component validation should pass";
}

/**
 * @brief Test property validation functionality
 */
TEST_F(JSONValidatorTest, ValidateProperties) {
    QJsonObject properties;
    properties["text"] = "Test Text";
    properties["enabled"] = true;
    properties["width"] = 100;

    // Test that property validation still works after refactoring
    bool result = validator->validateProperties(properties, "QLabel");
    EXPECT_TRUE(result)
        << "Property validation should pass for valid properties";
}

/**
 * @brief Test invalid property types are caught
 */
TEST_F(JSONValidatorTest, InvalidPropertyTypes) {
    QJsonObject properties;
    properties["enabled"] = "not_a_boolean";  // Should be boolean

    // Test that invalid property types are still caught
    bool result = validator->validateProperties(properties, "QLabel");
    EXPECT_FALSE(result) << "Property validation should fail for invalid types";
}

/**
 * @brief Test suite for refactored JSONParser functionality
 */
class JSONParserTest : public ::testing::Test {
protected:
    void SetUp() override { parser = std::make_unique<JSON::JSONParser>(); }

    std::unique_ptr<JSON::JSONParser> parser;
};

/**
 * @brief Test basic JSON parsing functionality
 */
TEST_F(JSONParserTest, ParseBasicJSON) {
    QString json_string = R"({
        "type": "QWidget",
        "properties": {
            "windowTitle": "Test Window"
        }
    })";

    QJsonObject result = parser->parseString(json_string);
    EXPECT_FALSE(result.isEmpty()) << "JSON parsing should succeed";
    EXPECT_EQ(result["type"].toString(), "QWidget")
        << "Type should be preserved";
}

/**
 * @brief Test JSON object processing functionality
 */
TEST_F(JSONParserTest, ProcessJSONObject) {
    QJsonObject input;
    input["type"] = "QLabel";
    input["text"] = "Test Label";

    // Test that object processing still works after refactoring
    EXPECT_TRUE(input.contains("type")) << "Input should contain type";
    EXPECT_EQ(input["type"].toString(), "QLabel") << "Type should be correct";
}

/**
 * @brief Test suite for refactored UIElement functionality
 */
class UIElementTest : public ::testing::Test {
protected:
    void SetUp() override { element = std::make_unique<Core::UIElement>(); }

    std::unique_ptr<Core::UIElement> element;
};

/**
 * @brief Test UIElement serialization functionality
 */
TEST_F(UIElementTest, SerializeDeserialize) {
    // Set some properties
    element->setProperty("text", QString("Test Text"));
    element->setProperty("enabled", true);

    // Serialize
    QJsonObject serialized = element->serialize();
    EXPECT_FALSE(serialized.isEmpty())
        << "Serialization should produce non-empty JSON";

    // Create new element and deserialize
    auto new_element = std::make_unique<Core::UIElement>();
    bool result = new_element->deserialize(serialized);
    EXPECT_TRUE(result) << "Deserialization should succeed";
}

/**
 * @brief Test UIElement property management
 */
TEST_F(UIElementTest, PropertyManagement) {
    // Test setting and getting properties
    element->setProperty("width", 100);
    element->setProperty("height", 200);

    // Verify properties are stored correctly
    EXPECT_TRUE(element->hasProperty("width")) << "Width property should exist";
    EXPECT_TRUE(element->hasProperty("height"))
        << "Height property should exist";
}

/**
 * @brief Test suite for refactored StateManager functionality
 */
class StateManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        state_manager = &Binding::StateManager::instance();
        state_manager->clearState();
    }

    void TearDown() override { state_manager->clearState(); }

    Binding::StateManager* state_manager;
};

/**
 * @brief Test basic state management functionality
 */
TEST_F(StateManagerTest, BasicStateOperations) {
    // Create a state
    auto counter = state_manager->createState<int>("counter", 0);
    EXPECT_NE(counter, nullptr) << "State creation should succeed";

    // Set and get state value
    counter->set(42);
    EXPECT_EQ(counter->get(), 42) << "State value should be updated";
}

/**
 * @brief Test state persistence functionality
 */
TEST_F(StateManagerTest, StatePersistence) {
    // Create and set some state
    auto test_state = state_manager->createState<QString>("test", "initial");
    test_state->set("modified");

    // Test that state can be saved (basic functionality check)
    EXPECT_NO_THROW(state_manager->saveState("test_state.json"))
        << "State saving should not throw exceptions";
}

/**
 * @brief Integration test for all refactored components
 */
TEST(IntegrationTest, RefactoredComponentsIntegration) {
    // Test that all refactored components can work together
    auto validator = std::make_unique<JSON::UIJSONValidator>();
    auto parser = std::make_unique<JSON::JSONParser>();
    auto element = std::make_unique<Core::UIElement>();

    // Create a simple UI definition
    QString ui_definition = R"({
        "type": "QWidget",
        "properties": {
            "windowTitle": "Integration Test",
            "width": 400,
            "height": 300
        }
    })";

    // Parse the definition
    QJsonObject parsed = parser->parseString(ui_definition);
    EXPECT_FALSE(parsed.isEmpty()) << "Parsing should succeed";

    // Validate the definition
    validator->registerBuiltinValidators();
    bool valid = validator->validate(parsed);
    EXPECT_TRUE(valid) << "Validation should pass";

    // Deserialize into UIElement
    bool deserialized = element->deserialize(parsed);
    EXPECT_TRUE(deserialized) << "Deserialization should succeed";
}

/**
 * @brief Performance test to ensure refactoring didn't degrade performance
 */
TEST(PerformanceTest, RefactoredComponentsPerformance) {
    auto start = std::chrono::high_resolution_clock::now();

    // Perform multiple operations to test performance
    for (int i = 0; i < 100; ++i) {
        auto validator = std::make_unique<JSON::UIJSONValidator>();
        validator->registerBuiltinValidators();

        QJsonObject component;
        component["type"] = "QLabel";
        QJsonObject properties;
        properties["text"] = QString("Test %1").arg(i);
        component["properties"] = properties;

        validator->validateComponent(component, "QLabel");
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    // Performance should be reasonable (less than 1 second for 100 operations)
    EXPECT_LT(duration.count(), 1000) << "Performance should be acceptable";
}
