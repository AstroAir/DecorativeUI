/**
 * @file ConditionalRendering.cpp
 * @brief Implementation of conditional rendering system for DeclarativeUI
 */

#include "ConditionalRendering.hpp"
#include "../Exceptions/UIExceptions.hpp"
#include "DeclarativeBuilder.hpp"

#include <QDebug>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QTimer>
#include <QVBoxLayout>
#include <algorithm>

namespace DeclarativeUI::Core {

// ConditionalItem implementation
ConditionalItem::ConditionalItem(QObject* parent) : QObject(parent) {
    qDebug() << "🔀 ConditionalItem created";
}

void ConditionalItem::setCondition(ConditionFunction condition) {
    condition_ = std::move(condition);
}

void ConditionalItem::setAsyncCondition(AsyncConditionFunction condition) {
    async_condition_ = std::move(condition);
}

void ConditionalItem::setWidgetFactory(WidgetFactory factory) {
    widget_factory_ = std::move(factory);
}

QWidget* ConditionalItem::getWidget() {
    if (!widget_ && widget_factory_) {
        createWidget();
    }
    return widget_;
}

void ConditionalItem::clearWidget() {
    if (widget_) {
        disconnect(widget_, &QWidget::destroyed, this,
                   &ConditionalItem::onWidgetDestroyed);
        widget_->deleteLater();
        widget_ = nullptr;
    }
}

ConditionResult ConditionalItem::evaluateCondition() {
    if (condition_) {
        try {
            return condition_();
        } catch (const std::exception& e) {
            qWarning() << "🔀 Condition evaluation failed:" << e.what();
            return ConditionResult::False;
        }
    }
    return ConditionResult::False;
}

void ConditionalItem::evaluateAsync(
    std::function<void(ConditionResult)> callback) {
    if (async_condition_) {
        try {
            async_condition_(std::move(callback));
        } catch (const std::exception& e) {
            qWarning() << "🔀 Async condition evaluation failed:" << e.what();
            callback(ConditionResult::False);
        }
    } else {
        // Fallback to synchronous evaluation
        callback(evaluateCondition());
    }
}

void ConditionalItem::setVisible(bool visible) {
    if (is_visible_ != visible) {
        is_visible_ = visible;

        if (widget_) {
            if (config_.animate_transitions) {
                // Animate visibility change
                auto effect = new QGraphicsOpacityEffect();
                widget_->setGraphicsEffect(effect);

                auto animation = new QPropertyAnimation(effect, "opacity");
                animation->setDuration(config_.animation_duration.count());
                animation->setStartValue(visible ? 0.0 : 1.0);
                animation->setEndValue(visible ? 1.0 : 0.0);

                connect(animation, &QPropertyAnimation::finished,
                        [this, visible]() {
                            if (widget_) {
                                widget_->setVisible(visible);
                                if (visible) {
                                    widget_->setGraphicsEffect(nullptr);
                                }
                            }
                        });

                animation->start(QAbstractAnimation::DeleteWhenStopped);
            } else {
                widget_->setVisible(visible);
            }
        }

        emit visibilityChanged(visible);
    }
}

void ConditionalItem::onWidgetDestroyed() { widget_ = nullptr; }

void ConditionalItem::createWidget() {
    if (widget_factory_) {
        try {
            widget_ = widget_factory_().release();
            if (widget_) {
                connect(widget_, &QWidget::destroyed, this,
                        &ConditionalItem::onWidgetDestroyed);
                emit widgetCreated(widget_);
                qDebug() << "🔀 Widget created for conditional item";
            }
        } catch (const std::exception& e) {
            qWarning() << "🔀 Widget creation failed:" << e.what();
        }
    }
}

// ConditionalRenderer implementation
ConditionalRenderer::ConditionalRenderer(QObject* parent) : UIElement(parent) {
    // Initialize debounce timer
    debounce_timer_ = new QTimer(this);
    debounce_timer_->setSingleShot(true);
    connect(debounce_timer_, &QTimer::timeout, this,
            &ConditionalRenderer::onDebounceTimer);

    qDebug() << "🔀 ConditionalRenderer created";
}

ConditionalItem* ConditionalRenderer::addCondition(ConditionFunction condition,
                                                   WidgetFactory factory) {
    auto item = std::make_unique<ConditionalItem>(this);
    item->setCondition(std::move(condition));
    item->setWidgetFactory(std::move(factory));
    item->setConfig(config_);

    connect(item.get(), &ConditionalItem::conditionChanged, this,
            &ConditionalRenderer::onConditionChanged);

    auto* item_ptr = item.get();
    items_.push_back(std::move(item));

    qDebug() << "🔀 Condition added, total conditions:" << items_.size();
    return item_ptr;
}

ConditionalItem* ConditionalRenderer::addAsyncCondition(
    AsyncConditionFunction condition, WidgetFactory factory) {
    auto item = std::make_unique<ConditionalItem>(this);
    item->setAsyncCondition(std::move(condition));
    item->setWidgetFactory(std::move(factory));
    item->setConfig(config_);

    connect(item.get(), &ConditionalItem::conditionChanged, this,
            &ConditionalRenderer::onConditionChanged);

    auto* item_ptr = item.get();
    items_.push_back(std::move(item));

    qDebug() << "🔀 Async condition added, total conditions:" << items_.size();
    return item_ptr;
}

ConditionalItem* ConditionalRenderer::addElse(WidgetFactory factory) {
    // "Else" is always true
    return addCondition([]() { return ConditionResult::True; },
                        std::move(factory));
}

void ConditionalRenderer::evaluateConditions() {
    if (evaluation_pending_) {
        return;
    }

    evaluation_pending_ = true;

    for (auto& item : items_) {
        ConditionResult result = item->evaluateCondition();
        if (result == ConditionResult::True) {
            switchToItem(item.get());
            evaluation_pending_ = false;
            emit conditionsEvaluated();
            return;
        }
    }

    // No condition was true, hide all
    switchToItem(nullptr);
    evaluation_pending_ = false;
    emit conditionsEvaluated();
}

void ConditionalRenderer::evaluateConditionsAsync() {
    if (evaluation_pending_) {
        return;
    }

    evaluation_pending_ = true;

    // Evaluate conditions asynchronously
    evaluateConditionsAsyncRecursive(0);
}

void ConditionalRenderer::render() {
    if (!getWidget()) {
        qWarning() << "🔀 Cannot render: no container widget";
        return;
    }

    auto layout = getWidget()->layout();
    if (!layout) {
        layout = new QVBoxLayout(getWidget());
        layout->setContentsMargins(0, 0, 0, 0);
    }

    // Add current item's widget to layout
    if (current_item_ && current_item_->getWidget()) {
        layout->addWidget(current_item_->getWidget());
        current_item_->setVisible(true);
    }

    emit renderingComplete();
}

void ConditionalRenderer::setConfig(const ConditionalConfig& config) {
    config_ = config;

    // Update debounce timer
    debounce_timer_->setInterval(config_.debounce_delay.count());

    // Update all items
    for (auto& item : items_) {
        item->setConfig(config_);
    }
}

void ConditionalRenderer::bindToState(const QString& state_key) {
    bound_state_keys_.clear();
    bound_state_keys_.push_back(state_key);
    setupStateBindings();
}

void ConditionalRenderer::bindToStates(const std::vector<QString>& state_keys) {
    bound_state_keys_ = state_keys;
    setupStateBindings();
}

void ConditionalRenderer::initialize() {
    auto container = std::make_unique<QWidget>();
    auto layout = new QVBoxLayout(container.get());
    layout->setContentsMargins(0, 0, 0, 0);

    setWidget(container.release());

    // Initial evaluation
    if (config_.reactive) {
        evaluateConditions();
        render();
    }
}

void ConditionalRenderer::onConditionChanged() {
    if (config_.reactive) {
        if (config_.debounce_delay.count() > 0) {
            debounce_timer_->start();
        } else {
            evaluateConditions();
            render();
        }
    }
}

void ConditionalRenderer::onStateChanged() { onConditionChanged(); }

void ConditionalRenderer::onDebounceTimer() {
    evaluateConditions();
    render();
}

void ConditionalRenderer::setupStateBindings() {
    if (!config_.reactive) {
        return;
    }

    auto& state_manager = Binding::StateManager::instance();

    for (const auto& state_key : bound_state_keys_) {
        // Connect to state changes
        // Note: This is a simplified implementation
        // In a real implementation, you'd need to connect to state change
        // signals
        qDebug() << "🔀 Bound to state:" << state_key;
    }
}

void ConditionalRenderer::updateFromState() {
    if (config_.reactive) {
        evaluateConditions();
        render();
    }
}

void ConditionalRenderer::switchToItem(ConditionalItem* item) {
    if (current_item_ == item) {
        return;
    }

    // Hide current item
    if (current_item_) {
        current_item_->setVisible(false);
        if (current_item_->getWidget() && getWidget() &&
            getWidget()->layout()) {
            getWidget()->layout()->removeWidget(current_item_->getWidget());
        }
    }

    current_item_ = item;

    qDebug() << "🔀 Switched to" << (item ? "new item" : "no item");
}

void ConditionalRenderer::hideAllItems() {
    for (auto& item : items_) {
        item->setVisible(false);
    }
    current_item_ = nullptr;
}

void ConditionalRenderer::animateTransition(QWidget* from, QWidget* to) {
    if (!config_.animate_transitions) {
        return;
    }

    // Simple fade transition
    if (from) {
        auto effect = new QGraphicsOpacityEffect();
        from->setGraphicsEffect(effect);

        auto animation = new QPropertyAnimation(effect, "opacity");
        animation->setDuration(config_.animation_duration.count() / 2);
        animation->setStartValue(1.0);
        animation->setEndValue(0.0);

        connect(animation, &QPropertyAnimation::finished,
                [from]() { from->hide(); });

        animation->start(QAbstractAnimation::DeleteWhenStopped);
    }

    if (to) {
        to->show();

        auto effect = new QGraphicsOpacityEffect();
        to->setGraphicsEffect(effect);

        auto animation = new QPropertyAnimation(effect, "opacity");
        animation->setDuration(config_.animation_duration.count() / 2);
        animation->setStartValue(0.0);
        animation->setEndValue(1.0);

        connect(animation, &QPropertyAnimation::finished,
                [to]() { to->setGraphicsEffect(nullptr); });

        animation->start(QAbstractAnimation::DeleteWhenStopped);
    }
}

void ConditionalRenderer::evaluateConditionsAsyncRecursive(size_t index) {
    if (index >= items_.size()) {
        // No condition was true, hide all
        switchToItem(nullptr);
        evaluation_pending_ = false;
        emit conditionsEvaluated();
        return;
    }

    auto& item = items_[index];
    item->evaluateAsync([this, index](ConditionResult result) {
        if (result == ConditionResult::True) {
            switchToItem(items_[index].get());
            evaluation_pending_ = false;
            emit conditionsEvaluated();
        } else {
            // Try next condition
            evaluateConditionsAsyncRecursive(index + 1);
        }
    });
}

// ConditionalBuilder implementation
ConditionalBuilder::ConditionalBuilder(ConditionalRenderer* renderer) {
    if (renderer) {
        renderer_ = std::unique_ptr<ConditionalRenderer>(renderer);
    } else {
        renderer_ = std::make_unique<ConditionalRenderer>();
    }
}

ConditionalBuilder& ConditionalBuilder::when(ConditionFunction condition,
                                             WidgetFactory factory) {
    renderer_->addCondition(std::move(condition), std::move(factory));
    return *this;
}

ConditionalBuilder& ConditionalBuilder::whenAsync(
    AsyncConditionFunction condition, WidgetFactory factory) {
    renderer_->addAsyncCondition(std::move(condition), std::move(factory));
    return *this;
}

ConditionalBuilder& ConditionalBuilder::otherwise(WidgetFactory factory) {
    renderer_->addElse(std::move(factory));
    return *this;
}

ConditionalBuilder& ConditionalBuilder::whenState(
    const QString& state_key, const QVariant& expected_value,
    WidgetFactory factory) {
    auto condition = [state_key, expected_value]() -> ConditionResult {
        auto& state_manager = Binding::StateManager::instance();
        try {
            auto state = state_manager.getState<QVariant>(state_key);
            if (state && state->get() == expected_value) {
                return ConditionResult::True;
            }
        } catch (const std::exception& e) {
            qWarning() << "🔀 State condition evaluation failed:" << e.what();
        }
        return ConditionResult::False;
    };

    return when(condition, std::move(factory));
}

ConditionalBuilder& ConditionalBuilder::whenStateTrue(const QString& state_key,
                                                      WidgetFactory factory) {
    return whenState(state_key, QVariant(true), std::move(factory));
}

ConditionalBuilder& ConditionalBuilder::whenStateFalse(const QString& state_key,
                                                       WidgetFactory factory) {
    return whenState(state_key, QVariant(false), std::move(factory));
}

ConditionalBuilder& ConditionalBuilder::whenAll(
    const std::vector<ConditionFunction>& conditions, WidgetFactory factory) {
    auto combined_condition = [conditions]() -> ConditionResult {
        for (const auto& condition : conditions) {
            if (condition() != ConditionResult::True) {
                return ConditionResult::False;
            }
        }
        return ConditionResult::True;
    };

    return when(combined_condition, std::move(factory));
}

ConditionalBuilder& ConditionalBuilder::whenAny(
    const std::vector<ConditionFunction>& conditions, WidgetFactory factory) {
    auto combined_condition = [conditions]() -> ConditionResult {
        for (const auto& condition : conditions) {
            if (condition() == ConditionResult::True) {
                return ConditionResult::True;
            }
        }
        return ConditionResult::False;
    };

    return when(combined_condition, std::move(factory));
}

ConditionalBuilder& ConditionalBuilder::config(
    const ConditionalConfig& config) {
    config_ = config;
    return *this;
}

ConditionalBuilder& ConditionalBuilder::reactive(bool enabled) {
    config_.reactive = enabled;
    return *this;
}

ConditionalBuilder& ConditionalBuilder::animated(bool enabled) {
    config_.animate_transitions = enabled;
    return *this;
}

ConditionalBuilder& ConditionalBuilder::lazy(bool enabled) {
    config_.lazy_creation = enabled;
    return *this;
}

std::unique_ptr<ConditionalRenderer> ConditionalBuilder::build() {
    renderer_->setConfig(config_);
    return std::move(renderer_);
}

ConditionalRenderer* ConditionalBuilder::buildInto(
    ConditionalRenderer* renderer) {
    if (renderer) {
        renderer->setConfig(config_);
        // Transfer items from our renderer to the target renderer
        // This is a simplified implementation
    }
    return renderer;
}

// Conditional namespace implementation
namespace Conditional {

std::unique_ptr<ConditionalRenderer> when(ConditionFunction condition,
                                          WidgetFactory factory) {
    return ConditionalBuilder()
        .when(std::move(condition), std::move(factory))
        .build();
}

std::unique_ptr<ConditionalRenderer> whenElse(ConditionFunction condition,
                                              WidgetFactory true_factory,
                                              WidgetFactory false_factory) {
    return ConditionalBuilder()
        .when(std::move(condition), std::move(true_factory))
        .otherwise(std::move(false_factory))
        .build();
}

std::unique_ptr<ConditionalRenderer> whenState(const QString& state_key,
                                               const QVariant& expected_value,
                                               WidgetFactory factory) {
    return ConditionalBuilder()
        .whenState(state_key, expected_value, std::move(factory))
        .build();
}

std::unique_ptr<ConditionalRenderer> switchOn(
    const std::vector<std::pair<ConditionFunction, WidgetFactory>>& cases,
    WidgetFactory default_factory) {
    ConditionalBuilder builder;

    for (const auto& [condition, factory] : cases) {
        builder.when(condition, factory);
    }

    if (default_factory) {
        builder.otherwise(std::move(default_factory));
    }

    return builder.build();
}

std::unique_ptr<ConditionalRenderer> whenAsync(AsyncConditionFunction condition,
                                               WidgetFactory factory) {
    return ConditionalBuilder()
        .whenAsync(std::move(condition), std::move(factory))
        .build();
}

}  // namespace Conditional

// Conditions namespace implementation
namespace Conditions {

ConditionFunction stateEquals(const QString& state_key, const QVariant& value) {
    return [state_key, value]() -> ConditionResult {
        auto& state_manager = Binding::StateManager::instance();
        try {
            auto state = state_manager.getState<QVariant>(state_key);
            if (state && state->get() == value) {
                return ConditionResult::True;
            }
        } catch (const std::exception& e) {
            qWarning() << "🔀 State condition failed:" << e.what();
        }
        return ConditionResult::False;
    };
}

ConditionFunction stateTrue(const QString& state_key) {
    return stateEquals(state_key, QVariant(true));
}

ConditionFunction stateFalse(const QString& state_key) {
    return stateEquals(state_key, QVariant(false));
}

ConditionFunction stateExists(const QString& state_key) {
    return [state_key]() -> ConditionResult {
        auto& state_manager = Binding::StateManager::instance();
        try {
            auto state = state_manager.getState<QVariant>(state_key);
            return state ? ConditionResult::True : ConditionResult::False;
        } catch (const std::exception&) {
            return ConditionResult::False;
        }
    };
}

ConditionFunction and_(const std::vector<ConditionFunction>& conditions) {
    return [conditions]() -> ConditionResult {
        for (const auto& condition : conditions) {
            if (condition() != ConditionResult::True) {
                return ConditionResult::False;
            }
        }
        return ConditionResult::True;
    };
}

ConditionFunction or_(const std::vector<ConditionFunction>& conditions) {
    return [conditions]() -> ConditionResult {
        for (const auto& condition : conditions) {
            if (condition() == ConditionResult::True) {
                return ConditionResult::True;
            }
        }
        return ConditionResult::False;
    };
}

ConditionFunction not_(ConditionFunction condition) {
    return [condition]() -> ConditionResult {
        ConditionResult result = condition();
        return (result == ConditionResult::True) ? ConditionResult::False
                                                 : ConditionResult::True;
    };
}

ConditionFunction stringEmpty(std::function<QString()> getter) {
    return [getter]() -> ConditionResult {
        try {
            QString value = getter();
            return value.isEmpty() ? ConditionResult::True
                                   : ConditionResult::False;
        } catch (const std::exception& e) {
            qWarning() << "🔀 String condition failed:" << e.what();
            return ConditionResult::False;
        }
    };
}

ConditionFunction stringContains(std::function<QString()> getter,
                                 const QString& substring) {
    return [getter, substring]() -> ConditionResult {
        try {
            QString value = getter();
            return value.contains(substring) ? ConditionResult::True
                                             : ConditionResult::False;
        } catch (const std::exception& e) {
            qWarning() << "🔀 String condition failed:" << e.what();
            return ConditionResult::False;
        }
    };
}

}  // namespace Conditions

}  // namespace DeclarativeUI::Core
