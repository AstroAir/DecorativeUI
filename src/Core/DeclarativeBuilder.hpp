#pragma once

#include "UIElement.hpp"

#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QWidget>

#include <array>
#include <functional>
#include <memory>
#include <tuple>
#include <utility>
#include <vector>

/**
 * @file DeclarativeBuilder.hpp
 * @brief Declarative UI builder utilities for constructing QWidget hierarchies.
 *
 * This header provides a compact, high-performance builder API for creating
 * QWidget-derived objects in a declarative, fluent style. The design goals are:
 *  - clear expressive syntax for constructing widgets and wiring
 * events/properties,
 *  - minimal runtime overhead using move-only builders and small configurator
 * lists,
 *  - defensive error handling that maps construction failures to domain
 * exceptions,
 *  - ability to attach layouts and children, and apply arbitrary configurators.
 *
 * The builder is intentionally lightweight and does not replace Qt's ownership
 * model; constructed widgets are returned as std::unique_ptr and must be
 * parented or transferred into Qt object hierarchies by the caller.
 *
 * Threading:
 *  - Builders are not thread-safe; use them from a single thread (typically the
 *    GUI/main thread) and transfer ownership of produced widgets between
 * threads only via safe Qt mechanisms.
 */

namespace DeclarativeUI::Core {

/**
 * @brief DeclarativeBuilder: fluent builder for QWidget-derived types.
 *
 * Template parameter WidgetType must satisfy is_qt_widget_v (a compile-time
 * trait defined in UIElement / Concepts). The builder supports:
 *  - setting properties (by name) which are applied when build() is called,
 *  - binding logical properties to callback-based PropertyValue providers,
 *  - registering event handlers by name (common signals such as "clicked"),
 *  - adding child widgets using nested builders,
 *  - assigning a layout object to the constructed widget,
 *  - adding arbitrary configurator lambdas for custom initialization steps,
 *  - two build entry points: build() which throws on failure and buildSafe()
 *    which returns nullptr on error.
 *
 * Usage example:
 * @code
 * auto btn = DeclarativeUI::Core::button()
 *                .property("text", QString("Press me"))
 *                .on("clicked", [](){ qDebug() << "pressed"; })
 *                .build();
 * @endcode
 *
 * Performance notes:
 *  - Instances are move-only to avoid copies and to encourage ephemeral use.
 *  - configurators_ is a small dynamic array of callables applied during build.
 *
 * Error model:
 *  - build() throws DeclarativeUI::Exceptions::* exceptions
 * (ComponentCreationException, PropertyBindingException, LayoutException) on
 * failure.
 *  - buildSafe() catches exceptions, logs critically and returns nullptr.
 */
template <typename WidgetType>
class DeclarativeBuilder {
    static_assert(is_qt_widget_v<WidgetType>,
                  "WidgetType must be a QWidget-derived type");

    // NOTE: capability flags are currently simplified; keep for future SFINAE.
    static constexpr bool has_text_property = true;
    static constexpr bool has_icon_property = true;
    static constexpr bool has_enabled_property = true;

public:
    /**
     * @brief Construct an empty DeclarativeBuilder.
     *
     * Initializes an underlying ConcreteUIElement and empty configurator lists.
     * Builders should be configured immediately and then moved or used to call
     * build()/buildSafe().
     */
    explicit DeclarativeBuilder();

    ~DeclarativeBuilder() = default;

    // Move-only semantics to avoid accidental copies and minimize allocations.
    DeclarativeBuilder(const DeclarativeBuilder &) = delete;
    DeclarativeBuilder &operator=(const DeclarativeBuilder &) = delete;
    DeclarativeBuilder(DeclarativeBuilder &&) = default;
    DeclarativeBuilder &operator=(DeclarativeBuilder &&) = default;

