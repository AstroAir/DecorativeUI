/**
 * @file ComponentGallery.cpp
 * @brief Implementation of the component gallery widget
 */

#include "ComponentGallery.hpp"
#include <QApplication>
#include <QClipboard>
#include <QDebug>
#include <QMessageBox>

// Include enhanced components
#include "../../src/Components/Enhanced/ComponentEnhancer.hpp"

// For the enhanced component namespace
using namespace DeclarativeUI::Components;

ComponentGallery::ComponentGallery(QWidget* parent)
    : QWidget(parent),
      main_layout_(nullptr),
      splitter_(nullptr),
      component_list_(nullptr),
      demo_widget_(nullptr),
      demo_layout_(nullptr),
      demo_group_(nullptr),
      current_demo_(nullptr),
      code_group_(nullptr),
      code_viewer_(nullptr),
      copy_button_(nullptr),
      current_component_("") {
    setupUI();
    createComponentList();
    createComponentDemo();
    createCodeViewer();

    // Select first component by default
    if (component_list_->count() > 0) {
        component_list_->setCurrentRow(0);
        onComponentSelected(component_list_->item(0)->text());
    }
}

void ComponentGallery::setupUI() {
    main_layout_ = new QHBoxLayout(this);
    main_layout_->setContentsMargins(8, 8, 8, 8);
    main_layout_->setSpacing(8);

    splitter_ = new QSplitter(Qt::Horizontal);
    main_layout_->addWidget(splitter_);
}

void ComponentGallery::createComponentList() {
    component_list_ = new QListWidget();
    component_list_->setMaximumWidth(200);
    component_list_->setMinimumWidth(150);

    // Add all available components
    QStringList components = {
        "Button",       "LineEdit",  "CheckBox",   "ComboBox",  "Label",
        "ProgressBar",  "Slider",    "SpinBox",    "TextEdit",  "RadioButton",
        "GroupBox",     "TabWidget", "ScrollArea", "Splitter",  "Calendar",
        "DateTimeEdit", "Dial",      "LCDNumber",  "TableView", "ListView"};

    for (const QString& component : components) {
        component_list_->addItem(component);
    }

    connect(component_list_, &QListWidget::itemClicked,
            [this](QListWidgetItem* item) {
                if (item)
                    onComponentSelected(item->text());
            });

    splitter_->addWidget(component_list_);
}

void ComponentGallery::createComponentDemo() {
    demo_widget_ = new QWidget();
    demo_layout_ = new QVBoxLayout(demo_widget_);

    demo_group_ = new QGroupBox("Component Demo");
    demo_layout_->addWidget(demo_group_);

    splitter_->addWidget(demo_widget_);
}

void ComponentGallery::createCodeViewer() {
    code_group_ = new QGroupBox("Code Example");
    auto code_layout = new QVBoxLayout(code_group_);

    code_viewer_ = new QTextEdit();
    code_viewer_->setReadOnly(true);
    code_viewer_->setFont(QFont("Consolas", 10));
    code_viewer_->setMaximumHeight(200);

    copy_button_ = new QPushButton("Copy Code");
    connect(copy_button_, &QPushButton::clicked, this,
            &ComponentGallery::onCodeCopyRequested);

    code_layout->addWidget(code_viewer_);
    code_layout->addWidget(copy_button_);

    demo_layout_->addWidget(code_group_);
}

void ComponentGallery::onComponentSelected(const QString& component_name) {
    current_component_ = component_name;
    showComponent(component_name);
    updateCodeExample(component_name);
}

