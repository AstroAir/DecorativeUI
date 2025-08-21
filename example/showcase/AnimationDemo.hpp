/**
 * @file AnimationDemo.hpp
 * @brief Animation system demonstration widget
 */

#pragma once

#include <QComboBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QVBoxLayout>
#include <QWidget>
#include <memory>

#include "Animation/AnimationEngine.hpp"

using namespace DeclarativeUI;

class AnimationDemo : public QWidget {
    Q_OBJECT

public:
    explicit AnimationDemo(QWidget* parent = nullptr);

private slots:
    void onStartAnimation();
    void onStopAnimation();
    void onAnimationTypeChanged();

private:
    void setupUI();
    void createAnimationControls();
    void createAnimationTarget();

    QVBoxLayout* main_layout_;
    QGroupBox* controls_group_;
    QGroupBox* target_group_;

    QPushButton* start_button_;
    QPushButton* stop_button_;
    QComboBox* animation_type_;
    QSlider* duration_slider_;

    QWidget* animation_target_;
    std::shared_ptr<Animation::Animation> current_animation_;
};
