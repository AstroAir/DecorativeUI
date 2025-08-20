#include "SpecializedCommands.hpp"
#include <QButtonGroup>
#include <QDebug>
#include <QIcon>

namespace DeclarativeUI::Command::UI {

// **RadioButtonCommand implementation**
RadioButtonCommand::RadioButtonCommand(QObject* parent)
    : ButtonCommand(parent) {
    // Override default properties for radio button
    getState()->setProperty("text", QString("Radio Button"));
    getState()->setProperty("checkable",
                            true);  // Radio buttons are always checkable
    getState()->setProperty("autoExclusive", true);
    getState()->setProperty("buttonGroup", QString());

    qDebug() << "🔘 RadioButtonCommand created";
}

UICommandMetadata RadioButtonCommand::getMetadata() const {
    UICommandMetadata metadata(
        "RadioButton", "QRadioButton", "Radio Button",
        "A radio button control for exclusive selection");
    metadata.supported_events = {"clicked", "toggled"};
    metadata.default_properties = {{"text", "Radio Button"},
                                   {"enabled", true},
                                   {"checkable", true},
                                   {"checked", false},
                                   {"autoExclusive", true}};
    return metadata;
}

RadioButtonCommand& RadioButtonCommand::setAutoExclusive(bool autoExclusive) {
    getState()->setProperty("autoExclusive", autoExclusive);
    return *this;
}

bool RadioButtonCommand::isAutoExclusive() const {
    return getState()->getProperty<bool>("autoExclusive", true);
}

RadioButtonCommand& RadioButtonCommand::setButtonGroup(
    const QString& groupName) {
    getState()->setProperty("buttonGroup", groupName);
    return *this;
}

QString RadioButtonCommand::getButtonGroup() const {
    return getState()->getProperty<QString>("buttonGroup");
}

void RadioButtonCommand::onWidgetCreated(QWidget* widget) {
    ButtonCommand::onWidgetCreated(widget);
    setupRadioButtonBehavior();
}

void RadioButtonCommand::syncToWidget() {
    ButtonCommand::syncToWidget();

    if (auto* radioButton = qobject_cast<QRadioButton*>(getWidget())) {
        radioButton->setAutoExclusive(isAutoExclusive());
        setupRadioButtonBehavior();
    }
}

void RadioButtonCommand::syncFromWidget() {
    ButtonCommand::syncFromWidget();

    if (auto* radioButton = qobject_cast<QRadioButton*>(getWidget())) {
        getState()->setProperty("autoExclusive", radioButton->autoExclusive());
    }
}

void RadioButtonCommand::setupRadioButtonBehavior() {
    auto* radioButton = qobject_cast<QRadioButton*>(getWidget());
    if (!radioButton) {
        return;
    }

    // Handle button group if specified
    QString groupName = getButtonGroup();
    if (!groupName.isEmpty()) {
        // In a real implementation, you would manage button groups
        // For now, just log the group assignment
        qDebug() << "📻 Radio button assigned to group:" << groupName;
    }
}

// **CheckBoxCommand implementation**
CheckBoxCommand::CheckBoxCommand(QObject* parent) : ButtonCommand(parent) {
    // Override default properties for checkbox
    getState()->setProperty("text", QString("Check Box"));
    getState()->setProperty("checkable",
                            true);  // Checkboxes are always checkable
    getState()->setProperty("tristate", false);
    getState()->setProperty("checkState", static_cast<int>(Qt::Unchecked));

    qDebug() << "☑️ CheckBoxCommand created";
}

UICommandMetadata CheckBoxCommand::getMetadata() const {
    UICommandMetadata metadata("CheckBox", "QCheckBox", "Check Box",
                               "A checkbox control for multiple selection");
    metadata.supported_events = {"clicked", "toggled", "stateChanged"};
    metadata.default_properties = {
        {"text", "Check Box"}, {"enabled", true},
        {"checkable", true},   {"checked", false},
        {"tristate", false},   {"checkState", static_cast<int>(Qt::Unchecked)}};
    return metadata;
}

CheckBoxCommand& CheckBoxCommand::setTristate(bool tristate) {
    getState()->setProperty("tristate", tristate);
    return *this;
}

bool CheckBoxCommand::isTristate() const {
    return getState()->getProperty<bool>("tristate", false);
}

CheckBoxCommand& CheckBoxCommand::setCheckState(int state) {
    getState()->setProperty("checkState", state);
    // Also update the basic checked property for compatibility
    getState()->setProperty("checked", state == Qt::Checked);
    return *this;
}

int CheckBoxCommand::getCheckState() const {
    return getState()->getProperty<int>("checkState",
                                        static_cast<int>(Qt::Unchecked));
}

CheckBoxCommand& CheckBoxCommand::onStateChanged(
    std::function<void(int)> handler) {
    state_changed_handler_ = handler;
    return *this;
}

void CheckBoxCommand::onWidgetCreated(QWidget* widget) {
    ButtonCommand::onWidgetCreated(widget);

    if (auto* checkBox = qobject_cast<QCheckBox*>(widget)) {
        // Connect checkbox-specific signals
        connect(checkBox, &QCheckBox::checkStateChanged, this,
                [this](Qt::CheckState state) {
                    getState()->setProperty("checkState",
                                            static_cast<int>(state));
                    getState()->setProperty("checked", state == Qt::Checked);
                    emit stateChanged(static_cast<int>(state));
                    handleEvent("stateChanged", static_cast<int>(state));
                });
    }
}

void CheckBoxCommand::syncToWidget() {
    ButtonCommand::syncToWidget();

    if (auto* checkBox = qobject_cast<QCheckBox*>(getWidget())) {
        checkBox->setTristate(isTristate());
        checkBox->setCheckState(static_cast<Qt::CheckState>(getCheckState()));
    }
}

void CheckBoxCommand::syncFromWidget() {
    ButtonCommand::syncFromWidget();

    if (auto* checkBox = qobject_cast<QCheckBox*>(getWidget())) {
        getState()->setProperty("tristate", checkBox->isTristate());
        getState()->setProperty("checkState",
                                static_cast<int>(checkBox->checkState()));
    }
}

void CheckBoxCommand::handleEvent(const QString& eventType,
                                  const QVariant& eventData) {
    if (eventType == "stateChanged" && state_changed_handler_) {
        state_changed_handler_(eventData.toInt());
    }

    ButtonCommand::handleEvent(eventType, eventData);
}

// **ToggleButtonCommand implementation**
ToggleButtonCommand::ToggleButtonCommand(QObject* parent)
    : ButtonCommand(parent) {
    // Override default properties for toggle button
    getState()->setProperty("text", QString("Toggle"));
    getState()->setProperty("checkable",
                            true);  // Toggle buttons are always checkable
    getState()->setProperty("checkedText", QString("On"));
    getState()->setProperty("uncheckedText", QString("Off"));
    getState()->setProperty("checkedIcon", QString());
    getState()->setProperty("uncheckedIcon", QString());

    qDebug() << "🔄 ToggleButtonCommand created";
}

UICommandMetadata ToggleButtonCommand::getMetadata() const {
    UICommandMetadata metadata("ToggleButton", "QPushButton", "Toggle Button",
                               "A button that toggles between two states");
    metadata.supported_events = {"clicked", "toggled"};
    metadata.default_properties = {
        {"text", "Toggle"}, {"enabled", true},     {"checkable", true},
        {"checked", false}, {"checkedText", "On"}, {"uncheckedText", "Off"}};
    return metadata;
}

ToggleButtonCommand& ToggleButtonCommand::setToggleText(
    const QString& checkedText, const QString& uncheckedText) {
    getState()->setProperty("checkedText", checkedText);
    getState()->setProperty("uncheckedText", uncheckedText);
    updateToggleAppearance();
    return *this;
}

QString ToggleButtonCommand::getCheckedText() const {
    return getState()->getProperty<QString>("checkedText", "On");
}

QString ToggleButtonCommand::getUncheckedText() const {
    return getState()->getProperty<QString>("uncheckedText", "Off");
}

ToggleButtonCommand& ToggleButtonCommand::setToggleIcon(
    const QString& checkedIcon, const QString& uncheckedIcon) {
    getState()->setProperty("checkedIcon", checkedIcon);
    getState()->setProperty("uncheckedIcon", uncheckedIcon);
    updateToggleAppearance();
    return *this;
}

QString ToggleButtonCommand::getCheckedIcon() const {
    return getState()->getProperty<QString>("checkedIcon");
}

QString ToggleButtonCommand::getUncheckedIcon() const {
    return getState()->getProperty<QString>("uncheckedIcon");
}

void ToggleButtonCommand::onWidgetCreated(QWidget* widget) {
    ButtonCommand::onWidgetCreated(widget);
    updateToggleAppearance();
}

void ToggleButtonCommand::syncToWidget() {
    ButtonCommand::syncToWidget();
    updateToggleAppearance();
}

void ToggleButtonCommand::handleEvent(const QString& eventType,
                                      const QVariant& eventData) {
    if (eventType == "toggled") {
        updateToggleAppearance();
    }

    ButtonCommand::handleEvent(eventType, eventData);
}

void ToggleButtonCommand::updateToggleAppearance() {
    auto* button = qobject_cast<QPushButton*>(getWidget());
    if (!button) {
        return;
    }

    bool checked = isChecked();

    // Update text based on state
    QString text = checked ? getCheckedText() : getUncheckedText();
    button->setText(text);
    getState()->setProperty("text", text);

    // Update icon based on state
    QString iconPath = checked ? getCheckedIcon() : getUncheckedIcon();
    if (!iconPath.isEmpty()) {
        button->setIcon(QIcon(iconPath));
        getState()->setProperty("icon", iconPath);
    }
}

// **ToolButtonCommand implementation**
ToolButtonCommand::ToolButtonCommand(QObject* parent) : ButtonCommand(parent) {
    // Override default properties for tool button
    getState()->setProperty("text", QString("Tool"));
    getState()->setProperty("toolButtonStyle",
                            static_cast<int>(Qt::ToolButtonIconOnly));
    getState()->setProperty("arrowType", static_cast<int>(Qt::NoArrow));
    getState()->setProperty("popupMode",
                            static_cast<int>(QToolButton::DelayedPopup));
    getState()->setProperty("autoRaise", true);

    qDebug() << "🔧 ToolButtonCommand created";
}

UICommandMetadata ToolButtonCommand::getMetadata() const {
    UICommandMetadata metadata("ToolButton", "QToolButton", "Tool Button",
                               "A tool button control for toolbars");
    metadata.supported_events = {"clicked", "toggled"};
    metadata.default_properties = {
        {"text", "Tool"},
        {"enabled", true},
        {"checkable", false},
        {"checked", false},
        {"toolButtonStyle", static_cast<int>(Qt::ToolButtonIconOnly)},
        {"autoRaise", true}};
    return metadata;
}

ToolButtonCommand& ToolButtonCommand::setToolButtonStyle(int style) {
    getState()->setProperty("toolButtonStyle", style);
    return *this;
}

int ToolButtonCommand::getToolButtonStyle() const {
    return getState()->getProperty<int>(
        "toolButtonStyle", static_cast<int>(Qt::ToolButtonIconOnly));
}

ToolButtonCommand& ToolButtonCommand::setArrowType(int arrowType) {
    getState()->setProperty("arrowType", arrowType);
    return *this;
}

int ToolButtonCommand::getArrowType() const {
    return getState()->getProperty<int>("arrowType",
                                        static_cast<int>(Qt::NoArrow));
}

ToolButtonCommand& ToolButtonCommand::setPopupMode(int mode) {
    getState()->setProperty("popupMode", mode);
    return *this;
}

int ToolButtonCommand::getPopupMode() const {
    return getState()->getProperty<int>(
        "popupMode", static_cast<int>(QToolButton::DelayedPopup));
}

ToolButtonCommand& ToolButtonCommand::setAutoRaise(bool autoRaise) {
    getState()->setProperty("autoRaise", autoRaise);
    return *this;
}

bool ToolButtonCommand::isAutoRaise() const {
    return getState()->getProperty<bool>("autoRaise", true);
}

void ToolButtonCommand::onWidgetCreated(QWidget* widget) {
    ButtonCommand::onWidgetCreated(widget);
}

void ToolButtonCommand::syncToWidget() {
    ButtonCommand::syncToWidget();

    if (auto* toolButton = qobject_cast<QToolButton*>(getWidget())) {
        toolButton->setToolButtonStyle(
            static_cast<Qt::ToolButtonStyle>(getToolButtonStyle()));
        toolButton->setArrowType(static_cast<Qt::ArrowType>(getArrowType()));
        toolButton->setPopupMode(
            static_cast<QToolButton::ToolButtonPopupMode>(getPopupMode()));
        toolButton->setAutoRaise(isAutoRaise());
    }
}

void ToolButtonCommand::syncFromWidget() {
    ButtonCommand::syncFromWidget();

    if (auto* toolButton = qobject_cast<QToolButton*>(getWidget())) {
        getState()->setProperty(
            "toolButtonStyle", static_cast<int>(toolButton->toolButtonStyle()));
        getState()->setProperty("arrowType",
                                static_cast<int>(toolButton->arrowType()));
        getState()->setProperty("popupMode",
                                static_cast<int>(toolButton->popupMode()));
        getState()->setProperty("autoRaise", toolButton->autoRaise());
    }
}

// **SpinBoxCommand implementation**
SpinBoxCommand::SpinBoxCommand(QObject* parent) : BaseUICommand(parent) {
    // Set up default properties
    getState()->setProperty("value", 0);
    getState()->setProperty("minimum", 0);
    getState()->setProperty("maximum", 99);
    getState()->setProperty("singleStep", 1);
    getState()->setProperty("prefix", QString());
    getState()->setProperty("suffix", QString());

    qDebug() << "🔢 SpinBoxCommand created";
}

UICommandMetadata SpinBoxCommand::getMetadata() const {
    UICommandMetadata metadata("SpinBox", "QSpinBox", "Spin Box",
                               "A numeric input control with spin buttons");
    metadata.supported_events = {"valueChanged"};
    metadata.default_properties = {
        {"value", 0}, {"minimum", 0}, {"maximum", 99}, {"singleStep", 1}};
    return metadata;
}

SpinBoxCommand& SpinBoxCommand::setValue(int value) {
    getState()->setProperty("value", value);
    return *this;
}

int SpinBoxCommand::getValue() const {
    return getState()->getProperty<int>("value", 0);
}

SpinBoxCommand& SpinBoxCommand::setMinimum(int minimum) {
    getState()->setProperty("minimum", minimum);
    return *this;
}

int SpinBoxCommand::getMinimum() const {
    return getState()->getProperty<int>("minimum", 0);
}

SpinBoxCommand& SpinBoxCommand::setMaximum(int maximum) {
    getState()->setProperty("maximum", maximum);
    return *this;
}

int SpinBoxCommand::getMaximum() const {
    return getState()->getProperty<int>("maximum", 99);
}

SpinBoxCommand& SpinBoxCommand::setRange(int minimum, int maximum) {
    setMinimum(minimum);
    setMaximum(maximum);
    return *this;
}

SpinBoxCommand& SpinBoxCommand::setSingleStep(int step) {
    getState()->setProperty("singleStep", step);
    return *this;
}

int SpinBoxCommand::getSingleStep() const {
    return getState()->getProperty<int>("singleStep", 1);
}

SpinBoxCommand& SpinBoxCommand::setPrefix(const QString& prefix) {
    getState()->setProperty("prefix", prefix);
    return *this;
}

QString SpinBoxCommand::getPrefix() const {
    return getState()->getProperty<QString>("prefix");
}

SpinBoxCommand& SpinBoxCommand::setSuffix(const QString& suffix) {
    getState()->setProperty("suffix", suffix);
    return *this;
}

QString SpinBoxCommand::getSuffix() const {
    return getState()->getProperty<QString>("suffix");
}

SpinBoxCommand& SpinBoxCommand::onValueChanged(
    std::function<void(int)> handler) {
    value_changed_handler_ = handler;
    return *this;
}

void SpinBoxCommand::onWidgetCreated(QWidget* widget) {
    BaseUICommand::onWidgetCreated(widget);

    if (auto* spinBox = qobject_cast<QSpinBox*>(widget)) {
        // Connect spinbox signals to command events
        connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this,
                [this](int value) {
                    getState()->setProperty("value", value);
                    emit valueChanged(value);
                    handleEvent("valueChanged", value);
                });
    }
}

