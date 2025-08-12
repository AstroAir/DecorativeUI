#pragma once

#include <QCoreApplication>
#include <QDir>
#include <QElapsedTimer>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTest>
#include <QTextStream>
#include <functional>
#include <memory>
#include <vector>

namespace DeclarativeUI::Testing {

/**
 * @brief Test execution result
 */
struct TestResult {
    QString test_name;
    bool passed;
    qint64 execution_time_ms;
    QString error_message;
    QJsonObject details;
};

/**
 * @brief Test suite result
 */
struct TestSuiteResult {
    QString suite_name;
    std::vector<TestResult> test_results;
    qint64 total_time_ms;
    int passed_count;
    int failed_count;
    double pass_rate;
};

/**
 * @brief Comprehensive test runner with reporting capabilities
 */
class TestRunner {
public:
    /**
     * @brief Test execution configuration
     */
    struct Config {
        bool verbose = false;
        bool generate_report = true;
        QString report_format = "json";  // json, xml, html
        QString output_directory = "test_reports";
        int timeout_ms = 30000;  // 30 seconds per test
        bool stop_on_failure = false;
        bool measure_performance = true;
        bool measure_memory = false;
    };

    explicit TestRunner(const Config& config = Config{}) : config_(config) {
        setupOutputDirectory();
    }

    /**
     * @brief Register a test function
     */
    template <typename TestClass>
    void registerTest(const QString& test_name,
                      std::function<int()> test_func) {
        TestInfo info;
        info.name = test_name;
        info.function = test_func;
        info.category = "unit";  // Default category
        registered_tests_.push_back(info);
    }

    /**
     * @brief Register a test class
     */
    template <typename TestClass>
    void registerTestClass(const QString& test_name,
                           const QString& category = "unit") {
        TestInfo info;
        info.name = test_name;
        info.category = category;
        info.function = []() {
            TestClass test;
            return QTest::qExec(&test);
        };
        registered_tests_.push_back(info);
    }

    /**
     * @brief Run all registered tests
     */
    TestSuiteResult runAllTests() {
        TestSuiteResult suite_result;
        suite_result.suite_name = "DeclarativeUI Test Suite";

        QElapsedTimer total_timer;
        total_timer.start();

        if (config_.verbose) {
            qDebug() << "Starting test execution...";
            qDebug() << "Total tests to run:" << registered_tests_.size();
        }

        for (const auto& test_info : registered_tests_) {
            TestResult result = runSingleTest(test_info);
            suite_result.test_results.push_back(result);

            if (result.passed) {
                suite_result.passed_count++;
            } else {
                suite_result.failed_count++;
                if (config_.stop_on_failure) {
                    qWarning() << "Stopping execution due to test failure:"
                               << test_info.name;
                    break;
                }
            }

            if (config_.verbose) {
                QString status = result.passed ? "PASS" : "FAIL";
                qDebug() << QString("[%1] %2 (%3ms)")
                                .arg(status)
                                .arg(test_info.name)
                                .arg(result.execution_time_ms);
            }
        }

        suite_result.total_time_ms = total_timer.elapsed();
        suite_result.pass_rate =
            (double)suite_result.passed_count /
            (suite_result.passed_count + suite_result.failed_count) * 100.0;

        if (config_.generate_report) {
            generateReport(suite_result);
        }

        printSummary(suite_result);
        return suite_result;
    }

    /**
     * @brief Run tests by category
     */
    TestSuiteResult runTestsByCategory(const QString& category) {
        TestSuiteResult suite_result;
        suite_result.suite_name =
            QString("DeclarativeUI %1 Tests").arg(category);

        QElapsedTimer total_timer;
        total_timer.start();

        for (const auto& test_info : registered_tests_) {
            if (test_info.category == category) {
                TestResult result = runSingleTest(test_info);
                suite_result.test_results.push_back(result);

                if (result.passed) {
                    suite_result.passed_count++;
                } else {
                    suite_result.failed_count++;
                }
            }
        }

        suite_result.total_time_ms = total_timer.elapsed();
        suite_result.pass_rate =
            (double)suite_result.passed_count /
            (suite_result.passed_count + suite_result.failed_count) * 100.0;

        if (config_.generate_report) {
            generateReport(suite_result);
        }

        return suite_result;
    }

    /**
     * @brief Run specific test by name
     */
    TestResult runTest(const QString& test_name) {
        for (const auto& test_info : registered_tests_) {
            if (test_info.name == test_name) {
                return runSingleTest(test_info);
            }
        }

        TestResult result;
        result.test_name = test_name;
        result.passed = false;
        result.error_message = "Test not found";
        return result;
    }

    /**
     * @brief Get list of registered tests
     */
    QStringList getRegisteredTests() const {
        QStringList tests;
        for (const auto& test_info : registered_tests_) {
            tests.append(test_info.name);
        }
        return tests;
    }

