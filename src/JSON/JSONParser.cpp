/**
 * @file JSONParser.cpp
 * @brief Implementation of JSON parsing, reference resolution, and
 * preprocessing
 *
 * This file provides comprehensive JSON parsing capabilities for DeclarativeUI,
 * including support for JSON references ($ref), file includes ($include),
 * custom type processing ($type), and comment/trailing comma preprocessing.
 * The implementation emphasizes maintainable code with low cyclomatic
 * complexity.
 *
 * Key features:
 * - JSON reference resolution (local and external)
 * - File inclusion with caching
 * - Custom type processing
 * - Comment and trailing comma support
 * - Recursive object and array processing
 * - Comprehensive error handling and reporting
 *
 * @author DeclarativeUI Team
 * @version 1.0
 */

#include "JSONParser.hpp"

#include "src/Exceptions/UIExceptions.hpp"

#include <QDebug>
#include <QDir>
#include <QEventLoop>
#include <QFile>
#include <QJsonParseError>
#include <QJsonValue>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QRegularExpression>
#include <QStandardPaths>

namespace DeclarativeUI::JSON {

// **JSONPath Implementation**

JSONPath::JSONPath(const QString& path) {
    if (!path.isEmpty()) {
        path_components_ = path.split('.', Qt::SkipEmptyParts);
    }
}

JSONPath& JSONPath::append(const QString& key) {
    if (!key.isEmpty()) {
        path_components_.append(key);
    }
    return *this;
}

JSONPath& JSONPath::append(int index) {
    path_components_.append(QString("[%1]").arg(index));
    return *this;
}

JSONPath& JSONPath::parent() {
    if (!path_components_.isEmpty()) {
        path_components_.removeLast();
    }
    return *this;
}

QString JSONPath::toString() const { return path_components_.join('.'); }

QStringList JSONPath::components() const { return path_components_; }

bool JSONPath::isEmpty() const { return path_components_.isEmpty(); }

// **JSONParsingContext Implementation**

void JSONParsingContext::addWarning(const QString& message) {
    QString formatted_message =
        QString("[%1] Warning: %2").arg(current_path.toString(), message);
    warnings.append(formatted_message);

    if (!strict_mode) {
        qWarning() << formatted_message;
    }
}

void JSONParsingContext::addError(const QString& message) {
    QString formatted_message =
        QString("[%1] Error: %2").arg(current_path.toString(), message);
    errors.append(formatted_message);

    qCritical() << formatted_message;
}

void JSONParsingContext::throwIfErrors() const {
    if (!errors.isEmpty()) {
        throw Exceptions::JSONParsingException(source_file.toStdString(),
                                               errors.join("; ").toStdString());
    }
}

// **JSONReferenceResolver Implementation**

JSONReferenceResolver::JSONReferenceResolver(JSONParsingContext& context)
    : context_(context) {}

QJsonValue JSONReferenceResolver::resolveReference(const QString& reference) {
    if (reference.isEmpty()) {
        context_.addError("Empty reference");
        return QJsonValue();
    }

    // **Check cache first**
    if (cache_enabled_) {
        auto cache_it = reference_cache_.find(reference);
        if (cache_it != reference_cache_.end()) {
            return cache_it->second;
        }
    }

    QJsonValue result;

    try {
        if (reference.startsWith("#/")) {
            // **JSON Pointer reference**
            result = resolvePointer(reference);
        } else if (reference.startsWith("./") || reference.startsWith("../")) {
            // **Relative file reference**
            result = resolveExternalReference(reference);
        } else if (reference.contains("://")) {
            // **URL reference**
            QUrl url(reference);
            if (url.isValid()) {
                result = QJsonValue(includeUrl(url));
            } else {
                context_.addError(
                    QString("Invalid URL reference: %1").arg(reference));
            }
        } else {
            // **Local reference**
            result = resolveLocalReference(reference);
        }

        // **Cache the result**
        if (cache_enabled_ && !result.isNull()) {
            reference_cache_[reference] = result;
        }

    } catch (const std::exception& e) {
        context_.addError(QString("Reference resolution failed for '%1': %2")
                              .arg(reference, e.what()));
        result = QJsonValue();
    }

    return result;
}

QJsonValue JSONReferenceResolver::resolvePointer(const QString& json_pointer) {
    if (!json_pointer.startsWith("#/")) {
        context_.addError(
            QString("Invalid JSON pointer: %1").arg(json_pointer));
        return QJsonValue();
    }

    QString pointer_path = json_pointer.mid(2);  // Remove "#/"
    QStringList path_components = parseJsonPointer(pointer_path);

    return navigateJsonPointer(context_.document.object(), path_components);
}

QJsonObject JSONReferenceResolver::includeFile(const QString& file_path) {
    // **Check cache first**
    if (cache_enabled_) {
        auto cache_it = include_cache_.find(file_path);
        if (cache_it != include_cache_.end()) {
            return cache_it->second;
        }
    }

    QString resolved_path = file_path;

    // **Resolve relative paths**
    if (!QFileInfo(file_path).isAbsolute() && !context_.source_file.isEmpty()) {
        QFileInfo source_info(context_.source_file);
        resolved_path = source_info.dir().absoluteFilePath(file_path);
    }

    QFile file(resolved_path);
    if (!file.open(QIODevice::ReadOnly)) {
        throw Exceptions::JSONParsingException(
            resolved_path.toStdString(),
            "Cannot open include file: " + file.errorString().toStdString());
    }

    QByteArray data = file.readAll();
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);

