// Examples/enhanced_components_showcase.cpp
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QTabWidget>
#include <QTextCharFormat>
#include <QPushButton>
#include <QStatusBar>
#include <QLabel>
#include <QDebug>

// Include enhanced and new components
#include "../../Components/Calendar.hpp"
#include "../../Components/FontDialog.hpp"
#include "../../Components/ListView.hpp"
#include "../../Components/ToolButton.hpp"
#include "../../Components/Frame.hpp"
#include "../../Components/Widget.hpp"

using namespace DeclarativeUI::Components;

class EnhancedComponentsShowcase : public QMainWindow {
    Q_OBJECT

public:
    EnhancedComponentsShowcase(QWidget* parent = nullptr) : QMainWindow(parent) {
        setupUI();
    }

private:
    void setupUI() {
        setWindowTitle("Enhanced Components Showcase");
        setMinimumSize(1000, 700);

        auto* centralWidget = new QWidget();
        setCentralWidget(centralWidget);
        auto* mainLayout = new QVBoxLayout(centralWidget);

        auto* tabWidget = new QTabWidget();
        mainLayout->addWidget(tabWidget);

        createEnhancedCalendarTab(tabWidget);
        createNewComponentsTab(tabWidget);
        createContainerComponentsTab(tabWidget);
    }

    void createEnhancedCalendarTab(QTabWidget* tabWidget) {
        auto* calendarTab = new Widget();
        calendarTab->vBoxLayout()
                   .margins(10, 10, 10, 10)
                   .spacing(15);
        calendarTab->initialize();

        // Enhanced Calendar with all features
        auto* calendar = new Calendar();
        calendar->gridVisible(true)
                .navigationBarVisible(true)
                .dateEditEnabled(true)
                .firstDayOfWeek(Qt::Monday)
                .showWeekNumbers(true)
                .highlightToday(true)
                .enableMultiSelection(true)
                .onClicked([this](const QDate& date) {
                    qDebug() << "Date clicked:" << date.toString();
                    statusBar()->showMessage(QString("Selected: %1").arg(date.toString()));
                })
                .onDateDoubleClicked([this](const QDate& date) {
                    qDebug() << "Date double-clicked:" << date.toString();
                })
                .onSelectionChanged([this]() {
                    qDebug() << "Calendar selection changed";
                });

        // Add special dates and holidays
        QList<QDate> holidays = {
            QDate(2024, 1, 1),   // New Year
            QDate(2024, 7, 4),   // Independence Day
            QDate(2024, 12, 25)  // Christmas
        };
        calendar->setHolidays(holidays);

        // Add special dates
        calendar->addSpecialDate(QDate::currentDate().addDays(7), "Important Meeting")
                .addSpecialDate(QDate::currentDate().addDays(14), "Project Deadline");

        // Custom formatting
        QTextCharFormat weekendFormat;
        weekendFormat.setForeground(QColor(255, 0, 0));
        weekendFormat.setBackground(QColor(255, 240, 240));
        calendar->weekdayTextFormat(Qt::Saturday, weekendFormat)
                .weekdayTextFormat(Qt::Sunday, weekendFormat);

        calendar->initialize();

        // Control buttons
        auto* buttonFrame = new Frame();
        buttonFrame->frameShape(QFrame::StyledPanel)
                   .hBoxLayout()
                   .spacing(10);
        buttonFrame->initialize();

        auto* selectRangeBtn = new QPushButton("Select Week Range");
        connect(selectRangeBtn, &QPushButton::clicked, [calendar]() {
            QDate start = QDate::currentDate();
            QDate end = start.addDays(6);
            calendar->selectDateRange(start, end);
        });

        auto* clearBtn = new QPushButton("Clear Selection");
        connect(clearBtn, &QPushButton::clicked, [calendar]() {
            calendar->clearSelection();
        });

        auto* todayBtn = new QPushButton("Go to Today");
        connect(todayBtn, &QPushButton::clicked, [calendar]() {
            calendar->setSelectedDate(QDate::currentDate());
        });

        buttonFrame->addWidget(selectRangeBtn);
        buttonFrame->addWidget(clearBtn);
        buttonFrame->addWidget(todayBtn);

        calendarTab->addWidget(calendar->getWidget());
        calendarTab->addWidget(buttonFrame->getWidget());

        tabWidget->addTab(calendarTab->getWidget(), "Enhanced Calendar");
    }

