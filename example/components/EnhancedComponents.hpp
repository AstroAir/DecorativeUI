/**
 * @file EnhancedComponents.hpp
 * @brief Collection of enhanced component utilities and extensions
 */

#pragma once

#include <QColor>
#include <QIcon>
#include <QString>
#include <QWidget>
#include <memory>

// Include all enhanced components
#include "Chart.hpp"
#include "DataTable.hpp"
#include "NavigationBar.hpp"
#include "SearchBox.hpp"
#include "StatusIndicator.hpp"

/**
 * @brief Utility class for enhanced component creation and management
 */
class EnhancedComponents {
public:
    // Factory methods for enhanced components
    static std::unique_ptr<NavigationBar> createNavigationBar(
        QWidget* parent = nullptr);
    static std::unique_ptr<StatusIndicator> createStatusIndicator(
        QWidget* parent = nullptr);
    static std::unique_ptr<SearchBox> createSearchBox(
        QWidget* parent = nullptr);
    static std::unique_ptr<DataTable> createDataTable(
        QWidget* parent = nullptr);
    static std::unique_ptr<Chart> createChart(QWidget* parent = nullptr);

    // Utility functions
    static void applyModernStyling(QWidget* widget);
    static void addDropShadow(QWidget* widget);
    static void addHoverEffect(QWidget* widget);
    static QIcon createColoredIcon(const QString& icon_name,
                                   const QColor& color);

    // Theme utilities
    static void applyLightTheme(QWidget* widget);
    static void applyDarkTheme(QWidget* widget);
    static void applyCustomTheme(QWidget* widget, const QColor& primary,
                                 const QColor& secondary);

private:
    EnhancedComponents() = default;
};
