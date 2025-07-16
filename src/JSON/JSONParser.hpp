// JSON/JSONParser.hpp
#pragma once
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <QVariant>
#include <concepts>
#include <functional>
#include <memory>
#include <optional>
#include <unordered_map>

namespace DeclarativeUI::JSON {

// **JSON parsing concepts**
template <typename T>
concept JSONConvertible = requires(T t, const QJsonValue &value) {
    { T::fromJson(value) } -> std::convertible_to<T>;
    { t.toJson() } -> std::convertible_to<QJsonValue>;
};

// **JSON path navigation**
class JSONPath {
public:
    explicit JSONPath(const QString &path = "");

    JSONPath &append(const QString &key);
    JSONPath &append(int index);
    JSONPath &parent();

    [[nodiscard]] QString toString() const;
    [[nodiscard]] QStringList components() const;
    [[nodiscard]] bool isEmpty() const;

private:
    QStringList path_components_;
};

// **JSON parsing context**
struct JSONParsingContext {
    QString source_file;
    JSONPath current_path;
    QJsonDocument document;
    std::unordered_map<QString, QJsonValue> resolved_references;
    std::unordered_map<QString, std::function<QJsonValue(const QJsonValue &)>>
        custom_parsers;

    // **Error handling**
    QStringList warnings;
    QStringList errors;
    bool strict_mode = false;

    void addWarning(const QString &message);
    void addError(const QString &message);
    void throwIfErrors() const;
};

// **JSON reference resolver**
class JSONReferenceResolver {
public:
    explicit JSONReferenceResolver(JSONParsingContext &context);

    // **Reference resolution**
    [[nodiscard]] QJsonValue resolveReference(const QString &reference);
    [[nodiscard]] QJsonValue resolvePointer(const QString &json_pointer);

    // **Include support**
    [[nodiscard]] QJsonObject includeFile(const QString &file_path);
    [[nodiscard]] QJsonObject includeUrl(const QUrl &url);

    // **Cache management**
    void clearCache();
    void setCacheEnabled(bool enabled);

private:
    JSONParsingContext &context_;
    std::unordered_map<QString, QJsonValue> reference_cache_;
    std::unordered_map<QString, QJsonObject> include_cache_;
    bool cache_enabled_ = true;

    QJsonValue resolveLocalReference(const QString &reference);
    QJsonValue resolveExternalReference(const QString &reference);
    QStringList parseJsonPointer(const QString &pointer);
    QJsonValue navigateJsonPointer(const QJsonValue &root,
                                   const QStringList &path);
};

// **Main JSON parser class**
class JSONParser {
public:
    explicit JSONParser();
    ~JSONParser() = default;

    // **Parsing methods**
    [[nodiscard]] QJsonObject parseFile(const QString &file_path);
    [[nodiscard]] QJsonObject parseString(const QString &json_string);
    [[nodiscard]] QJsonObject parseUrl(const QUrl &url);

    // **Advanced parsing with context**
    [[nodiscard]] QJsonObject parseWithContext(const QString &source,
                                               JSONParsingContext &context);

    // **Configuration**
    JSONParser &setStrictMode(bool strict);
    JSONParser &setAllowComments(bool allow);
    JSONParser &setAllowTrailingCommas(bool allow);
    JSONParser &setMaxDepth(int max_depth);
    JSONParser &setIncludeResolver(
        std::function<QString(const QString &)> resolver);

    // **Custom type parsers**
    template <JSONConvertible T>
    JSONParser &registerTypeParser(const QString &type_name);

    JSONParser &registerCustomParser(
        const QString &key,
        std::function<QJsonValue(const QJsonValue &)> parser);

    // **Validation**
    [[nodiscard]] bool validateAgainstSchema(const QJsonObject &data,
                                             const QJsonObject &schema) const;

    // **Error handling**
    [[nodiscard]] QStringList getWarnings() const;
    [[nodiscard]] QStringList getErrors() const;
    void clearMessages();

    // **Utility methods**
    [[nodiscard]] static QString formatJsonPath(const JSONPath &path);
    [[nodiscard]] static QJsonValue getValueAtPath(const QJsonObject &root,
                                                   const JSONPath &path);
    [[nodiscard]] static bool setValueAtPath(QJsonObject &root,
                                             const JSONPath &path,
                                             const QJsonValue &value);

private:
    // **Configuration**
    bool strict_mode_ = false;
    bool allow_comments_ = true;
    bool allow_trailing_commas_ = true;
    int max_depth_ = 100;

    // **Resolvers and parsers**
    std::unique_ptr<JSONReferenceResolver> reference_resolver_;
    std::function<QString(const QString &)> include_resolver_;
    std::unordered_map<QString, std::function<QJsonValue(const QJsonValue &)>>
        custom_parsers_;

    // **Parsing state**
    std::unique_ptr<JSONParsingContext> current_context_;

    // **Internal parsing methods**
    QJsonDocument parseJsonDocument(const QString &source,
                                    const QString &file_path = "");
    QJsonObject processJsonObject(const QJsonObject &input,
                                  JSONParsingContext &context);
    QJsonArray processJsonArray(const QJsonArray &input,
                                JSONParsingContext &context);
    QJsonValue processJsonValue(const QJsonValue &input,
                                JSONParsingContext &context);

