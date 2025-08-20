// Components/ToolBar.cpp
#include "ToolBar.hpp"

namespace DeclarativeUI::Components {

// **Implementation**
ToolBar::ToolBar(QObject* parent)
    : UIElement(parent), toolbar_widget_(nullptr) {}

ToolBar& ToolBar::windowTitle(const QString& title) {
    return static_cast<ToolBar&>(setProperty("windowTitle", title));
}

ToolBar& ToolBar::movable(bool movable) {
    return static_cast<ToolBar&>(setProperty("movable", movable));
}

ToolBar& ToolBar::allowedAreas(Qt::ToolBarAreas areas) {
    return static_cast<ToolBar&>(
        setProperty("allowedAreas", static_cast<int>(areas)));
}

ToolBar& ToolBar::orientation(Qt::Orientation orientation) {
    return static_cast<ToolBar&>(
        setProperty("orientation", static_cast<int>(orientation)));
}

ToolBar& ToolBar::iconSize(const QSize& iconSize) {
    return static_cast<ToolBar&>(setProperty("iconSize", iconSize));
}

ToolBar& ToolBar::toolButtonStyle(Qt::ToolButtonStyle toolButtonStyle) {
    return static_cast<ToolBar&>(
        setProperty("toolButtonStyle", static_cast<int>(toolButtonStyle)));
}

ToolBar& ToolBar::floatable(bool floatable) {
    return static_cast<ToolBar&>(setProperty("floatable", floatable));
}

ToolBar& ToolBar::floating(bool floating) {
    return static_cast<ToolBar&>(setProperty("floating", floating));
}

ToolBar& ToolBar::addAction(QAction* action) {
    if (toolbar_widget_ && action) {
        toolbar_widget_->addAction(action);
    }
    return *this;
}

ToolBar& ToolBar::addAction(const QString& text,
                            std::function<void()> handler) {
    if (toolbar_widget_) {
        QAction* action = toolbar_widget_->addAction(text);
        if (handler) {
            connect(action, &QAction::triggered, this,
                    [handler]() { handler(); });
        }
    }
    return *this;
}

ToolBar& ToolBar::addAction(const QIcon& icon, const QString& text,
                            std::function<void()> handler) {
    if (toolbar_widget_) {
        QAction* action = toolbar_widget_->addAction(icon, text);
        if (handler) {
            connect(action, &QAction::triggered, this,
                    [handler]() { handler(); });
        }
    }
    return *this;
}

ToolBar& ToolBar::addSeparator() {
    if (toolbar_widget_) {
        toolbar_widget_->addSeparator();
    }
    return *this;
}

ToolBar& ToolBar::addWidget(QWidget* widget) {
    if (toolbar_widget_ && widget) {
        toolbar_widget_->addWidget(widget);
    }
    return *this;
}

ToolBar& ToolBar::insertAction(QAction* before, QAction* action) {
    if (toolbar_widget_ && before && action) {
        toolbar_widget_->insertAction(before, action);
    }
    return *this;
}

ToolBar& ToolBar::insertSeparator(QAction* before) {
    if (toolbar_widget_ && before) {
        toolbar_widget_->insertSeparator(before);
    }
    return *this;
}

ToolBar& ToolBar::insertWidget(QAction* before, QWidget* widget) {
    if (toolbar_widget_ && before && widget) {
        toolbar_widget_->insertWidget(before, widget);
    }
    return *this;
}

ToolBar& ToolBar::removeAction(QAction* action) {
    if (toolbar_widget_ && action) {
        toolbar_widget_->removeAction(action);
    }
    return *this;
}

ToolBar& ToolBar::clear() {
    if (toolbar_widget_) {
        toolbar_widget_->clear();
    }
    return *this;
}

ToolBar& ToolBar::onActionTriggered(std::function<void(QAction*)> handler) {
    action_triggered_handler_ = std::move(handler);
    return *this;
}

ToolBar& ToolBar::onMovableChanged(std::function<void(bool)> handler) {
    movable_changed_handler_ = std::move(handler);
    return *this;
}

ToolBar& ToolBar::onAllowedAreasChanged(
    std::function<void(Qt::ToolBarAreas)> handler) {
    allowed_areas_changed_handler_ = std::move(handler);
    return *this;
}

ToolBar& ToolBar::onOrientationChanged(
    std::function<void(Qt::Orientation)> handler) {
    orientation_changed_handler_ = std::move(handler);
    return *this;
}

ToolBar& ToolBar::onIconSizeChanged(std::function<void(const QSize&)> handler) {
    icon_size_changed_handler_ = std::move(handler);
    return *this;
}

ToolBar& ToolBar::onToolButtonStyleChanged(
    std::function<void(Qt::ToolButtonStyle)> handler) {
    tool_button_style_changed_handler_ = std::move(handler);
    return *this;
}

ToolBar& ToolBar::onTopLevelChanged(std::function<void(bool)> handler) {
    top_level_changed_handler_ = std::move(handler);
    return *this;
}

ToolBar& ToolBar::onVisibilityChanged(std::function<void(bool)> handler) {
    visibility_changed_handler_ = std::move(handler);
    return *this;
}

ToolBar& ToolBar::style(const QString& stylesheet) {
    return static_cast<ToolBar&>(setProperty("styleSheet", stylesheet));
}

void ToolBar::initialize() {
    if (!toolbar_widget_) {
        toolbar_widget_ = new QToolBar();
        setWidget(toolbar_widget_);

        // Connect signals
        if (action_triggered_handler_) {
            connect(
                toolbar_widget_, &QToolBar::actionTriggered, this,
                [this](QAction* action) { action_triggered_handler_(action); });
        }

        if (movable_changed_handler_) {
            connect(
                toolbar_widget_, &QToolBar::movableChanged, this,
                [this](bool movable) { movable_changed_handler_(movable); });
        }

        if (allowed_areas_changed_handler_) {
            connect(toolbar_widget_, &QToolBar::allowedAreasChanged, this,
                    [this](Qt::ToolBarAreas allowedAreas) {
                        allowed_areas_changed_handler_(allowedAreas);
                    });
        }

        if (orientation_changed_handler_) {
            connect(toolbar_widget_, &QToolBar::orientationChanged, this,
                    [this](Qt::Orientation orientation) {
                        orientation_changed_handler_(orientation);
                    });
        }

        if (icon_size_changed_handler_) {
            connect(toolbar_widget_, &QToolBar::iconSizeChanged, this,
                    [this](const QSize& iconSize) {
                        icon_size_changed_handler_(iconSize);
                    });
        }

        if (tool_button_style_changed_handler_) {
            connect(toolbar_widget_, &QToolBar::toolButtonStyleChanged, this,
                    [this](Qt::ToolButtonStyle toolButtonStyle) {
                        tool_button_style_changed_handler_(toolButtonStyle);
                    });
        }

        if (top_level_changed_handler_) {
            connect(toolbar_widget_, &QToolBar::topLevelChanged, this,
                    [this](bool topLevel) {
                        top_level_changed_handler_(topLevel);
                    });
        }

        if (visibility_changed_handler_) {
            connect(
                toolbar_widget_, &QToolBar::visibilityChanged, this,
                [this](bool visible) { visibility_changed_handler_(visible); });
        }
    }
}

bool ToolBar::isMovable() const {
    return toolbar_widget_ ? toolbar_widget_->isMovable() : true;
}

Qt::ToolBarAreas ToolBar::getAllowedAreas() const {
    return toolbar_widget_ ? toolbar_widget_->allowedAreas()
                           : Qt::AllToolBarAreas;
}

Qt::Orientation ToolBar::getOrientation() const {
    return toolbar_widget_ ? toolbar_widget_->orientation() : Qt::Horizontal;
}

QSize ToolBar::getIconSize() const {
    return toolbar_widget_ ? toolbar_widget_->iconSize() : QSize();
}

Qt::ToolButtonStyle ToolBar::getToolButtonStyle() const {
    return toolbar_widget_ ? toolbar_widget_->toolButtonStyle()
                           : Qt::ToolButtonIconOnly;
}

bool ToolBar::isFloatable() const {
    return toolbar_widget_ ? toolbar_widget_->isFloatable() : true;
}

bool ToolBar::isFloating() const {
    return toolbar_widget_ ? toolbar_widget_->isFloating() : false;
}

QList<QAction*> ToolBar::getActions() const {
    return toolbar_widget_ ? toolbar_widget_->actions() : QList<QAction*>();
}

}  // namespace DeclarativeUI::Components
