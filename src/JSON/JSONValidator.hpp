// JSON/JSONValidator.hpp
#pragma once
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QRegularExpression>
#include <QString>
#include <QStringList>
#include <concepts>
#include <functional>
#include <memory>
#include <unordered_map>
#include "../Exceptions/UIExceptions.hpp"
#include "JSONParser.hpp"

namespace DeclarativeUI::JSON {

/**
 * @file JSONValidator.hpp
 * @brief Validation framework for DeclarativeUI JSON documents.
 *
 * This header declares a flexible validation subsystem used to:
 *  - express validation rules (both predicate-based and class-based),
 *  - execute validation on UI JSON documents, components, properties,
 *    layouts, events and bindings,
 *  - accumulate structured ValidationResult objects with path, severity,
 *    human-friendly messages and optional expected/actual values,
 *  - register built-in validators and custom validators that can be applied
 *    per-component, per-property or globally,
 *  - integrate a lightweight JSON Schema-like validator (JSONSchemaValidator).
 *
 * Design goals:
 *  - Provide clear diagnostics (path + message + severity) suitable for both
 *    machine consumption and developer-facing logs.
 *  - Allow pluggable rules implemented as objects (IValidationRule) or simple
 *    predicates (ValidationPredicate).
 *  - Keep validation context and results separate so the same validators can be
 *    reused across multiple runs.
 *
 * Error handling:
 *  - Validation itself does not throw on rule failures; results are collected
 *    into ValidationContext.results. A consumer may call throwIfErrors() to
 *    convert fatal results into exceptions.
 */

/** Validation rule concept: callable taking QJsonValue and returning bool. */
template <typename T>
concept ValidationRuleConcept = requires(T rule, const QJsonValue &value) {
    { rule(value) } -> std::convertible_to<bool>;
};

/** Shorthand concept for predicate types returning bool when invoked with
 * QJsonValue. */
template <typename T>
concept ValidationPredicate =
    std::is_invocable_r_v<bool, T, const QJsonValue &>;

/** Severity levels used in ValidationResult. */
enum class ValidationSeverity { Info, Warning, Error, Critical };

/**
 * @struct ValidationResult
 * @brief Represents the outcome of running a single validation rule.
 *
 * Fields:
 *  - is_valid: whether the checked value satisfied the rule.
 *  - severity: severity level to classify the result.
 *  - message: human-readable description of the failure or success.
 *  - path: JSONPath indicating the location of the checked value.
 *  - rule_name: optional identifier for the rule that produced this result.
 *  - actual_value / expected_value: optional values to aid diagnostics.
 *
 * Utility methods provide textual formatting and convenience checks.
 */
struct ValidationResult {
    bool is_valid = true;
    ValidationSeverity severity = ValidationSeverity::Info;
    QString message;
    JSONPath path;
    QString rule_name;
    QJsonValue actual_value;
    QJsonValue expected_value;

    ValidationResult() = default;
    ValidationResult(bool valid, ValidationSeverity sev, const QString &msg,
                     const JSONPath &p = JSONPath(), const QString &rule = "");

    /**
     * @brief Convert the result to a human-readable one-line string.
     * @return Formatted string containing severity, path and message.
     */
    [[nodiscard]] QString toString() const;

    /** @return true if severity represents an error or worse. */
    [[nodiscard]] bool isError() const;

    /** @return true if severity represents a warning. */
    [[nodiscard]] bool isWarning() const;
};

/**
 * @struct ValidationContext
 * @brief Mutable context used while validating a document or a subtree.
 *
 * Holds:
 *  - current_path: current traversal location (JSONPath) used by validators.
 *  - root_object: the top-level object under validation.
 *  - schema: optional schema object used for schema-based validation helpers.
 *  - variables: user-provided variables available to rules.
 *  - custom_validators: map of named validator callables available to rules.
 *
 * Configuration:
 *  - strict_mode: if true certain failures may be treated as fatal.
 *  - allow_additional_properties: controls whether unknown properties are
 * permitted.
 *  - max_validation_depth: guard against pathological recursion.
 *
 * Results:
 *  - results: collected ValidationResult entries produced while validating.
 *
 * Convenience functions are provided to emit results and inspect the context.
 */
struct ValidationContext {
    JSONPath current_path;
    QJsonObject root_object;
    QJsonObject schema;
    std::unordered_map<QString, QJsonValue> variables;
    std::unordered_map<QString, std::function<bool(const QJsonValue &)>>
        custom_validators;

