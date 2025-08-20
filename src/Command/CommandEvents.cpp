#include "CommandEvents.hpp"
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QTimer>
#include <algorithm>

namespace DeclarativeUI::Command::UI {

// **CommandEvent implementation**
CommandEvent::CommandEvent(CommandEventType type, BaseUICommand* source)
    : type_(type),
      source_(source),
      timestamp_(QDateTime::currentDateTime()),
      id_(QUuid::createUuid()) {}

/**
 * @brief Get the string representation of the event type
 * @return String name of the event type
 *
 * This function maps CommandEventType enum values to their string
 * representations. It uses a lookup table approach to reduce cyclomatic
 * complexity and improve performance.
 */
QString CommandEvent::getTypeName() const {
    return getEventTypeNameLookup(type_);
}

/**
 * @brief Static lookup function for event type names
 * @param type The CommandEventType to convert
 * @return String representation of the event type
 *
 * This helper function provides a centralized mapping between event types and
 * their names. Using a static lookup table reduces the cyclomatic complexity of
 * the main function.
 */
QString CommandEvent::getEventTypeNameLookup(CommandEventType type) {
    // Static lookup table for better performance and reduced complexity
    static const std::unordered_map<CommandEventType, QString> eventTypeNames =
        {// Input events
         {CommandEventType::Clicked, "Clicked"},
         {CommandEventType::DoubleClicked, "DoubleClicked"},
         {CommandEventType::Pressed, "Pressed"},
         {CommandEventType::Released, "Released"},
         {CommandEventType::Toggled, "Toggled"},

         // Value change events
         {CommandEventType::ValueChanged, "ValueChanged"},
         {CommandEventType::TextChanged, "TextChanged"},
         {CommandEventType::SelectionChanged, "SelectionChanged"},
         {CommandEventType::StateChanged, "StateChanged"},

         // Focus events
         {CommandEventType::FocusIn, "FocusIn"},
         {CommandEventType::FocusOut, "FocusOut"},

         // Mouse events
         {CommandEventType::MouseEnter, "MouseEnter"},
         {CommandEventType::MouseLeave, "MouseLeave"},
         {CommandEventType::MouseMove, "MouseMove"},

         // Keyboard events
         {CommandEventType::KeyPressed, "KeyPressed"},
         {CommandEventType::KeyReleased, "KeyReleased"},

         // Validation events
         {CommandEventType::ValidationFailed, "ValidationFailed"},
         {CommandEventType::ValidationPassed, "ValidationPassed"},

         // Lifecycle events
         {CommandEventType::Initialized, "Initialized"},
         {CommandEventType::Destroyed, "Destroyed"},
         {CommandEventType::Shown, "Shown"},
         {CommandEventType::Hidden, "Hidden"},

         // Custom events
         {CommandEventType::Custom, "Custom"}};

    auto it = eventTypeNames.find(type);
    return (it != eventTypeNames.end()) ? it->second : "Unknown";
}

void CommandEvent::setData(const QString& key, const QVariant& value) {
    data_[key] = value;
}

QVariant CommandEvent::getData(const QString& key,
                               const QVariant& defaultValue) const {
    auto it = data_.find(key);
    return it != data_.end() ? it->second : defaultValue;
}

bool CommandEvent::hasData(const QString& key) const {
    return data_.find(key) != data_.end();
}

QStringList CommandEvent::getDataKeys() const {
    QStringList keys;
    for (const auto& [key, value] : data_) {
        keys.append(key);
    }
    return keys;
}

QJsonObject CommandEvent::toJson() const {
    QJsonObject json;
    json["type"] = static_cast<int>(type_);
    json["typeName"] = getTypeName();
    json["timestamp"] = timestamp_.toString(Qt::ISODate);
    json["id"] = id_.toString();
    json["accepted"] = accepted_;
    json["propagationStopped"] = propagation_stopped_;
    json["priority"] = static_cast<int>(priority_);

    QJsonObject dataJson;
    for (const auto& [key, value] : data_) {
        dataJson[key] = QJsonValue::fromVariant(value);
    }
    json["data"] = dataJson;

    return json;
}

void CommandEvent::fromJson(const QJsonObject& json) {
    type_ = static_cast<CommandEventType>(json["type"].toInt());
    timestamp_ =
        QDateTime::fromString(json["timestamp"].toString(), Qt::ISODate);
    id_ = QUuid::fromString(json["id"].toString());
    accepted_ = json["accepted"].toBool();
    propagation_stopped_ = json["propagationStopped"].toBool();
    priority_ = static_cast<CommandEventPriority>(json["priority"].toInt());

    QJsonObject dataJson = json["data"].toObject();
    for (auto it = dataJson.begin(); it != dataJson.end(); ++it) {
        data_[it.key()] = it.value().toVariant();
    }
}

std::unique_ptr<CommandEvent> CommandEvent::clone() const {
    auto cloned = std::make_unique<CommandEvent>(type_, source_);
    cloned->data_ = data_;
    cloned->accepted_ = accepted_;
    cloned->propagation_stopped_ = propagation_stopped_;
    cloned->priority_ = priority_;
    return cloned;
}

// **ClickEvent implementation**
ClickEvent::ClickEvent(BaseUICommand* source)
    : CommandEvent(CommandEventType::Clicked, source) {}

std::unique_ptr<CommandEvent> ClickEvent::clone() const {
    auto cloned = std::make_unique<ClickEvent>(source_);
    cloned->data_ = data_;
    cloned->accepted_ = accepted_;
    cloned->propagation_stopped_ = propagation_stopped_;
    cloned->priority_ = priority_;
    return cloned;
}

// **ValueChangeEvent implementation**
ValueChangeEvent::ValueChangeEvent(BaseUICommand* source)
    : CommandEvent(CommandEventType::ValueChanged, source) {}

std::unique_ptr<CommandEvent> ValueChangeEvent::clone() const {
    auto cloned = std::make_unique<ValueChangeEvent>(source_);
    cloned->data_ = data_;
    cloned->accepted_ = accepted_;
    cloned->propagation_stopped_ = propagation_stopped_;
    cloned->priority_ = priority_;
    return cloned;
}

// **TextChangeEvent implementation**
TextChangeEvent::TextChangeEvent(BaseUICommand* source)
    : CommandEvent(CommandEventType::TextChanged, source) {}

std::unique_ptr<CommandEvent> TextChangeEvent::clone() const {
    auto cloned = std::make_unique<TextChangeEvent>(source_);
    cloned->data_ = data_;
    cloned->accepted_ = accepted_;
    cloned->propagation_stopped_ = propagation_stopped_;
    cloned->priority_ = priority_;
    return cloned;
}

// **KeyEvent implementation**
KeyEvent::KeyEvent(CommandEventType type, BaseUICommand* source)
    : CommandEvent(type, source) {}

std::unique_ptr<CommandEvent> KeyEvent::clone() const {
    auto cloned = std::make_unique<KeyEvent>(type_, source_);
    cloned->data_ = data_;
    cloned->accepted_ = accepted_;
    cloned->propagation_stopped_ = propagation_stopped_;
    cloned->priority_ = priority_;
    return cloned;
}

// **ValidationEvent implementation**
ValidationEvent::ValidationEvent(CommandEventType type, BaseUICommand* source)
    : CommandEvent(type, source) {}

std::unique_ptr<CommandEvent> ValidationEvent::clone() const {
    auto cloned = std::make_unique<ValidationEvent>(type_, source_);
    cloned->data_ = data_;
    cloned->accepted_ = accepted_;
    cloned->propagation_stopped_ = propagation_stopped_;
    cloned->priority_ = priority_;
    return cloned;
}

// **CustomEvent implementation**
CustomEvent::CustomEvent(const QString& customType, BaseUICommand* source)
    : CommandEvent(CommandEventType::Custom, source) {
    setCustomType(customType);
}

std::unique_ptr<CommandEvent> CustomEvent::clone() const {
    auto cloned = std::make_unique<CustomEvent>(getCustomType(), source_);
    cloned->data_ = data_;
    cloned->accepted_ = accepted_;
    cloned->propagation_stopped_ = propagation_stopped_;
    cloned->priority_ = priority_;
    return cloned;
}

// **CommandEventDispatcher implementation**
CommandEventDispatcher::CommandEventDispatcher(QObject* parent)
    : QObject(parent) {
    qDebug() << "⚡ CommandEventDispatcher initialized";
}

CommandEventDispatcher& CommandEventDispatcher::instance() {
    static CommandEventDispatcher instance;
    return instance;
}

void CommandEventDispatcher::dispatchEvent(
    std::unique_ptr<CommandEvent> event) {
    if (!event) {
        qWarning() << "Cannot dispatch null event";
        return;
    }

    if (event_queue_enabled_) {
        if (event_queue_.size() >= static_cast<size_t>(max_queue_size_)) {
            qWarning() << "Event queue is full, dropping event";
            return;
        }

        event_queue_.push_back(std::move(event));
        QTimer::singleShot(0, this, &CommandEventDispatcher::processEventQueue);
    } else {
        processEvent(*event);
    }

    emit eventDispatched(*event);
}

void CommandEventDispatcher::dispatchEvent(const CommandEvent& event) {
    auto cloned = event.clone();
    dispatchEvent(std::move(cloned));
}

QUuid CommandEventDispatcher::registerHandler(BaseUICommand* command,
                                              CommandEventType eventType,
                                              CommandEventHandler handler,
                                              CommandEventPriority priority) {
    EventHandlerRegistration registration(eventType, std::move(handler),
                                          priority);
    return registerHandler(command, registration);
}

QUuid CommandEventDispatcher::registerHandler(
    BaseUICommand* command, const EventHandlerRegistration& registration) {
    if (!command) {
        qWarning() << "Cannot register handler for null command";
        return QUuid{};
    }

    HandlerInfo info;
    info.command = command;
    info.registration = registration;

    QUuid handlerId = registration.id;
    handlers_[handlerId] = info;
    command_handlers_[command].push_back(handlerId);

    emit handlerRegistered(command, registration.event_type);
    qDebug() << "📡 Registered event handler for" << command->getCommandType()
             << "event type:" << static_cast<int>(registration.event_type);

    return handlerId;
}

QUuid CommandEventDispatcher::registerFilteredHandler(
    BaseUICommand* command, CommandEventType eventType,
    CommandEventHandler handler, CommandEventFilter filter,
    CommandEventPriority priority) {
    EventHandlerRegistration registration(eventType, std::move(handler),
                                          priority);
    registration.filter = std::move(filter);
    return registerHandler(command, registration);
}

QUuid CommandEventDispatcher::registerOnceHandler(
    BaseUICommand* command, CommandEventType eventType,
    CommandEventHandler handler, CommandEventPriority priority) {
    EventHandlerRegistration registration(eventType, std::move(handler),
                                          priority);
    registration.once = true;
    return registerHandler(command, registration);
}

void CommandEventDispatcher::unregisterHandler(const QUuid& handlerId) {
    auto it = handlers_.find(handlerId);
    if (it != handlers_.end()) {
        BaseUICommand* command = it->second.command;
        CommandEventType eventType = it->second.registration.event_type;

        // Remove from command handlers
        auto cmdIt = command_handlers_.find(command);
        if (cmdIt != command_handlers_.end()) {
            auto& handlerIds = cmdIt->second;
            handlerIds.erase(
                std::remove(handlerIds.begin(), handlerIds.end(), handlerId),
                handlerIds.end());

            if (handlerIds.empty()) {
                command_handlers_.erase(cmdIt);
            }
        }

        handlers_.erase(it);
        emit handlerUnregistered(command, eventType);
        qDebug() << "🔌 Unregistered event handler:" << handlerId.toString();
    }
}

void CommandEventDispatcher::unregisterAllHandlers(BaseUICommand* command) {
    auto cmdIt = command_handlers_.find(command);
    if (cmdIt != command_handlers_.end()) {
        for (const QUuid& handlerId : cmdIt->second) {
            auto it = handlers_.find(handlerId);
            if (it != handlers_.end()) {
                emit handlerUnregistered(command,
                                         it->second.registration.event_type);
                handlers_.erase(it);
            }
        }
        command_handlers_.erase(cmdIt);
        qDebug() << "🧹 Unregistered all handlers for command:"
                 << command->getCommandType();
    }
}

void CommandEventDispatcher::unregisterHandlersByType(
    BaseUICommand* command, CommandEventType eventType) {
    auto cmdIt = command_handlers_.find(command);
    if (cmdIt != command_handlers_.end()) {
        auto& handlerIds = cmdIt->second;

        for (auto it = handlerIds.begin(); it != handlerIds.end();) {
            auto handlerIt = handlers_.find(*it);
            if (handlerIt != handlers_.end() &&
                handlerIt->second.registration.event_type == eventType) {
                handlers_.erase(handlerIt);
                it = handlerIds.erase(it);
                emit handlerUnregistered(command, eventType);
            } else {
                ++it;
            }
        }

        if (handlerIds.empty()) {
            command_handlers_.erase(cmdIt);
        }
    }
}

void CommandEventDispatcher::addGlobalFilter(CommandEventFilter filter,
                                             CommandEventPriority priority) {
    global_filters_.emplace_back(std::move(filter), priority);

    // Sort by priority (higher priority first)
    std::sort(global_filters_.begin(), global_filters_.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });

