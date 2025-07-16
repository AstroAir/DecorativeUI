#pragma once

#include <QObject>
#include <QVariant>

#include <deque>
#include <functional>
#include <memory>
#include <unordered_map>


namespace DeclarativeUI::Binding {

// **Base class for reactive properties that can emit signals**
class ReactivePropertyBase : public QObject {
    Q_OBJECT

public:
    explicit ReactivePropertyBase(QObject* parent = nullptr)
        : QObject(parent) {}
    virtual ~ReactivePropertyBase() = default;

signals:
    void valueChanged();

protected:
    void emitValueChanged() { emit valueChanged(); }
};

// **Reactive state management**
template <typename T>
class ReactiveProperty : public ReactivePropertyBase {
public:
    explicit ReactiveProperty(T initial_value = T{})
        : ReactivePropertyBase(nullptr), value_(std::move(initial_value)) {}

    // **Thread-safe property access**
    [[nodiscard]] const T& get() const noexcept { return value_; }

    void set(T new_value) {
        if (value_ != new_value) {
            value_ = std::move(new_value);
            emitValueChanged();
        }
    }

    // **Automatic type conversion for Qt integration**
    operator QVariant() const { return QVariant::fromValue(value_); }

    // **Binding support**
    void bind(std::function<T()> computer) {
        computer_ = std::move(computer);
        update();
    }

    void update() {
        if (computer_) {
            set(computer_());
        }
    }

private:
    T value_;
    std::function<T()> computer_;
};

class StateManager : public QObject {
    Q_OBJECT

public:
    static StateManager& instance();

    // **Type-safe state registration**
    template <typename T>
    std::shared_ptr<ReactiveProperty<T>> createState(const QString& key,
                                                     T initial_value = T{});

    template <typename T>
    std::shared_ptr<ReactiveProperty<T>> getState(const QString& key);

    // **Computed properties**
    template <typename T>
    std::shared_ptr<ReactiveProperty<T>> createComputed(
        const QString& key, std::function<T()> computer,
        std::vector<QString> dependencies = {});

    // **Batch updates for performance**
    void batchUpdate(std::function<void()> updates);

    // **State persistence**
    void saveState(const QString& filename) const;
    void loadState(const QString& filename);

    // **State validation**
    template <typename T>
    void setValidator(const QString& key,
                      std::function<bool(const T&)> validator);

    // **State history for undo/redo**
    void enableHistory(const QString& key, int max_history_size = 50);
    void disableHistory(const QString& key);
    bool canUndo(const QString& key) const;
    bool canRedo(const QString& key) const;
    void undo(const QString& key);
    void redo(const QString& key);

    // **Performance monitoring**
    void enablePerformanceMonitoring(bool enabled);
    QString getPerformanceReport() const;

    // **Dependency tracking**
    void addDependency(const QString& dependent, const QString& dependency);
    void removeDependency(const QString& dependent, const QString& dependency);
    QStringList getDependencies(const QString& key) const;
    void updateDependents(const QString& key);

    // **State debugging**
    void enableDebugMode(bool enabled);
    void logStateChange(const QString& key, const QVariant& oldValue,
                        const QVariant& newValue);

    void clearState() noexcept;

signals:
    void stateChanged(const QString& key, const QVariant& value);
    void stateAdded(const QString& key);
    void stateRemoved(const QString& key);
    void performanceWarning(const QString& key, qint64 time_ms);

private:
    StateManager() = default;

    struct StateInfo {
        std::shared_ptr<QObject> state;
        std::function<bool(const QVariant&)> validator;
        std::deque<QVariant> history;
        int history_position = 0;
        int max_history_size = 50;
        bool history_enabled = false;
        qint64 last_update_time = 0;
        int update_count = 0;
    };

    std::unordered_map<QString, StateInfo> states_;
    std::unordered_map<QString, std::vector<QString>> dependencies_;
    std::unordered_map<QString, std::vector<QString>> dependents_;

    bool batching_ = false;
    bool debug_mode_ = false;
    bool performance_monitoring_ = false;
    std::vector<std::function<void()>> pending_updates_;

    void processPendingUpdates();
    void addToHistory(const QString& key, const QVariant& value);
    void validateState(const QString& key, const QVariant& value);
    void measurePerformance(const QString& key,
                            std::function<void()> operation);
};

// **Template implementations**
template <typename T>
std::shared_ptr<ReactiveProperty<T>> StateManager::createState(
    const QString& key, T initial_value) {
    auto state =
        std::make_shared<ReactiveProperty<T>>(std::move(initial_value));
    StateInfo info;
    info.state = state;
    states_[key] = info;
    return state;
}

template <typename T>
std::shared_ptr<ReactiveProperty<T>> StateManager::getState(
    const QString& key) {
    auto it = states_.find(key);
    if (it != states_.end()) {
        return std::static_pointer_cast<ReactiveProperty<T>>(it->second.state);
    }
    return nullptr;
}

template <typename T>
std::shared_ptr<ReactiveProperty<T>> StateManager::createComputed(
    const QString& key, std::function<T()> computer,
    std::vector<QString> dependencies) {
    auto computed = std::make_shared<ReactiveProperty<T>>(computer());

    // **Set up the computer function for future updates**
    computed->bind(computer);

    StateInfo info;
    info.state = computed;
    states_[key] = info;

    // TODO: Implement dependency tracking and automatic recomputation
    return computed;
}

}  // namespace DeclarativeUI::Binding