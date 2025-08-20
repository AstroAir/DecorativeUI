// Components/DateTimeEdit.cpp
#include "DateTimeEdit.hpp"

namespace DeclarativeUI::Components {

// **Constructors**
DateTimeEdit::DateTimeEdit(QObject* parent)
    : UIElement(parent), mode_(EditMode::DateTime) {
    // Widget creation is deferred to initialize() method
}

DateTimeEdit::DateTimeEdit(EditMode mode, QObject* parent)
    : UIElement(parent), mode_(mode) {
    // Widget creation is deferred to initialize() method
}

// **Fluent Interface for Date/Time Configuration**
DateTimeEdit& DateTimeEdit::setEditMode(EditMode mode) {
    mode_ = mode;
    createWidget();
    setupWidget();
    return *this;
}

DateTimeEdit& DateTimeEdit::setDateTime(const QDateTime& dt) {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(getWidget())) {
        dateTimeEdit->setDateTime(dt);
    }
    return *this;
}

DateTimeEdit& DateTimeEdit::setDate(const QDate& date) {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(getWidget())) {
        dateTimeEdit->setDate(date);
    }
    return *this;
}

DateTimeEdit& DateTimeEdit::setTime(const QTime& time) {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(getWidget())) {
        dateTimeEdit->setTime(time);
    }
    return *this;
}

DateTimeEdit& DateTimeEdit::setMinimumDateTime(const QDateTime& dt) {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(getWidget())) {
        dateTimeEdit->setMinimumDateTime(dt);
    }
    return *this;
}

DateTimeEdit& DateTimeEdit::setMaximumDateTime(const QDateTime& dt) {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(getWidget())) {
        dateTimeEdit->setMaximumDateTime(dt);
    }
    return *this;
}

DateTimeEdit& DateTimeEdit::setDisplayFormat(const QString& format) {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(getWidget())) {
        dateTimeEdit->setDisplayFormat(format);
    }
    return *this;
}

DateTimeEdit& DateTimeEdit::setCalendarPopup(bool enable) {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(getWidget())) {
        dateTimeEdit->setCalendarPopup(enable);
    }
    return *this;
}

DateTimeEdit& DateTimeEdit::setCurrentSection(QDateTimeEdit::Section section) {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(getWidget())) {
        dateTimeEdit->setCurrentSection(section);
    }
    return *this;
}

DateTimeEdit& DateTimeEdit::setReadOnly(bool readOnly) {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(getWidget())) {
        dateTimeEdit->setReadOnly(readOnly);
    }
    return *this;
}

// **Event Handlers**
DateTimeEdit& DateTimeEdit::onDateTimeChanged(
    std::function<void(const QDateTime&)> handler) {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(getWidget())) {
        QObject::connect(dateTimeEdit, &QDateTimeEdit::dateTimeChanged,
                         handler);
    }
    return *this;
}

DateTimeEdit& DateTimeEdit::onDateChanged(
    std::function<void(const QDate&)> handler) {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(getWidget())) {
        QObject::connect(dateTimeEdit, &QDateTimeEdit::dateChanged, handler);
    }
    return *this;
}

DateTimeEdit& DateTimeEdit::onTimeChanged(
    std::function<void(const QTime&)> handler) {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(getWidget())) {
        QObject::connect(dateTimeEdit, &QDateTimeEdit::timeChanged, handler);
    }
    return *this;
}

DateTimeEdit& DateTimeEdit::onEditingFinished(std::function<void()> handler) {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(getWidget())) {
        QObject::connect(dateTimeEdit, &QDateTimeEdit::editingFinished,
                         handler);
    }
    return *this;
}

// **Getters**
QDateTime DateTimeEdit::getDateTime() const {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(getWidget())) {
        return dateTimeEdit->dateTime();
    }
    return QDateTime();
}

QDate DateTimeEdit::getDate() const {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(getWidget())) {
        return dateTimeEdit->date();
    }
    return QDate();
}

QTime DateTimeEdit::getTime() const {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(getWidget())) {
        return dateTimeEdit->time();
    }
    return QTime();
}

bool DateTimeEdit::isReadOnly() const {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(getWidget())) {
        return dateTimeEdit->isReadOnly();
    }
    return false;
}

// **Advanced Features**
DateTimeEdit& DateTimeEdit::setTimeZone(const QTimeZone& timeZone) {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(getWidget())) {
        dateTimeEdit->setTimeZone(timeZone);
    }
    return *this;
}

DateTimeEdit& DateTimeEdit::setCurrentSectionIndex(int index) {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(getWidget())) {
        dateTimeEdit->setCurrentSectionIndex(index);
    }
    return *this;
}

DateTimeEdit& DateTimeEdit::setSelectedSection(QDateTimeEdit::Section section) {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(getWidget())) {
        dateTimeEdit->setSelectedSection(section);
    }
    return *this;
}

DateTimeEdit& DateTimeEdit::setAccelerated(bool on) {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(getWidget())) {
        dateTimeEdit->setAccelerated(on);
    }
    return *this;
}

DateTimeEdit& DateTimeEdit::setButtonSymbols(
    QAbstractSpinBox::ButtonSymbols bs) {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(getWidget())) {
        dateTimeEdit->setButtonSymbols(bs);
    }
    return *this;
}

DateTimeEdit& DateTimeEdit::setCorrectionMode(
    QAbstractSpinBox::CorrectionMode cm) {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(getWidget())) {
        dateTimeEdit->setCorrectionMode(cm);
    }
    return *this;
}

DateTimeEdit& DateTimeEdit::setKeyboardTracking(bool kt) {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(getWidget())) {
        dateTimeEdit->setKeyboardTracking(kt);
    }
    return *this;
}

DateTimeEdit& DateTimeEdit::setWrapping(bool w) {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(getWidget())) {
        dateTimeEdit->setWrapping(w);
    }
    return *this;
}

// **Utility Methods**
DateTimeEdit& DateTimeEdit::stepBy(int steps) {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(getWidget())) {
        dateTimeEdit->stepBy(steps);
    }
    return *this;
}

DateTimeEdit& DateTimeEdit::stepUp() {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(getWidget())) {
        dateTimeEdit->stepUp();
    }
    return *this;
}

DateTimeEdit& DateTimeEdit::stepDown() {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(getWidget())) {
        dateTimeEdit->stepDown();
    }
    return *this;
}

DateTimeEdit& DateTimeEdit::selectAll() {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(getWidget())) {
        dateTimeEdit->selectAll();
    }
    return *this;
}

DateTimeEdit& DateTimeEdit::clear() {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(getWidget())) {
        dateTimeEdit->clear();
    }
    return *this;
}

// **Initialization**
void DateTimeEdit::initialize() {
    if (!getWidget()) {
        createWidget();
        setupWidget();
    }
}

// **Private Helper Methods**
void DateTimeEdit::createWidget() {
    QWidget* widget = nullptr;
    switch (mode_) {
        case EditMode::DateOnly:
            widget = new QDateEdit();
            break;
        case EditMode::TimeOnly:
            widget = new QTimeEdit();
            break;
        case EditMode::DateTime:
        default:
            widget = new QDateTimeEdit();
            break;
    }
    setWidget(widget);
}

void DateTimeEdit::setupWidget() {
    if (auto dateTimeEdit = qobject_cast<QDateTimeEdit*>(getWidget())) {
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