void SpinBoxCommand::syncToWidget() {
    BaseUICommand::syncToWidget();

    if (auto* spinBox = qobject_cast<QSpinBox*>(getWidget())) {
        spinBox->setMinimum(getMinimum());
        spinBox->setMaximum(getMaximum());
        spinBox->setSingleStep(getSingleStep());
        spinBox->setValue(getValue());
        spinBox->setPrefix(getPrefix());
        spinBox->setSuffix(getSuffix());
    }
}

void SpinBoxCommand::syncFromWidget() {
    if (auto* spinBox = qobject_cast<QSpinBox*>(getWidget())) {
        getState()->setProperty("value", spinBox->value());
        getState()->setProperty("minimum", spinBox->minimum());
        getState()->setProperty("maximum", spinBox->maximum());
        getState()->setProperty("singleStep", spinBox->singleStep());
        getState()->setProperty("prefix", spinBox->prefix());
        getState()->setProperty("suffix", spinBox->suffix());
    }
}

void SpinBoxCommand::handleEvent(const QString& eventType,
                                 const QVariant& eventData) {
    if (eventType == "valueChanged" && value_changed_handler_) {
        value_changed_handler_(eventData.toInt());
    }

    BaseUICommand::handleEvent(eventType, eventData);
}

// **SliderCommand implementation**
SliderCommand::SliderCommand(QObject* parent) : BaseUICommand(parent) {
    // Set up default properties
    getState()->setProperty("value", 0);
    getState()->setProperty("minimum", 0);
    getState()->setProperty("maximum", 99);
    getState()->setProperty("orientation", static_cast<int>(Qt::Horizontal));
    getState()->setProperty("tickPosition", static_cast<int>(QSlider::NoTicks));
    getState()->setProperty("tickInterval", 0);

    qDebug() << "🎚️ SliderCommand created";
}

