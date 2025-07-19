#include "DebuggingConsole.hpp"
#include <QDebug>

namespace DeclarativeUI::Debug {

DebuggingConsole::DebuggingConsole(QWidget* parent) 
    : QWidget(parent) {
    layout_ = new QVBoxLayout(this);
    
    export_button_ = new QPushButton("Export Log", this);
    filter_combo_ = new QComboBox(this);
    log_display_ = new QTextEdit(this);
    
    filter_combo_->addItems({"All", "Error", "Warning", "Info", "Debug"});
    
    layout_->addWidget(export_button_);
    layout_->addWidget(filter_combo_);
    layout_->addWidget(log_display_);
    
    connect(export_button_, &QPushButton::clicked, this, &DebuggingConsole::onExportLogClicked);
    connect(filter_combo_, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &DebuggingConsole::onFilterChanged);
    
    qDebug() << "DebuggingConsole created";
}

void DebuggingConsole::onExportLogClicked() {
    qDebug() << "Export log clicked";
}

void DebuggingConsole::onFilterChanged() {
    qDebug() << "Filter changed to:" << filter_combo_->currentText();
}

}  // namespace DeclarativeUI::Debug
