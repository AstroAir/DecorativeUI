/**
 * @file StateManager.cpp
 * @brief Implementation of the reactive state management system for
 * DeclarativeUI
 *
 * This file provides a comprehensive state management solution with:
 * - Type-safe reactive properties with automatic change notifications
 * - Computed properties with dependency tracking
 * - Undo/redo functionality with configurable history
 * - Batch updates for performance optimization
 * - State persistence and loading
 * - Thread-safe operations with recursive mutex protection
 * - Performance monitoring and debugging capabilities
 *
 * The implementation emphasizes maintainable code with low cyclomatic
 * complexity by breaking down complex operations into focused helper functions.
 *
 * @author DeclarativeUI Team
 * @version 1.0
 */

#include "StateManager.hpp"

#include <QDebug>
#include <QElapsedTimer>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMutexLocker>
#include <QTimer>
#include <algorithm>

namespace DeclarativeUI::Binding {

// ============================================================================
// STATEMANAGER IMPLEMENTATION
// ============================================================================

StateManager& StateManager::instance() {
    static StateManager instance;
    return instance;
}

void StateManager::batchUpdate(std::function<void()> updates) {
    if (!updates) {
        qWarning() << "Batch update function is null";
        return;
    }

    QMutexLocker locker(&global_lock_);

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

    } catch (const std::exception& e) {
        batching_ = false;
        qWarning() << "Batch update failed:" << e.what();
        throw;
    }
}

