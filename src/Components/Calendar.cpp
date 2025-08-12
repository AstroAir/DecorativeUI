// Components/Calendar.cpp
#include "Calendar.hpp"

namespace DeclarativeUI::Components {

// **Constructor**
Calendar::Calendar(QObject* parent)
    : UIElement(parent), calendar_widget_(nullptr), multi_selection_enabled_(false), highlight_today_enabled_(true) {}

// **Basic Calendar Configuration**
Calendar& Calendar::selectedDate(const QDate& date) {
    return static_cast<Calendar&>(setProperty("selectedDate", date));
}

Calendar& Calendar::minimumDate(const QDate& date) {
    return static_cast<Calendar&>(setProperty("minimumDate", date));
}

Calendar& Calendar::maximumDate(const QDate& date) {
    return static_cast<Calendar&>(setProperty("maximumDate", date));
}

Calendar& Calendar::firstDayOfWeek(Qt::DayOfWeek dayOfWeek) {
    return static_cast<Calendar&>(setProperty("firstDayOfWeek", static_cast<int>(dayOfWeek)));
}

Calendar& Calendar::gridVisible(bool visible) {
    return static_cast<Calendar&>(setProperty("gridVisible", visible));
}

Calendar& Calendar::navigationBarVisible(bool visible) {
    return static_cast<Calendar&>(setProperty("navigationBarVisible", visible));
}

Calendar& Calendar::dateEditEnabled(bool enabled) {
    return static_cast<Calendar&>(setProperty("dateEditEnabled", enabled));
}

Calendar& Calendar::dateEditAcceptDelay(int delay) {
    return static_cast<Calendar&>(setProperty("dateEditAcceptDelay", delay));
}

Calendar& Calendar::verticalHeaderFormat(QCalendarWidget::VerticalHeaderFormat format) {
    return static_cast<Calendar&>(setProperty("verticalHeaderFormat", static_cast<int>(format)));
}

Calendar& Calendar::horizontalHeaderFormat(QCalendarWidget::HorizontalHeaderFormat format) {
    return static_cast<Calendar&>(setProperty("horizontalHeaderFormat", static_cast<int>(format)));
}

Calendar& Calendar::selectionMode(QCalendarWidget::SelectionMode mode) {
    return static_cast<Calendar&>(setProperty("selectionMode", static_cast<int>(mode)));
}

// **Event Handlers**
Calendar& Calendar::onSelectionChanged(std::function<void()> handler) {
    selection_changed_handler_ = std::move(handler);
    return *this;
}

Calendar& Calendar::onClicked(std::function<void(const QDate&)> handler) {
    clicked_handler_ = std::move(handler);
    return *this;
}

Calendar& Calendar::onActivated(std::function<void(const QDate&)> handler) {
    activated_handler_ = std::move(handler);
    return *this;
}

Calendar& Calendar::onCurrentPageChanged(std::function<void(int, int)> handler) {
    current_page_changed_handler_ = std::move(handler);
    return *this;
}

Calendar& Calendar::style(const QString& stylesheet) {
    return static_cast<Calendar&>(setProperty("styleSheet", stylesheet));
}

// **Enhanced Calendar Features**
Calendar& Calendar::locale(const QLocale& locale) {
    if (calendar_widget_) {
        calendar_widget_->setLocale(locale);
    }
    return *this;
}

Calendar& Calendar::weekdayTextFormat(Qt::DayOfWeek dayOfWeek, const QTextCharFormat& format) {
    if (calendar_widget_) {
        calendar_widget_->setWeekdayTextFormat(dayOfWeek, format);
    }
    return *this;
}

Calendar& Calendar::dateTextFormat(const QDate& date, const QTextCharFormat& format) {
    if (calendar_widget_) {
        calendar_widget_->setDateTextFormat(date, format);
    }
    return *this;
}

Calendar& Calendar::headerTextFormat(const QTextCharFormat& format) {
    if (calendar_widget_) {
        calendar_widget_->setHeaderTextFormat(format);
    }
    return *this;
}

Calendar& Calendar::setDateRange(const QDate& min, const QDate& max) {
    if (calendar_widget_) {
        calendar_widget_->setDateRange(min, max);
    }
    return *this;
}

Calendar& Calendar::highlightToday(bool highlight) {
    highlight_today_enabled_ = highlight;
    if (calendar_widget_ && highlight) {
        QTextCharFormat todayFormat;
        todayFormat.setBackground(QColor(255, 255, 0, 100)); // Light yellow highlight
        calendar_widget_->setDateTextFormat(QDate::currentDate(), todayFormat);
    }
    return *this;
}

Calendar& Calendar::showWeekNumbers(bool show) {
    if (calendar_widget_) {
        calendar_widget_->setVerticalHeaderFormat(show ?
            QCalendarWidget::ISOWeekNumbers : QCalendarWidget::NoVerticalHeader);
    }
    return *this;
}

Calendar& Calendar::enableMultiSelection(bool enable) {
    multi_selection_enabled_ = enable;
    return *this;
}

Calendar& Calendar::selectedDates(const QList<QDate>& dates) {
    selected_dates_ = dates;
    if (calendar_widget_ && multi_selection_enabled_) {
        // Highlight selected dates
        QTextCharFormat selectedFormat;
        selectedFormat.setBackground(QColor(0, 120, 215, 100)); // Light blue
        for (const QDate& date : dates) {
            calendar_widget_->setDateTextFormat(date, selectedFormat);
        }
    }
    return *this;
}

Calendar& Calendar::addSpecialDate(const QDate& date, const QString& tooltip) {
    if (!special_dates_.contains(date)) {
        special_dates_.append(date);
        if (calendar_widget_) {
            QTextCharFormat specialFormat;
            specialFormat.setBackground(QColor(255, 165, 0, 100)); // Light orange
            specialFormat.setToolTip(tooltip);
            calendar_widget_->setDateTextFormat(date, specialFormat);
        }
    }
    return *this;
}

Calendar& Calendar::removeSpecialDate(const QDate& date) {
    special_dates_.removeAll(date);
    if (calendar_widget_) {
        calendar_widget_->setDateTextFormat(date, QTextCharFormat()); // Reset format
    }
    return *this;
}

Calendar& Calendar::setHolidays(const QList<QDate>& holidays) {
    holidays_ = holidays;
    if (calendar_widget_) {
        QTextCharFormat holidayFormat;
        holidayFormat.setForeground(QColor(255, 0, 0)); // Red text
        holidayFormat.setBackground(QColor(255, 200, 200, 100)); // Light red background
        for (const QDate& holiday : holidays) {
            calendar_widget_->setDateTextFormat(holiday, holidayFormat);
        }
    }
    return *this;
}

Calendar& Calendar::onDateHovered(std::function<void(const QDate&)> handler) {
    date_hovered_handler_ = std::move(handler);
    return *this;
}

Calendar& Calendar::onDateDoubleClicked(std::function<void(const QDate&)> handler) {
    date_double_clicked_handler_ = std::move(handler);
    return *this;
}

// **Initialization**
void Calendar::initialize() {
    if (!calendar_widget_) {
        calendar_widget_ = new QCalendarWidget();
        setWidget(calendar_widget_);

        // Connect signals
        if (selection_changed_handler_) {
            connect(calendar_widget_, &QCalendarWidget::selectionChanged, this,
                    [this]() { selection_changed_handler_(); });
        }

        if (clicked_handler_) {
            connect(calendar_widget_, &QCalendarWidget::clicked, this,
                    [this](const QDate& date) { clicked_handler_(date); });
        }

        if (activated_handler_) {
            connect(calendar_widget_, &QCalendarWidget::activated, this,
                    [this](const QDate& date) { activated_handler_(date); });
        }

        if (current_page_changed_handler_) {
            connect(calendar_widget_, &QCalendarWidget::currentPageChanged, this,
                    [this](int year, int month) { current_page_changed_handler_(year, month); });
        }
    }
}

// **Getter Methods**
QDate Calendar::getSelectedDate() const {
    return calendar_widget_ ? calendar_widget_->selectedDate() : QDate::currentDate();
}

void Calendar::setSelectedDate(const QDate& date) {
    if (calendar_widget_) {
        calendar_widget_->setSelectedDate(date);
    }
}

QDate Calendar::getMinimumDate() const {
    return calendar_widget_ ? calendar_widget_->minimumDate() : QDate();
}

QDate Calendar::getMaximumDate() const {
    return calendar_widget_ ? calendar_widget_->maximumDate() : QDate();
}

bool Calendar::isDateEditEnabled() const {
    return calendar_widget_ ? calendar_widget_->isDateEditEnabled() : true;
}

QList<QDate> Calendar::getSelectedDates() const {
    return selected_dates_;
}

QLocale Calendar::getLocale() const {
    return calendar_widget_ ? calendar_widget_->locale() : QLocale();
}

QTextCharFormat Calendar::getWeekdayTextFormat(Qt::DayOfWeek dayOfWeek) const {
    return calendar_widget_ ? calendar_widget_->weekdayTextFormat(dayOfWeek) : QTextCharFormat();
}

QTextCharFormat Calendar::getDateTextFormat(const QDate& date) const {
    return calendar_widget_ ? calendar_widget_->dateTextFormat(date) : QTextCharFormat();
}

bool Calendar::isWeekNumbersShown() const {
    return calendar_widget_ ?
        (calendar_widget_->verticalHeaderFormat() == QCalendarWidget::ISOWeekNumbers) : false;
}

// **Utility Methods**
void Calendar::clearSelection() {
    selected_dates_.clear();
    if (calendar_widget_) {
        // Reset all date formats to default
        for (const QDate& date : special_dates_) {
            QTextCharFormat specialFormat;
            specialFormat.setBackground(QColor(255, 165, 0, 100)); // Restore special dates
            calendar_widget_->setDateTextFormat(date, specialFormat);
        }
        for (const QDate& holiday : holidays_) {
            QTextCharFormat holidayFormat;
            holidayFormat.setForeground(QColor(255, 0, 0));
            holidayFormat.setBackground(QColor(255, 200, 200, 100));
            calendar_widget_->setDateTextFormat(holiday, holidayFormat);
        }
    }
}

void Calendar::selectDateRange(const QDate& start, const QDate& end) {
    if (!multi_selection_enabled_ || !start.isValid() || !end.isValid()) return;

    selected_dates_.clear();
    QDate current = start;
    while (current <= end) {
        selected_dates_.append(current);
        current = current.addDays(1);
    }

    if (calendar_widget_) {
        QTextCharFormat selectedFormat;
        selectedFormat.setBackground(QColor(0, 120, 215, 100)); // Light blue
        for (const QDate& date : selected_dates_) {
            calendar_widget_->setDateTextFormat(date, selectedFormat);
        }
    }
}

}  // namespace DeclarativeUI::Components
