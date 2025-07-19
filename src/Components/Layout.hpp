#pragma once
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLayout>
#include <QMargins>
#include <QVBoxLayout>
#include <QWidget>
#include <vector>

#include "../Core/UIElement.hpp"

namespace DeclarativeUI::Components {

class Layout : public Core::UIElement {
    Q_OBJECT
public:
    enum class Type { HBox, VBox, Grid };

    explicit Layout(Type type = Type::VBox, QObject *parent = nullptr);

    // Fluent interface
    Layout &addWidget(QWidget *widget, int row = -1, int col = -1,
                      int rowSpan = 1, int colSpan = 1);
    Layout &addElement(Core::UIElement *element, int row = -1, int col = -1,
                       int rowSpan = 1, int colSpan = 1);
    Layout &spacing(int spacing);
    Layout &margins(const QMargins &margins);
    Layout &setAlignment(Qt::Alignment alignment);

    void initialize() override;
    QLayout *getLayout() const;

private:
    Type layout_type_;
    QLayout *layout_ = nullptr;
    std::vector<QWidget *> widgets_;
    std::vector<Core::UIElement *> elements_;
    int spacing_ = -1;
    QMargins margins_;
    Qt::Alignment alignment_ = Qt::Alignment();
};



}  // namespace DeclarativeUI::Components
