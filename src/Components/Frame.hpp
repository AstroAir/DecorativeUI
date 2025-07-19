// Components/Frame.hpp
#pragma once
#include <QFrame>
#include <QWidget>

#include "../Core/UIElement.hpp"

namespace DeclarativeUI::Components {

class Frame : public Core::UIElement {
    Q_OBJECT

public:
    explicit Frame(QObject* parent = nullptr);

    // **Fluent interface for frame**
    Frame& frameStyle(QFrame::Shape shape, QFrame::Shadow shadow = QFrame::Plain);
    Frame& frameShape(QFrame::Shape shape);
    Frame& frameShadow(QFrame::Shadow shadow);
    Frame& lineWidth(int width);
    Frame& midLineWidth(int width);
    Frame& margin(int margin);
    Frame& contentsMargins(int left, int top, int right, int bottom);
    Frame& contentsMargins(const QMargins& margins);
    Frame& layout(QLayout* layout);
    Frame& addWidget(QWidget* widget);
    Frame& style(const QString& stylesheet);
    
    // **Layout convenience methods**
    Frame& hBoxLayout();
    Frame& vBoxLayout();
    Frame& gridLayout();
    Frame& spacing(int space);

    void initialize() override;
    QFrame::Shape getFrameShape() const;
    QFrame::Shadow getFrameShadow() const;
    int getLineWidth() const;
    int getMidLineWidth() const;
    QRect getFrameRect() const;
    QMargins getContentsMargins() const;
    void setLayout(QLayout* layout);

private:
    QFrame* frame_widget_;
};



}  // namespace DeclarativeUI::Components