    // **Configuration**
    bool strict_mode = false;
    bool allow_additional_properties = true;
    int max_validation_depth = 100;
    int current_depth = 0;

    // **Results**
    std::vector<ValidationResult> results;

    /**
     * @brief Append a ValidationResult to the context.
     * @param result The ValidationResult to add.
     */
    void addResult(const ValidationResult &result);

    /**
     * @brief Convenience: add an error result at the current path.
     * @param message Human-readable error message.
     * @param rule_name Optional rule identifier.
     */
    void addError(const QString &message, const QString &rule_name = "");

    /**
     * @brief Convenience: add a warning result at the current path.
     * @param message Human-readable warning message.
     * @param rule_name Optional rule identifier.
     */
    void addWarning(const QString &message, const QString &rule_name = "");

    /**
     * @brief Convenience: add an informational result at the current path.
     * @param message Human-readable info message.
     * @param rule_name Optional rule identifier.
     */
    void addInfo(const QString &message, const QString &rule_name = "");

    /** @return true if any collected results are errors or worse. */
    [[nodiscard]] bool hasErrors() const;

    /** @return true if any collected results are warnings. */
    [[nodiscard]] bool hasWarnings() const;

    /** @return copy of collected error results. */
    [[nodiscard]] std::vector<ValidationResult> getErrors() const;

    /** @return copy of collected warning results. */
    [[nodiscard]] std::vector<ValidationResult> getWarnings() const;

    /**
     * @brief Throw an exception if the context contains error-level results.
     *
     * Intended for callers that want to treat validation errors as fatal.
     * The thrown exception type follows the project's exception model.
     */
    void throwIfErrors() const;
};

/**
 * @class IValidationRule
 * @brief Abstract interface for object-based validation rules.
 *
 * Implementers should provide:
 *  - validate(): run the rule against a QJsonValue using the supplied context.
 *  - getName()/getDescription(): metadata useful for reporting.
 *  - getConfiguration(): optional rule configuration represented as
 * QJsonObject.
 *
 * Using object-based rules enables richer diagnostics and configuration than
 * simple predicate-based rules.
 */
class IValidationRule {
public:
    virtual ~IValidationRule() = default;

    /**
     * @brief Execute this rule against a JSON value.
     * @param value Value to validate.
     * @param context ValidationContext with current path and settings.
     * @return ValidationResult describing outcome and any diagnostics.
     */
    virtual ValidationResult validate(const QJsonValue &value,
                                      ValidationContext &context) = 0;

    /** @return canonical rule name (identifier). */
    virtual QString getName() const = 0;

    /** @return human-friendly description of the rule. */
    virtual QString getDescription() const = 0;

    /** @return JSON configuration describing the rule (optional). */
    virtual QJsonObject getConfiguration() const = 0;
};

/**
 * @class ValidationRuleImpl
 * @brief Adapter allowing a simple predicate to be used as an IValidationRule.
 *
 * Template parameter PredicateType must be invocable with (const QJsonValue&)
 * and return bool. On failure the adapter constructs a ValidationResult with
 * Error severity; successes return Info severity.
 */
template <ValidationPredicate PredicateType>
class ValidationRuleImpl : public IValidationRule {
public:
    ValidationRuleImpl(const QString &name, const QString &description,
                       PredicateType predicate)
        : name_(name),
          description_(description),
          predicate_(std::move(predicate)) {}

    ValidationResult validate(const QJsonValue &value,
                              ValidationContext &context) override {
        try {
            bool is_valid = predicate_(value);

            if (!is_valid) {
                return ValidationResult(
                    false, ValidationSeverity::Error,
                    QString("Validation rule '%1' failed").arg(name_),
                    context.current_path, name_);
            }

            return ValidationResult(
                true, ValidationSeverity::Info,
                QString("Validation rule '%1' passed").arg(name_),
                context.current_path, name_);

        } catch (const std::exception &e) {
            return ValidationResult(
                false, ValidationSeverity::Critical,
                QString("Validation rule '%1' threw exception: %2")
                    .arg(name_, e.what()),
                context.current_path, name_);
        }
    }

