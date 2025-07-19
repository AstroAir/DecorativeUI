// Components/Widget.hpp
#pragma once
#include <QWidget>
#include <QLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>

#include "../Core/UIElement.hpp"

namespace DeclarativeUI::Components {

class Widget : public Core::UIElement {
    Q_OBJECT

public:
    explicit Widget(QObject* parent = nullptr);

    // **Fluent interface for generic widget**
    Widget& size(const QSize& size);
    Widget& minimumSize(const QSize& size);
    Widget& maximumSize(const QSize& size);
    Widget& fixedSize(const QSize& size);
    Widget& sizePolicy(QSizePolicy::Policy horizontal, QSizePolicy::Policy vertical);
    Widget& sizePolicy(const QSizePolicy& policy);
    Widget& geometry(const QRect& rect);
    Widget& position(const QPoint& pos);
    Widget& visible(bool visible);
    Widget& enabled(bool enabled);
    Widget& toolTip(const QString& tooltip);
    Widget& statusTip(const QString& statusTip);
    Widget& whatsThis(const QString& whatsThis);
    Widget& windowTitle(const QString& title);
    Widget& windowIcon(const QIcon& icon);
    Widget& windowFlags(Qt::WindowFlags flags);
    Widget& windowState(Qt::WindowStates state);
    Widget& focusPolicy(Qt::FocusPolicy policy);
    Widget& contextMenuPolicy(Qt::ContextMenuPolicy policy);
    Widget& cursor(const QCursor& cursor);
    Widget& font(const QFont& font);
    Widget& palette(const QPalette& palette);
    Widget& autoFillBackground(bool enabled);
    Widget& updatesEnabled(bool enabled);
    Widget& layout(QLayout* layout);
    Widget& vBoxLayout();
    Widget& hBoxLayout();
    Widget& gridLayout(int rows = 0, int columns = 0);
    Widget& formLayout();
    Widget& addWidget(QWidget* widget);
    Widget& addWidget(QWidget* widget, int row, int column, Qt::Alignment alignment = Qt::Alignment());
    Widget& addWidget(QWidget* widget, int row, int column, int rowSpan, int columnSpan, Qt::Alignment alignment = Qt::Alignment());
    Widget& addLayout(QLayout* layout);
    Widget& spacing(int spacing);
    Widget& margins(int left, int top, int right, int bottom);
    Widget& margins(const QMargins& margins);
    Widget& style(const QString& stylesheet);

    void initialize() override;
    QSize getSize() const;
    QSize getMinimumSize() const;
    QSize getMaximumSize() const;
    QRect getGeometry() const;
    QPoint getPosition() const;
    bool isVisible() const;
    bool isEnabled() const;
    QString getToolTip() const;
    QLayout* getLayout() const;
    void show();
    void hide();
    void setFocus();
    void clearFocus();
    void update();
    void repaint();

private:
    QWidget* widget_;
};





}  // namespace DeclarativeUI::Components
