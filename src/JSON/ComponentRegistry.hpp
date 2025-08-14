// JSON/ComponentRegistry.hpp
#pragma once

#include <QJsonObject>
#include <QString>
#include <QWidget>
#include <concepts>
#include <functional>
#include <memory>
#include <unordered_map>
#include "../Exceptions/UIExceptions.hpp"

/**
 * @file ComponentRegistry.hpp
 * @brief Registry and factory utilities for creating QWidget-based components
 * from JSON.
 *
 * This header defines a small framework used by the DeclarativeUI system to:
 *  - register factories that produce QWidget-derived objects given a
 * QJsonObject
 *  - create components by looking up a factory by a textual type name
 *
 * Factories are type-safe wrappers around std::function-based factory callables
 * and are stored via a non-templated interface so they can be held in a single
 * container indexed by QString.
 *
 * All documented interfaces are exception-safe: registration or creation errors
 * will throw Exceptions::ComponentRegistrationException or related types
 * defined in ../Exceptions/UIExceptions.hpp.
 */

namespace DeclarativeUI::JSON {

/**
 * @brief Concept describing the required signature of a component factory
 * callable.
 *
 * The callable must be invocable with a const QJsonObject& and return a
 * std::unique_ptr<QWidget> (or a type convertible to that).
 *
 * @tparam T Callable type to validate.
 */
template <typename T>
concept ComponentFactoryConcept = requires(T t, const QJsonObject &config) {
    { t(config) } -> std::convertible_to<std::unique_ptr<QWidget>>;
};

/**
 * @brief Abstract non-templated interface for component factories.
 *
 * This interface allows storing concrete factories in a single container
 * without exposing template parameters. Concrete factories implement `create`
 * to produce a QWidget instance from a QJsonObject configuration.
 */
class IComponentFactory {
public:
    virtual ~IComponentFactory() = default;

    /**
     * @brief Create a QWidget instance from json configuration.
     * @param config JSON object containing the configuration for the widget.
     * @return A unique_ptr to the created QWidget. Ownership is transferred to
     * the caller.
     *
     * Implementations should throw a descriptive exception (from UIExceptions)
     * on error.
     */
    virtual std::unique_ptr<QWidget> create(const QJsonObject &config) = 0;

    /**
     * @brief Returns a human-readable type name for the factory.
     * @return QString containing the factory/ widget type name.
     *
     * The returned string is used primarily for diagnostics and debugging.
     */
    virtual QString getTypeName() const = 0;
};

/**
 * @brief Template-based concrete factory implementing IComponentFactory.
 *
 * Wraps a std::function that returns std::unique_ptr<WidgetType> and adapts it
 * to the non-templated IComponentFactory interface.
 *
 * @tparam WidgetType Concrete QWidget-derived type produced by the factory
 * callable.
 */
template <typename WidgetType>
class ComponentFactoryImpl : public IComponentFactory {
public:
    /**
     * @brief Construct from a std::function that produces WidgetType instances.
     * @param factory Callable that accepts const QJsonObject& and returns
     * std::unique_ptr<WidgetType>.
     */
    explicit ComponentFactoryImpl(
        std::function<std::unique_ptr<WidgetType>(const QJsonObject &)> factory)
        : factory_(std::move(factory)) {}

    /**
     * @brief Create a widget using the stored factory callable.
     * @param config Configuration passed to the concrete factory.
     * @return std::unique_ptr<QWidget> owning the created widget.
     */
    std::unique_ptr<QWidget> create(const QJsonObject &config) override {
        return factory_(config);
    }

    /**
     * @brief Get a type name representing WidgetType.
     * @return QString with a name derived from typeid(WidgetType).name()
     *
     * Note: this is primarily for diagnostics and may be mangled; users may
     * prefer to provide explicit names when registering.
     */
    QString getTypeName() const override {
        return QString::fromUtf8(typeid(WidgetType).name());
    }

private:
    std::function<std::unique_ptr<WidgetType>(const QJsonObject &)> factory_;
};

/**
 * @brief Global registry mapping textual type names to component factories.
 *
 * ComponentRegistry is a singleton that stores factories keyed by QString type
 * names. It supports registering new component factories, creating instances by
 * type name, querying registered types, and clearing the registry (useful in
 * tests).
 *
 * Threading: This class is not documented as thread-safe. If access from
 * multiple threads is required, add external synchronization.
 */
class ComponentRegistry {
public:
    /**
     * @brief Get the singleton instance of the registry.
     * @return Reference to the global ComponentRegistry.
     */
    static ComponentRegistry &instance();

