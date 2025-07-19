// Examples/settings_example.cpp
#include <QApplication>
#include <QCheckBox>
#include <QColorDialog>
#include <QComboBox>
#include <QFileInfo>
#include <QFontDialog>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>
#include <QSlider>
#include <QSpinBox>
#include <QStandardPaths>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QWidget>
#include <memory>


#include "../../Binding/StateManager.hpp"
#include "../../HotReload/HotReloadManager.hpp"
#include "../../JSON/ComponentRegistry.hpp"
#include "../../JSON/JSONUILoader.hpp"


using namespace DeclarativeUI;

class SettingsApp : public QObject {
    Q_OBJECT

public:
    SettingsApp(QObject* parent = nullptr) : QObject(parent) {
        setupApplication();
        loadSettings();
    }

    void run() {
        try {
            createUI();
            setupHotReload();

            if (main_widget_) {
                main_widget_->show();
            }

        } catch (const std::exception& e) {
            QMessageBox::critical(nullptr, "Settings Error", e.what());
        }
    }

private slots:
    void onGeneralSettingsChanged() { updateGeneralSettings(); }

    void onAppearanceSettingsChanged() { updateAppearanceSettings(); }

    void onAdvancedSettingsChanged() { updateAdvancedSettings(); }

    void onLanguageChanged(int index) {
        QStringList languages = {"English",  "中文",    "Español",
                                 "Français", "Deutsch", "日本語"};
        if (index >= 0 && index < languages.size()) {
            language_state_->set(languages[index]);
        }
    }

    void onThemeChanged(int index) {
        QStringList themes = {"Light", "Dark", "Auto"};
        if (index >= 0 && index < themes.size()) {
            theme_state_->set(themes[index]);
        }
    }

    void onFontSizeChanged(int size) { font_size_state_->set(size); }

    void onNotificationsToggled(bool enabled) {
        notifications_state_->set(enabled);
    }

    void onAutoSaveToggled(bool enabled) { auto_save_state_->set(enabled); }

    void onAutoBackupToggled(bool enabled) { auto_backup_state_->set(enabled); }

    void onCacheIntervalChanged(int interval) {
        cache_interval_state_->set(interval);
    }

    void onLogLevelChanged(int index) {
        QStringList levels = {"Debug", "Info", "Warning", "Error"};
        if (index >= 0 && index < levels.size()) {
            log_level_state_->set(levels[index]);
        }
    }

    void onApplySettings() {
        saveSettings();
        updateStatus("✅ Settings applied successfully!");

        // **Show confirmation**
        QMessageBox::information(
            nullptr, "Settings Applied",
            "Settings have been saved and applied successfully!");
    }

    void onResetToDefaults() {
        auto reply = QMessageBox::question(
            nullptr, "Reset Settings",
            "Are you sure you want to reset all settings to defaults?",
            QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            resetToDefaults();
            updateStatus("🔄 Settings reset to defaults");
        }
    }

    void onExportSettings() {
        QSettings settings;
        QString config_path =
            QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);

        QMessageBox::information(
            nullptr, "Settings Exported",
            QString("Settings exported to:\n%1").arg(config_path));

        updateStatus("📤 Settings exported");
    }

    void onImportSettings() {
        QMessageBox::information(
            nullptr, "Import Settings",
            "Import settings functionality would be implemented here.");
        updateStatus("📥 Settings import requested");
    }

    void onHotReloadCompleted(const QString& file_path) {
        qDebug() << "🔥 Hot reload completed for:" << file_path;
        updateStatus(
            QString("✅ Reloaded: %1").arg(QFileInfo(file_path).fileName()));
    }

