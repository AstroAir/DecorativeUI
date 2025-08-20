# Animation API Reference

The Animation module provides a comprehensive animation system for DeclarativeUI applications with high-performance features, GPU acceleration support, and flexible configuration options.

## AnimationEngine Class

The central singleton managing all animations in the application.

### Header

```cpp
#include "Animation/AnimationEngine.hpp"
```

### Class Declaration

```cpp
namespace DeclarativeUI::Animation {
    class AnimationEngine : public QObject {
        Q_OBJECT
    public:
        static AnimationEngine& instance();

        // Animation creation methods...
    };
}
```

### Methods

#### Animation Creation

```cpp
std::shared_ptr<Animation> createAnimation();
```

Creates a new animation instance with object pooling optimization.

```cpp
std::shared_ptr<AnimationGroup> createAnimationGroup(AnimationGroup::GroupType type);
```

Creates an animation group for sequential or parallel animation coordination.

#### Convenience Animation Methods

```cpp
std::shared_ptr<Animation> animateProperty(
    QObject* object,
    const QString& property,
    const QVariant& start_value,
    const QVariant& end_value,
    int duration_ms = 1000,
    EasingType easing = EasingType::Linear
);
```

Animates a property of any QObject with automatic interpolation.

```cpp
std::shared_ptr<Animation> fadeIn(QWidget* widget, int duration_ms = 500);
std::shared_ptr<Animation> fadeOut(QWidget* widget, int duration_ms = 500);
```

Convenience methods for opacity animations.

```cpp
std::shared_ptr<Animation> slideIn(QWidget* widget, const QString& direction, int duration_ms = 500);
std::shared_ptr<Animation> slideOut(QWidget* widget, const QString& direction, int duration_ms = 500);
```

Slide animations with directional support ("left", "right", "up", "down").

```cpp
std::shared_ptr<Animation> scaleAnimation(QWidget* widget, double from_scale, double to_scale, int duration_ms = 500);
```

Scale transformation animations.

#### Global Control

```cpp
void pauseAllAnimations();
void resumeAllAnimations();
void stopAllAnimations();
```

Global animation lifecycle control.

#### Performance Optimization

```cpp
void enableGlobalGPUAcceleration(bool enabled);
void setGlobalPlaybackRate(double rate);
void enableAnimationPooling(bool enabled);
void setMaxConcurrentAnimations(int max_count);
```

Performance tuning and optimization settings.

#### Monitoring

```cpp
QJsonObject getPerformanceMetrics() const;
int getActiveAnimationCount() const;
double getAverageFrameRate() const;
```

Performance monitoring and metrics collection.

### Signals

```cpp
void animationStarted(const QString& animation_id);
void animationFinished(const QString& animation_id);
void performanceAlert(const QString& metric, double value);
```

## Animation Class

Individual animation instance with comprehensive configuration options.

### Methods

#### Configuration

```cpp
void setTarget(QObject* object, const QString& property_name);
```

Sets the target object and property for animation.

```cpp
void setValues(const QVariant& start_value, const QVariant& end_value);
```

Defines the animation value range.

```cpp
void setProperties(const AnimationProperties& properties);
```

Applies comprehensive animation configuration.

```cpp
void setCustomSetter(std::function<void(const QVariant&)> setter);
```

Sets a custom function for applying animation values.

#### Timeline Support

```cpp
template<typename T>
void setTimeline(const AnimationTimeline<T>& timeline);
```

Applies keyframe-based timeline animation.

#### Control

```cpp
void start();
void stop();
void pause();
void resume();
void restart();
```

Animation lifecycle control.

#### State Queries

```cpp
AnimationState getState() const;
double getProgress() const;  // 0.0 to 1.0
int getCurrentTime() const;
int getDuration() const;
```

### Signals

```cpp
void started();
void finished();
void paused();
void resumed();
void progressChanged(double progress);
void valueChanged(const QVariant& value);
```

## AnimationGroup Class

Coordinates multiple animations for complex sequences.

### Enums

```cpp
enum class GroupType {
    Sequential,  // Animations play one after another
    Parallel     // Animations play simultaneously
};
```

### Methods

#### Animation Management

```cpp
void addAnimation(std::shared_ptr<Animation> animation);
void removeAnimation(std::shared_ptr<Animation> animation);
void clear();
```

#### Control

```cpp
void start();
void stop();
void pause();
void resume();
```

#### State Queries

```cpp
AnimationState getState() const;
double getProgress() const;
int getDuration() const;
```

### Signals

```cpp
void started();
void finished();
void paused();
void resumed();
void progressChanged(double progress);
```

## Data Structures

### AnimationProperties

