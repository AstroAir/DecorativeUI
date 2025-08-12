#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QSignalSpy>
#include <QSlider>
#include <QTest>
#include <QValidator>
#include <memory>

#include "../Components/Button.hpp"
#include "../Components/CheckBox.hpp"
#include "../Components/ComboBox.hpp"
#include "../Components/Dial.hpp"
#include "../Components/DoubleSpinBox.hpp"
#include "../Components/LCDNumber.hpp"
#include "../Components/LineEdit.hpp"
#include "../Components/ProgressBar.hpp"
#include "../Components/RadioButton.hpp"
#include "../Components/Slider.hpp"
#include "../Components/SpinBox.hpp"

using namespace DeclarativeUI::Components;
using namespace DeclarativeUI::Exceptions;

class ComponentsTest : public QObject {
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

    // **Button Component Tests**
    void testButtonCreation() {
        auto button = std::make_unique<Button>();

        // Test fluent interface
        button->text("Test Button")
            .enabled(true)
            .style("QPushButton { background-color: blue; }");

        button->initialize();

        auto* widget = button->getWidget();
        QVERIFY(widget != nullptr);

        auto* push_button = qobject_cast<QPushButton*>(widget);
        QVERIFY(push_button != nullptr);
        QCOMPARE(push_button->text(), QString("Test Button"));
        QVERIFY(push_button->isEnabled());
    }

    void testButtonWithIcon() {
        auto button = std::make_unique<Button>();

        QIcon test_icon = QIcon::fromTheme("document-save");
        button->text("Save").icon(test_icon);

        button->initialize();

        auto* widget = button->getWidget();
        QVERIFY(widget != nullptr);

        auto* push_button = qobject_cast<QPushButton*>(widget);
        QVERIFY(push_button != nullptr);
        QCOMPARE(push_button->text(), QString("Save"));
        QVERIFY(!push_button->icon().isNull());
    }

    void testButtonClickHandler() {
        auto button = std::make_unique<Button>();

        bool clicked = false;
        button->text("Click Me").onClick([&clicked]() { clicked = true; });

        button->initialize();

        auto* widget = button->getWidget();
        QVERIFY(widget != nullptr);

        auto* push_button = qobject_cast<QPushButton*>(widget);
        QVERIFY(push_button != nullptr);

        // Simulate click
        push_button->click();
        QVERIFY(clicked);
    }

    void testButtonDisabled() {
        auto button = std::make_unique<Button>();

        button->text("Disabled Button").enabled(false);

        button->initialize();

        auto* widget = button->getWidget();
        QVERIFY(widget != nullptr);

        auto* push_button = qobject_cast<QPushButton*>(widget);
        QVERIFY(push_button != nullptr);
        QVERIFY(!push_button->isEnabled());
    }

    // **CheckBox Component Tests**
    void testCheckBoxCreation() {
        auto checkbox = std::make_unique<CheckBox>();

        checkbox->text("Test CheckBox")
            .checked(true)
            .style("QCheckBox { color: red; }");

        checkbox->initialize();

        auto* widget = checkbox->getWidget();
        QVERIFY(widget != nullptr);

        auto* check_box = qobject_cast<QCheckBox*>(widget);
        QVERIFY(check_box != nullptr);
        QCOMPARE(check_box->text(), QString("Test CheckBox"));
        QVERIFY(check_box->isChecked());
    }

    void testCheckBoxTristate() {
        auto checkbox = std::make_unique<CheckBox>();

        checkbox->text("Tristate CheckBox").tristate(true);

        checkbox->initialize();

        auto* widget = checkbox->getWidget();
        QVERIFY(widget != nullptr);

        auto* check_box = qobject_cast<QCheckBox*>(widget);
        QVERIFY(check_box != nullptr);
        QVERIFY(check_box->isTristate());
    }

