#pragma once

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <QVariant>
#include <QWidget>

#include <functional>
#include <memory>

#include "../Binding/StateManager.hpp"

namespace DeclarativeUI::JSON {

/**
 * @file JSONUILoader.hpp
 * @brief High-level JSON → QWidget loader used by the DeclarativeUI system.
 *
 * This header declares JSONUILoader, a convenience class that maps a JSON
 * representation of a user interface into a live QWidget tree. The loader
 * provides:
 *  - loading from files, raw JSON strings, or in-memory QJsonObject,
 *  - validation entry points for JSON structure used by the loader,
 *  - binding to a shared StateManager for reactive property updates,
 *  - registration of named event handlers and custom property converters,
 *  - a recursive widget creation pipeline that applies properties, layouts,
 *    events and children.
 *
 * Error and diagnostic reporting is emitted via Qt signals (loadingStarted,
 * loadingFinished, loadingFailed). The loader is intended for use on the UI
 * thread; callers should ensure long-running includes/network fetches are
 * performed asynchronously if required.
 */

/**
 * @class JSONUILoader
 * @brief Loads a QWidget hierarchy from JSON description.
 *
 * The JSON structure expected by this loader is intentionally flexible but
 * generally follows the pattern:
 *
 * {
 *   "type": "QWidgetTypeName",
 *   "properties": { "geometry": "...", "text": "...", ... },
 *   "layout": { ... },
 *   "events": { "onClick": "handlerName", ... },
 *   "bindings": { "propertyName": { "stateKey": "..." } },
 *   "children": [ { childObject }, ... ]
 * }
 *
 * Responsibilities:
 *  - Construct concrete QWidget instances for declared "type" values (the
 *    concrete mapping is provided by the application or by built-in factories).
 *  - Apply properties using Qt meta-object system and optional registered
 *    property converters for complex types.
 *  - Install layouts and add children to parent widgets.
 *  - Wire named event handlers previously registered via registerEventHandler.
 *  - Connect property bindings through a shared Binding::StateManager instance
 *    when provided via bindStateManager().
 *
 * The loader itself does not own or manage application-level event handler
 * state beyond calling the registered std::function; clients should ensure
 * handlers remain valid for the lifetime they are used.
 */
class JSONUILoader : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Construct a loader.
     * @param parent Optional QObject parent for QObject lifetime management.
     *
     * Constructing the loader does not perform parsing. Use one of the load
     * methods to build a widget tree.
     */
    explicit JSONUILoader(QObject *parent = nullptr);
    ~JSONUILoader() override = default;

    /**
     * @brief Load UI from a JSON file on disk.
     * @param file_path Filesystem path to the JSON document describing the UI.
     * @return unique_ptr<QWidget> Root widget of the loaded UI or nullptr on
     * failure.
     *
     * Emits loadingStarted(file_path) at the start and either
     * loadingFinished(file_path) on success or loadingFailed(file_path, error)
     * on failure. The returned widget is not inserted into any parent; the
     * caller is responsible for ownership.
     */
    [[nodiscard]] std::unique_ptr<QWidget> loadFromFile(
        const QString &file_path);

    /**
     * @brief Load UI from a JSON string.
     * @param json_string Raw JSON document text.
     * @return unique_ptr<QWidget> Root widget or nullptr on failure.
     *
     * Emits loading lifecycle signals as loadFromFile.
     */
    [[nodiscard]] std::unique_ptr<QWidget> loadFromString(
        const QString &json_string);

    /**
     * @brief Load UI from an already-parsed QJsonObject.
     * @param json_object Top-level JSON object representing the UI.
     * @return unique_ptr<QWidget> Root widget or nullptr on failure.
     *
     * This overload is convenient for callers that have preprocessed the JSON
     * (e.g. resolved includes/references) and want direct loading.
     */
    [[nodiscard]] std::unique_ptr<QWidget> loadFromObject(
        const QJsonObject &json_object);

    /**
     * @brief Validate JSON structure for compatibility with the loader.
     * @param json_object JSON object to validate.
     * @return true if the JSON appears valid for this loader; false otherwise.
     *
     * This function performs syntactic/structural checks (presence of required
     * keys, basic type checks) but does not guarantee successful widget
     * creation (those errors are reported during load).
     */
    [[nodiscard]] bool validateJSON(const QJsonObject &json_object) const;

    /**
     * @brief Bind a shared StateManager for property bindings.
     * @param state_manager Shared pointer to a Binding::StateManager instance.
     *
     * When bound, the loader will create connections between widget properties
     * and state keys declared in "bindings" sections of the JSON. If not bound,
     * binding declarations are ignored and a warning may be emitted.
     */
    void bindStateManager(std::shared_ptr<Binding::StateManager> state_manager);

    /**
     * @brief Register a named event handler callable.
     * @param handler_name Name used in JSON event declarations (e.g.
     * "onClick").
     * @param handler Callable invoked when the event is triggered.
     *
     * Event handlers are stored by name; the loader will look up handler names
     * referenced in JSON "events" blocks and invoke the stored callable when
     * wiring signals to actions. The callable signature is application-defined;
     * this API uses a zero-argument std::function<void()> for simplicity — use
     * lambdas that capture contextual data when needed.
     */
    void registerEventHandler(const QString &handler_name,
                              std::function<void()> handler);

