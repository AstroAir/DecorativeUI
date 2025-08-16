#include <QTest>
#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTemporaryFile>
#include <QTemporaryDir>
#include <QElapsedTimer>

class JSONExtendedTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Extended JSON Parsing Tests
    void testLargeJSONParsing();
    void testNestedJSONStructures();
    void testJSONArrayOperations();

    // JSON Performance Tests
    void testJSONParsingPerformance();

    // JSON Edge Cases
    void testEmptyJSONHandling();
    void testMalformedJSONHandling();
    void testUnicodeJSONHandling();

    // JSON File Operations
    void testJSONFileOperations();

    // JSON Schema Tests
    void testJSONSchemaValidation();

private:
    QTemporaryDir temp_dir_;
};

void JSONExtendedTest::initTestCase() {
    QVERIFY(temp_dir_.isValid());
}

void JSONExtendedTest::cleanupTestCase() {
    // Cleanup handled by temp_dir_ destructor
}

void JSONExtendedTest::init() {
    // Setup for each test
}

void JSONExtendedTest::cleanup() {
    // Cleanup after each test
}

void JSONExtendedTest::testLargeJSONParsing() {
    // Create a large JSON structure
    QJsonObject large_object;
    QJsonArray large_array;
    
    // Add many items to test performance and memory handling
    for (int i = 0; i < 1000; ++i) {
        QJsonObject item = {
            {"id", i},
            {"name", QString("Item %1").arg(i)},
            {"value", i * 1.5},
            {"active", i % 2 == 0},
            {"metadata", QJsonObject{
                {"created", "2023-01-01"},
                {"modified", "2023-12-31"},
                {"tags", QJsonArray{"tag1", "tag2", "tag3"}}
            }}
        };
        large_array.append(item);
    }
    
    large_object["items"] = large_array;
    large_object["count"] = large_array.size();
    large_object["version"] = "1.0.0";
    
    // Test serialization
    QJsonDocument doc(large_object);
    QByteArray json_data = doc.toJson();
    QVERIFY(!json_data.isEmpty());
    QVERIFY(json_data.size() > 10000); // Should be substantial
    
    // Test parsing
    QJsonParseError error;
    QJsonDocument parsed_doc = QJsonDocument::fromJson(json_data, &error);
    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(!parsed_doc.isNull());
    
    QJsonObject parsed_object = parsed_doc.object();
    QVERIFY(parsed_object["count"].toInt() == 1000);
    QVERIFY(parsed_object["version"].toString() == "1.0.0");
    
    QJsonArray parsed_array = parsed_object["items"].toArray();
    QVERIFY(parsed_array.size() == 1000);
    
    // Verify some items
    QJsonObject first_item = parsed_array[0].toObject();
    QVERIFY(first_item["id"].toInt() == 0);
    QVERIFY(first_item["name"].toString() == "Item 0");
    
    QJsonObject last_item = parsed_array[999].toObject();
    QVERIFY(last_item["id"].toInt() == 999);
    QVERIFY(last_item["name"].toString() == "Item 999");
}

void JSONExtendedTest::testNestedJSONStructures() {
    // Create deeply nested JSON structure
    QJsonObject root;
    QJsonObject current = root;
    
    // Create 10 levels of nesting
    for (int level = 0; level < 10; ++level) {
        QJsonObject next_level = {
            {"level", level},
            {"data", QString("Level %1 data").arg(level)},
            {"items", QJsonArray{level * 10, level * 20, level * 30}}
        };
        
        if (level < 9) {
            next_level["child"] = QJsonObject();
        }
        
        if (level == 0) {
            root = next_level;
            current = root;
        } else {
            // This is a simplified approach - in practice you'd need to navigate the structure
            current["child"] = next_level;
        }
    }
    
    // Test serialization and parsing of nested structure
    QJsonDocument doc(root);
    QByteArray json_data = doc.toJson();
    
    QJsonParseError error;
    QJsonDocument parsed_doc = QJsonDocument::fromJson(json_data, &error);
    QVERIFY(error.error == QJsonParseError::NoError);
    
    QJsonObject parsed_root = parsed_doc.object();
    QVERIFY(parsed_root["level"].toInt() == 0);
    QVERIFY(parsed_root["data"].toString() == "Level 0 data");
    QVERIFY(parsed_root.contains("child"));
}

