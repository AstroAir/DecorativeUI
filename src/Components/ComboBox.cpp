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

}  // namespace DeclarativeUI::Components
