/**
 * @file Layout.cpp
 * @brief Implementation of Layout component with improved maintainability.
 *
 * This file contains the refactored Layout class implementation with:
 * - Reduced cyclomatic complexity in initialize() function
 * - Extracted helper methods for better code organization
 * - Improved documentation and error handling
 * - Better separation of concerns for different layout types
 *
 * @author DeclarativeUI Team
 * @version 2.0
 * @date 2024
 */

#include "Layout.hpp"

namespace DeclarativeUI::Components {

// Implementation
Layout::Layout(Type type, QObject *parent)
    : UIElement(parent), layout_type_(type) {}

Layout &Layout::addWidget(QWidget *widget, int row, int col, int rowSpan,
                          int colSpan) {
    widgets_.push_back(widget);
    // For grid, row/col/rowSpan/colSpan are used in initialize
    return *this;
}

Layout &Layout::addElement(Core::UIElement *element, int row, int col,
                           int rowSpan, int colSpan) {
    elements_.push_back(element);
    // For grid, row/col/rowSpan/colSpan are used in initialize
    return *this;
}

Layout &Layout::spacing(int spacing) {
    spacing_ = spacing;
    return *this;
}

Layout &Layout::margins(const QMargins &margins) {
    margins_ = margins;
    return *this;
}

Layout &Layout::setAlignment(Qt::Alignment alignment) {
    alignment_ = alignment;
    return *this;
}

/**
 * @brief Initializes the layout with reduced complexity.
 *
 * This method has been refactored to use helper methods for better
 * maintainability:
 * - createLayoutByType(): Creates the appropriate layout type
 * - configureLayoutProperties(): Sets spacing and margins
 * - addWidgetsToGridLayout()/addWidgetsToBoxLayout(): Adds widgets based on
 * layout type
 * - attachLayoutToWidget(): Attaches the layout to the widget
 */
void Layout::initialize() {
    // Early return if already initialized
    if (layout_)
        return;

    // Create layout based on type
    createLayoutByType();

    // Configure layout properties
    configureLayoutProperties();

    // Add widgets based on layout type
    if (auto *grid = qobject_cast<QGridLayout *>(layout_)) {
        addWidgetsToGridLayout(grid);
    } else if (auto *box = qobject_cast<QBoxLayout *>(layout_)) {
        addWidgetsToBoxLayout(box);
    }

    // Attach layout to widget
    attachLayoutToWidget();
}

QLayout *Layout::getLayout() const { return layout_; }

/**
 * @brief Creates the appropriate layout type based on layout_type_.
 */
void Layout::createLayoutByType() {
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
}

/**
 * @brief Configures layout properties such as spacing and margins.
 */
void Layout::configureLayoutProperties() {
    if (!layout_)
        return;

    if (spacing_ >= 0)
        layout_->setSpacing(spacing_);
    layout_->setContentsMargins(margins_);
}

/**
 * @brief Adds widgets and elements to a grid layout.
 * @param grid Pointer to the grid layout
 */
void Layout::addWidgetsToGridLayout(QGridLayout *grid) {
    if (!grid)
        return;

    int row = 0;

    // Add regular widgets
    for (auto *w : widgets_) {
        grid->addWidget(w, row++, 0);
    }

    // Add UI elements
    for (auto *e : elements_) {
        e->initialize();
        if (QWidget *ew = e->getWidget())
            grid->addWidget(ew, row++, 0);
    }
}

/**
 * @brief Adds widgets and elements to a box layout.
 * @param box Pointer to the box layout
 */
void Layout::addWidgetsToBoxLayout(QBoxLayout *box) {
    if (!box)
        return;

    // Add regular widgets
    for (auto *w : widgets_)
        box->addWidget(w, 0, alignment_);

    // Add UI elements
    for (auto *e : elements_) {
        e->initialize();
        if (QWidget *ew = e->getWidget())
            box->addWidget(ew, 0, alignment_);
    }
}

/**
 * @brief Attaches the layout to the widget, creating a container if necessary.
 */
void Layout::attachLayoutToWidget() {
    if (!layout_)
        return;

    if (!getWidget()) {
        QWidget *container = new QWidget();
        container->setLayout(layout_);
        setWidget(container);
    } else {
        getWidget()->setLayout(layout_);
    }
}

}  // namespace DeclarativeUI::Components