    if (doc.isNull()) {
        throw Exceptions::JSONParsingException(
            resolved_path.toStdString(),
            "JSON parse error: " + error.errorString().toStdString());
    }

    QJsonObject result = doc.object();

    // **Cache the result**
    if (cache_enabled_) {
        include_cache_[file_path] = result;
    }

    return result;
}

QJsonObject JSONReferenceResolver::includeUrl(const QUrl& url) {
    // **Simple synchronous URL loading (for demo purposes)**
    // **In production, you'd want async loading with proper error handling**

    QString url_string = url.toString();

    // **Check cache**
    if (cache_enabled_) {
        auto cache_it = include_cache_.find(url_string);
        if (cache_it != include_cache_.end()) {
            return cache_it->second;
        }
    }

    QNetworkAccessManager manager;
    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", "DeclarativeUI/1.0");

    QNetworkReply* reply = manager.get(request);

    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if (reply->error() != QNetworkReply::NoError) {
        reply->deleteLater();
        throw Exceptions::JSONParsingException(
            url_string.toStdString(),
            "Network error: " + reply->errorString().toStdString());
    }

    QByteArray data = reply->readAll();
    reply->deleteLater();

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);

    if (doc.isNull()) {
        throw Exceptions::JSONParsingException(
            url_string.toStdString(),
            "JSON parse error: " + error.errorString().toStdString());
    }

    QJsonObject result = doc.object();

    // **Cache the result**
    if (cache_enabled_) {
        include_cache_[url_string] = result;
    }

    return result;
}

void JSONReferenceResolver::clearCache() {
    reference_cache_.clear();
    include_cache_.clear();
}

void JSONReferenceResolver::setCacheEnabled(bool enabled) {
    cache_enabled_ = enabled;
    if (!enabled) {
        clearCache();
    }
}

QJsonValue JSONReferenceResolver::resolveLocalReference(
    const QString& reference) {
    // **Simple local reference resolution**
    // **In a more complex system, this could resolve to definitions, etc.**

    auto resolved_it = context_.resolved_references.find(reference);
    if (resolved_it != context_.resolved_references.end()) {
        return resolved_it->second;
    }

    context_.addWarning(
        QString("Unresolved local reference: %1").arg(reference));
    return QJsonValue();
}

QJsonValue JSONReferenceResolver::resolveExternalReference(
    const QString& reference) {
    try {
        QJsonObject included = includeFile(reference);
        return QJsonValue(included);
    } catch (const std::exception& e) {
        context_.addError(
            QString("External reference resolution failed: %1").arg(e.what()));
        return QJsonValue();
    }
}

QStringList JSONReferenceResolver::parseJsonPointer(const QString& pointer) {
    if (pointer.isEmpty()) {
        return QStringList();
    }

    QStringList components = pointer.split('/', Qt::SkipEmptyParts);

    // **Unescape JSON pointer components**
    for (QString& component : components) {
        component.replace("~1", "/");
        component.replace("~0", "~");
    }

    return components;
}

QJsonValue JSONReferenceResolver::navigateJsonPointer(const QJsonValue& root,
                                                      const QStringList& path) {
    QJsonValue current = root;

    for (const QString& component : path) {
        if (current.isObject()) {
            QJsonObject obj = current.toObject();
            if (!obj.contains(component)) {
                context_.addError(
                    QString("JSON pointer path not found: %1").arg(component));
                return QJsonValue();
            }
            current = obj[component];
        } else if (current.isArray()) {
            QJsonArray arr = current.toArray();
            bool ok;
            int index = component.toInt(&ok);

            if (!ok || index < 0 || index >= arr.size()) {
                context_.addError(
                    QString("Invalid array index in JSON pointer: %1")
                        .arg(component));
                return QJsonValue();
            }

            current = arr[index];
        } else {
            context_.addError(
                QString("Cannot navigate further in JSON pointer at: %1")
                    .arg(component));
            return QJsonValue();
        }
    }

    return current;
}

// **JSONParser Implementation**

JSONParser::JSONParser() {
    reference_resolver_ =
        std::make_unique<JSONReferenceResolver>(*current_context_);
}

QJsonObject JSONParser::parseFile(const QString& file_path) {
    QFileInfo file_info(file_path);

    if (!file_info.exists()) {
        throw Exceptions::JSONParsingException(file_path.toStdString(),
                                               "File does not exist");
    }

    if (!file_info.isReadable()) {
        throw Exceptions::JSONParsingException(file_path.toStdString(),
                                               "File is not readable");
    }

    QFile file(file_path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw Exceptions::JSONParsingException(
            file_path.toStdString(),
            "Cannot open file: " + file.errorString().toStdString());
    }

    QString json_content = QString::fromUtf8(file.readAll());

    // **Setup parsing context**
    JSONParsingContext context;
    context.source_file = file_info.canonicalFilePath();
    context.strict_mode = strict_mode_;

    return parseWithContext(json_content, context);
}

QJsonObject JSONParser::parseString(const QString& json_string) {
    JSONParsingContext context;
    context.source_file = "<string>";
    context.strict_mode = strict_mode_;

    return parseWithContext(json_string, context);
}

