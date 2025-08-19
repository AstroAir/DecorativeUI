// Components/ProgressBar.hpp
#pragma once
#include <QGraphicsDropShadowEffect>
#include <QLabel>
#include <QProgressBar>
#include <QPropertyAnimation>
#include <QTimer>
#include <functional>
#include <memory>

#include "../Core/UIElement.hpp"

namespace DeclarativeUI::Components {

/**
 * @brief Progress bar state for enhanced functionality
 */
enum class ProgressState {
    Normal,
    Warning,
    Error,
    Success,
    Paused,
    Indeterminate
};

class ProgressBar : public Core::UIElement {
    Q_OBJECT

public:
    explicit ProgressBar(QObject* parent = nullptr);

    // **Basic fluent interface (backward compatible)**
    ProgressBar& minimum(int min);
    ProgressBar& maximum(int max);
    ProgressBar& value(int value);
    ProgressBar& orientation(Qt::Orientation orientation);
    ProgressBar& textVisible(bool visible);
    ProgressBar& format(const QString& format);
    ProgressBar& invertedAppearance(bool inverted);
    ProgressBar& style(const QString& stylesheet);

    // **Enhanced fluent interface**
    ProgressBar& tooltip(const QString& tooltip_text);
    ProgressBar& accessibleName(const QString& name);
    ProgressBar& accessibleDescription(const QString& description);

    // **Visual enhancements**
    ProgressBar& dropShadow(bool enabled = true,
                            const QColor& color = QColor(0, 0, 0, 80));
    ProgressBar& animation(bool enabled = true);
    ProgressBar& smoothProgress(bool enabled = true);
    ProgressBar& borderRadius(int radius);
    ProgressBar& customColors(const QColor& fill,
                              const QColor& background = QColor(),
                              const QColor& text = QColor());
    ProgressBar& gradient(const QColor& start, const QColor& end);
    ProgressBar& stripes(bool enabled = true, bool animated = true);
    ProgressBar& glow(bool enabled = true,
                      const QColor& color = QColor(255, 255, 255, 100));

    // **State management**
    ProgressBar& state(ProgressState progress_state);
    ProgressBar& indeterminate(bool enabled = true);
    ProgressBar& paused(bool paused = true);
    ProgressBar& showPercentage(bool enabled = true);
    ProgressBar& showTimeRemaining(bool enabled = true);
    ProgressBar& showSpeed(bool enabled = true);
    ProgressBar& customText(const QString& text);

    // **Animation and timing**
    ProgressBar& animationDuration(int milliseconds);
    ProgressBar& updateInterval(int milliseconds);
    ProgressBar& easing(QEasingCurve::Type easing_type);
    ProgressBar& pulseEffect(bool enabled = true);
    ProgressBar& bounceEffect(bool enabled = true);

    // **Advanced features**
    ProgressBar& segments(int segment_count);
    ProgressBar& segmentSpacing(int spacing);
    ProgressBar& multiProgress(const QList<int>& values,
                               const QList<QColor>& colors = {});
    ProgressBar& threshold(int value, const QColor& color = QColor(255, 0, 0));
    ProgressBar& milestones(const QList<int>& milestone_values);

    // **Event handlers**
    ProgressBar& onValueChanged(std::function<void(int)> handler);
    ProgressBar& onStateChanged(std::function<void(ProgressState)> handler);
    ProgressBar& onCompleted(std::function<void()> handler);
    ProgressBar& onThresholdReached(std::function<void(int)> handler);
    ProgressBar& onMilestoneReached(std::function<void(int)> handler);

    // **Accessibility**
    ProgressBar& role(const QString& aria_role);
    ProgressBar& describedBy(const QString& element_id);
    ProgressBar& labelledBy(const QString& element_id);

    // **Data and timing**
    ProgressBar& estimatedTime(int total_seconds);
    ProgressBar& startTime(const QDateTime& start_time);
    ProgressBar& dataSize(qint64 total_bytes);
    ProgressBar& speed(qint64 bytes_per_second);

    void initialize() override;
    int getValue() const;
    void setValue(int value);
    int getMinimum() const;
    int getMaximum() const;
    void reset();

private:
    QProgressBar* progress_bar_widget_;
};

}  // namespace DeclarativeUI::Components
