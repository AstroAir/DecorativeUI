// Components/RadioButton.hpp
#pragma once
#include <QRadioButton>
#include <QButtonGroup>

#include "../Core/UIElement.hpp"

namespace DeclarativeUI::Components {

class RadioButton : public Core::UIElement {
    Q_OBJECT

public:
    explicit RadioButton(QObject* parent = nullptr);

    // **Fluent interface for radio button**
    RadioButton& text(const QString& text);
    RadioButton& checked(bool checked);
    RadioButton& autoExclusive(bool exclusive);
    RadioButton& onToggled(std::function<void(bool)> handler);
    RadioButton& onClicked(std::function<void()> handler);
    RadioButton& style(const QString& stylesheet);

    void initialize() override;
    bool isChecked() const;
    void setChecked(bool checked);

private:
    QRadioButton* radio_button_widget_;
    std::function<void(bool)> toggled_handler_;
    std::function<void()> clicked_handler_;
};

class ButtonGroup : public Core::UIElement {
    Q_OBJECT

public:
    explicit ButtonGroup(QObject* parent = nullptr);

    // **Fluent interface for button group**
    ButtonGroup& addButton(QAbstractButton* button, int id = -1);
    ButtonGroup& removeButton(QAbstractButton* button);
    ButtonGroup& exclusive(bool exclusive);
    ButtonGroup& onButtonClicked(std::function<void(QAbstractButton*)> handler);
    ButtonGroup& onButtonToggled(std::function<void(QAbstractButton*, bool)> handler);
    ButtonGroup& onIdClicked(std::function<void(int)> handler);

    void initialize() override;
    QAbstractButton* checkedButton() const;
    int checkedId() const;
    QAbstractButton* button(int id) const;
    void setId(QAbstractButton* button, int id);

private:
    QButtonGroup* button_group_;
    std::function<void(QAbstractButton*)> button_clicked_handler_;
    std::function<void(QAbstractButton*, bool)> button_toggled_handler_;
    std::function<void(int)> id_clicked_handler_;
};



}  // namespace DeclarativeUI::Components