    void testCheckBoxStateChanged() {
        auto checkbox = std::make_unique<CheckBox>();

        int last_state = -1;
        checkbox->text("State Test").onStateChanged([&last_state](int state) {
            last_state = state;
        });

        checkbox->initialize();

        auto* widget = checkbox->getWidget();
        QVERIFY(widget != nullptr);

        auto* check_box = qobject_cast<QCheckBox*>(widget);
        QVERIFY(check_box != nullptr);

        // Change state
        check_box->setChecked(true);
        QCOMPARE(last_state, static_cast<int>(Qt::Checked));

        check_box->setChecked(false);
        QCOMPARE(last_state, static_cast<int>(Qt::Unchecked));
    }

    void testCheckBoxToggled() {
        auto checkbox = std::make_unique<CheckBox>();

        bool last_toggled = false;
        checkbox->text("Toggle Test").onToggled([&last_toggled](bool checked) {
            last_toggled = checked;
        });

        checkbox->initialize();

        auto* widget = checkbox->getWidget();
        QVERIFY(widget != nullptr);

        auto* check_box = qobject_cast<QCheckBox*>(widget);
        QVERIFY(check_box != nullptr);

        // Toggle
        check_box->toggle();
        QCOMPARE(last_toggled, check_box->isChecked());
    }

    // **LineEdit Component Tests**
    void testLineEditCreation() {
        auto line_edit = std::make_unique<LineEdit>();

        line_edit->text("Initial Text")
            .placeholder("Enter text here")
            .maxLength(50)
            .readOnly(false);

        line_edit->initialize();

        auto* widget = line_edit->getWidget();
        QVERIFY(widget != nullptr);

        auto* qt_line_edit = qobject_cast<QLineEdit*>(widget);
        QVERIFY(qt_line_edit != nullptr);
        QCOMPARE(qt_line_edit->text(), QString("Initial Text"));
        QCOMPARE(qt_line_edit->placeholderText(), QString("Enter text here"));
        QCOMPARE(qt_line_edit->maxLength(), 50);
        QVERIFY(!qt_line_edit->isReadOnly());
    }

    void testLineEditEchoMode() {
        auto line_edit = std::make_unique<LineEdit>();

        line_edit->text("Password").echoMode(QLineEdit::Password);

        line_edit->initialize();

        auto* widget = line_edit->getWidget();
        QVERIFY(widget != nullptr);

        auto* qt_line_edit = qobject_cast<QLineEdit*>(widget);
        QVERIFY(qt_line_edit != nullptr);
        QCOMPARE(qt_line_edit->echoMode(), QLineEdit::Password);
    }

    void testLineEditTextChanged() {
        auto line_edit = std::make_unique<LineEdit>();

        QString last_text;
        line_edit->onTextChanged(
            [&last_text](const QString& text) { last_text = text; });

        line_edit->initialize();

        auto* widget = line_edit->getWidget();
        QVERIFY(widget != nullptr);

        auto* qt_line_edit = qobject_cast<QLineEdit*>(widget);
        QVERIFY(qt_line_edit != nullptr);

        // Change text
        qt_line_edit->setText("New Text");
        QCOMPARE(last_text, QString("New Text"));
    }

    void testLineEditReturnPressed() {
        auto line_edit = std::make_unique<LineEdit>();

        bool return_pressed = false;
        line_edit->onReturnPressed(
            [&return_pressed]() { return_pressed = true; });

        line_edit->initialize();

        auto* widget = line_edit->getWidget();
        QVERIFY(widget != nullptr);

        auto* qt_line_edit = qobject_cast<QLineEdit*>(widget);
        QVERIFY(qt_line_edit != nullptr);

        // Simulate return key press
        QTest::keyPress(qt_line_edit, Qt::Key_Return);
        QVERIFY(return_pressed);
    }

    // **ProgressBar Component Tests**
    void testProgressBarCreation() {
        auto progress_bar = std::make_unique<ProgressBar>();

        progress_bar->minimum(0)
            .maximum(100)
            .value(50)
            .textVisible(true)
            .format("%p%");

        progress_bar->initialize();

        auto* widget = progress_bar->getWidget();
        QVERIFY(widget != nullptr);

        auto* qt_progress_bar = qobject_cast<QProgressBar*>(widget);
        QVERIFY(qt_progress_bar != nullptr);
        QCOMPARE(qt_progress_bar->minimum(), 0);
        QCOMPARE(qt_progress_bar->maximum(), 100);
        QCOMPARE(qt_progress_bar->value(), 50);
        QVERIFY(qt_progress_bar->isTextVisible());
        QCOMPARE(qt_progress_bar->format(), QString("%p%"));
    }

