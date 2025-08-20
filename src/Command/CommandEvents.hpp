#pragma once

#include <QDateTime>
#include <QEvent>
#include <QObject>
#include <QString>
#include <QUuid>
#include <QVariant>
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

#include "UICommand.hpp"

// **Hash specialization for QUuid to use with std::unordered_map**
namespace std {
template <>
struct hash<QUuid> {
    std::size_t operator()(const QUuid& uuid) const noexcept {
        return qHash(uuid);
    }
};
}  // namespace std

namespace DeclarativeUI::Command::UI {

// **Forward declarations**
class CommandEvent;
class CommandEventDispatcher;

// **Command Event Types**
enum class CommandEventType {
    // Input events
    Clicked = 1000,
    DoubleClicked,
    Pressed,
    Released,
    Toggled,

    // Value change events
    ValueChanged,
    TextChanged,
    SelectionChanged,
    StateChanged,

    // Focus events
    FocusIn,
    FocusOut,

    // Mouse events
    MouseEnter,
    MouseLeave,
    MouseMove,

    // Keyboard events
    KeyPressed,
    KeyReleased,

    // Validation events
    ValidationFailed,
    ValidationPassed,

    // Lifecycle events
    Initialized,
    Destroyed,
    Shown,
    Hidden,

    // Custom events
    Custom = 2000
};

// **Command Event Priority**
enum class CommandEventPriority { Low = 0, Normal = 1, High = 2, Critical = 3 };

// **Command Event - abstract base for all command events**
class CommandEvent {
public:
    explicit CommandEvent(CommandEventType type,
                          BaseUICommand* source = nullptr);
    virtual ~CommandEvent() = default;

    // **Event properties**
    CommandEventType getType() const { return type_; }
    QString getTypeName() const;
    BaseUICommand* getSource() const { return source_; }

    // **Static helper functions**
    static QString getEventTypeNameLookup(CommandEventType type);
    QDateTime getTimestamp() const { return timestamp_; }
    QUuid getId() const { return id_; }

    // **Event data**
    void setData(const QString& key, const QVariant& value);
    QVariant getData(const QString& key,
                     const QVariant& defaultValue = QVariant{}) const;
    bool hasData(const QString& key) const;
    QStringList getDataKeys() const;

    // **Event control**
    void accept() { accepted_ = true; }
    void ignore() { accepted_ = false; }
    bool isAccepted() const { return accepted_; }

    void stopPropagation() { propagation_stopped_ = true; }
    bool isPropagationStopped() const { return propagation_stopped_; }

    // **Priority and filtering**
    void setPriority(CommandEventPriority priority) { priority_ = priority; }
    CommandEventPriority getPriority() const { return priority_; }

    // **Serialization**
    virtual QJsonObject toJson() const;
    virtual void fromJson(const QJsonObject& json);

    // **Cloning**
    virtual std::unique_ptr<CommandEvent> clone() const;

protected:
    CommandEventType type_;
    BaseUICommand* source_;
    QDateTime timestamp_;
    QUuid id_;
    std::unordered_map<QString, QVariant> data_;
    bool accepted_ = false;
    bool propagation_stopped_ = false;
    CommandEventPriority priority_ = CommandEventPriority::Normal;
};

// **Specific Command Event Types**

// **Click Event**
class ClickEvent : public CommandEvent {
public:
    explicit ClickEvent(BaseUICommand* source = nullptr);

    void setButton(int button) { setData("button", button); }
    int getButton() const { return getData("button", 1).toInt(); }

    void setModifiers(int modifiers) { setData("modifiers", modifiers); }
    int getModifiers() const { return getData("modifiers", 0).toInt(); }

    void setPosition(const QPoint& position) { setData("position", position); }
    QPoint getPosition() const {
        return getData("position", QPoint{}).toPoint();
    }

    // Aliases for test compatibility
    void setMouseButton(int button) { setButton(button); }
    int getMouseButton() const { return getButton(); }
    void setButtonPosition(const QPoint& position) { setPosition(position); }
    QPoint getButtonPosition() const { return getPosition(); }

    std::unique_ptr<CommandEvent> clone() const override;
};

// **Value Change Event**
class ValueChangeEvent : public CommandEvent {
public:
    explicit ValueChangeEvent(BaseUICommand* source = nullptr);

    void setOldValue(const QVariant& value) { setData("oldValue", value); }
    QVariant getOldValue() const { return getData("oldValue"); }

    void setNewValue(const QVariant& value) { setData("newValue", value); }
    QVariant getNewValue() const { return getData("newValue"); }

    std::unique_ptr<CommandEvent> clone() const override;
};

// **Text Change Event**
class TextChangeEvent : public CommandEvent {
public:
    explicit TextChangeEvent(BaseUICommand* source = nullptr);

