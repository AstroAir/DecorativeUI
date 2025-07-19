// Components/SpinBox.cpp
#include "SpinBox.hpp"

namespace DeclarativeUI::Components {

// **SpinBox Implementation**
SpinBox::SpinBox(QObject* parent)
    : UIElement(parent), spinbox_widget_(nullptr) {}

SpinBox& SpinBox::minimum(int min) {
    return static_cast<SpinBox&>(setProperty("minimum", min));
}

SpinBox& SpinBox::maximum(int max) {
    return static_cast<SpinBox&>(setProperty("maximum", max));
}

SpinBox& SpinBox::value(int value) {
    return static_cast<SpinBox&>(setProperty("value", value));
}

SpinBox& SpinBox::singleStep(int step) {
    return static_cast<SpinBox&>(setProperty("singleStep", step));
}

SpinBox& SpinBox::prefix(const QString& prefix) {
    return static_cast<SpinBox&>(setProperty("prefix", prefix));
}

SpinBox& SpinBox::suffix(const QString& suffix) {
    return static_cast<SpinBox&>(setProperty("suffix", suffix));
}

SpinBox& SpinBox::wrapping(bool wrap) {
    return static_cast<SpinBox&>(setProperty("wrapping", wrap));
}

SpinBox& SpinBox::readOnly(bool readonly) {
    return static_cast<SpinBox&>(setProperty("readOnly", readonly));
}

SpinBox& SpinBox::specialValueText(const QString& text) {
    return static_cast<SpinBox&>(setProperty("specialValueText", text));
}

SpinBox& SpinBox::displayIntegerBase(int base) {
    return static_cast<SpinBox&>(setProperty("displayIntegerBase", base));
}

SpinBox& SpinBox::onValueChanged(std::function<void(int)> handler) {
    value_changed_handler_ = std::move(handler);
    return *this;
}

SpinBox& SpinBox::onTextChanged(
    std::function<void(const QString&)> handler) {
    text_changed_handler_ = std::move(handler);
    return *this;
}

SpinBox& SpinBox::style(const QString& stylesheet) {
    return static_cast<SpinBox&>(setProperty("styleSheet", stylesheet));
}

void SpinBox::initialize() {
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

int SpinBox::getValue() const {
    return spinbox_widget_ ? spinbox_widget_->value() : 0;
}

int SpinBox::getMinimum() const {
    return spinbox_widget_ ? spinbox_widget_->minimum() : 0;
}

int SpinBox::getMaximum() const {
    return spinbox_widget_ ? spinbox_widget_->maximum() : 100;
}

int SpinBox::getSingleStep() const {
    return spinbox_widget_ ? spinbox_widget_->singleStep() : 1;
}

QString SpinBox::getPrefix() const {
    return spinbox_widget_ ? spinbox_widget_->prefix() : QString();
}

QString SpinBox::getSuffix() const {
    return spinbox_widget_ ? spinbox_widget_->suffix() : QString();
}

bool SpinBox::isWrapping() const {
    return spinbox_widget_ ? spinbox_widget_->wrapping() : false;
}

bool SpinBox::isReadOnly() const {
    return spinbox_widget_ ? spinbox_widget_->isReadOnly() : false;
}

void SpinBox::setValue(int value) {
    if (spinbox_widget_) {
        spinbox_widget_->setValue(value);
    }
}

void SpinBox::setMinimum(int min) {
    if (spinbox_widget_) {
        spinbox_widget_->setMinimum(min);
    }
}

void SpinBox::setMaximum(int max) {
    if (spinbox_widget_) {
        spinbox_widget_->setMaximum(max);
    }
}

void SpinBox::setRange(int min, int max) {
    if (spinbox_widget_) {
        spinbox_widget_->setRange(min, max);
    }
}

void SpinBox::setSingleStep(int step) {
    if (spinbox_widget_) {
        spinbox_widget_->setSingleStep(step);
    }
}

void SpinBox::stepUp() {
    if (spinbox_widget_) {
        spinbox_widget_->stepUp();
    }
}

void SpinBox::stepDown() {
    if (spinbox_widget_) {
        spinbox_widget_->stepDown();
    }
}

void SpinBox::selectAll() {
    if (spinbox_widget_) {
        spinbox_widget_->selectAll();
    }
}

void SpinBox::clear() {
    if (spinbox_widget_) {
        spinbox_widget_->clear();
    }
}



}  // namespace DeclarativeUI::Components
