#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSignalSpy>
#include <QTest>
#include <QVBoxLayout>
#include <QWidget>
#include <memory>

#include "../Core/DeclarativeBuilder.hpp"
#include "../Core/UIElement.hpp"
#include "../Exceptions/UIExceptions.hpp"

using namespace DeclarativeUI::Core;
using namespace DeclarativeUI::Exceptions;

// Test UIElement implementation for testing
class TestUIElement : public UIElement {
    Q_OBJECT

public:
    explicit TestUIElement(QObject* parent = nullptr) : UIElement(parent) {}

    void initialize() override {
        if (!getWidget()) {
            auto widget = std::make_unique<QLabel>();
            widget->setText("Test Element");
            setWidget(widget.release());
            applyStoredProperties();
        }
    }
};

class CoreTest : public QObject {
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

    // **UIElement Tests**
    void testUIElementCreation() {
        auto element = std::make_unique<TestUIElement>();
        
        QVERIFY(element->getWidget() == nullptr); // Not initialized yet
        
        element->initialize();
        
        QVERIFY(element->getWidget() != nullptr);
        auto* label = qobject_cast<QLabel*>(element->getWidget());
        QVERIFY(label != nullptr);
        QCOMPARE(label->text(), QString("Test Element"));
    }

    void testUIElementPropertySetting() {
        auto element = std::make_unique<TestUIElement>();
        
        // Set properties before initialization
        element->setProperty("text", QString("Custom Text"))
               .setProperty("toolTip", QString("Custom Tooltip"))
               .setProperty("enabled", false);
        
        element->initialize();
        
        auto* widget = element->getWidget();
        QVERIFY(widget != nullptr);
        
        // Properties should be applied during initialization
        QCOMPARE(widget->property("text").toString(), QString("Custom Text"));
        QCOMPARE(widget->property("toolTip").toString(), QString("Custom Tooltip"));
        QVERIFY(!widget->isEnabled());
    }

    void testUIElementEventHandling() {
        auto element = std::make_unique<TestUIElement>();
        
        bool event_triggered = false;
        element->onEvent("test_event", [&event_triggered]() { event_triggered = true; });
        
        element->initialize();
        
        // Verify event handler was stored
        auto handlers = element->getEventHandlers();
        QVERIFY(handlers.contains("test_event"));
        
        // Trigger the event handler manually
        handlers["test_event"]();
        QVERIFY(event_triggered);
    }

    void testUIElementPropertyBinding() {
        auto element = std::make_unique<TestUIElement>();
        
        int counter = 0;
        element->bindProperty("text", [&counter]() -> PropertyValue {
            return PropertyValue(QString("Count: %1").arg(++counter));
        });
        
        element->initialize();
        
        auto* widget = element->getWidget();
        QVERIFY(widget != nullptr);
        
        // Initial binding should be applied
        QCOMPARE(widget->property("text").toString(), QString("Count: 1"));
        
        // Refresh should update bound properties
        element->refresh();
        QCOMPARE(widget->property("text").toString(), QString("Count: 2"));
    }

    void testUIElementLifecycle() {
        auto element = std::make_unique<TestUIElement>();
        
        // Test initialization
        element->initialize();
        QVERIFY(element->getWidget() != nullptr);
        
        // Test refresh
        element->refresh(); // Should not crash
        
        // Test invalidate
        element->invalidate(); // Should not crash
        
        // Test cleanup
        element->cleanup(); // Should not crash
    }

    // **DeclarativeBuilder Tests**
    void testDeclarativeBuilderBasicCreation() {
        auto widget = create<QLabel>()
            .property("text", QString("Hello World"))
            .property("toolTip", QString("Test tooltip"))
            .build();
        
        QVERIFY(widget != nullptr);
        QCOMPARE(widget->text(), QString("Hello World"));
        QCOMPARE(widget->toolTip(), QString("Test tooltip"));
    }

    void testDeclarativeBuilderWithLayout() {
        auto widget = create<QWidget>()
            .layout<QVBoxLayout>()
            .property("windowTitle", QString("Test Window"))
            .build();
        
        QVERIFY(widget != nullptr);
        QCOMPARE(widget->windowTitle(), QString("Test Window"));
        QVERIFY(widget->layout() != nullptr);
        
        auto* layout = qobject_cast<QVBoxLayout*>(widget->layout());
        QVERIFY(layout != nullptr);
    }

