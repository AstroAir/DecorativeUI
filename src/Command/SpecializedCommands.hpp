#pragma once

#include <QCheckBox>
#include <QProgressBar>
#include <QRadioButton>
#include <QSlider>
#include <QSpinBox>
#include <QToolButton>
#include "CoreCommands.hpp"

namespace DeclarativeUI::Command::UI {

// **Radio Button Command - inherits from ButtonCommand**
class RadioButtonCommand : public ButtonCommand {
    Q_OBJECT

public:
    explicit RadioButtonCommand(QObject* parent = nullptr);
    virtual ~RadioButtonCommand() = default;

    // **Override base command interface**
    UICommandMetadata getMetadata() const override;
    QString getCommandType() const override { return "RadioButton"; }
    QString getWidgetType() const override { return "QRadioButton"; }

    // **Radio button specific interface**
    RadioButtonCommand& setAutoExclusive(bool autoExclusive);
    bool isAutoExclusive() const;

    RadioButtonCommand& setButtonGroup(const QString& groupName);
    QString getButtonGroup() const;

    // **Override widget lifecycle for radio button specific behavior**
    void onWidgetCreated(QWidget* widget) override;
    void syncToWidget() override;
    void syncFromWidget() override;

private:
    void setupRadioButtonBehavior();
};

// **Check Box Command - inherits from ButtonCommand**
class CheckBoxCommand : public ButtonCommand {
    Q_OBJECT

public:
    explicit CheckBoxCommand(QObject* parent = nullptr);
    virtual ~CheckBoxCommand() = default;

    // **Override base command interface**
    UICommandMetadata getMetadata() const override;
    QString getCommandType() const override { return "CheckBox"; }
    QString getWidgetType() const override { return "QCheckBox"; }

    // **Checkbox specific interface**
    CheckBoxCommand& setTristate(bool tristate);
    bool isTristate() const;

    CheckBoxCommand& setCheckState(
        int state);  // Qt::Unchecked, Qt::PartiallyChecked, Qt::Checked
    int getCheckState() const;

    // **Event handling for tristate**
    CheckBoxCommand& onStateChanged(std::function<void(int)> handler);

    // **Override widget lifecycle for checkbox specific behavior**
    void onWidgetCreated(QWidget* widget) override;
    void syncToWidget() override;
    void syncFromWidget() override;

signals:
    void stateChanged(int state);

protected:
    void handleEvent(const QString& eventType,
                     const QVariant& eventData = QVariant{}) override;

private:
    std::function<void(int)> state_changed_handler_;
};

// **Toggle Button Command - inherits from ButtonCommand**
class ToggleButtonCommand : public ButtonCommand {
    Q_OBJECT

public:
    explicit ToggleButtonCommand(QObject* parent = nullptr);
    virtual ~ToggleButtonCommand() = default;

    // **Override base command interface**
    UICommandMetadata getMetadata() const override;
    QString getCommandType() const override { return "ToggleButton"; }
    QString getWidgetType() const override {
        return "QPushButton";
    }  // Uses QPushButton in checkable mode

    // **Toggle button specific interface**
    ToggleButtonCommand& setToggleText(const QString& checkedText,
                                       const QString& uncheckedText);
    QString getCheckedText() const;
    QString getUncheckedText() const;

    ToggleButtonCommand& setToggleIcon(const QString& checkedIcon,
                                       const QString& uncheckedIcon);
    QString getCheckedIcon() const;
    QString getUncheckedIcon() const;

    // **Override widget lifecycle for toggle behavior**
    void onWidgetCreated(QWidget* widget) override;
    void syncToWidget() override;

protected:
    void handleEvent(const QString& eventType,
                     const QVariant& eventData = QVariant{}) override;

private:
    void updateToggleAppearance();
};

// **Tool Button Command - inherits from ButtonCommand**
class ToolButtonCommand : public ButtonCommand {
    Q_OBJECT

public:
    explicit ToolButtonCommand(QObject* parent = nullptr);
    virtual ~ToolButtonCommand() = default;

    // **Override base command interface**
    UICommandMetadata getMetadata() const override;
    QString getCommandType() const override { return "ToolButton"; }
    QString getWidgetType() const override { return "QToolButton"; }

    // **Tool button specific interface**
    ToolButtonCommand& setToolButtonStyle(int style);  // Qt::ToolButtonStyle
    int getToolButtonStyle() const;

    ToolButtonCommand& setArrowType(int arrowType);  // Qt::ArrowType
    int getArrowType() const;

    ToolButtonCommand& setPopupMode(
        int mode);  // QToolButton::ToolButtonPopupMode
    int getPopupMode() const;

    ToolButtonCommand& setAutoRaise(bool autoRaise);
    bool isAutoRaise() const;