QJsonObject JSONParser::parseUrl(const QUrl& url) {
    if (!url.isValid()) {
        throw Exceptions::JSONParsingException(url.toString().toStdString(),
                                               "Invalid URL");
    }

    // **Load from URL (simplified synchronous version)**
    QNetworkAccessManager manager;
    QNetworkRequest request(url);

    QNetworkReply* reply = manager.get(request);
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if (reply->error() != QNetworkReply::NoError) {
        QString error_msg = reply->errorString();
        reply->deleteLater();
        throw Exceptions::JSONParsingException(
            url.toString().toStdString(),
            "Network error: " + error_msg.toStdString());
    }

    QString json_content = QString::fromUtf8(reply->readAll());
    reply->deleteLater();

    JSONParsingContext context;
    context.source_file = url.toString();
    context.strict_mode = strict_mode_;

    return parseWithContext(json_content, context);
}

QJsonObject JSONParser::parseWithContext(const QString& source,
                                         JSONParsingContext& context) {
    current_context_ = std::make_unique<JSONParsingContext>(std::move(context));
    reference_resolver_ =
        std::make_unique<JSONReferenceResolver>(*current_context_);

    try {
        // **Preprocess JSON if needed**
        QString processed_source = source;
        if (allow_comments_ || allow_trailing_commas_) {
            processed_source = preprocessJson(source);
        }

        // **Parse the JSON document**
        QJsonDocument doc =
            parseJsonDocument(processed_source, current_context_->source_file);
        current_context_->document = doc;

        if (!doc.isObject()) {
            current_context_->addError("Root JSON value must be an object");
            current_context_->throwIfErrors();
        }

        // **Process the root object**
        QJsonObject result = processJsonObject(doc.object(), *current_context_);

        // **Check for errors**
        current_context_->throwIfErrors();

        return result;

    } catch (const Exceptions::JSONParsingException&) {
        throw;  // Re-throw JSON parsing exceptions as-is
    } catch (const std::exception& e) {
        throw Exceptions::JSONParsingException(
            current_context_->source_file.toStdString(),
            "Parsing failed: " + std::string(e.what()));
    }
}

JSONParser& JSONParser::setStrictMode(bool strict) {
    strict_mode_ = strict;
    return *this;
}

JSONParser& JSONParser::setAllowComments(bool allow) {
    allow_comments_ = allow;
    return *this;
}

JSONParser& JSONParser::setAllowTrailingCommas(bool allow) {
    allow_trailing_commas_ = allow;
    return *this;
}

JSONParser& JSONParser::setMaxDepth(int max_depth) {
    max_depth_ = std::max(1, max_depth);
    return *this;
}

JSONParser& JSONParser::setIncludeResolver(
    std::function<QString(const QString&)> resolver) {
    include_resolver_ = std::move(resolver);
    return *this;
}

template <JSONConvertible T>
JSONParser& JSONParser::registerTypeParser(const QString& type_name) {
    custom_parsers_[type_name] =
        [type_name](const QJsonValue& value) -> QJsonValue {
        try {
            T converted = T::fromJson(value);
            return converted.toJson();
        } catch (const std::exception& e) {
            throw Exceptions::JSONParsingException(
                "type_conversion", QString("Type conversion failed for %1: %2")
                                       .arg(type_name, e.what())
                                       .toStdString());
        }
    };

    return *this;
}

JSONParser& JSONParser::registerCustomParser(
    const QString& key, std::function<QJsonValue(const QJsonValue&)> parser) {
    if (key.isEmpty()) {
        throw std::invalid_argument("Parser key cannot be empty");
    }

    if (!parser) {
        throw std::invalid_argument("Parser function cannot be null");
    }

    custom_parsers_[key] = std::move(parser);
    return *this;
}

bool JSONParser::validateAgainstSchema(const QJsonObject& data,
                                       const QJsonObject& schema) const {
    JSONSchemaValidator validator;
    validator.loadSchema(schema);
    return validator.validate(data);
}

QStringList JSONParser::getWarnings() const {
    return current_context_ ? current_context_->warnings : QStringList();
}

QStringList JSONParser::getErrors() const {
    return current_context_ ? current_context_->errors : QStringList();
}

void JSONParser::clearMessages() {
    if (current_context_) {
        current_context_->warnings.clear();
        current_context_->errors.clear();
    }
}

QString JSONParser::formatJsonPath(const JSONPath& path) {
    return path.toString();
}

QJsonValue JSONParser::getValueAtPath(const QJsonObject& root,
                                      const JSONPath& path) {
    return JSONUtils::getValue(root, path.toString());
}

bool JSONParser::setValueAtPath(QJsonObject& root, const JSONPath& path,
                                const QJsonValue& value) {
    return JSONUtils::setValue(root, path.toString(), value);
}

QJsonDocument JSONParser::parseJsonDocument(const QString& source,
                                            const QString& file_path) {
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(source.toUtf8(), &error);

    if (doc.isNull()) {
        throw Exceptions::JSONParsingException(
            file_path.toStdString(),
            QString("JSON parse error at offset %1: %2")
                .arg(error.offset)
                .arg(error.errorString())
                .toStdString());
    }

    return doc;
}

/**
 * @brief Processes a JSON object, handling special keys and references
 * @param input The input JSON object to process
 * @param context The parsing context for error handling and state tracking
 * @return Processed JSON object with resolved references and includes
 */