void StateManager::clearState() noexcept {
    try {
        QMutexLocker locker(&global_lock_);

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

    for (const auto& update : pending_updates_) {
        try {
            update();
        } catch (const std::exception& e) {
            qWarning() << "Pending update failed:" << e.what();
        }
    }

    pending_updates_.clear();
}

bool StateManager::hasState(const QString& key) const {
    QMutexLocker locker(&global_lock_);
    return states_.find(key) != states_.end();
}

void StateManager::removeState(const QString& key) {
    QMutexLocker locker(&global_lock_);

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
    QMutexLocker locker(&global_lock_);
    auto it = states_.find(key);
    if (it != states_.end()) {
        auto& info = it->second;
        info.history_enabled = true;
        info.max_history_size = max_history_size;

        // Initialize history with current state value
        initializeStateHistory(info, key);

        qDebug() << "📝 History enabled for state:" << key
                 << "with max size:" << max_history_size
                 << "initial position:" << info.history_position;
    } else {
        qWarning() << "❌ Cannot enable history: State" << key
                   << "does not exist";
    }
}

void StateManager::disableHistory(const QString& key) {
    QMutexLocker locker(&global_lock_);
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
    QMutexLocker locker(&global_lock_);
    auto it = states_.find(key);
    if (it != states_.end() && it->second.history_enabled) {
        return it->second.history_position > 0;
    }
    return false;
}

bool StateManager::canRedo(const QString& key) const {
    QMutexLocker locker(&global_lock_);
    auto it = states_.find(key);
    if (it != states_.end() && it->second.history_enabled) {
        return it->second.history_position <
               static_cast<int>(it->second.history.size()) - 1;
    }
    return false;
}

void StateManager::undo(const QString& key) {
    QMutexLocker locker(&global_lock_);
    auto it = states_.find(key);

    if (it != states_.end() && validateUndoOperation(key, it->second)) {
        performUndoOperation(it->second, key);
    }
}

void StateManager::redo(const QString& key) {
    QMutexLocker locker(&global_lock_);
    auto it = states_.find(key);

    if (it != states_.end() && validateRedoOperation(key, it->second)) {
        performRedoOperation(it->second, key);
    }
}

void StateManager::addDependency(const QString& key,
                                 const QString& depends_on) {
    QMutexLocker locker(&global_lock_);
    dependencies_[key].push_back(depends_on);
    dependents_[depends_on].push_back(key);
    qDebug() << "🔗 Dependency added:" << key << "depends on" << depends_on;
}

void StateManager::removeDependency(const QString& key,
                                    const QString& depends_on) {
    QMutexLocker locker(&global_lock_);

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
    qDebug() << "🔗❌ Dependency removed:" << key << "no longer depends on"
             << depends_on;
}

QStringList StateManager::getDependencies(const QString& key) const {
    QMutexLocker locker(&global_lock_);

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
    QMutexLocker locker(&global_lock_);

    auto it = dependents_.find(key);
    if (it != dependents_.end()) {
        // Update computed dependents (with functions)
        updateComputedDependents(key);

        // Update manual dependents (without functions)
        updateManualDependents(key);
    }
}

void StateManager::enableDebugMode(bool enabled) {
    debug_mode_ = enabled;
    qDebug() << "🐛 Debug mode:" << (enabled ? "enabled" : "disabled");
}

void StateManager::addToHistory(const QString& key, const QVariant& value) {
    QMutexLocker locker(&global_lock_);
    auto it = states_.find(key);
    if (it != states_.end() && it->second.history_enabled) {
        auto& info = it->second;

        // Remove any redo history when adding new value
        if (info.history_position < static_cast<int>(info.history.size()) - 1) {
            info.history.erase(info.history.begin() + info.history_position + 1,
                               info.history.end());
        }

        // Add new value to history
        info.history.push_back(value);
        info.history_position = static_cast<int>(info.history.size()) - 1;

        // Limit history size
        while (static_cast<int>(info.history.size()) > info.max_history_size) {
            info.history.pop_front();
            info.history_position--;
        }

        qDebug() << "📝 Added to history:" << key
                 << "position:" << info.history_position
                 << "size:" << info.history.size();
    }
}

void StateManager::enablePerformanceMonitoring(bool enabled) {
    performance_monitoring_ = enabled;
    qDebug() << "⚡ Performance monitoring:"
             << (enabled ? "enabled" : "disabled");
}

QString StateManager::getPerformanceReport() const {
    QString report = "📊 StateManager Performance Report\n";
    report += "=================================\n";
    report += QString("States count: %1\n").arg(states_.size());
    report += QString("Dependencies count: %1\n").arg(dependencies_.size());
    report += QString("Debug mode: %1\n").arg(debug_mode_ ? "ON" : "OFF");
    report += QString("Performance monitoring: %1\n")
                  .arg(performance_monitoring_ ? "ON" : "OFF");
    report += QString("Batching mode: %1\n").arg(batching_ ? "ON" : "OFF");

    // Add individual state information
    if (!states_.empty()) {
        report += "\nState Details:\n";
        for (const auto& [key, info] : states_) {
            report +=
                QString("- %1: %2 updates\n").arg(key).arg(info.update_count);
        }
    }

    return report;
}

void StateManager::saveState(const QString& filename) const {
    QMutexLocker locker(&global_lock_);

    // Create root JSON object with metadata
    QJsonObject rootObject;
    rootObject["version"] = "1.0";
    rootObject["timestamp"] =
        QDateTime::currentDateTime().toString(Qt::ISODate);

    // Add state data and dependencies
    rootObject["states"] = createStateDataJson();
    rootObject["dependencies"] = createDependenciesJson();

    // Write to file
    if (!writeJsonToFile(rootObject, filename)) {
        qWarning() << "❌ Failed to save state to:" << filename;
    }
}

/**
 * @brief Loads state data from a JSON file
 * @param filename Path to the state file to load
 *
 * This function handles file reading, JSON parsing, version validation,
 * and state restoration in a structured manner.
 */
void StateManager::loadState(const QString& filename) {
    // **Read and parse JSON file**
    QJsonObject rootObject = readStateFile(filename);
    if (rootObject.isEmpty()) {
        return;  // Error already logged in readStateFile
    }

    // **Validate file version**
    if (!validateStateFileVersion(rootObject)) {
        return;  // Error already logged in validateStateFileVersion
    }

    QMutexLocker locker(&global_lock_);

    // **Load state data and dependencies**
    loadStateData(rootObject["states"].toObject());
    loadStateDependencies(rootObject["dependencies"].toObject());

    qDebug() << "📂 State loaded from:" << filename;
}

/**
 * @brief Reads and parses a state file into a JSON object
 * @param filename Path to the state file
 * @return Parsed JSON object or empty object on error
 */
QJsonObject StateManager::readStateFile(const QString& filename) const {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "❌ Failed to load state from:" << filename;
        return QJsonObject();
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);

    if (error.error != QJsonParseError::NoError) {
        qWarning() << "❌ JSON parse error:" << error.errorString();
        return QJsonObject();
    }

    return doc.object();
}

