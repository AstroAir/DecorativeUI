#include "ComponentRegistry.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QDoubleSpinBox>
#include <QFrame>
#include <QGroupBox>
#include <QJsonArray>
#include <QLabel>
#include <QLineEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollArea>
#include <QSlider>
#include <QSpinBox>
#include <QSplitter>
#include <QTabWidget>
#include <QTextEdit>

namespace DeclarativeUI::JSON {

ComponentRegistry& ComponentRegistry::instance() {
    static ComponentRegistry instance;
    return instance;
}

std::unique_ptr<QWidget> ComponentRegistry::createComponent(
    const QString& type_name, const QJsonObject& config) {
    auto factory_it = factories_.find(type_name);
    if (factory_it == factories_.end()) {
        throw Exceptions::ComponentRegistrationException(
            "Component type not registered: " + type_name.toStdString());
    }

    try {
        auto widget = factory_it->second->create(config);

        if (!widget) {
            throw Exceptions::ComponentCreationException(
                "Factory returned null widget for type: " +
                type_name.toStdString());
        }

        return widget;

    } catch (const std::exception& e) {
        throw Exceptions::ComponentCreationException(type_name.toStdString() +
                                                     ": " + e.what());
    }
}

bool ComponentRegistry::hasComponent(const QString& type_name) const noexcept {
    return factories_.find(type_name) != factories_.end();
}

QStringList ComponentRegistry::getRegisteredTypes() const {
    QStringList types;
    for (const auto& [type_name, factory] : factories_) {
        types.append(type_name);
    }
    return types;
}

void ComponentRegistry::clear() noexcept {
    try {
        factories_.clear();
        registerBuiltinComponents();
    } catch (...) {
        // No-throw guarantee
    }
}

void ComponentRegistry::registerBuiltinComponents() {
    try {
        // Register components by category to improve maintainability
        registerBasicWidgets();
        registerInputWidgets();
        registerNumericWidgets();
        registerDisplayWidgets();
        registerContainerWidgets();

        qDebug() << "✅ Registered" << factories_.size()
                 << "built-in components";

    } catch (const std::exception& e) {
        qWarning() << "❌ Failed to register built-in components:" << e.what();
        throw Exceptions::ComponentRegistrationException(
            "Built-in component registration failed: " + std::string(e.what()));
    }
}

void ComponentRegistry::registerBasicWidgets() {
    // Register basic widget components with minimal configuration
    registerComponent<QWidget>("QWidget", [](const QJsonObject&) {
        return std::make_unique<QWidget>();
    });

    registerComponent<QLabel>("QLabel", [this](const QJsonObject& config) {
        auto label = std::make_unique<QLabel>();
        configureLabelProperties(label, config);
        return label;
    });

    registerComponent<QPushButton>(
        "QPushButton", [this](const QJsonObject& config) {
            auto button = std::make_unique<QPushButton>();
            configureButtonProperties(button, config);
            return button;
        });
}

void ComponentRegistry::registerInputWidgets() {
    // Register input widget components with their specific configurations
    registerComponent<QLineEdit>(
        "QLineEdit", [this](const QJsonObject& config) {
            auto lineEdit = std::make_unique<QLineEdit>();
            configureLineEditProperties(lineEdit, config);
            return lineEdit;
        });

    registerComponent<QTextEdit>(
        "QTextEdit", [this](const QJsonObject& config) {
            auto textEdit = std::make_unique<QTextEdit>();
            configureTextEditProperties(textEdit, config);
            return textEdit;
        });

    registerComponent<QCheckBox>(
        "QCheckBox", [this](const QJsonObject& config) {
            auto checkBox = std::make_unique<QCheckBox>();
            configureCheckBoxProperties(checkBox, config);
            return checkBox;
        });

    registerComponent<QRadioButton>(
        "QRadioButton", [this](const QJsonObject& config) {
            auto radioButton = std::make_unique<QRadioButton>();
            configureRadioButtonProperties(radioButton, config);
            return radioButton;
        });

    registerComponent<QComboBox>(
        "QComboBox", [this](const QJsonObject& config) {
            auto comboBox = std::make_unique<QComboBox>();
            configureComboBoxProperties(comboBox, config);
            return comboBox;
        });
}

void ComponentRegistry::registerNumericWidgets() {
    // Register numeric input widget components
    registerComponent<QSpinBox>("QSpinBox", [this](const QJsonObject& config) {
        auto spinBox = std::make_unique<QSpinBox>();
        configureSpinBoxProperties(spinBox, config);
        return spinBox;
    });

    registerComponent<QDoubleSpinBox>(
        "QDoubleSpinBox", [this](const QJsonObject& config) {
            auto doubleSpinBox = std::make_unique<QDoubleSpinBox>();
            configureDoubleSpinBoxProperties(doubleSpinBox, config);
            return doubleSpinBox;
        });

    registerComponent<QSlider>("QSlider", [this](const QJsonObject& config) {
        auto slider = std::make_unique<QSlider>();
        configureSliderProperties(slider, config);
        return slider;
    });
}

void ComponentRegistry::registerDisplayWidgets() {
    // Register display widget components
    registerComponent<QProgressBar>(
        "QProgressBar", [this](const QJsonObject& config) {
            auto progressBar = std::make_unique<QProgressBar>();
            configureProgressBarProperties(progressBar, config);
            return progressBar;
        });
}

void ComponentRegistry::registerContainerWidgets() {
    // Register container widget components
    registerComponent<QGroupBox>(
        "QGroupBox", [this](const QJsonObject& config) {
            auto groupBox = std::make_unique<QGroupBox>();
            configureGroupBoxProperties(groupBox, config);
            return groupBox;
        });

    registerComponent<QFrame>("QFrame", [this](const QJsonObject& config) {
        auto frame = std::make_unique<QFrame>();
        configureFrameProperties(frame, config);
        return frame;
    });

    registerComponent<QScrollArea>(
        "QScrollArea", [this](const QJsonObject& config) {
            auto scrollArea = std::make_unique<QScrollArea>();
            configureScrollAreaProperties(scrollArea, config);
            return scrollArea;
        });

    registerComponent<QTabWidget>(
        "QTabWidget", [this](const QJsonObject& config) {
            auto tabWidget = std::make_unique<QTabWidget>();
            configureTabWidgetProperties(tabWidget, config);
            return tabWidget;
        });

    registerComponent<QSplitter>(
        "QSplitter", [this](const QJsonObject& config) {
            auto splitter = std::make_unique<QSplitter>();
            configureSplitterProperties(splitter, config);
            return splitter;
        });
}

// Property configuration helper methods implementation
void ComponentRegistry::configureLabelProperties(std::unique_ptr<QLabel>& label,
                                                 const QJsonObject& config) {
    if (!config.contains("properties"))
        return;

    QJsonObject props = config["properties"].toObject();
    if (props.contains("text")) {
        label->setText(props["text"].toString());
    }
    if (props.contains("wordWrap")) {
        label->setWordWrap(props["wordWrap"].toBool());
    }
    if (props.contains("alignment")) {
        label->setAlignment(
            static_cast<Qt::Alignment>(props["alignment"].toInt()));
    }
}

void ComponentRegistry::configureButtonProperties(
    std::unique_ptr<QPushButton>& button, const QJsonObject& config) {
    if (!config.contains("properties"))
        return;

    QJsonObject props = config["properties"].toObject();
    if (props.contains("text")) {
        button->setText(props["text"].toString());
    }
    if (props.contains("checkable")) {
        button->setCheckable(props["checkable"].toBool());
    }
    if (props.contains("checked")) {
        button->setChecked(props["checked"].toBool());
    }
}

void ComponentRegistry::configureLineEditProperties(
    std::unique_ptr<QLineEdit>& lineEdit, const QJsonObject& config) {
    if (!config.contains("properties"))
        return;

    QJsonObject props = config["properties"].toObject();
    if (props.contains("text")) {
        lineEdit->setText(props["text"].toString());
    }
    if (props.contains("placeholderText")) {
        lineEdit->setPlaceholderText(props["placeholderText"].toString());
    }
    if (props.contains("readOnly")) {
        lineEdit->setReadOnly(props["readOnly"].toBool());
    }
    if (props.contains("maxLength")) {
        lineEdit->setMaxLength(props["maxLength"].toInt());
    }
}

void ComponentRegistry::configureTextEditProperties(
    std::unique_ptr<QTextEdit>& textEdit, const QJsonObject& config) {
    if (!config.contains("properties"))
        return;

    QJsonObject props = config["properties"].toObject();
    if (props.contains("html")) {
        textEdit->setHtml(props["html"].toString());
    } else if (props.contains("plainText")) {
        textEdit->setPlainText(props["plainText"].toString());
    }
    if (props.contains("readOnly")) {
        textEdit->setReadOnly(props["readOnly"].toBool());
    }
}

void ComponentRegistry::configureCheckBoxProperties(
    std::unique_ptr<QCheckBox>& checkBox, const QJsonObject& config) {
    if (!config.contains("properties"))
        return;

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

void ComponentRegistry::configureRadioButtonProperties(
    std::unique_ptr<QRadioButton>& radioButton, const QJsonObject& config) {
    if (!config.contains("properties"))
        return;

    QJsonObject props = config["properties"].toObject();
    if (props.contains("text")) {
        radioButton->setText(props["text"].toString());
    }
    if (props.contains("checked")) {
        radioButton->setChecked(props["checked"].toBool());
    }
}

void ComponentRegistry::configureComboBoxProperties(
    std::unique_ptr<QComboBox>& comboBox, const QJsonObject& config) {
    if (!config.contains("properties"))
        return;

    QJsonObject props = config["properties"].toObject();
    if (props.contains("items")) {
        QJsonArray items = props["items"].toArray();
        for (const QJsonValue& item : items) {
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

void ComponentRegistry::configureSpinBoxProperties(
    std::unique_ptr<QSpinBox>& spinBox, const QJsonObject& config) {
    if (!config.contains("properties"))
        return;

    QJsonObject props = config["properties"].toObject();
    if (props.contains("value")) {
        spinBox->setValue(props["value"].toInt());
    }
    if (props.contains("minimum")) {
        spinBox->setMinimum(props["minimum"].toInt());
    }
    if (props.contains("maximum")) {
        spinBox->setMaximum(props["maximum"].toInt());
    }
    if (props.contains("singleStep")) {
        spinBox->setSingleStep(props["singleStep"].toInt());
    }
    if (props.contains("prefix")) {
        spinBox->setPrefix(props["prefix"].toString());
    }
    if (props.contains("suffix")) {
        spinBox->setSuffix(props["suffix"].toString());
    }
}

void ComponentRegistry::configureDoubleSpinBoxProperties(
    std::unique_ptr<QDoubleSpinBox>& doubleSpinBox, const QJsonObject& config) {
    if (!config.contains("properties"))
        return;

    QJsonObject props = config["properties"].toObject();
    if (props.contains("value")) {
        doubleSpinBox->setValue(props["value"].toDouble());
    }
    if (props.contains("minimum")) {
        doubleSpinBox->setMinimum(props["minimum"].toDouble());
    }
    if (props.contains("maximum")) {
        doubleSpinBox->setMaximum(props["maximum"].toDouble());
    }
    if (props.contains("singleStep")) {
        doubleSpinBox->setSingleStep(props["singleStep"].toDouble());
    }
    if (props.contains("decimals")) {
        doubleSpinBox->setDecimals(props["decimals"].toInt());
    }
}

void ComponentRegistry::configureSliderProperties(
    std::unique_ptr<QSlider>& slider, const QJsonObject& config) {
    if (!config.contains("properties"))
        return;

    QJsonObject props = config["properties"].toObject();
    if (props.contains("orientation")) {
        QString orientation = props["orientation"].toString().toLower();
        slider->setOrientation(orientation == "vertical" ? Qt::Vertical
                                                         : Qt::Horizontal);
    }
    if (props.contains("value")) {
        slider->setValue(props["value"].toInt());
    }
    if (props.contains("minimum")) {
        slider->setMinimum(props["minimum"].toInt());
    }
    if (props.contains("maximum")) {
        slider->setMaximum(props["maximum"].toInt());
    }
    if (props.contains("singleStep")) {
        slider->setSingleStep(props["singleStep"].toInt());
    }
    if (props.contains("pageStep")) {
        slider->setPageStep(props["pageStep"].toInt());
    }
}

void ComponentRegistry::configureProgressBarProperties(
    std::unique_ptr<QProgressBar>& progressBar, const QJsonObject& config) {
    if (!config.contains("properties"))
        return;

    QJsonObject props = config["properties"].toObject();
    if (props.contains("value")) {
        progressBar->setValue(props["value"].toInt());
    }
    if (props.contains("minimum")) {
        progressBar->setMinimum(props["minimum"].toInt());
    }
    if (props.contains("maximum")) {
        progressBar->setMaximum(props["maximum"].toInt());
    }
    if (props.contains("format")) {
        progressBar->setFormat(props["format"].toString());
    }
    if (props.contains("textVisible")) {
        progressBar->setTextVisible(props["textVisible"].toBool());
    }
}

void ComponentRegistry::configureGroupBoxProperties(
    std::unique_ptr<QGroupBox>& groupBox, const QJsonObject& config) {
    if (!config.contains("properties"))
        return;

    QJsonObject props = config["properties"].toObject();
    if (props.contains("title")) {
        groupBox->setTitle(props["title"].toString());
    }
    if (props.contains("checkable")) {
        groupBox->setCheckable(props["checkable"].toBool());
    }
    if (props.contains("checked")) {
        groupBox->setChecked(props["checked"].toBool());
    }
}

void ComponentRegistry::configureFrameProperties(std::unique_ptr<QFrame>& frame,
                                                 const QJsonObject& config) {
    if (!config.contains("properties"))
        return;

    QJsonObject props = config["properties"].toObject();
    if (props.contains("frameShape")) {
        QString shape = props["frameShape"].toString().toLower();
        if (shape == "box") {
            frame->setFrameShape(QFrame::Box);
        } else if (shape == "panel") {
            frame->setFrameShape(QFrame::Panel);
        } else if (shape == "hline") {
            frame->setFrameShape(QFrame::HLine);
        } else if (shape == "vline") {
            frame->setFrameShape(QFrame::VLine);
        }
    }
    if (props.contains("frameShadow")) {
        QString shadow = props["frameShadow"].toString().toLower();
        if (shadow == "raised") {
            frame->setFrameShadow(QFrame::Raised);
        } else if (shadow == "sunken") {
            frame->setFrameShadow(QFrame::Sunken);
        }
    }
    if (props.contains("lineWidth")) {
        frame->setLineWidth(props["lineWidth"].toInt());
    }
}

void ComponentRegistry::configureScrollAreaProperties(
    std::unique_ptr<QScrollArea>& scrollArea, const QJsonObject& config) {
    if (!config.contains("properties"))
        return;

    QJsonObject props = config["properties"].toObject();
    if (props.contains("widgetResizable")) {
        scrollArea->setWidgetResizable(props["widgetResizable"].toBool());
    }
    if (props.contains("horizontalScrollBarPolicy")) {
        QString policy =
            props["horizontalScrollBarPolicy"].toString().toLower();
        if (policy == "alwayson") {
            scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        } else if (policy == "alwaysoff") {
            scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        } else {
            scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        }
    }
    if (props.contains("verticalScrollBarPolicy")) {
        QString policy = props["verticalScrollBarPolicy"].toString().toLower();
        if (policy == "alwayson") {
            scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        } else if (policy == "alwaysoff") {
            scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        } else {
            scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        }
    }
}

void ComponentRegistry::configureTabWidgetProperties(
    std::unique_ptr<QTabWidget>& tabWidget, const QJsonObject& config) {
    if (!config.contains("properties"))
        return;

    QJsonObject props = config["properties"].toObject();
    if (props.contains("currentIndex")) {
        tabWidget->setCurrentIndex(props["currentIndex"].toInt());
    }
    if (props.contains("tabPosition")) {
        QString position = props["tabPosition"].toString().toLower();
        if (position == "north") {
            tabWidget->setTabPosition(QTabWidget::North);
        } else if (position == "south") {
            tabWidget->setTabPosition(QTabWidget::South);
        } else if (position == "west") {
            tabWidget->setTabPosition(QTabWidget::West);
        } else if (position == "east") {
            tabWidget->setTabPosition(QTabWidget::East);
        }
    }
    if (props.contains("tabsClosable")) {
        tabWidget->setTabsClosable(props["tabsClosable"].toBool());
    }
    if (props.contains("movable")) {
        tabWidget->setMovable(props["movable"].toBool());
    }
}

void ComponentRegistry::configureSplitterProperties(
    std::unique_ptr<QSplitter>& splitter, const QJsonObject& config) {
    if (!config.contains("properties"))
        return;

    QJsonObject props = config["properties"].toObject();
    if (props.contains("orientation")) {
        QString orientation = props["orientation"].toString().toLower();
        splitter->setOrientation(orientation == "vertical" ? Qt::Vertical
                                                           : Qt::Horizontal);
    }
    if (props.contains("childrenCollapsible")) {
        splitter->setChildrenCollapsible(props["childrenCollapsible"].toBool());
    }
}

}  // namespace DeclarativeUI::JSON
