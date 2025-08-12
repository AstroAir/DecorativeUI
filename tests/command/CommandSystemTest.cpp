#include <QApplication>
#include <QSignalSpy>
#include <QtTest/QtTest>
#include <memory>

#include "../../src/Command/CommandBuilder.hpp"
#include "../../src/Command/CommandEvents.hpp"
#include "../../src/Command/CoreCommands.hpp"
#include "../../src/Command/MVCIntegration.hpp"
#include "../../src/Command/SpecializedCommands.hpp"
#include "../../src/Command/UICommand.hpp"
#include "../../src/Command/UICommandFactory.hpp"
#include "../../src/Command/WidgetMapper.hpp"

using namespace DeclarativeUI::Command::UI;

class CommandSystemTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    // Core functionality tests
    void testCommandCreation();
    void testCommandState();
    void testCommandHierarchy();
    void testWidgetMapping();
    void testPropertyBinding();
    void testEventSystem();
    void testCommandBuilder();
    void testMVCIntegration();
    void testInheritance();
    void testValidation();

    // Performance tests
    void testCommandCreationPerformance();
    void testEventDispatchingPerformance();

private:
    void registerTestCommands();
};

void CommandSystemTest::initTestCase() {
    // Register core commands
    registerCoreCommands();
    registerSpecializedCommands();

    qDebug() << "🧪 Command System Test Suite initialized";
}

void CommandSystemTest::cleanupTestCase() {
    qDebug() << "🧪 Command System Test Suite completed";
}

void CommandSystemTest::testCommandCreation() {
    qDebug() << "🧪 Testing command creation...";

    // Test factory creation
    auto& factory = UICommandFactory::instance();

    // Test button creation
    auto button = factory.createCommand("Button");
    QVERIFY(button != nullptr);
    QCOMPARE(button->getCommandType(), QString("Button"));
    QCOMPARE(button->getWidgetType(), QString("QPushButton"));

    // Test with configuration
    QJsonObject config;
    config["text"] = "Test Button";
    config["enabled"] = true;

    auto configuredButton = factory.createCommand("Button", config);
    QVERIFY(configuredButton != nullptr);
    QCOMPARE(configuredButton->getState()->getProperty<QString>("text"),
             QString("Test Button"));
    QCOMPARE(configuredButton->getState()->getProperty<bool>("enabled"), true);

    qDebug() << "✅ Command creation test passed";
}

void CommandSystemTest::testCommandState() {
    qDebug() << "🧪 Testing command state management...";

    auto button = UICommandFactory::instance().createCommand("Button");
    QVERIFY(button != nullptr);

    auto state = button->getState();
    QVERIFY(state != nullptr);

    // Test property setting and getting
    state->setProperty("text", QString("Test Text"));
    QCOMPARE(state->getProperty<QString>("text"), QString("Test Text"));

    // Test property change signals
    QSignalSpy spy(state, &UICommandState::propertyChanged);
    state->setProperty("enabled", false);
    QCOMPARE(spy.count(), 1);

    // Test validation
    state->setValidator("text", [](const QVariant& value) {
        return !value.toString().isEmpty();
    });

    QVERIFY(state->validateProperty("text", "Valid"));
    QVERIFY(!state->validateProperty("text", ""));

    qDebug() << "✅ Command state test passed";
}

void CommandSystemTest::testCommandHierarchy() {
    qDebug() << "🧪 Testing command hierarchy...";

    auto container = UICommandFactory::instance().createCommand("Container");
    auto button1 = UICommandFactory::instance().createCommand("Button");
    auto button2 = UICommandFactory::instance().createCommand("Button");

    QVERIFY(container != nullptr);
    QVERIFY(button1 != nullptr);
    QVERIFY(button2 != nullptr);

    // Test adding children
    container->addChild(button1);
    container->addChild(button2);

    auto children = container->getChildren();
    QCOMPARE(children.size(), 2);
    QCOMPARE(children[0], button1);
    QCOMPARE(children[1], button2);

    // Test parent relationships
    QCOMPARE(button1->getParent(), container.get());
    QCOMPARE(button2->getParent(), container.get());

    // Test removing children
    container->removeChild(button1);
    children = container->getChildren();
    QCOMPARE(children.size(), 1);
    QCOMPARE(children[0], button2);

    qDebug() << "✅ Command hierarchy test passed";
}

