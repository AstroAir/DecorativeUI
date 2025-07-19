// Components/DockWidget.cpp
#include "DockWidget.hpp"
#include <QMainWindow>

namespace DeclarativeUI::Components {

// **Implementation**
DockWidget::DockWidget(QObject* parent)
    : UIElement(parent), dock_widget_(nullptr) {}

DockWidget& DockWidget::windowTitle(const QString& title) {
    return static_cast<DockWidget&>(setProperty("windowTitle", title));
}

DockWidget& DockWidget::widget(QWidget* widget) {
    if (dock_widget_ && widget) {
        dock_widget_->setWidget(widget);
    }
    return *this;
}

DockWidget& DockWidget::features(QDockWidget::DockWidgetFeatures features) {
    return static_cast<DockWidget&>(setProperty("features", static_cast<int>(features)));
}

DockWidget& DockWidget::allowedAreas(Qt::DockWidgetAreas areas) {
    return static_cast<DockWidget&>(setProperty("allowedAreas", static_cast<int>(areas)));
}

DockWidget& DockWidget::floating(bool floating) {
    return static_cast<DockWidget&>(setProperty("floating", floating));
}

DockWidget& DockWidget::titleBarWidget(QWidget* widget) {
    if (dock_widget_) {
        dock_widget_->setTitleBarWidget(widget);
    }
    return *this;
}

DockWidget& DockWidget::onFeaturesChanged(std::function<void(QDockWidget::DockWidgetFeatures)> handler) {
    features_changed_handler_ = std::move(handler);
    return *this;
}

DockWidget& DockWidget::onTopLevelChanged(std::function<void(bool)> handler) {
    top_level_changed_handler_ = std::move(handler);
    return *this;
}

DockWidget& DockWidget::onAllowedAreasChanged(std::function<void(Qt::DockWidgetAreas)> handler) {
    allowed_areas_changed_handler_ = std::move(handler);
    return *this;
}

DockWidget& DockWidget::onVisibilityChanged(std::function<void(bool)> handler) {
    visibility_changed_handler_ = std::move(handler);
    return *this;
}

DockWidget& DockWidget::onDockLocationChanged(std::function<void(Qt::DockWidgetArea)> handler) {
    dock_location_changed_handler_ = std::move(handler);
    return *this;
}

DockWidget& DockWidget::style(const QString& stylesheet) {
    return static_cast<DockWidget&>(setProperty("styleSheet", stylesheet));
}

void DockWidget::initialize() {
    if (!dock_widget_) {
        dock_widget_ = new QDockWidget();
        setWidget(dock_widget_);

        // Connect signals
        if (features_changed_handler_) {
            connect(dock_widget_, &QDockWidget::featuresChanged, this,
                    [this](QDockWidget::DockWidgetFeatures features) { features_changed_handler_(features); });
        }

        if (top_level_changed_handler_) {
            connect(dock_widget_, &QDockWidget::topLevelChanged, this,
                    [this](bool topLevel) { top_level_changed_handler_(topLevel); });
        }

        if (allowed_areas_changed_handler_) {
            connect(dock_widget_, &QDockWidget::allowedAreasChanged, this,
                    [this](Qt::DockWidgetAreas allowedAreas) { allowed_areas_changed_handler_(allowedAreas); });
        }

        if (visibility_changed_handler_) {
            connect(dock_widget_, &QDockWidget::visibilityChanged, this,
                    [this](bool visible) { visibility_changed_handler_(visible); });
        }

        if (dock_location_changed_handler_) {
            connect(dock_widget_, &QDockWidget::dockLocationChanged, this,
                    [this](Qt::DockWidgetArea area) { dock_location_changed_handler_(area); });
        }
    }
}

QWidget* DockWidget::getWidget() const {
    return dock_widget_ ? dock_widget_->widget() : nullptr;
}

void DockWidget::setWidget(QWidget* widget) {
    if (dock_widget_) {
        dock_widget_->setWidget(widget);
    }
}

bool DockWidget::isFloating() const {
    return dock_widget_ ? dock_widget_->isFloating() : false;
}

Qt::DockWidgetArea DockWidget::getDockWidgetArea() const {
    if (dock_widget_) {
        // To get dock area, we need to check with the parent main window
        if (auto main_window = qobject_cast<QMainWindow*>(dock_widget_->parent())) {
            return main_window->dockWidgetArea(dock_widget_);
        }
    }
    return Qt::NoDockWidgetArea;
}

QDockWidget::DockWidgetFeatures DockWidget::getFeatures() const {
    return dock_widget_ ? dock_widget_->features() : QDockWidget::DockWidgetClosable;
}

Qt::DockWidgetAreas DockWidget::getAllowedAreas() const {
    return dock_widget_ ? dock_widget_->allowedAreas() : Qt::AllDockWidgetAreas;
}

}  // namespace DeclarativeUI::Components
