#pragma once

/**
 * @file StateManager.hpp
 * @brief Provides a reactive state management system for Qt-based UI,
 * supporting type-safe state, computed properties, dependency tracking,
 * undo/redo history, validation, batch updates, persistence, and performance
 * monitoring.
 */

#include <QDateTime>
#include <QObject>
#include <QRecursiveMutex>
#include <QVariant>

#include <deque>
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

namespace DeclarativeUI::Binding {

/**
 * @class ReactivePropertyBase
 * @brief Base class for reactive properties that can emit value change signals.
 *
 * Provides a common interface for all reactive properties, enabling
 * signal-based notification of value changes.
 */
class ReactivePropertyBase : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Constructs a ReactivePropertyBase.
     * @param parent Parent QObject.
     */
    explicit ReactivePropertyBase(QObject* parent = nullptr)
        : QObject(parent) {}
    /**
     * @brief Virtual destructor.
     */
    virtual ~ReactivePropertyBase() = default;

signals:
    /**
     * @brief Emitted when the property's value changes.
     */
    void valueChanged();

protected:
    /**
     * @brief Emits the valueChanged signal.
     */
    void emitValueChanged() { emit valueChanged(); }
};

/**
 * @class ReactiveProperty
 * @brief Template class for a type-safe, reactive property supporting value
 * change notification and computed bindings.
 * @tparam T The type of the property value.
 *
 * Provides thread-safe access, automatic Qt QVariant conversion, and support
 * for computed (derived) properties.
 */
template <typename T>
class ReactiveProperty : public ReactivePropertyBase {
public:
    /**
     * @brief Constructs a ReactiveProperty with an initial value.
     * @param initial_value The initial value of the property.
     */
    explicit ReactiveProperty(T initial_value = T{})
        : ReactivePropertyBase(nullptr), value_(std::move(initial_value)) {}

    /**
     * @brief Gets the current value (thread-safe).
     * @return Const reference to the value.
     */
    [[nodiscard]] const T& get() const noexcept { return value_; }

    /**
     * @brief Sets the property value. Emits valueChanged if the value changes.
     * @param new_value The new value to set.
     */
    void set(T new_value) {
        if (value_ != new_value) {
            value_ = std::move(new_value);
            emitValueChanged();
        }
    }

    /**
     * @brief Implicit conversion to QVariant for Qt integration.
     * @return QVariant containing the value.
     */
    operator QVariant() const { return QVariant::fromValue(value_); }

    /**
     * @brief Binds the property to a compute function, making it a computed
     * property.
     * @param computer Function that computes the property's value.
     */
    void bind(std::function<T()> computer) {
        computer_ = std::move(computer);
        update();
    }

    /**
     * @brief Updates the property value by invoking the compute function, if
     * set.
     */
    void update() {
        if (computer_) {
            set(computer_());
        }
    }

private:
    T value_;  ///< The property value.
    std::function<T()>
        computer_;  ///< Optional compute function for computed properties.
};

/**
 * @class StateManager
 * @brief Central manager for application state, providing type-safe state
 * registration, computed properties, dependency tracking, undo/redo history,
 * validation, batch updates, persistence, and performance monitoring.
 *
 * StateManager is a singleton that manages all application state in a
 * thread-safe manner. It supports both simple and computed (derived) state,
 * dependency tracking for automatic updates, undo/redo history, validation,
 * batch updates, persistence to disk, and performance diagnostics.
 */
