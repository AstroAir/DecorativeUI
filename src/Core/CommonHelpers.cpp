/**
 * @file CommonHelpers.cpp
 * @brief Implementation of common helper functions for DeclarativeUI framework
 *
 * This file contains the implementations of reusable helper functions that
 * reduce code duplication and provide consistent behavior across the framework.
 *
 * @author DeclarativeUI Team
 * @version 1.0
 * @date 2024
 */

#include "CommonHelpers.hpp"
#include <QJsonArray>

namespace DeclarativeUI::Core::Helpers {

namespace Validation {

/**
 * @brief Validate JSON object has required fields
 * @param jsonObj The JSON object to validate
 * @param requiredFields List of required field names
 * @return true if all required fields are present
 *
 * This function checks that all specified fields exist in the JSON object.
 * It's commonly used in JSON schema validation and configuration parsing.
 */
bool validateJsonRequiredFields(const QJsonObject& jsonObj,
                                const QStringList& requiredFields) {
    for (const QString& field : requiredFields) {
        if (!jsonObj.contains(field)) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Validate property type matches expected type
 * @param value The value to check
 * @param expectedType The expected type string ("string", "number", "boolean",
 * etc.)
 * @return true if types match
 *
 * This function provides consistent type validation for JSON values,
 * commonly used in schema validation and property binding.
 */
bool validatePropertyType(const QJsonValue& value,
                          const QString& expectedType) {
    if (expectedType == "string" && !value.isString()) {
        return false;
    }
    if (expectedType == "number" && !value.isDouble()) {
        return false;
    }
    if (expectedType == "boolean" && !value.isBool()) {
        return false;
    }
    if (expectedType == "array" && !value.isArray()) {
        return false;
    }
    if (expectedType == "object" && !value.isObject()) {
        return false;
    }
    return true;
}

}  // namespace Validation

namespace StringUtils {

/**
 * @brief Generate a unique state key for command properties
 * @param commandType The command type (e.g., "ButtonCommand")
 * @param commandId The command ID (UUID string)
 * @param property The property name (e.g., "text", "enabled")
 * @return Formatted state key in the format "CommandType_ID_Property"
 *
 * This function provides consistent state key generation across the framework,
 * ensuring unique and predictable keys for state management.
 */
QString generateStateKey(const QString& commandType, const QString& commandId,
                         const QString& property) {
    return QString("%1_%2_%3").arg(commandType, commandId, property);
}

/**
 * @brief Generate a unique action name for command events
 * @param commandType The command type (e.g., "ButtonCommand")
 * @param commandId The command ID (UUID string)
 * @param eventType The event type (e.g., "clicked", "valueChanged")
 * @return Formatted action name in the format "CommandType_ID_EventType"
 *
 * This function provides consistent action name generation for event handling
 * and MVC integration patterns.
 */
QString generateActionName(const QString& commandType, const QString& commandId,
                           const QString& eventType) {
    return QString("%1_%2_%3").arg(commandType, commandId, eventType);
}

/**
 * @brief Format error messages consistently
 * @param operation The operation that failed (e.g., "Widget creation")
 * @param target The target of the operation (e.g., "Button", "Layout")
 * @param reason The reason for failure (e.g., "Invalid parameters")
 * @return Formatted error message
 *
 * This function ensures consistent error message formatting across the
 * framework, making error handling more predictable and user-friendly.
 */
QString formatErrorMessage(const QString& operation, const QString& target,
                           const QString& reason) {
    return QString("%1 failed for %2: %3").arg(operation, target, reason);
}

}  // namespace StringUtils

}  // namespace DeclarativeUI::Core::Helpers
