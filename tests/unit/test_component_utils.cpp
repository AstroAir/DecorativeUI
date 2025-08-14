// tests/unit/test_component_utils.cpp
#include <QApplication>
#include <QFormLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QtTest/QtTest>
#include <memory>

#include "../../src/Core/ComponentUtils.hpp"

using namespace DeclarativeUI::Core::Utils;

class ComponentUtilsTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // **Property Setter Tests**
    void testPropertySetter();
    void testPropertySetterValidation();
    void testPropertySetterGetters();

    // **Layout Utils Tests**
    void testLayoutCreation();
    void testLayoutApplication();
    void testAddWidget();
    void testGridLayoutUtils();
    void testFormLayoutUtils();

    // **Styling Utils Tests**
    void testThemeStyleApplication();
    void testDropShadowEffect();
    void testBorderRadius();

    // **Animation Utils Tests**
    void testFadeAnimation();
    void testSlideAnimation();

    // **Event Utils Tests**
    void testSafeSignalConnection();
    void testDelayedExecution();

    // **Validation Utils Tests**
    void testWidgetValidation();
    void testLayoutHierarchyValidation();

private:
    QApplication* app;
    std::unique_ptr<QWidget> testWidget;
};

void ComponentUtilsTest::initTestCase() {
    if (!QApplication::instance()) {
        int argc = 0;
        char* argv[] = {nullptr};
        app = new QApplication(argc, argv);
    }
}

void ComponentUtilsTest::cleanupTestCase() {
    if (app) {
        delete app;
        app = nullptr;
    }
}

void ComponentUtilsTest::init() { testWidget = std::make_unique<QWidget>(); }

void ComponentUtilsTest::cleanup() { testWidget.reset(); }

// **Property Setter Tests**
void ComponentUtilsTest::testPropertySetter() {
    auto propSetter = properties(testWidget.get());

    // Test setting properties
    QVERIFY(propSetter.setProperty("objectName", QString("TestWidget")));
    QVERIFY(propSetter.setProperty("toolTip", QString("Test tooltip")));
    QVERIFY(propSetter.setProperty("enabled", true));

    // Verify properties were set
    QCOMPARE(testWidget->objectName(), QString("TestWidget"));
    QCOMPARE(testWidget->toolTip(), QString("Test tooltip"));
    QVERIFY(testWidget->isEnabled());
}

void ComponentUtilsTest::testPropertySetterValidation() {
    auto propSetter = properties(testWidget.get());

    // Test property existence check
    QVERIFY(propSetter.hasProperty("objectName"));
    QVERIFY(propSetter.hasProperty("toolTip"));
    QVERIFY(!propSetter.hasProperty("nonExistentProperty"));

    // Test setting invalid property
    QVERIFY(!propSetter.setProperty("invalidProperty", "value"));
}

void ComponentUtilsTest::testPropertySetterGetters() {
    auto propSetter = properties(testWidget.get());

    // Set a property
    propSetter.setProperty("objectName", QString("GetterTest"));

    // Test getting property
    auto name = propSetter.getProperty<QString>("objectName");
    QVERIFY(name.has_value());
    QCOMPARE(name.value(), QString("GetterTest"));

    // Test getting non-existent property
    auto invalid = propSetter.getProperty<QString>("nonExistent");
    QVERIFY(!invalid.has_value());

    // Test listing properties
    auto properties = propSetter.listProperties();
    QVERIFY(properties.size() > 0);
    QVERIFY(std::find(properties.begin(), properties.end(),
                      QString("objectName")) != properties.end());
}

// **Layout Utils Tests**
void ComponentUtilsTest::testLayoutCreation() {
    // Test VBox layout creation
    auto vboxLayout = Layout::createLayout<QVBoxLayout>();
    QVERIFY(vboxLayout != nullptr);
    QVERIFY(qobject_cast<QVBoxLayout*>(vboxLayout.get()) != nullptr);

    // Test HBox layout creation
    auto hboxLayout = Layout::createLayout<QHBoxLayout>();
    QVERIFY(hboxLayout != nullptr);
    QVERIFY(qobject_cast<QHBoxLayout*>(hboxLayout.get()) != nullptr);

    // Test Grid layout creation
    auto gridLayout = Layout::createLayout<QGridLayout>();
    QVERIFY(gridLayout != nullptr);
    QVERIFY(qobject_cast<QGridLayout*>(gridLayout.get()) != nullptr);
}

void ComponentUtilsTest::testLayoutApplication() {
    auto layout = Layout::createLayout<QVBoxLayout>();
    QVERIFY(layout != nullptr);

    // Test applying layout to widget
    bool success = Layout::applyLayout(testWidget.get(), std::move(layout));
    QVERIFY(success);
    QVERIFY(testWidget->layout() != nullptr);
    QVERIFY(qobject_cast<QVBoxLayout*>(testWidget->layout()) != nullptr);
}

void ComponentUtilsTest::testAddWidget() {
    auto layout = Layout::createLayout<QVBoxLayout>();
    Layout::applyLayout(testWidget.get(), std::move(layout));

    auto childWidget = new QWidget();

    // Test adding widget to layout
    bool success = Layout::addWidget(testWidget->layout(), childWidget);
    QVERIFY(success);
    QCOMPARE(testWidget->layout()->count(), 1);

    // Test adding widget at specific position
    auto anotherWidget = new QWidget();
    success = Layout::addWidget(testWidget->layout(), anotherWidget, 0);
    QVERIFY(success);
    QCOMPARE(testWidget->layout()->count(), 2);
}

