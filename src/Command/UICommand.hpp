#pragma once

#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QUuid>
#include <QVariant>
#include <QWidget>
#include <functional>
#include <memory>
#include <type_traits>
#include <unordered_map>

#include "../Binding/StateManager.hpp"
#include "../Core/UIElement.hpp"

namespace DeclarativeUI::Command::UI {

// **Forward declarations**
class BaseUICommand;
class WidgetMapper;
class UICommandFactory;

// **UI Command metadata for type information and widget mapping**
struct UICommandMetadata {
    QString command_type;
    QString widget_type;
    QString display_name;
    QString description;
    QJsonObject default_properties;
    QStringList supported_events;
    QStringList required_properties;

    UICommandMetadata() = default;
    UICommandMetadata(const QString& cmd_type, const QString& widget_type,
                      const QString& name = "", const QString& desc = "")
        : command_type(cmd_type),
          widget_type(widget_type),
          display_name(name),
          description(desc) {}
};

// **UI Command state management - completely decoupled from QWidget**
class UICommandState : public QObject {
    Q_OBJECT

public:
    explicit UICommandState(QObject* parent = nullptr);
    virtual ~UICommandState() = default;

    // **Property management**
    template <typename T>
    void setProperty(const QString& name, const T& value);

    template <typename T>
    T getProperty(const QString& name, const T& defaultValue = T{}) const;

    bool hasProperty(const QString& name) const;
    void removeProperty(const QString& name);
    QStringList getPropertyNames() const;

    // **State validation**
    void setValidator(const QString& property,
                      std::function<bool(const QVariant&)> validator);
    bool validateProperty(const QString& property, const QVariant& value) const;
    bool validateAllProperties() const;

    // **State serialization**
    QJsonObject toJson() const;
    void fromJson(const QJsonObject& json);

    // **State comparison**
    bool equals(const UICommandState& other) const;
    QStringList getDifferences(const UICommandState& other) const;
    QStringList diff(const UICommandState& other) const {
        return getDifferences(other);
    }  // Alias for convenience

    // **Property management**
    void clearProperties();

signals:
    void propertyChanged(const QString& name, const QVariant& value);
    void propertyRemoved(const QString& property);
    void stateChanged();
    void validationFailed(const QString& property, const QString& reason);

private:
    std::unordered_map<QString, QVariant> properties_;
    std::unordered_map<QString, std::function<bool(const QVariant&)>>
        validators_;
};

// **Base UI Command - pure abstraction without QWidget inheritance**
class BaseUICommand : public QObject {
    Q_OBJECT

public:
    explicit BaseUICommand(QObject* parent = nullptr);
    virtual ~BaseUICommand() = default;

    // **Core Command interface**
    virtual UICommandMetadata getMetadata() const = 0;
    virtual QString getCommandType() const = 0;
    virtual QString getWidgetType() const = 0;

    // **State management**
    UICommandState* getState() const { return state_.get(); }

    // **Widget lifecycle - managed by WidgetMapper**
    virtual void onWidgetCreated(QWidget* widget);
    virtual void onWidgetDestroyed();
    virtual void syncToWidget();
    virtual void syncFromWidget();

    // **Event handling abstraction**
    virtual void handleEvent(const QString& eventType,
                             const QVariant& eventData = QVariant{});

    // **Property binding support**
    template <typename T>
    void bindProperty(const QString& property, std::function<T()> binding);

    void bindToState(const QString& stateKey, const QString& property = "");
    void unbindFromState(const QString& property = "");

    // **Command hierarchy support**
    void addChild(std::shared_ptr<BaseUICommand> child);
    void removeChild(std::shared_ptr<BaseUICommand> child);
    std::vector<std::shared_ptr<BaseUICommand>> getChildren() const;
    BaseUICommand* getParent() const;

    // **Widget access (managed by WidgetMapper)**
    QWidget* getWidget() const { return widget_; }
    bool hasWidget() const { return widget_ != nullptr; }

    // **Unique identification**
    QUuid getId() const { return id_; }

signals:
    void stateChanged();
    void eventTriggered(const QString& eventType, const QVariant& eventData);
    void propertyChanged(const QString& property, const QVariant& value);
    void childAdded(std::shared_ptr<BaseUICommand> child);
    void childRemoved(std::shared_ptr<BaseUICommand> child);

protected:
    // **Internal state and widget management**
    std::unique_ptr<UICommandState> state_;
    QWidget* widget_ = nullptr;  // Managed by WidgetMapper
    QUuid id_;

    // **Hierarchy management**
    std::vector<std::shared_ptr<BaseUICommand>> children_;
    BaseUICommand* parent_ = nullptr;

    // **Property bindings**
    std::unordered_map<QString, std::function<QVariant()>> property_bindings_;
    std::unordered_map<QString, QString>
        state_bindings_;  // property -> state key

    // **Event handlers**
    std::unordered_map<QString, std::function<void(const QVariant&)>>
        event_handlers_;

    // **Helper methods**
    void updateBoundProperties();
    void connectToStateManager();
    void disconnectFromStateManager();

    friend class WidgetMapper;
    friend class UICommandFactory;

private:
    void setWidget(QWidget* widget);  // Only WidgetMapper can set widget
    void setParent(BaseUICommand* parent);
};

// **Template implementations**
template <typename T>
void UICommandState::setProperty(const QString& name, const T& value) {
    QVariant variant;

    if constexpr (std::is_same_v<std::decay_t<T>, const char*> ||
                  std::is_same_v<std::decay_t<T>, char*>) {
        variant = QVariant(QString(value));
    } else if constexpr (std::is_array_v<T> &&
                         std::is_same_v<std::remove_extent_t<T>, char>) {
        variant = QVariant(QString(value));
    } else {
        variant = QVariant::fromValue(value);
    }

    if (validateProperty(name, variant)) {
        properties_[name] = variant;
        emit propertyChanged(name, variant);
        emit stateChanged();
    } else {
        emit validationFailed(name, "Validation failed for property value");
    }
}

template <typename T>
T UICommandState::getProperty(const QString& name,
                              const T& defaultValue) const {
    auto it = properties_.find(name);
    if (it != properties_.end()) {
        return it->second.value<T>();
    }
    return defaultValue;
}

template <typename T>
void BaseUICommand::bindProperty(const QString& property,
                                 std::function<T()> binding) {
    property_bindings_[property] = [binding]() -> QVariant {
        return QVariant::fromValue(binding());
    };

    // Initial update
    updateBoundProperties();
}

}  // namespace DeclarativeUI::Command::UI

Q_DECLARE_METATYPE(DeclarativeUI::Command::UI::UICommandMetadata)
