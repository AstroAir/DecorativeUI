#pragma once

/**
 * @file ComponentSystemAdapter.hpp
 * @brief Provides an adapter for integrating the legacy Component system with
 * the new Command architecture in DeclarativeUI.
 *
 * This adapter enables seamless interoperability between Component-based and
 * Command-based UIs, supporting conversion, synchronization, hybrid containers,
 * and migration utilities.
 */

#include <QLayout>
#include <QObject>
#include <QString>
#include <QWidget>
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

#include "../../Components/Button.hpp"
#include "../../Components/Layout.hpp"
#include "../../Components/Widget.hpp"
#include "../UICommand.hpp"

namespace DeclarativeUI::Command::Adapters {

/**
 * @class ComponentSystemAdapter
 * @brief Adapter that integrates the existing Component system with the new
 * Command architecture.
 *
 * Allows conversion between Components and Commands, bidirectional
 * synchronization, batch operations, migration helpers, and registration of
 * custom converters. Enables hybrid applications and gradual migration from
 * Components to Commands.
 */
class ComponentSystemAdapter : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Constructs the adapter.
     * @param parent Parent QObject.
     */
    explicit ComponentSystemAdapter(QObject* parent = nullptr);

    /**
     * @brief Virtual destructor.
     */
    virtual ~ComponentSystemAdapter() = default;

    // **Component to Command conversion**

    /**
     * @brief Converts a Button Component to a Command.
     * @param button Pointer to the Button Component.
     * @return Shared pointer to the corresponding Command.
     */
    std::shared_ptr<UI::BaseUICommand> convertToCommand(
        Components::Button* button);

    /**
     * @brief Converts a Widget Component to a Command.
     * @param widget Pointer to the Widget Component.
     * @return Shared pointer to the corresponding Command.
     */
    std::shared_ptr<UI::BaseUICommand> convertToCommand(
        Components::Widget* widget);

    /**
     * @brief Converts a Layout Component to a Command.
     * @param layout Pointer to the Layout Component.
     * @return Shared pointer to the corresponding Command.
     */
    std::shared_ptr<UI::BaseUICommand> convertToCommand(
        Components::Layout* layout);

    // **Command to Component conversion**

    /**
     * @brief Converts a Command to a Button Component.
     * @param command Shared pointer to the Command.
     * @return Unique pointer to the corresponding Button Component.
     */
    std::unique_ptr<Components::Button> convertToButton(
        std::shared_ptr<UI::BaseUICommand> command);

    /**
     * @brief Converts a Command to a Widget Component.
     * @param command Shared pointer to the Command.
     * @return Unique pointer to the corresponding Widget Component.
     */
    std::unique_ptr<Components::Widget> convertToWidget(
        std::shared_ptr<UI::BaseUICommand> command);

    /**
     * @brief Converts a Command to a Layout Component.
     * @param command Shared pointer to the Command.
     * @return Unique pointer to the corresponding Layout Component.
     */
    std::unique_ptr<Components::Layout> convertToLayout(
        std::shared_ptr<UI::BaseUICommand> command);

    // **Hybrid container**

    /**
     * @class HybridContainer
     * @brief Container that can hold both Components and Commands, supporting
     * mixed UI hierarchies.
     */
    class HybridContainer;

    /**
     * @brief Creates a new HybridContainer instance.
     * @return Unique pointer to the HybridContainer.
     */
    std::unique_ptr<HybridContainer> createHybridContainer();

    // **Bidirectional synchronization**

    /**
     * @brief Establishes bidirectional synchronization between a Button
     * Component and a Command.
     * @param button Pointer to the Button Component.
     * @param command Shared pointer to the Command.
     */
    void establishSync(Components::Button* button,
                       std::shared_ptr<UI::BaseUICommand> command);

    /**
     * @brief Establishes bidirectional synchronization between a Widget
     * Component and a Command.
     * @param widget Pointer to the Widget Component.
     * @param command Shared pointer to the Command.
     */
    void establishSync(Components::Widget* widget,
                       std::shared_ptr<UI::BaseUICommand> command);

