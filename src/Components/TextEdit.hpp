#pragma once

#include <QColor>
#include <QFont>
#include <QPlainTextEdit>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextEdit>
#include <QSyntaxHighlighter>
#include <QCompleter>
#include <QTimer>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>
#include <QUndoStack>
#include <QTextBrowser>
#include <memory>
#include <functional>

#include "../Core/UIElement.hpp"

namespace DeclarativeUI::Components {

/**
 * @brief Text validation result for enhanced text edit
 */
struct TextEditValidationResult {
    bool is_valid;
    QString error_message;
    QString suggestion;
    int error_position = -1;

    TextEditValidationResult(bool valid = true, const QString& error = "", const QString& hint = "", int pos = -1)
        : is_valid(valid), error_message(error), suggestion(hint), error_position(pos) {}
};

/**
 * @brief Find and replace options
 */
struct FindReplaceOptions {
    bool case_sensitive = false;
    bool whole_words = false;
    bool use_regex = false;
    bool wrap_around = true;
    bool backward = false;
};

class TextEdit : public Core::UIElement {
    Q_OBJECT

public:
    explicit TextEdit(QObject *parent = nullptr);

    // **Basic fluent interface (backward compatible)**
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

    // **Enhanced fluent interface**
    TextEdit &tooltip(const QString& tooltip_text);
    TextEdit &accessibleName(const QString& name);
    TextEdit &accessibleDescription(const QString& description);
    TextEdit &shortcut(const QKeySequence& shortcut);

    // **Validation**
    TextEdit &required(bool required = true);
    TextEdit &minLength(int min_length);
    TextEdit &maxLength(int max_length);
    TextEdit &validator(std::function<TextEditValidationResult(const QString&)> validation_func);
    TextEdit &onValidationFailed(std::function<void(const QString&, int)> error_handler);
    TextEdit &validateOnType(bool validate_while_typing = true);
    TextEdit &validateOnFocus(bool validate_on_focus_lost = true);
    TextEdit &spellCheck(bool enabled = true);
    TextEdit &grammarCheck(bool enabled = true);

    // **Auto-completion and suggestions**
    TextEdit &autoComplete(const QStringList& completions);
    TextEdit &autoCompleteMode(QCompleter::CompletionMode mode);
    TextEdit &wordSuggestions(bool enabled = true);
    TextEdit &customCompleter(QCompleter* completer);

    // **Syntax highlighting and formatting**
    TextEdit &syntaxHighlighter(QSyntaxHighlighter* highlighter);
    TextEdit &language(const QString& language_name);
    TextEdit &lineNumbers(bool enabled = true);
    TextEdit &currentLineHighlight(bool enabled = true, const QColor& color = QColor(255, 255, 0, 50));
    TextEdit &bracketMatching(bool enabled = true);
    TextEdit &codeCompletion(bool enabled = true);
    TextEdit &autoIndent(bool enabled = true);
    TextEdit &tabsToSpaces(bool enabled = true, int spaces = 4);

    // **Visual enhancements**
    TextEdit &dropShadow(bool enabled = true, const QColor& color = QColor(0, 0, 0, 80));
    TextEdit &hoverEffect(bool enabled = true);
    TextEdit &focusAnimation(bool enabled = true);
    TextEdit &borderRadius(int radius);
    TextEdit &customColors(const QColor& background, const QColor& text = QColor(), const QColor& border = QColor());
    TextEdit &margins(int left, int top, int right, int bottom);

    // **Advanced features**
    TextEdit &findReplace(bool enabled = true);
    TextEdit &undoRedo(bool enabled = true);
    TextEdit &autoSave(bool enabled = true, int interval_seconds = 30);
    TextEdit &wordCount(bool enabled = true);
    TextEdit &characterCount(bool enabled = true);
    TextEdit &readingTime(bool enabled = true);
    TextEdit &textStatistics(bool enabled = true);

    // **Collaboration features**
    TextEdit &trackChanges(bool enabled = true);
    TextEdit &comments(bool enabled = true);
    TextEdit &versionHistory(bool enabled = true);
    TextEdit &collaborativeEditing(bool enabled = true);

    // **Export and import**
    TextEdit &supportedFormats(const QStringList& formats);
    TextEdit &exportFormat(const QString& default_format);
    TextEdit &importFormat(const QString& default_format);

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
