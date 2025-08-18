/**
 * @file SampleDataGenerator.cpp
 * @brief Implementation of sample data generator
 */

#include "SampleDataGenerator.hpp"
#include <QDebug>

// Static member initialization
QStringList SampleDataGenerator::first_names_;
QStringList SampleDataGenerator::last_names_;
QStringList SampleDataGenerator::company_suffixes_;
QStringList SampleDataGenerator::lorem_words_;
bool SampleDataGenerator::data_initialized_ = false;

SampleDataGenerator::SampleDataGenerator() {
    if (!data_initialized_) {
        initializeData();
    }
}

QStringList SampleDataGenerator::generateNames(int count) {
    if (!data_initialized_) initializeData();
    
    QStringList names;
    for (int i = 0; i < count; ++i) {
        QString first = getRandomItem(first_names_);
        QString last = getRandomItem(last_names_);
        names.append(QString("%1 %2").arg(first, last));
    }
    return names;
}

QStringList SampleDataGenerator::generateEmails(int count) {
    QStringList emails;
    QStringList domains = {"gmail.com", "yahoo.com", "hotmail.com", "company.com", "example.org"};
    
    for (int i = 0; i < count; ++i) {
        QString name = generateNames(1).first().toLower().replace(" ", ".");
        QString domain = getRandomItem(domains);
        emails.append(QString("%1@%2").arg(name, domain));
    }
    return emails;
}

QStringList SampleDataGenerator::generateAddresses(int count) {
    QStringList addresses;
    QStringList streets = {"Main St", "Oak Ave", "Pine Rd", "Elm Dr", "Maple Ln"};
    QStringList cities = {"Springfield", "Franklin", "Georgetown", "Madison", "Riverside"};
    
    for (int i = 0; i < count; ++i) {
        int number = getRandomInt(1, 9999);
        QString street = getRandomItem(streets);
        QString city = getRandomItem(cities);
        addresses.append(QString("%1 %2, %3").arg(number).arg(street, city));
    }
    return addresses;
}

QStringList SampleDataGenerator::generateCompanyNames(int count) {
    if (!data_initialized_) initializeData();
    
    QStringList companies;
    QStringList prefixes = {"Global", "Advanced", "Dynamic", "Innovative", "Premier"};
    QStringList bases = {"Tech", "Solutions", "Systems", "Industries", "Enterprises"};
    
    for (int i = 0; i < count; ++i) {
        QString prefix = getRandomItem(prefixes);
        QString base = getRandomItem(bases);
        QString suffix = getRandomItem(company_suffixes_);
        companies.append(QString("%1 %2 %3").arg(prefix, base, suffix));
    }
    return companies;
}

QList<int> SampleDataGenerator::generateRandomIntegers(int count, int min, int max) {
    QList<int> integers;
    for (int i = 0; i < count; ++i) {
        integers.append(getRandomInt(min, max));
    }
    return integers;
}

QList<double> SampleDataGenerator::generateRandomDoubles(int count, double min, double max) {
    QList<double> doubles;
    for (int i = 0; i < count; ++i) {
        doubles.append(getRandomDouble(min, max));
    }
    return doubles;
}

QList<QPointF> SampleDataGenerator::generateChartData(int points, double x_min, double x_max) {
    QList<QPointF> data;
    double x_step = (x_max - x_min) / (points - 1);
    
    for (int i = 0; i < points; ++i) {
        double x = x_min + i * x_step;
        double y = 50 + 30 * qSin(x * 0.1) + getRandomDouble(-10, 10);
        data.append(QPointF(x, y));
    }
    return data;
}

QList<QPointF> SampleDataGenerator::generateTimeSeriesData(int days) {
    QList<QPointF> data;
    QDateTime start_date = QDateTime::currentDateTime().addDays(-days);
    
    for (int i = 0; i < days; ++i) {
        double x = i;
        double y = 100 + 50 * qSin(i * 0.2) + getRandomDouble(-20, 20);
        data.append(QPointF(x, qMax(0.0, y)));
    }
    return data;
}

QList<QStringList> SampleDataGenerator::generateTableData(int rows, int columns) {
    QList<QStringList> data;
    
    for (int row = 0; row < rows; ++row) {
        QStringList row_data;
        for (int col = 0; col < columns; ++col) {
            row_data.append(QString("Cell %1-%2").arg(row + 1).arg(col + 1));
        }
        data.append(row_data);
    }
    return data;
}