    void testProgressBarOrientation() {
        auto progress_bar = std::make_unique<ProgressBar>();

        progress_bar->orientation(Qt::Vertical).invertedAppearance(true);

        progress_bar->initialize();

        auto* widget = progress_bar->getWidget();
        QVERIFY(widget != nullptr);

        auto* qt_progress_bar = qobject_cast<QProgressBar*>(widget);
        QVERIFY(qt_progress_bar != nullptr);
        QCOMPARE(qt_progress_bar->orientation(), Qt::Vertical);
        QVERIFY(qt_progress_bar->invertedAppearance());
    }

    void testProgressBarValueMethods() {
        auto progress_bar = std::make_unique<ProgressBar>();

        progress_bar->minimum(10).maximum(90).value(30);

        progress_bar->initialize();

        QCOMPARE(progress_bar->getMinimum(), 10);
        QCOMPARE(progress_bar->getMaximum(), 90);
        QCOMPARE(progress_bar->getValue(), 30);

        // Test setValue method
        progress_bar->setValue(60);
        QCOMPARE(progress_bar->getValue(), 60);

        // Test reset method
        progress_bar->reset();
        QCOMPARE(progress_bar->getValue(), progress_bar->getMinimum());
    }

    // **Slider Component Tests**
    void testSliderCreation() {
        auto slider = std::make_unique<Slider>();

        slider->minimum(0)
            .maximum(100)
            .value(25)
            .orientation(Qt::Horizontal)
            .tickPosition(QSlider::TicksBelow)
            .tickInterval(10);

        slider->initialize();

        auto* widget = slider->getWidget();
        QVERIFY(widget != nullptr);

        auto* qt_slider = qobject_cast<QSlider*>(widget);
        QVERIFY(qt_slider != nullptr);
        QCOMPARE(qt_slider->minimum(), 0);
        QCOMPARE(qt_slider->maximum(), 100);
        QCOMPARE(qt_slider->value(), 25);
        QCOMPARE(qt_slider->orientation(), Qt::Horizontal);
        QCOMPARE(qt_slider->tickPosition(), QSlider::TicksBelow);
        QCOMPARE(qt_slider->tickInterval(), 10);
    }

    void testSliderValueChanged() {
        auto slider = std::make_unique<Slider>();

        int last_value = -1;
        slider->onValueChanged(
            [&last_value](int value) { last_value = value; });

        slider->initialize();

        auto* widget = slider->getWidget();
        QVERIFY(widget != nullptr);

        auto* qt_slider = qobject_cast<QSlider*>(widget);
        QVERIFY(qt_slider != nullptr);

        // Change value
        qt_slider->setValue(75);
        QCOMPARE(last_value, 75);
    }

    // **ComboBox Component Tests**
    void testComboBoxCreation() {
        auto combo_box = std::make_unique<ComboBox>();

        QStringList items = {"Item 1", "Item 2", "Item 3"};
        combo_box->items(items).currentIndex(1).editable(false);

        combo_box->initialize();

        auto* widget = combo_box->getWidget();
        QVERIFY(widget != nullptr);

        auto* qt_combo_box = qobject_cast<QComboBox*>(widget);
        QVERIFY(qt_combo_box != nullptr);
        QCOMPARE(qt_combo_box->count(), 3);
        QCOMPARE(qt_combo_box->currentIndex(), 1);
        QCOMPARE(qt_combo_box->currentText(), QString("Item 2"));
        QVERIFY(!qt_combo_box->isEditable());
    }

