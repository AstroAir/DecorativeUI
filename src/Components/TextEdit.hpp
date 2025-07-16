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

class PlainTextEdit : public Core::UIElement {
    Q_OBJECT

public:
    explicit PlainTextEdit(QObject *parent = nullptr);

    // **Fluent interface for plain text edit**
    PlainTextEdit &text(const QString &text);
    PlainTextEdit &placeholder(const QString &placeholder);
    PlainTextEdit &readOnly(bool readonly);
    PlainTextEdit &wordWrapMode(QTextOption::WrapMode mode);
    PlainTextEdit &lineWrapMode(QPlainTextEdit::LineWrapMode mode);
    PlainTextEdit &tabStopWidth(int width);
    PlainTextEdit &cursorWidth(int width);
    PlainTextEdit &textInteractionFlags(Qt::TextInteractionFlags flags);
    PlainTextEdit &font(const QFont &font);
    PlainTextEdit &textColor(const QColor &color);
    PlainTextEdit &backgroundColor(const QColor &color);
    PlainTextEdit &selectionColor(const QColor &color);
    PlainTextEdit &onTextChanged(std::function<void()> handler);
    PlainTextEdit &onSelectionChanged(std::function<void()> handler);
    PlainTextEdit &onCursorPositionChanged(std::function<void()> handler);
    PlainTextEdit &style(const QString &stylesheet);

    void initialize() override;

    // **Text operations**
    QString getText() const;
    QString getPlainText() const;
    void setText(const QString &text);
    void setPlainText(const QString &text);
    void appendText(const QString &text);
    void insertText(const QString &text);
    void clear();

    // **Cursor operations**
    QTextCursor getCursor() const;
    void setCursor(const QTextCursor &cursor);
    void moveCursor(QTextCursor::MoveOperation operation,
                    QTextCursor::MoveMode mode = QTextCursor::MoveAnchor);
    void selectAll();
    QString getSelectedText() const;
    bool hasSelection() const;

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

    // **Document operations**
    QTextDocument *getDocument() const;
    void setDocument(QTextDocument *document);
    bool isModified() const;
    void setModified(bool modified);

    // **Getters**
    bool isReadOnly() const;
    QTextOption::WrapMode getWordWrapMode() const;
    QPlainTextEdit::LineWrapMode getLineWrapMode() const;
    int getTabStopWidth() const;
    int getCursorWidth() const;

private:
    QPlainTextEdit *plain_text_edit_widget_;
    std::function<void()> text_changed_handler_;
    std::function<void()> selection_changed_handler_;
    std::function<void()> cursor_position_changed_handler_;
};

// **TextEdit Implementation**
inline TextEdit::TextEdit(QObject *parent)
    : UIElement(parent), text_edit_widget_(nullptr) {}

inline TextEdit &TextEdit::text(const QString &text) {
    return static_cast<TextEdit &>(setProperty("plainText", text));
}

inline TextEdit &TextEdit::html(const QString &html) {
    return static_cast<TextEdit &>(setProperty("html", html));
}

inline TextEdit &TextEdit::placeholder(const QString &placeholder) {
    return static_cast<TextEdit &>(setProperty("placeholderText", placeholder));
}

inline TextEdit &TextEdit::readOnly(bool readonly) {
    return static_cast<TextEdit &>(setProperty("readOnly", readonly));
}

inline TextEdit &TextEdit::acceptRichText(bool accept) {
    return static_cast<TextEdit &>(setProperty("acceptRichText", accept));
}

inline TextEdit &TextEdit::wordWrapMode(QTextOption::WrapMode mode) {
    return static_cast<TextEdit &>(
        setProperty("wordWrapMode", static_cast<int>(mode)));
}

inline TextEdit &TextEdit::lineWrapMode(QTextEdit::LineWrapMode mode) {
    return static_cast<TextEdit &>(
        setProperty("lineWrapMode", static_cast<int>(mode)));
}

inline TextEdit &TextEdit::lineWrapColumnOrWidth(int width) {
    return static_cast<TextEdit &>(setProperty("lineWrapColumnOrWidth", width));
}

inline TextEdit &TextEdit::tabStopWidth(int width) {
    return static_cast<TextEdit &>(setProperty("tabStopWidth", width));
}

inline TextEdit &TextEdit::cursorWidth(int width) {
    return static_cast<TextEdit &>(setProperty("cursorWidth", width));
}

inline TextEdit &TextEdit::textInteractionFlags(
    Qt::TextInteractionFlags flags) {
    return static_cast<TextEdit &>(
        setProperty("textInteractionFlags", static_cast<int>(flags)));
}