    /**
     * @brief Removes synchronization for a Button Component.
     * @param button Pointer to the Button Component.
     */
    void removeSync(Components::Button* button);

    /**
     * @brief Removes synchronization for a Widget Component.
     * @param widget Pointer to the Widget Component.
     */
    void removeSync(Components::Widget* widget);

    // **Batch conversion operations**

    /**
     * @brief Converts an entire QWidget hierarchy to a vector of Commands.
     * @param rootWidget Pointer to the root QWidget.
     * @return Vector of shared pointers to Commands.
     */
    std::vector<std::shared_ptr<UI::BaseUICommand>> convertComponentHierarchy(
        QWidget* rootWidget);

    /**
     * @brief Converts a Command hierarchy to a QWidget hierarchy.
     * @param rootCommand Shared pointer to the root Command.
     * @return Unique pointer to the root QWidget.
     */
    std::unique_ptr<QWidget> convertCommandHierarchy(
        std::shared_ptr<UI::BaseUICommand> rootCommand);

    // **Migration helpers**

    /**
     * @brief Migrates a Component to a Command, transferring state and
     * properties.
     * @param component Pointer to the Component (QWidget).
     * @param command Shared pointer to the Command.
     */
    void migrateComponentToCommand(QWidget* component,
                                   std::shared_ptr<UI::BaseUICommand> command);

    /**
     * @brief Migrates a Command to a Component, transferring state and
     * properties.
     * @param command Shared pointer to the Command.
     * @param component Pointer to the Component (QWidget).
     */
    void migrateCommandToComponent(std::shared_ptr<UI::BaseUICommand> command,
                                   QWidget* component);

    // **Registration of custom converters**

    /**
     * @brief Registers a custom converter for a specific Component and Command
     * type.
     * @tparam ComponentType The Component type (must inherit QObject).
     * @tparam CommandType The Command type (must inherit BaseUICommand).
     */
    template <typename ComponentType, typename CommandType>
    void registerConverter();

    /**
     * @brief Registers a custom converter using function objects.
     * @param componentType Name of the Component type.
     * @param commandType Name of the Command type.
     * @param toCommand Function to convert from Component to Command.
     * @param toComponent Function to convert from Command to Component.
     */
    void registerCustomConverter(
        const QString& componentType, const QString& commandType,
        std::function<std::shared_ptr<UI::BaseUICommand>(QObject*)> toCommand,
        std::function<
            std::unique_ptr<QObject>(std::shared_ptr<UI::BaseUICommand>)>
            toComponent);

    // **Query methods**

    /**
     * @brief Checks if a converter exists for the given Component type.
     * @param componentType Name of the Component type.
     * @return True if a converter exists.
     */
    bool hasConverter(const QString& componentType) const;

    /**
     * @brief Checks if a reverse converter exists for the given Command type.
     * @param commandType Name of the Command type.
     * @return True if a reverse converter exists.
     */
    bool hasReverseConverter(const QString& commandType) const;

    /**
     * @brief Gets a list of supported Component types for conversion.
     * @return List of Component type names.
     */
    QStringList getSupportedComponentTypes() const;

    /**
     * @brief Gets a list of supported Command types for conversion.
     * @return List of Command type names.
     */
    QStringList getSupportedCommandTypes() const;

    // **Core conversion methods**

    /**
     * @brief Creates a Command from a generic Component (QObject).
     * @param component Pointer to the Component.
     * @return Shared pointer to the created Command.
     */
    std::shared_ptr<UI::BaseUICommand> createCommandFromComponent(
        QObject* component);

    /**
     * @brief Creates a Component (QObject) from a Command.
     * @param command Shared pointer to the Command.
     * @return Unique pointer to the created Component.
     */
    std::unique_ptr<QObject> createComponentFromCommand(
        std::shared_ptr<UI::BaseUICommand> command);

