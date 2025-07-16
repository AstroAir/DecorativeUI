// Examples/dashboard_example.cpp
#include <QApplication>
#include <QFileInfo>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QRandomGenerator>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <memory>


#include "../Binding/StateManager.hpp"
#include "../HotReload/HotReloadManager.hpp"
#include "../JSON/ComponentRegistry.hpp"
#include "../JSON/JSONUILoader.hpp"


using namespace DeclarativeUI;

class DashboardApp : public QObject {
    Q_OBJECT

public:
    DashboardApp(QObject* parent = nullptr) : QObject(parent) {
        setupApplication();
    }

    void run() {
        try {
            createUI();
            setupHotReload();
            startDataUpdates();

            if (main_widget_) {
                main_widget_->show();
            }

        } catch (const std::exception& e) {
            QMessageBox::critical(nullptr, "Dashboard Error", e.what());
        }
    }

private slots:
    void updateMetrics() {
        // **Generate random metrics**
        auto random = QRandomGenerator::global();

        // CPU usage (0-100%)
        int cpu_usage = random->bounded(10, 90);
        cpu_usage_state_->set(cpu_usage);

        // Memory usage (0-100%)
        int memory_usage = random->bounded(30, 85);
        memory_usage_state_->set(memory_usage);

        // Network speed (0-1000 Mbps)
        int network_speed = random->bounded(50, 1000);
        network_speed_state_->set(network_speed);

        // Disk usage (0-100%)
        int disk_usage = random->bounded(20, 95);
        disk_usage_state_->set(disk_usage);

        // Active users (0-5000)
        int active_users = random->bounded(100, 5000);
        active_users_state_->set(active_users);

        // Server uptime (increment)
        int current_uptime = uptime_state_->get();
        uptime_state_->set(current_uptime + 1);

        // **Update status based on metrics**
        updateSystemStatus();

        // **Update charts**
        updateChartData();
    }

    void onRefreshClicked() {
        refresh_count_state_->set(refresh_count_state_->get() + 1);
        updateMetrics();

        status_state_->set("🔄 Dashboard refreshed");

        // **Clear status after 2 seconds**
        QTimer::singleShot(
            2000, [this]() { status_state_->set("📊 Dashboard Active"); });
    }

    void onExportClicked() {
        // **Create export data**
        QJsonObject export_data;
        export_data["timestamp"] = QDateTime::currentDateTime().toString();
        export_data["cpu_usage"] = cpu_usage_state_->get();
        export_data["memory_usage"] = memory_usage_state_->get();
        export_data["network_speed"] = network_speed_state_->get();
        export_data["disk_usage"] = disk_usage_state_->get();
        export_data["active_users"] = active_users_state_->get();
        export_data["uptime"] = uptime_state_->get();

        QMessageBox::information(
            nullptr, "Data Exported",
            QString("Dashboard data exported:\n%1")
                .arg(QString::fromUtf8(QJsonDocument(export_data).toJson())));
    }

    void onHotReloadCompleted(const QString& file_path) {
        qDebug() << "🔥 Hot reload completed for:" << file_path;
        status_state_->set(
            QString("✅ Reloaded: %1").arg(QFileInfo(file_path).fileName()));

        QTimer::singleShot(
            3000, [this]() { status_state_->set("📊 Dashboard Active"); });
    }

private:
    std::unique_ptr<QWidget> main_widget_;
    std::unique_ptr<JSON::JSONUILoader> ui_loader_;
    std::unique_ptr<HotReload::HotReloadManager> hot_reload_manager_;
    std::shared_ptr<Binding::StateManager> state_manager_;
    std::unique_ptr<QTimer> update_timer_;

    // **Dashboard metrics states**
    std::shared_ptr<Binding::ReactiveProperty<int>> cpu_usage_state_;
    std::shared_ptr<Binding::ReactiveProperty<int>> memory_usage_state_;
    std::shared_ptr<Binding::ReactiveProperty<int>> network_speed_state_;
    std::shared_ptr<Binding::ReactiveProperty<int>> disk_usage_state_;
    std::shared_ptr<Binding::ReactiveProperty<int>> active_users_state_;
    std::shared_ptr<Binding::ReactiveProperty<int>> uptime_state_;
    std::shared_ptr<Binding::ReactiveProperty<QString>> system_status_state_;
    std::shared_ptr<Binding::ReactiveProperty<int>> refresh_count_state_;
    std::shared_ptr<Binding::ReactiveProperty<QString>> status_state_;