void ComponentUtilsTest::testGridLayoutUtils() {
    auto gridLayout = Layout::createLayout<QGridLayout>();
    Layout::applyLayout(testWidget.get(), std::move(gridLayout));

    auto* grid = qobject_cast<QGridLayout*>(testWidget->layout());
    QVERIFY(grid != nullptr);

    auto childWidget = new QWidget();

    // Test adding widget to grid
    bool success = Layout::addWidgetToGrid(grid, childWidget, 0, 0);
    QVERIFY(success);
    QCOMPARE(grid->count(), 1);

    // Test adding widget with span
    auto spanWidget = new QWidget();
    success =
        Layout::addWidgetToGrid(grid, spanWidget, 1, 0, 1, 2, Qt::AlignCenter);
    QVERIFY(success);
    QCOMPARE(grid->count(), 2);

    // Test invalid position
    auto invalidWidget = new QWidget();
    success = Layout::addWidgetToGrid(grid, invalidWidget, -1, -1);
    QVERIFY(!success);
    delete invalidWidget;
}

void ComponentUtilsTest::testFormLayoutUtils() {
    auto formLayout = Layout::createLayout<QFormLayout>();
    Layout::applyLayout(testWidget.get(), std::move(formLayout));

    auto* form = qobject_cast<QFormLayout*>(testWidget->layout());
    QVERIFY(form != nullptr);

    // Test adding labeled row
    auto fieldWidget = new QWidget();
    bool success = Layout::addFormRow(form, "Test Label", fieldWidget);
    QVERIFY(success);
    QCOMPARE(form->rowCount(), 1);

    // Test adding row with widget label
    auto labelWidget = new QWidget();
    auto fieldWidget2 = new QWidget();
    success = Layout::addFormRow(form, labelWidget, fieldWidget2);
    QVERIFY(success);
    QCOMPARE(form->rowCount(), 2);
}

// **Styling Utils Tests**
void ComponentUtilsTest::testThemeStyleApplication() {
    // Test applying theme style
    Styling::applyThemeStyle(testWidget.get(), "TestComponent");

    // Verify that stylesheet was applied
    QVERIFY(!testWidget->styleSheet().isEmpty());
}

void ComponentUtilsTest::testDropShadowEffect() {
    // Test adding drop shadow
    Styling::addDropShadow(testWidget.get(), QColor(0, 0, 0, 100), 15,
                           QPointF(3, 3));

    // Verify that graphics effect was applied
    QVERIFY(testWidget->graphicsEffect() != nullptr);
}

void ComponentUtilsTest::testBorderRadius() {
    // Test setting border radius
    Styling::setBorderRadius(testWidget.get(), 10);

    // Verify that border radius was applied to stylesheet
    QVERIFY(testWidget->styleSheet().contains("border-radius: 10px"));

    // Test adding to existing stylesheet
    testWidget->setStyleSheet("background-color: red;");
    Styling::setBorderRadius(testWidget.get(), 5);

    QString stylesheet = testWidget->styleSheet();
    QVERIFY(stylesheet.contains("background-color: red"));
    QVERIFY(stylesheet.contains("border-radius: 5px"));
}

// **Animation Utils Tests**
void ComponentUtilsTest::testFadeAnimation() {
    testWidget->show();

    // Test creating fade animation
    auto animation =
        AnimationUtils::createFadeAnimation(testWidget.get(), 0.0, 1.0, 200);
    QVERIFY(animation != nullptr);
    QCOMPARE(animation->duration(), 200);
    QCOMPARE(animation->propertyName(), QByteArray("windowOpacity"));
}

void ComponentUtilsTest::testSlideAnimation() {
    testWidget->show();

    // Test creating slide animation
    QPoint fromPos(0, 0);
    QPoint toPos(100, 100);
    auto animation = AnimationUtils::createSlideAnimation(testWidget.get(),
                                                          fromPos, toPos, 300);

    QVERIFY(animation != nullptr);
    QCOMPARE(animation->duration(), 300);
    QCOMPARE(animation->propertyName(), QByteArray("pos"));
}

// **Event Utils Tests**
void ComponentUtilsTest::testSafeSignalConnection() {
    auto button = new QPushButton("Test", testWidget.get());
    bool clicked = false;

    // Test safe signal connection
    bool success =
        Events::connectSafely(button, &QPushButton::clicked, testWidget.get(),
                              [&clicked]() { clicked = true; });
    QVERIFY(success);

    // Simulate button click
    button->click();
    QVERIFY(clicked);
}

void ComponentUtilsTest::testDelayedExecution() {
    bool executed = false;

    // Test delayed execution
    Events::executeDelayed(50, [&executed]() { executed = true; });

    // Wait for execution
    QTest::qWait(100);
    QVERIFY(executed);
}

// **Validation Utils Tests**
void ComponentUtilsTest::testWidgetValidation() {
    // Test valid widget
    QVERIFY(Validation::validateWidget(testWidget.get(), "TestWidget"));

    // Test null widget
    QVERIFY(!Validation::validateWidget(static_cast<QWidget*>(nullptr),
                                        "NullWidget"));
}

void ComponentUtilsTest::testLayoutHierarchyValidation() {
    // Create a simple hierarchy
    auto childWidget = new QWidget(testWidget.get());
    auto grandChildWidget = new QWidget(childWidget);

    // Test validation
    QVERIFY(Validation::validateLayoutHierarchy(testWidget.get()));

    // Test null widget
    QVERIFY(!Validation::validateLayoutHierarchy(nullptr));
}

QTEST_MAIN(ComponentUtilsTest)
#include "test_component_utils.moc"