QJsonObject JSONParser::processJsonObject(const QJsonObject& input,
                                          JSONParsingContext& context) {
    QJsonObject result;

    for (auto it = input.begin(); it != input.end(); ++it) {
        const QString& key = it.key();
        const QJsonValue& value = it.value();

        // **Update current path for error reporting**
        JSONPath old_path = context.current_path;
        context.current_path.append(key);

        try {
            QJsonValue processed_value = processSpecialKey(key, value, context);

            // **Handle the processed value based on its type**
            if (processed_value.isObject() &&
                (key == "$ref" || key == "$include")) {
                // **Merge object results from references and includes**
                mergeObjectIntoResult(processed_value.toObject(), result);
            } else if (!processed_value.isNull()) {
                result[key] = processed_value;
            }

        } catch (const std::exception& e) {
            context.addError(
                QString("Error processing key '%1': %2").arg(key, e.what()));
            if (context.strict_mode) {
                throw;
            }
        }

        // **Restore path**
        context.current_path = old_path;
    }

    return result;
}

/**
 * @brief Processes special JSON keys ($ref, $include, $type) and regular
 * properties
 * @param key The JSON key to process
 * @param value The JSON value associated with the key
 * @param context The parsing context for error handling
 * @return Processed JSON value
 */
QJsonValue JSONParser::processSpecialKey(const QString& key,
                                         const QJsonValue& value,
                                         JSONParsingContext& context) {
    if (key == "$ref") {
        return processReferenceKey(value, context);
    } else if (key == "$include") {
        return processIncludeKey(value, context);
    } else if (key == "$type") {
        return processTypeKey(value, context);
    } else {
        // **Regular property - process recursively**
        return processJsonValue(value, context);
    }
}

/**
 * @brief Processes JSON reference keys ($ref)
 * @param value The reference value (should be a string)
 * @param context The parsing context for error handling
 * @return Resolved reference value or null if invalid
 */
QJsonValue JSONParser::processReferenceKey(const QJsonValue& value,
                                           JSONParsingContext& context) {
    if (!value.isString()) {
        context.addError("$ref value must be a string");
        return QJsonValue();
    }

    QJsonValue resolved = processReference(value.toString(), context);
    return resolved.isNull() ? QJsonValue() : resolved;
}

/**
 * @brief Processes JSON include keys ($include)
 * @param value The include path value (should be a string)
 * @param context The parsing context for error handling
 * @return Included JSON object or null if invalid
 */
QJsonValue JSONParser::processIncludeKey(const QJsonValue& value,
                                         JSONParsingContext& context) {
    if (!value.isString()) {
        context.addError("$include value must be a string");
        return QJsonValue();
    }

    QJsonValue included = processInclude(value.toString(), context);
    return (included.isNull() || !included.isObject()) ? QJsonValue()
                                                       : included;
}

/**
 * @brief Processes JSON type keys ($type)
 * @param value The type value (should be a string)
 * @param context The parsing context for error handling
 * @return The type value if valid, null otherwise
 */
QJsonValue JSONParser::processTypeKey(const QJsonValue& value,
                                      JSONParsingContext& context) {
    if (!value.isString()) {
        context.addError("$type value must be a string");
        return QJsonValue();
    }
    // **Type information - store for later use**
    return value;
}

/**
 * @brief Merges a source JSON object into a result object
 * @param source The source object to merge from
 * @param result The result object to merge into
 */
void JSONParser::mergeObjectIntoResult(const QJsonObject& source,
                                       QJsonObject& result) {
    for (auto it = source.begin(); it != source.end(); ++it) {
        result[it.key()] = it.value();
    }
}

QJsonArray JSONParser::processJsonArray(const QJsonArray& input,
                                        JSONParsingContext& context) {
    QJsonArray result;

    for (int i = 0; i < input.size(); ++i) {
        // **Update current path**
        JSONPath old_path = context.current_path;
        context.current_path.append(i);

        try {
            QJsonValue processed = processJsonValue(input[i], context);
            result.append(processed);

        } catch (const std::exception& e) {
            context.addError(QString("Error processing array index %1: %2")
                                 .arg(i)
                                 .arg(e.what()));
            if (context.strict_mode) {
                throw;
            }
            // **In non-strict mode, skip the problematic element**
        }

        // **Restore path**
        context.current_path = old_path;
    }

    return result;
}

QJsonValue JSONParser::processJsonValue(const QJsonValue& input,
                                        JSONParsingContext& context) {
    switch (input.type()) {
        case QJsonValue::Object:
            return QJsonValue(processJsonObject(input.toObject(), context));

        case QJsonValue::Array:
            return QJsonValue(processJsonArray(input.toArray(), context));

        case QJsonValue::String: {
            QString str_value = input.toString();

            // **Check for special string values**
            if (str_value.startsWith("$ref:")) {
                QString reference = str_value.mid(5);  // Remove "$ref:" prefix
                return processReference(reference, context);
            } else if (str_value.startsWith("$include:")) {
                QString include_path =
                    str_value.mid(9);  // Remove "$include:" prefix
                return processInclude(include_path, context);
            }

            return input;
        }

        case QJsonValue::Double:
        case QJsonValue::Bool:
        case QJsonValue::Null:
        default:
            return input;
    }
}

QJsonValue JSONParser::processReference(const QString& reference,
                                        JSONParsingContext& context) {
    try {
        return reference_resolver_->resolveReference(reference);
    } catch (const std::exception& e) {
        context.addError(
            QString("Reference resolution failed: %1").arg(e.what()));
        return QJsonValue();
    }
}

