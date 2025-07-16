#include "StateManager.hpp"

#include <QDebug>
#include <QTimer>

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

}  // namespace DeclarativeUI::Binding

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
