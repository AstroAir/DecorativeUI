/**
 * @file Chart.hpp
 * @brief Data visualization chart component
 */

#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QPieSeries>
#include <memory>

QT_CHARTS_USE_NAMESPACE

enum class ChartType {
    Line,
    Bar,
    Pie,
    Area
};

class Chart : public QWidget {
    Q_OBJECT

public:
    explicit Chart(QWidget* parent = nullptr);
    
    void setChartType(ChartType type);
    void setData(const QList<QPointF>& data);
    void setTitle(const QString& title);
    void setAxisLabels(const QString& x_label, const QString& y_label);

signals:
    void chartTypeChanged(ChartType type);

private slots:
    void onChartTypeChanged();
    void onExportChart();

private:
    void setupUI();
    void createChart();
    void updateChart();

    QVBoxLayout* main_layout_;
    QHBoxLayout* controls_layout_;
    
    QComboBox* type_combo_;
    QPushButton* export_button_;
    QLabel* title_label_;
    
    QChartView* chart_view_;
    QChart* chart_;
    
    ChartType current_type_;
    QList<QPointF> chart_data_;
    QString chart_title_;
    QString x_axis_label_;
    QString y_axis_label_;
};
