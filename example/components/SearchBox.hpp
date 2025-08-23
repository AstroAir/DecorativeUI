/**
 * @file SearchBox.hpp
 * @brief Enhanced search box component with autocomplete and filtering
 */

#pragma once

#include <QCompleter>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QStringListModel>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <memory>

class SearchBox : public QWidget {
    Q_OBJECT

public:
    explicit SearchBox(QWidget* parent = nullptr);

    void setSearchData(const QStringList& data);
    void setPlaceholderText(const QString& text);
    void setSearchDelay(int ms);

signals:
    void searchRequested(const QString& query);
    void itemSelected(const QString& item);

private slots:
    void onTextChanged();
    void onSearchTriggered();
    void onItemClicked();

private:
    void setupUI();
    void updateSuggestions();

    QHBoxLayout* layout_;
    QLineEdit* search_input_;
    QPushButton* search_button_;
    QListWidget* suggestions_;

    QStringList search_data_;
    QTimer* search_timer_;
    std::unique_ptr<QCompleter> completer_;
};
