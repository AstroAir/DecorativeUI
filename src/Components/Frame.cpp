// Components/Frame.cpp
#include "Frame.hpp"
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

namespace DeclarativeUI::Components {

// **Implementation**
Frame::Frame(QObject* parent) : UIElement(parent), frame_widget_(nullptr) {}

Frame& Frame::frameStyle(QFrame::Shape shape, QFrame::Shadow shadow) {
    if (frame_widget_) {
        frame_widget_->setFrameStyle(static_cast<int>(shape) |
                                     static_cast<int>(shadow));
    }
    return *this;
}

Frame& Frame::frameShape(QFrame::Shape shape) {
    return static_cast<Frame&>(
        setProperty("frameShape", static_cast<int>(shape)));
}

Frame& Frame::frameShadow(QFrame::Shadow shadow) {
    return static_cast<Frame&>(
        setProperty("frameShadow", static_cast<int>(shadow)));
}

Frame& Frame::lineWidth(int width) {
    return static_cast<Frame&>(setProperty("lineWidth", width));
}

Frame& Frame::midLineWidth(int width) {
    return static_cast<Frame&>(setProperty("midLineWidth", width));
}

Frame& Frame::margin(int margin) {
    return static_cast<Frame&>(setProperty("margin", margin));
}

Frame& Frame::contentsMargins(int left, int top, int right, int bottom) {
    if (frame_widget_) {
        frame_widget_->setContentsMargins(left, top, right, bottom);
    }
    return *this;
}

Frame& Frame::contentsMargins(const QMargins& margins) {
    if (frame_widget_) {
        frame_widget_->setContentsMargins(margins);
    }
    return *this;
}

Frame& Frame::layout(QLayout* layout) {
    if (frame_widget_ && layout) {
        frame_widget_->setLayout(layout);
    }
    return *this;
}

Frame& Frame::addWidget(QWidget* widget) {
    if (frame_widget_ && widget) {
        if (!frame_widget_->layout()) {
            // Create a default layout if none exists
            auto* defaultLayout = new QVBoxLayout(frame_widget_);
            frame_widget_->setLayout(defaultLayout);
        }
        frame_widget_->layout()->addWidget(widget);
    }
    return *this;
}

Frame& Frame::style(const QString& stylesheet) {
    return static_cast<Frame&>(setProperty("styleSheet", stylesheet));
}

void Frame::initialize() {
    if (!frame_widget_) {
        frame_widget_ = new QFrame();
        setWidget(frame_widget_);
    }
}

QFrame::Shape Frame::getFrameShape() const {
    return frame_widget_ ? frame_widget_->frameShape() : QFrame::NoFrame;
}

QFrame::Shadow Frame::getFrameShadow() const {
    return frame_widget_ ? frame_widget_->frameShadow() : QFrame::Plain;
}

int Frame::getLineWidth() const {
    return frame_widget_ ? frame_widget_->lineWidth() : 1;
}

int Frame::getMidLineWidth() const {
    return frame_widget_ ? frame_widget_->midLineWidth() : 0;
}

QRect Frame::getFrameRect() const {
    return frame_widget_ ? frame_widget_->frameRect() : QRect();
}

QMargins Frame::getContentsMargins() const {
    return frame_widget_ ? frame_widget_->contentsMargins() : QMargins();
}

void Frame::setLayout(QLayout* layout) {
    if (frame_widget_ && layout) {
        frame_widget_->setLayout(layout);
    }
}

Frame& Frame::hBoxLayout() {
    auto* layout = new QHBoxLayout(frame_widget_);
    frame_widget_->setLayout(layout);
    return *this;
}

Frame& Frame::vBoxLayout() {
    auto* layout = new QVBoxLayout(frame_widget_);
    frame_widget_->setLayout(layout);
    return *this;
}

Frame& Frame::gridLayout() {
    auto* layout = new QGridLayout(frame_widget_);
    frame_widget_->setLayout(layout);
    return *this;
}

Frame& Frame::spacing(int space) {
    if (frame_widget_ && frame_widget_->layout()) {
        frame_widget_->layout()->setSpacing(space);
    }
    return *this;
}

}  // namespace DeclarativeUI::Components
