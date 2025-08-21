#pragma once

/**
 * @file PropertyBindingTemplate.hpp
 * @brief Provides a template-based implementation of property binding for
 * Qt-based UI, supporting type-safe, reactive, and computed bindings with
 * conversion, validation, and performance monitoring.
 */

#include <QDateTime>
#include <QMetaObject>
#include <QMetaProperty>
#include <QObject>
#include <QPointer>
#include <QString>
#include <QVariant>
#include <QWidget>
#include "PropertyBinding.hpp"
#include "StateManager.hpp"

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

/**
 * @struct is_bindable_type
 * @brief Type trait to determine if a type is suitable for property binding.
 *        A bindable type must be copy-constructible and move-constructible.
 * @tparam T The type to check.
 */
template <typename T>
struct is_bindable_type {
    static constexpr bool value =
        std::is_copy_constructible_v<T> && std::is_move_constructible_v<T>;
};

/**
 * @brief Helper variable template for is_bindable_type.
 */
template <typename T>
constexpr bool is_bindable_type_v = is_bindable_type<T>::value;

/**
 * @struct is_compute_function
 * @brief Type trait to determine if a type is a valid compute function for
 * binding. A compute function must be invocable and return a bindable type.
 * @tparam T The function type to check.
 */
template <typename T>
struct is_compute_function {
    static constexpr bool value =
        std::is_invocable_v<T> && is_bindable_type_v<std::invoke_result_t<T>>;
};

/**
 * @brief Helper variable template for is_compute_function.
 */
template <typename T>
constexpr bool is_compute_function_v = is_compute_function<T>::value;

// **C++20 Concepts (if available)**
#if __cplusplus >= 202002L
/**
 * @concept BindableType
 * @brief Concept for types that can be used in property binding.
 */
template <typename T>
concept BindableType = is_bindable_type_v<T>;

/**
 * @concept ComputeFunction
 * @brief Concept for functions that can be used as compute functions in
 * binding.
 */
template <typename T>
concept ComputeFunction = is_compute_function_v<T>;
#endif

// Use definitions from PropertyBinding.hpp

/**
 * @class PropertyBinding
 * @brief Template-based implementation of a property binding between a reactive
 * source and a QWidget property.
 *
 * Supports one-way, two-way, and computed bindings, with optional type
 * conversion, validation, error handling, and performance tracking.
 *
 * @tparam SourceType The type of the source property.
 * @tparam TargetType The type of the target property (defaults to SourceType).
 */
template <typename SourceType, typename TargetType = SourceType>
class PropertyBinding : public QObject, public IPropertyBinding {
    static_assert(is_bindable_type_v<SourceType>,
                  "SourceType must be bindable");
    static_assert(is_bindable_type_v<TargetType>,
                  "TargetType must be bindable");
    // Note: Template classes cannot use Q_OBJECT in Qt MOC

public:
    /**
     * @brief Default constructor.
     * @param parent Parent QObject.
     */
    explicit PropertyBinding(QObject *parent = nullptr);

    /**
     * @brief Constructs a binding between a reactive property and a QWidget
     * property.
     * @param source Shared pointer to the reactive source property.
     * @param target_widget Pointer to the target QWidget.
     * @param target_property Name of the target property.
     * @param direction Binding direction (OneWay, TwoWay, OneTime).
     * @param parent Parent QObject.
     */
    PropertyBinding(std::shared_ptr<ReactiveProperty<SourceType>> source,
                    QWidget *target_widget, const QString &target_property,
                    BindingDirection direction = BindingDirection::OneWay,
                    QObject *parent = nullptr);