UICommandMetadata SliderCommand::getMetadata() const {
    UICommandMetadata metadata("Slider", "QSlider", "Slider",
                               "A slider control for value selection");
    metadata.supported_events = {"valueChanged", "sliderPressed",
                                 "sliderReleased"};
    metadata.default_properties = {
        {"value", 0},
        {"minimum", 0},
        {"maximum", 99},
        {"orientation", static_cast<int>(Qt::Horizontal)}};
    return metadata;
}

SliderCommand& SliderCommand::setValue(int value) {
    getState()->setProperty("value", value);
    return *this;
}

int SliderCommand::getValue() const {
    return getState()->getProperty<int>("value", 0);
}

SliderCommand& SliderCommand::setMinimum(int minimum) {
    getState()->setProperty("minimum", minimum);
    return *this;
}

int SliderCommand::getMinimum() const {
    return getState()->getProperty<int>("minimum", 0);
}

SliderCommand& SliderCommand::setMaximum(int maximum) {
    getState()->setProperty("maximum", maximum);
    return *this;
}

int SliderCommand::getMaximum() const {
    return getState()->getProperty<int>("maximum", 99);
}

SliderCommand& SliderCommand::setRange(int minimum, int maximum) {
    setMinimum(minimum);
    setMaximum(maximum);
    return *this;
}

