// tests/unit/test_accessibility.cpp
#include <QtTest/QtTest>
#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <memory>

#include "../../src/Core/Theme.hpp"

using namespace DeclarativeUI::Core;

class AccessibilityTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    // **Accessibility Role Tests**
    void testAccessibilityRoles();

    // **Accessibility State Tests**
    void testAccessibilityState();

    // **Accessibility Properties Tests**
    void testAccessibilityProperties();

    // **Accessibility Manager Tests**
    void testAccessibilityManagerSingleton();
    void testComponentRegistration();
    void testPropertyUpdates();
    void testAccessibilityFeatures();
    void testHighContrastMode();
    void testScreenReaderSupport();
    void testKeyboardNavigation();

    // **Accessibility Builder Tests**
    void testAccessibilityBuilder();
    void testAccessibilityBuilderChaining();

    // **Utility Tests**
    void testAccessibilityUtilities();

private:
    QApplication* app;
    std::unique_ptr<QWidget> testWidget;
};

void AccessibilityTest::initTestCase() {
    if (!QApplication::instance()) {
        int argc = 0;
        char* argv[] = {nullptr};
        app = new QApplication(argc, argv);
    }
}

void AccessibilityTest::cleanupTestCase() {
    if (app) {
        delete app;
        app = nullptr;
    }
}

// **Accessibility Role Tests**
void AccessibilityTest::testAccessibilityRoles() {
    // Test that accessibility roles are defined
    QVERIFY(static_cast<int>(Accessibility::AccessibilityRole::Button) >= 0);
    QVERIFY(static_cast<int>(Accessibility::AccessibilityRole::CheckBox) >= 0);
    QVERIFY(static_cast<int>(Accessibility::AccessibilityRole::ComboBox) >= 0);
    QVERIFY(static_cast<int>(Accessibility::AccessibilityRole::Dialog) >= 0);
    QVERIFY(static_cast<int>(Accessibility::AccessibilityRole::TextEdit) >= 0);
    QVERIFY(static_cast<int>(Accessibility::AccessibilityRole::Window) >= 0);
}

// **Accessibility State Tests**
void AccessibilityTest::testAccessibilityState() {
    Accessibility::AccessibilityState state;
    
    // Test default values
    QVERIFY(state.is_enabled);
    QVERIFY(state.is_visible);
    QVERIFY(!state.is_focused);
    QVERIFY(!state.is_selected);
    QVERIFY(!state.is_checked);
    QVERIFY(!state.is_expanded);
    QVERIFY(!state.is_pressed);
    QVERIFY(!state.is_read_only);
    QVERIFY(!state.is_required);
    QVERIFY(!state.has_popup);
    QVERIFY(!state.is_modal);
    
    // Test state modification
    state.is_enabled = false;
    state.is_focused = true;
    state.is_checked = true;
    
    QVERIFY(!state.is_enabled);
    QVERIFY(state.is_focused);
    QVERIFY(state.is_checked);
}

// **Accessibility Properties Tests**
void AccessibilityTest::testAccessibilityProperties() {
    Accessibility::AccessibilityProperties props;
    
    // Test default values
    QVERIFY(props.name.isEmpty());
    QVERIFY(props.description.isEmpty());
    QVERIFY(props.help_text.isEmpty());
    QVERIFY(props.value.isEmpty());
    QCOMPARE(props.role, Accessibility::AccessibilityRole::Button);
    QCOMPARE(props.tab_index, 0);
    
    // Test property setting
    props.name = "Test Button";
    props.description = "A test button for accessibility";
    props.help_text = "Click to perform test action";
    props.role = Accessibility::AccessibilityRole::Button;
    props.state.is_enabled = true;
    props.tab_index = 1;
    
    QCOMPARE(props.name, QString("Test Button"));
    QCOMPARE(props.description, QString("A test button for accessibility"));
    QCOMPARE(props.help_text, QString("Click to perform test action"));
    QCOMPARE(props.role, Accessibility::AccessibilityRole::Button);
    QVERIFY(props.state.is_enabled);
    QCOMPARE(props.tab_index, 1);
    
    // Test hierarchical relationships
    props.parent_id = "parent_widget";
    props.child_ids = {"child1", "child2"};
    props.label_id = "label_widget";
    props.described_by_id = "description_widget";
    
    QCOMPARE(props.parent_id, QString("parent_widget"));
    QCOMPARE(props.child_ids.size(), 2);
    QCOMPARE(props.child_ids[0], QString("child1"));
    QCOMPARE(props.child_ids[1], QString("child2"));
    QCOMPARE(props.label_id, QString("label_widget"));
    QCOMPARE(props.described_by_id, QString("description_widget"));
    
    // Test range properties
    props.min_value = 0.0;
    props.max_value = 100.0;
    props.current_value = 50.0;
    props.step_value = 1.0;
    
    QVERIFY(props.min_value.has_value());
    QVERIFY(props.max_value.has_value());
    QVERIFY(props.current_value.has_value());
    QVERIFY(props.step_value.has_value());
    QCOMPARE(props.min_value.value(), 0.0);
    QCOMPARE(props.max_value.value(), 100.0);
    QCOMPARE(props.current_value.value(), 50.0);
    QCOMPARE(props.step_value.value(), 1.0);
}

// **Accessibility Manager Tests**
void AccessibilityTest::testAccessibilityManagerSingleton() {
    Accessibility::AccessibilityManager& manager1 = Accessibility::AccessibilityManager::instance();
    Accessibility::AccessibilityManager& manager2 = Accessibility::AccessibilityManager::instance();
    
    QCOMPARE(&manager1, &manager2);
}

