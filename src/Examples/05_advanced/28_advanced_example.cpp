// Examples/advanced_example.cpp
#include <QApplication>
#include <QDebug>
#include <QFileDialog>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QMessageBox>
#include <QStatusBar>
#include <QTabWidget>
#include <QTextStream>
#include <QTimer>
#include <QVBoxLayout>

#include "../../Binding/StateManager.hpp"
#include "../../Components/Button.hpp"
#include "../../Components/CheckBox.hpp"
#include "../../Components/ComboBox.hpp"
#include "../../Components/Label.hpp"
#include "../../Components/LineEdit.hpp"
#include "../../Components/ProgressBar.hpp"
#include "../../Components/Slider.hpp"
#include "../../Components/SpinBox.hpp"
#include "../../Components/TableView.hpp"
#include "../../Components/TextEdit.hpp"
#include "../../Components/TreeView.hpp"
#include "../../HotReload/HotReloadManager.hpp"
#include "../../HotReload/PerformanceMonitor.hpp"

using namespace DeclarativeUI;

class AdvancedExampleWindow : public QMainWindow {
    Q_OBJECT

public:
    AdvancedExampleWindow(QWidget* parent = nullptr);

private slots:
    void onBasicComponentsDemo();
    void onAdvancedComponentsDemo();
    void onStateManagementDemo();
    void onPerformanceMonitoringDemo();
    void onHotReloadDemo();
    void onDataBindingDemo();

private:
    void setupUI();
    void setupHotReload();
    void setupPerformanceMonitoring();
    void setupStateManagement();
    void createBasicComponentsTab();
    void createAdvancedComponentsTab();
    void createStateManagementTab();
    void createPerformanceTab();
    void createHotReloadTab();

    QTabWidget* tab_widget_;
    std::unique_ptr<HotReload::HotReloadManager> hot_reload_manager_;
    std::unique_ptr<HotReload::PerformanceMonitor> performance_monitor_;
    Binding::StateManager* state_manager_;

    // Component examples
    std::unique_ptr<Components::Button> demo_button_;
    std::unique_ptr<Components::Label> status_label_;
    std::unique_ptr<Components::LineEdit> text_input_;
    std::unique_ptr<Components::ComboBox> combo_box_;
    std::unique_ptr<Components::CheckBox> check_box_;
    std::unique_ptr<Components::Slider> slider_;
    std::unique_ptr<Components::ProgressBar> progress_bar_;
    std::unique_ptr<Components::SpinBox> spin_box_;
    std::unique_ptr<Components::TextEdit> text_edit_;
    std::unique_ptr<Components::TableView> table_view_;
    std::unique_ptr<Components::TreeView> tree_view_;

    // Performance monitoring
    QTimer* performance_timer_;
    int operation_counter_ = 0;
};

AdvancedExampleWindow::AdvancedExampleWindow(QWidget* parent)
    : QMainWindow(parent), state_manager_(&Binding::StateManager::instance()) {
    setWindowTitle("🔥 Advanced Qt Hot Reload Demo");
    setMinimumSize(1200, 800);

    setupStateManagement();
    setupHotReload();
    setupPerformanceMonitoring();
    setupUI();

    // **Show welcome message**
    QTimer::singleShot(500, [this]() {
        QMessageBox::information(
            this, "Welcome",
            "Welcome to the Advanced Qt Hot Reload Demo!\n\n"
            "This demo showcases:\n"
            "• New advanced components (TableView, TreeView, TextEdit, etc.)\n"
            "• Enhanced state management with validation and history\n"
            "• Performance monitoring and optimization\n"
            "• Improved hot reload system with better error handling\n"
            "• Comprehensive data binding examples\n\n"
            "Try editing the UI files in the Resources directory to see hot "
            "reload in action!");
    });
}

void AdvancedExampleWindow::setupUI() {
    auto* central_widget = new QWidget(this);
    setCentralWidget(central_widget);

    auto* layout = new QVBoxLayout(central_widget);

    // **Title**
    auto title_label = std::make_unique<Components::Label>();
    title_label->text("🔥 Advanced Qt Hot Reload Demo")
        .font(QFont("Arial", 24, QFont::Bold))
        .color(QColor("#2c3e50"))
        .alignment(Qt::AlignCenter);
    title_label->initialize();
    layout->addWidget(title_label->getWidget());

    // **Tab widget**
    tab_widget_ = new QTabWidget(this);
    layout->addWidget(tab_widget_);

    createBasicComponentsTab();
    createAdvancedComponentsTab();
    createStateManagementTab();
    createPerformanceTab();
    createHotReloadTab();

    // **Status bar**
    auto* status_bar = statusBar();
    status_bar->showMessage("Ready - Hot reload is active");

    // **Update status periodically**
    auto* status_timer = new QTimer(this);
    connect(status_timer, &QTimer::timeout, [this]() {
        QString status =
            QString("Operations: %1 | Success Rate: %2% | Hot Reload: %3")
                .arg(operation_counter_)
                .arg(performance_monitor_->getSuccessRate() * 100.0, 0, 'f', 1)
                .arg(hot_reload_manager_->isEnabled() ? "Active" : "Disabled");
        statusBar()->showMessage(status);
    });
    status_timer->start(2000);
}

