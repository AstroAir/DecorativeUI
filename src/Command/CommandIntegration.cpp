#include "CommandIntegration.hpp"
#include <QApplication>
#include <QDebug>

namespace DeclarativeUI::Command::Integration {

using namespace DeclarativeUI::Command::Integration;

// CommandableUIElement
CommandableUIElement::CommandableUIElement(QObject *parent)
    : UIElement(parent) {}

CommandableUIElement &CommandableUIElement::onCommand(
    const QString &command_name, const CommandContext &context) {
    command_bindings_[command_name] = context;
    return *this;
}

CommandableUIElement &CommandableUIElement::onShortcut(
    const QKeySequence &shortcut, const QString &command_name,
    const CommandContext &context) {
    if (!widget_) {
        throw std::runtime_error(
            "Widget must be created before adding shortcuts");
    }
    auto *shortcut_obj = new QShortcut(shortcut, widget_.get());
    connect(shortcut_obj, &QShortcut::activated, this,
            [this, command_name, context]() {
                executeCommand(command_name, context);
            });
    shortcuts_[shortcut] = {command_name, context};
    return *this;
}

CommandResult<QVariant> CommandableUIElement::executeCommand(
    const QString &command_name, const CommandContext &context) {
    auto &manager = CommandManager::instance();
    return manager.getInvoker().execute(command_name, context);
}

QFuture<CommandResult<QVariant>> CommandableUIElement::executeCommandAsync(
    const QString &command_name, const CommandContext &context) {
    auto &manager = CommandManager::instance();
    return manager.getInvoker().executeAsync(command_name, context);
}

const std::unordered_map<QString, CommandContext> &
CommandableUIElement::getCommandBindings() const {
    return command_bindings_;
}

void CommandableUIElement::connectCommandSignals() {
    auto &manager = CommandManager::instance();
    connect(&manager.getInvoker(), &CommandInvoker::commandExecuted, this,
            &CommandableUIElement::commandExecuted);
    connect(&manager.getInvoker(), &CommandInvoker::commandFailed, this,
            [this](const QString &command_name, const CommandError &error) {
                emit commandFailed(command_name, error.getMessage());
            });
}

// CommandButton
CommandButton::CommandButton(QObject *parent) : CommandableUIElement(parent) {}

CommandButton &CommandButton::text(const QString &text) {
    return static_cast<CommandButton &>(setProperty("text", text));
}

CommandButton &CommandButton::onClick(const QString &command_name,
                                      const CommandContext &context) {
    onClick_ = [this, command_name, context]() {
        executeCommand(command_name, context);
    };
    return *this;
}

CommandButton &CommandButton::icon(const QIcon &icon) {
    return static_cast<CommandButton &>(setProperty("icon", icon));
}

CommandButton &CommandButton::tooltip(const QString &tooltip) {
    return static_cast<CommandButton &>(setProperty("toolTip", tooltip));
}

CommandButton &CommandButton::enabled(bool enabled) {
    return static_cast<CommandButton &>(setProperty("enabled", enabled));
}

void CommandButton::initialize() {
    if (!widget_) {
        auto button = std::make_unique<QPushButton>();
        setWidget(button.release());
        if (onClick_) {
            connect(static_cast<QPushButton *>(widget_.get()),
                    &QPushButton::clicked, this, onClick_);
        }
        applyStoredProperties();
        connectCommandSignals();
    }
}

CommandMetadata CommandButton::getMetadata() const {
    return CommandMetadata("CommandButton", "Command-enabled button widget");
}

// CommandMenu
CommandMenu::CommandMenu(QObject *parent) : QObject(parent) {}

CommandMenu &CommandMenu::addAction(const QString &text,
                                    const QString &command_name,
                                    const CommandContext &context) {
    auto *action = new QAction(text, this);
    connect(action, &QAction::triggered, this, [this, command_name, context]() {
        executeCommand(command_name, context);
    });
    actions_.push_back({action, command_name, context});
    return *this;
}

CommandMenu &CommandMenu::addSeparator() {
    separators_.push_back(actions_.size());
    return *this;
}

CommandMenu &CommandMenu::addSubmenu(
    const QString &title, std::function<void(CommandMenu &)> configure) {
    auto submenu = std::make_unique<CommandMenu>(this);
    configure(*submenu);
    submenus_[title] = std::move(submenu);
    return *this;
}

CommandMenu &CommandMenu::setShortcut(const QKeySequence &shortcut) {
    if (!actions_.empty()) {
        actions_.back().action->setShortcut(shortcut);
    }
    return *this;
}

CommandMenu &CommandMenu::setIcon(const QIcon &icon) {
    if (!actions_.empty()) {
        actions_.back().action->setIcon(icon);
    }
    return *this;
}

QMenu *CommandMenu::buildMenu(QWidget *parent) {
    auto *menu = new QMenu(parent);
    for (size_t i = 0; i < actions_.size(); ++i) {
        if (std::find(separators_.begin(), separators_.end(), i) !=
            separators_.end()) {
            menu->addSeparator();
        }
        menu->addAction(actions_[i].action);
    }
    for (const auto &[title, submenu] : submenus_) {
        auto *sub_menu = submenu->buildMenu(menu);
        sub_menu->setTitle(title);
        menu->addMenu(sub_menu);
    }
    return menu;
}

QMenuBar *CommandMenu::buildMenuBar(QWidget *parent) {
    auto *menuBar = new QMenuBar(parent);
    for (const auto &[title, submenu] : submenus_) {
        auto *menu = submenu->buildMenu(menuBar);
        menu->setTitle(title);
        menuBar->addMenu(menu);
    }
    return menuBar;
}

CommandResult<QVariant> CommandMenu::executeCommand(
    const QString &command_name, const CommandContext &context) {
    auto &manager = CommandManager::instance();
    return manager.getInvoker().execute(command_name, context);
}

// CommandToolBar
CommandToolBar::CommandToolBar(QObject *parent) : QObject(parent) {}

CommandToolBar &CommandToolBar::addButton(const QString &text,
                                          const QString &command_name,
                                          const CommandContext &context) {
    ButtonInfo info;
    info.text = text;
    info.command_name = command_name;
    info.context = context;
    buttons_.push_back(info);
    return *this;
}

CommandToolBar &CommandToolBar::setIcon(const QIcon &icon) {
    if (!buttons_.empty()) {
        buttons_.back().icon = icon;
    }
    return *this;
}

CommandToolBar &CommandToolBar::setToolTip(const QString &tooltip) {
    if (!buttons_.empty()) {
        buttons_.back().tooltip = tooltip;
    }
    return *this;
}

CommandToolBar &CommandToolBar::addSeparator() {
    separators_.push_back(buttons_.size());
    return *this;
}

QToolBar *CommandToolBar::buildToolBar(QWidget *parent) {
    auto *toolbar = new QToolBar(parent);
    for (size_t i = 0; i < buttons_.size(); ++i) {
        if (std::find(separators_.begin(), separators_.end(), i) !=
            separators_.end()) {
            toolbar->addSeparator();
        }
        const auto &button = buttons_[i];
        auto *action = toolbar->addAction(button.icon, button.text);
        action->setToolTip(button.tooltip);
        connect(action, &QAction::triggered, this, [this, button]() {
            executeCommand(button.command_name, button.context);
        });
    }
    return toolbar;
}

CommandResult<QVariant> CommandToolBar::executeCommand(
    const QString &command_name, const CommandContext &context) {
    auto &manager = CommandManager::instance();
    return manager.getInvoker().execute(command_name, context);
}

// CommandConfigurationLoader
CommandConfigurationLoader::CommandConfigurationLoader() = default;

CommandConfigurationLoader &CommandConfigurationLoader::instance() {
    static CommandConfigurationLoader instance;
    return instance;
}

bool CommandConfigurationLoader::loadConfiguration(
    const QString &json_content) {
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json_content.toUtf8(), &error);
    if (error.error != QJsonParseError::NoError) {
        qDebug() << "JSON parse error:" << error.errorString();
        return false;
    }
    QJsonObject root = doc.object();
    if (root.contains("commands")) {
        loadCommands(root["commands"].toObject());
    }
    if (root.contains("shortcuts")) {
        loadShortcuts(root["shortcuts"].toObject());
    }
    if (root.contains("menus")) {
        loadMenus(root["menus"].toArray());
    }
    if (root.contains("toolbars")) {
        loadToolbars(root["toolbars"].toArray());
    }
    return true;
}

