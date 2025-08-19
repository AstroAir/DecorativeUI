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
    Frame& frameStyle(QFrame::Shape shape,
                      QFrame::Shadow shadow = QFrame::Plain);
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

    // **Enhanced fluent interface**
    Frame& tooltip(const QString& tooltip_text);
    Frame& accessibleName(const QString& name);
    Frame& accessibleDescription(const QString& description);
    Frame& shortcut(const QKeySequence& shortcut);

    // **Visual enhancements**
    Frame& dropShadow(bool enabled = true,
                      const QColor& color = QColor(0, 0, 0, 80));
    Frame& hoverEffect(bool enabled = true);
    Frame& focusEffect(bool enabled = true);
    Frame& borderRadius(int radius);
    Frame& customColors(const QColor& background,
                        const QColor& border = QColor());
    Frame& gradient(const QColor& start, const QColor& end,
                    Qt::Orientation orientation = Qt::Vertical);
    Frame& opacity(qreal opacity);

    // **Interactive features**
    Frame& clickable(bool enabled = true);
    Frame& hoverable(bool enabled = true);
    Frame& draggable(bool enabled = true);
    Frame& resizable(bool enabled = true);
    Frame& contextMenu(QMenu* menu);

    // **Animation and effects**
    Frame& fadeAnimation(bool enabled = true);
    Frame& slideAnimation(bool enabled = true);
    Frame& scaleAnimation(bool enabled = true);
    Frame& rotateAnimation(bool enabled = true);

    // **Event handlers**
    Frame& onClick(std::function<void()> click_handler);
    Frame& onDoubleClick(std::function<void()> double_click_handler);
    Frame& onRightClick(std::function<void()> right_click_handler);
    Frame& onHover(std::function<void(bool)> hover_handler);
    Frame& onFocus(std::function<void(bool)> focus_handler);
    Frame& onResize(std::function<void(const QSize&)> resize_handler);

    // **Accessibility**
    Frame& role(const QString& aria_role);
    Frame& tabIndex(int index);
    Frame& describedBy(const QString& element_id);
    Frame& labelledBy(const QString& element_id);

    // **Advanced layout management**
    Frame& autoLayout(bool enabled = true);
    Frame& layoutDirection(Qt::LayoutDirection direction);
    Frame& sizePolicy(QSizePolicy::Policy horizontal,
                      QSizePolicy::Policy vertical);
    Frame& minimumSize(const QSize& size);
    Frame& maximumSize(const QSize& size);
    Frame& fixedSize(const QSize& size);

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
