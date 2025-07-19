#include "AnimationEngine.hpp"

#include <QDebug>
#include <QJsonArray>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <algorithm>
#include <cmath>
#include <mutex>

namespace DeclarativeUI::Animation {

// **Easing function implementations**
template<typename T>
double AnimationTimeline<T>::applyEasing(double t, EasingType easing) const {
    switch (easing) {
        case EasingType::Linear:
            return t;
        case EasingType::QuadIn:
            return t * t;
        case EasingType::QuadOut:
            return 1.0 - (1.0 - t) * (1.0 - t);
        case EasingType::QuadInOut:
            return t < 0.5 ? 2.0 * t * t : 1.0 - 2.0 * (1.0 - t) * (1.0 - t);
        case EasingType::CubicIn:
            return t * t * t;
        case EasingType::CubicOut:
            return 1.0 - std::pow(1.0 - t, 3.0);
        case EasingType::CubicInOut:
            return t < 0.5 ? 4.0 * t * t * t : 1.0 - 4.0 * std::pow(1.0 - t, 3.0);
        case EasingType::SineIn:
            return 1.0 - std::cos(t * M_PI / 2.0);
        case EasingType::SineOut:
            return std::sin(t * M_PI / 2.0);
        case EasingType::SineInOut:
            return -(std::cos(M_PI * t) - 1.0) / 2.0;
        case EasingType::BounceOut: {
            const double n1 = 7.5625;
            const double d1 = 2.75;
            if (t < 1.0 / d1) {
                return n1 * t * t;
            } else if (t < 2.0 / d1) {
                t -= 1.5 / d1;
                return n1 * t * t + 0.75;
            } else if (t < 2.5 / d1) {
                t -= 2.25 / d1;
                return n1 * t * t + 0.9375;
            } else {
                t -= 2.625 / d1;
                return n1 * t * t + 0.984375;
            }
        }
        default:
            return t;
    }
}

template<typename T>
T AnimationTimeline<T>::interpolateValues(const T& from, const T& to, double ratio) const {
    // Default linear interpolation - specializations can be provided for specific types
    if constexpr (std::is_arithmetic_v<T>) {
        return static_cast<T>(from + (to - from) * ratio);
    } else {
        // For non-arithmetic types, return the target value when ratio >= 0.5
        return ratio >= 0.5 ? to : from;
    }
}

// **Animation implementation**
Animation::Animation(QObject* parent) : QObject(parent) {
    timer_ = std::make_unique<QTimer>(this);
    timer_->setInterval(16);  // ~60 FPS
    connect(timer_.get(), &QTimer::timeout, this, &Animation::onTimerUpdate);
}

Animation::~Animation() {
    stop();
}

void Animation::setTarget(QObject* object, const QString& property_name) {
    target_.object = object;
    target_.property_name = property_name;
}

void Animation::setValues(const QVariant& start_value, const QVariant& end_value) {
    target_.start_value = start_value;
    target_.end_value = end_value;
}

void Animation::setProperties(const AnimationProperties& properties) {
    properties_ = properties;
}

void Animation::setCustomSetter(std::function<void(const QVariant&)> setter) {
    target_.custom_setter = std::move(setter);
}

void Animation::start() {
    if (state_ == AnimationState::Running) return;
    if (!target_.isValid()) {
        qWarning() << "🔥 Cannot start animation: invalid target";
        return;
    }

    state_ = AnimationState::Running;
    progress_ = 0.0;
    current_iteration_ = 0;
    reverse_direction_ = false;
    
    start_time_ = std::chrono::steady_clock::now();
    elapsed_time_ = std::chrono::milliseconds(0);
    
    timer_->start();
    emit started();
    
    qDebug() << "🔥 Animation started for" << target_.property_name;
}

void Animation::stop() {
    if (state_ == AnimationState::Stopped) return;
    
    timer_->stop();
    state_ = AnimationState::Stopped;
    progress_ = 0.0;
    
    emit finished();
    qDebug() << "🔥 Animation stopped";
}

void Animation::pause() {
    if (state_ != AnimationState::Running) return;
    
    timer_->stop();
    state_ = AnimationState::Paused;
    pause_time_ = std::chrono::steady_clock::now();
    
    emit paused();
    qDebug() << "🔥 Animation paused";
}

void Animation::resume() {
    if (state_ != AnimationState::Paused) return;
    
    // Adjust start time to account for pause duration
    auto pause_duration = std::chrono::steady_clock::now() - pause_time_;
    start_time_ += pause_duration;
    
    state_ = AnimationState::Running;
    timer_->start();
    
    emit resumed();
    qDebug() << "🔥 Animation resumed";
}

void Animation::restart() {
    stop();
    start();
}

int Animation::getCurrentTime() const {
    if (state_ == AnimationState::Stopped) return 0;
    
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time_);
    return static_cast<int>(elapsed.count());
}

