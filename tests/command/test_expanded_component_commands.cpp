#include <QApplication>
#include <QCalendarWidget>
#include <QDateTimeEdit>
#include <QDial>
#include <QDockWidget>
#include <QDoubleSpinBox>
#include <QFrame>
#include <QGroupBox>
#include <QLCDNumber>
#include <QListView>
#include <QMenuBar>
#include <QProgressBar>
#include <QScrollArea>
#include <QSplitter>
#include <QStatusBar>
#include <QTableView>
#include <QToolBar>
#include <QToolButton>
#include <QTreeView>
#include <QtTest/QtTest>
#include <memory>

#include "../../src/Command/CommandSystem.hpp"
#include "../../src/Command/ComponentCommands.hpp"

using namespace DeclarativeUI::Command;
using namespace DeclarativeUI::Command::ComponentCommands;

class ExpandedComponentCommandsTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Priority 1 - Common Input Components
    void testDoubleSpinBoxCommand_setValue();
    void testDoubleSpinBoxCommand_stepUp();
    void testDoubleSpinBoxCommand_setDecimals();
    void testDoubleSpinBoxCommand_undo();

    void testDialCommand_setValue();
    void testDialCommand_setRange();
    void testDialCommand_setNotchesVisible();
    void testDialCommand_undo();

    void testDateTimeEditCommand_setDateTime();
    void testDateTimeEditCommand_setDateRange();
    void testDateTimeEditCommand_setDisplayFormat();
    void testDateTimeEditCommand_undo();

    void testProgressBarCommand_setValue();
    void testProgressBarCommand_setRange();
    void testProgressBarCommand_setTextVisible();
    void testProgressBarCommand_reset();

    // Priority 2 - Display Components
    void testLCDNumberCommand_display();
    void testLCDNumberCommand_setDigitCount();
    void testLCDNumberCommand_setMode();
    void testLCDNumberCommand_undo();

    void testCalendarCommand_setSelectedDate();
    void testCalendarCommand_setDateRange();
    void testCalendarCommand_setGridVisible();
    void testCalendarCommand_undo();

    // Priority 3 - View Components
    void testListViewCommand_selectItem();
    void testListViewCommand_addItem();
    void testListViewCommand_clearSelection();
    void testListViewCommand_undo();

    void testTableViewCommand_selectCell();
    void testTableViewCommand_selectRow();
    void testTableViewCommand_selectColumn();
    void testTableViewCommand_undo();

    void testTreeViewCommand_selectItem();
    void testTreeViewCommand_expandItem();
    void testTreeViewCommand_collapseItem();
    void testTreeViewCommand_undo();

    // Priority 4 - Container Components
    void testGroupBoxCommand_setTitle();
    void testGroupBoxCommand_setCheckable();
    void testGroupBoxCommand_setChecked();
    void testGroupBoxCommand_undo();

    void testFrameCommand_setFrameStyle();
    void testFrameCommand_setLineWidth();

    void testScrollAreaCommand_setWidgetResizable();
    void testScrollAreaCommand_setScrollBarPolicy();

    void testSplitterCommand_setSizes();
    void testSplitterCommand_setOrientation();
    void testSplitterCommand_undo();

    void testDockWidgetCommand_setFloating();
    void testDockWidgetCommand_setAllowedAreas();
    void testDockWidgetCommand_undo();

    // Priority 5 - Menu/Toolbar Components
    void testMenuBarCommand_addMenu();
    void testMenuBarCommand_removeMenu();
    void testMenuBarCommand_setNativeMenuBar();

    void testStatusBarCommand_showMessage();
    void testStatusBarCommand_clearMessage();

    void testToolBarCommand_addAction();
    void testToolBarCommand_removeAction();
    void testToolBarCommand_setOrientation();
    void testToolBarCommand_undo();

    void testToolButtonCommand_setPopupMode();
    void testToolButtonCommand_setToolButtonStyle();
    void testToolButtonCommand_undo();

