// Core/UIElement.cpp
#include "UIElement.hpp"
#include <QApplication>
#include <QTimer>

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

    } catch (const std::exception &e) {
        throw Exceptions::UIException("Failed to initialize UIElement: " +
                                      std::string(e.what()));
    }
}

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

void UIElement::cleanup() noexcept {
    try {
        event_handlers_.clear();
        bindings_.clear();
        properties_.clear();
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
    if (!widget_)
        return;

    const QMetaObject *metaObj = widget_->metaObject();

    for (const auto &[event, handler] : event_handlers_) {
        // **Find and connect signals dynamically**
        for (int i = 0; i < metaObj->methodCount(); ++i) {
            QMetaMethod method = metaObj->method(i);

            if (method.methodType() == QMetaMethod::Signal &&
                QString::fromUtf8(method.name()) == event) {
                QMetaObject::invokeMethod(
                    widget_.get(), method.name(), Qt::DirectConnection,
                    Q_ARG(std::function<void()>, handler));
                break;
            }
        }
    }
}

void UIElement::onPropertyChanged() { updateBoundProperties(); }

void UIElement::setWidget(QWidget *widget) {
    if (!widget) {
        throw Exceptions::UIException("Widget cannot be null");
    }

    widget_ = std::unique_ptr<QWidget>(widget);
    applyStoredProperties();
    connectSignals();
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
UIElement &UIElement::animate(const QString &property, const QVariant &target_value,
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
        qWarning() << "Failed to create animation for property" << property << ":" << e.what();
    }

    return *this;
}

UIElement &UIElement::fadeIn(const AnimationConfig &config) {
    if (!config.enabled || !widget_) {
        return *this;
    }

    try {
        auto &engine = Animation::AnimationEngine::instance();
        auto animation = engine.fadeIn(widget_.get(), static_cast<int>(config.duration.count()));

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
        auto animation = engine.fadeOut(widget_.get(), static_cast<int>(config.duration.count()));

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
        auto animation = engine.slideIn(widget_.get(), "left", static_cast<int>(config.duration.count()));

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
        auto animation = engine.slideOut(widget_.get(), "right", static_cast<int>(config.duration.count()));

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
        auto animation = engine.animateProperty(widget_.get(), "windowOpacity",
                                              current_opacity, QVariant(opacity),
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
std::shared_ptr<Animation::Animation> UIElement::createAnimation(const QString &property,
                                                               const QVariant &target_value,
                                                               const AnimationConfig &config) {
    if (!widget_) {
        return nullptr;
    }

    try {
        auto &engine = Animation::AnimationEngine::instance();
        QVariant current_value = widget_->property(property.toUtf8().constData());

        auto animation = engine.animateProperty(widget_.get(), property,
                                              current_value, target_value,
                                              static_cast<int>(config.duration.count()),
                                              config.toAnimationProperties().easing);

        setupAnimation(animation, config);
        return animation;
    } catch (const std::exception &e) {
        qWarning() << "Failed to create animation for property" << property << ":" << e.what();
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
        connect(animation.get(), &Animation::Animation::finished,
                this, &UIElement::onAnimationFinished);
    } catch (const std::exception &e) {
        qWarning() << "Failed to setup animation:" << e.what();
    }
}

}  // namespace DeclarativeUI::Core
