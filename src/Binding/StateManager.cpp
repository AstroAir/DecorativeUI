#include "StateManager.hpp"

#include <QDebug>
#include <QTimer>
#include <algorithm>

namespace DeclarativeUI::Binding {

StateManager &StateManager::instance() {
    static StateManager instance;
    return instance;
}

void StateManager::batchUpdate(std::function<void()> updates) {
    if (!updates) {
        qWarning() << "Batch update function is null";
        return;
    }

    if (batching_) {
        // **Already batching, just execute**
        updates();
        return;
    }

    try {
        batching_ = true;
        updates();

        // **Process all pending updates**
        processPendingUpdates();

        batching_ = false;

    } catch (const std::exception &e) {
        batching_ = false;
        qWarning() << "Batch update failed:" << e.what();
        throw;
    }
}

void StateManager::clearState() noexcept {
    try {
        pending_updates_.clear();
        states_.clear();
        batching_ = false;

        qDebug() << "🗑️ State manager cleared";

    } catch (...) {
        // **No-throw guarantee**
        qWarning() << "Exception during state clear";
    }
}

void StateManager::processPendingUpdates() {
    if (pending_updates_.empty())
        return;

    qDebug() << "🔄 Processing" << pending_updates_.size()
             << "pending state updates";

    for (const auto &update : pending_updates_) {
        try {
            update();
        } catch (const std::exception &e) {
            qWarning() << "Pending update failed:" << e.what();
        }
    }

    pending_updates_.clear();
}

bool StateManager::hasState(const QString& key) const {
    return states_.find(key) != states_.end();
}

void StateManager::removeState(const QString& key) {
    auto it = states_.find(key);
    if (it != states_.end()) {
        emit stateRemoved(key);
        states_.erase(it);
        
        // Also remove any dependencies
        dependencies_.erase(key);
        dependents_.erase(key);
        
        // Remove this key from other dependencies
        for (auto& [k, deps] : dependencies_) {
            auto depIt = std::find(deps.begin(), deps.end(), key);
            if (depIt != deps.end()) {
                deps.erase(depIt);
            }
        }
        for (auto& [k, deps] : dependents_) {
            auto depIt = std::find(deps.begin(), deps.end(), key);
            if (depIt != deps.end()) {
                deps.erase(depIt);
            }
        }
        qDebug() << "🗑️ State removed:" << key;
    }
}

void StateManager::enableHistory(const QString& key, int max_history_size) {
    auto it = states_.find(key);
    if (it != states_.end()) {
        auto& info = it->second;
        info.history_enabled = true;
        info.max_history_size = max_history_size;
        info.history.clear();
        info.history_position = -1;
        qDebug() << "📝 History enabled for state:" << key << "with max size:" << max_history_size;
    } else {
        qWarning() << "❌ Cannot enable history: State" << key << "does not exist";
    }
}

void StateManager::disableHistory(const QString& key) {
    auto it = states_.find(key);
    if (it != states_.end()) {
        auto& info = it->second;
        info.history_enabled = false;
        info.history.clear();
        info.history_position = -1;
        qDebug() << "🚫 History disabled for state:" << key;
    }
}

bool StateManager::canUndo(const QString& key) const {
    auto it = states_.find(key);
    if (it != states_.end() && it->second.history_enabled) {
        return it->second.history_position > 0;
    }
    return false;
}

bool StateManager::canRedo(const QString& key) const {
    auto it = states_.find(key);
    if (it != states_.end() && it->second.history_enabled) {
        return it->second.history_position < static_cast<int>(it->second.history.size()) - 1;
    }
    return false;
}

void StateManager::undo(const QString& key) {
    auto it = states_.find(key);
    if (it != states_.end() && it->second.history_enabled && canUndo(key)) {
        auto& info = it->second;
        info.history_position--;
        
        if (info.history_position >= 0 && info.history_position < static_cast<int>(info.history.size())) {
            // For now, just emit the signal - the actual value setting would need type information
            auto value = info.history[info.history_position];
            emit stateChanged(key, value);
            qDebug() << "↶ Undo applied to state:" << key << "to position:" << info.history_position;
        }
    }
}

void StateManager::redo(const QString& key) {
    auto it = states_.find(key);
    if (it != states_.end() && it->second.history_enabled && canRedo(key)) {
        auto& info = it->second;
        info.history_position++;
        
        if (info.history_position >= 0 && info.history_position < static_cast<int>(info.history.size())) {
            // For now, just emit the signal - the actual value setting would need type information
            auto value = info.history[info.history_position];
            emit stateChanged(key, value);
            qDebug() << "↷ Redo applied to state:" << key << "to position:" << info.history_position;
        }
    }
}

void StateManager::addDependency(const QString& key, const QString& depends_on) {
    dependencies_[key].push_back(depends_on);
    dependents_[depends_on].push_back(key);
    qDebug() << "🔗 Dependency added:" << key << "depends on" << depends_on;
}

void StateManager::removeDependency(const QString& key, const QString& depends_on) {
    auto it = dependencies_.find(key);
    if (it != dependencies_.end()) {
        auto& deps = it->second;
        auto depIt = std::find(deps.begin(), deps.end(), depends_on);
        if (depIt != deps.end()) {
            deps.erase(depIt);
        }
    }
    
    auto depIt = dependents_.find(depends_on);
    if (depIt != dependents_.end()) {
        auto& deps = depIt->second;
        auto keyIt = std::find(deps.begin(), deps.end(), key);
        if (keyIt != deps.end()) {
            deps.erase(keyIt);
        }
    }
    qDebug() << "🔗❌ Dependency removed:" << key << "no longer depends on" << depends_on;
}

QStringList StateManager::getDependencies(const QString& key) const {
    auto it = dependencies_.find(key);
    if (it != dependencies_.end()) {
        QStringList result;
        for (const auto& dep : it->second) {
            result.append(dep);
        }
        return result;
    }
    return QStringList();
}

void StateManager::updateDependents(const QString& key) {
    auto it = dependents_.find(key);
    if (it != dependents_.end()) {
        for (const auto& dependent : it->second) {
            qDebug() << "🔄 Updating dependent state:" << dependent;
            // TODO: Trigger recomputation of dependent states
        }
    }
}

void StateManager::enableDebugMode(bool enabled) {
    debug_mode_ = enabled;
    qDebug() << "🐛 Debug mode:" << (enabled ? "enabled" : "disabled");
}

void StateManager::enablePerformanceMonitoring(bool enabled) {
    performance_monitoring_ = enabled;
    qDebug() << "⚡ Performance monitoring:" << (enabled ? "enabled" : "disabled");
}

QString StateManager::getPerformanceReport() const {
    QString report = "📊 StateManager Performance Report\n";
    report += "=================================\n";
    report += QString("States count: %1\n").arg(states_.size());
    report += QString("Dependencies count: %1\n").arg(dependencies_.size());
    report += QString("Debug mode: %1\n").arg(debug_mode_ ? "ON" : "OFF");
    report += QString("Performance monitoring: %1\n").arg(performance_monitoring_ ? "ON" : "OFF");
    report += QString("Batching mode: %1\n").arg(batching_ ? "ON" : "OFF");
    return report;
}

}  // namespace DeclarativeUI::Binding

