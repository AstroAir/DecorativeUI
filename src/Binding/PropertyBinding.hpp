#pragma once

#include <QMetaObject>
#include <QMetaProperty>
#include <QObject>
#include <QString>
#include <QVariant>
#include <QWidget>

#include <functional>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace DeclarativeUI::Binding {

// **Forward declarations**
template <typename T>
class ReactiveProperty;
class StateManager;

// **Binding expression concepts**
template <typename T>
concept BindableType =
    std::is_copy_constructible_v<T> && std::is_move_constructible_v<T> &&
    requires(T t) { QVariant::fromValue(t); };

template <typename T>
concept ComputeFunction =
    std::is_invocable_v<T> && BindableType<std::invoke_result_t<T>>;

// **Binding direction enumeration**
enum class BindingDirection {
    OneWay,  // Source -> Target only
    TwoWay,  // Source <-> Target bidirectional
    OneTime  // Single evaluation at binding time
};

// **Binding update mode**
enum class UpdateMode {
    Immediate,  // Update immediately when source changes
    Deferred,   // Batch updates for performance
    Manual      // Manual update only
};

// **Base binding interface**
class IPropertyBinding {
public:
    virtual ~IPropertyBinding() = default;

    virtual void update() = 0;
    virtual void disconnect() = 0;
    virtual bool isValid() const = 0;
    virtual QString getSourcePath() const = 0;
    virtual QString getTargetPath() const = 0;
    virtual BindingDirection getDirection() const = 0;
};

// **Property binding implementation**
template <BindableType SourceType, BindableType TargetType = SourceType>
class PropertyBinding : public QObject, public IPropertyBinding {
    Q_OBJECT

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

    // **Bind with computed expression**
    template <ComputeFunction ComputeFunc>
    PropertyBinding(ComputeFunc compute_function, QWidget *target_widget,
                    const QString &target_property,
                    const std::vector<QString> &dependencies = {},
                    QObject *parent = nullptr);

    ~PropertyBinding() override;

    // **Configuration**
    PropertyBinding &setUpdateMode(UpdateMode mode);
    PropertyBinding &setDebounceInterval(int milliseconds);
    PropertyBinding &setValidationFunction(
        std::function<bool(const TargetType &)> validator);
    PropertyBinding &setErrorHandler(
        std::function<void(const QString &)> handler);

    // **Binding control**
    void update() override;
    void disconnect() override;
    void reconnect();

    // **Status**
    [[nodiscard]] bool isValid() const override;
    [[nodiscard]] bool isConnected() const;
    [[nodiscard]] QString getSourcePath() const override;
    [[nodiscard]] QString getTargetPath() const override;
    [[nodiscard]] BindingDirection getDirection() const override;
    [[nodiscard]] UpdateMode getUpdateMode() const;

    // **Value access**
    [[nodiscard]] SourceType getSourceValue() const;
    [[nodiscard]] TargetType getTargetValue() const;

    // **Manual value setting (for two-way bindings)**
    void setSourceValue(const SourceType &value);
    void setTargetValue(const TargetType &value);

signals:
    void bindingUpdated(const QString &property_path);
    void bindingError(const QString &error_message);
    void sourceValueChanged(const SourceType &new_value);
    void targetValueChanged(const TargetType &new_value);

private slots:
    void onSourceChanged(const SourceType &new_value);
    void onTargetChanged();
    void onDebouncedUpdate();

private:
    // **Source and target references**
    std::shared_ptr<ReactiveProperty<SourceType>> source_property_;
    QWidget *target_widget_;
    QString target_property_name_;

    // **Computed binding support**
    std::function<SourceType()> compute_function_;
    std::vector<QString> dependencies_;

    // **Conversion functions**
    std::function<TargetType(const SourceType &)> source_to_target_converter_;
    std::function<SourceType(const TargetType &)> target_to_source_converter_;