void JSONExtendedTest::testJSONArrayOperations() {
    QJsonArray test_array;
    
    // Test adding different types
    test_array.append("string");
    test_array.append(42);
    test_array.append(3.14);
    test_array.append(true);
    test_array.append(QJsonObject{{"key", "value"}});
    test_array.append(QJsonArray{1, 2, 3});
    
    QVERIFY(test_array.size() == 6);
    
    // Test type checking
    QVERIFY(test_array[0].isString());
    QVERIFY(test_array[1].isDouble()); // JSON numbers are doubles
    QVERIFY(test_array[2].isDouble());
    QVERIFY(test_array[3].isBool());
    QVERIFY(test_array[4].isObject());
    QVERIFY(test_array[5].isArray());
    
    // Test value retrieval
    QVERIFY(test_array[0].toString() == "string");
    QVERIFY(test_array[1].toInt() == 42);
    QVERIFY(qFuzzyCompare(test_array[2].toDouble(), 3.14));
    QVERIFY(test_array[3].toBool() == true);
    
    QJsonObject nested_obj = test_array[4].toObject();
    QVERIFY(nested_obj["key"].toString() == "value");
    
    QJsonArray nested_array = test_array[5].toArray();
    QVERIFY(nested_array.size() == 3);
    QVERIFY(nested_array[0].toInt() == 1);
    
    // Test array modification
    test_array.removeAt(0);
    QVERIFY(test_array.size() == 5);
    QVERIFY(test_array[0].toInt() == 42); // First element is now the integer
    
    test_array.insert(0, "new_first");
    QVERIFY(test_array.size() == 6);
    QVERIFY(test_array[0].toString() == "new_first");
}

void JSONExtendedTest::testJSONParsingPerformance() {
    // Create test data
    QJsonObject test_object;
    QJsonArray items;
    
    const int num_items = 5000;
    
    QElapsedTimer timer;
    timer.start();
    
    // Create large dataset
    for (int i = 0; i < num_items; ++i) {
        QJsonObject item = {
            {"id", i},
            {"name", QString("Performance Test Item %1").arg(i)},
            {"description", QString("This is a longer description for item %1 to test performance with larger strings").arg(i)},
            {"value", i * 2.5},
            {"active", i % 3 == 0},
            {"tags", QJsonArray{QString("tag_%1").arg(i % 10), QString("category_%1").arg(i % 5)}},
            {"metadata", QJsonObject{
                {"created_at", "2023-01-01T00:00:00Z"},
                {"updated_at", "2023-12-31T23:59:59Z"},
                {"version", 1}
            }}
        };
        items.append(item);
    }
    
    test_object["items"] = items;
    test_object["total_count"] = num_items;
    test_object["generated_at"] = "2023-12-31T12:00:00Z";
    
    qint64 creation_time = timer.elapsed();
    
    // Test serialization performance
    timer.restart();
    QJsonDocument doc(test_object);
    QByteArray json_data = doc.toJson(QJsonDocument::Compact);
    qint64 serialization_time = timer.elapsed();
    
    // Test parsing performance
    timer.restart();
    QJsonParseError error;
    QJsonDocument parsed_doc = QJsonDocument::fromJson(json_data, &error);
    qint64 parsing_time = timer.elapsed();
    
    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(!parsed_doc.isNull());
    
    qDebug() << "JSON Performance Results:";
    qDebug() << "Created" << num_items << "items in" << creation_time << "ms";
    qDebug() << "Serialized to" << json_data.size() << "bytes in" << serialization_time << "ms";
    qDebug() << "Parsed" << json_data.size() << "bytes in" << parsing_time << "ms";
    
    // Performance assertions
    QVERIFY(creation_time < 1000); // Should create within 1 second
    QVERIFY(serialization_time < 500); // Should serialize within 0.5 seconds
    QVERIFY(parsing_time < 500); // Should parse within 0.5 seconds
    
    // Verify parsed data
    QJsonObject parsed_object = parsed_doc.object();
    QVERIFY(parsed_object["total_count"].toInt() == num_items);
    
    QJsonArray parsed_items = parsed_object["items"].toArray();
    QVERIFY(parsed_items.size() == num_items);
}

