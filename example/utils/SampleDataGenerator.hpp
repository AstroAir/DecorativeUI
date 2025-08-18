/**
 * @file SampleDataGenerator.hpp
 * @brief Generate sample data for demonstrations
 */

#pragma once

#include <QString>
#include <QStringList>
#include <QJsonObject>
#include <QJsonArray>
#include <QPointF>
#include <QColor>
#include <QDateTime>
#include <QRandomGenerator>

/**
 * @brief Sample data structure for various demonstrations
 */
struct SampleData {
    QString name;
    QString description;
    QVariant value;
    QDateTime timestamp;
    QString category;
    
    SampleData(const QString& n, const QString& d, const QVariant& v, 
               const QString& c = "default")
        : name(n), description(d), value(v), timestamp(QDateTime::currentDateTime()), category(c) {}
};

/**
 * @brief Generator for various types of sample data
 */
class SampleDataGenerator {
public:
    SampleDataGenerator();
    
    // Basic data generation
    static QStringList generateNames(int count = 100);
    static QStringList generateEmails(int count = 100);
    static QStringList generateAddresses(int count = 100);
    static QStringList generateCompanyNames(int count = 50);
    
    // Numeric data generation
    static QList<int> generateRandomIntegers(int count, int min = 0, int max = 100);
    static QList<double> generateRandomDoubles(int count, double min = 0.0, double max = 100.0);
    static QList<QPointF> generateChartData(int points, double x_min = 0.0, double x_max = 100.0);
    static QList<QPointF> generateTimeSeriesData(int days = 30);
    
    // Table data generation
    static QList<QStringList> generateTableData(int rows, int columns);
    static QList<QStringList> generatePersonData(int count = 100);
    static QList<QStringList> generateSalesData(int count = 100);
    static QList<QStringList> generateInventoryData(int count = 100);
    
    // JSON data generation
    static QJsonObject generateUserProfile();
    static QJsonArray generateUserProfiles(int count = 10);
    static QJsonObject generateApplicationSettings();
    static QJsonObject generatePerformanceMetrics();
    
    // Color data generation
    static QList<QColor> generateColorPalette(int count = 10);
    static QColor generateRandomColor();
    static QStringList generateColorNames(int count = 20);
    
    // Date/Time data generation
    static QList<QDateTime> generateDateRange(const QDateTime& start, const QDateTime& end, int count);
    static QStringList generateTimeZones();
    
    // Text data generation
    static QString generateLoremIpsum(int words = 50);
    static QStringList generateSentences(int count = 10);
    static QStringList generateParagraphs(int count = 5);
    
    // Configuration data
    static QJsonObject generateUIConfiguration();
    static QJsonObject generateThemeConfiguration();
    static QJsonObject generateComponentConfiguration(const QString& component_type);
    
private:
    static QStringList first_names_;
    static QStringList last_names_;
    static QStringList company_suffixes_;
    static QStringList lorem_words_;
    static bool data_initialized_;
    
    static void initializeData();
    static QString getRandomItem(const QStringList& list);
    static int getRandomInt(int min, int max);
    static double getRandomDouble(double min, double max);
};