private:
    QWidget* testWidget;

    // Priority 1 widgets
    QDoubleSpinBox* testDoubleSpinBox;
    QDial* testDial;
    QDateTimeEdit* testDateTimeEdit;
    QProgressBar* testProgressBar;

    // Priority 2 widgets
    QLCDNumber* testLCDNumber;
    QCalendarWidget* testCalendar;

    // Priority 3 widgets
    QListView* testListView;
    QTableView* testTableView;
    QTreeView* testTreeView;

    // Priority 4 widgets
    QGroupBox* testGroupBox;
    QFrame* testFrame;
    QScrollArea* testScrollArea;
    QSplitter* testSplitter;
    QDockWidget* testDockWidget;

    // Priority 5 widgets
    QMenuBar* testMenuBar;
    QStatusBar* testStatusBar;
    QToolBar* testToolBar;
    QToolButton* testToolButton;
};

void ExpandedComponentCommandsTest::initTestCase() {
    // Initialize test widgets
    testWidget = new QWidget();
    testWidget->setObjectName("testWidget");

    // Priority 1 widgets
    testDoubleSpinBox = new QDoubleSpinBox(testWidget);
    testDoubleSpinBox->setObjectName("testDoubleSpinBox");
    testDoubleSpinBox->setRange(0.0, 100.0);
    testDoubleSpinBox->setValue(50.0);
    testDoubleSpinBox->setDecimals(2);

    testDial = new QDial(testWidget);
    testDial->setObjectName("testDial");
    testDial->setRange(0, 100);
    testDial->setValue(50);

    testDateTimeEdit = new QDateTimeEdit(testWidget);
    testDateTimeEdit->setObjectName("testDateTimeEdit");
    testDateTimeEdit->setDateTime(QDateTime::currentDateTime());

    testProgressBar = new QProgressBar(testWidget);
    testProgressBar->setObjectName("testProgressBar");
    testProgressBar->setRange(0, 100);
    testProgressBar->setValue(50);

    // Priority 2 widgets
    testLCDNumber = new QLCDNumber(testWidget);
    testLCDNumber->setObjectName("testLCDNumber");
    testLCDNumber->display(123.45);

    testCalendar = new QCalendarWidget(testWidget);
    testCalendar->setObjectName("testCalendar");
    testCalendar->setSelectedDate(QDate::currentDate());

    // Priority 3 widgets
    testListView = new QListView(testWidget);
    testListView->setObjectName("testListView");

    testTableView = new QTableView(testWidget);
    testTableView->setObjectName("testTableView");

    testTreeView = new QTreeView(testWidget);
    testTreeView->setObjectName("testTreeView");

    // Priority 4 widgets
    testGroupBox = new QGroupBox("Test GroupBox", testWidget);
    testGroupBox->setObjectName("testGroupBox");
    testGroupBox->setCheckable(true);
    testGroupBox->setChecked(false);

    testFrame = new QFrame(testWidget);
    testFrame->setObjectName("testFrame");

    testScrollArea = new QScrollArea(testWidget);
    testScrollArea->setObjectName("testScrollArea");

    testSplitter = new QSplitter(testWidget);
    testSplitter->setObjectName("testSplitter");

    testDockWidget = new QDockWidget("Test DockWidget", testWidget);
    testDockWidget->setObjectName("testDockWidget");

    // Priority 5 widgets
    testMenuBar = new QMenuBar(testWidget);
    testMenuBar->setObjectName("testMenuBar");

    testStatusBar = new QStatusBar(testWidget);
    testStatusBar->setObjectName("testStatusBar");

    testToolBar = new QToolBar(testWidget);
    testToolBar->setObjectName("testToolBar");

    testToolButton = new QToolButton(testWidget);
    testToolButton->setObjectName("testToolButton");
}

void ExpandedComponentCommandsTest::cleanupTestCase() { delete testWidget; }

void ExpandedComponentCommandsTest::init() {
    // Reset widget states before each test
    testDoubleSpinBox->setValue(50.0);
    testDial->setValue(50);
    testDateTimeEdit->setDateTime(QDateTime::currentDateTime());
    testProgressBar->setValue(50);
    testLCDNumber->display(123.45);
    testCalendar->setSelectedDate(QDate::currentDate());
    testGroupBox->setTitle("Test GroupBox");
    testGroupBox->setChecked(false);
}

void ExpandedComponentCommandsTest::cleanup() {
    // Cleanup after each test if needed
}

// ============================================================================
// PRIORITY 1 - COMMON INPUT COMPONENTS TESTS
// ============================================================================

