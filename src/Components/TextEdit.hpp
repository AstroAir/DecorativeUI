#pragma once

#include <QColor>
#include <QFont>
#include <QPlainTextEdit>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextEdit>

#include "../Core/UIElement.hpp"

namespace DeclarativeUI::Components {

class TextEdit : public Core::UIElement {
    Q_OBJECT

public:
    explicit TextEdit(QObject *parent = nullptr);

    // **Fluent interface for text edit**
    TextEdit &text(const QString &text);
    TextEdit &html(const QString &html);
    TextEdit &placeholder(const QString &placeholder);
    TextEdit &readOnly(bool readonly);
    TextEdit &acceptRichText(bool accept);
    TextEdit &wordWrapMode(QTextOption::WrapMode mode);
    TextEdit &lineWrapMode(QTextEdit::LineWrapMode mode);
    TextEdit &lineWrapColumnOrWidth(int width);
    TextEdit &tabStopWidth(int width);
    TextEdit &cursorWidth(int width);
    TextEdit &textInteractionFlags(Qt::TextInteractionFlags flags);
    TextEdit &font(const QFont &font);
    TextEdit &textColor(const QColor &color);
    TextEdit &backgroundColor(const QColor &color);
    TextEdit &selectionColor(const QColor &color);
    TextEdit &onTextChanged(std::function<void()> handler);
    TextEdit &onSelectionChanged(std::function<void()> handler);
    TextEdit &onCursorPositionChanged(std::function<void()> handler);
    TextEdit &style(const QString &stylesheet);

    void initialize() override;

    // **Text operations**
    QString getText() const;
    QString getHtml() const;
    QString getPlainText() const;
    void setText(const QString &text);
    void setHtml(const QString &html);
    void setPlainText(const QString &text);
    void append(const QString &text);
    void insertText(const QString &text);
    void insertHtml(const QString &html);
    void clear();

    // **Cursor operations**
    QTextCursor getCursor() const;
    void setCursor(const QTextCursor &cursor);
    void moveCursor(QTextCursor::MoveOperation operation,
                    QTextCursor::MoveMode mode = QTextCursor::MoveAnchor);
    void selectAll();
    void selectWord();
    void selectLine();
    QString getSelectedText() const;
    bool hasSelection() const;

    // **Formatting operations**
    void setCurrentFont(const QFont &font);
    void setFontFamily(const QString &family);
    void setFontSize(int size);
    void setFontWeight(int weight);
    void setFontItalic(bool italic);
    void setFontUnderline(bool underline);
    void setTextColor(const QColor &color);
    void setTextBackgroundColor(const QColor &color);
    void setAlignment(Qt::Alignment alignment);

    // **Edit operations**
    void undo();
    void redo();
    void cut();
    void copy();
    void paste();
    bool canUndo() const;
    bool canRedo() const;

    // **Search operations**
    bool find(const QString &text,
              QTextDocument::FindFlags flags = QTextDocument::FindFlags());
    void replace(const QString &oldText, const QString &newText);
    void replaceAll(const QString &oldText, const QString &newText);

    // **Document operations**
    QTextDocument *getDocument() const;
    void setDocument(QTextDocument *document);
    bool isModified() const;
    void setModified(bool modified);

    // **Getters**
    bool isReadOnly() const;
    bool getAcceptRichText() const;
    QTextOption::WrapMode getWordWrapMode() const;
    QTextEdit::LineWrapMode getLineWrapMode() const;
    int getLineWrapColumnOrWidth() const;
    int getTabStopWidth() const;
    int getCursorWidth() const;

private:
    QTextEdit *text_edit_widget_;
    std::function<void()> text_changed_handler_;
    std::function<void()> selection_changed_handler_;
    std::function<void()> cursor_position_changed_handler_;
};















}  // namespace DeclarativeUI::Components