/**
 * @brief Validates the version of a state file
 * @param rootObject The root JSON object from the state file
 * @return true if version is supported, false otherwise
 */
bool StateManager::validateStateFileVersion(
    const QJsonObject& rootObject) const {
    QString version = rootObject["version"].toString();
    if (version != "1.0") {
        qWarning() << "❌ Unsupported state file version:" << version;
        return false;
    }
    return true;
}

/**
 * @brief Loads state data from a JSON object
 * @param statesObject JSON object containing state data
 */
void StateManager::loadStateData(const QJsonObject& statesObject) {
    for (auto it = statesObject.begin(); it != statesObject.end(); ++it) {
        QString key = it.key();
        QJsonObject stateEntry = it.value().toObject();
        QVariant value = parseStateValue(stateEntry);
        QString type = stateEntry["type"].toString();

        // Create the appropriate ReactiveProperty based on the type
        if (type == "QString") {
            createState<QString>(key, value.toString());
        } else if (type == "int") {
            createState<int>(key, value.toInt());
        } else if (type == "double") {
            createState<double>(key, value.toDouble());
        } else if (type == "bool") {
            createState<bool>(key, value.toBool());
        } else {
            qWarning() << "Unknown state type during load:" << type
                       << "for key:" << key;
        }
    }
}

/**
 * @brief Loads state dependencies from a JSON object
 * @param dependenciesObject JSON object containing dependency data
 */
void StateManager::loadStateDependencies(
    const QJsonObject& dependenciesObject) {
    for (auto it = dependenciesObject.begin(); it != dependenciesObject.end();
         ++it) {
        QString key = it.key();
        QJsonArray depsArray = it.value().toArray();

        std::vector<QString> deps;
        for (const auto& depValue : depsArray) {
            deps.push_back(depValue.toString());
        }
        dependencies_[key] = deps;
    }
}

/**
 * @brief Parses a state value from a JSON state entry
 * @param stateEntry JSON object containing type and value information
 * @return Parsed QVariant value
 */
QVariant StateManager::parseStateValue(const QJsonObject& stateEntry) const {
    QString type = stateEntry["type"].toString();

    if (type == "QString") {
        return stateEntry["value"].toString();
    } else if (type == "int") {
        return stateEntry["value"].toInt();
    } else if (type == "double") {
        return stateEntry["value"].toDouble();
    } else if (type == "bool") {
        return stateEntry["value"].toBool();
    }

    return QVariant();  // Return invalid variant for unknown types
}

void StateManager::logStateChange(const QString& key, const QVariant& oldValue,
                                  const QVariant& newValue) {
    if (!debug_mode_) {
        return;
    }

    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    qDebug() << QString("[%1] State changed: %2 | %3 -> %4")
                    .arg(timestamp)
                    .arg(key)
                    .arg(oldValue.toString())
                    .arg(newValue.toString());
}

void StateManager::validateState(const QString& key, const QVariant& value) {
    QMutexLocker locker(&global_lock_);
    auto it = states_.find(key);
    if (it != states_.end() && it->second.validator) {
        if (!it->second.validator(value)) {
            qWarning() << "❌ Validation failed for state:" << key
                       << "value:" << value;
            throw std::invalid_argument(
                QString("Validation failed for state: %1")
                    .arg(key)
                    .toStdString());
        }
    }
}

void StateManager::measurePerformance(const QString& key,
                                      std::function<void()> operation) {
    if (!performance_monitoring_ || !operation) {
        if (operation) {
            operation();
        }
        return;
    }

    QElapsedTimer timer;
    timer.start();

    try {
        operation();
    } catch (...) {
        qint64 elapsed = timer.elapsed();
        qWarning() << "⚡ Performance measurement failed for state:" << key
                   << "time:" << elapsed << "ms";
        throw;
    }

    qint64 elapsed = timer.elapsed();

    // Log performance if it exceeds threshold
    const qint64 performanceThreshold = 10;  // 10ms threshold
    if (elapsed > performanceThreshold) {
        qWarning() << "⚡ Performance warning for state:" << key
                   << "time:" << elapsed << "ms";
        emit performanceWarning(key, elapsed);
    }

    if (debug_mode_) {
        qDebug() << "⚡ Performance measurement for state:" << key
                 << "time:" << elapsed << "ms";
    }
}