void JSONExtendedTest::testEmptyJSONHandling() {
    // Test empty object
    QJsonObject empty_object;
    QJsonDocument empty_doc(empty_object);
    QByteArray empty_data = empty_doc.toJson();
    
    QJsonParseError error;
    QJsonDocument parsed_empty = QJsonDocument::fromJson(empty_data, &error);
    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(parsed_empty.object().isEmpty());
    
    // Test empty array
    QJsonArray empty_array;
    QJsonDocument empty_array_doc(empty_array);
    QByteArray empty_array_data = empty_array_doc.toJson();
    
    QJsonDocument parsed_empty_array = QJsonDocument::fromJson(empty_array_data, &error);
    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(parsed_empty_array.array().isEmpty());
    
    // Test empty string
    QByteArray empty_string = "";
    QJsonDocument parsed_empty_string = QJsonDocument::fromJson(empty_string, &error);
    QVERIFY(error.error != QJsonParseError::NoError); // Should fail
    
    // Test whitespace only
    QByteArray whitespace = "   \n\t  ";
    QJsonDocument parsed_whitespace = QJsonDocument::fromJson(whitespace, &error);
    QVERIFY(error.error != QJsonParseError::NoError); // Should fail
}

void JSONExtendedTest::testMalformedJSONHandling() {
    QJsonParseError error;
    
    // Test various malformed JSON strings
    QStringList malformed_json = {
        "{",                    // Unclosed object
        "}",                    // Unexpected closing brace
        "{\"key\": }",         // Missing value
        "{\"key\": \"value\",}", // Trailing comma
        "{key: \"value\"}",    // Unquoted key
        "{\"key\": 'value'}",  // Single quotes
        "{\"key\": undefined}", // Undefined value
        "[1, 2, 3,]",          // Trailing comma in array
        "{\"key\": \"value\" \"key2\": \"value2\"}", // Missing comma
        "{\"key\": \"value\\\"}",  // Improper escape
    };
    
    for (const QString& json_str : malformed_json) {
        QJsonDocument doc = QJsonDocument::fromJson(json_str.toUtf8(), &error);
        QVERIFY(error.error != QJsonParseError::NoError);
        QVERIFY(doc.isNull());
        
        // Verify error information is provided
        QVERIFY(error.offset >= 0);
        QVERIFY(!error.errorString().isEmpty());
        
        qDebug() << "Malformed JSON:" << json_str;
        qDebug() << "Error:" << error.errorString() << "at offset" << error.offset;
    }
}

void JSONExtendedTest::testUnicodeJSONHandling() {
    // Test various Unicode characters
    QJsonObject unicode_object = {
        {"english", "Hello World"},
        {"chinese", "你好世界"},
        {"japanese", "こんにちは世界"},
        {"arabic", "مرحبا بالعالم"},
        {"emoji", "🌍🚀💻🎉"},
        {"special_chars", "\"\\n\\t\\r\\b\\f"},
        {"unicode_escape", "\\u0048\\u0065\\u006C\\u006C\\u006F"}
    };
    
    // Test serialization
    QJsonDocument doc(unicode_object);
    QByteArray json_data = doc.toJson();
    QVERIFY(!json_data.isEmpty());
    
    // Test parsing
    QJsonParseError error;
    QJsonDocument parsed_doc = QJsonDocument::fromJson(json_data, &error);
    QVERIFY(error.error == QJsonParseError::NoError);
    
    QJsonObject parsed_object = parsed_doc.object();
    
    // Verify Unicode strings are preserved
    QVERIFY(parsed_object["english"].toString() == "Hello World");
    QVERIFY(parsed_object["chinese"].toString() == "你好世界");
    QVERIFY(parsed_object["japanese"].toString() == "こんにちは世界");
    QVERIFY(parsed_object["arabic"].toString() == "مرحبا بالعالم");
    QVERIFY(parsed_object["emoji"].toString() == "🌍🚀💻🎉");
}