inline TextEdit &TextEdit::font(const QFont &font) {
    return static_cast<TextEdit &>(setProperty("font", font));
}

inline TextEdit &TextEdit::textColor(const QColor &color) {
    return static_cast<TextEdit &>(setProperty("textColor", color));
}

inline TextEdit &TextEdit::backgroundColor(const QColor &color) {
    return static_cast<TextEdit &>(setProperty("backgroundColor", color));
}

inline TextEdit &TextEdit::selectionColor(const QColor &color) {
    return static_cast<TextEdit &>(setProperty("selectionColor", color));
}

inline TextEdit &TextEdit::onTextChanged(std::function<void()> handler) {
    text_changed_handler_ = std::move(handler);
    return *this;
}

inline TextEdit &TextEdit::onSelectionChanged(std::function<void()> handler) {
    selection_changed_handler_ = std::move(handler);
    return *this;
}

inline TextEdit &TextEdit::onCursorPositionChanged(
    std::function<void()> handler) {
    cursor_position_changed_handler_ = std::move(handler);
    return *this;
}

inline TextEdit &TextEdit::style(const QString &stylesheet) {
    return static_cast<TextEdit &>(setProperty("styleSheet", stylesheet));
}

inline void TextEdit::initialize() {
    if (!text_edit_widget_) {
        text_edit_widget_ = new QTextEdit();
        setWidget(text_edit_widget_);

        // Connect signals
        if (text_changed_handler_) {
            connect(text_edit_widget_, &QTextEdit::textChanged, this,
                    [this]() { text_changed_handler_(); });
        }

        if (selection_changed_handler_) {
            connect(text_edit_widget_, &QTextEdit::selectionChanged, this,
                    [this]() { selection_changed_handler_(); });
        }

        if (cursor_position_changed_handler_) {
            connect(text_edit_widget_, &QTextEdit::cursorPositionChanged, this,
                    [this]() { cursor_position_changed_handler_(); });
        }
    }
}

// **Text operations**
inline QString TextEdit::getText() const {
    return text_edit_widget_ ? text_edit_widget_->toPlainText() : QString();
}

inline QString TextEdit::getHtml() const {
    return text_edit_widget_ ? text_edit_widget_->toHtml() : QString();
}

inline QString TextEdit::getPlainText() const {
    return text_edit_widget_ ? text_edit_widget_->toPlainText() : QString();
}

inline void TextEdit::setText(const QString &text) {
    if (text_edit_widget_) {
        text_edit_widget_->setPlainText(text);
    }
}

inline void TextEdit::setHtml(const QString &html) {
    if (text_edit_widget_) {
        text_edit_widget_->setHtml(html);
    }
}

inline void TextEdit::setPlainText(const QString &text) {
    if (text_edit_widget_) {
        text_edit_widget_->setPlainText(text);
    }
}

inline void TextEdit::append(const QString &text) {
    if (text_edit_widget_) {
        text_edit_widget_->append(text);
    }
}

inline void TextEdit::insertText(const QString &text) {
    if (text_edit_widget_) {
        text_edit_widget_->insertPlainText(text);
    }
}

inline void TextEdit::insertHtml(const QString &html) {
    if (text_edit_widget_) {
        text_edit_widget_->insertHtml(html);
    }
}

inline void TextEdit::clear() {
    if (text_edit_widget_) {
        text_edit_widget_->clear();
    }
}

// **Cursor operations**
inline QTextCursor TextEdit::getCursor() const {
    return text_edit_widget_ ? text_edit_widget_->textCursor() : QTextCursor();
}

inline void TextEdit::setCursor(const QTextCursor &cursor) {
    if (text_edit_widget_) {
        text_edit_widget_->setTextCursor(cursor);
    }
}

inline void TextEdit::moveCursor(QTextCursor::MoveOperation operation,
                                 QTextCursor::MoveMode mode) {
    if (text_edit_widget_) {
        text_edit_widget_->moveCursor(operation, mode);
    }
}

inline void TextEdit::selectAll() {
    if (text_edit_widget_) {
        text_edit_widget_->selectAll();
    }
}

inline void TextEdit::selectWord() {
    if (text_edit_widget_) {
        QTextCursor cursor = text_edit_widget_->textCursor();
        cursor.select(QTextCursor::WordUnderCursor);
        text_edit_widget_->setTextCursor(cursor);
    }
}