    /**
     * @brief Queue a named property to be applied to the widget at build time.
     *
     * The property is stored as a configurator lambda that will call
     * QWidget::setProperty(name, QVariant::fromValue(value)) when the widget
     * is constructed. Exceptions thrown during assignment are converted to
     * PropertyBindingException.
     *
     * @tparam T type of the value (deduced).
     * @param name property name (as used by Qt meta-object/property system).
     * @param value value to store; forwarded into the stored lambda.
     * @return reference to *this for fluent chaining.
     */
    template <typename T>
    DeclarativeBuilder &property(const QString &name, T &&value);

    /**
     * @brief Register an event handler by name.
     *
     * Common event names (e.g. "clicked") are recognized and connected to
     * matching Qt signals where applicable. The handler is stored in the
     * underlying UIElement and applied during build(). If a handler cannot be
     * connected at build time a warning is emitted.
     *
     * @param event textual event name (implementation-specific).
     * @param handler callable to invoke when event is emitted.
     * @return reference to *this for chaining.
     *
     * @throws DeclarativeUI::Exceptions::PropertyBindingException on invalid
     * usage.
     */
    DeclarativeBuilder &on(const QString &event, std::function<void()> handler);

    /**
     * @brief Bind a logical property to a provider function.
     *
     * The binding function should return a PropertyValue when invoked. The
     * binding is stored on the UIElement and consumers may evaluate it at
     * runtime.
     *
     * @param property property name to bind.
     * @param binding function producing PropertyValue.
     * @return reference to *this for fluent chaining.
     */
    DeclarativeBuilder &bind(const QString &property,
                             std::function<PropertyValue()> binding);

    /**
     * @brief Add a child widget using a nested builder configuration.
     *
     * The provided config function receives a builder for the ChildType which
     * must also be a QWidget-derived type. The child is constructed by calling
     * child_builder.build() and ownership of the created widget is stored in
     * children_ until this builder's build() is invoked.
     *
     * @tparam ChildType type of child widget to create.
     * @param config configuration function called with the child builder.
     * @return reference to *this for chaining.
     *
     * @throws DeclarativeUI::Exceptions::ComponentCreationException on failure.
     */
    template <typename ChildType>
    DeclarativeBuilder &child(
        std::function<void(DeclarativeBuilder<ChildType> &)> config);

    /**
     * @brief Assign a layout object to the widget being built.
     *
     * The provided config function may initialize the layout prior to it being
     * set on the widget. Ownership of the layout is transferred into the
     * constructed QWidget during build().
     *
     * @tparam LayoutType QObject-derived QLayout type.
     * @param config optional initializer called with the new layout instance.
     * @return reference to *this for chaining.
     *
     * @throws DeclarativeUI::Exceptions::LayoutException on allocation failure.
     */
    template <typename LayoutType>
    DeclarativeBuilder &layout(
        std::function<void(LayoutType *)> config = nullptr);

    /**
     * @brief Build and return a std::unique_ptr owning the constructed widget.
     *
     * This method performs all stored configuration steps, connects event
     * handlers and attaches children/layout. On failure an appropriate
     * DeclarativeUI::Exceptions::* exception is thrown describing the reason.
     *
     * The returned widget is not parented by the builder; the caller should
     * set a parent or insert it into a Qt hierarchy to ensure proper lifetime.
     *
     * @return unique_ptr owning the constructed WidgetType.
     * @throws DeclarativeUI::Exceptions::ComponentCreationException,
     * LayoutException, ...
     */
    [[nodiscard]] std::unique_ptr<WidgetType> build();

    /**
     * @brief Safe variant of build() that catches exceptions and returns
     * nullptr.
     *
     * Equivalent to calling build() but exceptions are logged and suppressed,
     * returning nullptr on any failure. Use when callers prefer not to handle
     * exceptions at the call site.
     *
     * @return unique_ptr owning the constructed WidgetType, or nullptr on
     * error.
     */
    [[nodiscard]] std::unique_ptr<WidgetType> buildSafe() noexcept;

private:
    std::unique_ptr<UIElement>
        element_;  ///< Underlying UIElement holding properties/events.

