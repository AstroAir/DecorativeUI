/**
 * @file ThemeManager.cpp
 * @brief Implementation of theme management widget
 */

#include "ThemeManager.hpp"
#include <QDebug>

ThemeManager::ThemeManager(QWidget* parent)
    : QWidget(parent),
      main_layout_(nullptr),
      selector_group_(nullptr),
      customizer_group_(nullptr),
      preview_group_(nullptr),
      theme_combo_(nullptr),
      preview_button_(nullptr),
      apply_button_(nullptr),
      reset_button_(nullptr),
      preview_widget_(nullptr),
      current_theme_("light") {
    setupUI();
    createThemeSelector();
    createColorCustomizer();
    createPreview();
    loadAvailableThemes();
}

void ThemeManager::setupUI() {
    main_layout_ = new QVBoxLayout(this);
    main_layout_->setContentsMargins(8, 8, 8, 8);
}

void ThemeManager::createThemeSelector() {
    selector_group_ = new QGroupBox("Theme Selection");
    auto selector_layout = new QHBoxLayout(selector_group_);

    theme_combo_ = new QComboBox();
    preview_button_ = new QPushButton("Preview");
    apply_button_ = new QPushButton("Apply");
    reset_button_ = new QPushButton("Reset");

    selector_layout->addWidget(new QLabel("Theme:"));
    selector_layout->addWidget(theme_combo_);
    selector_layout->addWidget(preview_button_);
    selector_layout->addWidget(apply_button_);
    selector_layout->addWidget(reset_button_);
    selector_layout->addStretch();

    // Connect signals
    connect(theme_combo_, &QComboBox::currentTextChanged, this,
            &ThemeManager::onThemeSelected);
    connect(preview_button_, &QPushButton::clicked, this,
            &ThemeManager::onPreviewTheme);
    connect(apply_button_, &QPushButton::clicked, this,
            &ThemeManager::onApplyTheme);
    connect(reset_button_, &QPushButton::clicked, this,
            &ThemeManager::onResetTheme);

    main_layout_->addWidget(selector_group_);
}

void ThemeManager::createColorCustomizer() {
    customizer_group_ = new QGroupBox("Color Customization");
    auto customizer_layout = new QVBoxLayout(customizer_group_);

    auto color_layout = new QHBoxLayout();

    auto primary_button = new QPushButton("Primary Color");
    auto secondary_button = new QPushButton("Secondary Color");
    auto accent_button = new QPushButton("Accent Color");

    primary_button->setStyleSheet(
        "QPushButton { background-color: #3498db; color: white; }");
    secondary_button->setStyleSheet(
        "QPushButton { background-color: #2c3e50; color: white; }");
    accent_button->setStyleSheet(
        "QPushButton { background-color: #e74c3c; color: white; }");

    color_layout->addWidget(primary_button);
    color_layout->addWidget(secondary_button);
    color_layout->addWidget(accent_button);
    color_layout->addStretch();

    customizer_layout->addLayout(color_layout);

    // Connect color buttons
    connect(primary_button, &QPushButton::clicked, this,
            &ThemeManager::onCustomColorChanged);
    connect(secondary_button, &QPushButton::clicked, this,
            &ThemeManager::onCustomColorChanged);
    connect(accent_button, &QPushButton::clicked, this,
            &ThemeManager::onCustomColorChanged);

    main_layout_->addWidget(customizer_group_);
}

void ThemeManager::createPreview() {
    preview_group_ = new QGroupBox("Theme Preview");
    auto preview_layout = new QVBoxLayout(preview_group_);

    preview_widget_ = new QWidget();
    preview_widget_->setMinimumHeight(200);

    // Create preview content
    auto content_layout = new QVBoxLayout(preview_widget_);

    auto title_label = new QLabel("Theme Preview");
    title_label->setStyleSheet(
        "QLabel { font-size: 16px; font-weight: bold; }");

    auto sample_button = new QPushButton("Sample Button");
    auto sample_input = new QLineEdit("Sample input text");
    auto sample_checkbox = new QCheckBox("Sample checkbox");

    content_layout->addWidget(title_label);
    content_layout->addWidget(sample_button);
    content_layout->addWidget(sample_input);
    content_layout->addWidget(sample_checkbox);
    content_layout->addStretch();

    preview_layout->addWidget(preview_widget_);
    main_layout_->addWidget(preview_group_);

    // Add description
    auto description = new QLabel(
        "Select a theme from the dropdown and click 'Preview' to see how it "
        "looks. "
        "Use 'Apply' to set the theme for the entire application.");
    description->setWordWrap(true);
    description->setStyleSheet("QLabel { color: #666; font-style: italic; }");
    main_layout_->addWidget(description);
}

void ThemeManager::loadAvailableThemes() {
    theme_combo_->addItems(
        {"Light", "Dark", "Blue", "Green", "Purple", "Custom"});

    // Initialize theme stylesheets
    theme_stylesheets_["Light"] = "";
    theme_stylesheets_["Dark"] =
        "QWidget { background-color: #2b2b2b; color: #ffffff; }"
        "QGroupBox { border: 1px solid #555555; }"
        "QPushButton { background-color: #404040; border: 1px solid #555555; }"
        "QLineEdit { background-color: #404040; border: 1px solid #555555; }";
    theme_stylesheets_["Blue"] =
        "QWidget { background-color: #ecf0f1; }"
        "QPushButton { background-color: #3498db; color: white; border: none; }"
        "QLineEdit { border: 2px solid #3498db; }";
    theme_stylesheets_["Green"] =
        "QWidget { background-color: #ecf0f1; }"
        "QPushButton { background-color: #27ae60; color: white; border: none; }"
        "QLineEdit { border: 2px solid #27ae60; }";
    theme_stylesheets_["Purple"] =
        "QWidget { background-color: #ecf0f1; }"
        "QPushButton { background-color: #9b59b6; color: white; border: none; }"
        "QLineEdit { border: 2px solid #9b59b6; }";
    theme_stylesheets_["Custom"] = theme_stylesheets_["Light"];
}

void ThemeManager::onThemeSelected() {
    current_theme_ = theme_combo_->currentText();
    qDebug() << "Theme selected:" << current_theme_;
}

void ThemeManager::onCustomColorChanged() {
    auto button = qobject_cast<QPushButton*>(sender());
    if (!button)
        return;

    QColor color = QColorDialog::getColor(Qt::blue, this, "Select Color");
    if (color.isValid()) {
        button->setStyleSheet(
            QString("QPushButton { background-color: %1; color: white; }")
                .arg(color.name()));
        qDebug() << "Custom color changed:" << color.name();
    }
}

void ThemeManager::onPreviewTheme() {
    applyThemeToPreview(current_theme_);
    qDebug() << "Previewing theme:" << current_theme_;
}

void ThemeManager::onApplyTheme() {
    emit themeChanged(current_theme_);
    qDebug() << "Applied theme:" << current_theme_;
}

void ThemeManager::onResetTheme() {
    theme_combo_->setCurrentText("Light");
    current_theme_ = "Light";
    applyThemeToPreview("Light");
    emit themeChanged("Light");
    qDebug() << "Theme reset to Light";
}

void ThemeManager::applyThemeToPreview(const QString& theme_name) {
    QString stylesheet = theme_stylesheets_.value(theme_name, "");
    preview_widget_->setStyleSheet(stylesheet);
}

#include "ThemeManager.moc"
