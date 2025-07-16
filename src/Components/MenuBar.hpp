// Components/MenuBar.hpp
#pragma once

#include <QAction>
#include <QActionGroup>
#include <QFont>
#include <QIcon>
#include <QKeySequence>
#include <QMenu>
#include <QMenuBar>
#include <QPixmap>
#include <QStyle>
#include <QStyleOption>
#include <functional>
#include <unordered_map>


#include "../Core/UIElement.hpp"

namespace DeclarativeUI::Components {

/**
 * @brief A comprehensive menu bar component for application menus
 *
 * Features:
 * - Hierarchical menu structure
 * - Action groups and separators
 * - Keyboard shortcuts
 * - Icons and checkable items
 * - Context menus
 * - Custom styling
 * - Accessibility support
 */
class MenuBar : public Core::UIElement {
public:
    MenuBar() {
        widget_ = std::make_unique<QMenuBar>();
        setupWidget();
    }

    virtual ~MenuBar() = default;

    // **Fluent Interface for Menu Management**
    MenuBar& addMenu(const QString& title) {
        if (auto menuBar = qobject_cast<QMenuBar*>(widget_.get())) {
            QMenu* menu = menuBar->addMenu(title);
            current_menu_ = menu;
            menus_[title] = menu;
        }
        return *this;
    }

    MenuBar& addMenu(QMenu* menu) {
        if (auto menuBar = qobject_cast<QMenuBar*>(widget_.get())) {
            menuBar->addMenu(menu);
            current_menu_ = menu;
            menus_[menu->title()] = menu;
        }
        return *this;
    }

    MenuBar& addMenu(const QIcon& icon, const QString& title) {
        if (auto menuBar = qobject_cast<QMenuBar*>(widget_.get())) {
            QMenu* menu = menuBar->addMenu(icon, title);
            current_menu_ = menu;
            menus_[title] = menu;
        }
        return *this;
    }

    MenuBar& insertMenu(QAction* before, QMenu* menu) {
        if (auto menuBar = qobject_cast<QMenuBar*>(widget_.get())) {
            menuBar->insertMenu(before, menu);
            current_menu_ = menu;
            menus_[menu->title()] = menu;
        }
        return *this;
    }

    MenuBar& removeMenu(QMenu* menu) {
        if (auto menuBar = qobject_cast<QMenuBar*>(widget_.get())) {
            menuBar->removeAction(menu->menuAction());
            for (auto it = menus_.begin(); it != menus_.end(); ++it) {
                if (it->second == menu) {
                    menus_.erase(it);
                    break;
                }
            }
            if (current_menu_ == menu) {
                current_menu_ = nullptr;
            }
        }
        return *this;
    }

    MenuBar& setActiveMenu(const QString& title) {
        auto it = menus_.find(title);
        if (it != menus_.end()) {
            current_menu_ = it->second;
        }
        return *this;
    }

    // **Action Management**
    MenuBar& addAction(const QString& text,
                       std::function<void()> handler = nullptr) {
        if (current_menu_) {
            QAction* action = current_menu_->addAction(text);
            if (handler) {
                QObject::connect(action, &QAction::triggered, handler);
            }
            actions_[text] = action;
        }
        return *this;
    }

    MenuBar& addAction(const QIcon& icon, const QString& text,
                       std::function<void()> handler = nullptr) {
        if (current_menu_) {
            QAction* action = current_menu_->addAction(icon, text);
            if (handler) {
                QObject::connect(action, &QAction::triggered, handler);
            }
            actions_[text] = action;
        }
        return *this;
    }

    MenuBar& addAction(const QString& text, const QKeySequence& shortcut,
                       std::function<void()> handler = nullptr) {
        if (current_menu_) {
            QAction* action = current_menu_->addAction(text);
            action->setShortcut(shortcut);
            if (handler) {
                QObject::connect(action, &QAction::triggered, handler);
            }
            actions_[text] = action;
        }
        return *this;
    }

    MenuBar& addAction(const QIcon& icon, const QString& text,
                       const QKeySequence& shortcut,
                       std::function<void()> handler = nullptr) {
        if (current_menu_) {
            QAction* action = current_menu_->addAction(icon, text);
            action->setShortcut(shortcut);
            if (handler) {
                QObject::connect(action, &QAction::triggered, handler);
            }
            actions_[text] = action;
        }
        return *this;
    }

