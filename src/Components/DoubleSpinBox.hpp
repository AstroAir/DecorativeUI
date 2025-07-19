// Components/DoubleSpinBox.hpp
#pragma once
#include <QDoubleSpinBox>
#include <QWidget>
#include <functional>

#include "../Core/UIElement.hpp"

namespace DeclarativeUI::Components {

class DoubleSpinBox : public Core::UIElement {
    Q_OBJECT

public:
    explicit DoubleSpinBox(QObject* parent = nullptr);

    // **Fluent interface for double spin box**
    DoubleSpinBox& minimum(double min);
    DoubleSpinBox& maximum(double max);
    DoubleSpinBox& value(double val);
    DoubleSpinBox& singleStep(double step);
    DoubleSpinBox& decimals(int decimals);
    DoubleSpinBox& prefix(const QString& prefix);
    DoubleSpinBox& suffix(const QString& suffix);
    DoubleSpinBox& onValueChanged(std::function<void(double)> handler);

    // **Getters**
    void initialize() override;
    double getValue() const;
    double getMinimum() const;
    double getMaximum() const;
    double getSingleStep() const;
    int getDecimals() const;
    QString getPrefix() const;
    QString getSuffix() const;

private:
    QDoubleSpinBox* double_spin_box_widget_;
    std::function<void(double)> value_changed_handler_;
};

}  // namespace DeclarativeUI::Components
