// Components/DateTimeEdit.hpp
#pragma once

#include <QCalendarWidget>
#include <QDate>
#include <QDateEdit>
#include <QDateTime>
#include <QDateTimeEdit>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTime>
#include <QTimeEdit>
#include <QTimer>
#include <QVBoxLayout>
#include <functional>

#include "../Core/UIElement.hpp"

namespace DeclarativeUI::Components {

/**
 * @brief A comprehensive date and time editing component
 *
 * Features:
 * - Date editing with calendar popup
 * - Time editing with precise controls
 * - DateTime editing with combined interface
 * - Custom date/time formats
 * - Range validation
 * - Timezone support
 * - Keyboard shortcuts
 * - Accessibility support
 */
class DateTimeEdit : public Core::UIElement {
    Q_OBJECT

public:
    enum class EditMode { DateOnly, TimeOnly, DateTime };

    explicit DateTimeEdit(QObject* parent = nullptr);
    explicit DateTimeEdit(EditMode mode, QObject* parent = nullptr);
    virtual ~DateTimeEdit() = default;

    // **Fluent Interface for Date/Time Configuration**
    DateTimeEdit& setEditMode(EditMode mode);
    DateTimeEdit& setDateTime(const QDateTime& dt);
    DateTimeEdit& setDate(const QDate& date);
    DateTimeEdit& setTime(const QTime& time);
    DateTimeEdit& setMinimumDateTime(const QDateTime& dt);
    DateTimeEdit& setMaximumDateTime(const QDateTime& dt);
    DateTimeEdit& setDisplayFormat(const QString& format);
    DateTimeEdit& setCalendarPopup(bool enable);
    DateTimeEdit& setCurrentSection(QDateTimeEdit::Section section);
    DateTimeEdit& setReadOnly(bool readOnly);

    // **Event Handlers**
    DateTimeEdit& onDateTimeChanged(
        std::function<void(const QDateTime&)> handler);
    DateTimeEdit& onDateChanged(std::function<void(const QDate&)> handler);
    DateTimeEdit& onTimeChanged(std::function<void(const QTime&)> handler);
    DateTimeEdit& onEditingFinished(std::function<void()> handler);

    // **Getters**
    QDateTime getDateTime() const;
    QDate getDate() const;
    QTime getTime() const;
    bool isReadOnly() const;

    // **Advanced Features**
    DateTimeEdit& setTimeZone(const QTimeZone& timeZone);
    DateTimeEdit& setCurrentSectionIndex(int index);
    DateTimeEdit& setSelectedSection(QDateTimeEdit::Section section);
    DateTimeEdit& setAccelerated(bool on);
    DateTimeEdit& setButtonSymbols(QAbstractSpinBox::ButtonSymbols bs);
    DateTimeEdit& setCorrectionMode(QAbstractSpinBox::CorrectionMode cm);
    DateTimeEdit& setKeyboardTracking(bool kt);
    DateTimeEdit& setWrapping(bool w);

    // **Utility Methods**
    DateTimeEdit& stepBy(int steps);
    DateTimeEdit& stepUp();
    DateTimeEdit& stepDown();
    DateTimeEdit& selectAll();
    DateTimeEdit& clear();

    // **Initialization**
    void initialize() override;

private:
    EditMode mode_ = EditMode::DateTime;

    void createWidget();
    void setupWidget();
};

}  // namespace DeclarativeUI::Components
