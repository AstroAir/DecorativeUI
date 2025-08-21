/**
 * @file Lifecycle.cpp
 * @brief Implementation of component lifecycle management system
 */

#include "Lifecycle.hpp"
#include "../Exceptions/UIExceptions.hpp"
#include "UIElement.hpp"

#include <QDebug>
#include <QTimer>
#include <algorithm>
#include <chrono>

namespace DeclarativeUI::Core {

ComponentLifecycle::ComponentLifecycle(QObject* parent) : QObject(parent) {
    qDebug() << "🔄 ComponentLifecycle created";
}

ComponentLifecycle::~ComponentLifecycle() {
    if (is_mounted_) {
        qWarning() << "⚠️ ComponentLifecycle destroyed while still mounted - "
                      "forcing cleanup";
        cleanupEffects();
    }
    qDebug() << "🔄 ComponentLifecycle destroyed";
}

void ComponentLifecycle::onMount(LifecycleHook hook) {
    if (!hook) {
        qWarning() << "⚠️ Attempted to register null mount hook";
        return;
    }
    mount_hooks_.push_back(std::move(hook));
}

void ComponentLifecycle::onUnmount(LifecycleHook hook) {
    if (!hook) {
        qWarning() << "⚠️ Attempted to register null unmount hook";
        return;
    }
    unmount_hooks_.push_back(std::move(hook));
}

void ComponentLifecycle::onUpdate(LifecycleHook hook) {
    if (!hook) {
        qWarning() << "⚠️ Attempted to register null update hook";
        return;
    }
    update_hooks_.push_back(std::move(hook));
}

void ComponentLifecycle::onError(LifecycleHook hook) {
    if (!hook) {
        qWarning() << "⚠️ Attempted to register null error hook";
        return;
    }
    error_hooks_.push_back(std::move(hook));
}

void ComponentLifecycle::useEffect(std::function<CleanupFunction()> effect,
                                   const std::vector<QVariant>& dependencies) {
    if (!effect) {
        qWarning() << "⚠️ Attempted to register null effect";
        return;
    }

    auto effect_hook =
        std::make_unique<EffectHook>(std::move(effect), dependencies);
    effects_.push_back(std::move(effect_hook));
    metrics_.effect_count++;

    qDebug() << "🔄 Effect registered with" << dependencies.size()
             << "dependencies";
}

void ComponentLifecycle::mount(QWidget* widget) {
    if (is_mounted_) {
        qWarning() << "⚠️ Component already mounted";
        return;
    }

    if (!widget) {
        throw Exceptions::ComponentCreationException(
            "Cannot mount with null widget");
    }

    widget_ = widget;
    setPhase(LifecyclePhase::BeforeMount);

    try {
        // Execute mount hooks
        LifecycleContext context(LifecyclePhase::Mounted);
        context.widget = widget_;
        executeHooks(mount_hooks_, context);

        // Run initial effects
        runEffects({});

        // Connect to widget destruction
        connect(widget_, &QWidget::destroyed, this,
                &ComponentLifecycle::onWidgetDestroyed);

        is_mounted_ = true;
        setPhase(LifecyclePhase::Mounted);

        updateMetrics(LifecyclePhase::Mounted);
        emit mounted();

        qDebug() << "✅ Component mounted successfully";

    } catch (const std::exception& e) {
        handleError(QString("Mount failed: %1").arg(e.what()));
        throw;
    }
}

void ComponentLifecycle::unmount() {
    if (!is_mounted_) {
        qDebug() << "ℹ️ Component not mounted, skipping unmount";
        return;
    }

    setPhase(LifecyclePhase::BeforeUnmount);

    try {
        // Execute unmount hooks
        LifecycleContext context(LifecyclePhase::BeforeUnmount);
        context.widget = widget_;
        executeHooks(unmount_hooks_, context);

        // Cleanup all effects
        cleanupEffects();

        // Disconnect from widget
        if (widget_) {
            disconnect(widget_, &QWidget::destroyed, this,
                       &ComponentLifecycle::onWidgetDestroyed);
        }

        is_mounted_ = false;
        widget_ = nullptr;
        setPhase(LifecyclePhase::Unmounted);

        updateMetrics(LifecyclePhase::Unmounted);
        emit unmounted();

        qDebug() << "✅ Component unmounted successfully";

    } catch (const std::exception& e) {
        handleError(QString("Unmount failed: %1").arg(e.what()));
        // Continue with cleanup even if hooks fail
        is_mounted_ = false;
        widget_ = nullptr;
        cleanupEffects();
    }
}

void ComponentLifecycle::update(
    const std::unordered_map<QString, QVariant>& previous_props,
    const std::unordered_map<QString, QVariant>& current_props) {
    if (!is_mounted_) {
        qWarning() << "⚠️ Cannot update unmounted component";
        return;
    }

    setPhase(LifecyclePhase::BeforeUpdate);

    try {
        // Execute update hooks
        LifecycleContext context(LifecyclePhase::Updated);
        context.widget = widget_;
        context.previous_props = previous_props;
        context.current_props = current_props;
        executeHooks(update_hooks_, context);

        // Run effects that depend on changed properties
        runEffects(current_props);

        setPhase(LifecyclePhase::Updated);
        metrics_.update_count++;
        updateMetrics(LifecyclePhase::Updated);

        emit updated();

        qDebug() << "✅ Component updated successfully";

    } catch (const std::exception& e) {
        handleError(QString("Update failed: %1").arg(e.what()));
        throw;
    }
}

void ComponentLifecycle::handleError(const QString& error_message) {
    qCritical() << "❌ Component error:" << error_message;

    setPhase(LifecyclePhase::Error);

    try {
        LifecycleContext context(LifecyclePhase::Error);
        context.widget = widget_;
        context.error_message = error_message;
        executeHooks(error_hooks_, context);

        emit errorOccurred(error_message);

    } catch (const std::exception& e) {
        qCritical() << "❌ Error in error handler:" << e.what();
        // Don't throw from error handler to prevent infinite loops
    }
}

void ComponentLifecycle::onWidgetDestroyed() {
    qDebug() << "🔄 Widget destroyed, cleaning up lifecycle";
    if (is_mounted_) {
        unmount();
    }
}

void ComponentLifecycle::executeHooks(const std::vector<LifecycleHook>& hooks,
                                      const LifecycleContext& context) {
    for (const auto& hook : hooks) {
        try {
            hook(context);
        } catch (const std::exception& e) {
            qWarning() << "⚠️ Hook execution failed:" << e.what();
            // Continue executing other hooks
        }
    }
}

void ComponentLifecycle::runEffects(
    const std::unordered_map<QString, QVariant>& current_props) {
    for (auto& effect : effects_) {
        if (dependenciesChanged(*effect, current_props)) {
            // Cleanup previous effect if needed
            if (effect->needs_cleanup && effect->cleanup) {
                try {
                    effect->cleanup();
                    metrics_.cleanup_count++;
                } catch (const std::exception& e) {
                    qWarning() << "⚠️ Effect cleanup failed:" << e.what();
                }
                effect->needs_cleanup = false;
            }

            // Run new effect
            try {
                effect->cleanup = effect->effect();
                effect->needs_cleanup = (effect->cleanup != nullptr);
            } catch (const std::exception& e) {
                qWarning() << "⚠️ Effect execution failed:" << e.what();
            }
        }
    }
}

void ComponentLifecycle::cleanupEffects() {
    for (auto& effect : effects_) {
        if (effect->needs_cleanup && effect->cleanup) {
            try {
                effect->cleanup();
                metrics_.cleanup_count++;
            } catch (const std::exception& e) {
                qWarning() << "⚠️ Effect cleanup failed:" << e.what();
            }
            effect->needs_cleanup = false;
        }
    }
}

bool ComponentLifecycle::dependenciesChanged(
    const EffectHook& effect,
    const std::unordered_map<QString, QVariant>& props) {
    // If no dependencies, run on every update
    if (effect.dependencies.empty()) {
        return true;
    }

    // Check if any dependency changed
    for (const auto& dep : effect.dependencies) {
        QString key = dep.toString();
        auto it = props.find(key);
        if (it != props.end() && it->second != dep) {
            return true;
        }
    }

    return false;
}

void ComponentLifecycle::updateMetrics(LifecyclePhase phase) {
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - phase_start_time_);