QList<QStringList> SampleDataGenerator::generatePersonData(int count) {
    QList<QStringList> data;
    QStringList names = generateNames(count);
    QStringList emails = generateEmails(count);
    QStringList addresses = generateAddresses(count);
    
    for (int i = 0; i < count; ++i) {
        QStringList person;
        person << names[i] << emails[i] << addresses[i] << QString::number(getRandomInt(18, 80));
        data.append(person);
    }
    return data;
}

QList<QStringList> SampleDataGenerator::generateSalesData(int count) {
    QList<QStringList> data;
    QStringList products = {"Widget A", "Widget B", "Gadget X", "Tool Y", "Device Z"};
    
    for (int i = 0; i < count; ++i) {
        QStringList sale;
        sale << QString::number(i + 1); // ID
        sale << getRandomItem(products); // Product
        sale << QString::number(getRandomInt(1, 100)); // Quantity
        sale << QString("$%1").arg(getRandomDouble(10.0, 1000.0), 0, 'f', 2); // Price
        sale << QDateTime::currentDateTime().addDays(-getRandomInt(0, 365)).toString("yyyy-MM-dd"); // Date
        data.append(sale);
    }
    return data;
}

QList<QStringList> SampleDataGenerator::generateInventoryData(int count) {
    QList<QStringList> data;
    QStringList categories = {"Electronics", "Clothing", "Books", "Home", "Sports"};
    
    for (int i = 0; i < count; ++i) {
        QStringList item;
        item << QString("ITEM%1").arg(i + 1, 4, 10, QChar('0')); // SKU
        item << QString("Product %1").arg(i + 1); // Name
        item << getRandomItem(categories); // Category
        item << QString::number(getRandomInt(0, 1000)); // Stock
        item << QString("$%1").arg(getRandomDouble(5.0, 500.0), 0, 'f', 2); // Price
        data.append(item);
    }
    return data;
}

QJsonObject SampleDataGenerator::generateUserProfile() {
    QJsonObject profile;
    profile["name"] = generateNames(1).first();
    profile["email"] = generateEmails(1).first();
    profile["age"] = getRandomInt(18, 80);
    profile["address"] = generateAddresses(1).first();
    profile["phone"] = QString("(%1) %2-%3")
                      .arg(getRandomInt(100, 999))
                      .arg(getRandomInt(100, 999))
                      .arg(getRandomInt(1000, 9999));
    return profile;
}

QJsonArray SampleDataGenerator::generateUserProfiles(int count) {
    QJsonArray profiles;
    for (int i = 0; i < count; ++i) {
        profiles.append(generateUserProfile());
    }
    return profiles;
}

QJsonObject SampleDataGenerator::generateApplicationSettings() {
    QJsonObject settings;
    settings["theme"] = "light";
    settings["language"] = "en";
    settings["auto_save"] = true;
    settings["update_interval"] = 1000;
    settings["window_width"] = 1200;
    settings["window_height"] = 800;
    return settings;
}

