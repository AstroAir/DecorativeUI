#include "PerformanceProfilerWidget.hpp"
#include <QDebug>
#include <QTimer>
#include <QDateTime>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonObject>
#include <QElapsedTimer>

namespace DeclarativeUI::Debug {

PerformanceProfilerWidget::PerformanceProfilerWidget(QWidget* parent)
    : QWidget(parent), is_profiling_(false), profiling_timer_(nullptr), profiling_start_time_(0) {
    layout_ = new QVBoxLayout(this);

    start_button_ = new QPushButton("Start Profiling", this);
    stop_button_ = new QPushButton("Stop Profiling", this);
    reset_button_ = new QPushButton("Reset", this);
    export_button_ = new QPushButton("Export Profile", this);
    progress_bar_ = new QProgressBar(this);
    status_label_ = new QLabel("Ready", this);

    // Initially disable stop button
    stop_button_->setEnabled(false);

    // Set up progress bar
    progress_bar_->setRange(0, 100);
    progress_bar_->setValue(0);

    layout_->addWidget(start_button_);
    layout_->addWidget(stop_button_);
    layout_->addWidget(reset_button_);
    layout_->addWidget(export_button_);
    layout_->addWidget(progress_bar_);
    layout_->addWidget(status_label_);

    connect(start_button_, &QPushButton::clicked, this, &PerformanceProfilerWidget::onStartProfilingClicked);
    connect(stop_button_, &QPushButton::clicked, this, &PerformanceProfilerWidget::onStopProfilingClicked);
    connect(reset_button_, &QPushButton::clicked, this, &PerformanceProfilerWidget::onResetProfilingClicked);
    connect(export_button_, &QPushButton::clicked, this, &PerformanceProfilerWidget::onExportProfileClicked);

    // Set up profiling timer for progress updates
    profiling_timer_ = new QTimer(this);
    connect(profiling_timer_, &QTimer::timeout, this, &PerformanceProfilerWidget::updateProfilingProgress);

    qDebug() << "PerformanceProfilerWidget created";
}

void PerformanceProfilerWidget::onStartProfilingClicked() {
    if (is_profiling_) {
        return;
    }

    is_profiling_ = true;
    profiling_start_time_ = QDateTime::currentMSecsSinceEpoch();

    // Update UI state
    start_button_->setEnabled(false);
    stop_button_->setEnabled(true);
    status_label_->setText("Profiling...");
    progress_bar_->setValue(0);

    // Start the progress timer (update every 100ms)
    profiling_timer_->start(100);

    qDebug() << "Profiling started at" << QDateTime::currentDateTime().toString();
}

void PerformanceProfilerWidget::onStopProfilingClicked() {
    if (!is_profiling_) {
        return;
    }

    is_profiling_ = false;
    profiling_timer_->stop();

    qint64 duration = QDateTime::currentMSecsSinceEpoch() - profiling_start_time_;

    // Update UI state
    start_button_->setEnabled(true);
    stop_button_->setEnabled(false);
    status_label_->setText(QString("Stopped - Duration: %1s").arg(duration / 1000.0, 0, 'f', 1));
    progress_bar_->setValue(100);

    qDebug() << "Profiling stopped. Duration:" << duration << "ms";
}

void PerformanceProfilerWidget::onResetProfilingClicked() {
    if (is_profiling_) {
        // Stop profiling first
        onStopProfilingClicked();
    }

    // Reset UI state
    status_label_->setText("Ready");
    progress_bar_->setValue(0);
    profiling_start_time_ = 0;

    qDebug() << "Profiling data reset";
}

void PerformanceProfilerWidget::onExportProfileClicked() {
    QString defaultPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString defaultFileName = QString("performance_profile_%1.json")
                             .arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss"));
    QString defaultFilePath = defaultPath + "/" + defaultFileName;

    QString fileName = QFileDialog::getSaveFileName(
        this,
        "Export Performance Profile",
        defaultFilePath,
        "JSON Files (*.json);;All Files (*)"
    );

    if (fileName.isEmpty()) {
        return;
    }

    // Create sample profile data
    QJsonObject profileData;
    profileData["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    profileData["duration_ms"] = is_profiling_ ?
        (QDateTime::currentMSecsSinceEpoch() - profiling_start_time_) : 0;
    profileData["status"] = is_profiling_ ? "active" : "stopped";

    // Add sample performance metrics
    QJsonObject metrics;
    metrics["cpu_usage"] = 45.2;
    metrics["memory_usage_mb"] = 256;
    metrics["frame_rate"] = 60.0;
    metrics["render_time_ms"] = 16.7;
    profileData["metrics"] = metrics;

    // Write to file
    QJsonDocument doc(profileData);
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();

        QMessageBox::information(this, "Export Successful",
                               QString("Profile exported to: %1").arg(fileName));
        qDebug() << "Profile exported to:" << fileName;
    } else {
        QMessageBox::warning(this, "Export Failed",
                           QString("Could not open file for writing: %1").arg(fileName));
    }
}

void PerformanceProfilerWidget::updateProfilingProgress() {
    if (!is_profiling_) {
        return;
    }

    qint64 elapsed = QDateTime::currentMSecsSinceEpoch() - profiling_start_time_;
    double seconds = elapsed / 1000.0;

    // Update status with elapsed time
    status_label_->setText(QString("Profiling... %1s").arg(seconds, 0, 'f', 1));

    // Update progress bar (simulate progress over 30 seconds)
    int progress = qMin(100, static_cast<int>((elapsed / 30000.0) * 100));
    progress_bar_->setValue(progress);
}

}  // namespace DeclarativeUI::Debug