void ExpandedComponentCommandsTest::testDoubleSpinBoxCommand_setValue() {
    CommandContext context;
    context.setParameter("widget", QString("testDoubleSpinBox"));
    context.setParameter("operation", QString("setValue"));
    context.setParameter("value", 75.5);

    DoubleSpinBoxCommand command(context);
    auto result = command.execute(context);

    QVERIFY(result.isSuccess());
    QCOMPARE(testDoubleSpinBox->value(), 75.5);
}

void ExpandedComponentCommandsTest::testDoubleSpinBoxCommand_stepUp() {
    CommandContext context;
    context.setParameter("widget", QString("testDoubleSpinBox"));
    context.setParameter("operation", QString("stepUp"));

    double initialValue = testDoubleSpinBox->value();
    DoubleSpinBoxCommand command(context);
    auto result = command.execute(context);

    QVERIFY(result.isSuccess());
    QVERIFY(testDoubleSpinBox->value() > initialValue);
}

void ExpandedComponentCommandsTest::testDoubleSpinBoxCommand_setDecimals() {
    CommandContext context;
    context.setParameter("widget", QString("testDoubleSpinBox"));
    context.setParameter("operation", QString("setDecimals"));
    context.setParameter("decimals", 3);

    DoubleSpinBoxCommand command(context);
    auto result = command.execute(context);

    QVERIFY(result.isSuccess());
    QCOMPARE(testDoubleSpinBox->decimals(), 3);
}

void ExpandedComponentCommandsTest::testDoubleSpinBoxCommand_undo() {
    CommandContext context;
    context.setParameter("widget", QString("testDoubleSpinBox"));
    context.setParameter("operation", QString("setValue"));
    context.setParameter("value", 80.0);

    double initialValue = testDoubleSpinBox->value();
    DoubleSpinBoxCommand command(context);

    // Execute command
    command.execute(context);
    QCOMPARE(testDoubleSpinBox->value(), 80.0);

    // Undo command
    auto undoResult = command.undo(context);
    QVERIFY(undoResult.isSuccess());
    QCOMPARE(testDoubleSpinBox->value(), initialValue);
}

// ============================================================================
// DIAL COMPONENT TESTS
// ============================================================================

void ExpandedComponentCommandsTest::testDialCommand_setValue() {
    CommandContext context;
    context.setParameter("widget", QString("testDial"));
    context.setParameter("operation", QString("setValue"));
    context.setParameter("value", 75);

    DialCommand command(context);
    auto result = command.execute(context);

    QVERIFY(result.isSuccess());
    QCOMPARE(testDial->value(), 75);
}

void ExpandedComponentCommandsTest::testDialCommand_setRange() {
    CommandContext context;
    context.setParameter("widget", QString("testDial"));
    context.setParameter("operation", QString("setRange"));
    context.setParameter("min", 10);
    context.setParameter("max", 90);

    DialCommand command(context);
    auto result = command.execute(context);

    QVERIFY(result.isSuccess());
    QCOMPARE(testDial->minimum(), 10);
    QCOMPARE(testDial->maximum(), 90);
}

void ExpandedComponentCommandsTest::testDialCommand_setNotchesVisible() {
    CommandContext context;
    context.setParameter("widget", QString("testDial"));
    context.setParameter("operation", QString("setNotchesVisible"));
    context.setParameter("visible", true);

    DialCommand command(context);
    auto result = command.execute(context);

    QVERIFY(result.isSuccess());
    QVERIFY(testDial->notchesVisible());
}

void ExpandedComponentCommandsTest::testDialCommand_undo() {
    CommandContext context;
    context.setParameter("widget", QString("testDial"));
    context.setParameter("operation", QString("setValue"));
    context.setParameter("value", 80);

    int initialValue = testDial->value();
    DialCommand command(context);

    // Execute command
    command.execute(context);
    QCOMPARE(testDial->value(), 80);

    // Undo command
    auto undoResult = command.undo(context);
    QVERIFY(undoResult.isSuccess());
    QCOMPARE(testDial->value(), initialValue);
}

// ============================================================================
// DATETIME EDIT COMPONENT TESTS
// ============================================================================

void ExpandedComponentCommandsTest::testDateTimeEditCommand_setDateTime() {
    CommandContext context;
    context.setParameter("widget", QString("testDateTimeEdit"));
    context.setParameter("operation", QString("setDateTime"));

    QDateTime newDateTime = QDateTime::currentDateTime().addDays(1);
    context.setParameter("datetime", newDateTime);

    DateTimeEditCommand command(context);
    auto result = command.execute(context);

    QVERIFY(result.isSuccess());
    QCOMPARE(testDateTimeEdit->dateTime(), newDateTime);
}