    // **Computed states**
    std::shared_ptr<Binding::ReactiveProperty<QString>> cpu_text_state_;
    std::shared_ptr<Binding::ReactiveProperty<QString>> memory_text_state_;
    std::shared_ptr<Binding::ReactiveProperty<QString>> network_text_state_;
    std::shared_ptr<Binding::ReactiveProperty<QString>> disk_text_state_;
    std::shared_ptr<Binding::ReactiveProperty<QString>> users_text_state_;
    std::shared_ptr<Binding::ReactiveProperty<QString>> uptime_text_state_;

    void setupApplication() {
        // **Initialize state manager**
        state_manager_ = std::shared_ptr<Binding::StateManager>(
            &Binding::StateManager::instance(), [](Binding::StateManager*) {});

        // **Create reactive states**
        cpu_usage_state_ = state_manager_->createState<int>("cpu_usage", 0);
        memory_usage_state_ =
            state_manager_->createState<int>("memory_usage", 0);
        network_speed_state_ =
            state_manager_->createState<int>("network_speed", 0);
        disk_usage_state_ = state_manager_->createState<int>("disk_usage", 0);
        active_users_state_ =
            state_manager_->createState<int>("active_users", 0);
        uptime_state_ = state_manager_->createState<int>("uptime", 0);
        system_status_state_ =
            state_manager_->createState<QString>("system_status", "🟢 Healthy");
        refresh_count_state_ =
            state_manager_->createState<int>("refresh_count", 0);
        status_state_ = state_manager_->createState<QString>(
            "status", "📊 Dashboard Active");

        // **Create computed states**
        cpu_text_state_ =
            state_manager_->createComputed<QString>("cpu_text", [this]() {
                int usage = cpu_usage_state_->get();
                QString emoji = usage > 80 ? "🔴" : usage > 60 ? "🟡" : "🟢";
                return QString("%1 CPU: %2%").arg(emoji).arg(usage);
            });

        memory_text_state_ =
            state_manager_->createComputed<QString>("memory_text", [this]() {
                int usage = memory_usage_state_->get();
                QString emoji = usage > 80 ? "🔴" : usage > 60 ? "🟡" : "🟢";
                return QString("%1 Memory: %2%").arg(emoji).arg(usage);
            });

        network_text_state_ =
            state_manager_->createComputed<QString>("network_text", [this]() {
                int speed = network_speed_state_->get();
                return QString("🌐 Network: %1 Mbps").arg(speed);
            });

        disk_text_state_ =
            state_manager_->createComputed<QString>("disk_text", [this]() {
                int usage = disk_usage_state_->get();
                QString emoji = usage > 90 ? "🔴" : usage > 70 ? "🟡" : "🟢";
                return QString("%1 Disk: %2%").arg(emoji).arg(usage);
            });

        users_text_state_ =
            state_manager_->createComputed<QString>("users_text", [this]() {
                return QString("👥 Users: %1").arg(active_users_state_->get());
            });

        uptime_text_state_ =
            state_manager_->createComputed<QString>("uptime_text", [this]() {
                int seconds = uptime_state_->get();
                int hours = seconds / 3600;
                int minutes = (seconds % 3600) / 60;
                int secs = seconds % 60;
                return QString("⏱️ Uptime: %1:%2:%3")
                    .arg(hours, 2, 10, QChar('0'))
                    .arg(minutes, 2, 10, QChar('0'))
                    .arg(secs, 2, 10, QChar('0'));
            });

        // **Setup UI loader**
        ui_loader_ = std::make_unique<JSON::JSONUILoader>();
        ui_loader_->bindStateManager(state_manager_);

        // **Register event handlers**
        ui_loader_->registerEventHandler("onRefreshClicked",
                                         [this]() { onRefreshClicked(); });
        ui_loader_->registerEventHandler("onExportClicked",
                                         [this]() { onExportClicked(); });

        // **Register components**
        registerComponents();
    }

