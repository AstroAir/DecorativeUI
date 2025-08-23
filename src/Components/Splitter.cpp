// Components/Splitter.cpp
#include "Splitter.hpp"

namespace DeclarativeUI::Components {

// **Implementation**
Splitter::Splitter(QObject* parent)
    : UIElement(parent), splitter_widget_(nullptr) {}

Splitter& Splitter::orientation(Qt::Orientation orientation) {
    return static_cast<Splitter&>(
        setProperty("orientation", static_cast<int>(orientation)));
}

Splitter& Splitter::childrenCollapsible(bool collapsible) {
    return static_cast<Splitter&>(
        setProperty("childrenCollapsible", collapsible));
}

Splitter& Splitter::handleWidth(int width) {
    return static_cast<Splitter&>(setProperty("handleWidth", width));
}

Splitter& Splitter::opaqueResize(bool opaque) {
    return static_cast<Splitter&>(setProperty("opaqueResize", opaque));
}

Splitter& Splitter::addWidget(QWidget* widget) {
    if (splitter_widget_ && widget) {
        splitter_widget_->addWidget(widget);
    }
    return *this;
}

Splitter& Splitter::insertWidget(int index, QWidget* widget) {
    if (splitter_widget_ && widget) {
        splitter_widget_->insertWidget(index, widget);
    }
    return *this;
}

Splitter& Splitter::setSizes(const QList<int>& sizes) {
    if (splitter_widget_) {
        splitter_widget_->setSizes(sizes);
    }
    return *this;
}

Splitter& Splitter::setStretchFactor(int index, int stretch) {
    if (splitter_widget_) {
        splitter_widget_->setStretchFactor(index, stretch);
    }
    return *this;
}

Splitter& Splitter::onSplitterMoved(std::function<void(int, int)> handler) {
    splitter_moved_handler_ = std::move(handler);
    return *this;
}

Splitter& Splitter::style(const QString& stylesheet) {
    return static_cast<Splitter&>(setProperty("styleSheet", stylesheet));
}

void Splitter::initialize() {
    if (!splitter_widget_) {
        splitter_widget_ = new QSplitter();
        setWidget(splitter_widget_);

        // Connect signals
        if (splitter_moved_handler_) {
            connect(splitter_widget_, &QSplitter::splitterMoved, this,
                    [this](int pos, int index) {
                        splitter_moved_handler_(pos, index);
                    });
        }
    }
}

QWidget* Splitter::getWidget() const { return splitter_widget_; }

Qt::Orientation Splitter::getOrientation() const {
    return splitter_widget_ ? splitter_widget_->orientation() : Qt::Horizontal;
}

QList<int> Splitter::getSizes() const {
    return splitter_widget_ ? splitter_widget_->sizes() : QList<int>();
}

int Splitter::getCount() const {
    return splitter_widget_ ? splitter_widget_->count() : 0;
}

QWidget* Splitter::getWidget(int index) const {
    return splitter_widget_ ? splitter_widget_->widget(index) : nullptr;
}

void Splitter::removeWidget(QWidget* widget) {
    if (splitter_widget_ && widget) {
        widget->setParent(nullptr);
    }
}

void Splitter::replaceWidget(int index, QWidget* widget) {
    if (splitter_widget_ && widget) {
        splitter_widget_->replaceWidget(index, widget);
    }
}

}  // namespace DeclarativeUI::Components
