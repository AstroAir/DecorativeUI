#pragma once

#include <QColor>
#include <QEasingCurve>
#include <QFont>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsEffect>
#include <QGraphicsOpacityEffect>
#include <QIcon>
#include <QJsonObject>
#include <QMetaObject>
#include <QMetaProperty>
#include <QObject>
#include <QPixmap>
#include <QPropertyAnimation>
#include <QRect>
#include <QSize>
#include <QTimer>
#include <QVariant>
#include <QWidget>

#include <array>
#include <chrono>
#include <functional>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

#include "../Animation/AnimationEngine.hpp"
#include "../Exceptions/UIExceptions.hpp"

Q_DECLARE_METATYPE(std::function<void()>)

namespace DeclarativeUI::Core {

// **Forward declarations**
class UIElement;

// **Type traits for compile-time type checking**
template <typename T>
struct is_qt_widget : std::is_base_of<QWidget, T> {};

template <typename T>
constexpr bool is_qt_widget_v = is_qt_widget<T>::value;

template <typename T>
struct has_meta_object {
    template <typename U>
    static auto test(int)
        -> decltype(std::declval<U>().metaObject(),
                    std::declval<U>().setProperty("", QVariant{}),
                    std::declval<U>().property(""), std::true_type{});

    template <typename>
    static std::false_type test(...);

    using type = decltype(test<T>(0));
    static constexpr bool value = type::value;
};

template <typename T>
constexpr bool has_meta_object_v = has_meta_object<T>::value;

// **Property value variant for flexible property system**
using PropertyValue =
    std::variant<int, double, bool, QString, QColor, QFont,
                 std::function<void()>, QSize, QRect, QIcon, QPixmap, QVariant>;

// **Animation configuration - Bridge to new Animation system**
struct AnimationConfig {
    std::chrono::milliseconds duration{250};
    QEasingCurve::Type easing = QEasingCurve::OutCubic;
    bool enabled = true;

    // **Convert to new Animation system properties**
    Animation::AnimationProperties toAnimationProperties() const {
        Animation::AnimationProperties props;
        props.duration_ms = static_cast<int>(duration.count());
        props.easing = convertEasingType(easing);
        return props;
    }

private:
    /**
     * @brief Convert QEasingCurve::Type to Animation::EasingType
     * @param qt_easing The Qt easing curve type to convert
     * @return Corresponding Animation::EasingType
     *
     * This function provides a mapping between Qt's easing curve types and the
     * internal Animation::EasingType enum. Uses a lookup table approach to
     * reduce cyclomatic complexity and improve performance.
     */
    static Animation::EasingType convertEasingType(
        QEasingCurve::Type qt_easing) {
        return getEasingTypeLookup(qt_easing);
    }

    /**
     * @brief Static lookup function for easing type conversion
     * @param qt_easing The Qt easing curve type
     * @return Corresponding Animation::EasingType
     *
     * This helper function provides a centralized mapping between Qt easing
     * types and Animation easing types using a lookup table for better
     * performance.
     */
    static Animation::EasingType getEasingTypeLookup(
        QEasingCurve::Type qt_easing) {
        // Static lookup table for better performance and reduced complexity
        static const std::unordered_map<QEasingCurve::Type,
                                        Animation::EasingType>
            easingTypeMap = {
                // Linear easing
                {QEasingCurve::Linear, Animation::EasingType::Linear},

                // Quadratic easing
                {QEasingCurve::InQuad, Animation::EasingType::QuadIn},
                {QEasingCurve::OutQuad, Animation::EasingType::QuadOut},
                {QEasingCurve::InOutQuad, Animation::EasingType::QuadInOut},

                // Cubic easing
                {QEasingCurve::InCubic, Animation::EasingType::CubicIn},
                {QEasingCurve::OutCubic, Animation::EasingType::CubicOut},
                {QEasingCurve::InOutCubic, Animation::EasingType::CubicInOut},

                // Quartic easing
                {QEasingCurve::InQuart, Animation::EasingType::QuartIn},
                {QEasingCurve::OutQuart, Animation::EasingType::QuartOut},
                {QEasingCurve::InOutQuart, Animation::EasingType::QuartInOut},

                // Sinusoidal easing
                {QEasingCurve::InSine, Animation::EasingType::SineIn},
                {QEasingCurve::OutSine, Animation::EasingType::SineOut},
                {QEasingCurve::InOutSine, Animation::EasingType::SineInOut},

                // Exponential easing
                {QEasingCurve::InExpo, Animation::EasingType::ExpoIn},
                {QEasingCurve::OutExpo, Animation::EasingType::ExpoOut},
                {QEasingCurve::InOutExpo, Animation::EasingType::ExpoInOut},

                // Circular easing
                {QEasingCurve::InCirc, Animation::EasingType::CircIn},
                {QEasingCurve::OutCirc, Animation::EasingType::CircOut},
                {QEasingCurve::InOutCirc, Animation::EasingType::CircInOut},

                // Back easing
                {QEasingCurve::InBack, Animation::EasingType::BackIn},
                {QEasingCurve::OutBack, Animation::EasingType::BackOut},
                {QEasingCurve::InOutBack, Animation::EasingType::BackInOut},

                // Elastic easing
                {QEasingCurve::InElastic, Animation::EasingType::ElasticIn},
                {QEasingCurve::OutElastic, Animation::EasingType::ElasticOut},
                {QEasingCurve::InOutElastic,
                 Animation::EasingType::ElasticInOut},

                // Bounce easing
                {QEasingCurve::InBounce, Animation::EasingType::BounceIn},
                {QEasingCurve::OutBounce, Animation::EasingType::BounceOut},
                {QEasingCurve::InOutBounce,
                 Animation::EasingType::BounceInOut}};

        auto it = easingTypeMap.find(qt_easing);
        return (it != easingTypeMap.end()) ? it->second
                                           : Animation::EasingType::Linear;
    }
};

// **Theme configuration**
struct ThemeConfig {
    QString primary_color = "#3498db";
    QString secondary_color = "#2ecc71";
    QString background_color = "#ecf0f1";
    QString text_color = "#2c3e50";
    QString border_color = "#bdc3c7";
    int border_radius = 4;
    QString font_family = "Arial";
    int font_size = 12;
};

// **Performance monitoring**
struct PerformanceMetrics {
    std::chrono::microseconds last_update_time{0};
    std::chrono::microseconds total_update_time{0};
    int update_count = 0;
    double average_update_time() const {
        return update_count > 0
                   ? static_cast<double>(total_update_time.count()) /
                         update_count
                   : 0.0;
    }
};

class UIElement : public QObject {
    Q_OBJECT

public:
    explicit UIElement(QObject *parent = nullptr);
    virtual ~UIElement() = default;

