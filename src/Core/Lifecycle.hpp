#pragma once

/**
 * @file Lifecycle.hpp
 * @brief Component lifecycle management system for DeclarativeUI
 *
 * This header provides comprehensive lifecycle management for UI components,
 * including mount/unmount hooks, update callbacks, and proper resource
 * management. The system integrates seamlessly with the existing UIElement and
 * DeclarativeBuilder architecture while providing React-like lifecycle hooks.
 */

#include <QObject>
#include <QString>
#include <QVariant>
#include <QWidget>
#include <chrono>
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

namespace DeclarativeUI::Core {

// Forward declarations
class UIElement;
template <typename WidgetType>
class DeclarativeBuilder;

/**
 * @brief Lifecycle phase enumeration
 */
enum class LifecyclePhase {
    BeforeMount,    ///< Before component is mounted to DOM
    Mounted,        ///< Component has been mounted and is visible
    BeforeUpdate,   ///< Before component properties are updated
    Updated,        ///< After component properties have been updated
    BeforeUnmount,  ///< Before component is unmounted
    Unmounted,      ///< Component has been unmounted and cleaned up
    Error           ///< Component encountered an error
};

/**
 * @brief Lifecycle hook context providing information about the lifecycle event
 */
struct LifecycleContext {
    LifecyclePhase phase;
    QWidget* widget = nullptr;
    std::unordered_map<QString, QVariant> previous_props;
    std::unordered_map<QString, QVariant> current_props;
    std::chrono::steady_clock::time_point timestamp;
    QString error_message;  ///< Only populated during Error phase

    LifecycleContext(LifecyclePhase p)
        : phase(p), timestamp(std::chrono::steady_clock::now()) {}
};

/**
 * @brief Lifecycle hook function type
 */
using LifecycleHook = std::function<void(const LifecycleContext&)>;

/**
 * @brief Cleanup function type for resource management
 */
using CleanupFunction = std::function<void()>;

/**
 * @brief Effect hook similar to React's useEffect
 */
struct EffectHook {
    std::function<CleanupFunction()>
        effect;  ///< Effect function that returns cleanup
    std::vector<QVariant> dependencies;  ///< Dependency array for effect
    CleanupFunction cleanup;             ///< Stored cleanup function
    bool needs_cleanup = false;          ///< Whether cleanup is needed

    EffectHook(std::function<CleanupFunction()> eff,
               std::vector<QVariant> deps = {})
        : effect(std::move(eff)), dependencies(std::move(deps)) {}
};

/**
 * @brief Lifecycle manager for a single component
 *
 * Manages all lifecycle hooks and effects for a component instance.
 * Integrates with UIElement to provide automatic lifecycle management.
 */
class ComponentLifecycle : public QObject {
    Q_OBJECT

public:
    explicit ComponentLifecycle(QObject* parent = nullptr);
    ~ComponentLifecycle() override;

    // **Lifecycle hook registration**
    void onMount(LifecycleHook hook);
    void onUnmount(LifecycleHook hook);
    void onUpdate(LifecycleHook hook);
    void onError(LifecycleHook hook);

    // **Effect system (React useEffect-like)**
    void useEffect(std::function<CleanupFunction()> effect,
                   const std::vector<QVariant>& dependencies = {});

    // **Lifecycle phase management**
    void mount(QWidget* widget);
    void unmount();
    void update(const std::unordered_map<QString, QVariant>& previous_props,
                const std::unordered_map<QString, QVariant>& current_props);
    void handleError(const QString& error_message);

    // **State queries**
    bool isMounted() const { return is_mounted_; }
    LifecyclePhase getCurrentPhase() const { return current_phase_; }
    QWidget* getWidget() const { return widget_; }

    // **Performance monitoring**
    struct LifecycleMetrics {
        std::chrono::milliseconds mount_time{0};
        std::chrono::milliseconds update_time{0};
        std::chrono::milliseconds unmount_time{0};
        int update_count = 0;
        int effect_count = 0;
        int cleanup_count = 0;
    };

    const LifecycleMetrics& getMetrics() const { return metrics_; }

signals:
    void phaseChanged(LifecyclePhase phase);
    void mounted();
    void unmounted();
    void updated();
    void errorOccurred(const QString& error);

private slots:
    void onWidgetDestroyed();

private:
    // **Core state**
    bool is_mounted_ = false;
    LifecyclePhase current_phase_ = LifecyclePhase::BeforeMount;
    QWidget* widget_ = nullptr;

    // **Hook storage**
    std::vector<LifecycleHook> mount_hooks_;
    std::vector<LifecycleHook> unmount_hooks_;
    std::vector<LifecycleHook> update_hooks_;
    std::vector<LifecycleHook> error_hooks_;

    // **Effect system**
    std::vector<std::unique_ptr<EffectHook>> effects_;

    // **Performance metrics**
    LifecycleMetrics metrics_;
    std::chrono::steady_clock::time_point phase_start_time_;

    // **Internal methods**
    void executeHooks(const std::vector<LifecycleHook>& hooks,
                      const LifecycleContext& context);
    void runEffects(const std::unordered_map<QString, QVariant>& current_props);
    void cleanupEffects();
    bool dependenciesChanged(
        const EffectHook& effect,
        const std::unordered_map<QString, QVariant>& props);
    void updateMetrics(LifecyclePhase phase);
    void setPhase(LifecyclePhase phase);
};

/**
 * @brief Lifecycle hook builder for fluent API integration
 *
 * Provides a fluent interface for registering lifecycle hooks
 * that integrates seamlessly with DeclarativeBuilder.
 */
class LifecycleBuilder {
public:
    explicit LifecycleBuilder(ComponentLifecycle* lifecycle);

    // **Fluent lifecycle hook registration**
    LifecycleBuilder& onMount(LifecycleHook hook);
    LifecycleBuilder& onUnmount(LifecycleHook hook);
    LifecycleBuilder& onUpdate(LifecycleHook hook);
    LifecycleBuilder& onError(LifecycleHook hook);

    // **Effect registration**
    LifecycleBuilder& useEffect(std::function<CleanupFunction()> effect,
                                const std::vector<QVariant>& dependencies = {});

    // **Convenience methods**
    LifecycleBuilder& onMount(std::function<void()> simple_hook);
    LifecycleBuilder& onUnmount(std::function<void()> simple_hook);
    LifecycleBuilder& onUpdate(std::function<void()> simple_hook);

private:
    ComponentLifecycle* lifecycle_;
};

}  // namespace DeclarativeUI::Core