inline void TextEdit::selectLine() {
    if (text_edit_widget_) {
        QTextCursor cursor = text_edit_widget_->textCursor();
        cursor.select(QTextCursor::LineUnderCursor);
        text_edit_widget_->setTextCursor(cursor);
    }
}

inline QString TextEdit::getSelectedText() const {
    return text_edit_widget_ ? text_edit_widget_->textCursor().selectedText()
                             : QString();
}

inline bool TextEdit::hasSelection() const {
    return text_edit_widget_ ? text_edit_widget_->textCursor().hasSelection()
                             : false;
}

// **Edit operations**
inline void TextEdit::undo() {
    if (text_edit_widget_) {
        text_edit_widget_->undo();
    }
}

inline void TextEdit::redo() {
    if (text_edit_widget_) {
        text_edit_widget_->redo();
    }
}

inline void TextEdit::cut() {
    if (text_edit_widget_) {
        text_edit_widget_->cut();
    }
}

inline void TextEdit::copy() {
    if (text_edit_widget_) {
        text_edit_widget_->copy();
    }
}

inline void TextEdit::paste() {
    if (text_edit_widget_) {
        text_edit_widget_->paste();
    }
}

inline bool TextEdit::canUndo() const {
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    return text_edit_widget_ ? text_edit_widget_->document()->isUndoAvailable()
                             : false;
#else
    return text_edit_widget_ ? text_edit_widget_->document()->isUndoAvailable()
                             : false;
#endif
}

inline bool TextEdit::canRedo() const {
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    return text_edit_widget_ ? text_edit_widget_->document()->isRedoAvailable()
                             : false;
#else
    return text_edit_widget_ ? text_edit_widget_->document()->isRedoAvailable()
                             : false;
#endif
}

// **Search operations**
inline bool TextEdit::find(const QString &text,
                           QTextDocument::FindFlags flags) {
    return text_edit_widget_ ? text_edit_widget_->find(text, flags) : false;
}

// **Document operations**
inline QTextDocument *TextEdit::getDocument() const {
    return text_edit_widget_ ? text_edit_widget_->document() : nullptr;
}

inline void TextEdit::setDocument(QTextDocument *document) {
    if (text_edit_widget_) {
        text_edit_widget_->setDocument(document);
    }
}

inline bool TextEdit::isModified() const {
    return text_edit_widget_ ? text_edit_widget_->document()->isModified()
                             : false;
}

inline void TextEdit::setModified(bool modified) {
    if (text_edit_widget_) {
        text_edit_widget_->document()->setModified(modified);
    }
}

// **Getters**
inline bool TextEdit::isReadOnly() const {
    return text_edit_widget_ ? text_edit_widget_->isReadOnly() : false;
}

inline bool TextEdit::getAcceptRichText() const {
    return text_edit_widget_ ? text_edit_widget_->acceptRichText() : true;
}

inline QTextOption::WrapMode TextEdit::getWordWrapMode() const {
    return text_edit_widget_ ? text_edit_widget_->wordWrapMode()
                             : QTextOption::WrapAtWordBoundaryOrAnywhere;
}

inline QTextEdit::LineWrapMode TextEdit::getLineWrapMode() const {
    return text_edit_widget_ ? text_edit_widget_->lineWrapMode()
                             : QTextEdit::WidgetWidth;
}

inline int TextEdit::getLineWrapColumnOrWidth() const {
    return text_edit_widget_ ? text_edit_widget_->lineWrapColumnOrWidth() : 0;
}

inline int TextEdit::getTabStopWidth() const {
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    return text_edit_widget_ ? text_edit_widget_->tabStopDistance() / 1.0 : 80;
#else
    return text_edit_widget_ ? text_edit_widget_->tabStopWidth() : 80;
#endif
}

inline int TextEdit::getCursorWidth() const {
    return text_edit_widget_ ? text_edit_widget_->cursorWidth() : 1;
}

// **PlainTextEdit Implementation**
inline PlainTextEdit::PlainTextEdit(QObject *parent)
    : UIElement(parent), plain_text_edit_widget_(nullptr) {}

inline PlainTextEdit &PlainTextEdit::text(const QString &text) {
    return static_cast<PlainTextEdit &>(setProperty("plainText", text));
}

inline PlainTextEdit &PlainTextEdit::placeholder(const QString &placeholder) {
    return static_cast<PlainTextEdit &>(
        setProperty("placeholderText", placeholder));
}

inline PlainTextEdit &PlainTextEdit::readOnly(bool readonly) {
    return static_cast<PlainTextEdit &>(setProperty("readOnly", readonly));
}