    /**
     * @brief Register a converter for custom property types.
     * @param property_type Identifier used in JSON to indicate the target type.
     * @param converter Callable that converts a QJsonValue into a QVariant
     * suitable for setting on a widget property.
     *
     * The loader uses Qt's meta-object system to set properties; when a
     * property's expected type is non-trivial (e.g. QSize, QColor, custom
     * enums) a converter can be supplied to produce a QVariant with the correct
     * type.
     */
    void registerPropertyConverter(
        const QString &property_type,
        std::function<QVariant(const QJsonValue &)> converter);

signals:
    /**
     * @brief Emitted when loading begins for a given source (file path or
     * label).
     * @param source Source identifier provided to load method.
     */
    void loadingStarted(const QString &source);

    /**
     * @brief Emitted when loading successfully completes.
     * @param source Source identifier.
     */
    void loadingFinished(const QString &source);

    /**
     * @brief Emitted when loading fails.
     * @param source Source identifier.
     * @param error Human-readable error description.
     */
    void loadingFailed(const QString &source, const QString &error);

private:
    std::shared_ptr<Binding::StateManager> state_manager_;
    std::unordered_map<QString, std::function<void()>> event_handlers_;
    std::unordered_map<QString, std::function<QVariant(const QJsonValue &)>>
        property_converters_;

    /**
     * @brief Recursively create a QWidget (and subtree) from a JSON object.
     * @param widget_object JSON object describing a single widget node.
     * @return unique_ptr<QWidget> created widget (ownership transferred to
     * caller).
     *
     * This method is the core of the loader pipeline. It:
     *  - resolves the declared "type" into a concrete QWidget instance,
     *  - applies properties via applyProperties(),
     *  - installs layouts via setupLayout(),
     *  - binds events via bindEvents(),
     *  - registers bindings via setupPropertyBindings(),
     *  - creates and adds children via addChildren().
     */
    std::unique_ptr<QWidget> createWidgetFromObject(
        const QJsonObject &widget_object);

    /**
     * @brief Apply properties from JSON to a widget using the Qt meta-object
     * system.
     * @param widget Target widget to configure.
     * @param properties JSON object containing property → value mappings.
     *
     * The function consults property_converters_ for typed conversions. Errors
     * while setting properties should be handled gracefully and may trigger
     * loadingFailed signals if fatal.
     */
    void applyProperties(QWidget *widget, const QJsonObject &properties);

    /**
     * @brief Bind event declarations to registered handlers/signals.
     * @param widget Widget whose events are being bound.
     * @param events JSON object mapping event names to handler identifiers.
     *
     * The loader uses the event_handlers_ map to resolve named handlers. If a
     * handler name is not found a warning is produced.
     */
    void bindEvents(QWidget *widget, const QJsonObject &events);

    /**
     * @brief Create and apply a layout for a parent widget from JSON config.
     * @param parent Parent widget that will receive the layout.
     * @param layout_config JSON object describing layout type and properties.
     *
     * Supports common Qt layouts and property-based configuration. Children are
     * still added via addChildren().
     */
    void setupLayout(QWidget *parent, const QJsonObject &layout_config);

    /**
     * @brief Instantiate and append child widgets to a parent.
     * @param parent Parent widget to which children will be added.
     * @param children JSON array of child widget descriptions.
     */
    void addChildren(QWidget *parent, const QJsonArray &children);

    /**
     * @brief Set up declarative property bindings between widget properties and
     * application state.
     * @param widget Widget whose properties are bound.
     * @param bindings JSON object describing bindings (property → binding
     * descriptor).
     *
     * When a StateManager is bound, this function will register observers/slots
     * so property updates reflect changes in the application state.
     */
    void setupPropertyBindings(QWidget *widget, const QJsonObject &bindings);

    /**
     * @brief Convert a raw QJsonValue into a QVariant suitable for
     * meta-property assignment.
     * @param value JSON value to convert.
     * @param property_type Optional textual hint indicating the desired target
     * type.
     * @return QVariant ready for assignment; an invalid QVariant indicates
     * failure.
     *
     * The method uses registered property_converters_ when a property_type hint
     * is provided or when the meta-type requires special handling.
     */
    QVariant convertJSONValue(const QJsonValue &value,
                              const QString &property_type = "");

    /**
     * @brief Validate a single widget JSON object for required/known fields.
     * @param widget_object JSON object to validate.
     * @return true if the object is valid for widget creation; false otherwise.
     */
    bool validateWidgetObject(const QJsonObject &widget_object) const;

    /**
     * @brief Validate an individual property value for basic type/coercion
     * compatibility.
     * @param property_name Name of the property being validated.
     * @param value JSON value for the property.
     * @return true if the value is acceptable; false if it will likely fail at
     * runtime.
     */
    bool validatePropertyValue(const QString &property_name,
                               const QJsonValue &value) const;
};

}  // namespace DeclarativeUI::JSON
