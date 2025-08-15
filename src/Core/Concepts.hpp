// Core/Concepts.hpp
#pragma once

#include <QJsonObject>
#include <QMetaObject>
#include <QObject>
#include <QString>
#include <QVariant>
#include <QWidget>
#include <concepts>
#include <type_traits>

namespace DeclarativeUI::Core::Concepts {

// **Widget Type Concepts**
template <typename T>
concept QtWidget = std::derived_from<T, QWidget>;

template <typename T>
concept QtObject = std::derived_from<T, QObject>;

template <typename T>
concept HasMetaObject = QtObject<T> && requires(T t) {
    { t.metaObject() } -> std::convertible_to<const QMetaObject*>;
    { t.setProperty("", QVariant{}) } -> std::same_as<bool>;
    { t.property("") } -> std::convertible_to<QVariant>;
};

// **Property Value Concepts**
template <typename T>
concept PropertyValue =
    std::convertible_to<T, QVariant> || std::same_as<T, QString> ||
    std::same_as<T, int> || std::same_as<T, double> || std::same_as<T, bool>;

template <typename T>
concept StringLike =
    std::convertible_to<T, QString> || std::convertible_to<T, const char*> ||
    std::same_as<T, std::string>;

// **Callable Concepts**
template <typename F>
concept VoidCallback =
    std::invocable<F> && std::same_as<std::invoke_result_t<F>, void>;

template <typename F, typename... Args>
concept CallableWith = std::invocable<F, Args...>;

template <typename F, typename R, typename... Args>
concept CallableReturning =
    std::invocable<F, Args...> &&
    std::convertible_to<std::invoke_result_t<F, Args...>, R>;

// **Container Concepts**
template <typename T>
concept Container = requires(T t) {
    typename T::value_type;
    typename T::iterator;
    { t.begin() } -> std::same_as<typename T::iterator>;
    { t.end() } -> std::same_as<typename T::iterator>;
    { t.size() } -> std::convertible_to<std::size_t>;
};

template <typename T>
concept IterableContainer = Container<T> && requires(T t) {
    { std::begin(t) } -> std::input_iterator;
    { std::end(t) } -> std::input_iterator;
};

// **Layout Concepts**
template <typename T>
concept LayoutType = std::derived_from<T, QLayout>;

template <typename T>
concept BoxLayoutType = LayoutType<T> && requires(T* t) {
    t->addWidget(static_cast<QWidget*>(nullptr));
    t->addStretch();
    t->addSpacing(0);
};

template <typename T>
concept GridLayoutType = LayoutType<T> && requires(T* t) {
    t->addWidget(static_cast<QWidget*>(nullptr), 0, 0);
    t->addWidget(static_cast<QWidget*>(nullptr), 0, 0, 1, 1);
};

// **Animation Concepts**
template <typename T>
concept AnimatableProperty =
    PropertyValue<T> && requires(T t) { QVariant::fromValue(t); };

template <typename T>
concept DurationLike = std::convertible_to<T, std::chrono::milliseconds> ||
                       std::convertible_to<T, int>;

// **Validation Concepts**
template <typename F, typename T>
concept Validator =
    std::invocable<F, const T*> &&
    std::convertible_to<std::invoke_result_t<F, const T*>, bool>;

// **Event Handler Concepts**
template <typename F>
concept ClickHandler = VoidCallback<F>;

template <typename F, typename T>
concept ValueChangedHandler = CallableWith<F, T>;

template <typename F>
concept StateChangedHandler = CallableWith<F, bool>;

// **Styling Concepts**
template <typename T>
concept ColorLike = std::convertible_to<T, QColor> || StringLike<T>;

template <typename T>
concept FontLike = std::convertible_to<T, QFont> || StringLike<T>;

template <typename T>
concept SizeLike = std::convertible_to<T, QSize> || requires(T t) {
    { t.width() } -> std::convertible_to<int>;
    { t.height() } -> std::convertible_to<int>;
};

// **Geometry Concepts**
template <typename T>
concept PointLike = std::convertible_to<T, QPoint> || requires(T t) {
    { t.x() } -> std::convertible_to<int>;
    { t.y() } -> std::convertible_to<int>;
};

template <typename T>
concept RectLike = std::convertible_to<T, QRect> || requires(T t) {
    { t.x() } -> std::convertible_to<int>;
    { t.y() } -> std::convertible_to<int>;
    { t.width() } -> std::convertible_to<int>;
    { t.height() } -> std::convertible_to<int>;
};

// **Numeric Concepts**
template <typename T>
concept Numeric = std::integral<T> || std::floating_point<T>;

template <typename T>
concept PositiveNumeric = Numeric<T> && requires(T t) { requires t >= T{0}; };

// **Range Concepts**
template <typename T>
concept BoundedValue = requires(T t) {
    { t.minimum() } -> std::convertible_to<typename T::value_type>;
    { t.maximum() } -> std::convertible_to<typename T::value_type>;
    { t.value() } -> std::convertible_to<typename T::value_type>;
};

// **Configuration Concepts**
template <typename T>
concept ConfigurationStruct =
    std::is_aggregate_v<T> && std::is_default_constructible_v<T>;

template <typename T>
concept ThemeConfiguration = ConfigurationStruct<T> && requires(T t) {
    t.primary_color;
    t.secondary_color;
    t.background_color;
    t.text_color;
};

template <typename T>
concept AnimationConfiguration = ConfigurationStruct<T> && requires(T t) {
    t.duration;
    t.easing;
    t.enabled;
};

// **Memory Management Concepts**
template <typename T>
concept SmartPointer = requires(T t) {
    typename T::element_type;
    { t.get() } -> std::convertible_to<typename T::element_type*>;
    { t.reset() } -> std::same_as<void>;
    { static_cast<bool>(t) } -> std::same_as<bool>;
};

template <typename T>
concept UniquePointer =
    SmartPointer<T> &&
    std::same_as<T, std::unique_ptr<typename T::element_type>>;

template <typename T>
concept SharedPointer =
    SmartPointer<T> &&
    std::same_as<T, std::shared_ptr<typename T::element_type>>;

// **RAII Concepts**
template <typename T>
concept RAIIResource =
    std::is_destructible_v<T> && !std::is_trivially_destructible_v<T>;

// **Fluent Interface Concepts**
template <typename T, typename R>
concept FluentMethod = requires(T t) {
    { t } -> std::convertible_to<R&>;
};

template <typename T>
concept FluentInterface = requires(T t) {
    { t } -> std::same_as<T&>;
};

// **Serialization Concepts**
template <typename T>
concept Serializable = requires(T t) {
    { t.serialize() } -> std::convertible_to<QJsonObject>;
    { t.deserialize(QJsonObject{}) } -> std::convertible_to<bool>;
};

// **Performance Monitoring Concepts**
template <typename T>
concept PerformanceMonitorable = requires(T t) {
    t.getPerformanceMetrics();
    t.enablePerformanceMonitoring(true);
};

}  // namespace DeclarativeUI::Core::Concepts