inline PlainTextEdit &PlainTextEdit::wordWrapMode(QTextOption::WrapMode mode) {
    return static_cast<PlainTextEdit &>(
        setProperty("wordWrapMode", static_cast<int>(mode)));
}

inline PlainTextEdit &PlainTextEdit::lineWrapMode(
    QPlainTextEdit::LineWrapMode mode) {
    return static_cast<PlainTextEdit &>(
        setProperty("lineWrapMode", static_cast<int>(mode)));
}

inline PlainTextEdit &PlainTextEdit::tabStopWidth(int width) {
    return static_cast<PlainTextEdit &>(setProperty("tabStopWidth", width));
}

inline PlainTextEdit &PlainTextEdit::cursorWidth(int width) {
    return static_cast<PlainTextEdit &>(setProperty("cursorWidth", width));
}

inline PlainTextEdit &PlainTextEdit::textInteractionFlags(
    Qt::TextInteractionFlags flags) {
    return static_cast<PlainTextEdit &>(
        setProperty("textInteractionFlags", static_cast<int>(flags)));
}

inline PlainTextEdit &PlainTextEdit::font(const QFont &font) {
    return static_cast<PlainTextEdit &>(setProperty("font", font));
}

inline PlainTextEdit &PlainTextEdit::textColor(const QColor &color) {
    return static_cast<PlainTextEdit &>(setProperty("textColor", color));
}

inline PlainTextEdit &PlainTextEdit::backgroundColor(const QColor &color) {
    return static_cast<PlainTextEdit &>(setProperty("backgroundColor", color));
}

inline PlainTextEdit &PlainTextEdit::selectionColor(const QColor &color) {
    return static_cast<PlainTextEdit &>(setProperty("selectionColor", color));
}

inline PlainTextEdit &PlainTextEdit::onTextChanged(
    std::function<void()> handler) {
    text_changed_handler_ = std::move(handler);
    return *this;
}

inline PlainTextEdit &PlainTextEdit::onSelectionChanged(
    std::function<void()> handler) {
    selection_changed_handler_ = std::move(handler);
    return *this;
}

inline PlainTextEdit &PlainTextEdit::onCursorPositionChanged(
    std::function<void()> handler) {
    cursor_position_changed_handler_ = std::move(handler);
    return *this;
}

inline PlainTextEdit &PlainTextEdit::style(const QString &stylesheet) {
    return static_cast<PlainTextEdit &>(setProperty("styleSheet", stylesheet));
}

inline void PlainTextEdit::initialize() {
    if (!plain_text_edit_widget_) {
        plain_text_edit_widget_ = new QPlainTextEdit();
        setWidget(plain_text_edit_widget_);

        // Connect signals
        if (text_changed_handler_) {
            connect(plain_text_edit_widget_, &QPlainTextEdit::textChanged, this,
                    [this]() { text_changed_handler_(); });
        }

        if (selection_changed_handler_) {
            connect(plain_text_edit_widget_, &QPlainTextEdit::selectionChanged,
                    this, [this]() { selection_changed_handler_(); });
        }

        if (cursor_position_changed_handler_) {
            connect(plain_text_edit_widget_,
                    &QPlainTextEdit::cursorPositionChanged, this,
                    [this]() { cursor_position_changed_handler_(); });
        }
    }
}

// **Text operations**
inline QString PlainTextEdit::getText() const {
    return plain_text_edit_widget_ ? plain_text_edit_widget_->toPlainText()
                                   : QString();
}

inline QString PlainTextEdit::getPlainText() const {
    return plain_text_edit_widget_ ? plain_text_edit_widget_->toPlainText()
                                   : QString();
}

inline void PlainTextEdit::setText(const QString &text) {
    if (plain_text_edit_widget_) {
        plain_text_edit_widget_->setPlainText(text);
    }
}

inline void PlainTextEdit::setPlainText(const QString &text) {
    if (plain_text_edit_widget_) {
        plain_text_edit_widget_->setPlainText(text);
    }
}

inline void PlainTextEdit::appendText(const QString &text) {
    if (plain_text_edit_widget_) {
        plain_text_edit_widget_->appendPlainText(text);
    }
}

inline void PlainTextEdit::insertText(const QString &text) {
    if (plain_text_edit_widget_) {
        plain_text_edit_widget_->insertPlainText(text);
    }
}

inline void PlainTextEdit::clear() {
    if (plain_text_edit_widget_) {
        plain_text_edit_widget_->clear();
    }
}

// **Cursor operations**
inline QTextCursor PlainTextEdit::getCursor() const {
    return plain_text_edit_widget_ ? plain_text_edit_widget_->textCursor()
                                   : QTextCursor();
}

