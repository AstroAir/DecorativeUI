// Components/LCDNumber.cpp
#include "LCDNumber.hpp"

namespace DeclarativeUI::Components {

// **Implementation**
LCDNumber::LCDNumber(QObject* parent)
    : UIElement(parent), lcd_widget_(nullptr) {}

LCDNumber& LCDNumber::digitCount(int count) {
    return static_cast<LCDNumber&>(setProperty("digitCount", count));
}

LCDNumber& LCDNumber::value(double value) {
    return static_cast<LCDNumber&>(setProperty("value", value));
}

LCDNumber& LCDNumber::intValue(int value) {
    return static_cast<LCDNumber&>(setProperty("intValue", value));
}

LCDNumber& LCDNumber::mode(QLCDNumber::Mode mode) {
    return static_cast<LCDNumber&>(setProperty("mode", static_cast<int>(mode)));
}

LCDNumber& LCDNumber::segmentStyle(QLCDNumber::SegmentStyle style) {
    return static_cast<LCDNumber&>(
        setProperty("segmentStyle", static_cast<int>(style)));
}

LCDNumber& LCDNumber::smallDecimalPoint(bool small) {
    return static_cast<LCDNumber&>(setProperty("smallDecimalPoint", small));
}

LCDNumber& LCDNumber::onOverflow(std::function<void()> handler) {
    overflow_handler_ = std::move(handler);
    return *this;
}

LCDNumber& LCDNumber::style(const QString& stylesheet) {
    return static_cast<LCDNumber&>(setProperty("styleSheet", stylesheet));
}

void LCDNumber::initialize() {
    if (!lcd_widget_) {
        lcd_widget_ = new QLCDNumber();
        setWidget(lcd_widget_);

        // Connect signals
        if (overflow_handler_) {
            connect(lcd_widget_, &QLCDNumber::overflow, this,
                    [this]() { overflow_handler_(); });
        }
    }
}

double LCDNumber::getValue() const {
    return lcd_widget_ ? lcd_widget_->value() : 0.0;
}

void LCDNumber::setValue(double value) {
    if (lcd_widget_) {
        lcd_widget_->display(value);
    }
}

void LCDNumber::setValue(int value) {
    if (lcd_widget_) {
        lcd_widget_->display(value);
    }
}

int LCDNumber::getDigitCount() const {
    return lcd_widget_ ? lcd_widget_->digitCount() : 5;
}

QLCDNumber::Mode LCDNumber::getMode() const {
    return lcd_widget_ ? lcd_widget_->mode() : QLCDNumber::Dec;
}

}  // namespace DeclarativeUI::Components
