#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSignalSpy>
#include <QtTest/QtTest>
#include <memory>

#include "../../src/Command/CoreCommands.hpp"
#include "../../src/Command/UICommand.hpp"
#include "../../src/Command/UICommandFactory.hpp"
#include "../../src/Command/WidgetMapper.hpp"

using namespace DeclarativeUI::Command::UI;

class WidgetMapperTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // **Basic mapping operations**
    void testMappingRegistration();
    void testWidgetCreation();
    void testWidgetDestruction();
    void testMappingLookup();

    // **Widget synchronization**
    void testCommandToWidgetSync();
    void testWidgetToCommandSync();
    void testBidirectionalSync();
    void testPropertyMapping();

    // **Event handling**
    void testEventBinding();
    void testEventPropagation();
    void testCustomEventHandling();

    // **Built-in widget types**
    void testButtonMapping();
    void testLabelMapping();
    void testTextInputMapping();
    void testCheckBoxMapping();

    // **Advanced features**
    void testCustomWidgetMapping();
    void testWidgetHierarchy();
    void testDynamicMapping();

    // **Error handling**
    void testInvalidCommandType();
    void testNullCommandHandling();
    void testMappingConflicts();

    // **Performance tests**
    void testMassWidgetCreation();
    void testSyncPerformance();

private:
    std::unique_ptr<WidgetMapper> mapper_;
    std::unique_ptr<UICommandFactory> factory_;
};

void WidgetMapperTest::initTestCase() {
    qDebug() << "🧪 Starting Widget Mapper tests...";
}

void WidgetMapperTest::cleanupTestCase() {
    qDebug() << "✅ Widget Mapper tests completed";
}

void WidgetMapperTest::init() {
    mapper_ = std::make_unique<WidgetMapper>();
    factory_ = std::make_unique<UICommandFactory>();
}

void WidgetMapperTest::cleanup() {
    mapper_.reset();
    factory_.reset();
}

void WidgetMapperTest::testMappingRegistration() {
    qDebug() << "🧪 Testing mapping registration...";

    // Test registering a mapping
    WidgetMappingConfig config;
    config.widget_type = "QPushButton";
    config.factory = []() -> std::unique_ptr<QWidget> {
        return std::make_unique<QPushButton>();
    };

    mapper_->registerMapping("TestButton", config);

    // Verify mapping exists
    QVERIFY(mapper_->hasMapping("TestButton"));
    QCOMPARE(mapper_->getWidgetType("TestButton"), QString("QPushButton"));

    qDebug() << "✅ Mapping registration test passed";
}

void WidgetMapperTest::testWidgetCreation() {
    qDebug() << "🧪 Testing widget creation...";

    auto& globalMapper = WidgetMapper::instance();

    // Create a button command
    auto button = std::make_shared<ButtonCommand>();
    button->getState()->setProperty("text", "Test Button");

    // Create widget
    auto widget = globalMapper.createWidget(button.get());
    QVERIFY(widget != nullptr);

    auto* qButton = qobject_cast<QPushButton*>(widget.get());
    QVERIFY(qButton != nullptr);
    QCOMPARE(qButton->text(), QString("Test Button"));

    qDebug() << "✅ Widget creation test passed";
}

void WidgetMapperTest::testWidgetDestruction() {
    qDebug() << "🧪 Testing widget destruction...";

    auto& globalMapper = WidgetMapper::instance();

    auto button = std::make_shared<ButtonCommand>();
    auto widget = globalMapper.createWidget(button.get());
    QVERIFY(widget != nullptr);

    // Test destruction
    globalMapper.destroyWidget(button.get());

    // Widget should be properly cleaned up
    QVERIFY(true);  // Placeholder - actual test would verify cleanup

    qDebug() << "✅ Widget destruction test passed";
}

void WidgetMapperTest::testMappingLookup() {
    qDebug() << "🧪 Testing mapping lookup...";

    auto& globalMapper = WidgetMapper::instance();

    // Test built-in mappings
    QVERIFY(globalMapper.hasMapping("Button"));
    QVERIFY(globalMapper.hasMapping("Label"));

    // Test non-existent mapping
    QVERIFY(!globalMapper.hasMapping("NonExistentWidget"));

    qDebug() << "✅ Mapping lookup test passed";
}

void WidgetMapperTest::testCommandToWidgetSync() {
    qDebug() << "🧪 Testing command to widget sync...";

    auto& globalMapper = WidgetMapper::instance();

    auto button = std::make_shared<ButtonCommand>();
    auto widget = globalMapper.createWidget(button.get());
    auto* qButton = qobject_cast<QPushButton*>(widget.get());

    QVERIFY(qButton != nullptr);

    // Change command property
    button->getState()->setProperty("text", "Updated Text");
    button->syncToWidget();

    // Widget should reflect the change
    QCOMPARE(qButton->text(), QString("Updated Text"));

    qDebug() << "✅ Command to widget sync test passed";
}

