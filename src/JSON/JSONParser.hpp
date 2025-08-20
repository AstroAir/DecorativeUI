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

/**
 * @file JSONParser.hpp
 * @brief JSON parsing, resolution, validation, and utility declarations used by
 * the DeclarativeUI system.
 *
 * This header provides a small, cohesive set of types used to:
 *  - parse JSON documents from files, strings, and URLs,
 *  - navigate and manipulate JSON values using path expressions,
 *  - resolve JSON References and includes (local and external),
 *  - register and run custom parsers for application-specific typed values,
 *  - validate JSON data against a JSON Schema-like object,
 *  - and convenience utilities for common JSON operations.
 *
 * The APIs are designed to keep parsing state (source file, current path,
 * warnings/errors) in a JSONParsingContext that can be reused across the
 * parsing flow. Error reporting is accumulated into the context; callers may
 * choose between strict (throw on first error) and non-strict (accumulate
 * errors and continue) modes.
 *
 * Threading: Instances of JSONParser and JSONSchemaValidator are not
 * documented as thread-safe. If you need concurrent parsing use separate
 * parser/validator instances or add external synchronization.
 */

/**
 * @brief Concept for types that can be converted to/from JSON via static
 * helpers.
 *
 * A JSONConvertible type must provide:
 *  - static T::fromJson(const QJsonValue&) -> T (or convertible)
 *  - instance method T::toJson() -> QJsonValue (or convertible)
 *
 * This concept is used by registerTypeParser to allow registering type-specific
 * parsing logic in a compile-time safe way.
 *
 * @tparam T type being tested for JSON convertibility.
 */
template <typename T>
concept JSONConvertible = requires(T t, const QJsonValue &value) {
    { T::fromJson(value) } -> std::convertible_to<T>;
    { t.toJson() } -> std::convertible_to<QJsonValue>;
};

/**
 * @class JSONPath
 * @brief Represents a mutable path into a JSON document (sequence of
 * keys/indices).
 *
 * JSONPath provides simple, chainable operations to build and inspect a path
 * used during parsing and validation. Paths are represented as Qt QStringList
 * internally and rendered as a dot/slash style string for diagnostics.
 */
class JSONPath {
public:
    /**
     * @brief Construct an empty or initial path.
     * @param path optional textual representation to initialize components.
     */
    explicit JSONPath(const QString &path = "");

    /**
     * @brief Append an object key component to the path.
     * @param key object property name to append.
     * @return reference to this path to allow chaining.
     */
    JSONPath &append(const QString &key);

    /**
     * @brief Append an array index component to the path.
     * @param index numeric index to append.
     * @return reference to this path to allow chaining.
     */
    JSONPath &append(int index);

    /**
     * @brief Move the path to its parent (remove last component).
     * @return reference to this path after parent move.
     *
     * If the path is already empty this is a no-op.
     */
    JSONPath &parent();

    /**
     * @brief Render the path as a human readable string used in error messages.
     * @return QString representation, e.g. "/root/items/0/name".
     */
    [[nodiscard]] QString toString() const;

    /**
     * @brief Return the internal list of components.
     * @return QStringList of path components in order.
     */
    [[nodiscard]] QStringList components() const;

    /**
     * @brief Check whether the path is empty.
     * @return true if no components are present.
     */
    [[nodiscard]] bool isEmpty() const;

private:
    QStringList path_components_;
};

/**
 * @struct JSONParsingContext
 * @brief Holds state and error/warning accumulation for a parsing operation.
 *
 * The context contains the source filename, current JSONPath position within
 * the document, the parsed QJsonDocument, caches of resolved references and
 * registered custom parsers, and lists of human-readable warnings/errors
 * generated during processing.
 *
 * Typical usage:
 *  - create a context and pass it into JSONParser/JSONReferenceResolver
 *  - call throwIfErrors() at the end to convert accumulated errors into an
 * exception
 */
struct JSONParsingContext {
    QString source_file;    ///< Source file path for diagnostic messages.
    JSONPath current_path;  ///< Current location within the JSON document.
    QJsonDocument
        document;  ///< Parsed document (kept for reference resolution).
    std::unordered_map<QString, QJsonValue>
        resolved_references;  ///< Cache of resolved references by reference
                              ///< string.
    std::unordered_map<QString, std::function<QJsonValue(const QJsonValue &)>>
        custom_parsers;  ///< Type-specific custom parsers.

    // Error/warning accumulation
    QStringList warnings;  ///< Non-fatal issues encountered while parsing.
    QStringList errors;    ///< Fatal or recoverable errors encountered.
    bool strict_mode =
        false;  ///< When true, parser should throw on first error.

    /**
     * @brief Add a warning message to the context.
     * @param message Human-readable warning text.
     */
    void addWarning(const QString &message);

