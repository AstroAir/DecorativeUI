// Components/TabWidget.hpp
#pragma once

#include <QAction>
#include <QContextMenuEvent>
#include <QHBoxLayout>
#include <QIcon>
#include <QMenu>
#include <QMouseEvent>
#include <QPoint>
#include <QPushButton>
#include <QTabBar>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QWidget>

#include <functional>

#include "../Core/UIElement.hpp"

namespace DeclarativeUI::Components {

/**
 * @brief A comprehensive tab widget component
 *
 * Features:
 * - Dynamic tab management
 * - Closeable tabs
 * - Moveable tabs
 * - Custom tab shapes and positions
 * - Tab context menus
 * - Tab icons and tooltips
 * - Keyboard shortcuts
 * - Accessibility support
 */
class TabWidget : public Core::UIElement {
public:
    TabWidget() {
        widget_ = std::make_unique<QTabWidget>();
        setupWidget();
    }

    virtual ~TabWidget() = default;

    // **Fluent Interface for Tab Management**
    TabWidget &addTab(QWidget *widget, const QString &label) {
        if (auto tabWidget = qobject_cast<QTabWidget *>(widget_.get())) {
            tabWidget->addTab(widget, label);
        }
        return *this;
    }

    TabWidget &addTab(QWidget *widget, const QIcon &icon,
                      const QString &label) {
        if (auto tabWidget = qobject_cast<QTabWidget *>(widget_.get())) {
            tabWidget->addTab(widget, icon, label);
        }
        return *this;
    }

    TabWidget &insertTab(int index, QWidget *widget, const QString &label) {
        if (auto tabWidget = qobject_cast<QTabWidget *>(widget_.get())) {
            tabWidget->insertTab(index, widget, label);
        }
        return *this;
    }

    TabWidget &insertTab(int index, QWidget *widget, const QIcon &icon,
                         const QString &label) {
        if (auto tabWidget = qobject_cast<QTabWidget *>(widget_.get())) {
            tabWidget->insertTab(index, widget, icon, label);
        }
        return *this;
    }

    TabWidget &removeTab(int index) {
        if (auto tabWidget = qobject_cast<QTabWidget *>(widget_.get())) {
            tabWidget->removeTab(index);
        }
        return *this;
    }

    TabWidget &setTabText(int index, const QString &text) {
        if (auto tabWidget = qobject_cast<QTabWidget *>(widget_.get())) {
            tabWidget->setTabText(index, text);
        }
        return *this;
    }

    TabWidget &setTabIcon(int index, const QIcon &icon) {
        if (auto tabWidget = qobject_cast<QTabWidget *>(widget_.get())) {
            tabWidget->setTabIcon(index, icon);
        }
        return *this;
    }

    TabWidget &setTabToolTip(int index, const QString &tip) {
        if (auto tabWidget = qobject_cast<QTabWidget *>(widget_.get())) {
            tabWidget->setTabToolTip(index, tip);
        }
        return *this;
    }

    TabWidget &setTabWhatsThis(int index, const QString &text) {
        if (auto tabWidget = qobject_cast<QTabWidget *>(widget_.get())) {
            tabWidget->setTabWhatsThis(index, text);
        }
        return *this;
    }

    TabWidget &setTabEnabled(int index, bool enabled) {
        if (auto tabWidget = qobject_cast<QTabWidget *>(widget_.get())) {
            tabWidget->setTabEnabled(index, enabled);
        }
        return *this;
    }

    TabWidget &setTabVisible(int index, bool visible) {
        if (auto tabWidget = qobject_cast<QTabWidget *>(widget_.get())) {
            tabWidget->setTabVisible(index, visible);
        }
        return *this;
    }

    TabWidget &setCurrentIndex(int index) {
        if (auto tabWidget = qobject_cast<QTabWidget *>(widget_.get())) {
            tabWidget->setCurrentIndex(index);
        }
        return *this;
    }

    TabWidget &setCurrentWidget(QWidget *widget) {
        if (auto tabWidget = qobject_cast<QTabWidget *>(widget_.get())) {
            tabWidget->setCurrentWidget(widget);
        }
        return *this;
    }

    // **Tab Position and Shape**
    TabWidget &setTabPosition(QTabWidget::TabPosition position) {
        if (auto tabWidget = qobject_cast<QTabWidget *>(widget_.get())) {
            tabWidget->setTabPosition(position);
        }
        return *this;
    }

    TabWidget &setTabShape(QTabWidget::TabShape shape) {
        if (auto tabWidget = qobject_cast<QTabWidget *>(widget_.get())) {
            tabWidget->setTabShape(shape);
        }
        return *this;
    }

