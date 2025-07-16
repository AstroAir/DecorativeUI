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

// **Implementation**
inline TableView::TableView(QObject* parent)
    : UIElement(parent), table_widget_(nullptr), default_model_(nullptr) {}

inline TableView& TableView::model(QAbstractItemModel* model) {
    return static_cast<TableView&>(
        setProperty("model", QVariant::fromValue(model)));
}

inline TableView& TableView::selectionBehavior(
    QAbstractItemView::SelectionBehavior behavior) {
    return static_cast<TableView&>(
        setProperty("selectionBehavior", static_cast<int>(behavior)));
}

inline TableView& TableView::selectionMode(
    QAbstractItemView::SelectionMode mode) {
    return static_cast<TableView&>(
        setProperty("selectionMode", static_cast<int>(mode)));
}

inline TableView& TableView::alternatingRowColors(bool alternate) {
    return static_cast<TableView&>(
        setProperty("alternatingRowColors", alternate));
}

inline TableView& TableView::sortingEnabled(bool enabled) {
    return static_cast<TableView&>(setProperty("sortingEnabled", enabled));
}

inline TableView& TableView::showGrid(bool show) {
    return static_cast<TableView&>(setProperty("showGrid", show));
}

inline TableView& TableView::gridStyle(Qt::PenStyle style) {
    return static_cast<TableView&>(
        setProperty("gridStyle", static_cast<int>(style)));
}

inline TableView& TableView::wordWrap(bool wrap) {
    return static_cast<TableView&>(setProperty("wordWrap", wrap));
}

inline TableView& TableView::cornerButtonEnabled(bool enabled) {
    return static_cast<TableView&>(setProperty("cornerButtonEnabled", enabled));
}

inline TableView& TableView::horizontalHeaderVisible(bool visible) {
    return static_cast<TableView&>(
        setProperty("horizontalHeaderVisible", visible));
}

inline TableView& TableView::verticalHeaderVisible(bool visible) {
    return static_cast<TableView&>(
        setProperty("verticalHeaderVisible", visible));
}

inline TableView& TableView::horizontalHeaderStretchLastSection(bool stretch) {
    return static_cast<TableView&>(
        setProperty("horizontalHeaderStretchLastSection", stretch));
}

inline TableView& TableView::verticalHeaderStretchLastSection(bool stretch) {
    return static_cast<TableView&>(
        setProperty("verticalHeaderStretchLastSection", stretch));
}

inline TableView& TableView::onCellClicked(
    std::function<void(int, int)> handler) {
    cell_clicked_handler_ = std::move(handler);
    return *this;
}

inline TableView& TableView::onCellDoubleClicked(
    std::function<void(int, int)> handler) {
    cell_double_clicked_handler_ = std::move(handler);
    return *this;
}

inline TableView& TableView::onSelectionChanged(std::function<void()> handler) {
    selection_changed_handler_ = std::move(handler);
    return *this;
}

inline TableView& TableView::style(const QString& stylesheet) {
    return static_cast<TableView&>(setProperty("styleSheet", stylesheet));
}

inline void TableView::initialize() {
    if (!table_widget_) {
        table_widget_ = new QTableView();
        setWidget(table_widget_);

        // Setup default model if none provided
        setupDefaultModel();

        // Connect signals
        if (cell_clicked_handler_) {
            connect(table_widget_, &QTableView::clicked, this,
                    [this](const QModelIndex& index) {
                        cell_clicked_handler_(index.row(), index.column());
                    });
        }

        if (cell_double_clicked_handler_) {
            connect(table_widget_, &QTableView::doubleClicked, this,
                    [this](const QModelIndex& index) {
                        cell_double_clicked_handler_(index.row(),
                                                     index.column());
                    });
        }

        if (selection_changed_handler_) {
            connect(table_widget_->selectionModel(),
                    &QItemSelectionModel::selectionChanged, this,
                    [this]() { selection_changed_handler_(); });
        }

        connectModelSignals();
    }
}

inline void TableView::setupDefaultModel() {
    if (!default_model_) {
        default_model_ = new QStandardItemModel(this);
        table_widget_->setModel(default_model_);
    }
}

inline void TableView::connectModelSignals() {
    // Connect model signals for data updates
    if (table_widget_->model()) {
        // Additional signal connections can be added here
    }
}

inline void TableView::setColumnWidth(int column, int width) {
    if (table_widget_) {
        table_widget_->setColumnWidth(column, width);
    }
}

inline void TableView::setRowHeight(int row, int height) {
    if (table_widget_) {
        table_widget_->setRowHeight(row, height);
    }
}

inline void TableView::resizeColumnsToContents() {
    if (table_widget_) {
        table_widget_->resizeColumnsToContents();
    }
}

inline void TableView::resizeRowsToContents() {
    if (table_widget_) {
        table_widget_->resizeRowsToContents();
    }
}

inline void TableView::hideColumn(int column) {
    if (table_widget_) {
        table_widget_->hideColumn(column);
    }
}

inline void TableView::hideRow(int row) {
    if (table_widget_) {
        table_widget_->hideRow(row);
    }
}

inline void TableView::showColumn(int column) {
    if (table_widget_) {
        table_widget_->showColumn(column);
    }
}

inline void TableView::showRow(int row) {
    if (table_widget_) {
        table_widget_->showRow(row);
    }
}

inline void TableView::selectRow(int row) {
    if (table_widget_) {
        table_widget_->selectRow(row);
    }
}

inline void TableView::selectColumn(int column) {
    if (table_widget_) {
        table_widget_->selectColumn(column);
    }
}

inline void TableView::clearSelection() {
    if (table_widget_) {
        table_widget_->clearSelection();
    }
}

inline void TableView::setData(int row, int column, const QVariant& value) {
    if (default_model_) {
        // Ensure the model has enough rows and columns
        if (row >= default_model_->rowCount()) {
            default_model_->setRowCount(row + 1);
        }
        if (column >= default_model_->columnCount()) {
            default_model_->setColumnCount(column + 1);
        }

        QModelIndex index = default_model_->index(row, column);
        default_model_->setData(index, value);
    }
}

inline QVariant TableView::getData(int row, int column) const {
    if (default_model_) {
        QModelIndex index = default_model_->index(row, column);
        return default_model_->data(index);
    }
    return QVariant();
}

inline void TableView::insertRow(int row) {
    if (default_model_) {
        default_model_->insertRow(row);
    }
}

inline void TableView::insertColumn(int column) {
    if (default_model_) {
        default_model_->insertColumn(column);
    }
}

inline void TableView::removeRow(int row) {
    if (default_model_) {
        default_model_->removeRow(row);
    }
}

inline void TableView::removeColumn(int column) {
    if (default_model_) {
        default_model_->removeColumn(column);
    }
}

inline void TableView::clearData() {
    if (default_model_) {
        default_model_->clear();
    }
}

inline QAbstractItemModel* TableView::getModel() const {
    return table_widget_ ? table_widget_->model() : nullptr;
}

inline QModelIndex TableView::getCurrentIndex() const {
    return table_widget_ ? table_widget_->currentIndex() : QModelIndex();
}

inline QModelIndexList TableView::getSelectedIndexes() const {
    return table_widget_ ? table_widget_->selectionModel()->selectedIndexes()
                         : QModelIndexList();
}

inline int TableView::getRowCount() const {
    return default_model_ ? default_model_->rowCount() : 0;
}

inline int TableView::getColumnCount() const {
    return default_model_ ? default_model_->columnCount() : 0;
}

}  // namespace DeclarativeUI::Components