    MenuBar& addCheckableAction(const QString& text, bool checked = false,
                                std::function<void(bool)> handler = nullptr) {
        if (current_menu_) {
            QAction* action = current_menu_->addAction(text);
            action->setCheckable(true);
            action->setChecked(checked);
            if (handler) {
                QObject::connect(action, &QAction::toggled, handler);
            }
            actions_[text] = action;
        }
        return *this;
    }

    MenuBar& addSeparator() {
        if (current_menu_) {
            current_menu_->addSeparator();
        }
        return *this;
    }

    MenuBar& addSubMenu(const QString& title) {
        if (current_menu_) {
            QMenu* subMenu = current_menu_->addMenu(title);
            current_menu_ = subMenu;
            menus_[title] = subMenu;
        }
        return *this;
    }

    MenuBar& addSubMenu(const QIcon& icon, const QString& title) {
        if (current_menu_) {
            QMenu* subMenu = current_menu_->addMenu(icon, title);
            current_menu_ = subMenu;
            menus_[title] = subMenu;
        }
        return *this;
    }

    // **Action Groups**
    MenuBar& createActionGroup(const QString& groupName) {
        auto actionGroup = std::make_unique<QActionGroup>(widget_.get());
        current_action_group_ = actionGroup.get();
        action_groups_[groupName] = std::move(actionGroup);
        return *this;
    }

    MenuBar& addActionToGroup(const QString& groupName,
                              const QString& actionName) {
        auto groupIt = action_groups_.find(groupName);
        auto actionIt = actions_.find(actionName);
        if (groupIt != action_groups_.end() && actionIt != actions_.end()) {
            groupIt->second->addAction(actionIt->second);
        }
        return *this;
    }

    MenuBar& setActionGroupExclusive(const QString& groupName, bool exclusive) {
        auto it = action_groups_.find(groupName);
        if (it != action_groups_.end()) {
            it->second->setExclusive(exclusive);
        }
        return *this;
    }

    // **Action Configuration**
    MenuBar& setActionEnabled(const QString& actionName, bool enabled) {
        auto it = actions_.find(actionName);
        if (it != actions_.end()) {
            it->second->setEnabled(enabled);
        }
        return *this;
    }

    MenuBar& setActionVisible(const QString& actionName, bool visible) {
        auto it = actions_.find(actionName);
        if (it != actions_.end()) {
            it->second->setVisible(visible);
        }
        return *this;
    }

    MenuBar& setActionChecked(const QString& actionName, bool checked) {
        auto it = actions_.find(actionName);
        if (it != actions_.end()) {
            it->second->setChecked(checked);
        }
        return *this;
    }

    MenuBar& setActionIcon(const QString& actionName, const QIcon& icon) {
        auto it = actions_.find(actionName);
        if (it != actions_.end()) {
            it->second->setIcon(icon);
        }
        return *this;
    }

    MenuBar& setActionShortcut(const QString& actionName,
                               const QKeySequence& shortcut) {
        auto it = actions_.find(actionName);
        if (it != actions_.end()) {
            it->second->setShortcut(shortcut);
        }
        return *this;
    }

    MenuBar& setActionStatusTip(const QString& actionName,
                                const QString& statusTip) {
        auto it = actions_.find(actionName);
        if (it != actions_.end()) {
            it->second->setStatusTip(statusTip);
        }
        return *this;
    }

    MenuBar& setActionToolTip(const QString& actionName,
                              const QString& toolTip) {
        auto it = actions_.find(actionName);
        if (it != actions_.end()) {
            it->second->setToolTip(toolTip);
        }
        return *this;
    }

    MenuBar& setActionWhatsThis(const QString& actionName,
                                const QString& whatsThis) {
        auto it = actions_.find(actionName);
        if (it != actions_.end()) {
            it->second->setWhatsThis(whatsThis);
        }
        return *this;
    }

    // **Menu Bar Configuration**
    MenuBar& setDefaultUp(bool defaultUp) {
        if (auto menuBar = qobject_cast<QMenuBar*>(widget_.get())) {
            menuBar->setDefaultUp(defaultUp);
        }
        return *this;
    }

    MenuBar& setNativeMenuBar(bool nativeMenuBar) {
        if (auto menuBar = qobject_cast<QMenuBar*>(widget_.get())) {
            menuBar->setNativeMenuBar(nativeMenuBar);
        }
        return *this;
    }

    MenuBar& setCornerWidget(QWidget* widget,
                             Qt::Corner corner = Qt::TopRightCorner) {
        if (auto menuBar = qobject_cast<QMenuBar*>(widget_.get())) {
            menuBar->setCornerWidget(widget, corner);
        }
        return *this;
    }

