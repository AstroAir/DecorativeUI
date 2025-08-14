#include "WidgetMapper.hpp"
#include <QDebug>
#include <QMetaMethod>
#include <QSignalMapper>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QRadioButton>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QSlider>
#include <QProgressBar>
#include <QComboBox>
#include <QListView>
#include <QTableView>
#include <QTreeView>
#include <QGroupBox>
#include <QTabWidget>
#include <QScrollArea>
#include <QSplitter>
#include <QDial>
#include <QCalendarWidget>
#include <QLCDNumber>
#include <QFrame>
#include <QToolButton>
#include <QDateTimeEdit>
#include <algorithm>

namespace DeclarativeUI::Command::UI {

WidgetMapper::WidgetMapper(QObject* parent) : QObject(parent) {
    setupDefaultMappings();
    qDebug() << "🗺️ WidgetMapper initialized";
}

WidgetMapper& WidgetMapper::instance() {
    static WidgetMapper instance;
    return instance;
}

void WidgetMapper::registerMapping(const QString& command_type, const WidgetMappingConfig& config) {
    mappings_[command_type] = config;
    qDebug() << "📝 Registered mapping:" << command_type << "->" << config.widget_type;
}

std::unique_ptr<QWidget> WidgetMapper::createWidget(BaseUICommand* command) {
    if (!command) {
        qWarning() << "Cannot create widget for null command";
        return nullptr;
    }

    const QString command_type = command->getCommandType();
    auto it = mappings_.find(command_type);
    if (it == mappings_.end()) {
        qWarning() << "No mapping found for command type:" << command_type;
        return nullptr;
    }

    const auto& config = it->second;
    if (!config.factory) {
        qWarning() << "No factory function for command type:" << command_type;
        return nullptr;
    }

    try {
        auto widget = config.factory();
        if (!widget) {
            qWarning() << "Factory returned null widget for:" << command_type;
            return nullptr;
        }

        // Set up the widget-command relationship
        establishBinding(command, widget.get());

        // Apply custom setup if available
        if (config.custom_setup) {
            config.custom_setup(widget.get(), command);
        }

        emit widgetCreated(command, widget.get());
        qDebug() << "✨ Created widget for command:" << command_type;

        return widget;

    } catch (const std::exception& e) {
        handleSyncError(command, QString("Widget creation failed: %1").arg(e.what()));
        return nullptr;
    }
}

void WidgetMapper::destroyWidget(BaseUICommand* command) {
    if (!command) {
        return;
    }

    removeBinding(command);
    emit widgetDestroyed(command);
    qDebug() << "🗑️ Destroyed widget for command:" << command->getCommandType();
}

void WidgetMapper::establishBinding(BaseUICommand* command, QWidget* widget) {
    if (!command || !widget) {
        qWarning() << "Cannot establish binding with null command or widget";
        return;
    }

    // Remove existing binding if any
    removeBinding(command);

    // Create new binding info
    BindingInfo binding;
    binding.command = command;
    binding.widget = widget;

    // Set up property synchronization
    const QString command_type = command->getCommandType();
    auto mapping_it = mappings_.find(command_type);
    if (mapping_it != mappings_.end()) {
        const auto& config = mapping_it->second;

        // Connect property synchronization
        for (const auto& prop_config : config.property_mappings) {
            connectPropertySync(command, widget, prop_config);
        }

        // Connect event mappings
        for (const auto& event_config : config.event_mappings) {
            connectEventMapping(command, widget, event_config);
        }
    }

    // Connect to command property changes
    auto connection = connect(command, &BaseUICommand::propertyChanged,
                             this, &WidgetMapper::onCommandPropertyChanged);
    binding.connections.push_back(connection);

    // Store the binding
    active_bindings_[command] = std::move(binding);

    // Initial sync from command to widget
    syncCommandToWidget(command);

    emit bindingEstablished(command, widget);
    qDebug() << "🔗 Established binding for:" << command->getCommandType();
}

void WidgetMapper::removeBinding(BaseUICommand* command) {
    auto it = active_bindings_.find(command);
    if (it != active_bindings_.end()) {
        // Disconnect all connections
        for (const auto& connection : it->second.connections) {
            disconnect(connection);
        }

        active_bindings_.erase(it);
        emit bindingRemoved(command);
        qDebug() << "🔌 Removed binding for:" << command->getCommandType();
    }
}

void WidgetMapper::syncCommandToWidget(BaseUICommand* command, const QString& property) {
    auto it = active_bindings_.find(command);
    if (it == active_bindings_.end()) {
        return;
    }

    QWidget* widget = it->second.widget;
    if (!widget) {
        return;
    }

    const QString command_type = command->getCommandType();
    auto mapping_it = mappings_.find(command_type);
    if (mapping_it == mappings_.end()) {
        return;
    }

    const auto& config = mapping_it->second;

    // Sync specific property or all properties
    for (const auto& prop_config : config.property_mappings) {
        if (!property.isEmpty() && prop_config.command_property != property) {
            continue;
        }

        try {
            QVariant command_value = command->getState()->getProperty<QVariant>(prop_config.command_property);
            QVariant widget_value = command_value;

            // Apply converter if available
            if (prop_config.command_to_widget_converter) {
                widget_value = convertProperty(command_value, prop_config.command_to_widget_converter);
            }

            setWidgetProperty(widget, prop_config.widget_property, widget_value);

        } catch (const std::exception& e) {
            handleSyncError(command, QString("Property sync failed for %1: %2")
                          .arg(prop_config.command_property, e.what()));
        }
    }
}

void WidgetMapper::syncWidgetToCommand(QWidget* widget, BaseUICommand* command, const QString& property) {
    if (!widget || !command) {
        return;
    }

    const QString command_type = command->getCommandType();
    auto mapping_it = mappings_.find(command_type);
    if (mapping_it == mappings_.end()) {
        return;
    }

    const auto& config = mapping_it->second;

    // Sync specific property or all bidirectional properties
    for (const auto& prop_config : config.property_mappings) {
        if (!prop_config.bidirectional) {
            continue;
        }

        if (!property.isEmpty() && prop_config.widget_property != property) {
            continue;
        }

        try {
            QVariant widget_value = getWidgetProperty(widget, prop_config.widget_property);
            QVariant command_value = widget_value;

            // Apply converter if available
            if (prop_config.widget_to_command_converter) {
                command_value = convertProperty(widget_value, prop_config.widget_to_command_converter);
            }

            command->getState()->setProperty(prop_config.command_property, command_value);

        } catch (const std::exception& e) {
            handleSyncError(command, QString("Widget to command sync failed for %1: %2")
                          .arg(prop_config.widget_property, e.what()));
        }
    }
}

void WidgetMapper::connectEvents(BaseUICommand* command, QWidget* widget) {
    const QString command_type = command->getCommandType();
    auto mapping_it = mappings_.find(command_type);
    if (mapping_it == mappings_.end()) {
        return;
    }

    const auto& config = mapping_it->second;
    for (const auto& event_config : config.event_mappings) {
        connectEventMapping(command, widget, event_config);
    }
}

void WidgetMapper::disconnectEvents(BaseUICommand* command, QWidget* widget) {
    // Events are disconnected when binding is removed
    Q_UNUSED(command)
    Q_UNUSED(widget)
}

bool WidgetMapper::hasMapping(const QString& command_type) const {
    return mappings_.find(command_type) != mappings_.end();
}

QString WidgetMapper::getWidgetType(const QString& command_type) const {
    auto it = mappings_.find(command_type);
    return it != mappings_.end() ? it->second.widget_type : QString();
}

QStringList WidgetMapper::getSupportedCommandTypes() const {
    QStringList types;
    for (const auto& [type, config] : mappings_) {
        types.append(type);
    }
    return types;
}

void WidgetMapper::onCommandPropertyChanged(const QString& property, const QVariant& value) {
    auto* command = qobject_cast<BaseUICommand*>(sender());
    if (command) {
        syncCommandToWidget(command, property);
    }
}

void WidgetMapper::onWidgetPropertyChanged() {
    // This would be connected to specific widget property change signals
    // Implementation depends on specific widget types
}

void WidgetMapper::setupDefaultMappings() {
    // Set up default mappings for common Qt widgets
    qDebug() << "🔧 Setting up default widget mappings";

    // ========================================================================
    // BUTTON COMPONENTS
    // ========================================================================

    // QPushButton mapping
    WidgetMappingConfig buttonConfig("QPushButton", []() -> std::unique_ptr<QWidget> {
        return std::make_unique<QPushButton>();
    });
    buttonConfig.property_mappings = {
        PropertySyncConfig("text", "text"),
        PropertySyncConfig("enabled", "enabled"),
        PropertySyncConfig("visible", "visible"),
        PropertySyncConfig("checkable", "checkable"),
        PropertySyncConfig("checked", "checked")
    };
    buttonConfig.event_mappings = {
        EventMappingConfig("clicked()", "clicked"),
        EventMappingConfig("pressed()", "pressed"),
        EventMappingConfig("released()", "released"),
        EventMappingConfig("toggled(bool)", "toggled")
    };
    registerMapping("ButtonCommand", buttonConfig);

    // QCheckBox mapping
    WidgetMappingConfig checkBoxConfig("QCheckBox", []() -> std::unique_ptr<QWidget> {
        return std::make_unique<QCheckBox>();
    });
    checkBoxConfig.property_mappings = {
        PropertySyncConfig("text", "text"),
        PropertySyncConfig("checked", "checked"),
        PropertySyncConfig("enabled", "enabled"),
        PropertySyncConfig("visible", "visible"),
        PropertySyncConfig("tristate", "tristate")
    };
    checkBoxConfig.event_mappings = {
        EventMappingConfig("clicked()", "clicked"),
        EventMappingConfig("toggled(bool)", "toggled"),
        EventMappingConfig("stateChanged(int)", "stateChanged")
    };
    registerMapping("CheckBoxCommand", checkBoxConfig);

    // QRadioButton mapping
    WidgetMappingConfig radioButtonConfig("QRadioButton", []() -> std::unique_ptr<QWidget> {
        return std::make_unique<QRadioButton>();
    });
    radioButtonConfig.property_mappings = {
        PropertySyncConfig("text", "text"),
        PropertySyncConfig("checked", "checked"),
        PropertySyncConfig("enabled", "enabled"),
        PropertySyncConfig("visible", "visible")
    };
    radioButtonConfig.event_mappings = {
        EventMappingConfig("clicked()", "clicked"),
        EventMappingConfig("toggled(bool)", "toggled")
    };
    registerMapping("RadioButtonCommand", radioButtonConfig);

    qDebug() << "✅ Button component mappings setup complete";

    // ========================================================================
    // TEXT COMPONENTS
    // ========================================================================

    // QLabel mapping
    WidgetMappingConfig labelConfig("QLabel", []() -> std::unique_ptr<QWidget> {
        return std::make_unique<QLabel>();
    });
    labelConfig.property_mappings = {
        PropertySyncConfig("text", "text"),
        PropertySyncConfig("enabled", "enabled"),
        PropertySyncConfig("visible", "visible"),
        PropertySyncConfig("wordWrap", "wordWrap"),
        PropertySyncConfig("alignment", "alignment")
    };
    registerMapping("LabelCommand", labelConfig);

    // QLineEdit mapping
    WidgetMappingConfig lineEditConfig("QLineEdit", []() -> std::unique_ptr<QWidget> {
        return std::make_unique<QLineEdit>();
    });
    lineEditConfig.property_mappings = {
        PropertySyncConfig("text", "text"),
        PropertySyncConfig("placeholderText", "placeholderText"),
        PropertySyncConfig("readOnly", "readOnly"),
        PropertySyncConfig("enabled", "enabled"),
        PropertySyncConfig("visible", "visible"),
        PropertySyncConfig("maxLength", "maxLength"),
        PropertySyncConfig("echoMode", "echoMode")
    };
    lineEditConfig.event_mappings = {
        EventMappingConfig("textChanged(QString)", "textChanged"),
        EventMappingConfig("textEdited(QString)", "textEdited"),
        EventMappingConfig("returnPressed()", "returnPressed"),
        EventMappingConfig("editingFinished()", "editingFinished")
    };
    registerMapping("LineEditCommand", lineEditConfig);

    // QTextEdit mapping
    WidgetMappingConfig textEditConfig("QTextEdit", []() -> std::unique_ptr<QWidget> {
        return std::make_unique<QTextEdit>();
    });
    textEditConfig.property_mappings = {
        PropertySyncConfig("plainText", "plainText"),
        PropertySyncConfig("html", "html"),
        PropertySyncConfig("readOnly", "readOnly"),
        PropertySyncConfig("enabled", "enabled"),
        PropertySyncConfig("visible", "visible")
    };
    textEditConfig.event_mappings = {
        EventMappingConfig("textChanged()", "textChanged"),
        EventMappingConfig("selectionChanged()", "selectionChanged"),
        EventMappingConfig("cursorPositionChanged()", "cursorPositionChanged")
    };
    registerMapping("TextEditCommand", textEditConfig);

    qDebug() << "✅ Text component mappings setup complete";

    // ========================================================================
    // INPUT COMPONENTS
    // ========================================================================

    // QSpinBox mapping
    WidgetMappingConfig spinBoxConfig("QSpinBox", []() -> std::unique_ptr<QWidget> {
        return std::make_unique<QSpinBox>();
    });
    spinBoxConfig.property_mappings = {
        PropertySyncConfig("value", "value"),
        PropertySyncConfig("minimum", "minimum"),
        PropertySyncConfig("maximum", "maximum"),
        PropertySyncConfig("singleStep", "singleStep"),
        PropertySyncConfig("enabled", "enabled"),
        PropertySyncConfig("visible", "visible"),
        PropertySyncConfig("readOnly", "readOnly")
    };
    spinBoxConfig.event_mappings = {
        EventMappingConfig("valueChanged(int)", "valueChanged"),
        EventMappingConfig("editingFinished()", "editingFinished")
    };
    registerMapping("SpinBoxCommand", spinBoxConfig);

    // QSlider mapping
    WidgetMappingConfig sliderConfig("QSlider", []() -> std::unique_ptr<QWidget> {
        return std::make_unique<QSlider>();
    });
    sliderConfig.property_mappings = {
        PropertySyncConfig("value", "value"),
        PropertySyncConfig("minimum", "minimum"),
        PropertySyncConfig("maximum", "maximum"),
        PropertySyncConfig("singleStep", "singleStep"),
        PropertySyncConfig("pageStep", "pageStep"),
        PropertySyncConfig("orientation", "orientation"),
        PropertySyncConfig("enabled", "enabled"),
        PropertySyncConfig("visible", "visible")
    };
    sliderConfig.event_mappings = {
        EventMappingConfig("valueChanged(int)", "valueChanged"),
        EventMappingConfig("sliderPressed()", "sliderPressed"),
        EventMappingConfig("sliderReleased()", "sliderReleased"),
        EventMappingConfig("sliderMoved(int)", "sliderMoved")
    };
    registerMapping("SliderCommand", sliderConfig);

    // QComboBox mapping
    WidgetMappingConfig comboBoxConfig("QComboBox", []() -> std::unique_ptr<QWidget> {
        return std::make_unique<QComboBox>();
    });
    comboBoxConfig.property_mappings = {
        PropertySyncConfig("currentIndex", "currentIndex"),
        PropertySyncConfig("currentText", "currentText"),
        PropertySyncConfig("enabled", "enabled"),
        PropertySyncConfig("visible", "visible"),
        PropertySyncConfig("editable", "editable"),
        PropertySyncConfig("maxCount", "maxCount")
    };
    comboBoxConfig.event_mappings = {
        EventMappingConfig("currentIndexChanged(int)", "currentIndexChanged"),
        EventMappingConfig("currentTextChanged(QString)", "currentTextChanged"),
        EventMappingConfig("activated(int)", "activated"),
        EventMappingConfig("highlighted(int)", "highlighted")
    };
    registerMapping("ComboBoxCommand", comboBoxConfig);

    qDebug() << "✅ Input component mappings setup complete";

    // ========================================================================
    // CONTAINER COMPONENTS
    // ========================================================================

    // QTabWidget mapping
    WidgetMappingConfig tabWidgetConfig("QTabWidget", []() -> std::unique_ptr<QWidget> {
        return std::make_unique<QTabWidget>();
    });
    tabWidgetConfig.property_mappings = {
        PropertySyncConfig("currentIndex", "currentIndex"),
        PropertySyncConfig("tabPosition", "tabPosition"),
        PropertySyncConfig("tabShape", "tabShape"),
        PropertySyncConfig("enabled", "enabled"),
        PropertySyncConfig("visible", "visible"),
        PropertySyncConfig("tabsClosable", "tabsClosable"),
        PropertySyncConfig("movable", "movable")
    };
    tabWidgetConfig.event_mappings = {
        EventMappingConfig("currentChanged(int)", "currentChanged"),
        EventMappingConfig("tabCloseRequested(int)", "tabCloseRequested"),
        EventMappingConfig("tabBarClicked(int)", "tabBarClicked"),
        EventMappingConfig("tabBarDoubleClicked(int)", "tabBarDoubleClicked")
    };
    registerMapping("TabWidgetCommand", tabWidgetConfig);

    // QGroupBox mapping
    WidgetMappingConfig groupBoxConfig("QGroupBox", []() -> std::unique_ptr<QWidget> {
        return std::make_unique<QGroupBox>();
    });
    groupBoxConfig.property_mappings = {
        PropertySyncConfig("title", "title"),
        PropertySyncConfig("checkable", "checkable"),
        PropertySyncConfig("checked", "checked"),
        PropertySyncConfig("enabled", "enabled"),
        PropertySyncConfig("visible", "visible"),
        PropertySyncConfig("alignment", "alignment")
    };
    groupBoxConfig.event_mappings = {
        EventMappingConfig("clicked(bool)", "clicked"),
        EventMappingConfig("toggled(bool)", "toggled")
    };
    registerMapping("GroupBoxCommand", groupBoxConfig);

    // QScrollArea mapping
    WidgetMappingConfig scrollAreaConfig("QScrollArea", []() -> std::unique_ptr<QWidget> {
        return std::make_unique<QScrollArea>();
    });
    scrollAreaConfig.property_mappings = {
        PropertySyncConfig("enabled", "enabled"),
        PropertySyncConfig("visible", "visible"),
        PropertySyncConfig("widgetResizable", "widgetResizable"),
        PropertySyncConfig("horizontalScrollBarPolicy", "horizontalScrollBarPolicy"),
        PropertySyncConfig("verticalScrollBarPolicy", "verticalScrollBarPolicy")
    };
    registerMapping("ScrollAreaCommand", scrollAreaConfig);

    qDebug() << "✅ Container component mappings setup complete";

    // ========================================================================
    // PRIORITY 1 - COMMON INPUT COMPONENTS
    // ========================================================================

    // QDoubleSpinBox mapping
    WidgetMappingConfig doubleSpinBoxConfig("QDoubleSpinBox", []() -> std::unique_ptr<QWidget> {
        return std::make_unique<QDoubleSpinBox>();
    });
    doubleSpinBoxConfig.property_mappings = {
        PropertySyncConfig("value", "value"),
        PropertySyncConfig("minimum", "minimum"),
        PropertySyncConfig("maximum", "maximum"),
        PropertySyncConfig("singleStep", "singleStep"),
        PropertySyncConfig("decimals", "decimals"),
        PropertySyncConfig("enabled", "enabled"),
        PropertySyncConfig("visible", "visible"),
        PropertySyncConfig("readOnly", "readOnly")
    };
    doubleSpinBoxConfig.event_mappings = {
        EventMappingConfig("valueChanged(double)", "valueChanged"),
        EventMappingConfig("editingFinished()", "editingFinished")
    };
    registerMapping("DoubleSpinBoxCommand", doubleSpinBoxConfig);

    // QDial mapping
    WidgetMappingConfig dialConfig("QDial", []() -> std::unique_ptr<QWidget> {
        return std::make_unique<QDial>();
    });
    dialConfig.property_mappings = {
        PropertySyncConfig("value", "value"),
        PropertySyncConfig("minimum", "minimum"),
        PropertySyncConfig("maximum", "maximum"),
        PropertySyncConfig("singleStep", "singleStep"),
        PropertySyncConfig("pageStep", "pageStep"),
        PropertySyncConfig("notchesVisible", "notchesVisible"),
        PropertySyncConfig("enabled", "enabled"),
        PropertySyncConfig("visible", "visible")
    };
    dialConfig.event_mappings = {
        EventMappingConfig("valueChanged(int)", "valueChanged"),
        EventMappingConfig("sliderPressed()", "sliderPressed"),
        EventMappingConfig("sliderReleased()", "sliderReleased")
    };
    registerMapping("DialCommand", dialConfig);

    // QDateTimeEdit mapping
    WidgetMappingConfig dateTimeEditConfig("QDateTimeEdit", []() -> std::unique_ptr<QWidget> {
        return std::make_unique<QDateTimeEdit>();
    });
    dateTimeEditConfig.property_mappings = {
        PropertySyncConfig("dateTime", "dateTime"),
        PropertySyncConfig("date", "date"),
        PropertySyncConfig("time", "time"),
        PropertySyncConfig("minimumDate", "minimumDate"),
        PropertySyncConfig("maximumDate", "maximumDate"),
        PropertySyncConfig("displayFormat", "displayFormat"),
        PropertySyncConfig("enabled", "enabled"),
        PropertySyncConfig("visible", "visible"),
        PropertySyncConfig("readOnly", "readOnly")
    };
    dateTimeEditConfig.event_mappings = {
        EventMappingConfig("dateTimeChanged(QDateTime)", "dateTimeChanged"),
        EventMappingConfig("dateChanged(QDate)", "dateChanged"),
        EventMappingConfig("timeChanged(QTime)", "timeChanged")
    };
    registerMapping("DateTimeEditCommand", dateTimeEditConfig);

    // QProgressBar mapping
    WidgetMappingConfig progressBarConfig("QProgressBar", []() -> std::unique_ptr<QWidget> {
        return std::make_unique<QProgressBar>();
    });
    progressBarConfig.property_mappings = {
        PropertySyncConfig("value", "value"),
        PropertySyncConfig("minimum", "minimum"),
        PropertySyncConfig("maximum", "maximum"),
        PropertySyncConfig("text", "text"),
        PropertySyncConfig("textVisible", "textVisible"),
        PropertySyncConfig("orientation", "orientation"),
        PropertySyncConfig("enabled", "enabled"),
        PropertySyncConfig("visible", "visible")
    };
    progressBarConfig.event_mappings = {
        EventMappingConfig("valueChanged(int)", "valueChanged")
    };
    registerMapping("ProgressBarCommand", progressBarConfig);

    qDebug() << "✅ Priority 1 input component mappings setup complete";

    // ========================================================================
    // PRIORITY 2 - DISPLAY COMPONENTS
    // ========================================================================

    // QLCDNumber mapping
    WidgetMappingConfig lcdNumberConfig("QLCDNumber", []() -> std::unique_ptr<QWidget> {
        return std::make_unique<QLCDNumber>();
    });
    lcdNumberConfig.property_mappings = {
        PropertySyncConfig("value", "value"),
        PropertySyncConfig("digitCount", "digitCount"),
        PropertySyncConfig("mode", "mode"),
        PropertySyncConfig("segmentStyle", "segmentStyle"),
        PropertySyncConfig("enabled", "enabled"),
        PropertySyncConfig("visible", "visible")
    };
    registerMapping("LCDNumberCommand", lcdNumberConfig);

    // QCalendarWidget mapping
    WidgetMappingConfig calendarConfig("QCalendarWidget", []() -> std::unique_ptr<QWidget> {
        return std::make_unique<QCalendarWidget>();
    });
    calendarConfig.property_mappings = {
        PropertySyncConfig("selectedDate", "selectedDate"),
        PropertySyncConfig("minimumDate", "minimumDate"),
        PropertySyncConfig("maximumDate", "maximumDate"),
        PropertySyncConfig("gridVisible", "gridVisible"),
        PropertySyncConfig("enabled", "enabled"),
        PropertySyncConfig("visible", "visible")
    };
    calendarConfig.event_mappings = {
        EventMappingConfig("clicked(QDate)", "clicked"),
        EventMappingConfig("selectionChanged()", "selectionChanged"),
        EventMappingConfig("activated(QDate)", "activated")
    };
    registerMapping("CalendarCommand", calendarConfig);

    qDebug() << "✅ Priority 2 display component mappings setup complete";

    // ========================================================================
    // PRIORITY 3 - VIEW COMPONENTS
    // ========================================================================

    // QListView mapping
    WidgetMappingConfig listViewConfig("QListView", []() -> std::unique_ptr<QWidget> {
        return std::make_unique<QListView>();
    });
    listViewConfig.property_mappings = {
        PropertySyncConfig("currentIndex", "currentIndex"),
        PropertySyncConfig("selectionMode", "selectionMode"),
        PropertySyncConfig("enabled", "enabled"),
        PropertySyncConfig("visible", "visible")
    };
    listViewConfig.event_mappings = {
        EventMappingConfig("clicked(QModelIndex)", "clicked"),
        EventMappingConfig("doubleClicked(QModelIndex)", "doubleClicked"),
        EventMappingConfig("activated(QModelIndex)", "activated")
    };
    registerMapping("ListViewCommand", listViewConfig);

    // QTableView mapping
    WidgetMappingConfig tableViewConfig("QTableView", []() -> std::unique_ptr<QWidget> {
        return std::make_unique<QTableView>();
    });
    tableViewConfig.property_mappings = {
        PropertySyncConfig("currentIndex", "currentIndex"),
        PropertySyncConfig("selectionMode", "selectionMode"),
        PropertySyncConfig("selectionBehavior", "selectionBehavior"),
        PropertySyncConfig("enabled", "enabled"),
        PropertySyncConfig("visible", "visible")
    };
    tableViewConfig.event_mappings = {
        EventMappingConfig("clicked(QModelIndex)", "clicked"),
        EventMappingConfig("doubleClicked(QModelIndex)", "doubleClicked"),
        EventMappingConfig("activated(QModelIndex)", "activated")
    };
    registerMapping("TableViewCommand", tableViewConfig);

    // QTreeView mapping
    WidgetMappingConfig treeViewConfig("QTreeView", []() -> std::unique_ptr<QWidget> {
        return std::make_unique<QTreeView>();
    });
    treeViewConfig.property_mappings = {
        PropertySyncConfig("currentIndex", "currentIndex"),
        PropertySyncConfig("selectionMode", "selectionMode"),
        PropertySyncConfig("selectionBehavior", "selectionBehavior"),
        PropertySyncConfig("enabled", "enabled"),
        PropertySyncConfig("visible", "visible")
    };
    treeViewConfig.event_mappings = {
        EventMappingConfig("clicked(QModelIndex)", "clicked"),
        EventMappingConfig("doubleClicked(QModelIndex)", "doubleClicked"),
        EventMappingConfig("expanded(QModelIndex)", "expanded"),
        EventMappingConfig("collapsed(QModelIndex)", "collapsed")
    };
    registerMapping("TreeViewCommand", treeViewConfig);

    qDebug() << "✅ Priority 3 view component mappings setup complete";
    qDebug() << "🎯 All default widget mappings setup complete";
}

void WidgetMapper::connectPropertySync(BaseUICommand* command, QWidget* widget, const PropertySyncConfig& config) {
    // Property sync connections are handled in syncCommandToWidget/syncWidgetToCommand
    Q_UNUSED(command)
    Q_UNUSED(widget)
    Q_UNUSED(config)
}

void WidgetMapper::connectEventMapping(BaseUICommand* command, QWidget* widget, const EventMappingConfig& config) {
    // Connect widget signal to command event
    const QMetaObject* meta = widget->metaObject();
    int signal_index = meta->indexOfSignal(config.widget_signal.toUtf8().constData());

    if (signal_index >= 0) {
        // Create a lambda to handle the signal and forward to command
        QMetaObject::Connection connection;
        if (auto* button = qobject_cast<QPushButton*>(widget)) {
            connection = connect(button, &QPushButton::clicked, [command, config]() {
                QVariant eventData;
                if (config.signal_to_event_converter) {
                    eventData = config.signal_to_event_converter(QVariantList{});
                }
                command->handleEvent(config.command_event, eventData);
            });
        }

        // Store connection for later cleanup
        auto it = active_bindings_.find(command);
        if (it != active_bindings_.end()) {
            it->second.connections.push_back(connection);
        }
    }
}

QVariant WidgetMapper::convertProperty(const QVariant& value, const std::function<QVariant(const QVariant&)>& converter) {
    if (converter) {
        return converter(value);
    }
    return value;
}

void WidgetMapper::handleSyncError(BaseUICommand* command, const QString& error) {
    qWarning() << "Sync error for command" << command->getCommandType() << ":" << error;
    emit syncError(command, error);
}

bool WidgetMapper::setWidgetProperty(QWidget* widget, const QString& property, const QVariant& value) {
    return widget->setProperty(property.toUtf8().constData(), value);
}

QVariant WidgetMapper::getWidgetProperty(QWidget* widget, const QString& property) {
    return widget->property(property.toUtf8().constData());
}

bool WidgetMapper::connectToSignal(QWidget* widget, const QString& signal, QObject* receiver, const char* slot) {
    const QMetaObject* meta = widget->metaObject();
    int signal_index = meta->indexOfSignal(signal.toUtf8().constData());
    return signal_index >= 0;
}

}  // namespace DeclarativeUI::Command::UI
