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

// **Validation rule concepts**
template <typename T>
concept ValidationRuleConcept = requires(T rule, const QJsonValue &value) {
    { rule(value) } -> std::convertible_to<bool>;
};

template <typename T>
concept ValidationPredicate =
    std::is_invocable_r_v<bool, T, const QJsonValue &>;

// **Validation severity levels**
enum class ValidationSeverity { Info, Warning, Error, Critical };

// **Validation result**
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

    [[nodiscard]] QString toString() const;
    [[nodiscard]] bool isError() const;
    [[nodiscard]] bool isWarning() const;
};

// **Validation context**
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

    void addResult(const ValidationResult &result);
    void addError(const QString &message, const QString &rule_name = "");
    void addWarning(const QString &message, const QString &rule_name = "");
    void addInfo(const QString &message, const QString &rule_name = "");

    [[nodiscard]] bool hasErrors() const;
    [[nodiscard]] bool hasWarnings() const;
    [[nodiscard]] std::vector<ValidationResult> getErrors() const;
    [[nodiscard]] std::vector<ValidationResult> getWarnings() const;

    void throwIfErrors() const;
};

// **Custom validation rule interface**
class IValidationRule {
public:
    virtual ~IValidationRule() = default;

    virtual ValidationResult validate(const QJsonValue &value,
                                      ValidationContext &context) = 0;
    virtual QString getName() const = 0;
    virtual QString getDescription() const = 0;
    virtual QJsonObject getConfiguration() const = 0;
};

// **Template-based validation rule**
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

// **UI-specific validator**
class UIJSONValidator {
public:
    explicit UIJSONValidator();
    ~UIJSONValidator() = default;

    // **Validation methods**
    [[nodiscard]] bool validate(const QJsonObject &ui_definition);
    [[nodiscard]] bool validateComponent(const QJsonObject &component,
                                         const QString &component_type = "");
    [[nodiscard]] bool validateLayout(const QJsonObject &layout);
    [[nodiscard]] bool validateProperties(const QJsonObject &properties,
                                          const QString &widget_type = "");
    [[nodiscard]] bool validateEvents(const QJsonObject &events);
    [[nodiscard]] bool validateBindings(const QJsonObject &bindings);

    // **Configuration**
    UIJSONValidator &setStrictMode(bool strict);
    UIJSONValidator &setAllowUnknownComponents(bool allow);
    UIJSONValidator &setAllowUnknownProperties(bool allow);
    UIJSONValidator &setMaxNestingDepth(int depth);

    // **Custom validation rules**
    UIJSONValidator &addComponentValidator(
        const QString &component_type,
        std::shared_ptr<IValidationRule> validator);
    UIJSONValidator &addPropertyValidator(
        const QString &property_name,
        std::shared_ptr<IValidationRule> validator);
    UIJSONValidator &addGlobalValidator(
        std::shared_ptr<IValidationRule> validator);

    // **Built-in rule registration**
    UIJSONValidator &registerBuiltinValidators();

    // **Results access**
    [[nodiscard]] std::vector<ValidationResult> getValidationResults() const;
    [[nodiscard]] std::vector<ValidationResult> getErrors() const;
    [[nodiscard]] std::vector<ValidationResult> getWarnings() const;
    [[nodiscard]] QStringList getErrorMessages() const;
    [[nodiscard]] QStringList getWarningMessages() const;

    void clearResults();

    // **Schema-based validation**
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

// **Type validation**
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

// **Range validation for numbers**
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

// **String length validation**
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

// **Pattern validation**
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

// **Enum validation**
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

// **Required properties validation**
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

// **UI-specific validators**
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

// **Quick validation functions**
[[nodiscard]] bool isValidUIDefinition(const QJsonObject &ui_definition);
[[nodiscard]] bool isValidComponent(const QJsonObject &component);
[[nodiscard]] bool isValidLayout(const QJsonObject &layout);

// **Error formatting**
[[nodiscard]] QString formatValidationResults(
    const std::vector<ValidationResult> &results);
[[nodiscard]] QString formatValidationSummary(
    const std::vector<ValidationResult> &results);

// **Validation result filtering**
[[nodiscard]] std::vector<ValidationResult> filterByPath(
    const std::vector<ValidationResult> &results, const JSONPath &path);
[[nodiscard]] std::vector<ValidationResult> filterBySeverity(
    const std::vector<ValidationResult> &results,
    ValidationSeverity min_severity);
[[nodiscard]] std::vector<ValidationResult> filterByRule(
    const std::vector<ValidationResult> &results, const QString &rule_name);

// **Validation rule factories**
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