// ============================================================================
// HELPER FUNCTION IMPLEMENTATIONS
// ============================================================================

QVariant StateManager::getCurrentStateValue(
    const std::shared_ptr<ReactivePropertyBase>& state) const {
    if (!state) {
        return QVariant();
    }

    // Try to cast to known types and extract values
    if (auto stringState =
            std::dynamic_pointer_cast<ReactiveProperty<QString>>(state)) {
        return QVariant::fromValue(stringState->get());
    } else if (auto intState =
                   std::dynamic_pointer_cast<ReactiveProperty<int>>(state)) {
        return QVariant::fromValue(intState->get());
    } else if (auto doubleState =
                   std::dynamic_pointer_cast<ReactiveProperty<double>>(state)) {
        return QVariant::fromValue(doubleState->get());
    } else if (auto boolState =
                   std::dynamic_pointer_cast<ReactiveProperty<bool>>(state)) {
        return QVariant::fromValue(boolState->get());
    }

    return QVariant();  // Unsupported type
}

bool StateManager::applyValueToState(
    const std::shared_ptr<ReactivePropertyBase>& state, const QVariant& value) {
    if (!state || !value.isValid()) {
        return false;
    }

    // Try to apply value based on type
    if (value.canConvert<QString>()) {
        if (auto reactiveState =
                std::dynamic_pointer_cast<ReactiveProperty<QString>>(state)) {
            reactiveState->set(value.toString());
            return true;
        }
    } else if (value.canConvert<int>()) {
        if (auto reactiveState =
                std::dynamic_pointer_cast<ReactiveProperty<int>>(state)) {
            reactiveState->set(value.toInt());
            return true;
        }
    } else if (value.canConvert<double>()) {
        if (auto reactiveState =
                std::dynamic_pointer_cast<ReactiveProperty<double>>(state)) {
            reactiveState->set(value.toDouble());
            return true;
        }
    } else if (value.canConvert<bool>()) {
        if (auto reactiveState =
                std::dynamic_pointer_cast<ReactiveProperty<bool>>(state)) {
            reactiveState->set(value.toBool());
            return true;
        }
    }

    return false;  // Unsupported type or conversion failed
}

void StateManager::initializeStateHistory(StateInfo& info, const QString& key) {
    info.history.clear();

    if (info.state) {
        // Cast QObject to ReactivePropertyBase
        auto reactiveState =
            std::dynamic_pointer_cast<ReactivePropertyBase>(info.state);
        if (reactiveState) {
            QVariant currentValue = getCurrentStateValue(reactiveState);
            if (currentValue.isValid()) {
                info.history.push_back(currentValue);
                info.history_position = 0;
                qDebug() << "📝 History initialized for state:" << key
                         << "with current value:" << currentValue;
            } else {
                info.history_position = -1;
                qDebug() << "⚠️ History initialized for state:" << key
                         << "but current value is invalid";
            }
        } else {
            info.history_position = -1;
            qDebug() << "⚠️ History initialized for state:" << key
                     << "but state is not ReactivePropertyBase";
        }
    } else {
        info.history_position = -1;
        qDebug() << "⚠️ History initialized for state:" << key
                 << "but state is null";
    }
}

bool StateManager::validateHistoryPosition(const StateInfo& info,
                                           int position) const {
    return position >= 0 && position < static_cast<int>(info.history.size());
}

bool StateManager::applyHistoryValue(const StateInfo& info,
                                     const QVariant& value,
                                     const QString& key) {
    if (!info.state || !value.isValid()) {
        qWarning() << "❌ Cannot apply history value for state:" << key
                   << "- invalid state or value";
        return false;
    }

    // Cast QObject to ReactivePropertyBase
    auto reactiveState =
        std::dynamic_pointer_cast<ReactivePropertyBase>(info.state);
    if (!reactiveState) {
        qWarning() << "❌ Cannot apply history value for state:" << key
                   << "- state is not ReactivePropertyBase";
        return false;
    }

    // Apply the value using the helper function
    bool success = applyValueToState(reactiveState, value);
    if (success) {
        qDebug() << "✅ History value applied to state:" << key
                 << "value:" << value;
    } else {
        qWarning() << "❌ Failed to apply history value to state:" << key
                   << "value:" << value;
    }

    return success;
}

