// Components/ScrollArea.hpp
#pragma once
#include <QScrollArea>
#include <QWidget>

#include "../Core/UIElement.hpp"

namespace DeclarativeUI::Components {

class ScrollArea : public Core::UIElement {
    Q_OBJECT

public:
    explicit ScrollArea(QObject* parent = nullptr);

    // **Fluent interface for scroll area**
    ScrollArea& widget(QWidget* widget);
    ScrollArea& widgetResizable(bool resizable);
    ScrollArea& horizontalScrollBarPolicy(Qt::ScrollBarPolicy policy);
    ScrollArea& verticalScrollBarPolicy(Qt::ScrollBarPolicy policy);
    ScrollArea& alignment(Qt::Alignment alignment);
    ScrollArea& ensureVisible(int x, int y, int xmargin = 50, int ymargin = 50);
    ScrollArea& ensureWidgetVisible(QWidget* childWidget, int xmargin = 50,
                                    int ymargin = 50);
    ScrollArea& style(const QString& stylesheet);

    void initialize() override;
    // Return the QScrollArea widget managed by this component
    QWidget* getWidget() const;
    // Set the content widget displayed inside the QScrollArea
    void setContentWidget(QWidget* widget);
    bool isWidgetResizable() const;
    Qt::ScrollBarPolicy getHorizontalScrollBarPolicy() const;
    Qt::ScrollBarPolicy getVerticalScrollBarPolicy() const;
    QScrollBar* horizontalScrollBar() const;
    QScrollBar* verticalScrollBar() const;

private:
    QScrollArea* scroll_area_widget_;
};

}  // namespace DeclarativeUI::Components