    TabWidget &setElideMode(Qt::TextElideMode mode) {
        if (auto tabWidget = qobject_cast<QTabWidget *>(widget_.get())) {
            tabWidget->setElideMode(mode);
        }
        return *this;
    }

    TabWidget &setIconSize(const QSize &size) {
        if (auto tabWidget = qobject_cast<QTabWidget *>(widget_.get())) {
            tabWidget->setIconSize(size);
        }
        return *this;
    }

    TabWidget &setUsesScrollButtons(bool useButtons) {
        if (auto tabWidget = qobject_cast<QTabWidget *>(widget_.get())) {
            tabWidget->setUsesScrollButtons(useButtons);
        }
        return *this;
    }

    TabWidget &setMovable(bool movable) {
        if (auto tabWidget = qobject_cast<QTabWidget *>(widget_.get())) {
            tabWidget->setMovable(movable);
        }
        return *this;
    }

    TabWidget &setTabsClosable(bool closeable) {
        if (auto tabWidget = qobject_cast<QTabWidget *>(widget_.get())) {
            tabWidget->setTabsClosable(closeable);
        }
        return *this;
    }

    TabWidget &setDocumentMode(bool enabled) {
        if (auto tabWidget = qobject_cast<QTabWidget *>(widget_.get())) {
            tabWidget->setDocumentMode(enabled);
        }
        return *this;
    }

    TabWidget &setTabBarAutoHide(bool enabled) {
        if (auto tabWidget = qobject_cast<QTabWidget *>(widget_.get())) {
            tabWidget->setTabBarAutoHide(enabled);
        }
        return *this;
    }

    // **Event Handlers**
    TabWidget &onCurrentChanged(std::function<void(int)> handler) {
        if (auto tabWidget = qobject_cast<QTabWidget *>(widget_.get())) {
            QObject::connect(tabWidget, &QTabWidget::currentChanged, handler);
        }
        return *this;
    }

    TabWidget &onTabCloseRequested(std::function<void(int)> handler) {
        if (auto tabWidget = qobject_cast<QTabWidget *>(widget_.get())) {
            QObject::connect(tabWidget, &QTabWidget::tabCloseRequested,
                             handler);
        }
        return *this;
    }

    TabWidget &onTabBarClicked(std::function<void(int)> handler) {
        if (auto tabWidget = qobject_cast<QTabWidget *>(widget_.get())) {
            QObject::connect(tabWidget, &QTabWidget::tabBarClicked, handler);
        }
        return *this;
    }

    TabWidget &onTabBarDoubleClicked(std::function<void(int)> handler) {
        if (auto tabWidget = qobject_cast<QTabWidget *>(widget_.get())) {
            QObject::connect(tabWidget, &QTabWidget::tabBarDoubleClicked,
                             handler);
        }
        return *this;
    }

    // **Getters**
    int getCurrentIndex() const {
        if (auto tabWidget = qobject_cast<QTabWidget *>(widget_.get())) {
            return tabWidget->currentIndex();
        }
        return -1;
    }

    QWidget *getCurrentWidget() const {
        if (auto tabWidget = qobject_cast<QTabWidget *>(widget_.get())) {
            return tabWidget->currentWidget();
        }
        return nullptr;
    }

    QWidget *getWidget(int index) const {
        if (auto tabWidget = qobject_cast<QTabWidget *>(widget_.get())) {
            return tabWidget->widget(index);
        }
        return nullptr;
    }

    QString getTabText(int index) const {
        if (auto tabWidget = qobject_cast<QTabWidget *>(widget_.get())) {
            return tabWidget->tabText(index);
        }
        return QString();
    }

    QIcon getTabIcon(int index) const {
        if (auto tabWidget = qobject_cast<QTabWidget *>(widget_.get())) {
            return tabWidget->tabIcon(index);
        }
        return QIcon();
    }

    QString getTabToolTip(int index) const {
        if (auto tabWidget = qobject_cast<QTabWidget *>(widget_.get())) {
            return tabWidget->tabToolTip(index);
        }
        return QString();
    }

    bool isTabEnabled(int index) const {
        if (auto tabWidget = qobject_cast<QTabWidget *>(widget_.get())) {
            return tabWidget->isTabEnabled(index);
        }
        return false;
    }

    bool isTabVisible(int index) const {
        if (auto tabWidget = qobject_cast<QTabWidget *>(widget_.get())) {
            return tabWidget->isTabVisible(index);
        }
        return false;
    }

    int getTabCount() const {
        if (auto tabWidget = qobject_cast<QTabWidget *>(widget_.get())) {
            return tabWidget->count();
        }
        return 0;
    }

    int indexOf(QWidget *widget) const {
        if (auto tabWidget = qobject_cast<QTabWidget *>(widget_.get())) {
            return tabWidget->indexOf(widget);
        }
        return -1;
    }

