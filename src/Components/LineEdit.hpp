// Components/LineEdit.hpp
#pragma once
#include <QLineEdit>
#include <QValidator>

#include "../Core/UIElement.hpp"

namespace DeclarativeUI::Components {

class LineEdit : public Core::UIElement {
    Q_OBJECT

public:
    explicit LineEdit(QObject* parent = nullptr);

    // **Fluent interface for line edit**
    LineEdit& text(const QString& text);
    LineEdit& placeholder(const QString& placeholder);
    LineEdit& readOnly(bool readonly);
    LineEdit& maxLength(int length);
    LineEdit& validator(QValidator* validator);
    LineEdit& echoMode(QLineEdit::EchoMode mode);
    LineEdit& onTextChanged(std::function<void(const QString&)> handler);
    LineEdit& onTextEdited(std::function<void(const QString&)> handler);
    LineEdit& onReturnPressed(std::function<void()> handler);
    LineEdit& style(const QString& stylesheet);

    void initialize() override;
    QString getText() const;
    void setText(const QString& text);

private:
    QLineEdit* line_edit_widget_;
    std::function<void(const QString&)> text_changed_handler_;
    std::function<void(const QString&)> text_edited_handler_;
    std::function<void()> return_pressed_handler_;
};



}  // namespace DeclarativeUI::Components
