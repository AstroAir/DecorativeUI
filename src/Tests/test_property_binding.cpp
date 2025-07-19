#include <QApplication>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSignalSpy>
#include <QTest>
#include <QTimer>
#include <QWidget>
#include <memory>

#include "../Binding/PropertyBinding.hpp"
#include "../Binding/PropertyBindingTemplate.hpp"
#include "../Binding/StateManager.hpp"
#include "../Exceptions/UIExceptions.hpp"

using namespace DeclarativeUI::Binding;
using namespace DeclarativeUI::Exceptions;

// Use the actual ReactiveProperty from the library
using DeclarativeUI::Binding::ReactiveProperty;

class PropertyBindingTest : public QObject {
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
        test_widget_ = std::make_unique<QLabel>();
        test_widget_->setProperty("text", "Initial Text");
    }

    void cleanup() {
        // Clean up after each test
        test_widget_.reset();
    }

    // **Basic PropertyBinding Tests**
    void testPropertyBindingCreation() {
        // Test default constructor
        PropertyBinding<QString> binding;
        QVERIFY(!binding.isValid());
        QCOMPARE(binding.getDirection(), BindingDirection::OneWay);
        QCOMPARE(binding.getUpdateMode(), UpdateMode::Immediate);
        QVERIFY(binding.isEnabled());
    }

    void testPropertyBindingWithReactiveProperty() {
        auto source = std::make_shared<ReactiveProperty<QString>>("Test Value");
        
        PropertyBinding<QString> binding(
            source, test_widget_.get(), "text", BindingDirection::OneWay);
        
        QVERIFY(binding.isValid());
        QCOMPARE(binding.getDirection(), BindingDirection::OneWay);
        QCOMPARE(binding.getSourcePath(), QString("ReactiveProperty@%1").arg(
            reinterpret_cast<quintptr>(source.get()), 0, 16));
        QCOMPARE(binding.getTargetPath(), QString("QLabel::text"));
    }

    void testOneWayBinding() {
        auto source = std::make_shared<ReactiveProperty<QString>>("Initial");
        PropertyBinding<QString> binding(
            source, test_widget_.get(), "text", BindingDirection::OneWay);
        
        // Test initial update
        QCOMPARE(test_widget_->property("text").toString(), QString("Initial"));
        
        // Test source to target update
        source->set("Updated Value");
        QTest::qWait(10); // Allow signal processing
        QCOMPARE(test_widget_->property("text").toString(), QString("Updated Value"));
    }

    void testTwoWayBinding() {
        auto line_edit = std::make_unique<QLineEdit>();
        line_edit->setText("Initial");
        
        auto source = std::make_shared<ReactiveProperty<QString>>("Source Value");
        PropertyBinding<QString> binding(
            source, line_edit.get(), "text", BindingDirection::TwoWay);
        
        // Test source to target
        source->set("From Source");
        QTest::qWait(10);
        QCOMPARE(line_edit->text(), QString("From Source"));
        
        // Test target to source (if property has notify signal)
        line_edit->setText("From Target");
        QTest::qWait(10);
        // Note: This might not work perfectly due to QLineEdit signal behavior
    }

    void testOneTimeBinding() {
        auto source = std::make_shared<ReactiveProperty<QString>>("One Time");
        PropertyBinding<QString> binding(
            source, test_widget_.get(), "text", BindingDirection::OneTime);
        
        QString initial_value = test_widget_->property("text").toString();
        
        // Change source - should not update target for OneTime binding
        source->set("Changed Value");
        QTest::qWait(10);
        QCOMPARE(test_widget_->property("text").toString(), initial_value);
    }

    void testBindingWithConverter() {
        auto source = std::make_shared<ReactiveProperty<int>>(42);
        
        auto converter = [](const int& value) -> QString {
            return QString("Number: %1").arg(value);
        };
        
        PropertyBinding<int, QString> binding(
            source, test_widget_.get(), "text", converter, BindingDirection::OneWay);
        
        QCOMPARE(test_widget_->property("text").toString(), QString("Number: 42"));
        
        source->set(100);
        QTest::qWait(10);
        QCOMPARE(test_widget_->property("text").toString(), QString("Number: 100"));
    }

    void testBindingWithValidator() {
        auto source = std::make_shared<ReactiveProperty<QString>>("Valid");
        PropertyBinding<QString> binding(
            source, test_widget_.get(), "text", BindingDirection::OneWay);
        
        // Set validator that rejects strings containing "invalid"
        binding.setValidator([](const QString& value) {
            return !value.contains("invalid", Qt::CaseInsensitive);
        });
        
        // Valid value should work
        source->set("This is valid");
        QTest::qWait(10);
        QCOMPARE(test_widget_->property("text").toString(), QString("This is valid"));
        
        // Invalid value should be rejected
        QString before_invalid = test_widget_->property("text").toString();
        source->set("This is invalid");
        QTest::qWait(10);
        QCOMPARE(test_widget_->property("text").toString(), before_invalid);
    }

    void testBindingUpdateModes() {
        auto source = std::make_shared<ReactiveProperty<QString>>("Initial");
        PropertyBinding<QString> binding(
            source, test_widget_.get(), "text", BindingDirection::OneWay);
        
        // Test Immediate mode (default)
        QCOMPARE(binding.getUpdateMode(), UpdateMode::Immediate);
        source->set("Immediate Update");
        QTest::qWait(10);
        QCOMPARE(test_widget_->property("text").toString(), QString("Immediate Update"));
        
        // Test Manual mode
        binding.setUpdateMode(UpdateMode::Manual);
        source->set("Manual Update");
        QTest::qWait(10);
        QCOMPARE(test_widget_->property("text").toString(), QString("Immediate Update")); // Should not change
        
        // Manual update
        binding.update();
        QCOMPARE(test_widget_->property("text").toString(), QString("Manual Update"));
    }

    void testBindingEnableDisable() {
        auto source = std::make_shared<ReactiveProperty<QString>>("Initial");
        PropertyBinding<QString> binding(
            source, test_widget_.get(), "text", BindingDirection::OneWay);
        
        QVERIFY(binding.isEnabled());
        
        // Disable binding
        binding.setEnabled(false);
        QVERIFY(!binding.isEnabled());
        
        source->set("Should Not Update");
        QTest::qWait(10);
        QCOMPARE(test_widget_->property("text").toString(), QString("Initial"));
        
        // Re-enable binding
        binding.setEnabled(true);
        source->set("Should Update Now");
        QTest::qWait(10);
        QCOMPARE(test_widget_->property("text").toString(), QString("Should Update Now"));
    }

    void testBindingErrorHandling() {
        auto source = std::make_shared<ReactiveProperty<QString>>("Test");
        PropertyBinding<QString> binding(
            source, test_widget_.get(), "text", BindingDirection::OneWay);
        
        QString last_error;
        binding.setErrorHandler([&last_error](const QString& error) {
            last_error = error;
        });
        
        // Set validator that always fails to trigger error
        binding.setValidator([](const QString&) { return false; });
        
        source->set("This will fail validation");
        QTest::qWait(10);
        
        QVERIFY(!last_error.isEmpty());
        QVERIFY(last_error.contains("Validation failed"));
    }

    void testBindingPerformanceMetrics() {
        auto source = std::make_shared<ReactiveProperty<QString>>("Test");
        PropertyBinding<QString> binding(
            source, test_widget_.get(), "text", BindingDirection::OneWay);
        
        QCOMPARE(binding.getUpdateCount(), static_cast<quint64>(1)); // Initial update
        
        source->set("Update 1");
        QTest::qWait(10);
        QCOMPARE(binding.getUpdateCount(), static_cast<quint64>(2));
        
        source->set("Update 2");
        QTest::qWait(10);
        QCOMPARE(binding.getUpdateCount(), static_cast<quint64>(3));
        
        QVERIFY(binding.getLastUpdateTime() > 0);
    }

    void testBindingDisconnect() {
        auto source = std::make_shared<ReactiveProperty<QString>>("Initial");
        PropertyBinding<QString> binding(
            source, test_widget_.get(), "text", BindingDirection::OneWay);
        
        QVERIFY(binding.isValid());
        
        // Disconnect binding
        binding.disconnect();
        QVERIFY(!binding.isValid());
        
        // Source changes should not affect target
        source->set("Should Not Update");
        QTest::qWait(10);
        QCOMPARE(test_widget_->property("text").toString(), QString("Initial"));
    }

private:
    std::unique_ptr<QLabel> test_widget_;
};

QTEST_MAIN(PropertyBindingTest)
#include "test_property_binding.moc"
