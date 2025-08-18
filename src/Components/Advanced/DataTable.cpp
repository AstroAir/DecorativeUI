#include "DataTable.hpp"

#include <QApplication>
#include <QHeaderView>
#include <QScrollBar>
#include <QSplitter>
#include <QFileDialog>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QTextStream>
#include <QDebug>

#include "../../Exceptions/UIExceptions.hpp"

namespace DeclarativeUI::Components::Advanced {

DataTable::DataTable(QObject* parent)
    : Core::UIElement(parent)
    , config_()
    , source_model_(nullptr)
{
}

DataTable& DataTable::model(QAbstractItemModel* model) {
    source_model_ = model;
    if (proxy_model_) {
        proxy_model_->setSourceModel(model);
    }
    return *this;
}

DataTable& DataTable::columns(const QList<DataTableColumn>& columns) {
    columns_ = columns;
    updateColumns();
    return *this;
}

DataTable& DataTable::config(const DataTableConfig& config) {
    config_ = config;
    return *this;
}

DataTable& DataTable::sortable(bool enabled) {
    config_.sortable = enabled;
    if (table_view_) {
        table_view_->setSortingEnabled(enabled);
    }
    return *this;
}

DataTable& DataTable::filterable(bool enabled) {
    config_.filterable = enabled;
    if (filter_input_) {
        filter_input_->setVisible(enabled);
    }
    return *this;
}

DataTable& DataTable::editable(bool enabled) {
    config_.editable = enabled;
    if (table_view_) {
        table_view_->setEditTriggers(enabled ?
            QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed :
            QAbstractItemView::NoEditTriggers);
    }
    return *this;
}

DataTable& DataTable::selectable(bool enabled) {
    config_.selectable = enabled;
    if (table_view_) {
        table_view_->setSelectionMode(enabled ?
            QAbstractItemView::ExtendedSelection :
            QAbstractItemView::NoSelection);
    }
    return *this;
}

DataTable& DataTable::multiSelect(bool enabled) {
    config_.multi_select = enabled;
    if (table_view_) {
        table_view_->setSelectionMode(enabled ?
            QAbstractItemView::ExtendedSelection :
            QAbstractItemView::SingleSelection);
    }
    return *this;
}

DataTable& DataTable::pagination(bool enabled) {
    config_.pagination = enabled;
    if (pagination_container_) {
        pagination_container_->getWidget()->setVisible(enabled);
    }
    return *this;
}

DataTable& DataTable::itemsPerPage(int count) {
    config_.items_per_page = count;
    updatePagination();
    return *this;
}

DataTable& DataTable::virtualScrolling(bool enabled) {
    config_.virtual_scrolling = enabled;
    return *this;
}

DataTable& DataTable::showToolbar(bool enabled) {
    config_.show_toolbar = enabled;
    if (toolbar_) {
        toolbar_->setVisible(enabled);
    }
    return *this;
}

DataTable& DataTable::showStatusBar(bool enabled) {
    config_.show_status_bar = enabled;
    if (status_container_) {
        status_container_->getWidget()->setVisible(enabled);
    }
    return *this;
}

DataTable& DataTable::onRowSelected(std::function<void(int)> handler) {
    row_selected_handler_ = std::move(handler);
    return *this;
}

DataTable& DataTable::onRowsSelected(std::function<void(const QList<int>&)> handler) {
    rows_selected_handler_ = std::move(handler);
    return *this;
}

DataTable& DataTable::onCellClicked(std::function<void(int, int)> handler) {
    cell_clicked_handler_ = std::move(handler);
    return *this;
}

DataTable& DataTable::onCellDoubleClicked(std::function<void(int, int)> handler) {
    cell_double_clicked_handler_ = std::move(handler);
    return *this;
}

DataTable& DataTable::onCellEdited(std::function<void(int, int, const QVariant&)> handler) {
    cell_edited_handler_ = std::move(handler);
    return *this;
}

DataTable& DataTable::onSortChanged(std::function<void(int, Qt::SortOrder)> handler) {
    sort_changed_handler_ = std::move(handler);
    return *this;
}

DataTable& DataTable::onFilterChanged(std::function<void(const QString&)> handler) {
    filter_changed_handler_ = std::move(handler);
    return *this;
}

DataTable& DataTable::onSelectionChanged(std::function<void(const DataTableSelection&)> handler) {
    selection_changed_handler_ = std::move(handler);
    return *this;
}

DataTable& DataTable::onBulkOperation(std::function<void(const QString&, const QList<int>&)> handler) {
    bulk_operation_handler_ = std::move(handler);
    return *this;
}

DataTable& DataTable::cellRenderer(const QString& column, std::function<QWidget*(const QVariant&, QWidget*)> renderer) {
    cell_renderers_[column] = std::move(renderer);
    return *this;
}

DataTable& DataTable::editorFactory(const QString& column, std::function<QWidget*(QWidget*)> factory) {
    editor_factories_[column] = std::move(factory);
    return *this;
}

DataTable& DataTable::columnValidator(const QString& column, std::function<bool(const QVariant&)> validator) {
    column_validators_[column] = std::move(validator);
    return *this;
}

QAbstractItemModel* DataTable::getModel() const {
    return source_model_;
}

void DataTable::setModel(QAbstractItemModel* model) {
    source_model_ = model;
    if (proxy_model_) {
        proxy_model_->setSourceModel(model);
    }
    updateStatusBar();
}

void DataTable::addColumn(const DataTableColumn& column) {
    columns_.append(column);
    updateColumns();
}

void DataTable::removeColumn(const QString& key) {
    columns_.removeIf([&key](const DataTableColumn& col) {
        return col.key == key;
    });
    updateColumns();
}

void DataTable::setColumnVisible(const QString& key, bool visible) {
    auto* column = getColumn(key);
    if (column) {
        column->visible = visible;
        int index = getColumnIndex(key);
        if (index >= 0 && table_view_) {
            table_view_->setColumnHidden(index, !visible);
        }
    }
}

void DataTable::setColumnWidth(const QString& key, int width) {
    auto* column = getColumn(key);
    if (column) {
        column->width = width;
        int index = getColumnIndex(key);
        if (index >= 0 && table_view_) {
            table_view_->setColumnWidth(index, width);
        }
    }
}

void DataTable::sortByColumn(int column, Qt::SortOrder order) {
    if (table_view_) {
        table_view_->sortByColumn(column, order);
    }
}

void DataTable::setFilter(const QString& filter) {
    if (proxy_model_) {
        proxy_model_->setFilterWildcard(filter);
    }
    if (filter_input_) {
        filter_input_->setText(filter);
    }
    updateStatusBar();
}

void DataTable::clearFilter() {
    setFilter("");
}

DataTableSelection DataTable::getSelection() const {
    return current_selection_;
}

void DataTable::selectRow(int row) {
    if (table_view_) {
        table_view_->selectRow(row);
    }
}

void DataTable::selectRows(const QList<int>& rows) {
    if (table_view_) {
        table_view_->clearSelection();
        for (int row : rows) {
            table_view_->selectRow(row);
        }
    }
}

void DataTable::clearSelection() {
    if (table_view_) {
        table_view_->clearSelection();
    }
}

void DataTable::exportData(const QString& format, const QString& filename) {
    if (!source_model_) return;

    if (format.toLower() == "csv") {
        exportToCSV(filename);
    } else if (format.toLower() == "json") {
        exportToJSON(filename);
    }
}

void DataTable::refresh() {
    if (table_view_) {
        table_view_->reset();
        updateStatusBar();
    }
}

void DataTable::initialize() {
    try {
        setupUI();
        setupTableView();
        setupToolbar();
        setupStatusBar();
        setupPagination();
        setupEventHandlers();
        setupStyling();

        qDebug() << "✅ DataTable initialized successfully";
    } catch (const std::exception& e) {
        throw Exceptions::UIException("Failed to initialize DataTable: " + std::string(e.what()));
    }
}

void DataTable::setupUI() {
    // Create main container
    main_container_ = std::make_unique<Container>();
    main_container_->initialize();

    // Create proxy model for sorting and filtering
    proxy_model_ = std::make_unique<QSortFilterProxyModel>();
    if (source_model_) {
        proxy_model_->setSourceModel(source_model_);
    }

    // Create table view
    table_view_ = std::make_unique<QTableView>();
    table_view_->setModel(proxy_model_.get());

    // Create toolbar
    if (config_.show_toolbar) {
        toolbar_ = std::make_unique<QToolBar>();
    }

    // Create status container
    if (config_.show_status_bar) {
        status_container_ = std::make_unique<Container>();
        status_container_->initialize();
        status_label_ = std::make_unique<QLabel>("Ready");
    }

    // Create pagination container
    if (config_.pagination) {
        pagination_container_ = std::make_unique<Container>();
        pagination_container_->initialize();
    }

    // Set main widget
    setWidget(main_container_->getWidget());
}

void DataTable::setupTableView() {
    if (!table_view_) return;

    // Configure table view
    table_view_->setSortingEnabled(config_.sortable);
    table_view_->setAlternatingRowColors(config_.alternating_rows);
    table_view_->setShowGrid(config_.show_grid);
    table_view_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_view_->setSelectionMode(config_.multi_select ?
        QAbstractItemView::ExtendedSelection :
        QAbstractItemView::SingleSelection);
    table_view_->setEditTriggers(config_.editable ?
        QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed :
        QAbstractItemView::NoEditTriggers);

    // Configure headers
    QHeaderView* horizontal_header = table_view_->horizontalHeader();
    horizontal_header->setStretchLastSection(true);
    horizontal_header->setSectionResizeMode(config_.resizable_columns ?
        QHeaderView::Interactive : QHeaderView::Fixed);
    horizontal_header->setSectionsMovable(config_.reorderable_columns);

    QHeaderView* vertical_header = table_view_->verticalHeader();
    vertical_header->setDefaultSectionSize(32);
    vertical_header->setMinimumSectionSize(24);

    // Set custom delegate
    auto* delegate = new DataTableItemDelegate(this);
    table_view_->setItemDelegate(delegate);
    connect(delegate, &DataTableItemDelegate::cellEdited,
            this, &DataTable::cellEdited);
}

void DataTable::setupToolbar() {
    if (!toolbar_) return;

    // Filter input
    if (config_.filterable) {
        toolbar_->addWidget(new QLabel("Filter:"));
        filter_input_ = new QLineEdit();
        filter_input_->setPlaceholderText("Search...");
        filter_input_->setMaximumWidth(200);
        toolbar_->addWidget(filter_input_);
        toolbar_->addSeparator();
    }

    // Export button
    export_button_ = new QPushButton("Export");
    export_button_->setIcon(QIcon(":/icons/export.png"));
    toolbar_->addWidget(export_button_);

    // Refresh button
    refresh_button_ = new QPushButton("Refresh");
    refresh_button_->setIcon(QIcon(":/icons/refresh.png"));
    toolbar_->addWidget(refresh_button_);

    // Bulk actions menu
    if (config_.selectable) {
        toolbar_->addSeparator();
        auto* bulk_actions_button = new QPushButton("Actions");
        bulk_actions_menu_ = new QMenu();
        bulk_actions_menu_->addAction("Delete Selected", this, &DataTable::onBulkActionTriggered);
        bulk_actions_menu_->addAction("Export Selected", this, &DataTable::onBulkActionTriggered);
        bulk_actions_button->setMenu(bulk_actions_menu_);
        toolbar_->addWidget(bulk_actions_button);
    }
}

void DataTable::setupStatusBar() {
    if (!status_container_ || !status_label_) return;

    updateStatusBar();
}

void DataTable::setupPagination() {
    if (!pagination_container_) return;

    // TODO: Implement pagination controls
    updatePagination();
}

void DataTable::setupEventHandlers() {
    if (table_view_) {
        // Selection changes
        connect(table_view_->selectionModel(), &QItemSelectionModel::selectionChanged,
                this, &DataTable::onTableSelectionChanged);

        // Cell clicks
        connect(table_view_.get(), &QTableView::clicked,
                this, &DataTable::onTableCellClicked);
        connect(table_view_.get(), &QTableView::doubleClicked,
                this, &DataTable::onTableCellDoubleClicked);

        // Header clicks for sorting
        connect(table_view_->horizontalHeader(), &QHeaderView::sectionClicked,
                this, &DataTable::onHeaderSectionClicked);
    }

    if (filter_input_) {
        connect(filter_input_, &QLineEdit::textChanged,
                this, &DataTable::onFilterTextChanged);
    }

    if (export_button_) {
        connect(export_button_, &QPushButton::clicked, [this]() {
            QString filename = QFileDialog::getSaveFileName(nullptr, "Export Data", "", "CSV Files (*.csv);;JSON Files (*.json)");
            if (!filename.isEmpty()) {
                QString format = filename.endsWith(".json") ? "json" : "csv";
                exportData(format, filename);
            }
        });
    }

    if (refresh_button_) {
        connect(refresh_button_, &QPushButton::clicked, this, &DataTable::refresh);
    }
}

void DataTable::setupStyling() {
    if (table_view_) {
        table_view_->setStyleSheet(R"(
            QTableView {
                gridline-color: #e0e0e0;
                background-color: white;
                alternate-background-color: #f8f9fa;
                selection-background-color: #e3f2fd;
            }
            QTableView::item {
                padding: 8px;
                border: none;
            }
            QTableView::item:selected {
                background-color: #e3f2fd;
                color: #1976d2;
            }
            QHeaderView::section {
                background-color: #f5f5f5;
                padding: 8px;
                border: 1px solid #e0e0e0;
                font-weight: bold;
            }
            QHeaderView::section:hover {
                background-color: #eeeeee;
            }
        )");
    }
}

void DataTable::onTableSelectionChanged() {
    updateSelection();
    updateStatusBar();

    // Emit signals
    QList<int> selected_rows = getSelectedRows();
    current_selection_.selected_rows = selected_rows;

    emit selectionChanged(current_selection_);
    if (selection_changed_handler_) {
        selection_changed_handler_(current_selection_);
    }

    if (!selected_rows.isEmpty()) {
        emit rowsSelected(selected_rows);
        if (rows_selected_handler_) {
            rows_selected_handler_(selected_rows);
        }

        if (selected_rows.size() == 1) {
            emit rowSelected(selected_rows.first());
            if (row_selected_handler_) {
                row_selected_handler_(selected_rows.first());
            }
        }
    }
}

void DataTable::onTableCellClicked(const QModelIndex& index) {
    if (!index.isValid()) return;

    int row = index.row();
    int column = index.column();

    emit cellClicked(row, column);
    if (cell_clicked_handler_) {
        cell_clicked_handler_(row, column);
    }
}

void DataTable::onTableCellDoubleClicked(const QModelIndex& index) {
    if (!index.isValid()) return;

    int row = index.row();
    int column = index.column();

    emit cellDoubleClicked(row, column);
    if (cell_double_clicked_handler_) {
        cell_double_clicked_handler_(row, column);
    }
}

void DataTable::onHeaderSectionClicked(int logical_index) {
    if (!proxy_model_) return;

    Qt::SortOrder order = proxy_model_->sortOrder();
    if (proxy_model_->sortColumn() == logical_index) {
        order = (order == Qt::AscendingOrder) ? Qt::DescendingOrder : Qt::AscendingOrder;
    } else {
        order = Qt::AscendingOrder;
    }

    emit sortChanged(logical_index, order);
    if (sort_changed_handler_) {
        sort_changed_handler_(logical_index, order);
    }
}

void DataTable::onFilterTextChanged(const QString& text) {
    applyFilter(text);

    emit filterChanged(text);
    if (filter_changed_handler_) {
        filter_changed_handler_(text);
    }
}

void DataTable::onBulkActionTriggered() {
    auto* action = qobject_cast<QAction*>(sender());
    if (!action) return;

    QString operation = action->text();
    QList<int> selected_rows = getSelectedRows();

    if (!selected_rows.isEmpty()) {
        emit bulkOperation(operation, selected_rows);
        if (bulk_operation_handler_) {
            bulk_operation_handler_(operation, selected_rows);
        }
    }
}

// Helper methods
void DataTable::updateColumns() {
    // TODO: Update table columns based on configuration
}

void DataTable::updateSelection() {
    current_selection_.selected_rows = getSelectedRows();
}

void DataTable::updateStatusBar() {
    if (!status_label_) return;

    int total_rows = proxy_model_ ? proxy_model_->rowCount() : 0;
    int selected_count = current_selection_.selected_rows.size();

    QString status = QString("Total: %1").arg(total_rows);
    if (selected_count > 0) {
        status += QString(" | Selected: %1").arg(selected_count);
    }

    status_label_->setText(status);
}

void DataTable::updatePagination() {
    // TODO: Update pagination controls
}

int DataTable::getColumnIndex(const QString& key) const {
    for (int i = 0; i < columns_.size(); ++i) {
        if (columns_[i].key == key) {
            return i;
        }
    }
    return -1;
}

DataTableColumn* DataTable::getColumn(const QString& key) {
    for (auto& column : columns_) {
        if (column.key == key) {
            return &column;
        }
    }
    return nullptr;
}

QList<int> DataTable::getSelectedRows() const {
    QList<int> rows;
    if (table_view_) {
        QModelIndexList selected = table_view_->selectionModel()->selectedRows();
        for (const QModelIndex& index : selected) {
            rows.append(index.row());
        }
    }
    return rows;
}

void DataTable::applyFilter(const QString& filter) {
    if (proxy_model_) {
        proxy_model_->setFilterWildcard(filter);
        updateStatusBar();
    }
}

void DataTable::exportToCSV(const QString& filename) {
    // TODO: Implement CSV export
    qDebug() << "Exporting to CSV:" << filename;
}

void DataTable::exportToJSON(const QString& filename) {
    // TODO: Implement JSON export
    qDebug() << "Exporting to JSON:" << filename;
}

// DataTableItemDelegate implementation
DataTableItemDelegate::DataTableItemDelegate(DataTable* parent)
    : QStyledItemDelegate(parent), data_table_(parent) {
}

QWidget* DataTableItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    Q_UNUSED(option)