private:
    std::unique_ptr<QWidget> main_widget_;
    std::unique_ptr<JSON::JSONUILoader> ui_loader_;
    std::unique_ptr<HotReload::HotReloadManager> hot_reload_manager_;
    std::shared_ptr<Binding::StateManager> state_manager_;
    std::unique_ptr<QSettings> settings_;

    // **Settings states**
    std::shared_ptr<Binding::ReactiveProperty<QString>> language_state_;
    std::shared_ptr<Binding::ReactiveProperty<QString>> theme_state_;
    std::shared_ptr<Binding::ReactiveProperty<int>> font_size_state_;
    std::shared_ptr<Binding::ReactiveProperty<bool>> notifications_state_;
    std::shared_ptr<Binding::ReactiveProperty<bool>> auto_save_state_;
    std::shared_ptr<Binding::ReactiveProperty<bool>> auto_backup_state_;
    std::shared_ptr<Binding::ReactiveProperty<int>> cache_interval_state_;
    std::shared_ptr<Binding::ReactiveProperty<QString>> log_level_state_;
    std::shared_ptr<Binding::ReactiveProperty<QString>> status_state_;

    // **Computed states**
    std::shared_ptr<Binding::ReactiveProperty<QString>> general_summary_state_;
    std::shared_ptr<Binding::ReactiveProperty<QString>>
        appearance_summary_state_;
    std::shared_ptr<Binding::ReactiveProperty<QString>> advanced_summary_state_;

    void setupApplication() {
        // **Initialize settings**
        settings_ = std::make_unique<QSettings>("HotReloadDemo", "SettingsApp");

        // **Initialize state manager**
        state_manager_ = std::shared_ptr<Binding::StateManager>(
            &Binding::StateManager::instance(), [](Binding::StateManager*) {});

        // **Create reactive states**
        language_state_ =
            state_manager_->createState<QString>("language", "English");
        theme_state_ = state_manager_->createState<QString>("theme", "Light");
        font_size_state_ = state_manager_->createState<int>("font_size", 12);
        notifications_state_ =
            state_manager_->createState<bool>("notifications", true);
        auto_save_state_ = state_manager_->createState<bool>("auto_save", true);
        auto_backup_state_ =
            state_manager_->createState<bool>("auto_backup", false);
        cache_interval_state_ =
            state_manager_->createState<int>("cache_interval", 60);
        log_level_state_ =
            state_manager_->createState<QString>("log_level", "Info");
        status_state_ =
            state_manager_->createState<QString>("status", "⚙️ Settings ready");

        // **Create computed states**
        general_summary_state_ = state_manager_->createComputed<QString>(
            "general_summary", [this]() {
                return QString("Language: %1, Notifications: %2")
                    .arg(language_state_->get())
                    .arg(notifications_state_->get() ? "On" : "Off");
            });

        appearance_summary_state_ = state_manager_->createComputed<QString>(
            "appearance_summary", [this]() {
                return QString("Theme: %1, Font Size: %2px")
                    .arg(theme_state_->get())
                    .arg(font_size_state_->get());
            });

        advanced_summary_state_ = state_manager_->createComputed<QString>(
            "advanced_summary", [this]() {
                return QString("Cache: %1min, Log Level: %2")
                    .arg(cache_interval_state_->get())
                    .arg(log_level_state_->get());
            });

        // **Setup UI loader**
        ui_loader_ = std::make_unique<JSON::JSONUILoader>();
        ui_loader_->bindStateManager(state_manager_);

        // **Register event handlers**
        registerEventHandlers();

        // **Register components**
        registerComponents();
    }

    void registerEventHandlers() {
        ui_loader_->registerEventHandler("onLanguageChanged",
                                         [this]() { onLanguageChanged(0); });
        ui_loader_->registerEventHandler("onThemeChanged",
                                         [this]() { onThemeChanged(0); });
        ui_loader_->registerEventHandler("onFontSizeChanged",
                                         [this]() { onFontSizeChanged(12); });
        ui_loader_->registerEventHandler("onNotificationsToggled", [this]() {
            onNotificationsToggled(true);
        });
        ui_loader_->registerEventHandler("onAutoSaveToggled",
                                         [this]() { onAutoSaveToggled(true); });
        ui_loader_->registerEventHandler(
            "onAutoBackupToggled", [this]() { onAutoBackupToggled(true); });
        ui_loader_->registerEventHandler(
            "onCacheIntervalChanged", [this]() { onCacheIntervalChanged(60); });
        ui_loader_->registerEventHandler("onLogLevelChanged",
                                         [this]() { onLogLevelChanged(0); });
        ui_loader_->registerEventHandler("onApplySettings",
                                         [this]() { onApplySettings(); });
        ui_loader_->registerEventHandler("onResetToDefaults",
                                         [this]() { onResetToDefaults(); });
        ui_loader_->registerEventHandler("onExportSettings",
                                         [this]() { onExportSettings(); });
        ui_loader_->registerEventHandler("onImportSettings",
                                         [this]() { onImportSettings(); });
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

        registry.registerComponent<QComboBox>(
            "QComboBox", [](const QJsonObject& config) {
                auto comboBox = std::make_unique<QComboBox>();
                if (config.contains("properties")) {
                    QJsonObject props = config["properties"].toObject();
                    if (props.contains("items")) {
                        QJsonArray items = props["items"].toArray();
                        for (const auto& item : items) {
                            comboBox->addItem(item.toString());
                        }
                    }
                }
                return comboBox;
            });

        registry.registerComponent<QCheckBox>(
            "QCheckBox", [](const QJsonObject& config) {
                auto checkBox = std::make_unique<QCheckBox>();
                if (config.contains("properties")) {
                    QJsonObject props = config["properties"].toObject();
                    if (props.contains("text")) {
                        checkBox->setText(props["text"].toString());
                    }
                }
                return checkBox;
            });

        registry.registerComponent<QSlider>(
            "QSlider", [](const QJsonObject& config) {
                auto slider = std::make_unique<QSlider>();
                if (config.contains("properties")) {
                    QJsonObject props = config["properties"].toObject();
                    if (props.contains("orientation")) {
                        slider->setOrientation(static_cast<Qt::Orientation>(
                            props["orientation"].toInt()));
                    }
                    if (props.contains("minimum")) {
                        slider->setMinimum(props["minimum"].toInt());
                    }
                    if (props.contains("maximum")) {
                        slider->setMaximum(props["maximum"].toInt());
                    }
                }
                return slider;
            });
    }

    void createUI() {
        try {
            QString ui_file_path =
                "Resources/ui_definitions/settings_dialog.json";
            main_widget_ = ui_loader_->loadFromFile(ui_file_path);

            if (!main_widget_) {
                throw std::runtime_error(
                    "Failed to create settings widget from JSON");
            }

            qDebug() << "✅ Settings UI loaded successfully";

        } catch (const std::exception& e) {
            qCritical() << "Settings UI Creation failed:" << e.what();
            main_widget_ = createFallbackUI();
        }
    }

    std::unique_ptr<QWidget> createFallbackUI() {
        auto widget = std::make_unique<QWidget>();
        widget->setWindowTitle("Settings - Fallback UI");
        widget->setMinimumSize(600, 400);

        auto layout = new QVBoxLayout(widget.get());

        auto title = new QLabel("⚙️ Settings");
        title->setAlignment(Qt::AlignCenter);
        title->setStyleSheet(
            "font-size: 24px; font-weight: bold; margin-bottom: 20px;");
        layout->addWidget(title);

        auto apply_btn = new QPushButton("Apply Settings");
        connect(apply_btn, &QPushButton::clicked, this,
                &SettingsApp::onApplySettings);
        layout->addWidget(apply_btn);

        return widget;
    }

    void setupHotReload() {
        hot_reload_manager_ = std::make_unique<HotReload::HotReloadManager>();
        hot_reload_manager_->registerUIFile(
            "Resources/ui_definitions/settings_dialog.json",
            main_widget_.get());

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
                            << "Settings hot reload failed:" << e.what();
                    }
                });
    }

    void loadSettings() {
        language_state_->set(
            settings_->value("language", "English").toString());
        theme_state_->set(settings_->value("theme", "Light").toString());
        font_size_state_->set(settings_->value("font_size", 12).toInt());
        notifications_state_->set(
            settings_->value("notifications", true).toBool());
        auto_save_state_->set(settings_->value("auto_save", true).toBool());
        auto_backup_state_->set(
            settings_->value("auto_backup", false).toBool());
        cache_interval_state_->set(
            settings_->value("cache_interval", 60).toInt());
        log_level_state_->set(settings_->value("log_level", "Info").toString());
    }

    void saveSettings() {
        settings_->setValue("language", language_state_->get());
        settings_->setValue("theme", theme_state_->get());
        settings_->setValue("font_size", font_size_state_->get());
        settings_->setValue("notifications", notifications_state_->get());
        settings_->setValue("auto_save", auto_save_state_->get());
        settings_->setValue("auto_backup", auto_backup_state_->get());
        settings_->setValue("cache_interval", cache_interval_state_->get());
        settings_->setValue("log_level", log_level_state_->get());
        settings_->sync();
    }

    void resetToDefaults() {
        language_state_->set("English");
        theme_state_->set("Light");
        font_size_state_->set(12);
        notifications_state_->set(true);
        auto_save_state_->set(true);
        auto_backup_state_->set(false);
        cache_interval_state_->set(60);
        log_level_state_->set("Info");

        // **Update computed states**
        general_summary_state_->update();
        appearance_summary_state_->update();
        advanced_summary_state_->update();
    }

    void updateGeneralSettings() { general_summary_state_->update(); }

    void updateAppearanceSettings() { appearance_summary_state_->update(); }

    void updateAdvancedSettings() { advanced_summary_state_->update(); }

    void updateStatus(const QString& message) {
        status_state_->set(message);

        // **Clear status after 3 seconds**
        QTimer::singleShot(
            3000, [this]() { status_state_->set("⚙️ Settings ready"); });
    }
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    try {
        SettingsApp settings_app;
        settings_app.run();

        return app.exec();

    } catch (const std::exception& e) {
        qCritical() << "Settings application error:" << e.what();
        return -1;
    }
}

#include "38_settings_example.moc"
