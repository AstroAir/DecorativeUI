#pragma once

#include <QObject>
#include <QString>
#include <QVariant>
#include <QWidget>
#include <memory>
#include <functional>
#include <unordered_map>

#include "../UICommand.hpp"
#include "../UICommandFactory.hpp"
#include "../WidgetMapper.hpp"
#include "../CommandEvents.hpp"
#include "../../Core/UIElement.hpp"
#include "../../Components/Button.hpp"
#include "../../Components/Widget.hpp"

namespace DeclarativeUI::Command::Adapters {

/**
 * @brief Adapter that wraps existing UIElement components to work with the Command system
 * 
 * This adapter allows existing UIElement-based components to be used seamlessly
 * with the new Command architecture while maintaining backward compatibility.
 */
class UIElementCommandAdapter : public UI::BaseUICommand {
    Q_OBJECT
    
public:
    explicit UIElementCommandAdapter(std::unique_ptr<Core::UIElement> element, QObject* parent = nullptr);
    virtual ~UIElementCommandAdapter() = default;
    
    // **BaseUICommand interface implementation**
    UI::UICommandMetadata getMetadata() const override;
    QString getCommandType() const override;
    QString getWidgetType() const override;
    
    // **UIElement access**
    Core::UIElement* getUIElement() const { return ui_element_.get(); }
    
    // **Widget lifecycle overrides**
    void onWidgetCreated(QWidget* widget) override;
    void onWidgetDestroyed() override;
    void syncToWidget() override;
    void syncFromWidget() override;
    
    // **Event handling**
    void handleEvent(const QString& eventType, const QVariant& eventData = QVariant{}) override;
    
    // **Property synchronization**
    void syncUIElementToCommand();
    void syncCommandToUIElement();
    
protected:
    std::unique_ptr<Core::UIElement> ui_element_;
    QString command_type_;
    QString widget_type_;
    
private:
    void setupPropertySynchronization();
    void setupEventForwarding();
    void connectUIElementSignals();
    void disconnectUIElementSignals();
    
    std::vector<QMetaObject::Connection> connections_;
};

/**
 * @brief Factory for creating Command adapters from existing UIElement components
 */
class UIElementAdapterFactory {
public:
    // **Create adapter from UIElement**
    static std::shared_ptr<UIElementCommandAdapter> createAdapter(std::unique_ptr<Core::UIElement> element);
    
    // **Create adapter for specific component types**
    static std::shared_ptr<UIElementCommandAdapter> createButtonAdapter();
    static std::shared_ptr<UIElementCommandAdapter> createWidgetAdapter();
    
    // **Register adapter for automatic creation**
    template<typename UIElementType>
    static void registerAdapter(const QString& commandType);
    
    // **Create adapter from type name**
    static std::shared_ptr<UIElementCommandAdapter> createAdapterByType(const QString& elementType);
    
private:
    static std::unordered_map<QString, std::function<std::unique_ptr<Core::UIElement>()>> element_factories_;
};

/**
 * @brief Reverse adapter that creates UIElement from Command
 * 
 * This allows Command objects to be used in existing UIElement-based code.
 */
class CommandUIElementAdapter : public Core::UIElement {
    Q_OBJECT
    
public:
    explicit CommandUIElementAdapter(std::shared_ptr<UI::BaseUICommand> command, QObject* parent = nullptr);
    virtual ~CommandUIElementAdapter() = default;
    
    // **UIElement interface implementation**
    void initialize() override;
    void cleanup() noexcept override;
    void refresh() override;
    
    // **Command access**
    std::shared_ptr<UI::BaseUICommand> getCommand() const { return command_; }
    
    // **Property forwarding to command**
    template<typename T>
    CommandUIElementAdapter& setCommandProperty(const QString& name, T&& value);
    
    template<typename T>
    T getCommandProperty(const QString& name, const T& defaultValue = T{}) const;

public slots:
    void syncUIElementToCommand();

protected:
    std::shared_ptr<UI::BaseUICommand> command_;
    
private:
    void setupCommandIntegration();
    void cleanupCommandIntegration();
    void syncPropertiesToCommand();
    void syncPropertiesFromCommand();
    
private slots:
    void onCommandStateChanged();
    void onCommandEventTriggered(const QString& eventType, const QVariant& eventData);
};

/**
 * @brief Migration helper for converting existing UIElement code to Command system
 */
class UIElementMigrationHelper {
public:
    // **Convert existing UIElement hierarchy to Command hierarchy**
    static std::shared_ptr<UI::BaseUICommand> convertToCommandHierarchy(Core::UIElement* rootElement);
    
    // **Convert Command hierarchy to UIElement hierarchy**
    static std::unique_ptr<Core::UIElement> convertToUIElementHierarchy(std::shared_ptr<UI::BaseUICommand> rootCommand);
    
    // **Migrate property bindings**
    static void migratePropertyBindings(Core::UIElement* element, std::shared_ptr<UI::BaseUICommand> command);
    
