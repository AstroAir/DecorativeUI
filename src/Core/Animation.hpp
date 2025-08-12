// Core/Animation.hpp
#pragma once

#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QParallelAnimationGroup>
#include <QEasingCurve>
#include <QVariant>
#include <QTimer>
#include <QPauseAnimation>
#include <chrono>
#include <functional>
#include <memory>
#include <vector>
#include <concepts>
#include <type_traits>

#include "Concepts.hpp"
#include "SmartPointers.hpp"

namespace DeclarativeUI::Core::Animation {

using namespace std::chrono_literals;

// **Animation configuration with designated initializers**
struct AnimationConfig {
    std::chrono::milliseconds duration{300ms};
    QEasingCurve::Type easing{QEasingCurve::OutCubic};
    std::chrono::milliseconds delay{0ms};
    int loop_count{1};
    bool auto_reverse{false};
    std::function<void()> on_finished{};
    std::function<void()> on_started{};
};

// **Easing curve presets**
namespace Easing {
    constexpr auto Linear = QEasingCurve::Linear;
    constexpr auto InQuad = QEasingCurve::InQuad;
    constexpr auto OutQuad = QEasingCurve::OutQuad;
    constexpr auto InOutQuad = QEasingCurve::InOutQuad;
    constexpr auto InCubic = QEasingCurve::InCubic;
    constexpr auto OutCubic = QEasingCurve::OutCubic;
    constexpr auto InOutCubic = QEasingCurve::InOutCubic;
    constexpr auto InQuart = QEasingCurve::InQuart;
    constexpr auto OutQuart = QEasingCurve::OutQuart;
    constexpr auto InOutQuart = QEasingCurve::InOutQuart;
    constexpr auto InBounce = QEasingCurve::InBounce;
    constexpr auto OutBounce = QEasingCurve::OutBounce;
    constexpr auto InOutBounce = QEasingCurve::InOutBounce;
    constexpr auto InElastic = QEasingCurve::InElastic;
    constexpr auto OutElastic = QEasingCurve::OutElastic;
    constexpr auto InOutElastic = QEasingCurve::InOutElastic;
}

// **Modern C++20 Animation Builder**
template<Concepts::QtObject T>
class AnimationBuilder {
public:
    explicit AnimationBuilder(T* target) : target_(target) {}

    // **Property animation with perfect forwarding**
    template<typename PropertyType>
    requires Concepts::AnimatableProperty<PropertyType>
    AnimationBuilder& animate(const char* property, PropertyType&& from, PropertyType&& to) {
        auto animation = std::make_unique<QPropertyAnimation>(target_, property);
        animation->setStartValue(QVariant::fromValue(std::forward<PropertyType>(from)));
        animation->setEndValue(QVariant::fromValue(std::forward<PropertyType>(to)));
        animations_.push_back(std::move(animation));
        return *this;
    }

    // **Duration with chrono literals**
    template<typename Rep, typename Period>
    AnimationBuilder& duration(std::chrono::duration<Rep, Period> dur) {
        config_.duration = std::chrono::duration_cast<std::chrono::milliseconds>(dur);
        return *this;
    }

    // **Easing curve**
    AnimationBuilder& easing(QEasingCurve::Type curve) {
        config_.easing = curve;
        return *this;
    }

    // **Delay with chrono literals**
    template<typename Rep, typename Period>
    AnimationBuilder& delay(std::chrono::duration<Rep, Period> del) {
        config_.delay = std::chrono::duration_cast<std::chrono::milliseconds>(del);
        return *this;
    }

    // **Loop configuration**
    AnimationBuilder& loop(int count = -1) {
        config_.loop_count = count;
        return *this;
    }

    AnimationBuilder& autoReverse(bool reverse = true) {
        config_.auto_reverse = reverse;
        return *this;
    }

    // **Event handlers with concepts**
    template<typename F>
    requires Concepts::VoidCallback<F>
    AnimationBuilder& onFinished(F&& callback) {
        config_.on_finished = std::forward<F>(callback);
        return *this;
    }

    template<typename F>
    requires Concepts::VoidCallback<F>
    AnimationBuilder& onStarted(F&& callback) {
        config_.on_started = std::forward<F>(callback);
        return *this;
    }

    // **Build and start animation**
    [[nodiscard]] qt_unique_ptr<QAbstractAnimation> build() {
        if (animations_.empty()) {
            return nullptr;
        }

        qt_unique_ptr<QAbstractAnimation> result;

        if (animations_.size() == 1) {
            result = qt_unique_ptr<QAbstractAnimation>(animations_[0].release());
        } else {
            auto group = std::make_unique<QParallelAnimationGroup>();
            for (auto& anim : animations_) {
                group->addAnimation(anim.release());
            }
            result = qt_unique_ptr<QAbstractAnimation>(group.release());
        }

        // Apply configuration
        if (auto* propAnim = qobject_cast<QPropertyAnimation*>(result.get())) {
            propAnim->setDuration(config_.duration.count());
            propAnim->setEasingCurve(config_.easing);
        } else if (auto* group = qobject_cast<QAnimationGroup*>(result.get())) {
            for (int i = 0; i < group->animationCount(); ++i) {
                if (auto* propAnim = qobject_cast<QPropertyAnimation*>(group->animationAt(i))) {
                    propAnim->setDuration(config_.duration.count());
                    propAnim->setEasingCurve(config_.easing);
                }
            }
        }

        result->setLoopCount(config_.loop_count);

        // Connect signals
        if (config_.on_finished) {
            QObject::connect(result.get(), &QAbstractAnimation::finished, config_.on_finished);
        }
        if (config_.on_started) {
            QObject::connect(result.get(), &QAbstractAnimation::stateChanged,
                           [callback = config_.on_started](QAbstractAnimation::State newState) {
                               if (newState == QAbstractAnimation::Running) {
                                   callback();
                               }
                           });
        }

        return result;
    }

