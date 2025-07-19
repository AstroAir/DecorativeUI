// Examples/comprehensive_example.cpp - Simplified Working Version
#include <QApplication>
#include <QDebug>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <memory>


#include "../../Components/Button.hpp"
#include "../../Components/CheckBox.hpp"
#include "../../Components/ComboBox.hpp"
#include "../../Components/LineEdit.hpp"
#include "../../Components/ProgressBar.hpp"
#include "../../Components/Slider.hpp"
#include "../../HotReload/PerformanceMonitor.hpp"


using namespace DeclarativeUI;

class ComprehensiveExampleWindow : public QMainWindow {
    Q_OBJECT

public:
    ComprehensiveExampleWindow(QWidget* parent = nullptr)
        : QMainWindow(parent) {
        setWindowTitle("🚀 Comprehensive Qt Components Demo");
        setMinimumSize(1000, 700);

        setupMenuBar();
        setupUI();
        setupPerformanceMonitoring();

        // Show welcome message
        QTimer::singleShot(500, [this]() {
            QMessageBox::information(
                this, "Welcome",
                "Welcome to the Comprehensive Qt Components Demo!\n\n"
                "This demo showcases working components from the DeclarativeUI "
                "framework.\n\n"
                "Built with Qt and the DeclarativeUI framework.");
        });
    }

private:
    void setupMenuBar() {
        // File menu
        auto* file_menu = menuBar()->addMenu("&File");
        file_menu->addAction("&New", QKeySequence::New,
                             [this]() { onFileNew(); });
        file_menu->addAction("&Open", QKeySequence::Open,
                             [this]() { onFileOpen(); });
        file_menu->addAction("&Save", QKeySequence::Save,
                             [this]() { onFileSave(); });
        file_menu->addSeparator();
        file_menu->addAction("E&xit", QKeySequence::Quit,
                             [this]() { close(); });

        // Edit menu
        auto* edit_menu = menuBar()->addMenu("&Edit");
        edit_menu->addAction("&Undo", QKeySequence::Undo,
                             [this]() { onEditUndo(); });
        edit_menu->addAction("&Redo", QKeySequence::Redo,
                             [this]() { onEditRedo(); });

        // Help menu
        auto* help_menu = menuBar()->addMenu("&Help");
        help_menu->addAction("&About", [this]() { onHelpAbout(); });
    }

    void setupUI() {
        auto* central_widget = new QWidget();
        setCentralWidget(central_widget);

        auto* main_layout = new QVBoxLayout(central_widget);

        // Create components demo
        createComponentsDemo();
        main_layout->addWidget(components_widget_);

        // Status bar
        statusBar()->showMessage("Ready - Components loaded successfully");
    }

    void createComponentsDemo() {
        components_widget_ = new QWidget();
        auto* layout = new QVBoxLayout(components_widget_);

        // Button examples
        auto* button_group = new QGroupBox("Button Examples");
        auto* button_layout = new QHBoxLayout(button_group);

        primary_button_ = std::make_unique<Components::Button>();
        primary_button_->text("Primary Button")
            .style(
                "QPushButton { background-color: #007bff; color: white; "
                "font-weight: bold; }")
            .onClick([this]() {
                statusBar()->showMessage("Primary button clicked!", 2000);
            });
        primary_button_->initialize();

        secondary_button_ = std::make_unique<Components::Button>();
        secondary_button_->text("Secondary Button")
            .style("QPushButton { background-color: #6c757d; color: white; }")
            .onClick([this]() {
                statusBar()->showMessage("Secondary button clicked!", 2000);
            });
        secondary_button_->initialize();

        button_layout->addWidget(primary_button_->getWidget());
        button_layout->addWidget(secondary_button_->getWidget());

        // Input examples
        auto* input_group = new QGroupBox("Input Examples");
        auto* input_layout = new QFormLayout(input_group);

        line_edit_ = std::make_unique<Components::LineEdit>();
        line_edit_->placeholder("Enter text here...")
            .onTextChanged([this](const QString& text) {
                statusBar()->showMessage(QString("Text changed: %1").arg(text),
                                         1000);
            });
        line_edit_->initialize();

        combo_box_ = std::make_unique<Components::ComboBox>();
        combo_box_->addItem("Option 1")
            .addItem("Option 2")
            .addItem("Option 3")
            .onCurrentTextChanged([this](const QString& text) {
                statusBar()->showMessage(QString("Selected: %1").arg(text),
                                         1000);
            });
        combo_box_->initialize();

        check_box_ = std::make_unique<Components::CheckBox>();
        check_box_->text("Enable feature").onToggled([this](bool checked) {
            statusBar()->showMessage(
                QString("Feature %1").arg(checked ? "enabled" : "disabled"),
                1000);
        });
        check_box_->initialize();

        input_layout->addRow("Text Input:", line_edit_->getWidget());
        input_layout->addRow("Combo Box:", combo_box_->getWidget());
        input_layout->addRow("Check Box:", check_box_->getWidget());

        // Slider and progress bar
        auto* control_group = new QGroupBox("Control Examples");
        auto* control_layout = new QFormLayout(control_group);

        slider_ = std::make_unique<Components::Slider>();
        slider_->minimum(0).maximum(100).value(50).onValueChanged(
            [this](int value) {
                statusBar()->showMessage(QString("Slider value: %1").arg(value),
                                         1000);
                if (progress_bar_) {
                    progress_bar_->setValue(value);
                }
            });
        slider_->initialize();

        progress_bar_ = std::make_unique<Components::ProgressBar>();
        progress_bar_->minimum(0).maximum(100).value(50);
        progress_bar_->initialize();

        control_layout->addRow("Slider:", slider_->getWidget());
        control_layout->addRow("Progress:", progress_bar_->getWidget());

        layout->addWidget(button_group);
        layout->addWidget(input_group);
        layout->addWidget(control_group);
        layout->addStretch();
    }

    void setupPerformanceMonitoring() {
        performance_monitor_ =
            std::make_unique<HotReload::PerformanceMonitor>();
        performance_monitor_->startMonitoring();
    }

    // Event handlers
    void onFileNew() { statusBar()->showMessage("New file", 2000); }

    void onFileOpen() { statusBar()->showMessage("Open file", 2000); }

    void onFileSave() { statusBar()->showMessage("Save file", 2000); }

    void onEditUndo() { statusBar()->showMessage("Undo", 2000); }

    void onEditRedo() { statusBar()->showMessage("Redo", 2000); }

    void onHelpAbout() {
        QMessageBox::about(this, "About",
                           "Comprehensive Qt Components Demo\n\n"
                           "This application demonstrates working components "
                           "from the DeclarativeUI framework.\n\n"
                           "Built with Qt and the DeclarativeUI framework.");
    }

private:
    QWidget* components_widget_ = nullptr;

    // Components
    std::unique_ptr<Components::Button> primary_button_;
    std::unique_ptr<Components::Button> secondary_button_;
    std::unique_ptr<Components::LineEdit> line_edit_;
    std::unique_ptr<Components::ComboBox> combo_box_;
    std::unique_ptr<Components::CheckBox> check_box_;
    std::unique_ptr<Components::Slider> slider_;
    std::unique_ptr<Components::ProgressBar> progress_bar_;

    // Performance monitoring
    std::unique_ptr<HotReload::PerformanceMonitor> performance_monitor_;
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    ComprehensiveExampleWindow window;
    window.show();

    return app.exec();
}

#include "29_comprehensive_example.moc"