    // **Special processing**
    QJsonValue processReference(const QString &reference,
                                JSONParsingContext &context);
    QJsonValue processInclude(const QString &include_path,
                              JSONParsingContext &context);
    QJsonValue processCustomType(const QString &type_name,
                                 const QJsonValue &value,
                                 JSONParsingContext &context);

    // **Validation helpers**
    bool validateObjectStructure(const QJsonObject &obj,
                                 JSONParsingContext &context);
    bool validateArrayStructure(const QJsonArray &arr,
                                JSONParsingContext &context);

    // **Comment and comma handling**
    QString preprocessJson(const QString &json_string);
    QString removeComments(const QString &json_string);
    QString removeTrailingCommas(const QString &json_string);

    // **Error context**
    void setParsingContext(const QString &source,
                           const QString &file_path = "");
    QString formatError(const QString &message,
                        const JSONPath &path = JSONPath()) const;
};

// **JSON schema validator**
class JSONSchemaValidator {
public:
    explicit JSONSchemaValidator();

    // **Schema loading**
    void loadSchema(const QJsonObject &schema);
    void loadSchemaFromFile(const QString &schema_file);
    void loadSchemaFromString(const QString &schema_string);

    // **Validation**
    [[nodiscard]] bool validate(const QJsonObject &data);
    [[nodiscard]] bool validate(const QJsonArray &data);
    [[nodiscard]] bool validate(const QJsonValue &data);

    // **Error reporting**
    [[nodiscard]] QStringList getValidationErrors() const;
    [[nodiscard]] QStringList getValidationWarnings() const;
    void clearValidationMessages();

    // **Schema introspection**
    [[nodiscard]] QJsonObject getSchema() const;
    [[nodiscard]] QStringList getRequiredProperties(
        const QString &object_path = "") const;
    [[nodiscard]] QJsonObject getPropertySchema(
        const QString &property_path) const;

private:
    QJsonObject schema_;
    QStringList validation_errors_;
    QStringList validation_warnings_;

    // **Validation methods**
    bool validateObject(const QJsonObject &obj, const QJsonObject &schema,
                        const JSONPath &path);
    bool validateArray(const QJsonArray &arr, const QJsonObject &schema,
                       const JSONPath &path);
    bool validateString(const QString &str, const QJsonObject &schema,
                        const JSONPath &path);
    bool validateNumber(double num, const QJsonObject &schema,
                        const JSONPath &path);
    bool validateBoolean(bool val, const QJsonObject &schema,
                         const JSONPath &path);

    // **Schema constraint validation**
    bool validateType(const QJsonValue &value,
                      const QJsonValue &type_constraint, const JSONPath &path);
    bool validateEnum(const QJsonValue &value, const QJsonArray &enum_values,
                      const JSONPath &path);
    bool validateProperties(const QJsonObject &obj,
                            const QJsonObject &properties,
                            const JSONPath &path);
    bool validateRequired(const QJsonObject &obj, const QJsonArray &required,
                          const JSONPath &path);
    bool validateMinMax(double value, const QJsonObject &schema,
                        const JSONPath &path);
    bool validateLength(const QString &str, const QJsonObject &schema,
                        const JSONPath &path);
    bool validatePattern(const QString &str, const QString &pattern,
                         const JSONPath &path);

    void addValidationError(const QString &message,
                            const JSONPath &path = JSONPath());
    void addValidationWarning(const QString &message,
                              const JSONPath &path = JSONPath());
};

// **JSON manipulation utilities**
namespace JSONUtils {

// **Path navigation**
[[nodiscard]] QJsonValue getValue(const QJsonObject &root, const QString &path);
[[nodiscard]] bool setValue(QJsonObject &root, const QString &path,
                            const QJsonValue &value);
[[nodiscard]] bool removeValue(QJsonObject &root, const QString &path);
[[nodiscard]] bool hasValue(const QJsonObject &root, const QString &path);

// **Object manipulation**
[[nodiscard]] QJsonObject merge(const QJsonObject &base,
                                const QJsonObject &overlay);
[[nodiscard]] QJsonObject deepCopy(const QJsonObject &source);
[[nodiscard]] QJsonArray deepCopy(const QJsonArray &source);

// **Type conversion utilities**
template <typename T>
[[nodiscard]] std::optional<T> tryConvert(const QJsonValue &value);

[[nodiscard]] QString toString(const QJsonValue &value);
[[nodiscard]] int toInt(const QJsonValue &value, int default_value = 0);
[[nodiscard]] double toDouble(const QJsonValue &value,
                              double default_value = 0.0);
[[nodiscard]] bool toBool(const QJsonValue &value, bool default_value = false);

// **Validation utilities**
[[nodiscard]] bool isValidJsonPointer(const QString &pointer);
[[nodiscard]] bool isValidReference(const QString &reference);

// **Pretty printing**
[[nodiscard]] QString prettyPrint(const QJsonObject &obj, int indent = 2);
[[nodiscard]] QString prettyPrint(const QJsonArray &arr, int indent = 2);

}  // namespace JSONUtils

}  // namespace DeclarativeUI::JSON
