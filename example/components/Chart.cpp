/**
 * @file Chart.cpp
 * @brief Basic implementation of chart component
 */

#include "Chart.hpp"
#include <QDebug>

Chart::Chart(QWidget* parent)
    : QWidget(parent)
    , main_layout_(nullptr)
    , controls_layout_(nullptr)
    , type_combo_(nullptr)
    , export_button_(nullptr)
    , title_label_(nullptr)
    , chart_view_(nullptr)
    , chart_(nullptr)
    , current_type_(ChartType::Line)
    , chart_title_("Sample Chart")
    , x_axis_label_("X Axis")
    , y_axis_label_("Y Axis")
{
    setupUI();
    createChart();
}

void Chart::setupUI() {
    main_layout_ = new QVBoxLayout(this);
    
    // Create controls
    controls_layout_ = new QHBoxLayout();
    
    type_combo_ = new QComboBox();
    type_combo_->addItems({"Line Chart", "Bar Chart", "Pie Chart", "Area Chart"});
    
    export_button_ = new QPushButton("Export");
    title_label_ = new QLabel(chart_title_);
    title_label_->setStyleSheet("QLabel { font-size: 16px; font-weight: bold; }");
    
    controls_layout_->addWidget(new QLabel("Type:"));
    controls_layout_->addWidget(type_combo_);
    controls_layout_->addWidget(export_button_);
    controls_layout_->addStretch();
    controls_layout_->addWidget(title_label_);
    
    main_layout_->addLayout(controls_layout_);
    
    // Connect signals
    connect(type_combo_, &QComboBox::currentTextChanged, this, &Chart::onChartTypeChanged);
    connect(export_button_, &QPushButton::clicked, this, &Chart::onExportChart);
}

void Chart::createChart() {
    chart_ = new QChart();
    chart_->setTitle(chart_title_);
    chart_->setAnimationOptions(QChart::SeriesAnimations);
    
    chart_view_ = new QChartView(chart_);
    chart_view_->setRenderHint(QPainter::Antialiasing);
    
    main_layout_->addWidget(chart_view_);
    
    // Add sample data
    QList<QPointF> sample_data;
    for (int i = 0; i < 10; ++i) {
        sample_data.append(QPointF(i, qrand() % 100));
    }
    setData(sample_data);
}

void Chart::setChartType(ChartType type) {
    current_type_ = type;
    updateChart();
    emit chartTypeChanged(type);
}

void Chart::setData(const QList<QPointF>& data) {
    chart_data_ = data;
    updateChart();
}

void Chart::setTitle(const QString& title) {
    chart_title_ = title;
    if (chart_) {
        chart_->setTitle(title);
    }
    if (title_label_) {
        title_label_->setText(title);
    }
}

void Chart::setAxisLabels(const QString& x_label, const QString& y_label) {
    x_axis_label_ = x_label;
    y_axis_label_ = y_label;
    updateChart();
}

void Chart::onChartTypeChanged() {
    QString type_text = type_combo_->currentText();
    
    if (type_text == "Line Chart") {
        setChartType(ChartType::Line);
    } else if (type_text == "Bar Chart") {
        setChartType(ChartType::Bar);
    } else if (type_text == "Pie Chart") {
        setChartType(ChartType::Pie);
    } else if (type_text == "Area Chart") {
        setChartType(ChartType::Area);
    }
}

void Chart::onExportChart() {
    qDebug() << "Chart export requested";
    // In a real implementation, this would export the chart to an image file
}

void Chart::updateChart() {
    if (!chart_) return;
    
    // Clear existing series
    chart_->removeAllSeries();
    
    switch (current_type_) {
        case ChartType::Line: {
            auto series = new QLineSeries();
            series->setName("Data");
            for (const QPointF& point : chart_data_) {
                series->append(point);
            }
            chart_->addSeries(series);
            chart_->createDefaultAxes();
            break;
        }
        case ChartType::Bar: {
            auto series = new QBarSeries();
            auto set = new QBarSet("Data");
            for (const QPointF& point : chart_data_) {
                *set << point.y();
            }
            series->append(set);
            chart_->addSeries(series);
            chart_->createDefaultAxes();
            break;
        }
        case ChartType::Pie: {
            auto series = new QPieSeries();
            for (int i = 0; i < chart_data_.size(); ++i) {
                const QPointF& point = chart_data_[i];
                series->append(QString("Item %1").arg(i + 1), point.y());
            }
            chart_->addSeries(series);
            break;
        }
        case ChartType::Area: {
            auto series = new QLineSeries();
            series->setName("Data");
            for (const QPointF& point : chart_data_) {
                series->append(point);
            }
            chart_->addSeries(series);
            chart_->createDefaultAxes();
            break;
        }
    }
    
    chart_->setTitle(chart_title_);
}

#include "Chart.moc"
