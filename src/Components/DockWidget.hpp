// Components/DockWidget.hpp
#pragma once
#include <QDockWidget>
#include <QWidget>

#include "../Core/UIElement.hpp"

namespace DeclarativeUI::Components {

class DockWidget : public Core::UIElement {
    Q_OBJECT

public:
    explicit DockWidget(QObject* parent = nullptr);

    // **Fluent interface for dock widget**
    DockWidget& windowTitle(const QString& title);
    DockWidget& widget(QWidget* widget);
    DockWidget& features(QDockWidget::DockWidgetFeatures features);
    DockWidget& allowedAreas(Qt::DockWidgetAreas areas);
    DockWidget& floating(bool floating);
    DockWidget& titleBarWidget(QWidget* widget);
    DockWidget& onFeaturesChanged(std::function<void(QDockWidget::DockWidgetFeatures)> handler);
    DockWidget& onTopLevelChanged(std::function<void(bool)> handler);
    DockWidget& onAllowedAreasChanged(std::function<void(Qt::DockWidgetAreas)> handler);
    DockWidget& onVisibilityChanged(std::function<void(bool)> handler);
    DockWidget& onDockLocationChanged(std::function<void(Qt::DockWidgetArea)> handler);
    DockWidget& style(const QString& stylesheet);

    void initialize() override;
    QWidget* getWidget() const;
    void setWidget(QWidget* widget);
    bool isFloating() const;
    Qt::DockWidgetArea getDockWidgetArea() const;
    QDockWidget::DockWidgetFeatures getFeatures() const;
    Qt::DockWidgetAreas getAllowedAreas() const;

private:
    QDockWidget* dock_widget_;
    std::function<void(QDockWidget::DockWidgetFeatures)> features_changed_handler_;
    std::function<void(bool)> top_level_changed_handler_;
    std::function<void(Qt::DockWidgetAreas)> allowed_areas_changed_handler_;
    std::function<void(bool)> visibility_changed_handler_;
    std::function<void(Qt::DockWidgetArea)> dock_location_changed_handler_;
};





}  // namespace DeclarativeUI::Components
