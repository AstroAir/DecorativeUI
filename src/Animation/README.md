# Animation Module

The Animation module provides a comprehensive, high-performance animation system for DeclarativeUI applications. It offers modern C++20 features, GPU acceleration support, and a flexible API for creating smooth, responsive animations.

## Overview

This directory contains the core animation infrastructure:

- **AnimationEngine**: Central animation management and coordination
- **Animation**: Individual animation instances with advanced features
- **AnimationGroup**: Coordinated animation sequences and parallel execution
- **AnimationPool**: Performance optimization through object pooling
- **Timeline Support**: Keyframe-based animation timelines

## Key Features

### 🚀 **High Performance**
- Object pooling for reduced memory allocation
- GPU acceleration support (when available)
- Optimized easing function implementations
- Concurrent animation processing

### 🎯 **Flexible Animation Types**
- Property-based animations
- Timeline-based keyframe animations
- Custom setter function support
- Sequential and parallel animation groups

### ⚙️ **Advanced Configuration**
- 20+ built-in easing functions
- Configurable duration, delay, and repeat
- Auto-reverse and infinite loop support
- Variable playback rates

### 📊 **Performance Monitoring**
- Real-time performance metrics
- Frame rate monitoring
- Memory usage tracking
- Bottleneck detection

## Components

### AnimationEngine (`AnimationEngine.hpp/.cpp`)

Central singleton managing all animations in the application.

**Key Features:**
- Animation creation and lifecycle management
- Global performance optimization settings
- Convenience methods for common animations
- Performance monitoring and metrics

**Core Methods:**
```cpp
// Animation creation
std::shared_ptr<Animation> createAnimation();
std::shared_ptr<AnimationGroup> createAnimationGroup(GroupType type);

// Convenience animations
std::shared_ptr<Animation> fadeIn(QWidget* widget, int duration_ms = 500);
std::shared_ptr<Animation> fadeOut(QWidget* widget, int duration_ms = 500);
std::shared_ptr<Animation> slideIn(QWidget* widget, const QString& direction, int duration_ms = 500);

// Global control
void pauseAllAnimations();
void resumeAllAnimations();
void stopAllAnimations();

// Performance optimization
void enableGlobalGPUAcceleration(bool enabled);
void setGlobalPlaybackRate(double rate);
void setMaxConcurrentAnimations(int max_count);
```

### Animation (`AnimationEngine.hpp/.cpp`)

Individual animation instance with comprehensive configuration options.

**Key Features:**
- Property-based animation with automatic interpolation
- Timeline support for complex keyframe animations
- Custom setter functions for non-property animations
- Repeat, auto-reverse, and delay support
- Progress callbacks and event signals

**Animation States:**
- `Stopped`: Animation is not running
- `Running`: Animation is actively playing
- `Paused`: Animation is temporarily suspended
- `Finished`: Animation has completed

**Core Methods:**
```cpp
// Configuration
void setTarget(QObject* object, const QString& property_name);
void setValues(const QVariant& start_value, const QVariant& end_value);
void setProperties(const AnimationProperties& properties);

// Control
void start();
void stop();
void pause();
void resume();

// State queries
AnimationState getState() const;
double getProgress() const;
int getDuration() const;
```

### AnimationGroup (`AnimationEngine.hpp/.cpp`)

Coordinates multiple animations for complex sequences.

**Group Types:**
- **Sequential**: Animations play one after another
- **Parallel**: Animations play simultaneously

**Key Features:**
- Automatic progress calculation across all animations
- Group-level control (start, stop, pause, resume)
- Event signals for group lifecycle
- Support for nested groups

### AnimationProperties Structure

Comprehensive configuration for animation behavior:

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

### Easing Functions

20+ built-in easing functions for natural motion:

