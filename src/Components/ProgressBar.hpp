// Components/ProgressBar.hpp
#pragma once
#include <QProgressBar>

#include "../Core/UIElement.hpp"

namespace DeclarativeUI::Components {

class ProgressBar : public Core::UIElement {
    Q_OBJECT

public:
    explicit ProgressBar(QObject* parent = nullptr);

    // **Fluent interface for progress bar**
    ProgressBar& minimum(int min);
    ProgressBar& maximum(int max);
    ProgressBar& value(int value);
    ProgressBar& orientation(Qt::Orientation orientation);
    ProgressBar& textVisible(bool visible);
    ProgressBar& format(const QString& format);
    ProgressBar& invertedAppearance(bool inverted);
    ProgressBar& style(const QString& stylesheet);

    void initialize() override;
    int getValue() const;
    void setValue(int value);
    int getMinimum() const;
    int getMaximum() const;
    void reset();

private:
    QProgressBar* progress_bar_widget_;
};

// **Implementation**
inline ProgressBar::ProgressBar(QObject* parent)
    : UIElement(parent), progress_bar_widget_(nullptr) {}

inline ProgressBar& ProgressBar::minimum(int min) {
    return static_cast<ProgressBar&>(setProperty("minimum", min));
}

inline ProgressBar& ProgressBar::maximum(int max) {
    return static_cast<ProgressBar&>(setProperty("maximum", max));
}

inline ProgressBar& ProgressBar::value(int value) {
    return static_cast<ProgressBar&>(setProperty("value", value));
}

inline ProgressBar& ProgressBar::orientation(Qt::Orientation orientation) {
    return static_cast<ProgressBar&>(
        setProperty("orientation", static_cast<int>(orientation)));
}

inline ProgressBar& ProgressBar::textVisible(bool visible) {
    return static_cast<ProgressBar&>(setProperty("textVisible", visible));
}

inline ProgressBar& ProgressBar::format(const QString& format) {
    return static_cast<ProgressBar&>(setProperty("format", format));
}

inline ProgressBar& ProgressBar::invertedAppearance(bool inverted) {
    return static_cast<ProgressBar&>(
        setProperty("invertedAppearance", inverted));
}

inline ProgressBar& ProgressBar::style(const QString& stylesheet) {
    return static_cast<ProgressBar&>(setProperty("styleSheet", stylesheet));
}

inline void ProgressBar::initialize() {
    if (!progress_bar_widget_) {
        progress_bar_widget_ = new QProgressBar();
        setWidget(progress_bar_widget_);
    }
}

inline int ProgressBar::getValue() const {
    return progress_bar_widget_ ? progress_bar_widget_->value() : 0;
}

inline void ProgressBar::setValue(int value) {
    if (progress_bar_widget_) {
        progress_bar_widget_->setValue(value);
    }
}

inline int ProgressBar::getMinimum() const {
    return progress_bar_widget_ ? progress_bar_widget_->minimum() : 0;
}

inline int ProgressBar::getMaximum() const {
    return progress_bar_widget_ ? progress_bar_widget_->maximum() : 100;
}

inline void ProgressBar::reset() {
    if (progress_bar_widget_) {
        progress_bar_widget_->reset();
    }
}

}  // namespace DeclarativeUI::Components