void CommandSystemTest::testWidgetMapping() {
    qDebug() << "🧪 Testing widget mapping...";

    auto& mapper = WidgetMapper::instance();

    // Test mapping registration
    QVERIFY(mapper.hasMapping("Button"));
    QCOMPARE(mapper.getWidgetType("Button"), QString("QPushButton"));

    // Test widget creation
    auto button = UICommandFactory::instance().createCommand("Button");
    button->getState()->setProperty("text", "Test Button");

    auto widget = mapper.createWidget(button.get());
    QVERIFY(widget != nullptr);

    auto* qButton = qobject_cast<QPushButton*>(widget.get());
    QVERIFY(qButton != nullptr);
    QCOMPARE(qButton->text(), QString("Test Button"));

    qDebug() << "✅ Widget mapping test passed";
}

void CommandSystemTest::testPropertyBinding() {
    qDebug() << "🧪 Testing property binding...";

    auto button = UICommandFactory::instance().createCommand("Button");
    auto widget = WidgetMapper::instance().createWidget(button.get());

    QVERIFY(button != nullptr);
    QVERIFY(widget != nullptr);

    // Test command to widget sync
    button->getState()->setProperty("text", "New Text");
    button->syncToWidget();

    auto* qButton = qobject_cast<QPushButton*>(widget.get());
    QVERIFY(qButton != nullptr);
    QCOMPARE(qButton->text(), QString("New Text"));

    // Test widget to command sync
    qButton->setText("Widget Text");
    button->syncFromWidget();
    QCOMPARE(button->getState()->getProperty<QString>("text"),
             QString("Widget Text"));

    qDebug() << "✅ Property binding test passed";
}

void CommandSystemTest::testEventSystem() {
    qDebug() << "🧪 Testing event system...";

    auto& dispatcher = CommandEventDispatcher::instance();
    auto button = UICommandFactory::instance().createCommand("Button");

    // Test event handler registration
    bool eventReceived = false;
    auto handlerId = dispatcher.registerHandler(
        button.get(), CommandEventType::Clicked,
        [&eventReceived](const CommandEvent& event) {
            eventReceived = true;
            QCOMPARE(event.getType(), CommandEventType::Clicked);
        });

    QVERIFY(!handlerId.isNull());

    // Test event dispatching
    auto clickEvent = std::make_unique<ClickEvent>(button.get());
    dispatcher.dispatchEvent(std::move(clickEvent));

    // Process events
    QCoreApplication::processEvents();

    QVERIFY(eventReceived);

    // Test handler removal
    dispatcher.unregisterHandler(handlerId);
    QCOMPARE(dispatcher.getHandlerCount(button.get()), 0);

    qDebug() << "✅ Event system test passed";
}

void CommandSystemTest::testCommandBuilder() {
    qDebug() << "🧪 Testing command builder...";

    // Test simple builder
    auto button = CommandBuilder("Button")
                      .text("Builder Button")
                      .enabled(true)
                      .tooltip("Built with CommandBuilder")
                      .build();

    QVERIFY(button != nullptr);
    QCOMPARE(button->getCommandType(), QString("Button"));
    QCOMPARE(button->getState()->getProperty<QString>("text"),
             QString("Builder Button"));
    QCOMPARE(button->getState()->getProperty<bool>("enabled"), true);

    // Test hierarchy builder
    auto container =
        CommandHierarchyBuilder("Container")
            .layout("VBox")
            .spacing(10)
            .addChild("Label",
                      [](CommandBuilder& label) { label.text("Test Label"); })
            .addChild("Button",
                      [](CommandBuilder& btn) { btn.text("Test Button"); })
            .build();

    QVERIFY(container != nullptr);
    QCOMPARE(container->getChildren().size(), 2);

    qDebug() << "✅ Command builder test passed";
}

void CommandSystemTest::testMVCIntegration() {
    qDebug() << "🧪 Testing MVC integration...";

    auto& bridge = MVCIntegrationBridge::instance();
    auto button = UICommandFactory::instance().createCommand("Button");

    // Test state binding
    bridge.bindCommandToStateManager(button, "test.button.state", "text");

    auto boundKeys = bridge.getBoundStateKeys(button);
    QVERIFY(boundKeys.contains("test.button.state"));

    // Test action registration
    bridge.registerCommandAsAction(button, "test.button.action");

    auto registeredActions = bridge.getRegisteredActions(button);
    QVERIFY(registeredActions.contains("test.button.action"));

    // Test cleanup
    bridge.unbindCommandFromStateManager(button, "text");
    boundKeys = bridge.getBoundStateKeys(button);
    QVERIFY(!boundKeys.contains("test.button.state"));

    qDebug() << "✅ MVC integration test passed";
}