    /**
     * @brief Add an error message to the context.
     * @param message Human-readable error text.
     *
     * If strict_mode is true the implementation may throw immediately.
     */
    void addError(const QString &message);

    /**
     * @brief Throw an exception if errors were accumulated.
     *
     * @throw std::runtime_error or a project-specific exception when errors
     * exist. The exact exception type follows the project's error model (see
     * UIExceptions).
     */
    void throwIfErrors() const;
};

/**
 * @class JSONReferenceResolver
 * @brief Resolves JSON References ($ref-like strings) and included fragments.
 *
 * Responsibilities:
 *  - resolve local JSON Pointers within the current document,
 *  - resolve external references (file://, http(s) etc.) and cache results,
 *  - support include semantics where entire JSON objects are pulled into place,
 *  - provide cache management and optional caching toggle for testability.
 *
 * The resolver records results in the JSONParsingContext.resolved_references
 * map to avoid repeated network/file access.
 */
class JSONReferenceResolver {
public:
    /**
     * @brief Construct a resolver bound to a JSONParsingContext.
     * @param context Parsing context used for document root and diagnostics.
     */
    explicit JSONReferenceResolver(JSONParsingContext &context);

    /**
     * @brief Resolve a generic reference string.
     * @param reference Reference token (may be local pointer or external URI).
     * @return Resolved QJsonValue (may be object, array, primitive).
     *
     * The method will consult caches and context to locate or fetch the target.
     * On error an informative QJsonValue (e.g. null) is returned and the
     * context will receive an error message.
     */
    [[nodiscard]] QJsonValue resolveReference(const QString &reference);

    /**
     * @brief Resolve a JSON Pointer against the current document.
     * @param json_pointer JSON Pointer string (RFC 6901 style).
     * @return Resolved QJsonValue or QJsonValue() for not found.
     */
    [[nodiscard]] QJsonValue resolvePointer(const QString &json_pointer);

    /**
     * @brief Include and parse an external file as QJsonObject.
     * @param file_path local filesystem path to include.
     * @return Parsed QJsonObject (empty on error).
     */
    [[nodiscard]] QJsonObject includeFile(const QString &file_path);

    /**
     * @brief Include and parse an external URL as QJsonObject.
     * @param url remote URL to fetch and parse.
     * @return Parsed QJsonObject (empty on error).
     */
    [[nodiscard]] QJsonObject includeUrl(const QUrl &url);

    /**
     * @brief Clear resolver caches (resolved references and includes).
     *
     * Useful in unit tests to ensure deterministic behavior across runs.
     */
    void clearCache();

    /**
     * @brief Enable or disable caching of resolved resources.
     * @param enabled when false the resolver will always refetch external
     * resources.
     */
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

/**
 * @class JSONParser
 * @brief High-level JSON parsing and preprocessing utility used by
 * DeclarativeUI.
 *
 * JSONParser exposes convenience routines to parse JSON from multiple sources,
 * perform include/reference resolution, run custom type parsers, and return a
 * fully processed QJsonObject ready for use by higher-level UI factories.
 *
 * Configuration options control comment allowance, trailing comma handling,
 * maximum recursion depth, and whether parsing operates in strict mode.
 */
class JSONParser {
public:
    /**
     * @brief Construct a JSONParser with sensible defaults.
     *
     * Defaults:
     *  - strict_mode = false
     *  - allow_comments = true
     *  - allow_trailing_commas = true
     *  - max_depth = 100
     */
    explicit JSONParser();
    ~JSONParser() = default;

    /**
     * @brief Parse a JSON file from disk and return the top-level object.
     * @param file_path Path to the JSON file to parse.
     * @return QJsonObject representing the parsed (and processed) document.
     *
     * On error this method will either throw or return an empty object
     * depending on the parser's configured strictness and error handling
     * semantics.
     */
    [[nodiscard]] QJsonObject parseFile(const QString &file_path);

    /**
     * @brief Parse a JSON string and return the top-level object.
     * @param json_string Raw JSON text to parse.
     * @return QJsonObject of the processed document.
     */
    [[nodiscard]] QJsonObject parseString(const QString &json_string);

    /**
     * @brief Parse JSON content obtained from a URL.
     * @param url URL to fetch and parse (supports file:// and http/https).
     * @return QJsonObject of the processed document.
     */
    [[nodiscard]] QJsonObject parseUrl(const QUrl &url);

    /**
     * @brief Parse using an externally provided context.
     *
     * This allows callers to pre-populate context (e.g. custom parsers or
     * caching) and have the parser run processing against that context.
     *
     * @param source Source string or file contents to parse.
     * @param context Mutable parsing context used for resolution and error
     * reporting.
     * @return QJsonObject processed with the supplied context.
     */
    [[nodiscard]] QJsonObject parseWithContext(const QString &source,
                                               JSONParsingContext &context);