    /**
     * @brief Register a factory that produces WidgetType instances.
     *
     * The provided type_name is used as the lookup key for subsequent
     * createComponent calls. The factory callable must accept a const
     * QJsonObject& and return a std::unique_ptr<WidgetType>.
     *
     * @tparam WidgetType The concrete QWidget-derived type produced.
     * @param type_name Textual name used to identify the component type.
     * @param factory Callable conforming to
     * std::function<std::unique_ptr<WidgetType>(const QJsonObject&)>
     * @throws Exceptions::ComponentRegistrationException if type_name is empty,
     * factory is null, or an internal error occurs during registration.
     */
    template <typename WidgetType>
    void registerComponent(
        const QString &type_name,
        std::function<std::unique_ptr<WidgetType>(const QJsonObject &)>
            factory);

    /**
     * @brief Create a component instance given its registered type name and
     * JSON config.
     * @param type_name Registered type name.
     * @param config Configuration object passed to the factory.
     * @return A unique_ptr<QWidget> holding the created component.
     * @throws Exceptions::ComponentRegistrationException if the type is not
     * registered or creation fails.
     */
    [[nodiscard]] std::unique_ptr<QWidget> createComponent(
        const QString &type_name, const QJsonObject &config);

    /**
     * @brief Check whether a component type is registered.
     * @param type_name Type name to query.
     * @return true if a factory is registered for type_name, false otherwise.
     */
    [[nodiscard]] bool hasComponent(const QString &type_name) const noexcept;

    /**
     * @brief Retrieve a list of all registered component type names.
     * @return QStringList with registered type names. Order is unspecified.
     */
    [[nodiscard]] QStringList getRegisteredTypes() const;

    /**
     * @brief Clear all registered factories.
     *
     * Intended for use in unit tests to reset global state. After calling
     * clear, builtin components will not be re-registered automatically unless
     * registerBuiltinComponents() is invoked (constructor registers builtins).
     */
    void clear() noexcept;

private:
    /**
     * @brief Private constructor; registers builtin components.
     *
     * Use ComponentRegistry::instance() to obtain the singleton.
     */
    ComponentRegistry() { registerBuiltinComponents(); }

    std::unordered_map<QString, std::unique_ptr<IComponentFactory>> factories_;

    /**
     * @brief Register built-in component types.
     *
     * Implementations should populate factories_ with any components that are
     * always available in the system. This function is called from the
     * constructor and is not intended to be public.
     */
    void registerBuiltinComponents();
};

/**
 * @brief Template implementation of registerComponent.
 *
 * Validates inputs and wraps the provided factory callable in a
 * ComponentFactoryImpl so it can be stored in the non-templated container.
 *
 * @tparam WidgetType Concrete widget type the factory returns.
 * @param type_name Key used to register the factory.
 * @param factory Callable producing std::unique_ptr<WidgetType>.
 * @throws Exceptions::ComponentRegistrationException on invalid input or if
 *         the underlying factory creation throws.
 */
template <typename WidgetType>
void ComponentRegistry::registerComponent(
    const QString &type_name,
    std::function<std::unique_ptr<WidgetType>(const QJsonObject &)> factory) {
    if (type_name.isEmpty()) {
        throw Exceptions::ComponentRegistrationException(
            "Empty component type name");
    }

    if (!factory) {
        throw Exceptions::ComponentRegistrationException(
            type_name.toStdString() + ": Factory function is null");
    }

    try {
        auto component_factory =
            std::make_unique<ComponentFactoryImpl<WidgetType>>(
                std::move(factory));

        factories_[type_name] = std::move(component_factory);
    } catch (const std::exception &e) {
        throw Exceptions::ComponentRegistrationException(
            type_name.toStdString() + ": " + e.what());
    }
}

}  // namespace DeclarativeUI::JSON