    /**
     * @brief Gets the singleton instance of the adapter.
     * @return Reference to the singleton instance.
     */
    static ComponentSystemAdapter& instance();

signals:
    /**
     * @brief Emitted when a Component is converted to a Command.
     * @param component Pointer to the Component.
     * @param command Shared pointer to the Command.
     */
    void componentConverted(QObject* component,
                            std::shared_ptr<UI::BaseUICommand> command);

    /**
     * @brief Emitted when a Command is converted to a Component.
     * @param command Shared pointer to the Command.
     * @param component Pointer to the Component.
     */
    void commandConverted(std::shared_ptr<UI::BaseUICommand> command,
                          QObject* component);

    /**
     * @brief Emitted when synchronization is established between a Component
     * and a Command.
     * @param component Pointer to the Component.
     * @param command Shared pointer to the Command.
     */
    void syncEstablished(QObject* component,
                         std::shared_ptr<UI::BaseUICommand> command);

    /**
     * @brief Emitted when synchronization is removed for a Component.
     * @param component Pointer to the Component.
     */
    void syncRemoved(QObject* component);

    /**
     * @brief Emitted when a conversion error occurs.
     * @param error Error message.
     */
    void conversionError(const QString& error);

private slots:
    /**
     * @brief Slot called when a Component changes, for synchronization.
     */
    void onComponentChanged();

private:
    // **Converter registry**

    /**
     * @struct ConverterInfo
     * @brief Holds function objects and type names for a registered converter.
     */
    struct ConverterInfo {
        std::function<std::shared_ptr<UI::BaseUICommand>(QObject*)>
            to_command;  ///< Function to convert to Command.
        std::function<std::unique_ptr<QObject>(
            std::shared_ptr<UI::BaseUICommand>)>
            to_component;        ///< Function to convert to Component.
        QString component_type;  ///< Name of the Component type.
        QString command_type;    ///< Name of the Command type.
    };

    std::unordered_map<QString, ConverterInfo>
        converters_;  ///< Map of Component type to ConverterInfo.
    std::unordered_map<QString, QString>
        command_to_component_map_;  ///< Map of Command type to Component type.

    // **Synchronization tracking**

    /**
     * @struct SyncInfo
     * @brief Holds synchronization context and connections for a
     * Component-Command pair.
     */
    struct SyncInfo {
        QObject* component;  ///< Pointer to the Component.
        std::shared_ptr<UI::BaseUICommand>
            command;  ///< Shared pointer to the Command.
        std::vector<QMetaObject::Connection>
            connections;  ///< List of active connections.
    };

    std::unordered_map<QObject*, SyncInfo>
        active_syncs_;  ///< Map of Component to SyncInfo.
    std::unordered_map<QObject*, std::shared_ptr<UI::BaseUICommand>>
        sync_context_;  ///< Map for quick lookup.

    // **Helper methods**

    /**
     * @brief Sets up built-in converters for standard Component/Command types.
     */
    void setupBuiltinConverters();

    /**
     * @brief Sets up synchronization from Component to Command.
     * @param component Pointer to the Component.
     * @param command Shared pointer to the Command.
     * @param syncInfo Reference to the SyncInfo structure.
     */
    void setupComponentToCommandSync(QObject* component,
                                     std::shared_ptr<UI::BaseUICommand> command,
                                     SyncInfo& syncInfo);

    /**
     * @brief Sets up synchronization from Command to Component.
     * @param command Shared pointer to the Command.
     * @param component Pointer to the Component.
     * @param syncInfo Reference to the SyncInfo structure.
     */
    void setupCommandToComponentSync(std::shared_ptr<UI::BaseUICommand> command,
                                     QObject* component, SyncInfo& syncInfo);

    /**
     * @brief Cleans up synchronization connections for a SyncInfo.
     * @param syncInfo Reference to the SyncInfo structure.
     */
    void cleanupSync(SyncInfo& syncInfo);

    /**
     * @brief Handles conversion errors by emitting the conversionError signal.
     * @param error Error message.
     */
    void handleConversionError(const QString& error);
};

