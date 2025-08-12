#pragma once

#include <QApplication>
#include <QElapsedTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTemporaryFile>
#include <QTest>
#include <QWidget>
#include <functional>
#include <memory>
#include <random>
#include <vector>

namespace DeclarativeUI::Testing {

/**
 * @brief Utility class for common test operations
 */
class TestUtilities {
public:
    /**
     * @brief Initialize Qt application for testing
     */
    static void initializeQtApplication() {
        if (!QApplication::instance()) {
            static int argc = 0;
            static char* argv[] = {nullptr};
            new QApplication(argc, argv);
        }
    }

    /**
     * @brief Create a temporary directory for test files
     */
    static std::unique_ptr<QTemporaryDir> createTempDir() {
        auto temp_dir = std::make_unique<QTemporaryDir>();
        if (!temp_dir->isValid()) {
            qWarning() << "Failed to create temporary directory";
            return nullptr;
        }
        return temp_dir;
    }

    /**
     * @brief Create a temporary file with specified content
     */
    static std::unique_ptr<QTemporaryFile> createTempFile(
        const QString& content, const QString& suffix = ".tmp") {
        auto temp_file = std::make_unique<QTemporaryFile>();
        temp_file->setFileTemplate(QString("test_XXXXXX%1").arg(suffix));

        if (!temp_file->open()) {
            qWarning() << "Failed to create temporary file";
            return nullptr;
        }

        temp_file->write(content.toUtf8());
        temp_file->close();
        return temp_file;
    }

    /**
     * @brief Generate random string for testing
     */
    static QString generateRandomString(int length = 10) {
        static const QString chars =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<> dis(0, chars.length() - 1);

        QString result;
        result.reserve(length);
        for (int i = 0; i < length; ++i) {
            result.append(chars[dis(gen)]);
        }
        return result;
    }

    /**
     * @brief Generate random integer in range
     */
    static int generateRandomInt(int min = 0, int max = 100) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(min, max);
        return dis(gen);
    }

    /**
     * @brief Generate random double in range
     */
    static double generateRandomDouble(double min = 0.0, double max = 1.0) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(min, max);
        return dis(gen);
    }

    /**
     * @brief Wait for condition with timeout
     */
    static bool waitForCondition(std::function<bool()> condition,
                                 int timeout_ms = 5000) {
        QElapsedTimer timer;
        timer.start();

        while (timer.elapsed() < timeout_ms) {
            if (condition()) {
                return true;
            }
            QApplication::processEvents();
            QTest::qWait(10);
        }
        return false;
    }

    /**
     * @brief Measure execution time of a function
     */
    template <typename Func>
    static qint64 measureExecutionTime(Func&& func) {
        QElapsedTimer timer;
        timer.start();
        func();
        return timer.elapsed();
    }

    /**
     * @brief Create test JSON object
     */
    static QJsonObject createTestJsonObject(const QString& type = "QLabel") {
        QJsonObject obj;
        obj["type"] = type;

        QJsonObject properties;
        properties["text"] = "Test Text";
        properties["enabled"] = true;
        properties["visible"] = true;
        obj["properties"] = properties;

        return obj;
    }

    /**
     * @brief Create complex test JSON with children
     */
    static QJsonObject createComplexTestJson() {
        QJsonObject root;
        root["type"] = "QWidget";

        QJsonObject properties;
        properties["windowTitle"] = "Test Window";
        properties["minimumWidth"] = 300;
        properties["minimumHeight"] = 200;
        root["properties"] = properties;

        QJsonArray children;

        // Add label child
        QJsonObject label;
        label["type"] = "QLabel";
        QJsonObject labelProps;
        labelProps["text"] = "Test Label";
        labelProps["alignment"] = "AlignCenter";
        label["properties"] = labelProps;
        children.append(label);

        // Add button child
        QJsonObject button;
        button["type"] = "QPushButton";
        QJsonObject buttonProps;
        buttonProps["text"] = "Test Button";
        buttonProps["enabled"] = true;
        button["properties"] = buttonProps;
        children.append(button);

        root["children"] = children;
        return root;
    }

    /**
     * @brief Verify widget properties
     */
    static bool verifyWidgetProperties(QWidget* widget,
                                       const QJsonObject& expected) {
        if (!widget || expected.isEmpty()) {
            return false;
        }

        auto properties = expected["properties"].toObject();
        for (auto it = properties.begin(); it != properties.end(); ++it) {
            QString property_name = it.key();
            QVariant expected_value = it.value().toVariant();
            QVariant actual_value =
                widget->property(property_name.toLocal8Bit());

            if (actual_value != expected_value) {
                qWarning() << "Property mismatch:" << property_name
                           << "expected:" << expected_value
                           << "actual:" << actual_value;
                return false;
            }
        }
        return true;
    }

    /**
     * @brief Count widgets of specific type in hierarchy
     */
    template <typename T>
    static int countWidgetsOfType(QWidget* root) {
        if (!root)
            return 0;

        int count = 0;
        if (qobject_cast<T*>(root)) {
            count++;
        }

        for (auto* child : root->findChildren<T*>()) {
            count++;
        }
        return count;
    }

    /**
     * @brief Create mock signal spy with timeout
     */
    static bool waitForSignal(QObject* sender, const char* signal,
                              int timeout_ms = 5000) {
        QSignalSpy spy(sender, signal);
        return spy.wait(timeout_ms);
    }

    /**
     * @brief Simulate user interaction delay
     */
    static void simulateUserDelay(int min_ms = 50, int max_ms = 200) {
        int delay = generateRandomInt(min_ms, max_ms);
        QTest::qWait(delay);
    }

    /**
     * @brief Generate test data set
     */
    static std::vector<QString> generateTestDataSet(int count = 100) {
        std::vector<QString> data;
        data.reserve(count);

        for (int i = 0; i < count; ++i) {
            data.push_back(
                QString("TestData_%1_%2").arg(i).arg(generateRandomString(5)));
        }
        return data;
    }

    /**
     * @brief Create stress test data
     */
    static std::vector<QJsonObject> generateStressTestData(int count = 1000) {
        std::vector<QJsonObject> data;
        data.reserve(count);

        QStringList widget_types = {"QLabel", "QPushButton", "QLineEdit",
                                    "QCheckBox"};

        for (int i = 0; i < count; ++i) {
            QJsonObject obj;
            obj["type"] = widget_types[i % widget_types.size()];

            QJsonObject properties;
            properties["text"] = QString("Stress Test %1").arg(i);
            properties["enabled"] = (i % 2 == 0);
            properties["visible"] = true;
            obj["properties"] = properties;

            data.push_back(obj);
        }
        return data;
    }
};