    /**
     * @name Configuration setters
     * Methods return *this to allow fluent configuration.
     */
    JSONParser &setStrictMode(bool strict);
    JSONParser &setAllowComments(bool allow);
    JSONParser &setAllowTrailingCommas(bool allow);
    JSONParser &setMaxDepth(int max_depth);

    /**
     * @brief Set a callback used to resolve include paths to actual file
     * contents.
     * @param resolver Function taking an include string and returning a
     * resolved path.
     *
     * The resolver is useful for test injection or custom resource mapping
     * (e.g. searching multiple directories).
     */
    JSONParser &setIncludeResolver(
        std::function<QString(const QString &)> resolver);

    /**
     * @brief Register a parser for a custom convertible type.
     *
     * The template parameter must satisfy JSONConvertible. The parser
     * registration enables processCustomType to locate and invoke known typed
     * parsers based on metadata embedded in the JSON input.
     *
     * @tparam T type implementing JSONConvertible.
     * @param type_name textual key used to identify the type in JSON.
     * @return reference to this parser for chaining.
     */
    template <JSONConvertible T>
    JSONParser &registerTypeParser(const QString &type_name);

    /**
     * @brief Register a low-level custom parser bound to a JSON key.
     * @param key JSON key that triggers the custom parser.
     * @param parser Callable that receives the raw QJsonValue and returns a
     * transformed QJsonValue.
     * @return reference to this parser for chaining.
     */
    JSONParser &registerCustomParser(
        const QString &key,
        std::function<QJsonValue(const QJsonValue &)> parser);

    /**
     * @brief Validate a processed JSON object against a schema object.
     * @param data data to validate.
     * @param schema schema to validate against.
     * @return true if validation succeeds; false otherwise (use
     * getErrors/getWarnings for details).
     */
    [[nodiscard]] bool validateAgainstSchema(const QJsonObject &data,
                                             const QJsonObject &schema) const;

    /**
     * @brief Retrieve accumulated non-fatal warnings from the last parse.
     * @return QStringList of warnings.
     */
    [[nodiscard]] QStringList getWarnings() const;

    /**
     * @brief Retrieve accumulated errors from the last parse.
     * @return QStringList of error messages.
     */
    [[nodiscard]] QStringList getErrors() const;

    /**
     * @brief Clear all accumulated messages in the parser (warnings and
     * errors).
     */
    void clearMessages();

    /**
     * @brief Utility: format a JSONPath for presentation.
     * @param path JSONPath to format.
     * @return human-friendly QString representation.
     */
    [[nodiscard]] static QString formatJsonPath(const JSONPath &path);

    /**
     * @brief Utility: retrieve a value from a QJsonObject using a JSONPath.
     * @param root root object to search.
     * @param path path describing the value location.
     * @return QJsonValue at the path or QJsonValue() if not present.
     */
    [[nodiscard]] static QJsonValue getValueAtPath(const QJsonObject &root,
                                                   const JSONPath &path);

    /**
     * @brief Utility: set a value at the specified JSONPath within a
     * QJsonObject.
     * @param root root object to modify.
     * @param path location to set the value.
     * @param value value to assign.
     * @return true on success, false if path could not be created/set.
     */
    [[nodiscard]] static bool setValueAtPath(QJsonObject &root,
                                             const JSONPath &path,
                                             const QJsonValue &value);

private:
    // Configuration
    bool strict_mode_ = false;
    bool allow_comments_ = true;
    bool allow_trailing_commas_ = true;
    int max_depth_ = 100;

    // Resolvers and parsers
    std::unique_ptr<JSONReferenceResolver> reference_resolver_;
    std::function<QString(const QString &)> include_resolver_;
    std::unordered_map<QString, std::function<QJsonValue(const QJsonValue &)>>
        custom_parsers_;

    // Parsing state (used internally during an active parse)
    std::unique_ptr<JSONParsingContext> current_context_;

    // Internal parsing pipeline methods (helpers)
    QJsonDocument parseJsonDocument(const QString &source,
                                    const QString &file_path = "");
    QJsonObject processJsonObject(const QJsonObject &input,
                                  JSONParsingContext &context);
    QJsonArray processJsonArray(const QJsonArray &input,
                                JSONParsingContext &context);
    QJsonValue processJsonValue(const QJsonValue &input,
                                JSONParsingContext &context);

    // Special processing (references, includes, custom typed values)
    QJsonValue processReference(const QString &reference,
                                JSONParsingContext &context);
    QJsonValue processInclude(const QString &include_path,
                              JSONParsingContext &context);
    QJsonValue processCustomType(const QString &type_name,
                                 const QJsonValue &value,
                                 JSONParsingContext &context);

