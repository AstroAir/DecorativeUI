// Components/TreeView.hpp
#pragma once
#include <QAbstractItemModel>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QModelIndex>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QTreeView>


#include "../Core/UIElement.hpp"

namespace DeclarativeUI::Components {

class TreeView : public Core::UIElement {
    Q_OBJECT

public:
    explicit TreeView(QObject* parent = nullptr);

    // **Fluent interface for tree view**
    TreeView& model(QAbstractItemModel* model);
    TreeView& selectionBehavior(QAbstractItemView::SelectionBehavior behavior);
    TreeView& selectionMode(QAbstractItemView::SelectionMode mode);
    TreeView& alternatingRowColors(bool alternate);
    TreeView& sortingEnabled(bool enabled);
    TreeView& animated(bool animated);
    TreeView& allColumnsShowFocus(bool show);
    TreeView& autoExpandDelay(int delay);
    TreeView& expandsOnDoubleClick(bool expand);
    TreeView& headerHidden(bool hidden);
    TreeView& itemsExpandable(bool expandable);
    TreeView& rootIsDecorated(bool decorated);
    TreeView& uniformRowHeights(bool uniform);
    TreeView& wordWrap(bool wrap);
    TreeView& onItemClicked(std::function<void(const QModelIndex&)> handler);
    TreeView& onItemDoubleClicked(
        std::function<void(const QModelIndex&)> handler);
    TreeView& onItemExpanded(std::function<void(const QModelIndex&)> handler);
    TreeView& onItemCollapsed(std::function<void(const QModelIndex&)> handler);
    TreeView& onSelectionChanged(std::function<void()> handler);
    TreeView& style(const QString& stylesheet);

    void initialize() override;

    // **Tree operations**
    void expand(const QModelIndex& index);
    void collapse(const QModelIndex& index);
    void expandAll();
    void collapseAll();
    void expandToDepth(int depth);
    void setExpanded(const QModelIndex& index, bool expanded);
    bool isExpanded(const QModelIndex& index) const;
    void scrollTo(const QModelIndex& index);
    void resizeColumnToContents(int column);
    void setColumnWidth(int column, int width);
    void hideColumn(int column);
    void showColumn(int column);

    // **Data operations**
    QStandardItem* addRootItem(const QString& text);
    QStandardItem* addChildItem(QStandardItem* parent, const QString& text);
    void removeItem(QStandardItem* item);
    void clearData();

    // **Getters**
    QAbstractItemModel* getModel() const;
    QModelIndex getCurrentIndex() const;
    QModelIndexList getSelectedIndexes() const;
    QStandardItem* getRootItem(int row) const;
    QStandardItem* getItemFromIndex(const QModelIndex& index) const;
    QModelIndex getIndexFromItem(QStandardItem* item) const;

private:
    QTreeView* tree_widget_;
    QStandardItemModel* default_model_;
    std::function<void(const QModelIndex&)> item_clicked_handler_;
    std::function<void(const QModelIndex&)> item_double_clicked_handler_;
    std::function<void(const QModelIndex&)> item_expanded_handler_;
    std::function<void(const QModelIndex&)> item_collapsed_handler_;
    std::function<void()> selection_changed_handler_;

    void setupDefaultModel();
    void connectModelSignals();
};

// **Implementation**
inline TreeView::TreeView(QObject* parent)
    : UIElement(parent), tree_widget_(nullptr), default_model_(nullptr) {}

inline TreeView& TreeView::model(QAbstractItemModel* model) {
    return static_cast<TreeView&>(
        setProperty("model", QVariant::fromValue(model)));
}

inline TreeView& TreeView::selectionBehavior(
    QAbstractItemView::SelectionBehavior behavior) {
    return static_cast<TreeView&>(
        setProperty("selectionBehavior", static_cast<int>(behavior)));
}

inline TreeView& TreeView::selectionMode(
    QAbstractItemView::SelectionMode mode) {
    return static_cast<TreeView&>(
        setProperty("selectionMode", static_cast<int>(mode)));
}

inline TreeView& TreeView::alternatingRowColors(bool alternate) {
    return static_cast<TreeView&>(
        setProperty("alternatingRowColors", alternate));
}

inline TreeView& TreeView::sortingEnabled(bool enabled) {
    return static_cast<TreeView&>(setProperty("sortingEnabled", enabled));
}

inline TreeView& TreeView::animated(bool animated) {
    return static_cast<TreeView&>(setProperty("animated", animated));
}

inline TreeView& TreeView::allColumnsShowFocus(bool show) {
    return static_cast<TreeView&>(setProperty("allColumnsShowFocus", show));
}

inline TreeView& TreeView::autoExpandDelay(int delay) {
    return static_cast<TreeView&>(setProperty("autoExpandDelay", delay));
}

inline TreeView& TreeView::expandsOnDoubleClick(bool expand) {
    return static_cast<TreeView&>(setProperty("expandsOnDoubleClick", expand));
}

inline TreeView& TreeView::headerHidden(bool hidden) {
    return static_cast<TreeView&>(setProperty("headerHidden", hidden));
}

inline TreeView& TreeView::itemsExpandable(bool expandable) {
    return static_cast<TreeView&>(setProperty("itemsExpandable", expandable));
}

inline TreeView& TreeView::rootIsDecorated(bool decorated) {
    return static_cast<TreeView&>(setProperty("rootIsDecorated", decorated));
}

inline TreeView& TreeView::uniformRowHeights(bool uniform) {
    return static_cast<TreeView&>(setProperty("uniformRowHeights", uniform));
}

inline TreeView& TreeView::wordWrap(bool wrap) {
    return static_cast<TreeView&>(setProperty("wordWrap", wrap));
}

inline TreeView& TreeView::onItemClicked(
    std::function<void(const QModelIndex&)> handler) {
    item_clicked_handler_ = std::move(handler);
    return *this;
}

inline TreeView& TreeView::onItemDoubleClicked(
    std::function<void(const QModelIndex&)> handler) {
    item_double_clicked_handler_ = std::move(handler);
    return *this;
}

inline TreeView& TreeView::onItemExpanded(
    std::function<void(const QModelIndex&)> handler) {
    item_expanded_handler_ = std::move(handler);
    return *this;
}

inline TreeView& TreeView::onItemCollapsed(
    std::function<void(const QModelIndex&)> handler) {
    item_collapsed_handler_ = std::move(handler);
    return *this;
}

inline TreeView& TreeView::onSelectionChanged(std::function<void()> handler) {
    selection_changed_handler_ = std::move(handler);
    return *this;
}

inline TreeView& TreeView::style(const QString& stylesheet) {
    return static_cast<TreeView&>(setProperty("styleSheet", stylesheet));
}

inline void TreeView::initialize() {
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
            connect(tree_widget_->selectionModel(),
                    &QItemSelectionModel::selectionChanged, this,
                    [this]() { selection_changed_handler_(); });
        }