void ComponentGallery::showComponent(const QString& name) {
    // Clear previous demo
    if (current_demo_) {
        current_demo_->deleteLater();
        current_demo_ = nullptr;
    }

    // Create new demo widget
    current_demo_ = new QWidget();
    auto layout = new QVBoxLayout(current_demo_);

    // Create component-specific demo with enhancements
    if (name == "Button") {
        // Basic button with enhancements
        auto button = new QPushButton("Enhanced Button");
        connect(button, &QPushButton::clicked, []() {
            QMessageBox::information(nullptr, "Button",
                                     "Enhanced button clicked!");
        });

        // Apply enhancements using the ComponentEnhancer
        Enhanced::ComponentEnhancer::enhance(
            button,
            Enhanced::enhance()
                .accessibleName("Sample Button")
                .accessibleDescription(
                    "A button that demonstrates enhanced features")
                .tooltip("Click me to see enhanced button functionality")
                .dropShadow(true)
                .hoverEffect(true)
                .focusEffect(true)
                .borderRadius(8)
                .focusColor(QColor("#3498db"))
                .shortcut(QKeySequence("Ctrl+B"))
                .build());

        layout->addWidget(button);

        // Validation button example
        auto validation_button = new QPushButton("Validation Button");
        Enhanced::ComponentEnhancer::enhance(
            validation_button,
            Enhanced::enhance()
                .accessibleName("Validation Button")
                .tooltip("This button demonstrates validation features")
                .required(true)
                .validator([](QWidget* widget) -> bool {
                    // Example validation: button must be enabled
                    return widget->isEnabled();
                })
                .validationError("Button must be enabled to be valid")
                .dropShadow(true)
                .borderRadius(4)
                .build());

        layout->addWidget(validation_button);

    } else if (name == "LineEdit") {
        // Enhanced line edit with validation
        auto line_edit = new QLineEdit();
        line_edit->setPlaceholderText("Enter your email...");

        Enhanced::ComponentEnhancer::enhance(
            line_edit,
            Enhanced::enhance()
                .accessibleName("Email Input")
                .accessibleDescription("Enter your email address")
                .tooltip("Please enter a valid email address")
                .required(true)
                .validator([](QWidget* widget) -> bool {
                    auto* edit = qobject_cast<QLineEdit*>(widget);
                    if (!edit)
                        return false;
                    QString text = edit->text();
                    return text.contains("@") && text.contains(".");
                })
                .validationError("Please enter a valid email address")
                .validateOnChange(true)
                .focusEffect(true)
                .focusColor(QColor("#3498db"))
                .borderRadius(4)
                .selectAllOnFocus(true)
                .build());

        layout->addWidget(line_edit);

        // Auto-complete example
        auto autocomplete_edit = new QLineEdit();
        autocomplete_edit->setPlaceholderText("Type a country name...");

        QStringList countries = {
            "United States", "United Kingdom", "Canada", "Australia", "Germany",
            "France",        "Japan",          "China",  "Brazil",    "India"};

        Enhanced::ComponentEnhancer::enhance(
            autocomplete_edit,
            Enhanced::enhance()
                .accessibleName("Country Input")
                .tooltip("Start typing to see country suggestions")
                .autoComplete(countries)
                .focusEffect(true)
                .borderRadius(4)
                .build());

        layout->addWidget(autocomplete_edit);

    } else if (name == "CheckBox") {
        // Enhanced checkboxes
        auto checkbox1 = new QCheckBox("Enhanced Option 1");
        Enhanced::ComponentEnhancer::enhance(
            checkbox1,
            Enhanced::enhance()
                .accessibleName("Option 1")
                .accessibleDescription(
                    "First checkbox option with enhancements")
                .tooltip(
                    "This is an enhanced checkbox with accessibility features")
                .hoverEffect(true)
                .focusEffect(true)
                .shortcut(QKeySequence("Ctrl+1"))
                .build());

        auto checkbox2 = new QCheckBox("Required Option");
        checkbox2->setChecked(true);
        Enhanced::ComponentEnhancer::enhance(
            checkbox2, Enhanced::enhance()
                           .accessibleName("Required Option")
                           .tooltip("This option is required")
                           .required(true)
                           .validator([](QWidget* widget) -> bool {
                               auto* checkbox =
                                   qobject_cast<QCheckBox*>(widget);
                               return checkbox && checkbox->isChecked();
                           })
                           .validationError("This option must be checked")
                           .focusEffect(true)
                           .build());

        auto checkbox3 = new QCheckBox("Disabled Option");
        checkbox3->setEnabled(false);
        Enhanced::ComponentEnhancer::enhance(
            checkbox3,
            Enhanced::enhance()
                .accessibleName("Disabled Option")
                .accessibleDescription("This option is currently disabled")
                .tooltip("This option is disabled for demonstration")
                .build());

        layout->addWidget(checkbox1);
        layout->addWidget(checkbox2);
        layout->addWidget(checkbox3);

    } else if (name == "ProgressBar") {
        auto progress = new QProgressBar();
        progress->setValue(65);
        Enhanced::ComponentEnhancer::enhance(
            progress,
            Enhanced::enhance()
                .accessibleName("Progress Indicator")
                .accessibleDescription("Shows current progress at 65%")
                .tooltip("Current progress: 65%")
                .borderRadius(8)
                .build());
        layout->addWidget(progress);

        auto animated_progress = new QProgressBar();
        animated_progress->setRange(0, 0);  // Indeterminate
        Enhanced::ComponentEnhancer::enhance(
            animated_progress,
            Enhanced::enhance()
                .accessibleName("Loading Indicator")
                .accessibleDescription("Indeterminate progress indicator")
                .tooltip("Loading in progress...")
                .borderRadius(8)
                .build());
        layout->addWidget(animated_progress);

    } else {
        // Generic demo for other components with basic enhancements
        auto label =
            new QLabel(QString("Enhanced Demo for %1 component").arg(name));
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet(
            "QLabel { padding: 20px; background-color: #f0f0f0; border: 1px "
            "solid #ccc; }");

        Enhanced::ComponentEnhancer::enhance(
            label,
            Enhanced::enhance()
                .accessibleName(QString("%1 Demo").arg(name))
                .accessibleDescription(
                    QString("Demonstration of enhanced %1 component").arg(name))
                .tooltip(
                    QString("This shows an enhanced %1 component").arg(name))
                .borderRadius(8)
                .dropShadow(true)
                .build());

        layout->addWidget(label);
    }

    layout->addStretch();

    // Add to demo group
    if (demo_group_->layout()) {
        delete demo_group_->layout();
    }
    demo_group_->setLayout(new QVBoxLayout());
    demo_group_->layout()->addWidget(current_demo_);
    demo_group_->setTitle(QString("Enhanced %1 Demo").arg(name));
}