    // **Modern C++ move semantics and RAII**
    UIElement(const UIElement &) = delete;
    UIElement &operator=(const UIElement &) = delete;

    // **Fluent interface for declarative syntax**
    template <typename T>
    UIElement &setProperty(const QString &name, T &&value);

    template <typename T>
    std::unique_ptr<T> createWidget();

    UIElement &bindProperty(const QString &property,
                            const std::function<PropertyValue()> &binding);

    UIElement &onEvent(const QString &event,
                       const std::function<void()> &handler);

    // **Advanced styling with CSS-like syntax**
    UIElement &setStyle(const QString &css_style);
    UIElement &addClass(const QString &class_name);
    UIElement &removeClass(const QString &class_name);
    UIElement &toggleClass(const QString &class_name);
    bool hasClass(const QString &class_name) const;

    // **Animation support**
    UIElement &animate(const QString &property, const QVariant &target_value,
                       const AnimationConfig &config = {});
    UIElement &fadeIn(const AnimationConfig &config = {});
    UIElement &fadeOut(const AnimationConfig &config = {});
    UIElement &slideIn(const AnimationConfig &config = {});
    UIElement &slideOut(const AnimationConfig &config = {});

    // **Effects**
    UIElement &setShadow(const QColor &color = QColor(0, 0, 0, 50),
                         const QPointF &offset = QPointF(2, 2),
                         qreal blur_radius = 5);
    UIElement &setOpacity(qreal opacity, const AnimationConfig &config = {});
    UIElement &setBlur(qreal blur_radius);

    // **Responsive design**
    UIElement &setResponsive(bool responsive = true);
    UIElement &setBreakpoint(int width, std::function<void()> callback);

    // **Accessibility**
    UIElement &setAccessibleName(const QString &name);
    UIElement &setAccessibleDescription(const QString &description);
    UIElement &setAccessibleRole(const QString &role);

    // **Exception-safe property access**
    [[nodiscard]] PropertyValue getProperty(const QString &name) const;

    // **Public getters for protected members**
    [[nodiscard]] const std::unordered_map<QString, PropertyValue> &
    getProperties() const {
        return properties_;
    }

    [[nodiscard]] const std::unordered_map<QString, std::function<void()>> &
    getEventHandlers() const {
        return event_handlers_;
    }

    // **Widget lifecycle management**
    virtual void initialize() = 0;
    virtual void cleanup() noexcept;
    virtual void refresh();
    virtual void invalidate();

    // **Widget management**
    void setWidget(QWidget *widget);
    QWidget *getWidget() const noexcept { return widget_.get(); }

    // **Performance monitoring**
    const PerformanceMetrics &getPerformanceMetrics() const {
        return performance_metrics_;
    }
    void enablePerformanceMonitoring(bool enabled) {
        performance_monitoring_enabled_ = enabled;
    }