        connectModelSignals();
    }
}

inline void TreeView::setupDefaultModel() {
    if (!default_model_) {
        default_model_ = new QStandardItemModel(this);
        tree_widget_->setModel(default_model_);
    }
}

inline void TreeView::connectModelSignals() {
    // Connect model signals for data updates
    if (tree_widget_->model()) {
        // Additional signal connections can be added here
    }
}

inline void TreeView::expand(const QModelIndex& index) {
    if (tree_widget_) {
        tree_widget_->expand(index);
    }
}

inline void TreeView::collapse(const QModelIndex& index) {
    if (tree_widget_) {
        tree_widget_->collapse(index);
    }
}

inline void TreeView::expandAll() {
    if (tree_widget_) {
        tree_widget_->expandAll();
    }
}

inline void TreeView::collapseAll() {
    if (tree_widget_) {
        tree_widget_->collapseAll();
    }
}

inline void TreeView::expandToDepth(int depth) {
    if (tree_widget_) {
        tree_widget_->expandToDepth(depth);
    }
}

inline void TreeView::setExpanded(const QModelIndex& index, bool expanded) {
    if (tree_widget_) {
        tree_widget_->setExpanded(index, expanded);
    }
}

inline bool TreeView::isExpanded(const QModelIndex& index) const {
    return tree_widget_ ? tree_widget_->isExpanded(index) : false;
}

inline void TreeView::scrollTo(const QModelIndex& index) {
    if (tree_widget_) {
        tree_widget_->scrollTo(index);
    }
}

inline void TreeView::resizeColumnToContents(int column) {
    if (tree_widget_) {
        tree_widget_->resizeColumnToContents(column);
    }
}

inline void TreeView::setColumnWidth(int column, int width) {
    if (tree_widget_) {
        tree_widget_->setColumnWidth(column, width);
    }
}

inline void TreeView::hideColumn(int column) {
    if (tree_widget_) {
        tree_widget_->hideColumn(column);
    }
}

inline void TreeView::showColumn(int column) {
    if (tree_widget_) {
        tree_widget_->showColumn(column);
    }
}

inline QStandardItem* TreeView::addRootItem(const QString& text) {
    if (default_model_) {
        QStandardItem* item = new QStandardItem(text);
        default_model_->appendRow(item);
        return item;
    }
    return nullptr;
}

inline QStandardItem* TreeView::addChildItem(QStandardItem* parent,
                                             const QString& text) {
    if (parent) {
        QStandardItem* item = new QStandardItem(text);
        parent->appendRow(item);
        return item;
    }
    return nullptr;
}

inline void TreeView::removeItem(QStandardItem* item) {
    if (item && default_model_) {
        default_model_->removeRow(item->row(), item->parent()
                                                   ? item->parent()->index()
                                                   : QModelIndex());
    }
}

inline void TreeView::clearData() {
    if (default_model_) {
        default_model_->clear();
    }
}

inline QAbstractItemModel* TreeView::getModel() const {
    return tree_widget_ ? tree_widget_->model() : nullptr;
}

inline QModelIndex TreeView::getCurrentIndex() const {
    return tree_widget_ ? tree_widget_->currentIndex() : QModelIndex();
}

inline QModelIndexList TreeView::getSelectedIndexes() const {
    return tree_widget_ ? tree_widget_->selectionModel()->selectedIndexes()
                        : QModelIndexList();
}

inline QStandardItem* TreeView::getRootItem(int row) const {
    return default_model_ ? default_model_->item(row) : nullptr;
}

inline QStandardItem* TreeView::getItemFromIndex(
    const QModelIndex& index) const {
    return default_model_ ? default_model_->itemFromIndex(index) : nullptr;
}

inline QModelIndex TreeView::getIndexFromItem(QStandardItem* item) const {
    return item ? item->index() : QModelIndex();
}

}  // namespace DeclarativeUI::Components
