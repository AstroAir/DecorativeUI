/**
 * @file DataTable.cpp
 * @brief Basic implementation of data table component
 */

#include "DataTable.hpp"
#include <QDebug>

DataTable::DataTable(QWidget* parent)
    : QWidget(parent),
      main_layout_(nullptr),
      controls_layout_(nullptr),
      table_(nullptr),
      prev_button_(nullptr),
      next_button_(nullptr),
      page_label_(nullptr),
      page_spin_(nullptr),
      page_size_combo_(nullptr),
      current_page_(0),
      page_size_(10),
      sorting_enabled_(true),
      filtering_enabled_(true) {
    setupUI();
}

void DataTable::setupUI() {
    main_layout_ = new QVBoxLayout(this);

    // Create table
    table_ = new QTableWidget();
    table_->setSortingEnabled(sorting_enabled_);
    table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_->setAlternatingRowColors(true);

    // Create controls
    controls_layout_ = new QHBoxLayout();
    prev_button_ = new QPushButton("Previous");
    next_button_ = new QPushButton("Next");
    page_label_ = new QLabel("Page:");
    page_spin_ = new QSpinBox();
    page_size_combo_ = new QComboBox();
    page_size_combo_->addItems({"10", "25", "50", "100"});

    controls_layout_->addWidget(prev_button_);
    controls_layout_->addWidget(next_button_);
    controls_layout_->addWidget(page_label_);
    controls_layout_->addWidget(page_spin_);
    controls_layout_->addWidget(new QLabel("Page Size:"));
    controls_layout_->addWidget(page_size_combo_);
    controls_layout_->addStretch();

    main_layout_->addWidget(table_);
    main_layout_->addLayout(controls_layout_);

    // Connect signals
    connect(table_, &QTableWidget::cellClicked, this,
            &DataTable::onItemClicked);
    connect(prev_button_, &QPushButton::clicked, [this]() {
        if (current_page_ > 0) {
            current_page_--;
            updateTable();
            emit pageChanged(current_page_);
        }
    });
    connect(next_button_, &QPushButton::clicked, [this]() {
        current_page_++;
        updateTable();
        emit pageChanged(current_page_);
    });
}

void DataTable::setData(const QList<QStringList>& data) {
    table_data_ = data;
    updateTable();
}

void DataTable::setHeaders(const QStringList& headers) {
    headers_ = headers;
    table_->setColumnCount(headers.size());
    table_->setHorizontalHeaderLabels(headers);
}

void DataTable::setPageSize(int size) {
    page_size_ = size;
    updateTable();
}

void DataTable::enableSorting(bool enabled) {
    sorting_enabled_ = enabled;
    table_->setSortingEnabled(enabled);
}

void DataTable::enableFiltering(bool enabled) { filtering_enabled_ = enabled; }

void DataTable::onItemClicked(int row, int column) {
    emit itemSelected(row, column);
}

void DataTable::onPageChanged() { updateTable(); }

void DataTable::onFilterChanged() { updateTable(); }

void DataTable::updateTable() {
    // Calculate page bounds
    int start_row = current_page_ * page_size_;
    int end_row = qMin(start_row + page_size_, table_data_.size());

    // Set table size
    table_->setRowCount(end_row - start_row);

    // Populate table
    for (int i = start_row; i < end_row; ++i) {
        const QStringList& row_data = table_data_[i];
        for (int j = 0; j < row_data.size() && j < table_->columnCount(); ++j) {
            table_->setItem(i - start_row, j,
                            new QTableWidgetItem(row_data[j]));
        }
    }

    updatePagination();
}

void DataTable::updatePagination() {
    int total_pages = (table_data_.size() + page_size_ - 1) / page_size_;

    prev_button_->setEnabled(current_page_ > 0);
    next_button_->setEnabled(current_page_ < total_pages - 1);

    page_spin_->setRange(1, qMax(1, total_pages));
    page_spin_->setValue(current_page_ + 1);

    page_label_->setText(
        QString("Page %1 of %2").arg(current_page_ + 1).arg(total_pages));
}

#include "DataTable.moc"
