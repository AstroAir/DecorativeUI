/**
 * @file ExampleHelpers.hpp
 * @brief Common helper functions and utilities for the showcase
 */

#pragma once

#include <QString>
#include <QStringList>
#include <QColor>
#include <QWidget>
#include <QJsonObject>
#include <QJsonDocument>
#include <memory>

/**
 * @brief Collection of helper functions for the showcase application
 */
class ExampleHelpers {
public:
    ExampleHelpers();
    
    // String utilities
    static QString formatFileSize(qint64 bytes);
    static QString formatDuration(qint64 milliseconds);
    static QString formatMemoryUsage(qint64 bytes);
    static QString capitalizeFirst(const QString& text);
    
    // Color utilities
    static QColor interpolateColor(const QColor& from, const QColor& to, qreal progress);
    static QColor generateRandomColor();
    static QStringList getColorPalette(const QString& theme = "default");
    
    // Widget utilities
    static void centerWidget(QWidget* widget, QWidget* parent = nullptr);
    static void fadeInWidget(QWidget* widget, int duration_ms = 300);
    static void fadeOutWidget(QWidget* widget, int duration_ms = 300);
    static void slideInWidget(QWidget* widget, const QString& direction = "left", int duration_ms = 300);
    
    // Data generation
    static QStringList generateSampleNames(int count = 100);
    static QList<QPointF> generateSampleChartData(int points = 50);
    static QList<QStringList> generateSampleTableData(int rows = 100, int columns = 5);
    
    // File utilities
    static bool saveJsonToFile(const QJsonObject& json, const QString& filename);
    static QJsonObject loadJsonFromFile(const QString& filename);
    static QString getResourcePath(const QString& resource_name);
    static QStringList getAvailableThemes();
    
    // Performance utilities
    static qint64 getCurrentMemoryUsage();
    static double getCurrentCpuUsage();
    static int getFrameRate();
    
    // Validation utilities
    static bool isValidEmail(const QString& email);
    static bool isValidUrl(const QString& url);
    static bool isValidColor(const QString& color);
    
    // Debug utilities
    static void logComponentCreation(const QString& component_name);
    static void logPerformanceMetric(const QString& metric_name, qint64 value);
    static void logUserAction(const QString& action, const QJsonObject& data = QJsonObject());
    
private:
    static QString sample_names_cache_;
    static QList<QPointF> sample_chart_data_cache_;
    static bool caches_initialized_;
    
    static void initializeCaches();
};
