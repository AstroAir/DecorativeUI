// Examples/comprehensive_components_example.cpp
#include <QApplication>
#include <QDebug>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QPushButton>
#include <QTabWidget>
#include <QVBoxLayout>

// Include all new components
#include "Components/Calendar.hpp"
#include "Components/ColorDialog.hpp"
#include "Components/Dial.hpp"
#include "Components/DockWidget.hpp"
#include "Components/FileDialog.hpp"
#include "Components/LCDNumber.hpp"
#include "Components/MessageBox.hpp"
#include "Components/RadioButton.hpp"
#include "Components/ScrollArea.hpp"
#include "Components/Splitter.hpp"
#include "Components/StatusBar.hpp"
#include "Components/ToolBar.hpp"

using namespace DeclarativeUI::Components;

class ComprehensiveComponentsExample : public QMainWindow {
    Q_OBJECT

public:
    ComprehensiveComponentsExample(QWidget* parent = nullptr)
        : QMainWindow(parent) {
        setupUI();
    }

private:
    void setupUI() {
        setWindowTitle("Comprehensive Qt Components Example");
        setMinimumSize(1200, 800);

        // Create central widget with tabs
        auto* centralWidget = new QWidget();
        setCentralWidget(centralWidget);

        auto* mainLayout = new QVBoxLayout(centralWidget);
        auto* tabWidget = new QTabWidget();
        mainLayout->addWidget(tabWidget);

        // Create tabs for different component categories
        createInputComponentsTab(tabWidget);
        createDisplayComponentsTab(tabWidget);
        createContainerComponentsTab(tabWidget);
        createDialogComponentsTab(tabWidget);
        createAdvancedComponentsTab(tabWidget);

        // Setup status bar
        setupStatusBar();

        // Setup toolbar
        setupToolBar();

        // Setup dock widgets
        setupDockWidgets();
    }

    void createInputComponentsTab(QTabWidget* tabWidget) {
        auto* inputTab = new QWidget();
        auto* layout = new QGridLayout(inputTab);

        // RadioButton and ButtonGroup example
        auto* radioGroup = new ButtonGroup();
        radioGroup->exclusive(true).onButtonClicked(
            [this](QAbstractButton* button) {
                statusBar()->showMessage(
                    QString("Radio button clicked: %1").arg(button->text()));
            });

        auto* radio1 = new RadioButton();
        radio1->text("Option 1").checked(true).onToggled([this](bool checked) {
            if (checked)
                qDebug() << "Option 1 selected";
        });

        auto* radio2 = new RadioButton();
        radio2->text("Option 2").onToggled([this](bool checked) {
            if (checked)
                qDebug() << "Option 2 selected";
        });

        auto* radio3 = new RadioButton();
        radio3->text("Option 3").onToggled([this](bool checked) {
            if (checked)
                qDebug() << "Option 3 selected";
        });

        radio1->initialize();
        radio2->initialize();
        radio3->initialize();
        radioGroup->initialize();

        radioGroup->addButton(qobject_cast<QRadioButton*>(radio1->getWidget()),
                              1);
        radioGroup->addButton(qobject_cast<QRadioButton*>(radio2->getWidget()),
                              2);
        radioGroup->addButton(qobject_cast<QRadioButton*>(radio3->getWidget()),
                              3);

        layout->addWidget(radio1->getWidget(), 0, 0);
        layout->addWidget(radio2->getWidget(), 0, 1);
        layout->addWidget(radio3->getWidget(), 0, 2);

        // Dial example
        auto* dial = new Dial();
        dial->minimum(0)
            .maximum(100)
            .value(50)
            .notchesVisible(true)
            .onValueChanged([this](int value) {
                statusBar()->showMessage(QString("Dial value: %1").arg(value));
            });
        dial->initialize();

        layout->addWidget(dial->getWidget(), 1, 0, 1, 3);

        tabWidget->addTab(inputTab, "Input Components");
    }

    void createDisplayComponentsTab(QTabWidget* tabWidget) {
        auto* displayTab = new QWidget();
        auto* layout = new QGridLayout(displayTab);

        // LCDNumber example
        auto* lcd = new LCDNumber();
        lcd->digitCount(6)
            .value(123.45)
            .mode(QLCDNumber::Dec)
            .segmentStyle(QLCDNumber::Filled);
        lcd->initialize();

        layout->addWidget(lcd->getWidget(), 0, 0);

        // Calendar example
        auto* calendar = new Calendar();
        calendar->gridVisible(true).navigationBarVisible(true).onClicked(
            [this](const QDate& date) {
                statusBar()->showMessage(
                    QString("Date selected: %1").arg(date.toString()));
            });
        calendar->initialize();

        layout->addWidget(calendar->getWidget(), 1, 0);

        tabWidget->addTab(displayTab, "Display Components");
    }