QJsonObject SampleDataGenerator::generatePerformanceMetrics() {
    QJsonObject metrics;
    metrics["cpu_usage"] = getRandomDouble(0.0, 100.0);
    metrics["memory_usage"] = getRandomInt(100, 2000);
    metrics["disk_usage"] = getRandomDouble(0.0, 100.0);
    metrics["network_speed"] = getRandomDouble(1.0, 100.0);
    metrics["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    return metrics;
}

QList<QColor> SampleDataGenerator::generateColorPalette(int count) {
    QList<QColor> colors;
    for (int i = 0; i < count; ++i) {
        colors.append(generateRandomColor());
    }
    return colors;
}

QColor SampleDataGenerator::generateRandomColor() {
    return QColor(getRandomInt(0, 255), getRandomInt(0, 255), getRandomInt(0, 255));
}

QStringList SampleDataGenerator::generateColorNames(int count) {
    QStringList base_colors = {"Red", "Blue", "Green", "Yellow", "Purple", "Orange", "Pink", "Brown"};
    QStringList modifiers = {"Light", "Dark", "Bright", "Pale", "Deep", "Vivid"};
    
    QStringList colors;
    for (int i = 0; i < count; ++i) {
        if (getRandomInt(0, 1)) {
            colors.append(QString("%1 %2").arg(getRandomItem(modifiers), getRandomItem(base_colors)));
        } else {
            colors.append(getRandomItem(base_colors));
        }
    }
    return colors;
}

QList<QDateTime> SampleDataGenerator::generateDateRange(const QDateTime& start, const QDateTime& end, int count) {
    QList<QDateTime> dates;
    qint64 start_ms = start.toMSecsSinceEpoch();
    qint64 end_ms = end.toMSecsSinceEpoch();
    qint64 range_ms = end_ms - start_ms;
    
    for (int i = 0; i < count; ++i) {
        qint64 random_ms = start_ms + (range_ms * getRandomDouble(0.0, 1.0));
        dates.append(QDateTime::fromMSecsSinceEpoch(random_ms));
    }
    return dates;
}

QStringList SampleDataGenerator::generateTimeZones() {
    return {"UTC", "EST", "PST", "GMT", "CET", "JST", "AEST", "IST"};
}

QString SampleDataGenerator::generateLoremIpsum(int words) {
    if (!data_initialized_) initializeData();
    
    QStringList result;
    for (int i = 0; i < words; ++i) {
        result.append(getRandomItem(lorem_words_));
    }
    return result.join(" ");
}

QStringList SampleDataGenerator::generateSentences(int count) {
    QStringList sentences;
    for (int i = 0; i < count; ++i) {
        QString sentence = generateLoremIpsum(getRandomInt(5, 15));
        sentence[0] = sentence[0].toUpper();
        sentence += ".";
        sentences.append(sentence);
    }
    return sentences;
}

QStringList SampleDataGenerator::generateParagraphs(int count) {
    QStringList paragraphs;
    for (int i = 0; i < count; ++i) {
        QStringList sentences = generateSentences(getRandomInt(3, 8));
        paragraphs.append(sentences.join(" "));
    }
    return paragraphs;
}

QJsonObject SampleDataGenerator::generateUIConfiguration() {
    QJsonObject config;
    config["type"] = "QWidget";
    config["properties"] = QJsonObject{
        {"windowTitle", "Sample Window"},
        {"width", 800},
        {"height", 600}
    };
    return config;
}

QJsonObject SampleDataGenerator::generateThemeConfiguration() {
    QJsonObject theme;
    theme["name"] = "Sample Theme";
    theme["primary_color"] = "#3498db";
    theme["secondary_color"] = "#2c3e50";
    theme["background_color"] = "#ecf0f1";
    theme["text_color"] = "#2c3e50";
    return theme;
}

QJsonObject SampleDataGenerator::generateComponentConfiguration(const QString& component_type) {
    QJsonObject config;
    config["type"] = component_type;
    config["properties"] = QJsonObject{
        {"enabled", true},
        {"visible", true}
    };
    
    if (component_type == "Button") {
        config["properties"].toObject()["text"] = "Sample Button";
    } else if (component_type == "LineEdit") {
        config["properties"].toObject()["placeholder"] = "Enter text...";
    }
    
    return config;
}

void SampleDataGenerator::initializeData() {
    first_names_ = {"John", "Jane", "Michael", "Sarah", "David", "Emma", "Chris", "Lisa", 
                   "Robert", "Maria", "James", "Anna", "William", "Jessica", "Richard", "Ashley"};
    
    last_names_ = {"Smith", "Johnson", "Williams", "Brown", "Jones", "Garcia", "Miller", "Davis",
                  "Rodriguez", "Martinez", "Hernandez", "Lopez", "Gonzalez", "Wilson", "Anderson", "Thomas"};
    
    company_suffixes_ = {"Inc", "LLC", "Corp", "Ltd", "Co"};
    
    lorem_words_ = {"lorem", "ipsum", "dolor", "sit", "amet", "consectetur", "adipiscing", "elit",
                   "sed", "do", "eiusmod", "tempor", "incididunt", "ut", "labore", "et", "dolore",
                   "magna", "aliqua", "enim", "ad", "minim", "veniam", "quis", "nostrud"};
    
    data_initialized_ = true;
}

QString SampleDataGenerator::getRandomItem(const QStringList& list) {
    if (list.isEmpty()) return QString();
    return list[getRandomInt(0, list.size() - 1)];
}

int SampleDataGenerator::getRandomInt(int min, int max) {
    return QRandomGenerator::global()->bounded(min, max + 1);
}

double SampleDataGenerator::getRandomDouble(double min, double max) {
    return min + (max - min) * QRandomGenerator::global()->generateDouble();
}
