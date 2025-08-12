// Components/DateTimeEdit.cpp
#include "DateTimeEdit.hpp"

namespace DeclarativeUI::Components {

// **Constructors**
DateTimeEdit::DateTimeEdit(QObject* parent)
    : UIElement(parent), mode_(EditMode::DateTime) {
    createWidget();
    setupWidget();
}

DateTimeEdit::DateTimeEdit(EditMode mode, QObject* parent)
    : UIElement(parent), mode_(mode) {
    createWidget();
    setupWidget();
}

// **Fluent Interface for Date/Time Configuration**
DateTimeEdit& DateTimeEdit::setEditMode(EditMode mode) {
    mode_ = mode;
    createWidget();
    setupWidget();
    return *this;
}

DateTimeEdit& DateTimeEdit::setDateTime(const QDateTime& dt) {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
        dateTimeEdit->setDateTime(dt);
    }
    return *this;
}

DateTimeEdit& DateTimeEdit::setDate(const QDate& date) {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
        dateTimeEdit->setDate(date);
    }
    return *this;
}

DateTimeEdit& DateTimeEdit::setTime(const QTime& time) {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
        dateTimeEdit->setTime(time);
    }
    return *this;
}

DateTimeEdit& DateTimeEdit::setMinimumDateTime(const QDateTime& dt) {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
        dateTimeEdit->setMinimumDateTime(dt);
    }
    return *this;
}

DateTimeEdit& DateTimeEdit::setMaximumDateTime(const QDateTime& dt) {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
        dateTimeEdit->setMaximumDateTime(dt);
    }
    return *this;
}

DateTimeEdit& DateTimeEdit::setDisplayFormat(const QString& format) {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
        dateTimeEdit->setDisplayFormat(format);
    }
    return *this;
}

DateTimeEdit& DateTimeEdit::setCalendarPopup(bool enable) {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
        dateTimeEdit->setCalendarPopup(enable);
    }
    return *this;
}

DateTimeEdit& DateTimeEdit::setCurrentSection(QDateTimeEdit::Section section) {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
        dateTimeEdit->setCurrentSection(section);
    }
    return *this;
}

DateTimeEdit& DateTimeEdit::setReadOnly(bool readOnly) {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
        dateTimeEdit->setReadOnly(readOnly);
    }
    return *this;
}

// **Event Handlers**
DateTimeEdit& DateTimeEdit::onDateTimeChanged(std::function<void(const QDateTime&)> handler) {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
        QObject::connect(dateTimeEdit, &QDateTimeEdit::dateTimeChanged, handler);
    }
    return *this;
}

DateTimeEdit& DateTimeEdit::onDateChanged(std::function<void(const QDate&)> handler) {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
        QObject::connect(dateTimeEdit, &QDateTimeEdit::dateChanged, handler);
    }
    return *this;
}

DateTimeEdit& DateTimeEdit::onTimeChanged(std::function<void(const QTime&)> handler) {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
        QObject::connect(dateTimeEdit, &QDateTimeEdit::timeChanged, handler);
    }
    return *this;
}

DateTimeEdit& DateTimeEdit::onEditingFinished(std::function<void()> handler) {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
        QObject::connect(dateTimeEdit, &QDateTimeEdit::editingFinished, handler);
    }
    return *this;
}

// **Getters**
QDateTime DateTimeEdit::getDateTime() const {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
        return dateTimeEdit->dateTime();
    }
    return QDateTime();
}

QDate DateTimeEdit::getDate() const {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
        return dateTimeEdit->date();
    }
    return QDate();
}

QTime DateTimeEdit::getTime() const {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
        return dateTimeEdit->time();
    }
    return QTime();
}

bool DateTimeEdit::isReadOnly() const {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
        return dateTimeEdit->isReadOnly();
    }
    return false;
}

// **Advanced Features**
DateTimeEdit& DateTimeEdit::setTimeZone(const QTimeZone& timeZone) {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
        dateTimeEdit->setTimeZone(timeZone);
    }
    return *this;
}

DateTimeEdit& DateTimeEdit::setCurrentSectionIndex(int index) {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
        dateTimeEdit->setCurrentSectionIndex(index);
    }
    return *this;
}

DateTimeEdit& DateTimeEdit::setSelectedSection(QDateTimeEdit::Section section) {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
        dateTimeEdit->setSelectedSection(section);
    }
    return *this;
}

DateTimeEdit& DateTimeEdit::setAccelerated(bool on) {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
        dateTimeEdit->setAccelerated(on);
    }
    return *this;
}

DateTimeEdit& DateTimeEdit::setButtonSymbols(QAbstractSpinBox::ButtonSymbols bs) {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
        dateTimeEdit->setButtonSymbols(bs);
    }
    return *this;
}

DateTimeEdit& DateTimeEdit::setCorrectionMode(QAbstractSpinBox::CorrectionMode cm) {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
        dateTimeEdit->setCorrectionMode(cm);
    }
    return *this;
}

DateTimeEdit& DateTimeEdit::setKeyboardTracking(bool kt) {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
        dateTimeEdit->setKeyboardTracking(kt);
    }
    return *this;
}

DateTimeEdit& DateTimeEdit::setWrapping(bool w) {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
        dateTimeEdit->setWrapping(w);
    }
    return *this;
}

// **Utility Methods**
DateTimeEdit& DateTimeEdit::stepBy(int steps) {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
        dateTimeEdit->stepBy(steps);
    }
    return *this;
}

DateTimeEdit& DateTimeEdit::stepUp() {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
        dateTimeEdit->stepUp();
    }
    return *this;
}

DateTimeEdit& DateTimeEdit::stepDown() {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
        dateTimeEdit->stepDown();
    }
    return *this;
}

DateTimeEdit& DateTimeEdit::selectAll() {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
        dateTimeEdit->selectAll();
    }
    return *this;
}

DateTimeEdit& DateTimeEdit::clear() {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(widget_.get())) {
        dateTimeEdit->clear();
    }
    return *this;
}

// **Initialization**
void DateTimeEdit::initialize() {
    if (!widget_) {
        createWidget();
        setupWidget();
    }
}

// **Private Helper Methods**
void DateTimeEdit::createWidget() {
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
    setWidget(widget_.get());
}

void DateTimeEdit::setupWidget() {
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

}  // namespace DeclarativeUI::Components