void AccessibilityTest::testComponentRegistration() {
    testWidget = std::make_unique<QWidget>();
    auto& manager = Accessibility::AccessibilityManager::instance();
    
    Accessibility::AccessibilityProperties props;
    props.name = "Test Widget";
    props.description = "Widget for testing accessibility";
    props.role = Accessibility::AccessibilityRole::Window;
    
    // Test component registration
    manager.registerComponent(testWidget.get(), props);
    
    // Test property retrieval
    auto retrievedProps = manager.getProperties(testWidget.get());
    QVERIFY(retrievedProps.has_value());
    QCOMPARE(retrievedProps->name, QString("Test Widget"));
    QCOMPARE(retrievedProps->description, QString("Widget for testing accessibility"));
    QCOMPARE(retrievedProps->role, Accessibility::AccessibilityRole::Window);
}

void AccessibilityTest::testPropertyUpdates() {
    testWidget = std::make_unique<QWidget>();
    auto& manager = Accessibility::AccessibilityManager::instance();
    
    // Register component
    Accessibility::AccessibilityProperties initialProps;
    initialProps.name = "Initial Name";
    manager.registerComponent(testWidget.get(), initialProps);
    
    // Update properties
    Accessibility::AccessibilityProperties updatedProps;
    updatedProps.name = "Updated Name";
    updatedProps.description = "Updated description";
    manager.updateProperties(testWidget.get(), updatedProps);
    
    // Verify update
    auto retrievedProps = manager.getProperties(testWidget.get());
    QVERIFY(retrievedProps.has_value());
    QCOMPARE(retrievedProps->name, QString("Updated Name"));
    QCOMPARE(retrievedProps->description, QString("Updated description"));
}

void AccessibilityTest::testAccessibilityFeatures() {
    auto& manager = Accessibility::AccessibilityManager::instance();
    
    // Test enabling/disabling accessibility
    manager.setAccessibilityEnabled(true);
    QVERIFY(manager.isAccessibilityEnabled());
    
    manager.setAccessibilityEnabled(false);
    QVERIFY(!manager.isAccessibilityEnabled());
    
    // Re-enable for other tests
    manager.setAccessibilityEnabled(true);
}

void AccessibilityTest::testHighContrastMode() {
    auto& manager = Accessibility::AccessibilityManager::instance();
    
    // Test high contrast mode
    manager.setHighContrastMode(true);
    QVERIFY(manager.isHighContrastEnabled());
    
    manager.setHighContrastMode(false);
    QVERIFY(!manager.isHighContrastEnabled());
}

void AccessibilityTest::testScreenReaderSupport() {
    auto& manager = Accessibility::AccessibilityManager::instance();
    
    // Test screen reader announcements
    manager.announceToScreenReader("Test announcement", 1);
    manager.announceToScreenReader("High priority announcement", 5);
    
    // Test passes if no exceptions are thrown
    QVERIFY(true);
}

void AccessibilityTest::testKeyboardNavigation() {
    auto& manager = Accessibility::AccessibilityManager::instance();
    
    // Test keyboard navigation settings
    manager.setKeyboardNavigationEnabled(true);
    QVERIFY(manager.isKeyboardNavigationEnabled());
    
    manager.setKeyboardNavigationEnabled(false);
    QVERIFY(!manager.isKeyboardNavigationEnabled());
    
    // Re-enable for other tests
    manager.setKeyboardNavigationEnabled(true);
}

// **Accessibility Builder Tests**
void AccessibilityTest::testAccessibilityBuilder() {
    testWidget = std::make_unique<QWidget>();
    
    auto builder = Accessibility::accessibilityFor();
    
    // Test builder methods
    auto props = builder
        .name("Test Button")
        .description("A button for testing")
        .helpText("Click to test")
        .role(Accessibility::AccessibilityRole::Button)
        .enabled(true)
        .required(false)
        .tabIndex(2)
        .build();
    
    QCOMPARE(props.name, QString("Test Button"));
    QCOMPARE(props.description, QString("A button for testing"));
    QCOMPARE(props.help_text, QString("Click to test"));
    QCOMPARE(props.role, Accessibility::AccessibilityRole::Button);
    QVERIFY(props.state.is_enabled);
    QVERIFY(!props.state.is_required);
    QCOMPARE(props.tab_index, 2);
}

void AccessibilityTest::testAccessibilityBuilderChaining() {
    testWidget = std::make_unique<QWidget>();
    
    // Test method chaining
    auto builder = Accessibility::accessibilityFor()
        .name("Chained Button")
        .description("Testing method chaining")
        .role(Accessibility::AccessibilityRole::Button)
        .enabled(true)
        .tabIndex(3);
    
    // Test that builder can be applied to component
    builder.applyTo(testWidget.get());
    
    // Verify that properties were applied
    auto& manager = Accessibility::AccessibilityManager::instance();
    auto props = manager.getProperties(testWidget.get());
    QVERIFY(props.has_value());
    QCOMPARE(props->name, QString("Chained Button"));
    QCOMPARE(props->description, QString("Testing method chaining"));
    QCOMPARE(props->role, Accessibility::AccessibilityRole::Button);
    QCOMPARE(props->tab_index, 3);
}

// **Utility Tests**
void AccessibilityTest::testAccessibilityUtilities() {
    // Test accessibility utility function
    auto& manager = Accessibility::accessibility();
    QCOMPARE(&manager, &Accessibility::AccessibilityManager::instance());
    
    // Test accessibility builder factory
    auto builder = Accessibility::accessibilityFor();
    auto props = builder.name("Utility Test").build();
    QCOMPARE(props.name, QString("Utility Test"));
}

QTEST_MAIN(AccessibilityTest)
#include "test_accessibility.moc"