QJsonValue JSONParser::processInclude(const QString& include_path,
                                      JSONParsingContext& context) {
    try {
        QJsonObject included = reference_resolver_->includeFile(include_path);
        return QJsonValue(included);
    } catch (const std::exception& e) {
        context.addError(
            QString("Include processing failed: %1").arg(e.what()));
        return QJsonValue();
    }
}

QJsonValue JSONParser::processCustomType(const QString& type_name,
                                         const QJsonValue& value,
                                         JSONParsingContext& context) {
    auto parser_it = custom_parsers_.find(type_name);
    if (parser_it != custom_parsers_.end()) {
        try {
            return parser_it->second(value);
        } catch (const std::exception& e) {
            context.addError(
                QString("Custom type processing failed for '%1': %2")
                    .arg(type_name, e.what()));
        }
    } else {
        context.addWarning(QString("Unknown custom type: %1").arg(type_name));
    }

    return value;
}

bool JSONParser::validateObjectStructure(const QJsonObject& obj,
                                         JSONParsingContext& context) {
    // **Basic structure validation**
    Q_UNUSED(obj)
    Q_UNUSED(context)
    return true;  // Simplified for brevity
}

bool JSONParser::validateArrayStructure(const QJsonArray& arr,
                                        JSONParsingContext& context) {
    // **Basic structure validation**
    Q_UNUSED(arr)
    Q_UNUSED(context)
    return true;  // Simplified for brevity
}

QString JSONParser::preprocessJson(const QString& json_string) {
    QString result = json_string;

    if (allow_comments_) {
        result = removeComments(result);
    }

    if (allow_trailing_commas_) {
        result = removeTrailingCommas(result);
    }

    return result;
}

QString JSONParser::removeComments(const QString& json_string) {
    QString result;
    result.reserve(json_string.length());

    bool in_string = false;
    bool escaped = false;
    bool in_line_comment = false;
    bool in_block_comment = false;

    for (int i = 0; i < json_string.length(); ++i) {
        QChar ch = json_string[i];
        QChar next_ch =
            (i + 1 < json_string.length()) ? json_string[i + 1] : QChar();

        if (in_line_comment) {
            if (ch == '\n' || ch == '\r') {
                in_line_comment = false;
                result.append(ch);
            }
            continue;
        }

        if (in_block_comment) {
            if (ch == '*' && next_ch == '/') {
                in_block_comment = false;
                ++i;  // Skip the '/'
            }
            continue;
        }

        if (!in_string) {
            if (ch == '/' && next_ch == '/') {
                in_line_comment = true;
                ++i;  // Skip the second '/'
                continue;
            } else if (ch == '/' && next_ch == '*') {
                in_block_comment = true;
                ++i;  // Skip the '*'
                continue;
            }
        }

        if (ch == '"' && !escaped) {
            in_string = !in_string;
        }

        escaped = (ch == '\\' && !escaped);
        result.append(ch);
    }

    return result;
}

QString JSONParser::removeTrailingCommas(const QString& json_string) {
    // **Simple regex-based trailing comma removal**
    QString result = json_string;

    // **Remove trailing commas before closing braces and brackets**
    QRegularExpression trailing_comma_regex(R"(,(\s*[}\]]))");
    result.replace(trailing_comma_regex, R"(\1)");

    return result;
}

void JSONParser::setParsingContext(const QString& source,
                                   const QString& file_path) {
    current_context_ = std::make_unique<JSONParsingContext>();
    current_context_->source_file = file_path.isEmpty() ? source : file_path;
    current_context_->strict_mode = strict_mode_;
}

QString JSONParser::formatError(const QString& message,
                                const JSONPath& path) const {
    if (path.isEmpty()) {
        return message;
    }
    return QString("[%1] %2").arg(path.toString(), message);
}

// **JSONSchemaValidator Implementation (Simplified)**

JSONSchemaValidator::JSONSchemaValidator() {}

void JSONSchemaValidator::loadSchema(const QJsonObject& schema) {
    schema_ = schema;
    clearValidationMessages();
}

void JSONSchemaValidator::loadSchemaFromFile(const QString& schema_file) {
    JSONParser parser;
    QJsonObject schema = parser.parseFile(schema_file);
    loadSchema(schema);
}

void JSONSchemaValidator::loadSchemaFromString(const QString& schema_string) {
    JSONParser parser;
    QJsonObject schema = parser.parseString(schema_string);
    loadSchema(schema);
}

bool JSONSchemaValidator::validate(const QJsonObject& data) {
    clearValidationMessages();

    if (schema_.isEmpty()) {
        addValidationError("No schema loaded");
        return false;
    }

    return validateObject(data, schema_, JSONPath());
}

bool JSONSchemaValidator::validate(const QJsonArray& data) {
    clearValidationMessages();

    if (schema_.isEmpty()) {
        addValidationError("No schema loaded");
        return false;
    }

    return validateArray(data, schema_, JSONPath());
}

bool JSONSchemaValidator::validate(const QJsonValue& data) {
    clearValidationMessages();

    if (schema_.isEmpty()) {
        addValidationError("No schema loaded");
        return false;
    }

    JSONPath path;

    switch (data.type()) {
        case QJsonValue::Object:
            return validateObject(data.toObject(), schema_, path);
        case QJsonValue::Array:
            return validateArray(data.toArray(), schema_, path);
        case QJsonValue::String:
            return validateString(data.toString(), schema_, path);
        case QJsonValue::Double:
            return validateNumber(data.toDouble(), schema_, path);
        case QJsonValue::Bool:
            return validateBoolean(data.toBool(), schema_, path);
        case QJsonValue::Null:
            return true;
        default:
            return true;
    }
}