    /**
     * @brief Constructs a binding with a custom converter function.
     * @tparam ConverterFunc Type of the converter function.
     * @param source Shared pointer to the reactive source property.
     * @param target_widget Pointer to the target QWidget.
     * @param target_property Name of the target property.
     * @param converter Function to convert from SourceType to TargetType.
     * @param direction Binding direction.
     * @param parent Parent QObject.
     */
    template <typename ConverterFunc>
    PropertyBinding(std::shared_ptr<ReactiveProperty<SourceType>> source,
                    QWidget *target_widget, const QString &target_property,
                    ConverterFunc converter,
                    BindingDirection direction = BindingDirection::OneWay,
                    QObject *parent = nullptr);

    /**
     * @brief Constructs a binding using a compute function (no source
     * property).
     * @tparam ComputeFunc Type of the compute function.
     * @param compute_func Function that computes the target value.
     * @param target_widget Pointer to the target QWidget.
     * @param target_property Name of the target property.
     * @param update_mode Update mode (Immediate, Deferred, Manual).
     * @param parent Parent QObject.
     */
    template <typename ComputeFunc>
    PropertyBinding(ComputeFunc compute_func, QWidget *target_widget,
                    const QString &target_property,
                    UpdateMode update_mode = UpdateMode::Immediate,
                    QObject *parent = nullptr);

    /**
     * @brief Destructor. Disconnects all signal connections.
     */
    ~PropertyBinding() override;

    // **IPropertyBinding interface**

    /**
     * @brief Updates the target property from the source or compute function.
     */
    void update() override;

    /**
     * @brief Disconnects the binding, removing all signal connections.
     */
    void disconnect() override;

    /**
     * @brief Checks if the binding is still valid.
     * @return True if valid, false otherwise.
     */
    bool isValid() const override;

    /**
     * @brief Gets the source property path as a string.
     * @return Source property path.
     */
    QString getSourcePath() const override;

    /**
     * @brief Gets the target property path as a string.
     * @return Target property path.
     */
    QString getTargetPath() const override;

    /**
     * @brief Gets the binding direction.
     * @return BindingDirection value.
     */
    BindingDirection getDirection() const override;

    // **Property binding specific methods**

    /**
     * @brief Sets the update mode for the binding.
     * @param mode Update mode (Immediate, Deferred, Manual).
     */
    void setUpdateMode(UpdateMode mode);

    /**
     * @brief Gets the current update mode.
     * @return UpdateMode value.
     */
    UpdateMode getUpdateMode() const;

    /**
     * @brief Enables or disables the binding.
     * @param enabled True to enable, false to disable.
     */
    void setEnabled(bool enabled);

    /**
     * @brief Checks if the binding is enabled.
     * @return True if enabled, false otherwise.
     */
    bool isEnabled() const;

    /**
     * @brief Sets a validator function for the target value.
     * @param validator Function that returns true if the value is valid.
     */
    void setValidator(std::function<bool(const TargetType &)> validator);

    /**
     * @brief Sets a converter function from SourceType to TargetType.
     * @param converter Function to convert source value to target value.
     */
    void setConverter(std::function<TargetType(const SourceType &)> converter);

    // **Error handling**

    /**
     * @brief Sets an error handler function for binding errors.
     * @param handler Function that receives error messages.
     */
    void setErrorHandler(std::function<void(const QString &)> handler);

    // **Performance monitoring**

    /**
     * @brief Gets the number of successful updates performed by this binding.
     * @return Update count.
     */
    quint64 getUpdateCount() const;

    /**
     * @brief Gets the timestamp of the last update (in milliseconds since
     * epoch).
     * @return Last update time.
     */
    qint64 getLastUpdateTime() const;

private:
    // **Private members**

    std::shared_ptr<ReactiveProperty<SourceType>>
        m_source;  ///< Source reactive property.
    QPointer<QWidget>
        m_target_widget;  ///< Target QWidget (guarded, nulls when deleted).
    QString m_target_property;     ///< Name of the target property.
    BindingDirection m_direction;  ///< Binding direction.
    UpdateMode m_update_mode;      ///< Update mode.
    bool m_enabled;                ///< Whether the binding is enabled.
    bool m_valid;                  ///< Whether the binding is valid.

