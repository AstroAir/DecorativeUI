// Components/Widget.hpp
#pragma once
#include <QFormLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QBoxLayout>
#include <QLayout>
#include <QVBoxLayout>
#include <QWidget>
#include <concepts>

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

// === Missing template implementations (added to resolve linker errors) ===

// Geometry setter
template <Core::Concepts::RectLike T>
constexpr Widget& Widget::geometry(T&& rect_value) {
    if (widget_) {
        if constexpr (std::same_as<std::decay_t<T>, QRect>) {
            widget_->setGeometry(rect_value);
        } else {
            widget_->setGeometry(QRect{rect_value.x(), rect_value.y(), rect_value.width(), rect_value.height()});
        }
    }
    if constexpr (std::same_as<std::decay_t<T>, QRect>) {
        return static_cast<Widget&>(setProperty("geometry", std::forward<T>(rect_value)));
    } else {
        return static_cast<Widget&>(setProperty("geometry", QRect{rect_value.x(), rect_value.y(), rect_value.width(), rect_value.height()}));
    }
}

// Font setter
template <Core::Concepts::FontLike T>
Widget& Widget::font(T&& font_value) {
    if constexpr (std::same_as<std::decay_t<T>, QFont>) {
        return static_cast<Widget&>(setProperty("font", std::forward<T>(font_value)));
    } else {
        return static_cast<Widget&>(setProperty("font", QFont(font_value)));
    }
}

// Text related helpers
template <Core::Concepts::StringLike T>
Widget& Widget::toolTip(T&& tooltip) {
    if constexpr (std::same_as<std::decay_t<T>, QString>) {
        return static_cast<Widget&>(setProperty("toolTip", std::forward<T>(tooltip)));
    } else {
        return static_cast<Widget&>(setProperty("toolTip", QString{tooltip}));
    }
}

template <Core::Concepts::StringLike T>
Widget& Widget::statusTip(T&& statusTip) {
    if constexpr (std::same_as<std::decay_t<T>, QString>) {
        return static_cast<Widget&>(setProperty("statusTip", std::forward<T>(statusTip)));
    } else {
        return static_cast<Widget&>(setProperty("statusTip", QString{statusTip}));
    }
}

template <Core::Concepts::StringLike T>
Widget& Widget::whatsThis(T&& whatsThis) {
    if constexpr (std::same_as<std::decay_t<T>, QString>) {
        return static_cast<Widget&>(setProperty("whatsThis", std::forward<T>(whatsThis)));
    } else {
        return static_cast<Widget&>(setProperty("whatsThis", QString{whatsThis}));
    }
}

template <Core::Concepts::StringLike T>
Widget& Widget::windowTitle(T&& title) {
    if constexpr (std::same_as<std::decay_t<T>, QString>) {
        return static_cast<Widget&>(setProperty("windowTitle", std::forward<T>(title)));
    } else {
        return static_cast<Widget&>(setProperty("windowTitle", QString{title}));
    }
}

// Generic layout setter
template <Core::Concepts::LayoutType T>
Widget& Widget::layout(T* layout_ptr) {
    if (widget_ && layout_ptr) {
        if (widget_->layout() && widget_->layout() != layout_ptr) {
            delete widget_->layout();
        }
        layout_ptr->setParent(widget_); // ensure correct parent
        widget_->setLayout(layout_ptr);
    }
    return *this;
}

// Add a child layout to existing layout
template <Core::Concepts::LayoutType T>
Widget& Widget::addLayout(T* layout_ptr) {
    if (widget_ && widget_->layout() && layout_ptr) {
        if (auto* grid = qobject_cast<QGridLayout*>(widget_->layout())) {
            // Append layout on next available row (simple heuristic)
            int row = grid->rowCount();
            grid->addLayout(layout_ptr, row, 0, 1, grid->columnCount() > 0 ? grid->columnCount() : 1);
        } else if (auto* box = qobject_cast<QBoxLayout*>(widget_->layout())) {
            box->addLayout(layout_ptr);
        } else {
            widget_->layout()->addItem(layout_ptr);
        }
    }
    return *this;
}

// Add single widget (no position)
template <Core::Concepts::QtWidget T>
Widget& Widget::addWidget(T* child) {
    if (widget_ && child) {
        if (!widget_->layout()) {
            // default to vertical layout
            widget_->setLayout(new QVBoxLayout(widget_));
        }
        if (auto* grid = qobject_cast<QGridLayout*>(widget_->layout())) {
            int row = grid->rowCount();
            grid->addWidget(child, row, 0);
        } else if (auto* box = qobject_cast<QBoxLayout*>(widget_->layout())) {
            box->addWidget(child);
        } else {
            widget_->layout()->addWidget(child);
        }
    }
    return *this;
}

// Add widget at grid position (row, column)
template <Core::Concepts::QtWidget T>
Widget& Widget::addWidget(T* child, int row, int column, Qt::Alignment alignment) {
    if (widget_ && child) {
        if (!widget_->layout()) {
            widget_->setLayout(new QGridLayout(widget_));
        }
        if (auto* grid = qobject_cast<QGridLayout*>(widget_->layout())) {
            grid->addWidget(child, row, column, alignment);
        } else {
            // Fallback: ignore positioning; apply alignment if box layout
            if (auto* box = qobject_cast<QBoxLayout*>(widget_->layout())) {
                box->addWidget(child, alignment);
            } else {
                widget_->layout()->addWidget(child);
            }
        }
    }
    return *this;
}

// Add widget at grid position with span
template <Core::Concepts::QtWidget T>
Widget& Widget::addWidget(T* child, int row, int column, int rowSpan, int columnSpan, Qt::Alignment alignment) {
    if (widget_ && child) {
        if (!widget_->layout()) {
            widget_->setLayout(new QGridLayout(widget_));
        }
        if (auto* grid = qobject_cast<QGridLayout*>(widget_->layout())) {
            grid->addWidget(child, row, column, rowSpan, columnSpan, alignment);
        } else {
            if (auto* box = qobject_cast<QBoxLayout*>(widget_->layout())) {
                box->addWidget(child, alignment);
            } else {
                widget_->layout()->addWidget(child);
            }
        }
    }
    return *this;
}

// Add multiple widgets from a container (container holds QWidget* or derived)
template <typename T>
    requires Core::Concepts::Container<T>
Widget& Widget::addWidgets(T&& widget_container) {
    for (auto* w : widget_container) {
        if constexpr (std::is_pointer_v<decltype(w)>) {
            addWidget(w);
        }
    }
    return *this;
}

// Event hooks
template <typename F>
    requires Core::Concepts::VoidCallback<F>
Widget& Widget::onResize(F&& resize_handler) {
    return static_cast<Widget&>(onEvent("resize", std::forward<F>(resize_handler)));
}

template <typename F>
    requires Core::Concepts::VoidCallback<F>
Widget& Widget::onShow(F&& show_handler) {
    return static_cast<Widget&>(onEvent("show", std::forward<F>(show_handler)));
}

template <typename F>
    requires Core::Concepts::VoidCallback<F>
Widget& Widget::onHide(F&& hide_handler) {
    return static_cast<Widget&>(onEvent("hide", std::forward<F>(hide_handler)));
}

}  // namespace DeclarativeUI::Components
