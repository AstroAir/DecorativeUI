// Components/Widget.hpp
#pragma once
#include <QFormLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLayout>
#include <QVBoxLayout>
#include <QWidget>
#include <concepts>
#include <span>

#include "../Core/Concepts.hpp"
#include "../Core/UIElement.hpp"

namespace DeclarativeUI::Components {

class Widget : public Core::UIElement {
    Q_OBJECT

public:
    explicit Widget(QObject* parent = nullptr);

    // **Modern C++20 Fluent interface with concepts**
    template <Core::Concepts::SizeLike T>
    constexpr Widget& size(T&& size_value);

    template <Core::Concepts::SizeLike T>
    constexpr Widget& minimumSize(T&& size_value);

    template <Core::Concepts::SizeLike T>
    constexpr Widget& maximumSize(T&& size_value);

    template <Core::Concepts::SizeLike T>
    constexpr Widget& fixedSize(T&& size_value);

    Widget& sizePolicy(QSizePolicy::Policy horizontal,
                       QSizePolicy::Policy vertical);
    Widget& sizePolicy(const QSizePolicy& policy);

    template <Core::Concepts::RectLike T>
    constexpr Widget& geometry(T&& rect_value);

    template <Core::Concepts::PointLike T>
    constexpr Widget& position(T&& pos_value);

    Widget& visible(bool visible) noexcept;
    Widget& enabled(bool enabled) noexcept;

    template <Core::Concepts::StringLike T>
    Widget& toolTip(T&& tooltip);

    template <Core::Concepts::StringLike T>
    Widget& statusTip(T&& statusTip);

    template <Core::Concepts::StringLike T>
    Widget& whatsThis(T&& whatsThis);

    template <Core::Concepts::StringLike T>
    Widget& windowTitle(T&& title);

    Widget& windowIcon(const QIcon& icon);
    Widget& windowFlags(Qt::WindowFlags flags) noexcept;
    Widget& windowState(Qt::WindowStates state) noexcept;
    Widget& focusPolicy(Qt::FocusPolicy policy) noexcept;
    Widget& contextMenuPolicy(Qt::ContextMenuPolicy policy) noexcept;
    Widget& cursor(const QCursor& cursor);

    template <Core::Concepts::FontLike T>
    Widget& font(T&& font_value);

    Widget& palette(const QPalette& palette);
    Widget& autoFillBackground(bool enabled) noexcept;
    Widget& updatesEnabled(bool enabled) noexcept;

    // **Layout management with concepts**
    template <Core::Concepts::LayoutType T>
    Widget& layout(T* layout_ptr);

    Widget& vBoxLayout();
    Widget& hBoxLayout();
    Widget& gridLayout(int rows = 0, int columns = 0);
    Widget& formLayout();

    // **Widget management with concepts**
    template <Core::Concepts::QtWidget T>
    Widget& addWidget(T* widget);

    template <Core::Concepts::QtWidget T>
    Widget& addWidget(T* widget, int row, int column,
                      Qt::Alignment alignment = Qt::Alignment{});

    template <Core::Concepts::QtWidget T>
    Widget& addWidget(T* widget, int row, int column, int rowSpan,
                      int columnSpan,
                      Qt::Alignment alignment = Qt::Alignment{});

    template <Core::Concepts::LayoutType T>
    Widget& addLayout(T* layout_ptr);

    Widget& spacing(int spacing) noexcept;
    constexpr Widget& margins(int left, int top, int right,
                              int bottom) noexcept {
        if (widget_ && widget_->layout()) {
            widget_->layout()->setContentsMargins(left, top, right, bottom);
        }
        return *this;
    }
    Widget& margins(const QMargins& margins);

    template <Core::Concepts::StringLike T>
    Widget& style(T&& stylesheet);

    void initialize() override;

    // **Modern C++20 getters with [[nodiscard]]**
    [[nodiscard]] QSize getSize() const noexcept;
    [[nodiscard]] QSize getMinimumSize() const noexcept;
    [[nodiscard]] QSize getMaximumSize() const noexcept;
    [[nodiscard]] QRect getGeometry() const noexcept;
    [[nodiscard]] QPoint getPosition() const noexcept;
    [[nodiscard]] bool isVisible() const noexcept;
    [[nodiscard]] bool isEnabled() const noexcept;
    [[nodiscard]] QString getToolTip() const;
    [[nodiscard]] QLayout* getLayout() const noexcept;

    // **Widget operations**
    void show();
    void hide();
    void setFocus();
    void clearFocus();
    void update();
    void repaint();

    // **Modern C++20 utility methods**
    template <typename T>
        requires Core::Concepts::Container<T>
    Widget& addWidgets(T&& widget_container);

    template <typename F>
        requires Core::Concepts::VoidCallback<F>
    Widget& onResize(F&& resize_handler);

    template <typename F>
        requires Core::Concepts::VoidCallback<F>
    Widget& onShow(F&& show_handler);

    template <typename F>
        requires Core::Concepts::VoidCallback<F>
    Widget& onHide(F&& hide_handler);

private:
    QWidget* widget_;
};

// **Template method implementations**
template <Core::Concepts::SizeLike T>
constexpr Widget& Widget::size(T&& size_value) {
    if constexpr (std::same_as<std::decay_t<T>, QSize>) {
        return static_cast<Widget&>(
            setProperty("size", std::forward<T>(size_value)));
    } else {
        return static_cast<Widget&>(setProperty("size", QSize{size_value}));
    }
}

template <Core::Concepts::SizeLike T>
constexpr Widget& Widget::minimumSize(T&& size_value) {
    if constexpr (std::same_as<std::decay_t<T>, QSize>) {
        return static_cast<Widget&>(
            setProperty("minimumSize", std::forward<T>(size_value)));
    } else {
        return static_cast<Widget&>(
            setProperty("minimumSize", QSize{size_value}));
    }
}

template <Core::Concepts::SizeLike T>
constexpr Widget& Widget::maximumSize(T&& size_value) {
    if constexpr (std::same_as<std::decay_t<T>, QSize>) {
        return static_cast<Widget&>(
            setProperty("maximumSize", std::forward<T>(size_value)));
    } else {
        return static_cast<Widget&>(
            setProperty("maximumSize", QSize{size_value}));
    }
}

template <Core::Concepts::SizeLike T>
constexpr Widget& Widget::fixedSize(T&& size_value) {
    if (widget_) {
        if constexpr (std::same_as<std::decay_t<T>, QSize>) {
            widget_->setFixedSize(std::forward<T>(size_value));
        } else {
            widget_->setFixedSize(QSize{size_value});
        }
    }
    return *this;
}

template <Core::Concepts::PointLike T>
constexpr Widget& Widget::position(T&& pos_value) {
    if (widget_) {
        if constexpr (std::same_as<std::decay_t<T>, QPoint>) {
            widget_->move(std::forward<T>(pos_value));
        } else {
            widget_->move(QPoint{pos_value});
        }
    }
    return static_cast<Widget&>(
        setProperty("pos", std::forward<T>(pos_value)));
}

template <Core::Concepts::StringLike T>
Widget& Widget::style(T&& stylesheet) {
    if (widget_) {
        if constexpr (std::same_as<std::decay_t<T>, QString>) {
            widget_->setStyleSheet(std::forward<T>(stylesheet));
        } else {
            widget_->setStyleSheet(QString{stylesheet});
        }
    }
    return *this;
}

}  // namespace DeclarativeUI::Components
