// Components/GroupBox.hpp
#pragma once

#include <QCheckBox>
#include <QFont>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QPainter>
#include <QStyle>
#include <QStyleOption>
#include <QVBoxLayout>
#include <functional>


#include "../Core/UIElement.hpp"

namespace DeclarativeUI::Components {

/**
 * @brief A comprehensive group box component for organizing related controls
 *
 * Features:
 * - Customizable title and appearance
 * - Checkable group box functionality
 * - Flexible layout management
 * - Collapsible content
 * - Custom styling options
 * - Accessibility support
 */
class GroupBox : public Core::UIElement {
    Q_OBJECT

public:
    explicit GroupBox(QObject* parent = nullptr);
    explicit GroupBox(const QString& title, QObject* parent = nullptr);
    virtual ~GroupBox() = default;

    // **Fluent Interface for Group Box Configuration**
    GroupBox& setTitle(const QString& title);
    GroupBox& setCheckable(bool checkable);
    GroupBox& setChecked(bool checked);
    GroupBox& setAlignment(Qt::Alignment alignment);
    GroupBox& setFlat(bool flat);

    // **Layout Management**
    GroupBox& setVBoxLayout();
    GroupBox& setHBoxLayout();
    GroupBox& setGridLayout(int rows = 0, int cols = 0);
    GroupBox& setFormLayout();
    GroupBox& addWidget(QWidget* widget);
    GroupBox& addWidget(QWidget* widget, int row, int col);
    GroupBox& addWidget(QWidget* widget, int row, int col, int rowSpan, int colSpan);
    GroupBox& addRow(const QString& label, QWidget* field);
    GroupBox& addRow(QWidget* label, QWidget* field);
    GroupBox& addStretch(int stretch = 0);
    GroupBox& addSpacing(int size);
    GroupBox& setSpacing(int spacing);
    GroupBox& setMargins(int left, int top, int right, int bottom);
    GroupBox& setMargins(int margin);

    // **Styling**
    GroupBox& setTitleFont(const QFont& font);
    GroupBox& setTitleColor(const QColor& color);
    GroupBox& setBackgroundColor(const QColor& color);
    GroupBox& setBorderColor(const QColor& color);
    GroupBox& setBorderRadius(int radius);
    GroupBox& setStyleSheet(const QString& styleSheet);

    // **Event Handlers**
    GroupBox& onToggled(std::function<void(bool)> handler);
    GroupBox& onClicked(std::function<void(bool)> handler);

    // **Enhanced fluent interface**
    GroupBox& tooltip(const QString& tooltip_text);
    GroupBox& accessibleName(const QString& name);
    GroupBox& accessibleDescription(const QString& description);
    GroupBox& shortcut(const QKeySequence& shortcut);

    // **Validation**
    GroupBox& required(bool required = true);
    GroupBox& validator(std::function<bool()> validation_func);
    GroupBox& onValidationFailed(std::function<void(const QString&)> error_handler);
    GroupBox& validateChildren(bool validate_children = true);

    // **Visual enhancements**
    GroupBox& dropShadow(bool enabled = true, const QColor& color = QColor(0, 0, 0, 80));
    GroupBox& hoverEffect(bool enabled = true);
    GroupBox& focusEffect(bool enabled = true);
    GroupBox& customColors(const QColor& background, const QColor& border = QColor(), const QColor& title = QColor());
    GroupBox& gradient(const QColor& start, const QColor& end, Qt::Orientation orientation = Qt::Vertical);

    // **Advanced event handlers**
    GroupBox& onHover(std::function<void(bool)> hover_handler);
    GroupBox& onFocus(std::function<void(bool)> focus_handler);
    GroupBox& onValidationChanged(std::function<void(bool, const QString&)> validation_handler);
    GroupBox& onChildValidationChanged(std::function<void(QWidget*, bool, const QString&)> child_validation_handler);

    // **Accessibility**
    GroupBox& role(const QString& aria_role);
    GroupBox& tabIndex(int index);
    GroupBox& describedBy(const QString& element_id);
    GroupBox& labelledBy(const QString& element_id);

    // **Getters**
    QString getTitle() const;
    bool isCheckable() const;
    bool isChecked() const;
    Qt::Alignment getAlignment() const;
    bool isFlat() const;
    QLayout* getLayout() const;

    // **Advanced Features**
    GroupBox& setCollapsible(bool collapsible);
    GroupBox& setCollapsed(bool collapsed);
    bool isCollapsed() const;
    GroupBox& setMinimumContentHeight(int height);
    GroupBox& setMaximumContentHeight(int height);

    // **Utility Methods**
    GroupBox& clear();
    GroupBox& removeWidget(QWidget* widget);
    int getWidgetCount() const;
    QWidget* getWidget(int index) const;
    int indexOf(QWidget* widget) const;

    // **Animation Support**
    GroupBox& setAnimated(bool animated);
    GroupBox& setAnimationDuration(int duration);

    // **Initialization**
    void initialize() override;

private:
    QLayout* current_layout_ = nullptr;
    bool is_collapsible_ = false;
    bool is_animated_ = false;
    int animation_duration_ = 300;
    int min_content_height_ = 0;
    int max_content_height_ = 16777215;
    QString title_;  // Store title for deferred widget creation

    void setupWidget();
};

}  // namespace DeclarativeUI::Components