    void testDeclarativeBuilderWithChildren() {
        auto widget = create<QWidget>()
            .layout<QVBoxLayout>()
            .child<QLabel>([](auto& label) {
                label.property("text", QString("Child Label"));
            })
            .child<QPushButton>([](auto& button) {
                button.property("text", QString("Child Button"));
            })
            .build();
        
        QVERIFY(widget != nullptr);
        QVERIFY(widget->layout() != nullptr);
        
        // Check that children were added to layout
        auto* layout = widget->layout();
        QCOMPARE(layout->count(), 2);
        
        auto* label = qobject_cast<QLabel*>(layout->itemAt(0)->widget());
        auto* button = qobject_cast<QPushButton*>(layout->itemAt(1)->widget());
        
        QVERIFY(label != nullptr);
        QVERIFY(button != nullptr);
        QCOMPARE(label->text(), QString("Child Label"));
        QCOMPARE(button->text(), QString("Child Button"));
    }

    void testDeclarativeBuilderEventHandling() {
        bool clicked = false;
        
        auto widget = create<QPushButton>()
            .property("text", QString("Click Me"))
            .on("clicked", [&clicked]() { clicked = true; })
            .build();
        
        QVERIFY(widget != nullptr);
        QCOMPARE(widget->text(), QString("Click Me"));
        
        // Simulate click
        widget->click();
        QVERIFY(clicked);
    }

    void testDeclarativeBuilderPropertyBinding() {
        int counter = 0;
        
        auto widget = create<QLabel>()
            .bind("text", [&counter]() -> PropertyValue {
                return PropertyValue(QString("Count: %1").arg(++counter));
            })
            .build();
        
        QVERIFY(widget != nullptr);
        QCOMPARE(widget->text(), QString("Count: 1"));
    }

    void testDeclarativeBuilderComplexLayout() {
        auto main_widget = create<QWidget>()
            .property("windowTitle", QString("Complex Layout"))
            .layout<QVBoxLayout>()
            .child<QLabel>([](auto& label) {
                label.property("text", QString("Header Label"));
            })
            .child<QWidget>([](DeclarativeBuilder<QWidget>& container) {
                container.template layout<QHBoxLayout>()
                    .template child<QLineEdit>([](DeclarativeBuilder<QLineEdit>& edit) {
                        edit.property("placeholderText", QString("Enter text"));
                    })
                    .template child<QPushButton>([](DeclarativeBuilder<QPushButton>& button) {
                        button.property("text", QString("Submit"));
                    });
            })
            .child<QLabel>([](auto& label) {
                label.property("text", QString("Footer Label"));
            })
            .build();
        
        QVERIFY(main_widget != nullptr);
        QCOMPARE(main_widget->windowTitle(), QString("Complex Layout"));
        
        auto* main_layout = qobject_cast<QVBoxLayout*>(main_widget->layout());
        QVERIFY(main_layout != nullptr);
        QCOMPARE(main_layout->count(), 3);
        
        // Check header
        auto* header = qobject_cast<QLabel*>(main_layout->itemAt(0)->widget());
        QVERIFY(header != nullptr);
        QCOMPARE(header->text(), QString("Header Label"));
        
        // Check middle container
        auto* container = qobject_cast<QWidget*>(main_layout->itemAt(1)->widget());
        QVERIFY(container != nullptr);
        
        auto* h_layout = qobject_cast<QHBoxLayout*>(container->layout());
        QVERIFY(h_layout != nullptr);
        QCOMPARE(h_layout->count(), 2);
        
        auto* line_edit = qobject_cast<QLineEdit*>(h_layout->itemAt(0)->widget());
        auto* button = qobject_cast<QPushButton*>(h_layout->itemAt(1)->widget());
        QVERIFY(line_edit != nullptr);
        QVERIFY(button != nullptr);
        QCOMPARE(line_edit->placeholderText(), QString("Enter text"));
        QCOMPARE(button->text(), QString("Submit"));
        
        // Check footer
        auto* footer = qobject_cast<QLabel*>(main_layout->itemAt(2)->widget());
        QVERIFY(footer != nullptr);
        QCOMPARE(footer->text(), QString("Footer Label"));
    }