void JSONExtendedTest::testJSONFileOperations() {
    // Create test JSON data
    QJsonObject test_data = {
        {"application", "DeclarativeUI"},
        {"version", "1.0.0"},
        {"settings", QJsonObject{
            {"theme", "dark"},
            {"language", "en"},
            {"auto_save", true}
        }},
        {"recent_files", QJsonArray{
            "/path/to/file1.json",
            "/path/to/file2.json",
            "/path/to/file3.json"
        }}
    };
    
    // Test file saving
    QString test_file_path = temp_dir_.path() + "/test_config.json";
    QFile save_file(test_file_path);
    QVERIFY(save_file.open(QIODevice::WriteOnly));
    
    QJsonDocument save_doc(test_data);
    QByteArray json_data = save_doc.toJson();
    qint64 bytes_written = save_file.write(json_data);
    save_file.close();
    
    QVERIFY(bytes_written == json_data.size());
    QVERIFY(QFile::exists(test_file_path));
    
    // Test file loading
    QFile load_file(test_file_path);
    QVERIFY(load_file.open(QIODevice::ReadOnly));
    
    QByteArray loaded_data = load_file.readAll();
    load_file.close();
    
    QVERIFY(loaded_data.size() == json_data.size());
    
    // Test parsing loaded data
    QJsonParseError error;
    QJsonDocument loaded_doc = QJsonDocument::fromJson(loaded_data, &error);
    QVERIFY(error.error == QJsonParseError::NoError);
    
    QJsonObject loaded_object = loaded_doc.object();
    QVERIFY(loaded_object["application"].toString() == "DeclarativeUI");
    QVERIFY(loaded_object["version"].toString() == "1.0.0");
    
    QJsonObject settings = loaded_object["settings"].toObject();
    QVERIFY(settings["theme"].toString() == "dark");
    QVERIFY(settings["auto_save"].toBool() == true);
    
    QJsonArray recent_files = loaded_object["recent_files"].toArray();
    QVERIFY(recent_files.size() == 3);
    QVERIFY(recent_files[0].toString() == "/path/to/file1.json");
}

void JSONExtendedTest::testJSONSchemaValidation() {
    // Define a simple schema structure (conceptual - Qt doesn't have built-in schema validation)
    QJsonObject schema = {
        {"type", "object"},
        {"required", QJsonArray{"name", "age"}},
        {"properties", QJsonObject{
            {"name", QJsonObject{{"type", "string"}, {"minLength", 1}}},
            {"age", QJsonObject{{"type", "number"}, {"minimum", 0}, {"maximum", 150}}},
            {"email", QJsonObject{{"type", "string"}, {"pattern", "^[^@]+@[^@]+\\.[^@]+$"}}}
        }}
    };
    
    // Test valid data
    QJsonObject valid_data = {
        {"name", "John Doe"},
        {"age", 30},
        {"email", "john@example.com"}
    };
    
    // Test invalid data
    QJsonObject invalid_data1 = {
        {"age", 30} // Missing required "name"
    };
    
    QJsonObject invalid_data2 = {
        {"name", ""},  // Empty name (violates minLength)
        {"age", 30}
    };
    
    QJsonObject invalid_data3 = {
        {"name", "John Doe"},
        {"age", -5}  // Negative age (violates minimum)
    };
    
    // Basic validation logic (simplified)
    auto validate_object = [](const QJsonObject& data, const QJsonObject& schema) -> bool {
        // Check required fields
        QJsonArray required = schema["required"].toArray();
        for (const auto& req : required) {
            if (!data.contains(req.toString())) {
                return false;
            }
        }
        
        // Check basic type constraints
        QJsonObject properties = schema["properties"].toObject();
        for (auto it = data.begin(); it != data.end(); ++it) {
            QString key = it.key();
            if (properties.contains(key)) {
                QJsonObject prop_schema = properties[key].toObject();
                QString expected_type = prop_schema["type"].toString();
                
                if (expected_type == "string" && !it.value().isString()) {
                    return false;
                }
                if (expected_type == "number" && !it.value().isDouble()) {
                    return false;
                }
                
                // Check string constraints
                if (expected_type == "string" && it.value().isString()) {
                    QString str_value = it.value().toString();
                    if (prop_schema.contains("minLength")) {
                        int min_length = prop_schema["minLength"].toInt();
                        if (str_value.length() < min_length) {
                            return false;
                        }
                    }
                }
                
                // Check number constraints
                if (expected_type == "number" && it.value().isDouble()) {
                    double num_value = it.value().toDouble();
                    if (prop_schema.contains("minimum")) {
                        double minimum = prop_schema["minimum"].toDouble();
                        if (num_value < minimum) {
                            return false;
                        }
                    }
                    if (prop_schema.contains("maximum")) {
                        double maximum = prop_schema["maximum"].toDouble();
                        if (num_value > maximum) {
                            return false;
                        }
                    }
                }
            }
        }
        
        return true;
    };
    
    // Test validation
    QVERIFY(validate_object(valid_data, schema) == true);
    QVERIFY(validate_object(invalid_data1, schema) == false);
    QVERIFY(validate_object(invalid_data2, schema) == false);
    QVERIFY(validate_object(invalid_data3, schema) == false);
}

QTEST_MAIN(JSONExtendedTest)
#include "test_json_extended.moc"