/**
 * @class ComponentSystemAdapter::HybridContainer
 * @brief Hybrid container that can seamlessly hold both Components and
 * Commands.
 *
 * Supports adding/removing items, layout management, conversion, and
 * synchronization.
 */
class ComponentSystemAdapter::HybridContainer : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Constructs a HybridContainer.
     * @param parent Parent QObject.
     */
    explicit HybridContainer(QObject* parent = nullptr);

    /**
     * @brief Virtual destructor.
     */
    virtual ~HybridContainer() = default;

    // **Add items**

    /**
     * @brief Adds a Component to the container.
     * @param component Unique pointer to the Component (QObject).
     */
    void addComponent(std::unique_ptr<QObject> component);

    /**
     * @brief Adds a Command to the container.
     * @param command Shared pointer to the Command.
     */
    void addCommand(std::shared_ptr<UI::BaseUICommand> command);

    /**
     * @brief Adds a QWidget to the container.
     * @param widget Unique pointer to the QWidget.
     */
    void addWidget(std::unique_ptr<QWidget> widget);

    // **Remove items**

    /**
     * @brief Removes a Component from the container.
     * @param component Pointer to the Component.
     */
    void removeComponent(QObject* component);

    /**
     * @brief Removes a Command from the container.
     * @param command Shared pointer to the Command.
     */
    void removeCommand(std::shared_ptr<UI::BaseUICommand> command);

    /**
     * @brief Clears all items from the container.
     */
    void clear();

    // **Layout management**

    /**
     * @brief Sets the layout type for the container (e.g., "VBox", "HBox").
     * @param layoutType Name of the layout type.
     */
    void setLayout(const QString& layoutType);

    /**
     * @brief Gets the current layout type.
     * @return Layout type name.
     */
    QString getLayout() const { return layout_type_; }

    /**
     * @brief Sets the spacing between items.
     * @param spacing Spacing value in pixels.
     */
    void setSpacing(int spacing);

    /**
     * @brief Gets the current spacing value.
     * @return Spacing in pixels.
     */
    int getSpacing() const { return spacing_; }

    /**
     * @brief Sets the margins for the container.
     * @param left Left margin.
     * @param top Top margin.
     * @param right Right margin.
     * @param bottom Bottom margin.
     */
    void setMargins(int left, int top, int right, int bottom);

    /**
     * @brief Sets all margins to the same value.
     * @param margin Margin value for all sides.
     */
    void setMargins(int margin) { setMargins(margin, margin, margin, margin); }

    // **Access methods**

    /**
     * @brief Gets all Components in the container.
     * @return Vector of pointers to Components.
     */
    std::vector<QObject*> getComponents() const;

    /**
     * @brief Gets all Commands in the container.
     * @return Vector of shared pointers to Commands.
     */
    std::vector<std::shared_ptr<UI::BaseUICommand>> getCommands() const;

    /**
     * @brief Gets all QWidgets in the container.
     * @return Vector of pointers to QWidgets.
     */
    std::vector<QWidget*> getWidgets() const;

    /**
     * @brief Gets the number of items in the container.
     * @return Item count.
     */
    int getItemCount() const;

    /**
     * @brief Checks if the container is empty.
     * @return True if empty, false otherwise.
     */
    bool isEmpty() const { return getItemCount() == 0; }

    // **Conversion methods**

    /**
     * @brief Converts the container contents to a single Command.
     * @return Shared pointer to the resulting Command.
     */
    std::shared_ptr<UI::BaseUICommand> toCommand();

    /**
     * @brief Converts the container contents to a QWidget.
     * @return Unique pointer to the resulting QWidget.
     */
    std::unique_ptr<QWidget> toWidget();

    // **Synchronization**

    /**
     * @brief Enables or disables automatic synchronization for all items.
     * @param enabled True to enable, false to disable.
     */
    void enableAutoSync(bool enabled) { auto_sync_enabled_ = enabled; }

    /**
     * @brief Checks if automatic synchronization is enabled.
     * @return True if enabled, false otherwise.
     */
    bool isAutoSyncEnabled() const { return auto_sync_enabled_; }

    /**
     * @brief Synchronizes all items in the container.
     */
    void syncAll();

