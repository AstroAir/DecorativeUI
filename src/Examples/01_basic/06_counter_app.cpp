// Examples/01_basic/06_counter_app.cpp
#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QSlider>
#include <QVBoxLayout>
#include <memory>


#include "../../Binding/StateManager.hpp"
#include "../../Core/DeclarativeBuilder.hpp"
#include "../../Exceptions/UIExceptions.hpp"
#include "../../JSON/ComponentRegistry.hpp"
#include "../../JSON/JSONUILoader.hpp"


using namespace DeclarativeUI;

class CounterApp : public QObject {
    Q_OBJECT

public:
    CounterApp(QObject* parent = nullptr) : QObject(parent) {
        // **Initialize state manager**
        state_manager_ = std::shared_ptr<Binding::StateManager>(
            &Binding::StateManager::instance(), [](Binding::StateManager*) {});

        // **Create reactive state**
        counter_state_ = state_manager_->createState<int>("counter", 0);

        // **Create computed state**
        counter_text_ =
            state_manager_->createComputed<QString>("counter_text", [this]() {
                int count = counter_state_->get();
                QString emoji = count > 0 ? "📈" : count < 0 ? "📉" : "🎯";
                return QString("%1 Count: %2").arg(emoji).arg(count);
            });

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

        // **Register built-in components**
        registerComponents();
    }

    void incrementCounter() {
        int current = counter_state_->get();
        counter_state_->set(current + 1);
        counter_text_->update();
    }

    void decrementCounter() {
        int current = counter_state_->get();
        counter_state_->set(current - 1);
        counter_text_->update();
    }

    void resetCounter() {
        counter_state_->set(0);
        counter_text_->update();
    }

    [[nodiscard]] std::unique_ptr<QWidget> createUI();
    [[nodiscard]] std::unique_ptr<QWidget> createFallbackUI();

private:
    std::shared_ptr<Binding::StateManager> state_manager_;
    std::unique_ptr<JSON::JSONUILoader> ui_loader_;
    std::shared_ptr<Binding::ReactiveProperty<int>> counter_state_;
    std::shared_ptr<Binding::ReactiveProperty<QString>> counter_text_;

    void registerComponents();
};

void CounterApp::registerComponents() {
    auto& registry = JSON::ComponentRegistry::instance();

    // **Register standard Qt widgets**
    registry.registerComponent<QWidget>("QWidget", [](const QJsonObject&) {
        return std::make_unique<QWidget>();
    });

    registry.registerComponent<QLabel>("QLabel", [](const QJsonObject& config) {
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

    // **Register new component types**
    registry.registerComponent<QLineEdit>(
        "QLineEdit", [](const QJsonObject& config) {
            auto lineEdit = std::make_unique<QLineEdit>();

            if (config.contains("properties")) {
                QJsonObject props = config["properties"].toObject();
                if (props.contains("text")) {
                    lineEdit->setText(props["text"].toString());
                }
                if (props.contains("placeholderText")) {
                    lineEdit->setPlaceholderText(
                        props["placeholderText"].toString());
                }
                if (props.contains("readOnly")) {
                    lineEdit->setReadOnly(props["readOnly"].toBool());
                }
                if (props.contains("maxLength")) {
                    lineEdit->setMaxLength(props["maxLength"].toInt());
                }
            }

            return lineEdit;
        });

    registry.registerComponent<QCheckBox>(
        "QCheckBox", [](const QJsonObject& config) {
            auto checkBox = std::make_unique<QCheckBox>();

            if (config.contains("properties")) {
                QJsonObject props = config["properties"].toObject();
                if (props.contains("text")) {
                    checkBox->setText(props["text"].toString());
                }
                if (props.contains("checked")) {
                    checkBox->setChecked(props["checked"].toBool());
                }
                if (props.contains("tristate")) {
                    checkBox->setTristate(props["tristate"].toBool());
                }
            }

            return checkBox;
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
                if (props.contains("currentIndex")) {
                    comboBox->setCurrentIndex(props["currentIndex"].toInt());
                }
                if (props.contains("editable")) {
                    comboBox->setEditable(props["editable"].toBool());
                }
            }

            return comboBox;
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
                if (props.contains("value")) {
                    slider->setValue(props["value"].toInt());
                }
                if (props.contains("singleStep")) {
                    slider->setSingleStep(props["singleStep"].toInt());
                }
                if (props.contains("pageStep")) {
                    slider->setPageStep(props["pageStep"].toInt());
                }
                if (props.contains("tickPosition")) {
                    slider->setTickPosition(static_cast<QSlider::TickPosition>(
                        props["tickPosition"].toInt()));
                }
                if (props.contains("tickInterval")) {
                    slider->setTickInterval(props["tickInterval"].toInt());
                }
            }

            return slider;
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
                if (props.contains("orientation")) {
                    progressBar->setOrientation(static_cast<Qt::Orientation>(
                        props["orientation"].toInt()));
                }
                if (props.contains("textVisible")) {
                    progressBar->setTextVisible(props["textVisible"].toBool());
                }
                if (props.contains("format")) {
                    progressBar->setFormat(props["format"].toString());
                }
                if (props.contains("invertedAppearance")) {
                    progressBar->setInvertedAppearance(
                        props["invertedAppearance"].toBool());
                }
            }

            return progressBar;
        });
}
std::unique_ptr<QWidget> CounterApp::createUI() {
    try {
        QString ui_file_path = "Resources/ui_definitions/counter_app.json";
        auto main_widget = ui_loader_->loadFromFile(ui_file_path);

        if (!main_widget) {
            throw std::runtime_error("Failed to create main widget from JSON");
        }

        qDebug() << "✅ UI loaded successfully from" << ui_file_path;
        return main_widget;

    } catch (const std::exception& e) {
        qCritical() << "UI Creation failed:" << e.what();

        // **Fallback to programmatic UI if JSON fails**
        return createFallbackUI();
    }
}

