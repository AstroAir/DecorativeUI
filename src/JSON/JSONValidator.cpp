/**
 * @file JSONValidator.cpp
 * @brief Implementation of the JSON validation framework for DeclarativeUI
 *
 * This file provides comprehensive validation capabilities for UI JSON
 * definitions, including component structure validation, property type
 * checking, event handler validation, and binding expression validation. The
 * implementation focuses on maintainable code with low cyclomatic complexity
 * through helper functions.
 *
 * Key features:
 * - Component structure validation with depth limiting
 * - Property type and compatibility validation
 * - Event handler and binding validation
 * - Extensible validation rule system
 * - Detailed error reporting with path information
 *
 * @author DeclarativeUI Team
 * @version 1.0
 */

#include "JSONValidator.hpp"
#include <QDebug>
#include <QMetaObject>
#include <QWidget>
#include <algorithm>
#include "ComponentRegistry.hpp"

namespace DeclarativeUI::JSON {

// **ValidationResult Implementation**

/**
 * @brief Constructs a validation result with all necessary information
 * @param valid Whether the validation passed
 * @param sev Severity level of the validation result
 * @param msg Human-readable validation message
 * @param p JSON path where the validation occurred
 * @param rule Name of the validation rule that generated this result
 */
ValidationResult::ValidationResult(bool valid, ValidationSeverity sev,
                                   const QString &msg, const JSONPath &p,
                                   const QString &rule)
    : is_valid(valid), severity(sev), message(msg), path(p), rule_name(rule) {}

QString ValidationResult::toString() const {
    QString severity_str;
    switch (severity) {
        case ValidationSeverity::Info:
            severity_str = "INFO";
            break;
        case ValidationSeverity::Warning:
            severity_str = "WARNING";
            break;
        case ValidationSeverity::Error:
            severity_str = "ERROR";
            break;
        case ValidationSeverity::Critical:
            severity_str = "CRITICAL";
            break;
    }

    QString path_str = path.isEmpty() ? "root" : path.toString();
    QString rule_str =
        rule_name.isEmpty() ? "" : QString(" [%1]").arg(rule_name);

    return QString("[%1] %2: %3%4")
        .arg(severity_str, path_str, message, rule_str);
}

bool ValidationResult::isError() const {
    return severity == ValidationSeverity::Error ||
           severity == ValidationSeverity::Critical;
}

bool ValidationResult::isWarning() const {
    return severity == ValidationSeverity::Warning;
}

// **ValidationContext Implementation**

void ValidationContext::addResult(const ValidationResult &result) {
    results.push_back(result);

    if (result.isError()) {
        qWarning() << "Validation Error:" << result.toString();
    } else if (result.isWarning()) {
        qWarning() << "Validation Warning:" << result.toString();
    }
}

void ValidationContext::addError(const QString &message,
                                 const QString &rule_name) {
    addResult(ValidationResult(false, ValidationSeverity::Error, message,
                               current_path, rule_name));
}

void ValidationContext::addWarning(const QString &message,
                                   const QString &rule_name) {
    addResult(ValidationResult(false, ValidationSeverity::Warning, message,
                               current_path, rule_name));
}

void ValidationContext::addInfo(const QString &message,
                                const QString &rule_name) {
    addResult(ValidationResult(true, ValidationSeverity::Info, message,
                               current_path, rule_name));
}

bool ValidationContext::hasErrors() const {
    return std::any_of(results.begin(), results.end(),
                       [](const ValidationResult &r) { return r.isError(); });
}

bool ValidationContext::hasWarnings() const {
    return std::any_of(results.begin(), results.end(),
                       [](const ValidationResult &r) { return r.isWarning(); });
}

std::vector<ValidationResult> ValidationContext::getErrors() const {
    std::vector<ValidationResult> errors;
    std::copy_if(results.begin(), results.end(), std::back_inserter(errors),
                 [](const ValidationResult &r) { return r.isError(); });
    return errors;
}

std::vector<ValidationResult> ValidationContext::getWarnings() const {
    std::vector<ValidationResult> warnings;
    std::copy_if(results.begin(), results.end(), std::back_inserter(warnings),
                 [](const ValidationResult &r) { return r.isWarning(); });
    return warnings;
}

void ValidationContext::throwIfErrors() const {
    if (hasErrors()) {
        QStringList error_messages;
        for (const auto &error : getErrors()) {
            error_messages.append(error.toString());
        }

        throw Exceptions::JSONValidationException(
            error_messages.join("; ").toStdString());
    }
}

// **UIJSONValidator Implementation**

UIJSONValidator::UIJSONValidator() {
    initializeKnownComponents();
    initializeKnownProperties();
    schema_validator_ = std::make_unique<JSONSchemaValidator>();
}

/**
 * @brief Validates a complete UI definition JSON object
 * @param ui_definition The root UI definition object to validate
 * @return true if the UI definition is valid, false otherwise
 *
 * This is the main entry point for UI validation. It performs:
 * - Basic structure validation (requires 'type' property)
 * - Component structure validation
 * - Global validator execution
 * - Error accumulation and reporting
 */
bool UIJSONValidator::validate(const QJsonObject &ui_definition) {
    context_.results.clear();
    context_.root_object = ui_definition;
    context_.current_path = JSONPath();
    context_.current_depth = 0;

    try {
        // **Basic structure validation**
        if (!ui_definition.contains("type")) {
            context_.addError("Root object must have a 'type' property",
                              "structure");
            return false;
        }

        // **Validate as a component**
        bool valid =
            validateComponentStructure(ui_definition, context_.current_path);

        // **Run global validators**
        runGlobalValidators(ui_definition, context_.current_path);

        return valid && !context_.hasErrors();

    } catch (const std::exception &e) {
        context_.addError(QString("Validation exception: %1").arg(e.what()),
                          "exception");
        return false;
    }
}

bool UIJSONValidator::validateComponent(const QJsonObject &component,
                                        const QString &component_type) {
    JSONPath old_path = context_.current_path;

    try {
        if (!component_type.isEmpty()) {
            context_.current_path.append(component_type);
        }

        bool valid =
            validateComponentStructure(component, context_.current_path);

        context_.current_path = old_path;
        return valid;

    } catch (const std::exception &e) {
        context_.current_path = old_path;
        context_.addError(
            QString("Component validation failed: %1").arg(e.what()),
            "component");
        return false;
    }
}

bool UIJSONValidator::validateLayout(const QJsonObject &layout) {
    JSONPath old_path = context_.current_path;
    context_.current_path.append("layout");

    try {
        bool valid = validateLayoutConfiguration(layout, context_.current_path);

        context_.current_path = old_path;
        return valid;

    } catch (const std::exception &e) {
        context_.current_path = old_path;
        context_.addError(QString("Layout validation failed: %1").arg(e.what()),
                          "layout");
        return false;
    }
}

bool UIJSONValidator::validateProperties(const QJsonObject &properties,
                                         const QString &widget_type) {
    JSONPath old_path = context_.current_path;
    context_.current_path.append("properties");

    try {
        bool valid = true;

        for (auto it = properties.begin(); it != properties.end(); ++it) {
            const QString &prop_name = it.key();
            const QJsonValue &prop_value = it.value();

            JSONPath prop_path = context_.current_path;
            prop_path.append(prop_name);

            if (!validatePropertyValue(prop_name, prop_value, widget_type,
                                       prop_path)) {
                valid = false;
            }

            // **Run property-specific validators**
            runValidatorsForProperty(prop_name, prop_value, prop_path);
        }

        context_.current_path = old_path;
        return valid;

    } catch (const std::exception &e) {
        context_.current_path = old_path;
        context_.addError(
            QString("Properties validation failed: %1").arg(e.what()),
            "properties");
        return false;
    }
}

bool UIJSONValidator::validateEvents(const QJsonObject &events) {
    JSONPath old_path = context_.current_path;
    context_.current_path.append("events");

    try {
        bool valid = true;

        for (auto it = events.begin(); it != events.end(); ++it) {
            const QString &event_name = it.key();
            const QString &handler_name = it.value().toString();

            JSONPath event_path = context_.current_path;
            event_path.append(event_name);

            if (!validateEventHandler(event_name, handler_name, event_path)) {
                valid = false;
            }
        }

        context_.current_path = old_path;
        return valid;

    } catch (const std::exception &e) {
        context_.current_path = old_path;
        context_.addError(QString("Events validation failed: %1").arg(e.what()),
                          "events");
        return false;
    }
}

bool UIJSONValidator::validateBindings(const QJsonObject &bindings) {
    JSONPath old_path = context_.current_path;
    context_.current_path.append("bindings");

    try {
        bool valid = true;

        for (auto it = bindings.begin(); it != bindings.end(); ++it) {
            const QString &prop_name = it.key();
            const QString &binding_expr = it.value().toString();

            JSONPath binding_path = context_.current_path;
            binding_path.append(prop_name);

            if (!validatePropertyBinding(prop_name, binding_expr,
                                         binding_path)) {
                valid = false;
            }
        }

        context_.current_path = old_path;
        return valid;

    } catch (const std::exception &e) {
        context_.current_path = old_path;
        context_.addError(
            QString("Bindings validation failed: %1").arg(e.what()),
            "bindings");
        return false;
    }
}

UIJSONValidator &UIJSONValidator::setStrictMode(bool strict) {
    context_.strict_mode = strict;
    return *this;
}

UIJSONValidator &UIJSONValidator::setAllowUnknownComponents(bool allow) {
    context_.allow_additional_properties = allow;
    return *this;
}

UIJSONValidator &UIJSONValidator::setAllowUnknownProperties(bool allow) {
    context_.allow_additional_properties = allow;
    return *this;
}

UIJSONValidator &UIJSONValidator::setMaxNestingDepth(int depth) {
    context_.max_validation_depth = std::max(1, depth);
    return *this;
}

UIJSONValidator &UIJSONValidator::addComponentValidator(
    const QString &component_type, std::shared_ptr<IValidationRule> validator) {
    if (!validator) {
        throw std::invalid_argument("Validator cannot be null");
    }

    component_validators_[component_type].push_back(std::move(validator));
    return *this;
}

UIJSONValidator &UIJSONValidator::addPropertyValidator(
    const QString &property_name, std::shared_ptr<IValidationRule> validator) {
    if (!validator) {
        throw std::invalid_argument("Validator cannot be null");
    }

    property_validators_[property_name].push_back(std::move(validator));
    return *this;
}

UIJSONValidator &UIJSONValidator::addGlobalValidator(
    std::shared_ptr<IValidationRule> validator) {
    if (!validator) {
        throw std::invalid_argument("Validator cannot be null");
    }

    global_validators_.push_back(std::move(validator));
    return *this;
}

UIJSONValidator &UIJSONValidator::registerBuiltinValidators() {
    using namespace BuiltinValidators;

    // **Component validators**
    addComponentValidator(
        "*", std::make_shared<ComponentTypeValidator>(known_components_));

    // **Layout validators**
    addGlobalValidator(std::make_shared<LayoutValidator>());

    // **Property validators**
    addPropertyValidator("text",
                         std::make_shared<TypeValidator>(QJsonValue::String));
    addPropertyValidator("enabled",
                         std::make_shared<TypeValidator>(QJsonValue::Bool));
    addPropertyValidator("visible",
                         std::make_shared<TypeValidator>(QJsonValue::Bool));
    addPropertyValidator("minimumSize",
                         std::make_shared<TypeValidator>(QJsonValue::Array));
    addPropertyValidator("maximumSize",
                         std::make_shared<TypeValidator>(QJsonValue::Array));

    // **Size array validator**
    auto size_validator = std::make_shared<
        ValidationRuleImpl<std::function<bool(const QJsonValue &)>>>(
        "size_array", "Validates size arrays [width, height]",
        [](const QJsonValue &value) -> bool {
            if (!value.isArray())
                return false;
            QJsonArray arr = value.toArray();
            return arr.size() == 2 && arr[0].isDouble() && arr[1].isDouble();
        });

    addPropertyValidator("minimumSize", size_validator);
    addPropertyValidator("maximumSize", size_validator);

    qDebug() << "✅ Registered built-in validators";
    return *this;
}

std::vector<ValidationResult> UIJSONValidator::getValidationResults() const {
    return context_.results;
}

std::vector<ValidationResult> UIJSONValidator::getErrors() const {
    return context_.getErrors();
}

std::vector<ValidationResult> UIJSONValidator::getWarnings() const {
    return context_.getWarnings();
}

QStringList UIJSONValidator::getErrorMessages() const {
    QStringList messages;
    for (const auto &error : getErrors()) {
        messages.append(error.message);
    }
    return messages;
}

QStringList UIJSONValidator::getWarningMessages() const {
    QStringList messages;
    for (const auto &warning : getWarnings()) {
        messages.append(warning.message);
    }
    return messages;
}

void UIJSONValidator::clearResults() { context_.results.clear(); }

UIJSONValidator &UIJSONValidator::loadSchema(const QJsonObject &schema) {
    schema_validator_->loadSchema(schema);
    context_.schema = schema;
    return *this;
}

UIJSONValidator &UIJSONValidator::loadSchemaFromFile(
    const QString &schema_file) {
    schema_validator_->loadSchemaFromFile(schema_file);

    JSONParser parser;
    context_.schema = parser.parseFile(schema_file);

    return *this;
}

bool UIJSONValidator::validateAgainstSchema() const {
    if (context_.schema.isEmpty()) {
        return true;  // No schema to validate against
    }

    return schema_validator_->validate(context_.root_object);
}

bool UIJSONValidator::validateComponentStructure(const QJsonObject &component,
                                                 const JSONPath &path) {
    // **Setup validation context with depth checking**
    if (!setupValidationContext(path)) {
        return false;
    }

    JSONPath old_path = context_.current_path;
    context_.current_path = path;
    context_.current_depth++;

    try {
        bool valid = validateComponentTypeAndRunValidators(component, path);
        valid &= validateComponentSections(component);

        context_.current_depth--;
        context_.current_path = old_path;
        return valid;

    } catch (const std::exception &e) {
        context_.current_depth--;
        context_.current_path = old_path;
        context_.addError(
            QString("Component structure validation failed: %1").arg(e.what()),
            "exception");
        return false;
    }
}

// **Helper methods for reducing complexity**

/**
 * @brief Sets up validation context and checks depth limits
 * @param path The current validation path
 * @return true if validation can proceed, false if depth limit exceeded
 */
bool UIJSONValidator::setupValidationContext(const JSONPath &path) {
    if (context_.current_depth >= context_.max_validation_depth) {
        context_.addError("Maximum validation depth exceeded", "depth");
        return false;
    }
    return true;
}

/**
 * @brief Validates component type and runs component-specific validators
 * @param component The component object to validate
 * @param path The current validation path
 * @return true if validation passes, false otherwise
 */
bool UIJSONValidator::validateComponentTypeAndRunValidators(
    const QJsonObject &component, const JSONPath &path) {
    // **Validate required 'type' property**
    if (!component.contains("type")) {
        context_.addError("Component must have a 'type' property", "structure");
        return false;
    }

    QString component_type = component["type"].toString();
    if (!validateComponentType(component_type, path)) {
        return false;
    }

    // **Run component-specific validators**
    runValidatorsForComponent(component_type, component, path);
    return true;
}

/**
 * @brief Validates all component sections (properties, events, bindings,
 * layout, children)
 * @param component The component object to validate
 * @return true if all sections are valid, false otherwise
 */
bool UIJSONValidator::validateComponentSections(const QJsonObject &component) {
    bool valid = true;
    QString widget_type = component.value("type").toString();

    // **Validate properties section**
    if (component.contains("properties")) {
        if (!component["properties"].isObject()) {
            context_.addError("'properties' must be an object", "structure");
            valid = false;
        } else {
            valid &= validateProperties(component["properties"].toObject(),
                                        widget_type);
        }
    }

    // **Validate events section**
    if (component.contains("events")) {
        if (!component["events"].isObject()) {
            context_.addError("'events' must be an object", "structure");
            valid = false;
        } else {
            valid &= validateEvents(component["events"].toObject());
        }
    }

    // **Validate bindings section**
    if (component.contains("bindings")) {
        if (!component["bindings"].isObject()) {
            context_.addError("'bindings' must be an object", "structure");
            valid = false;
        } else {
            valid &= validateBindings(component["bindings"].toObject());
        }
    }

    // **Validate layout section**
    if (component.contains("layout")) {
        if (!component["layout"].isObject()) {
            context_.addError("'layout' must be an object", "structure");
            valid = false;
        } else {
            valid &= validateLayout(component["layout"].toObject());
        }
    }

    // **Validate children section**
    if (component.contains("children")) {
        if (!component["children"].isArray()) {
            context_.addError("'children' must be an array", "structure");
            valid = false;
        } else {
            valid &= validateComponentChildren(component["children"].toArray(),
                                               context_.current_path);
        }
    }

    return valid;
}

bool UIJSONValidator::validateComponentType(const QString &type,
                                            const JSONPath &path) {
    Q_UNUSED(path)

    if (type.isEmpty()) {
        context_.addError("Component type cannot be empty", "type");
        return false;
    }

    // **Check if component type is registered**
    if (!ComponentRegistry::instance().hasComponent(type)) {
        if (context_.strict_mode || !context_.allow_additional_properties) {
            context_.addError(QString("Unknown component type: %1").arg(type),
                              "type");
            return false;
        } else {
            context_.addWarning(QString("Unknown component type: %1").arg(type),
                                "type");
        }
    }

    return true;
}

bool UIJSONValidator::validateComponentProperties(const QJsonObject &component,
                                                  const JSONPath &path) {
    if (!component.contains("properties")) {
        return true;  // Properties are optional
    }

    QString widget_type = component.value("type").toString();
    return validateProperties(component["properties"].toObject(), widget_type);
}

bool UIJSONValidator::validateComponentChildren(const QJsonArray &children,
                                                const JSONPath &path) {
    JSONPath old_path = context_.current_path;
    context_.current_path = path;
    context_.current_path.append("children");

    try {
        bool valid = true;

        for (int i = 0; i < children.size(); ++i) {
            const QJsonValue &child = children[i];

            if (!child.isObject()) {
                JSONPath child_path = context_.current_path;
                child_path.append(i);
                context_.current_path = child_path;
                context_.addError("Child element must be an object",
                                  "children");
                valid = false;
                continue;
            }

            JSONPath child_path = context_.current_path;
            child_path.append(i);

            if (!validateComponentStructure(child.toObject(), child_path)) {
                valid = false;
            }
        }

        context_.current_path = old_path;
        return valid;

    } catch (const std::exception &e) {
        context_.current_path = old_path;
        context_.addError(
            QString("Children validation failed: %1").arg(e.what()),
            "children");
        return false;
    }
}

bool UIJSONValidator::validateLayoutConfiguration(const QJsonObject &layout,
                                                  const JSONPath &path) {
    Q_UNUSED(path)

    bool valid = true;

    // **Validate layout type**
    if (!layout.contains("type")) {
        context_.addError("Layout must have a 'type' property", "layout");
        return false;
    }

    QString layout_type = layout["type"].toString();
    QStringList valid_layouts = {"VBoxLayout", "HBoxLayout", "GridLayout",
                                 "FormLayout", "StackedLayout"};

    if (!valid_layouts.contains(layout_type)) {
        context_.addError(QString("Invalid layout type: %1").arg(layout_type),
                          "layout");
        valid = false;
    }

    // **Validate layout-specific properties**
    if (layout.contains("spacing")) {
        if (!layout["spacing"].isDouble()) {
            context_.addError("Layout 'spacing' must be a number", "layout");
            valid = false;
        } else {
            double spacing = layout["spacing"].toDouble();
            if (spacing < 0) {
                context_.addError("Layout 'spacing' cannot be negative",
                                  "layout");
                valid = false;
            }
        }
    }

    if (layout.contains("margins")) {
        if (!layout["margins"].isArray()) {
            context_.addError("Layout 'margins' must be an array", "layout");
            valid = false;
        } else {
            QJsonArray margins = layout["margins"].toArray();
            if (margins.size() != 4) {
                context_.addError(
                    "Layout 'margins' must have exactly 4 elements "
                    "[left, top, right, bottom]",
                    "layout");
                valid = false;
            } else {
                for (int i = 0; i < margins.size(); ++i) {
                    if (!margins[i].isDouble()) {
                        context_.addError(
                            QString(
                                "Layout margin at index %1 must be a number")
                                .arg(i),
                            "layout");
                        valid = false;
                    } else if (margins[i].toDouble() < 0) {
                        context_.addError(
                            QString(
                                "Layout margin at index %1 cannot be negative")
                                .arg(i),
                            "layout");
                        valid = false;
                    }
                }
            }
        }
    }

    return valid;
}

bool UIJSONValidator::validatePropertyValue(const QString &property_name,
                                            const QJsonValue &value,
                                            const QString &widget_type,
                                            const JSONPath &path) {
    Q_UNUSED(path)

    // **Validate basic property type**
    if (!validateBasicPropertyType(property_name, value)) {
        return false;
    }

    // **Validate widget-specific property compatibility**
    if (!validateWidgetSpecificProperty(property_name, widget_type)) {
        return false;
    }

    // **Validate special property formats**
    return validateSpecialPropertyFormats(property_name, value);
}

/**
 * @brief Validates basic property types against expected JSON value types
 * @param property_name The name of the property to validate
 * @param value The JSON value to validate
 * @return true if the property type is valid, false otherwise
 */
bool UIJSONValidator::validateBasicPropertyType(const QString &property_name,
                                                const QJsonValue &value) {
    // **Basic type validation for common properties**
    static const std::unordered_map<QString, QJsonValue::Type> property_types =
        {{"text", QJsonValue::String},
         {"title", QJsonValue::String},
         {"windowTitle", QJsonValue::String},
         {"styleSheet", QJsonValue::String},
         {"toolTip", QJsonValue::String},
         {"whatsThis", QJsonValue::String},
         {"enabled", QJsonValue::Bool},
         {"visible", QJsonValue::Bool},
         {"checked", QJsonValue::Bool},
         {"readOnly", QJsonValue::Bool},
         {"checkable", QJsonValue::Bool},
         {"width", QJsonValue::Double},
         {"height", QJsonValue::Double},
         {"x", QJsonValue::Double},
         {"y", QJsonValue::Double},
         {"value", QJsonValue::Double},
         {"minimum", QJsonValue::Double},
         {"maximum", QJsonValue::Double},
         {"singleStep", QJsonValue::Double},
         {"pageStep", QJsonValue::Double},
         {"currentIndex", QJsonValue::Double},
         {"maxLength", QJsonValue::Double},
         {"alignment", QJsonValue::Double}};

    auto type_it = property_types.find(property_name);
    if (type_it != property_types.end()) {
        QJsonValue::Type expected_type = type_it->second;
        if (value.type() != expected_type) {
            QString expected_type_name = getJsonValueTypeName(expected_type);
            QString actual_type_name = getJsonValueTypeName(value.type());

            context_.addError(
                QString("Property '%1' expects %2, got %3")
                    .arg(property_name, expected_type_name, actual_type_name),
                "property_type");
            return false;
        }
    }
    return true;
}

/**
 * @brief Validates widget-specific property compatibility
 * @param property_name The name of the property to validate
 * @param widget_type The widget type to validate against
 * @return true if the property is compatible with the widget type, false
 * otherwise
 */
bool UIJSONValidator::validateWidgetSpecificProperty(
    const QString &property_name, const QString &widget_type) {
    if (widget_type.isEmpty()) {
        return true;
    }

    if (known_properties_.find(widget_type) != known_properties_.end()) {
        const QStringList &valid_props = known_properties_.at(widget_type);
        if (!valid_props.contains(property_name) &&
            !valid_props.contains("*")) {
            if (context_.strict_mode || !context_.allow_additional_properties) {
                context_.addError(
                    QString("Property '%1' is not valid for widget type '%2'")
                        .arg(property_name, widget_type),
                    "property_compatibility");
                return false;
            } else {
                context_.addWarning(QString("Property '%1' may not be "
                                            "supported by widget type '%2'")
                                        .arg(property_name, widget_type),
                                    "property_compatibility");
            }
        }
    }
    return true;
}

/**
 * @brief Validates special property formats (size arrays, alignment,
 * orientation)
 * @param property_name The name of the property to validate
 * @param value The JSON value to validate
 * @return true if the property format is valid, false otherwise
 */
bool UIJSONValidator::validateSpecialPropertyFormats(
    const QString &property_name, const QJsonValue &value) {
    // **Size and geometry properties validation**
    if (property_name == "minimumSize" || property_name == "maximumSize" ||
        property_name == "size" || property_name == "geometry") {
        return validateSizeProperty(property_name, value);
    }

    // **Alignment property validation**
    if (property_name == "alignment") {
        return validateAlignmentProperty(value);
    }

    // **Orientation property validation**
    if (property_name == "orientation") {
        return validateOrientationProperty(value);
    }

    return true;
}

/**
 * @brief Validates size-related properties (arrays with 2 or 4 numeric
 * elements)
 * @param property_name The name of the size property
 * @param value The JSON array value to validate
 * @return true if the size property is valid, false otherwise
 */
bool UIJSONValidator::validateSizeProperty(const QString &property_name,
                                           const QJsonValue &value) {
    if (!value.isArray()) {
        context_.addError(
            QString("Property '%1' must be an array").arg(property_name),
            "property_format");
        return false;
    }

    QJsonArray arr = value.toArray();
    if (arr.size() != 2 && arr.size() != 4) {
        context_.addError(
            QString("Property '%1' array must have 2 elements [width, height] "
                    "or 4 elements [x, y, width, height]")
                .arg(property_name),
            "property_format");
        return false;
    }

    for (int i = 0; i < arr.size(); ++i) {
        if (!arr[i].isDouble()) {
            context_.addError(
                QString(
                    "Property '%1' array element at index %2 must be a number")
                    .arg(property_name)
                    .arg(i),
                "property_format");
            return false;
        }
        if (arr[i].toDouble() < 0) {
            context_.addError(QString("Property '%1' array element at index %2 "
                                      "cannot be negative")
                                  .arg(property_name)
                                  .arg(i),
                              "property_format");
            return false;
        }
    }
    return true;
}

/**
 * @brief Validates alignment property values
 * @param value The JSON value to validate
 * @return true if the alignment is valid, false otherwise
 */
bool UIJSONValidator::validateAlignmentProperty(const QJsonValue &value) {
    if (value.isDouble()) {
        int alignment = static_cast<int>(value.toDouble());
        // **Basic Qt::Alignment validation**
        if (alignment < 0 || alignment > 255) {
            context_.addWarning(
                QString("Alignment value %1 may be invalid").arg(alignment),
                "property_value");
        }
    }
    return true;
}

/**
 * @brief Validates orientation property values
 * @param value The JSON value to validate
 * @return true if the orientation is valid, false otherwise
 */
bool UIJSONValidator::validateOrientationProperty(const QJsonValue &value) {
    if (value.isString()) {
        QString orientation = value.toString().toLower();
        if (orientation != "horizontal" && orientation != "vertical") {
            context_.addError(
                QString(
                    "Orientation must be 'horizontal' or 'vertical', got '%1'")
                    .arg(value.toString()),
                "property_value");
            return false;
        }
    }
    return true;
}

/**
 * @brief Gets the string representation of a JSON value type
 * @param type The QJsonValue::Type to convert
 * @return String representation of the type
 */
QString UIJSONValidator::getJsonValueTypeName(QJsonValue::Type type) const {
    switch (type) {
        case QJsonValue::String:
            return "string";
        case QJsonValue::Double:
            return "number";
        case QJsonValue::Bool:
            return "boolean";
        case QJsonValue::Array:
            return "array";
        case QJsonValue::Object:
            return "object";
        default:
            return "unknown";
    }
}

bool UIJSONValidator::validateEventHandler(const QString &event_name,
                                           const QString &handler_name,
                                           const JSONPath &path) {
    Q_UNUSED(path)

    if (event_name.isEmpty()) {
        context_.addError("Event name cannot be empty", "event");
        return false;
    }

    if (handler_name.isEmpty()) {
        context_.addError("Event handler name cannot be empty", "event");
        return false;
    }

    // **Validate common Qt signal names**
    QStringList common_signals = {"clicked",
                                  "pressed",
                                  "released",
                                  "toggled",
                                  "triggered",
                                  "textChanged",
                                  "textEdited",
                                  "returnPressed",
                                  "valueChanged",
                                  "sliderPressed",
                                  "sliderReleased",
                                  "currentIndexChanged",
                                  "activated",
                                  "highlighted",
                                  "itemClicked",
                                  "itemDoubleClicked",
                                  "itemSelectionChanged",
                                  "finished",
                                  "accepted",
                                  "rejected"};

    if (!common_signals.contains(event_name)) {
        context_.addWarning(
            QString("Event '%1' may not be a standard Qt signal")
                .arg(event_name),
            "event");
    }

    // **Basic handler name validation**
    if (!handler_name.contains(
            QRegularExpression("^[a-zA-Z_][a-zA-Z0-9_]*$"))) {
        context_.addError(
            QString("Event handler name '%1' is not a valid identifier")
                .arg(handler_name),
            "event");
        return false;
    }

    return true;
}

bool UIJSONValidator::validatePropertyBinding(const QString &property_name,
                                              const QString &binding_expression,
                                              const JSONPath &path) {
    Q_UNUSED(path)

    if (property_name.isEmpty()) {
        context_.addError("Property name for binding cannot be empty",
                          "binding");
        return false;
    }

    if (binding_expression.isEmpty()) {
        context_.addError("Binding expression cannot be empty", "binding");
        return false;
    }

    // **Basic binding expression validation**
    if (!binding_expression.contains(
            QRegularExpression("^[a-zA-Z_][a-zA-Z0-9_]*$"))) {
        context_.addWarning(
            QString("Binding expression '%1' may not be a valid state key")
                .arg(binding_expression),
            "binding");
    }

    return true;
}

void UIJSONValidator::initializeKnownComponents() {
    known_components_ = ComponentRegistry::instance().getRegisteredTypes();

    // **Add additional known components**
    QStringList additional_components = {
        "QWidget",   "QLabel",         "QPushButton",  "QLineEdit",
        "QTextEdit", "QCheckBox",      "QRadioButton", "QComboBox",
        "QSpinBox",  "QDoubleSpinBox", "QSlider",      "QProgressBar",
        "QGroupBox", "QFrame",         "QScrollArea",  "QTabWidget",
        "QSplitter", "QListWidget",    "QTreeWidget",  "QTableWidget"};

    for (const QString &component : additional_components) {
        if (!known_components_.contains(component)) {
            known_components_.append(component);
        }
    }
}

void UIJSONValidator::initializeKnownProperties() {
    // **Initialize common properties for widget types**
    QStringList common_props = {
        "objectName",  "geometry",   "size",       "minimumSize", "maximumSize",
        "pos",         "x",          "y",          "width",       "height",
        "enabled",     "visible",    "styleSheet", "toolTip",     "whatsThis",
        "windowTitle", "windowIcon", "font",       "palette",     "cursor",
        "focusPolicy", "tabOrder"};

    // **Widget-specific properties**
    known_properties_["QLabel"] =
        common_props + QStringList{"text",          "alignment", "wordWrap",
                                   "indent",        "margin",    "pixmap",
                                   "scaledContents"};

    known_properties_["QPushButton"] =
        common_props + QStringList{"text",      "icon",    "iconSize",
                                   "checkable", "checked", "autoDefault",
                                   "default"};

    known_properties_["QLineEdit"] =
        common_props + QStringList{"text",           "placeholderText",
                                   "maxLength",      "readOnly",
                                   "echoMode",       "inputMask",
                                   "cursorPosition", "alignment",
                                   "dragEnabled",    "clearButtonEnabled"};

    known_properties_["QTextEdit"] =
        common_props + QStringList{"html",
                                   "plainText",
                                   "readOnly",
                                   "undoRedoEnabled",
                                   "lineWrapMode",
                                   "lineWrapColumnOrWidth",
                                   "tabChangesFocus",
                                   "documentTitle"};

    known_properties_["QCheckBox"] =
        common_props + QStringList{"text", "checked", "tristate", "checkState"};

    known_properties_["QRadioButton"] =
        common_props + QStringList{"text", "checked", "autoExclusive"};

    known_properties_["QComboBox"] =
        common_props + QStringList{"items",
                                   "currentIndex",
                                   "currentText",
                                   "editable",
                                   "maxCount",
                                   "maxVisibleItems",
                                   "minimumContentsLength",
                                   "insertPolicy"};

    known_properties_["QSpinBox"] =
        common_props + QStringList{"value",
                                   "minimum",
                                   "maximum",
                                   "singleStep",
                                   "prefix",
                                   "suffix",
                                   "displayIntegerBase",
                                   "readOnly",
                                   "buttonSymbols",
                                   "accelerated"};

    known_properties_["QDoubleSpinBox"] =
        common_props + QStringList{"value",      "minimum",  "maximum",
                                   "singleStep", "decimals", "prefix",
                                   "suffix",     "readOnly", "buttonSymbols",
                                   "accelerated"};

    known_properties_["QSlider"] =
        common_props + QStringList{"value",
                                   "minimum",
                                   "maximum",
                                   "singleStep",
                                   "pageStep",
                                   "orientation",
                                   "invertedAppearance",
                                   "invertedControls",
                                   "tracking",
                                   "tickPosition",
                                   "tickInterval"};

    known_properties_["QProgressBar"] =
        common_props + QStringList{"value",       "minimum",
                                   "maximum",     "format",
                                   "orientation", "invertedAppearance",
                                   "textVisible", "textDirection"};

    known_properties_["QGroupBox"] =
        common_props +
        QStringList{"title", "flat", "checkable", "checked", "alignment"};

    // **Initialize required properties**
    required_properties_["*"] = QStringList{"type"};
}

void UIJSONValidator::runValidatorsForComponent(const QString &component_type,
                                                const QJsonObject &component,
                                                const JSONPath &path) {
    JSONPath old_path = context_.current_path;
    context_.current_path = path;

    // **Run component-specific validators**
    auto validators_it = component_validators_.find(component_type);
    if (validators_it != component_validators_.end()) {
        for (const auto &validator : validators_it->second) {
            ValidationResult result =
                validator->validate(QJsonValue(component), context_);
            context_.addResult(result);
        }
    }

    // **Run wildcard validators**
    auto wildcard_it = component_validators_.find("*");
    if (wildcard_it != component_validators_.end()) {
        for (const auto &validator : wildcard_it->second) {
            ValidationResult result =
                validator->validate(QJsonValue(component), context_);
            context_.addResult(result);
        }
    }

    context_.current_path = old_path;
}

void UIJSONValidator::runValidatorsForProperty(const QString &property_name,
                                               const QJsonValue &value,
                                               const JSONPath &path) {
    JSONPath old_path = context_.current_path;
    context_.current_path = path;

    auto validators_it = property_validators_.find(property_name);
    if (validators_it != property_validators_.end()) {
        for (const auto &validator : validators_it->second) {
            ValidationResult result = validator->validate(value, context_);
            context_.addResult(result);
        }
    }

    context_.current_path = old_path;
}

void UIJSONValidator::runGlobalValidators(const QJsonObject &component,
                                          const JSONPath &path) {
    JSONPath old_path = context_.current_path;
    context_.current_path = path;

    for (const auto &validator : global_validators_) {
        ValidationResult result =
            validator->validate(QJsonValue(component), context_);
        context_.addResult(result);
    }

    context_.current_path = old_path;
}

// **Built-in Validators Implementation**

namespace BuiltinValidators {

// **TypeValidator**
TypeValidator::TypeValidator(QJsonValue::Type expected_type)
    : expected_type_(expected_type) {}

ValidationResult TypeValidator::validate(const QJsonValue &value,
                                         ValidationContext &context) {
    Q_UNUSED(context)

    if (value.type() != expected_type_) {
        QString expected_name = [this]() -> QString {
            switch (expected_type_) {
                case QJsonValue::String:
                    return "string";
                case QJsonValue::Double:
                    return "number";
                case QJsonValue::Bool:
                    return "boolean";
                case QJsonValue::Array:
                    return "array";
                case QJsonValue::Object:
                    return "object";
                case QJsonValue::Null:
                    return "null";
                default:
                    return "unknown";
            }
        }();

        QString actual_name = [&value]() -> QString {
            switch (value.type()) {
                case QJsonValue::String:
                    return "string";
                case QJsonValue::Double:
                    return "number";
                case QJsonValue::Bool:
                    return "boolean";
                case QJsonValue::Array:
                    return "array";
                case QJsonValue::Object:
                    return "object";
                case QJsonValue::Null:
                    return "null";
                default:
                    return "unknown";
            }
        }();

        return ValidationResult(
            false, ValidationSeverity::Error,
            QString("Expected type %1, got %2").arg(expected_name, actual_name),
            context.current_path, getName());
    }

    return ValidationResult(true, ValidationSeverity::Info,
                            "Type validation passed", context.current_path,
                            getName());
}

QString TypeValidator::getName() const { return "TypeValidator"; }

QString TypeValidator::getDescription() const {
    return QString("Validates that value is of expected type");
}

QJsonObject TypeValidator::getConfiguration() const {
    QJsonObject config;
    switch (expected_type_) {
        case QJsonValue::String:
            config["expectedType"] = "string";
            break;
        case QJsonValue::Double:
            config["expectedType"] = "number";
            break;
        case QJsonValue::Bool:
            config["expectedType"] = "boolean";
            break;
        case QJsonValue::Array:
            config["expectedType"] = "array";
            break;
        case QJsonValue::Object:
            config["expectedType"] = "object";
            break;
        case QJsonValue::Null:
            config["expectedType"] = "null";
            break;
        default:
            config["expectedType"] = "unknown";
            break;
    }
    return config;
}

// **RangeValidator**
RangeValidator::RangeValidator(double min_value, double max_value,
                               bool inclusive)
    : min_value_(min_value), max_value_(max_value), inclusive_(inclusive) {}

ValidationResult RangeValidator::validate(const QJsonValue &value,
                                          ValidationContext &context) {
    if (!value.isDouble()) {
        return ValidationResult(false, ValidationSeverity::Error,
                                "Value must be a number for range validation",
                                context.current_path, getName());
    }

    double num_value = value.toDouble();

    bool in_range;
    if (inclusive_) {
        in_range = (num_value >= min_value_ && num_value <= max_value_);
    } else {
        in_range = (num_value > min_value_ && num_value < max_value_);
    }

    if (!in_range) {
        QString range_desc =
            inclusive_ ? QString("[%1, %2]").arg(min_value_).arg(max_value_)
                       : QString("(%1, %2)").arg(min_value_).arg(max_value_);

        return ValidationResult(false, ValidationSeverity::Error,
                                QString("Value %1 is not in range %2")
                                    .arg(num_value)
                                    .arg(range_desc),
                                context.current_path, getName());
    }

    return ValidationResult(true, ValidationSeverity::Info,
                            "Range validation passed", context.current_path,
                            getName());
}

QString RangeValidator::getName() const { return "RangeValidator"; }

QString RangeValidator::getDescription() const {
    return QString("Validates that numeric value is within specified range");
}

QJsonObject RangeValidator::getConfiguration() const {
    QJsonObject config;
    config["minValue"] = min_value_;
    config["maxValue"] = max_value_;
    config["inclusive"] = inclusive_;
    return config;
}

// **LengthValidator**
LengthValidator::LengthValidator(int min_length, int max_length)
    : min_length_(min_length), max_length_(max_length) {}

ValidationResult LengthValidator::validate(const QJsonValue &value,
                                           ValidationContext &context) {
    if (!value.isString()) {
        return ValidationResult(false, ValidationSeverity::Error,
                                "Value must be a string for length validation",
                                context.current_path, getName());
    }

    QString str_value = value.toString();
    int length = str_value.length();

    if (length < min_length_) {
        return ValidationResult(
            false, ValidationSeverity::Error,
            QString("String length %1 is less than minimum %2")
                .arg(length)
                .arg(min_length_),
            context.current_path, getName());
    }

    if (max_length_ >= 0 && length > max_length_) {
        return ValidationResult(
            false, ValidationSeverity::Error,
            QString("String length %1 is greater than maximum %2")
                .arg(length)
                .arg(max_length_),
            context.current_path, getName());
    }

    return ValidationResult(true, ValidationSeverity::Info,
                            "Length validation passed", context.current_path,
                            getName());
}

QString LengthValidator::getName() const { return "LengthValidator"; }

QString LengthValidator::getDescription() const {
    return QString("Validates string length constraints");
}

QJsonObject LengthValidator::getConfiguration() const {
    QJsonObject config;
    config["minLength"] = min_length_;
    if (max_length_ >= 0) {
        config["maxLength"] = max_length_;
    }
    return config;
}

// **PatternValidator**
PatternValidator::PatternValidator(const QString &pattern,
                                   const QString &description)
    : regex_(pattern), pattern_(pattern), custom_description_(description) {
    if (!regex_.isValid()) {
        throw std::invalid_argument("Invalid regular expression pattern: " +
                                    pattern.toStdString());
    }
}

ValidationResult PatternValidator::validate(const QJsonValue &value,
                                            ValidationContext &context) {
    if (!value.isString()) {
        return ValidationResult(false, ValidationSeverity::Error,
                                "Value must be a string for pattern validation",
                                context.current_path, getName());
    }

    QString str_value = value.toString();

    if (!regex_.match(str_value).hasMatch()) {
        return ValidationResult(
            false, ValidationSeverity::Error,
            QString("String '%1' does not match pattern '%2'")
                .arg(str_value, pattern_),
            context.current_path, getName());
    }

    return ValidationResult(true, ValidationSeverity::Info,
                            "Pattern validation passed", context.current_path,
                            getName());
}

QString PatternValidator::getName() const { return "PatternValidator"; }

QString PatternValidator::getDescription() const {
    return custom_description_.isEmpty()
               ? QString("Validates string against regex pattern")
               : custom_description_;
}

QJsonObject PatternValidator::getConfiguration() const {
    QJsonObject config;
    config["pattern"] = pattern_;
    if (!custom_description_.isEmpty()) {
        config["description"] = custom_description_;
    }
    return config;
}

// **EnumValidator**
EnumValidator::EnumValidator(const QJsonArray &allowed_values)
    : allowed_values_(allowed_values) {}

ValidationResult EnumValidator::validate(const QJsonValue &value,
                                         ValidationContext &context) {
    for (const QJsonValue &allowed : allowed_values_) {
        if (value == allowed) {
            return ValidationResult(true, ValidationSeverity::Info,
                                    "Enum validation passed",
                                    context.current_path, getName());
        }
    }

    QStringList allowed_strings;
    for (const QJsonValue &allowed : allowed_values_) {
        allowed_strings.append(allowed.isString() ? allowed.toString()
                                                  : "non-string");
    }

    return ValidationResult(false, ValidationSeverity::Error,
                            QString("Value not in allowed enum values: [%1]")
                                .arg(allowed_strings.join(", ")),
                            context.current_path, getName());
}

QString EnumValidator::getName() const { return "EnumValidator"; }

QString EnumValidator::getDescription() const {
    return QString("Validates that value is one of the allowed enum values");
}

QJsonObject EnumValidator::getConfiguration() const {
    QJsonObject config;
    config["allowedValues"] = allowed_values_;
    return config;
}

// **RequiredPropertiesValidator**
RequiredPropertiesValidator::RequiredPropertiesValidator(
    const QStringList &required_properties)
    : required_properties_(required_properties) {}

ValidationResult RequiredPropertiesValidator::validate(
    const QJsonValue &value, ValidationContext &context) {
    if (!value.isObject()) {
        return ValidationResult(
            false, ValidationSeverity::Error,
            "Value must be an object for required properties validation",
            context.current_path, getName());
    }

    QJsonObject obj = value.toObject();
    QStringList missing_properties;

    for (const QString &required_prop : required_properties_) {
        if (!obj.contains(required_prop)) {
            missing_properties.append(required_prop);
        }
    }

    if (!missing_properties.isEmpty()) {
        return ValidationResult(false, ValidationSeverity::Error,
                                QString("Missing required properties: [%1]")
                                    .arg(missing_properties.join(", ")),
                                context.current_path, getName());
    }

    return ValidationResult(true, ValidationSeverity::Info,
                            "Required properties validation passed",
                            context.current_path, getName());
}

QString RequiredPropertiesValidator::getName() const {
    return "RequiredPropertiesValidator";
}

QString RequiredPropertiesValidator::getDescription() const {
    return QString("Validates that all required properties are present");
}

QJsonObject RequiredPropertiesValidator::getConfiguration() const {
    QJsonObject config;
    QJsonArray props_array;
    for (const QString &prop : required_properties_) {
        props_array.append(prop);
    }
    config["requiredProperties"] = props_array;
    return config;
}

// **ComponentTypeValidator**
ComponentTypeValidator::ComponentTypeValidator(
    const QStringList &known_components)
    : known_components_(known_components) {}

ValidationResult ComponentTypeValidator::validate(const QJsonValue &value,
                                                  ValidationContext &context) {
    if (!value.isObject()) {
        return ValidationResult(
            false, ValidationSeverity::Error,
            "Value must be an object for component type validation",
            context.current_path, getName());
    }

    QJsonObject obj = value.toObject();

    if (!obj.contains("type")) {
        return ValidationResult(false, ValidationSeverity::Error,
                                "Component must have a 'type' property",
                                context.current_path, getName());
    }

    QString component_type = obj["type"].toString();

    if (!known_components_.contains(component_type)) {
        return ValidationResult(
            false, ValidationSeverity::Warning,
            QString("Unknown component type: %1").arg(component_type),
            context.current_path, getName());
    }

    return ValidationResult(true, ValidationSeverity::Info,
                            "Component type validation passed",
                            context.current_path, getName());
}

QString ComponentTypeValidator::getName() const {
    return "ComponentTypeValidator";
}

QString ComponentTypeValidator::getDescription() const {
    return QString("Validates that component type is known/registered");
}

QJsonObject ComponentTypeValidator::getConfiguration() const {
    QJsonObject config;
    QJsonArray types_array;
    for (const QString &type : known_components_) {
        types_array.append(type);
    }
    config["knownComponents"] = types_array;
    return config;
}

// **LayoutValidator**
LayoutValidator::LayoutValidator() {
    valid_layout_types_ = {"VBoxLayout", "HBoxLayout", "GridLayout",
                           "FormLayout", "StackedLayout"};
}

ValidationResult LayoutValidator::validate(const QJsonValue &value,
                                           ValidationContext &context) {
    if (!value.isObject()) {
        return ValidationResult(true, ValidationSeverity::Info,
                                "Not a layout object", context.current_path,
                                getName());
    }

    QJsonObject obj = value.toObject();

    // **Only validate if this looks like a layout configuration**
    if (!obj.contains("layout")) {
        return ValidationResult(true, ValidationSeverity::Info,
                                "No layout configuration", context.current_path,
                                getName());
    }

    QJsonValue layout_value = obj["layout"];
    if (!layout_value.isObject()) {
        return ValidationResult(false, ValidationSeverity::Error,
                                "Layout configuration must be an object",
                                context.current_path, getName());
    }

    QJsonObject layout_obj = layout_value.toObject();

    if (!layout_obj.contains("type")) {
        return ValidationResult(
            false, ValidationSeverity::Error,
            "Layout configuration must have a 'type' property",
            context.current_path, getName());
    }

    QString layout_type = layout_obj["type"].toString();

    if (!valid_layout_types_.contains(layout_type)) {
        return ValidationResult(
            false, ValidationSeverity::Error,
            QString("Invalid layout type: %1. Valid types: [%2]")
                .arg(layout_type, valid_layout_types_.join(", ")),
            context.current_path, getName());
    }

    return ValidationResult(true, ValidationSeverity::Info,
                            "Layout validation passed", context.current_path,
                            getName());
}

QString LayoutValidator::getName() const { return "LayoutValidator"; }

QString LayoutValidator::getDescription() const {
    return QString("Validates layout configuration");
}

QJsonObject LayoutValidator::getConfiguration() const {
    QJsonObject config;
    QJsonArray types_array;
    for (const QString &type : valid_layout_types_) {
        types_array.append(type);
    }
    config["validLayoutTypes"] = types_array;
    return config;
}

// **PropertyCompatibilityValidator**
PropertyCompatibilityValidator::PropertyCompatibilityValidator(
    const QString &widget_type)
    : widget_type_(widget_type) {
    initializeWidgetProperties();
}

ValidationResult PropertyCompatibilityValidator::validate(
    const QJsonValue &value, ValidationContext &context) {
    if (!value.isObject()) {
        return ValidationResult(true, ValidationSeverity::Info,
                                "Not an object with properties",
                                context.current_path, getName());
    }

    QJsonObject obj = value.toObject();

    if (!obj.contains("properties") || !obj.contains("type")) {
        return ValidationResult(true, ValidationSeverity::Info,
                                "No properties or type to validate",
                                context.current_path, getName());
    }

    QString component_type = obj["type"].toString();
    QJsonObject properties = obj["properties"].toObject();

    auto props_it = widget_properties_.find(component_type);
    if (props_it == widget_properties_.end()) {
        return ValidationResult(
            true, ValidationSeverity::Info,
            QString("No property compatibility info for type: %1")
                .arg(component_type),
            context.current_path, getName());
    }

    const QStringList &valid_props = props_it->second;
    QStringList invalid_props;

    for (auto it = properties.begin(); it != properties.end(); ++it) {
        const QString &prop_name = it.key();
        if (!valid_props.contains(prop_name) && !valid_props.contains("*")) {
            invalid_props.append(prop_name);
        }
    }

    if (!invalid_props.isEmpty()) {
        return ValidationResult(
            false, ValidationSeverity::Warning,
            QString("Properties may not be compatible with %1: [%2]")
                .arg(component_type, invalid_props.join(", ")),
            context.current_path, getName());
    }

    return ValidationResult(true, ValidationSeverity::Info,
                            "Property compatibility validation passed",
                            context.current_path, getName());
}

QString PropertyCompatibilityValidator::getName() const {
    return "PropertyCompatibilityValidator";
}

QString PropertyCompatibilityValidator::getDescription() const {
    return QString("Validates property compatibility with widget types");
}

QJsonObject PropertyCompatibilityValidator::getConfiguration() const {
    QJsonObject config;
    config["widgetType"] = widget_type_;
    return config;
}

void PropertyCompatibilityValidator::initializeWidgetProperties() {
    // **Basic properties common to all widgets**
    QStringList common_props = {
        "objectName", "geometry", "size",       "minimumSize", "maximumSize",
        "pos",        "x",        "y",          "width",       "height",
        "enabled",    "visible",  "styleSheet", "toolTip",     "whatsThis",
        "font",       "palette"};

    // **Widget-specific properties (same as in UIJSONValidator)**
    widget_properties_["QLabel"] =
        common_props +
        QStringList{"text", "alignment", "wordWrap", "indent", "margin"};

    widget_properties_["QPushButton"] =
        common_props +
        QStringList{"text", "icon", "iconSize", "checkable", "checked"};

    widget_properties_["QLineEdit"] =
        common_props + QStringList{"text", "placeholderText", "maxLength",
                                   "readOnly", "echoMode"};

    // **Add more widget types as needed**
    widget_properties_["*"] = common_props;  // Fallback for unknown types
}

}  // namespace BuiltinValidators

// **ValidationUtils Implementation**

namespace ValidationUtils {

bool isValidUIDefinition(const QJsonObject &ui_definition) {
    UIJSONValidator validator;
    validator.registerBuiltinValidators();
    return validator.validate(ui_definition);
}

bool isValidComponent(const QJsonObject &component) {
    UIJSONValidator validator;
    validator.registerBuiltinValidators();
    return validator.validateComponent(component);
}

bool isValidLayout(const QJsonObject &layout) {
    UIJSONValidator validator;
    validator.registerBuiltinValidators();
    return validator.validateLayout(layout);
}

QString formatValidationResults(const std::vector<ValidationResult> &results) {
    QStringList formatted;

    for (const auto &result : results) {
        formatted.append(result.toString());
    }

    return formatted.join("\n");
}

QString formatValidationSummary(const std::vector<ValidationResult> &results) {
    int errors = 0, warnings = 0, info = 0;

    for (const auto &result : results) {
        switch (result.severity) {
            case ValidationSeverity::Error:
            case ValidationSeverity::Critical:
                errors++;
                break;
            case ValidationSeverity::Warning:
                warnings++;
                break;
            case ValidationSeverity::Info:
                info++;
                break;
        }
    }

    return QString(
               "Validation Summary: %1 errors, %2 warnings, %3 info messages")
        .arg(errors)
        .arg(warnings)
        .arg(info);
}

std::vector<ValidationResult> filterByPath(
    const std::vector<ValidationResult> &results, const JSONPath &path) {
    std::vector<ValidationResult> filtered;
    QString path_str = path.toString();

    std::copy_if(results.begin(), results.end(), std::back_inserter(filtered),
                 [&path_str](const ValidationResult &r) {
                     return r.path.toString().startsWith(path_str);
                 });

    return filtered;
}

std::vector<ValidationResult> filterBySeverity(
    const std::vector<ValidationResult> &results,
    ValidationSeverity min_severity) {
    std::vector<ValidationResult> filtered;

    std::copy_if(results.begin(), results.end(), std::back_inserter(filtered),
                 [min_severity](const ValidationResult &r) {
                     return static_cast<int>(r.severity) >=
                            static_cast<int>(min_severity);
                 });

    return filtered;
}

std::vector<ValidationResult> filterByRule(
    const std::vector<ValidationResult> &results, const QString &rule_name) {
    std::vector<ValidationResult> filtered;

    std::copy_if(results.begin(), results.end(), std::back_inserter(filtered),
                 [&rule_name](const ValidationResult &r) {
                     return r.rule_name == rule_name;
                 });

    return filtered;
}

template <ValidationPredicate PredicateType>
std::shared_ptr<ValidationRuleImpl<PredicateType>> createRule(
    const QString &name, const QString &description, PredicateType predicate) {
    return std::make_shared<ValidationRuleImpl<PredicateType>>(
        name, description, std::move(predicate));
}

std::shared_ptr<IValidationRule> createTypeRule(
    QJsonValue::Type expected_type) {
    return std::make_shared<BuiltinValidators::TypeValidator>(expected_type);
}

std::shared_ptr<IValidationRule> createRangeRule(double min_val,
                                                 double max_val) {
    return std::make_shared<BuiltinValidators::RangeValidator>(min_val,
                                                               max_val);
}

std::shared_ptr<IValidationRule> createLengthRule(int min_len, int max_len) {
    return std::make_shared<BuiltinValidators::LengthValidator>(min_len,
                                                                max_len);
}

std::shared_ptr<IValidationRule> createPatternRule(const QString &pattern) {
    return std::make_shared<BuiltinValidators::PatternValidator>(pattern);
}

std::shared_ptr<IValidationRule> createEnumRule(
    const QJsonArray &allowed_values) {
    return std::make_shared<BuiltinValidators::EnumValidator>(allowed_values);
}

// **Explicit template instantiation**
template std::shared_ptr<
    ValidationRuleImpl<std::function<bool(const QJsonValue &)>>>
createRule<std::function<bool(const QJsonValue &)>>(
    const QString &name, const QString &description,
    std::function<bool(const QJsonValue &)> predicate);

}  // namespace ValidationUtils

}  // namespace DeclarativeUI::JSON