    /**
     * @brief Get tests by category
     */
    QStringList getTestsByCategory(const QString& category) const {
        QStringList tests;
        for (const auto& test_info : registered_tests_) {
            if (test_info.category == category) {
                tests.append(test_info.name);
            }
        }
        return tests;
    }

private:
    struct TestInfo {
        QString name;
        QString category;
        std::function<int()> function;
    };

    Config config_;
    std::vector<TestInfo> registered_tests_;

    void setupOutputDirectory() {
        if (config_.generate_report) {
            QDir dir;
            if (!dir.exists(config_.output_directory)) {
                dir.mkpath(config_.output_directory);
            }
        }
    }

    TestResult runSingleTest(const TestInfo& test_info) {
        TestResult result;
        result.test_name = test_info.name;

        QElapsedTimer timer;
        timer.start();

        try {
            int exit_code = test_info.function();
            result.passed = (exit_code == 0);
            if (exit_code != 0) {
                result.error_message =
                    QString("Test failed with exit code: %1").arg(exit_code);
            }
        } catch (const std::exception& e) {
            result.passed = false;
            result.error_message = QString("Exception: %1").arg(e.what());
        } catch (...) {
            result.passed = false;
            result.error_message = "Unknown exception occurred";
        }

        result.execution_time_ms = timer.elapsed();

        // Add performance details if enabled
        if (config_.measure_performance) {
            result.details["execution_time_ms"] = result.execution_time_ms;
            result.details["category"] = test_info.category;
        }

        return result;
    }

    void generateReport(const TestSuiteResult& suite_result) {
        if (config_.report_format == "json") {
            generateJsonReport(suite_result);
        } else if (config_.report_format == "xml") {
            generateXmlReport(suite_result);
        } else if (config_.report_format == "html") {
            generateHtmlReport(suite_result);
        }
    }

    void generateJsonReport(const TestSuiteResult& suite_result) {
        QJsonObject report;
        report["suite_name"] = suite_result.suite_name;
        report["total_time_ms"] = suite_result.total_time_ms;
        report["passed_count"] = suite_result.passed_count;
        report["failed_count"] = suite_result.failed_count;
        report["pass_rate"] = suite_result.pass_rate;
        report["timestamp"] =
            QDateTime::currentDateTime().toString(Qt::ISODate);

        QJsonArray tests;
        for (const auto& test_result : suite_result.test_results) {
            QJsonObject test_obj;
            test_obj["name"] = test_result.test_name;
            test_obj["passed"] = test_result.passed;
            test_obj["execution_time_ms"] = test_result.execution_time_ms;
            test_obj["error_message"] = test_result.error_message;
            test_obj["details"] = test_result.details;
            tests.append(test_obj);
        }
        report["tests"] = tests;

        QString filename =
            QString("%1/test_report_%2.json")
                .arg(config_.output_directory)
                .arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss"));

        QFile file(filename);
        if (file.open(QIODevice::WriteOnly)) {
            QJsonDocument doc(report);
            file.write(doc.toJson());
            file.close();

            if (config_.verbose) {
                qDebug() << "JSON report generated:" << filename;
            }
        }
    }

    void generateXmlReport(const TestSuiteResult& suite_result) {
        // XML report generation would go here
        // For now, just create a placeholder
        QString filename =
            QString("%1/test_report_%2.xml")
                .arg(config_.output_directory)
                .arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss"));

        QFile file(filename);
        if (file.open(QIODevice::WriteOnly)) {
            QTextStream stream(&file);
            stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
            stream << "<testsuite name=\"" << suite_result.suite_name
                   << "\">\n";
            // Add test cases here
            stream << "</testsuite>\n";
            file.close();
        }
    }

    void generateHtmlReport(const TestSuiteResult& suite_result) {
        // HTML report generation would go here
        QString filename =
            QString("%1/test_report_%2.html")
                .arg(config_.output_directory)
                .arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss"));

        QFile file(filename);
        if (file.open(QIODevice::WriteOnly)) {
            QTextStream stream(&file);
            stream << "<!DOCTYPE html>\n<html>\n<head>\n";
            stream << "<title>Test Report - " << suite_result.suite_name
                   << "</title>\n";
            stream << "</head>\n<body>\n";
            stream << "<h1>" << suite_result.suite_name << "</h1>\n";
            stream << "<p>Pass Rate: " << suite_result.pass_rate << "%</p>\n";
            // Add test results here
            stream << "</body>\n</html>\n";
            file.close();
        }
    }

    void printSummary(const TestSuiteResult& suite_result) {
        qDebug() << "=== Test Suite Summary ===";
        qDebug() << "Suite:" << suite_result.suite_name;
        qDebug() << "Total Tests:"
                 << (suite_result.passed_count + suite_result.failed_count);
        qDebug() << "Passed:" << suite_result.passed_count;
        qDebug() << "Failed:" << suite_result.failed_count;
        qDebug() << "Pass Rate:"
                 << QString::number(suite_result.pass_rate, 'f', 2) << "%";
        qDebug() << "Total Time:" << suite_result.total_time_ms << "ms";
        qDebug() << "==========================";
    }
};

}  // namespace DeclarativeUI::Testing
