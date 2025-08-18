/**
 * @file PerformanceMonitor.hpp
 * @brief Performance monitoring and profiling widget
 */

#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QProgressBar>
#include <QTimer>
#include <QTextEdit>
#include <QPushButton>
#include <memory>

#include "HotReload/PerformanceMonitor.hpp"

using namespace DeclarativeUI;

class PerformanceMonitor : public QWidget {
    Q_OBJECT

public:
    explicit PerformanceMonitor(QWidget* parent = nullptr);

private slots:
    void onStartMonitoring();
    void onStopMonitoring();
    void onClearMetrics();
    void updateMetrics();

private:
    void setupUI();
    void createMetricsDisplay();
    void createControls();
    void displayMetrics();

    QVBoxLayout* main_layout_;
    QGroupBox* metrics_group_;
    QGroupBox* controls_group_;
    
    QLabel* cpu_label_;
    QProgressBar* cpu_bar_;
    QLabel* memory_label_;
    QProgressBar* memory_bar_;
    QLabel* fps_label_;
    
    QPushButton* start_button_;
    QPushButton* stop_button_;
    QPushButton* clear_button_;
    
    QTextEdit* metrics_log_;
    QTimer* update_timer_;
    
    std::unique_ptr<HotReload::PerformanceMonitor> performance_monitor_;
    bool monitoring_active_;
};