    QString getName() const override { return name_; }
    QString getDescription() const override { return description_; }
    QJsonObject getConfiguration() const override { return QJsonObject(); }

private:
    QString name_;
    QString description_;
    PredicateType predicate_;
};

/**
 * @class UIJSONValidator
 * @brief High-level validator tailored to UI JSON definitions.
 *
 * This class orchestrates validation of UI definitions (full documents,
 * components, layouts, properties, events and bindings). It supports:
 *  - per-component and per-property validators,
 *  - registration of global validators,
 *  - configuration flags affecting strictness and allowed unknowns,
 *  - schema-based validation through JSONSchemaValidator.
 *
 * Usage:
 *  - configure validator (strictness, known components/properties),
 *  - optionally register custom rules,
 *  - call validate(...) to run validation and then query results/getErrors.
 */
class UIJSONValidator {
public:
    explicit UIJSONValidator();
    ~UIJSONValidator() = default;

    // **Validation methods**

    /**
     * @brief Validate a whole UI definition document.
     * @param ui_definition Top-level QJsonObject describing the UI.
     * @return true if no error-level validation results were produced.
     */
    [[nodiscard]] bool validate(const QJsonObject &ui_definition);

    /**
     * @brief Validate a single component object (or subtree).
     * @param component Component JSON object to validate.
     * @param component_type Optional declared type name used for type-specific
     * checks.
     * @return true if component passes validation (no error-level results).
     */
    [[nodiscard]] bool validateComponent(const QJsonObject &component,
                                         const QString &component_type = "");

    [[nodiscard]] bool validateLayout(const QJsonObject &layout);
    [[nodiscard]] bool validateProperties(const QJsonObject &properties,
                                          const QString &widget_type = "");
    [[nodiscard]] bool validateEvents(const QJsonObject &events);
    [[nodiscard]] bool validateBindings(const QJsonObject &bindings);

    // **Configuration setters (fluent API)**
    UIJSONValidator &setStrictMode(bool strict);
    UIJSONValidator &setAllowUnknownComponents(bool allow);
    UIJSONValidator &setAllowUnknownProperties(bool allow);
    UIJSONValidator &setMaxNestingDepth(int depth);

    // **Custom validation rules registration**
    UIJSONValidator &addComponentValidator(
        const QString &component_type,
        std::shared_ptr<IValidationRule> validator);
    UIJSONValidator &addPropertyValidator(
        const QString &property_name,
        std::shared_ptr<IValidationRule> validator);
    UIJSONValidator &addGlobalValidator(
        std::shared_ptr<IValidationRule> validator);

    /** Register the built-in rules provided in BuiltinValidators namespace. */
    UIJSONValidator &registerBuiltinValidators();

    // **Results accessors**
    [[nodiscard]] std::vector<ValidationResult> getValidationResults() const;
    [[nodiscard]] std::vector<ValidationResult> getErrors() const;
    [[nodiscard]] std::vector<ValidationResult> getWarnings() const;
    [[nodiscard]] QStringList getErrorMessages() const;
    [[nodiscard]] QStringList getWarningMessages() const;

    /** Clear accumulated results so the validator can be reused. */
    void clearResults();

    // **Schema-based validation helpers**
    UIJSONValidator &loadSchema(const QJsonObject &schema);
    UIJSONValidator &loadSchemaFromFile(const QString &schema_file);
    [[nodiscard]] bool validateAgainstSchema() const;

private:
    ValidationContext context_;

    // **Component and property validation**
    std::unordered_map<QString, std::vector<std::shared_ptr<IValidationRule>>>
        component_validators_;
    std::unordered_map<QString, std::vector<std::shared_ptr<IValidationRule>>>
        property_validators_;
    std::vector<std::shared_ptr<IValidationRule>> global_validators_;

    // **Known UI components and properties**
    QStringList known_components_;
    std::unordered_map<QString, QStringList> known_properties_;
    std::unordered_map<QString, QStringList> required_properties_;

