#pragma once

/**
 * @file SearchBox.hpp
 * @brief Advanced search input component with auto-completion, filtering, and search history
 * 
 * The SearchBox component provides a comprehensive search interface with:
 * - Real-time auto-completion
 * - Search suggestions and history
 * - Advanced filtering options
 * - Keyboard navigation
 * - Customizable result templates
 */

#include <QCompleter>
#include <QStringListModel>
#include <QTimer>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QFrame>
#include <QLabel>
#include <QToolButton>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>
#include <functional>
#include <memory>

#include "../../Core/UIElement.hpp"
#include "../LineEdit.hpp"
#include "../Button.hpp"
#include "../Container.hpp"
#include "../ListView.hpp"

namespace DeclarativeUI::Components::Advanced {

/**
 * @brief Search suggestion data structure
 */
struct SearchSuggestion {
    QString text;
    QString description;
    QIcon icon;
    QVariant data;
    int priority = 0;
    
    SearchSuggestion() = default;
    SearchSuggestion(const QString& text, const QString& desc = QString(), 
                    const QIcon& icon = QIcon(), const QVariant& data = QVariant(), int priority = 0)
        : text(text), description(desc), icon(icon), data(data), priority(priority) {}
};

/**
 * @brief Advanced search configuration
 */
struct SearchConfig {
    bool auto_complete = true;
    bool show_suggestions = true;
    bool show_history = true;
    bool case_sensitive = false;
    int min_chars_for_suggestions = 1;
    int max_suggestions = 10;
    int max_history_items = 20;
    int suggestion_delay_ms = 300;
    bool highlight_matches = true;
    bool fuzzy_matching = false;
};

/**
 * @brief Advanced SearchBox component with auto-completion and rich features
 */
class SearchBox : public Core::UIElement {
    Q_OBJECT

public:
    explicit SearchBox(QObject* parent = nullptr);
    ~SearchBox() override = default;

    // **Fluent interface for configuration**
    SearchBox& placeholder(const QString& text);
    SearchBox& text(const QString& text);
    SearchBox& suggestions(const QStringList& suggestions);
    SearchBox& suggestions(const QList<SearchSuggestion>& suggestions);
    SearchBox& config(const SearchConfig& config);
    SearchBox& autoComplete(bool enabled);
    SearchBox& showHistory(bool enabled);
    SearchBox& caseSensitive(bool enabled);
    SearchBox& fuzzyMatching(bool enabled);
    SearchBox& maxSuggestions(int count);
    SearchBox& suggestionDelay(int milliseconds);
    
    // **Event handlers**
    SearchBox& onSearchChanged(std::function<void(const QString&)> handler);
    SearchBox& onSearchSubmitted(std::function<void(const QString&)> handler);
    SearchBox& onSuggestionSelected(std::function<void(const SearchSuggestion&)> handler);
    SearchBox& onSuggestionRequested(std::function<QList<SearchSuggestion>(const QString&)> handler);
    SearchBox& onHistoryRequested(std::function<QStringList()> handler);
    
    // **Styling and appearance**
    SearchBox& searchIcon(const QIcon& icon);
    SearchBox& clearIcon(const QIcon& icon);
    SearchBox& suggestionTemplate(std::function<QWidget*(const SearchSuggestion&)> template_func);
    
    // **Public methods**
    QString getText() const;
    void setText(const QString& text);
    void clearText();
    void addToHistory(const QString& text);
    void clearHistory();
    void showSuggestions();
    void hideSuggestions();
    bool hasFocus() const;
    void setFocus();
    
    // **UIElement interface**
    void initialize() override;

signals:
    void searchChanged(const QString& text);
    void searchSubmitted(const QString& text);
    void suggestionSelected(const SearchSuggestion& suggestion);
    void focusChanged(bool focused);

private slots:
    void onTextChanged(const QString& text);
    void onTextSubmitted();
    void onSuggestionClicked(const SearchSuggestion& suggestion);
    void onClearClicked();
    void onSuggestionTimerTimeout();
    void updateSuggestions();

private:
    // **Core components**
    std::unique_ptr<Container> main_container_;
    std::unique_ptr<Container> input_container_;
    std::unique_ptr<LineEdit> search_input_;
    std::unique_ptr<Button> search_button_;
    std::unique_ptr<Button> clear_button_;
    std::unique_ptr<QFrame> suggestions_frame_;
    std::unique_ptr<QListWidget> suggestions_list_;
    
    // **Configuration and state**
    SearchConfig config_;
    QStringList search_history_;
    QList<SearchSuggestion> current_suggestions_;
    QList<SearchSuggestion> static_suggestions_;
    
    // **Timers and animations**
    std::unique_ptr<QTimer> suggestion_timer_;
    std::unique_ptr<QPropertyAnimation> suggestions_animation_;
    
    // **Event handlers**
    std::function<void(const QString&)> search_changed_handler_;
    std::function<void(const QString&)> search_submitted_handler_;
    std::function<void(const SearchSuggestion&)> suggestion_selected_handler_;
    std::function<QList<SearchSuggestion>(const QString&)> suggestion_requested_handler_;
    std::function<QStringList()> history_requested_handler_;
    std::function<QWidget*(const SearchSuggestion&)> suggestion_template_func_;
    
    // **Icons**
    QIcon search_icon_;
    QIcon clear_icon_;
    
    // **Helper methods**
    void setupUI();
    void setupSuggestions();
    void setupAnimations();
    void setupEventHandlers();
    void setupStyling();
    
    void filterSuggestions(const QString& query);
    void populateSuggestionsList(const QList<SearchSuggestion>& suggestions);
    void applySuggestionTemplate(QListWidgetItem* item, const SearchSuggestion& suggestion);
    void positionSuggestionsFrame();
    void animateSuggestionsVisibility(bool visible);
    
    QList<SearchSuggestion> fuzzyMatch(const QString& query, const QList<SearchSuggestion>& suggestions);
    QList<SearchSuggestion> exactMatch(const QString& query, const QList<SearchSuggestion>& suggestions);
    int calculateMatchScore(const QString& query, const QString& text);
    QString highlightMatches(const QString& text, const QString& query);
    
    bool eventFilter(QObject* object, QEvent* event) override;
    void keyPressEvent(QKeyEvent* event);
    void focusInEvent(QFocusEvent* event);
    void focusOutEvent(QFocusEvent* event);
};

/**
 * @brief Custom suggestion list widget with enhanced keyboard navigation
 */
class SuggestionListWidget : public QListWidget {
    Q_OBJECT

public:
    explicit SuggestionListWidget(QWidget* parent = nullptr);

signals:
    void suggestionActivated(const SearchSuggestion& suggestion);
    void escapePressed();

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    void activateCurrentSuggestion();
};

/**
 * @brief Custom suggestion item widget for rich display
 */
class SuggestionItemWidget : public QWidget {
    Q_OBJECT

public:
    explicit SuggestionItemWidget(const SearchSuggestion& suggestion, QWidget* parent = nullptr);
    
    void setHighlightQuery(const QString& query);
    const SearchSuggestion& getSuggestion() const { return suggestion_; }

private:
    SearchSuggestion suggestion_;
    QLabel* icon_label_;
    QLabel* text_label_;
    QLabel* description_label_;
    QString highlight_query_;
    
    void setupUI();
    void updateDisplay();
};

} // namespace DeclarativeUI::Components::Advanced

Q_DECLARE_METATYPE(DeclarativeUI::Components::Advanced::SearchSuggestion)