- **Linear**: Constant speed
- **Quad**: Quadratic acceleration/deceleration
- **Cubic**: Cubic curves for smooth motion
- **Quart/Quint**: Higher-order polynomial curves
- **Sine**: Sinusoidal motion
- **Expo**: Exponential acceleration
- **Circ**: Circular motion curves
- **Back**: Overshoot effects
- **Elastic**: Spring-like motion
- **Bounce**: Bouncing effects

## Usage Examples

### Basic Property Animation

```cpp
#include "Animation/AnimationEngine.hpp"

auto& engine = AnimationEngine::instance();

// Animate widget opacity
auto animation = engine.animateProperty(
    widget, "opacity", 
    0.0, 1.0,           // From 0% to 100% opacity
    500,                // 500ms duration
    EasingType::CubicOut
);

animation->start();
```

### Timeline-Based Animation

```cpp
// Create keyframe timeline
AnimationTimeline<double> timeline;
timeline.addKeyframe(0.0, 0.0, EasingType::Linear);      // Start at 0
timeline.addKeyframe(0.3, 0.8, EasingType::CubicOut);    // Quick rise
timeline.addKeyframe(0.7, 0.9, EasingType::Linear);      // Plateau
timeline.addKeyframe(1.0, 1.0, EasingType::CubicIn);     // Final approach

auto animation = engine.createAnimation();
animation->setTarget(widget, "opacity");
animation->setTimeline(timeline);
animation->start();
```

### Animation Groups

```cpp
// Sequential animation chain
auto group = engine.createAnimationGroup(AnimationGroup::Sequential);

auto fadeIn = engine.fadeIn(widget1, 300);
auto slideIn = engine.slideIn(widget2, "left", 400);
auto scaleUp = engine.scaleAnimation(widget3, 0.5, 1.0, 250);

group->addAnimation(fadeIn);
group->addAnimation(slideIn);
group->addAnimation(scaleUp);

group->start();  // Animations play in sequence
```

### Custom Animation with Setter

```cpp
auto animation = engine.createAnimation();

// Custom animation for non-property values
animation->setCustomSetter([widget](const QVariant& value) {
    double progress = value.toDouble();
    // Custom logic for complex animations
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

### Performance Optimization

```cpp
// Enable global optimizations
engine.enableGlobalGPUAcceleration(true);
engine.setMaxConcurrentAnimations(30);
engine.enableAnimationPooling(true);

// Monitor performance
QJsonObject metrics = engine.getPerformanceMetrics();
qDebug() << "Active animations:" << engine.getActiveAnimationCount();
qDebug() << "Average FPS:" << engine.getAverageFrameRate();
```

## Integration with UIElement

The Animation module integrates seamlessly with the Core UIElement system:

```cpp
// UIElement provides animation convenience methods
auto element = std::make_unique<UIElement>();
element->fadeIn(AnimationConfig{500ms, QEasingCurve::OutCubic});
element->slideOut("right", AnimationConfig{300ms});

// AnimationConfig bridges to Animation system
AnimationConfig config{750ms, QEasingCurve::InOutQuad};
auto props = config.toAnimationProperties();  // Converts to AnimationProperties
```

## Dependencies

- **Qt6**: Core, Widgets modules
- **C++20**: Modern language features
- **Internal**: Core module for UIElement integration

## Performance Considerations

- **Object Pooling**: Enabled by default to reduce allocations
- **Concurrent Limits**: Default maximum of 50 concurrent animations
- **GPU Acceleration**: Available when OpenGL context is present
- **Memory Management**: Automatic cleanup of finished animations

## Testing

Comprehensive unit tests available in `tests/unit/`:
- Animation lifecycle and state management
- Easing function accuracy
- Performance benchmarks
- Memory leak detection

```bash
cd build
ctest -R Animation
```

## Future Enhancements

- **Physics-based animations**: Spring and damping systems
- **Gesture-driven animations**: Touch and mouse interaction
- **3D transformations**: When OpenGL integration is complete
- **Animation scripting**: JSON-based animation definitions
