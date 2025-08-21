// Components/ScrollArea.cpp
#include "ScrollArea.hpp"
#include <QScrollBar>

namespace DeclarativeUI::Components {

// **Implementation**
ScrollArea::ScrollArea(QObject* parent)
    : UIElement(parent), scroll_area_widget_(nullptr) {}

ScrollArea& ScrollArea::widget(QWidget* widget) {
    if (scroll_area_widget_ && widget) {
        scroll_area_widget_->setWidget(widget);
    }
    return *this;
}

ScrollArea& ScrollArea::widgetResizable(bool resizable) {
    return static_cast<ScrollArea&>(setProperty("widgetResizable", resizable));
}

ScrollArea& ScrollArea::horizontalScrollBarPolicy(Qt::ScrollBarPolicy policy) {
    return static_cast<ScrollArea&>(
        setProperty("horizontalScrollBarPolicy", static_cast<int>(policy)));
}

ScrollArea& ScrollArea::verticalScrollBarPolicy(Qt::ScrollBarPolicy policy) {
    return static_cast<ScrollArea&>(
        setProperty("verticalScrollBarPolicy", static_cast<int>(policy)));
}

ScrollArea& ScrollArea::alignment(Qt::Alignment alignment) {
    return static_cast<ScrollArea&>(
        setProperty("alignment", static_cast<int>(alignment)));
}

ScrollArea& ScrollArea::ensureVisible(int x, int y, int xmargin, int ymargin) {
    if (scroll_area_widget_) {
        scroll_area_widget_->ensureVisible(x, y, xmargin, ymargin);
    }
    return *this;
}

ScrollArea& ScrollArea::ensureWidgetVisible(QWidget* childWidget, int xmargin,
                                            int ymargin) {
    if (scroll_area_widget_ && childWidget) {
        scroll_area_widget_->ensureWidgetVisible(childWidget, xmargin, ymargin);
    }
    return *this;
}

ScrollArea& ScrollArea::style(const QString& stylesheet) {
    return static_cast<ScrollArea&>(setProperty("styleSheet", stylesheet));
}

void ScrollArea::initialize() {
    if (!scroll_area_widget_) {
        scroll_area_widget_ = new QScrollArea();
        // Important: call the base UIElement::setWidget to register lifecycle
        // and properties
        Core::UIElement::setWidget(scroll_area_widget_);
    }
}

QWidget* ScrollArea::getWidget() const {
    // Return the QScrollArea itself so tests can qobject_cast to QScrollArea
    return scroll_area_widget_;
}

void ScrollArea::setContentWidget(QWidget* widget) {
    if (scroll_area_widget_) {
        scroll_area_widget_->setWidget(widget);
    }
}

bool ScrollArea::isWidgetResizable() const {
    return scroll_area_widget_ ? scroll_area_widget_->widgetResizable() : false;
}

Qt::ScrollBarPolicy ScrollArea::getHorizontalScrollBarPolicy() const {
    return scroll_area_widget_
               ? scroll_area_widget_->horizontalScrollBarPolicy()
               : Qt::ScrollBarAsNeeded;
}

Qt::ScrollBarPolicy ScrollArea::getVerticalScrollBarPolicy() const {
    return scroll_area_widget_ ? scroll_area_widget_->verticalScrollBarPolicy()
                               : Qt::ScrollBarAsNeeded;
}

QScrollBar* ScrollArea::horizontalScrollBar() const {
    return scroll_area_widget_ ? scroll_area_widget_->horizontalScrollBar()
                               : nullptr;
}

QScrollBar* ScrollArea::verticalScrollBar() const {
    return scroll_area_widget_ ? scroll_area_widget_->verticalScrollBar()
                               : nullptr;
}

}  // namespace DeclarativeUI::Components