signals:
    /**
     * @brief Emitted when an item is added to the container.
     * @param item Pointer to the added item.
     */
    void itemAdded(QObject* item);

    /**
     * @brief Emitted when an item is removed from the container.
     * @param item Pointer to the removed item.
     */
    void itemRemoved(QObject* item);

    /**
     * @brief Emitted when the layout type changes.
     * @param layoutType New layout type.
     */
    void layoutChanged(const QString& layoutType);

private:
    // **Item storage**

    /**
     * @struct HybridItem
     * @brief Represents an item in the HybridContainer (Component, Command, or
     * Widget).
     */
    struct HybridItem {
        enum Type { Component, Command, Widget } type;
        QObject* component = nullptr;
        std::shared_ptr<UI::BaseUICommand> command;
        QWidget* widget = nullptr;

        HybridItem(QObject* comp) : type(Component), component(comp) {}
        HybridItem(std::shared_ptr<UI::BaseUICommand> cmd)
            : type(Command), command(cmd) {}
        HybridItem(QWidget* w) : type(Widget), widget(w) {}
    };

    std::vector<HybridItem> items_;  ///< List of items in the container.
    QString layout_type_ = "VBox";   ///< Layout type.
    int spacing_ = 6;                ///< Spacing between items.
    int margin_left_ = 9, margin_top_ = 9, margin_right_ = 9,
        margin_bottom_ = 9;          ///< Margins.
    bool auto_sync_enabled_ = true;  ///< Whether auto-sync is enabled.

    /**
     * @brief Synchronizes a single item.
     * @param item Reference to the HybridItem.
     */
    void syncItem(HybridItem& item);

    /**
     * @brief Sets up connections for an item.
     * @param item Reference to the HybridItem.
     */
    void setupItemConnections(HybridItem& item);

    /**
     * @brief Cleans up connections for an item.
     * @param item Reference to the HybridItem.
     */
    void cleanupItemConnections(HybridItem& item);
};

/**
 * @class ButtonAdapter
 * @brief Enhanced Button adapter that provides full compatibility between
 * Component and Command buttons.
 *
 * Supports conversion and bidirectional synchronization.
 */
class ButtonAdapter {
public:
    /**
     * @brief Converts a Button Component to a Command.
     * @param button Pointer to the Button Component.
     * @return Shared pointer to the Command.
     */
    static std::shared_ptr<UI::BaseUICommand> toCommand(
        Components::Button* button);

    /**
     * @brief Converts a Command to a Button Component.
     * @param command Shared pointer to the Command.
     * @return Unique pointer to the Button Component.
     */
    static std::unique_ptr<Components::Button> toComponent(
        std::shared_ptr<UI::BaseUICommand> command);

    /**
     * @brief Establishes bidirectional synchronization between a Button
     * Component and a Command.
     * @param button Pointer to the Button Component.
     * @param command Shared pointer to the Command.
     */
    static void establishSync(Components::Button* button,
                              std::shared_ptr<UI::BaseUICommand> command);

    /**
     * @brief Synchronizes properties from Button to Command.
     * @param button Pointer to the Button Component.
     * @param command Shared pointer to the Command.
     */
    static void syncButtonToCommand(Components::Button* button,
                                    std::shared_ptr<UI::BaseUICommand> command);

    /**
     * @brief Synchronizes properties from Command to Button.
     * @param command Shared pointer to the Command.
     * @param button Pointer to the Button Component.
     */
    static void syncCommandToButton(std::shared_ptr<UI::BaseUICommand> command,
                                    Components::Button* button);

private:
    static void setupButtonPropertySync(
        Components::Button* button, std::shared_ptr<UI::BaseUICommand> command);
    static void setupButtonEventSync(
        Components::Button* button, std::shared_ptr<UI::BaseUICommand> command);
};

