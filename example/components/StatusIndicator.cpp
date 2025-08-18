/**
 * @file StatusIndicator.cpp
 * @brief Implementation of status indicator component
 */

#include "StatusIndicator.hpp"
#include <QPaintEvent>
#include <QDebug>

StatusIndicator::StatusIndicator(QWidget* parent)
    : QWidget(parent)
    , layout_(nullptr)
    , icon_label_(nullptr)
    , message_label_(nullptr)
    , current_status_(StatusType::Info)
    , current_message_("")
    , auto_hide_enabled_(false)
    , auto_hide_timeout_(3000)
    , animation_enabled_(true)
    , show_icon_(true)
    , show_message_(true)
    , auto_hide_timer_(nullptr)
    , animation_timer_(nullptr)
    , pulse_opacity_(1.0)
{
    setupUI();
    setFixedHeight(30);
    hide(); // Initially hidden
}

void StatusIndicator::setupUI() {
    layout_ = new QHBoxLayout(this);
    layout_->setContentsMargins(8, 4, 8, 4);
    layout_->setSpacing(8);
    
    icon_label_ = new QLabel();
    icon_label_->setFixedSize(16, 16);
    icon_label_->setAlignment(Qt::AlignCenter);
    
    message_label_ = new QLabel();
    message_label_->setWordWrap(false);
    
    layout_->addWidget(icon_label_);
    layout_->addWidget(message_label_, 1);
    
    // Setup timers
    auto_hide_timer_ = new QTimer(this);
    auto_hide_timer_->setSingleShot(true);
    connect(auto_hide_timer_, &QTimer::timeout, this, &StatusIndicator::onAutoHideTimeout);
    
    animation_timer_ = new QTimer(this);
    connect(animation_timer_, &QTimer::timeout, this, &StatusIndicator::updateAnimation);
    
    // Setup pulse animation
    pulse_animation_ = std::make_unique<QPropertyAnimation>(this, "windowOpacity");
    pulse_animation_->setDuration(1000);
    pulse_animation_->setLoopCount(-1);
    pulse_animation_->setKeyValueAt(0, 1.0);
    pulse_animation_->setKeyValueAt(0.5, 0.7);
    pulse_animation_->setKeyValueAt(1, 1.0);
}

void StatusIndicator::setStatus(StatusType type, const QString& message) {
    current_status_ = type;
    current_message_ = message;
    
    updateAppearance();
    
    // Show the indicator
    show();
    
    // Start pulse animation for loading status
    if (animation_enabled_ && type == StatusType::Loading) {
        startPulseAnimation();
    } else {
        stopPulseAnimation();
    }
    
    // Setup auto-hide if enabled
    if (auto_hide_enabled_ && type != StatusType::Loading) {
        auto_hide_timer_->start(auto_hide_timeout_);
    }
    
    emit statusChanged(type, message);
}

void StatusIndicator::clearStatus() {
    hide();
    stopPulseAnimation();
    auto_hide_timer_->stop();
    
    current_message_.clear();
    emit statusCleared();
}

void StatusIndicator::setAutoHide(bool enabled, int timeout_ms) {
    auto_hide_enabled_ = enabled;
    auto_hide_timeout_ = timeout_ms;
}

void StatusIndicator::setAnimationEnabled(bool enabled) {
    animation_enabled_ = enabled;
    if (!enabled) {
        stopPulseAnimation();
    }
}

void StatusIndicator::setShowIcon(bool show) {
    show_icon_ = show;
    icon_label_->setVisible(show);
}

void StatusIndicator::setShowMessage(bool show) {
    show_message_ = show;
    message_label_->setVisible(show);
}

void StatusIndicator::updateAppearance() {
    QColor status_color = getStatusColor(current_status_);
    QString status_icon = getStatusIcon(current_status_);
    
    // Update icon
    if (show_icon_) {
        icon_label_->setText(status_icon);
        icon_label_->setStyleSheet(QString("QLabel { color: %1; font-size: 14px; font-weight: bold; }")
                                  .arg(status_color.name()));
    }
    
    // Update message
    if (show_message_) {
        message_label_->setText(current_message_);
        message_label_->setStyleSheet(QString("QLabel { color: %1; }")
                                     .arg(status_color.name()));
    }
    
    // Update background
    setStyleSheet(QString(
        "StatusIndicator {"
        "    background-color: %1;"
        "    border: 1px solid %2;"
        "    border-radius: 4px;"
        "}"
    ).arg(status_color.name()).arg(status_color.darker(120).name()));
}

void StatusIndicator::startPulseAnimation() {
    if (animation_enabled_ && pulse_animation_) {
        pulse_animation_->start();
    }
}

void StatusIndicator::stopPulseAnimation() {
    if (pulse_animation_) {
        pulse_animation_->stop();
        setWindowOpacity(1.0);
    }
}

QColor StatusIndicator::getStatusColor(StatusType type) const {
    switch (type) {
        case StatusType::Success:
            return QColor("#27ae60");
        case StatusType::Warning:
            return QColor("#f39c12");
        case StatusType::Error:
            return QColor("#e74c3c");
        case StatusType::Loading:
            return QColor("#3498db");
        case StatusType::Info:
        default:
            return QColor("#34495e");
    }
}

QString StatusIndicator::getStatusIcon(StatusType type) const {
    switch (type) {
        case StatusType::Success:
            return "✓";
        case StatusType::Warning:
            return "⚠";
        case StatusType::Error:
            return "✗";
        case StatusType::Loading:
            return "⟳";
        case StatusType::Info:
        default:
            return "ℹ";
    }
}

void StatusIndicator::onAutoHideTimeout() {
    clearStatus();
}

void StatusIndicator::updateAnimation() {
    // Update animation frame
    if (current_status_ == StatusType::Loading) {
        static int rotation = 0;
        rotation = (rotation + 30) % 360;
        
        // Rotate the loading icon
        icon_label_->setStyleSheet(QString(
            "QLabel {"
            "    color: %1;"
            "    font-size: 14px;"
            "    font-weight: bold;"
            "    transform: rotate(%2deg);"
            "}"
        ).arg(getStatusColor(current_status_).name()).arg(rotation));
    }
}

void StatusIndicator::paintEvent(QPaintEvent* event) {
    QWidget::paintEvent(event);
    
    // Custom painting if needed
    if (current_status_ == StatusType::Loading && animation_enabled_) {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        
        // Draw loading spinner background
        QRect spinner_rect = icon_label_->geometry();
        painter.setPen(QPen(getStatusColor(current_status_).lighter(), 2));
        painter.drawEllipse(spinner_rect.adjusted(2, 2, -2, -2));
    }
}

#include "StatusIndicator.moc"