    // **Override widget lifecycle for tool button specific behavior**
    void onWidgetCreated(QWidget* widget) override;
    void syncToWidget() override;
    void syncFromWidget() override;
};

// **Spin Box Command - inherits from BaseUICommand (numeric input)**
class SpinBoxCommand : public BaseUICommand {
    Q_OBJECT

public:
    explicit SpinBoxCommand(QObject* parent = nullptr);
    virtual ~SpinBoxCommand() = default;

    // **BaseUICommand interface**
    UICommandMetadata getMetadata() const override;
    QString getCommandType() const override { return "SpinBox"; }
    QString getWidgetType() const override { return "QSpinBox"; }

    // **SpinBox specific interface**
    SpinBoxCommand& setValue(int value);
    int getValue() const;

    SpinBoxCommand& setMinimum(int minimum);
    int getMinimum() const;

    SpinBoxCommand& setMaximum(int maximum);
    int getMaximum() const;

    SpinBoxCommand& setRange(int minimum, int maximum);

    SpinBoxCommand& setSingleStep(int step);
    int getSingleStep() const;

    SpinBoxCommand& setPrefix(const QString& prefix);
    QString getPrefix() const;

    SpinBoxCommand& setSuffix(const QString& suffix);
    QString getSuffix() const;

    // **Event handling**
    SpinBoxCommand& onValueChanged(std::function<void(int)> handler);

    // **Widget lifecycle overrides**
    void onWidgetCreated(QWidget* widget) override;
    void syncToWidget() override;
    void syncFromWidget() override;

signals:
    void valueChanged(int value);

protected:
    void handleEvent(const QString& eventType,
                     const QVariant& eventData = QVariant{}) override;

private:
    std::function<void(int)> value_changed_handler_;
};

// **Slider Command - inherits from BaseUICommand**
class SliderCommand : public BaseUICommand {
    Q_OBJECT

public:
    explicit SliderCommand(QObject* parent = nullptr);
    virtual ~SliderCommand() = default;

    // **BaseUICommand interface**
    UICommandMetadata getMetadata() const override;
    QString getCommandType() const override { return "Slider"; }
    QString getWidgetType() const override { return "QSlider"; }

    // **Slider specific interface**
    SliderCommand& setValue(int value);
    int getValue() const;

    SliderCommand& setMinimum(int minimum);
    int getMinimum() const;

    SliderCommand& setMaximum(int maximum);
    int getMaximum() const;

    SliderCommand& setRange(int minimum, int maximum);

    SliderCommand& setOrientation(int orientation);  // Qt::Orientation
    int getOrientation() const;

    SliderCommand& setTickPosition(int position);  // QSlider::TickPosition
    int getTickPosition() const;

    SliderCommand& setTickInterval(int interval);
    int getTickInterval() const;

    // **Event handling**
    SliderCommand& onValueChanged(std::function<void(int)> handler);
    SliderCommand& onSliderPressed(std::function<void()> handler);
    SliderCommand& onSliderReleased(std::function<void()> handler);

    // **Widget lifecycle overrides**
    void onWidgetCreated(QWidget* widget) override;
    void syncToWidget() override;
    void syncFromWidget() override;

signals:
    void valueChanged(int value);
    void sliderPressed();
    void sliderReleased();

protected:
    void handleEvent(const QString& eventType,
                     const QVariant& eventData = QVariant{}) override;

private:
    std::function<void(int)> value_changed_handler_;
    std::function<void()> slider_pressed_handler_;
    std::function<void()> slider_released_handler_;
};

// **Progress Bar Command - inherits from BaseUICommand (display only)**
class ProgressBarCommand : public BaseUICommand {
    Q_OBJECT

public:
    explicit ProgressBarCommand(QObject* parent = nullptr);
    virtual ~ProgressBarCommand() = default;

    // **BaseUICommand interface**
    UICommandMetadata getMetadata() const override;
    QString getCommandType() const override { return "ProgressBar"; }
    QString getWidgetType() const override { return "QProgressBar"; }

    // **ProgressBar specific interface**
    ProgressBarCommand& setValue(int value);
    int getValue() const;

    ProgressBarCommand& setMinimum(int minimum);
    int getMinimum() const;

    ProgressBarCommand& setMaximum(int maximum);
    int getMaximum() const;

    ProgressBarCommand& setRange(int minimum, int maximum);

    ProgressBarCommand& setFormat(const QString& format);
    QString getFormat() const;

    ProgressBarCommand& setTextVisible(bool visible);
    bool isTextVisible() const;

    ProgressBarCommand& setOrientation(int orientation);  // Qt::Orientation
    int getOrientation() const;

    // **Widget lifecycle overrides**
    void onWidgetCreated(QWidget* widget) override;
    void syncToWidget() override;
};

// **Registration function for specialized commands**
void registerSpecializedCommands();

}  // namespace DeclarativeUI::Command::UI
