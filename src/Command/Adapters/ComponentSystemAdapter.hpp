#pragma once

#include <QObject>
#include <QString>
#include <QWidget>
#include <QLayout>
#include <memory>
#include <functional>
#include <unordered_map>
#include <vector>

#include "../UICommand.hpp"
#include "../UICommandFactory.hpp"
#include "../WidgetMapper.hpp"
#include "../CommandBuilder.hpp"
#include "../../Components/Button.hpp"
#include "../../Components/Widget.hpp"
#include "../../Components/Layout.hpp"

namespace DeclarativeUI::Command::Adapters {

/**
 * @brief Adapter that integrates the existing Component system with the new Command architecture
 * 
 * This adapter allows seamless interoperability between the existing Component-based UI
 * and the new Command-based UI, enabling gradual migration and hybrid applications.
 */
class ComponentSystemAdapter : public QObject {
    Q_OBJECT
    
public:
    explicit ComponentSystemAdapter(QObject* parent = nullptr);
    virtual ~ComponentSystemAdapter() = default;
    
    // **Component to Command conversion**
    std::shared_ptr<UI::BaseUICommand> convertToCommand(Components::Button* button);
    std::shared_ptr<UI::BaseUICommand> convertToCommand(Components::Widget* widget);
    std::shared_ptr<UI::BaseUICommand> convertToCommand(Components::Layout* layout);
    
    // **Command to Component conversion**
    std::unique_ptr<Components::Button> convertToButton(std::shared_ptr<UI::BaseUICommand> command);
    std::unique_ptr<Components::Widget> convertToWidget(std::shared_ptr<UI::BaseUICommand> command);
    std::unique_ptr<Components::Layout> convertToLayout(std::shared_ptr<UI::BaseUICommand> command);
    
    // **Hybrid container that can hold both Components and Commands**
    class HybridContainer;
    std::unique_ptr<HybridContainer> createHybridContainer();
    
    // **Bidirectional synchronization**
    void establishSync(Components::Button* button, std::shared_ptr<UI::BaseUICommand> command);
    void establishSync(Components::Widget* widget, std::shared_ptr<UI::BaseUICommand> command);
    void removeSync(Components::Button* button);
    void removeSync(Components::Widget* widget);
    
    // **Batch conversion operations**
    std::vector<std::shared_ptr<UI::BaseUICommand>> convertComponentHierarchy(QWidget* rootWidget);
    std::unique_ptr<QWidget> convertCommandHierarchy(std::shared_ptr<UI::BaseUICommand> rootCommand);
    
    // **Migration helpers**
    void migrateComponentToCommand(QWidget* component, std::shared_ptr<UI::BaseUICommand> command);
    void migrateCommandToComponent(std::shared_ptr<UI::BaseUICommand> command, QWidget* component);
    
    // **Registration of custom converters**
    template<typename ComponentType, typename CommandType>
    void registerConverter();
    
    void registerCustomConverter(const QString& componentType, const QString& commandType,
                                std::function<std::shared_ptr<UI::BaseUICommand>(QObject*)> toCommand,
                                std::function<std::unique_ptr<QObject>(std::shared_ptr<UI::BaseUICommand>)> toComponent);
    
    // **Query methods**
    bool hasConverter(const QString& componentType) const;
    bool hasReverseConverter(const QString& commandType) const;
    QStringList getSupportedComponentTypes() const;
    QStringList getSupportedCommandTypes() const;
    
    // **Core conversion methods**
    std::shared_ptr<UI::BaseUICommand> createCommandFromComponent(QObject* component);
    std::unique_ptr<QObject> createComponentFromCommand(std::shared_ptr<UI::BaseUICommand> command);

    // **Singleton access**
    static ComponentSystemAdapter& instance();

signals:
    void componentConverted(QObject* component, std::shared_ptr<UI::BaseUICommand> command);
    void commandConverted(std::shared_ptr<UI::BaseUICommand> command, QObject* component);
    void syncEstablished(QObject* component, std::shared_ptr<UI::BaseUICommand> command);
    void syncRemoved(QObject* component);
    void conversionError(const QString& error);

private slots:
    void onComponentChanged();

private:
    // **Converter registry**
    struct ConverterInfo {
        std::function<std::shared_ptr<UI::BaseUICommand>(QObject*)> to_command;
        std::function<std::unique_ptr<QObject>(std::shared_ptr<UI::BaseUICommand>)> to_component;
        QString component_type;
        QString command_type;
    };
    
    std::unordered_map<QString, ConverterInfo> converters_;
    std::unordered_map<QString, QString> command_to_component_map_;
    
    // **Synchronization tracking**
    struct SyncInfo {
        QObject* component;
        std::shared_ptr<UI::BaseUICommand> command;
        std::vector<QMetaObject::Connection> connections;
    };
    
