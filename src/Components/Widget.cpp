// Components/Widget.cpp
#include "Widget.hpp"

namespace DeclarativeUI::Components {

// **Implementation**
Widget::Widget(QObject* parent)
    : UIElement(parent), widget_(nullptr) {}

Widget& Widget::size(const QSize& size) {
    return static_cast<Widget&>(setProperty("size", size));
}

Widget& Widget::minimumSize(const QSize& size) {
    return static_cast<Widget&>(setProperty("minimumSize", size));
}

Widget& Widget::maximumSize(const QSize& size) {
    return static_cast<Widget&>(setProperty("maximumSize", size));
}

Widget& Widget::fixedSize(const QSize& size) {
    if (widget_) {
        widget_->setFixedSize(size);
    }
    return *this;
}

Widget& Widget::sizePolicy(QSizePolicy::Policy horizontal, QSizePolicy::Policy vertical) {
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

Widget& Widget::geometry(const QRect& rect) {
    return static_cast<Widget&>(setProperty("geometry", rect));
}

Widget& Widget::position(const QPoint& pos) {
    if (widget_) {
        widget_->move(pos);
    }
    return *this;
}

Widget& Widget::visible(bool visible) {
    return static_cast<Widget&>(setProperty("visible", visible));
}

Widget& Widget::enabled(bool enabled) {
    return static_cast<Widget&>(setProperty("enabled", enabled));
}

Widget& Widget::toolTip(const QString& tooltip) {
    return static_cast<Widget&>(setProperty("toolTip", tooltip));
}

Widget& Widget::statusTip(const QString& statusTip) {
    return static_cast<Widget&>(setProperty("statusTip", statusTip));
}

Widget& Widget::whatsThis(const QString& whatsThis) {
    return static_cast<Widget&>(setProperty("whatsThis", whatsThis));
}

Widget& Widget::windowTitle(const QString& title) {
    return static_cast<Widget&>(setProperty("windowTitle", title));
}

Widget& Widget::windowIcon(const QIcon& icon) {
    return static_cast<Widget&>(setProperty("windowIcon", icon));
}

Widget& Widget::windowFlags(Qt::WindowFlags flags) {
    return static_cast<Widget&>(setProperty("windowFlags", static_cast<int>(flags)));
}

Widget& Widget::windowState(Qt::WindowStates state) {
    return static_cast<Widget&>(setProperty("windowState", static_cast<int>(state)));
}

Widget& Widget::focusPolicy(Qt::FocusPolicy policy) {
    return static_cast<Widget&>(setProperty("focusPolicy", static_cast<int>(policy)));
}

Widget& Widget::contextMenuPolicy(Qt::ContextMenuPolicy policy) {
    return static_cast<Widget&>(setProperty("contextMenuPolicy", static_cast<int>(policy)));
}

Widget& Widget::cursor(const QCursor& cursor) {
    return static_cast<Widget&>(setProperty("cursor", cursor));
}

Widget& Widget::font(const QFont& font) {
    return static_cast<Widget&>(setProperty("font", font));
}

Widget& Widget::palette(const QPalette& palette) {
    return static_cast<Widget&>(setProperty("palette", palette));
}

Widget& Widget::autoFillBackground(bool enabled) {
    return static_cast<Widget&>(setProperty("autoFillBackground", enabled));
}

Widget& Widget::updatesEnabled(bool enabled) {
    return static_cast<Widget&>(setProperty("updatesEnabled", enabled));
}

Widget& Widget::layout(QLayout* layout) {
    if (widget_ && layout) {
        widget_->setLayout(layout);
    }
    return *this;
}

Widget& Widget::vBoxLayout() {
    if (widget_) {
        auto* layout = new QVBoxLayout(widget_);
        widget_->setLayout(layout);
    }
    return *this;
}

Widget& Widget::hBoxLayout() {
    if (widget_) {
        auto* layout = new QHBoxLayout(widget_);
        widget_->setLayout(layout);
    }
    return *this;
}

Widget& Widget::gridLayout(int rows, int columns) {
    if (widget_) {
        auto* layout = new QGridLayout(widget_);
        widget_->setLayout(layout);
    }
    return *this;
}

Widget& Widget::formLayout() {
    if (widget_) {
        auto* layout = new QFormLayout(widget_);
        widget_->setLayout(layout);
    }
    return *this;
}

Widget& Widget::addWidget(QWidget* widget) {
    if (widget_ && widget) {
        if (!widget_->layout()) {
            vBoxLayout(); // Create default layout
        }
        widget_->layout()->addWidget(widget);
    }
    return *this;
}

Widget& Widget::addWidget(QWidget* widget, int row, int column, Qt::Alignment alignment) {
    if (widget_ && widget) {
        auto* gridLayout = qobject_cast<QGridLayout*>(widget_->layout());
        if (gridLayout) {
            gridLayout->addWidget(widget, row, column, alignment);
        }
    }
    return *this;
}

Widget& Widget::addWidget(QWidget* widget, int row, int column, int rowSpan, int columnSpan, Qt::Alignment alignment) {
    if (widget_ && widget) {
        auto* gridLayout = qobject_cast<QGridLayout*>(widget_->layout());
        if (gridLayout) {
            gridLayout->addWidget(widget, row, column, rowSpan, columnSpan, alignment);
        }
    }
    return *this;
}

Widget& Widget::addLayout(QLayout* layout) {
    if (widget_ && layout) {
        if (!widget_->layout()) {
            vBoxLayout(); // Create default layout
        }
        widget_->layout()->addItem(layout);
    }
    return *this;
}

Widget& Widget::spacing(int spacing) {
    if (widget_ && widget_->layout()) {
        widget_->layout()->setSpacing(spacing);
    }
    return *this;
}

Widget& Widget::margins(int left, int top, int right, int bottom) {
    if (widget_ && widget_->layout()) {
        widget_->layout()->setContentsMargins(left, top, right, bottom);
    }
    return *this;
}

Widget& Widget::margins(const QMargins& margins) {
    if (widget_ && widget_->layout()) {
        widget_->layout()->setContentsMargins(margins);
    }
    return *this;
}

Widget& Widget::style(const QString& stylesheet) {
    return static_cast<Widget&>(setProperty("styleSheet", stylesheet));
}

void Widget::initialize() {
    if (!widget_) {
        widget_ = new QWidget();
        setWidget(widget_);
    }
}

QSize Widget::getSize() const {
    return widget_ ? widget_->size() : QSize();
}

QSize Widget::getMinimumSize() const {
    return widget_ ? widget_->minimumSize() : QSize();
}

QSize Widget::getMaximumSize() const {
    return widget_ ? widget_->maximumSize() : QSize();
}

QRect Widget::getGeometry() const {
    return widget_ ? widget_->geometry() : QRect();
}

QPoint Widget::getPosition() const {
    return widget_ ? widget_->pos() : QPoint();
}

bool Widget::isVisible() const {
    return widget_ ? widget_->isVisible() : false;
}

bool Widget::isEnabled() const {
    return widget_ ? widget_->isEnabled() : true;
}

QString Widget::getToolTip() const {
    return widget_ ? widget_->toolTip() : QString();
}

QLayout* Widget::getLayout() const {
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
