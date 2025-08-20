#pragma once
#include <QLayout>
#include <QWidget>
#include <vector>

#include "../Core/UIElement.hpp"

namespace DeclarativeUI::Components {

class Container : public Core::UIElement {
    Q_OBJECT
public:
    explicit Container(QObject *parent = nullptr);

    // Fluent interface
    Container &addWidget(QWidget *widget);
    Container &addElement(Core::UIElement *element);
    Container &setLayout(QLayout *layout);

    void initialize() override;

private:
    std::vector<QWidget *> widgets_;
    std::vector<Core::UIElement *> elements_;
    QLayout *layout_ = nullptr;
};

}  // namespace DeclarativeUI::Components
