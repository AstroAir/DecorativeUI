// Components/ToolButton.hpp
#pragma once
#include <QToolButton>
#include <QAction>
#include <QMenu>
#include <QIcon>

#include "../Core/UIElement.hpp"

namespace DeclarativeUI::Components {

class ToolButton : public Core::UIElement {
    Q_OBJECT

public:
    explicit ToolButton(QObject* parent = nullptr);

    // **Fluent interface for tool button**
    ToolButton& text(const QString& text);
    ToolButton& icon(const QIcon& icon);
    ToolButton& iconSize(const QSize& size);
    ToolButton& toolButtonStyle(Qt::ToolButtonStyle style);
    ToolButton& popupMode(QToolButton::ToolButtonPopupMode mode);
    ToolButton& menu(QMenu* menu);
    ToolButton& defaultAction(QAction* action);
    ToolButton& checkable(bool checkable);
    ToolButton& checked(bool checked);
    ToolButton& autoRaise(bool autoRaise);
    ToolButton& arrowType(Qt::ArrowType type);
    ToolButton& onClicked(std::function<void()> handler);
    ToolButton& onPressed(std::function<void()> handler);
    ToolButton& onReleased(std::function<void()> handler);
    ToolButton& onToggled(std::function<void(bool)> handler);
    ToolButton& onTriggered(std::function<void(QAction*)> handler);
    ToolButton& style(const QString& stylesheet);

    void initialize() override;
    QString getText() const;
    QIcon getIcon() const;
    bool isCheckable() const;
    bool isChecked() const;
    void setChecked(bool checked);
    QMenu* getMenu() const;
    QAction* getDefaultAction() const;
    void showMenu();
    void click();

private:
    QToolButton* tool_button_widget_;
    std::function<void()> clicked_handler_;
    std::function<void()> pressed_handler_;
    std::function<void()> released_handler_;
    std::function<void(bool)> toggled_handler_;
    std::function<void(QAction*)> triggered_handler_;
};



}  // namespace DeclarativeUI::Components