SliderCommand& SliderCommand::setOrientation(int orientation) {
    getState()->setProperty("orientation", orientation);
    return *this;
}

int SliderCommand::getOrientation() const {
    return getState()->getProperty<int>("orientation",
                                        static_cast<int>(Qt::Horizontal));
}

SliderCommand& SliderCommand::setTickPosition(int position) {
    getState()->setProperty("tickPosition", position);
    return *this;
}

int SliderCommand::getTickPosition() const {
    return getState()->getProperty<int>("tickPosition",
                                        static_cast<int>(QSlider::NoTicks));
}

SliderCommand& SliderCommand::setTickInterval(int interval) {
    getState()->setProperty("tickInterval", interval);
    return *this;
}

int SliderCommand::getTickInterval() const {
    return getState()->getProperty<int>("tickInterval", 0);
}

SliderCommand& SliderCommand::onValueChanged(std::function<void(int)> handler) {
    value_changed_handler_ = handler;
    return *this;
}

SliderCommand& SliderCommand::onSliderPressed(std::function<void()> handler) {
    slider_pressed_handler_ = handler;
    return *this;
}

SliderCommand& SliderCommand::onSliderReleased(std::function<void()> handler) {
    slider_released_handler_ = handler;
    return *this;
}

void SliderCommand::onWidgetCreated(QWidget* widget) {
    BaseUICommand::onWidgetCreated(widget);

    if (auto* slider = qobject_cast<QSlider*>(widget)) {
        // Connect slider signals to command events
        connect(slider, &QSlider::valueChanged, this, [this](int value) {
            getState()->setProperty("value", value);
            emit valueChanged(value);
            handleEvent("valueChanged", value);
        });

        connect(slider, &QSlider::sliderPressed, this, [this]() {
            emit sliderPressed();
            handleEvent("sliderPressed");
        });

        connect(slider, &QSlider::sliderReleased, this, [this]() {
            emit sliderReleased();
            handleEvent("sliderReleased");
        });
    }
}