void AdvancedExampleWindow::createBasicComponentsTab() {
    auto* tab = new QWidget();
    tab_widget_->addTab(tab, "Basic Components");

    auto* layout = new QVBoxLayout(tab);

    // **Button demo**
    auto* button_group = new QGroupBox("Button Demo");
    auto* button_layout = new QHBoxLayout(button_group);

    demo_button_ = std::make_unique<Components::Button>();
    demo_button_->text("Click Me!")
        .style(
            "QPushButton { background-color: #3498db; color: white; padding: "
            "10px; border-radius: 5px; }")
        .onClick([this]() {
            operation_counter_++;
            QMessageBox::information(
                this, "Button Clicked",
                QString("Button clicked %1 times!").arg(operation_counter_));
        });
    demo_button_->initialize();
    button_layout->addWidget(demo_button_->getWidget());

    layout->addWidget(button_group);

    // **Input controls**
    auto* input_group = new QGroupBox("Input Controls");
    auto* input_layout = new QVBoxLayout(input_group);

    // Line edit
    text_input_ = std::make_unique<Components::LineEdit>();
    text_input_->placeholder("Enter text here...")
        .onTextChanged([this](const QString& text) {
            if (status_label_) {
                status_label_->setText(QString("Text: %1").arg(text));
            }
        });
    text_input_->initialize();
    input_layout->addWidget(text_input_->getWidget());

    // Combo box
    combo_box_ = std::make_unique<Components::ComboBox>();
    combo_box_->items({"Option 1", "Option 2", "Option 3"})
        .onCurrentIndexChanged([this](int index) {
            if (status_label_) {
                status_label_->setText(QString("Selected: %1").arg(index));
            }
        });
    combo_box_->initialize();
    input_layout->addWidget(combo_box_->getWidget());

    // Checkbox
    check_box_ = std::make_unique<Components::CheckBox>();
    check_box_->text("Enable feature").onToggled([this](bool checked) {
        if (status_label_) {
            status_label_->setText(
                QString("Checked: %1").arg(checked ? "Yes" : "No"));
        }
    });
    check_box_->initialize();
    input_layout->addWidget(check_box_->getWidget());

    layout->addWidget(input_group);

    // **Status label**
    status_label_ = std::make_unique<Components::Label>();
    status_label_->text("Status: Ready")
        .style(
            "QLabel { background-color: #ecf0f1; padding: 10px; border-radius: "
            "5px; }");
    status_label_->initialize();
    layout->addWidget(status_label_->getWidget());

    layout->addStretch();
}

void AdvancedExampleWindow::createAdvancedComponentsTab() {
    auto* tab = new QWidget();
    tab_widget_->addTab(tab, "Advanced Components");

    auto* layout = new QVBoxLayout(tab);

    // **Table view demo**
    auto* table_group = new QGroupBox("Table View Demo");
    auto* table_layout = new QVBoxLayout(table_group);

    table_view_ = std::make_unique<Components::TableView>();
    table_view_->alternatingRowColors(true).sortingEnabled(true).onCellClicked(
        [this](int row, int column) {
            QMessageBox::information(
                this, "Cell Clicked",
                QString("Clicked cell at row %1, column %2")
                    .arg(row)
                    .arg(column));
        });
    table_view_->initialize();

    // **Populate table with sample data**
    table_view_->setData(0, 0, "Name");
    table_view_->setData(0, 1, "Age");
    table_view_->setData(0, 2, "City");
    table_view_->setData(1, 0, "Alice");
    table_view_->setData(1, 1, 25);
    table_view_->setData(1, 2, "New York");
    table_view_->setData(2, 0, "Bob");
    table_view_->setData(2, 1, 30);
    table_view_->setData(2, 2, "London");

    table_layout->addWidget(table_view_->getWidget());
    layout->addWidget(table_group);

    // **Tree view demo**
    auto* tree_group = new QGroupBox("Tree View Demo");
    auto* tree_layout = new QVBoxLayout(tree_group);

    tree_view_ = std::make_unique<Components::TreeView>();
    tree_view_->headerHidden(true).animated(true).onItemClicked(
        [this](const QModelIndex& index) {
            auto* item = tree_view_->getItemFromIndex(index);
            if (item) {
                QMessageBox::information(
                    this, "Item Clicked",
                    QString("Clicked item: %1").arg(item->text()));
            }
        });
    tree_view_->initialize();

    // **Populate tree with sample data**
    auto* root1 = tree_view_->addRootItem("Root Item 1");
    tree_view_->addChildItem(root1, "Child 1.1");
    tree_view_->addChildItem(root1, "Child 1.2");

    auto* root2 = tree_view_->addRootItem("Root Item 2");
    tree_view_->addChildItem(root2, "Child 2.1");
    tree_view_->addChildItem(root2, "Child 2.2");

    tree_layout->addWidget(tree_view_->getWidget());
    layout->addWidget(tree_group);

    // **Text edit demo**
    auto* text_group = new QGroupBox("Text Edit Demo");
    auto* text_layout = new QVBoxLayout(text_group);

    text_edit_ = std::make_unique<Components::TextEdit>();
    text_edit_
        ->html(
            "<h2>Rich Text Demo</h2><p>This is a <b>bold</b> text with "
            "<i>italic</i> styling.</p>")
        .onTextChanged([]() {
            // Update word count or other metrics
        });
    text_edit_->initialize();

    text_layout->addWidget(text_edit_->getWidget());
    layout->addWidget(text_group);
}

