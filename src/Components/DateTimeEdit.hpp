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
public:
    enum class EditMode { DateOnly, TimeOnly, DateTime };

    DateTimeEdit() {
        widget_ = std::make_unique<QDateTimeEdit>();
        setupWidget();
    }

    explicit DateTimeEdit(EditMode mode) : mode_(mode) {
        createWidget();
        setupWidget();
    }

    virtual ~DateTimeEdit() = default;

    // **Fluent Interface for Date/Time Configuration**
    DateTimeEdit& setEditMode(EditMode mode) {
        mode_ = mode;
        createWidget();
        setupWidget();
        return *this;
    }

    DateTimeEdit& setDateTime(const QDateTime& dt) {
        if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
            dateTimeEdit->setDateTime(dt);
        }
        return *this;
    }

    DateTimeEdit& setDate(const QDate& date) {
        if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
            dateTimeEdit->setDate(date);
        }
        return *this;
    }

    DateTimeEdit& setTime(const QTime& time) {
        if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
            dateTimeEdit->setTime(time);
        }
        return *this;
    }

    DateTimeEdit& setMinimumDateTime(const QDateTime& dt) {
        if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
            dateTimeEdit->setMinimumDateTime(dt);
        }
        return *this;
    }

    DateTimeEdit& setMaximumDateTime(const QDateTime& dt) {
        if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
            dateTimeEdit->setMaximumDateTime(dt);
        }
        return *this;
    }

    DateTimeEdit& setDisplayFormat(const QString& format) {
        if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
            dateTimeEdit->setDisplayFormat(format);
        }
        return *this;
    }

    DateTimeEdit& setCalendarPopup(bool enable) {
        if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
            dateTimeEdit->setCalendarPopup(enable);
        }
        return *this;
    }

    DateTimeEdit& setCurrentSection(QDateTimeEdit::Section section) {
        if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
            dateTimeEdit->setCurrentSection(section);
        }
        return *this;
    }

    DateTimeEdit& setReadOnly(bool readOnly) {
        if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
            dateTimeEdit->setReadOnly(readOnly);
        }
        return *this;
    }

    // **Event Handlers**
    DateTimeEdit& onDateTimeChanged(
        std::function<void(const QDateTime&)> handler) {
        if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
            QObject::connect(dateTimeEdit, &QDateTimeEdit::dateTimeChanged,
                             handler);
        }
        return *this;
    }

    DateTimeEdit& onDateChanged(std::function<void(const QDate&)> handler) {
        if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
            QObject::connect(dateTimeEdit, &QDateTimeEdit::dateChanged,
                             handler);
        }
        return *this;
    }

    DateTimeEdit& onTimeChanged(std::function<void(const QTime&)> handler) {
        if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
            QObject::connect(dateTimeEdit, &QDateTimeEdit::timeChanged,
                             handler);
        }
        return *this;
    }

    DateTimeEdit& onEditingFinished(std::function<void()> handler) {
        if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
            QObject::connect(dateTimeEdit, &QDateTimeEdit::editingFinished,
                             handler);
        }
        return *this;
    }

    // **Getters**
    QDateTime getDateTime() const {
        if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
            return dateTimeEdit->dateTime();
        }
        return QDateTime();
    }

    QDate getDate() const {
        if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
            return dateTimeEdit->date();
        }
        return QDate();
    }

    QTime getTime() const {
        if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
            return dateTimeEdit->time();
        }
        return QTime();
    }

    bool isReadOnly() const {
        if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
            return dateTimeEdit->isReadOnly();
        }
        return false;
    }

    // **Advanced Features**
    DateTimeEdit& setTimeZone(const QTimeZone& timeZone) {
        if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
            dateTimeEdit->setTimeZone(timeZone);
        }
        return *this;
    }

    DateTimeEdit& setCurrentSectionIndex(int index) {
        if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
            dateTimeEdit->setCurrentSectionIndex(index);
        }
        return *this;
    }

    DateTimeEdit& setSelectedSection(QDateTimeEdit::Section section) {
        if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
            dateTimeEdit->setSelectedSection(section);
        }
        return *this;
    }

    DateTimeEdit& setAccelerated(bool on) {
        if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
            dateTimeEdit->setAccelerated(on);
        }
        return *this;
    }

    DateTimeEdit& setButtonSymbols(QAbstractSpinBox::ButtonSymbols bs) {
        if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
            dateTimeEdit->setButtonSymbols(bs);
        }
        return *this;
    }

    DateTimeEdit& setCorrectionMode(QAbstractSpinBox::CorrectionMode cm) {
        if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
            dateTimeEdit->setCorrectionMode(cm);
        }
        return *this;
    }

    DateTimeEdit& setKeyboardTracking(bool kt) {
        if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
            dateTimeEdit->setKeyboardTracking(kt);
        }
        return *this;
    }

    DateTimeEdit& setWrapping(bool w) {
        if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
            dateTimeEdit->setWrapping(w);
        }
        return *this;
    }

    // **Utility Methods**
    DateTimeEdit& stepBy(int steps) {
        if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
            dateTimeEdit->stepBy(steps);
        }
        return *this;
    }

    DateTimeEdit& stepUp() {
        if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
            dateTimeEdit->stepUp();
        }
        return *this;
    }

    DateTimeEdit& stepDown() {
        if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
            dateTimeEdit->stepDown();
        }
        return *this;
    }

    DateTimeEdit& selectAll() {
        if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
            dateTimeEdit->selectAll();
        }
        return *this;
    }

    DateTimeEdit& clear() {
        if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
            dateTimeEdit->clear();
        }
        return *this;
    }

private:
    EditMode mode_ = EditMode::DateTime;

    void createWidget() {
        switch (mode_) {
            case EditMode::DateOnly:
                widget_ = std::make_unique<QDateEdit>();
                break;
            case EditMode::TimeOnly:
                widget_ = std::make_unique<QTimeEdit>();
                break;
            case EditMode::DateTime:
            default:
                widget_ = std::make_unique<QDateTimeEdit>();
                break;
        }
    }

    void setupWidget() {
        if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
            // **Default configuration**
            dateTimeEdit->setCalendarPopup(true);
            dateTimeEdit->setDateTime(QDateTime::currentDateTime());

            // **Default format based on mode**
            switch (mode_) {
                case EditMode::DateOnly:
                    dateTimeEdit->setDisplayFormat("yyyy-MM-dd");
                    break;
                case EditMode::TimeOnly:
                    dateTimeEdit->setDisplayFormat("HH:mm:ss");
                    break;
                case EditMode::DateTime:
                default:
                    dateTimeEdit->setDisplayFormat("yyyy-MM-dd HH:mm:ss");
                    break;
            }

            // **Enable keyboard tracking for responsive updates**
            dateTimeEdit->setKeyboardTracking(true);

            // **Enable acceleration for faster input**
            dateTimeEdit->setAccelerated(true);

            // **Set reasonable date range**
            dateTimeEdit->setMinimumDate(QDate(1900, 1, 1));
            dateTimeEdit->setMaximumDate(QDate(2100, 12, 31));
        }
    }
};

}  // namespace DeclarativeUI::Components