    void createContainerComponentsTab(QTabWidget* tabWidget) {
        auto* containerTab = new QWidget();
        auto* layout = new QVBoxLayout(containerTab);

        // Splitter example
        auto* splitter = new Splitter();
        splitter->orientation(Qt::Horizontal)
            .childrenCollapsible(true)
            .onSplitterMoved([this](int pos, int index) {
                statusBar()->showMessage(
                    QString("Splitter moved: pos=%1, index=%2")
                        .arg(pos)
                        .arg(index));
            });
        splitter->initialize();

        // Add widgets to splitter
        auto* leftWidget = new QWidget();
        leftWidget->setStyleSheet("background-color: lightblue;");
        auto* rightWidget = new QWidget();
        rightWidget->setStyleSheet("background-color: lightgreen;");

        splitter->addWidget(leftWidget);
        splitter->addWidget(rightWidget);
        splitter->setSizes({200, 300});

        layout->addWidget(splitter->getWidget(0));  // Get the first widget

        // ScrollArea example
        auto* scrollArea = new ScrollArea();
        scrollArea->widgetResizable(true)
            .horizontalScrollBarPolicy(Qt::ScrollBarAsNeeded)
            .verticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        scrollArea->initialize();

        auto* scrollContent = new QWidget();
        scrollContent->setMinimumSize(800, 600);
        scrollContent->setStyleSheet("background-color: lightyellow;");
        scrollArea->widget(scrollContent);

        layout->addWidget(scrollArea->getWidget());

        tabWidget->addTab(containerTab, "Container Components");
    }

    void createDialogComponentsTab(QTabWidget* tabWidget) {
        auto* dialogTab = new QWidget();
        auto* layout = new QVBoxLayout(dialogTab);

        // MessageBox examples
        auto* msgButton = new QPushButton("Show Message Box");
        connect(msgButton, &QPushButton::clicked, [this]() {
            MessageBox::information(this, "Information",
                                    "This is an information message!");
        });
        layout->addWidget(msgButton);

        auto* questionButton = new QPushButton("Show Question Dialog");
        connect(questionButton, &QPushButton::clicked, [this]() {
            auto result = MessageBox::question(this, "Question",
                                               "Do you want to continue?");
            statusBar()->showMessage(result == QMessageBox::Yes
                                         ? "User clicked Yes"
                                         : "User clicked No");
        });
        layout->addWidget(questionButton);

        // FileDialog examples
        auto* fileButton = new QPushButton("Open File Dialog");
        connect(fileButton, &QPushButton::clicked, [this]() {
            QString fileName = FileDialog::getOpenFileName(
                this, "Open File", "", "All Files (*.*)");
            if (!fileName.isEmpty()) {
                statusBar()->showMessage(
                    QString("Selected file: %1").arg(fileName));
            }
        });
        layout->addWidget(fileButton);

        // ColorDialog example
        auto* colorButton = new QPushButton("Open Color Dialog");
        connect(colorButton, &QPushButton::clicked, [this]() {
            QColor color = ColorDialog::getColor(Qt::red, this, "Select Color");
            if (color.isValid()) {
                statusBar()->showMessage(
                    QString("Selected color: %1").arg(color.name()));
            }
        });
        layout->addWidget(colorButton);

        tabWidget->addTab(dialogTab, "Dialog Components");
    }

    void createAdvancedComponentsTab(QTabWidget* tabWidget) {
        auto* advancedTab = new QWidget();
        auto* layout = new QVBoxLayout(advancedTab);

        auto* label = new QLabel(
            "Advanced components are integrated into the main window:");
        layout->addWidget(label);

        auto* statusLabel =
            new QLabel("• StatusBar: Check the bottom of the window");
        layout->addWidget(statusLabel);

        auto* toolbarLabel =
            new QLabel("• ToolBar: Check the top of the window");
        layout->addWidget(toolbarLabel);

        auto* dockLabel =
            new QLabel("• DockWidget: Check the sides of the window");
        layout->addWidget(dockLabel);

        tabWidget->addTab(advancedTab, "Advanced Components");
    }

    void setupStatusBar() {
        auto* statusBar = new StatusBar();
        statusBar->showMessage("Ready - Comprehensive Components Example")
            .setSizeGripEnabled(true);
        statusBar->initialize();

        setStatusBar(qobject_cast<QStatusBar*>(statusBar->getWidget()));
    }

    void setupToolBar() {
        auto* toolBar = new ToolBar();
        toolBar->windowTitle("Main Toolbar")
            .movable(true)
            .addAction("New", []() { qDebug() << "New action triggered"; })
            .addAction("Open", []() { qDebug() << "Open action triggered"; })
            .addSeparator()
            .addAction("Save", []() { qDebug() << "Save action triggered"; })
            .onActionTriggered([this](QAction* action) {
                statusBar()->showMessage(
                    QString("Toolbar action: %1").arg(action->text()));
            });
        toolBar->initialize();

        addToolBar(qobject_cast<QToolBar*>(toolBar->getWidget()));
    }

    void setupDockWidgets() {
        // Left dock widget
        auto* leftDock = new DockWidget();
        leftDock->windowTitle("Properties")
            .allowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea)
            .onDockLocationChanged([this](Qt::DockWidgetArea area) {
                statusBar()->showMessage(QString("Dock moved to area: %1")
                                             .arg(static_cast<int>(area)));
            });
        leftDock->initialize();

        auto* leftContent = new QWidget();
        leftContent->setStyleSheet("background-color: lightcyan;");
        leftDock->widget(leftContent);

        addDockWidget(Qt::LeftDockWidgetArea,
                      qobject_cast<QDockWidget*>(leftDock->getWidget()));

        // Right dock widget
        auto* rightDock = new DockWidget();
        rightDock->windowTitle("Tools").allowedAreas(Qt::LeftDockWidgetArea |
                                                     Qt::RightDockWidgetArea);
        rightDock->initialize();

        auto* rightContent = new QWidget();
        rightContent->setStyleSheet("background-color: lavender;");
        rightDock->widget(rightContent);

        addDockWidget(Qt::RightDockWidgetArea,
                      qobject_cast<QDockWidget*>(rightDock->getWidget()));
    }
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    ComprehensiveComponentsExample window;
    window.show();

    return app.exec();
}

#include "08_comprehensive_components.moc"