    // **Build and start immediately**
    void start() {
        if (auto animation = build()) {
            // Add delay if specified
            if (config_.delay > 0ms) {
                QTimer::singleShot(config_.delay.count(), [anim = animation.release()]() {
                    anim->start(QAbstractAnimation::DeleteWhenStopped);
                });
            } else {
                animation.release()->start(QAbstractAnimation::DeleteWhenStopped);
            }
        }
    }

private:
    T* target_;
    AnimationConfig config_;
    std::vector<std::unique_ptr<QPropertyAnimation>> animations_;
};

// **Animation presets for common effects**
namespace Presets {

template<Concepts::QtWidget T>
[[nodiscard]] auto fadeIn(T* widget, std::chrono::milliseconds duration = 300ms) {
    return AnimationBuilder(widget)
        .animate("windowOpacity", 0.0, 1.0)
        .duration(duration)
        .easing(Easing::OutCubic);
}

template<Concepts::QtWidget T>
[[nodiscard]] auto fadeOut(T* widget, std::chrono::milliseconds duration = 300ms) {
    return AnimationBuilder(widget)
        .animate("windowOpacity", 1.0, 0.0)
        .duration(duration)
        .easing(Easing::OutCubic);
}

template<Concepts::QtWidget T>
[[nodiscard]] auto slideIn(T* widget, const QPoint& from, const QPoint& to, 
                          std::chrono::milliseconds duration = 400ms) {
    return AnimationBuilder(widget)
        .animate("pos", from, to)
        .duration(duration)
        .easing(Easing::OutQuart);
}

template<Concepts::QtWidget T>
[[nodiscard]] auto scaleIn(T* widget, std::chrono::milliseconds duration = 300ms) {
    const auto originalSize = widget->size();
    const auto startSize = QSize(0, 0);
    
    return AnimationBuilder(widget)
        .animate("size", startSize, originalSize)
        .duration(duration)
        .easing(Easing::OutBounce);
}

template<Concepts::QtWidget T>
[[nodiscard]] auto bounce(T* widget, int intensity = 10, std::chrono::milliseconds duration = 600ms) {
    const auto originalPos = widget->pos();
    const auto bouncePos = originalPos + QPoint(0, -intensity);
    
    return AnimationBuilder(widget)
        .animate("pos", originalPos, bouncePos)
        .duration(duration / 2)
        .easing(Easing::OutElastic)
        .autoReverse(true);
}

template<Concepts::QtWidget T>
[[nodiscard]] auto shake(T* widget, int intensity = 5, std::chrono::milliseconds duration = 500ms) {
    const auto originalPos = widget->pos();
    
    return AnimationBuilder(widget)
        .animate("pos", originalPos, originalPos + QPoint(intensity, 0))
        .duration(duration / 8)
        .loop(8)
        .easing(Easing::Linear);
}

}  // namespace Presets

// **Animation sequence builder**
class AnimationSequence {
public:
    AnimationSequence() : group_(std::make_unique<QSequentialAnimationGroup>()) {}

    // **Add animation to sequence**
    template<typename AnimationType>
    AnimationSequence& then(AnimationType&& animation) {
        if constexpr (std::is_same_v<std::decay_t<AnimationType>, AnimationBuilder<typename AnimationType::target_type>>) {
            group_->addAnimation(animation.build().release());
        } else {
            group_->addAnimation(std::forward<AnimationType>(animation));
        }
        return *this;
    }

    // **Add pause to sequence**
    AnimationSequence& pause(std::chrono::milliseconds duration) {
        auto* pauseAnimation = new QPauseAnimation(duration.count());
        group_->addAnimation(pauseAnimation);
        return *this;
    }

    // **Build the sequence**
    [[nodiscard]] qt_unique_ptr<QSequentialAnimationGroup> build() {
        return qt_unique_ptr<QSequentialAnimationGroup>(group_.release());
    }

    // **Start the sequence**
    void start() {
        group_.release()->start(QAbstractAnimation::DeleteWhenStopped);
    }

private:
    std::unique_ptr<QSequentialAnimationGroup> group_;
};

// **Utility functions**
template<Concepts::QtObject T>
[[nodiscard]] constexpr auto animate(T* target) -> AnimationBuilder<T> {
    return AnimationBuilder<T>(target);
}

template<typename... Animations>
[[nodiscard]] auto parallel(Animations&&... animations) {
    auto group = std::make_unique<QParallelAnimationGroup>();
    (group->addAnimation(std::forward<Animations>(animations)), ...);
    return qt_unique_ptr<QParallelAnimationGroup>(group.release());
}

template<typename... Animations>
[[nodiscard]] auto sequence(Animations&&... animations) {
    auto seq = AnimationSequence();
    (seq.then(std::forward<Animations>(animations)), ...);
    return seq.build();
}

}  // namespace DeclarativeUI::Core::Animation
