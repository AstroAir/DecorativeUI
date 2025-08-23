/**
 * @file Widget.hpp
 * @brief Core widget component for DeclarativeUI framework
 *
 * This file defines the Widget class, which serves as the fundamental building
 * block for creating UI components in the DeclarativeUI framework. It provides
 * a modern C++20 fluent interface with concepts for type safety and declarative
 * UI construction.
 *
 * @author DeclarativeUI Team
 * @version 1.0
 * @date 2024
 */

#pragma once
#include <QBoxLayout>
#include <QFormLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLayout>
#include <QVBoxLayout>
#include <QWidget>
#include <concepts>

#include "../Core/Concepts.hpp"
#include "../Core/UIElement.hpp"

namespace DeclarativeUI::Components {

/**
 * @class Widget
 * @brief Core widget component providing fluent interface for UI construction
 *
 * The Widget class extends Core::UIElement to provide a comprehensive set of
 * methods for creating and configuring Qt widgets using a modern C++20 fluent
 * interface. It supports concepts for type safety, layout management, and
 * declarative UI construction patterns.
 *
 * Key features:
 * - Modern C++20 concepts for type safety
 * - Fluent interface for method chaining
 * - Comprehensive layout management
 * - Property binding and event handling
 * - RAII-based resource management
 *
 * @example
 * @code
 * auto widget = Widget()
 *     .size(QSize(400, 300))
 *     .windowTitle("My Application")
 *     .vBoxLayout()
 *     .addWidget(button)
 *     .show();
 * @endcode
 */
class Widget : public Core::UIElement {
    Q_OBJECT

public:
    /**
     * @brief Construct a new Widget object
     * @param parent Parent QObject for memory management
     *
     * Creates a new Widget instance with the specified parent. The widget
     * is initialized with default properties and is ready for configuration
     * through the fluent interface.
     */
    explicit Widget(QObject* parent = nullptr);

    /**
     * @name Size Management
     * @brief Methods for controlling widget size and size policies
     * @{
     */

    /**
     * @brief Set the widget size using concepts for type safety
     * @tparam T Type that satisfies SizeLike concept (QSize, etc.)
     * @param size_value The size value to set
     * @return Reference to this widget for method chaining
     *
     * Sets the widget's size using a type-safe concept-based approach.
     * Accepts QSize or compatible types that satisfy the SizeLike concept.
     */
    template <Core::Concepts::SizeLike T>
    constexpr Widget& size(T&& size_value);

    /**
     * @brief Set the minimum size for the widget
     * @tparam T Type that satisfies SizeLike concept
     * @param size_value The minimum size to set
     * @return Reference to this widget for method chaining
     */
    template <Core::Concepts::SizeLike T>
    constexpr Widget& minimumSize(T&& size_value);

    /**
     * @brief Set the maximum size for the widget
     * @tparam T Type that satisfies SizeLike concept
     * @param size_value The maximum size to set
     * @return Reference to this widget for method chaining
     */
    template <Core::Concepts::SizeLike T>
    constexpr Widget& maximumSize(T&& size_value);

    /**
     * @brief Set a fixed size for the widget (both min and max)
     * @tparam T Type that satisfies SizeLike concept
     * @param size_value The fixed size to set
     * @return Reference to this widget for method chaining
     */
    template <Core::Concepts::SizeLike T>
    constexpr Widget& fixedSize(T&& size_value);

    /**
     * @brief Set the size policy for horizontal and vertical directions
     * @param horizontal Horizontal size policy
     * @param vertical Vertical size policy
     * @return Reference to this widget for method chaining
     */
    Widget& sizePolicy(QSizePolicy::Policy horizontal,
                       QSizePolicy::Policy vertical);

    /**
     * @brief Set the size policy using a QSizePolicy object
     * @param policy The size policy to apply
     * @return Reference to this widget for method chaining
     */
    Widget& sizePolicy(const QSizePolicy& policy);

    /** @} */

    /**
     * @name Position and Geometry
     * @brief Methods for controlling widget position and geometry
     * @{
     */

