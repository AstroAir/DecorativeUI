#include "JSONUILoader.hpp"
#include "ComponentRegistry.hpp"

#include <QColor>
#include <QFile>
#include <QFont>
#include <QFormLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QJsonParseError>
#include <QLayout>
#include <QMetaObject>
#include <QMetaProperty>
#include <QPushButton>
#include <QRect>
#include <QSize>
#include <QVBoxLayout>

#include "../Exceptions/UIExceptions.hpp"

namespace DeclarativeUI::JSON {

JSONUILoader::JSONUILoader(QObject *parent) : QObject(parent) {
    // **Register default property converters**
    property_converters_["color"] = [](const QJsonValue &value) {
        return QVariant::fromValue(QColor(value.toString()));
    };

    property_converters_["font"] = [](const QJsonValue &value) {
        QFont font;
        if (value.isObject()) {
            QJsonObject font_obj = value.toObject();
            font.setFamily(font_obj["family"].toString());
            font.setPointSize(font_obj["size"].toInt(12));
            font.setBold(font_obj["bold"].toBool(false));
            font.setItalic(font_obj["italic"].toBool(false));
        }
        return QVariant::fromValue(font);
    };

    property_converters_["size"] = [](const QJsonValue &value) {
        if (value.isArray()) {
            QJsonArray size_array = value.toArray();
            return QVariant::fromValue(
                QSize(size_array[0].toInt(), size_array[1].toInt()));
        }
        return QVariant{};
    };

    property_converters_["rect"] = [](const QJsonValue &value) {
        if (value.isArray()) {
            QJsonArray rect_array = value.toArray();
            return QVariant::fromValue(
                QRect(rect_array[0].toInt(), rect_array[1].toInt(),
                      rect_array[2].toInt(), rect_array[3].toInt()));
        }
        return QVariant{};
    };

    property_converters_["minimumSize"] = [](const QJsonValue &value) {
        if (value.isArray()) {
            QJsonArray size_array = value.toArray();
            return QVariant::fromValue(
                QSize(size_array[0].toInt(), size_array[1].toInt()));
        }
        return QVariant{};
    };

    property_converters_["maximumSize"] = [](const QJsonValue &value) {
        if (value.isArray()) {
            QJsonArray size_array = value.toArray();
            return QVariant::fromValue(
                QSize(size_array[0].toInt(), size_array[1].toInt()));
        }
        return QVariant{};
    };
}

std::unique_ptr<QWidget> JSONUILoader::loadFromFile(const QString &file_path) {
    emit loadingStarted(file_path);

    try {
        QFile file(file_path);
        if (!file.open(QIODevice::ReadOnly)) {
            throw Exceptions::JSONParsingException(
                file_path.toStdString(),
                "Cannot open file: " + file.errorString().toStdString());
        }

        QByteArray data = file.readAll();
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(data, &error);

        if (doc.isNull()) {
            throw Exceptions::JSONParsingException(
                file_path.toStdString(), error.errorString().toStdString());
        }

        auto widget = loadFromObject(doc.object());
        emit loadingFinished(file_path);
        return widget;

    } catch (const std::exception &e) {
        emit loadingFailed(file_path, QString::fromStdString(e.what()));
        throw;
    }
}

std::unique_ptr<QWidget> JSONUILoader::loadFromString(
    const QString &json_string) {
    emit loadingStarted("string");

    try {
        QJsonParseError error;
        QJsonDocument doc =
            QJsonDocument::fromJson(json_string.toUtf8(), &error);

        if (doc.isNull()) {
            throw Exceptions::JSONParsingException(
                "string", error.errorString().toStdString());
        }

        auto widget = loadFromObject(doc.object());
        emit loadingFinished("string");
        return widget;

    } catch (const std::exception &e) {
        emit loadingFailed("string", QString::fromStdString(e.what()));
        throw;
    }
}

std::unique_ptr<QWidget> JSONUILoader::loadFromObject(
    const QJsonObject &json_object) {
    if (!validateJSON(json_object)) {
        throw Exceptions::JSONValidationException("Invalid JSON structure");
    }

    return createWidgetFromObject(json_object);
}

bool JSONUILoader::validateJSON(const QJsonObject &json_object) const {
    // **Validate required fields**
    if (!json_object.contains("type")) {
        return false;
    }

    QString type = json_object["type"].toString();
    if (!ComponentRegistry::instance().hasComponent(type)) {
        return false;
    }

    // **Validate properties structure**
    if (json_object.contains("properties") &&
        !json_object["properties"].isObject()) {
        return false;
    }

    // **Validate children structure**
    if (json_object.contains("children") &&
        !json_object["children"].isArray()) {
        return false;
    }

    // **Validate events structure**
    if (json_object.contains("events") && !json_object["events"].isObject()) {
        return false;
    }

    return validateWidgetObject(json_object);
}

std::unique_ptr<QWidget> JSONUILoader::createWidgetFromObject(
    const QJsonObject &widget_object) {
    try {
        QString type = widget_object["type"].toString();

        // **Create widget using registry**
        auto widget =
            ComponentRegistry::instance().createComponent(type, widget_object);

        if (!widget) {
            throw Exceptions::ComponentCreationException(type.toStdString());
        }

        // **Apply properties**
        if (widget_object.contains("properties")) {
            applyProperties(widget.get(),
                            widget_object["properties"].toObject());
        }

        // **Bind events**
        if (widget_object.contains("events")) {
            bindEvents(widget.get(), widget_object["events"].toObject());
        }

        // **Setup property bindings**
        if (widget_object.contains("bindings")) {
            setupPropertyBindings(widget.get(),
                                  widget_object["bindings"].toObject());
        }

        // **Setup layout**
        if (widget_object.contains("layout")) {
            setupLayout(widget.get(), widget_object["layout"].toObject());
        }

        // **Add children**
        if (widget_object.contains("children")) {
            addChildren(widget.get(), widget_object["children"].toArray());
        }

        return widget;

    } catch (const std::exception &e) {
        throw Exceptions::ComponentCreationException(
            "Widget creation failed: " + std::string(e.what()));
    }
}

void JSONUILoader::applyProperties(QWidget *widget,
                                   const QJsonObject &properties) {
    if (!widget)
        return;

    for (auto it = properties.begin(); it != properties.end(); ++it) {
        const QString &property_name = it.key();
        const QJsonValue &property_value = it.value();

        try {
            QVariant variant_value =
                convertJSONValue(property_value, property_name);

            bool success = widget->setProperty(
                property_name.toUtf8().constData(), variant_value);

            if (!success) {
                qWarning() << "Failed to set property" << property_name
                           << "on widget" << widget->metaObject()->className();
            }

        } catch (const std::exception &e) {
            qWarning() << "Property conversion failed for" << property_name
                       << ":" << e.what();
        }
    }
}

void JSONUILoader::bindEvents(QWidget *widget, const QJsonObject &events) {
    if (!widget)
        return;

    for (auto it = events.begin(); it != events.end(); ++it) {
        const QString &event_name = it.key();
        const QString &handler_name = it.value().toString();

        // **Find event handler**
        auto handler_it = event_handlers_.find(handler_name);
        if (handler_it == event_handlers_.end()) {
            qWarning() << "Event handler not found:" << handler_name;
            continue;
        }

        // **Handle specific widget types and signals**
        bool signal_connected = false;

        if (event_name == "clicked") {
            if (auto *button = qobject_cast<QPushButton *>(widget)) {
                QObject::connect(button, &QPushButton::clicked,
                                 [handler_it]() { handler_it->second(); });
                signal_connected = true;
                qDebug() << "✅ Connected signal:" << event_name
                         << "to handler:" << handler_name;
            }
        }

        if (!signal_connected) {
            qWarning() << "❌ Failed to connect signal:" << event_name
                       << "for widget" << widget->metaObject()->className();
        }
    }
}

void JSONUILoader::setupLayout(QWidget *parent,
                               const QJsonObject &layout_config) {
    if (!parent)
        return;

    QString layout_type = layout_config["type"].toString();
    QLayout *layout = nullptr;

    // **Create layout based on type**
    if (layout_type == "VBoxLayout") {
        layout = new QVBoxLayout();
    } else if (layout_type == "HBoxLayout") {
        layout = new QHBoxLayout();
    } else if (layout_type == "GridLayout") {
        layout = new QGridLayout();
    } else if (layout_type == "FormLayout") {
        layout = new QFormLayout();
    } else {
        qWarning() << "Unknown layout type:" << layout_type;
        return;
    }

    // **Apply layout properties**
    if (layout_config.contains("spacing")) {
        layout->setSpacing(layout_config["spacing"].toInt());
    }

    if (layout_config.contains("margins")) {
        QJsonArray margins = layout_config["margins"].toArray();
        if (margins.size() == 4) {
            layout->setContentsMargins(margins[0].toInt(), margins[1].toInt(),
                                       margins[2].toInt(), margins[3].toInt());
        }
    }

    parent->setLayout(layout);
}

void JSONUILoader::addChildren(QWidget *parent, const QJsonArray &children) {
    if (!parent)
        return;

    QLayout *layout = parent->layout();

    for (const QJsonValue &child_value : children) {
        if (!child_value.isObject())
            continue;

        try {
            auto child_widget = createWidgetFromObject(child_value.toObject());

            if (layout) {
                // **Handle grid layout positioning**
                if (auto *grid_layout = qobject_cast<QGridLayout *>(layout)) {
                    QJsonObject child_obj = child_value.toObject();
                    int row = child_obj.value("row").toInt(0);
                    int col = child_obj.value("column").toInt(0);
                    int row_span = child_obj.value("rowSpan").toInt(1);
                    int col_span = child_obj.value("columnSpan").toInt(1);

                    grid_layout->addWidget(child_widget.release(), row, col,
                                           row_span, col_span);
                } else {
                    layout->addWidget(child_widget.release());
                }
            } else {
                child_widget->setParent(parent);
            }

        } catch (const std::exception &e) {
            qWarning() << "Failed to create child widget:" << e.what();
        }
    }
}

void JSONUILoader::setupPropertyBindings(QWidget *widget,
                                         const QJsonObject &bindings) {
    if (!widget || !state_manager_) {
        return;
    }

    for (auto it = bindings.begin(); it != bindings.end(); ++it) {
        const QString &property_name = it.key();
        const QString &state_key = it.value().toString();

        // **Create property binding with state manager**
        try {
            // This would need to be implemented based on your state system
            // Example for string properties:
            if (auto state = state_manager_->getState<QString>(state_key)) {
                connect(state.get(),
                        &Binding::ReactivePropertyBase::valueChanged,
                        [widget, property_name, state]() {
                            auto value = state->get();
                            widget->setProperty(
                                property_name.toUtf8().constData(), value);
                        });

                // **Set initial value**
                widget->setProperty(property_name.toUtf8().constData(),
                                    state->get());
            }
        } catch (const std::exception &e) {
            qWarning() << "Property binding failed:" << property_name
                       << "to state" << state_key << ":" << e.what();
        }
    }
}

QVariant JSONUILoader::convertJSONValue(const QJsonValue &value,
                                        const QString &property_type) {
    // **Check for custom converter**
    if (!property_type.isEmpty()) {
        auto converter_it = property_converters_.find(property_type);
        if (converter_it != property_converters_.end()) {
            return converter_it->second(value);
        }
    }

    // **Default conversion**
    switch (value.type()) {
        case QJsonValue::Bool:
            return value.toBool();
        case QJsonValue::Double:
            return value.toDouble();
        case QJsonValue::String:
            return value.toString();
        case QJsonValue::Array: {
            QJsonArray array = value.toArray();
            QVariantList list;
            for (const QJsonValue &item : array) {
                list.append(convertJSONValue(item));
            }
            return list;
        }
        case QJsonValue::Object: {
            QJsonObject obj = value.toObject();
            QVariantMap map;
            for (auto it = obj.begin(); it != obj.end(); ++it) {
                map[it.key()] = convertJSONValue(it.value());
            }
            return map;
        }
        default:
            return QVariant{};
    }
}

bool JSONUILoader::validateWidgetObject(
    const QJsonObject &widget_object) const {
    // **Additional validation logic**
    return true;  // Simplified for brevity
}

bool JSONUILoader::validatePropertyValue(const QString &property_name,
                                         const QJsonValue &value) const {
    // **Property-specific validation**
    return true;  // Simplified for brevity
}

void JSONUILoader::bindStateManager(
    std::shared_ptr<Binding::StateManager> state_manager) {
    state_manager_ = std::move(state_manager);
}

void JSONUILoader::registerEventHandler(const QString &handler_name,
                                        std::function<void()> handler) {
    event_handlers_[handler_name] = std::move(handler);
}

void JSONUILoader::registerPropertyConverter(
    const QString &property_type,
    std::function<QVariant(const QJsonValue &)> converter) {
    property_converters_[property_type] = std::move(converter);
}

}  // namespace DeclarativeUI::JSON
