/**
 * @file 05_event_handling.cpp
 * @brief Basic event handling and user interactions
 *
 * This example demonstrates:
 * - Various types of user events
 * - Event handler registration
 * - Event data passing
 * - Custom event handling patterns
 *
 * Learning objectives:
 * - Understand different event types
 * - Learn event handler patterns
 * - See event data flow and processing
 */

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDateTime>
#include <QDebug>
#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QObject>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>
#include <QTextEdit>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

// Include DeclarativeUI components
#include "JSON/JSONUILoader.hpp"

using namespace DeclarativeUI;

/**
 * @brief Event handling demonstration
 */
class EventHandlingApp : public QObject {
    Q_OBJECT

public:
    EventHandlingApp(QObject* parent = nullptr) : QObject(parent) {
        setupUILoader();
        createUI();
        setupEventLogging();
    }

    void show() {
        if (main_widget_) {
            main_widget_->show();
        }
    }

private slots:
    void onButtonClicked() {
        logEvent("Button", "clicked", "Simple button click event");

        // Find and update counter
        static int click_count = 0;
        click_count++;

        auto counter_label = main_widget_->findChild<QLabel*>("clickCounter");
        if (counter_label) {
            counter_label->setText(QString("Clicks: %1").arg(click_count));
        }
    }

    void onTextChanged(const QString& text) {
        logEvent(
            "LineEdit", "textChanged",
            QString("Text: '%1' (length: %2)").arg(text).arg(text.length()));

        // Update character count
        auto char_count_label = main_widget_->findChild<QLabel*>("charCount");
        if (char_count_label) {
            char_count_label->setText(
                QString("Characters: %1").arg(text.length()));
        }
    }

    void onSliderChanged(int value) {
        logEvent("Slider", "valueChanged", QString("Value: %1").arg(value));

        // Update slider value display
        auto slider_value_label =
            main_widget_->findChild<QLabel*>("sliderValue");
        if (slider_value_label) {
            slider_value_label->setText(QString("Slider: %1%").arg(value));
        }
    }

    void onCheckBoxToggled(bool checked) {
        logEvent("CheckBox", "toggled",
                 QString("Checked: %1").arg(checked ? "true" : "false"));

        // Update checkbox status
        auto checkbox_status =
            main_widget_->findChild<QLabel*>("checkboxStatus");
        if (checkbox_status) {
            QString status = checked ? "✅ Enabled" : "❌ Disabled";
            checkbox_status->setText(status);
        }
    }

    void onComboBoxChanged(const QString& text) {
        logEvent("ComboBox", "currentTextChanged",
                 QString("Selected: '%1'").arg(text));

        // Update selection display
        auto combo_selection =
            main_widget_->findChild<QLabel*>("comboSelection");
        if (combo_selection) {
            combo_selection->setText(QString("Selected: %1").arg(text));
        }
    }

    void onSpinBoxChanged(int value) {
        logEvent("SpinBox", "valueChanged", QString("Value: %1").arg(value));

        // Update spin box display
        auto spinbox_value = main_widget_->findChild<QLabel*>("spinboxValue");
        if (spinbox_value) {
            spinbox_value->setText(QString("Number: %1").arg(value));
        }
    }

    void onClearLogClicked() {
        auto log_display = main_widget_->findChild<QTextEdit*>("eventLog");
        if (log_display) {
            log_display->clear();
            logEvent("System", "clearLog", "Event log cleared");
        }
    }

    void onTestAllEventsClicked() {
        logEvent("System", "testAllEvents", "Testing all event types...");

        // Trigger various events programmatically
        auto line_edit = main_widget_->findChild<QLineEdit*>("textInput");
        auto slider = main_widget_->findChild<QSlider*>("valueSlider");
        auto checkbox = main_widget_->findChild<QCheckBox*>("toggleCheck");
        auto combobox = main_widget_->findChild<QComboBox*>("optionCombo");
        auto spinbox = main_widget_->findChild<QSpinBox*>("numberSpin");

        if (line_edit) {
            line_edit->setText("Auto-generated text");
        }

        if (slider) {
            slider->setValue(75);
        }

        if (checkbox) {
            checkbox->setChecked(!checkbox->isChecked());
        }

        if (combobox) {
            combobox->setCurrentIndex((combobox->currentIndex() + 1) %
                                      combobox->count());
        }

        if (spinbox) {
            spinbox->setValue(42);
        }

        logEvent("System", "testAllEvents",
                 "All events triggered programmatically");
    }

private:
    std::unique_ptr<QWidget> main_widget_;
    std::unique_ptr<JSON::JSONUILoader> ui_loader_;

    void setupUILoader() {
        ui_loader_ = std::make_unique<JSON::JSONUILoader>();

        // Register event handlers for JSON UI
        ui_loader_->registerEventHandler("buttonClick",
                                         [this]() { onButtonClicked(); });

        ui_loader_->registerEventHandler("clearLog",
                                         [this]() { onClearLogClicked(); });

        ui_loader_->registerEventHandler(
            "testAllEvents", [this]() { onTestAllEventsClicked(); });

        qDebug() << "✅ Event handlers registered";
    }

    void createUI() {
        try {
            QString ui_file = "resources/event_handling_ui.json";

            if (QFileInfo::exists(ui_file)) {
                main_widget_ = ui_loader_->loadFromFile(ui_file);
                if (main_widget_) {
                    main_widget_->setWindowTitle(
                        "05 - Event Handling | DeclarativeUI");
                    connectUIEvents();
                    qDebug() << "✅ UI loaded from JSON";
                    return;
                }
            }

            // Fallback to programmatic UI
            main_widget_ = createProgrammaticUI();

        } catch (const std::exception& e) {
            qCritical() << "UI creation failed:" << e.what();
            main_widget_ = createProgrammaticUI();
        }
    }