    // Stored configurators applied to the raw widget pointer at the end of
    // build.
    std::vector<std::function<void(WidgetType *)>> configurators_;

    // Layout object to attach to the constructed widget (ownership
    // transferred).
    std::unique_ptr<QLayout> layout_;

    // Children created by nested builders; transferred into the widget during
    // build().
    std::vector<std::unique_ptr<QWidget>> children_;

    // Internal flags and optional cached widget for reuse in advanced
    // scenarios.
    mutable bool is_built_ = false;
    mutable bool has_cached_widget_ = false;
    mutable std::unique_ptr<WidgetType> cached_widget_;

    // Apply configurator list to a constructed widget instance.
    void applyConfiguration(WidgetType *widget);

    // Batch application entry; kept separate to allow instrumentation /
    // optimization.
    void applyConfigurationBatch(WidgetType *widget);

    // Thread-local pool for frequently created widget instances (optional
    // optimization).
    static thread_local std::vector<std::unique_ptr<WidgetType>> widget_pool_;
};

/**
 * @brief Convenience factory to create a DeclarativeBuilder<T>.
 *
 * Ensures T is a QWidget-derived type at compile time and returns a fresh
 * builder.
 *
 * @tparam T QWidget-derived type.
 * @return DeclarativeBuilder<T> instance for fluent configuration.
 */
template <typename T>
[[nodiscard]] DeclarativeBuilder<T> create() {
    static_assert(is_qt_widget_v<T>, "T must be a QWidget-derived type");
    return DeclarativeBuilder<T>{};
}

/**
 * @brief Specialized factory helpers for common widgets.
 *
 * Shorthand functions to improve readability of call-sites.
 */
[[nodiscard]] inline auto button() { return create<QPushButton>(); }
[[nodiscard]] inline auto label() { return create<QLabel>(); }
[[nodiscard]] inline auto widget() { return create<QWidget>(); }

}  // namespace DeclarativeUI::Core

namespace DeclarativeUI::Core {

/**
 * @brief ConcreteUIElement: UIElement implementation used by
 * DeclarativeBuilder.
 *
 * ConcreteUIElement stores property/value pairs and event handlers and knows
 * how to initialize a concrete WidgetType instance. Initialization may throw
 * and exceptions are translated into domain-specific exceptions for the caller.
 *
 * Template parameter WidgetType must be a QWidget-derived type.
 */
template <typename WidgetType>
class ConcreteUIElement : public UIElement {
    static_assert(is_qt_widget_v<WidgetType>,
                  "WidgetType must be a QWidget-derived type");

public:
    /**
     * @brief Construct with optional QObject parent.
     * @param parent parent for the UIElement (not the created QWidget).
     */
    explicit ConcreteUIElement(QObject *parent = nullptr) : UIElement(parent) {}

