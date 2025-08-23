/**
 * @file form-example.cpp
 * @brief Comprehensive form application demonstrating DeclarativeUI
 * capabilities
 *
 * This example showcases a complete form application with:
 * - Component registration with property configuration
 * - State management and data binding
 * - Event handling and form validation
 * - Hot reload functionality for development
 * - Fallback UI creation for error handling
 *
 * The implementation emphasizes maintainable code with low cyclomatic
 * complexity by breaking down the large registerComponents function into
 * focused helper functions.
 *
 * @author DeclarativeUI Team
 * @version 1.0
 */

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QSlider>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <memory>

#include "Binding/StateManager.hpp"
#include "HotReload/HotReloadManager.hpp"
#include "JSON/ComponentRegistry.hpp"
#include "JSON/JSONUILoader.hpp"

using namespace DeclarativeUI;

class FormDemoApp : public QObject {
    Q_OBJECT

public:
    FormDemoApp(QObject* parent = nullptr) : QObject(parent) {
        setupApplication();
    }

    void run() {
        try {
            // **Create UI**
            createUI();

            // **Setup hot reloading**
            setupHotReload();

            // **Show main window**
            if (main_widget_) {
                main_widget_->show();
            }

        } catch (const std::exception& e) {
            QMessageBox::critical(nullptr, "Application Error", e.what());
        }
    }

private slots:
    void onNameChanged(const QString& name) {
        name_state_->set(name);
        updateFormValidation();
    }

    void onEmailChanged(const QString& email) {
        email_state_->set(email);
        updateFormValidation();
    }

    void onAgeChanged(int age) {
        age_state_->set(age);
        updateFormValidation();
    }

    void onCountryChanged(int index) {
        QStringList countries = {"United States", "Canada", "United Kingdom",
                                 "Germany",       "France", "Japan",
                                 "Australia"};
        if (index >= 0 && index < countries.size()) {
            country_state_->set(countries[index]);
        }
        updateFormValidation();
    }

    void onNewsletterToggled(bool checked) { newsletter_state_->set(checked); }

    void onProgressValueChanged(int value) {
        progress_value_state_->set(value);

        QString status;
        if (value < 25) {
            status = "🔴 Getting started...";
        } else if (value < 50) {
            status = "🟡 Making progress...";
        } else if (value < 75) {
            status = "🟠 Almost there...";
        } else if (value < 100) {
            status = "🟢 Nearly complete!";
        } else {
            status = "✅ Complete!";
        }
        progress_status_state_->set(status);
    }

    void onSubmitForm() {
        if (!form_valid_state_->get()) {
            QMessageBox::warning(nullptr, "Invalid Form",
                                 "Please fill in all required fields.");
            return;
        }

        // **Create form data**
        QJsonObject form_data;
        form_data["name"] = name_state_->get();
        form_data["email"] = email_state_->get();
        form_data["age"] = age_state_->get();
        form_data["country"] = country_state_->get();
        form_data["newsletter"] = newsletter_state_->get();
        form_data["progress"] = progress_value_state_->get();

        QString json_string = QJsonDocument(form_data).toJson();

        QMessageBox::information(nullptr, "Form Submitted",
                                 QString("Form data:\n%1").arg(json_string));

        // **Reset form**
        resetForm();
    }

    void onResetForm() { resetForm(); }

    void onHotReloadCompleted(const QString& file_path) {
        qDebug() << "🔥 Hot reload completed for:" << file_path;
        status_state_->set(
            QString("✅ Reloaded: %1").arg(QFileInfo(file_path).fileName()));

        // **Clear status after 3 seconds**
        QTimer::singleShot(
            3000, [this]() { status_state_->set("🚀 Hot Reload Active"); });
    }

private:
    std::unique_ptr<QWidget> main_widget_;
    std::unique_ptr<JSON::JSONUILoader> ui_loader_;
    std::unique_ptr<HotReload::HotReloadManager> hot_reload_manager_;
    std::shared_ptr<Binding::StateManager> state_manager_;

