// Components/TableView.hpp
#pragma once
#include <QAbstractItemModel>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QModelIndex>
#include <QStandardItemModel>
#include <QTableView>


#include "../Core/UIElement.hpp"

namespace DeclarativeUI::Components {

class TableView : public Core::UIElement {
    Q_OBJECT

public:
    explicit TableView(QObject* parent = nullptr);

    // **Fluent interface for table view**
    TableView& model(QAbstractItemModel* model);
    TableView& selectionBehavior(QAbstractItemView::SelectionBehavior behavior);
    TableView& selectionMode(QAbstractItemView::SelectionMode mode);
    TableView& alternatingRowColors(bool alternate);
    TableView& sortingEnabled(bool enabled);
    TableView& showGrid(bool show);
    TableView& gridStyle(Qt::PenStyle style);
    TableView& wordWrap(bool wrap);
    TableView& cornerButtonEnabled(bool enabled);
    TableView& horizontalHeaderVisible(bool visible);
    TableView& verticalHeaderVisible(bool visible);
    TableView& horizontalHeaderStretchLastSection(bool stretch);
    TableView& verticalHeaderStretchLastSection(bool stretch);
    TableView& onCellClicked(std::function<void(int, int)> handler);
    TableView& onCellDoubleClicked(std::function<void(int, int)> handler);
    TableView& onSelectionChanged(std::function<void()> handler);
    TableView& style(const QString& stylesheet);

    void initialize() override;

    // **Table operations**
    void setColumnWidth(int column, int width);
    void setRowHeight(int row, int height);
    void resizeColumnsToContents();
    void resizeRowsToContents();
    void hideColumn(int column);
    void hideRow(int row);
    void showColumn(int column);
    void showRow(int row);
    void selectRow(int row);
    void selectColumn(int column);
    void clearSelection();

    // **Data operations**
    void setData(int row, int column, const QVariant& value);
    QVariant getData(int row, int column) const;
    void insertRow(int row);
    void insertColumn(int column);
    void removeRow(int row);
    void removeColumn(int column);
    void clearData();

    // **Getters**
    QAbstractItemModel* getModel() const;
    QModelIndex getCurrentIndex() const;
    QModelIndexList getSelectedIndexes() const;
    int getRowCount() const;
    int getColumnCount() const;

private:
    QTableView* table_widget_;
    QStandardItemModel* default_model_;
    std::function<void(int, int)> cell_clicked_handler_;
    std::function<void(int, int)> cell_double_clicked_handler_;
    std::function<void()> selection_changed_handler_;

    void setupDefaultModel();
    void connectModelSignals();
};




}  // namespace DeclarativeUI::Components