    // **Function objects**
    std::function<TargetType(const SourceType &)>
        m_converter;  ///< Converter function.
    std::function<bool(const TargetType &)>
        m_validator;  ///< Validator function.
    std::function<void(const QString &)>
        m_error_handler;  ///< Error handler function.

    // **Performance tracking**
    quint64 m_update_count;     ///< Number of updates performed.
    qint64 m_last_update_time;  ///< Timestamp of last update.

    // **Connection management**
    QMetaObject::Connection
        m_source_connection;  ///< Connection to source property changes.
    QMetaObject::Connection
        m_target_connection;  ///< Connection to target property changes.

    // **Private methods**

    /**
     * @brief Updates the target property from the source value.
     */
    void updateTargetFromSource();

    /**
     * @brief Updates the source property from the target value (for two-way
     * binding).
     */
    void updateSourceFromTarget();

    /**
     * @brief Converts a source value to a target value using the converter or
     * default conversion.
     * @param source_value The source value.
     * @return Converted target value.
     */
    TargetType convertSourceToTarget(const SourceType &source_value);

    /**
     * @brief Converts a target value to a source value using default
     * conversion.
     * @param target_value The target value.
     * @return Converted source value.
     */
    SourceType convertTargetToSource(const TargetType &target_value);

    /**
     * @brief Validates the target value using the validator function, if set.
     * @param value The target value.
     * @return True if valid, false otherwise.
     */
    bool validateTargetValue(const TargetType &value);

    /**
     * @brief Handles errors by invoking the error handler, if set.
     * @param error_message The error message.
     */
    void handleError(const QString &error_message);

    /**
     * @brief Generates a string representation of the source path.
     * @return Source path string.
     */
    QString generateSourcePath() const;

    /**
     * @brief Generates a string representation of the target path.
     * @return Target path string.
     */
    QString generateTargetPath() const;
};

//
// Template implementation follows (must be in header for templates)
//

/**
 * @brief Default constructor. Initializes members to default values.
 * @param parent Parent QObject.
 */
template <typename SourceType, typename TargetType>
PropertyBinding<SourceType, TargetType>::PropertyBinding(QObject *parent)
    : QObject(parent),
      m_source(nullptr),
      m_target_widget(nullptr),
      m_direction(BindingDirection::OneWay),
      m_update_mode(UpdateMode::Immediate),
      m_enabled(true),
      m_valid(false),
      m_update_count(0),
      m_last_update_time(0) {}

/**
 * @brief Constructs a binding between a reactive property and a QWidget
 * property. Sets up signal connections based on the binding direction.
 * @param source Shared pointer to the reactive source property.
 * @param target_widget Pointer to the target QWidget.
 * @param target_property Name of the target property.
 * @param direction Binding direction (OneWay, TwoWay, OneTime).
 * @param parent Parent QObject.
 */