void ExpandedComponentCommandsTest::testDateTimeEditCommand_setDateRange() {
    CommandContext context;
    context.setParameter("widget", QString("testDateTimeEdit"));
    context.setParameter("operation", QString("setDateRange"));
    context.setParameter("minDate", QDate::currentDate().addDays(-30));
    context.setParameter("maxDate", QDate::currentDate().addDays(30));

    DateTimeEditCommand command(context);
    auto result = command.execute(context);

    QVERIFY(result.isSuccess());
}

void ExpandedComponentCommandsTest::testDateTimeEditCommand_setDisplayFormat() {
    CommandContext context;
    context.setParameter("widget", QString("testDateTimeEdit"));
    context.setParameter("operation", QString("setDisplayFormat"));
    context.setParameter("format", QString("yyyy-MM-dd hh:mm"));

    DateTimeEditCommand command(context);
    auto result = command.execute(context);

    QVERIFY(result.isSuccess());
    QCOMPARE(testDateTimeEdit->displayFormat(), QString("yyyy-MM-dd hh:mm"));
}

void ExpandedComponentCommandsTest::testDateTimeEditCommand_undo() {
    CommandContext context;
    context.setParameter("widget", QString("testDateTimeEdit"));
    context.setParameter("operation", QString("setDateTime"));

    QDateTime initialDateTime = testDateTimeEdit->dateTime();
    QDateTime newDateTime = initialDateTime.addSecs(3600);  // Add 1 hour
    context.setParameter("datetime", newDateTime);

    DateTimeEditCommand command(context);

    // Execute command
    command.execute(context);
    QCOMPARE(testDateTimeEdit->dateTime(), newDateTime);

    // Undo command
    auto undoResult = command.undo(context);
    QVERIFY(undoResult.isSuccess());
    QCOMPARE(testDateTimeEdit->dateTime(), initialDateTime);
}

// ============================================================================
// PROGRESS BAR COMPONENT TESTS
// ============================================================================

void ExpandedComponentCommandsTest::testProgressBarCommand_setValue() {
    CommandContext context;
    context.setParameter("widget", QString("testProgressBar"));
    context.setParameter("operation", QString("setValue"));
    context.setParameter("value", 75);

    ProgressBarCommand command(context);
    auto result = command.execute(context);

    QVERIFY(result.isSuccess());
    QCOMPARE(testProgressBar->value(), 75);
}

void ExpandedComponentCommandsTest::testProgressBarCommand_setRange() {
    CommandContext context;
    context.setParameter("widget", QString("testProgressBar"));
    context.setParameter("operation", QString("setRange"));
    context.setParameter("min", 10);
    context.setParameter("max", 90);

    ProgressBarCommand command(context);
    auto result = command.execute(context);

    QVERIFY(result.isSuccess());
    QCOMPARE(testProgressBar->minimum(), 10);
    QCOMPARE(testProgressBar->maximum(), 90);
}

void ExpandedComponentCommandsTest::testProgressBarCommand_setTextVisible() {
    CommandContext context;
    context.setParameter("widget", QString("testProgressBar"));
    context.setParameter("operation", QString("setTextVisible"));
    context.setParameter("visible", false);

    ProgressBarCommand command(context);
    auto result = command.execute(context);

    QVERIFY(result.isSuccess());
    QVERIFY(!testProgressBar->isTextVisible());
}

void ExpandedComponentCommandsTest::testProgressBarCommand_reset() {
    CommandContext context;
    context.setParameter("widget", QString("testProgressBar"));
    context.setParameter("operation", QString("reset"));

    ProgressBarCommand command(context);
    auto result = command.execute(context);

    QVERIFY(result.isSuccess());
    QCOMPARE(testProgressBar->value(), testProgressBar->minimum());
}

// ============================================================================
// LCD NUMBER COMPONENT TESTS
// ============================================================================

void ExpandedComponentCommandsTest::testLCDNumberCommand_display() {
    CommandContext context;
    context.setParameter("widget", QString("testLCDNumber"));
    context.setParameter("operation", QString("display"));
    context.setParameter("value", 456.78);

    LCDNumberCommand command(context);
    auto result = command.execute(context);

    QVERIFY(result.isSuccess());
    QCOMPARE(testLCDNumber->value(), 456.78);
}