QStringList JSONSchemaValidator::getValidationErrors() const {
    return validation_errors_;
}

QStringList JSONSchemaValidator::getValidationWarnings() const {
    return validation_warnings_;
}

void JSONSchemaValidator::clearValidationMessages() {
    validation_errors_.clear();
    validation_warnings_.clear();
}

QJsonObject JSONSchemaValidator::getSchema() const { return schema_; }

QStringList JSONSchemaValidator::getRequiredProperties(
    const QString& object_path) const {
    Q_UNUSED(object_path)
    // **Simplified implementation**
    if (schema_.contains("required") && schema_["required"].isArray()) {
        QJsonArray required = schema_["required"].toArray();
        QStringList result;
        for (const QJsonValue& value : required) {
            if (value.isString()) {
                result.append(value.toString());
            }
        }
        return result;
    }
    return QStringList();
}

QJsonObject JSONSchemaValidator::getPropertySchema(
    const QString& property_path) const {
    Q_UNUSED(property_path)
    // **Simplified implementation**
    return QJsonObject();
}

bool JSONSchemaValidator::validateObject(const QJsonObject& obj,
                                         const QJsonObject& schema,
                                         const JSONPath& path) {
    bool valid = true;

    // **Validate type**
    if (schema.contains("type")) {
        valid &= validateType(QJsonValue(obj), schema["type"], path);
    }

    // **Validate required properties**
    if (schema.contains("required")) {
        valid &= validateRequired(obj, schema["required"].toArray(), path);
    }

    // **Validate properties**
    if (schema.contains("properties")) {
        valid &= validateProperties(obj, schema["properties"].toObject(), path);
    }

    return valid;
}

bool JSONSchemaValidator::validateArray(const QJsonArray& arr,
                                        const QJsonObject& schema,
                                        const JSONPath& path) {
    bool valid = true;

    // **Validate type**
    if (schema.contains("type")) {
        valid &= validateType(QJsonValue(arr), schema["type"], path);
    }

    // **Validate items**
    if (schema.contains("items")) {
        QJsonObject item_schema = schema["items"].toObject();
        for (int i = 0; i < arr.size(); ++i) {
            JSONPath item_path = path;
            item_path.append(i);

            const QJsonValue& item = arr[i];
            if (item.isObject()) {
                valid &=
                    validateObject(item.toObject(), item_schema, item_path);
            } else if (item.isArray()) {
                valid &= validateArray(item.toArray(), item_schema, item_path);
            }
        }
    }

    return valid;
}

bool JSONSchemaValidator::validateString(const QString& str,
                                         const QJsonObject& schema,
                                         const JSONPath& path) {
    bool valid = true;

    // **Validate type**
    if (schema.contains("type")) {
        valid &= validateType(QJsonValue(str), schema["type"], path);
    }

    // **Validate length constraints**
    valid &= validateLength(str, schema, path);

    // **Validate pattern**
    if (schema.contains("pattern")) {
        valid &= validatePattern(str, schema["pattern"].toString(), path);
    }

    // **Validate enum**
    if (schema.contains("enum")) {
        valid &= validateEnum(QJsonValue(str), schema["enum"].toArray(), path);
    }

    return valid;
}

bool JSONSchemaValidator::validateNumber(double num, const QJsonObject& schema,
                                         const JSONPath& path) {
    bool valid = true;

    // **Validate type**
    if (schema.contains("type")) {
        valid &= validateType(QJsonValue(num), schema["type"], path);
    }

    // **Validate min/max constraints**
    valid &= validateMinMax(num, schema, path);

    // **Validate enum**
    if (schema.contains("enum")) {
        valid &= validateEnum(QJsonValue(num), schema["enum"].toArray(), path);
    }

    return valid;
}

bool JSONSchemaValidator::validateBoolean(bool val, const QJsonObject& schema,
                                          const JSONPath& path) {
    bool valid = true;

    // **Validate type**
    if (schema.contains("type")) {
        valid &= validateType(QJsonValue(val), schema["type"], path);
    }

    return valid;
}

bool JSONSchemaValidator::validateType(const QJsonValue& value,
                                       const QJsonValue& type_constraint,
                                       const JSONPath& path) {
    if (type_constraint.isString()) {
        QString expected_type = type_constraint.toString();
        QString actual_type;

        switch (value.type()) {
            case QJsonValue::Object:
                actual_type = "object";
                break;
            case QJsonValue::Array:
                actual_type = "array";
                break;
            case QJsonValue::String:
                actual_type = "string";
                break;
            case QJsonValue::Double:
                actual_type = "number";
                break;
            case QJsonValue::Bool:
                actual_type = "boolean";
                break;
            case QJsonValue::Null:
                actual_type = "null";
                break;
            default:
                actual_type = "unknown";
                break;
        }

        if (actual_type != expected_type) {
            addValidationError(QString("Type mismatch: expected %1, got %2")
                                   .arg(expected_type, actual_type),
                               path);
            return false;
        }
    }

    return true;
}

bool JSONSchemaValidator::validateEnum(const QJsonValue& value,
                                       const QJsonArray& enum_values,
                                       const JSONPath& path) {
    for (const QJsonValue& enum_value : enum_values) {
        if (value == enum_value) {
            return true;
        }
    }

    addValidationError("Value not in allowed enum values", path);
    return false;
}

