#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QSlider>
#include <QSpinBox>
#include <QTabWidget>
#include <QTextEdit>
#include <QtTest/QtTest>
#include <memory>

#include "../../src/Command/CommandSystem.hpp"
#include "../../src/Command/ComponentCommands.hpp"

using namespace DeclarativeUI::Command;
using namespace DeclarativeUI::Command::ComponentCommands;

class ComponentCommandsTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Button component tests
    void testButtonCommand_click();
    void testButtonCommand_setText();
    void testButtonCommand_setEnabled();

    // CheckBox component tests
    void testCheckBoxCommand_toggle();
    void testCheckBoxCommand_setChecked();
    void testCheckBoxCommand_undo();

    // RadioButton component tests
    void testRadioButtonCommand_select();
    void testRadioButtonCommand_undo();

    // Label component tests
    void testLabelCommand_setText();
    void testLabelCommand_clear();
    void testLabelCommand_undo();

    // LineEdit component tests
    void testLineEditCommand_setText();
    void testLineEditCommand_clear();
    void testLineEditCommand_selectAll();
    void testLineEditCommand_undo();

    // TextEdit component tests
    void testTextEditCommand_setText();
    void testTextEditCommand_append();
    void testTextEditCommand_clear();
    void testTextEditCommand_undo();

    // SpinBox component tests
    void testSpinBoxCommand_setValue();
    void testSpinBoxCommand_stepUp();
    void testSpinBoxCommand_stepDown();
    void testSpinBoxCommand_undo();

    // Slider component tests
    void testSliderCommand_setValue();
    void testSliderCommand_undo();

    // ComboBox component tests
    void testComboBoxCommand_setCurrentIndex();
    void testComboBoxCommand_setCurrentText();
    void testComboBoxCommand_addItem();
    void testComboBoxCommand_undo();

private:
    QWidget* testWidget;
    QPushButton* testButton;
    QCheckBox* testCheckBox;
    QRadioButton* testRadioButton;
    QLabel* testLabel;
    QLineEdit* testLineEdit;
    QTextEdit* testTextEdit;
    QSpinBox* testSpinBox;
    QSlider* testSlider;
    QComboBox* testComboBox;
    QTabWidget* testTabWidget;
};

void ComponentCommandsTest::initTestCase() {
    // Initialize test widgets
    testWidget = new QWidget();
    testWidget->setObjectName("testWidget");

    testButton = new QPushButton("Test Button", testWidget);
    testButton->setObjectName("testButton");

    testCheckBox = new QCheckBox("Test CheckBox", testWidget);
    testCheckBox->setObjectName("testCheckBox");

    testRadioButton = new QRadioButton("Test RadioButton", testWidget);
    testRadioButton->setObjectName("testRadioButton");

    testLabel = new QLabel("Test Label", testWidget);
    testLabel->setObjectName("testLabel");

    testLineEdit = new QLineEdit("Test LineEdit", testWidget);
    testLineEdit->setObjectName("testLineEdit");

    testTextEdit = new QTextEdit("Test TextEdit", testWidget);
    testTextEdit->setObjectName("testTextEdit");

    testSpinBox = new QSpinBox(testWidget);
    testSpinBox->setObjectName("testSpinBox");
    testSpinBox->setRange(0, 100);
    testSpinBox->setValue(50);

    testSlider = new QSlider(Qt::Horizontal, testWidget);
    testSlider->setObjectName("testSlider");
    testSlider->setRange(0, 100);
    testSlider->setValue(50);

    testComboBox = new QComboBox(testWidget);
    testComboBox->setObjectName("testComboBox");
    testComboBox->addItems({"Item 1", "Item 2", "Item 3"});

    testTabWidget = new QTabWidget(testWidget);
    testTabWidget->setObjectName("testTabWidget");
    testTabWidget->addTab(new QWidget(), "Tab 1");
    testTabWidget->addTab(new QWidget(), "Tab 2");
}

void ComponentCommandsTest::cleanupTestCase() { delete testWidget; }

void ComponentCommandsTest::init() {
    // Reset widget states before each test
    testButton->setText("Test Button");
    testButton->setEnabled(true);
    testCheckBox->setChecked(false);
    testRadioButton->setChecked(false);
    testLabel->setText("Test Label");
    testLineEdit->setText("Test LineEdit");
    testTextEdit->setPlainText("Test TextEdit");
    testSpinBox->setValue(50);
    testSlider->setValue(50);
    testComboBox->setCurrentIndex(0);
    testTabWidget->setCurrentIndex(0);
}

