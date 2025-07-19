#pragma once

#include <QObject>
#include <QString>
#include <QWidget>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QParallelAnimationGroup>
#include <QEasingCurve>
#include <QTimer>
#include <QJsonObject>
// #include <QOpenGLWidget>  // Commented out - may not be available in this Qt installation

#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <chrono>
#include <atomic>
#include <shared_mutex>

namespace DeclarativeUI::Animation {

// **Easing function types**
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

// **Animation state**
enum class AnimationState {
    Stopped,
    Running,
    Paused,
    Finished
};

// **Keyframe for timeline animations**
template<typename T>
struct Keyframe {
    double time_ratio;  // 0.0 to 1.0
    T value;
    EasingType easing = EasingType::Linear;
    
    Keyframe(double time, const T& val, EasingType ease = EasingType::Linear)
        : time_ratio(time), value(val), easing(ease) {}
};

// **Animation timeline**
template<typename T>
class AnimationTimeline {
public:
    void addKeyframe(double time_ratio, const T& value, EasingType easing = EasingType::Linear) {
        keyframes_.emplace_back(time_ratio, value, easing);
        std::sort(keyframes_.begin(), keyframes_.end(), 
                 [](const Keyframe<T>& a, const Keyframe<T>& b) {
                     return a.time_ratio < b.time_ratio;
                 });
    }

    T interpolate(double time_ratio) const {
        if (keyframes_.empty()) return T{};
        if (keyframes_.size() == 1) return keyframes_[0].value;
        
        // Clamp time ratio
        time_ratio = std::max(0.0, std::min(1.0, time_ratio));
        
        // Find surrounding keyframes
        auto it = std::upper_bound(keyframes_.begin(), keyframes_.end(), time_ratio,
                                  [](double time, const Keyframe<T>& kf) {
                                      return time < kf.time_ratio;
                                  });
        
        if (it == keyframes_.begin()) return keyframes_[0].value;
        if (it == keyframes_.end()) return keyframes_.back().value;
        
        auto next_kf = *it;
        auto prev_kf = *(--it);
        
        // Calculate local time ratio between keyframes
        double local_ratio = (time_ratio - prev_kf.time_ratio) / 
                           (next_kf.time_ratio - prev_kf.time_ratio);
        
        // Apply easing
        double eased_ratio = applyEasing(local_ratio, prev_kf.easing);
        
        // Interpolate between values
        return interpolateValues(prev_kf.value, next_kf.value, eased_ratio);
    }

    const std::vector<Keyframe<T>>& getKeyframes() const { return keyframes_; }
    void clear() { keyframes_.clear(); }
    
    // Made public to allow access from Animation class
    double applyEasing(double t, EasingType easing) const;

private:
    std::vector<Keyframe<T>> keyframes_;
    
    T interpolateValues(const T& from, const T& to, double ratio) const;
};

// **Animation properties**
struct AnimationProperties {
    int duration_ms = 1000;
    EasingType easing = EasingType::Linear;
    int delay_ms = 0;
    int repeat_count = 1;  // -1 for infinite
    bool auto_reverse = false;
    bool use_gpu_acceleration = false;
    double playback_rate = 1.0;
};

// **Animation target**
struct AnimationTarget {
    QObject* object = nullptr;
    QString property_name;
    QVariant start_value;
    QVariant end_value;
    std::function<void(const QVariant&)> custom_setter;
    
    bool isValid() const { return object != nullptr && !property_name.isEmpty(); }
};

// **Advanced animation class**
class Animation : public QObject {
    Q_OBJECT

public:
    explicit Animation(QObject* parent = nullptr);
    ~Animation() override;

    // **Configuration**
    void setTarget(QObject* object, const QString& property_name);
    void setValues(const QVariant& start_value, const QVariant& end_value);
    void setProperties(const AnimationProperties& properties);
    void setCustomSetter(std::function<void(const QVariant&)> setter);

    // **Timeline support**
    template<typename T>
    void setTimeline(const AnimationTimeline<T>& timeline);

    // **Control**
    void start();
    void stop();
    void pause();
    void resume();
    void restart();

    // **State**
    AnimationState getState() const { return state_; }
    double getProgress() const { return progress_; }
    int getCurrentTime() const;
    int getDuration() const { return properties_.duration_ms; }

    // **Advanced features**
    void enableGPUAcceleration(bool enabled);
    void setPlaybackRate(double rate);
    void addProgressCallback(std::function<void(double)> callback);

signals:
    void started();
    void finished();
    void paused();
    void resumed();
    void progressChanged(double progress);
    void valueChanged(const QVariant& value);

private slots:
    void onTimerUpdate();

private:
    AnimationTarget target_;
    AnimationProperties properties_;
    AnimationState state_ = AnimationState::Stopped;
    
    std::unique_ptr<QTimer> timer_;
    std::chrono::steady_clock::time_point start_time_;
    std::chrono::steady_clock::time_point pause_time_;
    std::chrono::milliseconds elapsed_time_{0};
    
    double progress_ = 0.0;
    int current_iteration_ = 0;
    bool reverse_direction_ = false;
    
    std::vector<std::function<void(double)>> progress_callbacks_;
    
    void updateAnimation();
    QVariant interpolateValue(double progress) const;
    double applyEasing(double t) const;
    void applyValue(const QVariant& value);
};

// **Animation group for coordinating multiple animations**
class AnimationGroup : public QObject {
    Q_OBJECT

public:
    enum class GroupType {
        Sequential,  // Animations play one after another
        Parallel     // Animations play simultaneously
    };

    explicit AnimationGroup(GroupType type, QObject* parent = nullptr);
    ~AnimationGroup() override;

