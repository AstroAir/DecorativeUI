// Components/Calendar.hpp
#pragma once
#include <QCalendarWidget>
#include <QDate>
#include <QTextCharFormat>
#include <QLocale>
#include <QColor>

#include "../Core/UIElement.hpp"

namespace DeclarativeUI::Components {

class Calendar : public Core::UIElement {
    Q_OBJECT

public:
    explicit Calendar(QObject* parent = nullptr);

    // **Fluent interface for calendar**
    Calendar& selectedDate(const QDate& date);
    Calendar& minimumDate(const QDate& date);
    Calendar& maximumDate(const QDate& date);
    Calendar& firstDayOfWeek(Qt::DayOfWeek dayOfWeek);
    Calendar& gridVisible(bool visible);
    Calendar& navigationBarVisible(bool visible);
    Calendar& dateEditEnabled(bool enabled);
    Calendar& dateEditAcceptDelay(int delay);
    Calendar& verticalHeaderFormat(QCalendarWidget::VerticalHeaderFormat format);
    Calendar& horizontalHeaderFormat(QCalendarWidget::HorizontalHeaderFormat format);
    Calendar& selectionMode(QCalendarWidget::SelectionMode mode);
    Calendar& onSelectionChanged(std::function<void()> handler);
    Calendar& onClicked(std::function<void(const QDate&)> handler);
    Calendar& onActivated(std::function<void(const QDate&)> handler);
    Calendar& onCurrentPageChanged(std::function<void(int, int)> handler);
    Calendar& style(const QString& stylesheet);

    // **Enhanced Calendar Features**
    Calendar& locale(const QLocale& locale);
    Calendar& weekdayTextFormat(Qt::DayOfWeek dayOfWeek, const QTextCharFormat& format);
    Calendar& dateTextFormat(const QDate& date, const QTextCharFormat& format);
    Calendar& headerTextFormat(const QTextCharFormat& format);
    Calendar& setDateRange(const QDate& min, const QDate& max);
    Calendar& highlightToday(bool highlight);
    Calendar& showWeekNumbers(bool show);
    Calendar& enableMultiSelection(bool enable);
    Calendar& selectedDates(const QList<QDate>& dates);
    Calendar& addSpecialDate(const QDate& date, const QString& tooltip = QString());
    Calendar& removeSpecialDate(const QDate& date);
    Calendar& setHolidays(const QList<QDate>& holidays);
    Calendar& onDateHovered(std::function<void(const QDate&)> handler);
    Calendar& onDateDoubleClicked(std::function<void(const QDate&)> handler);

    void initialize() override;
    QDate getSelectedDate() const;
    void setSelectedDate(const QDate& date);
    QList<QDate> getSelectedDates() const;
    QDate getMinimumDate() const;
    QDate getMaximumDate() const;
    bool isDateEditEnabled() const;
    QLocale getLocale() const;
    QTextCharFormat getWeekdayTextFormat(Qt::DayOfWeek dayOfWeek) const;
    QTextCharFormat getDateTextFormat(const QDate& date) const;
    bool isWeekNumbersShown() const;
    void clearSelection();
    void selectDateRange(const QDate& start, const QDate& end);

private:
    QCalendarWidget* calendar_widget_;
    std::function<void()> selection_changed_handler_;
    std::function<void(const QDate&)> clicked_handler_;
    std::function<void(const QDate&)> activated_handler_;
    std::function<void(int, int)> current_page_changed_handler_;
    std::function<void(const QDate&)> date_hovered_handler_;
    std::function<void(const QDate&)> date_double_clicked_handler_;

    // Enhanced features
    QList<QDate> special_dates_;
    QList<QDate> holidays_;
    QList<QDate> selected_dates_;
    bool multi_selection_enabled_;
    bool highlight_today_enabled_;
};







}  // namespace DeclarativeUI::Components
