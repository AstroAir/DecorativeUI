/**
 * @file 03_simple_state.cpp
 * @brief Basic state management and reactive updates
 *
 * This example demonstrates:
 * - Creating reactive state with StateManager
 * - Computed state properties
 * - State change notifications
 * - Basic state validation
 *
 * Learning objectives:
 * - Understand reactive state management
 * - Learn how to create computed properties
 * - See automatic UI updates when state changes
 */

#include <QApplication>
#include <QDebug>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QWidget>

// Include DeclarativeUI state management
#include "Binding/StateManager.hpp"

using namespace DeclarativeUI;

/**
 * @brief Simple state management demonstration
 */
class SimpleStateApp : public QObject {
    Q_OBJECT

public:
    SimpleStateApp(QObject* parent = nullptr) : QObject(parent) {
        setupState();
        createUI();
        connectState();
    }

    void show() {
        if (main_widget_) {
            main_widget_->show();
        }
    }

private slots:
    void onIncrementClicked() {
        int current = counter_state_->get();
        counter_state_->set(current + 1);
        qDebug() << "Counter incremented to:" << current + 1;
    }

    void onDecrementClicked() {
        int current = counter_state_->get();
        counter_state_->set(current - 1);
        qDebug() << "Counter decremented to:" << current - 1;
    }

    void onResetClicked() {
        counter_state_->set(0);
        qDebug() << "Counter reset to 0";
    }

    void onSliderChanged(int value) {
        slider_state_->set(value);
        qDebug() << "Slider value changed to:" << value;
    }

    void onSpinBoxChanged(int value) {
        // Update counter state from spin box
        counter_state_->set(value);
    }

private:
    std::unique_ptr<QWidget> main_widget_;

    // UI components
    QLabel* counter_display_;
    QLabel* status_display_;
    QLabel* computed_display_;
    QProgressBar* progress_bar_;
    QSlider* slider_;
    QSpinBox* spin_box_;

    // State management
    std::shared_ptr<Binding::StateManager> state_manager_;
    std::shared_ptr<Binding::ReactiveProperty<int>> counter_state_;
    std::shared_ptr<Binding::ReactiveProperty<int>> slider_state_;
    std::shared_ptr<Binding::ReactiveProperty<QString>> counter_text_state_;
    std::shared_ptr<Binding::ReactiveProperty<QString>> status_text_state_;
    std::shared_ptr<Binding::ReactiveProperty<int>> progress_value_state_;

    void setupState() {
        // Get state manager instance
        state_manager_ = std::shared_ptr<Binding::StateManager>(
            &Binding::StateManager::instance(), [](Binding::StateManager*) {});

        // Create reactive states
        counter_state_ = state_manager_->createState<int>("counter", 0);
        slider_state_ = state_manager_->createState<int>("slider", 50);

        // Create computed states
        counter_text_state_ =
            state_manager_->createComputed<QString>("counter_text", [this]() {
                int count = counter_state_->get();
                QString emoji = count > 0 ? "📈" : count < 0 ? "📉" : "🎯";
                return QString("%1 Count: %2").arg(emoji).arg(count);
            });

        status_text_state_ =
            state_manager_->createComputed<QString>("status_text", [this]() {
                int count = counter_state_->get();
                int slider_val = slider_state_->get();

                if (count == 0) {
                    return QString("🎯 Ready to count! Slider: %1%")
                        .arg(slider_val);
                } else if (count > 0) {
                    return QString("✅ Positive count! Slider: %1%")
                        .arg(slider_val);
                } else {
                    return QString("⚠️ Negative count! Slider: %1%")
                        .arg(slider_val);
                }
            });

        progress_value_state_ =
            state_manager_->createComputed<int>("progress_value", [this]() {
                int count = counter_state_->get();
                int slider_val = slider_state_->get();

                // Combine counter and slider for progress calculation
                int combined = qAbs(count) * 10 + slider_val;
                return qMin(100, combined);
            });

        qDebug() << "✅ State management initialized";
    }

