// Components/Widget.cpp
#include "Widget.hpp"
#include <algorithm>
#include <ranges>

namespace DeclarativeUI::Components {

using namespace Core::Concepts;

// **Implementation**
Widget::Widget(QObject* parent) : UIElement(parent), widget_(nullptr) {}

// Template methods are implemented in the header file

Widget& Widget::sizePolicy(QSizePolicy::Policy horizontal,
                           QSizePolicy::Policy vertical) {
    if (widget_) {
        widget_->setSizePolicy(horizontal, vertical);
    }
    return *this;
}

Widget& Widget::sizePolicy(const QSizePolicy& policy) {
    if (widget_) {
        widget_->setSizePolicy(policy);
    }
    return *this;
}

// Template methods for geometry and position are implemented in header

Widget& Widget::visible(bool visible) noexcept {
    return static_cast<Widget&>(setProperty("visible", visible));
}

Widget& Widget::enabled(bool enabled) noexcept {
    return static_cast<Widget&>(setProperty("enabled", enabled));
}

// Template methods for string-like parameters are implemented in header

Widget& Widget::windowIcon(const QIcon& icon) {
    return static_cast<Widget&>(setProperty("windowIcon", icon));
}

Widget& Widget::windowFlags(Qt::WindowFlags flags) noexcept {
    return static_cast<Widget&>(
        setProperty("windowFlags", static_cast<int>(flags)));
}

Widget& Widget::windowState(Qt::WindowStates state) noexcept {
    return static_cast<Widget&>(
        setProperty("windowState", static_cast<int>(state)));
}

Widget& Widget::focusPolicy(Qt::FocusPolicy policy) noexcept {
    return static_cast<Widget&>(
        setProperty("focusPolicy", static_cast<int>(policy)));
}

Widget& Widget::contextMenuPolicy(Qt::ContextMenuPolicy policy) noexcept {
    return static_cast<Widget&>(
        setProperty("contextMenuPolicy", static_cast<int>(policy)));
}

Widget& Widget::cursor(const QCursor& cursor) {
    return static_cast<Widget&>(setProperty("cursor", cursor));
}

// Template method for font is implemented in header

Widget& Widget::palette(const QPalette& palette) {
    return static_cast<Widget&>(setProperty("palette", palette));
}

Widget& Widget::autoFillBackground(bool enabled) noexcept {
    return static_cast<Widget&>(setProperty("autoFillBackground", enabled));
}

Widget& Widget::updatesEnabled(bool enabled) noexcept {
    return static_cast<Widget&>(setProperty("updatesEnabled", enabled));
}

// Template methods for layout and widget management are implemented in header

Widget& Widget::vBoxLayout() {
    if (widget_) {
        // Delete existing layout if present
        if (widget_->layout()) {
            delete widget_->layout();
        }
        auto* layout = new QVBoxLayout(widget_);
        widget_->setLayout(layout);
    }
    return *this;
}

Widget& Widget::hBoxLayout() {
    if (widget_) {
        // Delete existing layout if present
        if (widget_->layout()) {
            delete widget_->layout();
        }
        auto* layout = new QHBoxLayout(widget_);
        widget_->setLayout(layout);
    }
    return *this;
}

Widget& Widget::gridLayout(int rows, int columns) {
    if (widget_) {
        // Delete existing layout if present
        if (widget_->layout()) {
            delete widget_->layout();
        }
        auto* layout = new QGridLayout(widget_);
        widget_->setLayout(layout);
    }
    return *this;
}

Widget& Widget::formLayout() {
    if (widget_) {
        // Delete existing layout if present
        if (widget_->layout()) {
            delete widget_->layout();
        }
        auto* layout = new QFormLayout(widget_);
        widget_->setLayout(layout);
    }
    return *this;
}

Widget& Widget::spacing(int spacing) noexcept {
    if (widget_ && widget_->layout()) {
        widget_->layout()->setSpacing(spacing);
    }
    return *this;
}

Widget& Widget::margins(const QMargins& margins) {
    if (widget_ && widget_->layout()) {
        widget_->layout()->setContentsMargins(margins);
    }
    return *this;
}

void Widget::initialize() {
    if (!widget_) {
        widget_ = new QWidget();
        setWidget(widget_);
    }
}

QSize Widget::getSize() const noexcept {
    return widget_ ? widget_->size() : QSize();
}

QSize Widget::getMinimumSize() const noexcept {
    return widget_ ? widget_->minimumSize() : QSize();
}

QSize Widget::getMaximumSize() const noexcept {
    return widget_ ? widget_->maximumSize() : QSize();
}

QRect Widget::getGeometry() const noexcept {
    return widget_ ? widget_->geometry() : QRect();
}

QPoint Widget::getPosition() const noexcept {
    return widget_ ? widget_->pos() : QPoint();
}

bool Widget::isVisible() const noexcept {
    return widget_ ? widget_->isVisible() : false;
}

bool Widget::isEnabled() const noexcept {
    return widget_ ? widget_->isEnabled() : true;
}

QString Widget::getToolTip() const {
    return widget_ ? widget_->toolTip() : QString();
}

QLayout* Widget::getLayout() const noexcept {
    return widget_ ? widget_->layout() : nullptr;
}

void Widget::show() {
    if (widget_) {
        widget_->show();
    }
}

void Widget::hide() {
    if (widget_) {
        widget_->hide();
    }
}

void Widget::setFocus() {
    if (widget_) {
        widget_->setFocus();
    }
}

void Widget::clearFocus() {
    if (widget_) {
        widget_->clearFocus();
    }
}

void Widget::update() {
    if (widget_) {
        widget_->update();
    }
}

void Widget::repaint() {
    if (widget_) {
        widget_->repaint();
    }
}

}  // namespace DeclarativeUI::Components