void SliderCommand::syncToWidget() {
    BaseUICommand::syncToWidget();

    if (auto* slider = qobject_cast<QSlider*>(getWidget())) {
        slider->setMinimum(getMinimum());
        slider->setMaximum(getMaximum());
        slider->setValue(getValue());
        slider->setOrientation(static_cast<Qt::Orientation>(getOrientation()));
        slider->setTickPosition(
            static_cast<QSlider::TickPosition>(getTickPosition()));
        slider->setTickInterval(getTickInterval());
    }
}

void SliderCommand::syncFromWidget() {
    if (auto* slider = qobject_cast<QSlider*>(getWidget())) {
        getState()->setProperty("value", slider->value());
        getState()->setProperty("minimum", slider->minimum());
        getState()->setProperty("maximum", slider->maximum());
        getState()->setProperty("orientation",
                                static_cast<int>(slider->orientation()));
        getState()->setProperty("tickPosition",
                                static_cast<int>(slider->tickPosition()));
        getState()->setProperty("tickInterval", slider->tickInterval());
    }
}

void SliderCommand::handleEvent(const QString& eventType,
                                const QVariant& eventData) {
    if (eventType == "valueChanged" && value_changed_handler_) {
        value_changed_handler_(eventData.toInt());
    } else if (eventType == "sliderPressed" && slider_pressed_handler_) {
        slider_pressed_handler_();
    } else if (eventType == "sliderReleased" && slider_released_handler_) {
        slider_released_handler_();
    }

    BaseUICommand::handleEvent(eventType, eventData);
}