    void testComboBoxCurrentIndexChanged() {
        auto combo_box = std::make_unique<ComboBox>();

        QStringList items = {"Option A", "Option B", "Option C"};
        int last_index = -1;

        combo_box->items(items).onCurrentIndexChanged(
            [&last_index](int index) { last_index = index; });

        combo_box->initialize();

        auto* widget = combo_box->getWidget();
        QVERIFY(widget != nullptr);

        auto* qt_combo_box = qobject_cast<QComboBox*>(widget);
        QVERIFY(qt_combo_box != nullptr);

        // Change current index
        qt_combo_box->setCurrentIndex(2);
        QCOMPARE(last_index, 2);
    }

    // **RadioButton Component Tests**
    void testRadioButtonCreation() {
        auto radio_button = std::make_unique<RadioButton>();

        radio_button->text("Option A").checked(true).autoExclusive(true);

        radio_button->initialize();

        auto* widget = radio_button->getWidget();
        QVERIFY(widget != nullptr);

        auto* qt_radio = qobject_cast<QRadioButton*>(widget);
        QVERIFY(qt_radio != nullptr);
        QCOMPARE(qt_radio->text(), QString("Option A"));
        QVERIFY(qt_radio->isChecked());
        QVERIFY(qt_radio->autoExclusive());
    }

    void testRadioButtonToggled() {
        auto radio_button = std::make_unique<RadioButton>();

        bool last_toggled = false;
        radio_button->text("Toggle Test")
            .onToggled(
                [&last_toggled](bool checked) { last_toggled = checked; });

        radio_button->initialize();

        auto* widget = radio_button->getWidget();
        auto* qt_radio = qobject_cast<QRadioButton*>(widget);
        QVERIFY(qt_radio != nullptr);

        // Simulate toggle
        qt_radio->setChecked(true);
        QVERIFY(last_toggled);

        qt_radio->setChecked(false);
        QVERIFY(!last_toggled);
    }

    // **Dial Component Tests**
    void testDialCreation() {
        auto dial = std::make_unique<Dial>();

        dial->minimum(0).maximum(360).value(180).wrapping(true).notchesVisible(
            true);

        dial->initialize();

        auto* widget = dial->getWidget();
        QVERIFY(widget != nullptr);

        auto* qt_dial = qobject_cast<QDial*>(widget);
        QVERIFY(qt_dial != nullptr);
        QCOMPARE(qt_dial->minimum(), 0);
        QCOMPARE(qt_dial->maximum(), 360);
        QCOMPARE(qt_dial->value(), 180);
        QVERIFY(qt_dial->wrapping());
        QVERIFY(qt_dial->notchesVisible());
    }

    void testDialValueChanged() {
        auto dial = std::make_unique<Dial>();

        int last_value = -1;
        dial->onValueChanged([&last_value](int value) { last_value = value; });

        dial->initialize();

        auto* widget = dial->getWidget();
        auto* qt_dial = qobject_cast<QDial*>(widget);
        QVERIFY(qt_dial != nullptr);

        // Change value
        qt_dial->setValue(45);
        QCOMPARE(last_value, 45);
    }

    // **LCDNumber Component Tests**
    void testLCDNumberCreation() {
        auto lcd = std::make_unique<LCDNumber>();

        lcd->digitCount(6)
            .value(123.45)
            .mode(QLCDNumber::Dec)
            .segmentStyle(QLCDNumber::Filled);

        lcd->initialize();

        auto* widget = lcd->getWidget();
        QVERIFY(widget != nullptr);

        auto* qt_lcd = qobject_cast<QLCDNumber*>(widget);
        QVERIFY(qt_lcd != nullptr);
        QCOMPARE(qt_lcd->digitCount(), 6);
        QCOMPARE(qt_lcd->value(), 123.45);
        QCOMPARE(qt_lcd->mode(), QLCDNumber::Dec);
        QCOMPARE(qt_lcd->segmentStyle(), QLCDNumber::Filled);
    }