/**
 * @class WidgetAdapter
 * @brief Enhanced Widget adapter for generic widget conversion and
 * synchronization.
 */
class WidgetAdapter {
public:
    /**
     * @brief Converts a Widget Component to a Command.
     * @param widget Pointer to the Widget Component.
     * @return Shared pointer to the Command.
     */
    static std::shared_ptr<UI::BaseUICommand> toCommand(
        Components::Widget* widget);

    /**
     * @brief Converts a Command to a Widget Component.
     * @param command Shared pointer to the Command.
     * @return Unique pointer to the Widget Component.
     */
    static std::unique_ptr<Components::Widget> toComponent(
        std::shared_ptr<UI::BaseUICommand> command);

    /**
     * @brief Establishes bidirectional synchronization between a Widget
     * Component and a Command.
     * @param widget Pointer to the Widget Component.
     * @param command Shared pointer to the Command.
     */
    static void establishSync(Components::Widget* widget,
                              std::shared_ptr<UI::BaseUICommand> command);

    /**
     * @brief Synchronizes properties from Widget to Command.
     * @param widget Pointer to the Widget Component.
     * @param command Shared pointer to the Command.
     */
    static void syncWidgetToCommand(Components::Widget* widget,
                                    std::shared_ptr<UI::BaseUICommand> command);

    /**
     * @brief Synchronizes properties from Command to Widget.
     * @param command Shared pointer to the Command.
     * @param widget Pointer to the Widget Component.
     */
    static void syncCommandToWidget(std::shared_ptr<UI::BaseUICommand> command,
                                    Components::Widget* widget);

private:
    static void setupWidgetPropertySync(
        Components::Widget* widget, std::shared_ptr<UI::BaseUICommand> command);
    static void setupWidgetEventSync(
        Components::Widget* widget, std::shared_ptr<UI::BaseUICommand> command);
};

/**
 * @class LayoutAdapter
 * @brief Adapter for converting between Component layouts and Command
 * containers.
 *
 * Supports conversion of layout hierarchies and properties.
 */
class LayoutAdapter {
public:
    /**
     * @brief Converts a Layout Component to a Command container.
     * @param layout Pointer to the Layout Component.
     * @return Shared pointer to the Command container.
     */
    static std::shared_ptr<UI::BaseUICommand> toCommand(
        Components::Layout* layout);

    /**
     * @brief Converts a Command container to a Layout Component.
     * @param command Shared pointer to the Command container.
     * @return Unique pointer to the Layout Component.
     */
    static std::unique_ptr<Components::Layout> toComponent(
        std::shared_ptr<UI::BaseUICommand> command);

    /**
     * @brief Converts a layout hierarchy from Components to Commands.
     * @param layout Pointer to the root Layout Component.
     * @return Shared pointer to the root Command.
     */
    static std::shared_ptr<UI::BaseUICommand> convertLayoutHierarchy(
        Components::Layout* layout);

    /**
     * @brief Converts a command hierarchy to a Layout Component hierarchy.
     * @param command Shared pointer to the root Command.
     * @return Unique pointer to the root Layout Component.
     */
    static std::unique_ptr<Components::Layout> convertCommandHierarchy(
        std::shared_ptr<UI::BaseUICommand> command);

private:
    static void convertLayoutProperties(
        Components::Layout* layout, std::shared_ptr<UI::BaseUICommand> command);
    static void convertCommandProperties(
        std::shared_ptr<UI::BaseUICommand> command, Components::Layout* layout);
    static void convertLayoutChildren(
        Components::Layout* layout, std::shared_ptr<UI::BaseUICommand> command);
    static void convertCommandChildren(
        std::shared_ptr<UI::BaseUICommand> command, Components::Layout* layout);
};

// **Template implementation**
/**
 * @brief Registers a converter for a specific Component and Command type.
 * @tparam ComponentType The Component type (inherits QObject).
 * @tparam CommandType The Command type (inherits BaseUICommand).
 */