    // **Animation management**
    void addAnimation(std::shared_ptr<Animation> animation);
    void removeAnimation(std::shared_ptr<Animation> animation);
    void clear();

    // **Control**
    void start();
    void stop();
    void pause();
    void resume();

    // **State**
    AnimationState getState() const { return state_; }
    double getProgress() const;
    int getDuration() const;

signals:
    void started();
    void finished();
    void paused();
    void resumed();
    void progressChanged(double progress);

private:
    GroupType type_;
    std::vector<std::shared_ptr<Animation>> animations_;
    AnimationState state_ = AnimationState::Stopped;
    int current_animation_index_ = 0;
    
    void onAnimationFinished();
    void updateGroupProgress();
};

// **Animation pool for performance optimization**
class AnimationPool {
public:
    static AnimationPool& instance();

    std::shared_ptr<Animation> acquire();
    void release(std::shared_ptr<Animation> animation);
    
    void setPoolSize(size_t size);
    size_t getAvailableCount() const;
    size_t getAllocatedCount() const;

private:
    std::vector<std::shared_ptr<Animation>> available_animations_;
    std::unordered_set<std::shared_ptr<Animation>> allocated_animations_;
    mutable std::shared_mutex mutex_;
    size_t max_pool_size_ = 100;
    
    void expandPool();
};

// **GPU-accelerated animation renderer**
// Commented out due to missing QOpenGLWidget dependency
/*
class GPUAnimationRenderer : public QOpenGLWidget {
    Q_OBJECT

public:
    explicit GPUAnimationRenderer(QWidget* parent = nullptr);
    ~GPUAnimationRenderer() override;

    void addAnimatedWidget(QWidget* widget);
    void removeAnimatedWidget(QWidget* widget);
    
    void enableHardwareAcceleration(bool enabled);
    void setRenderQuality(int quality);  // 0-100

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;

private:
    std::vector<QWidget*> animated_widgets_;
    bool hardware_acceleration_enabled_ = true;
    int render_quality_ = 80;
    
    void renderWidget(QWidget* widget);
    void applyGPUEffects(QWidget* widget);
};
*/

// **Main animation engine**
class AnimationEngine : public QObject {
    Q_OBJECT

public:
    static AnimationEngine& instance();
    explicit AnimationEngine(QObject* parent = nullptr);
    ~AnimationEngine() override;

    // **Animation creation**
    std::shared_ptr<Animation> createAnimation();
    std::shared_ptr<AnimationGroup> createAnimationGroup(AnimationGroup::GroupType type);
    
    // **Convenience methods**
    std::shared_ptr<Animation> animateProperty(QObject* object, const QString& property,
                                              const QVariant& start_value, const QVariant& end_value,
                                              int duration_ms = 1000, EasingType easing = EasingType::Linear);
    
    std::shared_ptr<Animation> fadeIn(QWidget* widget, int duration_ms = 500);
    std::shared_ptr<Animation> fadeOut(QWidget* widget, int duration_ms = 500);
    std::shared_ptr<Animation> slideIn(QWidget* widget, const QString& direction, int duration_ms = 500);
    std::shared_ptr<Animation> slideOut(QWidget* widget, const QString& direction, int duration_ms = 500);
    std::shared_ptr<Animation> scaleAnimation(QWidget* widget, double from_scale, double to_scale, int duration_ms = 500);

    // **Global control**
    void pauseAllAnimations();
    void resumeAllAnimations();
    void stopAllAnimations();
    
    // **Performance optimization**
    void enableGlobalGPUAcceleration(bool enabled);
    void setGlobalPlaybackRate(double rate);
    void enableAnimationPooling(bool enabled);
    void setMaxConcurrentAnimations(int max_count);

    // **Monitoring**
    QJsonObject getPerformanceMetrics() const;
    int getActiveAnimationCount() const;
    double getAverageFrameRate() const;

    // **Configuration**
    void setDefaultEasing(EasingType easing);
    void setDefaultDuration(int duration_ms);
    void enableVSync(bool enabled);

signals:
    void animationStarted(const QString& animation_id);
    void animationFinished(const QString& animation_id);
    void performanceAlert(const QString& metric, double value);

private slots:
    void onGlobalTimer();
    void onPerformanceCheck();

private:
    std::vector<std::shared_ptr<Animation>> active_animations_;
    std::vector<std::shared_ptr<AnimationGroup>> active_groups_;
    mutable std::shared_mutex animations_mutex_;
    
    // std::unique_ptr<GPUAnimationRenderer> gpu_renderer_;  // Commented out due to missing QOpenGLWidget
    std::unique_ptr<QTimer> global_timer_;
    std::unique_ptr<QTimer> performance_timer_;
    
    // **Configuration**
    std::atomic<bool> global_gpu_acceleration_{false};
    std::atomic<double> global_playback_rate_{1.0};
    std::atomic<bool> animation_pooling_enabled_{true};
    std::atomic<int> max_concurrent_animations_{50};
    EasingType default_easing_ = EasingType::Linear;
    int default_duration_ms_ = 1000;
    bool vsync_enabled_ = true;
    
    // **Performance tracking**
    std::atomic<size_t> total_animations_created_{0};
    std::atomic<size_t> total_animations_completed_{0};
    std::atomic<double> total_frame_time_{0.0};
    std::atomic<size_t> frame_count_{0};
    
    void registerAnimation(std::shared_ptr<Animation> animation);
    void unregisterAnimation(std::shared_ptr<Animation> animation);
    void updatePerformanceMetrics();
    void checkPerformanceAlerts();
    void optimizeAnimations();
};

}  // namespace DeclarativeUI::Animation
