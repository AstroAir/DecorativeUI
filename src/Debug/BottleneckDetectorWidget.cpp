#include "BottleneckDetectorWidget.hpp"
#include <QDebug>

namespace DeclarativeUI::Debug {

BottleneckDetectorWidget::BottleneckDetectorWidget(QWidget* parent) 
    : QWidget(parent) {
    layout_ = new QVBoxLayout(this);
    
    refresh_button_ = new QPushButton("Refresh", this);
    resolve_button_ = new QPushButton("Resolve", this);
    bottleneck_list_ = new QListWidget(this);
    
    layout_->addWidget(refresh_button_);
    layout_->addWidget(resolve_button_);
    layout_->addWidget(bottleneck_list_);
    
    connect(refresh_button_, &QPushButton::clicked, this, &BottleneckDetectorWidget::onRefreshClicked);
    connect(resolve_button_, &QPushButton::clicked, this, &BottleneckDetectorWidget::onResolveBottleneckClicked);
    connect(bottleneck_list_, &QListWidget::itemClicked, this, &BottleneckDetectorWidget::onBottleneckSelected);
    
    qDebug() << "BottleneckDetectorWidget created";
}

void BottleneckDetectorWidget::onBottleneckSelected() {
    qDebug() << "Bottleneck selected";
}

void BottleneckDetectorWidget::onRefreshClicked() {
    qDebug() << "Refresh clicked";
}

void BottleneckDetectorWidget::onResolveBottleneckClicked() {
    qDebug() << "Resolve bottleneck clicked";
}

}  // namespace DeclarativeUI::Debug
