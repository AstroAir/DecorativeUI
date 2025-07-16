// Examples/hot_reload_example.cpp
#include <QApplication>
#include <QFileInfo>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <memory>

#include "../Binding/StateManager.hpp"
#include "../HotReload/HotReloadManager.hpp"
#include "../JSON/ComponentRegistry.hpp"
#include "../JSON/JSONUILoader.hpp"

using namespace DeclarativeUI;

class HotReloadCounterApp : public QObject {
    Q_OBJECT

public:
    HotReloadCounterApp(QObject *parent = nullptr) : QObject(parent) {
        setupApplication();
    }

    void run() {
        try {
            // **Load initial UI**
            loadUI();

            // **Setup hot reloading**
            setupHotReload();

            // **Show main window**
            if (main_widget_) {
                main_widget_->show();
            }

        } catch (const std::exception &e) {
            showError("Application Startup Error", e.what());
        }
    }

private slots:
    void incrementCounter() {
        int current = counter_state_->get();
        counter_state_->set(current + 1);
        counter_text_state_->update();
    }

    void decrementCounter() {
        int current = counter_state_->get();
        counter_state_->set(current - 1);
        counter_text_state_->update();
    }

    void resetCounter() {
        counter_state_->set(0);
        counter_text_state_->update();
    }

    void onHotReloadCompleted(const QString &file_path) {
        qDebug() << "🔥 Hot reload completed for:" << file_path;

        // **Update status**
        if (auto status_state =
                state_manager_->getState<QString>("status_text")) {
            status_state->set(QString("✅ Reloaded: %1")
                                  .arg(QFileInfo(file_path).fileName()));

            // **Clear status after 3 seconds**
            QTimer::singleShot(3000, [status_state]() {
                status_state->set("🚀 Hot Reload Active - Edit JSON files!");
            });
        }
    }

    void onHotReloadFailed(const QString &file_path, const QString &error) {
        qWarning() << "❌ Hot reload failed for" << file_path << ":" << error;
        showError("Hot Reload Error",
                  QString("Failed to reload %1:\n%2").arg(file_path, error));
    }

private:
    std::unique_ptr<QWidget> main_widget_;
    std::unique_ptr<JSON::JSONUILoader> ui_loader_;
    std::unique_ptr<HotReload::HotReloadManager> hot_reload_manager_;
    std::shared_ptr<Binding::StateManager> state_manager_;

    // **Application state**
    std::shared_ptr<Binding::ReactiveProperty<int>> counter_state_;
    std::shared_ptr<Binding::ReactiveProperty<QString>> counter_text_state_;
    std::shared_ptr<Binding::ReactiveProperty<QString>> status_text_state_;

    void setupApplication() {
        // **Initialize state manager**
        state_manager_ = std::shared_ptr<Binding::StateManager>(
            &Binding::StateManager::instance(), [](Binding::StateManager *) {});

        // **Create reactive states**
        counter_state_ = state_manager_->createState<int>("counter", 0);

        counter_text_state_ =
            state_manager_->createComputed<QString>("counter_text", [this]() {
                int count = counter_state_->get();
                QString emoji = count > 0 ? "📈" : count < 0 ? "📉" : "🎯";
                return QString("%1 Count: %2").arg(emoji).arg(count);
            });

        status_text_state_ = state_manager_->createState<QString>(
            "status_text", "🚀 Hot Reload Active - Edit JSON files!");

        // **Setup UI loader**
        ui_loader_ = std::make_unique<JSON::JSONUILoader>();
        ui_loader_->bindStateManager(state_manager_);

        // **Register event handlers**
        ui_loader_->registerEventHandler("incrementCounter",
                                         [this]() { incrementCounter(); });
        ui_loader_->registerEventHandler("decrementCounter",
                                         [this]() { decrementCounter(); });
        ui_loader_->registerEventHandler("resetCounter",
                                         [this]() { resetCounter(); });

        // **Setup hot reload manager**
        hot_reload_manager_ = std::make_unique<HotReload::HotReloadManager>();
        hot_reload_manager_->setFileFilters({"*.json"});
        hot_reload_manager_->setReloadDelay(200);

        // **Connect hot reload signals**
        connect(hot_reload_manager_.get(),
                &HotReload::HotReloadManager::reloadCompleted, this,
                &HotReloadCounterApp::onHotReloadCompleted);

        connect(hot_reload_manager_.get(),
                &HotReload::HotReloadManager::reloadFailed, this,
                &HotReloadCounterApp::onHotReloadFailed);

        // **Register built-in components**
        registerComponents();
    }

    void registerComponents() {
        auto &registry = JSON::ComponentRegistry::instance();

        // **Register standard Qt widgets**
        registry.registerComponent<QWidget>("QWidget", [](const QJsonObject &) {
            return std::make_unique<QWidget>();
        });

        registry.registerComponent<QLabel>(
            "QLabel", [](const QJsonObject &config) {
                auto label = std::make_unique<QLabel>();

                // **Apply initial properties from config**
                if (config.contains("properties")) {
                    QJsonObject props = config["properties"].toObject();
                    if (props.contains("text")) {
                        label->setText(props["text"].toString());
                    }
                }

                return label;
            });

        registry.registerComponent<QPushButton>(
            "QPushButton", [](const QJsonObject &config) {
                auto button = std::make_unique<QPushButton>();

                if (config.contains("properties")) {
                    QJsonObject props = config["properties"].toObject();
                    if (props.contains("text")) {
                        button->setText(props["text"].toString());
                    }
                }

                return button;
            });
    }

    void loadUI() {
        QString ui_file_path = "Resources/ui_definitions/counter_app.json";

        try {
            main_widget_ = ui_loader_->loadFromFile(ui_file_path);

            if (!main_widget_) {
                throw std::runtime_error("Failed to create main widget");
            }

            qDebug() << "✅ UI loaded successfully from" << ui_file_path;

        } catch (const std::exception &e) {
            throw std::runtime_error(std::string("UI loading failed: ") +
                                     e.what());
        }
    }

    void setupHotReload() {
        try {
            QString ui_directory = "Resources/ui_definitions/";

            // **Register UI directory for hot reloading**
            hot_reload_manager_->registerUIDirectory(ui_directory, true);

            // **Register specific UI file**
            QString main_ui_file = ui_directory + "counter_app.json";
            hot_reload_manager_->registerUIFile(main_ui_file,
                                                main_widget_.get());

            qDebug() << "🔥 Hot reload enabled for:" << ui_directory;

        } catch (const std::exception &e) {
            qWarning() << "❌ Hot reload setup failed:" << e.what();
            showError("Hot Reload Setup Error", e.what());
        }
    }

    void showError(const QString &title, const QString &message) {
        QMessageBox::critical(nullptr, title, message);
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // **Set application properties**
    app.setApplicationName("DeclarativeUI Hot Reload Demo");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("DeclarativeUI");

    try {
        // **Create and run application**
        HotReloadCounterApp counter_app;
        counter_app.run();

        qDebug() << "🚀 Application started successfully";
        qDebug()
            << "💡 Try editing 'Resources/ui_definitions/counter_app.json' to "
               "see hot reload in action!";

        return app.exec();

    } catch (const std::exception &e) {
        qCritical() << "💥 Application error:" << e.what();
        QMessageBox::critical(nullptr, "Application Error",
                              QString("Fatal error: %1").arg(e.what()));
        return -1;
    } catch (...) {
        qCritical() << "💥 Unknown application error";
        QMessageBox::critical(nullptr, "Application Error",
                              "An unknown fatal error occurred");
        return -1;
    }
}

#include "hot_reload_example.moc"
