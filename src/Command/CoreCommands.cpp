#include "CoreCommands.hpp"
#include <QDebug>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QVBoxLayout>

namespace DeclarativeUI::Command::UI {

// **ButtonCommand implementation**
ButtonCommand::ButtonCommand(QObject* parent) : BaseUICommand(parent) {
    // Set up default properties
    getState()->setProperty("text", QString("Button"));
    getState()->setProperty("enabled", true);
    getState()->setProperty("checkable", false);
    getState()->setProperty("checked", false);
    getState()->setProperty("icon", QString());
    getState()->setProperty("toolTip", QString());

    qDebug() << "🔘 ButtonCommand created";
}

UICommandMetadata ButtonCommand::getMetadata() const {
    UICommandMetadata metadata("Button", "QPushButton", "Button",
                               "A clickable button control");
    metadata.supported_events = {"clicked", "toggled"};
    metadata.default_properties = {{"text", "Button"},
                                   {"enabled", true},
                                   {"checkable", false},
                                   {"checked", false}};
    return metadata;
}

ButtonCommand& ButtonCommand::setText(const QString& text) {
    getState()->setProperty("text", text);
    return *this;
}

QString ButtonCommand::getText() const {
    return getState()->getProperty<QString>("text");
}

ButtonCommand& ButtonCommand::setEnabled(bool enabled) {
    getState()->setProperty("enabled", enabled);
    return *this;
}

bool ButtonCommand::isEnabled() const {
    return getState()->getProperty<bool>("enabled", true);
}

ButtonCommand& ButtonCommand::setIcon(const QString& iconPath) {
    getState()->setProperty("icon", iconPath);
    return *this;
}

QString ButtonCommand::getIcon() const {
    return getState()->getProperty<QString>("icon");
}

ButtonCommand& ButtonCommand::setToolTip(const QString& tooltip) {
    getState()->setProperty("toolTip", tooltip);
    return *this;
}

QString ButtonCommand::getToolTip() const {
    return getState()->getProperty<QString>("toolTip");
}

ButtonCommand& ButtonCommand::setCheckable(bool checkable) {
    getState()->setProperty("checkable", checkable);
    return *this;
}

bool ButtonCommand::isCheckable() const {
    return getState()->getProperty<bool>("checkable", false);
}

ButtonCommand& ButtonCommand::setChecked(bool checked) {
    getState()->setProperty("checked", checked);
    return *this;
}

bool ButtonCommand::isChecked() const {
    return getState()->getProperty<bool>("checked", false);
}

ButtonCommand& ButtonCommand::onClick(std::function<void()> handler) {
    click_handler_ = handler;
    return *this;
}

ButtonCommand& ButtonCommand::onToggled(std::function<void(bool)> handler) {
    toggle_handler_ = handler;
    return *this;
}

void ButtonCommand::onWidgetCreated(QWidget* widget) {
    BaseUICommand::onWidgetCreated(widget);

    if (auto* button = qobject_cast<QPushButton*>(widget)) {
        // Connect button signals to command events
        connect(button, &QPushButton::clicked, this, [this]() {
            emit clicked();
            handleEvent("clicked");
        });

        connect(button, &QPushButton::toggled, this, [this](bool checked) {
            getState()->setProperty("checked", checked);
            emit toggled(checked);
            handleEvent("toggled", checked);
        });
    }
}

void ButtonCommand::syncToWidget() {
    BaseUICommand::syncToWidget();

    if (auto* button = qobject_cast<QPushButton*>(getWidget())) {
        button->setText(getText());
        button->setEnabled(isEnabled());
        button->setCheckable(isCheckable());
        button->setChecked(isChecked());
        button->setToolTip(getToolTip());

        QString iconPath = getIcon();
        if (!iconPath.isEmpty()) {
            button->setIcon(QIcon(iconPath));
        }
    }
}

void ButtonCommand::syncFromWidget() {
    if (auto* button = qobject_cast<QPushButton*>(getWidget())) {
        getState()->setProperty("text", button->text());
        getState()->setProperty("enabled", button->isEnabled());
        getState()->setProperty("checkable", button->isCheckable());
        getState()->setProperty("checked", button->isChecked());
        getState()->setProperty("toolTip", button->toolTip());
    }
}

void ButtonCommand::handleEvent(const QString& eventType,
                                const QVariant& eventData) {
    if (eventType == "clicked" && click_handler_) {
        click_handler_();
    } else if (eventType == "toggled" && toggle_handler_) {
        toggle_handler_(eventData.toBool());
    }

    BaseUICommand::handleEvent(eventType, eventData);
}

// **LabelCommand implementation**
LabelCommand::LabelCommand(QObject* parent) : BaseUICommand(parent) {
    // Set up default properties
    getState()->setProperty("text", QString("Label"));
    getState()->setProperty("alignment",
                            static_cast<int>(Qt::AlignLeft | Qt::AlignVCenter));
    getState()->setProperty("wordWrap", false);
    getState()->setProperty("pixmap", QString());

    qDebug() << "🏷️ LabelCommand created";
}

UICommandMetadata LabelCommand::getMetadata() const {
    UICommandMetadata metadata("Label", "QLabel", "Label",
                               "A text or image display control");
    metadata.default_properties = {
        {"text", "Label"},
        {"alignment", static_cast<int>(Qt::AlignLeft | Qt::AlignVCenter)},
        {"wordWrap", false}};
    return metadata;
}

LabelCommand& LabelCommand::setText(const QString& text) {
    getState()->setProperty("text", text);
    return *this;
}

QString LabelCommand::getText() const {
    return getState()->getProperty<QString>("text");
}

LabelCommand& LabelCommand::setAlignment(int alignment) {
    getState()->setProperty("alignment", alignment);
    return *this;
}

int LabelCommand::getAlignment() const {
    return getState()->getProperty<int>(
        "alignment", static_cast<int>(Qt::AlignLeft | Qt::AlignVCenter));
}

LabelCommand& LabelCommand::setWordWrap(bool wrap) {
    getState()->setProperty("wordWrap", wrap);
    return *this;
}

bool LabelCommand::getWordWrap() const {
    return getState()->getProperty<bool>("wordWrap", false);
}

LabelCommand& LabelCommand::setPixmap(const QString& pixmapPath) {
    getState()->setProperty("pixmap", pixmapPath);
    return *this;
}

QString LabelCommand::getPixmap() const {
    return getState()->getProperty<QString>("pixmap");
}

void LabelCommand::onWidgetCreated(QWidget* widget) {
    BaseUICommand::onWidgetCreated(widget);
    // Labels typically don't have interactive events to connect
}

void LabelCommand::syncToWidget() {
    BaseUICommand::syncToWidget();

    if (auto* label = qobject_cast<QLabel*>(getWidget())) {
        label->setText(getText());
        label->setAlignment(static_cast<Qt::Alignment>(getAlignment()));
        label->setWordWrap(getWordWrap());

        QString pixmapPath = getPixmap();
        if (!pixmapPath.isEmpty()) {
            label->setPixmap(QPixmap(pixmapPath));
        }
    }
}

// **TextInputCommand implementation**
TextInputCommand::TextInputCommand(QObject* parent) : BaseUICommand(parent) {
    // Set up default properties
    getState()->setProperty("text", QString());
    getState()->setProperty("placeholder", QString());
    getState()->setProperty("readOnly", false);
    getState()->setProperty("maxLength", 32767);
    getState()->setProperty("echoMode", static_cast<int>(QLineEdit::Normal));

    qDebug() << "📝 TextInputCommand created";
}

UICommandMetadata TextInputCommand::getMetadata() const {
    UICommandMetadata metadata("TextInput", "QLineEdit", "Text Input",
                               "A single-line text input control");
    metadata.supported_events = {"textChanged", "returnPressed",
                                 "editingFinished"};
    metadata.default_properties = {{"text", ""},
                                   {"placeholder", ""},
                                   {"readOnly", false},
                                   {"maxLength", 32767}};
    return metadata;
}

TextInputCommand& TextInputCommand::setText(const QString& text) {
    getState()->setProperty("text", text);
    return *this;
}

QString TextInputCommand::getText() const {
    return getState()->getProperty<QString>("text");
}

TextInputCommand& TextInputCommand::setPlaceholder(const QString& placeholder) {
    getState()->setProperty("placeholder", placeholder);
    return *this;
}

QString TextInputCommand::getPlaceholder() const {
    return getState()->getProperty<QString>("placeholder");
}

TextInputCommand& TextInputCommand::setReadOnly(bool readOnly) {
    getState()->setProperty("readOnly", readOnly);
    return *this;
}

bool TextInputCommand::isReadOnly() const {
    return getState()->getProperty<bool>("readOnly", false);
}

TextInputCommand& TextInputCommand::setMaxLength(int maxLength) {
    getState()->setProperty("maxLength", maxLength);
    return *this;
}

int TextInputCommand::getMaxLength() const {
    return getState()->getProperty<int>("maxLength", 32767);
}

TextInputCommand& TextInputCommand::setEchoMode(int mode) {
    getState()->setProperty("echoMode", mode);
    return *this;
}

int TextInputCommand::getEchoMode() const {
    return getState()->getProperty<int>("echoMode",
                                        static_cast<int>(QLineEdit::Normal));
}

TextInputCommand& TextInputCommand::onTextChanged(
    std::function<void(const QString&)> handler) {
    text_changed_handler_ = handler;
    return *this;
}

TextInputCommand& TextInputCommand::onReturnPressed(
    std::function<void()> handler) {
    return_pressed_handler_ = handler;
    return *this;
}

TextInputCommand& TextInputCommand::onEditingFinished(
    std::function<void()> handler) {
    editing_finished_handler_ = handler;
    return *this;
}

void TextInputCommand::onWidgetCreated(QWidget* widget) {
    BaseUICommand::onWidgetCreated(widget);

    if (auto* lineEdit = qobject_cast<QLineEdit*>(widget)) {
        // Connect line edit signals to command events
        connect(lineEdit, &QLineEdit::textChanged, this,
                [this](const QString& text) {
                    getState()->setProperty("text", text);
                    emit textChanged(text);
                    handleEvent("textChanged", text);
                });

        connect(lineEdit, &QLineEdit::returnPressed, this, [this]() {
            emit returnPressed();
            handleEvent("returnPressed");
        });

        connect(lineEdit, &QLineEdit::editingFinished, this, [this]() {
            emit editingFinished();
            handleEvent("editingFinished");
        });
    }
}

void TextInputCommand::syncToWidget() {
    BaseUICommand::syncToWidget();

    if (auto* lineEdit = qobject_cast<QLineEdit*>(getWidget())) {
        lineEdit->setText(getText());
        lineEdit->setPlaceholderText(getPlaceholder());
        lineEdit->setReadOnly(isReadOnly());
        lineEdit->setMaxLength(getMaxLength());
        lineEdit->setEchoMode(static_cast<QLineEdit::EchoMode>(getEchoMode()));
    }
}

void TextInputCommand::syncFromWidget() {
    if (auto* lineEdit = qobject_cast<QLineEdit*>(getWidget())) {
        getState()->setProperty("text", lineEdit->text());
        getState()->setProperty("placeholder", lineEdit->placeholderText());
        getState()->setProperty("readOnly", lineEdit->isReadOnly());
        getState()->setProperty("maxLength", lineEdit->maxLength());
        getState()->setProperty("echoMode",
                                static_cast<int>(lineEdit->echoMode()));
    }
}

void TextInputCommand::handleEvent(const QString& eventType,
                                   const QVariant& eventData) {
    if (eventType == "textChanged" && text_changed_handler_) {
        text_changed_handler_(eventData.toString());
    } else if (eventType == "returnPressed" && return_pressed_handler_) {
        return_pressed_handler_();
    } else if (eventType == "editingFinished" && editing_finished_handler_) {
        editing_finished_handler_();
    }

    BaseUICommand::handleEvent(eventType, eventData);
}

// **MenuItemCommand implementation**
MenuItemCommand::MenuItemCommand(QObject* parent) : BaseUICommand(parent) {
    // Set up default properties
    getState()->setProperty("text", QString("Menu Item"));
    getState()->setProperty("shortcut", QString());
    getState()->setProperty("icon", QString());
    getState()->setProperty("checkable", false);
    getState()->setProperty("checked", false);
    getState()->setProperty("separator", false);

    qDebug() << "📋 MenuItemCommand created";
}

UICommandMetadata MenuItemCommand::getMetadata() const {
    UICommandMetadata metadata("MenuItem", "QAction", "Menu Item",
                               "A menu item or action control");
    metadata.supported_events = {"triggered", "toggled"};
    metadata.default_properties = {{"text", "Menu Item"},
                                   {"checkable", false},
                                   {"checked", false},
                                   {"separator", false}};
    return metadata;
}

MenuItemCommand& MenuItemCommand::setText(const QString& text) {
    getState()->setProperty("text", text);
    return *this;
}

QString MenuItemCommand::getText() const {
    return getState()->getProperty<QString>("text");
}

MenuItemCommand& MenuItemCommand::setShortcut(const QString& shortcut) {
    getState()->setProperty("shortcut", shortcut);
    return *this;
}

QString MenuItemCommand::getShortcut() const {
    return getState()->getProperty<QString>("shortcut");
}

MenuItemCommand& MenuItemCommand::setIcon(const QString& iconPath) {
    getState()->setProperty("icon", iconPath);
    return *this;
}

QString MenuItemCommand::getIcon() const {
    return getState()->getProperty<QString>("icon");
}

MenuItemCommand& MenuItemCommand::setCheckable(bool checkable) {
    getState()->setProperty("checkable", checkable);
    return *this;
}

bool MenuItemCommand::isCheckable() const {
    return getState()->getProperty<bool>("checkable", false);
}

MenuItemCommand& MenuItemCommand::setChecked(bool checked) {
    getState()->setProperty("checked", checked);
    return *this;
}

bool MenuItemCommand::isChecked() const {
    return getState()->getProperty<bool>("checked", false);
}

MenuItemCommand& MenuItemCommand::setSeparator(bool separator) {
    getState()->setProperty("separator", separator);
    return *this;
}

bool MenuItemCommand::isSeparator() const {
    return getState()->getProperty<bool>("separator", false);
}

MenuItemCommand& MenuItemCommand::onTriggered(std::function<void()> handler) {
    triggered_handler_ = handler;
    return *this;
}

MenuItemCommand& MenuItemCommand::onToggled(std::function<void(bool)> handler) {
    toggled_handler_ = handler;
    return *this;
}

void MenuItemCommand::onWidgetCreated(QWidget* widget) {
    // MenuItemCommand uses QAction, not QWidget
    Q_UNUSED(widget)

    if (action_) {
        // Connect action signals to command events
        connect(action_, &QAction::triggered, this, [this]() {
            emit triggered();
            handleEvent("triggered");
        });

        connect(action_, &QAction::toggled, this, [this](bool checked) {
            getState()->setProperty("checked", checked);
            emit toggled(checked);
            handleEvent("toggled", checked);
        });
    }
}

void MenuItemCommand::syncToWidget() {
    if (action_) {
        action_->setText(getText());
        action_->setShortcut(QKeySequence(getShortcut()));
        action_->setCheckable(isCheckable());
        action_->setChecked(isChecked());
        action_->setSeparator(isSeparator());

        QString iconPath = getIcon();
        if (!iconPath.isEmpty()) {
            action_->setIcon(QIcon(iconPath));
        }
    }
}

void MenuItemCommand::syncFromWidget() {
    if (action_) {
        getState()->setProperty("text", action_->text());
        getState()->setProperty("shortcut", action_->shortcut().toString());
        getState()->setProperty("checkable", action_->isCheckable());
        getState()->setProperty("checked", action_->isChecked());
        getState()->setProperty("separator", action_->isSeparator());
    }
}

QAction* MenuItemCommand::getAction() const { return action_; }

void MenuItemCommand::setAction(QAction* action) {
    if (action_ != action) {
        action_ = action;
        if (action_) {
            onWidgetCreated(nullptr);  // Set up connections
            syncToWidget();
        }
    }
}

void MenuItemCommand::handleEvent(const QString& eventType,
                                  const QVariant& eventData) {
    if (eventType == "triggered" && triggered_handler_) {
        triggered_handler_();
    } else if (eventType == "toggled" && toggled_handler_) {
        toggled_handler_(eventData.toBool());
    }

    BaseUICommand::handleEvent(eventType, eventData);
}

// **ContainerCommand implementation**
ContainerCommand::ContainerCommand(QObject* parent) : BaseUICommand(parent) {
    // Set up default properties
    getState()->setProperty("layout", QString("VBox"));
    getState()->setProperty("spacing", 6);
    getState()->setProperty("marginLeft", 9);
    getState()->setProperty("marginTop", 9);
    getState()->setProperty("marginRight", 9);
    getState()->setProperty("marginBottom", 9);

    qDebug() << "📦 ContainerCommand created";
}

UICommandMetadata ContainerCommand::getMetadata() const {
    UICommandMetadata metadata("Container", "QWidget", "Container",
                               "A container widget that holds other controls");
    metadata.default_properties = {{"layout", "VBox"}, {"spacing", 6},
                                   {"marginLeft", 9},  {"marginTop", 9},
                                   {"marginRight", 9}, {"marginBottom", 9}};
    return metadata;
}

ContainerCommand& ContainerCommand::setLayout(const QString& layoutType) {
    getState()->setProperty("layout", layoutType);
    return *this;
}

QString ContainerCommand::getLayout() const {
    return getState()->getProperty<QString>("layout", "VBox");
}

ContainerCommand& ContainerCommand::setSpacing(int spacing) {
    getState()->setProperty("spacing", spacing);
    return *this;
}

int ContainerCommand::getSpacing() const {
    return getState()->getProperty<int>("spacing", 6);
}

ContainerCommand& ContainerCommand::setMargins(int left, int top, int right,
                                               int bottom) {
    getState()->setProperty("marginLeft", left);
    getState()->setProperty("marginTop", top);
    getState()->setProperty("marginRight", right);
    getState()->setProperty("marginBottom", bottom);
    return *this;
}

ContainerCommand& ContainerCommand::setMargins(int margin) {
    return setMargins(margin, margin, margin, margin);
}

ContainerCommand& ContainerCommand::addChild(
    std::shared_ptr<BaseUICommand> child) {
    BaseUICommand::addChild(child);
    addChildWidget(child);
    return *this;
}

ContainerCommand& ContainerCommand::removeChild(
    std::shared_ptr<BaseUICommand> child) {
    removeChildWidget(child);
    BaseUICommand::removeChild(child);
    return *this;
}

void ContainerCommand::onWidgetCreated(QWidget* widget) {
    BaseUICommand::onWidgetCreated(widget);
    setupLayout();

    // Add existing children to the widget
    for (auto& child : getChildren()) {
        addChildWidget(child);
    }
}

void ContainerCommand::syncToWidget() {
    BaseUICommand::syncToWidget();
    setupLayout();
}

void ContainerCommand::setupLayout() {
    QWidget* widget = getWidget();
    if (!widget) {
        return;
    }

    QString layoutType = getLayout();
    QLayout* layout = nullptr;

    if (layoutType == "VBox") {
        layout = new QVBoxLayout(widget);
    } else if (layoutType == "HBox") {
        layout = new QHBoxLayout(widget);
    } else if (layoutType == "Grid") {
        layout = new QGridLayout(widget);
    }

    if (layout) {
        layout->setSpacing(getSpacing());
        layout->setContentsMargins(
            getState()->getProperty<int>("marginLeft", 9),
            getState()->getProperty<int>("marginTop", 9),
            getState()->getProperty<int>("marginRight", 9),
            getState()->getProperty<int>("marginBottom", 9));

        widget->setLayout(layout);
    }
}

void ContainerCommand::addChildWidget(std::shared_ptr<BaseUICommand> child) {
    QWidget* container = getWidget();
    if (!container || !child) {
        return;
    }

    QWidget* childWidget = child->getWidget();
    if (!childWidget) {
        // Create widget for child if it doesn't exist
        auto widget = WidgetMapper::instance().createWidget(child.get());
        childWidget = widget.release();  // Container takes ownership
    }

    if (childWidget && container->layout()) {
        container->layout()->addWidget(childWidget);
    }
}

void ContainerCommand::removeChildWidget(std::shared_ptr<BaseUICommand> child) {
    QWidget* container = getWidget();
    if (!container || !child) {
        return;
    }

    QWidget* childWidget = child->getWidget();
    if (childWidget && container->layout()) {
        container->layout()->removeWidget(childWidget);
    }
}

// **Core command registration**
void registerCoreCommands() {
    auto& factory = UICommandFactory::instance();
    auto& mapper = WidgetMapper::instance();

    qDebug() << "🔧 Registering core UI commands";

    // Register ButtonCommand
    factory.registerCommand<ButtonCommand>("Button", "QPushButton");
    mapper.registerMapping<QPushButton>("Button");

    // Register LabelCommand
    factory.registerCommand<LabelCommand>("Label", "QLabel");
    mapper.registerMapping<QLabel>("Label");

    // Register TextInputCommand
    factory.registerCommand<TextInputCommand>("TextInput", "QLineEdit");
    mapper.registerMapping<QLineEdit>("TextInput");

    // Register ContainerCommand
    factory.registerCommand<ContainerCommand>("Container", "QWidget");
    mapper.registerMapping<QWidget>("Container");

    // MenuItemCommand requires special handling since it uses QAction
    factory.registerCommand(
        "MenuItem",
        []() -> std::shared_ptr<BaseUICommand> {
            return std::make_shared<MenuItemCommand>();
        },
        UICommandMetadata("MenuItem", "QAction", "Menu Item",
                          "A menu item or action control"));

    qDebug() << "✅ Core UI commands registered successfully";
}

}  // namespace DeclarativeUI::Command::UI
