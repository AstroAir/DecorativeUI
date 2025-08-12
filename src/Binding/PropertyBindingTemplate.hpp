#pragma once

#include "PropertyBinding.hpp"
#include "StateManager.hpp"
#include <QMetaObject>
#include <QMetaProperty>
#include <QObject>
#include <QWidget>
#include <QString>
#include <QVariant>
#include <QString>
#include <QVariant>
#include <QWidget>
#include <QDateTime>

#include <functional>
#include <memory>
#include <type_traits>

namespace DeclarativeUI::Binding {

// **Forward declarations**
template <typename T>
class ReactiveProperty;
class ReactivePropertyBase;
class StateManager;

// **Binding expression type traits**
template <typename T>
struct is_bindable_type {
    static constexpr bool value = std::is_copy_constructible_v<T> &&
                                  std::is_move_constructible_v<T>;
};

template <typename T>
constexpr bool is_bindable_type_v = is_bindable_type<T>::value;

template <typename T>
struct is_compute_function {
    static constexpr bool value = std::is_invocable_v<T> &&
                                  is_bindable_type_v<std::invoke_result_t<T>>;
};

template <typename T>
constexpr bool is_compute_function_v = is_compute_function<T>::value;

// **C++20 Concepts (if available)**
#if __cplusplus >= 202002L
template <typename T>
concept BindableType = is_bindable_type_v<T>;

template <typename T>
concept ComputeFunction = is_compute_function_v<T>;
#endif

// Use definitions from PropertyBinding.hpp

// **Property binding implementation (template class without Q_OBJECT)**
template <typename SourceType, typename TargetType = SourceType>
class PropertyBinding : public QObject, public IPropertyBinding {
    static_assert(is_bindable_type_v<SourceType>, "SourceType must be bindable");
    static_assert(is_bindable_type_v<TargetType>, "TargetType must be bindable");
    // Note: Template classes cannot use Q_OBJECT in Qt MOC

public:
    // **Constructors**
    explicit PropertyBinding(QObject *parent = nullptr);

    // **Bind to reactive property**
    PropertyBinding(std::shared_ptr<ReactiveProperty<SourceType>> source,
                    QWidget *target_widget, const QString &target_property,
                    BindingDirection direction = BindingDirection::OneWay,
                    QObject *parent = nullptr);

    // **Bind with custom converter**
    template <typename ConverterFunc>
    PropertyBinding(std::shared_ptr<ReactiveProperty<SourceType>> source,
                    QWidget *target_widget, const QString &target_property,
                    ConverterFunc converter,
                    BindingDirection direction = BindingDirection::OneWay,
                    QObject *parent = nullptr);

    // **Bind with computed value**
    template <typename ComputeFunc>
    PropertyBinding(ComputeFunc compute_func,
                    QWidget *target_widget, const QString &target_property,
                    UpdateMode update_mode = UpdateMode::Immediate,
                    QObject *parent = nullptr);

    // **Destructor**
    ~PropertyBinding() override;

    // **IPropertyBinding interface**
    void update() override;
    void disconnect() override;
    bool isValid() const override;
    QString getSourcePath() const override;
    QString getTargetPath() const override;
    BindingDirection getDirection() const override;

    // **Property binding specific methods**
    void setUpdateMode(UpdateMode mode);
    UpdateMode getUpdateMode() const;

    void setEnabled(bool enabled);
    bool isEnabled() const;

    void setValidator(std::function<bool(const TargetType&)> validator);
    void setConverter(std::function<TargetType(const SourceType&)> converter);

    // **Error handling**
    void setErrorHandler(std::function<void(const QString&)> handler);

    // **Performance monitoring**
    quint64 getUpdateCount() const;
    qint64 getLastUpdateTime() const;

private:
    // **Private members**
    std::shared_ptr<ReactiveProperty<SourceType>> m_source;
    QWidget *m_target_widget;
    QString m_target_property;
    BindingDirection m_direction;
    UpdateMode m_update_mode;
    bool m_enabled;
    bool m_valid;

    // **Function objects**
    std::function<TargetType(const SourceType&)> m_converter;
    std::function<bool(const TargetType&)> m_validator;
    std::function<void(const QString&)> m_error_handler;

    // **Performance tracking**
    quint64 m_update_count;
    qint64 m_last_update_time;

