// Components/ToolBar.hpp
#pragma once
#include <QAction>
#include <QIcon>
#include <QToolBar>
#include <QWidget>

#include "../Core/UIElement.hpp"

namespace DeclarativeUI::Components {

class ToolBar : public Core::UIElement {
    Q_OBJECT

public:
    explicit ToolBar(QObject* parent = nullptr);

    // **Fluent interface for toolbar**
    ToolBar& windowTitle(const QString& title);
    ToolBar& movable(bool movable);
    ToolBar& allowedAreas(Qt::ToolBarAreas areas);
    ToolBar& orientation(Qt::Orientation orientation);
    ToolBar& iconSize(const QSize& iconSize);
    ToolBar& toolButtonStyle(Qt::ToolButtonStyle toolButtonStyle);
    ToolBar& floatable(bool floatable);
    ToolBar& floating(bool floating);
    ToolBar& addAction(QAction* action);
    ToolBar& addAction(const QString& text, std::function<void()> handler);
    ToolBar& addAction(const QIcon& icon, const QString& text,
                       std::function<void()> handler);
    ToolBar& addSeparator();
    ToolBar& addWidget(QWidget* widget);
    ToolBar& insertAction(QAction* before, QAction* action);
    ToolBar& insertSeparator(QAction* before);
    ToolBar& insertWidget(QAction* before, QWidget* widget);
    ToolBar& removeAction(QAction* action);
    ToolBar& clear();
    ToolBar& onActionTriggered(std::function<void(QAction*)> handler);
    ToolBar& onMovableChanged(std::function<void(bool)> handler);
    ToolBar& onAllowedAreasChanged(
        std::function<void(Qt::ToolBarAreas)> handler);
    ToolBar& onOrientationChanged(std::function<void(Qt::Orientation)> handler);
    ToolBar& onIconSizeChanged(std::function<void(const QSize&)> handler);
    ToolBar& onToolButtonStyleChanged(
        std::function<void(Qt::ToolButtonStyle)> handler);
    ToolBar& onTopLevelChanged(std::function<void(bool)> handler);
    ToolBar& onVisibilityChanged(std::function<void(bool)> handler);
    ToolBar& style(const QString& stylesheet);

    void initialize() override;
    bool isMovable() const;
    Qt::ToolBarAreas getAllowedAreas() const;
    Qt::Orientation getOrientation() const;
    QSize getIconSize() const;
    Qt::ToolButtonStyle getToolButtonStyle() const;
    bool isFloatable() const;
    bool isFloating() const;
    QList<QAction*> getActions() const;

private:
    QToolBar* toolbar_widget_;
    std::function<void(QAction*)> action_triggered_handler_;
    std::function<void(bool)> movable_changed_handler_;
    std::function<void(Qt::ToolBarAreas)> allowed_areas_changed_handler_;
    std::function<void(Qt::Orientation)> orientation_changed_handler_;
    std::function<void(const QSize&)> icon_size_changed_handler_;
    std::function<void(Qt::ToolButtonStyle)> tool_button_style_changed_handler_;
    std::function<void(bool)> top_level_changed_handler_;
    std::function<void(bool)> visibility_changed_handler_;
};

}  // namespace DeclarativeUI::Components