/**
 * @brief Performance measurement utilities
 */
class PerformanceUtils {
public:
    struct BenchmarkResult {
        qint64 total_time_ms;
        qint64 min_time_ms;
        qint64 max_time_ms;
        double avg_time_ms;
        int iterations;
    };

    /**
     * @brief Run benchmark with multiple iterations
     */
    template <typename Func>
    static BenchmarkResult benchmark(Func&& func, int iterations = 100) {
        std::vector<qint64> times;
        times.reserve(iterations);

        QElapsedTimer total_timer;
        total_timer.start();

        for (int i = 0; i < iterations; ++i) {
            QElapsedTimer iter_timer;
            iter_timer.start();
            func();
            times.push_back(iter_timer.elapsed());
        }

        qint64 total_time = total_timer.elapsed();
        qint64 min_time = *std::min_element(times.begin(), times.end());
        qint64 max_time = *std::max_element(times.begin(), times.end());
        double avg_time = (double)total_time / iterations;

        return {total_time, min_time, max_time, avg_time, iterations};
    }

    /**
     * @brief Memory usage measurement
     */
    static size_t getCurrentMemoryUsage() {
        // Platform-specific implementation would go here
        // For now, return a placeholder
        return 0;
    }

    /**
     * @brief CPU usage measurement
     */
    static double getCurrentCpuUsage() {
        // Platform-specific implementation would go here
        // For now, return a placeholder
        return 0.0;
    }
};

/**
 * @brief Mock object utilities
 */
class MockUtils {
public:
    /**
     * @brief Create mock widget with specified properties
     */
    static QWidget* createMockWidget(const QString& type = "QWidget") {
        if (type == "QLabel") {
            return new QLabel("Mock Label");
        } else if (type == "QPushButton") {
            return new QPushButton("Mock Button");
        } else if (type == "QLineEdit") {
            return new QLineEdit("Mock Text");
        } else {
            return new QWidget();
        }
    }

    /**
     * @brief Create mock JSON data
     */
    static QString createMockJsonData(const QString& widget_type = "QLabel") {
        QJsonObject obj = TestUtilities::createTestJsonObject(widget_type);
        QJsonDocument doc(obj);
        return doc.toJson(QJsonDocument::Compact);
    }

    /**
     * @brief Create invalid mock data for error testing
     */
    static QString createInvalidMockData() { return "{ invalid json data }"; }
};

/**
 * @brief Test assertion utilities
 */
class AssertUtils {
public:
    /**
     * @brief Assert widget is valid and initialized
     */
    static void assertWidgetValid(QWidget* widget,
                                  const QString& message = "") {
        QString full_message =
            message.isEmpty() ? "Widget should be valid" : message;
        QVERIFY2(widget != nullptr, full_message.toLocal8Bit());
        QVERIFY2(widget->isValid(),
                 (full_message + " and valid").toLocal8Bit());
    }

    /**
     * @brief Assert performance within bounds
     */
    static void assertPerformance(qint64 actual_ms, qint64 max_ms,
                                  const QString& operation = "") {
        QString message =
            QString("Performance check for %1: %2ms should be <= %3ms")
                .arg(operation.isEmpty() ? "operation" : operation)
                .arg(actual_ms)
                .arg(max_ms);
        QVERIFY2(actual_ms <= max_ms, message.toLocal8Bit());
    }

    /**
     * @brief Assert memory usage within bounds
     */
    static void assertMemoryUsage(size_t actual_bytes, size_t max_bytes,
                                  const QString& operation = "") {
        QString message =
            QString("Memory check for %1: %2 bytes should be <= %3 bytes")
                .arg(operation.isEmpty() ? "operation" : operation)
                .arg(actual_bytes)
                .arg(max_bytes);
        QVERIFY2(actual_bytes <= max_bytes, message.toLocal8Bit());
    }

