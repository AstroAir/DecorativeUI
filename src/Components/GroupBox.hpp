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
public:
    GroupBox() {
        widget_ = std::make_unique<QGroupBox>();
        setupWidget();
    }

    explicit GroupBox(const QString& title) {
        widget_ = std::make_unique<QGroupBox>(title);
        setupWidget();
    }

    virtual ~GroupBox() = default;

    // **Fluent Interface for Group Box Configuration**
    GroupBox& setTitle(const QString& title) {
        if (auto groupBox = qobject_cast<QGroupBox*>(widget_.get())) {
            groupBox->setTitle(title);
        }
        return *this;
    }

    GroupBox& setCheckable(bool checkable) {
        if (auto groupBox = qobject_cast<QGroupBox*>(widget_.get())) {
            groupBox->setCheckable(checkable);
        }
        return *this;
    }

    GroupBox& setChecked(bool checked) {
        if (auto groupBox = qobject_cast<QGroupBox*>(widget_.get())) {
            groupBox->setChecked(checked);
        }
        return *this;
    }

    GroupBox& setAlignment(Qt::Alignment alignment) {
        if (auto groupBox = qobject_cast<QGroupBox*>(widget_.get())) {
            groupBox->setAlignment(alignment);
        }
        return *this;
    }

    GroupBox& setFlat(bool flat) {
        if (auto groupBox = qobject_cast<QGroupBox*>(widget_.get())) {
            groupBox->setFlat(flat);
        }
        return *this;
    }

    // **Layout Management**
    GroupBox& setVBoxLayout() {
        if (auto groupBox = qobject_cast<QGroupBox*>(widget_.get())) {
            auto layout = new QVBoxLayout();
            groupBox->setLayout(layout);
            current_layout_ = layout;
        }
        return *this;
    }

    GroupBox& setHBoxLayout() {
        if (auto groupBox = qobject_cast<QGroupBox*>(widget_.get())) {
            auto layout = new QHBoxLayout();
            groupBox->setLayout(layout);
            current_layout_ = layout;
        }
        return *this;
    }

    GroupBox& setGridLayout(int rows = 0, int cols = 0) {
        if (auto groupBox = qobject_cast<QGroupBox*>(widget_.get())) {
            auto layout = new QGridLayout();
            groupBox->setLayout(layout);
            current_layout_ = layout;
        }
        return *this;
    }

    GroupBox& setFormLayout() {
        if (auto groupBox = qobject_cast<QGroupBox*>(widget_.get())) {
            auto layout = new QFormLayout();
            groupBox->setLayout(layout);
            current_layout_ = layout;
        }
        return *this;
    }

    GroupBox& addWidget(QWidget* widget) {
        if (current_layout_) {
            current_layout_->addWidget(widget);
        }
        return *this;
    }

    GroupBox& addWidget(QWidget* widget, int row, int col) {
        if (auto gridLayout = qobject_cast<QGridLayout*>(current_layout_)) {
            gridLayout->addWidget(widget, row, col);
        }
        return *this;
    }

    GroupBox& addWidget(QWidget* widget, int row, int col, int rowSpan,
                        int colSpan) {
        if (auto gridLayout = qobject_cast<QGridLayout*>(current_layout_)) {
            gridLayout->addWidget(widget, row, col, rowSpan, colSpan);
        }
        return *this;
    }

    GroupBox& addRow(const QString& label, QWidget* field) {
        if (auto formLayout = qobject_cast<QFormLayout*>(current_layout_)) {
            formLayout->addRow(label, field);
        }
        return *this;
    }

    GroupBox& addRow(QWidget* label, QWidget* field) {
        if (auto formLayout = qobject_cast<QFormLayout*>(current_layout_)) {
            formLayout->addRow(label, field);
        }
        return *this;
    }

    GroupBox& addStretch(int stretch = 0) {
        if (auto boxLayout = qobject_cast<QBoxLayout*>(current_layout_)) {
            boxLayout->addStretch(stretch);
        }
        return *this;
    }

    GroupBox& addSpacing(int size) {
        if (auto boxLayout = qobject_cast<QBoxLayout*>(current_layout_)) {
            boxLayout->addSpacing(size);
        }
        return *this;
    }

    GroupBox& setSpacing(int spacing) {
        if (current_layout_) {
            current_layout_->setSpacing(spacing);
        }
        return *this;
    }

    GroupBox& setMargins(int left, int top, int right, int bottom) {
        if (current_layout_) {
            current_layout_->setContentsMargins(left, top, right, bottom);
        }
        return *this;
    }

    GroupBox& setMargins(int margin) {
        return setMargins(margin, margin, margin, margin);
    }

    // **Styling**
    GroupBox& setTitleFont(const QFont& font) {
        if (auto groupBox = qobject_cast<QGroupBox*>(widget_.get())) {
            groupBox->setFont(font);
        }
        return *this;
    }

    GroupBox& setTitleColor(const QColor& color) {
        if (auto groupBox = qobject_cast<QGroupBox*>(widget_.get())) {
            QPalette palette = groupBox->palette();
            palette.setColor(QPalette::WindowText, color);
            groupBox->setPalette(palette);
        }
        return *this;
    }

    GroupBox& setBackgroundColor(const QColor& color) {
        if (auto groupBox = qobject_cast<QGroupBox*>(widget_.get())) {
            groupBox->setStyleSheet(
                QString("QGroupBox { background-color: %1; }")
                    .arg(color.name()));
        }
        return *this;
    }

    GroupBox& setBorderColor(const QColor& color) {
        if (auto groupBox = qobject_cast<QGroupBox*>(widget_.get())) {
            groupBox->setStyleSheet(
                QString("QGroupBox { border: 1px solid %1; }")
                    .arg(color.name()));
        }
        return *this;
    }

    GroupBox& setBorderRadius(int radius) {
        if (auto groupBox = qobject_cast<QGroupBox*>(widget_.get())) {
            groupBox->setStyleSheet(
                QString("QGroupBox { border-radius: %1px; }").arg(radius));
        }
        return *this;
    }

    GroupBox& setStyleSheet(const QString& styleSheet) {
        if (auto groupBox = qobject_cast<QGroupBox*>(widget_.get())) {
            groupBox->setStyleSheet(styleSheet);
        }
        return *this;
    }

    // **Event Handlers**
    GroupBox& onToggled(std::function<void(bool)> handler) {
        if (auto groupBox = qobject_cast<QGroupBox*>(widget_.get())) {
            QObject::connect(groupBox, &QGroupBox::toggled, handler);
        }
        return *this;
    }

    GroupBox& onClicked(std::function<void(bool)> handler) {
        if (auto groupBox = qobject_cast<QGroupBox*>(widget_.get())) {
            QObject::connect(groupBox, &QGroupBox::clicked, handler);
        }
        return *this;
    }

    // **Getters**
    QString getTitle() const {
        if (auto groupBox = qobject_cast<QGroupBox*>(widget_.get())) {
            return groupBox->title();
        }
        return QString();
    }

    bool isCheckable() const {
        if (auto groupBox = qobject_cast<QGroupBox*>(widget_.get())) {
            return groupBox->isCheckable();
        }
        return false;
    }

    bool isChecked() const {
        if (auto groupBox = qobject_cast<QGroupBox*>(widget_.get())) {
            return groupBox->isChecked();
        }
        return false;
    }

    Qt::Alignment getAlignment() const {
        if (auto groupBox = qobject_cast<QGroupBox*>(widget_.get())) {
            return groupBox->alignment();
        }
        return Qt::AlignLeft;
    }

    bool isFlat() const {
        if (auto groupBox = qobject_cast<QGroupBox*>(widget_.get())) {
            return groupBox->isFlat();
        }
        return false;
    }

    QLayout* getLayout() const { return current_layout_; }

    // **Advanced Features**
    GroupBox& setCollapsible(bool collapsible) {
        is_collapsible_ = collapsible;
        if (collapsible) {
            setCheckable(true);
            onToggled([this](bool checked) {
                if (auto groupBox = qobject_cast<QGroupBox*>(widget_.get())) {
                    for (int i = 0; i < groupBox->children().size(); ++i) {
                        if (auto child = qobject_cast<QWidget*>(
                                groupBox->children().at(i))) {
                            child->setVisible(checked);
                        }
                    }
                }
            });
        }
        return *this;
    }

    GroupBox& setCollapsed(bool collapsed) {
        if (is_collapsible_) {
            setChecked(!collapsed);
        }
        return *this;
    }

    bool isCollapsed() const { return is_collapsible_ && !isChecked(); }

    GroupBox& setMinimumContentHeight(int height) {
        min_content_height_ = height;
        return *this;
    }

    GroupBox& setMaximumContentHeight(int height) {
        max_content_height_ = height;
        return *this;
    }

    // **Utility Methods**
    GroupBox& clear() {
        if (current_layout_) {
            QLayoutItem* item;
            while ((item = current_layout_->takeAt(0)) != nullptr) {
                delete item->widget();
                delete item;
            }
        }
        return *this;
    }

    GroupBox& removeWidget(QWidget* widget) {
        if (current_layout_) {
            current_layout_->removeWidget(widget);
        }
        return *this;
    }

    int getWidgetCount() const {
        if (current_layout_) {
            return current_layout_->count();
        }
        return 0;
    }

    QWidget* getWidget(int index) const {
        if (current_layout_ && index >= 0 && index < current_layout_->count()) {
            QLayoutItem* item = current_layout_->itemAt(index);
            if (item) {
                return item->widget();
            }
        }
        return nullptr;
    }

    int indexOf(QWidget* widget) const {
        if (current_layout_) {
            return current_layout_->indexOf(widget);
        }
        return -1;
    }

    // **Animation Support**
    GroupBox& setAnimated(bool animated) {
        is_animated_ = animated;
        return *this;
    }

    GroupBox& setAnimationDuration(int duration) {
        animation_duration_ = duration;
        return *this;
    }

private:
    QLayout* current_layout_ = nullptr;
    bool is_collapsible_ = false;
    bool is_animated_ = false;
    int animation_duration_ = 300;
    int min_content_height_ = 0;
    int max_content_height_ = 16777215;

    void setupWidget() {
        if (auto groupBox = qobject_cast<QGroupBox*>(widget_.get())) {
            // **Default configuration**
            groupBox->setAlignment(Qt::AlignLeft);
            groupBox->setFlat(false);
            groupBox->setCheckable(false);

            // **Set default layout**
            setVBoxLayout();

            // **Enable accessibility**
            groupBox->setAccessibleName("Group Box");
            groupBox->setAccessibleDescription(
                "A container for grouping related controls");
        }
    }
};

}  // namespace DeclarativeUI::Components
