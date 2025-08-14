#pragma once

#include <QAction>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QPushButton>
#include "UICommand.hpp"
#include "UICommandFactory.hpp"
#include "WidgetMapper.hpp"

namespace DeclarativeUI::Command::UI {

// **Button Command - represents button controls without QWidget inheritance**
class ButtonCommand : public BaseUICommand {
    Q_OBJECT

public:
    explicit ButtonCommand(QObject* parent = nullptr);
    virtual ~ButtonCommand() = default;

    // **BaseUICommand interface**
    UICommandMetadata getMetadata() const override;
    QString getCommandType() const override { return "Button"; }
    QString getWidgetType() const override { return "QPushButton"; }

    // **Button-specific interface**
    ButtonCommand& setText(const QString& text);
    QString getText() const;

    ButtonCommand& setEnabled(bool enabled);
    bool isEnabled() const;

    ButtonCommand& setIcon(const QString& iconPath);
    QString getIcon() const;

    ButtonCommand& setToolTip(const QString& tooltip);
    QString getToolTip() const;

    ButtonCommand& setCheckable(bool checkable);
    bool isCheckable() const;

    ButtonCommand& setChecked(bool checked);
    bool isChecked() const;

    // **Event handling**
    ButtonCommand& onClick(std::function<void()> handler);
    ButtonCommand& onToggled(std::function<void(bool)> handler);

    // **Widget lifecycle overrides**
    void onWidgetCreated(QWidget* widget) override;
    void syncToWidget() override;
    void syncFromWidget() override;

signals:
    void clicked();
    void toggled(bool checked);

protected:
    void handleEvent(const QString& eventType,
                     const QVariant& eventData = QVariant{}) override;

private:
    std::function<void()> click_handler_;
    std::function<void(bool)> toggle_handler_;
};

// **Label Command - represents label/text display controls**
class LabelCommand : public BaseUICommand {
    Q_OBJECT

public:
    explicit LabelCommand(QObject* parent = nullptr);
    virtual ~LabelCommand() = default;

    // **BaseUICommand interface**
    UICommandMetadata getMetadata() const override;
    QString getCommandType() const override { return "Label"; }
    QString getWidgetType() const override { return "QLabel"; }

    // **Label-specific interface**
    LabelCommand& setText(const QString& text);
    QString getText() const;

    LabelCommand& setAlignment(int alignment);
    int getAlignment() const;

    LabelCommand& setWordWrap(bool wrap);
    bool getWordWrap() const;

    LabelCommand& setPixmap(const QString& pixmapPath);
    QString getPixmap() const;

    // **Widget lifecycle overrides**
    void onWidgetCreated(QWidget* widget) override;
    void syncToWidget() override;
};

// **Text Input Command - represents text input controls**
class TextInputCommand : public BaseUICommand {
    Q_OBJECT

public:
    explicit TextInputCommand(QObject* parent = nullptr);
    virtual ~TextInputCommand() = default;

    // **BaseUICommand interface**
    UICommandMetadata getMetadata() const override;
    QString getCommandType() const override { return "TextInput"; }
    QString getWidgetType() const override { return "QLineEdit"; }

    // **TextInput-specific interface**
    TextInputCommand& setText(const QString& text);
    QString getText() const;

    TextInputCommand& setPlaceholder(const QString& placeholder);
    QString getPlaceholder() const;

    TextInputCommand& setReadOnly(bool readOnly);
    bool isReadOnly() const;

    TextInputCommand& setMaxLength(int maxLength);
    int getMaxLength() const;

    TextInputCommand& setEchoMode(int mode);
    int getEchoMode() const;

    // **Event handling**
    TextInputCommand& onTextChanged(
        std::function<void(const QString&)> handler);
    TextInputCommand& onReturnPressed(std::function<void()> handler);
    TextInputCommand& onEditingFinished(std::function<void()> handler);