template <typename SourceType, typename TargetType>
PropertyBinding<SourceType, TargetType>::PropertyBinding(
    std::shared_ptr<ReactiveProperty<SourceType>> source,
    QWidget *target_widget, const QString &target_property,
    BindingDirection direction, QObject *parent)
    : QObject(parent),
      m_source(source),
      m_target_widget(target_widget),
      m_target_property(target_property),
      m_direction(direction),
      m_update_mode(UpdateMode::Immediate),
      m_enabled(true),
      m_valid(false),
      m_update_count(0),
      m_last_update_time(0) {
    if (m_source && m_target_widget) {
        // Set up source to target connection (not for OneTime bindings)
        if (m_direction != BindingDirection::OneTime) {
            m_source_connection = QObject::connect(
                m_source.get(), &ReactivePropertyBase::valueChanged, this,
                [this]() {
                    if (m_enabled && m_update_mode == UpdateMode::Immediate) {
                        updateTargetFromSource();
                    }
                });
        }

        // Set up target to source connection for two-way binding
        if (m_direction == BindingDirection::TwoWay) {
            // Connect to target widget's property change signal if available
            const QMetaObject *metaObject = m_target_widget->metaObject();
            int propIndex = metaObject->indexOfProperty(
                m_target_property.toLocal8Bit().data());
            if (propIndex != -1) {
                QMetaProperty metaProp = metaObject->property(propIndex);
                if (metaProp.hasNotifySignal()) {
                    QMetaMethod notifySignal = metaProp.notifySignal();
                    QMetaMethod updateSlot = this->metaObject()->method(
                        this->metaObject()->indexOfSlot(
                            "updateSourceFromTarget()"));
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

/**
 * @brief Constructs a binding with a custom converter function.
 *        Sets up signal connections and uses the converter for value
 * transformation.
 * @tparam ConverterFunc Type of the converter function.
 * @param source Shared pointer to the reactive source property.
 * @param target_widget Pointer to the target QWidget.
 * @param target_property Name of the target property.
 * @param converter Function to convert from SourceType to TargetType.
 * @param direction Binding direction.
 * @param parent Parent QObject.
 */
template <typename SourceType, typename TargetType>
template <typename ConverterFunc>
PropertyBinding<SourceType, TargetType>::PropertyBinding(
    std::shared_ptr<ReactiveProperty<SourceType>> source,
    QWidget *target_widget, const QString &target_property,
    ConverterFunc converter, BindingDirection direction, QObject *parent)
    : QObject(parent),
      m_source(source),
      m_target_widget(target_widget),
      m_target_property(target_property),
      m_direction(direction),
      m_update_mode(UpdateMode::Immediate),
      m_enabled(true),
      m_valid(false),
      m_update_count(0),
      m_last_update_time(0) {
    // Set converter before initialization
    m_converter = converter;

    // Now perform the same initialization as the base constructor
    if (m_source && m_target_widget) {
        // Set up source to target connection (not for OneTime bindings)
        if (m_direction != BindingDirection::OneTime) {
            m_source_connection = QObject::connect(
                m_source.get(), &ReactivePropertyBase::valueChanged, this,
                [this]() {
                    if (m_enabled && m_update_mode == UpdateMode::Immediate) {
                        updateTargetFromSource();
                    }
                });
        }

        // Set up target to source connection for two-way binding
        if (m_direction == BindingDirection::TwoWay) {
            // Connect to target widget's property change signal if available
            const QMetaObject *metaObject = m_target_widget->metaObject();
            int propIndex = metaObject->indexOfProperty(
                m_target_property.toLocal8Bit().data());
            if (propIndex != -1) {
                QMetaProperty metaProp = metaObject->property(propIndex);
                if (metaProp.hasNotifySignal()) {
                    QMetaMethod notifySignal = metaProp.notifySignal();
                    QMetaMethod updateSlot = this->metaObject()->method(
                        this->metaObject()->indexOfSlot(
                            "updateSourceFromTarget()"));
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

/**
 * @brief Constructs a binding using a compute function (no source property).
 *        The compute function is called to produce the target value.
 * @tparam ComputeFunc Type of the compute function.
 * @param compute_func Function that computes the target value.
 * @param target_widget Pointer to the target QWidget.
 * @param target_property Name of the target property.
 * @param update_mode Update mode (Immediate, Deferred, Manual).
 * @param parent Parent QObject.
 */
template <typename SourceType, typename TargetType>
template <typename ComputeFunc>
PropertyBinding<SourceType, TargetType>::PropertyBinding(
    ComputeFunc compute_func, QWidget *target_widget,
    const QString &target_property, UpdateMode update_mode, QObject *parent)
    : QObject(parent),
      m_source(nullptr),
      m_target_widget(target_widget),
      m_target_property(target_property),
      m_direction(BindingDirection::OneWay),
      m_update_mode(update_mode),
      m_enabled(true),
      m_valid(false),
      m_update_count(0),
      m_last_update_time(0) {
    if (m_target_widget) {
        // Create a converter that calls the compute function
        m_converter = [compute_func](const SourceType &) -> TargetType {
            return compute_func();
        };

        m_valid = true;

        // Initial update (always perform for compute functions to set initial
        // value)
        update();
    }
}

/**
 * @brief Destructor. Disconnects all signal connections.
 */
template <typename SourceType, typename TargetType>
PropertyBinding<SourceType, TargetType>::~PropertyBinding() {
    disconnect();
}

/**
 * @brief Updates the target property from the source or compute function.
 *        Performs conversion and validation if applicable.
 */
template <typename SourceType, typename TargetType>
void PropertyBinding<SourceType, TargetType>::update() {
    if (!m_valid || !m_enabled)
        return;

    updateTargetFromSource();
}

/**
 * @brief Disconnects the binding, removing all signal connections and marking
 * as invalid.
 */
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

/**
 * @brief Checks if the binding is still valid.
 * @return True if valid, false otherwise.
 */
template <typename SourceType, typename TargetType>
bool PropertyBinding<SourceType, TargetType>::isValid() const {
    return m_valid;
}

/**
 * @brief Gets the source property path as a string.
 * @return Source property path.
 */
template <typename SourceType, typename TargetType>
QString PropertyBinding<SourceType, TargetType>::getSourcePath() const {
    return generateSourcePath();
}

/**
 * @brief Gets the target property path as a string.
 * @return Target property path.
 */
template <typename SourceType, typename TargetType>
QString PropertyBinding<SourceType, TargetType>::getTargetPath() const {
    return generateTargetPath();
}

/**
 * @brief Gets the binding direction.
 * @return BindingDirection value.
 */
template <typename SourceType, typename TargetType>
BindingDirection PropertyBinding<SourceType, TargetType>::getDirection() const {
    return m_direction;
}

/**
 * @brief Sets the update mode for the binding.
 * @param mode Update mode (Immediate, Deferred, Manual).
 */
template <typename SourceType, typename TargetType>
void PropertyBinding<SourceType, TargetType>::setUpdateMode(UpdateMode mode) {
    m_update_mode = mode;
}

/**
 * @brief Gets the current update mode.
 * @return UpdateMode value.
 */
template <typename SourceType, typename TargetType>
UpdateMode PropertyBinding<SourceType, TargetType>::getUpdateMode() const {
    return m_update_mode;
}

/**
 * @brief Enables or disables the binding.
 * @param enabled True to enable, false to disable.
 */
template <typename SourceType, typename TargetType>
void PropertyBinding<SourceType, TargetType>::setEnabled(bool enabled) {
    m_enabled = enabled;
}

/**
 * @brief Checks if the binding is enabled.
 * @return True if enabled, false otherwise.
 */
template <typename SourceType, typename TargetType>
bool PropertyBinding<SourceType, TargetType>::isEnabled() const {
    return m_enabled;
}

/**
 * @brief Sets a validator function for the target value.
 * @param validator Function that returns true if the value is valid.
 */
template <typename SourceType, typename TargetType>
void PropertyBinding<SourceType, TargetType>::setValidator(
    std::function<bool(const TargetType &)> validator) {
    m_validator = validator;
}

/**
 * @brief Sets a converter function from SourceType to TargetType.
 * @param converter Function to convert source value to target value.
 */
template <typename SourceType, typename TargetType>
void PropertyBinding<SourceType, TargetType>::setConverter(
    std::function<TargetType(const SourceType &)> converter) {
    m_converter = converter;
}

/**
 * @brief Sets an error handler function for binding errors.
 * @param handler Function that receives error messages.
 */
template <typename SourceType, typename TargetType>
void PropertyBinding<SourceType, TargetType>::setErrorHandler(
    std::function<void(const QString &)> handler) {
    m_error_handler = handler;
}

/**
 * @brief Gets the number of successful updates performed by this binding.
 * @return Update count.
 */
template <typename SourceType, typename TargetType>
quint64 PropertyBinding<SourceType, TargetType>::getUpdateCount() const {
    return m_update_count;
}

/**
 * @brief Gets the timestamp of the last update (in milliseconds since epoch).
 * @return Last update time.
 */
template <typename SourceType, typename TargetType>
qint64 PropertyBinding<SourceType, TargetType>::getLastUpdateTime() const {
    return m_last_update_time;
}

/**
 * @brief Updates the target property from the source value, performing
 * conversion and validation. Handles errors and updates performance counters.
 */
template <typename SourceType, typename TargetType>
void PropertyBinding<SourceType, TargetType>::updateTargetFromSource() {
    // Check preconditions (note: m_source can be null for compute function
    // bindings)
    if (!m_target_widget || !m_valid || !m_enabled)
        return;
    try {
        TargetType target_value;

        if (m_source) {
            // Normal binding with source property
            SourceType source_value = m_source->get();
            target_value = convertSourceToTarget(source_value);
        } else if (m_converter) {
            // Compute function binding (no source, converter contains the
            // compute function) Use a dummy source value since the converter
            // ignores it for compute functions
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
        bool success =
            m_target_widget->setProperty(m_target_property.toLocal8Bit().data(),
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

/**
 * @brief Updates the source property from the target value (for two-way
 * binding). Performs conversion and error handling.
 */
template <typename SourceType, typename TargetType>
void PropertyBinding<SourceType, TargetType>::updateSourceFromTarget() {
    if (!m_source || !m_target_widget || !m_valid || !m_enabled)
        return;
    if (m_direction != BindingDirection::TwoWay)
        return;

    try {
        QVariant target_variant =
            m_target_widget->property(m_target_property.toLocal8Bit().data());

        if (target_variant.isValid()) {
            TargetType target_value = target_variant.value<TargetType>();
            SourceType source_value = convertTargetToSource(target_value);

            // Update the source (this will trigger valueChanged signal)
            m_source->set(source_value);
        }
    } catch (const std::exception &e) {
        handleError(
            QString("Exception during reverse update: %1").arg(e.what()));
    }
}

/**
 * @brief Converts a source value to a target value using the converter or
 * default conversion.
 * @param source_value The source value.
 * @return Converted target value.
 */
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

/**
 * @brief Converts a target value to a source value using default conversion.
 * @param target_value The target value.
 * @return Converted source value.
 */
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

/**
 * @brief Validates the target value using the validator function, if set.
 * @param value The target value.
 * @return True if valid, false otherwise.
 */
template <typename SourceType, typename TargetType>
bool PropertyBinding<SourceType, TargetType>::validateTargetValue(
    const TargetType &value) {
    if (m_validator) {
        return m_validator(value);
    }
    return true;  // No validator means always valid
}

/**
 * @brief Handles errors by invoking the error handler, if set.
 * @param error_message The error message.
 */
template <typename SourceType, typename TargetType>
void PropertyBinding<SourceType, TargetType>::handleError(
    const QString &error_message) {
    if (m_error_handler) {
        m_error_handler(error_message);
    }
    // Could also log to console or emit a signal
}

/**
 * @brief Generates a string representation of the source path.
 * @return Source path string.
 */
template <typename SourceType, typename TargetType>
QString PropertyBinding<SourceType, TargetType>::generateSourcePath() const {
    if (m_source) {
        return QString("ReactiveProperty@%1")
            .arg(reinterpret_cast<quintptr>(m_source.get()), 0, 16);
    }
    return "No Source";
}

/**
 * @brief Generates a string representation of the target path.
 * @return Target path string.
 */
template <typename SourceType, typename TargetType>
QString PropertyBinding<SourceType, TargetType>::generateTargetPath() const {
    if (m_target_widget) {
        return QString("%1::%2").arg(m_target_widget->metaObject()->className(),
                                     m_target_property);
    }
    return "No Target";
}

}  // namespace DeclarativeUI::Binding