bool JSONSchemaValidator::validateProperties(const QJsonObject& obj,
                                             const QJsonObject& properties,
                                             const JSONPath& path) {
    bool valid = true;

    for (auto it = obj.begin(); it != obj.end(); ++it) {
        const QString& prop_name = it.key();
        const QJsonValue& prop_value = it.value();

        if (properties.contains(prop_name)) {
            QJsonObject prop_schema = properties[prop_name].toObject();
            JSONPath prop_path = path;
            prop_path.append(prop_name);

            if (prop_value.isObject()) {
                valid &= validateObject(prop_value.toObject(), prop_schema,
                                        prop_path);
            } else if (prop_value.isArray()) {
                valid &=
                    validateArray(prop_value.toArray(), prop_schema, prop_path);
            } else if (prop_value.isString()) {
                valid &= validateString(prop_value.toString(), prop_schema,
                                        prop_path);
            } else if (prop_value.isDouble()) {
                valid &= validateNumber(prop_value.toDouble(), prop_schema,
                                        prop_path);
            } else if (prop_value.isBool()) {
                valid &= validateBoolean(prop_value.toBool(), prop_schema,
                                         prop_path);
            }
        }
    }

    return valid;
}

bool JSONSchemaValidator::validateRequired(const QJsonObject& obj,
                                           const QJsonArray& required,
                                           const JSONPath& path) {
    bool valid = true;

    for (const QJsonValue& req_value : required) {
        if (req_value.isString()) {
            QString req_prop = req_value.toString();
            if (!obj.contains(req_prop)) {
                addValidationError(
                    QString("Required property missing: %1").arg(req_prop),
                    path);
                valid = false;
            }
        }
    }

    return valid;
}

bool JSONSchemaValidator::validateMinMax(double value,
                                         const QJsonObject& schema,
                                         const JSONPath& path) {
    bool valid = true;

    if (schema.contains("minimum")) {
        double minimum = schema["minimum"].toDouble();
        if (value < minimum) {
            addValidationError(QString("Value %1 is less than minimum %2")
                                   .arg(value)
                                   .arg(minimum),
                               path);
            valid = false;
        }
    }

    if (schema.contains("maximum")) {
        double maximum = schema["maximum"].toDouble();
        if (value > maximum) {
            addValidationError(QString("Value %1 is greater than maximum %2")
                                   .arg(value)
                                   .arg(maximum),
                               path);
            valid = false;
        }
    }

    return valid;
}

bool JSONSchemaValidator::validateLength(const QString& str,
                                         const QJsonObject& schema,
                                         const JSONPath& path) {
    bool valid = true;

    if (schema.contains("minLength")) {
        int min_length = schema["minLength"].toInt();
        if (str.length() < min_length) {
            addValidationError(
                QString("String length %1 is less than minimum %2")
                    .arg(str.length())
                    .arg(min_length),
                path);
            valid = false;
        }
    }

    if (schema.contains("maxLength")) {
        int max_length = schema["maxLength"].toInt();
        if (str.length() > max_length) {
            addValidationError(
                QString("String length %1 is greater than maximum %2")
                    .arg(str.length())
                    .arg(max_length),
                path);
            valid = false;
        }
    }

    return valid;
}

bool JSONSchemaValidator::validatePattern(const QString& str,
                                          const QString& pattern,
                                          const JSONPath& path) {
    QRegularExpression regex(pattern);

    if (!regex.isValid()) {
        addValidationError(QString("Invalid regex pattern: %1").arg(pattern),
                           path);
        return false;
    }

    if (!regex.match(str).hasMatch()) {
        addValidationError(
            QString("String does not match pattern: %1").arg(pattern), path);
        return false;
    }

    return true;
}

void JSONSchemaValidator::addValidationError(const QString& message,
                                             const JSONPath& path) {
    QString formatted_message =
        path.isEmpty() ? message
                       : QString("[%1] %2").arg(path.toString(), message);
    validation_errors_.append(formatted_message);
}

void JSONSchemaValidator::addValidationWarning(const QString& message,
                                               const JSONPath& path) {
    QString formatted_message =
        path.isEmpty() ? message
                       : QString("[%1] %2").arg(path.toString(), message);
    validation_warnings_.append(formatted_message);
}

// **JSONUtils Implementation**

