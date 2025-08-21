#pragma once

/**
 * @file ConditionalRendering.hpp
 * @brief Conditional rendering system for DeclarativeUI components
 *
 * This header provides a comprehensive conditional rendering system that allows
 * components to be shown/hidden based on conditions, similar to React's
 * conditional rendering. The system integrates with the existing
 * DeclarativeBuilder and state management systems to provide reactive
 * conditional rendering.
 */

#include <QObject>
#include <QString>
#include <QTimer>
#include <QWidget>
#include <functional>
#include <memory>
#include <unordered_map>
#include <variant>
#include <vector>

#include "../Binding/StateManager.hpp"
#include "UIElement.hpp"

namespace DeclarativeUI::Core {

// Forward declarations
template <typename WidgetType>
class DeclarativeBuilder;
class ConditionalRenderer;

/**
 * @brief Condition evaluation result
 */
enum class ConditionResult {
    True,    ///< Condition is true, show content
    False,   ///< Condition is false, hide content
    Pending  ///< Condition is being evaluated (async)
};

/**
 * @brief Condition function type
 */
using ConditionFunction = std::function<ConditionResult()>;

/**
 * @brief Async condition function type
 */
using AsyncConditionFunction =
    std::function<void(std::function<void(ConditionResult)>)>;

/**
 * @brief Widget factory function type
 */
using WidgetFactory = std::function<std::unique_ptr<QWidget>()>;

/**
 * @brief Conditional rendering configuration
 */
struct ConditionalConfig {
    bool reactive = true;  ///< Whether to react to state changes
    std::chrono::milliseconds debounce_delay{
        50};  ///< Debounce delay for condition evaluation
    bool animate_transitions =
        true;  ///< Whether to animate show/hide transitions
    std::chrono::milliseconds animation_duration{200};  ///< Animation duration
    bool lazy_creation = true;  ///< Whether to create widgets lazily
    bool cache_widgets = true;  ///< Whether to cache created widgets
};

/**
 * @brief Conditional rendering item
 *
 * Represents a single conditional rendering case with its condition,
 * widget factory, and configuration.
 */
class ConditionalItem : public QObject {
    Q_OBJECT

public:
    explicit ConditionalItem(QObject* parent = nullptr);
    ~ConditionalItem() override = default;

    // **Condition management**
    void setCondition(ConditionFunction condition);
    void setAsyncCondition(AsyncConditionFunction condition);
    void setWidgetFactory(WidgetFactory factory);

    // **Widget management**
    QWidget* getWidget();
    void clearWidget();
    bool hasWidget() const { return widget_ != nullptr; }

    // **Evaluation**
    ConditionResult evaluateCondition();
    void evaluateAsync(std::function<void(ConditionResult)> callback);

    // **State**
    bool isVisible() const { return is_visible_; }
    void setVisible(bool visible);

    // **Configuration**
    void setConfig(const ConditionalConfig& config) { config_ = config; }
    const ConditionalConfig& getConfig() const { return config_; }

signals:
    void conditionChanged(ConditionResult result);
    void visibilityChanged(bool visible);
    void widgetCreated(QWidget* widget);

private slots:
    void onWidgetDestroyed();

private:
    ConditionFunction condition_;
    AsyncConditionFunction async_condition_;
    WidgetFactory widget_factory_;

    QWidget* widget_ = nullptr;
    bool is_visible_ = false;
    ConditionalConfig config_;

    void createWidget();
};

/**
 * @brief Conditional renderer that manages multiple conditional items
 *
 * The ConditionalRenderer manages a collection of conditional items,
 * evaluating their conditions and showing/hiding widgets accordingly.
 * It supports if-else chains, switch-like behavior, and complex conditions.
 */
class ConditionalRenderer : public UIElement {
    Q_OBJECT

public:
    explicit ConditionalRenderer(QObject* parent = nullptr);
    ~ConditionalRenderer() override = default;

    // **Conditional item management**
    ConditionalItem* addCondition(ConditionFunction condition,
                                  WidgetFactory factory);
    ConditionalItem* addAsyncCondition(AsyncConditionFunction condition,
                                       WidgetFactory factory);
    ConditionalItem* addElse(WidgetFactory factory);  // Always true condition

    // **Template-based item creation**
    template <typename WidgetType>
    ConditionalItem* addCondition(
        ConditionFunction condition,
        std::function<void(DeclarativeBuilder<WidgetType>&)> config);

    // **Evaluation and rendering**
    void evaluateConditions();
    void evaluateConditionsAsync();
    void render();

    // **Configuration**
    void setConfig(const ConditionalConfig& config);
    const ConditionalConfig& getConfig() const { return config_; }

    // **State management integration**
    void bindToState(const QString& state_key);
    void bindToStates(const std::vector<QString>& state_keys);

    // **UIElement implementation**
    void initialize() override;

signals:
    void conditionsEvaluated();
    void renderingComplete();

private slots:
    void onConditionChanged();
    void onStateChanged();
    void onDebounceTimer();

private:
    std::vector<std::unique_ptr<ConditionalItem>> items_;
    ConditionalConfig config_;

