#include <QApplication>
#include <QElapsedTimer>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QTextEdit>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <chrono>

// Include DeclarativeUI headers
#include "src/Components/Button.hpp"
#include "src/Components/Label.hpp"
#include "src/Core/DeclarativeBuilder.hpp"

/**
 * Performance Example
 *
 * Demonstrates performance monitoring and optimization techniques.
 * Shows how to measure and improve UI performance.
 */

class PerformanceWidget : public QWidget {
    Q_OBJECT

public:
    PerformanceWidget(QWidget *parent = nullptr) : QWidget(parent) {
        setupUI();
        setupPerformanceMonitoring();
    }

private slots:
    void runPerformanceTest() {
        m_log->append("Starting performance test...");

        QElapsedTimer timer;
        timer.start();

        // Simulate heavy UI operations
        for (int i = 0; i < 1000; ++i) {
            m_progressBar->setValue(i / 10);
            m_counterLabel->setText(QString("Operations: %1").arg(i));

            // Force UI update
            QApplication::processEvents();

            if (i % 100 == 0) {
                qint64 elapsed = timer.elapsed();
                m_log->append(QString("Completed %1 operations in %2ms")
                                  .arg(i)
                                  .arg(elapsed));
            }
        }

        qint64 totalTime = timer.elapsed();
        m_log->append(
            QString("Performance test completed in %1ms").arg(totalTime));
        m_log->append(QString("Average time per operation: %1ms")
                          .arg(totalTime / 1000.0));
    }

    void updatePerformanceMetrics() {
        static auto lastUpdate = std::chrono::high_resolution_clock::now();
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - lastUpdate);

        m_fpsLabel->setText(
            QString("Update interval: %1ms").arg(duration.count()));
        lastUpdate = now;
    }

private:
    void setupUI() {
        setWindowTitle("Performance Monitoring Example");
        resize(600, 500);

        auto layout = new QVBoxLayout(this);

        // Title
        auto title = new QLabel("Performance Monitoring and Optimization");
        title->setStyleSheet(
            "font-size: 16px; font-weight: bold; margin: 10px;");
        layout->addWidget(title);

        // Controls
        auto controlsLayout = new QHBoxLayout();
        auto testButton = new QPushButton("Run Performance Test");
        connect(testButton, &QPushButton::clicked, this,
                &PerformanceWidget::runPerformanceTest);
        controlsLayout->addWidget(testButton);

        auto clearButton = new QPushButton("Clear Log");
        connect(clearButton, &QPushButton::clicked,
                [this]() { m_log->clear(); });
        controlsLayout->addWidget(clearButton);

        layout->addLayout(controlsLayout);

        // Metrics
        auto metricsLayout = new QHBoxLayout();
        m_fpsLabel = new QLabel("Update interval: 0ms");
        m_counterLabel = new QLabel("Operations: 0");
        metricsLayout->addWidget(m_fpsLabel);
        metricsLayout->addWidget(m_counterLabel);
        layout->addLayout(metricsLayout);

        // Progress bar
        m_progressBar = new QProgressBar();
        m_progressBar->setRange(0, 100);
        layout->addWidget(m_progressBar);

        // Log
        m_log = new QTextEdit();
        m_log->setMaximumHeight(200);
        layout->addWidget(m_log);

        // Tips
        auto tips = new QLabel(
            "Performance Tips:\n"
            "• Use QTimer for regular updates instead of tight loops\n"
            "• Batch UI updates when possible\n"
            "• Profile your application to identify bottlenecks\n"
            "• Consider using QGraphicsView for complex scenes");
        tips->setStyleSheet(
            "background-color: #f0f0f0; padding: 10px; border-radius: 5px;");
        layout->addWidget(tips);
    }

    void setupPerformanceMonitoring() {
        // Update performance metrics every 100ms
        auto timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this,
                &PerformanceWidget::updatePerformanceMetrics);
        timer->start(100);
    }

private:
    QLabel *m_fpsLabel;
    QLabel *m_counterLabel;
    QProgressBar *m_progressBar;
    QTextEdit *m_log;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    PerformanceWidget widget;
    widget.show();

    return app.exec();
}

#include "performance.moc"