    // **Schema validation**
    std::unique_ptr<JSONSchemaValidator> schema_validator_;

    // **Internal validation methods**
    bool validateComponentStructure(const QJsonObject &component,
                                    const JSONPath &path);
    bool validateComponentType(const QString &type, const JSONPath &path);
    bool validateComponentProperties(const QJsonObject &component,
                                     const JSONPath &path);
    bool validateComponentChildren(const QJsonArray &children,
                                   const JSONPath &path);
    bool validateLayoutConfiguration(const QJsonObject &layout,
                                     const JSONPath &path);
    bool validatePropertyValue(const QString &property_name,
                               const QJsonValue &value,
                               const QString &widget_type,
                               const JSONPath &path);
    bool validateEventHandler(const QString &event_name,
                              const QString &handler_name,
                              const JSONPath &path);
    bool validatePropertyBinding(const QString &property_name,
                                 const QString &binding_expression,
                                 const JSONPath &path);

    // **Helper methods**
    void initializeKnownComponents();
    void initializeKnownProperties();
    void runValidatorsForComponent(const QString &component_type,
                                   const QJsonObject &component,
                                   const JSONPath &path);
    void runValidatorsForProperty(const QString &property_name,
                                  const QJsonValue &value,
                                  const JSONPath &path);
    void runGlobalValidators(const QJsonObject &component,
                             const JSONPath &path);
};

// **Built-in validation rules**
namespace BuiltinValidators {

/**
 * @class TypeValidator
 * @brief Validates that a QJsonValue has the expected QJsonValue::Type.
 */
class TypeValidator : public IValidationRule {
public:
    explicit TypeValidator(QJsonValue::Type expected_type);
    ValidationResult validate(const QJsonValue &value,
                              ValidationContext &context) override;
    QString getName() const override;
    QString getDescription() const override;
    QJsonObject getConfiguration() const override;

private:
    QJsonValue::Type expected_type_;
};

/**
 * @class RangeValidator
 * @brief Validates numeric values fall within [min_value, max_value] (inclusive
 * optional).
 */
class RangeValidator : public IValidationRule {
public:
    RangeValidator(double min_value, double max_value, bool inclusive = true);
    ValidationResult validate(const QJsonValue &value,
                              ValidationContext &context) override;
    QString getName() const override;
    QString getDescription() const override;
    QJsonObject getConfiguration() const override;

private:
    double min_value_;
    double max_value_;
    bool inclusive_;
};

/**
 * @class LengthValidator
 * @brief Validates string length constraints.
 *
 * If max_length is -1 only the minimum is enforced.
 */
class LengthValidator : public IValidationRule {
public:
    LengthValidator(int min_length, int max_length = -1);
    ValidationResult validate(const QJsonValue &value,
                              ValidationContext &context) override;
    QString getName() const override;
    QString getDescription() const override;
    QJsonObject getConfiguration() const override;

private:
    int min_length_;
    int max_length_;
};

/**
 * @class PatternValidator
 * @brief Validates string values against a QRegularExpression.
 */
class PatternValidator : public IValidationRule {
public:
    explicit PatternValidator(const QString &pattern,
                              const QString &description = "");
    ValidationResult validate(const QJsonValue &value,
                              ValidationContext &context) override;
    QString getName() const override;
    QString getDescription() const override;
    QJsonObject getConfiguration() const override;

private:
    QRegularExpression regex_;
    QString pattern_;
    QString custom_description_;
};

/**
 * @class EnumValidator
 * @brief Validates that a value is one of a fixed set of allowed values.
 */
class EnumValidator : public IValidationRule {
public:
    explicit EnumValidator(const QJsonArray &allowed_values);
    ValidationResult validate(const QJsonValue &value,
                              ValidationContext &context) override;
    QString getName() const override;
    QString getDescription() const override;
    QJsonObject getConfiguration() const override;

private:
    QJsonArray allowed_values_;
};

/**
 * @class RequiredPropertiesValidator
 * @brief Validates presence of required property names within an object.
 */
class RequiredPropertiesValidator : public IValidationRule {
public:
    explicit RequiredPropertiesValidator(
        const QStringList &required_properties);
    ValidationResult validate(const QJsonValue &value,
                              ValidationContext &context) override;
    QString getName() const override;
    QString getDescription() const override;
    QJsonObject getConfiguration() const override;

private:
    QStringList required_properties_;
};

/**
 * @class ComponentTypeValidator
 * @brief UI-specific validator that verifies component "type" values are known.
 */
class ComponentTypeValidator : public IValidationRule {
public:
    explicit ComponentTypeValidator(const QStringList &known_components);
    ValidationResult validate(const QJsonValue &value,
                              ValidationContext &context) override;
    QString getName() const override;
    QString getDescription() const override;
    QJsonObject getConfiguration() const override;

private:
    QStringList known_components_;
};

/**
 * @class LayoutValidator
 * @brief Validates layout configuration objects against supported layout types.
 */
class LayoutValidator : public IValidationRule {
public:
    LayoutValidator();
    ValidationResult validate(const QJsonValue &value,
                              ValidationContext &context) override;
    QString getName() const override;
    QString getDescription() const override;
    QJsonObject getConfiguration() const override;

private:
    QStringList valid_layout_types_;
};

/**
 * @class PropertyCompatibilityValidator
 * @brief Ensures a property is compatible with the target widget type.
 *
 * This validator uses an internal mapping of widget types → supported property
 * names to detect invalid property assignments.
 */
class PropertyCompatibilityValidator : public IValidationRule {
public:
    explicit PropertyCompatibilityValidator(const QString &widget_type);
    ValidationResult validate(const QJsonValue &value,
                              ValidationContext &context) override;
    QString getName() const override;
    QString getDescription() const override;
    QJsonObject getConfiguration() const override;

private:
    QString widget_type_;
    std::unordered_map<QString, QStringList> widget_properties_;