    // **State management**
    std::vector<QString> bound_state_keys_;
    QTimer* debounce_timer_ = nullptr;

    // **Current state**
    ConditionalItem* current_item_ = nullptr;
    bool evaluation_pending_ = false;

    // **Internal methods**
    void setupStateBindings();
    void updateFromState();
    void switchToItem(ConditionalItem* item);
    void hideAllItems();
    void animateTransition(QWidget* from, QWidget* to);
    void evaluateConditionsAsyncRecursive(size_t index);
};

/**
 * @brief Conditional rendering builder for fluent API
 *
 * Provides a fluent interface for building conditional rendering logic
 * that integrates seamlessly with DeclarativeBuilder.
 */
class ConditionalBuilder {
public:
    explicit ConditionalBuilder(ConditionalRenderer* renderer = nullptr);

    // **Condition building**
    ConditionalBuilder& when(ConditionFunction condition,
                             WidgetFactory factory);
    ConditionalBuilder& whenAsync(AsyncConditionFunction condition,
                                  WidgetFactory factory);
    ConditionalBuilder& otherwise(WidgetFactory factory);

    // **Template-based building**
    template <typename WidgetType>
    ConditionalBuilder& when(
        ConditionFunction condition,
        std::function<void(DeclarativeBuilder<WidgetType>&)> config);

    // **State-based conditions**
    ConditionalBuilder& whenState(const QString& state_key,
                                  const QVariant& expected_value,
                                  WidgetFactory factory);
    ConditionalBuilder& whenStateTrue(const QString& state_key,
                                      WidgetFactory factory);
    ConditionalBuilder& whenStateFalse(const QString& state_key,
                                       WidgetFactory factory);

    // **Complex conditions**
    ConditionalBuilder& whenAll(
        const std::vector<ConditionFunction>& conditions,
        WidgetFactory factory);
    ConditionalBuilder& whenAny(
        const std::vector<ConditionFunction>& conditions,
        WidgetFactory factory);

    // **Configuration**
    ConditionalBuilder& config(const ConditionalConfig& config);
    ConditionalBuilder& reactive(bool enabled = true);
    ConditionalBuilder& animated(bool enabled = true);
    ConditionalBuilder& lazy(bool enabled = true);

    // **Build**
    std::unique_ptr<ConditionalRenderer> build();
    ConditionalRenderer* buildInto(ConditionalRenderer* renderer);

private:
    std::unique_ptr<ConditionalRenderer> renderer_;
    ConditionalConfig config_;
};

/**
 * @brief Convenience functions for conditional rendering
 */
namespace Conditional {

// **Simple conditional rendering**
std::unique_ptr<ConditionalRenderer> when(ConditionFunction condition,
                                          WidgetFactory factory);
std::unique_ptr<ConditionalRenderer> whenElse(ConditionFunction condition,
                                              WidgetFactory true_factory,
                                              WidgetFactory false_factory);

// **State-based conditional rendering**
std::unique_ptr<ConditionalRenderer> whenState(const QString& state_key,
                                               const QVariant& expected_value,
                                               WidgetFactory factory);

// **Multiple conditions (switch-like)**
std::unique_ptr<ConditionalRenderer> switchOn(
    const std::vector<std::pair<ConditionFunction, WidgetFactory>>& cases,
    WidgetFactory default_factory = nullptr);

// **Async conditional rendering**
std::unique_ptr<ConditionalRenderer> whenAsync(AsyncConditionFunction condition,
                                               WidgetFactory factory);

}  // namespace Conditional

/**
 * @brief Common condition functions
 */
namespace Conditions {

// **State-based conditions**
ConditionFunction stateEquals(const QString& state_key, const QVariant& value);
ConditionFunction stateTrue(const QString& state_key);
ConditionFunction stateFalse(const QString& state_key);
ConditionFunction stateExists(const QString& state_key);

// **Logical operations**
ConditionFunction and_(const std::vector<ConditionFunction>& conditions);
ConditionFunction or_(const std::vector<ConditionFunction>& conditions);
ConditionFunction not_(ConditionFunction condition);

// **Value comparisons**
template <typename T>
ConditionFunction equals(std::function<T()> getter, const T& value);

template <typename T>
ConditionFunction greaterThan(std::function<T()> getter, const T& value);

template <typename T>
ConditionFunction lessThan(std::function<T()> getter, const T& value);

// **String conditions**
ConditionFunction stringEmpty(std::function<QString()> getter);
ConditionFunction stringContains(std::function<QString()> getter,
                                 const QString& substring);

// **Collection conditions**
template <typename Container>
ConditionFunction collectionEmpty(std::function<Container()> getter);

template <typename Container>
ConditionFunction collectionSize(std::function<Container()> getter,
                                 size_t expected_size);

}  // namespace Conditions

}  // namespace DeclarativeUI::Core
