#include <QApplication>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSignalSpy>
#include <QTest>
#include <QTimer>
#include <QWidget>
#include <memory>
#include <type_traits>

#include "../Binding/PropertyBindingTemplate.hpp"
#include "../Binding/StateManager.hpp"
#include "../Exceptions/UIExceptions.hpp"

using namespace DeclarativeUI::Binding;
using namespace DeclarativeUI::Exceptions;

// Test types for concept validation
struct BindableTestType {
    int value = 0;
    BindableTestType() = default;
    BindableTestType(int v) : value(v) {}
    BindableTestType(const BindableTestType&) = default;
    BindableTestType(BindableTestType&&) = default;
    BindableTestType& operator=(const BindableTestType&) = default;
    BindableTestType& operator=(BindableTestType&&) = default;

    bool operator==(const BindableTestType& other) const {
        return value == other.value;
    }
    bool operator!=(const BindableTestType& other) const {
        return !(*this == other);
    }
};

// Make BindableTestType work with QVariant
Q_DECLARE_METATYPE(BindableTestType)

struct NonBindableTestType {
    // Intentionally missing copy/move constructors to fail BindableType concept
    NonBindableTestType() = default;
    NonBindableTestType(const NonBindableTestType&) = delete;
    NonBindableTestType(NonBindableTestType&&) = delete;
};

// Use the actual ReactiveProperty from the library
using DeclarativeUI::Binding::ReactiveProperty;

class PropertyBindingTemplateTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        // Initialize Qt application if not already done
        if (!QApplication::instance()) {
            int argc = 0;
            char* argv[] = {nullptr};
            new QApplication(argc, argv);
        }

        // Register custom types
        qRegisterMetaType<BindableTestType>();
    }

    void cleanupTestCase() {
        // Clean up any global state
    }

    void init() {
        // Set up for each test
        test_widget_ = std::make_unique<QLabel>();
        test_widget_->setProperty("text", "Initial Text");
    }

    void cleanup() {
        // Clean up after each test
        test_widget_.reset();
    }

    // **Concept Tests**
    void testBindableTypeConcept() {
        // Test that basic types satisfy BindableType concept
        static_assert(BindableType<QString>);
        static_assert(BindableType<int>);
        static_assert(BindableType<double>);
        static_assert(BindableType<bool>);
        static_assert(BindableType<BindableTestType>);

        // Test that non-bindable types fail the concept
        static_assert(!BindableType<NonBindableTestType>);

        QVERIFY(true);  // If compilation succeeds, concepts work
    }

    void testComputeFunctionConcept() {
        // Test lambda functions
        auto string_compute = []() -> QString { return "computed"; };
        auto int_compute = []() -> int { return 42; };

        static_assert(ComputeFunction<decltype(string_compute)>);
        static_assert(ComputeFunction<decltype(int_compute)>);

        // Test function objects
        struct StringComputer {
            QString operator()() const { return "computed"; }
        };
        static_assert(ComputeFunction<StringComputer>);

        QVERIFY(true);  // If compilation succeeds, concepts work
    }

    // **Binding Direction Tests**
    void testBindingDirectionEnum() {
        PropertyBinding<QString> one_way_binding;
        QCOMPARE(one_way_binding.getDirection(), BindingDirection::OneWay);

        // Test enum values
        QVERIFY(BindingDirection::OneWay != BindingDirection::TwoWay);
        QVERIFY(BindingDirection::TwoWay != BindingDirection::OneTime);
        QVERIFY(BindingDirection::OneTime != BindingDirection::OneWay);
    }

    void testUpdateModeEnum() {
        PropertyBinding<QString> binding;
        QCOMPARE(binding.getUpdateMode(), UpdateMode::Immediate);

        binding.setUpdateMode(UpdateMode::Deferred);
        QCOMPARE(binding.getUpdateMode(), UpdateMode::Deferred);

        binding.setUpdateMode(UpdateMode::Manual);
        QCOMPARE(binding.getUpdateMode(), UpdateMode::Manual);
    }

    // **Template Instantiation Tests**
    void testTemplateInstantiationSameTypes() {
        auto source = std::make_shared<ReactiveProperty<QString>>("Test");
        PropertyBinding<QString, QString> binding(
            source, test_widget_.get(), "text", BindingDirection::OneWay);

        QVERIFY(binding.isValid());
        QCOMPARE(test_widget_->property("text").toString(), QString("Test"));
    }

    void testTemplateInstantiationDifferentTypes() {
        auto source = std::make_shared<ReactiveProperty<int>>(42);
        PropertyBinding<int, QString> binding(source, test_widget_.get(),
                                              "text", BindingDirection::OneWay);

        QVERIFY(binding.isValid());
        // Should convert int to QString
        QString text = test_widget_->property("text").toString();
        QVERIFY(text.contains("42"));
    }

    void testTemplateInstantiationCustomTypes() {
        auto source = std::make_shared<ReactiveProperty<BindableTestType>>(
            BindableTestType(123));
        PropertyBinding<BindableTestType, QString> binding(
            source, test_widget_.get(), "text", BindingDirection::OneWay);

        QVERIFY(binding.isValid());
        // Custom type should be convertible through QVariant
    }

    // **Converter Function Tests**
    void testCustomConverter() {
        auto source = std::make_shared<ReactiveProperty<int>>(42);

        auto converter = [](const int& value) -> QString {
            return QString("Value: %1").arg(value);
        };

        PropertyBinding<int, QString> binding(source, test_widget_.get(),
                                              "text", converter,
                                              BindingDirection::OneWay);

        QCOMPARE(test_widget_->property("text").toString(),
                 QString("Value: 42"));

        source->set(100);
        QTest::qWait(10);
        QCOMPARE(test_widget_->property("text").toString(),
                 QString("Value: 100"));
    }

    void testComplexConverter() {
        auto source = std::make_shared<ReactiveProperty<BindableTestType>>(
            BindableTestType(42));

        auto converter = [](const BindableTestType& value) -> QString {
            return QString("Custom: %1").arg(value.value);
        };

        PropertyBinding<BindableTestType, QString> binding(
            source, test_widget_.get(), "text", converter,
            BindingDirection::OneWay);

        QCOMPARE(test_widget_->property("text").toString(),
                 QString("Custom: 42"));
    }

    // **Compute Function Tests**
    void testComputeFunction() {
        int counter = 0;
        auto compute_func = [&counter]() -> QString {
            return QString("Count: %1").arg(++counter);
        };

        PropertyBinding<QString> binding(compute_func, test_widget_.get(),
                                         "text", UpdateMode::Immediate);

        QVERIFY(binding.isValid());
        QCOMPARE(test_widget_->property("text").toString(),
                 QString("Count: 1"));

        // Manual update should increment counter
        binding.update();
        QCOMPARE(test_widget_->property("text").toString(),
                 QString("Count: 2"));
    }

    void testComputeFunctionWithCapture() {
        QString prefix = "Prefix";
        int value = 42;

        auto compute_func = [prefix, &value]() -> QString {
            return QString("%1: %2").arg(prefix).arg(value);
        };

        PropertyBinding<QString> binding(compute_func, test_widget_.get(),
                                         "text", UpdateMode::Manual);

        QCOMPARE(test_widget_->property("text").toString(),
                 QString("Prefix: 42"));

        value = 100;
        binding.update();
        QCOMPARE(test_widget_->property("text").toString(),
                 QString("Prefix: 100"));
    }

    // **Error Handling Tests**
    void testConverterException() {
        auto source = std::make_shared<ReactiveProperty<int>>(42);

        auto throwing_converter = [](const int& value) -> QString {
            if (value > 50) {
                throw std::runtime_error("Value too large");
            }
            return QString::number(value);
        };

        PropertyBinding<int, QString> binding(source, test_widget_.get(),
                                              "text", throwing_converter,
                                              BindingDirection::OneWay);

        QString last_error;
        binding.setErrorHandler(
            [&last_error](const QString& error) { last_error = error; });

        // This should trigger the exception
        source->set(100);
        QTest::qWait(10);

        QVERIFY(!last_error.isEmpty());
        QVERIFY(last_error.contains("Exception"));
    }

    void testValidatorWithConverter() {
        auto source = std::make_shared<ReactiveProperty<int>>(10);

        auto converter = [](const int& value) -> QString {
            return QString("Number: %1").arg(value);
        };

        PropertyBinding<int, QString> binding(source, test_widget_.get(),
                                              "text", converter,
                                              BindingDirection::OneWay);

        // Validator that rejects strings longer than 10 characters
        binding.setValidator(
            [](const QString& value) { return value.length() <= 10; });

        // Short number should work
        source->set(5);
        QTest::qWait(10);
        QCOMPARE(test_widget_->property("text").toString(),
                 QString("Number: 5"));

        // Long number should be rejected
        QString before_long = test_widget_->property("text").toString();
        source->set(123456789);
        QTest::qWait(10);
        QCOMPARE(test_widget_->property("text").toString(), before_long);
    }

    // **Performance Tests**
    void testBindingPerformance() {
        auto source = std::make_shared<ReactiveProperty<int>>(0);
        PropertyBinding<int, QString> binding(source, test_widget_.get(),
                                              "text", BindingDirection::OneWay);

        // Measure performance of many updates
        QElapsedTimer timer;
        timer.start();

        for (int i = 0; i < 1000; ++i) {
            source->set(i);
            QTest::qWait(1);
        }

        qint64 elapsed = timer.elapsed();
        qDebug() << "1000 binding updates took" << elapsed << "ms";

        QVERIFY(binding.getUpdateCount() >= 1000);
        QVERIFY(elapsed < 10000);  // Should complete in reasonable time
    }

private:
    std::unique_ptr<QLabel> test_widget_;
};

QTEST_MAIN(PropertyBindingTemplateTest)
#include "test_property_binding_template.moc"