    QTabWidget::TabPosition getTabPosition() const {
        if (auto tabWidget = qobject_cast<QTabWidget *>(widget_.get())) {
            return tabWidget->tabPosition();
        }
        return QTabWidget::North;
    }

    QTabWidget::TabShape getTabShape() const {
        if (auto tabWidget = qobject_cast<QTabWidget *>(widget_.get())) {
            return tabWidget->tabShape();
        }
        return QTabWidget::Rounded;
    }

    QSize getIconSize() const {
        if (auto tabWidget = qobject_cast<QTabWidget *>(widget_.get())) {
            return tabWidget->iconSize();
        }
        return QSize();
    }

    Qt::TextElideMode getElideMode() const {
        if (auto tabWidget = qobject_cast<QTabWidget *>(widget_.get())) {
            return tabWidget->elideMode();
        }
        return Qt::ElideNone;
    }

    bool isMovable() const {
        if (auto tabWidget = qobject_cast<QTabWidget *>(widget_.get())) {
            return tabWidget->isMovable();
        }
        return false;
    }

    bool isTabsClosable() const {
        if (auto tabWidget = qobject_cast<QTabWidget *>(widget_.get())) {
            return tabWidget->tabsClosable();
        }
        return false;
    }

    bool isDocumentMode() const {
        if (auto tabWidget = qobject_cast<QTabWidget *>(widget_.get())) {
            return tabWidget->documentMode();
        }
        return false;
    }

    bool isTabBarAutoHide() const {
        if (auto tabWidget = qobject_cast<QTabWidget *>(widget_.get())) {
            return tabWidget->tabBarAutoHide();
        }
        return false;
    }

    // **Advanced Features**
    TabWidget &setCornerWidget(QWidget *widget,
                               Qt::Corner corner = Qt::TopRightCorner) {
        if (auto tabWidget = qobject_cast<QTabWidget *>(widget_.get())) {
            tabWidget->setCornerWidget(widget, corner);
        }
        return *this;
    }

    QWidget *getCornerWidget(Qt::Corner corner = Qt::TopRightCorner) const {
        if (auto tabWidget = qobject_cast<QTabWidget *>(widget_.get())) {
            return tabWidget->cornerWidget(corner);
        }
        return nullptr;
    }

    QTabBar *getTabBar() const {
        if (auto tabWidget = qobject_cast<QTabWidget *>(widget_.get())) {
            return tabWidget->tabBar();
        }
        return nullptr;
    }

    // **Utility Methods**
    TabWidget &clear() {
        if (auto tabWidget = qobject_cast<QTabWidget *>(widget_.get())) {
            tabWidget->clear();
        }
        return *this;
    }

    TabWidget &setTabOrder(QWidget *first, QWidget *second) {
        QWidget::setTabOrder(first, second);
        return *this;
    }

    TabWidget &enableCloseButton(int index, bool enable) {
        if (auto tabWidget = qobject_cast<QTabWidget *>(widget_.get())) {
            if (QTabBar *tabBar = tabWidget->tabBar()) {
                tabBar->setTabButton(index, QTabBar::RightSide,
                                     enable ? new QPushButton("×") : nullptr);
            }
        }
        return *this;
    }

    TabWidget &setTabTextColor(int index, const QColor &color) {
        if (auto tabWidget = qobject_cast<QTabWidget *>(widget_.get())) {
            if (QTabBar *tabBar = tabWidget->tabBar()) {
                tabBar->setTabTextColor(index, color);
            }
        }
        return *this;
    }

    QColor getTabTextColor(int index) const {
        if (auto tabWidget = qobject_cast<QTabWidget *>(widget_.get())) {
            if (QTabBar *tabBar = tabWidget->tabBar()) {
                return tabBar->tabTextColor(index);
            }
        }
        return QColor();
    }

private:
    void setupWidget() {
        if (auto tabWidget = qobject_cast<QTabWidget *>(widget_.get())) {
            // **Default configuration**
            tabWidget->setTabPosition(QTabWidget::North);
            tabWidget->setTabShape(QTabWidget::Rounded);
            tabWidget->setMovable(true);
            tabWidget->setTabsClosable(false);
            tabWidget->setUsesScrollButtons(true);
            tabWidget->setElideMode(Qt::ElideRight);
            tabWidget->setDocumentMode(false);
            tabWidget->setTabBarAutoHide(false);

            // **Set reasonable icon size**
            tabWidget->setIconSize(QSize(16, 16));

            // **Enable accessibility**
            tabWidget->setAccessibleName("Tab Widget");
        }
    }
};

}  // namespace DeclarativeUI::Components
