// Components/ToolButton.hpp
#pragma once
#include <QAction>
#include <QIcon>
#include <QMenu>
#include <QToolButton>

#include "../Core/UIElement.hpp"

namespace DeclarativeUI::Components {

class ToolButton : public Core::UIElement {
    Q_OBJECT

public:
    explicit ToolButton(QObject* parent = nullptr);

    // **Basic fluent interface (backward compatible)**
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

    // **Enhanced fluent interface**
    ToolButton& tooltip(const QString& tooltip_text);
    ToolButton& accessibleName(const QString& name);
    ToolButton& accessibleDescription(const QString& description);
    ToolButton& shortcut(const QKeySequence& shortcut);
    ToolButton& group(const QString& group_name);
    ToolButton& exclusive(bool exclusive = true);
    ToolButton& badge(const QString& badge_text,
                      const QColor& badge_color = QColor("#e74c3c"));
    ToolButton& notification(bool enabled = true,
                             const QColor& color = QColor("#ff4444"));
    ToolButton& dropShadow(bool enabled = true,
                           const QColor& color = QColor(0, 0, 0, 80));
    ToolButton& hoverEffect(bool enabled = true);
    ToolButton& pressAnimation(bool enabled = true);
    ToolButton& borderRadius(int radius);
    ToolButton& customColors(const QColor& normal,
                             const QColor& hover = QColor(),
                             const QColor& pressed = QColor());
    ToolButton& iconAnimation(bool enabled = true);
    ToolButton& textAnimation(bool enabled = true);
    ToolButton& onHover(std::function<void(bool)> hover_handler);
    ToolButton& onFocus(std::function<void(bool)> focus_handler);
    ToolButton& onDoubleClick(std::function<void()> double_click_handler);
    ToolButton& onRightClick(std::function<void()> right_click_handler);
    ToolButton& role(const QString& aria_role);
    ToolButton& tabIndex(int index);
    ToolButton& describedBy(const QString& element_id);

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