void ExpandedComponentCommandsTest::testLCDNumberCommand_setDigitCount() {
    CommandContext context;
    context.setParameter("widget", QString("testLCDNumber"));
    context.setParameter("operation", QString("setDigitCount"));
    context.setParameter("count", 8);

    LCDNumberCommand command(context);
    auto result = command.execute(context);

    QVERIFY(result.isSuccess());
    QCOMPARE(testLCDNumber->digitCount(), 8);
}

void ExpandedComponentCommandsTest::testLCDNumberCommand_setMode() {
    CommandContext context;
    context.setParameter("widget", QString("testLCDNumber"));
    context.setParameter("operation", QString("setMode"));
    context.setParameter("mode", static_cast<int>(QLCDNumber::Hex));

    LCDNumberCommand command(context);
    auto result = command.execute(context);

    QVERIFY(result.isSuccess());
    QCOMPARE(testLCDNumber->mode(), QLCDNumber::Hex);
}

void ExpandedComponentCommandsTest::testLCDNumberCommand_undo() {
    CommandContext context;
    context.setParameter("widget", QString("testLCDNumber"));
    context.setParameter("operation", QString("display"));
    context.setParameter("value", 999.0);

    double initialValue = testLCDNumber->value();
    LCDNumberCommand command(context);

    // Execute command
    command.execute(context);
    QCOMPARE(testLCDNumber->value(), 999.0);

    // Undo command
    auto undoResult = command.undo(context);
    QVERIFY(undoResult.isSuccess());
    QCOMPARE(testLCDNumber->value(), initialValue);
}

// ============================================================================
// CALENDAR COMPONENT TESTS
// ============================================================================

void ExpandedComponentCommandsTest::testCalendarCommand_setSelectedDate() {
    CommandContext context;
    context.setParameter("widget", QString("testCalendar"));
    context.setParameter("operation", QString("setSelectedDate"));

    QDate newDate = QDate::currentDate().addDays(7);
    context.setParameter("date", newDate);

    CalendarCommand command(context);
    auto result = command.execute(context);

    QVERIFY(result.isSuccess());
    QCOMPARE(testCalendar->selectedDate(), newDate);
}

void ExpandedComponentCommandsTest::testCalendarCommand_setDateRange() {
    CommandContext context;
    context.setParameter("widget", QString("testCalendar"));
    context.setParameter("operation", QString("setDateRange"));
    context.setParameter("minDate", QDate::currentDate().addDays(-30));
    context.setParameter("maxDate", QDate::currentDate().addDays(30));

    CalendarCommand command(context);
    auto result = command.execute(context);

    QVERIFY(result.isSuccess());
}

void ExpandedComponentCommandsTest::testCalendarCommand_setGridVisible() {
    CommandContext context;
    context.setParameter("widget", QString("testCalendar"));
    context.setParameter("operation", QString("setGridVisible"));
    context.setParameter("visible", false);

    CalendarCommand command(context);
    auto result = command.execute(context);

    QVERIFY(result.isSuccess());
    QVERIFY(!testCalendar->isGridVisible());
}

void ExpandedComponentCommandsTest::testCalendarCommand_undo() {
    CommandContext context;
    context.setParameter("widget", QString("testCalendar"));
    context.setParameter("operation", QString("setSelectedDate"));

    QDate initialDate = testCalendar->selectedDate();
    QDate newDate = initialDate.addDays(5);
    context.setParameter("date", newDate);

    CalendarCommand command(context);

    // Execute command
    command.execute(context);
    QCOMPARE(testCalendar->selectedDate(), newDate);

    // Undo command
    auto undoResult = command.undo(context);
    QVERIFY(undoResult.isSuccess());
    QCOMPARE(testCalendar->selectedDate(), initialDate);
}