    // Validation helpers
    bool validateObjectStructure(const QJsonObject &obj,
                                 JSONParsingContext &context);
    bool validateArrayStructure(const QJsonArray &arr,
                                JSONParsingContext &context);

    // Helper methods for reducing complexity in processJsonObject
    QJsonValue processSpecialKey(const QString &key, const QJsonValue &value,
                                 JSONParsingContext &context);
    QJsonValue processReferenceKey(const QJsonValue &value,
                                   JSONParsingContext &context);
    QJsonValue processIncludeKey(const QJsonValue &value,
                                 JSONParsingContext &context);
    QJsonValue processTypeKey(const QJsonValue &value,
                              JSONParsingContext &context);
    void mergeObjectIntoResult(const QJsonObject &source, QJsonObject &result);

    // Comment and trailing-comma preprocessing
    QString preprocessJson(const QString &json_string);
    QString removeComments(const QString &json_string);
    QString removeTrailingCommas(const QString &json_string);

    // Error context utilities
    void setParsingContext(const QString &source,
                           const QString &file_path = "");
    QString formatError(const QString &message,
                        const JSONPath &path = JSONPath()) const;
};

/**
 * @class JSONSchemaValidator
 * @brief Lightweight JSON Schema-like validator used to assert structure and
 * constraints.
 *
 * This validator accepts a QJsonObject describing schema constraints (type,
 * required, enum, min/max, pattern, properties, etc.) and validates data
 * against it. The goal is a pragmatic validator sufficient for typical UI
 * configuration checks rather than a full-featured JSON Schema implementation.
 */
class JSONSchemaValidator {
public:
    /**
     * @brief Construct an empty validator.
     */
    explicit JSONSchemaValidator();

    /** Schema loading helpers */
    void loadSchema(const QJsonObject &schema);
    void loadSchemaFromFile(const QString &schema_file);
    void loadSchemaFromString(const QString &schema_string);

    /** Validation entry points */
    [[nodiscard]] bool validate(const QJsonObject &data);
    [[nodiscard]] bool validate(const QJsonArray &data);
    [[nodiscard]] bool validate(const QJsonValue &data);

    /** Access validation diagnostics */
    [[nodiscard]] QStringList getValidationErrors() const;
    [[nodiscard]] QStringList getValidationWarnings() const;
    void clearValidationMessages();

    /** Schema introspection utilities */
    [[nodiscard]] QJsonObject getSchema() const;
    [[nodiscard]] QStringList getRequiredProperties(
        const QString &object_path = "") const;
    [[nodiscard]] QJsonObject getPropertySchema(
        const QString &property_path) const;

private:
    QJsonObject schema_;
    QStringList validation_errors_;
    QStringList validation_warnings_;

    // Internal recursive validators
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

    // Constraint helpers
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

/**
 * @namespace JSONUtils
 * @brief Collection of small JSON manipulation and conversion utilities.
 *
 * These helpers are intended for convenience and to keep parsing/validation
 * code concise and readable.
 */
namespace JSONUtils {

// Path navigation helpers
[[nodiscard]] QJsonValue getValue(const QJsonObject &root, const QString &path);
[[nodiscard]] bool setValue(QJsonObject &root, const QString &path,
                            const QJsonValue &value);
[[nodiscard]] bool removeValue(QJsonObject &root, const QString &path);
[[nodiscard]] bool hasValue(const QJsonObject &root, const QString &path);

// Object/array manipulation
[[nodiscard]] QJsonObject merge(const QJsonObject &base,
                                const QJsonObject &overlay);
[[nodiscard]] QJsonObject deepCopy(const QJsonObject &source);
[[nodiscard]] QJsonArray deepCopy(const QJsonArray &source);

// Type conversion utilities
template <typename T>
[[nodiscard]] std::optional<T> tryConvert(const QJsonValue &value);

[[nodiscard]] QString toString(const QJsonValue &value);
[[nodiscard]] int toInt(const QJsonValue &value, int default_value = 0);
[[nodiscard]] double toDouble(const QJsonValue &value,
                              double default_value = 0.0);
[[nodiscard]] bool toBool(const QJsonValue &value, bool default_value = false);

// Validation utilities
[[nodiscard]] bool isValidJsonPointer(const QString &pointer);
[[nodiscard]] bool isValidReference(const QString &reference);

// Pretty printing
[[nodiscard]] QString prettyPrint(const QJsonObject &obj, int indent = 2);
[[nodiscard]] QString prettyPrint(const QJsonArray &arr, int indent = 2);

}  // namespace JSONUtils

}  // namespace DeclarativeUI::JSON
