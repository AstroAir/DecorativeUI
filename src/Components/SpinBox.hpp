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

    // **Basic fluent interface (backward compatible)**
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

    // **Enhanced fluent interface**
    SpinBox& tooltip(const QString& tooltip_text);
    SpinBox& accessibleName(const QString& name);
    SpinBox& accessibleDescription(const QString& description);
    SpinBox& shortcut(const QKeySequence& shortcut);
    SpinBox& required(bool required = true);
    SpinBox& validator(std::function<bool(int)> validation_func);
    SpinBox& range(int min, int max);
    SpinBox& accelerated(bool enabled = true);
    SpinBox& keyboardTracking(bool enabled = true);
    SpinBox& correctionMode(QAbstractSpinBox::CorrectionMode mode);
    SpinBox& buttonSymbols(QAbstractSpinBox::ButtonSymbols symbols);
    SpinBox& alignment(Qt::Alignment alignment);
    SpinBox& frame(bool enabled = true);
    SpinBox& groupSeparator(bool enabled = true);
    SpinBox& onValidationChanged(
        std::function<void(bool, const QString&)> handler);
    SpinBox& onRangeChanged(std::function<void(int, int)> handler);

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