// **Template method implementations**
template<typename T>
void DeclarativeUI::Binding::StateManager::setValidator(const QString& key, std::function<bool(const T&)> validator) {
    auto it = states_.find(key);
    if (it != states_.end()) {
        // Convert typed validator to generic QVariant validator
        it->second.validator = [validator](const QVariant& value) -> bool {
            if (value.canConvert<T>()) {
                return validator(value.value<T>());
            }
            return false; // Invalid type
        };
        qDebug() << "✅ Validator set for state:" << key;
    } else {
        qWarning() << "❌ Cannot set validator: State" << key << "does not exist";
    }
}

// **Explicit template instantiations for common types**
template std::shared_ptr<DeclarativeUI::Binding::ReactiveProperty<QString>>
DeclarativeUI::Binding::StateManager::createState(const QString &key,
                                                  QString initial_value);

template std::shared_ptr<DeclarativeUI::Binding::ReactiveProperty<int>>
DeclarativeUI::Binding::StateManager::createState(const QString &key,
                                                  int initial_value);

template std::shared_ptr<DeclarativeUI::Binding::ReactiveProperty<double>>
DeclarativeUI::Binding::StateManager::createState(const QString &key,
                                                  double initial_value);

template std::shared_ptr<DeclarativeUI::Binding::ReactiveProperty<bool>>
DeclarativeUI::Binding::StateManager::createState(const QString &key,
                                                  bool initial_value);

// **Template method instantiations**
template void DeclarativeUI::Binding::StateManager::setValidator<int>(
    const QString& key, std::function<bool(const int&)> validator);

template void DeclarativeUI::Binding::StateManager::setValidator<QString>(
    const QString& key, std::function<bool(const QString&)> validator);

template void DeclarativeUI::Binding::StateManager::setValidator<double>(
    const QString& key, std::function<bool(const double&)> validator);

template void DeclarativeUI::Binding::StateManager::setValidator<bool>(
    const QString& key, std::function<bool(const bool&)> validator);