class StateManager : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Gets the singleton instance of the StateManager.
     * @return Reference to the StateManager.
     */
    static StateManager& instance();

    // **Type-safe state registration**

    /**
     * @brief Creates and registers a new state variable.
     * @tparam T The type of the state.
     * @param key Unique key for the state.
     * @param initial_value Initial value for the state.
     * @return Shared pointer to the created ReactiveProperty.
     */
    template <typename T>
    std::shared_ptr<ReactiveProperty<T>> createState(const QString& key,
                                                     T initial_value = T{});

    /**
     * @brief Retrieves a registered state variable.
     * @tparam T The type of the state.
     * @param key Unique key for the state.
     * @return Shared pointer to the ReactiveProperty, or nullptr if not found.
     */
    template <typename T>
    std::shared_ptr<ReactiveProperty<T>> getState(const QString& key);

    // **Computed properties**

    /**
     * @brief Creates a computed (derived) state variable.
     * @tparam T The type of the computed state.
     * @param key Unique key for the computed state.
     * @param computer Function that computes the value.
     * @param dependencies List of keys this computed state depends on.
     * @return Shared pointer to the computed ReactiveProperty.
     */
    template <typename T>
    std::shared_ptr<ReactiveProperty<T>> createComputed(
        const QString& key, std::function<T()> computer,
        std::vector<QString> dependencies = {});

    // **Batch updates for performance**

    /**
     * @brief Executes a batch of updates as a single operation for performance.
     * @param updates Function containing the batch of updates.
     */
    void batchUpdate(std::function<void()> updates);

    // **State persistence**

    /**
     * @brief Saves all state to a file.
     * @param filename Path to the file.
     */
    void saveState(const QString& filename) const;

    /**
     * @brief Loads state from a file.
     * @param filename Path to the file.
     */
    void loadState(const QString& filename);

    // **State validation**

    /**
     * @brief Sets a validator function for a state variable.
     * @tparam T The type of the state.
     * @param key State key.
     * @param validator Function that returns true if the value is valid.
     */
    template <typename T>
    void setValidator(const QString& key,
                      std::function<bool(const T&)> validator);

    // **State history for undo/redo**

    /**
     * @brief Enables undo/redo history for a state variable.
     * @param key State key.
     * @param max_history_size Maximum number of history entries to keep.
     */
    void enableHistory(const QString& key, int max_history_size = 50);

    /**
     * @brief Disables undo/redo history for a state variable.
     * @param key State key.
     */
    void disableHistory(const QString& key);

    /**
     * @brief Checks if undo is possible for a state variable.
     * @param key State key.
     * @return True if undo is possible, false otherwise.
     */
    bool canUndo(const QString& key) const;

    /**
     * @brief Checks if redo is possible for a state variable.
     * @param key State key.
     * @return True if redo is possible, false otherwise.
     */
    bool canRedo(const QString& key) const;

    /**
     * @brief Undoes the last change to a state variable.
     * @param key State key.
     */
    void undo(const QString& key);

    /**
     * @brief Redoes the last undone change to a state variable.
     * @param key State key.
     */
    void redo(const QString& key);

    // **Performance monitoring**

    /**
     * @brief Enables or disables performance monitoring for state updates.
     * @param enabled True to enable, false to disable.
     */
    void enablePerformanceMonitoring(bool enabled);

    /**
     * @brief Gets a performance report as a string.
     * @return Performance report.
     */
    QString getPerformanceReport() const;

    // **Dependency tracking**

    /**
     * @brief Adds a dependency between two state variables.
     * @param dependent Key of the dependent state.
     * @param dependency Key of the dependency state.
     */
    void addDependency(const QString& dependent, const QString& dependency);

    /**
     * @brief Removes a dependency between two state variables.
     * @param dependent Key of the dependent state.
     * @param dependency Key of the dependency state.
     */
    void removeDependency(const QString& dependent, const QString& dependency);

    /**
     * @brief Gets the list of dependencies for a state variable.
     * @param key State key.
     * @return List of dependency keys.
     */
    QStringList getDependencies(const QString& key) const;

    /**
     * @brief Updates all dependents of a state variable.
     * @param key State key.
     */
    void updateDependents(const QString& key);

    // **State debugging**

    /**
     * @brief Enables or disables debug mode for state changes.
     * @param enabled True to enable, false to disable.
     */
    void enableDebugMode(bool enabled);

    /**
     * @brief Logs a state change for debugging purposes.
     * @param key State key.
     * @param oldValue Previous value.
     * @param newValue New value.
     */
    void logStateChange(const QString& key, const QVariant& oldValue,
                        const QVariant& newValue);

    /**
     * @brief Clears all registered state variables.
     */
    void clearState() noexcept;

    // **Convenience methods for testing and simpler usage**

    /**
     * @brief Sets the value of a state variable, creating it if it does not
     * exist.
     * @tparam T The type of the state.
     * @param key State key.
     * @param value Value to set.
     */
    template <typename T>
    void setState(const QString& key, const T& value);

    /**
     * @brief Checks if a state variable exists.
     * @param key State key.
     * @return True if the state exists, false otherwise.
     */
    bool hasState(const QString& key) const;

    /**
     * @brief Removes a state variable.
     * @param key State key.
     */
    void removeState(const QString& key);

    /**
     * @brief Sets a validator function for a state variable (convenience
     * overload).
     * @tparam T The type of the state.
     * @param key State key.
     * @param validator Validator function.
     */
    template <typename T>
    void setStateValidator(const QString& key,
                           std::function<bool(const T&)> validator);