    /**
     * @brief Set the widget geometry (position and size)
     * @tparam T Type that satisfies RectLike concept (QRect, etc.)
     * @param rect_value The geometry rectangle to set
     * @return Reference to this widget for method chaining
     */
    template <Core::Concepts::RectLike T>
    constexpr Widget& geometry(T&& rect_value);

    /**
     * @brief Set the widget position
     * @tparam T Type that satisfies PointLike concept (QPoint, etc.)
     * @param pos_value The position to set
     * @return Reference to this widget for method chaining
     */
    template <Core::Concepts::PointLike T>
    constexpr Widget& position(T&& pos_value);

    /**
     * @brief Set widget visibility
     * @param visible True to make widget visible, false to hide
     * @return Reference to this widget for method chaining
     */
    Widget& visible(bool visible) noexcept;

    /**
     * @brief Set widget enabled state
     * @param enabled True to enable widget, false to disable
     * @return Reference to this widget for method chaining
     */
    Widget& enabled(bool enabled) noexcept;

    /** @} */

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
    return static_cast<Widget&>(setProperty("pos", std::forward<T>(pos_value)));
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
            widget_->setGeometry(QRect{rect_value.x(), rect_value.y(),
                                       rect_value.width(),
                                       rect_value.height()});
        }
    }
    if constexpr (std::same_as<std::decay_t<T>, QRect>) {
        return static_cast<Widget&>(
            setProperty("geometry", std::forward<T>(rect_value)));
    } else {
        return static_cast<Widget&>(setProperty(
            "geometry", QRect{rect_value.x(), rect_value.y(),
                              rect_value.width(), rect_value.height()}));
    }
}

// Font setter
template <Core::Concepts::FontLike T>
Widget& Widget::font(T&& font_value) {
    if constexpr (std::same_as<std::decay_t<T>, QFont>) {
        return static_cast<Widget&>(
            setProperty("font", std::forward<T>(font_value)));
    } else {
        return static_cast<Widget&>(setProperty("font", QFont(font_value)));
    }
}

// Text related helpers
template <Core::Concepts::StringLike T>
Widget& Widget::toolTip(T&& tooltip) {
    if constexpr (std::same_as<std::decay_t<T>, QString>) {
        return static_cast<Widget&>(
            setProperty("toolTip", std::forward<T>(tooltip)));
    } else {
        return static_cast<Widget&>(setProperty("toolTip", QString{tooltip}));
    }
}

template <Core::Concepts::StringLike T>
Widget& Widget::statusTip(T&& statusTip) {
    if constexpr (std::same_as<std::decay_t<T>, QString>) {
        return static_cast<Widget&>(
            setProperty("statusTip", std::forward<T>(statusTip)));
    } else {
        return static_cast<Widget&>(
            setProperty("statusTip", QString{statusTip}));
    }
}

template <Core::Concepts::StringLike T>
Widget& Widget::whatsThis(T&& whatsThis) {
    if constexpr (std::same_as<std::decay_t<T>, QString>) {
        return static_cast<Widget&>(
            setProperty("whatsThis", std::forward<T>(whatsThis)));
    } else {
        return static_cast<Widget&>(
            setProperty("whatsThis", QString{whatsThis}));
    }
}

template <Core::Concepts::StringLike T>
Widget& Widget::windowTitle(T&& title) {
    if constexpr (std::same_as<std::decay_t<T>, QString>) {
        return static_cast<Widget&>(
            setProperty("windowTitle", std::forward<T>(title)));
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
        layout_ptr->setParent(widget_);  // ensure correct parent
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
            grid->addLayout(layout_ptr, row, 0, 1,
                            grid->columnCount() > 0 ? grid->columnCount() : 1);
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
Widget& Widget::addWidget(T* child, int row, int column,
                          Qt::Alignment alignment) {
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
Widget& Widget::addWidget(T* child, int row, int column, int rowSpan,
                          int columnSpan, Qt::Alignment alignment) {
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
    return static_cast<Widget&>(
        onEvent("resize", std::forward<F>(resize_handler)));
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
