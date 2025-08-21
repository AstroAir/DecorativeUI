/**
 * @file AnimationDemo.cpp
 * @brief Implementation of animation demonstration
 */

#include "AnimationDemo.hpp"
#include <QDebug>

AnimationDemo::AnimationDemo(QWidget* parent)
    : QWidget(parent),
      main_layout_(nullptr),
      controls_group_(nullptr),
      target_group_(nullptr),
      start_button_(nullptr),
      stop_button_(nullptr),
      animation_type_(nullptr),
      duration_slider_(nullptr),
      animation_target_(nullptr) {
    setupUI();
    createAnimationControls();
    createAnimationTarget();
}

void AnimationDemo::setupUI() {
    main_layout_ = new QVBoxLayout(this);
    main_layout_->setContentsMargins(8, 8, 8, 8);
}

void AnimationDemo::createAnimationControls() {
    controls_group_ = new QGroupBox("Animation Controls");
    auto controls_layout = new QHBoxLayout(controls_group_);

    start_button_ = new QPushButton("Start Animation");
    stop_button_ = new QPushButton("Stop Animation");
    stop_button_->setEnabled(false);

    animation_type_ = new QComboBox();
    animation_type_->addItems(
        {"Fade In/Out", "Slide Left/Right", "Scale Up/Down", "Rotate"});

    duration_slider_ = new QSlider(Qt::Horizontal);
    duration_slider_->setRange(100, 3000);
    duration_slider_->setValue(1000);

    auto duration_label = new QLabel("Duration: 1000ms");

    controls_layout->addWidget(new QLabel("Type:"));
    controls_layout->addWidget(animation_type_);
    controls_layout->addWidget(start_button_);
    controls_layout->addWidget(stop_button_);
    controls_layout->addWidget(duration_label);
    controls_layout->addWidget(duration_slider_);

    // Connect signals
    connect(start_button_, &QPushButton::clicked, this,
            &AnimationDemo::onStartAnimation);
    connect(stop_button_, &QPushButton::clicked, this,
            &AnimationDemo::onStopAnimation);
    connect(animation_type_, &QComboBox::currentTextChanged, this,
            &AnimationDemo::onAnimationTypeChanged);
    connect(duration_slider_, &QSlider::valueChanged,
            [duration_label](int value) {
                duration_label->setText(QString("Duration: %1ms").arg(value));
            });

    main_layout_->addWidget(controls_group_);
}

void AnimationDemo::createAnimationTarget() {
    target_group_ = new QGroupBox("Animation Target");
    auto target_layout = new QVBoxLayout(target_group_);

    animation_target_ = new QWidget();
    animation_target_->setFixedSize(100, 100);
    animation_target_->setStyleSheet(
        "QWidget { "
        "background-color: #3498db; "
        "border: 2px solid #2980b9; "
        "border-radius: 8px; "
        "}");

    // Center the target
    auto center_layout = new QHBoxLayout();
    center_layout->addStretch();
    center_layout->addWidget(animation_target_);
    center_layout->addStretch();

    target_layout->addStretch();
    target_layout->addLayout(center_layout);
    target_layout->addStretch();

    main_layout_->addWidget(target_group_);

    // Add description
    auto description = new QLabel(
        "This demo shows the DeclarativeUI animation system in action. "
        "Select an animation type and click 'Start Animation' to see smooth "
        "transitions.");
    description->setWordWrap(true);
    description->setStyleSheet("QLabel { color: #666; font-style: italic; }");
    main_layout_->addWidget(description);
}

void AnimationDemo::onStartAnimation() {
    start_button_->setEnabled(false);
    stop_button_->setEnabled(true);

    QString type = animation_type_->currentText();
    int duration = duration_slider_->value();

    qDebug() << "Starting animation:" << type << "Duration:" << duration
             << "ms";

    // Simulate animation with a timer (real implementation would use
    // Animation::AnimationEngine)
    QTimer::singleShot(duration, this, [this]() { onStopAnimation(); });

    // Visual feedback
    animation_target_->setStyleSheet(
        "QWidget { "
        "background-color: #e74c3c; "
        "border: 2px solid #c0392b; "
        "border-radius: 8px; "
        "}");
}

void AnimationDemo::onStopAnimation() {
    start_button_->setEnabled(true);
    stop_button_->setEnabled(false);

    qDebug() << "Animation stopped";

    // Reset visual state
    animation_target_->setStyleSheet(
        "QWidget { "
        "background-color: #3498db; "
        "border: 2px solid #2980b9; "
        "border-radius: 8px; "
        "}");
}

void AnimationDemo::onAnimationTypeChanged() {
    QString type = animation_type_->currentText();
    qDebug() << "Animation type changed to:" << type;

    // Update target appearance based on animation type
    QString color = "#3498db";
    if (type.contains("Fade"))
        color = "#9b59b6";
    else if (type.contains("Slide"))
        color = "#e67e22";
    else if (type.contains("Scale"))
        color = "#27ae60";
    else if (type.contains("Rotate"))
        color = "#f39c12";

    animation_target_->setStyleSheet(QString("QWidget { "
                                             "background-color: %1; "
                                             "border: 2px solid %1; "
                                             "border-radius: 8px; "
                                             "}")
                                         .arg(color));
}

#include "AnimationDemo.moc"
