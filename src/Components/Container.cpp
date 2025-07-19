// Components/Container.cpp
#include "Container.hpp"

namespace DeclarativeUI::Components {

// Implementation
Container::Container(QObject *parent) : UIElement(parent) {}

Container &Container::addWidget(QWidget *widget) {
    widgets_.push_back(widget);
    return *this;
}

Container &Container::addElement(Core::UIElement *element) {
    elements_.push_back(element);
    return *this;
}

Container &Container::setLayout(QLayout *layout) {
    layout_ = layout;
    return *this;
}

void Container::initialize() {
    if (!getWidget()) {
        QWidget *container = new QWidget();
        setWidget(container);
    }
    if (layout_) {
        getWidget()->setLayout(layout_);
    }
    for (auto *w : widgets_) {
        w->setParent(getWidget());
        if (layout_)
            layout_->addWidget(w);
    }
    for (auto *e : elements_) {
        e->initialize();
        if (QWidget *ew = e->getWidget()) {
            ew->setParent(getWidget());
            if (layout_)
                layout_->addWidget(ew);
        }
    }
}

}  // namespace DeclarativeUI::Components
