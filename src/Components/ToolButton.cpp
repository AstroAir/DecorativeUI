// Components/ToolButton.cpp
#include "ToolButton.hpp"

namespace DeclarativeUI::Components {

// **Implementation**
ToolButton::ToolButton(QObject* parent)
    : UIElement(parent), tool_button_widget_(nullptr) {}

ToolButton& ToolButton::text(const QString& text) {
    return static_cast<ToolButton&>(setProperty("text", text));
}

ToolButton& ToolButton::icon(const QIcon& icon) {
    return static_cast<ToolButton&>(setProperty("icon", icon));
}

ToolButton& ToolButton::iconSize(const QSize& size) {
    return static_cast<ToolButton&>(setProperty("iconSize", size));
}

ToolButton& ToolButton::toolButtonStyle(Qt::ToolButtonStyle style) {
    return static_cast<ToolButton&>(setProperty("toolButtonStyle", static_cast<int>(style)));
}

ToolButton& ToolButton::popupMode(QToolButton::ToolButtonPopupMode mode) {
    return static_cast<ToolButton&>(setProperty("popupMode", static_cast<int>(mode)));
}

ToolButton& ToolButton::menu(QMenu* menu) {
    if (tool_button_widget_ && menu) {
        tool_button_widget_->setMenu(menu);
    }
    return *this;
}

ToolButton& ToolButton::defaultAction(QAction* action) {
    if (tool_button_widget_ && action) {
        tool_button_widget_->setDefaultAction(action);
    }
    return *this;
}

ToolButton& ToolButton::checkable(bool checkable) {
    return static_cast<ToolButton&>(setProperty("checkable", checkable));
}

ToolButton& ToolButton::checked(bool checked) {
    return static_cast<ToolButton&>(setProperty("checked", checked));
}

ToolButton& ToolButton::autoRaise(bool autoRaise) {
    return static_cast<ToolButton&>(setProperty("autoRaise", autoRaise));
}

ToolButton& ToolButton::arrowType(Qt::ArrowType type) {
    return static_cast<ToolButton&>(setProperty("arrowType", static_cast<int>(type)));
}

ToolButton& ToolButton::onClicked(std::function<void()> handler) {
    clicked_handler_ = std::move(handler);
    return *this;
}

ToolButton& ToolButton::onPressed(std::function<void()> handler) {
    pressed_handler_ = std::move(handler);
    return *this;
}

ToolButton& ToolButton::onReleased(std::function<void()> handler) {
    released_handler_ = std::move(handler);
    return *this;
}

ToolButton& ToolButton::onToggled(std::function<void(bool)> handler) {
    toggled_handler_ = std::move(handler);
    return *this;
}

ToolButton& ToolButton::onTriggered(std::function<void(QAction*)> handler) {
    triggered_handler_ = std::move(handler);
    return *this;
}

ToolButton& ToolButton::style(const QString& stylesheet) {
    return static_cast<ToolButton&>(setProperty("styleSheet", stylesheet));
}

void ToolButton::initialize() {
    if (!tool_button_widget_) {
        tool_button_widget_ = new QToolButton();
        setWidget(tool_button_widget_);

        // Connect signals
        if (clicked_handler_) {
            connect(tool_button_widget_, &QToolButton::clicked, this,
                    [this]() { clicked_handler_(); });
        }

        if (pressed_handler_) {
            connect(tool_button_widget_, &QToolButton::pressed, this,
                    [this]() { pressed_handler_(); });
        }

        if (released_handler_) {
            connect(tool_button_widget_, &QToolButton::released, this,
                    [this]() { released_handler_(); });
        }

        if (toggled_handler_) {
            connect(tool_button_widget_, &QToolButton::toggled, this,
                    [this](bool checked) { toggled_handler_(checked); });
        }

        if (triggered_handler_) {
            connect(tool_button_widget_, &QToolButton::triggered, this,
                    [this](QAction* action) { triggered_handler_(action); });
        }
    }
}

QString ToolButton::getText() const {
    return tool_button_widget_ ? tool_button_widget_->text() : QString();
}

QIcon ToolButton::getIcon() const {
    return tool_button_widget_ ? tool_button_widget_->icon() : QIcon();
}

bool ToolButton::isCheckable() const {
    return tool_button_widget_ ? tool_button_widget_->isCheckable() : false;
}

bool ToolButton::isChecked() const {
    return tool_button_widget_ ? tool_button_widget_->isChecked() : false;
}

void ToolButton::setChecked(bool checked) {
    if (tool_button_widget_) {
        tool_button_widget_->setChecked(checked);
    }
}

QMenu* ToolButton::getMenu() const {
    return tool_button_widget_ ? tool_button_widget_->menu() : nullptr;
}

QAction* ToolButton::getDefaultAction() const {
    return tool_button_widget_ ? tool_button_widget_->defaultAction() : nullptr;
}

void ToolButton::showMenu() {
    if (tool_button_widget_) {
        tool_button_widget_->showMenu();
    }
}

void ToolButton::click() {
    if (tool_button_widget_) {
        tool_button_widget_->click();
    }
}

}  // namespace DeclarativeUI::Components
