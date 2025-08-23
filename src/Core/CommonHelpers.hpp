/**
 * @file CommonHelpers.hpp
 * @brief Common helper functions and utilities for DeclarativeUI framework
 *
 * This file contains reusable helper functions and utilities that are commonly
 * used across the DeclarativeUI framework. These helpers reduce code
 * duplication and provide consistent implementations for common operations.
 *
 * @author DeclarativeUI Team
 * @version 1.0
 * @date 2024
 */

#pragma once

#include <QDebug>
#include <QJsonObject>
#include <QJsonValue>
#include <QString>
#include <QVariant>
#include <functional>
#include <memory>
#include <unordered_map>

namespace DeclarativeUI::Core::Helpers {

/**
 * @brief Common validation helper functions
 *
 * These functions provide reusable validation logic that appears
 * frequently across the codebase.
 */
namespace Validation {

/**
 * @brief Validate that a required parameter exists in a context
 * @param context The context object to check
 * @param paramName The parameter name to validate
 * @return true if parameter exists and is valid
 */
template <typename ContextType>
bool validateRequiredParameter(const ContextType& context,
                               const QString& paramName) {
    return context.hasParameter(paramName) &&
           !context.getParameter<QString>(paramName).isEmpty();
}

/**
 * @brief Validate multiple required parameters
 * @param context The context object to check
 * @param paramNames List of parameter names to validate
 * @return true if all parameters exist and are valid
 */
template <typename ContextType>
bool validateRequiredParameters(const ContextType& context,
                                const QStringList& paramNames) {
    for (const QString& paramName : paramNames) {
        if (!validateRequiredParameter(context, paramName)) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Validate JSON object has required fields
 * @param jsonObj The JSON object to validate
 * @param requiredFields List of required field names
 * @return true if all required fields are present
 */
bool validateJsonRequiredFields(const QJsonObject& jsonObj,
                                const QStringList& requiredFields);

/**
 * @brief Validate property type matches expected type
 * @param value The value to check
 * @param expectedType The expected type string
 * @return true if types match
 */
bool validatePropertyType(const QJsonValue& value, const QString& expectedType);

}  // namespace Validation

/**
 * @brief Common error handling and result creation helpers
 *
 * These functions provide consistent error handling and result creation
 * patterns used throughout the framework.
 */
namespace ErrorHandling {

/**
 * @brief Create a standardized error result
 * @tparam ResultType The result type to create
 * @param errorMessage The error message
 * @return Error result with the specified message
 */
template <typename ResultType>
ResultType createErrorResult(const QString& errorMessage) {
    return ResultType(errorMessage);
}

/**
 * @brief Create a widget not found error
 * @tparam ResultType The result type to create
 * @param widgetType The type of widget that wasn't found
 * @param widgetName The name of the widget that wasn't found
 * @return Standardized widget not found error
 */
template <typename ResultType>
ResultType createWidgetNotFoundError(const QString& widgetType,
                                     const QString& widgetName) {
    return ResultType(QString("%1 '%2' not found").arg(widgetType, widgetName));
}

/**
 * @brief Create a success result with message
 * @tparam ResultType The result type to create
 * @param operation The operation that succeeded
 * @param target The target of the operation
 * @return Standardized success result
 */
template <typename ResultType>
ResultType createSuccessResult(const QString& operation,
                               const QString& target) {
    return ResultType(QString("%1 %2 successful").arg(operation, target));
}

/**
 * @brief Log and handle sync errors consistently
 * @param source The source object where error occurred
 * @param errorMessage The error message
 */
template <typename SourceType>
void handleSyncError(std::shared_ptr<SourceType> source,
                     const QString& errorMessage) {
    qWarning() << "Sync error for" << source->getCommandType() << ":"
               << errorMessage;
    // Could emit signals or perform other error handling here
}

}  // namespace ErrorHandling

/**
 * @brief Common lookup table patterns
 *
 * These utilities help create and manage lookup tables that replace
 * complex switch statements for better performance and maintainability.
 */
namespace LookupTables {

/**
 * @brief Create a static lookup table for enum-to-string mappings
 * @tparam EnumType The enum type
 * @param mappings Initializer list of enum-to-string pairs
 * @return Function that performs the lookup
 */
template <typename EnumType>
std::function<QString(EnumType)> createEnumLookup(
    std::initializer_list<std::pair<EnumType, QString>> mappings) {
    static std::unordered_map<EnumType, QString> lookupMap(mappings);

    return [](EnumType value) -> QString {
        auto it = lookupMap.find(value);
        return (it != lookupMap.end()) ? it->second : "Unknown";
    };
}

/**
 * @brief Create a bidirectional lookup table
 * @tparam KeyType The key type
 * @tparam ValueType The value type
 * @param mappings Initializer list of key-value pairs
 * @return Pair of lookup functions (key->value, value->key)
 */
template <typename KeyType, typename ValueType>
std::pair<std::function<ValueType(KeyType)>, std::function<KeyType(ValueType)>>
createBidirectionalLookup(
    std::initializer_list<std::pair<KeyType, ValueType>> mappings) {
    static std::unordered_map<KeyType, ValueType> forwardMap(mappings);
    static std::unordered_map<ValueType, KeyType> reverseMap;

    // Build reverse map
    if (reverseMap.empty()) {
        for (const auto& pair : mappings) {
            reverseMap[pair.second] = pair.first;
        }
    }

    auto forwardLookup = [](KeyType key) -> ValueType {
        auto it = forwardMap.find(key);
        return (it != forwardMap.end()) ? it->second : ValueType{};
    };

    auto reverseLookup = [](ValueType value) -> KeyType {
        auto it = reverseMap.find(value);
        return (it != reverseMap.end()) ? it->second : KeyType{};
    };

    return {forwardLookup, reverseLookup};
}

}  // namespace LookupTables

/**
 * @brief Common string and formatting utilities
 *
 * These functions provide consistent string formatting and manipulation
 * patterns used throughout the framework.
 */
namespace StringUtils {

/**
 * @brief Generate a unique state key for command properties
 * @param commandType The command type
 * @param commandId The command ID
 * @param property The property name
 * @return Formatted state key
 */
QString generateStateKey(const QString& commandType, const QString& commandId,
                         const QString& property);

/**
 * @brief Generate a unique action name for command events
 * @param commandType The command type
 * @param commandId The command ID
 * @param eventType The event type
 * @return Formatted action name
 */
QString generateActionName(const QString& commandType, const QString& commandId,
                           const QString& eventType);

/**
 * @brief Format error messages consistently
 * @param operation The operation that failed
 * @param target The target of the operation
 * @param reason The reason for failure
 * @return Formatted error message
 */
QString formatErrorMessage(const QString& operation, const QString& target,
                           const QString& reason);

}  // namespace StringUtils

/**
 * @brief Thread-safe operation helpers
 *
 * These utilities provide common patterns for thread-safe operations
 * and lock management.
 */
namespace ThreadSafety {

/**
 * @brief RAII helper for lock upgrades
 * @tparam SharedLockType The shared lock type
 * @tparam ExclusiveLockType The exclusive lock type
 */
template <typename SharedLockType, typename ExclusiveLockType>
class LockUpgrader {
public:
    LockUpgrader(SharedLockType& sharedLock) : shared_lock_(sharedLock) {
        shared_lock_.unlock();
    }

    ExclusiveLockType getExclusiveLock() {
        return ExclusiveLockType(shared_lock_.mutex());
    }

private:
    SharedLockType& shared_lock_;
};

}  // namespace ThreadSafety

}  // namespace DeclarativeUI::Core::Helpers
