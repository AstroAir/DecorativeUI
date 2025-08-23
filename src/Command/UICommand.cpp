#include "UICommand.hpp"
#include <QDebug>
#include <QJsonDocument>
#include <algorithm>

namespace DeclarativeUI::Command::UI {

// **UICommandState implementation**
UICommandState::UICommandState(QObject* parent) : QObject(parent) {
    qDebug() << "🎯 UICommandState created";
}

bool UICommandState::hasProperty(const QString& name) const {
    return properties_.find(name) != properties_.end();
}

void UICommandState::removeProperty(const QString& name) {
    auto it = properties_.find(name);
    if (it != properties_.end()) {
        properties_.erase(it);
        emit stateChanged();
    }
}

QStringList UICommandState::getPropertyNames() const {
    QStringList names;
    for (const auto& [name, value] : properties_) {
        names.append(name);
    }
    return names;
}

void UICommandState::setValidator(
    const QString& property, std::function<bool(const QVariant&)> validator) {
    validators_[property] = validator;
}

bool UICommandState::validateProperty(const QString& property,
                                      const QVariant& value) const {
    auto it = validators_.find(property);
    if (it != validators_.end()) {
        return it->second(value);
    }
    return true;  // No validator means always valid
}

bool UICommandState::validateAllProperties() const {
    for (const auto& [name, value] : properties_) {
        if (!validateProperty(name, value)) {
            return false;
        }
    }
    return true;
}

QJsonObject UICommandState::toJson() const {
    QJsonObject json;
    for (const auto& [name, value] : properties_) {
        // Convert QVariant to JSON-compatible value
        if (value.typeId() == QMetaType::QString) {
            json[name] = value.toString();
        } else if (value.typeId() == QMetaType::Int) {
            json[name] = value.toInt();
        } else if (value.typeId() == QMetaType::Double) {
            json[name] = value.toDouble();
        } else if (value.typeId() == QMetaType::Bool) {
            json[name] = value.toBool();
        } else {
            json[name] = value.toString();  // Fallback to string
        }
    }
    return json;
}

void UICommandState::fromJson(const QJsonObject& json) {
    for (auto it = json.begin(); it != json.end(); ++it) {
        const QString& key = it.key();
        const QJsonValue& value = it.value();

        QVariant variant;
        if (value.isString()) {
            variant = value.toString();
        } else if (value.isDouble()) {
            variant = value.toDouble();
        } else if (value.isBool()) {
            variant = value.toBool();
        } else {
            variant = value.toVariant();
        }

        properties_[key] = variant;
    }
    emit stateChanged();
}

bool UICommandState::equals(const UICommandState& other) const {
    if (properties_.size() != other.properties_.size()) {
        return false;
    }

    for (const auto& [name, value] : properties_) {
        auto it = other.properties_.find(name);
        if (it == other.properties_.end() || it->second != value) {
            return false;
        }
    }
    return true;
}

QStringList UICommandState::getDifferences(const UICommandState& other) const {
    QStringList differences;

    // Check for properties that differ or are missing in other
    for (const auto& [name, value] : properties_) {
        auto it = other.properties_.find(name);
        if (it == other.properties_.end()) {
            differences.append(
                QString("Property '%1' missing in other").arg(name));
        } else if (it->second != value) {
            differences.append(
                QString("Property '%1' differs: %2 vs %3")
                    .arg(name, value.toString(), it->second.toString()));
        }
    }

    // Check for properties that exist in other but not in this
    for (const auto& [name, value] : other.properties_) {
        if (properties_.find(name) == properties_.end()) {
            differences.append(
                QString("Property '%1' missing in this").arg(name));
        }
    }

    return differences;
}

void UICommandState::clearProperties() {
    QStringList propertyNames;
    for (const auto& [name, value] : properties_) {
        propertyNames.append(name);
    }

    properties_.clear();

    // Emit signals for each removed property
    for (const QString& property : propertyNames) {
        emit propertyRemoved(property);
    }
}

// **BaseUICommand implementation**
BaseUICommand::BaseUICommand(QObject* parent)
    : QObject(parent),
      state_(std::make_unique<UICommandState>(this)),
      id_(QUuid::createUuid()) {
    // Connect state changes to our signals
    connect(state_.get(), &UICommandState::propertyChanged, this,
            &BaseUICommand::propertyChanged);
    connect(state_.get(), &UICommandState::stateChanged, this,
            &BaseUICommand::stateChanged);

    qDebug() << "🎯 BaseUICommand created with ID:" << id_.toString();
}

void BaseUICommand::onWidgetCreated(QWidget* widget) {
    if (!widget) {
        qWarning() << "Widget is null in onWidgetCreated";
        return;
    }

    qDebug() << "🔗 Widget created for command:" << getCommandType();

    // Sync current state to the new widget
    syncToWidget();
}

void BaseUICommand::onWidgetDestroyed() {
    qDebug() << "🔌 Widget destroyed for command:" << getCommandType();
    widget_ = nullptr;
}

void BaseUICommand::syncToWidget() {
    if (!widget_) {
        return;
    }

    // Sync all properties from command state to widget
    const auto propertyNames = state_->getPropertyNames();
    for (const QString& propertyName : propertyNames) {
        QVariant value = state_->getProperty<QVariant>(propertyName);
        widget_->setProperty(propertyName.toUtf8().constData(), value);
    }

    qDebug() << "📤 Synced" << propertyNames.size() << "properties to widget";
}

void BaseUICommand::syncFromWidget() {
    if (!widget_) {
        return;
    }

    // This would typically sync specific properties back from widget
    // Implementation depends on specific widget types and their properties
    qDebug() << "📥 Synced properties from widget";
}

void BaseUICommand::handleEvent(const QString& eventType,
                                const QVariant& eventData) {
    auto it = event_handlers_.find(eventType);
    if (it != event_handlers_.end()) {
        it->second(eventData);
    }

    emit eventTriggered(eventType, eventData);
    qDebug() << "⚡ Event handled:" << eventType;
}

void BaseUICommand::bindToState(const QString& stateKey,
                                const QString& property) {
    QString prop = property.isEmpty() ? "value" : property;
    state_bindings_[prop] = stateKey;
    connectToStateManager();
}

void BaseUICommand::unbindFromState(const QString& property) {
    if (property.isEmpty()) {
        // Unbind all state bindings
        state_bindings_.clear();
        disconnectFromStateManager();
        qDebug() << "🔌 Unbound all state bindings for command:"
                 << getCommandType();
    } else {
        // Unbind specific property
        auto it = state_bindings_.find(property);
        if (it != state_bindings_.end()) {
            state_bindings_.erase(it);
            qDebug() << "🔌 Unbound state binding for property:" << property;
        }
    }
}

void BaseUICommand::addChild(std::shared_ptr<BaseUICommand> child) {
    if (!child || child.get() == this) {
        return;
    }

    // Remove from previous parent if any
    if (child->parent_) {
        child->parent_->removeChild(child);
    }

    children_.push_back(child);
    child->setParent(this);
    emit childAdded(child);

    qDebug() << "👶 Child added to command:" << getCommandType();
}

void BaseUICommand::removeChild(std::shared_ptr<BaseUICommand> child) {
    auto it = std::find(children_.begin(), children_.end(), child);
    if (it != children_.end()) {
        (*it)->setParent(nullptr);
        children_.erase(it);
        emit childRemoved(child);

        qDebug() << "👋 Child removed from command:" << getCommandType();
    }
}

std::vector<std::shared_ptr<BaseUICommand>> BaseUICommand::getChildren() const {
    return children_;
}

BaseUICommand* BaseUICommand::getParent() const { return parent_; }

void BaseUICommand::updateBoundProperties() {
    for (const auto& [property, binding] : property_bindings_) {
        try {
            QVariant value = binding();
            state_->setProperty(property, value);
        } catch (const std::exception& e) {
            qWarning() << "Property binding failed for" << property << ":"
                       << e.what();
        }
    }
}

void BaseUICommand::connectToStateManager() {
    // Get state manager instance for future use
    [[maybe_unused]] auto& stateManager =
        DeclarativeUI::Binding::StateManager::instance();

    for (const auto& [property, stateKey] : state_bindings_) {
        // This would connect to state manager changes
        // Implementation depends on StateManager interface
        qDebug() << "🔗 Connected property" << property << "to state"
                 << stateKey;
    }
}

void BaseUICommand::disconnectFromStateManager() {
    // Disconnect from state manager
    qDebug() << "🔌 Disconnected from state manager";
}

void BaseUICommand::setWidget(QWidget* widget) {
    if (widget_ != widget) {
        if (widget_) {
            onWidgetDestroyed();
        }

        widget_ = widget;

        if (widget_) {
            onWidgetCreated(widget_);
        }
    }
}

void BaseUICommand::setParent(BaseUICommand* parent) { parent_ = parent; }

}  // namespace DeclarativeUI::Command::UI