    // **Form state**
    std::shared_ptr<Binding::ReactiveProperty<QString>> name_state_;
    std::shared_ptr<Binding::ReactiveProperty<QString>> email_state_;
    std::shared_ptr<Binding::ReactiveProperty<int>> age_state_;
    std::shared_ptr<Binding::ReactiveProperty<QString>> country_state_;
    std::shared_ptr<Binding::ReactiveProperty<bool>> newsletter_state_;
    std::shared_ptr<Binding::ReactiveProperty<int>> progress_value_state_;
    std::shared_ptr<Binding::ReactiveProperty<QString>> progress_status_state_;
    std::shared_ptr<Binding::ReactiveProperty<bool>> form_valid_state_;
    std::shared_ptr<Binding::ReactiveProperty<QString>> status_state_;

    void setupApplication() {
        // **Initialize state manager**
        state_manager_ = std::shared_ptr<Binding::StateManager>(
            &Binding::StateManager::instance(), [](Binding::StateManager*) {});

        // **Create reactive states**
        name_state_ = state_manager_->createState<QString>("name", "");
        email_state_ = state_manager_->createState<QString>("email", "");
        age_state_ = state_manager_->createState<int>("age", 25);
        country_state_ =
            state_manager_->createState<QString>("country", "United States");
        newsletter_state_ =
            state_manager_->createState<bool>("newsletter", false);
        progress_value_state_ =
            state_manager_->createState<int>("progress_value", 0);
        progress_status_state_ = state_manager_->createState<QString>(
            "progress_status", "🔴 Getting started...");
        form_valid_state_ =
            state_manager_->createState<bool>("form_valid", false);
        status_state_ = state_manager_->createState<QString>(
            "status", "🚀 Hot Reload Active");

        // **Setup UI loader**
        ui_loader_ = std::make_unique<JSON::JSONUILoader>();
        ui_loader_->bindStateManager(state_manager_);

        // **Register event handlers**
        registerEventHandlers();

        // **Register components**
        registerComponents();
    }

    void registerEventHandlers() {
        ui_loader_->registerEventHandler("onNameChanged",
                                         [this]() { onNameChanged(""); });
        ui_loader_->registerEventHandler("onEmailChanged",
                                         [this]() { onEmailChanged(""); });
        ui_loader_->registerEventHandler("onAgeChanged",
                                         [this]() { onAgeChanged(0); });
        ui_loader_->registerEventHandler("onCountryChanged",
                                         [this]() { onCountryChanged(0); });
        ui_loader_->registerEventHandler(
            "onNewsletterToggled", [this]() { onNewsletterToggled(false); });
        ui_loader_->registerEventHandler(
            "onProgressValueChanged", [this]() { onProgressValueChanged(0); });
        ui_loader_->registerEventHandler("onSubmitForm",
                                         [this]() { onSubmitForm(); });
        ui_loader_->registerEventHandler("onResetForm",
                                         [this]() { onResetForm(); });
    }

    /**
     * @brief Registers all UI components with the component registry
     *
     * This function organizes component registration into logical groups
     * to improve maintainability and reduce complexity.
     */
    void registerComponents() {
        registerBasicComponents();
        registerInputComponents();
        registerDisplayComponents();
    }

private:
    /**
     * @brief Registers basic UI components (QWidget, QLabel, QPushButton)
     */
    void registerBasicComponents() {
        auto& registry = JSON::ComponentRegistry::instance();

        // **Register QWidget - base component**
        registry.registerComponent<QWidget>("QWidget", [](const QJsonObject&) {
            return std::make_unique<QWidget>();
        });

        // **Register QLabel with text property support**
        registry.registerComponent<QLabel>(
            "QLabel", [](const QJsonObject& config) {
                auto label = std::make_unique<QLabel>();
                applyTextProperty(label.get(), config);
                return label;
            });

        // **Register QPushButton with text property support**
        registry.registerComponent<QPushButton>(
            "QPushButton", [](const QJsonObject& config) {
                auto button = std::make_unique<QPushButton>();
                applyTextProperty(button.get(), config);
                return button;
            });
    }

