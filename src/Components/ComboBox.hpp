// Components/ComboBox.hpp
#pragma once
#include <QComboBox>

#include "../Core/UIElement.hpp"

namespace DeclarativeUI::Components {

class ComboBox : public Core::UIElement {
    Q_OBJECT

public:
    explicit ComboBox(QObject* parent = nullptr);

    // **Fluent interface for combobox**
    ComboBox& items(const QStringList& items);
    ComboBox& addItem(const QString& item);
    ComboBox& addItems(const QStringList& items);
    ComboBox& currentIndex(int index);
    ComboBox& currentText(const QString& text);
    ComboBox& editable(bool editable);
    ComboBox& maxVisibleItems(int maxItems);
    ComboBox& onCurrentIndexChanged(std::function<void(int)> handler);
    ComboBox& onCurrentTextChanged(std::function<void(const QString&)> handler);
    ComboBox& onActivated(std::function<void(int)> handler);
    ComboBox& style(const QString& stylesheet);

    void initialize() override;
    int getCurrentIndex() const;
    QString getCurrentText() const;
    void setCurrentIndex(int index);
    void setCurrentText(const QString& text);
    void clearItems();

private:
    QComboBox* combobox_widget_;
    QStringList initial_items_;
    std::function<void(int)> current_index_changed_handler_;
    std::function<void(const QString&)> current_text_changed_handler_;
    std::function<void(int)> activated_handler_;
};

// **Implementation**
inline ComboBox::ComboBox(QObject* parent)
    : UIElement(parent), combobox_widget_(nullptr) {}

inline ComboBox& ComboBox::items(const QStringList& items) {
    initial_items_ = items;
    return *this;
}

inline ComboBox& ComboBox::addItem(const QString& item) {
    initial_items_.append(item);
    return *this;
}

inline ComboBox& ComboBox::addItems(const QStringList& items) {
    initial_items_.append(items);
    return *this;
}

inline ComboBox& ComboBox::currentIndex(int index) {
    return static_cast<ComboBox&>(setProperty("currentIndex", index));
}

inline ComboBox& ComboBox::currentText(const QString& text) {
    return static_cast<ComboBox&>(setProperty("currentText", text));
}

inline ComboBox& ComboBox::editable(bool editable) {
    return static_cast<ComboBox&>(setProperty("editable", editable));
}

inline ComboBox& ComboBox::maxVisibleItems(int maxItems) {
    return static_cast<ComboBox&>(setProperty("maxVisibleItems", maxItems));
}

inline ComboBox& ComboBox::onCurrentIndexChanged(
    std::function<void(int)> handler) {
    current_index_changed_handler_ = std::move(handler);
    return *this;
}

inline ComboBox& ComboBox::onCurrentTextChanged(
    std::function<void(const QString&)> handler) {
    current_text_changed_handler_ = std::move(handler);
    return *this;
}

inline ComboBox& ComboBox::onActivated(std::function<void(int)> handler) {
    activated_handler_ = std::move(handler);
    return *this;
}

inline ComboBox& ComboBox::style(const QString& stylesheet) {
    return static_cast<ComboBox&>(setProperty("styleSheet", stylesheet));
}

inline void ComboBox::initialize() {
    if (!combobox_widget_) {
        combobox_widget_ = new QComboBox();
        setWidget(combobox_widget_);

        // Add initial items
        if (!initial_items_.isEmpty()) {
            combobox_widget_->addItems(initial_items_);
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

inline int ComboBox::getCurrentIndex() const {
    return combobox_widget_ ? combobox_widget_->currentIndex() : -1;
}

inline QString ComboBox::getCurrentText() const {
    return combobox_widget_ ? combobox_widget_->currentText() : QString();
}

inline void ComboBox::setCurrentIndex(int index) {
    if (combobox_widget_) {
        combobox_widget_->setCurrentIndex(index);
    }
}

inline void ComboBox::setCurrentText(const QString& text) {
    if (combobox_widget_) {
        combobox_widget_->setCurrentText(text);
    }
}

inline void ComboBox::clearItems() {
    if (combobox_widget_) {
        combobox_widget_->clear();
    }
    initial_items_.clear();
}

}  // namespace DeclarativeUI::Components