    void testLCDNumberModes() {
        auto lcd = std::make_unique<LCDNumber>();

        // Test hexadecimal mode
        lcd->mode(QLCDNumber::Hex).intValue(255);

        lcd->initialize();

        auto* widget = lcd->getWidget();
        auto* qt_lcd = qobject_cast<QLCDNumber*>(widget);
        QVERIFY(qt_lcd != nullptr);
        QCOMPARE(qt_lcd->mode(), QLCDNumber::Hex);
        QCOMPARE(qt_lcd->intValue(), 255);
    }

    // **SpinBox Component Tests**
    void testSpinBoxCreation() {
        auto spin_box = std::make_unique<SpinBox>();

        spin_box->minimum(0)
            .maximum(100)
            .value(50)
            .singleStep(5)
            .prefix("Value: ")
            .suffix(" units");

        spin_box->initialize();

        auto* widget = spin_box->getWidget();
        QVERIFY(widget != nullptr);

        auto* qt_spin = qobject_cast<QSpinBox*>(widget);
        QVERIFY(qt_spin != nullptr);
        QCOMPARE(qt_spin->minimum(), 0);
        QCOMPARE(qt_spin->maximum(), 100);
        QCOMPARE(qt_spin->value(), 50);
        QCOMPARE(qt_spin->singleStep(), 5);
        QCOMPARE(qt_spin->prefix(), QString("Value: "));
        QCOMPARE(qt_spin->suffix(), QString(" units"));
    }

    void testSpinBoxValueChanged() {
        auto spin_box = std::make_unique<SpinBox>();

        int last_value = -1;
        spin_box->onValueChanged(
            [&last_value](int value) { last_value = value; });

        spin_box->initialize();

        auto* widget = spin_box->getWidget();
        auto* qt_spin = qobject_cast<QSpinBox*>(widget);
        QVERIFY(qt_spin != nullptr);

        // Change value
        qt_spin->setValue(75);
        QCOMPARE(last_value, 75);
    }

    // **DoubleSpinBox Component Tests**
    void testDoubleSpinBoxCreation() {
        auto double_spin = std::make_unique<DoubleSpinBox>();

        double_spin->minimum(0.0)
            .maximum(100.0)
            .value(50.5)
            .singleStep(0.1)
            .decimals(2)
            .prefix("$")
            .suffix(" USD");

        double_spin->initialize();

        auto* widget = double_spin->getWidget();
        QVERIFY(widget != nullptr);

        auto* qt_double_spin = qobject_cast<QDoubleSpinBox*>(widget);
        QVERIFY(qt_double_spin != nullptr);
        QCOMPARE(qt_double_spin->minimum(), 0.0);
        QCOMPARE(qt_double_spin->maximum(), 100.0);
        QCOMPARE(qt_double_spin->value(), 50.5);
        QCOMPARE(qt_double_spin->singleStep(), 0.1);
        QCOMPARE(qt_double_spin->decimals(), 2);
        QCOMPARE(qt_double_spin->prefix(), QString("$"));
        QCOMPARE(qt_double_spin->suffix(), QString(" USD"));
    }

    // **Error Handling Tests**
    void testComponentInitializationException() {
        // Test that components handle initialization errors gracefully
        auto button = std::make_unique<Button>();

        // This should not throw an exception
        button->initialize();
        QVERIFY(button->getWidget() != nullptr);

        // Double initialization should be safe
        button->initialize();
        QVERIFY(button->getWidget() != nullptr);
    }

    void testComponentPropertySetting() {
        auto line_edit = std::make_unique<LineEdit>();

        // Test setting properties before initialization
        line_edit->text("Before Init").placeholder("Placeholder");

        line_edit->initialize();

        auto* widget = line_edit->getWidget();
        QVERIFY(widget != nullptr);

        auto* qt_line_edit = qobject_cast<QLineEdit*>(widget);
        QVERIFY(qt_line_edit != nullptr);
        QCOMPARE(qt_line_edit->text(), QString("Before Init"));
        QCOMPARE(qt_line_edit->placeholderText(), QString("Placeholder"));

        // Test setting properties after initialization
        line_edit->text("After Init");
        // Properties set after initialization might not be applied
        // automatically This documents the expected behavior
    }
};

QTEST_MAIN(ComponentsTest)
#include "test_components.moc"