    void createUI() {
        main_widget_ = std::make_unique<QWidget>();
        main_widget_->setWindowTitle("03 - Simple State | DeclarativeUI");
        main_widget_->setMinimumSize(500, 450);

        auto layout = new QVBoxLayout(main_widget_.get());
        layout->setSpacing(20);
        layout->setContentsMargins(20, 20, 20, 20);

        // Header
        auto header = new QLabel("🔄 Simple State Management");
        header->setStyleSheet(
            "font-size: 20px; font-weight: bold; color: #2c3e50;");
        header->setAlignment(Qt::AlignCenter);
        layout->addWidget(header);

        // Counter display
        counter_display_ = new QLabel("🎯 Count: 0");
        counter_display_->setStyleSheet(
            "font-size: 24px; font-weight: bold; color: #3498db; "
            "background-color: #ecf0f1; padding: 15px; border-radius: 8px;");
        counter_display_->setAlignment(Qt::AlignCenter);
        layout->addWidget(counter_display_);

        // Status display
        status_display_ = new QLabel("🎯 Ready to count!");
        status_display_->setStyleSheet(
            "font-size: 14px; color: #7f8c8d; font-style: italic;");
        status_display_->setAlignment(Qt::AlignCenter);
        layout->addWidget(status_display_);

        // Counter controls
        auto counter_group = new QGroupBox("Counter Controls");
        auto counter_layout = new QHBoxLayout(counter_group);

        auto decrement_btn = new QPushButton("➖ Decrement");
        auto reset_btn = new QPushButton("🔄 Reset");
        auto increment_btn = new QPushButton("➕ Increment");

        // Style buttons
        QString button_style =
            "QPushButton { padding: 10px 20px; font-size: 14px; border-radius: "
            "5px; border: none; }"
            "QPushButton:hover { opacity: 0.8; }";

        decrement_btn->setStyleSheet(
            button_style +
            "QPushButton { background-color: #e74c3c; color: white; }");
        reset_btn->setStyleSheet(
            button_style +
            "QPushButton { background-color: #95a5a6; color: white; }");
        increment_btn->setStyleSheet(
            button_style +
            "QPushButton { background-color: #27ae60; color: white; }");

        connect(decrement_btn, &QPushButton::clicked, this,
                &SimpleStateApp::onDecrementClicked);
        connect(reset_btn, &QPushButton::clicked, this,
                &SimpleStateApp::onResetClicked);
        connect(increment_btn, &QPushButton::clicked, this,
                &SimpleStateApp::onIncrementClicked);

        counter_layout->addWidget(decrement_btn);
        counter_layout->addWidget(reset_btn);
        counter_layout->addWidget(increment_btn);
        layout->addWidget(counter_group);

        // Spin box control
        auto spinbox_group = new QGroupBox("Direct Value Input");
        auto spinbox_layout = new QHBoxLayout(spinbox_group);

        auto spinbox_label = new QLabel("Set Counter:");
        spin_box_ = new QSpinBox();
        spin_box_->setRange(-100, 100);
        spin_box_->setValue(0);

        connect(spin_box_, QOverload<int>::of(&QSpinBox::valueChanged), this,
                &SimpleStateApp::onSpinBoxChanged);

        spinbox_layout->addWidget(spinbox_label);
        spinbox_layout->addWidget(spin_box_);
        spinbox_layout->addStretch();
        layout->addWidget(spinbox_group);

        // Slider control
        auto slider_group = new QGroupBox("Slider Control");
        auto slider_layout = new QVBoxLayout(slider_group);

        auto slider_label = new QLabel("Slider Value: 50%");
        slider_label->setObjectName("sliderLabel");

        slider_ = new QSlider(Qt::Horizontal);
        slider_->setRange(0, 100);
        slider_->setValue(50);

        connect(slider_, &QSlider::valueChanged, this,
                &SimpleStateApp::onSliderChanged);
        connect(slider_, &QSlider::valueChanged, [slider_label](int value) {
            slider_label->setText(QString("Slider Value: %1%").arg(value));
        });

        slider_layout->addWidget(slider_label);
        slider_layout->addWidget(slider_);
        layout->addWidget(slider_group);

        // Progress bar (computed from state)
        auto progress_group = new QGroupBox("Computed Progress");
        auto progress_layout = new QVBoxLayout(progress_group);

        computed_display_ = new QLabel("Progress based on counter + slider");
        computed_display_->setAlignment(Qt::AlignCenter);

        progress_bar_ = new QProgressBar();
        progress_bar_->setRange(0, 100);
        progress_bar_->setValue(50);

        progress_layout->addWidget(computed_display_);
        progress_layout->addWidget(progress_bar_);
        layout->addWidget(progress_group);

        qDebug() << "✅ UI created successfully";
    }

    void connectState() {
        // Connect state changes to UI updates
        connect(counter_state_.get(),
                &Binding::ReactivePropertyBase::valueChanged, [this]() {
                    // Update counter display
                    QString text = counter_text_state_->get();
                    counter_display_->setText(text);

                    // Update spin box without triggering signal
                    spin_box_->blockSignals(true);
                    spin_box_->setValue(counter_state_->get());
                    spin_box_->blockSignals(false);

                    // Update status
                    status_display_->setText(status_text_state_->get());

                    // Update progress
                    progress_bar_->setValue(progress_value_state_->get());
                });

        connect(slider_state_.get(),
                &Binding::ReactivePropertyBase::valueChanged, [this]() {
                    // Update status and progress when slider changes
                    status_display_->setText(status_text_state_->get());
                    progress_bar_->setValue(progress_value_state_->get());
                });

        qDebug() << "✅ State connections established";
    }
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    app.setApplicationName("DeclarativeUI Simple State");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("DeclarativeUI Examples");

    try {
        qDebug() << "🚀 Starting Simple State example...";

        SimpleStateApp state_app;
        state_app.show();

        qDebug() << "💡 This example demonstrates:";
        qDebug() << "   - Reactive state management";
        qDebug() << "   - Computed state properties";
        qDebug() << "   - Automatic UI updates";
        qDebug() << "   - State change notifications";

        return app.exec();

    } catch (const std::exception& e) {
        qCritical() << "❌ Application error:" << e.what();
        return -1;
    }
}

#include "03_simple_state.moc"
