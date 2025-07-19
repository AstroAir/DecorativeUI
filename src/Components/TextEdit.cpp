// Components/TextEdit.cpp
#include "TextEdit.hpp"

namespace DeclarativeUI::Components {

// **TextEdit Implementation**
TextEdit::TextEdit(QObject *parent)
    : UIElement(parent), text_edit_widget_(nullptr) {}

TextEdit &TextEdit::text(const QString &text) {
    return static_cast<TextEdit &>(setProperty("plainText", text));
}

TextEdit &TextEdit::html(const QString &html) {
    return static_cast<TextEdit &>(setProperty("html", html));
}

TextEdit &TextEdit::placeholder(const QString &placeholder) {
    return static_cast<TextEdit &>(setProperty("placeholderText", placeholder));
}

TextEdit &TextEdit::readOnly(bool readonly) {
    return static_cast<TextEdit &>(setProperty("readOnly", readonly));
}

TextEdit &TextEdit::acceptRichText(bool accept) {
    return static_cast<TextEdit &>(setProperty("acceptRichText", accept));
}

TextEdit &TextEdit::wordWrapMode(QTextOption::WrapMode mode) {
    return static_cast<TextEdit &>(
        setProperty("wordWrapMode", static_cast<int>(mode)));
}

TextEdit &TextEdit::lineWrapMode(QTextEdit::LineWrapMode mode) {
    return static_cast<TextEdit &>(
        setProperty("lineWrapMode", static_cast<int>(mode)));
}

TextEdit &TextEdit::lineWrapColumnOrWidth(int width) {
    return static_cast<TextEdit &>(setProperty("lineWrapColumnOrWidth", width));
}

TextEdit &TextEdit::tabStopWidth(int width) {
    return static_cast<TextEdit &>(setProperty("tabStopWidth", width));
}

TextEdit &TextEdit::cursorWidth(int width) {
    return static_cast<TextEdit &>(setProperty("cursorWidth", width));
}

TextEdit &TextEdit::textInteractionFlags(
    Qt::TextInteractionFlags flags) {
    return static_cast<TextEdit &>(
        setProperty("textInteractionFlags", static_cast<int>(flags)));
}

TextEdit &TextEdit::font(const QFont &font) {
    return static_cast<TextEdit &>(setProperty("font", font));
}

TextEdit &TextEdit::textColor(const QColor &color) {
    return static_cast<TextEdit &>(setProperty("textColor", color));
}

TextEdit &TextEdit::backgroundColor(const QColor &color) {
    return static_cast<TextEdit &>(setProperty("backgroundColor", color));
}

TextEdit &TextEdit::selectionColor(const QColor &color) {
    return static_cast<TextEdit &>(setProperty("selectionColor", color));
}

TextEdit &TextEdit::onTextChanged(std::function<void()> handler) {
    text_changed_handler_ = std::move(handler);
    return *this;
}

TextEdit &TextEdit::onSelectionChanged(std::function<void()> handler) {
    selection_changed_handler_ = std::move(handler);
    return *this;
}

TextEdit &TextEdit::onCursorPositionChanged(
    std::function<void()> handler) {
    cursor_position_changed_handler_ = std::move(handler);
    return *this;
}

TextEdit &TextEdit::style(const QString &stylesheet) {
    return static_cast<TextEdit &>(setProperty("styleSheet", stylesheet));
}

void TextEdit::initialize() {
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
QString TextEdit::getText() const {
    return text_edit_widget_ ? text_edit_widget_->toPlainText() : QString();
}

QString TextEdit::getHtml() const {
    return text_edit_widget_ ? text_edit_widget_->toHtml() : QString();
}

QString TextEdit::getPlainText() const {
    return text_edit_widget_ ? text_edit_widget_->toPlainText() : QString();
}

void TextEdit::setText(const QString &text) {
    if (text_edit_widget_) {
        text_edit_widget_->setPlainText(text);
    }
}

void TextEdit::setHtml(const QString &html) {
    if (text_edit_widget_) {
        text_edit_widget_->setHtml(html);
    }
}

void TextEdit::setPlainText(const QString &text) {
    if (text_edit_widget_) {
        text_edit_widget_->setPlainText(text);
    }
}

void TextEdit::append(const QString &text) {
    if (text_edit_widget_) {
        text_edit_widget_->append(text);
    }
}

void TextEdit::insertText(const QString &text) {
    if (text_edit_widget_) {
        text_edit_widget_->insertPlainText(text);
    }
}

void TextEdit::insertHtml(const QString &html) {
    if (text_edit_widget_) {
        text_edit_widget_->insertHtml(html);
    }
}

void TextEdit::clear() {
    if (text_edit_widget_) {
        text_edit_widget_->clear();
    }
}

// **Cursor operations**
QTextCursor TextEdit::getCursor() const {
    return text_edit_widget_ ? text_edit_widget_->textCursor() : QTextCursor();
}

void TextEdit::setCursor(const QTextCursor &cursor) {
    if (text_edit_widget_) {
        text_edit_widget_->setTextCursor(cursor);
    }
}

void TextEdit::moveCursor(QTextCursor::MoveOperation operation,
                         QTextCursor::MoveMode mode) {
    if (text_edit_widget_) {
        text_edit_widget_->moveCursor(operation, mode);
    }
}

void TextEdit::selectAll() {
    if (text_edit_widget_) {
        text_edit_widget_->selectAll();
    }
}

void TextEdit::selectWord() {
    if (text_edit_widget_) {
        QTextCursor cursor = text_edit_widget_->textCursor();
        cursor.select(QTextCursor::WordUnderCursor);
        text_edit_widget_->setTextCursor(cursor);
    }
}

void TextEdit::selectLine() {
    if (text_edit_widget_) {
        QTextCursor cursor = text_edit_widget_->textCursor();
        cursor.select(QTextCursor::LineUnderCursor);
        text_edit_widget_->setTextCursor(cursor);
    }
}

QString TextEdit::getSelectedText() const {
    return text_edit_widget_ ? text_edit_widget_->textCursor().selectedText()
                             : QString();
}

bool TextEdit::hasSelection() const {
    return text_edit_widget_ ? text_edit_widget_->textCursor().hasSelection()
                             : false;
}

// **Edit operations**
void TextEdit::undo() {
    if (text_edit_widget_) {
        text_edit_widget_->undo();
    }
}

void TextEdit::redo() {
    if (text_edit_widget_) {
        text_edit_widget_->redo();
    }
}

void TextEdit::cut() {
    if (text_edit_widget_) {
        text_edit_widget_->cut();
    }
}

void TextEdit::copy() {
    if (text_edit_widget_) {
        text_edit_widget_->copy();
    }
}

void TextEdit::paste() {
    if (text_edit_widget_) {
        text_edit_widget_->paste();
    }
}

bool TextEdit::canUndo() const {
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    return text_edit_widget_ ? text_edit_widget_->document()->isUndoAvailable()
                             : false;
#else
    return text_edit_widget_ ? text_edit_widget_->document()->isUndoAvailable()
                             : false;
#endif
}

bool TextEdit::canRedo() const {
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    return text_edit_widget_ ? text_edit_widget_->document()->isRedoAvailable()
                             : false;
#else
    return text_edit_widget_ ? text_edit_widget_->document()->isRedoAvailable()
                             : false;
#endif
}

// **Search operations**
bool TextEdit::find(const QString &text,
                   QTextDocument::FindFlags flags) {
    return text_edit_widget_ ? text_edit_widget_->find(text, flags) : false;
}

// **Document operations**
QTextDocument *TextEdit::getDocument() const {
    return text_edit_widget_ ? text_edit_widget_->document() : nullptr;
}

void TextEdit::setDocument(QTextDocument *document) {
    if (text_edit_widget_) {
        text_edit_widget_->setDocument(document);
    }
}

bool TextEdit::isModified() const {
    return text_edit_widget_ ? text_edit_widget_->document()->isModified()
                             : false;
}

void TextEdit::setModified(bool modified) {
    if (text_edit_widget_) {
        text_edit_widget_->document()->setModified(modified);
    }
}

// **Getters**
bool TextEdit::isReadOnly() const {
    return text_edit_widget_ ? text_edit_widget_->isReadOnly() : false;
}

bool TextEdit::getAcceptRichText() const {
    return text_edit_widget_ ? text_edit_widget_->acceptRichText() : true;
}

QTextOption::WrapMode TextEdit::getWordWrapMode() const {
    return text_edit_widget_ ? text_edit_widget_->wordWrapMode()
                             : QTextOption::WrapAtWordBoundaryOrAnywhere;
}

QTextEdit::LineWrapMode TextEdit::getLineWrapMode() const {
    return text_edit_widget_ ? text_edit_widget_->lineWrapMode()
                             : QTextEdit::WidgetWidth;
}

int TextEdit::getLineWrapColumnOrWidth() const {
    return text_edit_widget_ ? text_edit_widget_->lineWrapColumnOrWidth() : 0;
}

int TextEdit::getTabStopWidth() const {
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    return text_edit_widget_ ? text_edit_widget_->tabStopDistance() / 1.0 : 80;
#else
    return text_edit_widget_ ? text_edit_widget_->tabStopWidth() : 80;
#endif
}

int TextEdit::getCursorWidth() const {
    return text_edit_widget_ ? text_edit_widget_->cursorWidth() : 1;
}



}  // namespace DeclarativeUI::Components
