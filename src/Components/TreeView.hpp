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

}  // namespace DeclarativeUI::Components
