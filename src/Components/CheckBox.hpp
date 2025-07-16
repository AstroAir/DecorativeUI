// Components/CheckBox.hpp
#pragma once
#include <QCheckBox>

#include "../Core/UIElement.hpp"

namespace DeclarativeUI::Components {

class CheckBox : public Core::UIElement {
    Q_OBJECT

public:
    explicit CheckBox(QObject *parent = nullptr);

    // **Fluent interface for checkbox**
    CheckBox &text(const QString &text);
    CheckBox &checked(bool checked);
    CheckBox &tristate(bool tristate);
    CheckBox &onStateChanged(std::function<void(int)> handler);
    CheckBox &onToggled(std::function<void(bool)> handler);
    CheckBox &style(const QString &stylesheet);

    void initialize() override;
    bool isChecked() const;
    void setChecked(bool checked);
    Qt::CheckState checkState() const;
    void setCheckState(Qt::CheckState state);

private:
    QCheckBox *checkbox_widget_;
    std::function<void(int)> state_changed_handler_;
    std::function<void(bool)> toggled_handler_;
};

// **Implementation**
inline CheckBox::CheckBox(QObject *parent)
    : UIElement(parent), checkbox_widget_(nullptr) {}

inline CheckBox &CheckBox::text(const QString &text) {
    return static_cast<CheckBox &>(setProperty("text", text));
}

inline CheckBox &CheckBox::checked(bool checked) {
    return static_cast<CheckBox &>(setProperty("checked", checked));
}

inline CheckBox &CheckBox::tristate(bool tristate) {
    return static_cast<CheckBox &>(setProperty("tristate", tristate));
}

inline CheckBox &CheckBox::onStateChanged(std::function<void(int)> handler) {
    state_changed_handler_ = std::move(handler);
    return *this;
}

inline CheckBox &CheckBox::onToggled(std::function<void(bool)> handler) {
    toggled_handler_ = std::move(handler);
    return *this;
}

inline CheckBox &CheckBox::style(const QString &stylesheet) {
    return static_cast<CheckBox &>(setProperty("styleSheet", stylesheet));
}

inline void CheckBox::initialize() {
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

inline bool CheckBox::isChecked() const {
    return checkbox_widget_ ? checkbox_widget_->isChecked() : false;
}

inline void CheckBox::setChecked(bool checked) {
    if (checkbox_widget_) {
        checkbox_widget_->setChecked(checked);
    }
}

inline Qt::CheckState CheckBox::checkState() const {
    return checkbox_widget_ ? checkbox_widget_->checkState() : Qt::Unchecked;
}

inline void CheckBox::setCheckState(Qt::CheckState state) {
    if (checkbox_widget_) {
        checkbox_widget_->setCheckState(state);
    }
}

}  // namespace DeclarativeUI::Components
