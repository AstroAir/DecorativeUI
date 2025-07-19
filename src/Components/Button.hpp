// Components/Button.hpp
#pragma once
#include <QIcon>
#include <QPushButton>


#include "../Core/UIElement.hpp"

namespace DeclarativeUI::Components {

class Button : public Core::UIElement {
    Q_OBJECT

public:
    explicit Button(QObject* parent = nullptr);

    // **Fluent interface specific to buttons**
    Button& text(const QString& text);
    Button& icon(const QIcon& icon);
    Button& onClick(std::function<void()> handler);
    Button& enabled(bool enabled);
    Button& style(const QString& stylesheet);

    void initialize() override;

private:
    QPushButton* button_widget_;
};



}  // namespace DeclarativeUI::Components