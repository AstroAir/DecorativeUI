#include "UICommandFactory.hpp"
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <algorithm>
#include "CoreCommands.hpp"
#include "SpecializedCommands.hpp"

namespace DeclarativeUI::Command::UI {

UICommandFactory::UICommandFactory(QObject* parent) : QObject(parent) {
    setupBuiltinCommands();
    qDebug() << "🏭 UICommandFactory initialized";
}

UICommandFactory& UICommandFactory::instance() {
    static UICommandFactory instance;
    return instance;
}

void UICommandFactory::registerCommand(const QString& command_type,
                                       const CommandRegistrationInfo& info) {
    validateRegistration(command_type, info);
    registrations_[command_type] = info;
    emit commandRegistered(command_type);
    qDebug() << "📝 Registered command type:" << command_type;
}

void UICommandFactory::registerCommand(const QString& command_type,
                                       CommandFactory factory,
                                       const UICommandMetadata& metadata) {
    CommandRegistrationInfo info;
    info.command_type = command_type;
    info.factory = std::move(factory);
    info.metadata = metadata;
    info.widget_type = metadata.widget_type;

    registerCommand(command_type, info);
}

std::shared_ptr<BaseUICommand> UICommandFactory::createCommand(
    const QString& command_type) {
    return createCommandInternal(command_type);
}

std::shared_ptr<BaseUICommand> UICommandFactory::createCommand(
    const QString& command_type, const QJsonObject& config) {
    return createCommandInternal(command_type, config);
}

std::pair<std::shared_ptr<BaseUICommand>, std::unique_ptr<QWidget>>
UICommandFactory::createCommandWithWidget(const QString& command_type) {
    return createCommandWithWidget(command_type, QJsonObject{});
}

std::pair<std::shared_ptr<BaseUICommand>, std::unique_ptr<QWidget>>
UICommandFactory::createCommandWithWidget(const QString& command_type,
                                          const QJsonObject& config) {
    auto command = createCommandInternal(command_type, config);
    if (!command) {
        return {nullptr, nullptr};
    }

    auto widget = WidgetMapper::instance().createWidget(command.get());
    if (!widget) {
        qWarning() << "Failed to create widget for command:" << command_type;
        return {command, nullptr};
    }

    return {command, std::move(widget)};
}

std::vector<std::shared_ptr<BaseUICommand>> UICommandFactory::createCommands(
    const QStringList& command_types) {
    std::vector<std::shared_ptr<BaseUICommand>> commands;
    commands.reserve(command_types.size());

    for (const QString& type : command_types) {
        auto command = createCommand(type);
        if (command) {
            commands.push_back(command);
        } else {
            qWarning() << "Failed to create command of type:" << type;
        }
    }

    return commands;
}

std::vector<std::shared_ptr<BaseUICommand>>
UICommandFactory::createCommandsFromJson(const QJsonArray& commands) {
    std::vector<std::shared_ptr<BaseUICommand>> result;
    result.reserve(commands.size());

    for (const auto& value : commands) {
        if (!value.isObject()) {
            qWarning() << "Invalid command configuration: not an object";
            continue;
        }

        QJsonObject config = value.toObject();
        if (!config.contains("type")) {
            qWarning() << "Command configuration missing 'type' field";
            continue;
        }

        QString type = config["type"].toString();
        auto command = createCommand(type, config);
        if (command) {
            result.push_back(command);
        }
    }

    return result;
}

std::shared_ptr<BaseUICommand> UICommandFactory::createCommandHierarchy(
    const QJsonObject& hierarchy) {
    if (!hierarchy.contains("type")) {
        qWarning() << "Hierarchy root missing 'type' field";
        return nullptr;
    }

    QString root_type = hierarchy["type"].toString();
    auto root_command = createCommand(root_type, hierarchy);
    if (!root_command) {
        return nullptr;
    }

    // Process children if they exist
    if (hierarchy.contains("children") && hierarchy["children"].isArray()) {
        QJsonArray children = hierarchy["children"].toArray();
        for (const auto& child_value : children) {
            if (child_value.isObject()) {
                auto child_command =
                    createCommandHierarchy(child_value.toObject());
                if (child_command) {
                    root_command->addChild(child_command);
                }
            }
        }
    }

    return root_command;
}

bool UICommandFactory::isRegistered(const QString& command_type) const {
    return registrations_.find(command_type) != registrations_.end();
}

QStringList UICommandFactory::getRegisteredTypes() const {
    QStringList types;
    for (const auto& [type, info] : registrations_) {
        types.append(type);
    }
    return types;
}

UICommandMetadata UICommandFactory::getMetadata(
    const QString& command_type) const {
    auto it = registrations_.find(command_type);
    return it != registrations_.end() ? it->second.metadata
                                      : UICommandMetadata{};
}

QString UICommandFactory::getWidgetType(const QString& command_type) const {
    auto it = registrations_.find(command_type);
    return it != registrations_.end() ? it->second.widget_type : QString{};
}

void UICommandFactory::setDefaultProperties(const QString& command_type,
                                            const QJsonObject& properties) {
    default_properties_[command_type] = properties;
}

QJsonObject UICommandFactory::getDefaultProperties(
    const QString& command_type) const {
    auto it = default_properties_.find(command_type);
    return it != default_properties_.end() ? it->second : QJsonObject{};
}

bool UICommandFactory::validateCommandType(const QString& command_type) const {
    return isRegistered(command_type);
}

QStringList UICommandFactory::validateCommandConfig(
    const QString& command_type, const QJsonObject& config) const {
    QStringList errors;

    if (!isRegistered(command_type)) {
        errors.append(
            QString("Command type '%1' is not registered").arg(command_type));
        return errors;
    }

    const auto metadata = getMetadata(command_type);

    // Check required properties
    for (const QString& required_prop : metadata.required_properties) {
        if (!config.contains(required_prop)) {
            errors.append(QString("Required property '%1' is missing")
                              .arg(required_prop));
        }
    }

    // Additional validation could be added here

    return errors;
}

void UICommandFactory::unregisterCommand(const QString& command_type) {
    auto it = registrations_.find(command_type);
    if (it != registrations_.end()) {
        registrations_.erase(it);
        default_properties_.erase(command_type);
        emit commandUnregistered(command_type);
        qDebug() << "🗑️ Unregistered command type:" << command_type;
    }
}

void UICommandFactory::clear() {
    registrations_.clear();
    default_properties_.clear();
    qDebug() << "🧹 Cleared all command registrations";
}

void UICommandFactory::setupBuiltinCommands() {
    qDebug() << "🔧 Setting up built-in UI commands";

    // Register core UI commands from CoreCommands
    registerCommand<ButtonCommand>("Button", "QPushButton");
    registerCommand<LabelCommand>("Label", "QLabel");
    registerCommand<TextInputCommand>("TextInput", "QLineEdit");
    registerCommand<ContainerCommand>("Container", "QWidget");

    // Register specialized UI commands from SpecializedCommands
    registerCommand<RadioButtonCommand>("RadioButton", "QRadioButton");
    registerCommand<CheckBoxCommand>("CheckBox", "QCheckBox");
    registerCommand<ToggleButtonCommand>("ToggleButton", "QPushButton");
    registerCommand<ToolButtonCommand>("ToolButton", "QToolButton");
    registerCommand<SliderCommand>("Slider", "QSlider");
    registerCommand<ProgressBarCommand>("ProgressBar", "QProgressBar");
    registerCommand<SpinBoxCommand>("SpinBox", "QSpinBox");

    qDebug() << "✅ Built-in UI commands setup complete";
}

void UICommandFactory::applyConfiguration(BaseUICommand* command,
                                          const QJsonObject& config) {
    if (!command) {
        return;
    }

    // Apply default properties first
    const QString command_type = command->getCommandType();
    QJsonObject defaults = getDefaultProperties(command_type);
    for (auto it = defaults.begin(); it != defaults.end(); ++it) {
        command->getState()->setProperty(it.key(), it.value().toVariant());
    }

    // Apply configuration properties
    for (auto it = config.begin(); it != config.end(); ++it) {
        const QString& key = it.key();
        if (key == "type" || key == "children") {
            continue;  // Skip special keys
        }

        command->getState()->setProperty(key, it.value().toVariant());
    }
}

void UICommandFactory::validateRegistration(
    const QString& command_type, const CommandRegistrationInfo& info) {
    if (command_type.isEmpty()) {
        throw std::invalid_argument("Command type cannot be empty");
    }

    if (!info.factory) {
        throw std::invalid_argument("Command factory cannot be null");
    }

    // Additional validation could be added here
}

std::shared_ptr<BaseUICommand> UICommandFactory::createCommandInternal(
    const QString& command_type, const QJsonObject& config) {
    auto it = registrations_.find(command_type);
    if (it == registrations_.end()) {
        QString error =
            QString("Command type '%1' is not registered").arg(command_type);
        emit commandCreationFailed(command_type, error);
        qWarning() << error;
        return nullptr;
    }

    try {
        auto command = it->second.factory();
        if (!command) {
            QString error =
                QString("Factory returned null for command type '%1'")
                    .arg(command_type);
            emit commandCreationFailed(command_type, error);
            qWarning() << error;
            return nullptr;
        }

        // Apply configuration
        applyConfiguration(command.get(), config);

        emit commandCreated(command);
        qDebug() << "✨ Created command:" << command_type;

        return command;

    } catch (const std::exception& e) {
        QString error = QString("Failed to create command '%1': %2")
                            .arg(command_type, e.what());
        emit commandCreationFailed(command_type, error);
        qWarning() << error;
        return nullptr;
    }
}

// CommandBuilder implementation is in CommandBuilder.cpp

}  // namespace DeclarativeUI::Command::UI