    void registerComponents() {
        auto& registry = JSON::ComponentRegistry::instance();

        // **Register standard components**
        registry.registerComponent<QWidget>("QWidget", [](const QJsonObject&) {
            return std::make_unique<QWidget>();
        });

        registry.registerComponent<QLabel>(
            "QLabel", [](const QJsonObject& config) {
                auto label = std::make_unique<QLabel>();
                if (config.contains("properties")) {
                    QJsonObject props = config["properties"].toObject();
                    if (props.contains("text")) {
                        label->setText(props["text"].toString());
                    }
                }
                return label;
            });

        registry.registerComponent<QPushButton>(
            "QPushButton", [](const QJsonObject& config) {
                auto button = std::make_unique<QPushButton>();
                if (config.contains("properties")) {
                    QJsonObject props = config["properties"].toObject();
                    if (props.contains("text")) {
                        button->setText(props["text"].toString());
                    }
                }
                return button;
            });

        registry.registerComponent<QProgressBar>(
            "QProgressBar", [](const QJsonObject& config) {
                auto progressBar = std::make_unique<QProgressBar>();
                if (config.contains("properties")) {
                    QJsonObject props = config["properties"].toObject();
                    if (props.contains("minimum")) {
                        progressBar->setMinimum(props["minimum"].toInt());
                    }
                    if (props.contains("maximum")) {
                        progressBar->setMaximum(props["maximum"].toInt());
                    }
                    if (props.contains("value")) {
                        progressBar->setValue(props["value"].toInt());
                    }
                }
                return progressBar;
            });
    }

    void createUI() {
        try {
            QString ui_file_path = "Resources/ui_definitions/dashboard.json";
            main_widget_ = ui_loader_->loadFromFile(ui_file_path);

            if (!main_widget_) {
                throw std::runtime_error(
                    "Failed to create dashboard widget from JSON");
            }

            qDebug() << "✅ Dashboard UI loaded successfully";

        } catch (const std::exception& e) {
            qCritical() << "Dashboard UI Creation failed:" << e.what();
            main_widget_ = createFallbackUI();
        }
    }

    std::unique_ptr<QWidget> createFallbackUI() {
        auto widget = std::make_unique<QWidget>();
        widget->setWindowTitle("Dashboard - Fallback UI");
        widget->setMinimumSize(800, 600);

        auto layout = new QVBoxLayout(widget.get());

        auto title = new QLabel("📊 System Dashboard");
        title->setAlignment(Qt::AlignCenter);
        title->setStyleSheet(
            "font-size: 24px; font-weight: bold; margin-bottom: 20px;");
        layout->addWidget(title);

        auto cpu_label = new QLabel("CPU Usage: 0%");
        layout->addWidget(cpu_label);

        auto refresh_btn = new QPushButton("🔄 Refresh");
        connect(refresh_btn, &QPushButton::clicked, this,
                &DashboardApp::onRefreshClicked);
        layout->addWidget(refresh_btn);

        return widget;
    }

    void setupHotReload() {
        hot_reload_manager_ = std::make_unique<HotReload::HotReloadManager>();
        hot_reload_manager_->registerUIFile(
            "Resources/ui_definitions/dashboard.json", main_widget_.get());

        connect(hot_reload_manager_.get(),
                &HotReload::HotReloadManager::reloadCompleted, this,
                [this](const QString& file_path) {
                    try {
                        auto new_widget = ui_loader_->loadFromFile(file_path);
                        if (new_widget) {
                            main_widget_ = std::move(new_widget);
                            main_widget_->show();
                            onHotReloadCompleted(file_path);
                        }
                    } catch (const std::exception& e) {
                        qCritical()
                            << "Dashboard hot reload failed:" << e.what();
                    }
                });
    }

    void startDataUpdates() {
        update_timer_ = std::make_unique<QTimer>();
        connect(update_timer_.get(), &QTimer::timeout, this,
                &DashboardApp::updateMetrics);
        update_timer_->start(2000);  // Update every 2 seconds

        // **Initial update**
        updateMetrics();
    }

    void updateSystemStatus() {
        int cpu = cpu_usage_state_->get();
        int memory = memory_usage_state_->get();
        int disk = disk_usage_state_->get();

        QString status;
        if (cpu > 80 || memory > 80 || disk > 90) {
            status = "🔴 Warning";
        } else if (cpu > 60 || memory > 60 || disk > 70) {
            status = "🟡 Caution";
        } else {
            status = "🟢 Healthy";
        }

        system_status_state_->set(status);
    }

    void updateChartData() {
        // **This would update chart data if we had charts**
        // For now, just trigger computed state updates
        cpu_text_state_->update();
        memory_text_state_->update();
        network_text_state_->update();
        disk_text_state_->update();
        users_text_state_->update();
        uptime_text_state_->update();
    }
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    try {
        DashboardApp dashboard;
        dashboard.run();

        return app.exec();

    } catch (const std::exception& e) {
        qCritical() << "Dashboard application error:" << e.what();
        return -1;
    }
}

#include "dashboard_example.moc"
