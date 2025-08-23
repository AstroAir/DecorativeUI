// Components/DoubleSpinBox.cpp
#include "DoubleSpinBox.hpp"
#include <QDoubleSpinBox>

namespace DeclarativeUI::Components {

DoubleSpinBox::DoubleSpinBox(QObject* parent)
    : UIElement(parent), double_spin_box_widget_(nullptr) {}

void DoubleSpinBox::initialize() {
    if (!double_spin_box_widget_) {
        double_spin_box_widget_ = new QDoubleSpinBox();
        setWidget(double_spin_box_widget_);

        // Connect signals
        connect(double_spin_box_widget_,
                QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
                [this](double value) {
                    if (value_changed_handler_) {
                        value_changed_handler_(value);
                    }
                });
    }
}

DoubleSpinBox& DoubleSpinBox::minimum(double min) {
    return static_cast<DoubleSpinBox&>(setProperty("minimum", min));
}

DoubleSpinBox& DoubleSpinBox::maximum(double max) {
    return static_cast<DoubleSpinBox&>(setProperty("maximum", max));
}

DoubleSpinBox& DoubleSpinBox::value(double val) {
    return static_cast<DoubleSpinBox&>(setProperty("value", val));
}

DoubleSpinBox& DoubleSpinBox::singleStep(double step) {
    return static_cast<DoubleSpinBox&>(setProperty("singleStep", step));
}

DoubleSpinBox& DoubleSpinBox::decimals(int decimals) {
    return static_cast<DoubleSpinBox&>(setProperty("decimals", decimals));
}

DoubleSpinBox& DoubleSpinBox::prefix(const QString& prefix) {
    return static_cast<DoubleSpinBox&>(setProperty("prefix", prefix));
}

DoubleSpinBox& DoubleSpinBox::suffix(const QString& suffix) {
    return static_cast<DoubleSpinBox&>(setProperty("suffix", suffix));
}

DoubleSpinBox& DoubleSpinBox::onValueChanged(
    std::function<void(double)> handler) {
    value_changed_handler_ = handler;
    return *this;
}

double DoubleSpinBox::getValue() const {
    return double_spin_box_widget_ ? double_spin_box_widget_->value() : 0.0;
}

double DoubleSpinBox::getMinimum() const {
    return double_spin_box_widget_ ? double_spin_box_widget_->minimum() : 0.0;
}

double DoubleSpinBox::getMaximum() const {
    return double_spin_box_widget_ ? double_spin_box_widget_->maximum() : 100.0;
}

double DoubleSpinBox::getSingleStep() const {
    return double_spin_box_widget_ ? double_spin_box_widget_->singleStep()
                                   : 1.0;
}

int DoubleSpinBox::getDecimals() const {
    return double_spin_box_widget_ ? double_spin_box_widget_->decimals() : 2;
}

QString DoubleSpinBox::getPrefix() const {
    return double_spin_box_widget_ ? double_spin_box_widget_->prefix()
                                   : QString();
}

QString DoubleSpinBox::getSuffix() const {
    return double_spin_box_widget_ ? double_spin_box_widget_->suffix()
                                   : QString();
}

}  // namespace DeclarativeUI::Components
