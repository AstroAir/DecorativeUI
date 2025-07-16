// Components/SpinBox.hpp
#pragma once
#include <QDoubleSpinBox>
#include <QSpinBox>


#include "../Core/UIElement.hpp"

namespace DeclarativeUI::Components {

class SpinBox : public Core::UIElement {
    Q_OBJECT

public:
    explicit SpinBox(QObject* parent = nullptr);

    // **Fluent interface for spin box**
    SpinBox& minimum(int min);
    SpinBox& maximum(int max);
    SpinBox& value(int value);
    SpinBox& singleStep(int step);
    SpinBox& prefix(const QString& prefix);
    SpinBox& suffix(const QString& suffix);
    SpinBox& wrapping(bool wrap);
    SpinBox& readOnly(bool readonly);
    SpinBox& specialValueText(const QString& text);
    SpinBox& displayIntegerBase(int base);
    SpinBox& onValueChanged(std::function<void(int)> handler);
    SpinBox& onTextChanged(std::function<void(const QString&)> handler);
    SpinBox& style(const QString& stylesheet);

    void initialize() override;

    // **Getters**
    int getValue() const;
    int getMinimum() const;
    int getMaximum() const;
    int getSingleStep() const;
    QString getPrefix() const;
    QString getSuffix() const;
    bool isWrapping() const;
    bool isReadOnly() const;

    // **Setters**
    void setValue(int value);
    void setMinimum(int min);
    void setMaximum(int max);
    void setRange(int min, int max);
    void setSingleStep(int step);
    void stepUp();
    void stepDown();
    void selectAll();
    void clear();

private:
    QSpinBox* spinbox_widget_;
    std::function<void(int)> value_changed_handler_;
    std::function<void(const QString&)> text_changed_handler_;
};

class DoubleSpinBox : public Core::UIElement {
    Q_OBJECT

public:
    explicit DoubleSpinBox(QObject* parent = nullptr);

    // **Fluent interface for double spin box**
    DoubleSpinBox& minimum(double min);
    DoubleSpinBox& maximum(double max);
    DoubleSpinBox& value(double value);
    DoubleSpinBox& singleStep(double step);
    DoubleSpinBox& decimals(int decimals);
    DoubleSpinBox& prefix(const QString& prefix);
    DoubleSpinBox& suffix(const QString& suffix);
    DoubleSpinBox& wrapping(bool wrap);
    DoubleSpinBox& readOnly(bool readonly);
    DoubleSpinBox& specialValueText(const QString& text);
    DoubleSpinBox& onValueChanged(std::function<void(double)> handler);
    DoubleSpinBox& onTextChanged(std::function<void(const QString&)> handler);
    DoubleSpinBox& style(const QString& stylesheet);

    void initialize() override;

    // **Getters**
    double getValue() const;
    double getMinimum() const;
    double getMaximum() const;
    double getSingleStep() const;
    int getDecimals() const;
    QString getPrefix() const;
    QString getSuffix() const;
    bool isWrapping() const;
    bool isReadOnly() const;