    // Check for custom editor factory
    if (data_table_ && index.column() < data_table_->columns_.size()) {
        const QString& column_key = data_table_->columns_[index.column()].key;
        if (data_table_->editor_factories_.contains(column_key)) {
            return data_table_->editor_factories_[column_key](parent);
        }

        // Use default editor based on data type
        const QString& data_type = data_table_->columns_[index.column()].data_type;
        return createDefaultEditor(data_type, parent);
    }

    return QStyledItemDelegate::createEditor(parent, option, index);
}

void DataTableItemDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const {
    QStyledItemDelegate::setEditorData(editor, index);
}

void DataTableItemDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const {
    QStyledItemDelegate::setModelData(editor, model, index);

    // Emit cell edited signal
    emit cellEdited(index.row(), index.column(), index.data());
}

void DataTableItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    // Check for custom cell renderer
    if (data_table_ && index.column() < data_table_->columns_.size()) {
        const QString& column_key = data_table_->columns_[index.column()].key;
        if (data_table_->cell_renderers_.contains(column_key)) {
            // Custom rendering would be implemented here
            // For now, fall back to default
        }
    }

    QStyledItemDelegate::paint(painter, option, index);
}

QSize DataTableItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
    return QStyledItemDelegate::sizeHint(option, index);
}

QWidget* DataTableItemDelegate::createDefaultEditor(const QString& data_type, QWidget* parent) const {
    if (data_type == "number") {
        return new QSpinBox(parent);
    } else if (data_type == "date") {
        return new QDateEdit(parent);
    } else if (data_type == "boolean") {
        return new QCheckBox(parent);
    } else {
        return new QLineEdit(parent);
    }
}

} // namespace DeclarativeUI::Components::Advanced