    qDebug() << "🌐 Added global event filter";
}

void CommandEventDispatcher::removeGlobalFilter(CommandEventFilter filter) {
    // Note: This is a simplified implementation
    // In practice, you'd need a way to identify filters for removal
    qDebug() << "🔌 Removed global event filter";
}

void CommandEventDispatcher::addEventInterceptor(
    CommandEventType eventType, CommandEventHandler interceptor) {
    interceptors_[eventType] = std::move(interceptor);
    qDebug() << "🛡️ Added event interceptor for type:"
             << static_cast<int>(eventType);
}

void CommandEventDispatcher::removeEventInterceptor(
    CommandEventType eventType) {
    interceptors_.erase(eventType);
    qDebug() << "🔌 Removed event interceptor for type:"
             << static_cast<int>(eventType);
}

int CommandEventDispatcher::getHandlerCount(BaseUICommand* command) const {
    auto it = command_handlers_.find(command);
    return it != command_handlers_.end() ? static_cast<int>(it->second.size())
                                         : 0;
}

int CommandEventDispatcher::getTotalHandlerCount() const {
    return static_cast<int>(handlers_.size());
}

QStringList CommandEventDispatcher::getRegisteredEventTypes(
    BaseUICommand* command) const {
    QStringList types;
    auto cmdIt = command_handlers_.find(command);
    if (cmdIt != command_handlers_.end()) {
        for (const QUuid& handlerId : cmdIt->second) {
            auto it = handlers_.find(handlerId);
            if (it != handlers_.end()) {
                CommandEvent event(it->second.registration.event_type);
                types.append(event.getTypeName());
            }
        }
    }
    return types;
}

