// Components/ListView.hpp
#pragma once
#include <QListView>
#include <QAbstractItemModel>
#include <QStringListModel>
#include <QStandardItemModel>
#include <QModelIndex>

#include "../Core/UIElement.hpp"

namespace DeclarativeUI::Components {

class ListView : public Core::UIElement {
    Q_OBJECT

public:
    explicit ListView(QObject* parent = nullptr);

    // **Fluent interface for list view**
    ListView& model(QAbstractItemModel* model);
    ListView& stringListModel(const QStringList& strings);
    ListView& standardItemModel(QStandardItemModel* model);
    ListView& selectionMode(QAbstractItemView::SelectionMode mode);
    ListView& selectionBehavior(QAbstractItemView::SelectionBehavior behavior);
    ListView& editTriggers(QAbstractItemView::EditTriggers triggers);
    ListView& dragDropMode(QAbstractItemView::DragDropMode mode);
    ListView& defaultDropAction(Qt::DropAction action);
    ListView& alternatingRowColors(bool enable);
    ListView& sortingEnabled(bool enable);
    ListView& uniformItemSizes(bool uniform);
    ListView& wordWrap(bool wrap);
    ListView& spacing(int spacing);
    ListView& gridSize(const QSize& size);
    ListView& viewMode(QListView::ViewMode mode);
    ListView& movement(QListView::Movement movement);
    ListView& flow(QListView::Flow flow);
    ListView& resizeMode(QListView::ResizeMode mode);
    ListView& layoutMode(QListView::LayoutMode mode);
    ListView& batchSize(int batchSize);
    ListView& onClicked(std::function<void(const QModelIndex&)> handler);
    ListView& onDoubleClicked(std::function<void(const QModelIndex&)> handler);
    ListView& onPressed(std::function<void(const QModelIndex&)> handler);
    ListView& onActivated(std::function<void(const QModelIndex&)> handler);
    ListView& onEntered(std::function<void(const QModelIndex&)> handler);
    ListView& onViewportEntered(std::function<void()> handler);
    ListView& onIconSizeChanged(std::function<void(const QSize&)> handler);
    ListView& style(const QString& stylesheet);

    void initialize() override;
    QAbstractItemModel* getModel() const;
    QModelIndex getCurrentIndex() const;
    QModelIndexList getSelectedIndexes() const;
    void setCurrentIndex(const QModelIndex& index);
    void clearSelection();
    void selectAll();
    void scrollTo(const QModelIndex& index, QAbstractItemView::ScrollHint hint = QAbstractItemView::EnsureVisible);
    void addItem(const QString& text);
    void addItems(const QStringList& texts);
    void removeItem(int row);
    void clear();
    int count() const;
    QString itemText(int row) const;
    void setItemText(int row, const QString& text);

private:
    QListView* list_view_widget_;
    QStringListModel* string_list_model_;
    std::function<void(const QModelIndex&)> clicked_handler_;
    std::function<void(const QModelIndex&)> double_clicked_handler_;
    std::function<void(const QModelIndex&)> pressed_handler_;
    std::function<void(const QModelIndex&)> activated_handler_;
    std::function<void(const QModelIndex&)> entered_handler_;
    std::function<void()> viewport_entered_handler_;
    std::function<void(const QSize&)> icon_size_changed_handler_;
};





}  // namespace DeclarativeUI::Components