// **ProgressBarCommand implementation**
ProgressBarCommand::ProgressBarCommand(QObject* parent)
    : BaseUICommand(parent) {
    // Set up default properties
    getState()->setProperty("value", 0);
    getState()->setProperty("minimum", 0);
    getState()->setProperty("maximum", 100);
    getState()->setProperty("format", QString("%p%"));
    getState()->setProperty("textVisible", true);
    getState()->setProperty("orientation", static_cast<int>(Qt::Horizontal));

    qDebug() << "📊 ProgressBarCommand created";
}

UICommandMetadata ProgressBarCommand::getMetadata() const {
    UICommandMetadata metadata("ProgressBar", "QProgressBar", "Progress Bar",
                               "A progress indicator control");
    metadata.default_properties = {
        {"value", 0},
        {"minimum", 0},
        {"maximum", 100},
        {"format", "%p%"},
        {"textVisible", true},
        {"orientation", static_cast<int>(Qt::Horizontal)}};
    return metadata;
}

ProgressBarCommand& ProgressBarCommand::setValue(int value) {
    getState()->setProperty("value", value);
    return *this;
}

int ProgressBarCommand::getValue() const {
    return getState()->getProperty<int>("value", 0);
}

ProgressBarCommand& ProgressBarCommand::setMinimum(int minimum) {
    getState()->setProperty("minimum", minimum);
    return *this;
}