    // **Widget lifecycle overrides**
    void onWidgetCreated(QWidget* widget) override;
    void syncToWidget() override;
    void syncFromWidget() override;

signals:
    void textChanged(const QString& text);
    void returnPressed();
    void editingFinished();

protected:
    void handleEvent(const QString& eventType,
                     const QVariant& eventData = QVariant{}) override;

private:
    std::function<void(const QString&)> text_changed_handler_;
    std::function<void()> return_pressed_handler_;
    std::function<void()> editing_finished_handler_;
};

// **Menu Item Command - represents menu items and actions**
class MenuItemCommand : public BaseUICommand {
    Q_OBJECT

public:
    explicit MenuItemCommand(QObject* parent = nullptr);
    virtual ~MenuItemCommand() = default;

    // **BaseUICommand interface**
    UICommandMetadata getMetadata() const override;
    QString getCommandType() const override { return "MenuItem"; }
    QString getWidgetType() const override { return "QAction"; }

    // **MenuItem-specific interface**
    MenuItemCommand& setText(const QString& text);
    QString getText() const;

    MenuItemCommand& setShortcut(const QString& shortcut);
    QString getShortcut() const;

    MenuItemCommand& setIcon(const QString& iconPath);
    QString getIcon() const;

    MenuItemCommand& setCheckable(bool checkable);
    bool isCheckable() const;

    MenuItemCommand& setChecked(bool checked);
    bool isChecked() const;

    MenuItemCommand& setSeparator(bool separator);
    bool isSeparator() const;

    // **Event handling**
    MenuItemCommand& onTriggered(std::function<void()> handler);
    MenuItemCommand& onToggled(std::function<void(bool)> handler);

    // **Widget lifecycle overrides**
    void onWidgetCreated(QWidget* widget) override;
    void syncToWidget() override;
    void syncFromWidget() override;

    // **Special handling for QAction (not QWidget)**
    QAction* getAction() const;
    void setAction(QAction* action);

signals:
    void triggered();
    void toggled(bool checked);

protected:
    void handleEvent(const QString& eventType,
                     const QVariant& eventData = QVariant{}) override;

private:
    std::function<void()> triggered_handler_;
    std::function<void(bool)> toggled_handler_;
    QAction* action_ = nullptr;  // Special case for QAction
};

// **Container Command - represents container widgets that hold other commands**
class ContainerCommand : public BaseUICommand {
    Q_OBJECT

public:
    explicit ContainerCommand(QObject* parent = nullptr);
    virtual ~ContainerCommand() = default;

    // **BaseUICommand interface**
    UICommandMetadata getMetadata() const override;
    QString getCommandType() const override { return "Container"; }
    QString getWidgetType() const override { return "QWidget"; }

    // **Container-specific interface**
    ContainerCommand& setLayout(const QString& layoutType);
    QString getLayout() const;

    ContainerCommand& setSpacing(int spacing);
    int getSpacing() const;

    ContainerCommand& setMargins(int left, int top, int right, int bottom);
    ContainerCommand& setMargins(int margin);

    // **Child management (inherited from BaseUICommand but with
    // container-specific behavior)**
    ContainerCommand& addChild(std::shared_ptr<BaseUICommand> child);
    ContainerCommand& removeChild(std::shared_ptr<BaseUICommand> child);

    // **Widget lifecycle overrides**
    void onWidgetCreated(QWidget* widget) override;
    void syncToWidget() override;

private:
    void setupLayout();
    void addChildWidget(std::shared_ptr<BaseUICommand> child);
    void removeChildWidget(std::shared_ptr<BaseUICommand> child);
};

// **Registration function for core commands**
void registerCoreCommands();

}  // namespace DeclarativeUI::Command::UI

// **Automatic registration of core commands**
namespace {
struct CoreCommandRegistrar {
    CoreCommandRegistrar() {
        DeclarativeUI::Command::UI::registerCoreCommands();
    }
};
static CoreCommandRegistrar g_core_command_registrar;
}  // namespace
