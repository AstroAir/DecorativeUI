// Components/TableView.cpp
#include "TableView.hpp"

namespace DeclarativeUI::Components {

// **Implementation**
TableView::TableView(QObject* parent)
    : UIElement(parent), table_widget_(nullptr), default_model_(nullptr) {}

TableView& TableView::model(QAbstractItemModel* model) {
    return static_cast<TableView&>(
        setProperty("model", QVariant::fromValue(model)));
}

TableView& TableView::selectionBehavior(
    QAbstractItemView::SelectionBehavior behavior) {
    return static_cast<TableView&>(
        setProperty("selectionBehavior", static_cast<int>(behavior)));
}

TableView& TableView::selectionMode(
    QAbstractItemView::SelectionMode mode) {
    return static_cast<TableView&>(
        setProperty("selectionMode", static_cast<int>(mode)));
}

TableView& TableView::alternatingRowColors(bool alternate) {
    return static_cast<TableView&>(
        setProperty("alternatingRowColors", alternate));
}

TableView& TableView::sortingEnabled(bool enabled) {
    return static_cast<TableView&>(setProperty("sortingEnabled", enabled));
}

TableView& TableView::showGrid(bool show) {
    return static_cast<TableView&>(setProperty("showGrid", show));
}

TableView& TableView::gridStyle(Qt::PenStyle style) {
    return static_cast<TableView&>(
        setProperty("gridStyle", static_cast<int>(style)));
}

TableView& TableView::wordWrap(bool wrap) {
    return static_cast<TableView&>(setProperty("wordWrap", wrap));
}

TableView& TableView::cornerButtonEnabled(bool enabled) {
    return static_cast<TableView&>(setProperty("cornerButtonEnabled", enabled));
}

TableView& TableView::horizontalHeaderVisible(bool visible) {
    return static_cast<TableView&>(
        setProperty("horizontalHeaderVisible", visible));
}

TableView& TableView::verticalHeaderVisible(bool visible) {
    return static_cast<TableView&>(
        setProperty("verticalHeaderVisible", visible));
}

TableView& TableView::horizontalHeaderStretchLastSection(bool stretch) {
    return static_cast<TableView&>(
        setProperty("horizontalHeaderStretchLastSection", stretch));
}

TableView& TableView::verticalHeaderStretchLastSection(bool stretch) {
    return static_cast<TableView&>(
        setProperty("verticalHeaderStretchLastSection", stretch));
}

TableView& TableView::onCellClicked(
    std::function<void(int, int)> handler) {
    cell_clicked_handler_ = std::move(handler);
    return *this;
}

TableView& TableView::onCellDoubleClicked(
    std::function<void(int, int)> handler) {
    cell_double_clicked_handler_ = std::move(handler);
    return *this;
}

TableView& TableView::onSelectionChanged(std::function<void()> handler) {
    selection_changed_handler_ = std::move(handler);
    return *this;
}

TableView& TableView::style(const QString& stylesheet) {
    return static_cast<TableView&>(setProperty("styleSheet", stylesheet));
}

void TableView::initialize() {
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
                        cell_double_clicked_handler_(index.row(), index.column());
                    });
        }

        if (selection_changed_handler_) {
            connect(table_widget_->selectionModel(), &QItemSelectionModel::selectionChanged, this,
                    [this]() { selection_changed_handler_(); });
        }

        connectModelSignals();
    }
}

void TableView::setupDefaultModel() {
    if (!default_model_) {
        default_model_ = new QStandardItemModel(this);
        table_widget_->setModel(default_model_);
    }
}

void TableView::connectModelSignals() {
    // Connect model signals for data updates
    if (table_widget_->model()) {
        // Additional signal connections can be added here
    }
}

void TableView::setColumnWidth(int column, int width) {
    if (table_widget_) {
        table_widget_->setColumnWidth(column, width);
    }
}

void TableView::setRowHeight(int row, int height) {
    if (table_widget_) {
        table_widget_->setRowHeight(row, height);
    }
}

void TableView::resizeColumnsToContents() {
    if (table_widget_) {
        table_widget_->resizeColumnsToContents();
    }
}

void TableView::resizeRowsToContents() {
    if (table_widget_) {
        table_widget_->resizeRowsToContents();
    }
}

void TableView::hideColumn(int column) {
    if (table_widget_) {
        table_widget_->hideColumn(column);
    }
}

void TableView::hideRow(int row) {
    if (table_widget_) {
        table_widget_->hideRow(row);
    }
}

void TableView::showColumn(int column) {
    if (table_widget_) {
        table_widget_->showColumn(column);
    }
}

void TableView::showRow(int row) {
    if (table_widget_) {
        table_widget_->showRow(row);
    }
}

void TableView::selectRow(int row) {
    if (table_widget_) {
        table_widget_->selectRow(row);
    }
}

void TableView::selectColumn(int column) {
    if (table_widget_) {
        table_widget_->selectColumn(column);
    }
}

void TableView::clearSelection() {
    if (table_widget_) {
        table_widget_->clearSelection();
    }
}

void TableView::setData(int row, int column, const QVariant& value) {
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

QVariant TableView::getData(int row, int column) const {
    if (default_model_) {
        QModelIndex index = default_model_->index(row, column);
        return default_model_->data(index);
    }
    return QVariant();
}

void TableView::insertRow(int row) {
    if (default_model_) {
        default_model_->insertRow(row);
    }
}

void TableView::insertColumn(int column) {
    if (default_model_) {
        default_model_->insertColumn(column);
    }
}

void TableView::removeRow(int row) {
    if (default_model_) {
        default_model_->removeRow(row);
    }
}

void TableView::removeColumn(int column) {
    if (default_model_) {
        default_model_->removeColumn(column);
    }
}

void TableView::clearData() {
    if (default_model_) {
        default_model_->clear();
    }
}

QAbstractItemModel* TableView::getModel() const {
    return table_widget_ ? table_widget_->model() : nullptr;
}

QModelIndex TableView::getCurrentIndex() const {
    return table_widget_ ? table_widget_->currentIndex() : QModelIndex();
}

QModelIndexList TableView::getSelectedIndexes() const {
    return table_widget_ ? table_widget_->selectionModel()->selectedIndexes()
                         : QModelIndexList();
}

int TableView::getRowCount() const {
    return default_model_ ? default_model_->rowCount() : 0;
}

int TableView::getColumnCount() const {
    return default_model_ ? default_model_->columnCount() : 0;
}

}  // namespace DeclarativeUI::Components
