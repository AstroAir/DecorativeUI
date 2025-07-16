#pragma once
#include <QLayout>
#include <QWidget>
#include <vector>

#include "../Core/UIElement.hpp"

namespace DeclarativeUI::Components {

class Container : public Core::UIElement {
    Q_OBJECT
public:
    explicit Container(QObject *parent = nullptr);

    // Fluent interface
    Container &addWidget(QWidget *widget);
    Container &addElement(Core::UIElement *element);
    Container &setLayout(QLayout *layout);

    void initialize() override;

private:
    std::vector<QWidget *> widgets_;
    std::vector<Core::UIElement *> elements_;
    QLayout *layout_ = nullptr;
};

// Implementation
inline Container::Container(QObject *parent) : UIElement(parent) {}

inline Container &Container::addWidget(QWidget *widget) {
    widgets_.push_back(widget);
    return *this;
}

inline Container &Container::addElement(Core::UIElement *element) {
    elements_.push_back(element);
    return *this;
}

inline Container &Container::setLayout(QLayout *layout) {
    layout_ = layout;
    return *this;
}

inline void Container::initialize() {
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