    std::unordered_map<QObject*, SyncInfo> active_syncs_;
    std::unordered_map<QObject*, std::shared_ptr<UI::BaseUICommand>> sync_context_;

    // **Helper methods**
    void setupBuiltinConverters();
    void setupComponentToCommandSync(QObject* component, std::shared_ptr<UI::BaseUICommand> command, SyncInfo& syncInfo);
    void setupCommandToComponentSync(std::shared_ptr<UI::BaseUICommand> command, QObject* component, SyncInfo& syncInfo);
    void cleanupSync(SyncInfo& syncInfo);

    void handleConversionError(const QString& error);
};

/**
 * @brief Hybrid container that can seamlessly hold both Components and Commands
 */
class ComponentSystemAdapter::HybridContainer : public QObject {
    Q_OBJECT
    
public:
    explicit HybridContainer(QObject* parent = nullptr);
    virtual ~HybridContainer() = default;
    
    // **Add items (automatically converts as needed)**
    void addComponent(std::unique_ptr<QObject> component);
    void addCommand(std::shared_ptr<UI::BaseUICommand> command);
    void addWidget(std::unique_ptr<QWidget> widget);
    
    // **Remove items**
    void removeComponent(QObject* component);
    void removeCommand(std::shared_ptr<UI::BaseUICommand> command);
    void clear();
    
    // **Layout management**
    void setLayout(const QString& layoutType);
    QString getLayout() const { return layout_type_; }
    
    void setSpacing(int spacing);
    int getSpacing() const { return spacing_; }
    
    void setMargins(int left, int top, int right, int bottom);
    void setMargins(int margin) { setMargins(margin, margin, margin, margin); }
    
    // **Access methods**
    std::vector<QObject*> getComponents() const;
    std::vector<std::shared_ptr<UI::BaseUICommand>> getCommands() const;
    std::vector<QWidget*> getWidgets() const;
    
    int getItemCount() const;
    bool isEmpty() const { return getItemCount() == 0; }
    
    // **Conversion methods**
    std::shared_ptr<UI::BaseUICommand> toCommand();
    std::unique_ptr<QWidget> toWidget();
    
    // **Synchronization**
    void enableAutoSync(bool enabled) { auto_sync_enabled_ = enabled; }
    bool isAutoSyncEnabled() const { return auto_sync_enabled_; }
    void syncAll();
    
signals:
    void itemAdded(QObject* item);
    void itemRemoved(QObject* item);
    void layoutChanged(const QString& layoutType);
    
private:
    // **Item storage**
    struct HybridItem {
        enum Type { Component, Command, Widget } type;
        QObject* component = nullptr;
        std::shared_ptr<UI::BaseUICommand> command;
        QWidget* widget = nullptr;
        
        HybridItem(QObject* comp) : type(Component), component(comp) {}
        HybridItem(std::shared_ptr<UI::BaseUICommand> cmd) : type(Command), command(cmd) {}
        HybridItem(QWidget* w) : type(Widget), widget(w) {}
    };
    
    std::vector<HybridItem> items_;
    QString layout_type_ = "VBox";
    int spacing_ = 6;
    int margin_left_ = 9, margin_top_ = 9, margin_right_ = 9, margin_bottom_ = 9;
    bool auto_sync_enabled_ = true;
    
    void syncItem(HybridItem& item);
    void setupItemConnections(HybridItem& item);
    void cleanupItemConnections(HybridItem& item);
};

/**
 * @brief Enhanced Button adapter that provides full compatibility between Component and Command buttons
 */
class ButtonAdapter {
public:
    // **Convert Component Button to Command**
    static std::shared_ptr<UI::BaseUICommand> toCommand(Components::Button* button);
    
    // **Convert Command to Component Button**
    static std::unique_ptr<Components::Button> toComponent(std::shared_ptr<UI::BaseUICommand> command);
    
    // **Establish bidirectional synchronization**
    static void establishSync(Components::Button* button, std::shared_ptr<UI::BaseUICommand> command);
    
    // **Property synchronization helpers**
    static void syncButtonToCommand(Components::Button* button, std::shared_ptr<UI::BaseUICommand> command);
    static void syncCommandToButton(std::shared_ptr<UI::BaseUICommand> command, Components::Button* button);
    
private:
    static void setupButtonPropertySync(Components::Button* button, std::shared_ptr<UI::BaseUICommand> command);
    static void setupButtonEventSync(Components::Button* button, std::shared_ptr<UI::BaseUICommand> command);
};

/**
 * @brief Enhanced Widget adapter for generic widget conversion
 */
class WidgetAdapter {
public:
    // **Convert Component Widget to Command**
    static std::shared_ptr<UI::BaseUICommand> toCommand(Components::Widget* widget);
    
    // **Convert Command to Component Widget**
    static std::unique_ptr<Components::Widget> toComponent(std::shared_ptr<UI::BaseUICommand> command);
    