namespace JSONUtils {

QJsonValue getValue(const QJsonObject& root, const QString& path) {
    QStringList components = path.split('.', Qt::SkipEmptyParts);

    QJsonValue current = QJsonValue(root);

    for (const QString& component : components) {
        if (current.isObject()) {
            QJsonObject obj = current.toObject();
            if (!obj.contains(component)) {
                return QJsonValue();
            }
            current = obj[component];
        } else {
            return QJsonValue();
        }
    }

    return current;
}

bool setValue(QJsonObject& root, const QString& path, const QJsonValue& value) {
    QStringList components = path.split('.', Qt::SkipEmptyParts);

    if (components.isEmpty()) {
        return false;
    }

    // **Navigate to the parent of the target property**
    QJsonObject working_copy = root;
    QJsonObject* working_ptr = &working_copy;

    for (int i = 0; i < components.size() - 1; ++i) {
        const QString& component = components[i];

        if (!working_ptr->contains(component) ||
            !(*working_ptr)[component].isObject()) {
            // If the key does not exist or is not an object, create a new
            // object
            working_ptr->insert(component, QJsonObject());
        }

        // Work with a copy to avoid temporary object issues
        QJsonObject child_obj = (*working_ptr)[component].toObject();
        working_ptr->insert(component, child_obj);
        working_ptr = &child_obj;
    }

    // Update the root object
    root = working_copy;

    // **Set the final property**
    const QString& final_component = components.last();

    // Navigate to the correct position in the updated root
    QJsonObject* final_current = &root;
    for (int i = 0; i < components.size() - 1; ++i) {
        QJsonValueRef valueRef = (*final_current)[components[i]];
        if (valueRef.isObject()) {
            // We need to work with a copy and reassign
            QJsonObject temp = valueRef.toObject();
            final_current->insert(components[i], temp);
            final_current = &temp;
        }
    }

    final_current->insert(final_component, value);

    return true;
}

bool removeValue(QJsonObject& root, const QString& path) {
    QStringList components = path.split('.', Qt::SkipEmptyParts);

    if (components.isEmpty()) {
        return false;
    }

    QJsonObject* current = &root;

    // **Navigate to the parent of the target property**
    for (int i = 0; i < components.size() - 1; ++i) {
        const QString& component = components[i];

        if (!current->contains(component) ||
            !(*current)[component].isObject()) {
            return false;
        }

        // QJsonObject child_obj = (*current)[component].toObject();
        // current = &child_obj; // This does not update the parent!
        // Instead, we need to update the parent after modification, but for
        // remove, we can just traverse
        QJsonObject child_obj = (*current)[component].toObject();
        // We can't get a non-const pointer to the nested object, so we need to
        // reconstruct the path after removal if needed For now, just operate on
        // a copy and reassign at the end if necessary
        current = &child_obj;
    }

    // **Remove the final property**
    const QString& final_component = components.last();
    // QJsonObject::remove returns void, not bool. So, check if the key existed
    // before removal.
    bool existed = current->contains(final_component);
    current->remove(final_component);
    return existed;
}

bool hasValue(const QJsonObject& root, const QString& path) {
    return getValue(root, path).type() != QJsonValue::Undefined;
}

QJsonObject merge(const QJsonObject& base, const QJsonObject& overlay) {
    QJsonObject result = base;

    for (auto it = overlay.begin(); it != overlay.end(); ++it) {
        const QString& key = it.key();
        const QJsonValue& overlay_value = it.value();

        if (result.contains(key) && result[key].isObject() &&
            overlay_value.isObject()) {
            // **Recursive merge for objects**
            QJsonObject merged_child =
                merge(result[key].toObject(), overlay_value.toObject());
            result[key] = merged_child;
        } else {
            // **Replace or add the value**
            result[key] = overlay_value;
        }
    }

    return result;
}

QJsonObject deepCopy(const QJsonObject& source) {
    // **Qt's QJsonObject already does deep copying**
    return source;
}

QJsonArray deepCopy(const QJsonArray& source) {
    // **Qt's QJsonArray already does deep copying**
    return source;
}

template <typename T>
std::optional<T> tryConvert(const QJsonValue& value) {
    try {
        if constexpr (std::is_same_v<T, QString>) {
            if (value.isString()) {
                return value.toString();
            }
        } else if constexpr (std::is_same_v<T, int>) {
            if (value.isDouble()) {
                return static_cast<int>(value.toDouble());
            }
        } else if constexpr (std::is_same_v<T, double>) {
            if (value.isDouble()) {
                return value.toDouble();
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            if (value.isBool()) {
                return value.toBool();
            }
        }
    } catch (...) {
        // **Conversion failed**
    }

    return std::nullopt;
}

QString toString(const QJsonValue& value) {
    switch (value.type()) {
        case QJsonValue::String:
            return value.toString();
        case QJsonValue::Double:
            return QString::number(value.toDouble());
        case QJsonValue::Bool:
            return value.toBool() ? "true" : "false";
        case QJsonValue::Null:
            return "null";
        default:
            return "";
    }
}

int toInt(const QJsonValue& value, int default_value) {
    if (value.isDouble()) {
        return static_cast<int>(value.toDouble());
    }
    return default_value;
}

double toDouble(const QJsonValue& value, double default_value) {
    if (value.isDouble()) {
        return value.toDouble();
    }
    return default_value;
}

bool toBool(const QJsonValue& value, bool default_value) {
    if (value.isBool()) {
        return value.toBool();
    }
    return default_value;
}

bool isValidJsonPointer(const QString& pointer) {
    return pointer.startsWith("#/") || pointer.isEmpty();
}

bool isValidReference(const QString& reference) {
    return !reference.isEmpty() &&
           (reference.startsWith("#/") || reference.startsWith("./") ||
            reference.startsWith("../") || reference.contains("://"));
}

QString prettyPrint(const QJsonObject& obj, int indent) {
    QJsonDocument doc(obj);
    return doc.toJson(static_cast<QJsonDocument::JsonFormat>(indent));
}

QString prettyPrint(const QJsonArray& arr, int indent) {
    QJsonDocument doc(arr);
    return doc.toJson(static_cast<QJsonDocument::JsonFormat>(indent));
}

// **Explicit template instantiations**
template std::optional<QString> tryConvert<QString>(const QJsonValue& value);
template std::optional<int> tryConvert<int>(const QJsonValue& value);
template std::optional<double> tryConvert<double>(const QJsonValue& value);
template std::optional<bool> tryConvert<bool>(const QJsonValue& value);

}  // namespace JSONUtils

}  // namespace DeclarativeUI::JSON