void Animation::enableGPUAcceleration(bool enabled) {
    properties_.use_gpu_acceleration = enabled;
}

void Animation::setPlaybackRate(double rate) {
    properties_.playback_rate = std::max(0.1, std::min(10.0, rate));
}

void Animation::addProgressCallback(std::function<void(double)> callback) {
    progress_callbacks_.push_back(std::move(callback));
}

void Animation::onTimerUpdate() {
    updateAnimation();
}

void Animation::updateAnimation() {
    auto now = std::chrono::steady_clock::now();
    auto total_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time_);
    
    // Apply playback rate
    auto adjusted_elapsed = std::chrono::milliseconds(
        static_cast<int>(total_elapsed.count() * properties_.playback_rate));
    
    // Account for delay
    if (adjusted_elapsed.count() < properties_.delay_ms) {
        return;
    }
    
    auto animation_elapsed = adjusted_elapsed - std::chrono::milliseconds(properties_.delay_ms);
    double raw_progress = static_cast<double>(animation_elapsed.count()) / properties_.duration_ms;
    
    // Handle repetition and auto-reverse
    if (raw_progress >= 1.0) {
        if (properties_.repeat_count == -1 || current_iteration_ < properties_.repeat_count - 1) {
            current_iteration_++;
            start_time_ = now - std::chrono::milliseconds(properties_.delay_ms);
            
            if (properties_.auto_reverse) {
                reverse_direction_ = !reverse_direction_;
            }
            
            raw_progress = 0.0;
        } else {
            raw_progress = 1.0;
            timer_->stop();
            state_ = AnimationState::Finished;
        }
    }
    
    // Apply direction
    progress_ = reverse_direction_ ? 1.0 - raw_progress : raw_progress;
    
    // Apply easing and interpolate value
    double eased_progress = applyEasing(progress_);
    QVariant current_value = interpolateValue(eased_progress);
    
    // Apply the value
    applyValue(current_value);
    
    // Notify callbacks
    for (const auto& callback : progress_callbacks_) {
        callback(progress_);
    }
    
    emit progressChanged(progress_);
    emit valueChanged(current_value);
    
    // Check if animation finished
    if (state_ == AnimationState::Finished) {
        emit finished();
    }
}

QVariant Animation::interpolateValue(double progress) const {
    const QVariant& start = target_.start_value;
    const QVariant& end = target_.end_value;
    
    // Handle different QVariant types
    if (start.type() == QVariant::Double || start.type() == QVariant::Int) {
        double start_val = start.toDouble();
        double end_val = end.toDouble();
        double result = start_val + (end_val - start_val) * progress;
        return start.type() == QVariant::Int ? QVariant(static_cast<int>(result)) : QVariant(result);
    }
    
    if (start.type() == QVariant::Color) {
        QColor start_color = start.value<QColor>();
        QColor end_color = end.value<QColor>();
        
        int r = start_color.red() + (end_color.red() - start_color.red()) * progress;
        int g = start_color.green() + (end_color.green() - start_color.green()) * progress;
        int b = start_color.blue() + (end_color.blue() - start_color.blue()) * progress;
        int a = start_color.alpha() + (end_color.alpha() - start_color.alpha()) * progress;
        
        return QVariant(QColor(r, g, b, a));
    }
    
    // For other types, use discrete transition at 50%
    return progress >= 0.5 ? end : start;
}

