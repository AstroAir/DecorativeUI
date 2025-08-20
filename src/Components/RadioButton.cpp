// Components/RadioButton.cpp
#include "RadioButton.hpp"

namespace DeclarativeUI::Components {

// **RadioButton Implementation**
RadioButton::RadioButton(QObject* parent)
    : UIElement(parent), radio_button_widget_(nullptr) {}

RadioButton& RadioButton::text(const QString& text) {
    return static_cast<RadioButton&>(setProperty("text", text));
}

RadioButton& RadioButton::checked(bool checked) {
    return static_cast<RadioButton&>(setProperty("checked", checked));
}

RadioButton& RadioButton::autoExclusive(bool exclusive) {
    return static_cast<RadioButton&>(setProperty("autoExclusive", exclusive));
}

RadioButton& RadioButton::onToggled(std::function<void(bool)> handler) {
    toggled_handler_ = std::move(handler);
    return *this;
}

RadioButton& RadioButton::onClicked(std::function<void()> handler) {
    clicked_handler_ = std::move(handler);
    return *this;
}

RadioButton& RadioButton::style(const QString& stylesheet) {
    return static_cast<RadioButton&>(setProperty("styleSheet", stylesheet));
}

void RadioButton::initialize() {
    if (!radio_button_widget_) {
        radio_button_widget_ = new QRadioButton();
        setWidget(radio_button_widget_);

        // Connect signals
        if (toggled_handler_) {
            connect(radio_button_widget_, &QRadioButton::toggled, this,
                    [this](bool checked) { toggled_handler_(checked); });
        }

        if (clicked_handler_) {
            connect(radio_button_widget_, &QRadioButton::clicked, this,
                    [this]() { clicked_handler_(); });
        }
    }
}

bool RadioButton::isChecked() const {
    return radio_button_widget_ ? radio_button_widget_->isChecked() : false;
}

void RadioButton::setChecked(bool checked) {
    if (radio_button_widget_) {
        bool oldState = radio_button_widget_->isChecked();

        // For radio buttons, Qt might not emit toggled when setting to false
        // We need to temporarily disable autoExclusive to allow unchecking
        if (!checked && oldState) {
            bool wasAutoExclusive = radio_button_widget_->autoExclusive();
            radio_button_widget_->setAutoExclusive(false);
            radio_button_widget_->setChecked(checked);
            radio_button_widget_->setAutoExclusive(wasAutoExclusive);
        } else {
            radio_button_widget_->setChecked(checked);
        }
    }
}

// **ButtonGroup Implementation**
ButtonGroup::ButtonGroup(QObject* parent)
    : UIElement(parent), button_group_(nullptr) {}

ButtonGroup& ButtonGroup::addButton(QAbstractButton* button, int id) {
    if (button_group_ && button) {
        button_group_->addButton(button, id);
    }
    return *this;
}

ButtonGroup& ButtonGroup::removeButton(QAbstractButton* button) {
    if (button_group_ && button) {
        button_group_->removeButton(button);
    }
    return *this;
}

ButtonGroup& ButtonGroup::exclusive(bool exclusive) {
    if (button_group_) {
        button_group_->setExclusive(exclusive);
    }
    return *this;
}

ButtonGroup& ButtonGroup::onButtonClicked(
    std::function<void(QAbstractButton*)> handler) {
    button_clicked_handler_ = std::move(handler);
    return *this;
}

ButtonGroup& ButtonGroup::onButtonToggled(
    std::function<void(QAbstractButton*, bool)> handler) {
    button_toggled_handler_ = std::move(handler);
    return *this;
}

ButtonGroup& ButtonGroup::onIdClicked(std::function<void(int)> handler) {
    id_clicked_handler_ = std::move(handler);
    return *this;
}

void ButtonGroup::initialize() {
    if (!button_group_) {
        button_group_ = new QButtonGroup(this);

        // Connect signals
        if (button_clicked_handler_) {
            connect(
                button_group_,
                QOverload<QAbstractButton*>::of(&QButtonGroup::buttonClicked),
                this, [this](QAbstractButton* button) {
                    button_clicked_handler_(button);
                });
        }

        if (button_toggled_handler_) {
            connect(button_group_,
                    QOverload<QAbstractButton*, bool>::of(
                        &QButtonGroup::buttonToggled),
                    this, [this](QAbstractButton* button, bool checked) {
                        button_toggled_handler_(button, checked);
                    });
        }

        if (id_clicked_handler_) {
            connect(button_group_, QOverload<int>::of(&QButtonGroup::idClicked),
                    this, [this](int id) { id_clicked_handler_(id); });
        }
    }
}

QAbstractButton* ButtonGroup::checkedButton() const {
    return button_group_ ? button_group_->checkedButton() : nullptr;
}

int ButtonGroup::checkedId() const {
    return button_group_ ? button_group_->checkedId() : -1;
}

QAbstractButton* ButtonGroup::button(int id) const {
    return button_group_ ? button_group_->button(id) : nullptr;
}

void ButtonGroup::setId(QAbstractButton* button, int id) {
    if (button_group_ && button) {
        button_group_->setId(button, id);
    }
}

}  // namespace DeclarativeUI::Components
