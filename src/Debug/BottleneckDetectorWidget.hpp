#pragma once

#include <QWidget>
#include <QPushButton>
#include <QListWidget>
#include <QVBoxLayout>

namespace DeclarativeUI::Debug {

class BottleneckDetectorWidget : public QWidget {
    Q_OBJECT

public:
    explicit BottleneckDetectorWidget(QWidget* parent = nullptr);
    ~BottleneckDetectorWidget() = default;

private slots:
    void onBottleneckSelected();
    void onRefreshClicked();
    void onResolveBottleneckClicked();

private:
    QPushButton* refresh_button_;
    QPushButton* resolve_button_;
    QListWidget* bottleneck_list_;
    QVBoxLayout* layout_;
};

}  // namespace DeclarativeUI::Debug
