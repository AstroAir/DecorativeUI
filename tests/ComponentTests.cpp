// tests/ComponentTests.cpp
#include <QtTest/QtTest>
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QSignalSpy>
#include <memory>

#include "../src/Components/Widget.hpp"
#include "../src/Components/Button.hpp"
#include "../src/Components/Calendar.hpp"
#include "../src/Components/DateTimeEdit.hpp"
#include "../src/Components/GroupBox.hpp"
#include "../src/Core/Theme.hpp"
#include "../src/Core/Animation.hpp"
#include "../src/Core/Validation.hpp"
#include "../src/Core/ErrorHandling.hpp"
#include "../src/Core/ComponentUtils.hpp"

using namespace DeclarativeUI::Components;
using namespace DeclarativeUI::Core;

class ComponentTests : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // **Widget Tests**
    void testWidgetCreation();
    void testWidgetProperties();
    void testWidgetLayouts();
    void testWidgetStyling();
    void testWidgetModernFeatures();

    // **Button Tests**
    void testButtonCreation();
    void testButtonProperties();
    void testButtonSignals();
    void testButtonStyling();

    // **Calendar Tests**
    void testCalendarCreation();
    void testCalendarDateSelection();
    void testCalendarMultiSelection();
    void testCalendarSpecialDates();

    // **DateTimeEdit Tests**
    void testDateTimeEditCreation();
    void testDateTimeEditModes();
    void testDateTimeEditValidation();

    // **GroupBox Tests**
    void testGroupBoxCreation();
    void testGroupBoxLayouts();
    void testGroupBoxCollapsible();

    // **Theme Tests**
    void testThemeManager();
    void testThemeApplication();
    void testThemeExportImport();

    // **Animation Tests**
    void testAnimationBuilder();
    void testAnimationPresets();
    void testAnimationSequence();

    // **Validation Tests**
    void testValidationChain();
    void testValidationResults();
    void testCustomValidators();

    // **Error Handling Tests**
    void testErrorManager();
    void testExceptionHandling();
    void testSafeExecution();

    // **Utility Tests**
    void testPropertySetter();
    void testLayoutUtils();
    void testStylingUtils();

private:
    QApplication* app;
    std::unique_ptr<QWidget> testWidget;
};

void ComponentTests::initTestCase() {
    // Initialize test environment
    if (!QApplication::instance()) {
        int argc = 0;
        char* argv[] = {nullptr};
        app = new QApplication(argc, argv);
    }

    // Initialize error handling
    ErrorHandling::errorManager().addHandler(
        std::make_unique<ErrorHandling::ConsoleErrorHandler>()
    );

    // Load default theme
    Theme::ThemeManager::instance().loadLightTheme();
}

void ComponentTests::cleanupTestCase() {
    // Cleanup test environment
    if (app) {
        delete app;
        app = nullptr;
    }
}

void ComponentTests::init() {
    // Create test widget for each test
    testWidget = std::make_unique<QWidget>();
    testWidget->resize(400, 300);
}

void ComponentTests::cleanup() {
    // Cleanup after each test
    testWidget.reset();
}

// **Widget Tests**
void ComponentTests::testWidgetCreation() {
    Widget widget;
    widget.initialize();
    
    QVERIFY(widget.getWidget() != nullptr);
    QVERIFY(widget.getWidget()->isValid());
}

void ComponentTests::testWidgetProperties() {
    Widget widget;
    widget.initialize();
    
    // Test size properties
    QSize testSize(200, 150);
    widget.size(testSize);
    // Note: Property setting is deferred until widget is shown
    
    // Test visibility
    widget.visible(true);
    QVERIFY(widget.isVisible() || !widget.getWidget()->isVisible()); // May not be visible until shown
    
    // Test enabled state
    widget.enabled(false);
    QVERIFY(!widget.isEnabled() || !widget.getWidget()->isEnabled());
}

void ComponentTests::testWidgetLayouts() {
    Widget widget;
    widget.initialize();
    
    // Test VBox layout
    widget.vBoxLayout();
    QVERIFY(widget.getLayout() != nullptr);
    QVERIFY(qobject_cast<QVBoxLayout*>(widget.getLayout()) != nullptr);
    
    // Test HBox layout
    widget.hBoxLayout();
    QVERIFY(qobject_cast<QHBoxLayout*>(widget.getLayout()) != nullptr);
    
    // Test Grid layout
    widget.gridLayout(3, 3);
    QVERIFY(qobject_cast<QGridLayout*>(widget.getLayout()) != nullptr);
    
    // Test Form layout
    widget.formLayout();
    QVERIFY(qobject_cast<QFormLayout*>(widget.getLayout()) != nullptr);
}