void AdvancedExampleWindow::createStateManagementTab() {
    auto* tab = new QWidget();
    tab_widget_->addTab(tab, "State Management");

    auto* layout = new QVBoxLayout(tab);

    // **State management demo**
    auto* state_group = new QGroupBox("State Management Demo");
    auto* state_layout = new QVBoxLayout(state_group);

    // **Create reactive state**
    auto counter_state = state_manager_->createState<int>("demo_counter", 0);
    auto text_state =
        state_manager_->createState<QString>("demo_text", "Hello World");

    // **Enable state history**
    state_manager_->enableHistory("demo_counter", 20);
    state_manager_->enableHistory("demo_text", 10);

    // **Counter controls**
    auto* counter_widget = new QWidget();
    auto* counter_layout = new QHBoxLayout(counter_widget);

    auto increment_btn = std::make_unique<Components::Button>();
    increment_btn->text("Increment").onClick([counter_state]() {
        counter_state->set(counter_state->get() + 1);
    });
    increment_btn->initialize();
    counter_layout->addWidget(increment_btn->getWidget());

    auto decrement_btn = std::make_unique<Components::Button>();
    decrement_btn->text("Decrement").onClick([counter_state]() {
        counter_state->set(counter_state->get() - 1);
    });
    decrement_btn->initialize();
    counter_layout->addWidget(decrement_btn->getWidget());

    auto undo_btn = std::make_unique<Components::Button>();
    undo_btn->text("Undo").onClick([this]() {
        if (state_manager_->canUndo("demo_counter")) {
            state_manager_->undo("demo_counter");
        }
    });
    undo_btn->initialize();
    counter_layout->addWidget(undo_btn->getWidget());

    auto redo_btn = std::make_unique<Components::Button>();
    redo_btn->text("Redo").onClick([this]() {
        if (state_manager_->canRedo("demo_counter")) {
            state_manager_->redo("demo_counter");
        }
    });
    redo_btn->initialize();
    counter_layout->addWidget(redo_btn->getWidget());

    state_layout->addWidget(counter_widget);

    // **Counter display**
    auto counter_label = std::make_unique<Components::Label>();
    counter_label->text("Counter: 0")
        .style("QLabel { font-size: 18px; font-weight: bold; }");
    counter_label->initialize();

    // **Bind counter display to state**
    connect(counter_state.get(), &Binding::ReactivePropertyBase::valueChanged,
            [counter_label = counter_label.get(), counter_state]() {
                counter_label->setText(
                    QString("Counter: %1").arg(counter_state->get()));
            });

    state_layout->addWidget(counter_label->getWidget());

    layout->addWidget(state_group);
    layout->addStretch();
}

void AdvancedExampleWindow::createPerformanceTab() {
    auto* tab = new QWidget();
    tab_widget_->addTab(tab, "Performance");

    auto* layout = new QVBoxLayout(tab);

    // **Performance controls**
    auto* perf_group = new QGroupBox("Performance Monitoring");
    auto* perf_layout = new QVBoxLayout(perf_group);

    auto perf_report_btn = std::make_unique<Components::Button>();
    perf_report_btn->text("Generate Performance Report").onClick([this]() {
        QString report = performance_monitor_->generateReport();
        QMessageBox::information(this, "Performance Report", report);
    });
    perf_report_btn->initialize();
    perf_layout->addWidget(perf_report_btn->getWidget());

    layout->addWidget(perf_group);
    layout->addStretch();
}

