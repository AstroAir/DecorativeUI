#include "BottleneckDetectorWidget.hpp"
#include <QDebug>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QTimer>

namespace DeclarativeUI::Debug {

BottleneckDetectorWidget::BottleneckDetectorWidget(QWidget* parent)
    : QWidget(parent) {
    layout_ = new QVBoxLayout(this);

    refresh_button_ = new QPushButton("Refresh", this);
    resolve_button_ = new QPushButton("Resolve", this);
    bottleneck_list_ = new QListWidget(this);

    layout_->addWidget(refresh_button_);
    layout_->addWidget(resolve_button_);
    layout_->addWidget(bottleneck_list_);

    connect(refresh_button_, &QPushButton::clicked, this,
            &BottleneckDetectorWidget::onRefreshClicked);
    connect(resolve_button_, &QPushButton::clicked, this,
            &BottleneckDetectorWidget::onResolveBottleneckClicked);
    connect(bottleneck_list_, &QListWidget::itemClicked, this,
            &BottleneckDetectorWidget::onBottleneckSelected);

    // Initially disable resolve button until a bottleneck is selected
    resolve_button_->setEnabled(false);

    // Add some sample bottlenecks for demonstration
    addSampleBottlenecks();

    qDebug() << "BottleneckDetectorWidget created";
}

void BottleneckDetectorWidget::onBottleneckSelected() {
    QListWidgetItem* currentItem = bottleneck_list_->currentItem();
    if (currentItem) {
        resolve_button_->setEnabled(true);
        QString bottleneckText = currentItem->text();
        qDebug() << "Bottleneck selected:" << bottleneckText;

        // You could emit a signal here to notify other components
        // emit bottleneckSelected(bottleneckText);
    } else {
        resolve_button_->setEnabled(false);
    }
}

void BottleneckDetectorWidget::onRefreshClicked() {
    qDebug() << "Refreshing bottleneck detection...";

    // Clear current list
    bottleneck_list_->clear();

    // Simulate refresh delay
    refresh_button_->setEnabled(false);
    refresh_button_->setText("Refreshing...");

    // Use a timer to simulate async bottleneck detection
    QTimer::singleShot(1500, this, [this]() {
        // Re-populate with updated bottlenecks
        addSampleBottlenecks();

        refresh_button_->setEnabled(true);
        refresh_button_->setText("Refresh");

        qDebug() << "Bottleneck detection refresh completed";
    });
}

void BottleneckDetectorWidget::onResolveBottleneckClicked() {
    QListWidgetItem* currentItem = bottleneck_list_->currentItem();
    if (!currentItem) {
        return;
    }

    QString bottleneckText = currentItem->text();

    // Show confirmation dialog
    int result = QMessageBox::question(
        this, "Resolve Bottleneck",
        QString("Attempt to resolve the following bottleneck?\n\n%1\n\nNote: "
                "This is a demonstration. In a real implementation, this would "
                "apply specific optimizations.")
            .arg(bottleneckText),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

    if (result == QMessageBox::Yes) {
        // Simulate resolution process
        resolve_button_->setEnabled(false);
        resolve_button_->setText("Resolving...");

        QTimer::singleShot(2000, this, [this, currentItem]() {
            // Mark as resolved (change text color or add indicator)
            currentItem->setText(currentItem->text() + " [RESOLVED]");
            currentItem->setForeground(QColor(0, 128, 0));  // Green color

            resolve_button_->setEnabled(true);
            resolve_button_->setText("Resolve");

            QMessageBox::information(
                this, "Resolution Complete",
                "Bottleneck resolution attempt completed.\n\nIn a real "
                "implementation, this would apply specific optimizations based "
                "on the bottleneck type.");

            qDebug() << "Bottleneck resolution completed";
        });
    }
}

void BottleneckDetectorWidget::addSampleBottlenecks() {
    // Add some sample bottlenecks for demonstration
    QStringList sampleBottlenecks = {"CPU - Animation processing (85% usage)",
                                     "Memory - Large texture cache (512MB)",
                                     "I/O - Frequent file system access",
                                     "GPU - Complex shader compilation",
                                     "Network - Slow API response times"};

    for (const QString& bottleneck : sampleBottlenecks) {
        QListWidgetItem* item = new QListWidgetItem(bottleneck);
        item->setToolTip(
            "Click to select, then use Resolve button to attempt automatic "
            "resolution");
        bottleneck_list_->addItem(item);
    }

    qDebug() << "Added" << sampleBottlenecks.size() << "sample bottlenecks";
}

}  // namespace DeclarativeUI::Debug