// ============================================================================
// REFACTORED HELPER METHODS FOR REDUCED COMPLEXITY
// ============================================================================

bool StateManager::validateUndoOperation(const QString& key,
                                         StateInfo& info) const {
    return info.history_enabled && info.history_position > 0;
}

bool StateManager::validateRedoOperation(const QString& key,
                                         StateInfo& info) const {
    return info.history_enabled &&
           info.history_position < static_cast<int>(info.history.size()) - 1;
}

void StateManager::performUndoOperation(StateInfo& info, const QString& key) {
    info.history_position--;

    if (validateHistoryPosition(info, info.history_position)) {
        auto value = info.history[info.history_position];

        // Apply the history value using helper function
        if (applyHistoryValue(info, value, key)) {
            emit stateChanged(key, value);
            qDebug() << "↶ Undo applied to state:" << key
                     << "to position:" << info.history_position;
        } else {
            // Revert position if application failed
            info.history_position++;
            qWarning() << "❌ Undo failed for state:" << key
                       << "- reverting position";
        }
    } else {
        // Revert position if validation failed
        info.history_position++;
        qWarning() << "❌ Undo failed for state:" << key
                   << "- invalid history position";
    }
}

void StateManager::performRedoOperation(StateInfo& info, const QString& key) {
    info.history_position++;

    if (validateHistoryPosition(info, info.history_position)) {
        auto value = info.history[info.history_position];

        // Apply the history value using helper function
        if (applyHistoryValue(info, value, key)) {
            emit stateChanged(key, value);
            qDebug() << "↷ Redo applied to state:" << key
                     << "to position:" << info.history_position;
        } else {
            // Revert position if application failed
            info.history_position--;
            qWarning() << "❌ Redo failed for state:" << key
                       << "- reverting position";
        }
    } else {
        // Revert position if validation failed
        info.history_position--;
        qWarning() << "❌ Redo failed for state:" << key
                   << "- invalid history position";
    }
}

/**
 * @brief Updates computed dependent states when a dependency changes
 * @param key The key of the state that changed
 *
 * This function processes all computed dependents of the changed state,
 * recomputing their values and triggering cascading updates.
 */
void StateManager::updateComputedDependents(const QString& key) {
    auto it = dependents_.find(key);
    if (it == dependents_.end())
        return;

    for (const auto& dependent : it->second) {
        updateSingleComputedDependent(dependent);
    }
}

/**
 * @brief Updates a single computed dependent state
 * @param dependent The key of the dependent state to update
 */
void StateManager::updateSingleComputedDependent(const QString& dependent) {
    qDebug() << "🔄 Updating computed dependent state:" << dependent;

    auto dependentIt = state_data_.find(dependent);
    if (dependentIt == state_data_.end()) {
        return;
    }

    auto computedIt = computed_values_.find(dependent);
    if (computedIt == computed_values_.end()) {
        return;  // Not a computed state
    }

    try {
        QVariant newValue = computedIt->second();
        QVariant oldValue = dependentIt->second;

        if (shouldUpdateComputedValue(newValue, oldValue)) {
            applyComputedValueUpdate(dependent, newValue, oldValue);
        }
    } catch (const std::exception& e) {
        qWarning() << "❌ Error updating computed dependent state" << dependent
                   << ":" << e.what();
    }
}

/**
 * @brief Checks if a computed value should be updated
 * @param newValue The newly computed value
 * @param oldValue The current value
 * @return true if the value should be updated, false otherwise
 */
bool StateManager::shouldUpdateComputedValue(const QVariant& newValue,
                                             const QVariant& oldValue) const {
    return newValue != oldValue;
}

/**
 * @brief Applies a computed value update and triggers cascading updates
 * @param dependent The key of the dependent state
 * @param newValue The new computed value
 * @param oldValue The previous value
 */
void StateManager::applyComputedValueUpdate(const QString& dependent,
                                            const QVariant& newValue,
                                            const QVariant& oldValue) {
    // Update the state data
    state_data_[dependent] = newValue;

    // Emit change signal for the dependent state
    emit stateChanged(dependent, newValue);

    // Recursively update dependents of this dependent
    updateDependents(dependent);

    qDebug() << "✅ Computed dependent state updated:" << dependent << "from"
             << oldValue << "to" << newValue;
}