signals:
    /**
     * @brief Emitted when a state variable changes.
     * @param key State key.
     * @param value New value.
     */
    void stateChanged(const QString& key, const QVariant& value);

    /**
     * @brief Emitted when a state variable is added.
     * @param key State key.
     */
    void stateAdded(const QString& key);

    /**
     * @brief Emitted when a state variable is removed.
     * @param key State key.
     */
    void stateRemoved(const QString& key);

    /**
     * @brief Emitted when a performance warning is detected for a state
     * variable.
     * @param key State key.
     * @param time_ms Time taken for the update in milliseconds.
     */
    void performanceWarning(const QString& key, qint64 time_ms);

private:
    StateManager() = default;

    /**
     * @struct StateInfo
     * @brief Internal structure holding metadata and management info for each
     * state variable.
     */
    struct StateInfo {
        std::shared_ptr<QObject> state;  ///< Pointer to the state object.
        std::function<bool(const QVariant&)>
            validator;  ///< Validator function for the state.
        std::deque<QVariant>
            history;  ///< History of previous values for undo/redo.
        int history_position = 0;      ///< Current position in the history.
        int max_history_size = 50;     ///< Maximum history size.
        bool history_enabled = false;  ///< Whether history is enabled.
        qint64 last_update_time = 0;   ///< Timestamp of last update.
        int update_count = 0;          ///< Number of updates performed.
    };

    std::unordered_map<QString, StateInfo>
        states_;  ///< Map of state keys to StateInfo.
    std::unordered_map<QString, std::vector<QString>>
        dependencies_;  ///< Map of state dependencies.
    std::unordered_map<QString, std::vector<QString>>
        dependents_;  ///< Map of state dependents.
    std::unordered_map<QString, std::function<QVariant()>>
        computed_values_;  ///< Map of computed value functions for dependent
                           ///< states.
    std::unordered_map<QString, QVariant>
        state_data_;  ///< Map of state keys to current values for quick access.

    bool batching_ = false;    ///< Whether batch update mode is active.
    bool debug_mode_ = false;  ///< Whether debug mode is enabled.
    bool performance_monitoring_ =
        false;  ///< Whether performance monitoring is enabled.
    std::vector<std::function<void()>>
        pending_updates_;  ///< Pending updates for batch mode.

    // **Thread synchronization**
    mutable QRecursiveMutex
        global_lock_;  ///< Recursive mutex to handle nested calls safely.

    /**
     * @brief Processes all pending updates in batch mode.
     */
    void processPendingUpdates();

    /**
     * @brief Adds a value to the history for undo/redo.
     * @param key State key.
     * @param value Value to add to history.
     */
    void addToHistory(const QString& key, const QVariant& value);

    /**
     * @brief Validates a state value using the registered validator.
     * @param key State key.
     * @param value Value to validate.
     */
    void validateState(const QString& key, const QVariant& value);

    /**
     * @brief Measures and records the performance of a state operation.
     * @param key State key.
     * @param operation Function to execute and measure.
     */
    void measurePerformance(const QString& key,
                            std::function<void()> operation);

    /**
     * @brief Helper function to get current value from a reactive state.
     * @param state Shared pointer to the reactive property base
     * @return QVariant containing the current value
     */
    QVariant getCurrentStateValue(
        const std::shared_ptr<ReactivePropertyBase>& state);

    /**
     * @brief Helper function to apply a value to a reactive state.
     * @param state Shared pointer to the reactive property base
     * @param value QVariant value to apply
     * @return true if value was successfully applied
     */
    bool applyValueToState(const std::shared_ptr<ReactivePropertyBase>& state,
                           const QVariant& value);

    /**
     * @brief Helper function to initialize history for a state.
     * @param info Reference to the StateInfo to initialize
     * @param key State key for logging purposes
     */
    void initializeStateHistory(StateInfo& info, const QString& key);

    /**
     * @brief Helper function to validate history position bounds.
     * @param info Reference to the StateInfo to validate
     * @param position Position to validate
     * @return true if position is valid
     */
    bool validateHistoryPosition(const StateInfo& info, int position) const;

    /**
     * @brief Helper function to apply a history value to a state.
     * @param info Reference to the StateInfo containing the state
     * @param value QVariant value to apply
     * @param key State key for logging purposes
     * @return true if value was successfully applied
     */
    bool applyHistoryValue(const StateInfo& info, const QVariant& value,
                           const QString& key);
};

