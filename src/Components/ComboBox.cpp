// Components/ComboBox.cpp
#include "ComboBox.hpp"

namespace DeclarativeUI::Components {

// **Implementation**
ComboBox::ComboBox(QObject* parent)
    : UIElement(parent), combobox_widget_(nullptr) {}

ComboBox& ComboBox::items(const QStringList& items) {
    initial_items_ = items;
    return *this;
}

ComboBox& ComboBox::addItem(const QString& item) {
    initial_items_.append(item);
    return *this;
}

ComboBox& ComboBox::addItems(const QStringList& items) {
    initial_items_.append(items);
    return *this;
}

ComboBox& ComboBox::currentIndex(int index) {
    return static_cast<ComboBox&>(setProperty("currentIndex", index));
}

ComboBox& ComboBox::currentText(const QString& text) {
    return static_cast<ComboBox&>(setProperty("currentText", text));
}

ComboBox& ComboBox::editable(bool editable) {
    return static_cast<ComboBox&>(setProperty("editable", editable));
}

ComboBox& ComboBox::maxVisibleItems(int maxItems) {
    return static_cast<ComboBox&>(setProperty("maxVisibleItems", maxItems));
}

ComboBox& ComboBox::onCurrentIndexChanged(
    std::function<void(int)> handler) {
    current_index_changed_handler_ = std::move(handler);
    return *this;
}

ComboBox& ComboBox::onCurrentTextChanged(
    std::function<void(const QString&)> handler) {
    current_text_changed_handler_ = std::move(handler);
    return *this;
}

ComboBox& ComboBox::onActivated(std::function<void(int)> handler) {
    activated_handler_ = std::move(handler);
    return *this;
}

ComboBox& ComboBox::style(const QString& stylesheet) {
    return static_cast<ComboBox&>(setProperty("styleSheet", stylesheet));
}

void ComboBox::initialize() {
    if (!combobox_widget_) {
        combobox_widget_ = new QComboBox();
        setWidget(combobox_widget_);

        // Add initial items
        if (!initial_items_.isEmpty()) {
            combobox_widget_->addItems(initial_items_);
        }

        // Apply stored properties after items are added
        // This ensures currentIndex is set correctly
        const auto& properties = getProperties();
        for (const auto& [name, value] : properties) {
            if (name == "currentIndex") {
                std::visit([this](const auto& val) {
                    if constexpr (std::is_same_v<std::decay_t<decltype(val)>, int>) {
                        combobox_widget_->setCurrentIndex(val);
                    }
                }, value);
            }
        }

        // Connect signals
        if (current_index_changed_handler_) {
            connect(
                combobox_widget_,
                QOverload<int>::of(&QComboBox::currentIndexChanged), this,
                [this](int index) { current_index_changed_handler_(index); });
        }

        if (current_text_changed_handler_) {
            connect(combobox_widget_, &QComboBox::currentTextChanged, this,
                    [this](const QString& text) {
                        current_text_changed_handler_(text);
                    });
        }

        if (activated_handler_) {
            connect(combobox_widget_, QOverload<int>::of(&QComboBox::activated),
                    this, [this](int index) { activated_handler_(index); });
        }
    }
}

int ComboBox::getCurrentIndex() const {
    return combobox_widget_ ? combobox_widget_->currentIndex() : -1;
}

QString ComboBox::getCurrentText() const {
    return combobox_widget_ ? combobox_widget_->currentText() : QString();
}

void ComboBox::setCurrentIndex(int index) {
    if (combobox_widget_) {
        combobox_widget_->setCurrentIndex(index);
    }
}

void ComboBox::setCurrentText(const QString& text) {
    if (combobox_widget_) {
        combobox_widget_->setCurrentText(text);
    }
}

void ComboBox::clearItems() {
    if (combobox_widget_) {
        combobox_widget_->clear();
    }
    initial_items_.clear();
}

// **Missing method implementations for enhanced ComboBox**
void ComboBox::onCurrentIndexChangedInternal(int index) {
    // Enhanced index change handling
    if (current_index_changed_handler_) {
        current_index_changed_handler_(index);
    }
}

void ComboBox::onCurrentTextChangedInternal(const QString& text) {
    // Enhanced text change handling
    if (current_text_changed_handler_) {
        current_text_changed_handler_(text);
    }
}

void ComboBox::onValidationTimer() {
    // Enhanced validation handling
    // Implementation would go here for validation logic
}

void ComboBox::onItemsUpdateTimer() {
    // Enhanced items update handling
    if (dynamic_items_provider_) {
        QString current_text = combobox_widget_ ? combobox_widget_->currentText() : QString();
        updateItems(current_text);
    }
}

// **Visual enhancements implementation**
ComboBox& ComboBox::dropShadow(bool enabled, const QColor& color) {
    drop_shadow_enabled_ = enabled;
    shadow_color_ = color;
    return *this;
}

ComboBox& ComboBox::hoverEffect(bool enabled) {
    hover_effect_enabled_ = enabled;
    return *this;
}

ComboBox& ComboBox::expandAnimation(bool enabled) {
    expand_animation_enabled_ = enabled;
    return *this;
}

ComboBox& ComboBox::borderRadius(int radius) {
    border_radius_ = radius;
    return *this;
}

ComboBox& ComboBox::customColors(const QColor& background, const QColor& text, const QColor& border) {
    background_color_ = background;
    text_color_ = text;
    border_color_ = border;
    return *this;
}

ComboBox& ComboBox::iconSize(const QSize& size) {
    icon_size_ = size;
    return *this;
}

ComboBox& ComboBox::itemIcon(int index, const QIcon& icon) {
    if (combobox_widget_ && index >= 0 && index < combobox_widget_->count()) {
        combobox_widget_->setItemIcon(index, icon);
    }
    return *this;
}

ComboBox& ComboBox::itemData(int index, const QVariant& data, int role) {
    if (combobox_widget_ && index >= 0 && index < combobox_widget_->count()) {
        combobox_widget_->setItemData(index, data, role);
    }
    return *this;
}

// **State management implementation**
ComboBox& ComboBox::disabled(bool disabled, const QString& reason) {
    disabled_state_ = disabled;
    disabled_reason_ = reason;
    if (combobox_widget_) {
        combobox_widget_->setEnabled(!disabled);
        if (disabled && !reason.isEmpty()) {
            combobox_widget_->setToolTip(reason);
        }
    }
    return *this;
}

ComboBox& ComboBox::readOnly(bool readonly) {
    read_only_ = readonly;
    if (combobox_widget_) {
        combobox_widget_->setEditable(!readonly);
    }
    return *this;
}

ComboBox& ComboBox::loading(bool loading_state) {
    loading_state_ = loading_state;
    if (loading_state) {
        original_placeholder_ = placeholder_text_;
        placeholder_text_ = loading_text_.isEmpty() ? "Loading..." : loading_text_;
    } else {
        placeholder_text_ = original_placeholder_;
    }
    return *this;
}

ComboBox& ComboBox::loadingText(const QString& text) {
    loading_text_ = text;
    return *this;
}

// **Event handlers implementation**
ComboBox& ComboBox::onHover(std::function<void(bool)> hover_handler) {
    hover_handler_ = std::move(hover_handler);
    return *this;
}

ComboBox& ComboBox::onFocus(std::function<void(bool)> focus_handler) {
    focus_handler_ = std::move(focus_handler);
    return *this;
}

ComboBox& ComboBox::onDropdownToggle(std::function<void(bool)> toggle_handler) {
    dropdown_toggle_handler_ = std::move(toggle_handler);
    return *this;
}

ComboBox& ComboBox::onItemHighlighted(std::function<void(int)> highlight_handler) {
    item_highlighted_handler_ = std::move(highlight_handler);
    return *this;
}

ComboBox& ComboBox::onValidationChanged(std::function<void(bool, const QString&)> validation_handler) {
    validation_handler_ = std::move(validation_handler);
    return *this;
}

ComboBox& ComboBox::onItemsChanged(std::function<void()> items_changed_handler) {
    items_changed_handler_ = std::move(items_changed_handler);
    return *this;
}

// **Accessibility implementation**
ComboBox& ComboBox::role(const QString& aria_role) {
    aria_role_ = aria_role;
    return *this;
}

ComboBox& ComboBox::tabIndex(int index) {
    tab_index_ = index;
    return *this;
}

ComboBox& ComboBox::describedBy(const QString& element_id) {
    described_by_ = element_id;
    return *this;
}

ComboBox& ComboBox::labelledBy(const QString& element_id) {
    labelled_by_ = element_id;
    return *this;
}

// **Advanced features implementation**
ComboBox& ComboBox::multiSelect(bool enabled) {
    multi_select_enabled_ = enabled;
    return *this;
}

ComboBox& ComboBox::separator(int index, const QString& text) {
    separators_[index] = text;
    return *this;
}

ComboBox& ComboBox::groupItems(const QMap<QString, QStringList>& groups) {
    item_groups_ = groups;
    return *this;
}

ComboBox& ComboBox::customItemDelegate(QAbstractItemDelegate* delegate) {
    custom_delegate_ = delegate;
    if (combobox_widget_) {
        combobox_widget_->setItemDelegate(delegate);
    }
    return *this;
}

// **State queries implementation**
QStringList ComboBox::getAllItems() const {
    QStringList items;
    if (combobox_widget_) {
        for (int i = 0; i < combobox_widget_->count(); ++i) {
            items << combobox_widget_->itemText(i);
        }
    }
    return items;
}

QStringList ComboBox::getSelectedItems() const {
    // For multi-select implementation
    QStringList selected;
    if (multi_select_enabled_ && combobox_widget_) {
        // Implementation would depend on multi-select mechanism
        selected << combobox_widget_->currentText();
    }
    return selected;
}

// **Helper methods implementation**
void ComboBox::updateItems(const QString& filter_text) {
    if (dynamic_items_provider_) {
        QStringList new_items = dynamic_items_provider_(filter_text);
        if (combobox_widget_) {
            combobox_widget_->clear();
            combobox_widget_->addItems(new_items);
        }
        if (items_changed_handler_) {
            items_changed_handler_();
        }
        emit itemsUpdated(new_items);
    }
}

ComboBoxValidationResult ComboBox::validateSelection(const QString& text, int index) const {
    if (required_ && (text.isEmpty() || index < 0)) {
        return ComboBoxValidationResult(false, "Selection is required", "Please select an option");
    }

    if (!allowed_values_.isEmpty() && !allowed_values_.contains(text)) {
        return ComboBoxValidationResult(false, "Invalid selection", "Please select from available options");
    }

    if (custom_validator_ && !custom_validator_(text)) {
        return ComboBoxValidationResult(false, "Custom validation failed", "Please check your selection");
    }

    if (validation_func_) {
        return validation_func_(text, index);
    }

    return ComboBoxValidationResult(true);
}

void ComboBox::showValidationError(const QString& error) {
    validation_error_ = error;
    is_valid_ = false;

    if (error_handler_) {
        error_handler_(error);
    }

    if (validation_handler_) {
        validation_handler_(false, error);
    }

    emit validationChanged(false, error);
}

}  // namespace DeclarativeUI::Components
