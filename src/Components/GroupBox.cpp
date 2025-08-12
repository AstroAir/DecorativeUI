// Components/GroupBox.cpp
#include "GroupBox.hpp"

namespace DeclarativeUI::Components {

// **Constructors**
GroupBox::GroupBox(QObject* parent)
    : UIElement(parent), current_layout_(nullptr), is_collapsible_(false),
      is_animated_(false), animation_duration_(300), min_content_height_(0),
      max_content_height_(16777215) {
    // Widget creation is deferred to initialize() method
}

GroupBox::GroupBox(const QString& title, QObject* parent)
    : UIElement(parent), current_layout_(nullptr), is_collapsible_(false),
      is_animated_(false), animation_duration_(300), min_content_height_(0),
      max_content_height_(16777215), title_(title) {
    // Widget creation is deferred to initialize() method
}

// **Fluent Interface for Group Box Configuration**
GroupBox& GroupBox::setTitle(const QString& title) {
    title_ = title;  // Store for deferred creation
    if (auto groupBox = qobject_cast<QGroupBox*>(UIElement::getWidget())) {
        groupBox->setTitle(title);
    }
    return *this;
}

GroupBox& GroupBox::setCheckable(bool checkable) {
    if (auto groupBox = qobject_cast<QGroupBox*>(UIElement::getWidget())) {
        groupBox->setCheckable(checkable);
    }
    return *this;
}

GroupBox& GroupBox::setChecked(bool checked) {
    if (auto groupBox = qobject_cast<QGroupBox*>(UIElement::getWidget())) {
        groupBox->setChecked(checked);
    }
    return *this;
}

GroupBox& GroupBox::setAlignment(Qt::Alignment alignment) {
    if (auto groupBox = qobject_cast<QGroupBox*>(UIElement::getWidget())) {
        groupBox->setAlignment(alignment);
    }
    return *this;
}

GroupBox& GroupBox::setFlat(bool flat) {
    if (auto groupBox = qobject_cast<QGroupBox*>(UIElement::getWidget())) {
        groupBox->setFlat(flat);
    }
    return *this;
}

// **Layout Management**
GroupBox& GroupBox::setVBoxLayout() {
    if (auto groupBox = qobject_cast<QGroupBox*>(UIElement::getWidget())) {
        auto layout = new QVBoxLayout();
        groupBox->setLayout(layout);
        current_layout_ = layout;
    }
    return *this;
}

GroupBox& GroupBox::setHBoxLayout() {
    if (auto groupBox = qobject_cast<QGroupBox*>(UIElement::getWidget())) {
        auto layout = new QHBoxLayout();
        groupBox->setLayout(layout);
        current_layout_ = layout;
    }
    return *this;
}

GroupBox& GroupBox::setGridLayout(int rows, int cols) {
    if (auto groupBox = qobject_cast<QGroupBox*>(UIElement::getWidget())) {
        auto layout = new QGridLayout();
        groupBox->setLayout(layout);
        current_layout_ = layout;
    }
    return *this;
}

GroupBox& GroupBox::setFormLayout() {
    if (auto groupBox = qobject_cast<QGroupBox*>(UIElement::getWidget())) {
        auto layout = new QFormLayout();
        groupBox->setLayout(layout);
        current_layout_ = layout;
    }
    return *this;
}

GroupBox& GroupBox::addWidget(QWidget* widget) {
    if (current_layout_) {
        current_layout_->addWidget(widget);
    }
    return *this;
}

GroupBox& GroupBox::addWidget(QWidget* widget, int row, int col) {
    if (auto gridLayout = qobject_cast<QGridLayout*>(current_layout_)) {
        gridLayout->addWidget(widget, row, col);
    }
    return *this;
}

GroupBox& GroupBox::addWidget(QWidget* widget, int row, int col, int rowSpan, int colSpan) {
    if (auto gridLayout = qobject_cast<QGridLayout*>(current_layout_)) {
        gridLayout->addWidget(widget, row, col, rowSpan, colSpan);
    }
    return *this;
}

GroupBox& GroupBox::addRow(const QString& label, QWidget* field) {
    if (auto formLayout = qobject_cast<QFormLayout*>(current_layout_)) {
        formLayout->addRow(label, field);
    }
    return *this;
}

GroupBox& GroupBox::addRow(QWidget* label, QWidget* field) {
    if (auto formLayout = qobject_cast<QFormLayout*>(current_layout_)) {
        formLayout->addRow(label, field);
    }
    return *this;
}

GroupBox& GroupBox::addStretch(int stretch) {
    if (auto boxLayout = qobject_cast<QBoxLayout*>(current_layout_)) {
        boxLayout->addStretch(stretch);
    }
    return *this;
}

GroupBox& GroupBox::addSpacing(int size) {
    if (auto boxLayout = qobject_cast<QBoxLayout*>(current_layout_)) {
        boxLayout->addSpacing(size);
    }
    return *this;
}

GroupBox& GroupBox::setSpacing(int spacing) {
    if (current_layout_) {
        current_layout_->setSpacing(spacing);
    }
    return *this;
}

GroupBox& GroupBox::setMargins(int left, int top, int right, int bottom) {
    if (current_layout_) {
        current_layout_->setContentsMargins(left, top, right, bottom);
    }
    return *this;
}

GroupBox& GroupBox::setMargins(int margin) {
    return setMargins(margin, margin, margin, margin);
}

// **Styling**
GroupBox& GroupBox::setTitleFont(const QFont& font) {
    if (auto groupBox = qobject_cast<QGroupBox*>(UIElement::getWidget())) {
        groupBox->setFont(font);
    }
    return *this;
}

GroupBox& GroupBox::setTitleColor(const QColor& color) {
    if (auto groupBox = qobject_cast<QGroupBox*>(UIElement::getWidget())) {
        QPalette palette = groupBox->palette();
        palette.setColor(QPalette::WindowText, color);
        groupBox->setPalette(palette);
    }
    return *this;
}

GroupBox& GroupBox::setBackgroundColor(const QColor& color) {
    if (auto groupBox = qobject_cast<QGroupBox*>(UIElement::getWidget())) {
        groupBox->setStyleSheet(
            QString("QGroupBox { background-color: %1; }")
                .arg(color.name()));
    }
    return *this;
}

GroupBox& GroupBox::setBorderColor(const QColor& color) {
    if (auto groupBox = qobject_cast<QGroupBox*>(UIElement::getWidget())) {
        groupBox->setStyleSheet(
            QString("QGroupBox { border: 1px solid %1; }")
                .arg(color.name()));
    }
    return *this;
}

GroupBox& GroupBox::setBorderRadius(int radius) {
    if (auto groupBox = qobject_cast<QGroupBox*>(UIElement::getWidget())) {
        groupBox->setStyleSheet(
            QString("QGroupBox { border-radius: %1px; }").arg(radius));
    }
    return *this;
}

GroupBox& GroupBox::setStyleSheet(const QString& styleSheet) {
    if (auto groupBox = qobject_cast<QGroupBox*>(UIElement::getWidget())) {
        groupBox->setStyleSheet(styleSheet);
    }
    return *this;
}

// **Event Handlers**
GroupBox& GroupBox::onToggled(std::function<void(bool)> handler) {
    if (auto groupBox = qobject_cast<QGroupBox*>(UIElement::getWidget())) {
        QObject::connect(groupBox, &QGroupBox::toggled, handler);
    }
    return *this;
}

GroupBox& GroupBox::onClicked(std::function<void(bool)> handler) {
    if (auto groupBox = qobject_cast<QGroupBox*>(UIElement::getWidget())) {
        QObject::connect(groupBox, &QGroupBox::clicked, handler);
    }
    return *this;
}

// **Getters**
QString GroupBox::getTitle() const {
    if (auto groupBox = qobject_cast<QGroupBox*>(UIElement::getWidget())) {
        return groupBox->title();
    }
    return title_;  // Return stored title if widget not created yet
}

bool GroupBox::isCheckable() const {
    if (auto groupBox = qobject_cast<QGroupBox*>(UIElement::getWidget())) {
        return groupBox->isCheckable();
    }
    return false;
}

bool GroupBox::isChecked() const {
    if (auto groupBox = qobject_cast<QGroupBox*>(UIElement::getWidget())) {
        return groupBox->isChecked();
    }
    return false;
}

Qt::Alignment GroupBox::getAlignment() const {
    if (auto groupBox = qobject_cast<QGroupBox*>(UIElement::getWidget())) {
        return groupBox->alignment();
    }
    return Qt::AlignLeft;
}

bool GroupBox::isFlat() const {
    if (auto groupBox = qobject_cast<QGroupBox*>(UIElement::getWidget())) {
        return groupBox->isFlat();
    }
    return false;
}

QLayout* GroupBox::getLayout() const {
    return current_layout_;
}

// **Advanced Features**
GroupBox& GroupBox::setCollapsible(bool collapsible) {
    is_collapsible_ = collapsible;
    if (collapsible) {
        setCheckable(true);
        onToggled([this](bool checked) {
            if (auto groupBox = qobject_cast<QGroupBox*>(UIElement::getWidget())) {
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

GroupBox& GroupBox::setCollapsed(bool collapsed) {
    if (is_collapsible_) {
        setChecked(!collapsed);
    }
    return *this;
}

bool GroupBox::isCollapsed() const {
    return is_collapsible_ && !isChecked();
}

GroupBox& GroupBox::setMinimumContentHeight(int height) {
    min_content_height_ = height;
    return *this;
}

GroupBox& GroupBox::setMaximumContentHeight(int height) {
    max_content_height_ = height;
    return *this;
}

// **Utility Methods**
GroupBox& GroupBox::clear() {
    if (current_layout_) {
        QLayoutItem* item;
        while ((item = current_layout_->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
    }
    return *this;
}

GroupBox& GroupBox::removeWidget(QWidget* widget) {
    if (current_layout_) {
        current_layout_->removeWidget(widget);
    }
    return *this;
}

int GroupBox::getWidgetCount() const {
    if (current_layout_) {
        return current_layout_->count();
    }
    return 0;
}

QWidget* GroupBox::getWidget(int index) const {
    if (current_layout_ && index >= 0 && index < current_layout_->count()) {
        QLayoutItem* item = current_layout_->itemAt(index);
        if (item) {
            return item->widget();
        }
    }
    return nullptr;
}

int GroupBox::indexOf(QWidget* widget) const {
    if (current_layout_) {
        return current_layout_->indexOf(widget);
    }
    return -1;
}

// **Animation Support**
GroupBox& GroupBox::setAnimated(bool animated) {
    is_animated_ = animated;
    return *this;
}

GroupBox& GroupBox::setAnimationDuration(int duration) {
    animation_duration_ = duration;
    return *this;
}

// **Initialization**
void GroupBox::initialize() {
    if (!UIElement::getWidget()) {
        QGroupBox* groupBox;
        if (!title_.isEmpty()) {
            groupBox = new QGroupBox(title_);
        } else {
            groupBox = new QGroupBox();
        }
        setWidget(groupBox);
        setupWidget();
    }
}

// **Private Helper Methods**
void GroupBox::setupWidget() {
    if (auto groupBox = qobject_cast<QGroupBox*>(UIElement::getWidget())) {
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

}  // namespace DeclarativeUI::Components
