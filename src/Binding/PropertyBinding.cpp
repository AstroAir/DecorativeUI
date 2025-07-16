// Binding/PropertyBinding.cpp
#include "PropertyBinding.hpp"
#include "../Exceptions/UIExceptions.hpp"
#include "StateManager.hpp"


#include <QDebug>
#include <QMetaObject>
#include <QMetaProperty>
#include <QTimer>
#include <QUuid>
#include <typeinfo>


namespace DeclarativeUI::Binding {

// **PropertyBinding Implementation**

template <BindableType SourceType, BindableType TargetType>
PropertyBinding<SourceType, TargetType>::PropertyBinding(QObject* parent)
    : QObject(parent),
      target_widget_(nullptr),
      direction_(BindingDirection::OneWay),
      update_mode_(UpdateMode::Immediate),
      debounce_interval_(0),
      connected_(false),
      updating_(false) {}

template <BindableType SourceType, BindableType TargetType>
PropertyBinding<SourceType, TargetType>::PropertyBinding(
    std::shared_ptr<ReactiveProperty<SourceType>> source,
    QWidget* target_widget, const QString& target_property,
    BindingDirection direction, QObject* parent)
    : QObject(parent),
      source_property_(std::move(source)),
      target_widget_(target_widget),
      target_property_name_(target_property),
      direction_(direction),
      update_mode_(UpdateMode::Immediate),
      debounce_interval_(0),
      connected_(false),
      updating_(false) {
    if (!source_property_) {
        throw DeclarativeUI::Exceptions::PropertyBindingException(
            "Source property cannot be null");
    }

    if (!target_widget_) {
        throw DeclarativeUI::Exceptions::PropertyBindingException(
            "Target widget cannot be null");
    }

    if (target_property.isEmpty()) {
        throw DeclarativeUI::Exceptions::PropertyBindingException(
            "Target property name cannot be empty");
    }

    // **Setup default converters for same type**
    if constexpr (std::is_same_v<SourceType, TargetType>) {
        source_to_target_converter_ = [](const SourceType& value) {
            return value;
        };
        target_to_source_converter_ = [](const TargetType& value) {
            return value;
        };
    }

    setupConnections();
}

template <BindableType SourceType, BindableType TargetType>
template <typename ConverterFunc>
PropertyBinding<SourceType, TargetType>::PropertyBinding(
    std::shared_ptr<ReactiveProperty<SourceType>> source,
    QWidget* target_widget, const QString& target_property,
    ConverterFunc converter, BindingDirection direction, QObject* parent)
    : PropertyBinding(std::move(source), target_widget, target_property,
                      direction, parent) {
    source_to_target_converter_ = std::move(converter);

    // **For two-way bindings, we need a reverse converter**
    if (direction == BindingDirection::TwoWay) {
        static_assert(std::is_same_v<SourceType, TargetType>,
                      "Two-way bindings with converters require same source "
                      "and target types");
    }
}

template <BindableType SourceType, BindableType TargetType>
template <ComputeFunction ComputeFunc>
PropertyBinding<SourceType, TargetType>::PropertyBinding(
    ComputeFunc compute_function, QWidget* target_widget,
    const QString& target_property, const std::vector<QString>& dependencies,
    QObject* parent)
    : QObject(parent),
      target_widget_(target_widget),
      target_property_name_(target_property),
      compute_function_(std::move(compute_function)),
      dependencies_(dependencies),
      direction_(BindingDirection::OneWay),
      update_mode_(UpdateMode::Immediate),
      debounce_interval_(0),
      connected_(false),
      updating_(false) {
    if (!target_widget_) {
        throw DeclarativeUI::Exceptions::PropertyBindingException(
            "Target widget cannot be null");
    }

    if (target_property.isEmpty()) {
        throw DeclarativeUI::Exceptions::PropertyBindingException(
            "Target property name cannot be empty");
    }

    if (!compute_function_) {
        throw DeclarativeUI::Exceptions::PropertyBindingException(
            "Compute function cannot be null");
    }

    // **Setup converter for computed bindings**
    source_to_target_converter_ = [](const SourceType& value) {
        if constexpr (std::is_same_v<SourceType, TargetType>) {
            return value;
        } else {
            return static_cast<TargetType>(value);
        }
    };

    setupConnections();
}

template <BindableType SourceType, BindableType TargetType>
PropertyBinding<SourceType, TargetType>::~PropertyBinding() {
    disconnect();
}

template <BindableType SourceType, BindableType TargetType>
PropertyBinding<SourceType, TargetType>&
PropertyBinding<SourceType, TargetType>::setUpdateMode(UpdateMode mode) {
    update_mode_ = mode;

    if (mode == UpdateMode::Deferred && !debounce_timer_) {
        debounce_timer_ = std::make_unique<QTimer>(this);
        debounce_timer_->setSingleShot(true);
        debounce_timer_->setInterval(debounce_interval_ > 0 ? debounce_interval_
                                                            : 16);  // ~60 FPS

        connect(debounce_timer_.get(), &QTimer::timeout, this,
                &PropertyBinding::onDebouncedUpdate);
    }

    return *this;
}

template <BindableType SourceType, BindableType TargetType>
PropertyBinding<SourceType, TargetType>&
PropertyBinding<SourceType, TargetType>::setDebounceInterval(int milliseconds) {
    debounce_interval_ = std::max(0, milliseconds);

    if (debounce_timer_) {
        debounce_timer_->setInterval(debounce_interval_ > 0 ? debounce_interval_
                                                            : 16);
    }

    return *this;
}

template <BindableType SourceType, BindableType TargetType>
PropertyBinding<SourceType, TargetType>&
PropertyBinding<SourceType, TargetType>::setValidationFunction(
    std::function<bool(const TargetType&)> validator) {
    validator_ = std::move(validator);
    return *this;
}

template <BindableType SourceType, BindableType TargetType>
PropertyBinding<SourceType, TargetType>&
PropertyBinding<SourceType, TargetType>::setErrorHandler(
    std::function<void(const QString&)> handler) {
    error_handler_ = std::move(handler);
    return *this;
}

template <BindableType SourceType, BindableType TargetType>
void PropertyBinding<SourceType, TargetType>::update() {
    if (!isValid() || updating_)
        return;

    try {
        updating_ = true;

        if (update_mode_ == UpdateMode::Deferred && debounce_timer_) {
            debounce_timer_->start();
        } else {
            updateTargetFromSource();
        }

        updating_ = false;

    } catch (const std::exception& e) {
        updating_ = false;
        handleError(QString("Binding update failed: %1").arg(e.what()));
    }
}

template <BindableType SourceType, BindableType TargetType>
void PropertyBinding<SourceType, TargetType>::disconnect() {
    if (!connected_)
        return;

    teardownConnections();
    connected_ = false;

    emit bindingUpdated(getTargetPath());
}

template <BindableType SourceType, BindableType TargetType>
void PropertyBinding<SourceType, TargetType>::reconnect() {
    if (connected_) {
        disconnect();
    }

    setupConnections();
}

template <BindableType SourceType, BindableType TargetType>
bool PropertyBinding<SourceType, TargetType>::isValid() const {
    if (source_property_) {
        return target_widget_ && !target_property_name_.isEmpty();
    } else if (compute_function_) {
        return target_widget_ && !target_property_name_.isEmpty();
    }
    return false;
}

template <BindableType SourceType, BindableType TargetType>
bool PropertyBinding<SourceType, TargetType>::isConnected() const {
    return connected_;
}

template <BindableType SourceType, BindableType TargetType>
QString PropertyBinding<SourceType, TargetType>::getSourcePath() const {
    return generateSourcePath();
}

template <BindableType SourceType, BindableType TargetType>
QString PropertyBinding<SourceType, TargetType>::getTargetPath() const {
    return generateTargetPath();
}

template <BindableType SourceType, BindableType TargetType>
BindingDirection PropertyBinding<SourceType, TargetType>::getDirection() const {
    return direction_;
}

template <BindableType SourceType, BindableType TargetType>
UpdateMode PropertyBinding<SourceType, TargetType>::getUpdateMode() const {
    return update_mode_;
}

template <BindableType SourceType, BindableType TargetType>
SourceType PropertyBinding<SourceType, TargetType>::getSourceValue() const {
    if (source_property_) {
        return source_property_->get();
    } else if (compute_function_) {
        return compute_function_();
    }
    return SourceType{};
}

template <BindableType SourceType, BindableType TargetType>
TargetType PropertyBinding<SourceType, TargetType>::getTargetValue() const {
    if (!target_widget_)
        return TargetType{};

    QVariant variant =
        target_widget_->property(target_property_name_.toUtf8().constData());
    return variant.value<TargetType>();
}

template <BindableType SourceType, BindableType TargetType>
void PropertyBinding<SourceType, TargetType>::setSourceValue(
    const SourceType& value) {
    if (source_property_ && direction_ != BindingDirection::OneWay) {
        source_property_->set(value);
    }
}

template <BindableType SourceType, BindableType TargetType>
void PropertyBinding<SourceType, TargetType>::setTargetValue(
    const TargetType& value) {
    if (!target_widget_ || updating_)
        return;

    try {
        if (validateTargetValue(value)) {
            target_widget_->setProperty(
                target_property_name_.toUtf8().constData(),
                QVariant::fromValue(value));
            emit targetValueChanged(value);
        }
    } catch (const std::exception& e) {
        handleError(QString("Failed to set target value: %1").arg(e.what()));
    }
}

template <BindableType SourceType, BindableType TargetType>
void PropertyBinding<SourceType, TargetType>::onSourceChanged(
    const SourceType& new_value) {
    if (updating_)
        return;

    emit sourceValueChanged(new_value);

    if (update_mode_ != UpdateMode::Manual) {
        update();
    }
}

template <BindableType SourceType, BindableType TargetType>
void PropertyBinding<SourceType, TargetType>::onTargetChanged() {
    if (updating_ || direction_ == BindingDirection::OneWay)
        return;

    try {
        updating_ = true;
        updateSourceFromTarget();
        updating_ = false;

    } catch (const std::exception& e) {
        updating_ = false;
        handleError(
            QString("Target to source update failed: %1").arg(e.what()));
    }
}

template <BindableType SourceType, BindableType TargetType>
void PropertyBinding<SourceType, TargetType>::onDebouncedUpdate() {
    updateTargetFromSource();
}

template <BindableType SourceType, BindableType TargetType>
void PropertyBinding<SourceType, TargetType>::setupConnections() {
    if (connected_)
        return;

    try {
        // **Connect to source property changes**
        if (source_property_) {
            connect(source_property_.get(),
                    &ReactiveProperty<SourceType>::valueChanged, this,
                    &PropertyBinding::onSourceChanged);
        }

        // **Connect to target property changes for two-way bindings**
        if (direction_ == BindingDirection::TwoWay && target_widget_) {
            const QMetaObject* meta_obj = target_widget_->metaObject();
            QString notify_signal = target_property_name_ + "Changed";

            // **Find property change signal**
            for (int i = 0; i < meta_obj->methodCount(); ++i) {
                QMetaMethod method = meta_obj->method(i);

                if (method.methodType() == QMetaMethod::Signal &&
                    QString::fromUtf8(method.name()) == notify_signal) {
                    target_connection_ = QObject::connect(
                        target_widget_, method.methodSignature(), this,
                        SLOT(onTargetChanged()));
                    break;
                }
            }
        }

        connected_ = true;

        // **Initial update**
        if (direction_ != BindingDirection::OneTime) {
            update();
        }

    } catch (const std::exception& e) {
        handleError(QString("Connection setup failed: %1").arg(e.what()));
        throw DeclarativeUI::Exceptions::PropertyBindingException(
            "Failed to setup binding connections: " + std::string(e.what()));
    }
}

template <BindableType SourceType, BindableType TargetType>
void PropertyBinding<SourceType, TargetType>::teardownConnections() {
    if (source_property_) {
        QObject::disconnect(source_property_.get(), nullptr, this, nullptr);
    }

    if (target_connection_) {
        QObject::disconnect(target_connection_);
        target_connection_ = QMetaObject::Connection{};
    }

    if (debounce_timer_) {
        debounce_timer_->stop();
    }
}

template <BindableType SourceType, BindableType TargetType>
void PropertyBinding<SourceType, TargetType>::updateTargetFromSource() {
    if (!target_widget_ || updating_)
        return;

    try {
        SourceType source_value = getSourceValue();
        TargetType target_value = convertSourceToTarget(source_value);

        if (validateTargetValue(target_value)) {
            setTargetValue(target_value);
            emit bindingUpdated(getTargetPath());
        }

    } catch (const std::exception& e) {
        handleError(
            QString("Source to target update failed: %1").arg(e.what()));
    }
}

template <BindableType SourceType, BindableType TargetType>
void PropertyBinding<SourceType, TargetType>::updateSourceFromTarget() {
    if (!source_property_ || direction_ == BindingDirection::OneWay)
        return;

    try {
        TargetType target_value = getTargetValue();
        SourceType source_value = convertTargetToSource(target_value);

        source_property_->set(source_value);

    } catch (const std::exception& e) {
        handleError(
            QString("Target to source update failed: %1").arg(e.what()));
    }
}

template <BindableType SourceType, BindableType TargetType>
TargetType PropertyBinding<SourceType, TargetType>::convertSourceToTarget(
    const SourceType& source_value) {
    if (source_to_target_converter_) {
        return source_to_target_converter_(source_value);
    }

    // **Default conversion**
    if constexpr (std::is_same_v<SourceType, TargetType>) {
        return source_value;
    } else {
        return static_cast<TargetType>(source_value);
    }
}

template <BindableType SourceType, BindableType TargetType>
SourceType PropertyBinding<SourceType, TargetType>::convertTargetToSource(
    const TargetType& target_value) {
    if (target_to_source_converter_) {
        return target_to_source_converter_(target_value);
    }

    // **Default conversion**
    if constexpr (std::is_same_v<SourceType, TargetType>) {
        return target_value;
    } else {
        return static_cast<SourceType>(target_value);
    }
}

template <BindableType SourceType, BindableType TargetType>
bool PropertyBinding<SourceType, TargetType>::validateTargetValue(
    const TargetType& value) {
    if (validator_) {
        return validator_(value);
    }
    return true;
}

template <BindableType SourceType, BindableType TargetType>
void PropertyBinding<SourceType, TargetType>::handleError(
    const QString& error_message) {
    qWarning() << "PropertyBinding error:" << error_message;

    if (error_handler_) {
        error_handler_(error_message);
    }

    emit bindingError(error_message);
}

template <BindableType SourceType, BindableType TargetType>
QString PropertyBinding<SourceType, TargetType>::generateSourcePath() const {
    if (source_property_) {
        return QString("ReactiveProperty<%1>").arg(typeid(SourceType).name());
    } else if (compute_function_) {
        return QString("ComputedProperty<%1>").arg(typeid(SourceType).name());
    }
    return "Unknown";
}

template <BindableType SourceType, BindableType TargetType>
QString PropertyBinding<SourceType, TargetType>::generateTargetPath() const {
    if (target_widget_) {
        return QString("%1.%2").arg(target_widget_->objectName(),
                                    target_property_name_);
    }
    return "Unknown";
}

// **PropertyBindingManager Implementation**

PropertyBindingManager::PropertyBindingManager(QObject* parent)
    : QObject(parent), batch_updating_(false) {}

PropertyBindingManager::~PropertyBindingManager() { removeAllBindings(); }

template <BindableType SourceType, BindableType TargetType>
std::shared_ptr<PropertyBinding<SourceType, TargetType>>
PropertyBindingManager::createBinding(
    std::shared_ptr<ReactiveProperty<SourceType>> source,
    QWidget* target_widget, const QString& target_property,
    BindingDirection direction) {
    auto binding = std::make_shared<PropertyBinding<SourceType, TargetType>>(
        std::move(source), target_widget, target_property, direction, this);

    binding->setUpdateMode(global_update_mode_);
    binding->setDebounceInterval(global_debounce_interval_);

    QString binding_id = generateBindingId();
    bindings_[binding_id] = binding;

    emit bindingAdded(binding_id);

    return binding;
}

template <BindableType SourceType, BindableType TargetType,
          typename ConverterFunc>
std::shared_ptr<PropertyBinding<SourceType, TargetType>>
PropertyBindingManager::createBinding(
    std::shared_ptr<ReactiveProperty<SourceType>> source,
    QWidget* target_widget, const QString& target_property,
    ConverterFunc converter, BindingDirection direction) {
    auto binding = std::make_shared<PropertyBinding<SourceType, TargetType>>(
        std::move(source), target_widget, target_property, std::move(converter),
        direction, this);

    binding->setUpdateMode(global_update_mode_);
    binding->setDebounceInterval(global_debounce_interval_);

    QString binding_id = generateBindingId();
    bindings_[binding_id] = binding;

    emit bindingAdded(binding_id);

    return binding;
}

void PropertyBindingManager::addBinding(
    std::shared_ptr<IPropertyBinding> binding) {
    if (!binding) {
        throw std::invalid_argument("Binding cannot be null");
    }

    QString binding_id = generateBindingId();
    bindings_[binding_id] = std::move(binding);

    emit bindingAdded(binding_id);
}

void PropertyBindingManager::removeBinding(const QString& binding_id) {
    auto it = bindings_.find(binding_id);
    if (it != bindings_.end()) {
        it->second->disconnect();
        bindings_.erase(it);
        emit bindingRemoved(binding_id);
    }
}

void PropertyBindingManager::removeAllBindings() {
    for (auto& [id, binding] : bindings_) {
        binding->disconnect();
        emit bindingRemoved(id);
    }
    bindings_.clear();
}

void PropertyBindingManager::updateAllBindings() {
    if (batch_updating_)
        return;

    batch_updating_ = true;
    emit batchUpdateStarted();

    try {
        for (auto& [id, binding] : bindings_) {
            binding->update();
        }

        batch_updating_ = false;
        emit batchUpdateCompleted();

    } catch (const std::exception& e) {
        batch_updating_ = false;
        qWarning() << "Batch update failed:" << e.what();
        emit batchUpdateCompleted();
        throw;
    }
}

void PropertyBindingManager::setGlobalUpdateMode(UpdateMode mode) {
    global_update_mode_ = mode;
}

void PropertyBindingManager::setGlobalDebounceInterval(int milliseconds) {
    global_debounce_interval_ = std::max(0, milliseconds);
}

size_t PropertyBindingManager::getBindingCount() const {
    return bindings_.size();
}

std::vector<QString> PropertyBindingManager::getBindingIds() const {
    std::vector<QString> ids;
    ids.reserve(bindings_.size());

    for (const auto& [id, binding] : bindings_) {
        ids.push_back(id);
    }

    return ids;
}

std::shared_ptr<IPropertyBinding> PropertyBindingManager::getBinding(
    const QString& binding_id) const {
    auto it = bindings_.find(binding_id);
    return it != bindings_.end() ? it->second : nullptr;
}

QString PropertyBindingManager::generateBindingId() const {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

// **Utility Functions Implementation**

namespace BindingUtils {

template <BindableType T>
std::shared_ptr<PropertyBinding<T>> bind(
    std::shared_ptr<ReactiveProperty<T>> source, QWidget* target,
    const QString& property) {
    return std::make_shared<PropertyBinding<T>>(
        std::move(source), target, property, BindingDirection::OneWay);
}

template <BindableType T>
std::shared_ptr<PropertyBinding<T>> bindTwoWay(
    std::shared_ptr<ReactiveProperty<T>> source, QWidget* target,
    const QString& property) {
    return std::make_shared<PropertyBinding<T>>(
        std::move(source), target, property, BindingDirection::TwoWay);
}

template <ComputeFunction ComputeFunc>
auto bindComputed(ComputeFunc compute_function, QWidget* target,
                  const QString& property,
                  const std::vector<QString>& dependencies) {
    using ReturnType = std::invoke_result_t<ComputeFunc>;

    return std::make_shared<PropertyBinding<ReturnType>>(
        std::move(compute_function), target, property, dependencies);
}

template <BindableType T>
std::shared_ptr<PropertyBinding<T>> bindWithValidation(
    std::shared_ptr<ReactiveProperty<T>> source, QWidget* target,
    const QString& property, std::function<bool(const T&)> validator) {
    auto binding = std::make_shared<PropertyBinding<T>>(
        std::move(source), target, property, BindingDirection::OneWay);

    binding->setValidationFunction(std::move(validator));

    return binding;
}

}  // namespace BindingUtils

// **Explicit template instantiations for common types**
template class PropertyBinding<int>;
template class PropertyBinding<double>;
template class PropertyBinding<bool>;
template class PropertyBinding<QString>;
template class PropertyBinding<QColor>;
template class PropertyBinding<QFont>;
template class PropertyBinding<QSize>;
template class PropertyBinding<QRect>;

}  // namespace DeclarativeUI::Binding