    void connectUIEvents() {
        // Connect events for components loaded from JSON
        auto line_edit = main_widget_->findChild<QLineEdit*>("textInput");
        if (line_edit) {
            connect(line_edit, &QLineEdit::textChanged, this,
                    &EventHandlingApp::onTextChanged);
        }

        auto slider = main_widget_->findChild<QSlider*>("valueSlider");
        if (slider) {
            connect(slider, &QSlider::valueChanged, this,
                    &EventHandlingApp::onSliderChanged);
        }

        auto checkbox = main_widget_->findChild<QCheckBox*>("toggleCheck");
        if (checkbox) {
            connect(checkbox, &QCheckBox::toggled, this,
                    &EventHandlingApp::onCheckBoxToggled);
        }

        auto combobox = main_widget_->findChild<QComboBox*>("optionCombo");
        if (combobox) {
            connect(combobox, &QComboBox::currentTextChanged, this,
                    &EventHandlingApp::onComboBoxChanged);
        }

        auto spinbox = main_widget_->findChild<QSpinBox*>("numberSpin");
        if (spinbox) {
            connect(spinbox, QOverload<int>::of(&QSpinBox::valueChanged), this,
                    &EventHandlingApp::onSpinBoxChanged);
        }

        qDebug() << "✅ UI events connected";
    }

    std::unique_ptr<QWidget> createProgrammaticUI() {
        auto widget = std::make_unique<QWidget>();
        widget->setWindowTitle(
            "05 - Event Handling (Fallback) | DeclarativeUI");
        widget->setMinimumSize(600, 500);

        auto layout = new QVBoxLayout(widget.get());
        layout->setSpacing(15);
        layout->setContentsMargins(20, 20, 20, 20);

        // Header
        auto header = new QLabel("🎯 Event Handling Example");
        header->setStyleSheet(
            "font-size: 20px; font-weight: bold; color: #2c3e50;");
        header->setAlignment(Qt::AlignCenter);
        layout->addWidget(header);

        // Create controls grid
        auto controls_group = new QGroupBox("Interactive Controls");
        auto controls_layout = new QGridLayout(controls_group);

        // Button
        auto button = new QPushButton("🔘 Click Me!");
        auto click_counter = new QLabel("Clicks: 0");
        click_counter->setObjectName("clickCounter");
        connect(button, &QPushButton::clicked, this,
                &EventHandlingApp::onButtonClicked);
        controls_layout->addWidget(button, 0, 0);
        controls_layout->addWidget(click_counter, 0, 1);

        // Text input
        auto text_input = new QLineEdit();
        text_input->setObjectName("textInput");
        text_input->setPlaceholderText("Type something...");
        auto char_count = new QLabel("Characters: 0");
        char_count->setObjectName("charCount");
        connect(text_input, &QLineEdit::textChanged, this,
                &EventHandlingApp::onTextChanged);
        controls_layout->addWidget(text_input, 1, 0);
        controls_layout->addWidget(char_count, 1, 1);

        // Slider
        auto slider = new QSlider(Qt::Horizontal);
        slider->setObjectName("valueSlider");
        slider->setRange(0, 100);
        slider->setValue(50);
        auto slider_value = new QLabel("Slider: 50%");
        slider_value->setObjectName("sliderValue");
        connect(slider, &QSlider::valueChanged, this,
                &EventHandlingApp::onSliderChanged);
        controls_layout->addWidget(slider, 2, 0);
        controls_layout->addWidget(slider_value, 2, 1);

        layout->addWidget(controls_group);

        // Event log
        auto log_group = new QGroupBox("Event Log");
        auto log_layout = new QVBoxLayout(log_group);

        auto log_display = new QTextEdit();
        log_display->setObjectName("eventLog");
        log_display->setMaximumHeight(150);
        log_display->setReadOnly(true);
        log_layout->addWidget(log_display);

        auto clear_button = new QPushButton("🗑️ Clear Log");
        connect(clear_button, &QPushButton::clicked, this,
                &EventHandlingApp::onClearLogClicked);
        log_layout->addWidget(clear_button);

        layout->addWidget(log_group);

        qDebug() << "✅ Programmatic UI created";
        return widget;
    }

    void setupEventLogging() {
        // Log initial state
        QTimer::singleShot(100, [this]() {
            logEvent("System", "startup", "Event handling example initialized");
        });
    }

    void logEvent(const QString& source, const QString& event,
                  const QString& details) {
        auto log_display = main_widget_->findChild<QTextEdit*>("eventLog");
        if (log_display) {
            QString timestamp =
                QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
            QString log_entry = QString("[%1] %2::%3 - %4")
                                    .arg(timestamp, source, event, details);

            log_display->append(log_entry);

            // Auto-scroll to bottom
            auto cursor = log_display->textCursor();
            cursor.movePosition(QTextCursor::End);
            log_display->setTextCursor(cursor);
        }

        qDebug() << "Event:" << source << event << details;
    }
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    app.setApplicationName("DeclarativeUI Event Handling");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("DeclarativeUI Examples");

    try {
        qDebug() << "🚀 Starting Event Handling example...";

        EventHandlingApp event_app;
        event_app.show();

        qDebug() << "💡 This example demonstrates:";
        qDebug() << "   - Various types of user events";
        qDebug() << "   - Event handler registration";
        qDebug() << "   - Event data processing";
        qDebug() << "   - Real-time event logging";

        return app.exec();

    } catch (const std::exception& e) {
        qCritical() << "❌ Application error:" << e.what();
        return -1;
    }
}

#include "05_event_handling.moc"
