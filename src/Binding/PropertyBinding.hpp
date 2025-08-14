#pragma once

/**
 * @file PropertyBinding.hpp
 * @brief Provides property binding infrastructure for Qt-based UI, supporting
 * one-way, two-way, and one-time bindings, update modes, and performance
 * monitoring.
 */

#include <QMetaObject>
#include <QMetaProperty>
#include <QObject>
#include <QString>
#include <QVariant>
#include <QWidget>

#include <memory>
#include <unordered_map>
#include <vector>

namespace DeclarativeUI::Binding {

// **Forward declarations**
template <typename T>
class ReactiveProperty;
class StateManager;

/**
 * @enum BindingDirection
 * @brief Specifies the direction of the property binding.
 */
enum class BindingDirection {
    OneWay,  ///< Source -> Target only
    TwoWay,  ///< Source <-> Target bidirectional
    OneTime  ///< Single evaluation at binding time
};

/**
 * @enum UpdateMode
 * @brief Specifies how and when the binding updates are propagated.
 */
enum class UpdateMode {
    Immediate,  ///< Update immediately when source changes
    Deferred,   ///< Batch updates for performance
    Manual      ///< Manual update only
};

/**
 * @class IPropertyBinding
 * @brief Abstract interface for a property binding between two QObject
 * properties.
 *
 * Provides a common interface for all property bindings, allowing update,
 * disconnect, and query operations.
 */
class IPropertyBinding {
public:
    /**
     * @brief Virtual destructor.
     */
    virtual ~IPropertyBinding() = default;

    /**
     * @brief Updates the target property from the source.
     */
    virtual void update() = 0;

    /**
     * @brief Disconnects the binding, stopping further updates.
     */
    virtual void disconnect() = 0;

    /**
     * @brief Checks if the binding is still valid.
     * @return True if valid, false otherwise.
     */
    virtual bool isValid() const = 0;

    /**
     * @brief Gets the source property path as a string.
     * @return Source property path.
     */
    virtual QString getSourcePath() const = 0;

    /**
     * @brief Gets the target property path as a string.
     * @return Target property path.
     */
    virtual QString getTargetPath() const = 0;

    /**
     * @brief Gets the binding direction.
     * @return BindingDirection value.
     */
    virtual BindingDirection getDirection() const = 0;
};

/**
 * @class PropertyBindingManager
 * @brief Manages a collection of property bindings, providing batch operations
 * and performance monitoring.
 *
 * Allows adding, removing, updating, enabling/disabling, and querying bindings.
 * Supports performance tracking for diagnostics.
 */
class PropertyBindingManager : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Constructs a PropertyBindingManager.
     * @param parent Parent QObject.
     */
    explicit PropertyBindingManager(QObject *parent = nullptr);

    /**
     * @brief Destructor.
     */
    ~PropertyBindingManager() override;

    // **Binding management**

    /**
     * @brief Adds a property binding to the manager.
     * @param binding Shared pointer to the binding.
     */
    void addBinding(std::shared_ptr<IPropertyBinding> binding);

    /**
     * @brief Removes a property binding from the manager.
     * @param binding Shared pointer to the binding.
     */
    void removeBinding(std::shared_ptr<IPropertyBinding> binding);

    /**
     * @brief Removes all bindings managed by this manager.
     */
    void removeAllBindings();

    // **Batch operations**

    /**
     * @brief Updates all managed bindings.
     */
    void updateAllBindings();

    /**
     * @brief Enables all bindings (if previously disabled).
     */
    void enableAllBindings();

    /**
     * @brief Disables all bindings, preventing updates.
     */
    void disableAllBindings();

    // **Query methods**

    /**
     * @brief Gets the number of managed bindings.
     * @return Binding count.
     */
    int getBindingCount() const;

    /**
     * @brief Gets all managed bindings.
     * @return Vector of shared pointers to bindings.
     */
    std::vector<std::shared_ptr<IPropertyBinding>> getBindings() const;

    /**
     * @brief Gets all bindings associated with a specific QWidget.
     * @param widget Target QWidget.
     * @return Vector of shared pointers to bindings for the widget.
     */
    std::vector<std::shared_ptr<IPropertyBinding>> getBindingsForWidget(
        QWidget *widget) const;

    // **Performance monitoring**

    /**
     * @brief Enables or disables performance monitoring for bindings.
     * @param enabled True to enable, false to disable.
     */
    void enablePerformanceMonitoring(bool enabled);

    /**
     * @brief Checks if performance monitoring is enabled.
     * @return True if enabled, false otherwise.
     */
    bool isPerformanceMonitoringEnabled() const;

    /**
     * @brief Gets a performance report as a string.
     * @return Performance report.
     */
    QString getPerformanceReport() const;

private:
    // **Private members**
    std::vector<std::shared_ptr<IPropertyBinding>>
        m_bindings;                         ///< Managed bindings
    bool m_performance_monitoring_enabled;  ///< Performance monitoring flag

    // **Performance tracking**
    std::unordered_map<IPropertyBinding *, quint64>
        m_update_counts;  ///< Update count per binding
    std::unordered_map<IPropertyBinding *, qint64>
        m_update_times;  ///< Update time per binding (microseconds)

    // **Private methods**

    /**
     * @brief Tracks a binding for performance monitoring.
     * @param binding Pointer to the binding.
     */
    void trackBinding(IPropertyBinding *binding);

    /**
     * @brief Stops tracking a binding for performance monitoring.
     * @param binding Pointer to the binding.
     */
    void untrackBinding(IPropertyBinding *binding);
};

/**
 * @brief Gets the global property binding manager instance.
 * @return Pointer to the global PropertyBindingManager.
 */
PropertyBindingManager *getGlobalBindingManager();

}  // namespace DeclarativeUI::Binding