void StateManager::updateManualDependents(const QString& key) {
    auto it = dependents_.find(key);
    if (it == dependents_.end())
        return;

    for (const auto& dependent : it->second) {
        // Trigger recomputation of dependent states
        auto dependentIt = state_data_.find(dependent);
        if (dependentIt != state_data_.end()) {
            // Check if this dependent state has no computed value function
            auto computedIt = computed_values_.find(dependent);
            if (computedIt == computed_values_.end()) {
                // If no computed function, just emit a change notification
                // This allows manual dependent updates
                emit stateChanged(dependent, dependentIt->second);
                qDebug() << "🔄 Manual dependent state notified:" << dependent;
            }
        }
    }
}

QJsonObject StateManager::createStateDataJson() const {
    QJsonObject statesObject;

    // Save state data from the actual ReactiveProperty objects
    for (const auto& [key, stateInfo] : states_) {
        // Cast QObject to ReactivePropertyBase
        auto reactiveState =
            std::dynamic_pointer_cast<ReactivePropertyBase>(stateInfo.state);
        if (reactiveState) {
            QVariant currentValue = getCurrentStateValue(reactiveState);
            if (currentValue.isValid()) {
                statesObject[key] = convertVariantToJson(currentValue);
            }
        }
    }

    return statesObject;
}

QJsonObject StateManager::createDependenciesJson() const {
    QJsonObject dependenciesObject;

    for (const auto& [key, deps] : dependencies_) {
        QJsonArray depsArray;
        for (const auto& dep : deps) {
            depsArray.append(dep);
        }
        dependenciesObject[key] = depsArray;
    }

    return dependenciesObject;
}

QJsonObject StateManager::convertVariantToJson(const QVariant& value) const {
    QJsonObject stateEntry;

    // Convert QVariant to JSON-compatible format
    if (value.canConvert<QString>()) {
        stateEntry["value"] = value.toString();
        stateEntry["type"] = "QString";
    } else if (value.canConvert<int>()) {
        stateEntry["value"] = value.toInt();
        stateEntry["type"] = "int";
    } else if (value.canConvert<double>()) {
        stateEntry["value"] = value.toDouble();
        stateEntry["type"] = "double";
    } else if (value.canConvert<bool>()) {
        stateEntry["value"] = value.toBool();
        stateEntry["type"] = "bool";
    } else {
        // Fallback to string representation
        stateEntry["value"] = value.toString();
        stateEntry["type"] = "QString";
    }

    return stateEntry;
}

bool StateManager::writeJsonToFile(const QJsonObject& rootObject,
                                   const QString& filename) const {
    QJsonDocument doc(rootObject);
    QFile file(filename);

    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
        qDebug() << "💾 State saved to:" << filename;
        return true;
    }

    return false;
}

}  // namespace DeclarativeUI::Binding

// ============================================================================
// TEMPLATE METHOD IMPLEMENTATIONS
// ============================================================================
template <typename T>
void DeclarativeUI::Binding::StateManager::setValidator(
    const QString& key, std::function<bool(const T&)> validator) {
    QMutexLocker locker(&global_lock_);
    auto it = states_.find(key);
    if (it != states_.end()) {
        // Convert typed validator to generic QVariant validator
        it->second.validator = [validator](const QVariant& value) -> bool {
            if (value.canConvert<T>()) {
                return validator(value.value<T>());
            }
            return false;  // Invalid type
        };
        qDebug() << "✅ Validator set for state:" << key;
    } else {
        qWarning() << "❌ Cannot set validator: State" << key
                   << "does not exist";
    }
}

// **Explicit template instantiations for common types**
template std::shared_ptr<DeclarativeUI::Binding::ReactiveProperty<QString>>
DeclarativeUI::Binding::StateManager::createState(const QString& key,
                                                  QString initial_value);

template std::shared_ptr<DeclarativeUI::Binding::ReactiveProperty<int>>
DeclarativeUI::Binding::StateManager::createState(const QString& key,
                                                  int initial_value);

template std::shared_ptr<DeclarativeUI::Binding::ReactiveProperty<double>>
DeclarativeUI::Binding::StateManager::createState(const QString& key,
                                                  double initial_value);

template std::shared_ptr<DeclarativeUI::Binding::ReactiveProperty<bool>>
DeclarativeUI::Binding::StateManager::createState(const QString& key,
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
