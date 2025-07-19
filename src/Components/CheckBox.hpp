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



}  // namespace DeclarativeUI::Components