    /**
     * @brief Initialize internal state and create the underlying widget
     * instance.
     *
     * The method constructs a new WidgetType instance, applies stored
     * properties using QVariant conversions and connects any internal
     * UIElement-managed signals/slots. Errors from property application or
     * construction are caught and wrapped in ComponentCreationException for
     * clearer diagnostics.
     */
    void initialize() override {
        try {
            widget_ = std::make_unique<WidgetType>();

            // Apply stored properties using QVariant conversions.
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

template <typename WidgetType>
DeclarativeBuilder<WidgetType>::DeclarativeBuilder() {
    static_assert(is_qt_widget_v<WidgetType>,
                  "WidgetType must be a QWidget-derived type");
    element_ = std::make_unique<ConcreteUIElement<WidgetType>>();
}

/**
 * @brief Register an event handler with validation and exception translation.
 *
 * Stores the event handler on the underlying UIElement. The method validates
 * input and converts standard exceptions into PropertyBindingException or
 * ComponentCreationException as appropriate.
 */
template <typename WidgetType>
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

/**
 * @brief Bind a property to a provider function with validation.
 *
 * The binding is stored and evaluated by the UIElement or consumers at runtime.
 */
template <typename WidgetType>
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

/**
 * @brief Create and store a child widget using a nested builder.
 *
 * The child builder is executed immediately and its produced widget is stored
 * until this builder is built. Errors are translated to
 * ComponentCreationException.
 */
template <typename WidgetType>
template <typename ChildType>
DeclarativeBuilder<WidgetType> &DeclarativeBuilder<WidgetType>::child(
    std::function<void(DeclarativeBuilder<ChildType> &)> config) {
    static_assert(is_qt_widget_v<ChildType>,
                  "ChildType must be a QWidget-derived type");
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

/**
 * @brief Create and assign a layout object to the builder.
 *
 * Allocates a new LayoutType instance, optionally configures it, and stores
 * ownership until build() transfers it to the widget.
 */
template <typename WidgetType>
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

/**
 * @brief Build the widget, apply properties, connect signals and attach
 * children/layout.
 *
 * This function centralizes the construction logic and performs best-effort
 * diagnostics when property application or signal connections fail. Any
 * unhandled errors are wrapped in ComponentCreationException so callers can
 * react consistently.
 */
template <typename WidgetType>
std::unique_ptr<WidgetType> DeclarativeBuilder<WidgetType>::build() {
    try {
        // Create the widget instance
        auto widget = std::make_unique<WidgetType>();

        if (!widget) {
            throw DeclarativeUI::Exceptions::ComponentCreationException(
                "Failed to create widget of type: " +
                std::string(typeid(WidgetType).name()));
        }

        // Apply stored properties from the UIElement (if present)
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

            // Setup event handlers stored in element_
            const QMetaObject *meta_obj = widget->metaObject();
            auto event_handlers = element_->getEventHandlers();
            for (const auto &[event, handler] : event_handlers) {
                bool signal_connected = false;

                // Handle specific event names for common widget types
                // (extendable)
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

        // Attach layout if configured. Ownership of layout_ is transferred.
        if (layout_) {
            widget->setLayout(layout_.release());
        }

        // Add children: if a layout exists add to layout, otherwise set parent.
        QLayout *widget_layout = widget->layout();
        for (auto &child : children_) {
            if (widget_layout) {
                widget_layout->addWidget(child.release());
            } else {
                child->setParent(widget.get());
            }
        }

        // Apply any additional configurator lambdas registered via
        // property(...)
        applyConfiguration(widget.get());

        return widget;

    } catch (const std::exception &e) {
        throw DeclarativeUI::Exceptions::ComponentCreationException(
            "Widget build failed: " + std::string(e.what()));
    }
}

/**
 * @brief Safe build variant that swallows exceptions and logs critical
 * diagnostics.
 *
 * Use when callers prefer a null result on failure rather than exception
 * handling.
 */
template <typename WidgetType>
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

/**
 * @brief Apply stored configurators to the widget instance.
 *
 * Individual configurator lambdas are executed under try/catch to avoid a
 * single failing configurator from aborting the entire build process. Failures
 * are logged via qWarning().
 */
template <typename WidgetType>
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

/**
 * @brief Convenience property setter implemented as a configurator lambda.
 *
 * The property() call records a lambda capturing the property name and value.
 * The lambda converts the value to QVariant when applied during build.
 */
template <typename WidgetType>
template <typename T>
DeclarativeBuilder<WidgetType> &DeclarativeBuilder<WidgetType>::property(
    const QString &name, T &&value) {
    if (!element_) {
        throw DeclarativeUI::Exceptions::ComponentCreationException(
            "UIElement is null");
    }

    try {
        // Store property for later application
        configurators_.emplace_back(
            [name, value = std::forward<T>(value)](WidgetType *widget) {
                if (widget) {
                    widget->setProperty(name.toUtf8().constData(),
                                        QVariant::fromValue(value));
                }
            });
        return *this;
    } catch (const std::exception &e) {
        throw DeclarativeUI::Exceptions::PropertyBindingException(
            "Property setting failed for " + name.toStdString() + ": " +
            e.what());
    }
}

}  // namespace DeclarativeUI::Core
