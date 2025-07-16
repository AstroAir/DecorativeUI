#pragma once

#include "UIElement.hpp"

#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QWidget>

#include <functional>
#include <memory>
#include <vector>

namespace DeclarativeUI::Core {

// **Builder pattern with fluent interface**
template <QtWidget WidgetType>
class DeclarativeBuilder {
public:
    explicit DeclarativeBuilder();
    ~DeclarativeBuilder() = default;

    // **Fluent property setting**
    template <typename T>
    DeclarativeBuilder &property(const QString &name, T &&value);

    // **Event handling**
    DeclarativeBuilder &on(const QString &event, std::function<void()> handler);

    // **Property binding**
    DeclarativeBuilder &bind(const QString &property,
                             std::function<PropertyValue()> binding);

    // **Child management**
    template <QtWidget ChildType>
    DeclarativeBuilder &child(
        std::function<void(DeclarativeBuilder<ChildType> &)> config);

    // **Layout management**
    template <typename LayoutType>
    DeclarativeBuilder &layout(
        std::function<void(LayoutType *)> config = nullptr);

    // **Build final widget with RAII**
    [[nodiscard]] std::unique_ptr<WidgetType> build();

    // **Exception-safe building**
    [[nodiscard]] std::unique_ptr<WidgetType> buildSafe() noexcept;

private:
    std::unique_ptr<UIElement> element_;
    std::vector<std::function<void(WidgetType *)>> configurators_;
    std::unique_ptr<QLayout> layout_;
    std::vector<std::unique_ptr<QWidget>> children_;

    void applyConfiguration(WidgetType *widget);
};

// **Factory functions for declarative syntax**
template <QtWidget T>
[[nodiscard]] DeclarativeBuilder<T> create() {
    return DeclarativeBuilder<T>{};
}

// **Specialized builders for common widgets**
[[nodiscard]] inline auto button() { return create<QPushButton>(); }
[[nodiscard]] inline auto label() { return create<QLabel>(); }
[[nodiscard]] inline auto widget() { return create<QWidget>(); }

}  // namespace DeclarativeUI::Core


namespace DeclarativeUI::Core {

// **Concrete UIElement implementation for DeclarativeBuilder**
template <QtWidget WidgetType>
class ConcreteUIElement : public UIElement {
public:
    explicit ConcreteUIElement(QObject *parent = nullptr) : UIElement(parent) {}