    // **Establish bidirectional synchronization**
    static void establishSync(Components::Widget* widget, std::shared_ptr<UI::BaseUICommand> command);
    
    // **Property synchronization helpers**
    static void syncWidgetToCommand(Components::Widget* widget, std::shared_ptr<UI::BaseUICommand> command);
    static void syncCommandToWidget(std::shared_ptr<UI::BaseUICommand> command, Components::Widget* widget);
    
private:
    static void setupWidgetPropertySync(Components::Widget* widget, std::shared_ptr<UI::BaseUICommand> command);
    static void setupWidgetEventSync(Components::Widget* widget, std::shared_ptr<UI::BaseUICommand> command);
};

/**
 * @brief Layout adapter for converting between Component layouts and Command containers
 */
class LayoutAdapter {
public:
    // **Convert Component Layout to Command Container**
    static std::shared_ptr<UI::BaseUICommand> toCommand(Components::Layout* layout);
    
    // **Convert Command Container to Component Layout**
    static std::unique_ptr<Components::Layout> toComponent(std::shared_ptr<UI::BaseUICommand> command);
    
    // **Layout-specific conversion helpers**
    static std::shared_ptr<UI::BaseUICommand> convertLayoutHierarchy(Components::Layout* layout);
    static std::unique_ptr<Components::Layout> convertCommandHierarchy(std::shared_ptr<UI::BaseUICommand> command);
    
private:
    static void convertLayoutProperties(Components::Layout* layout, std::shared_ptr<UI::BaseUICommand> command);
    static void convertCommandProperties(std::shared_ptr<UI::BaseUICommand> command, Components::Layout* layout);
    static void convertLayoutChildren(Components::Layout* layout, std::shared_ptr<UI::BaseUICommand> command);
    static void convertCommandChildren(std::shared_ptr<UI::BaseUICommand> command, Components::Layout* layout);
};

// **Template implementation**
template<typename ComponentType, typename CommandType>
void ComponentSystemAdapter::registerConverter() {
    static_assert(std::is_base_of_v<QObject, ComponentType>, "ComponentType must inherit from QObject");
    static_assert(std::is_base_of_v<UI::BaseUICommand, CommandType>, "CommandType must inherit from BaseUICommand");
    
    QString componentTypeName = ComponentType::staticMetaObject.className();
    QString commandTypeName = CommandType::staticMetaObject.className();
    
    ConverterInfo info;
    info.component_type = componentTypeName;
    info.command_type = commandTypeName;
    
    info.to_command = [](QObject* component) -> std::shared_ptr<UI::BaseUICommand> {
        auto* typedComponent = qobject_cast<ComponentType*>(component);
        if (!typedComponent) {
            return nullptr;
        }
        
        auto command = std::make_shared<CommandType>();
        // Set up property synchronization
        // This would need component-specific implementation
        return command;
    };
    
    info.to_component = [](std::shared_ptr<UI::BaseUICommand> command) -> std::unique_ptr<QObject> {
        auto typedCommand = std::dynamic_pointer_cast<CommandType>(command);
        if (!typedCommand) {
            return nullptr;
        }
        
        auto component = std::make_unique<ComponentType>();
        // Set up property synchronization
        // This would need command-specific implementation
        return std::move(component);
    };
    
    converters_[componentTypeName] = info;
    command_to_component_map_[commandTypeName] = componentTypeName;
}

// **Convenience functions for Component-Command interoperability**
namespace ComponentCommand {

// **Quick conversion functions**
inline std::shared_ptr<UI::BaseUICommand> toCommand(QObject* component) {
    return ComponentSystemAdapter::instance().createCommandFromComponent(component);
}

inline std::unique_ptr<QObject> toComponent(std::shared_ptr<UI::BaseUICommand> command) {
    return ComponentSystemAdapter::instance().createComponentFromCommand(command);
}

// **Specific type conversions**
inline std::shared_ptr<UI::BaseUICommand> buttonToCommand(Components::Button* button) {
    return ButtonAdapter::toCommand(button);
}

inline std::unique_ptr<Components::Button> commandToButton(std::shared_ptr<UI::BaseUICommand> command) {
    return ButtonAdapter::toComponent(command);
}

// **Synchronization helpers**
inline void sync(Components::Button* button, std::shared_ptr<UI::BaseUICommand> command) {
    ButtonAdapter::establishSync(button, command);
}

inline void sync(Components::Widget* widget, std::shared_ptr<UI::BaseUICommand> command) {
    WidgetAdapter::establishSync(widget, command);
}

// **Hybrid container creation**
inline std::unique_ptr<ComponentSystemAdapter::HybridContainer> createHybrid() {
    return ComponentSystemAdapter::instance().createHybridContainer();
}

}  // namespace ComponentCommand

}  // namespace DeclarativeUI::Command::Adapters