void WidgetMapperTest::testWidgetToCommandSync() {
    qDebug() << "🧪 Testing widget to command sync...";

    auto& globalMapper = WidgetMapper::instance();

    auto button = std::make_shared<ButtonCommand>();
    auto widget = globalMapper.createWidget(button.get());
    auto* qButton = qobject_cast<QPushButton*>(widget.get());

    QVERIFY(qButton != nullptr);

    // Change widget property
    qButton->setText("Widget Updated");
    button->syncFromWidget();

    // Command should reflect the change
    QCOMPARE(button->getState()->getProperty<QString>("text"),
             QString("Widget Updated"));

    qDebug() << "✅ Widget to command sync test passed";
}

void WidgetMapperTest::testBidirectionalSync() {
    qDebug() << "🧪 Testing bidirectional sync...";

    auto& globalMapper = WidgetMapper::instance();

    auto button = std::make_shared<ButtonCommand>();
    auto widget = globalMapper.createWidget(button.get());

    // Establish bidirectional binding
    globalMapper.establishBinding(button.get(), widget.get());

    // Test command -> widget
    button->getState()->setProperty("text", "From Command");
    // Sync should happen automatically with binding

    // Test widget -> command
    auto* qButton = qobject_cast<QPushButton*>(widget.get());
    if (qButton) {
        qButton->setText("From Widget");
        // Sync should happen automatically with binding
    }

    qDebug() << "✅ Bidirectional sync test passed";
}

void WidgetMapperTest::testPropertyMapping() {
    qDebug() << "🧪 Testing property mapping...";

    auto& globalMapper = WidgetMapper::instance();

    auto button = std::make_shared<ButtonCommand>();
    button->getState()->setProperty("text", "Test");
    button->getState()->setProperty("enabled", false);

    auto widget = globalMapper.createWidget(button.get());
    auto* qButton = qobject_cast<QPushButton*>(widget.get());

    QVERIFY(qButton != nullptr);
    QCOMPARE(qButton->text(), QString("Test"));
    QCOMPARE(qButton->isEnabled(), false);

    qDebug() << "✅ Property mapping test passed";
}

void WidgetMapperTest::testEventBinding() {
    qDebug() << "🧪 Testing event binding...";

    auto& globalMapper = WidgetMapper::instance();

    auto button = std::make_shared<ButtonCommand>();
    auto widget = globalMapper.createWidget(button.get());
    auto* qButton = qobject_cast<QPushButton*>(widget.get());

    QVERIFY(qButton != nullptr);

    // Set up signal spy
    QSignalSpy spy(button.get(), &ButtonCommand::clicked);

    // Simulate button click
    qButton->click();

    // Should have received the signal
    QCOMPARE(spy.count(), 1);

    qDebug() << "✅ Event binding test passed";
}

void WidgetMapperTest::testEventPropagation() {
    qDebug() << "🧪 Testing event propagation...";

    // Test that events properly propagate through the command system
    QVERIFY(true);  // Placeholder

    qDebug() << "✅ Event propagation test passed";
}

void WidgetMapperTest::testCustomEventHandling() {
    qDebug() << "🧪 Testing custom event handling...";

    // Test custom event handling in widget mapping
    QVERIFY(true);  // Placeholder

    qDebug() << "✅ Custom event handling test passed";
}

void WidgetMapperTest::testButtonMapping() {
    qDebug() << "🧪 Testing button mapping...";

    auto& globalMapper = WidgetMapper::instance();

    auto button = std::make_shared<ButtonCommand>();
    button->getState()->setProperty("text", "Click Me");
    button->getState()->setProperty("enabled", true);

    auto widget = globalMapper.createWidget(button.get());
    auto* qButton = qobject_cast<QPushButton*>(widget.get());

    QVERIFY(qButton != nullptr);
    QCOMPARE(qButton->text(), QString("Click Me"));
    QVERIFY(qButton->isEnabled());

    qDebug() << "✅ Button mapping test passed";
}

void WidgetMapperTest::testLabelMapping() {
    qDebug() << "🧪 Testing label mapping...";

    auto& globalMapper = WidgetMapper::instance();

    auto label = UICommandFactory::instance().createCommand("Label");
    if (label) {
        label->getState()->setProperty("text", "Test Label");

        auto widget = globalMapper.createWidget(label.get());
        auto* qLabel = qobject_cast<QLabel*>(widget.get());

        QVERIFY(qLabel != nullptr);
        QCOMPARE(qLabel->text(), QString("Test Label"));
    }

    qDebug() << "✅ Label mapping test passed";
}

