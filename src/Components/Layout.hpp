#pragma once
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLayout>
#include <QMargins>
#include <QVBoxLayout>
#include <QWidget>
#include <vector>

#include "../Core/UIElement.hpp"

namespace DeclarativeUI::Components {

class Layout : public Core::UIElement {
    Q_OBJECT
public:
    enum class Type { HBox, VBox, Grid };

    explicit Layout(Type type = Type::VBox, QObject *parent = nullptr);

    // Fluent interface
    Layout &addWidget(QWidget *widget, int row = -1, int col = -1,
                      int rowSpan = 1, int colSpan = 1);
    Layout &addElement(Core::UIElement *element, int row = -1, int col = -1,
                       int rowSpan = 1, int colSpan = 1);
    Layout &spacing(int spacing);
    Layout &margins(const QMargins &margins);
    Layout &setAlignment(Qt::Alignment alignment);

    void initialize() override;
    QLayout *getLayout() const;

private:
    Type layout_type_;
    QLayout *layout_ = nullptr;
    std::vector<QWidget *> widgets_;
    std::vector<Core::UIElement *> elements_;
    int spacing_ = -1;
    QMargins margins_;
    Qt::Alignment alignment_ = Qt::Alignment();
};

// Implementation
inline Layout::Layout(Type type, QObject *parent)
    : UIElement(parent), layout_type_(type) {}

inline Layout &Layout::addWidget(QWidget *widget, int row, int col, int rowSpan,
                                 int colSpan) {
    widgets_.push_back(widget);
    // For grid, row/col/rowSpan/colSpan are used in initialize
    return *this;
}

inline Layout &Layout::addElement(Core::UIElement *element, int row, int col,
                                  int rowSpan, int colSpan) {
    elements_.push_back(element);
    // For grid, row/col/rowSpan/colSpan are used in initialize
    return *this;
}

inline Layout &Layout::spacing(int spacing) {
    spacing_ = spacing;
    return *this;
}

inline Layout &Layout::margins(const QMargins &margins) {
    margins_ = margins;
    return *this;
}

inline Layout &Layout::setAlignment(Qt::Alignment alignment) {
    alignment_ = alignment;
    return *this;
}

inline void Layout::initialize() {
    if (layout_)
        return;
    switch (layout_type_) {
        case Type::HBox:
            layout_ = new QHBoxLayout();
            break;
        case Type::VBox:
            layout_ = new QVBoxLayout();
            break;
        case Type::Grid:
            layout_ = new QGridLayout();
            break;
    }
    if (spacing_ >= 0)
        layout_->setSpacing(spacing_);
    layout_->setContentsMargins(margins_);
    // Add widgets
    if (auto *grid = qobject_cast<QGridLayout *>(layout_)) {
        int row = 0;
        for (auto *w : widgets_) {
            grid->addWidget(w, row++, 0);
        }
        for (auto *e : elements_) {
            e->initialize();
            if (QWidget *ew = e->getWidget())
                grid->addWidget(ew, row++, 0);
        }
    } else if (auto *box = qobject_cast<QBoxLayout *>(layout_)) {
        for (auto *w : widgets_)
            box->addWidget(w, 0, alignment_);
        for (auto *e : elements_) {
            e->initialize();
            if (QWidget *ew = e->getWidget())
                box->addWidget(ew, 0, alignment_);
        }
    }
    // Attach layout to this UIElement's widget
    if (!getWidget()) {
        QWidget *container = new QWidget();
        container->setLayout(layout_);
        setWidget(container);
    } else {
        getWidget()->setLayout(layout_);
    }
}

inline QLayout *Layout::getLayout() const { return layout_; }

}  // namespace DeclarativeUI::Components
