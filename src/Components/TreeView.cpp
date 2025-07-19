// Components/TreeView.cpp
#include "TreeView.hpp"

namespace DeclarativeUI::Components {

// **Implementation**
TreeView::TreeView(QObject* parent)
    : UIElement(parent), tree_widget_(nullptr), default_model_(nullptr) {}

TreeView& TreeView::model(QAbstractItemModel* model) {
    return static_cast<TreeView&>(
        setProperty("model", QVariant::fromValue(model)));
}

TreeView& TreeView::selectionBehavior(
    QAbstractItemView::SelectionBehavior behavior) {
    return static_cast<TreeView&>(
        setProperty("selectionBehavior", static_cast<int>(behavior)));
}

TreeView& TreeView::selectionMode(
    QAbstractItemView::SelectionMode mode) {
    return static_cast<TreeView&>(
        setProperty("selectionMode", static_cast<int>(mode)));
}

TreeView& TreeView::alternatingRowColors(bool alternate) {
    return static_cast<TreeView&>(
        setProperty("alternatingRowColors", alternate));
}

TreeView& TreeView::sortingEnabled(bool enabled) {
    return static_cast<TreeView&>(setProperty("sortingEnabled", enabled));
}

TreeView& TreeView::animated(bool animated) {
    return static_cast<TreeView&>(setProperty("animated", animated));
}

TreeView& TreeView::allColumnsShowFocus(bool show) {
    return static_cast<TreeView&>(setProperty("allColumnsShowFocus", show));
}

TreeView& TreeView::autoExpandDelay(int delay) {
    return static_cast<TreeView&>(setProperty("autoExpandDelay", delay));
}

TreeView& TreeView::expandsOnDoubleClick(bool expand) {
    return static_cast<TreeView&>(setProperty("expandsOnDoubleClick", expand));
}

TreeView& TreeView::headerHidden(bool hidden) {
    return static_cast<TreeView&>(setProperty("headerHidden", hidden));
}

TreeView& TreeView::itemsExpandable(bool expandable) {
    return static_cast<TreeView&>(setProperty("itemsExpandable", expandable));
}

TreeView& TreeView::rootIsDecorated(bool decorated) {
    return static_cast<TreeView&>(setProperty("rootIsDecorated", decorated));
}

TreeView& TreeView::uniformRowHeights(bool uniform) {
    return static_cast<TreeView&>(setProperty("uniformRowHeights", uniform));
}

TreeView& TreeView::wordWrap(bool wrap) {
    return static_cast<TreeView&>(setProperty("wordWrap", wrap));
}

TreeView& TreeView::onItemClicked(
    std::function<void(const QModelIndex&)> handler) {
    item_clicked_handler_ = std::move(handler);
    return *this;
}

TreeView& TreeView::onItemDoubleClicked(
    std::function<void(const QModelIndex&)> handler) {
    item_double_clicked_handler_ = std::move(handler);
    return *this;
}

TreeView& TreeView::onItemExpanded(
    std::function<void(const QModelIndex&)> handler) {
    item_expanded_handler_ = std::move(handler);
    return *this;
}

TreeView& TreeView::onItemCollapsed(
    std::function<void(const QModelIndex&)> handler) {
    item_collapsed_handler_ = std::move(handler);
    return *this;
}

TreeView& TreeView::onSelectionChanged(std::function<void()> handler) {
    selection_changed_handler_ = std::move(handler);
    return *this;
}

TreeView& TreeView::style(const QString& stylesheet) {
    return static_cast<TreeView&>(setProperty("styleSheet", stylesheet));
}

void TreeView::initialize() {
    if (!tree_widget_) {
        tree_widget_ = new QTreeView();
        setWidget(tree_widget_);

        // Setup default model if none provided
        setupDefaultModel();

        // Connect signals
        if (item_clicked_handler_) {
            connect(tree_widget_, &QTreeView::clicked, this,
                    [this](const QModelIndex& index) {
                        item_clicked_handler_(index);
                    });
        }

        if (item_double_clicked_handler_) {
            connect(tree_widget_, &QTreeView::doubleClicked, this,
                    [this](const QModelIndex& index) {
                        item_double_clicked_handler_(index);
                    });
        }

        if (item_expanded_handler_) {
            connect(tree_widget_, &QTreeView::expanded, this,
                    [this](const QModelIndex& index) {
                        item_expanded_handler_(index);
                    });
        }

        if (item_collapsed_handler_) {
            connect(tree_widget_, &QTreeView::collapsed, this,
                    [this](const QModelIndex& index) {
                        item_collapsed_handler_(index);
                    });
        }

        if (selection_changed_handler_) {
            connect(tree_widget_->selectionModel(), &QItemSelectionModel::selectionChanged, this,
                    [this]() { selection_changed_handler_(); });
        }

        connectModelSignals();
    }
}

void TreeView::setupDefaultModel() {
    if (!default_model_) {
        default_model_ = new QStandardItemModel(this);
        tree_widget_->setModel(default_model_);
    }
}

void TreeView::connectModelSignals() {
    // Connect model signals for data updates
    if (tree_widget_->model()) {
        // Additional signal connections can be added here
    }
}

void TreeView::expand(const QModelIndex& index) {
    if (tree_widget_) {
        tree_widget_->expand(index);
    }
}

void TreeView::collapse(const QModelIndex& index) {
    if (tree_widget_) {
        tree_widget_->collapse(index);
    }
}

void TreeView::expandAll() {
    if (tree_widget_) {
        tree_widget_->expandAll();
    }
}

void TreeView::collapseAll() {
    if (tree_widget_) {
        tree_widget_->collapseAll();
    }
}

void TreeView::expandToDepth(int depth) {
    if (tree_widget_) {
        tree_widget_->expandToDepth(depth);
    }
}

void TreeView::setExpanded(const QModelIndex& index, bool expanded) {
    if (tree_widget_) {
        tree_widget_->setExpanded(index, expanded);
    }
}

bool TreeView::isExpanded(const QModelIndex& index) const {
    return tree_widget_ ? tree_widget_->isExpanded(index) : false;
}

void TreeView::scrollTo(const QModelIndex& index) {
    if (tree_widget_) {
        tree_widget_->scrollTo(index);
    }
}

void TreeView::resizeColumnToContents(int column) {
    if (tree_widget_) {
        tree_widget_->resizeColumnToContents(column);
    }
}

void TreeView::setColumnWidth(int column, int width) {
    if (tree_widget_) {
        tree_widget_->setColumnWidth(column, width);
    }
}

void TreeView::hideColumn(int column) {
    if (tree_widget_) {
        tree_widget_->hideColumn(column);
    }
}

void TreeView::showColumn(int column) {
    if (tree_widget_) {
        tree_widget_->showColumn(column);
    }
}

QStandardItem* TreeView::addRootItem(const QString& text) {
    if (default_model_) {
        QStandardItem* item = new QStandardItem(text);
        default_model_->appendRow(item);
        return item;
    }
    return nullptr;
}

QStandardItem* TreeView::addChildItem(QStandardItem* parent,
                                     const QString& text) {
    if (parent) {
        QStandardItem* item = new QStandardItem(text);
        parent->appendRow(item);
        return item;
    }
    return nullptr;
}

void TreeView::removeItem(QStandardItem* item) {
    if (item && default_model_) {
        default_model_->removeRow(item->row(), item->parent()
                                                   ? item->parent()->index()
                                                   : QModelIndex());
    }
}

void TreeView::clearData() {
    if (default_model_) {
        default_model_->clear();
    }
}

QAbstractItemModel* TreeView::getModel() const {
    return tree_widget_ ? tree_widget_->model() : nullptr;
}

QModelIndex TreeView::getCurrentIndex() const {
    return tree_widget_ ? tree_widget_->currentIndex() : QModelIndex();
}

QModelIndexList TreeView::getSelectedIndexes() const {
    return tree_widget_ ? tree_widget_->selectionModel()->selectedIndexes()
                        : QModelIndexList();
}

QStandardItem* TreeView::getRootItem(int row) const {
    return default_model_ ? default_model_->item(row) : nullptr;
}

QStandardItem* TreeView::getItemFromIndex(
    const QModelIndex& index) const {
    return default_model_ ? default_model_->itemFromIndex(index) : nullptr;
}

QModelIndex TreeView::getIndexFromItem(QStandardItem* item) const {
    return item ? item->index() : QModelIndex();
}

}  // namespace DeclarativeUI::Components