    switch (phase) {
        case LifecyclePhase::Mounted:
            metrics_.mount_time = duration;
            break;
        case LifecyclePhase::Updated:
            metrics_.update_time = duration;
            break;
        case LifecyclePhase::Unmounted:
            metrics_.unmount_time = duration;
            break;
        default:
            break;
    }
}

void ComponentLifecycle::setPhase(LifecyclePhase phase) {
    if (current_phase_ != phase) {
        current_phase_ = phase;
        phase_start_time_ = std::chrono::steady_clock::now();
        emit phaseChanged(phase);
    }
}

// LifecycleBuilder implementation
LifecycleBuilder::LifecycleBuilder(ComponentLifecycle* lifecycle)
    : lifecycle_(lifecycle) {
    if (!lifecycle_) {
        throw std::invalid_argument(
            "LifecycleBuilder requires non-null ComponentLifecycle");
    }
}

LifecycleBuilder& LifecycleBuilder::onMount(LifecycleHook hook) {
    lifecycle_->onMount(std::move(hook));
    return *this;
}

LifecycleBuilder& LifecycleBuilder::onUnmount(LifecycleHook hook) {
    lifecycle_->onUnmount(std::move(hook));
    return *this;
}

LifecycleBuilder& LifecycleBuilder::onUpdate(LifecycleHook hook) {
    lifecycle_->onUpdate(std::move(hook));
    return *this;
}

LifecycleBuilder& LifecycleBuilder::onError(LifecycleHook hook) {
    lifecycle_->onError(std::move(hook));
    return *this;
}

LifecycleBuilder& LifecycleBuilder::useEffect(
    std::function<CleanupFunction()> effect,
    const std::vector<QVariant>& dependencies) {
    lifecycle_->useEffect(std::move(effect), dependencies);
    return *this;
}

LifecycleBuilder& LifecycleBuilder::onMount(std::function<void()> simple_hook) {
    return onMount([simple_hook](const LifecycleContext&) { simple_hook(); });
}

LifecycleBuilder& LifecycleBuilder::onUnmount(
    std::function<void()> simple_hook) {
    return onUnmount([simple_hook](const LifecycleContext&) { simple_hook(); });
}

LifecycleBuilder& LifecycleBuilder::onUpdate(
    std::function<void()> simple_hook) {
    return onUpdate([simple_hook](const LifecycleContext&) { simple_hook(); });
}

}  // namespace DeclarativeUI::Core