const std::vector<std::unique_ptr<CommandMenu>> &
CommandConfigurationLoader::getMenus() const {
    return menus_;
}

const std::vector<std::unique_ptr<CommandToolBar>> &
CommandConfigurationLoader::getToolbars() const {
    return toolbars_;
}

void CommandConfigurationLoader::loadCommands(const QJsonObject &commands) {
    for (auto it = commands.begin(); it != commands.end(); ++it) {
        QString command_name = it.key();
        QJsonObject command_config = it.value().toObject();
        // Store command configuration for later use
        // This could be used to configure command metadata, etc.
    }
}

void CommandConfigurationLoader::loadShortcuts(const QJsonObject &shortcuts) {
    for (auto it = shortcuts.begin(); it != shortcuts.end(); ++it) {
        QString shortcut_key = it.key();
        QJsonObject shortcut_config = it.value().toObject();
        QString command_name = shortcut_config["command"].toString();
        QJsonObject context_json = shortcut_config["context"].toObject();
        CommandContext context(context_json);
        // Register global shortcut
        // This would need to be implemented with QShortcut
    }
}

void CommandConfigurationLoader::loadMenus(const QJsonArray &menus) {
    for (const auto &menu_value : menus) {
        QJsonObject menu_obj = menu_value.toObject();
        auto menu = std::make_unique<CommandMenu>();
        loadMenuItems(menu.get(), menu_obj["items"].toArray());
        menus_.push_back(std::move(menu));
    }
}

