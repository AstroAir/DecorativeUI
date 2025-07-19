// Components/PlainTextEdit.hpp
#pragma once
#include <QPlainTextEdit>
#include <QWidget>
#include <functional>

#include "../Core/UIElement.hpp"

namespace DeclarativeUI::Components {

class PlainTextEdit : public Core::UIElement {
    Q_OBJECT

public:
    explicit PlainTextEdit(QObject* parent = nullptr);

    // **Fluent interface for plain text edit**
    PlainTextEdit& plainText(const QString& text);
    PlainTextEdit& placeholderText(const QString& placeholder);
    PlainTextEdit& readOnly(bool readOnly);
    PlainTextEdit& lineWrapMode(QPlainTextEdit::LineWrapMode mode);
    PlainTextEdit& maximumBlockCount(int maximum);
    PlainTextEdit& onTextChanged(std::function<void()> handler);
    PlainTextEdit& onSelectionChanged(std::function<void()> handler);

    // **Getters**
    void initialize() override;
    QString getPlainText() const;
    QString getPlaceholderText() const;
    bool isReadOnly() const;
    QPlainTextEdit::LineWrapMode getLineWrapMode() const;
    int getMaximumBlockCount() const;

private:
    QPlainTextEdit* plain_text_edit_widget_;
    std::function<void()> text_changed_handler_;
    std::function<void()> selection_changed_handler_;
};

}  // namespace DeclarativeUI::Components
