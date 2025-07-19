// Components/SpinBox.hpp
#pragma once
#include <QDoubleSpinBox>
#include <QSpinBox>


#include "../Core/UIElement.hpp"

namespace DeclarativeUI::Components {

class SpinBox : public Core::UIElement {
    Q_OBJECT

public:
    explicit SpinBox(QObject* parent = nullptr);

    // **Fluent interface for spin box**
    SpinBox& minimum(int min);
    SpinBox& maximum(int max);
    SpinBox& value(int value);
    SpinBox& singleStep(int step);
    SpinBox& prefix(const QString& prefix);
    SpinBox& suffix(const QString& suffix);
    SpinBox& wrapping(bool wrap);
    SpinBox& readOnly(bool readonly);
    SpinBox& specialValueText(const QString& text);
    SpinBox& displayIntegerBase(int base);
    SpinBox& onValueChanged(std::function<void(int)> handler);
    SpinBox& onTextChanged(std::function<void(const QString&)> handler);
    SpinBox& style(const QString& stylesheet);

    void initialize() override;

    // **Getters**
    int getValue() const;
    int getMinimum() const;
    int getMaximum() const;
    int getSingleStep() const;
    QString getPrefix() const;
    QString getSuffix() const;
    bool isWrapping() const;
    bool isReadOnly() const;

    // **Setters**
    void setValue(int value);
    void setMinimum(int min);
    void setMaximum(int max);
    void setRange(int min, int max);
    void setSingleStep(int step);
    void stepUp();
    void stepDown();
    void selectAll();
    void clear();

private:
    QSpinBox* spinbox_widget_;
    std::function<void(int)> value_changed_handler_;
    std::function<void(const QString&)> text_changed_handler_;
};







}  // namespace DeclarativeUI::Components