    void testDeclarativeBuilderSafeBuild() {
        // Test safe build that doesn't throw exceptions
        auto widget = create<QLabel>()
            .property("text", QString("Safe Build Test"))
            .buildSafe();
        
        QVERIFY(widget != nullptr);
        QCOMPARE(widget->text(), QString("Safe Build Test"));
    }

    void testDeclarativeBuilderWithInvalidProperty() {
        // Test that invalid properties don't crash the builder
        auto widget = create<QLabel>()
            .property("text", QString("Valid Property"))
            .property("nonexistent_property", QString("Invalid"))
            .buildSafe();
        
        QVERIFY(widget != nullptr);
        QCOMPARE(widget->text(), QString("Valid Property"));
        // Invalid property should be ignored or handled gracefully
    }

    // **Factory Function Tests**
    void testCreateFactoryFunction() {
        // Test that the create<T>() factory function works
        auto builder = create<QPushButton>();
        
        auto widget = builder
            .property("text", QString("Factory Test"))
            .build();
        
        QVERIFY(widget != nullptr);
        QCOMPARE(widget->text(), QString("Factory Test"));
    }

    void testCreateWithDifferentWidgetTypes() {
        // Test creating different widget types
        auto label = create<QLabel>()
            .property("text", QString("Label"))
            .build();
        
        auto button = create<QPushButton>()
            .property("text", QString("Button"))
            .build();
        
        auto line_edit = create<QLineEdit>()
            .property("text", QString("LineEdit"))
            .build();
        
        QVERIFY(label != nullptr);
        QVERIFY(button != nullptr);
        QVERIFY(line_edit != nullptr);
        
        QCOMPARE(label->text(), QString("Label"));
        QCOMPARE(button->text(), QString("Button"));
        QCOMPARE(line_edit->text(), QString("LineEdit"));
    }

    // **Error Handling Tests**
    void testBuilderExceptionHandling() {
        // Test that builder handles exceptions gracefully
        try {
            auto widget = create<QLabel>()
                .property("text", QString("Exception Test"))
                .build();
            
            QVERIFY(widget != nullptr);
        } catch (const std::exception& e) {
            QFAIL(QString("Builder threw unexpected exception: %1").arg(e.what()).toLocal8Bit());
        }
    }

    void testUIElementExceptionHandling() {
        auto element = std::make_unique<TestUIElement>();
        
        // Test that UIElement methods don't throw
        try {
            element->setProperty("test", QString("value"));
            element->onEvent("test", []() {});
            element->bindProperty("test", []() -> PropertyValue { return PropertyValue(QString("test")); });
            element->initialize();
            element->refresh();
            element->invalidate();
            element->cleanup();
            
            QVERIFY(true); // If we get here, no exceptions were thrown
        } catch (const std::exception& e) {
            QFAIL(QString("UIElement threw unexpected exception: %1").arg(e.what()).toLocal8Bit());
        }
    }

    // **Performance Tests**
    void testBuilderPerformance() {
        QElapsedTimer timer;
        timer.start();
        
        // Create many widgets to test performance
        std::vector<std::unique_ptr<QWidget>> widgets;
        for (int i = 0; i < 100; ++i) {
            auto widget = create<QLabel>()
                .property("text", QString("Widget %1").arg(i))
                .build();
            widgets.push_back(std::move(widget));
        }
        
        qint64 elapsed = timer.elapsed();
        qDebug() << "Created 100 widgets in" << elapsed << "ms";
        
        QVERIFY(elapsed < 1000); // Should complete in reasonable time
        QCOMPARE(widgets.size(), 100);
        
        // Verify all widgets were created correctly
        for (size_t i = 0; i < widgets.size(); ++i) {
            QVERIFY(widgets[i] != nullptr);
            auto* label = qobject_cast<QLabel*>(widgets[i].get());
            QVERIFY(label != nullptr);
            QCOMPARE(label->text(), QString("Widget %1").arg(i));
        }
    }
};

QTEST_MAIN(CoreTest)
#include "test_core.moc"
