/**
 * @file SearchBox.cpp
 * @brief Implementation of enhanced search box component
 */

#include "SearchBox.hpp"
#include <QDebug>

SearchBox::SearchBox(QWidget* parent)
    : QWidget(parent)
    , layout_(nullptr)
    , search_input_(nullptr)
    , search_button_(nullptr)
    , suggestions_(nullptr)
    , search_timer_(nullptr)
{
    setupUI();
    
    // Setup search timer for delayed search
    search_timer_ = new QTimer(this);
    search_timer_->setSingleShot(true);
    search_timer_->setInterval(300); // 300ms delay
    connect(search_timer_, &QTimer::timeout, this, &SearchBox::onSearchTriggered);
}

void SearchBox::setupUI() {
    layout_ = new QHBoxLayout(this);
    layout_->setContentsMargins(4, 4, 4, 4);
    layout_->setSpacing(4);
    
    // Create search input
    search_input_ = new QLineEdit();
    search_input_->setPlaceholderText("Search...");
    search_input_->setMinimumWidth(200);
    
    // Create search button
    search_button_ = new QPushButton("🔍");
    search_button_->setFixedSize(30, 30);
    search_button_->setToolTip("Search");
    
    // Create suggestions list (initially hidden)
    suggestions_ = new QListWidget();
    suggestions_->setMaximumHeight(150);
    suggestions_->hide();
    
    // Layout
    layout_->addWidget(search_input_);
    layout_->addWidget(search_button_);
    
    // Connect signals
    connect(search_input_, &QLineEdit::textChanged, this, &SearchBox::onTextChanged);
    connect(search_input_, &QLineEdit::returnPressed, this, &SearchBox::onSearchTriggered);
    connect(search_button_, &QPushButton::clicked, this, &SearchBox::onSearchTriggered);
    connect(suggestions_, &QListWidget::itemClicked, this, &SearchBox::onItemClicked);
    
    // Style the components
    setStyleSheet(
        "QLineEdit {"
        "    border: 2px solid #bdc3c7;"
        "    border-radius: 4px;"
        "    padding: 4px 8px;"
        "    font-size: 14px;"
        "}"
        "QLineEdit:focus {"
        "    border-color: #3498db;"
        "}"
        "QPushButton {"
        "    border: 2px solid #3498db;"
        "    border-radius: 4px;"
        "    background-color: #3498db;"
        "    color: white;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background-color: #2980b9;"
        "    border-color: #2980b9;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #21618c;"
        "}"
    );
}

void SearchBox::setSearchData(const QStringList& data) {
    search_data_ = data;
    
    // Setup completer
    completer_ = std::make_unique<QCompleter>(search_data_);
    completer_->setCaseSensitivity(Qt::CaseInsensitive);
    completer_->setFilterMode(Qt::MatchContains);
    search_input_->setCompleter(completer_.get());
    
    qDebug() << "Search data set with" << data.size() << "items";
}

void SearchBox::setPlaceholderText(const QString& text) {
    search_input_->setPlaceholderText(text);
}

void SearchBox::setSearchDelay(int ms) {
    search_timer_->setInterval(ms);
}

void SearchBox::onTextChanged() {
    QString text = search_input_->text();
    
    // Restart the search timer
    search_timer_->stop();
    if (!text.isEmpty()) {
        search_timer_->start();
        updateSuggestions();
    } else {
        suggestions_->hide();
    }
}

void SearchBox::onSearchTriggered() {
    QString query = search_input_->text().trimmed();
    if (!query.isEmpty()) {
        emit searchRequested(query);
        suggestions_->hide();
        qDebug() << "Search triggered for:" << query;
    }
}

void SearchBox::onItemClicked() {
    auto current_item = suggestions_->currentItem();
    if (current_item) {
        QString selected_text = current_item->text();
        search_input_->setText(selected_text);
        suggestions_->hide();
        emit itemSelected(selected_text);
        emit searchRequested(selected_text);
        qDebug() << "Item selected:" << selected_text;
    }
}

void SearchBox::updateSuggestions() {
    QString text = search_input_->text().trimmed();
    if (text.isEmpty() || search_data_.isEmpty()) {
        suggestions_->hide();
        return;
    }
    
    // Filter search data
    QStringList filtered_data;
    for (const QString& item : search_data_) {
        if (item.contains(text, Qt::CaseInsensitive)) {
            filtered_data.append(item);
            if (filtered_data.size() >= 10) break; // Limit to 10 suggestions
        }
    }
    
    if (filtered_data.isEmpty()) {
        suggestions_->hide();
        return;
    }
    
    // Update suggestions list
    suggestions_->clear();
    suggestions_->addItems(filtered_data);
    
    // Position suggestions below the search input
    QPoint global_pos = search_input_->mapToGlobal(QPoint(0, search_input_->height()));
    QPoint parent_pos = parentWidget()->mapFromGlobal(global_pos);
    
    suggestions_->setParent(parentWidget());
    suggestions_->move(parent_pos);
    suggestions_->setFixedWidth(search_input_->width() + search_button_->width() + layout_->spacing());
    suggestions_->show();
    suggestions_->raise();
}

#include "SearchBox.moc"
