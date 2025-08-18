/**
 * @file StatusIndicator.hpp
 * @brief Status indicator component with animations
 */

#pragma once

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QPropertyAnimation>
#include <QPainter>
#include <QColor>

/**
 * @brief Status types for the indicator
 */
enum class StatusType {
    Success,
    Warning,
    Error,
    Info,
    Loading
};

/**
 * @brief Animated status indicator widget
 */
class StatusIndicator : public QWidget {
    Q_OBJECT

public:
    explicit StatusIndicator(QWidget* parent = nullptr);
    
    // Status management
    void setStatus(StatusType type, const QString& message);
    void clearStatus();
    void setAutoHide(bool enabled, int timeout_ms = 3000);
    
    // Appearance
    void setAnimationEnabled(bool enabled);
    void setShowIcon(bool show);
    void setShowMessage(bool show);

signals:
    void statusChanged(StatusType type, const QString& message);
    void statusCleared();

protected:
    void paintEvent(QPaintEvent* event) override;

private slots:
    void onAutoHideTimeout();
    void updateAnimation();

private:
    void setupUI();
    void updateAppearance();
    void startPulseAnimation();
    void stopPulseAnimation();
    QColor getStatusColor(StatusType type) const;
    QString getStatusIcon(StatusType type) const;

    // UI Components
    QHBoxLayout* layout_;
    QLabel* icon_label_;
    QLabel* message_label_;
    
    // State
    StatusType current_status_;
    QString current_message_;
    bool auto_hide_enabled_;
    int auto_hide_timeout_;
    bool animation_enabled_;
    bool show_icon_;
    bool show_message_;
    
    // Animation
    QTimer* auto_hide_timer_;
    QTimer* animation_timer_;
    std::unique_ptr<QPropertyAnimation> pulse_animation_;
    qreal pulse_opacity_;
};