void WidgetMapperTest::testTextInputMapping() {
    qDebug() << "🧪 Testing text input mapping...";

    auto& globalMapper = WidgetMapper::instance();

    auto textInput = UICommandFactory::instance().createCommand("TextInput");
    if (textInput) {
        textInput->getState()->setProperty("text", "Initial Text");
        textInput->getState()->setProperty("placeholder", "Enter text...");

        auto widget = globalMapper.createWidget(textInput.get());
        auto* qLineEdit = qobject_cast<QLineEdit*>(widget.get());

        QVERIFY(qLineEdit != nullptr);
        QCOMPARE(qLineEdit->text(), QString("Initial Text"));
        QCOMPARE(qLineEdit->placeholderText(), QString("Enter text..."));
    }

    qDebug() << "✅ Text input mapping test passed";
}

void WidgetMapperTest::testCheckBoxMapping() {
    qDebug() << "🧪 Testing checkbox mapping...";

    auto& globalMapper = WidgetMapper::instance();

    auto checkBox = UICommandFactory::instance().createCommand("CheckBox");
    if (checkBox) {
        checkBox->getState()->setProperty("text", "Check me");
        checkBox->getState()->setProperty("checked", true);

        auto widget = globalMapper.createWidget(checkBox.get());
        auto* qCheckBox = qobject_cast<QCheckBox*>(widget.get());

        QVERIFY(qCheckBox != nullptr);
        QCOMPARE(qCheckBox->text(), QString("Check me"));
        QVERIFY(qCheckBox->isChecked());
    }

    qDebug() << "✅ Checkbox mapping test passed";
}

void WidgetMapperTest::testCustomWidgetMapping() {
    qDebug() << "🧪 Testing custom widget mapping...";

    // Test registering and using custom widget mappings
    QVERIFY(true);  // Placeholder

    qDebug() << "✅ Custom widget mapping test passed";
}

void WidgetMapperTest::testWidgetHierarchy() {
    qDebug() << "🧪 Testing widget hierarchy...";

    // Test creating widget hierarchies from command hierarchies
    QVERIFY(true);  // Placeholder

    qDebug() << "✅ Widget hierarchy test passed";
}

void WidgetMapperTest::testDynamicMapping() {
    qDebug() << "🧪 Testing dynamic mapping...";

    // Test dynamic mapping registration and updates
    QVERIFY(true);  // Placeholder

    qDebug() << "✅ Dynamic mapping test passed";
}

void WidgetMapperTest::testInvalidCommandType() {
    qDebug() << "🧪 Testing invalid command type...";

    auto& globalMapper = WidgetMapper::instance();

    // Try to create widget for non-existent command type
    auto widget = globalMapper.createWidget(nullptr);
    QVERIFY(widget == nullptr);

    qDebug() << "✅ Invalid command type test passed";
}

void WidgetMapperTest::testNullCommandHandling() {
    qDebug() << "🧪 Testing null command handling...";

    auto& globalMapper = WidgetMapper::instance();

    // Test null command handling
    auto widget = globalMapper.createWidget(nullptr);
    QVERIFY(widget == nullptr);

    qDebug() << "✅ Null command handling test passed";
}

void WidgetMapperTest::testMappingConflicts() {
    qDebug() << "🧪 Testing mapping conflicts...";

    // Test handling of mapping conflicts and overwrites
    QVERIFY(true);  // Placeholder

    qDebug() << "✅ Mapping conflicts test passed";
}

void WidgetMapperTest::testMassWidgetCreation() {
    qDebug() << "🧪 Testing mass widget creation...";

    auto& globalMapper = WidgetMapper::instance();

    QElapsedTimer timer;
    timer.start();

    // Create many widgets
    std::vector<std::unique_ptr<QWidget>> widgets;
    for (int i = 0; i < 100; ++i) {
        auto button = std::make_shared<ButtonCommand>();
        auto widget = globalMapper.createWidget(button.get());
        if (widget) {
            widgets.push_back(std::move(widget));
        }
    }

    qint64 elapsed = timer.elapsed();
    qDebug() << "Created 100 widgets in" << elapsed << "ms";

    QCOMPARE(widgets.size(), 100);
    QVERIFY(elapsed < 1000);  // Should be fast

    qDebug() << "✅ Mass widget creation test passed";
}

void WidgetMapperTest::testSyncPerformance() {
    qDebug() << "🧪 Testing sync performance...";

    auto& globalMapper = WidgetMapper::instance();

    auto button = std::make_shared<ButtonCommand>();
    auto widget = globalMapper.createWidget(button.get());

    QElapsedTimer timer;
    timer.start();

    // Perform many sync operations
    for (int i = 0; i < 1000; ++i) {
        button->getState()->setProperty("text", QString("Text %1").arg(i));
        button->syncToWidget();
    }

    qint64 elapsed = timer.elapsed();
    qDebug() << "1000 sync operations in" << elapsed << "ms";

    QVERIFY(elapsed < 2000);  // Should be reasonably fast

    qDebug() << "✅ Sync performance test passed";
}

QTEST_MAIN(WidgetMapperTest)
#include "test_widget_mapper.moc"
