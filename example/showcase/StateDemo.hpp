/**
 * @file StateDemo.hpp
 * @brief State management demonstration widget
 */

#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QSpinBox>
#include <QTextEdit>
#include <memory>

#include "Binding/StateManager.hpp"

using namespace DeclarativeUI;

class StateDemo : public QWidget {
    Q_OBJECT

public:
    explicit StateDemo(QWidget* parent = nullptr);

private slots:
    void onStateChanged();
    void onResetState();

private:
    void setupUI();
    void setupStateBindings();
    void updateDisplay();

    QVBoxLayout* main_layout_;
    QGroupBox* demo_group_;
    QTextEdit* state_display_;
    
    std::shared_ptr<Binding::StateManager> state_manager_;
};
