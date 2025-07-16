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


namespace DeclarativeUI::JSON {

// **Factory function concept**
template <typename T>
concept ComponentFactoryConcept = requires(T t, const QJsonObject &config) {
    { t(config) } -> std::convertible_to<std::unique_ptr<QWidget>>;
};

// **Component factory interface**
class IComponentFactory {
public:
    virtual ~IComponentFactory() = default;
    virtual std::unique_ptr<QWidget> create(const QJsonObject &config) = 0;
    virtual QString getTypeName() const = 0;
};

// **Template-based component factory**
template <typename WidgetType>
class ComponentFactoryImpl : public IComponentFactory {
public:
    explicit ComponentFactoryImpl(
        std::function<std::unique_ptr<WidgetType>(const QJsonObject &)> factory)
        : factory_(std::move(factory)) {}

    std::unique_ptr<QWidget> create(const QJsonObject &config) override {
        return factory_(config);
    }

    QString getTypeName() const override {
        return QString::fromUtf8(typeid(WidgetType).name());
    }

private:
    std::function<std::unique_ptr<WidgetType>(const QJsonObject &)> factory_;
};

// **Global component registry**
class ComponentRegistry {
public:
    static ComponentRegistry &instance();

    // **Register component factory**
    template <typename WidgetType>
    void registerComponent(
        const QString &type_name,
        std::function<std::unique_ptr<WidgetType>(const QJsonObject &)>
            factory);

    // **Create component from JSON**
    [[nodiscard]] std::unique_ptr<QWidget> createComponent(
        const QString &type_name, const QJsonObject &config);

    // **Check if component is registered**
    [[nodiscard]] bool hasComponent(const QString &type_name) const noexcept;

    // **Get all registered component types**
    [[nodiscard]] QStringList getRegisteredTypes() const;

    // **Clear registry (for testing)**
    void clear() noexcept;

private:
    ComponentRegistry() { registerBuiltinComponents(); }

    std::unordered_map<QString, std::unique_ptr<IComponentFactory>> factories_;

    void registerBuiltinComponents();
};

// **Template implementation**
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