    void initializeWidgetProperties();
};

}  // namespace BuiltinValidators

// **Validation utilities**
namespace ValidationUtils {

/**
 * @brief Quick checks useful for callers that need a boolean pass/fail.
 * @return true when the supplied JSON fragment matches basic structural rules.
 */
[[nodiscard]] bool isValidUIDefinition(const QJsonObject &ui_definition);
[[nodiscard]] bool isValidComponent(const QJsonObject &component);
[[nodiscard]] bool isValidLayout(const QJsonObject &layout);

/**
 * @brief Formatting helpers to render a set of ValidationResult objects into
 * text.
 */
[[nodiscard]] QString formatValidationResults(
    const std::vector<ValidationResult> &results);
[[nodiscard]] QString formatValidationSummary(
    const std::vector<ValidationResult> &results);

/**
 * @brief Filtering helpers to extract results by path, severity or rule name.
 */
[[nodiscard]] std::vector<ValidationResult> filterByPath(
    const std::vector<ValidationResult> &results, const JSONPath &path);
[[nodiscard]] std::vector<ValidationResult> filterBySeverity(
    const std::vector<ValidationResult> &results,
    ValidationSeverity min_severity);
[[nodiscard]] std::vector<ValidationResult> filterByRule(
    const std::vector<ValidationResult> &results, const QString &rule_name);

/**
 * @brief Factories for common validation rules expressed as IValidationRule
 * objects.
 */
template <ValidationPredicate PredicateType>
std::shared_ptr<ValidationRuleImpl<PredicateType>> createRule(
    const QString &name, const QString &description, PredicateType predicate);

[[nodiscard]] std::shared_ptr<IValidationRule> createTypeRule(
    QJsonValue::Type expected_type);
[[nodiscard]] std::shared_ptr<IValidationRule> createRangeRule(double min_val,
                                                               double max_val);
[[nodiscard]] std::shared_ptr<IValidationRule> createLengthRule(
    int min_len, int max_len = -1);
[[nodiscard]] std::shared_ptr<IValidationRule> createPatternRule(
    const QString &pattern);
[[nodiscard]] std::shared_ptr<IValidationRule> createEnumRule(
    const QJsonArray &allowed_values);

}  // namespace ValidationUtils

}  // namespace DeclarativeUI::JSON
