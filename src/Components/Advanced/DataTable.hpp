#pragma once

/**
 * @file DataTable.hpp
 * @brief Advanced data table component with sorting, filtering, pagination, and editing
 *
 * The DataTable component provides a comprehensive data grid interface with:
 * - Virtual scrolling for large datasets
 * - Column sorting and filtering
 * - In-line editing with validation
 * - Row selection and bulk operations
 * - Pagination and infinite scrolling
 * - Export functionality
 * - Responsive column resizing
 * - Custom cell renderers
 */

#include <QTableView>
#include <QHeaderView>
#include <QAbstractItemModel>
#include <QSortFilterProxyModel>
#include <QStyledItemDelegate>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QDateEdit>
#include <QProgressBar>
#include <QToolBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMenu>
#include <QAction>
#include <QTimer>
#include <functional>
#include <memory>

#include "../../Core/UIElement.hpp"
#include "../Container.hpp"

namespace DeclarativeUI::Components::Advanced {

/**
 * @brief Column configuration for DataTable
 */
struct DataTableColumn {
    QString key;
    QString title;
    QString data_type = "string";  // string, number, date, boolean, custom
    int width = -1;  // -1 for auto-size
    bool sortable = true;
    bool filterable = true;
    bool editable = false;
    bool visible = true;
    Qt::Alignment alignment = Qt::AlignLeft | Qt::AlignVCenter;
    std::function<QWidget*(const QVariant&, QWidget*)> cell_renderer;
    std::function<QWidget*(QWidget*)> editor_factory;
    std::function<bool(const QVariant&)> validator;
    QString format_string;  // For dates, numbers, etc.

    DataTableColumn() = default;
    DataTableColumn(const QString& key, const QString& title)
        : key(key), title(title) {}
};

/**
 * @brief DataTable configuration
 */
struct DataTableConfig {
    bool sortable = true;
    bool filterable = true;
    bool editable = false;
    bool selectable = true;
    bool multi_select = true;
    bool show_grid = true;
    bool alternating_rows = true;
    bool virtual_scrolling = true;
    bool pagination = false;
    int items_per_page = 50;
    int buffer_size = 100;  // For virtual scrolling
    bool show_toolbar = true;
    bool show_status_bar = true;
    bool resizable_columns = true;
    bool reorderable_columns = false;
    QString selection_mode = "row";  // row, cell, column
};

/**
 * @brief Selection information
 */
struct DataTableSelection {
    QList<int> selected_rows;
    QList<int> selected_columns;
    QList<QPair<int, int>> selected_cells;

    bool isEmpty() const {
        return selected_rows.isEmpty() && selected_columns.isEmpty() && selected_cells.isEmpty();
    }

    int count() const {
        return selected_rows.size() + selected_columns.size() + selected_cells.size();
    }
};

/**
 * @brief Advanced DataTable component with comprehensive data grid functionality
 */
class DataTable : public Core::UIElement {
    Q_OBJECT

public:
    explicit DataTable(QObject* parent = nullptr);
    ~DataTable() override = default;

    // **Fluent interface for configuration**
    DataTable& model(QAbstractItemModel* model);
    DataTable& columns(const QList<DataTableColumn>& columns);
    DataTable& config(const DataTableConfig& config);
    DataTable& sortable(bool enabled);
    DataTable& filterable(bool enabled);
    DataTable& editable(bool enabled);
    DataTable& selectable(bool enabled);
    DataTable& multiSelect(bool enabled);
    DataTable& pagination(bool enabled);
    DataTable& itemsPerPage(int count);
    DataTable& virtualScrolling(bool enabled);
    DataTable& showToolbar(bool enabled);
    DataTable& showStatusBar(bool enabled);

    // **Event handlers**
    DataTable& onRowSelected(std::function<void(int)> handler);
    DataTable& onRowsSelected(std::function<void(const QList<int>&)> handler);
    DataTable& onCellClicked(std::function<void(int, int)> handler);
    DataTable& onCellDoubleClicked(std::function<void(int, int)> handler);
    DataTable& onCellEdited(std::function<void(int, int, const QVariant&)> handler);
    DataTable& onSortChanged(std::function<void(int, Qt::SortOrder)> handler);
    DataTable& onFilterChanged(std::function<void(const QString&)> handler);
    DataTable& onSelectionChanged(std::function<void(const DataTableSelection&)> handler);
    DataTable& onBulkOperation(std::function<void(const QString&, const QList<int>&)> handler);

