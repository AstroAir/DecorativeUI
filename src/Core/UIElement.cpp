/**
 * @file UIElement.cpp
 * @brief Implementation of the core UIElement class for DeclarativeUI
 *
 * This file provides the foundation for all UI components in the DeclarativeUI
 * framework, including:
 * - Property management with type-safe storage and binding
 * - Animation support with fluent interface
 * - Theme configuration and styling
 * - Performance monitoring and responsive design
 * - Serialization and deserialization capabilities
 * - Validation and error handling
 *
 * The implementation emphasizes maintainable code with low cyclomatic
 * complexity by breaking down complex operations into focused helper functions.
 *
 * @author DeclarativeUI Team
 * @version 1.0
 */

#include "UIElement.hpp"
#include "Lifecycle.hpp"

#include <QApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMetaObject>
#include <QPalette>
#include <QTimer>

#include <chrono>

namespace DeclarativeUI::Core {

UIElement::UIElement(QObject *parent) : QObject(parent) {
    // **Initialize with exception safety**
    try {
        // Setup property change timer for batched updates
        auto *timer = new QTimer(this);
        timer->setSingleShot(true);
        timer->setInterval(16);  // ~60 FPS

        connect(timer, &QTimer::timeout, this, &UIElement::onPropertyChanged);

        connect(this, &UIElement::propertyUpdated,
                [timer]() { timer->start(); });

        // **Initialize lifecycle management**
        lifecycle_ = std::make_unique<ComponentLifecycle>(this);

    } catch (const std::exception &e) {
        throw Exceptions::UIException("Failed to initialize UIElement: " +
                                      std::string(e.what()));
    }
}

UIElement::~UIElement() = default;

UIElement &UIElement::bindProperty(
    const QString &property, const std::function<PropertyValue()> &binding) {
    if (!binding) {
        throw Exceptions::PropertyBindingException(
            property.toStdString() + ": binding function is null");
    }

    bindings_[property] = binding;

    // **Immediate update with exception handling**
    try {
        PropertyValue value = binding();
        setProperty(property, value);
    } catch (const std::exception &e) {
        throw Exceptions::PropertyBindingException(property.toStdString() +
                                                   ": " + e.what());
    }

    return *this;
}

UIElement &UIElement::onEvent(const QString &event,
                              const std::function<void()> &handler) {
    if (!handler) {
        throw std::invalid_argument("Event handler cannot be null");
    }

    event_handlers_[event] = handler;

    // **Connect to Qt signals if widget exists**
    if (widget_) {
        connectSignals();
    }

    return *this;
}

PropertyValue UIElement::getProperty(const QString &name) const {
    auto it = properties_.find(name);
    if (it == properties_.end()) {
        throw Exceptions::PropertyBindingException("Property not found: " +
                                                   name.toStdString());
    }
    return it->second;
}

bool UIElement::hasProperty(const QString &name) const {
    return properties_.find(name) != properties_.end();
}

void UIElement::initialize() {
    // Base implementation does minimal setup; subclasses may override.
    // This keeps UIElement non-abstract for unit tests that instantiate it.
}

void UIElement::cleanup() noexcept {
    try {
        // Trigger unmount lifecycle event before cleanup
        if (lifecycle_) {
            try {
                lifecycle_->unmount();
            } catch (const std::exception &e) {
                qWarning() << "Lifecycle unmount failed:" << e.what();
            }
        }

        event_handlers_.clear();
        bindings_.clear();
        properties_.clear();
        previous_properties_.clear();
        widget_.reset();
    } catch (...) {
        // **No-throw cleanup guarantee**
    }
}

void UIElement::updateBoundProperties() {
    for (const auto &[property, binding] : bindings_) {
        try {
            PropertyValue value = binding();
            properties_[property] = value;

            if (widget_) {
                std::visit(
                    [&](const auto &val) {
                        widget_->setProperty(property.toUtf8().constData(),
                                             QVariant::fromValue(val));
                    },
                    value);
            }
        } catch (const std::exception &e) {
            qWarning() << "Property binding update failed for" << property
                       << ":" << e.what();
        }
    }
}

void UIElement::connectSignals() {
    // **Dynamic signal connection using Qt's meta-object system**
    // Note: Derived classes should override this method to handle their
    // specific signals This base implementation provides common signal
    // connections
    if (!widget_)
        return;

    // For now, let derived classes handle their own signal connections
    // This avoids conflicts with specialized signal handling in components like
    // Button
    // TODO: Implement generic signal connection for basic widgets that don't
    // override this
}

void UIElement::onPropertyChanged() { updateBoundProperties(); }

void UIElement::setWidget(QWidget *widget) {
    if (!widget) {
        throw Exceptions::UIException("Widget cannot be null");
    }

    // Store previous properties for lifecycle update detection
    previous_properties_.clear();
    for (const auto &[name, value] : properties_) {
        std::visit(
            [&](const auto &val) {
                previous_properties_[name] = QVariant::fromValue(val);
            },
            value);
    }

    widget_ = std::unique_ptr<QWidget>(widget);
    applyStoredProperties();
    connectSignals();

    // Trigger mount lifecycle event
    if (lifecycle_) {
        try {
            lifecycle_->mount(widget);
        } catch (const std::exception &e) {
            qWarning() << "Lifecycle mount failed:" << e.what();
        }
    }
}

void UIElement::applyStoredProperties() {
    if (!widget_)
        return;

    for (const auto &[name, value] : properties_) {
        try {
            std::visit(
                [&](const auto &val) {
                    widget_->setProperty(name.toUtf8().constData(),
                                         QVariant::fromValue(val));
                },
                value);
        } catch (const std::exception &e) {
            qWarning() << "Failed to apply property" << name << ":" << e.what();
        }
    }
}

// **Implementation of missing virtual methods**
void UIElement::refresh() {
    if (!widget_)
        return;

    // Update bound properties first (re-evaluate bindings)
    updateBoundProperties();

    // Apply all stored properties
    applyStoredProperties();

    // Update the widget display
    widget_->update();

    // Emit refresh signal
    emit refreshed();
}

void UIElement::invalidate() {
    if (!widget_)
        return;

    // Mark widget as needing repaint
    widget_->repaint();

    // Emit invalidated signal
    emit invalidated();
}

bool UIElement::validate() const {
    // Run all validators
    for (const auto &validator : validators_) {
        if (!validator(this)) {
            return false;
        }
    }

    // Check if widget exists and is visible
    if (widget_ && widget_->isVisible()) {
        return true;
    }

    return widget_ != nullptr;
}

void UIElement::onWidgetResized() {
    if (!widget_)
        return;

    // Handle responsive breakpoints
    int width = widget_->width();
    for (const auto &[breakpoint, handler] : breakpoints_) {
        if (width >= breakpoint) {
            handler();
        }
    }

    // Emit resize signal
    emit widgetResized();
}

void UIElement::onAnimationFinished() {
    // Update performance metrics
    performance_metrics_.update_count++;

    // Emit animation finished signal with empty property name
    emit animationFinished(QString());
}

// **Animation method implementations using new Animation system**
UIElement &UIElement::animate(const QString &property,
                              const QVariant &target_value,
                              const AnimationConfig &config) {
    if (!config.enabled || !widget_) {
        return *this;
    }

    try {
        auto animation = createAnimation(property, target_value, config);
        if (animation) {
            animations_[property] = animation;
            animation->start();
            emit animationStarted(property);
        }
    } catch (const std::exception &e) {
        qWarning() << "Failed to create animation for property" << property
                   << ":" << e.what();
    }

    return *this;
}

UIElement &UIElement::fadeIn(const AnimationConfig &config) {
    if (!config.enabled || !widget_) {
        return *this;
    }

    try {
        auto &engine = Animation::AnimationEngine::instance();
        auto animation = engine.fadeIn(
            widget_.get(), static_cast<int>(config.duration.count()));

        if (animation) {
            animations_["opacity"] = animation;
            emit animationStarted("opacity");
        }
    } catch (const std::exception &e) {
        qWarning() << "Failed to create fade in animation:" << e.what();
    }

    return *this;
}

UIElement &UIElement::fadeOut(const AnimationConfig &config) {
    if (!config.enabled || !widget_) {
        return *this;
    }

    try {
        auto &engine = Animation::AnimationEngine::instance();
        auto animation = engine.fadeOut(
            widget_.get(), static_cast<int>(config.duration.count()));

        if (animation) {
            animations_["opacity"] = animation;
            emit animationStarted("opacity");
        }
    } catch (const std::exception &e) {
        qWarning() << "Failed to create fade out animation:" << e.what();
    }

    return *this;
}

UIElement &UIElement::slideIn(const AnimationConfig &config) {
    if (!config.enabled || !widget_) {
        return *this;
    }

    try {
        auto &engine = Animation::AnimationEngine::instance();
        auto animation = engine.slideIn(
            widget_.get(), "left", static_cast<int>(config.duration.count()));

        if (animation) {
            animations_["position"] = animation;
            emit animationStarted("position");
        }
    } catch (const std::exception &e) {
        qWarning() << "Failed to create slide in animation:" << e.what();
    }

    return *this;
}

UIElement &UIElement::slideOut(const AnimationConfig &config) {
    if (!config.enabled || !widget_) {
        return *this;
    }

    try {
        auto &engine = Animation::AnimationEngine::instance();
        auto animation = engine.slideOut(
            widget_.get(), "right", static_cast<int>(config.duration.count()));

        if (animation) {
            animations_["position"] = animation;
            emit animationStarted("position");
        }
    } catch (const std::exception &e) {
        qWarning() << "Failed to create slide out animation:" << e.what();
    }

    return *this;
}

UIElement &UIElement::setOpacity(qreal opacity, const AnimationConfig &config) {
    if (!widget_) {
        return *this;
    }

    if (!config.enabled) {
        // Set opacity immediately without animation
        widget_->setWindowOpacity(opacity);
        return *this;
    }

    try {
        auto &engine = Animation::AnimationEngine::instance();
        QVariant current_opacity = widget_->windowOpacity();
        auto animation = engine.animateProperty(
            widget_.get(), "windowOpacity", current_opacity, QVariant(opacity),
            static_cast<int>(config.duration.count()),
            config.toAnimationProperties().easing);

        if (animation) {
            animations_["windowOpacity"] = animation;
            emit animationStarted("windowOpacity");
        }
    } catch (const std::exception &e) {
        qWarning() << "Failed to create opacity animation:" << e.what();
        // Fallback to immediate setting
        widget_->setWindowOpacity(opacity);
    }

    return *this;
}

// **Animation helper implementations**
std::shared_ptr<Animation::Animation> UIElement::createAnimation(
    const QString &property, const QVariant &target_value,
    const AnimationConfig &config) {
    if (!widget_) {
        return nullptr;
    }

    try {
        auto &engine = Animation::AnimationEngine::instance();
        QVariant current_value =
            widget_->property(property.toUtf8().constData());

        auto animation = engine.animateProperty(
            widget_.get(), property, current_value, target_value,
            static_cast<int>(config.duration.count()),
            config.toAnimationProperties().easing);

        setupAnimation(animation, config);
        return animation;
    } catch (const std::exception &e) {
        qWarning() << "Failed to create animation for property" << property
                   << ":" << e.what();
        return nullptr;
    }
}

void UIElement::setupAnimation(std::shared_ptr<Animation::Animation> animation,
                               const AnimationConfig &config) {
    if (!animation) {
        return;
    }

    try {
        auto props = config.toAnimationProperties();
        animation->setProperties(props);

        // Connect animation finished signal to our slot
        connect(animation.get(), &Animation::Animation::finished, this,
                &UIElement::onAnimationFinished);
    } catch (const std::exception &e) {
        qWarning() << "Failed to setup animation:" << e.what();
    }
}

// **Validation implementation**
void UIElement::addValidator(std::function<bool(const UIElement *)> validator) {
    if (!validator) {
        throw std::invalid_argument("Validator function cannot be null");
    }

    validators_.push_back(std::move(validator));
}

// **Theme support implementation**
void UIElement::setTheme(const ThemeConfig &theme) {
    theme_ = theme;

    // Apply theme immediately if widget exists
    if (widget_) {
        applyTheme();
    }

    emit styleChanged();
}

void UIElement::applyTheme() {
    if (!widget_) {
        return;
    }

    try {
        // Apply color palette
        QPalette palette = widget_->palette();

        // Set primary colors
        palette.setColor(QPalette::Button, QColor(theme_.primary_color));
        palette.setColor(QPalette::ButtonText, QColor(theme_.text_color));
        palette.setColor(QPalette::Base, QColor(theme_.background_color));
        palette.setColor(QPalette::Window, QColor(theme_.background_color));
        palette.setColor(QPalette::WindowText, QColor(theme_.text_color));
        palette.setColor(QPalette::Text, QColor(theme_.text_color));

        // Set highlight colors
        palette.setColor(QPalette::Highlight, QColor(theme_.primary_color));
        palette.setColor(QPalette::HighlightedText,
                         QColor(theme_.background_color));

        widget_->setPalette(palette);

        // Apply font if specified
        if (!theme_.font_family.isEmpty()) {
            QFont font(theme_.font_family, theme_.font_size);
            widget_->setFont(font);
        }

        // Compile and apply stylesheet
        compileStylesheet();

    } catch (const std::exception &e) {
        qWarning() << "Failed to apply theme:" << e.what();
    }
}

void UIElement::compileStylesheet() {
    if (!widget_) {
        return;
    }

    try {
        QString stylesheet;

        // Build stylesheet from theme
        stylesheet += QString("QWidget { ");
        stylesheet +=
            QString("background-color: %1; ").arg(theme_.background_color);
        stylesheet += QString("color: %1; ").arg(theme_.text_color);
        stylesheet += QString("font-family: %1; ").arg(theme_.font_family);
        stylesheet += QString("font-size: %1pt; ").arg(theme_.font_size);

        // Add spacing and border
        stylesheet += QString("padding: 8px; ");
        stylesheet += QString("margin: 4px; ");
        stylesheet +=
            QString("border: 1px solid %1; ").arg(theme_.border_color);
        stylesheet +=
            QString("border-radius: %1px; ").arg(theme_.border_radius);

        stylesheet += "} ";

        // Button specific styles
        stylesheet += QString("QPushButton { ");
        stylesheet +=
            QString("background-color: %1; ").arg(theme_.primary_color);
        stylesheet += QString("color: %1; ").arg(theme_.background_color);
        stylesheet += QString("border: none; ");
        stylesheet += QString("padding: 8px 16px; ");
        stylesheet +=
            QString("border-radius: %1px; ").arg(theme_.border_radius);
        stylesheet += "} ";

        stylesheet += QString("QPushButton:hover { ");
        stylesheet +=
            QString("background-color: %1; ").arg(theme_.secondary_color);
        stylesheet += "} ";

        stylesheet += QString("QPushButton:pressed { ");
        stylesheet +=
            QString("background-color: %1; ").arg(theme_.border_color);
        stylesheet += "} ";

        // Apply the compiled stylesheet
        widget_->setStyleSheet(stylesheet);

    } catch (const std::exception &e) {
        qWarning() << "Failed to compile stylesheet:" << e.what();
    }
}

// **Performance monitoring implementation**
void UIElement::measurePerformance(std::function<void()> operation) {
    if (!operation) {
        return;
    }

    if (!performance_monitoring_enabled_) {
        // Just execute without measuring if monitoring is disabled
        operation();
        return;
    }

    auto start = std::chrono::high_resolution_clock::now();

    try {
        operation();

        auto end = std::chrono::high_resolution_clock::now();
        auto duration =
            std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        // Update performance metrics
        performance_metrics_.update_count++;
        performance_metrics_.total_update_time += duration;
        performance_metrics_.last_update_time = duration;

        // Log if operation took too long (> 16ms for 60fps)
        if (duration.count() > 16000) {
            qWarning() << "Slow operation detected:" << duration.count()
                       << "microseconds";
        }

    } catch (const std::exception &e) {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration =
            std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        qWarning() << "Performance measurement failed after" << duration.count()
                   << "microseconds:" << e.what();
        throw;
    }
}

// **Responsive design implementation**
void UIElement::checkBreakpoints() {
    if (!widget_ || !responsive_enabled_) {
        return;
    }

    int width = widget_->width();

    // Only emit signal if breakpoint actually changed
    if (width != current_width_) {
        current_width_ = width;
        emit breakpointChanged(width);

        // Apply responsive styles based on breakpoints
        try {
            QString responsive_class;

            if (width < 576) {
                responsive_class = "xs";
            } else if (width < 768) {
                responsive_class = "sm";
            } else if (width < 992) {
                responsive_class = "md";
            } else if (width < 1200) {
                responsive_class = "lg";
            } else {
                responsive_class = "xl";
            }

            // Set property for CSS-like responsive styling
            widget_->setProperty("responsive-class", responsive_class);

            // Recompile stylesheet with responsive styles
            compileStylesheet();

        } catch (const std::exception &e) {
            qWarning() << "Failed to apply responsive styles:" << e.what();
        }
    }
}

// **Serialization implementation**
QJsonObject UIElement::serialize() const {
    QJsonObject json;

    try {
        // Serialize basic properties
        QJsonObject properties_json;
        for (const auto &[name, value] : properties_) {
            std::visit(
                [&](const auto &val) {
                    using T = std::decay_t<decltype(val)>;

                    if constexpr (std::is_same_v<T, QString>) {
                        properties_json[name] = val;
                    } else if constexpr (std::is_same_v<T, int>) {
                        properties_json[name] = val;
                    } else if constexpr (std::is_same_v<T, double>) {
                        properties_json[name] = val;
                    } else if constexpr (std::is_same_v<T, bool>) {
                        properties_json[name] = val;
                    } else if constexpr (std::is_same_v<T, QSize>) {
                        QJsonObject size_obj;
                        size_obj["width"] = val.width();
                        size_obj["height"] = val.height();
                        properties_json[name] = size_obj;
                    } else if constexpr (std::is_same_v<T, QPoint>) {
                        QJsonObject point_obj;
                        point_obj["x"] = val.x();
                        point_obj["y"] = val.y();
                        properties_json[name] = point_obj;
                    } else if constexpr (std::is_same_v<T, QColor>) {
                        properties_json[name] = val.name();
                    } else {
                        // For other types, try to convert to string
                        properties_json[name] = QString("Unsupported type");
                    }
                },
                value);
        }
        json["properties"] = properties_json;

        // Serialize theme information
        QJsonObject theme_json;
        theme_json["primary_color"] = theme_.primary_color;
        theme_json["secondary_color"] = theme_.secondary_color;
        theme_json["background_color"] = theme_.background_color;
        theme_json["text_color"] = theme_.text_color;
        theme_json["border_color"] = theme_.border_color;
        theme_json["font_family"] = theme_.font_family;
        theme_json["font_size"] = theme_.font_size;
        theme_json["border_radius"] = theme_.border_radius;
        json["theme"] = theme_json;

        // Serialize widget information if available
        if (widget_) {
            QJsonObject widget_json;
            widget_json["class_name"] =
                QString(widget_->metaObject()->className());
            widget_json["object_name"] = widget_->objectName();
            widget_json["geometry"] =
                QJsonObject{{"x", widget_->x()},
                            {"y", widget_->y()},
                            {"width", widget_->width()},
                            {"height", widget_->height()}};
            widget_json["visible"] = widget_->isVisible();
            widget_json["enabled"] = widget_->isEnabled();
            json["widget"] = widget_json;
        }

        // Serialize performance metrics
        QJsonObject metrics_json;
        metrics_json["last_update_time"] =
            static_cast<qint64>(performance_metrics_.last_update_time.count());
        metrics_json["total_update_time"] =
            static_cast<qint64>(performance_metrics_.total_update_time.count());
        metrics_json["update_count"] = performance_metrics_.update_count;
        metrics_json["average_update_time"] =
            performance_metrics_.average_update_time();
        json["performance_metrics"] = metrics_json;

        // Serialize configuration flags
        QJsonObject config_json;
        config_json["performance_monitoring_enabled"] =
            performance_monitoring_enabled_;
        config_json["responsive_enabled"] = responsive_enabled_;
        config_json["current_width"] = current_width_;
        json["configuration"] = config_json;

    } catch (const std::exception &e) {
        qWarning() << "Failed to serialize UIElement:" << e.what();
        // Return partial JSON with error information
        json["error"] = QString("Serialization failed: %1").arg(e.what());
    }

    return json;
}

/**
 * @brief Deserializes UIElement from JSON object
 * @param json The JSON object containing serialized data
 * @return true if deserialization was successful, false otherwise
 *
 * This function coordinates the deserialization process by delegating
 * to specialized helper functions for different data sections.
 */
bool UIElement::deserialize(const QJsonObject &json) {
    try {
        // **Check for serialization errors**
        if (!validateDeserializationInput(json)) {
            return false;
        }

        // **Deserialize different sections**
        deserializeProperties(json);
        deserializeTheme(json);
        deserializeConfiguration(json);

        // **Apply deserialized data to widget**
        applyDeserializedData();

        return true;

    } catch (const std::exception &e) {
        qWarning() << "Failed to deserialize UIElement:" << e.what();
        return false;
    }
}

// **Deserialization helper method implementations**

/**
 * @brief Validates the input JSON object for deserialization
 * @param json The JSON object to validate
 * @return true if the input is valid, false otherwise
 */
bool UIElement::validateDeserializationInput(const QJsonObject &json) const {
    if (json.contains("error")) {
        qWarning() << "Cannot deserialize UIElement with error:"
                   << json["error"].toString();
        return false;
    }
    return true;
}

/**
 * @brief Deserializes properties from JSON object
 * @param json The JSON object containing properties data
 */
void UIElement::deserializeProperties(const QJsonObject &json) {
    if (!json.contains("properties") || !json["properties"].isObject()) {
        return;
    }

    QJsonObject properties_json = json["properties"].toObject();
    for (auto it = properties_json.begin(); it != properties_json.end(); ++it) {
        const QString &name = it.key();
        const QJsonValue &value = it.value();
        properties_[name] = parsePropertyValue(value);
    }
}

/**
 * @brief Deserializes theme configuration from JSON object
 * @param json The JSON object containing theme data
 */
void UIElement::deserializeTheme(const QJsonObject &json) {
    if (!json.contains("theme") || !json["theme"].isObject()) {
        return;
    }

    QJsonObject theme_json = json["theme"].toObject();

    // **Helper lambda to safely extract string values**
    auto extractString = [&theme_json](const QString &key, QString &target) {
        if (theme_json.contains(key)) {
            target = theme_json[key].toString();
        }
    };

    // **Helper lambda to safely extract integer values**
    auto extractInt = [&theme_json](const QString &key, int &target) {
        if (theme_json.contains(key)) {
            target = theme_json[key].toInt();
        }
    };

    // **Extract theme properties**
    extractString("primary_color", theme_.primary_color);
    extractString("secondary_color", theme_.secondary_color);
    extractString("background_color", theme_.background_color);
    extractString("text_color", theme_.text_color);
    extractString("border_color", theme_.border_color);
    extractString("font_family", theme_.font_family);
    extractInt("font_size", theme_.font_size);
    extractInt("border_radius", theme_.border_radius);
}

/**
 * @brief Deserializes configuration settings from JSON object
 * @param json The JSON object containing configuration data
 */
void UIElement::deserializeConfiguration(const QJsonObject &json) {
    if (!json.contains("configuration") || !json["configuration"].isObject()) {
        return;
    }

    QJsonObject config_json = json["configuration"].toObject();

    if (config_json.contains("performance_monitoring_enabled")) {
        performance_monitoring_enabled_ =
            config_json["performance_monitoring_enabled"].toBool();
    }
    if (config_json.contains("responsive_enabled")) {
        responsive_enabled_ = config_json["responsive_enabled"].toBool();
    }
    if (config_json.contains("current_width")) {
        current_width_ = config_json["current_width"].toInt();
    }
}

/**
 * @brief Applies deserialized data to the widget
 */
void UIElement::applyDeserializedData() {
    if (widget_) {
        applyStoredProperties();
        applyTheme();
    }
}

/**
 * @brief Parses a JSON value into a PropertyValue
 * @param value The JSON value to parse
 * @return Parsed PropertyValue
 */
PropertyValue UIElement::parsePropertyValue(const QJsonValue &value) const {
    if (value.isString()) {
        return PropertyValue{value.toString()};
    } else if (value.isDouble()) {
        double val = value.toDouble();
        // **Handle both int and double**
        if (val == static_cast<int>(val)) {
            return PropertyValue{static_cast<int>(val)};
        } else {
            return PropertyValue{val};
        }
    } else if (value.isBool()) {
        return PropertyValue{value.toBool()};
    } else if (value.isObject()) {
        QJsonObject obj = value.toObject();

        // **Try to deserialize as QSize**
        if (obj.contains("width") && obj.contains("height")) {
            QSize size(obj["width"].toInt(), obj["height"].toInt());
            return PropertyValue{size};
        }
        // **Try to deserialize as QPoint**
        else if (obj.contains("x") && obj.contains("y")) {
            QPoint point(obj["x"].toInt(), obj["y"].toInt());
            return PropertyValue{point};
        }
    }

    // **Return default PropertyValue for unsupported types**
    return PropertyValue{QString("Unsupported type")};
}

// **Lifecycle management implementation**
LifecycleBuilder &UIElement::lifecycle() {
    static thread_local std::unique_ptr<LifecycleBuilder> builder;
    builder = std::make_unique<LifecycleBuilder>(lifecycle_.get());
    return *builder;
}

UIElement &UIElement::onMount(std::function<void()> hook) {
    if (lifecycle_) {
        lifecycle_->onMount([hook](const LifecycleContext &) { hook(); });
    }
    return *this;
}

UIElement &UIElement::onUnmount(std::function<void()> hook) {
    if (lifecycle_) {
        lifecycle_->onUnmount([hook](const LifecycleContext &) { hook(); });
    }
    return *this;
}

UIElement &UIElement::onUpdate(std::function<void()> hook) {
    if (lifecycle_) {
        lifecycle_->onUpdate([hook](const LifecycleContext &) { hook(); });
    }
    return *this;
}

UIElement &UIElement::onError(std::function<void(const QString &)> hook) {
    if (lifecycle_) {
        lifecycle_->onError(
            [hook](const LifecycleContext &ctx) { hook(ctx.error_message); });
    }
    return *this;
}

UIElement &UIElement::useEffect(std::function<std::function<void()>()> effect,
                                const std::vector<QVariant> &dependencies) {
    if (lifecycle_) {
        lifecycle_->useEffect(effect, dependencies);
    }
    return *this;
}

}  // namespace DeclarativeUI::Core