void AdvancedExampleWindow::createHotReloadTab() {
    auto* tab = new QWidget();
    tab_widget_->addTab(tab, "Hot Reload");

    auto* layout = new QVBoxLayout(tab);

    // **Hot reload controls**
    auto* reload_group = new QGroupBox("Hot Reload Controls");
    auto* reload_layout = new QVBoxLayout(reload_group);

    auto reload_btn = std::make_unique<Components::Button>();
    reload_btn->text("Manual Reload").onClick([this]() {
        hot_reload_manager_->reloadAll();
    });
    reload_btn->initialize();
    reload_layout->addWidget(reload_btn->getWidget());

    auto enable_btn = std::make_unique<Components::Button>();
    enable_btn->text("Toggle Hot Reload").onClick([this]() {
        hot_reload_manager_->setEnabled(!hot_reload_manager_->isEnabled());
    });
    enable_btn->initialize();
    reload_layout->addWidget(enable_btn->getWidget());

    layout->addWidget(reload_group);
    layout->addStretch();
}

void AdvancedExampleWindow::setupHotReload() {
    hot_reload_manager_ = std::make_unique<HotReload::HotReloadManager>(this);

    // **Set up error handling**
    hot_reload_manager_->setErrorHandler(
        [this](const QString& file, const QString& error) {
            QMessageBox::warning(
                this, "Hot Reload Error",
                QString("Failed to reload %1:\n%2").arg(file).arg(error));
        });

    // **Register UI directory**
    hot_reload_manager_->registerUIDirectory("Resources/ui_definitions", true);

    // **Connect hot reload signals**
    connect(hot_reload_manager_.get(),
            &HotReload::HotReloadManager::reloadStarted,
            [this](const QString& file) {
                statusBar()->showMessage(QString("Reloading %1...").arg(file));
            });

    connect(hot_reload_manager_.get(),
            &HotReload::HotReloadManager::reloadCompleted,
            [this](const QString& file) {
                statusBar()->showMessage(
                    QString("Successfully reloaded %1").arg(file), 3000);
            });

    connect(
        hot_reload_manager_.get(), &HotReload::HotReloadManager::reloadFailed,
        [this](const QString& file, const QString& error) {
            statusBar()->showMessage(
                QString("Failed to reload %1: %2").arg(file).arg(error), 5000);
        });
}

void AdvancedExampleWindow::setupPerformanceMonitoring() {
    performance_monitor_ =
        std::make_unique<HotReload::PerformanceMonitor>(this);
    performance_monitor_->startMonitoring();
    performance_monitor_->setWarningThreshold(500);  // 500ms threshold

    // **Connect performance signals**
    connect(performance_monitor_.get(),
            &HotReload::PerformanceMonitor::performanceWarning,
            [](const QString& file, qint64 time) {
                qWarning() << "Performance warning:" << file << "took" << time
                           << "ms";
            });

    connect(performance_monitor_.get(),
            &HotReload::PerformanceMonitor::slowOperationDetected,
            [](const QString& operation, qint64 time) {
                qWarning() << "Slow operation detected:" << operation << "took"
                           << time << "ms";
            });
}

void AdvancedExampleWindow::setupStateManagement() {
    state_manager_->enablePerformanceMonitoring(true);
    state_manager_->enableDebugMode(true);

    // **Connect state management signals**
    connect(state_manager_, &Binding::StateManager::stateChanged,
            [](const QString& key, const QVariant& value) {
                qDebug() << "State changed:" << key << "=" << value;
            });

    connect(state_manager_, &Binding::StateManager::performanceWarning,
            [](const QString& key, qint64 time) {
                qWarning() << "State performance warning:" << key << "took"
                           << time << "ms";
            });
}

void AdvancedExampleWindow::onBasicComponentsDemo() {
    qDebug() << "🔧 Basic Components Demo triggered";
    // TODO: Implement basic components demonstration
}

void AdvancedExampleWindow::onAdvancedComponentsDemo() {
    qDebug() << "⚙️ Advanced Components Demo triggered";
    // TODO: Implement advanced components demonstration
}

void AdvancedExampleWindow::onStateManagementDemo() {
    qDebug() << "📊 State Management Demo triggered";
    // TODO: Implement state management demonstration
}

void AdvancedExampleWindow::onPerformanceMonitoringDemo() {
    qDebug() << "📈 Performance Monitoring Demo triggered";
    // TODO: Implement performance monitoring demonstration
}

void AdvancedExampleWindow::onHotReloadDemo() {
    qDebug() << "🔥 Hot Reload Demo triggered";
    // TODO: Implement hot reload demonstration
}

void AdvancedExampleWindow::onDataBindingDemo() {
    qDebug() << "🔗 Data Binding Demo triggered";
    // TODO: Implement data binding demonstration
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    AdvancedExampleWindow window;
    window.show();

    return app.exec();
}

#include "28_advanced_example.moc"
