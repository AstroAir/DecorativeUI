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

}  // namespace DeclarativeUI::Core