    void setOldText(const QString& text) { setData("oldText", text); }
    QString getOldText() const { return getData("oldText").toString(); }

    void setNewText(const QString& text) { setData("newText", text); }
    QString getNewText() const { return getData("newText").toString(); }

    void setCursorPosition(int position) {
        setData("cursorPosition", position);
    }
    int getCursorPosition() const {
        return getData("cursorPosition", -1).toInt();
    }

    std::unique_ptr<CommandEvent> clone() const override;
};

// **Key Event**
class KeyEvent : public CommandEvent {
public:
    explicit KeyEvent(CommandEventType type, BaseUICommand* source = nullptr);

    void setKey(int key) { setData("key", key); }
    int getKey() const { return getData("key", 0).toInt(); }

    void setModifiers(int modifiers) { setData("modifiers", modifiers); }
    int getModifiers() const { return getData("modifiers", 0).toInt(); }

    void setText(const QString& text) { setData("text", text); }
    QString getText() const { return getData("text").toString(); }

    void setAutoRepeat(bool autoRepeat) { setData("autoRepeat", autoRepeat); }
    bool isAutoRepeat() const { return getData("autoRepeat", false).toBool(); }

    std::unique_ptr<CommandEvent> clone() const override;
};

// **Validation Event**
class ValidationEvent : public CommandEvent {
public:
    explicit ValidationEvent(CommandEventType type,
                             BaseUICommand* source = nullptr);

    void setProperty(const QString& property) { setData("property", property); }
    QString getProperty() const { return getData("property").toString(); }

    void setValue(const QVariant& value) { setData("value", value); }
    QVariant getValue() const { return getData("value"); }

    void setErrorMessage(const QString& message) {
        setData("errorMessage", message);
    }
    QString getErrorMessage() const {
        return getData("errorMessage").toString();
    }

    std::unique_ptr<CommandEvent> clone() const override;
};

// **Custom Event**
class CustomEvent : public CommandEvent {
public:
    explicit CustomEvent(const QString& customType,
                         BaseUICommand* source = nullptr);

    void setCustomType(const QString& type) { setData("customType", type); }
    QString getCustomType() const { return getData("customType").toString(); }

    std::unique_ptr<CommandEvent> clone() const override;
};

// **Event Handler Function Types**
using CommandEventHandler = std::function<void(const CommandEvent&)>;
using CommandEventFilter = std::function<bool(const CommandEvent&)>;

// **Event Handler Registration**
struct EventHandlerRegistration {
    CommandEventType event_type;
    CommandEventHandler handler;
    CommandEventFilter filter;
    CommandEventPriority priority = CommandEventPriority::Normal;
    bool once = false;  // Execute only once
    QUuid id = QUuid::createUuid();

    EventHandlerRegistration() = default;
    EventHandlerRegistration(
        CommandEventType type, CommandEventHandler h,
        CommandEventPriority prio = CommandEventPriority::Normal)
        : event_type(type), handler(std::move(h)), priority(prio) {}
};

// **Command Event Dispatcher - manages event routing and handling**
class CommandEventDispatcher : public QObject {
    Q_OBJECT

public:
    explicit CommandEventDispatcher(QObject* parent = nullptr);
    virtual ~CommandEventDispatcher() = default;

    // **Event dispatching**
    void dispatchEvent(std::unique_ptr<CommandEvent> event);
    void dispatchEvent(const CommandEvent& event);

    // **Event handler registration**
    QUuid registerHandler(
        BaseUICommand* command, CommandEventType eventType,
        CommandEventHandler handler,
        CommandEventPriority priority = CommandEventPriority::Normal);
    QUuid registerHandler(BaseUICommand* command,
                          const EventHandlerRegistration& registration);

    // **Filtered event handlers**
    QUuid registerFilteredHandler(
        BaseUICommand* command, CommandEventType eventType,
        CommandEventHandler handler, CommandEventFilter filter,
        CommandEventPriority priority = CommandEventPriority::Normal);

    // **One-time event handlers**
    QUuid registerOnceHandler(
        BaseUICommand* command, CommandEventType eventType,
        CommandEventHandler handler,
        CommandEventPriority priority = CommandEventPriority::Normal);

    // **Handler management**
    void unregisterHandler(const QUuid& handlerId);
    void unregisterAllHandlers(BaseUICommand* command);
    void unregisterHandlersByType(BaseUICommand* command,
                                  CommandEventType eventType);

    // **Global event filters**
    void addGlobalFilter(
        CommandEventFilter filter,
        CommandEventPriority priority = CommandEventPriority::Normal);
    void removeGlobalFilter(CommandEventFilter filter);