void CommandConfigurationLoader::loadMenuItems(CommandMenu *menu,
                                               const QJsonArray &items) {
    for (const auto &item_value : items) {
        QJsonObject item = item_value.toObject();
        QString type = item["type"].toString();
        if (type == "action") {
            QString text = item["text"].toString();
            QString command = item["command"].toString();
            QJsonObject context_json = item["context"].toObject();
            CommandContext context(context_json);
            menu->addAction(text, command, context);
            if (item.contains("shortcut")) {
                QKeySequence shortcut(item["shortcut"].toString());
                menu->setShortcut(shortcut);
            }
            if (item.contains("icon")) {
                // QIcon icon(item["icon"].toString());
                // menu->setIcon(icon);
            }
        } else if (type == "separator") {
            menu->addSeparator();
        } else if (type == "submenu") {
            QString title = item["title"].toString();
            QJsonArray submenu_items = item["items"].toArray();
            menu->addSubmenu(title,
                             [this, submenu_items](CommandMenu &submenu) {
                                 loadMenuItems(&submenu, submenu_items);
                             });
        }
    }
}

void CommandConfigurationLoader::loadToolbars(const QJsonArray &toolbars) {
    for (const auto &toolbar_value : toolbars) {
        QJsonObject toolbar_obj = toolbar_value.toObject();
        auto toolbar = std::make_unique<CommandToolBar>();
        QJsonArray items = toolbar_obj["items"].toArray();
        for (const auto &item_value : items) {
            QJsonObject item = item_value.toObject();
            QString type = item["type"].toString();
            if (type == "button") {
                QString text = item["text"].toString();
                QString command = item["command"].toString();
                QJsonObject context_json = item["context"].toObject();
                CommandContext context(context_json);
                toolbar->addButton(text, command, context);
                if (item.contains("tooltip")) {
                    toolbar->setToolTip(item["tooltip"].toString());
                }
                if (item.contains("icon")) {
                    // QIcon icon(item["icon"].toString());
                    // toolbar->setIcon(icon);
                }
            } else if (type == "separator") {
                toolbar->addSeparator();
            }
        }
        toolbars_.push_back(std::move(toolbar));
    }
}

// UICommandInterceptor
UICommandInterceptor::UICommandInterceptor(QObject *parent)
    : CommandInterceptor(parent) {}

bool UICommandInterceptor::beforeExecute(ICommand *command,
                                         const CommandContext &context) {
    qDebug() << "Executing command:" << command->getMetadata().name;
    emit commandStarted(command->getMetadata().name);
    return true;
}

void UICommandInterceptor::afterExecute(ICommand *command,
                                        const CommandContext &context,
                                        const CommandResult<QVariant> &result) {
    qDebug() << "Command completed:" << command->getMetadata().name;
    emit commandCompleted(command->getMetadata().name, result.isSuccess());
}

void UICommandInterceptor::onError(ICommand *command,
                                   const CommandContext &context,
                                   const CommandError &error) {
    qDebug() << "Command error:" << command->getMetadata().name << "-"
             << error.getMessage();
    emit commandError(command->getMetadata().name, error.getMessage());
}

// **Utility functions implementations**
namespace Utils {

// **Create command context from widget properties**
CommandContext createContextFromWidget(QWidget *widget) {
    CommandContext context;

    // Extract relevant properties
    context.setParameter("widget_name", widget->objectName());
    context.setParameter("widget_class", widget->metaObject()->className());
    context.setParameter("enabled", widget->isEnabled());
    context.setParameter("visible", widget->isVisible());

    return context;
}

// **Create command context from JSON**
CommandContext createContextFromJson(const QString &json_string) {
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json_string.toUtf8(), &error);

    if (error.error != QJsonParseError::NoError) {
        return CommandContext{};
    }

    return CommandContext(doc.object());
}

// **Initialize command system with built-in commands**
void initializeCommandSystem() {
    // Register built-in commands
    Commands::registerBuiltinCommands();

    // Add UI interceptor
    auto interceptor = std::make_unique<UICommandInterceptor>();
    CommandManager::instance().addInterceptor(std::move(interceptor));

    // Enable command history
    CommandManager::instance().enableCommandHistory(true);

    // Enable audit trail
    CommandManager::instance().enableAuditTrail(true);
}

}  // namespace Utils

}  // namespace DeclarativeUI::Command::Integration