    /**
     * @brief Registers input components (QLineEdit, QComboBox, QCheckBox)
     */
    void registerInputComponents() {
        auto& registry = JSON::ComponentRegistry::instance();

        // **Register QLineEdit with text and placeholder support**
        registry.registerComponent<QLineEdit>(
            "QLineEdit", [](const QJsonObject& config) {
                auto lineEdit = std::make_unique<QLineEdit>();
                applyLineEditProperties(lineEdit.get(), config);
                return lineEdit;
            });

        // **Register QComboBox with items support**
        registry.registerComponent<QComboBox>(
            "QComboBox", [](const QJsonObject& config) {
                auto comboBox = std::make_unique<QComboBox>();
                applyComboBoxProperties(comboBox.get(), config);
                return comboBox;
            });

        // **Register QCheckBox with text and checked state support**
        registry.registerComponent<QCheckBox>(
            "QCheckBox", [](const QJsonObject& config) {
                auto checkBox = std::make_unique<QCheckBox>();
                applyCheckBoxProperties(checkBox.get(), config);
                return checkBox;
            });
    }

    /**
     * @brief Registers display components (QSlider, QProgressBar)
     */
    void registerDisplayComponents() {
        auto& registry = JSON::ComponentRegistry::instance();

        // **Register QSlider with range and orientation support**
        registry.registerComponent<QSlider>(
            "QSlider", [](const QJsonObject& config) {
                auto slider = std::make_unique<QSlider>();
                applySliderProperties(slider.get(), config);
                return slider;
            });

        // **Register QProgressBar with range and value support**
        registry.registerComponent<QProgressBar>(
            "QProgressBar", [](const QJsonObject& config) {
                auto progressBar = std::make_unique<QProgressBar>();
                applyProgressBarProperties(progressBar.get(), config);
                return progressBar;
            });
    }

    /**
     * @brief Helper functions for applying component properties
     */

    /**
     * @brief Applies text property to widgets that support setText()
     * @param widget The widget to apply properties to
     * @param config The JSON configuration object
     */
    template <typename T>
    static void applyTextProperty(T* widget, const QJsonObject& config) {
        if (config.contains("properties")) {
            QJsonObject props = config["properties"].toObject();
            if (props.contains("text")) {
                widget->setText(props["text"].toString());
            }
        }
    }

    /**
     * @brief Applies QLineEdit-specific properties
     * @param lineEdit The QLineEdit widget to configure
     * @param config The JSON configuration object
     */
    static void applyLineEditProperties(QLineEdit* lineEdit,
                                        const QJsonObject& config) {
        if (!config.contains("properties"))
            return;

        QJsonObject props = config["properties"].toObject();
        if (props.contains("placeholderText")) {
            lineEdit->setPlaceholderText(props["placeholderText"].toString());
        }
        if (props.contains("text")) {
            lineEdit->setText(props["text"].toString());
        }
    }

    /**
     * @brief Applies QComboBox-specific properties
     * @param comboBox The QComboBox widget to configure
     * @param config The JSON configuration object
     */
    static void applyComboBoxProperties(QComboBox* comboBox,
                                        const QJsonObject& config) {
        if (!config.contains("properties"))
            return;

        QJsonObject props = config["properties"].toObject();
        if (props.contains("items")) {
            QJsonArray items = props["items"].toArray();
            for (const auto& item : items) {
                comboBox->addItem(item.toString());
            }
        }
    }

    /**
     * @brief Applies QCheckBox-specific properties
     * @param checkBox The QCheckBox widget to configure
     * @param config The JSON configuration object
     */
    static void applyCheckBoxProperties(QCheckBox* checkBox,
                                        const QJsonObject& config) {
        if (!config.contains("properties"))
            return;

        QJsonObject props = config["properties"].toObject();
        if (props.contains("text")) {
            checkBox->setText(props["text"].toString());
        }
        if (props.contains("checked")) {
            checkBox->setChecked(props["checked"].toBool());
        }
    }