    // **Event interception**
    void addEventInterceptor(CommandEventType eventType,
                             CommandEventHandler interceptor);
    void removeEventInterceptor(CommandEventType eventType);

    // **Configuration**
    void setEventQueueEnabled(bool enabled) { event_queue_enabled_ = enabled; }
    bool isEventQueueEnabled() const { return event_queue_enabled_; }

    void setMaxQueueSize(int size) { max_queue_size_ = size; }
    int getMaxQueueSize() const { return max_queue_size_; }

    // **Statistics and debugging**
    int getHandlerCount(BaseUICommand* command) const;
    int getTotalHandlerCount() const;
    QStringList getRegisteredEventTypes(BaseUICommand* command) const;

    // **Singleton access**
    static CommandEventDispatcher& instance();

signals:
    void eventDispatched(const CommandEvent& event);
    void handlerRegistered(BaseUICommand* command, CommandEventType eventType);
    void handlerUnregistered(BaseUICommand* command,
                             CommandEventType eventType);
    void eventHandlingError(const CommandEvent& event, const QString& error);

private:
    // **Handler storage**
    struct HandlerInfo {
        BaseUICommand* command;
        EventHandlerRegistration registration;
    };
    std::unordered_map<QUuid, HandlerInfo> handlers_;
    std::unordered_map<BaseUICommand*, std::vector<QUuid>> command_handlers_;

    // **Global filters and interceptors**
    std::vector<std::pair<CommandEventFilter, CommandEventPriority>>
        global_filters_;
    std::unordered_map<CommandEventType, CommandEventHandler> interceptors_;

    // **Event queue**
    bool event_queue_enabled_ = false;
    int max_queue_size_ = 1000;
    std::vector<std::unique_ptr<CommandEvent>> event_queue_;

    // **Helper methods**
    void processEvent(const CommandEvent& event);
    std::vector<HandlerInfo*> getHandlersForEvent(const CommandEvent& event);
    bool passesGlobalFilters(const CommandEvent& event);
    void handleError(const CommandEvent& event, const QString& error);

private slots:
    void processEventQueue();
};

// **Convenience functions for event handling**
namespace Events {

// **Event creation helpers**
inline std::unique_ptr<ClickEvent> createClickEvent(BaseUICommand* source) {
    return std::make_unique<ClickEvent>(source);
}

inline std::unique_ptr<ValueChangeEvent> createValueChangeEvent(
    BaseUICommand* source, const QVariant& oldValue, const QVariant& newValue) {
    auto event = std::make_unique<ValueChangeEvent>(source);
    event->setOldValue(oldValue);
    event->setNewValue(newValue);
    return event;
}

inline std::unique_ptr<TextChangeEvent> createTextChangeEvent(
    BaseUICommand* source, const QString& oldText, const QString& newText) {
    auto event = std::make_unique<TextChangeEvent>(source);
    event->setOldText(oldText);
    event->setNewText(newText);
    return event;
}

inline std::unique_ptr<CustomEvent> createCustomEvent(
    BaseUICommand* source, const QString& customType) {
    return std::make_unique<CustomEvent>(customType, source);
}

// **Event dispatching helpers**
inline void dispatch(std::unique_ptr<CommandEvent> event) {
    CommandEventDispatcher::instance().dispatchEvent(std::move(event));
}

inline void dispatchClick(BaseUICommand* source) {
    dispatch(createClickEvent(source));
}

inline void dispatchValueChange(BaseUICommand* source, const QVariant& oldValue,
                                const QVariant& newValue) {
    dispatch(createValueChangeEvent(source, oldValue, newValue));
}

// **Handler registration helpers**
inline QUuid onClick(BaseUICommand* command, std::function<void()> handler) {
    return CommandEventDispatcher::instance().registerHandler(
        command, CommandEventType::Clicked, [handler](const CommandEvent&) {
            if (handler)
                handler();
        });
}

inline QUuid onValueChanged(BaseUICommand* command,
                            std::function<void(const QVariant&)> handler) {
    return CommandEventDispatcher::instance().registerHandler(
        command, CommandEventType::ValueChanged,
        [handler](const CommandEvent& event) {
            if (handler) {
                auto valueEvent = static_cast<const ValueChangeEvent&>(event);
                handler(valueEvent.getNewValue());
            }
        });
}

inline QUuid onTextChanged(BaseUICommand* command,
                           std::function<void(const QString&)> handler) {
    return CommandEventDispatcher::instance().registerHandler(
        command, CommandEventType::TextChanged,
        [handler](const CommandEvent& event) {
            if (handler) {
                auto textEvent = static_cast<const TextChangeEvent&>(event);
                handler(textEvent.getNewText());
            }
        });
}

}  // namespace Events

}  // namespace DeclarativeUI::Command::UI