    void initialize() override {
        try {
            widget_ = std::make_unique<WidgetType>();

            // **Apply stored properties**
            for (const auto &[name, value] : properties_) {
                std::visit(
                    [&](const auto &val) {
                        widget_->setProperty(name.toUtf8().constData(),
                                             QVariant::fromValue(val));
                    },
                    value);
            }

            connectSignals();
        } catch (const std::exception &e) {
            throw DeclarativeUI::Exceptions::ComponentCreationException(
                "Widget: " + std::string(e.what()));
        }
    }
};

template <QtWidget WidgetType>
DeclarativeBuilder<WidgetType>::DeclarativeBuilder() {
    element_ = std::make_unique<ConcreteUIElement<WidgetType>>();
}

template <QtWidget WidgetType>
template <typename T>
DeclarativeBuilder<WidgetType> &DeclarativeBuilder<WidgetType>::property(
    const QString &name, T &&value) {
    if (!element_) {
        throw DeclarativeUI::Exceptions::ComponentCreationException(
            "UIElement is null");
    }

    try {
        element_->setProperty(name, std::forward<T>(value));
        return *this;
    } catch (const std::exception &e) {
        throw DeclarativeUI::Exceptions::PropertyBindingException(
            name.toStdString() + ": " + e.what());
    }
}

template <QtWidget WidgetType>
DeclarativeBuilder<WidgetType> &DeclarativeBuilder<WidgetType>::on(
    const QString &event, std::function<void()> handler) {
    if (!element_) {
        throw DeclarativeUI::Exceptions::ComponentCreationException(
            "UIElement is null");
    }

    if (!handler) {
        throw std::invalid_argument("Event handler cannot be null");
    }

    try {
        element_->onEvent(event, std::move(handler));
        return *this;
    } catch (const std::exception &e) {
        throw DeclarativeUI::Exceptions::PropertyBindingException(
            "Event binding failed for " + event.toStdString() + ": " +
            e.what());
    }
}

template <QtWidget WidgetType>
DeclarativeBuilder<WidgetType> &DeclarativeBuilder<WidgetType>::bind(
    const QString &property, std::function<PropertyValue()> binding) {
    if (!element_) {
        throw DeclarativeUI::Exceptions::ComponentCreationException(
            "UIElement is null");
    }

    if (!binding) {
        throw std::invalid_argument("Binding function cannot be null");
    }

    try {
        element_->bindProperty(property, std::move(binding));
        return *this;
    } catch (const std::exception &e) {
        throw DeclarativeUI::Exceptions::PropertyBindingException(
            property.toStdString() + ": " + e.what());
    }
}

template <QtWidget WidgetType>
template <QtWidget ChildType>
DeclarativeBuilder<WidgetType> &DeclarativeBuilder<WidgetType>::child(
    std::function<void(DeclarativeBuilder<ChildType> &)> config) {
    if (!config) {
        throw std::invalid_argument(
            "Child configuration function cannot be null");
    }

    try {
        DeclarativeBuilder<ChildType> child_builder;
        config(child_builder);

        auto child_widget = child_builder.build();
        if (child_widget) {
            children_.push_back(std::move(child_widget));
        }

        return *this;
    } catch (const std::exception &e) {
        throw DeclarativeUI::Exceptions::ComponentCreationException(
            "Child widget creation failed: " + std::string(e.what()));
    }
}

template <QtWidget WidgetType>
template <typename LayoutType>
DeclarativeBuilder<WidgetType> &DeclarativeBuilder<WidgetType>::layout(
    std::function<void(LayoutType *)> config) {
    try {
        auto new_layout = std::make_unique<LayoutType>();

        if (config) {
            config(new_layout.get());
        }

        layout_ = std::move(new_layout);
        return *this;
    } catch (const std::exception &e) {
        throw DeclarativeUI::Exceptions::LayoutException(
            "Layout creation failed: " + std::string(e.what()));
    }
}

template <QtWidget WidgetType>
std::unique_ptr<WidgetType> DeclarativeBuilder<WidgetType>::build() {
    try {
        // **Create the widget**
        auto widget = std::make_unique<WidgetType>();

        if (!widget) {
            throw DeclarativeUI::Exceptions::ComponentCreationException(
                "Failed to create widget of type: " +
                std::string(typeid(WidgetType).name()));
        }

        // **Apply stored properties**
        if (element_) {
            auto properties = element_->getProperties();
            for (const auto &[name, value] : properties) {
                std::visit(
                    [&](const auto &val) {
                        bool success =
                            widget->setProperty(name.toUtf8().constData(),
                                                QVariant::fromValue(val));
                        if (!success) {
                            qWarning() << "Failed to set property" << name
                                       << "on widget"
                                       << widget->metaObject()->className();
                        }
                    },
                    value);
            }

            // **Setup event handlers**
            const QMetaObject *meta_obj = widget->metaObject();
            auto event_handlers = element_->getEventHandlers();
            for (const auto &[event, handler] : event_handlers) {
                bool signal_connected = false;

                // **Handle specific widget types and signals**
                if (event == "clicked") {
                    if (auto *button =
                            qobject_cast<QPushButton *>(widget.get())) {
                        QObject::connect(button, &QPushButton::clicked,
                                         handler);
                        signal_connected = true;
                        qDebug() << "✅ Connected signal:" << event;
                    }
                }

                if (!signal_connected) {
                    qWarning() << "❌ Failed to connect signal:" << event
                               << "for widget" << meta_obj->className();
                }
            }
        }

        // **Setup layout**
        if (layout_) {
            widget->setLayout(layout_.release());
        }

        // **Add children**
        QLayout *widget_layout = widget->layout();
        for (auto &child : children_) {
            if (widget_layout) {
                widget_layout->addWidget(child.release());
            } else {
                child->setParent(widget.get());
            }
        }

        // **Apply additional configurators**
        applyConfiguration(widget.get());

        return widget;

    } catch (const std::exception &e) {
        throw DeclarativeUI::Exceptions::ComponentCreationException(
            "Widget build failed: " + std::string(e.what()));
    }
}

template <QtWidget WidgetType>
std::unique_ptr<WidgetType>
DeclarativeBuilder<WidgetType>::buildSafe() noexcept {
    try {
        return build();
    } catch (const std::exception &e) {
        qCritical() << "Safe build failed:" << e.what();
        return nullptr;
    } catch (...) {
        qCritical() << "Safe build failed with unknown exception";
        return nullptr;
    }
}

template <QtWidget WidgetType>
void DeclarativeBuilder<WidgetType>::applyConfiguration(WidgetType *widget) {
    if (!widget)
        return;

    for (const auto &configurator : configurators_) {
        try {
            configurator(widget);
        } catch (const std::exception &e) {
            qWarning() << "Configuration failed:" << e.what();
        }
    }
}

}  // namespace DeclarativeUI::Core
