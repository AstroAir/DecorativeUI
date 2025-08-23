#pragma once

#include <QMetaObject>
#include <QMetaProperty>
#include <QObject>
#include <QString>
#include <QWidget>
#include <functional>
#include <memory>
#include <type_traits>
#include <unordered_map>

#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include "UICommand.hpp"

namespace DeclarativeUI::Command::UI {

// **Widget factory function type**
using WidgetFactory = std::function<std::unique_ptr<QWidget>()>;

// **Property synchronization configuration**
struct PropertySyncConfig {
    QString command_property;
    QString widget_property;
    bool bidirectional = true;
    std::function<QVariant(const QVariant&)> command_to_widget_converter;
    std::function<QVariant(const QVariant&)> widget_to_command_converter;

    PropertySyncConfig() = default;
    PropertySyncConfig(const QString& cmd_prop, const QString& widget_prop,
                       bool bidir = true)
        : command_property(cmd_prop),
          widget_property(widget_prop),
          bidirectional(bidir) {}
};

// **Event mapping configuration**
struct EventMappingConfig {
    QString widget_signal;
    QString command_event;
    std::function<QVariant(const QVariantList&)> signal_to_event_converter;

    EventMappingConfig() = default;
    EventMappingConfig(const QString& signal, const QString& event)
        : widget_signal(signal), command_event(event) {}
};

// **Widget mapping configuration for a command type**
struct WidgetMappingConfig {
    QString widget_type;
    WidgetFactory factory;
    std::vector<PropertySyncConfig> property_mappings;
    std::vector<EventMappingConfig> event_mappings;
    std::function<void(QWidget*, BaseUICommand*)> custom_setup;

    WidgetMappingConfig() = default;
    WidgetMappingConfig(const QString& type, WidgetFactory fact)
        : widget_type(type), factory(std::move(fact)) {}
};

// **Widget Mapper - handles Command to QWidget translation and
// synchronization**
class WidgetMapper : public QObject {
    Q_OBJECT

public:
    explicit WidgetMapper(QObject* parent = nullptr);
    virtual ~WidgetMapper() = default;

    // **Widget mapping registration**
    template <typename WidgetType>
    void registerMapping(const QString& command_type);

    void registerMapping(const QString& command_type,
                         const WidgetMappingConfig& config);

    // **Widget creation and management**
    std::unique_ptr<QWidget> createWidget(BaseUICommand* command);
    void destroyWidget(BaseUICommand* command);

    // **Synchronization management**
    void establishBinding(BaseUICommand* command, QWidget* widget);
    void removeBinding(BaseUICommand* command);

    // **Property synchronization**
    void syncCommandToWidget(BaseUICommand* command,
                             const QString& property = "");
    void syncWidgetToCommand(QWidget* widget, BaseUICommand* command,
                             const QString& property = "");

    // **Event forwarding**
    void connectEvents(BaseUICommand* command, QWidget* widget);
    void disconnectEvents(BaseUICommand* command, QWidget* widget);

    // **Configuration management**
    void setPropertyMapping(const QString& command_type,
                            const PropertySyncConfig& config);
    void setEventMapping(const QString& command_type,
                         const EventMappingConfig& config);
    void setCustomSetup(const QString& command_type,
                        std::function<void(QWidget*, BaseUICommand*)> setup);

    // **Query methods**
    bool hasMapping(const QString& command_type) const;
    QString getWidgetType(const QString& command_type) const;
    QStringList getSupportedCommandTypes() const;

    // **Singleton access**
    static WidgetMapper& instance();

signals:
    void widgetCreated(BaseUICommand* command, QWidget* widget);
    void widgetDestroyed(BaseUICommand* command);
    void bindingEstablished(BaseUICommand* command, QWidget* widget);
    void bindingRemoved(BaseUICommand* command);
    void syncError(BaseUICommand* command, const QString& error);

private slots:
    void onCommandPropertyChanged(const QString& property,
                                  const QVariant& value);
    void onWidgetPropertyChanged();

private:
    // **Mapping registry**
    std::unordered_map<QString, WidgetMappingConfig> mappings_;

    // **Active bindings**
    struct BindingInfo {
        BaseUICommand* command;
        QWidget* widget;
        std::vector<QMetaObject::Connection> connections;
    };
    std::unordered_map<BaseUICommand*, BindingInfo> active_bindings_;

    // **Helper methods**
    void setupDefaultMappings();
    void connectPropertySync(BaseUICommand* command, QWidget* widget,
                             const PropertySyncConfig& config);
    void connectEventMapping(BaseUICommand* command, QWidget* widget,
                             const EventMappingConfig& config);

    QVariant convertProperty(
        const QVariant& value,
        const std::function<QVariant(const QVariant&)>& converter);
    void handleSyncError(BaseUICommand* command, const QString& error);

    // **Qt Meta-Object utilities**
    bool setWidgetProperty(QWidget* widget, const QString& property,
                           const QVariant& value);
    QVariant getWidgetProperty(QWidget* widget, const QString& property);
    bool connectToSignal(QWidget* widget, const QString& signal,
                         QObject* receiver, const char* slot);
};

// **Template implementation for widget registration**
template <typename WidgetType>
void WidgetMapper::registerMapping(const QString& command_type) {
    static_assert(std::is_base_of_v<QWidget, WidgetType>,
                  "WidgetType must inherit from QWidget");

    WidgetMappingConfig config;
    config.widget_type = WidgetType::staticMetaObject.className();
    config.factory = []() -> std::unique_ptr<QWidget> {
        return std::make_unique<WidgetType>();
    };

    // Set up default property mappings based on common Qt properties
    if constexpr (std::is_same_v<WidgetType, QPushButton> ||
                  std::is_same_v<WidgetType, QLabel>) {
        config.property_mappings.emplace_back("text", "text");
    }

    if constexpr (std::is_same_v<WidgetType, QPushButton>) {
        config.event_mappings.emplace_back("clicked()", "clicked");
    }

    // Add enabled/visible mappings for all widgets
    config.property_mappings.emplace_back("enabled", "enabled");
    config.property_mappings.emplace_back("visible", "visible");

    registerMapping(command_type, config);
}

// **Utility class for automatic widget type registration**
template <typename WidgetType>
class WidgetMappingRegistrar {
public:
    explicit WidgetMappingRegistrar(const QString& command_type) {
        WidgetMapper::instance().registerMapping<WidgetType>(command_type);
    }
};

// **Macro for easy widget mapping registration**
#define REGISTER_WIDGET_MAPPING(CommandType, WidgetType)                   \
    static WidgetMappingRegistrar<WidgetType> g_##CommandType##_registrar( \
        #CommandType)

}  // namespace DeclarativeUI::Command::UI