    // **Connection management**
    QMetaObject::Connection m_source_connection;
    QMetaObject::Connection m_target_connection;

    // **Private methods**
    void updateTargetFromSource();
    void updateSourceFromTarget();
    TargetType convertSourceToTarget(const SourceType &source_value);
    SourceType convertTargetToSource(const TargetType &target_value);
    bool validateTargetValue(const TargetType &value);
    void handleError(const QString &error_message);
    QString generateSourcePath() const;
    QString generateTargetPath() const;
};

// **Template implementation (must be in header for templates)**

// **Default constructor**
template <typename SourceType, typename TargetType>
PropertyBinding<SourceType, TargetType>::PropertyBinding(QObject *parent)
    : QObject(parent), m_source(nullptr), m_target_widget(nullptr),
      m_direction(BindingDirection::OneWay), m_update_mode(UpdateMode::Immediate),
      m_enabled(true), m_valid(false), m_update_count(0), m_last_update_time(0) {
}

// **Bind to reactive property**
template <typename SourceType, typename TargetType>
PropertyBinding<SourceType, TargetType>::PropertyBinding(
    std::shared_ptr<ReactiveProperty<SourceType>> source,
    QWidget *target_widget, const QString &target_property,
    BindingDirection direction, QObject *parent)
    : QObject(parent), m_source(source), m_target_widget(target_widget),
      m_target_property(target_property), m_direction(direction),
      m_update_mode(UpdateMode::Immediate), m_enabled(true), m_valid(false),
      m_update_count(0), m_last_update_time(0) {

    if (m_source && m_target_widget) {
        // Set up source to target connection (not for OneTime bindings)
        if (m_direction != BindingDirection::OneTime) {
            m_source_connection = QObject::connect(
                m_source.get(), &ReactivePropertyBase::valueChanged,
                this, [this]() {
                    if (m_enabled && m_update_mode == UpdateMode::Immediate) {
                        updateTargetFromSource();
                    }
                });
        }

        // Set up target to source connection for two-way binding
        if (m_direction == BindingDirection::TwoWay) {
            // Connect to target widget's property change signal if available
            const QMetaObject *metaObject = m_target_widget->metaObject();
            int propIndex = metaObject->indexOfProperty(m_target_property.toLocal8Bit().data());
            if (propIndex != -1) {
                QMetaProperty metaProp = metaObject->property(propIndex);
                if (metaProp.hasNotifySignal()) {
                    QMetaMethod notifySignal = metaProp.notifySignal();
                    QMetaMethod updateSlot = this->metaObject()->method(
                        this->metaObject()->indexOfSlot("updateSourceFromTarget()"));
                    m_target_connection = QObject::connect(
                        m_target_widget, notifySignal, this, updateSlot);
                }
            }
        }

        m_valid = true;

        // Initial update (always perform, even for OneTime bindings)
        updateTargetFromSource();
    }
}

// **Bind with custom converter**
template <typename SourceType, typename TargetType>
template <typename ConverterFunc>
PropertyBinding<SourceType, TargetType>::PropertyBinding(
    std::shared_ptr<ReactiveProperty<SourceType>> source,
    QWidget *target_widget, const QString &target_property,
    ConverterFunc converter, BindingDirection direction, QObject *parent)
    : QObject(parent), m_source(source), m_target_widget(target_widget),
      m_target_property(target_property), m_direction(direction),
      m_update_mode(UpdateMode::Immediate), m_enabled(true), m_valid(false),
      m_update_count(0), m_last_update_time(0) {

    // Set converter before initialization
    m_converter = converter;

    // Now perform the same initialization as the base constructor
    if (m_source && m_target_widget) {
        // Set up source to target connection (not for OneTime bindings)
        if (m_direction != BindingDirection::OneTime) {
            m_source_connection = QObject::connect(
                m_source.get(), &ReactivePropertyBase::valueChanged,
                this, [this]() {
                    if (m_enabled && m_update_mode == UpdateMode::Immediate) {
                        updateTargetFromSource();
                    }
                });
        }

        // Set up target to source connection for two-way binding
        if (m_direction == BindingDirection::TwoWay) {
            // Connect to target widget's property change signal if available
            const QMetaObject *metaObject = m_target_widget->metaObject();
            int propIndex = metaObject->indexOfProperty(m_target_property.toLocal8Bit().data());
            if (propIndex != -1) {
                QMetaProperty metaProp = metaObject->property(propIndex);
                if (metaProp.hasNotifySignal()) {
                    QMetaMethod notifySignal = metaProp.notifySignal();
                    QMetaMethod updateSlot = this->metaObject()->method(
                        this->metaObject()->indexOfSlot("updateSourceFromTarget()"));
                    m_target_connection = QObject::connect(
                        m_target_widget, notifySignal, this, updateSlot);
                }
            }
        }

        m_valid = true;

        // Initial update (always perform, even for OneTime bindings)
        updateTargetFromSource();
    }
}

// **Bind with computed value**
template <typename SourceType, typename TargetType>
template <typename ComputeFunc>
PropertyBinding<SourceType, TargetType>::PropertyBinding(
    ComputeFunc compute_func, QWidget *target_widget,
    const QString &target_property, UpdateMode update_mode, QObject *parent)
    : QObject(parent), m_source(nullptr), m_target_widget(target_widget),
      m_target_property(target_property), m_direction(BindingDirection::OneWay),
      m_update_mode(update_mode), m_enabled(true), m_valid(false),
      m_update_count(0), m_last_update_time(0) {

    if (m_target_widget) {
        // Create a converter that calls the compute function
        m_converter = [compute_func](const SourceType&) -> TargetType {
            return compute_func();
        };

        m_valid = true;

        // Initial update (always perform for compute functions to set initial value)
        update();
    }
}

// **Destructor**
template <typename SourceType, typename TargetType>
PropertyBinding<SourceType, TargetType>::~PropertyBinding() {
    disconnect();
}

// **IPropertyBinding interface implementations**
template <typename SourceType, typename TargetType>
void PropertyBinding<SourceType, TargetType>::update() {
    if (!m_valid || !m_enabled) return;

    updateTargetFromSource();
}

template <typename SourceType, typename TargetType>
void PropertyBinding<SourceType, TargetType>::disconnect() {
    if (m_source_connection) {
        QObject::disconnect(m_source_connection);
        m_source_connection = {};
    }

    if (m_target_connection) {
        QObject::disconnect(m_target_connection);
        m_target_connection = {};
    }

    m_valid = false;
}

template <typename SourceType, typename TargetType>
bool PropertyBinding<SourceType, TargetType>::isValid() const {
    return m_valid;
}

template <typename SourceType, typename TargetType>
QString PropertyBinding<SourceType, TargetType>::getSourcePath() const {
    return generateSourcePath();
}

template <typename SourceType, typename TargetType>
QString PropertyBinding<SourceType, TargetType>::getTargetPath() const {
    return generateTargetPath();
}

template <typename SourceType, typename TargetType>
BindingDirection PropertyBinding<SourceType, TargetType>::getDirection() const {
    return m_direction;
}

// **Property binding specific methods**
template <typename SourceType, typename TargetType>
void PropertyBinding<SourceType, TargetType>::setUpdateMode(UpdateMode mode) {
    m_update_mode = mode;
}

template <typename SourceType, typename TargetType>
UpdateMode PropertyBinding<SourceType, TargetType>::getUpdateMode() const {
    return m_update_mode;
}

template <typename SourceType, typename TargetType>
void PropertyBinding<SourceType, TargetType>::setEnabled(bool enabled) {
    m_enabled = enabled;
}

template <typename SourceType, typename TargetType>
bool PropertyBinding<SourceType, TargetType>::isEnabled() const {
    return m_enabled;
}

template <typename SourceType, typename TargetType>
void PropertyBinding<SourceType, TargetType>::setValidator(
    std::function<bool(const TargetType&)> validator) {
    m_validator = validator;
}

template <typename SourceType, typename TargetType>
void PropertyBinding<SourceType, TargetType>::setConverter(
    std::function<TargetType(const SourceType&)> converter) {
    m_converter = converter;
}

template <typename SourceType, typename TargetType>
void PropertyBinding<SourceType, TargetType>::setErrorHandler(
    std::function<void(const QString&)> handler) {
    m_error_handler = handler;
}

template <typename SourceType, typename TargetType>
quint64 PropertyBinding<SourceType, TargetType>::getUpdateCount() const {
    return m_update_count;
}

template <typename SourceType, typename TargetType>
qint64 PropertyBinding<SourceType, TargetType>::getLastUpdateTime() const {
    return m_last_update_time;
}

// **Private method implementations**
template <typename SourceType, typename TargetType>
void PropertyBinding<SourceType, TargetType>::updateTargetFromSource() {

    // Check preconditions (note: m_source can be null for compute function bindings)
    if (!m_target_widget || !m_valid || !m_enabled) return;
    try {
        TargetType target_value;

        if (m_source) {
            // Normal binding with source property
            SourceType source_value = m_source->get();
            target_value = convertSourceToTarget(source_value);
        } else if (m_converter) {
            // Compute function binding (no source, converter contains the compute function)
            // Use a dummy source value since the converter ignores it for compute functions
            SourceType dummy_source{};
            target_value = m_converter(dummy_source);
        } else {
            handleError("No source property or compute function available");
            return;
        }

        if (m_validator && !validateTargetValue(target_value)) {
            handleError("Validation failed for target value");
            return;
        }

        // Set the property on the target widget
        bool success = m_target_widget->setProperty(
            m_target_property.toLocal8Bit().data(),
            QVariant::fromValue(target_value));

        if (success) {
            m_update_count++;
            m_last_update_time = QDateTime::currentMSecsSinceEpoch();
        } else {
            handleError("Failed to set property on target widget");
        }
    } catch (const std::exception &e) {
        handleError(QString("Exception during update: %1").arg(e.what()));
    }
}

template <typename SourceType, typename TargetType>
void PropertyBinding<SourceType, TargetType>::updateSourceFromTarget() {
    if (!m_source || !m_target_widget || !m_valid || !m_enabled) return;
    if (m_direction != BindingDirection::TwoWay) return;

    try {
        QVariant target_variant = m_target_widget->property(
            m_target_property.toLocal8Bit().data());

        if (target_variant.isValid()) {
            TargetType target_value = target_variant.value<TargetType>();
            SourceType source_value = convertTargetToSource(target_value);

            // Update the source (this will trigger valueChanged signal)
            m_source->set(source_value);
        }
    } catch (const std::exception &e) {
        handleError(QString("Exception during reverse update: %1").arg(e.what()));
    }
}

template <typename SourceType, typename TargetType>
TargetType PropertyBinding<SourceType, TargetType>::convertSourceToTarget(
    const SourceType &source_value) {
    if (m_converter) {
        return m_converter(source_value);
    }

    // Default conversion: try direct assignment if types are compatible
    if constexpr (std::is_same_v<SourceType, TargetType>) {
        return source_value;
    } else {
        // Try QVariant conversion
        QVariant variant = QVariant::fromValue(source_value);
        if (variant.canConvert<TargetType>()) {
            return variant.value<TargetType>();
        }

        // Fallback to default construction
        return TargetType{};
    }
}

template <typename SourceType, typename TargetType>
SourceType PropertyBinding<SourceType, TargetType>::convertTargetToSource(
    const TargetType &target_value) {
    // Default conversion: try direct assignment if types are compatible
    if constexpr (std::is_same_v<SourceType, TargetType>) {
        return target_value;
    } else {
        // Try QVariant conversion
        QVariant variant = QVariant::fromValue(target_value);
        if (variant.canConvert<SourceType>()) {
            return variant.value<SourceType>();
        }

        // Fallback to default construction
        return SourceType{};
    }
}

template <typename SourceType, typename TargetType>
bool PropertyBinding<SourceType, TargetType>::validateTargetValue(
    const TargetType &value) {
    if (m_validator) {
        return m_validator(value);
    }
    return true; // No validator means always valid
}

template <typename SourceType, typename TargetType>
void PropertyBinding<SourceType, TargetType>::handleError(
    const QString &error_message) {
    if (m_error_handler) {
        m_error_handler(error_message);
    }
    // Could also log to console or emit a signal
}

template <typename SourceType, typename TargetType>
QString PropertyBinding<SourceType, TargetType>::generateSourcePath() const {
    if (m_source) {
        return QString("ReactiveProperty@%1").arg(
            reinterpret_cast<quintptr>(m_source.get()), 0, 16);
    }
    return "No Source";
}

template <typename SourceType, typename TargetType>
QString PropertyBinding<SourceType, TargetType>::generateTargetPath() const {
    if (m_target_widget) {
        return QString("%1::%2").arg(
            m_target_widget->metaObject()->className(),
            m_target_property);
    }
    return "No Target";
}

}  // namespace DeclarativeUI::Binding