    // **Configuration**
    BindingDirection direction_ = BindingDirection::OneWay;
    UpdateMode update_mode_ = UpdateMode::Immediate;
    int debounce_interval_ = 0;

    // **Validation and error handling**
    std::function<bool(const TargetType &)> validator_;
    std::function<void(const QString &)> error_handler_;

    // **State**
    bool connected_ = false;
    bool updating_ = false;
    std::unique_ptr<QTimer> debounce_timer_;
    QMetaObject::Connection target_connection_;

    // **Internal helpers**
    void setupConnections();
    void teardownConnections();
    void updateTargetFromSource();
    void updateSourceFromTarget();
    TargetType convertSourceToTarget(const SourceType &source_value);
    SourceType convertTargetToSource(const TargetType &target_value);
    bool validateTargetValue(const TargetType &value);
    void handleError(const QString &error_message);
    QString generateSourcePath() const;
    QString generateTargetPath() const;
};

// **Binding manager for multiple bindings**
class PropertyBindingManager : public QObject {
    Q_OBJECT

public:
    explicit PropertyBindingManager(QObject *parent = nullptr);
    ~PropertyBindingManager() override;

    // **Binding creation**
    template <BindableType SourceType, BindableType TargetType = SourceType>
    std::shared_ptr<PropertyBinding<SourceType, TargetType>> createBinding(
        std::shared_ptr<ReactiveProperty<SourceType>> source,
        QWidget *target_widget, const QString &target_property,
        BindingDirection direction = BindingDirection::OneWay);

    template <BindableType SourceType, BindableType TargetType,
              typename ConverterFunc>
    std::shared_ptr<PropertyBinding<SourceType, TargetType>> createBinding(
        std::shared_ptr<ReactiveProperty<SourceType>> source,
        QWidget *target_widget, const QString &target_property,
        ConverterFunc converter,
        BindingDirection direction = BindingDirection::OneWay);

    // **Binding management**
    void addBinding(std::shared_ptr<IPropertyBinding> binding);
    void removeBinding(const QString &binding_id);
    void removeAllBindings();

    // **Batch operations**
    void updateAllBindings();
    void setGlobalUpdateMode(UpdateMode mode);
    void setGlobalDebounceInterval(int milliseconds);

    // **Status**
    [[nodiscard]] size_t getBindingCount() const;
    [[nodiscard]] std::vector<QString> getBindingIds() const;
    [[nodiscard]] std::shared_ptr<IPropertyBinding> getBinding(
        const QString &binding_id) const;

signals:
    void bindingAdded(const QString &binding_id);
    void bindingRemoved(const QString &binding_id);
    void batchUpdateStarted();
    void batchUpdateCompleted();

private:
    std::unordered_map<QString, std::shared_ptr<IPropertyBinding>> bindings_;
    UpdateMode global_update_mode_ = UpdateMode::Immediate;
    int global_debounce_interval_ = 0;
    bool batch_updating_ = false;

    QString generateBindingId() const;
};

// **Utility functions for common binding scenarios**
namespace BindingUtils {

// **Create simple one-way binding**
template <BindableType T>
std::shared_ptr<PropertyBinding<T>> bind(
    std::shared_ptr<ReactiveProperty<T>> source, QWidget *target,
    const QString &property);

// **Create two-way binding**
template <BindableType T>
std::shared_ptr<PropertyBinding<T>> bindTwoWay(
    std::shared_ptr<ReactiveProperty<T>> source, QWidget *target,
    const QString &property);

// **Create computed binding**
template <ComputeFunction ComputeFunc>
auto bindComputed(ComputeFunc compute_function, QWidget *target,
                  const QString &property,
                  const std::vector<QString> &dependencies = {});

// **Create binding with validation**
template <BindableType T>
std::shared_ptr<PropertyBinding<T>> bindWithValidation(
    std::shared_ptr<ReactiveProperty<T>> source, QWidget *target,
    const QString &property, std::function<bool(const T &)> validator);

}  // namespace BindingUtils

}  // namespace DeclarativeUI::Binding
