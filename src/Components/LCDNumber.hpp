// Components/LCDNumber.hpp
#pragma once
#include <QLCDNumber>

#include "../Core/UIElement.hpp"

namespace DeclarativeUI::Components {

class LCDNumber : public Core::UIElement {
    Q_OBJECT

public:
    explicit LCDNumber(QObject* parent = nullptr);

    // **Fluent interface for LCD number**
    LCDNumber& digitCount(int count);
    LCDNumber& value(double value);
    LCDNumber& intValue(int value);
    LCDNumber& mode(QLCDNumber::Mode mode);
    LCDNumber& segmentStyle(QLCDNumber::SegmentStyle style);
    LCDNumber& smallDecimalPoint(bool small);
    LCDNumber& onOverflow(std::function<void()> handler);
    LCDNumber& style(const QString& stylesheet);

    void initialize() override;
    double getValue() const;
    void setValue(double value);
    void setValue(int value);
    int getDigitCount() const;
    QLCDNumber::Mode getMode() const;

private:
    QLCDNumber* lcd_widget_;
    std::function<void()> overflow_handler_;
};

}  // namespace DeclarativeUI::Components
