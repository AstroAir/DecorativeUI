/**
 * @file EnhancedComponents.cpp
 * @brief Implementation of enhanced component utilities
 */

#include "EnhancedComponents.hpp"
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QDebug>

std::unique_ptr<NavigationBar> EnhancedComponents::createNavigationBar(QWidget* parent) {
    auto nav_bar = std::make_unique<NavigationBar>(parent);
    applyModernStyling(nav_bar.get());
    return nav_bar;
}

std::unique_ptr<StatusIndicator> EnhancedComponents::createStatusIndicator(QWidget* parent) {
    auto indicator = std::make_unique<StatusIndicator>(parent);
    applyModernStyling(indicator.get());
    return indicator;
}

std::unique_ptr<SearchBox> EnhancedComponents::createSearchBox(QWidget* parent) {
    auto search_box = std::make_unique<SearchBox>(parent);
    applyModernStyling(search_box.get());
    return search_box;
}

std::unique_ptr<DataTable> EnhancedComponents::createDataTable(QWidget* parent) {
    auto data_table = std::make_unique<DataTable>(parent);
    applyModernStyling(data_table.get());
    return data_table;
}

std::unique_ptr<Chart> EnhancedComponents::createChart(QWidget* parent) {
    auto chart = std::make_unique<Chart>(parent);
    applyModernStyling(chart.get());
    return chart;
}

void EnhancedComponents::applyModernStyling(QWidget* widget) {
    if (!widget) return;
    
    // Apply modern flat design styling
    widget->setStyleSheet(
        widget->styleSheet() +
        "QWidget {"
        "    font-family: 'Segoe UI', Arial, sans-serif;"
        "    font-size: 12px;"
        "}"
        "QPushButton {"
        "    border: none;"
        "    border-radius: 4px;"
        "    padding: 8px 16px;"
        "    background-color: #3498db;"
        "    color: white;"
        "    font-weight: 500;"
        "}"
        "QPushButton:hover {"
        "    background-color: #2980b9;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #21618c;"
        "}"
        "QLineEdit {"
        "    border: 2px solid #bdc3c7;"
        "    border-radius: 4px;"
        "    padding: 6px 12px;"
        "    background-color: white;"
        "}"
        "QLineEdit:focus {"
        "    border-color: #3498db;"
        "}"
        "QComboBox {"
        "    border: 2px solid #bdc3c7;"
        "    border-radius: 4px;"
        "    padding: 6px 12px;"
        "    background-color: white;"
        "}"
        "QComboBox:focus {"
        "    border-color: #3498db;"
        "}"
    );
}

void EnhancedComponents::addDropShadow(QWidget* widget) {
    if (!widget) return;
    
    auto shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(10);
    shadow->setColor(QColor(0, 0, 0, 80));
    shadow->setOffset(0, 2);
    widget->setGraphicsEffect(shadow);
}

void EnhancedComponents::addHoverEffect(QWidget* widget) {
    if (!widget) return;
    
    // Install event filter for hover effects
    widget->setAttribute(Qt::WA_Hover, true);
    
    // This would typically involve installing an event filter
    // For now, we'll just apply hover-capable styling
    QString current_style = widget->styleSheet();
    if (!current_style.contains(":hover")) {
        widget->setStyleSheet(current_style +
            "QWidget:hover {"
            "    background-color: rgba(52, 152, 219, 0.1);"
            "}"
        );
    }
}

QIcon EnhancedComponents::createColoredIcon(const QString& icon_name, const QColor& color) {
    // In a real implementation, this would create a colored version of an icon
    // For now, return an empty icon
    Q_UNUSED(icon_name)
    Q_UNUSED(color)
    return QIcon();
}

void EnhancedComponents::applyLightTheme(QWidget* widget) {
    if (!widget) return;
    
    widget->setStyleSheet(
        "QWidget {"
        "    background-color: #ffffff;"
        "    color: #2c3e50;"
        "}"
        "QPushButton {"
        "    background-color: #3498db;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 4px;"
        "    padding: 8px 16px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #2980b9;"
        "}"
        "QLineEdit {"
        "    background-color: white;"
        "    border: 2px solid #bdc3c7;"
        "    border-radius: 4px;"
        "    padding: 6px 12px;"
        "}"
        "QLineEdit:focus {"
        "    border-color: #3498db;"
        "}"
        "QGroupBox {"
        "    font-weight: bold;"
        "    border: 2px solid #bdc3c7;"
        "    border-radius: 4px;"
        "    margin-top: 10px;"
        "    padding-top: 10px;"
        "}"
        "QGroupBox::title {"
        "    subcontrol-origin: margin;"
        "    left: 10px;"
        "    padding: 0 5px 0 5px;"
        "}"
    );
}

void EnhancedComponents::applyDarkTheme(QWidget* widget) {
    if (!widget) return;
    
    widget->setStyleSheet(
        "QWidget {"
        "    background-color: #2c3e50;"
        "    color: #ecf0f1;"
        "}"
        "QPushButton {"
        "    background-color: #34495e;"
        "    color: #ecf0f1;"
        "    border: 1px solid #7f8c8d;"
        "    border-radius: 4px;"
        "    padding: 8px 16px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #4a6741;"
        "}"
        "QLineEdit {"
        "    background-color: #34495e;"
        "    border: 2px solid #7f8c8d;"
        "    border-radius: 4px;"
        "    padding: 6px 12px;"
        "    color: #ecf0f1;"
        "}"
        "QLineEdit:focus {"
        "    border-color: #3498db;"
        "}"
        "QGroupBox {"
        "    font-weight: bold;"
        "    border: 2px solid #7f8c8d;"
        "    border-radius: 4px;"
        "    margin-top: 10px;"
        "    padding-top: 10px;"
        "}"
        "QGroupBox::title {"
        "    subcontrol-origin: margin;"
        "    left: 10px;"
        "    padding: 0 5px 0 5px;"
        "}"
        "QTableWidget {"
        "    background-color: #34495e;"
        "    alternate-background-color: #2c3e50;"
        "    gridline-color: #7f8c8d;"
        "}"
        "QHeaderView::section {"
        "    background-color: #34495e;"
        "    border: 1px solid #7f8c8d;"
        "    padding: 4px;"
        "}"
    );
}

void EnhancedComponents::applyCustomTheme(QWidget* widget, const QColor& primary, const QColor& secondary) {
    if (!widget) return;
    
    widget->setStyleSheet(QString(
        "QWidget {"
        "    background-color: %2;"
        "    color: #2c3e50;"
        "}"
        "QPushButton {"
        "    background-color: %1;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 4px;"
        "    padding: 8px 16px;"
        "}"
        "QPushButton:hover {"
        "    background-color: %1;"
        "    opacity: 0.8;"
        "}"
        "QLineEdit {"
        "    background-color: white;"
        "    border: 2px solid %1;"
        "    border-radius: 4px;"
        "    padding: 6px 12px;"
        "}"
        "QLineEdit:focus {"
        "    border-color: %1;"
        "}"
    ).arg(primary.name()).arg(secondary.name()));
}