double Animation::applyEasing(double t) const {
    // Use the same easing implementation as AnimationTimeline
    AnimationTimeline<double> timeline;
    return timeline.applyEasing(t, properties_.easing);
}

void Animation::applyValue(const QVariant& value) {
    if (target_.custom_setter) {
        target_.custom_setter(value);
    } else if (target_.object && !target_.property_name.isEmpty()) {
        target_.object->setProperty(target_.property_name.toUtf8().constData(), value);
    }
}

// **AnimationPool implementation**
AnimationPool& AnimationPool::instance() {
    static AnimationPool instance;
    return instance;
}

std::shared_ptr<Animation> AnimationPool::acquire() {
    std::unique_lock<std::shared_mutex> lock(mutex_);
    
    if (available_animations_.empty()) {
        expandPool();
    }
    
    if (!available_animations_.empty()) {
        auto animation = available_animations_.back();
        available_animations_.pop_back();
        allocated_animations_.insert(animation);
        return animation;
    }
    
    // Pool exhausted, create new animation
    auto animation = std::make_shared<Animation>();
    allocated_animations_.insert(animation);
    return animation;
}

void AnimationPool::release(std::shared_ptr<Animation> animation) {
    if (!animation) return;
    
    std::unique_lock<std::shared_mutex> lock(mutex_);
    
    auto it = allocated_animations_.find(animation);
    if (it != allocated_animations_.end()) {
        allocated_animations_.erase(it);
        
        // Reset animation state
        animation->stop();
        
        if (available_animations_.size() < max_pool_size_) {
            available_animations_.push_back(animation);
        }
        // If pool is full, let the animation be destroyed
    }
}

void AnimationPool::setPoolSize(size_t size) {
    std::unique_lock<std::shared_mutex> lock(mutex_);
    max_pool_size_ = size;
    
    // Trim pool if necessary
    while (available_animations_.size() > max_pool_size_) {
        available_animations_.pop_back();
    }
}

size_t AnimationPool::getAvailableCount() const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return available_animations_.size();
}

size_t AnimationPool::getAllocatedCount() const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return allocated_animations_.size();
}

void AnimationPool::expandPool() {
    size_t expand_count = std::min(static_cast<size_t>(10), max_pool_size_ - available_animations_.size());
    
    for (size_t i = 0; i < expand_count; ++i) {
        available_animations_.push_back(std::make_shared<Animation>());
    }
}

// **AnimationEngine implementation**
AnimationEngine& AnimationEngine::instance() {
    static AnimationEngine instance;
    return instance;
}

AnimationEngine::AnimationEngine(QObject* parent) : QObject(parent) {
    // Setup global timer for animation updates
    global_timer_ = std::make_unique<QTimer>(this);
    global_timer_->setInterval(16);  // ~60 FPS
    connect(global_timer_.get(), &QTimer::timeout, this, &AnimationEngine::onGlobalTimer);
    global_timer_->start();
    
    // Setup performance monitoring
    performance_timer_ = std::make_unique<QTimer>(this);
    performance_timer_->setInterval(5000);  // Check every 5 seconds
    connect(performance_timer_.get(), &QTimer::timeout, this, &AnimationEngine::onPerformanceCheck);
    performance_timer_->start();
    
    qDebug() << "🔥 Animation Engine initialized";
}

AnimationEngine::~AnimationEngine() {
    stopAllAnimations();
    qDebug() << "🔥 Animation Engine destroyed";
}

std::shared_ptr<Animation> AnimationEngine::createAnimation() {
    std::shared_ptr<Animation> animation;
    
    if (animation_pooling_enabled_.load()) {
        animation = AnimationPool::instance().acquire();
    } else {
        animation = std::make_shared<Animation>();
    }
    
    registerAnimation(animation);
    total_animations_created_.fetch_add(1);
    
    return animation;
}