void ComponentGallery::updateCodeExample(const QString& component_name) {
    QString code_example;

    if (component_name == "Button") {
        code_example = R"(#include "Components/Button.hpp"
using namespace DeclarativeUI::Components;

auto button = std::make_unique<Button>();
button->text("Click Me")
      ->onClick([]() {
          qDebug() << "Button clicked!";
      })
      ->style("background-color: #3498db; color: white;")
      ->initialize();)";

    } else if (component_name == "LineEdit") {
        code_example = R"(#include "Components/LineEdit.hpp"
using namespace DeclarativeUI::Components;

auto lineEdit = std::make_unique<LineEdit>();
lineEdit->placeholder("Enter your name")
        ->maxLength(50)
        ->onTextChanged([](const QString& text) {
            qDebug() << "Text changed:" << text;
        })
        ->initialize();)";

    } else if (component_name == "CheckBox") {
        code_example = R"(#include "Components/CheckBox.hpp"
using namespace DeclarativeUI::Components;

auto checkBox = std::make_unique<CheckBox>();
checkBox->text("Enable feature")
        ->checked(true)
        ->onToggled([](bool checked) {
            qDebug() << "Checkbox toggled:" << checked;
        })
        ->initialize();)";

    } else {
        code_example = QString(R"(#include "Components/%1.hpp"
using namespace DeclarativeUI::Components;

auto component = std::make_unique<%1>();
// Configure component properties here
component->initialize();)")
                           .arg(component_name);
    }

    code_viewer_->setPlainText(code_example);
}

void ComponentGallery::onPropertyChanged() {
    // Handle property changes in the demo
    qDebug() << "Property changed for component:" << current_component_;
}

void ComponentGallery::onCodeCopyRequested() {
    QApplication::clipboard()->setText(code_viewer_->toPlainText());
    QMessageBox::information(this, "Code Copied",
                             "Code example copied to clipboard!");
}

#include "ComponentGallery.moc"
