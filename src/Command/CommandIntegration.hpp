#pragma once

#include "../Core/UIElement.hpp"
#include "BuiltinCommands.hpp"
#include "CommandSystem.hpp"

#include <QAction>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QKeySequence>
#include <QMenu>
#include <QMenuBar>
#include <QObject>
#include <QPushButton>
#include <QShortcut>
#include <QToolBar>

#include <functional>
#include <map>
#include <memory>

namespace DeclarativeUI::Command::Integration {

// **Command-enabled UI element base class**
class CommandableUIElement : public Core::UIElement {
    Q_OBJECT

public:
    explicit CommandableUIElement(QObject *parent = nullptr);
    CommandableUIElement &onCommand(const QString &command_name,
                                    const CommandContext &context = {});
    CommandableUIElement &onShortcut(const QKeySequence &shortcut,
                                     const QString &command_name,
                                     const CommandContext &context = {});
    virtual CommandResult<QVariant> executeCommand(
        const QString &command_name, const CommandContext &context = {});
    virtual QFuture<CommandResult<QVariant>> executeCommandAsync(
        const QString &command_name, const CommandContext &context = {});
    const std::unordered_map<QString, CommandContext> &getCommandBindings()
        const;

signals:
    void commandExecuted(const QString &command_name,
                         const CommandResult<QVariant> &result);
    void commandFailed(const QString &command_name, const QString &error);

protected:
    std::unordered_map<QString, CommandContext> command_bindings_;
    struct ShortcutBinding {
        QString command_name;
        CommandContext context;
    };
    std::map<QKeySequence, ShortcutBinding> shortcuts_;
    void connectCommandSignals();
};

// **Command-enabled button**
class CommandButton : public CommandableUIElement {
    Q_OBJECT

public:
    explicit CommandButton(QObject *parent = nullptr);
    CommandButton &text(const QString &text);
    CommandButton &onClick(const QString &command_name,
                           const CommandContext &context = {});
    CommandButton &icon(const QIcon &icon);
    CommandButton &tooltip(const QString &tooltip);
    CommandButton &enabled(bool enabled);
    void initialize() override;
    CommandMetadata getMetadata() const;

private:
    std::function<void()> onClick_;
};

// **Command-enabled menu system**
class CommandMenu : public QObject {
    Q_OBJECT

public:
    explicit CommandMenu(QObject *parent = nullptr);
    CommandMenu &addAction(const QString &text, const QString &command_name,
                           const CommandContext &context = {});
    CommandMenu &addSeparator();
    CommandMenu &addSubmenu(const QString &title,
                            std::function<void(CommandMenu &)> configure);
    CommandMenu &setShortcut(const QKeySequence &shortcut);
    CommandMenu &setIcon(const QIcon &icon);
    QMenu *buildMenu(QWidget *parent = nullptr);
    QMenuBar *buildMenuBar(QWidget *parent = nullptr);
    CommandResult<QVariant> executeCommand(const QString &command_name,
                                           const CommandContext &context = {});

signals:
    void commandExecuted(const QString &command_name,
                         const CommandResult<QVariant> &result);
    void commandFailed(const QString &command_name, const QString &error);

private:
    struct ActionBinding {
        QAction *action;
        QString command_name;
        CommandContext context;
    };
    std::vector<ActionBinding> actions_;
    std::vector<size_t> separators_;
    std::unordered_map<QString, std::unique_ptr<CommandMenu>> submenus_;
};

// **Command toolbar**
class CommandToolBar : public QObject {
    Q_OBJECT

public:
    explicit CommandToolBar(QObject *parent = nullptr);
    CommandToolBar &addButton(const QString &text, const QString &command_name,
                              const CommandContext &context = {});
    CommandToolBar &setIcon(const QIcon &icon);
    CommandToolBar &setToolTip(const QString &tooltip);
    CommandToolBar &addSeparator();
    QToolBar *buildToolBar(QWidget *parent = nullptr);
    CommandResult<QVariant> executeCommand(const QString &command_name,
                                           const CommandContext &context = {});

signals:
    void commandExecuted(const QString &command_name,
                         const CommandResult<QVariant> &result);
    void commandFailed(const QString &command_name, const QString &error);

private:
    struct ButtonInfo {
        QString text;
        QString command_name;
        CommandContext context;
        QIcon icon;
        QString tooltip;
    };
    std::vector<ButtonInfo> buttons_;
    std::vector<size_t> separators_;
};

// **Command configuration from JSON**
class CommandConfigurationLoader {
public:
    static CommandConfigurationLoader &instance();
    bool loadConfiguration(const QString &json_content);
    const std::vector<std::unique_ptr<CommandMenu>> &getMenus() const;
    const std::vector<std::unique_ptr<CommandToolBar>> &getToolbars() const;

private:
    CommandConfigurationLoader();
    std::vector<std::unique_ptr<CommandMenu>> menus_;
    std::vector<std::unique_ptr<CommandToolBar>> toolbars_;
    void loadCommands(const QJsonObject &commands);
    void loadShortcuts(const QJsonObject &shortcuts);
    void loadMenus(const QJsonArray &menus);
    void loadMenuItems(CommandMenu *menu, const QJsonArray &items);
    void loadToolbars(const QJsonArray &toolbars);
};

// **Command interceptor for UI integration**
class UICommandInterceptor : public CommandInterceptor {
    Q_OBJECT

public:
    explicit UICommandInterceptor(QObject *parent = nullptr);
    bool beforeExecute(ICommand *command,
                       const CommandContext &context) override;
    void afterExecute(ICommand *command, const CommandContext &context,
                      const CommandResult<QVariant> &result) override;
    void onError(ICommand *command, const CommandContext &context,
                 const CommandError &error) override;

signals:
    void commandStarted(const QString &command_name);
    void commandCompleted(const QString &command_name, bool success);
    void commandError(const QString &command_name, const QString &error);
};

// **Utility functions for command integration**
namespace Utils {

// **Create command context from widget properties**
inline CommandContext createContextFromWidget(QWidget *widget) {
    CommandContext context;

    // Extract relevant properties
    context.setParameter("widget_name", widget->objectName());
    context.setParameter("widget_class", widget->metaObject()->className());
    context.setParameter("enabled", widget->isEnabled());
    context.setParameter("visible", widget->isVisible());

    return context;
}

// **Create command context from JSON**
inline CommandContext createContextFromJson(const QString &json_string) {
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json_string.toUtf8(), &error);

    if (error.error != QJsonParseError::NoError) {
        return CommandContext{};
    }

    return CommandContext(doc.object());
}

// **Initialize command system with built-in commands**
inline void initializeCommandSystem() {
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
