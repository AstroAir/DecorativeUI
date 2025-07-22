// Examples/new_components_showcase.cpp
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QDebug>

#include "Components/RadioButton.hpp"
#include "Components/Dial.hpp"
#include "Components/LCDNumber.hpp"
#include "Components/Calendar.hpp"
#include "Components/Splitter.hpp"
#include "Components/ScrollArea.hpp"
#include "Components/MessageBox.hpp"
#include "Components/FileDialog.hpp"
#include "Components/ColorDialog.hpp"

using namespace DeclarativeUI::Components;

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    QMainWindow window;
    window.setWindowTitle("New Components Showcase");
    window.setMinimumSize(800, 600);

    auto* centralWidget = new QWidget();
    window.setCentralWidget(centralWidget);
    auto* mainLayout = new QVBoxLayout(centralWidget);

    // **Input Components Section**
    auto* inputGroup = new QGroupBox("New Input Components");
    auto* inputLayout = new QHBoxLayout(inputGroup);

    // RadioButton with ButtonGroup
    auto* radioGroup = new ButtonGroup();
    radioGroup->exclusive(true);

    auto* radio1 = new RadioButton();
    radio1->text("Fast")
          .checked(true)
          .onToggled([](bool checked) {
              if (checked) qDebug() << "Fast mode selected";
          });

    auto* radio2 = new RadioButton();
    radio2->text("Medium")
          .onToggled([](bool checked) {
              if (checked) qDebug() << "Medium mode selected";
          });

    auto* radio3 = new RadioButton();
    radio3->text("Slow")
          .onToggled([](bool checked) {
              if (checked) qDebug() << "Slow mode selected";
          });

    // Initialize components
    radio1->initialize();
    radio2->initialize();
    radio3->initialize();
    radioGroup->initialize();

    // Add to button group
    radioGroup->addButton(qobject_cast<QRadioButton*>(radio1->getWidget()), 1)
              .addButton(qobject_cast<QRadioButton*>(radio2->getWidget()), 2)
              .addButton(qobject_cast<QRadioButton*>(radio3->getWidget()), 3);

    inputLayout->addWidget(radio1->getWidget());
    inputLayout->addWidget(radio2->getWidget());
    inputLayout->addWidget(radio3->getWidget());

    // Dial component
    auto* dial = new Dial();
    dial->minimum(0)
        .maximum(360)
        .value(180)
        .notchesVisible(true)
        .wrapping(true)
        .onValueChanged([](int value) {
            qDebug() << "Dial value changed to:" << value;
        });
    dial->initialize();

    inputLayout->addWidget(dial->getWidget());
    mainLayout->addWidget(inputGroup);

    // **Display Components Section**
    auto* displayGroup = new QGroupBox("New Display Components");
    auto* displayLayout = new QHBoxLayout(displayGroup);

    // LCDNumber component
    auto* lcd = new LCDNumber();
    lcd->digitCount(8)
       .value(12345.67)
       .mode(QLCDNumber::Dec)
       .segmentStyle(QLCDNumber::Filled)
       .smallDecimalPoint(false);
    lcd->initialize();

    displayLayout->addWidget(lcd->getWidget());

    // Calendar component
    auto* calendar = new Calendar();
    calendar->gridVisible(true)
            .navigationBarVisible(true)
            .dateEditEnabled(true)
            .firstDayOfWeek(Qt::Monday)
            .onClicked([](const QDate& date) {
                qDebug() << "Date clicked:" << date.toString();
            })
            .onSelectionChanged([]() {
                qDebug() << "Calendar selection changed";
            });
    calendar->initialize();

    displayLayout->addWidget(calendar->getWidget());
    mainLayout->addWidget(displayGroup);

    // **Container Components Section**
    auto* containerGroup = new QGroupBox("New Container Components");
    auto* containerLayout = new QVBoxLayout(containerGroup);

    // Splitter component
    auto* splitter = new Splitter();
    splitter->orientation(Qt::Horizontal)
            .childrenCollapsible(true)
            .handleWidth(5)
            .onSplitterMoved([](int pos, int index) {
                qDebug() << "Splitter moved - Position:" << pos << "Index:" << index;
            });
    splitter->initialize();

    // Create content for splitter
    auto* leftPanel = new QWidget();
    leftPanel->setStyleSheet("background-color: #E3F2FD; border: 1px solid #2196F3;");
    leftPanel->setMinimumWidth(150);

    auto* rightPanel = new QWidget();
    rightPanel->setStyleSheet("background-color: #F3E5F5; border: 1px solid #9C27B0;");
    rightPanel->setMinimumWidth(150);

    splitter->addWidget(leftPanel)
            .addWidget(rightPanel)
            .setSizes({200, 300});

    containerLayout->addWidget(splitter->getWidget());

    // ScrollArea component
    auto* scrollArea = new ScrollArea();
    scrollArea->widgetResizable(true)
              .horizontalScrollBarPolicy(Qt::ScrollBarAsNeeded)
              .verticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollArea->initialize();

    auto* scrollContent = new QWidget();
    scrollContent->setMinimumSize(600, 400);
    scrollContent->setStyleSheet("background-color: #FFF3E0; border: 2px dashed #FF9800;");
    scrollArea->widget(scrollContent);

    containerLayout->addWidget(scrollArea->getWidget());
    mainLayout->addWidget(containerGroup);

    // **Dialog Components Section**
    auto* dialogGroup = new QGroupBox("Dialog Components");
    auto* dialogLayout = new QHBoxLayout(dialogGroup);

    // Message Box button
    auto* msgButton = new QPushButton("Show Info");
    QObject::connect(msgButton, &QPushButton::clicked, [&window]() {
        MessageBox::information(&window, "Information", 
                               "This demonstrates the new MessageBox component!");
    });
    dialogLayout->addWidget(msgButton);

    // File Dialog button
    auto* fileButton = new QPushButton("Open File");
    QObject::connect(fileButton, &QPushButton::clicked, [&window]() {
        QString file = FileDialog::getOpenFileName(&window, "Select File", 
                                                  "", "All Files (*.*)");
        if (!file.isEmpty()) {
            qDebug() << "Selected file:" << file;
        }
    });
    dialogLayout->addWidget(fileButton);

    // Color Dialog button
    auto* colorButton = new QPushButton("Pick Color");
    QObject::connect(colorButton, &QPushButton::clicked, [&window]() {
        QColor color = ColorDialog::getColor(Qt::blue, &window, "Choose Color");
        if (color.isValid()) {
            qDebug() << "Selected color:" << color.name();
        }
    });
    dialogLayout->addWidget(colorButton);

    mainLayout->addWidget(dialogGroup);

    // **Connect dial to LCD for demonstration**
    QObject::connect(qobject_cast<QDial*>(dial->getWidget()), &QDial::valueChanged,
                     [lcd](int value) {
                         lcd->setValue(value);
                     });

    window.show();
    return app.exec();
}
