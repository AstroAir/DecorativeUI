#pragma once

#include <QWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QComboBox>
#include <QVBoxLayout>

namespace DeclarativeUI::Debug {

class DebuggingConsole : public QWidget {
    Q_OBJECT

public:
    explicit DebuggingConsole(QWidget* parent = nullptr);
    ~DebuggingConsole() = default;

private slots:
    void onExportLogClicked();
    void onFilterChanged();

private:
    QPushButton* export_button_;
    QComboBox* filter_combo_;
    QTextEdit* log_display_;
    QVBoxLayout* layout_;
};

}  // namespace DeclarativeUI::Debug
