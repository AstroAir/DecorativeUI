// Components/CheckBox.cpp
#include "CheckBox.hpp"

namespace DeclarativeUI::Components {

// **Implementation**
CheckBox::CheckBox(QObject *parent)
    : UIElement(parent), checkbox_widget_(nullptr) {}

CheckBox &CheckBox::text(const QString &text) {
    return static_cast<CheckBox &>(setProperty("text", text));
}

CheckBox &CheckBox::checked(bool checked) {
    return static_cast<CheckBox &>(setProperty("checked", checked));
}

CheckBox &CheckBox::tristate(bool tristate) {
    return static_cast<CheckBox &>(setProperty("tristate", tristate));
}

CheckBox &CheckBox::onStateChanged(std::function<void(int)> handler) {
    state_changed_handler_ = std::move(handler);
    return *this;
}

CheckBox &CheckBox::onToggled(std::function<void(bool)> handler) {
    toggled_handler_ = std::move(handler);
    return *this;
}

CheckBox &CheckBox::style(const QString &stylesheet) {
    return static_cast<CheckBox &>(setProperty("styleSheet", stylesheet));
}

void CheckBox::initialize() {
    if (!checkbox_widget_) {
        checkbox_widget_ = new QCheckBox();
        setWidget(checkbox_widget_);

        // Connect signals
        if (state_changed_handler_) {
            connect(checkbox_widget_, &QCheckBox::checkStateChanged, this,
                    [this](Qt::CheckState state) {
                        state_changed_handler_(static_cast<int>(state));
                    });
        }

        if (toggled_handler_) {
            connect(checkbox_widget_, &QCheckBox::toggled, this,
                    [this](bool checked) { toggled_handler_(checked); });
        }
    }
}

bool CheckBox::isChecked() const {
    return checkbox_widget_ ? checkbox_widget_->isChecked() : false;
}

void CheckBox::setChecked(bool checked) {
    if (checkbox_widget_) {
        checkbox_widget_->setChecked(checked);
    }
}

Qt::CheckState CheckBox::checkState() const {
    return checkbox_widget_ ? checkbox_widget_->checkState() : Qt::Unchecked;
}

void CheckBox::setCheckState(Qt::CheckState state) {
    if (checkbox_widget_) {
        checkbox_widget_->setCheckState(state);
    }
}

}  // namespace DeclarativeUI::Components