    // **Migrate event handlers**
    static void migrateEventHandlers(Core::UIElement* element, std::shared_ptr<UI::BaseUICommand> command);
    
    // **Generate migration report**
    static QString generateMigrationReport(Core::UIElement* element);
    
    // **Validate migration**
    static bool validateMigration(Core::UIElement* original, std::shared_ptr<UI::BaseUICommand> migrated);
};

/**
 * @brief Compatibility layer for existing Components
 */
class ComponentCommandBridge {
public:
    // **Register existing component types with Command system**
    static void registerComponentMappings();
    
    // **Create Command from Component type**
    static std::shared_ptr<UI::BaseUICommand> createCommandFromComponent(const QString& componentType);
    
    // **Create Component from Command**
    static std::unique_ptr<Core::UIElement> createComponentFromCommand(std::shared_ptr<UI::BaseUICommand> command);
    
    // **Bidirectional synchronization**
    static void establishBidirectionalSync(Core::UIElement* component, std::shared_ptr<UI::BaseUICommand> command);
    
private:
    static std::unordered_map<QString, QString> component_to_command_mapping_;
    static std::unordered_map<QString, QString> command_to_component_mapping_;
};

/**
 * @brief Enhanced Button adapter with full Command integration
 */
class ButtonCommandAdapter : public UIElementCommandAdapter {
    Q_OBJECT
    
public:
    explicit ButtonCommandAdapter(QObject* parent = nullptr);
    
    // **Button-specific interface**
    ButtonCommandAdapter& text(const QString& text);
    ButtonCommandAdapter& icon(const QString& iconPath);
    ButtonCommandAdapter& onClick(std::function<void()> handler);
    ButtonCommandAdapter& enabled(bool enabled);
    
    // **Override metadata for button-specific information**
    UI::UICommandMetadata getMetadata() const override;
    QString getCommandType() const override { return "AdaptedButton"; }
    QString getWidgetType() const override { return "QPushButton"; }
    
protected:
    void setupButtonSpecificBindings();
    
private:
    std::function<void()> click_handler_;
};

/**
 * @brief Generic Widget adapter for any QWidget-based component
 */
class WidgetCommandAdapter : public UIElementCommandAdapter {
    Q_OBJECT
    
public:
    explicit WidgetCommandAdapter(QObject* parent = nullptr);
    
    // **Generic widget interface**
    WidgetCommandAdapter& size(const QSize& size);
    WidgetCommandAdapter& position(const QPoint& position);
    WidgetCommandAdapter& visible(bool visible);
    WidgetCommandAdapter& enabled(bool enabled);
    WidgetCommandAdapter& styleSheet(const QString& style);
    
    // **Override metadata for widget-specific information**
    UI::UICommandMetadata getMetadata() const override;
    QString getCommandType() const override { return "AdaptedWidget"; }
    QString getWidgetType() const override { return "QWidget"; }
    
protected:
    void setupWidgetSpecificBindings();
};

// **Template implementations**
template<typename T>
CommandUIElementAdapter& CommandUIElementAdapter::setCommandProperty(const QString& name, T&& value) {
    if (command_) {
        command_->getState()->setProperty(name, QVariant::fromValue(std::forward<T>(value)));
        syncPropertiesToCommand();
    }
    return *this;
}

template<typename T>
T CommandUIElementAdapter::getCommandProperty(const QString& name, const T& defaultValue) const {
    if (command_) {
        return command_->getState()->getProperty<T>(name, defaultValue);
    }
    return defaultValue;
}

template<typename UIElementType>
void UIElementAdapterFactory::registerAdapter(const QString& commandType) {
    static_assert(std::is_base_of_v<Core::UIElement, UIElementType>, 
                  "UIElementType must inherit from Core::UIElement");
    
    element_factories_[commandType] = []() -> std::unique_ptr<Core::UIElement> {
        return std::make_unique<UIElementType>();
    };
}

// **Convenience functions for easy migration**
namespace Migration {

// **Quick adapter creation**
inline std::shared_ptr<UIElementCommandAdapter> adaptButton() {
    return UIElementAdapterFactory::createButtonAdapter();
}

inline std::shared_ptr<UIElementCommandAdapter> adaptWidget() {
    return UIElementAdapterFactory::createWidgetAdapter();
}

// **Quick conversion**
inline std::shared_ptr<UI::BaseUICommand> toCommand(std::unique_ptr<Core::UIElement> element) {
    return UIElementAdapterFactory::createAdapter(std::move(element));
}

inline std::unique_ptr<Core::UIElement> toUIElement(std::shared_ptr<UI::BaseUICommand> command) {
    return std::make_unique<CommandUIElementAdapter>(command);
}

// **Bidirectional bridge**
inline void bridge(Core::UIElement* element, std::shared_ptr<UI::BaseUICommand> command) {
    ComponentCommandBridge::establishBidirectionalSync(element, command);
}

}  // namespace Migration

}  // namespace DeclarativeUI::Command::Adapters