std::shared_ptr<Animation> AnimationEngine::animateProperty(QObject* object, const QString& property,
                                                           const QVariant& start_value, const QVariant& end_value,
                                                           int duration_ms, EasingType easing) {
    auto animation = createAnimation();
    animation->setTarget(object, property);
    animation->setValues(start_value, end_value);
    
    AnimationProperties props;
    props.duration_ms = duration_ms;
    props.easing = easing;
    props.use_gpu_acceleration = global_gpu_acceleration_.load();
    props.playback_rate = global_playback_rate_.load();
    
    animation->setProperties(props);
    return animation;
}

std::shared_ptr<Animation> AnimationEngine::fadeIn(QWidget* widget, int duration_ms) {
    if (!widget) return nullptr;
    
    // Ensure widget has an opacity effect
    auto effect = qobject_cast<QGraphicsOpacityEffect*>(widget->graphicsEffect());
    if (!effect) {
        effect = new QGraphicsOpacityEffect(widget);
        widget->setGraphicsEffect(effect);
    }
    
    return animateProperty(effect, "opacity", 0.0, 1.0, duration_ms, EasingType::QuadOut);
}

std::shared_ptr<Animation> AnimationEngine::fadeOut(QWidget* widget, int duration_ms) {
    if (!widget) return nullptr;
    
    auto effect = qobject_cast<QGraphicsOpacityEffect*>(widget->graphicsEffect());
    if (!effect) {
        effect = new QGraphicsOpacityEffect(widget);
        widget->setGraphicsEffect(effect);
    }
    
    return animateProperty(effect, "opacity", 1.0, 0.0, duration_ms, EasingType::QuadIn);
}

void AnimationEngine::pauseAllAnimations() {
    std::shared_lock<std::shared_mutex> lock(animations_mutex_);
    
    for (auto& animation : active_animations_) {
        if (animation && animation->getState() == AnimationState::Running) {
            animation->pause();
        }
    }
    
    qDebug() << "🔥 All animations paused";
}

void AnimationEngine::resumeAllAnimations() {
    std::shared_lock<std::shared_mutex> lock(animations_mutex_);
    
    for (auto& animation : active_animations_) {
        if (animation && animation->getState() == AnimationState::Paused) {
            animation->resume();
        }
    }
    
    qDebug() << "🔥 All animations resumed";
}

void AnimationEngine::stopAllAnimations() {
    std::unique_lock<std::shared_mutex> lock(animations_mutex_);
    
    for (auto& animation : active_animations_) {
        if (animation) {
            animation->stop();
        }
    }
    
    active_animations_.clear();
    qDebug() << "🔥 All animations stopped";
}

int AnimationEngine::getActiveAnimationCount() const {
    std::shared_lock<std::shared_mutex> lock(animations_mutex_);
    return static_cast<int>(active_animations_.size());
}

QJsonObject AnimationEngine::getPerformanceMetrics() const {
    QJsonObject metrics;
    
    metrics["total_animations_created"] = static_cast<qint64>(total_animations_created_.load());
    metrics["total_animations_completed"] = static_cast<qint64>(total_animations_completed_.load());
    metrics["active_animation_count"] = getActiveAnimationCount();
    metrics["average_frame_rate"] = getAverageFrameRate();
    metrics["global_gpu_acceleration"] = global_gpu_acceleration_.load();
    metrics["global_playback_rate"] = global_playback_rate_.load();
    metrics["animation_pooling_enabled"] = animation_pooling_enabled_.load();
    
    if (animation_pooling_enabled_.load()) {
        metrics["pool_available_count"] = static_cast<qint64>(AnimationPool::instance().getAvailableCount());
        metrics["pool_allocated_count"] = static_cast<qint64>(AnimationPool::instance().getAllocatedCount());
    }
    
    return metrics;
}