template <typename ComponentType, typename CommandType>
void ComponentSystemAdapter::registerConverter() {
    static_assert(std::is_base_of_v<QObject, ComponentType>,
                  "ComponentType must inherit from QObject");
    static_assert(std::is_base_of_v<UI::BaseUICommand, CommandType>,
                  "CommandType must inherit from BaseUICommand");

    QString componentTypeName = ComponentType::staticMetaObject.className();
    QString commandTypeName = CommandType::staticMetaObject.className();

    ConverterInfo info;
    info.component_type = componentTypeName;
    info.command_type = commandTypeName;

    info.to_command =
        [](QObject* component) -> std::shared_ptr<UI::BaseUICommand> {
        auto* typedComponent = qobject_cast<ComponentType*>(component);
        if (!typedComponent) {
            return nullptr;
        }
        auto command = std::make_shared<CommandType>();
        // Set up property synchronization (component-specific)
        return command;
    };

    info.to_component = [](std::shared_ptr<UI::BaseUICommand> command)
        -> std::unique_ptr<QObject> {
        auto typedCommand = std::dynamic_pointer_cast<CommandType>(command);
        if (!typedCommand) {
            return nullptr;
        }
        auto component = std::make_unique<ComponentType>();
        // Set up property synchronization (command-specific)
        return std::move(component);
    };

    converters_[componentTypeName] = info;
    command_to_component_map_[commandTypeName] = componentTypeName;
}

// **Convenience functions for Component-Command interoperability**
namespace ComponentCommand {

/**
 * @brief Converts a generic Component (QObject) to a Command.
 * @param component Pointer to the Component.
 * @return Shared pointer to the Command.
 */
inline std::shared_ptr<UI::BaseUICommand> toCommand(QObject* component) {
    return ComponentSystemAdapter::instance().createCommandFromComponent(
        component);
}

/**
 * @brief Converts a Command to a generic Component (QObject).
 * @param command Shared pointer to the Command.
 * @return Unique pointer to the Component.
 */
inline std::unique_ptr<QObject> toComponent(
    std::shared_ptr<UI::BaseUICommand> command) {
    return ComponentSystemAdapter::instance().createComponentFromCommand(
        command);
}

/**
 * @brief Converts a Button Component to a Command.
 * @param button Pointer to the Button Component.
 * @return Shared pointer to the Command.
 */
inline std::shared_ptr<UI::BaseUICommand> buttonToCommand(
    Components::Button* button) {
    return ButtonAdapter::toCommand(button);
}

/**
 * @brief Converts a Command to a Button Component.
 * @param command Shared pointer to the Command.
 * @return Unique pointer to the Button Component.
 */
inline std::unique_ptr<Components::Button> commandToButton(
    std::shared_ptr<UI::BaseUICommand> command) {
    return ButtonAdapter::toComponent(command);
}

/**
 * @brief Establishes synchronization between a Button Component and a Command.
 * @param button Pointer to the Button Component.
 * @param command Shared pointer to the Command.
 */
inline void sync(Components::Button* button,
                 std::shared_ptr<UI::BaseUICommand> command) {
    ButtonAdapter::establishSync(button, command);
}

/**
 * @brief Establishes synchronization between a Widget Component and a Command.
 * @param widget Pointer to the Widget Component.
 * @param command Shared pointer to the Command.
 */
inline void sync(Components::Widget* widget,
                 std::shared_ptr<UI::BaseUICommand> command) {
    WidgetAdapter::establishSync(widget, command);
}

/**
 * @brief Creates a new HybridContainer for mixed Components and Commands.
 * @return Unique pointer to the HybridContainer.
 */
inline std::unique_ptr<ComponentSystemAdapter::HybridContainer> createHybrid() {
    return ComponentSystemAdapter::instance().createHybridContainer();
}

}  // namespace ComponentCommand

}  // namespace DeclarativeUI::Command::Adapters