    // **Cell renderers and editors**
    DataTable& cellRenderer(const QString& column, std::function<QWidget*(const QVariant&, QWidget*)> renderer);
    DataTable& editorFactory(const QString& column, std::function<QWidget*(QWidget*)> factory);
    DataTable& columnValidator(const QString& column, std::function<bool(const QVariant&)> validator);

    // **Public methods**
    QAbstractItemModel* getModel() const;
    void setModel(QAbstractItemModel* model);
    void addColumn(const DataTableColumn& column);
    void removeColumn(const QString& key);
    void setColumnVisible(const QString& key, bool visible);
    void setColumnWidth(const QString& key, int width);
    void sortByColumn(int column, Qt::SortOrder order);
    void setFilter(const QString& filter);
    void clearFilter();
    DataTableSelection getSelection() const;
    void selectRow(int row);
    void selectRows(const QList<int>& rows);
    void clearSelection();
    void exportData(const QString& format, const QString& filename);
    void refresh();

    // **UIElement interface**
    void initialize() override;

signals:
    void rowSelected(int row);
    void rowsSelected(const QList<int>& rows);
    void cellClicked(int row, int column);
    void cellDoubleClicked(int row, int column);
    void cellEdited(int row, int column, const QVariant& value);
    void sortChanged(int column, Qt::SortOrder order);
    void filterChanged(const QString& filter);
    void selectionChanged(const DataTableSelection& selection);
    void bulkOperation(const QString& operation, const QList<int>& rows);

private slots:
    void onTableSelectionChanged();
    void onTableCellClicked(const QModelIndex& index);
    void onTableCellDoubleClicked(const QModelIndex& index);
    void onHeaderSectionClicked(int logical_index);
    void onFilterTextChanged(const QString& text);
    void onBulkActionTriggered();

private:
    // **Core components**
    std::unique_ptr<Container> main_container_;
    std::unique_ptr<QToolBar> toolbar_;
    std::unique_ptr<QTableView> table_view_;
    std::unique_ptr<QSortFilterProxyModel> proxy_model_;
    std::unique_ptr<Container> status_container_;
    std::unique_ptr<QLabel> status_label_;
    std::unique_ptr<Container> pagination_container_;

    // **Configuration and state**
    DataTableConfig config_;
    QList<DataTableColumn> columns_;
    QAbstractItemModel* source_model_;
    DataTableSelection current_selection_;

    // **Toolbar components**
    QLineEdit* filter_input_;
    QPushButton* export_button_;
    QPushButton* refresh_button_;
    QMenu* bulk_actions_menu_;

    // **Event handlers**
    std::function<void(int)> row_selected_handler_;
    std::function<void(const QList<int>&)> rows_selected_handler_;
    std::function<void(int, int)> cell_clicked_handler_;
    std::function<void(int, int)> cell_double_clicked_handler_;
    std::function<void(int, int, const QVariant&)> cell_edited_handler_;
    std::function<void(int, Qt::SortOrder)> sort_changed_handler_;
    std::function<void(const QString&)> filter_changed_handler_;
    std::function<void(const DataTableSelection&)> selection_changed_handler_;
    std::function<void(const QString&, const QList<int>&)> bulk_operation_handler_;

    // **Custom renderers and editors**
    QMap<QString, std::function<QWidget*(const QVariant&, QWidget*)>> cell_renderers_;
    QMap<QString, std::function<QWidget*(QWidget*)>> editor_factories_;
    QMap<QString, std::function<bool(const QVariant&)>> column_validators_;

    // **Helper methods**
    void setupUI();
    void setupTableView();
    void setupToolbar();
    void setupStatusBar();
    void setupPagination();
    void setupEventHandlers();
    void setupStyling();

    void updateColumns();
    void updateSelection();
    void updateStatusBar();
    void updatePagination();

    int getColumnIndex(const QString& key) const;
    DataTableColumn* getColumn(const QString& key);
    QList<int> getSelectedRows() const;
    void applyFilter(const QString& filter);
    void performBulkOperation(const QString& operation);
    void exportToCSV(const QString& filename);
    void exportToJSON(const QString& filename);

    // Friend class for delegate access
    friend class DataTableItemDelegate;
};

/**
 * @brief Custom item delegate for advanced cell rendering and editing
 */
class DataTableItemDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:
    explicit DataTableItemDelegate(DataTable* parent);

    // **QStyledItemDelegate interface**
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;

signals:
    void cellEdited(int row, int column, const QVariant& value);

private:
    DataTable* data_table_;

    QWidget* createDefaultEditor(const QString& data_type, QWidget* parent) const;
    void paintCell(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
};

} // namespace DeclarativeUI::Components::Advanced