    // **Theme support**
    void setTheme(const ThemeConfig &theme);
    const ThemeConfig &getTheme() const { return theme_; }

    // **Validation**
    virtual bool validate() const;
    void addValidator(std::function<bool(const UIElement *)> validator);

    // **Serialization**
    QJsonObject serialize() const;
    bool deserialize(const QJsonObject &json);

protected:
    std::unordered_map<QString, PropertyValue> properties_;
    std::unordered_map<QString, std::function<PropertyValue()>> bindings_;
    std::unordered_map<QString, std::function<void()>> event_handlers_;
    std::unordered_set<QString> css_classes_;
    std::unordered_map<int, std::function<void()>> breakpoints_;
    std::vector<std::function<bool(const UIElement *)>> validators_;

    // **RAII widget management**
    std::unique_ptr<QWidget> widget_;

    // **Animation management - Using new Animation system**
    std::unordered_map<QString, std::shared_ptr<Animation::Animation>>
        animations_;
    AnimationConfig default_animation_config_;

    // **Theme and styling**
    ThemeConfig theme_;
    QString compiled_stylesheet_;

    // **Performance monitoring**
    PerformanceMetrics performance_metrics_;
    bool performance_monitoring_enabled_ = false;

    // **Responsive design**
    bool responsive_enabled_ = false;
    int current_width_ = 0;

    // **Helper methods**
    void updateBoundProperties();
    void connectSignals();
    void applyStoredProperties();
    void applyTheme();
    void compileStylesheet();
    void measurePerformance(std::function<void()> operation);
    void checkBreakpoints();

    // **Animation helpers - Using new Animation system**
    std::shared_ptr<Animation::Animation> createAnimation(
        const QString &property, const QVariant &target_value,
        const AnimationConfig &config);
    void setupAnimation(std::shared_ptr<Animation::Animation> animation,
                        const AnimationConfig &config);

    // **Deserialization helper methods to reduce complexity**
    bool validateDeserializationInput(const QJsonObject &json) const;
    void deserializeProperties(const QJsonObject &json);
    void deserializeTheme(const QJsonObject &json);
    void deserializeConfiguration(const QJsonObject &json);
    void applyDeserializedData();
    PropertyValue parsePropertyValue(const QJsonValue &value) const;

private slots:
    void onPropertyChanged();
    void onWidgetResized();
    void onAnimationFinished();

signals:
    void propertyUpdated(const QString &property);
    void styleChanged();
    void animationStarted(const QString &property);
    void animationFinished(const QString &property);
    void breakpointChanged(int width);
    void validated(bool success);
    void refreshed();
    void invalidated();
    void widgetResized();
};

// **Template implementation with perfect forwarding**
template <typename T>
UIElement &UIElement::setProperty(const QString &name, T &&value) {
    if constexpr (std::is_convertible_v<T, const char *>) {
        // Handle const char* types by converting to QString
        QString qstring_value = QString::fromUtf8(value);
        properties_[name] = PropertyValue{qstring_value};

        if (widget_) {
            bool success = widget_->setProperty(
                name.toUtf8().constData(), QVariant::fromValue(qstring_value));
            if (!success) {
                throw Exceptions::PropertyBindingException(name.toStdString());
            }
        }

        emit propertyUpdated(name);
        return *this;
    } else {
        static_assert(std::is_constructible_v<PropertyValue, T>,
                      "Property type must be supported");

        try {
            properties_[name] = PropertyValue{std::forward<T>(value)};

            if (widget_) {
                // **Safe property setting with Qt's meta-object system**
                bool success = widget_->setProperty(name.toUtf8().constData(),
                                                    QVariant::fromValue(value));

                if (!success) {
                    throw Exceptions::PropertyBindingException(
                        name.toStdString());
                }
            }

            emit propertyUpdated(name);
            return *this;
        } catch (const std::exception &e) {
            throw Exceptions::PropertyBindingException(name.toStdString() +
                                                       ": " + e.what());
        }
    }
}

template <typename T>
std::unique_ptr<T> UIElement::createWidget() {
    static_assert(is_qt_widget_v<T>, "T must be a QWidget-derived type");
    try {
        auto widget = std::make_unique<T>();

        // **Apply stored properties using Qt's meta-object system**
        for (const auto &[name, value] : properties_) {
            std::visit(
                [&](const auto &val) {
                    widget->setProperty(name.toUtf8().constData(),
                                        QVariant::fromValue(val));
                },
                value);
        }

        return widget;
    } catch (const std::exception &e) {
        throw Exceptions::ComponentCreationException(
            typeid(T).name() + std::string(": ") + e.what());
    }
}

}  // namespace DeclarativeUI::Core