    // **Event Handlers**
    MenuBar& onActionTriggered(std::function<void(QAction*)> handler) {
        if (auto menuBar = qobject_cast<QMenuBar*>(widget_.get())) {
            QObject::connect(menuBar, &QMenuBar::triggered, handler);
        }
        return *this;
    }

    MenuBar& onActionHovered(std::function<void(QAction*)> handler) {
        if (auto menuBar = qobject_cast<QMenuBar*>(widget_.get())) {
            QObject::connect(menuBar, &QMenuBar::hovered, handler);
        }
        return *this;
    }

    // **Getters**
    QMenu* getMenu(const QString& title) const {
        auto it = menus_.find(title);
        return (it != menus_.end()) ? it->second : nullptr;
    }

    QAction* getAction(const QString& name) const {
        auto it = actions_.find(name);
        return (it != actions_.end()) ? it->second : nullptr;
    }

    QActionGroup* getActionGroup(const QString& groupName) const {
        auto it = action_groups_.find(groupName);
        return (it != action_groups_.end()) ? it->second.get() : nullptr;
    }

    QMenu* getCurrentMenu() const { return current_menu_; }

    QAction* getActiveAction() const {
        if (auto menuBar = qobject_cast<QMenuBar*>(widget_.get())) {
            return menuBar->activeAction();
        }
        return nullptr;
    }

    QWidget* getCornerWidget(Qt::Corner corner = Qt::TopRightCorner) const {
        if (auto menuBar = qobject_cast<QMenuBar*>(widget_.get())) {
            return menuBar->cornerWidget(corner);
        }
        return nullptr;
    }

    bool isDefaultUp() const {
        if (auto menuBar = qobject_cast<QMenuBar*>(widget_.get())) {
            return menuBar->isDefaultUp();
        }
        return false;
    }

    bool isNativeMenuBar() const {
        if (auto menuBar = qobject_cast<QMenuBar*>(widget_.get())) {
            return menuBar->isNativeMenuBar();
        }
        return false;
    }

    // **Utility Methods**
    MenuBar& clear() {
        if (auto menuBar = qobject_cast<QMenuBar*>(widget_.get())) {
            menuBar->clear();
            menus_.clear();
            actions_.clear();
            action_groups_.clear();
            current_menu_ = nullptr;
            current_action_group_ = nullptr;
        }
        return *this;
    }

    MenuBar& setMenuFont(const QFont& font) {
        if (auto menuBar = qobject_cast<QMenuBar*>(widget_.get())) {
            menuBar->setFont(font);
        }
        return *this;
    }

    MenuBar& setMenuStyleSheet(const QString& styleSheet) {
        if (auto menuBar = qobject_cast<QMenuBar*>(widget_.get())) {
            menuBar->setStyleSheet(styleSheet);
        }
        return *this;
    }

    MenuBar& setMenuHeight(int height) {
        if (auto menuBar = qobject_cast<QMenuBar*>(widget_.get())) {
            menuBar->setFixedHeight(height);
        }
        return *this;
    }

    MenuBar& setMenuSpacing(int spacing) {
        if (auto menuBar = qobject_cast<QMenuBar*>(widget_.get())) {
            menuBar->setStyleSheet(
                QString("QMenuBar::item { padding: %1px; }").arg(spacing));
        }
        return *this;
    }

    int getMenuCount() const { return static_cast<int>(menus_.size()); }

    int getActionCount() const { return static_cast<int>(actions_.size()); }

    QStringList getMenuNames() const {
        QStringList names;
        for (const auto& pair : menus_) {
            names << pair.first;
        }
        return names;
    }

    QStringList getActionNames() const {
        QStringList names;
        for (const auto& pair : actions_) {
            names << pair.first;
        }
        return names;
    }

private:
    std::unordered_map<QString, QMenu*> menus_;
    std::unordered_map<QString, QAction*> actions_;
    std::unordered_map<QString, std::unique_ptr<QActionGroup>> action_groups_;
    QMenu* current_menu_ = nullptr;
    QActionGroup* current_action_group_ = nullptr;

    void setupWidget() {
        if (auto menuBar = qobject_cast<QMenuBar*>(widget_.get())) {
            // **Default configuration**
            menuBar->setNativeMenuBar(false);
            menuBar->setDefaultUp(false);

            // **Enable accessibility**
            menuBar->setAccessibleName("Menu Bar");
            menuBar->setAccessibleDescription("Application menu bar");
        }
    }
};

}  // namespace DeclarativeUI::Components