void CommandSystemTest::testInheritance() {
    qDebug() << "🧪 Testing command inheritance...";

    // Test radio button (inherits from button)
    auto radioButton =
        UICommandFactory::instance().createCommand("RadioButton");
    QVERIFY(radioButton != nullptr);
    QCOMPARE(radioButton->getCommandType(), QString("RadioButton"));
    QCOMPARE(radioButton->getWidgetType(), QString("QRadioButton"));

    // Test that it has button properties
    QVERIFY(radioButton->getState()->hasProperty("text"));
    QVERIFY(radioButton->getState()->hasProperty("enabled"));

    // Test radio-specific properties
    QVERIFY(radioButton->getState()->hasProperty("autoExclusive"));

    // Test checkbox (also inherits from button)
    auto checkBox = UICommandFactory::instance().createCommand("CheckBox");
    QVERIFY(checkBox != nullptr);
    QCOMPARE(checkBox->getCommandType(), QString("CheckBox"));

    // Test checkbox-specific properties
    QVERIFY(checkBox->getState()->hasProperty("tristate"));
    QVERIFY(checkBox->getState()->hasProperty("checkState"));

    qDebug() << "✅ Command inheritance test passed";
}

void CommandSystemTest::testValidation() {
    qDebug() << "🧪 Testing validation...";

    auto textInput = UICommandFactory::instance().createCommand("TextInput");
    auto state = textInput->getState();

    // Set up validation
    state->setValidator("text", [](const QVariant& value) {
        QString text = value.toString();
        return text.length() >= 3;
    });

    // Test valid input
    state->setProperty("text", "Valid");
    QVERIFY(state->validateAllProperties());

    // Test invalid input
    QSignalSpy spy(state, &UICommandState::validationFailed);
    state->setProperty("text", "No");  // Too short
    QCOMPARE(spy.count(), 1);

    qDebug() << "✅ Validation test passed";
}

void CommandSystemTest::testCommandCreationPerformance() {
    qDebug() << "🧪 Testing command creation performance...";

    QElapsedTimer timer;
    timer.start();

    const int iterations = 1000;
    std::vector<std::shared_ptr<BaseUICommand>> commands;
    commands.reserve(iterations);

    for (int i = 0; i < iterations; ++i) {
        auto command = UICommandFactory::instance().createCommand("Button");
        commands.push_back(command);
    }

    qint64 elapsed = timer.elapsed();
    qDebug() << "Created" << iterations << "commands in" << elapsed << "ms";
    qDebug() << "Average:" << (double(elapsed) / iterations)
             << "ms per command";

    // Performance should be reasonable (less than 1ms per command on average)
    QVERIFY(elapsed < iterations);

    qDebug() << "✅ Command creation performance test passed";
}

void CommandSystemTest::testEventDispatchingPerformance() {
    qDebug() << "🧪 Testing event dispatching performance...";

    auto& dispatcher = CommandEventDispatcher::instance();
    auto button = UICommandFactory::instance().createCommand("Button");

    // Register multiple handlers
    const int handlerCount = 100;
    std::vector<QUuid> handlerIds;

    for (int i = 0; i < handlerCount; ++i) {
        auto id = dispatcher.registerHandler(
            button.get(), CommandEventType::Clicked,
            [](const CommandEvent&) { /* empty handler */ });
        handlerIds.push_back(id);
    }

    QElapsedTimer timer;
    timer.start();

    const int eventCount = 1000;
    for (int i = 0; i < eventCount; ++i) {
        auto event = std::make_unique<ClickEvent>(button.get());
        dispatcher.dispatchEvent(std::move(event));
    }

    qint64 elapsed = timer.elapsed();
    qDebug() << "Dispatched" << eventCount << "events to" << handlerCount
             << "handlers in" << elapsed << "ms";
    qDebug() << "Average:" << (double(elapsed) / eventCount) << "ms per event";

    // Clean up
    for (const auto& id : handlerIds) {
        dispatcher.unregisterHandler(id);
    }

    qDebug() << "✅ Event dispatching performance test passed";
}

void CommandSystemTest::registerTestCommands() {
    // Test commands are already registered by the core system
}

QTEST_MAIN(CommandSystemTest)
#include "CommandSystemTest.moc"