    // **Setters**
    void setValue(double value);
    void setMinimum(double min);
    void setMaximum(double max);
    void setRange(double min, double max);
    void setSingleStep(double step);
    void setDecimals(int decimals);
    void stepUp();
    void stepDown();
    void selectAll();
    void clear();

private:
    QDoubleSpinBox* double_spinbox_widget_;
    std::function<void(double)> value_changed_handler_;
    std::function<void(const QString&)> text_changed_handler_;
};

// **SpinBox Implementation**
inline SpinBox::SpinBox(QObject* parent)
    : UIElement(parent), spinbox_widget_(nullptr) {}

inline SpinBox& SpinBox::minimum(int min) {
    return static_cast<SpinBox&>(setProperty("minimum", min));
}

inline SpinBox& SpinBox::maximum(int max) {
    return static_cast<SpinBox&>(setProperty("maximum", max));
}

inline SpinBox& SpinBox::value(int value) {
    return static_cast<SpinBox&>(setProperty("value", value));
}

inline SpinBox& SpinBox::singleStep(int step) {
    return static_cast<SpinBox&>(setProperty("singleStep", step));
}

inline SpinBox& SpinBox::prefix(const QString& prefix) {
    return static_cast<SpinBox&>(setProperty("prefix", prefix));
}

inline SpinBox& SpinBox::suffix(const QString& suffix) {
    return static_cast<SpinBox&>(setProperty("suffix", suffix));
}

inline SpinBox& SpinBox::wrapping(bool wrap) {
    return static_cast<SpinBox&>(setProperty("wrapping", wrap));
}

inline SpinBox& SpinBox::readOnly(bool readonly) {
    return static_cast<SpinBox&>(setProperty("readOnly", readonly));
}

inline SpinBox& SpinBox::specialValueText(const QString& text) {
    return static_cast<SpinBox&>(setProperty("specialValueText", text));
}

inline SpinBox& SpinBox::displayIntegerBase(int base) {
    return static_cast<SpinBox&>(setProperty("displayIntegerBase", base));
}

inline SpinBox& SpinBox::onValueChanged(std::function<void(int)> handler) {
    value_changed_handler_ = std::move(handler);
    return *this;
}

inline SpinBox& SpinBox::onTextChanged(
    std::function<void(const QString&)> handler) {
    text_changed_handler_ = std::move(handler);
    return *this;
}

inline SpinBox& SpinBox::style(const QString& stylesheet) {
    return static_cast<SpinBox&>(setProperty("styleSheet", stylesheet));
}

inline void SpinBox::initialize() {
    if (!spinbox_widget_) {
        spinbox_widget_ = new QSpinBox();
        setWidget(spinbox_widget_);

        // Connect signals
        if (value_changed_handler_) {
            connect(spinbox_widget_,
                    QOverload<int>::of(&QSpinBox::valueChanged), this,
                    [this](int value) { value_changed_handler_(value); });
        }

        if (text_changed_handler_) {
            connect(
                spinbox_widget_, &QSpinBox::textChanged, this,
                [this](const QString& text) { text_changed_handler_(text); });
        }
    }
}

inline int SpinBox::getValue() const {
    return spinbox_widget_ ? spinbox_widget_->value() : 0;
}

inline int SpinBox::getMinimum() const {
    return spinbox_widget_ ? spinbox_widget_->minimum() : 0;
}

inline int SpinBox::getMaximum() const {
    return spinbox_widget_ ? spinbox_widget_->maximum() : 100;
}

inline int SpinBox::getSingleStep() const {
    return spinbox_widget_ ? spinbox_widget_->singleStep() : 1;
}

inline QString SpinBox::getPrefix() const {
    return spinbox_widget_ ? spinbox_widget_->prefix() : QString();
}

inline QString SpinBox::getSuffix() const {
    return spinbox_widget_ ? spinbox_widget_->suffix() : QString();
}

inline bool SpinBox::isWrapping() const {
    return spinbox_widget_ ? spinbox_widget_->wrapping() : false;
}

inline bool SpinBox::isReadOnly() const {
    return spinbox_widget_ ? spinbox_widget_->isReadOnly() : false;
}

inline void SpinBox::setValue(int value) {
    if (spinbox_widget_) {
        spinbox_widget_->setValue(value);
    }
}

inline void SpinBox::setMinimum(int min) {
    if (spinbox_widget_) {
        spinbox_widget_->setMinimum(min);
    }
}

inline void SpinBox::setMaximum(int max) {
    if (spinbox_widget_) {
        spinbox_widget_->setMaximum(max);
    }
}

inline void SpinBox::setRange(int min, int max) {
    if (spinbox_widget_) {
        spinbox_widget_->setRange(min, max);
    }
}

inline void SpinBox::setSingleStep(int step) {
    if (spinbox_widget_) {
        spinbox_widget_->setSingleStep(step);
    }
}

inline void SpinBox::stepUp() {
    if (spinbox_widget_) {
        spinbox_widget_->stepUp();
    }
}

inline void SpinBox::stepDown() {
    if (spinbox_widget_) {
        spinbox_widget_->stepDown();
    }
}

inline void SpinBox::selectAll() {
    if (spinbox_widget_) {
        spinbox_widget_->selectAll();
    }
}

inline void SpinBox::clear() {
    if (spinbox_widget_) {
        spinbox_widget_->clear();
    }
}

// **DoubleSpinBox Implementation**
inline DoubleSpinBox::DoubleSpinBox(QObject* parent)
    : UIElement(parent), double_spinbox_widget_(nullptr) {}

inline DoubleSpinBox& DoubleSpinBox::minimum(double min) {
    return static_cast<DoubleSpinBox&>(setProperty("minimum", min));
}

inline DoubleSpinBox& DoubleSpinBox::maximum(double max) {
    return static_cast<DoubleSpinBox&>(setProperty("maximum", max));
}

inline DoubleSpinBox& DoubleSpinBox::value(double value) {
    return static_cast<DoubleSpinBox&>(setProperty("value", value));
}

inline DoubleSpinBox& DoubleSpinBox::singleStep(double step) {
    return static_cast<DoubleSpinBox&>(setProperty("singleStep", step));
}

inline DoubleSpinBox& DoubleSpinBox::decimals(int decimals) {
    return static_cast<DoubleSpinBox&>(setProperty("decimals", decimals));
}

inline DoubleSpinBox& DoubleSpinBox::prefix(const QString& prefix) {
    return static_cast<DoubleSpinBox&>(setProperty("prefix", prefix));
}

inline DoubleSpinBox& DoubleSpinBox::suffix(const QString& suffix) {
    return static_cast<DoubleSpinBox&>(setProperty("suffix", suffix));
}

inline DoubleSpinBox& DoubleSpinBox::wrapping(bool wrap) {
    return static_cast<DoubleSpinBox&>(setProperty("wrapping", wrap));
}

inline DoubleSpinBox& DoubleSpinBox::readOnly(bool readonly) {
    return static_cast<DoubleSpinBox&>(setProperty("readOnly", readonly));
}

inline DoubleSpinBox& DoubleSpinBox::specialValueText(const QString& text) {
    return static_cast<DoubleSpinBox&>(setProperty("specialValueText", text));
}

inline DoubleSpinBox& DoubleSpinBox::onValueChanged(
    std::function<void(double)> handler) {
    value_changed_handler_ = std::move(handler);
    return *this;
}

inline DoubleSpinBox& DoubleSpinBox::onTextChanged(
    std::function<void(const QString&)> handler) {
    text_changed_handler_ = std::move(handler);
    return *this;
}

inline DoubleSpinBox& DoubleSpinBox::style(const QString& stylesheet) {
    return static_cast<DoubleSpinBox&>(setProperty("styleSheet", stylesheet));
}

inline void DoubleSpinBox::initialize() {
    if (!double_spinbox_widget_) {
        double_spinbox_widget_ = new QDoubleSpinBox();
        setWidget(double_spinbox_widget_);

        // Connect signals
        if (value_changed_handler_) {
            connect(double_spinbox_widget_,
                    QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
                    [this](double value) { value_changed_handler_(value); });
        }

        if (text_changed_handler_) {
            connect(
                double_spinbox_widget_, &QDoubleSpinBox::textChanged, this,
                [this](const QString& text) { text_changed_handler_(text); });
        }
    }
}

inline double DoubleSpinBox::getValue() const {
    return double_spinbox_widget_ ? double_spinbox_widget_->value() : 0.0;
}

inline double DoubleSpinBox::getMinimum() const {
    return double_spinbox_widget_ ? double_spinbox_widget_->minimum() : 0.0;
}

inline double DoubleSpinBox::getMaximum() const {
    return double_spinbox_widget_ ? double_spinbox_widget_->maximum() : 100.0;
}

inline double DoubleSpinBox::getSingleStep() const {
    return double_spinbox_widget_ ? double_spinbox_widget_->singleStep() : 1.0;
}

inline int DoubleSpinBox::getDecimals() const {
    return double_spinbox_widget_ ? double_spinbox_widget_->decimals() : 2;
}

inline QString DoubleSpinBox::getPrefix() const {
    return double_spinbox_widget_ ? double_spinbox_widget_->prefix()
                                  : QString();
}

inline QString DoubleSpinBox::getSuffix() const {
    return double_spinbox_widget_ ? double_spinbox_widget_->suffix()
                                  : QString();
}

inline bool DoubleSpinBox::isWrapping() const {
    return double_spinbox_widget_ ? double_spinbox_widget_->wrapping() : false;
}

inline bool DoubleSpinBox::isReadOnly() const {
    return double_spinbox_widget_ ? double_spinbox_widget_->isReadOnly()
                                  : false;
}

inline void DoubleSpinBox::setValue(double value) {
    if (double_spinbox_widget_) {
        double_spinbox_widget_->setValue(value);
    }
}

inline void DoubleSpinBox::setMinimum(double min) {
    if (double_spinbox_widget_) {
        double_spinbox_widget_->setMinimum(min);
    }
}

inline void DoubleSpinBox::setMaximum(double max) {
    if (double_spinbox_widget_) {
        double_spinbox_widget_->setMaximum(max);
    }
}

inline void DoubleSpinBox::setRange(double min, double max) {
    if (double_spinbox_widget_) {
        double_spinbox_widget_->setRange(min, max);
    }
}

inline void DoubleSpinBox::setSingleStep(double step) {
    if (double_spinbox_widget_) {
        double_spinbox_widget_->setSingleStep(step);
    }
}

inline void DoubleSpinBox::setDecimals(int decimals) {
    if (double_spinbox_widget_) {
        double_spinbox_widget_->setDecimals(decimals);
    }
}

inline void DoubleSpinBox::stepUp() {
    if (double_spinbox_widget_) {
        double_spinbox_widget_->stepUp();
    }
}

inline void DoubleSpinBox::stepDown() {
    if (double_spinbox_widget_) {
        double_spinbox_widget_->stepDown();
    }
}

inline void DoubleSpinBox::selectAll() {
    if (double_spinbox_widget_) {
        double_spinbox_widget_->selectAll();
    }
}

inline void DoubleSpinBox::clear() {
    if (double_spinbox_widget_) {
        double_spinbox_widget_->clear();
    }
}

}  // namespace DeclarativeUI::Components