    /**
     * @brief Applies QSlider-specific properties
     * @param slider The QSlider widget to configure
     * @param config The JSON configuration object
     */
    static void applySliderProperties(QSlider* slider,
                                      const QJsonObject& config) {
        if (!config.contains("properties"))
            return;

        QJsonObject props = config["properties"].toObject();
        if (props.contains("orientation")) {
            slider->setOrientation(
                static_cast<Qt::Orientation>(props["orientation"].toInt()));
        }
        if (props.contains("minimum")) {
            slider->setMinimum(props["minimum"].toInt());
        }
        if (props.contains("maximum")) {
            slider->setMaximum(props["maximum"].toInt());
        }
        if (props.contains("value")) {
            slider->setValue(props["value"].toInt());
        }
    }

    /**
     * @brief Applies QProgressBar-specific properties
     * @param progressBar The QProgressBar widget to configure
     * @param config The JSON configuration object
     */
    static void applyProgressBarProperties(QProgressBar* progressBar,
                                           const QJsonObject& config) {
        if (!config.contains("properties"))
            return;

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

public:
    void createUI() {
        try {
            QString ui_file_path = "Resources/ui_definitions/form_demo.json";
            main_widget_ = ui_loader_->loadFromFile(ui_file_path);

            if (!main_widget_) {
                throw std::runtime_error(
                    "Failed to create main widget from JSON");
            }

            qDebug() << "✅ Form UI loaded successfully from" << ui_file_path;

        } catch (const std::exception& e) {
            qCritical() << "UI Creation failed:" << e.what();
            main_widget_ = createFallbackUI();
        }
    }

    std::unique_ptr<QWidget> createFallbackUI() {
        // **Create fallback UI programmatically**
        auto widget = std::make_unique<QWidget>();
        widget->setWindowTitle("Form Demo - Fallback UI");
        widget->setMinimumSize(400, 500);

        auto layout = new QVBoxLayout(widget.get());

        // **Title**
        auto title = new QLabel("📝 Form Demo");
        title->setAlignment(Qt::AlignCenter);
        title->setStyleSheet(
            "font-size: 24px; font-weight: bold; margin-bottom: 20px;");
        layout->addWidget(title);

        // **Name input**
        auto name_label = new QLabel("Name:");
        auto name_input = new QLineEdit();
        name_input->setPlaceholderText("Enter your name");
        layout->addWidget(name_label);
        layout->addWidget(name_input);

        // **Email input**
        auto email_label = new QLabel("Email:");
        auto email_input = new QLineEdit();
        email_input->setPlaceholderText("Enter your email");
        layout->addWidget(email_label);
        layout->addWidget(email_input);

        // **Submit button**
        auto submit_button = new QPushButton("Submit");
        layout->addWidget(submit_button);

        connect(submit_button, &QPushButton::clicked, this,
                &FormDemoApp::onSubmitForm);

        return widget;
    }

    void setupHotReload() {
        hot_reload_manager_ = std::make_unique<HotReload::HotReloadManager>();

        // **Watch UI definition files**
        hot_reload_manager_->registerUIFile(
            "Resources/ui_definitions/form_demo.json", main_widget_.get());

        // **Connect hot reload signals**
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
                        qCritical() << "Hot reload failed:" << e.what();
                    }
                });
    }

    void updateFormValidation() {
        bool is_valid =
            !name_state_->get().isEmpty() && !email_state_->get().isEmpty() &&
            age_state_->get() > 0 && !country_state_->get().isEmpty();

        form_valid_state_->set(is_valid);
    }

    void resetForm() {
        name_state_->set("");
        email_state_->set("");
        age_state_->set(25);
        country_state_->set("United States");
        newsletter_state_->set(false);
        progress_value_state_->set(0);
        progress_status_state_->set("🔴 Getting started...");
        updateFormValidation();
    }
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    try {
        FormDemoApp form_demo;
        form_demo.run();

        return app.exec();

    } catch (const std::exception& e) {
        qCritical() << "Application error:" << e.what();
        return -1;
    }
}

#include "form-example.moc"
