#include "PerformanceProfilerWidget.hpp"
#include <QDebug>

namespace DeclarativeUI::Debug {

PerformanceProfilerWidget::PerformanceProfilerWidget(QWidget* parent) 
    : QWidget(parent) {
    layout_ = new QVBoxLayout(this);
    
    start_button_ = new QPushButton("Start Profiling", this);
    stop_button_ = new QPushButton("Stop Profiling", this);
    reset_button_ = new QPushButton("Reset", this);
    export_button_ = new QPushButton("Export Profile", this);
    progress_bar_ = new QProgressBar(this);
    status_label_ = new QLabel("Ready", this);
    
    layout_->addWidget(start_button_);
    layout_->addWidget(stop_button_);
    layout_->addWidget(reset_button_);
    layout_->addWidget(export_button_);
    layout_->addWidget(progress_bar_);
    layout_->addWidget(status_label_);
    
    connect(start_button_, &QPushButton::clicked, this, &PerformanceProfilerWidget::onStartProfilingClicked);
    connect(stop_button_, &QPushButton::clicked, this, &PerformanceProfilerWidget::onStopProfilingClicked);
    connect(reset_button_, &QPushButton::clicked, this, &PerformanceProfilerWidget::onResetProfilingClicked);
    connect(export_button_, &QPushButton::clicked, this, &PerformanceProfilerWidget::onExportProfileClicked);
    
    qDebug() << "PerformanceProfilerWidget created";
}

void PerformanceProfilerWidget::onStartProfilingClicked() {
    qDebug() << "Start profiling clicked";
    status_label_->setText("Profiling...");
    progress_bar_->setValue(0);
}

void PerformanceProfilerWidget::onStopProfilingClicked() {
    qDebug() << "Stop profiling clicked";
    status_label_->setText("Stopped");
    progress_bar_->setValue(100);
}

void PerformanceProfilerWidget::onResetProfilingClicked() {
    qDebug() << "Reset profiling clicked";
    status_label_->setText("Ready");
    progress_bar_->setValue(0);
}

void PerformanceProfilerWidget::onExportProfileClicked() {
    qDebug() << "Export profile clicked";
}

}  // namespace DeclarativeUI::Debug