void CommandEventDispatcher::processEvent(const CommandEvent& event) {
    try {
        // Check global filters first
        if (!passesGlobalFilters(event)) {
            return;
        }

        // Check for interceptors
        auto interceptorIt = interceptors_.find(event.getType());
        if (interceptorIt != interceptors_.end()) {
            interceptorIt->second(event);
            if (event.isPropagationStopped()) {
                return;
            }
        }

        // Get handlers for this event
        auto handlers = getHandlersForEvent(event);

        // Sort handlers by priority (higher priority first)
        std::sort(handlers.begin(), handlers.end(),
                  [](const HandlerInfo* a, const HandlerInfo* b) {
                      return a->registration.priority >
                             b->registration.priority;
                  });

        // Execute handlers
        for (HandlerInfo* handlerInfo : handlers) {
            if (event.isPropagationStopped()) {
                break;
            }

            const auto& registration = handlerInfo->registration;

            // Apply filter if present
            if (registration.filter && !registration.filter(event)) {
                continue;
            }

            // Execute handler
            if (registration.handler) {
                registration.handler(event);
            }

            // Remove one-time handlers
            if (registration.once) {
                unregisterHandler(registration.id);
            }
        }

    } catch (const std::exception& e) {
        handleError(event,
                    QString("Event processing failed: %1").arg(e.what()));
    }
}