std::unique_ptr<QWidget> CounterApp::createFallbackUI() {
    try {
        return Core::create<QWidget>()
            .layout<QVBoxLayout>([this](QVBoxLayout* layout) {
                auto label =
                    Core::create<QLabel>()
                        .bind("text",
                              [this]() {
                                  return Core::PropertyValue{
                                      counter_text_->get()};
                              })
                        .property("alignment", Qt::AlignCenter)
                        .property(
                            "styleSheet",
                            "QLabel { font-size: 18px; font-weight: bold; }")
                        .build();

                auto button_container =
                    Core::create<QWidget>()
                        .layout<QHBoxLayout>([this](QHBoxLayout* btn_layout) {
                            auto dec_btn =
                                Core::create<QPushButton>()
                                    .property("text", "-")
                                    .property("minimumSize", QSize(50, 30))
                                    .on("clicked",
                                        [this]() { decrementCounter(); })
                                    .build();

                            auto inc_btn =
                                Core::create<QPushButton>()
                                    .property("text", "+")
                                    .property("minimumSize", QSize(50, 30))
                                    .on("clicked",
                                        [this]() { incrementCounter(); })
                                    .build();

                            auto reset_btn =
                                Core::create<QPushButton>()
                                    .property("text", "Reset")
                                    .property("minimumSize", QSize(80, 30))
                                    .on("clicked", [this]() { resetCounter(); })
                                    .build();

                            btn_layout->addWidget(dec_btn.release());
                            btn_layout->addWidget(inc_btn.release());
                            btn_layout->addWidget(reset_btn.release());
                        })
                        .build();

                layout->addWidget(label.release());
                layout->addWidget(button_container.release());
            })
            .property("windowTitle", "Declarative Counter App")
            .property("minimumSize", QSize(300, 150))
            .build();

    } catch (const Exceptions::UIException& e) {
        qCritical() << "Fallback UI Creation failed:" << e.what();
        return nullptr;
    }
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    try {
        // **Exception-safe application setup**
        CounterApp counter_app;
        auto main_widget = counter_app.createUI();

        if (!main_widget) {
            qCritical() << "Failed to create main widget";
            return -1;
        }

        main_widget->show();

        return app.exec();

    } catch (const std::exception& e) {
        qCritical() << "Application error:" << e.what();
        return -1;
    } catch (...) {
        qCritical() << "Unknown application error";
        return -1;
    }
}

#include "06_counter_app.moc"