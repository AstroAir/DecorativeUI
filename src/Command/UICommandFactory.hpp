#pragma once

#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QUuid>
#include <functional>
#include <memory>
#include <type_traits>
#include <unordered_map>

#include "UICommand.hpp"
#include "WidgetMapper.hpp"

namespace DeclarativeUI::Command::UI {

// **Command factory function type**
using CommandFactory = std::function<std::shared_ptr<BaseUICommand>()>;

// **Command registration info**
struct CommandRegistrationInfo {
    QString command_type;
    QString widget_type;
    CommandFactory factory;
    UICommandMetadata metadata;

    CommandRegistrationInfo() = default;
    CommandRegistrationInfo(const QString& cmd_type, const QString& widget_type,
                            CommandFactory fact,
                            const UICommandMetadata& meta = {})
        : command_type(cmd_type),
          widget_type(widget_type),
          factory(std::move(fact)),
          metadata(meta) {}
};

// **UI Command Factory - creates Command objects and manages their lifecycle**
class UICommandFactory : public QObject {
    Q_OBJECT

public:
    explicit UICommandFactory(QObject* parent = nullptr);
    virtual ~UICommandFactory() = default;

    // **Command registration**
    template <typename CommandType>
    void registerCommand(const QString& command_type,
                         const QString& widget_type = "");

    void registerCommand(const QString& command_type,
                         const CommandRegistrationInfo& info);
    void registerCommand(const QString& command_type, CommandFactory factory,
                         const UICommandMetadata& metadata = {});

    // **Command creation**
    std::shared_ptr<BaseUICommand> createCommand(const QString& command_type);
    std::shared_ptr<BaseUICommand> createCommand(const QString& command_type,
                                                 const QJsonObject& config);

    // **Command with widget creation**
    std::pair<std::shared_ptr<BaseUICommand>, std::unique_ptr<QWidget>>
    createCommandWithWidget(const QString& command_type);

    std::pair<std::shared_ptr<BaseUICommand>, std::unique_ptr<QWidget>>
    createCommandWithWidget(const QString& command_type,
                            const QJsonObject& config);

    // **Batch creation**
    std::vector<std::shared_ptr<BaseUICommand>> createCommands(
        const QStringList& command_types);
    std::vector<std::shared_ptr<BaseUICommand>> createCommandsFromJson(
        const QJsonArray& commands);

    // **Command hierarchy creation**
    std::shared_ptr<BaseUICommand> createCommandHierarchy(
        const QJsonObject& hierarchy);

    // **Query methods**
    bool isRegistered(const QString& command_type) const;
    QStringList getRegisteredTypes() const;
    UICommandMetadata getMetadata(const QString& command_type) const;
    QString getWidgetType(const QString& command_type) const;

    // **Configuration**
    void setDefaultProperties(const QString& command_type,
                              const QJsonObject& properties);
    QJsonObject getDefaultProperties(const QString& command_type) const;

    // **Validation**
    bool validateCommandType(const QString& command_type) const;
    QStringList validateCommandConfig(const QString& command_type,
                                      const QJsonObject& config) const;

    // **Singleton access**
    static UICommandFactory& instance();

    // **Cleanup**
    void unregisterCommand(const QString& command_type);
    void clear();

signals:
    void commandRegistered(const QString& command_type);
    void commandUnregistered(const QString& command_type);
    void commandCreated(std::shared_ptr<BaseUICommand> command);
    void commandCreationFailed(const QString& command_type,
                               const QString& error);

private:
    // **Registration storage**
    std::unordered_map<QString, CommandRegistrationInfo> registrations_;
    std::unordered_map<QString, QJsonObject> default_properties_;

    // **Helper methods**
    void setupBuiltinCommands();
    void applyConfiguration(BaseUICommand* command, const QJsonObject& config);
    void validateRegistration(const QString& command_type,
                              const CommandRegistrationInfo& info);

    std::shared_ptr<BaseUICommand> createCommandInternal(
        const QString& command_type, const QJsonObject& config = {});
};

// **Template implementation for command registration**
template <typename CommandType>
void UICommandFactory::registerCommand(const QString& command_type,
                                       const QString& widget_type) {
    static_assert(std::is_base_of_v<BaseUICommand, CommandType>,
                  "CommandType must inherit from BaseUICommand");

    CommandRegistrationInfo info;
    info.command_type = command_type;
    info.widget_type = widget_type.isEmpty()
                           ? CommandType::staticMetaObject.className()
                           : widget_type;
    info.factory = []() -> std::shared_ptr<BaseUICommand> {
        return std::make_shared<CommandType>();
    };

    // Create metadata from the command type
    auto temp_command = std::make_shared<CommandType>();
    info.metadata = temp_command->getMetadata();

    registerCommand(command_type, info);

    // Also register the widget mapping if widget type is specified
    if (!widget_type.isEmpty()) {
        // This would register the widget mapping - implementation depends on
        // specific widget types
        qDebug() << "📝 Registered command-widget mapping:" << command_type
                 << "->" << widget_type;
    }
}

// **Utility class for automatic command registration**
template <typename CommandType>
class CommandRegistrar {
public:
    explicit CommandRegistrar(const QString& command_type,
                              const QString& widget_type = "") {
        UICommandFactory::instance().registerCommand<CommandType>(command_type,
                                                                  widget_type);
    }
};

// **Macro for easy command registration**
#define REGISTER_UI_COMMAND(CommandType, widget_type)                 \
    static CommandRegistrar<CommandType> g_##CommandType##_registrar( \
        #CommandType, #widget_type)

// **Forward declaration - full definition in CommandBuilder.hpp**
class CommandBuilder;

}  // namespace DeclarativeUI::Command::UI