void ComponentCommandsTest::cleanup() {
    // Cleanup after each test if needed
}

// ============================================================================
// BUTTON COMPONENT TESTS
// ============================================================================

void ComponentCommandsTest::testButtonCommand_click() {
    CommandContext context;
    context.setParameter("widget", QString("testButton"));
    context.setParameter("operation", QString("click"));

    ButtonCommand command(context);
    auto result = command.execute(context);

    QVERIFY(result.isSuccess());
    QCOMPARE(result.getResult().toString(),
             QString("Button clicked successfully"));
}

void ComponentCommandsTest::testButtonCommand_setText() {
    CommandContext context;
    context.setParameter("widget", QString("testButton"));
    context.setParameter("operation", QString("setText"));
    context.setParameter("text", QString("New Button Text"));

    ButtonCommand command(context);
    auto result = command.execute(context);

    QVERIFY(result.isSuccess());
    QCOMPARE(testButton->text(), QString("New Button Text"));
}

void ComponentCommandsTest::testButtonCommand_setEnabled() {
    CommandContext context;
    context.setParameter("widget", QString("testButton"));
    context.setParameter("operation", QString("setEnabled"));
    context.setParameter("enabled", false);

    ButtonCommand command(context);
    auto result = command.execute(context);

    QVERIFY(result.isSuccess());
    QVERIFY(!testButton->isEnabled());
}

// ============================================================================
// CHECKBOX COMPONENT TESTS
// ============================================================================

void ComponentCommandsTest::testCheckBoxCommand_toggle() {
    CommandContext context;
    context.setParameter("widget", QString("testCheckBox"));
    context.setParameter("operation", QString("toggle"));

    CheckBoxCommand command(context);
    bool initialState = testCheckBox->isChecked();
    auto result = command.execute(context);

    QVERIFY(result.isSuccess());
    QCOMPARE(testCheckBox->isChecked(), !initialState);
}

void ComponentCommandsTest::testCheckBoxCommand_setChecked() {
    CommandContext context;
    context.setParameter("widget", QString("testCheckBox"));
    context.setParameter("operation", QString("setChecked"));
    context.setParameter("checked", true);

    CheckBoxCommand command(context);
    auto result = command.execute(context);

    QVERIFY(result.isSuccess());
    QVERIFY(testCheckBox->isChecked());
}

void ComponentCommandsTest::testCheckBoxCommand_undo() {
    CommandContext context;
    context.setParameter("widget", QString("testCheckBox"));
    context.setParameter("operation", QString("toggle"));

    CheckBoxCommand command(context);
    bool initialState = testCheckBox->isChecked();

    // Execute command
    command.execute(context);
    QCOMPARE(testCheckBox->isChecked(), !initialState);

    // Undo command
    auto undoResult = command.undo(context);
    QVERIFY(undoResult.isSuccess());
    QCOMPARE(testCheckBox->isChecked(), initialState);
}

// ============================================================================
// RADIO BUTTON COMPONENT TESTS
// ============================================================================

void ComponentCommandsTest::testRadioButtonCommand_select() {
    CommandContext context;
    context.setParameter("widget", QString("testRadioButton"));
    context.setParameter("operation", QString("select"));

    RadioButtonCommand command(context);
    auto result = command.execute(context);

    QVERIFY(result.isSuccess());
    QVERIFY(testRadioButton->isChecked());
}

void ComponentCommandsTest::testRadioButtonCommand_undo() {
    CommandContext context;
    context.setParameter("widget", QString("testRadioButton"));
    context.setParameter("operation", QString("select"));

    RadioButtonCommand command(context);
    bool initialState = testRadioButton->isChecked();

    // Execute command
    command.execute(context);
    QVERIFY(testRadioButton->isChecked());

    // Undo command
    auto undoResult = command.undo(context);
    QVERIFY(undoResult.isSuccess());
    QCOMPARE(testRadioButton->isChecked(), initialState);
}

QTEST_MAIN(ComponentCommandsTest)
#include "test_component_commands.moc"
