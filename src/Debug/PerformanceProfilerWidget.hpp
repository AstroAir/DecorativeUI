#pragma once

#include <QWidget>
#include <QPushButton>
#include <QProgressBar>
#include <QLabel>
#include <QVBoxLayout>

namespace DeclarativeUI::Debug {

class PerformanceProfilerWidget : public QWidget {
    Q_OBJECT

public:
    explicit PerformanceProfilerWidget(QWidget* parent = nullptr);
    ~PerformanceProfilerWidget() = default;

private slots:
    void onStartProfilingClicked();
    void onStopProfilingClicked();
    void onResetProfilingClicked();
    void onExportProfileClicked();

private:
    QPushButton* start_button_;
    QPushButton* stop_button_;
    QPushButton* reset_button_;
    QPushButton* export_button_;
    QProgressBar* progress_bar_;
    QLabel* status_label_;
    QVBoxLayout* layout_;
};

}  // namespace DeclarativeUI::Debug