// **Template implementations**

/**
 * @brief Creates and registers a new state variable.
 * @tparam T The type of the state.
 * @param key Unique key for the state.
 * @param initial_value Initial value for the state.
 * @return Shared pointer to the created ReactiveProperty.
 */
template <typename T>
std::shared_ptr<ReactiveProperty<T>> StateManager::createState(
    const QString& key, T initial_value) {
    auto state =
        std::make_shared<ReactiveProperty<T>>(std::move(initial_value));
    StateInfo info;
    info.state = state;
    info.update_count = 1;  // Initial creation counts as first update
    info.last_update_time = QDateTime::currentMSecsSinceEpoch();

    {
        QMutexLocker locker(&global_lock_);
        states_[key] = info;
    }

    // Emit signals
    emit stateAdded(key);
    emit stateChanged(key, QVariant::fromValue(initial_value));

    return state;
}

/**
 * @brief Retrieves a registered state variable.
 * @tparam T The type of the state.
 * @param key Unique key for the state.
 * @return Shared pointer to the ReactiveProperty, or nullptr if not found.
 */
template <typename T>
std::shared_ptr<ReactiveProperty<T>> StateManager::getState(
    const QString& key) {
    QMutexLocker locker(&global_lock_);
    auto it = states_.find(key);
    if (it != states_.end()) {
        return std::static_pointer_cast<ReactiveProperty<T>>(it->second.state);
    }
    return nullptr;
}

/**
 * @brief Creates a computed (derived) state variable.
 * @tparam T The type of the computed state.
 * @param key Unique key for the computed state.
 * @param computer Function that computes the value.
 * @param dependencies List of keys this computed state depends on.
 * @return Shared pointer to the computed ReactiveProperty.
 */
template <typename T>
std::shared_ptr<ReactiveProperty<T>> StateManager::createComputed(
    const QString& key, std::function<T()> computer,
    std::vector<QString> dependencies) {
    auto computed = std::make_shared<ReactiveProperty<T>>(computer());

    // **Set up the computer function for future updates**
    computed->bind(computer);

    StateInfo info;
    info.state = computed;

    {
        QMutexLocker locker(&global_lock_);
        states_[key] = info;
    }

    // TODO: Implement dependency tracking and automatic recomputation
    return computed;
}

// **Convenience template implementations**

/**
 * @brief Sets the value of a state variable, creating it if it does not exist.
 * @tparam T The type of the state.
 * @param key State key.
 * @param value Value to set.
 */
template <typename T>
void StateManager::setState(const QString& key, const T& value) {
    auto existing = getState<T>(key);
    if (existing) {
        // Check if state has a validator and update metrics with lock
        {
            QMutexLocker locker(&global_lock_);
            auto it = states_.find(key);
            if (it != states_.end() && it->second.validator) {
                if (!it->second.validator(QVariant::fromValue(value))) {
                    // Validation failed, don't update
                    return;
                }
            }

            // Add to history before updating
            if (it != states_.end() && it->second.history_enabled) {
                addToHistory(key, QVariant::fromValue(value));
            }

            // Update performance metrics
            if (it != states_.end()) {
                it->second.update_count++;
                it->second.last_update_time =
                    QDateTime::currentMSecsSinceEpoch();
            }
        }

        existing->set(value);
        emit stateChanged(key, QVariant::fromValue(value));
    } else {
        createState<T>(key, value);
    }
}

/**
 * @brief Sets a validator function for a state variable (convenience overload).
 * @tparam T The type of the state.
 * @param key State key.
 * @param validator Validator function.
 */
template <typename T>
void StateManager::setStateValidator(const QString& key,
                                     std::function<bool(const T&)> validator) {
    setValidator<T>(key, validator);
}

}  // namespace DeclarativeUI::Binding