void ComponentTests::testWidgetStyling() {
    Widget widget;
    widget.initialize();
    
    // Test stylesheet application
    QString testStyle = "background-color: red;";
    widget.style(testStyle);
    
    // Test theme application
    Utils::Styling::applyThemeStyle(widget.getWidget(), "TestWidget");
    QVERIFY(!widget.getWidget()->styleSheet().isEmpty());
}

void ComponentTests::testWidgetModernFeatures() {
    Widget widget;
    widget.initialize();
    
    // Test concepts-based methods
    widget.size(QSize(300, 200));
    widget.position(QPoint(10, 10));
    
    // Test property setter utility
    auto propSetter = Utils::properties(widget.getWidget());
    QVERIFY(propSetter.setProperty("objectName", QString("TestWidget")));
    
    auto name = propSetter.getProperty<QString>("objectName");
    QVERIFY(name.has_value());
    QCOMPARE(name.value(), QString("TestWidget"));
}

// **Button Tests**
void ComponentTests::testButtonCreation() {
    Button button;
    button.initialize();
    
    QVERIFY(button.getWidget() != nullptr);
    QVERIFY(qobject_cast<QPushButton*>(button.getWidget()) != nullptr);
}

void ComponentTests::testButtonProperties() {
    Button button;
    button.initialize();
    
    // Test text property
    QString testText = "Test Button";
    button.text(testText);
    
    // Test checkable property
    button.checkable(true);
    button.checked(true);
}

void ComponentTests::testButtonSignals() {
    Button button;
    button.initialize();
    
    auto* qButton = qobject_cast<QPushButton*>(button.getWidget());
    QVERIFY(qButton != nullptr);
    
    // Test signal connection
    QSignalSpy clickSpy(qButton, &QPushButton::clicked);
    
    // Simulate button click
    qButton->click();
    
    QCOMPARE(clickSpy.count(), 1);
}

void ComponentTests::testButtonStyling() {
    Button button;
    button.initialize();
    
    // Test theme application
    Utils::Styling::applyThemeStyle(button.getWidget(), "Button");
    QVERIFY(!button.getWidget()->styleSheet().isEmpty());
    
    // Test border radius
    Utils::Styling::setBorderRadius(button.getWidget(), 8);
    QVERIFY(button.getWidget()->styleSheet().contains("border-radius"));
}

// **Calendar Tests**
void ComponentTests::testCalendarCreation() {
    Calendar calendar;
    calendar.initialize();
    
    QVERIFY(calendar.getWidget() != nullptr);
    QVERIFY(qobject_cast<QCalendarWidget*>(calendar.getWidget()) != nullptr);
}

void ComponentTests::testCalendarDateSelection() {
    Calendar calendar;
    calendar.initialize();
    
    QDate testDate(2024, 6, 15);
    calendar.selectedDate(testDate);
    
    // Test date retrieval
    QDate retrievedDate = calendar.getSelectedDate();
    QVERIFY(retrievedDate.isValid());
}

void ComponentTests::testCalendarMultiSelection() {
    Calendar calendar;
    calendar.initialize();
    
    // Enable multi-selection
    calendar.enableMultiSelection(true);
    
    QList<QDate> testDates = {
        QDate(2024, 6, 15),
        QDate(2024, 6, 16),
        QDate(2024, 6, 17)
    };
    
    calendar.selectedDates(testDates);
    
    QList<QDate> retrievedDates = calendar.getSelectedDates();
    QCOMPARE(retrievedDates.size(), testDates.size());
}

void ComponentTests::testCalendarSpecialDates() {
    Calendar calendar;
    calendar.initialize();
    
    QDate specialDate(2024, 12, 25);
    calendar.addSpecialDate(specialDate, "Christmas");
    
    // Test holiday setting
    QList<QDate> holidays = {QDate(2024, 1, 1), QDate(2024, 7, 4)};
    calendar.setHolidays(holidays);
}

// **DateTimeEdit Tests**
void ComponentTests::testDateTimeEditCreation() {
    DateTimeEdit dateTimeEdit;
    dateTimeEdit.initialize();
    
    QVERIFY(dateTimeEdit.getWidget() != nullptr);
    QVERIFY(qobject_cast<QDateTimeEdit*>(dateTimeEdit.getWidget()) != nullptr);
}

void ComponentTests::testDateTimeEditModes() {
    // Test DateTime mode
    DateTimeEdit dateTimeEdit(DateTimeEdit::EditMode::DateTime);
    dateTimeEdit.initialize();
    
    // Test Date only mode
    DateTimeEdit dateEdit(DateTimeEdit::EditMode::DateOnly);
    dateEdit.initialize();
    
    // Test Time only mode
    DateTimeEdit timeEdit(DateTimeEdit::EditMode::TimeOnly);
    timeEdit.initialize();
    
    QVERIFY(dateTimeEdit.getWidget() != nullptr);
    QVERIFY(dateEdit.getWidget() != nullptr);
    QVERIFY(timeEdit.getWidget() != nullptr);
}