double AnimationEngine::getAverageFrameRate() const {
    size_t frame_count = frame_count_.load();
    return frame_count > 0 ? 1000.0 / (total_frame_time_.load() / frame_count) : 0.0;
}

void AnimationEngine::enableGlobalGPUAcceleration(bool enabled) {
    global_gpu_acceleration_.store(enabled);
    qDebug() << "🔥 Global GPU acceleration" << (enabled ? "enabled" : "disabled");
}

void AnimationEngine::setGlobalPlaybackRate(double rate) {
    global_playback_rate_.store(std::max(0.1, std::min(10.0, rate)));
    qDebug() << "🔥 Global playback rate set to" << rate;
}

void AnimationEngine::enableAnimationPooling(bool enabled) {
    animation_pooling_enabled_.store(enabled);
    qDebug() << "🔥 Animation pooling" << (enabled ? "enabled" : "disabled");
}

void AnimationEngine::onGlobalTimer() {
    auto start_time = std::chrono::steady_clock::now();
    
    // Update performance metrics
    updatePerformanceMetrics();
    
    auto end_time = std::chrono::steady_clock::now();
    auto frame_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    
    total_frame_time_.fetch_add(frame_time.count() / 1000.0);  // Convert to milliseconds
    frame_count_.fetch_add(1);
}

void AnimationEngine::onPerformanceCheck() {
    checkPerformanceAlerts();
    optimizeAnimations();
}

void AnimationEngine::registerAnimation(std::shared_ptr<Animation> animation) {
    std::unique_lock<std::shared_mutex> lock(animations_mutex_);
    active_animations_.push_back(animation);
    
    // Connect to finished signal to clean up
    connect(animation.get(), &Animation::finished, this, [this, animation]() {
        unregisterAnimation(animation);
        total_animations_completed_.fetch_add(1);
    });
}

void AnimationEngine::unregisterAnimation(std::shared_ptr<Animation> animation) {
    std::unique_lock<std::shared_mutex> lock(animations_mutex_);
    
    auto it = std::find(active_animations_.begin(), active_animations_.end(), animation);
    if (it != active_animations_.end()) {
        active_animations_.erase(it);
    }
    
    // Return to pool if pooling is enabled
    if (animation_pooling_enabled_.load()) {
        AnimationPool::instance().release(animation);
    }
}

void AnimationEngine::updatePerformanceMetrics() {
    // Clean up finished animations
    std::unique_lock<std::shared_mutex> lock(animations_mutex_);
    
    active_animations_.erase(
        std::remove_if(active_animations_.begin(), active_animations_.end(),
                      [](const std::weak_ptr<Animation>& weak_anim) {
                          auto anim = weak_anim.lock();
                          return !anim || anim->getState() == AnimationState::Finished;
                      }),
        active_animations_.end());
}

void AnimationEngine::checkPerformanceAlerts() {
    double frame_rate = getAverageFrameRate();
    if (frame_rate < 30.0 && frame_rate > 0.0) {
        emit performanceAlert("frame_rate", frame_rate);
    }
    
    int active_count = getActiveAnimationCount();
    if (active_count > max_concurrent_animations_.load()) {
        emit performanceAlert("active_animation_count", static_cast<double>(active_count));
    }
}

void AnimationEngine::optimizeAnimations() {
    // Simple optimization: limit concurrent animations
    int active_count = getActiveAnimationCount();
    int max_count = max_concurrent_animations_.load();
    
    if (active_count > max_count) {
        qDebug() << "🔥 Too many active animations (" << active_count << "), optimizing...";
        
        // In a real implementation, we might pause low-priority animations
        // or reduce animation quality
    }
}

// **AnimationGroup implementation**
AnimationGroup::~AnimationGroup() {
    qDebug() << "AnimationGroup destroyed";
}

// **Template instantiations**
template class AnimationTimeline<double>;
template class AnimationTimeline<int>;
template class AnimationTimeline<QColor>;

}  // namespace DeclarativeUI::Animation

#include "AnimationEngine.moc"