    void createNewComponentsTab(QTabWidget* tabWidget) {
        auto* newTab = new Widget();
        newTab->gridLayout()
              .margins(10, 10, 10, 10)
              .spacing(15);
        newTab->initialize();

        // ListView Component
        auto* listView = new ListView();
        listView->stringListModel({"Item 1", "Item 2", "Item 3", "Item 4", "Item 5"})
                .selectionMode(QAbstractItemView::ExtendedSelection)
                .alternatingRowColors(true)
                .onClicked([this](const QModelIndex& index) {
                    statusBar()->showMessage(QString("List item clicked: %1").arg(index.data().toString()));
                })
                .onDoubleClicked([this](const QModelIndex& index) {
                    qDebug() << "List item double-clicked:" << index.data().toString();
                });
        listView->initialize();

        // Add some items dynamically
        listView->addItem("Dynamic Item 1");
        listView->addItems({"Dynamic Item 2", "Dynamic Item 3"});

        // ToolButton Component
        auto* toolButton = new ToolButton();
        toolButton->text("Tool Action")
                  .icon(QIcon(":/icons/tool.png"))
                  .toolButtonStyle(Qt::ToolButtonTextBesideIcon)
                  .autoRaise(true)
                  .checkable(true)
                  .onClicked([this]() {
                      statusBar()->showMessage("Tool button clicked!");
                  })
                  .onToggled([this](bool checked) {
                      qDebug() << "Tool button toggled:" << checked;
                  });
        toolButton->initialize();

        // Font Dialog Button
        auto* fontBtn = new QPushButton("Choose Font");
        connect(fontBtn, &QPushButton::clicked, [this]() {
            bool ok;
            QFont font = FontDialog::getFont(&ok, QFont("Arial", 12), this, "Select Font");
            if (ok) {
                statusBar()->showMessage(QString("Selected font: %1, %2pt").arg(font.family()).arg(font.pointSize()));
            }
        });

        newTab->addWidget(listView->getWidget(), 0, 0, 3, 1);
        newTab->addWidget(toolButton->getWidget(), 0, 1);
        newTab->addWidget(fontBtn, 1, 1);

        tabWidget->addTab(newTab->getWidget(), "New Components");
    }

    void createContainerComponentsTab(QTabWidget* tabWidget) {
        auto* containerTab = new Widget();
        containerTab->vBoxLayout()
                    .margins(10, 10, 10, 10)
                    .spacing(15);
        containerTab->initialize();

        // Frame with different styles
        auto* styledFrame = new Frame();
        styledFrame->frameStyle(QFrame::Box, QFrame::Raised)
                   .lineWidth(2)
                   .midLineWidth(1)
                   .contentsMargins(10, 10, 10, 10)
                   .vBoxLayout();
        styledFrame->initialize();

        auto* frameLabel = new QLabel("This is a styled frame with Box shape and Raised shadow");
        frameLabel->setWordWrap(true);
        styledFrame->addWidget(frameLabel);

        // Generic Widget with custom layout
        auto* customWidget = new Widget();
        customWidget->minimumSize(QSize(300, 150))
                    .style("background-color: #f0f0f0; border: 1px solid #ccc; border-radius: 5px;")
                    .gridLayout()
                    .spacing(10)
                    .margins(15, 15, 15, 15);
        customWidget->initialize();

        // Add widgets to grid
        customWidget->addWidget(new QLabel("Row 0, Col 0"), 0, 0);
        customWidget->addWidget(new QLabel("Row 0, Col 1"), 0, 1);
        customWidget->addWidget(new QLabel("Row 1, Col 0"), 1, 0);
        customWidget->addWidget(new QLabel("Row 1, Col 1"), 1, 1);

        // Another frame with different style
        auto* sunkenFrame = new Frame();
        sunkenFrame->frameStyle(QFrame::Panel, QFrame::Sunken)
                   .lineWidth(3)
                   .style("background-color: #e8e8e8;")
                   .hBoxLayout()
                   .spacing(10);
        sunkenFrame->initialize();

        sunkenFrame->addWidget(new QLabel("Sunken Panel Frame"));
        sunkenFrame->addWidget(new QPushButton("Button in Frame"));

        containerTab->addWidget(styledFrame->getWidget());
        containerTab->addWidget(customWidget->getWidget());
        containerTab->addWidget(sunkenFrame->getWidget());

        tabWidget->addTab(containerTab->getWidget(), "Container Components");
    }
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    EnhancedComponentsShowcase window;
    window.show();

    return app.exec();
}

#include "09_enhanced_components.moc"
