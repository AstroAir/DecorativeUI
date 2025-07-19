// Components/ProgressBar.cpp
#include "ProgressBar.hpp"

namespace DeclarativeUI::Components {

// **Implementation**
ProgressBar::ProgressBar(QObject* parent)
    : UIElement(parent), progress_bar_widget_(nullptr) {}

ProgressBar& ProgressBar::minimum(int min) {
    return static_cast<ProgressBar&>(setProperty("minimum", min));
}

ProgressBar& ProgressBar::maximum(int max) {
    return static_cast<ProgressBar&>(setProperty("maximum", max));
}

ProgressBar& ProgressBar::value(int value) {
    return static_cast<ProgressBar&>(setProperty("value", value));
}

ProgressBar& ProgressBar::orientation(Qt::Orientation orientation) {
    return static_cast<ProgressBar&>(
        setProperty("orientation", static_cast<int>(orientation)));
}

ProgressBar& ProgressBar::textVisible(bool visible) {
    return static_cast<ProgressBar&>(setProperty("textVisible", visible));
}

ProgressBar& ProgressBar::format(const QString& format) {
    return static_cast<ProgressBar&>(setProperty("format", format));
}

ProgressBar& ProgressBar::invertedAppearance(bool inverted) {
    return static_cast<ProgressBar&>(
        setProperty("invertedAppearance", inverted));
}

ProgressBar& ProgressBar::style(const QString& stylesheet) {
    return static_cast<ProgressBar&>(setProperty("styleSheet", stylesheet));
}

void ProgressBar::initialize() {
    if (!progress_bar_widget_) {
        progress_bar_widget_ = new QProgressBar();
        setWidget(progress_bar_widget_);
    }
}

int ProgressBar::getValue() const {
    return progress_bar_widget_ ? progress_bar_widget_->value() : 0;
}

void ProgressBar::setValue(int value) {
    if (progress_bar_widget_) {
        progress_bar_widget_->setValue(value);
    }
}

int ProgressBar::getMinimum() const {
    return progress_bar_widget_ ? progress_bar_widget_->minimum() : 0;
}

int ProgressBar::getMaximum() const {
    return progress_bar_widget_ ? progress_bar_widget_->maximum() : 100;
}

void ProgressBar::reset() {
    if (progress_bar_widget_) {
        progress_bar_widget_->reset();
    }
}

}  // namespace DeclarativeUI::Components
