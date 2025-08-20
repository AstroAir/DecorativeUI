// Components/Splitter.hpp
#pragma once
#include <QSplitter>
#include <QWidget>

#include "../Core/UIElement.hpp"

namespace DeclarativeUI::Components {

class Splitter : public Core::UIElement {
    Q_OBJECT

public:
    explicit Splitter(QObject* parent = nullptr);

    // **Fluent interface for splitter**
    Splitter& orientation(Qt::Orientation orientation);
    Splitter& childrenCollapsible(bool collapsible);
    Splitter& handleWidth(int width);
    Splitter& opaqueResize(bool opaque);
    Splitter& addWidget(QWidget* widget);
    Splitter& insertWidget(int index, QWidget* widget);
    Splitter& setSizes(const QList<int>& sizes);
    Splitter& setStretchFactor(int index, int stretch);
    Splitter& onSplitterMoved(std::function<void(int, int)> handler);
    Splitter& style(const QString& stylesheet);

    void initialize() override;
    QWidget* getWidget() const;
    Qt::Orientation getOrientation() const;
    QList<int> getSizes() const;
    int getCount() const;
    QWidget* getWidget(int index) const;
    void removeWidget(QWidget* widget);
    void replaceWidget(int index, QWidget* widget);

private:
    QSplitter* splitter_widget_;
    std::function<void(int, int)> splitter_moved_handler_;
};

}  // namespace DeclarativeUI::Components