```cpp
struct AnimationProperties {
    int duration_ms = 1000;              // Animation duration
    EasingType easing = EasingType::Linear;  // Easing function
    int delay_ms = 0;                    // Start delay
    int repeat_count = 1;                // Repetitions (-1 for infinite)
    bool auto_reverse = false;           // Reverse on repeat
    bool use_gpu_acceleration = false;   // GPU acceleration
    double playback_rate = 1.0;          // Speed multiplier
};
```

### AnimationTarget

```cpp
struct AnimationTarget {
    QObject* object = nullptr;
    QString property_name;
    QVariant start_value;
    QVariant end_value;
    std::function<void(const QVariant&)> custom_setter;

    bool isValid() const;
};
```

### Keyframe Template

```cpp
template<typename T>
struct Keyframe {
    double time_ratio;  // 0.0 to 1.0
    T value;
    EasingType easing = EasingType::Linear;

    Keyframe(double time, const T& val, EasingType ease = EasingType::Linear);
};
```

### AnimationTimeline Template

```cpp
template<typename T>
class AnimationTimeline {
public:
    void addKeyframe(double time, const T& value, EasingType easing = EasingType::Linear);
    void clearKeyframes();
    T interpolate(double progress) const;
    bool isEmpty() const;
    size_t getKeyframeCount() const;
};
```

## Enums

### EasingType

```cpp
enum class EasingType {
    Linear,
    QuadIn, QuadOut, QuadInOut,
    CubicIn, CubicOut, CubicInOut,
    QuartIn, QuartOut, QuartInOut,
    QuintIn, QuintOut, QuintInOut,
    SineIn, SineOut, SineInOut,
    ExpoIn, ExpoOut, ExpoInOut,
    CircIn, CircOut, CircInOut,
    BackIn, BackOut, BackInOut,
    ElasticIn, ElasticOut, ElasticInOut,
    BounceIn, BounceOut, BounceInOut,
    Custom
};
```

### AnimationState

```cpp
enum class AnimationState {
    Stopped,
    Running,
    Paused,
    Finished
};
```

## Usage Examples

### Basic Property Animation

```cpp
auto& engine = AnimationEngine::instance();

auto animation = engine.animateProperty(
    widget, "opacity",
    0.0, 1.0,           // From transparent to opaque
    500,                // 500ms duration
    EasingType::CubicOut
);

animation->start();
```

### Timeline Animation

```cpp
AnimationTimeline<double> timeline;
timeline.addKeyframe(0.0, 0.0, EasingType::Linear);
timeline.addKeyframe(0.3, 0.8, EasingType::CubicOut);
timeline.addKeyframe(1.0, 1.0, EasingType::CubicIn);

auto animation = engine.createAnimation();
animation->setTarget(widget, "opacity");
animation->setTimeline(timeline);
animation->start();
```

### Animation Group

```cpp
auto group = engine.createAnimationGroup(AnimationGroup::Sequential);

group->addAnimation(engine.fadeIn(widget1, 300));
group->addAnimation(engine.slideIn(widget2, "left", 400));
group->addAnimation(engine.scaleAnimation(widget3, 0.5, 1.0, 250));

group->start();  // Animations play in sequence
```

### Custom Animation

```cpp
auto animation = engine.createAnimation();

animation->setCustomSetter([widget](const QVariant& value) {
    double progress = value.toDouble();
    widget->setGeometry(
        static_cast<int>(100 * progress),
        static_cast<int>(50 * progress),
        widget->width(),
        widget->height()
    );
});

animation->setValues(0.0, 1.0);
animation->start();
```

## Performance Optimization

### Object Pooling

The animation system uses object pooling by default to reduce memory allocations:

```cpp
engine.enableAnimationPooling(true);  // Default: enabled
```

### GPU Acceleration

Enable GPU acceleration for supported operations:

```cpp
engine.enableGlobalGPUAcceleration(true);
```

### Concurrent Animation Limits

Control the maximum number of concurrent animations:

```cpp
engine.setMaxConcurrentAnimations(30);  // Default: 50
```

### Performance Monitoring

```cpp
QJsonObject metrics = engine.getPerformanceMetrics();
qDebug() << "Active animations:" << engine.getActiveAnimationCount();
qDebug() << "Average FPS:" << engine.getAverageFrameRate();
```

## Integration with UIElement

The Animation module integrates with the Core UIElement system through the AnimationConfig bridge:

```cpp
// UIElement animation methods use AnimationConfig
element->fadeIn(AnimationConfig{500ms, QEasingCurve::OutCubic});

// AnimationConfig converts to AnimationProperties
AnimationConfig config{750ms, QEasingCurve::InOutQuad};
auto props = config.toAnimationProperties();
```
