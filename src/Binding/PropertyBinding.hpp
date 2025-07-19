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

// **Binding manager for multiple bindings**
class PropertyBindingManager : public QObject {
    Q_OBJECT

public:
    explicit PropertyBindingManager(QObject *parent = nullptr);
    ~PropertyBindingManager() override;

    // **Binding management**
    void addBinding(std::shared_ptr<IPropertyBinding> binding);
    void removeBinding(std::shared_ptr<IPropertyBinding> binding);
    void removeAllBindings();

    // **Batch operations**
    void updateAllBindings();
    void enableAllBindings();
    void disableAllBindings();

    // **Query methods**
    int getBindingCount() const;
    std::vector<std::shared_ptr<IPropertyBinding>> getBindings() const;
    std::vector<std::shared_ptr<IPropertyBinding>> getBindingsForWidget(QWidget *widget) const;

    // **Performance monitoring**
    void enablePerformanceMonitoring(bool enabled);
    bool isPerformanceMonitoringEnabled() const;
    QString getPerformanceReport() const;

private:
    // **Private members**
    std::vector<std::shared_ptr<IPropertyBinding>> m_bindings;
    bool m_performance_monitoring_enabled;

    // **Performance tracking**
    std::unordered_map<IPropertyBinding*, quint64> m_update_counts;
    std::unordered_map<IPropertyBinding*, qint64> m_update_times;

    // **Private methods**
    void trackBinding(IPropertyBinding *binding);
    void untrackBinding(IPropertyBinding *binding);
};

// **Global binding manager**
PropertyBindingManager* getGlobalBindingManager();

}  // namespace DeclarativeUI::Binding