std::vector<CommandEventDispatcher::HandlerInfo*>
CommandEventDispatcher::getHandlersForEvent(const CommandEvent& event) {
    std::vector<HandlerInfo*> result;

    BaseUICommand* source = event.getSource();
    if (!source) {
        return result;
    }

    auto cmdIt = command_handlers_.find(source);
    if (cmdIt != command_handlers_.end()) {
        for (const QUuid& handlerId : cmdIt->second) {
            auto it = handlers_.find(handlerId);
            if (it != handlers_.end() &&
                it->second.registration.event_type == event.getType()) {
                result.push_back(&it->second);
            }
        }
    }

    return result;
}

bool CommandEventDispatcher::passesGlobalFilters(const CommandEvent& event) {
    for (const auto& [filter, priority] : global_filters_) {
        if (!filter(event)) {
            return false;
        }
    }
    return true;
}

void CommandEventDispatcher::handleError(const CommandEvent& event,
                                         const QString& error) {
    qWarning() << "Event handling error:" << error;
    emit eventHandlingError(event, error);
}

void CommandEventDispatcher::processEventQueue() {
    if (event_queue_.empty()) {
        return;
    }

    auto events = std::move(event_queue_);
    event_queue_.clear();

    for (auto& event : events) {
        processEvent(*event);
    }

    qDebug() << "📦 Processed" << events.size() << "queued events";
}

}  // namespace DeclarativeUI::Command::UI