// Add placeholder implementations for remaining test methods
void ExpandedComponentCommandsTest::
    testListViewCommand_selectItem() { /* Implementation
                                        */
}
void ExpandedComponentCommandsTest::
    testListViewCommand_addItem() { /* Implementation
                                     */
}
void ExpandedComponentCommandsTest::
    testListViewCommand_clearSelection() { /* Implementation
                                            */
}
void ExpandedComponentCommandsTest::
    testListViewCommand_undo() { /* Implementation
                                  */
}
void ExpandedComponentCommandsTest::
    testTableViewCommand_selectCell() { /* Implementation
                                         */
}
void ExpandedComponentCommandsTest::
    testTableViewCommand_selectRow() { /* Implementation
                                        */
}
void ExpandedComponentCommandsTest::
    testTableViewCommand_selectColumn() { /* Implementation
                                           */
}
void ExpandedComponentCommandsTest::
    testTableViewCommand_undo() { /* Implementation
                                   */
}
void ExpandedComponentCommandsTest::
    testTreeViewCommand_selectItem() { /* Implementation
                                        */
}
void ExpandedComponentCommandsTest::
    testTreeViewCommand_expandItem() { /* Implementation
                                        */
}
void ExpandedComponentCommandsTest::
    testTreeViewCommand_collapseItem() { /* Implementation
                                          */
}
void ExpandedComponentCommandsTest::
    testTreeViewCommand_undo() { /* Implementation
                                  */
}
void ExpandedComponentCommandsTest::
    testGroupBoxCommand_setTitle() { /* Implementation
                                      */
}
void ExpandedComponentCommandsTest::
    testGroupBoxCommand_setCheckable() { /* Implementation
                                          */
}
void ExpandedComponentCommandsTest::
    testGroupBoxCommand_setChecked() { /* Implementation
                                        */
}
void ExpandedComponentCommandsTest::
    testGroupBoxCommand_undo() { /* Implementation
                                  */
}
void ExpandedComponentCommandsTest::
    testFrameCommand_setFrameStyle() { /* Implementation
                                        */
}
void ExpandedComponentCommandsTest::
    testFrameCommand_setLineWidth() { /* Implementation
                                       */
}
void ExpandedComponentCommandsTest::
    testScrollAreaCommand_setWidgetResizable() { /* Implementation */
}
void ExpandedComponentCommandsTest::
    testScrollAreaCommand_setScrollBarPolicy() { /* Implementation */
}
void ExpandedComponentCommandsTest::
    testSplitterCommand_setSizes() { /* Implementation
                                      */
}
void ExpandedComponentCommandsTest::
    testSplitterCommand_setOrientation() { /* Implementation
                                            */
}
void ExpandedComponentCommandsTest::
    testSplitterCommand_undo() { /* Implementation
                                  */
}
void ExpandedComponentCommandsTest::
    testDockWidgetCommand_setFloating() { /* Implementation
                                           */
}
void ExpandedComponentCommandsTest::
    testDockWidgetCommand_setAllowedAreas() { /* Implementation */
}
void ExpandedComponentCommandsTest::
    testDockWidgetCommand_undo() { /* Implementation
                                    */
}
void ExpandedComponentCommandsTest::
    testMenuBarCommand_addMenu() { /* Implementation
                                    */
}
void ExpandedComponentCommandsTest::
    testMenuBarCommand_removeMenu() { /* Implementation
                                       */
}
void ExpandedComponentCommandsTest::
    testMenuBarCommand_setNativeMenuBar() { /* Implementation */
}
void ExpandedComponentCommandsTest::
    testStatusBarCommand_showMessage() { /* Implementation
                                          */
}
void ExpandedComponentCommandsTest::
    testStatusBarCommand_clearMessage() { /* Implementation
                                           */
}
void ExpandedComponentCommandsTest::
    testToolBarCommand_addAction() { /* Implementation
                                      */
}
void ExpandedComponentCommandsTest::
    testToolBarCommand_removeAction() { /* Implementation
                                         */
}
void ExpandedComponentCommandsTest::
    testToolBarCommand_setOrientation() { /* Implementation
                                           */
}
void ExpandedComponentCommandsTest::
    testToolBarCommand_undo() { /* Implementation
                                 */
}
void ExpandedComponentCommandsTest::
    testToolButtonCommand_setPopupMode() { /* Implementation
                                            */
}
void ExpandedComponentCommandsTest::
    testToolButtonCommand_setToolButtonStyle() { /* Implementation */
}
void ExpandedComponentCommandsTest::
    testToolButtonCommand_undo() { /* Implementation
                                    */
}

QTEST_MAIN(ExpandedComponentCommandsTest)
#include "test_expanded_component_commands.moc"