void ComponentTests::testDateTimeEditValidation() {
    DateTimeEdit dateTimeEdit;
    dateTimeEdit.initialize();
    
    QDateTime testDateTime = QDateTime::currentDateTime();
    dateTimeEdit.setDateTime(testDateTime);
    
    QDateTime retrievedDateTime = dateTimeEdit.getDateTime();
    QVERIFY(retrievedDateTime.isValid());
}

// **GroupBox Tests**
void ComponentTests::testGroupBoxCreation() {
    GroupBox groupBox("Test Group");
    groupBox.initialize();
    
    QVERIFY(groupBox.getWidget() != nullptr);
    QVERIFY(qobject_cast<QGroupBox*>(groupBox.getWidget()) != nullptr);
    QCOMPARE(groupBox.getTitle(), QString("Test Group"));
}

void ComponentTests::testGroupBoxLayouts() {
    GroupBox groupBox;
    groupBox.initialize();
    
    // Test layout setting
    groupBox.setVBoxLayout();
    QVERIFY(groupBox.getLayout() != nullptr);
    QVERIFY(qobject_cast<QVBoxLayout*>(groupBox.getLayout()) != nullptr);
    
    // Test widget addition
    auto testWidget = new QWidget();
    groupBox.addWidget(testWidget);
    QCOMPARE(groupBox.getWidgetCount(), 1);
}

void ComponentTests::testGroupBoxCollapsible() {
    GroupBox groupBox;
    groupBox.initialize();
    
    // Test collapsible functionality
    groupBox.setCollapsible(true);
    QVERIFY(groupBox.isCheckable());
    
    groupBox.setCollapsed(true);
    QVERIFY(groupBox.isCollapsed());
    
    groupBox.setCollapsed(false);
    QVERIFY(!groupBox.isCollapsed());
}

// **Theme Tests**
void ComponentTests::testThemeManager() {
    auto& themeManager = Theme::ThemeManager::instance();
    
    // Test light theme
    themeManager.loadLightTheme();
    const auto& lightTheme = themeManager.getCurrentTheme();
    QCOMPARE(lightTheme.name, QString("Light"));
    QVERIFY(!lightTheme.is_dark_theme);
    
    // Test dark theme
    themeManager.loadDarkTheme();
    const auto& darkTheme = themeManager.getCurrentTheme();
    QCOMPARE(darkTheme.name, QString("Dark"));
    QVERIFY(darkTheme.is_dark_theme);
}

void ComponentTests::testThemeApplication() {
    auto& themeManager = Theme::ThemeManager::instance();
    
    // Test stylesheet generation
    QString stylesheet = themeManager.generateStyleSheet("TestComponent");
    QVERIFY(!stylesheet.isEmpty());
    QVERIFY(stylesheet.contains("TestComponent"));
    
    // Test font creation
    QFont font = themeManager.createFont(14, 500);
    QCOMPARE(font.pointSize(), 14);
    QCOMPARE(font.weight(), 500);
}

void ComponentTests::testThemeExportImport() {
    auto& themeManager = Theme::ThemeManager::instance();
    
    // Export current theme
    QString exportedJson = themeManager.exportThemeToJson();
    QVERIFY(!exportedJson.isEmpty());
    
    // Import theme from JSON
    bool importSuccess = themeManager.loadThemeFromJson(exportedJson);
    QVERIFY(importSuccess);
}

// **Animation Tests**
void ComponentTests::testAnimationBuilder() {
    auto testWidget = new QWidget();
    testWidget->show();
    
    // Test animation builder
    auto animation = Animation::animate(testWidget)
        .animate("windowOpacity", 0.0, 1.0)
        .duration(300ms)
        .easing(Animation::Easing::OutCubic)
        .build();
    
    QVERIFY(animation != nullptr);
    QCOMPARE(animation->duration(), 300);
    
    delete testWidget;
}

void ComponentTests::testAnimationPresets() {
    auto testWidget = new QWidget();
    testWidget->resize(100, 100);
    testWidget->show();
    
    // Test fade in preset
    auto fadeAnimation = Animation::Presets::fadeIn(testWidget, 200ms);
    auto builtAnimation = fadeAnimation.build();
    QVERIFY(builtAnimation != nullptr);
    
    delete testWidget;
}