    /**
     * @brief Assert no exceptions thrown
     */
    template <typename Func>
    static void assertNoExceptions(Func&& func, const QString& operation = "") {
        try {
            func();
        } catch (const std::exception& e) {
            QString message =
                QString("No exceptions expected for %1, but got: %2")
                    .arg(operation.isEmpty() ? "operation" : operation)
                    .arg(e.what());
            QFAIL(message.toLocal8Bit());
        } catch (...) {
            QString message =
                QString(
                    "No exceptions expected for %1, but got unknown exception")
                    .arg(operation.isEmpty() ? "operation" : operation);
            QFAIL(message.toLocal8Bit());
        }
    }
};

/**
 * @brief Test data generators for various scenarios
 */
class TestDataGenerator {
public:
    /**
     * @brief Generate component test data
     */
    struct ComponentTestData {
        QString type;
        QJsonObject properties;
        QStringList expected_signals;
        bool should_initialize;
    };

    static std::vector<ComponentTestData> generateComponentTestData() {
        return {{"Button",
                 {{"text", "Test Button"}, {"enabled", true}},
                 {"clicked", "pressed", "released"},
                 true},
                {"LineEdit",
                 {{"text", "Test Text"}, {"placeholder", "Enter text"}},
                 {"textChanged", "returnPressed", "editingFinished"},
                 true},
                {"Label",
                 {{"text", "Test Label"}, {"alignment", "AlignCenter"}},
                 {},
                 true},
                {"CheckBox",
                 {{"text", "Test CheckBox"}, {"checked", false}},
                 {"toggled", "stateChanged"},
                 true},
                {"ProgressBar",
                 {{"minimum", 0}, {"maximum", 100}, {"value", 50}},
                 {"valueChanged"},
                 true}};
    }

    /**
     * @brief Generate state management test data
     */
    struct StateTestData {
        QString key;
        QVariant value;
        QString computed_key;
        std::function<QVariant()> computed_func;
    };

    static std::vector<StateTestData> generateStateTestData() {
        std::vector<StateTestData> data;

        data.push_back(
            {"user.name", QString("John Doe"), "user.display", nullptr});
        data.push_back({"user.age", 30, "user.category", nullptr});
        data.push_back({"app.theme", QString("dark"), "app.style", nullptr});
        data.push_back({"ui.width", 800, "ui.aspect", nullptr});
        data.push_back({"ui.height", 600, "", nullptr});

        return data;
    }

    /**
     * @brief Generate command test data
     */
    struct CommandTestData {
        QString command_name;
        QJsonObject parameters;
        bool should_succeed;
        QString expected_error;
    };

    static std::vector<CommandTestData> generateCommandTestData() {
        return {{"set_property",
                 {{"property", "text"}, {"value", "Test Value"}},
                 true,
                 ""},
                {"update_state",
                 {{"key", "test.key"}, {"value", "Test State"}},
                 true,
                 ""},
                {"save_file",
                 {{"path", "/tmp/test.txt"}, {"content", "Test Content"}},
                 true,
                 ""},
                {"invalid_command",
                 {{"param", "value"}},
                 false,
                 "Command not found"},
                {"set_property",
                 {{"missing_property", "value"}},
                 false,
                 "Missing required parameter"}};
    }

    /**
     * @brief Generate performance test scenarios
     */
    struct PerformanceTestData {
        QString scenario_name;
        int iterations;
        qint64 max_time_ms;
        size_t max_memory_bytes;
    };

    static std::vector<PerformanceTestData> generatePerformanceTestData() {
        return {{"Component Creation", 1000, 2000, 10 * 1024 * 1024},
                {"State Updates", 5000, 1000, 5 * 1024 * 1024},
                {"Command Execution", 2000, 3000, 20 * 1024 * 1024},
                {"JSON Parsing", 500, 1500, 15 * 1024 * 1024},
                {"Cache Operations", 10000, 500, 50 * 1024 * 1024}};
    }

    /**
     * @brief Generate error test scenarios
     */
    struct ErrorTestData {
        QString scenario_name;
        std::function<void()> error_function;
        QString expected_exception_type;
        bool should_recover;
    };

    static std::vector<ErrorTestData> generateErrorTestData() {
        std::vector<ErrorTestData> data;

        data.push_back({"Null Pointer Access",
                        []() {
                            QWidget* null_widget = nullptr;
                            null_widget->show();  // Should cause error
                        },
                        "NullPointerException", false});

        data.push_back({"Invalid JSON",
                        []() { QJsonDocument::fromJson("{ invalid json }"); },
                        "ParseError", true});

        data.push_back({"File Not Found",
                        []() {
                            QFile file("non_existent_file.txt");
                            if (!file.open(QIODevice::ReadOnly)) {
                                throw std::runtime_error("File not found");
                            }
                        },
                        "FileNotFound", true});

        return data;
    }
};

}  // namespace DeclarativeUI::Testing
