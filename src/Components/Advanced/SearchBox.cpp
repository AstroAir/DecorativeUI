#include "SearchBox.hpp"

#include <QApplication>
#include <QKeyEvent>
#include <QFocusEvent>
#include <QListWidgetItem>
#include <QTextDocument>
#include <QRegularExpression>
#include <QDebug>

#include "../../Exceptions/UIExceptions.hpp"

namespace DeclarativeUI::Components::Advanced {

SearchBox::SearchBox(QObject* parent)
    : Core::UIElement(parent)
    , config_()
    , suggestion_timer_(std::make_unique<QTimer>())
    , suggestions_animation_(std::make_unique<QPropertyAnimation>())
{
    // Configure suggestion timer
    suggestion_timer_->setSingleShot(true);
    suggestion_timer_->setInterval(config_.suggestion_delay_ms);
    connect(suggestion_timer_.get(), &QTimer::timeout, this, &SearchBox::onSuggestionTimerTimeout);

    // Set default icons
    search_icon_ = QIcon(":/icons/search.png");
    clear_icon_ = QIcon(":/icons/clear.png");
}

SearchBox& SearchBox::placeholder(const QString& text) {
    return static_cast<SearchBox&>(setProperty("placeholder", text));
}

SearchBox& SearchBox::text(const QString& text) {
    if (search_input_) {
        search_input_->text(text);
    }
    return static_cast<SearchBox&>(setProperty("text", text));
}

SearchBox& SearchBox::suggestions(const QStringList& suggestions) {
    static_suggestions_.clear();
    for (const QString& suggestion : suggestions) {
        static_suggestions_.append(SearchSuggestion(suggestion));
    }
    return *this;
}

SearchBox& SearchBox::suggestions(const QList<SearchSuggestion>& suggestions) {
    static_suggestions_ = suggestions;
    return *this;
}

SearchBox& SearchBox::config(const SearchConfig& config) {
    config_ = config;
    if (suggestion_timer_) {
        suggestion_timer_->setInterval(config_.suggestion_delay_ms);
    }
    return *this;
}

SearchBox& SearchBox::autoComplete(bool enabled) {
    config_.auto_complete = enabled;
    return *this;
}

SearchBox& SearchBox::showHistory(bool enabled) {
    config_.show_history = enabled;
    return *this;
}

SearchBox& SearchBox::caseSensitive(bool enabled) {
    config_.case_sensitive = enabled;
    return *this;
}

SearchBox& SearchBox::fuzzyMatching(bool enabled) {
    config_.fuzzy_matching = enabled;
    return *this;
}

SearchBox& SearchBox::maxSuggestions(int count) {
    config_.max_suggestions = count;
    return *this;
}

SearchBox& SearchBox::suggestionDelay(int milliseconds) {
    config_.suggestion_delay_ms = milliseconds;
    if (suggestion_timer_) {
        suggestion_timer_->setInterval(milliseconds);
    }
    return *this;
}

SearchBox& SearchBox::onSearchChanged(std::function<void(const QString&)> handler) {
    search_changed_handler_ = std::move(handler);
    return *this;
}

SearchBox& SearchBox::onSearchSubmitted(std::function<void(const QString&)> handler) {
    search_submitted_handler_ = std::move(handler);
    return *this;
}

SearchBox& SearchBox::onSuggestionSelected(std::function<void(const SearchSuggestion&)> handler) {
    suggestion_selected_handler_ = std::move(handler);
    return *this;
}

SearchBox& SearchBox::onSuggestionRequested(std::function<QList<SearchSuggestion>(const QString&)> handler) {
    suggestion_requested_handler_ = std::move(handler);
    return *this;
}

SearchBox& SearchBox::onHistoryRequested(std::function<QStringList()> handler) {
    history_requested_handler_ = std::move(handler);
    return *this;
}

SearchBox& SearchBox::searchIcon(const QIcon& icon) {
    search_icon_ = icon;
    if (search_button_) {
        search_button_->icon(icon);
    }
    return *this;
}

SearchBox& SearchBox::clearIcon(const QIcon& icon) {
    clear_icon_ = icon;
    if (clear_button_) {
        clear_button_->icon(icon);
    }
    return *this;
}

SearchBox& SearchBox::suggestionTemplate(std::function<QWidget*(const SearchSuggestion&)> template_func) {
    suggestion_template_func_ = std::move(template_func);
    return *this;
}

QString SearchBox::getText() const {
    return search_input_ ? search_input_->getWidget()->property("text").toString() : QString();
}

void SearchBox::setText(const QString& text) {
    if (search_input_) {
        search_input_->text(text);
    }
}

void SearchBox::clearText() {
    setText("");
    hideSuggestions();
}

void SearchBox::addToHistory(const QString& text) {
    if (text.isEmpty()) return;

    // Remove if already exists
    search_history_.removeAll(text);

    // Add to front
    search_history_.prepend(text);

    // Limit history size
    while (search_history_.size() > config_.max_history_items) {
        search_history_.removeLast();
    }
}

void SearchBox::clearHistory() {
    search_history_.clear();
}

void SearchBox::showSuggestions() {
    if (suggestions_frame_ && !current_suggestions_.isEmpty()) {
        positionSuggestionsFrame();
        animateSuggestionsVisibility(true);
    }
}

void SearchBox::hideSuggestions() {
    if (suggestions_frame_) {
        animateSuggestionsVisibility(false);
    }
}

bool SearchBox::hasFocus() const {
    return search_input_ && search_input_->getWidget()->hasFocus();
}

void SearchBox::setFocus() {
    if (search_input_) {
        search_input_->getWidget()->setFocus();
    }
}

void SearchBox::initialize() {
    try {
        setupUI();
        setupSuggestions();
        setupAnimations();
        setupEventHandlers();
        setupStyling();

        qDebug() << "✅ SearchBox initialized successfully";
    } catch (const std::exception& e) {
        throw Exceptions::UIException("Failed to initialize SearchBox: " + std::string(e.what()));
    }
}

void SearchBox::onTextChanged(const QString& text) {
    // Emit signal
    emit searchChanged(text);

    // Call handler
    if (search_changed_handler_) {
        search_changed_handler_(text);
    }

    // Update clear button visibility
    if (clear_button_) {
        clear_button_->getWidget()->setVisible(!text.isEmpty());
    }

    // Start suggestion timer if enabled
    if (config_.show_suggestions && text.length() >= config_.min_chars_for_suggestions) {
        suggestion_timer_->start();
    } else {
        hideSuggestions();
    }
}

void SearchBox::onTextSubmitted() {
    QString text = getText();

    // Add to history
    if (config_.show_history && !text.isEmpty()) {
        addToHistory(text);
    }

    // Hide suggestions
    hideSuggestions();

    // Emit signal
    emit searchSubmitted(text);

    // Call handler
    if (search_submitted_handler_) {
        search_submitted_handler_(text);
    }
}

void SearchBox::onSuggestionClicked(const SearchSuggestion& suggestion) {
    // Set text
    setText(suggestion.text);

    // Add to history
    if (config_.show_history) {
        addToHistory(suggestion.text);
    }

    // Hide suggestions
    hideSuggestions();

    // Emit signal
    emit suggestionSelected(suggestion);

    // Call handler
    if (suggestion_selected_handler_) {
        suggestion_selected_handler_(suggestion);
    }
}

void SearchBox::onClearClicked() {
    clearText();
    setFocus();
}

void SearchBox::onSuggestionTimerTimeout() {
    updateSuggestions();
}

void SearchBox::updateSuggestions() {
    QString query = getText();
    if (query.length() < config_.min_chars_for_suggestions) {
        hideSuggestions();
        return;
    }

    QList<SearchSuggestion> suggestions;

    // Get suggestions from handler if available
    if (suggestion_requested_handler_) {
        suggestions = suggestion_requested_handler_(query);
    } else {
        // Use static suggestions
        suggestions = static_suggestions_;
    }

    // Add history suggestions if enabled
    if (config_.show_history && history_requested_handler_) {
        QStringList history = history_requested_handler_();
        for (const QString& item : history) {
            if (item.contains(query, config_.case_sensitive ? Qt::CaseSensitive : Qt::CaseInsensitive)) {
                suggestions.prepend(SearchSuggestion(item, "From history", QIcon(":/icons/history.png")));
            }
        }
    } else if (config_.show_history) {
        for (const QString& item : search_history_) {
            if (item.contains(query, config_.case_sensitive ? Qt::CaseSensitive : Qt::CaseInsensitive)) {
                suggestions.prepend(SearchSuggestion(item, "From history", QIcon(":/icons/history.png")));
            }
        }
    }

    // Filter suggestions
    filterSuggestions(query);

    // Show suggestions if any
    if (!current_suggestions_.isEmpty()) {
        populateSuggestionsList(current_suggestions_);
        showSuggestions();
    } else {
        hideSuggestions();
    }
}

void SearchBox::setupUI() {
    // Create main container
    main_container_ = std::make_unique<Container>();
    main_container_->initialize();

    // Create input container
    input_container_ = std::make_unique<Container>();
    input_container_->initialize();

    // Create search input
    search_input_ = std::make_unique<LineEdit>();
    search_input_->initialize();

    // Create search button
    search_button_ = std::make_unique<Button>();
    search_button_->icon(search_icon_);
    search_button_->style("border: none; background: transparent;");
    search_button_->initialize();

    // Create clear button
    clear_button_ = std::make_unique<Button>();
    clear_button_->icon(clear_icon_);
    clear_button_->style("border: none; background: transparent;");
    clear_button_->initialize();
    clear_button_->getWidget()->setVisible(false);

    // Create suggestions frame
    suggestions_frame_ = std::make_unique<QFrame>();
    suggestions_frame_->setWindowFlags(Qt::Popup);
    suggestions_frame_->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    suggestions_frame_->hide();

    // Create suggestions list
    suggestions_list_ = std::make_unique<QListWidget>(suggestions_frame_.get());

    // Set up layout
    auto* frame_layout = new QVBoxLayout(suggestions_frame_.get());
    frame_layout->setContentsMargins(0, 0, 0, 0);
    frame_layout->addWidget(suggestions_list_.get());

    // Set main widget
    setWidget(main_container_->getWidget());
}

void SearchBox::setupSuggestions() {
    if (!suggestions_list_) return;

    suggestions_list_->setAlternatingRowColors(true);
    suggestions_list_->setSelectionMode(QAbstractItemView::SingleSelection);
    suggestions_list_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    suggestions_list_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void SearchBox::setupAnimations() {
    if (!suggestions_frame_) return;

    suggestions_animation_->setTargetObject(suggestions_frame_.get());
    suggestions_animation_->setPropertyName("windowOpacity");
    suggestions_animation_->setDuration(200);
    suggestions_animation_->setEasingCurve(QEasingCurve::OutCubic);
}

void SearchBox::setupEventHandlers() {
    if (search_input_) {
        connect(search_input_->getWidget(), SIGNAL(textChanged(QString)),
                this, SLOT(onTextChanged(QString)));
        connect(search_input_->getWidget(), SIGNAL(returnPressed()),
                this, SLOT(onTextSubmitted()));
    }

    if (clear_button_) {
        clear_button_->onClick([this]() { onClearClicked(); });
    }

    if (search_button_) {
        search_button_->onClick([this]() { onTextSubmitted(); });
    }
}

void SearchBox::setupStyling() {
    if (main_container_) {
        main_container_->getWidget()->setStyleSheet(R"(
            QWidget {
                background-color: white;
                border: 1px solid #cccccc;
                border-radius: 4px;
            }
        )");
    }

    if (suggestions_frame_) {
        suggestions_frame_->setStyleSheet(R"(
            QFrame {
                background-color: white;
                border: 1px solid #cccccc;
                border-radius: 4px;
            }
            QListWidget {
                border: none;
                background-color: transparent;
            }
            QListWidget::item {
                padding: 8px;
                border-bottom: 1px solid #eeeeee;
            }
            QListWidget::item:hover {
                background-color: #f0f8ff;
            }
            QListWidget::item:selected {
                background-color: #e6f3ff;
            }
        )");
    }
}

void SearchBox::filterSuggestions(const QString& query) {
    QList<SearchSuggestion> all_suggestions = static_suggestions_;

    // Apply filtering based on configuration
    if (config_.fuzzy_matching) {
        current_suggestions_ = fuzzyMatch(query, all_suggestions);
    } else {
        current_suggestions_ = exactMatch(query, all_suggestions);
    }

    // Sort by priority and relevance
    std::sort(current_suggestions_.begin(), current_suggestions_.end(),
              [](const SearchSuggestion& a, const SearchSuggestion& b) {
                  return a.priority > b.priority;
              });

    // Limit to max suggestions
    if (current_suggestions_.size() > config_.max_suggestions) {
        current_suggestions_ = current_suggestions_.mid(0, config_.max_suggestions);
    }
}

QList<SearchSuggestion> SearchBox::exactMatch(const QString& query, const QList<SearchSuggestion>& suggestions) {
    QList<SearchSuggestion> matches;
    Qt::CaseSensitivity sensitivity = config_.case_sensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;

    for (const SearchSuggestion& suggestion : suggestions) {
        if (suggestion.text.contains(query, sensitivity)) {
            matches.append(suggestion);
        }
    }

    return matches;
}

QList<SearchSuggestion> SearchBox::fuzzyMatch(const QString& query, const QList<SearchSuggestion>& suggestions) {
    QList<QPair<SearchSuggestion, int>> scored_matches;

    for (const SearchSuggestion& suggestion : suggestions) {
        int score = calculateMatchScore(query, suggestion.text);
        if (score > 0) {
            scored_matches.append({suggestion, score});
        }
    }

    // Sort by score
    std::sort(scored_matches.begin(), scored_matches.end(),
              [](const QPair<SearchSuggestion, int>& a, const QPair<SearchSuggestion, int>& b) {
                  return a.second > b.second;
              });

    QList<SearchSuggestion> matches;
    for (const auto& pair : scored_matches) {
        matches.append(pair.first);
    }

    return matches;
}

int SearchBox::calculateMatchScore(const QString& query, const QString& text) {
    // Simple fuzzy matching score calculation
    int score = 0;
    int query_index = 0;

    for (int i = 0; i < text.length() && query_index < query.length(); ++i) {
        if (text[i].toLower() == query[query_index].toLower()) {
            score += (query.length() - query_index) * 10;
            query_index++;
        }
    }

    // Bonus for exact matches
    if (text.contains(query, Qt::CaseInsensitive)) {
        score += 100;
    }

    return query_index == query.length() ? score : 0;
}

void SearchBox::populateSuggestionsList(const QList<SearchSuggestion>& suggestions) {
    if (!suggestions_list_) return;

    suggestions_list_->clear();

    for (const SearchSuggestion& suggestion : suggestions) {
        auto* item = new QListWidgetItem();
        suggestions_list_->addItem(item);

        if (suggestion_template_func_) {
            auto* widget = suggestion_template_func_(suggestion);
            item->setSizeHint(widget->sizeHint());
            suggestions_list_->setItemWidget(item, widget);
        } else {
            // Default template
            auto* widget = new SuggestionItemWidget(suggestion);
            widget->setHighlightQuery(getText());
            item->setSizeHint(widget->sizeHint());
            suggestions_list_->setItemWidget(item, widget);
        }
    }
}

void SearchBox::positionSuggestionsFrame() {
    if (!suggestions_frame_ || !search_input_) return;

    QWidget* input_widget = search_input_->getWidget();
    QPoint global_pos = input_widget->mapToGlobal(QPoint(0, input_widget->height()));
    suggestions_frame_->move(global_pos);
    suggestions_frame_->resize(input_widget->width(), 200);
}

void SearchBox::animateSuggestionsVisibility(bool visible) {
    if (!suggestions_frame_ || !suggestions_animation_) return;

    if (visible) {
        suggestions_frame_->show();
        suggestions_animation_->setStartValue(0.0);
        suggestions_animation_->setEndValue(1.0);
    } else {
        suggestions_animation_->setStartValue(1.0);
        suggestions_animation_->setEndValue(0.0);
        connect(suggestions_animation_.get(), &QPropertyAnimation::finished,
                suggestions_frame_.get(), &QFrame::hide, Qt::UniqueConnection);
    }

    suggestions_animation_->start();
}

// SuggestionItemWidget implementation
SuggestionItemWidget::SuggestionItemWidget(const SearchSuggestion& suggestion, QWidget* parent)
    : QWidget(parent), suggestion_(suggestion) {
    setupUI();
    updateDisplay();
}

void SuggestionItemWidget::setupUI() {
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(8, 4, 8, 4);
    layout->setSpacing(8);

    // Icon
    icon_label_ = new QLabel();
    icon_label_->setFixedSize(16, 16);
    layout->addWidget(icon_label_);

    // Text container
    auto* text_layout = new QVBoxLayout();
    text_layout->setContentsMargins(0, 0, 0, 0);
    text_layout->setSpacing(2);

    text_label_ = new QLabel();
    text_label_->setStyleSheet("font-weight: bold;");
    text_layout->addWidget(text_label_);

    description_label_ = new QLabel();
    description_label_->setStyleSheet("color: #666; font-size: 11px;");
    text_layout->addWidget(description_label_);

    layout->addLayout(text_layout);
    layout->addStretch();
}

void SuggestionItemWidget::updateDisplay() {
    // Set icon
    if (!suggestion_.icon.isNull()) {
        icon_label_->setPixmap(suggestion_.icon.pixmap(16, 16));
    } else {
        icon_label_->hide();
    }

    // Set text with highlighting
    QString display_text = suggestion_.text;
    if (!highlight_query_.isEmpty()) {
        // Simple highlighting - replace with bold tags
        QString highlighted = suggestion_.text;
        QRegularExpression regex(QRegularExpression::escape(highlight_query_), QRegularExpression::CaseInsensitiveOption);
        highlighted.replace(regex, QString("<b>%1</b>").arg(highlight_query_));
        display_text = highlighted;
    }
    text_label_->setText(display_text);

    // Set description
    if (!suggestion_.description.isEmpty()) {
        description_label_->setText(suggestion_.description);
        description_label_->show();
    } else {
        description_label_->hide();
    }
}

void SuggestionItemWidget::setHighlightQuery(const QString& query) {
    highlight_query_ = query;
    updateDisplay();
}

QString SearchBox::highlightMatches(const QString& text, const QString& query) {
    if (query.isEmpty()) return text;

    QString highlighted = text;
    QRegularExpression regex(QRegularExpression::escape(query), QRegularExpression::CaseInsensitiveOption);
    highlighted.replace(regex, QString("<b>%1</b>").arg(query));

    return highlighted;
}

} // namespace DeclarativeUI::Components::Advanced