int ProgressBarCommand::getMinimum() const {
    return getState()->getProperty<int>("minimum", 0);
}

ProgressBarCommand& ProgressBarCommand::setMaximum(int maximum) {
    getState()->setProperty("maximum", maximum);
    return *this;
}

int ProgressBarCommand::getMaximum() const {
    return getState()->getProperty<int>("maximum", 100);
}

ProgressBarCommand& ProgressBarCommand::setRange(int minimum, int maximum) {
    setMinimum(minimum);
    setMaximum(maximum);
    return *this;
}

ProgressBarCommand& ProgressBarCommand::setFormat(const QString& format) {
    getState()->setProperty("format", format);
    return *this;
}

QString ProgressBarCommand::getFormat() const {
    return getState()->getProperty<QString>("format", "%p%");
}

ProgressBarCommand& ProgressBarCommand::setTextVisible(bool visible) {
    getState()->setProperty("textVisible", visible);
    return *this;
}

bool ProgressBarCommand::isTextVisible() const {
    return getState()->getProperty<bool>("textVisible", true);
}

ProgressBarCommand& ProgressBarCommand::setOrientation(int orientation) {
    getState()->setProperty("orientation", orientation);
    return *this;
}

int ProgressBarCommand::getOrientation() const {
    return getState()->getProperty<int>("orientation",
                                        static_cast<int>(Qt::Horizontal));
}

void ProgressBarCommand::onWidgetCreated(QWidget* widget) {
    BaseUICommand::onWidgetCreated(widget);
    // Progress bars typically don't have interactive events
}

void ProgressBarCommand::syncToWidget() {
    BaseUICommand::syncToWidget();

    if (auto* progressBar = qobject_cast<QProgressBar*>(getWidget())) {
        progressBar->setMinimum(getMinimum());
        progressBar->setMaximum(getMaximum());
        progressBar->setValue(getValue());
        progressBar->setFormat(getFormat());
        progressBar->setTextVisible(isTextVisible());
        progressBar->setOrientation(
            static_cast<Qt::Orientation>(getOrientation()));
    }
}

// **Specialized command registration**
void registerSpecializedCommands() {
    auto& factory = UICommandFactory::instance();
    auto& mapper = WidgetMapper::instance();

    qDebug() << "🔧 Registering specialized UI commands";

    // Register RadioButtonCommand
    factory.registerCommand<RadioButtonCommand>("RadioButton", "QRadioButton");
    mapper.registerMapping<QRadioButton>("RadioButton");

    // Register CheckBoxCommand
    factory.registerCommand<CheckBoxCommand>("CheckBox", "QCheckBox");
    mapper.registerMapping<QCheckBox>("CheckBox");

    // Register ToggleButtonCommand
    factory.registerCommand<ToggleButtonCommand>("ToggleButton", "QPushButton");
    // Uses same widget mapping as Button

    // Register ToolButtonCommand
    factory.registerCommand<ToolButtonCommand>("ToolButton", "QToolButton");
    mapper.registerMapping<QToolButton>("ToolButton");

    // Register SpinBoxCommand
    factory.registerCommand<SpinBoxCommand>("SpinBox", "QSpinBox");
    mapper.registerMapping<QSpinBox>("SpinBox");

    // Register SliderCommand
    factory.registerCommand<SliderCommand>("Slider", "QSlider");
    mapper.registerMapping<QSlider>("Slider");

    // Register ProgressBarCommand
    factory.registerCommand<ProgressBarCommand>("ProgressBar", "QProgressBar");
    mapper.registerMapping<QProgressBar>("ProgressBar");

    qDebug() << "✅ Specialized UI commands registered successfully";
}

}  // namespace DeclarativeUI::Command::UI
