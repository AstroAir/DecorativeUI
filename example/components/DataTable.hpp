/**
 * @file DataTable.hpp
 * @brief Advanced data table component with sorting, filtering, and pagination
 */

#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QComboBox>
#include <memory>

class DataTable : public QWidget {
    Q_OBJECT

public:
    explicit DataTable(QWidget* parent = nullptr);
    
    void setData(const QList<QStringList>& data);
    void setHeaders(const QStringList& headers);
    void setPageSize(int size);
    void enableSorting(bool enabled);
    void enableFiltering(bool enabled);

signals:
    void itemSelected(int row, int column);
    void pageChanged(int page);

private slots:
    void onItemClicked(int row, int column);
    void onPageChanged();
    void onFilterChanged();

private:
    void setupUI();
    void updateTable();
    void updatePagination();

    QVBoxLayout* main_layout_;
    QHBoxLayout* controls_layout_;
    QTableWidget* table_;
    
    QPushButton* prev_button_;
    QPushButton* next_button_;
    QLabel* page_label_;
    QSpinBox* page_spin_;
    QComboBox* page_size_combo_;
    
    QList<QStringList> table_data_;
    QStringList headers_;
    int current_page_;
    int page_size_;
    bool sorting_enabled_;
    bool filtering_enabled_;
};