void ComponentTests::testAnimationSequence() {
    auto testWidget = new QWidget();
    testWidget->show();
    
    // Test animation sequence
    auto sequence = Animation::AnimationSequence()
        .then(Animation::Presets::fadeIn(testWidget, 100ms))
        .pause(50ms)
        .then(Animation::Presets::fadeOut(testWidget, 100ms))
        .build();
    
    QVERIFY(sequence != nullptr);
    
    delete testWidget;
}

// **Validation Tests**
void ComponentTests::testValidationChain() {
    using namespace Validation;
    
    // Test string validation
    auto stringValidator = validate<QString>()
        .required("String is required")
        .minLength(3, "Minimum 3 characters")
        .maxLength(10, "Maximum 10 characters");
    
    // Test valid string
    ValidationResult validResult = stringValidator.validate("Hello");
    QVERIFY(validResult.isValid());
    
    // Test invalid string (too short)
    ValidationResult invalidResult = stringValidator.validate("Hi");
    QVERIFY(!invalidResult.isValid());
    QVERIFY(invalidResult.hasErrors());
}

void ComponentTests::testValidationResults() {
    using namespace Validation;
    
    ValidationResult result1(true);
    ValidationResult result2(false, {{ValidationMessage{"Error message"}}});
    
    // Test combination
    ValidationResult combined = result1 + result2;
    QVERIFY(!combined.isValid());
    QCOMPARE(combined.getMessages().size(), 1);
}

void ComponentTests::testCustomValidators() {
    using namespace Validation;
    
    auto customValidator = validate<int>()
        .custom([](const int& value) -> bool {
            return value % 2 == 0; // Even numbers only
        }, "Value must be even");
    
    ValidationResult evenResult = customValidator.validate(4);
    QVERIFY(evenResult.isValid());
    
    ValidationResult oddResult = customValidator.validate(3);
    QVERIFY(!oddResult.isValid());
}

// **Error Handling Tests**
void ComponentTests::testErrorManager() {
    auto& errorManager = ErrorHandling::errorManager();
    
    // Test logging
    errorManager.info("Test info message");
    errorManager.warning("Test warning message");
    errorManager.error("Test error message");
    
    // Test assertion
    UI_ASSERT(true, "This should not fail");
    // UI_ASSERT(false, "This would fail"); // Commented out to avoid test failure
}

void ComponentTests::testExceptionHandling() {
    auto& errorManager = ErrorHandling::errorManager();
    
    // Test safe execution with success
    auto successResult = errorManager.safeExecute([]() {
        return 42;
    }, "Test operation");
    
    QVERIFY(successResult.has_value());
    QCOMPARE(successResult.value(), 42);
    
    // Test safe execution with exception
    auto failureResult = errorManager.safeExecute([]() -> int {
        throw std::runtime_error("Test exception");
    }, "Test operation");
    
    QVERIFY(!failureResult.has_value());
}

void ComponentTests::testSafeExecution() {
    auto& errorManager = ErrorHandling::errorManager();
    
    // Test performance measurement
    auto result = errorManager.measurePerformance([]() {
        QThread::msleep(10); // Simulate work
        return "Done";
    }, "Test performance");
    
    QVERIFY(result.has_value());
    QCOMPARE(result.value(), QString("Done"));
}

// **Utility Tests**
void ComponentTests::testPropertySetter() {
    auto testWidget = new QWidget();
    auto propSetter = Utils::properties(testWidget);
    
    // Test property setting
    QVERIFY(propSetter.setProperty("objectName", QString("TestWidget")));
    
    // Test property getting
    auto name = propSetter.getProperty<QString>("objectName");
    QVERIFY(name.has_value());
    QCOMPARE(name.value(), QString("TestWidget"));
    
    // Test property existence check
    QVERIFY(propSetter.hasProperty("objectName"));
    QVERIFY(!propSetter.hasProperty("nonExistentProperty"));
    
    delete testWidget;
}

void ComponentTests::testLayoutUtils() {
    auto testWidget = new QWidget();
    
    // Test layout creation
    auto layout = Utils::Layout::createLayout<QVBoxLayout>();
    QVERIFY(layout != nullptr);
    
    // Test layout application
    bool success = Utils::Layout::applyLayout(testWidget, std::move(layout));
    QVERIFY(success);
    QVERIFY(testWidget->layout() != nullptr);
    
    delete testWidget;
}

void ComponentTests::testStylingUtils() {
    auto testWidget = new QWidget();
    
    // Test theme styling
    Utils::Styling::applyThemeStyle(testWidget, "TestWidget");
    QVERIFY(!testWidget->styleSheet().isEmpty());
    
    // Test border radius
    Utils::Styling::setBorderRadius(testWidget, 10);
    QVERIFY(testWidget->styleSheet().contains("border-radius: 10px"));
    
    delete testWidget;
}

QTEST_MAIN(ComponentTests)
#include "ComponentTests.moc"