inline void PlainTextEdit::setCursor(const QTextCursor &cursor) {
    if (plain_text_edit_widget_) {
        plain_text_edit_widget_->setTextCursor(cursor);
    }
}

inline void PlainTextEdit::moveCursor(QTextCursor::MoveOperation operation,
                                      QTextCursor::MoveMode mode) {
    if (plain_text_edit_widget_) {
        plain_text_edit_widget_->moveCursor(operation, mode);
    }
}

inline void PlainTextEdit::selectAll() {
    if (plain_text_edit_widget_) {
        plain_text_edit_widget_->selectAll();
    }
}

inline QString PlainTextEdit::getSelectedText() const {
    return plain_text_edit_widget_
               ? plain_text_edit_widget_->textCursor().selectedText()
               : QString();
}

inline bool PlainTextEdit::hasSelection() const {
    return plain_text_edit_widget_
               ? plain_text_edit_widget_->textCursor().hasSelection()
               : false;
}

// **Edit operations**
inline void PlainTextEdit::undo() {
    if (plain_text_edit_widget_) {
        plain_text_edit_widget_->undo();
    }
}

inline void PlainTextEdit::redo() {
    if (plain_text_edit_widget_) {
        plain_text_edit_widget_->redo();
    }
}

inline void PlainTextEdit::cut() {
    if (plain_text_edit_widget_) {
        plain_text_edit_widget_->cut();
    }
}

inline void PlainTextEdit::copy() {
    if (plain_text_edit_widget_) {
        plain_text_edit_widget_->copy();
    }
}

inline void PlainTextEdit::paste() {
    if (plain_text_edit_widget_) {
        plain_text_edit_widget_->paste();
    }
}

inline bool PlainTextEdit::canUndo() const {
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    return plain_text_edit_widget_
               ? plain_text_edit_widget_->document()->isUndoAvailable()
               : false;
#else
    return plain_text_edit_widget_
               ? plain_text_edit_widget_->document()->isUndoAvailable()
               : false;
#endif
}

inline bool PlainTextEdit::canRedo() const {
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    return plain_text_edit_widget_
               ? plain_text_edit_widget_->document()->isRedoAvailable()
               : false;
#else
    return plain_text_edit_widget_
               ? plain_text_edit_widget_->document()->isRedoAvailable()
               : false;
#endif
}

// **Search operations**
inline bool PlainTextEdit::find(const QString &text,
                                QTextDocument::FindFlags flags) {
    return plain_text_edit_widget_ ? plain_text_edit_widget_->find(text, flags)
                                   : false;
}

// **Document operations**
inline QTextDocument *PlainTextEdit::getDocument() const {
    return plain_text_edit_widget_ ? plain_text_edit_widget_->document()
                                   : nullptr;
}

inline void PlainTextEdit::setDocument(QTextDocument *document) {
    if (plain_text_edit_widget_) {
        plain_text_edit_widget_->setDocument(document);
    }
}

inline bool PlainTextEdit::isModified() const {
    return plain_text_edit_widget_
               ? plain_text_edit_widget_->document()->isModified()
               : false;
}

inline void PlainTextEdit::setModified(bool modified) {
    if (plain_text_edit_widget_) {
        plain_text_edit_widget_->document()->setModified(modified);
    }
}

// **Getters**
inline bool PlainTextEdit::isReadOnly() const {
    return plain_text_edit_widget_ ? plain_text_edit_widget_->isReadOnly()
                                   : false;
}

inline QTextOption::WrapMode PlainTextEdit::getWordWrapMode() const {
    return plain_text_edit_widget_ ? plain_text_edit_widget_->wordWrapMode()
                                   : QTextOption::WrapAtWordBoundaryOrAnywhere;
}

inline QPlainTextEdit::LineWrapMode PlainTextEdit::getLineWrapMode() const {
    return plain_text_edit_widget_ ? plain_text_edit_widget_->lineWrapMode()
                                   : QPlainTextEdit::WidgetWidth;
}

inline int PlainTextEdit::getTabStopWidth() const {
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    return plain_text_edit_widget_
               ? plain_text_edit_widget_->tabStopDistance() / 1.0
               : 80;
#else
    return plain_text_edit_widget_ ? plain_text_edit_widget_->tabStopWidth()
                                   : 80;
#endif
}

inline int PlainTextEdit::getCursorWidth() const {
    return plain_text_edit_widget_ ? plain_text_edit_widget_->cursorWidth() : 1;
}

}  // namespace DeclarativeUI::Components
