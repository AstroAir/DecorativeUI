/**
 * @file ExampleHelpers.cpp
 * @brief Implementation of example helper functions
 */

#include "ExampleHelpers.hpp"
#include <QApplication>
#include <QScreen>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QRandomGenerator>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QRegularExpression>
#include <QUrl>
#include <QDebug>

// Static member initialization
QString ExampleHelpers::sample_names_cache_;
QList<QPointF> ExampleHelpers::sample_chart_data_cache_;
bool ExampleHelpers::caches_initialized_ = false;

ExampleHelpers::ExampleHelpers() {
    if (!caches_initialized_) {
        initializeCaches();
    }
}

QString ExampleHelpers::formatFileSize(qint64 bytes) {
    const QStringList units = {"B", "KB", "MB", "GB", "TB"};
    int unit_index = 0;
    double size = bytes;
    
    while (size >= 1024.0 && unit_index < units.size() - 1) {
        size /= 1024.0;
        unit_index++;
    }
    
    return QString("%1 %2").arg(QString::number(size, 'f', 2)).arg(units[unit_index]);
}

QString ExampleHelpers::formatDuration(qint64 milliseconds) {
    qint64 seconds = milliseconds / 1000;
    qint64 minutes = seconds / 60;
    qint64 hours = minutes / 60;
    
    seconds %= 60;
    minutes %= 60;
    
    if (hours > 0) {
        return QString("%1:%2:%3")
               .arg(hours, 2, 10, QChar('0'))
               .arg(minutes, 2, 10, QChar('0'))
               .arg(seconds, 2, 10, QChar('0'));
    } else if (minutes > 0) {
        return QString("%1:%2")
               .arg(minutes, 2, 10, QChar('0'))
               .arg(seconds, 2, 10, QChar('0'));
    } else {
        return QString("%1.%2s")
               .arg(seconds)
               .arg(milliseconds % 1000, 3, 10, QChar('0'));
    }
}

QString ExampleHelpers::formatMemoryUsage(qint64 bytes) {
    return formatFileSize(bytes);
}

QString ExampleHelpers::capitalizeFirst(const QString& text) {
    if (text.isEmpty()) return text;
    return text.left(1).toUpper() + text.mid(1).toLower();
}

QColor ExampleHelpers::interpolateColor(const QColor& from, const QColor& to, qreal progress) {
    progress = qBound(0.0, progress, 1.0);
    
    int r = from.red() + (to.red() - from.red()) * progress;
    int g = from.green() + (to.green() - from.green()) * progress;
    int b = from.blue() + (to.blue() - from.blue()) * progress;
    int a = from.alpha() + (to.alpha() - from.alpha()) * progress;
    
    return QColor(r, g, b, a);
}

QColor ExampleHelpers::generateRandomColor() {
    auto generator = QRandomGenerator::global();
    return QColor(
        generator->bounded(256),
        generator->bounded(256),
        generator->bounded(256)
    );
}

QStringList ExampleHelpers::getColorPalette(const QString& theme) {
    if (theme == "dark") {
        return {"#2c3e50", "#34495e", "#7f8c8d", "#95a5a6", "#bdc3c7", "#ecf0f1"};
    } else if (theme == "vibrant") {
        return {"#e74c3c", "#e67e22", "#f39c12", "#27ae60", "#2980b9", "#9b59b6"};
    } else { // default/light
        return {"#3498db", "#2ecc71", "#f39c12", "#e74c3c", "#9b59b6", "#1abc9c"};
    }
}

void ExampleHelpers::centerWidget(QWidget* widget, QWidget* parent) {
    if (!widget) return;
    
    QRect parent_rect;
    if (parent) {
        parent_rect = parent->geometry();
    } else {
        parent_rect = QApplication::primaryScreen()->geometry();
    }
    
    QRect widget_rect = widget->geometry();
    widget_rect.moveCenter(parent_rect.center());
    widget->setGeometry(widget_rect);
}

void ExampleHelpers::fadeInWidget(QWidget* widget, int duration_ms) {
    if (!widget) return;
    
    auto effect = new QGraphicsOpacityEffect();
    widget->setGraphicsEffect(effect);
    
    auto animation = new QPropertyAnimation(effect, "opacity");
    animation->setDuration(duration_ms);
    animation->setStartValue(0.0);
    animation->setEndValue(1.0);
    animation->setEasingCurve(QEasingCurve::OutCubic);
    
    // Clean up effect after animation
    QObject::connect(animation, &QPropertyAnimation::finished, [widget, effect]() {
        widget->setGraphicsEffect(nullptr);
        effect->deleteLater();
    });
    
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void ExampleHelpers::fadeOutWidget(QWidget* widget, int duration_ms) {
    if (!widget) return;
    
    auto effect = new QGraphicsOpacityEffect();
    widget->setGraphicsEffect(effect);
    
    auto animation = new QPropertyAnimation(effect, "opacity");
    animation->setDuration(duration_ms);
    animation->setStartValue(1.0);
    animation->setEndValue(0.0);
    animation->setEasingCurve(QEasingCurve::OutCubic);
    
    // Hide widget after animation
    QObject::connect(animation, &QPropertyAnimation::finished, [widget, effect]() {
        widget->hide();
        widget->setGraphicsEffect(nullptr);
        effect->deleteLater();
    });
    
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void ExampleHelpers::slideInWidget(QWidget* widget, const QString& direction, int duration_ms) {
    if (!widget) return;
    
    QRect final_geometry = widget->geometry();
    QRect start_geometry = final_geometry;
    
    if (direction == "left") {
        start_geometry.moveLeft(-widget->width());
    } else if (direction == "right") {
        start_geometry.moveLeft(widget->parentWidget()->width());
    } else if (direction == "top") {
        start_geometry.moveTop(-widget->height());
    } else if (direction == "bottom") {
        start_geometry.moveTop(widget->parentWidget()->height());
    }
    
    widget->setGeometry(start_geometry);
    widget->show();
    
    auto animation = new QPropertyAnimation(widget, "geometry");
    animation->setDuration(duration_ms);
    animation->setStartValue(start_geometry);
    animation->setEndValue(final_geometry);
    animation->setEasingCurve(QEasingCurve::OutCubic);
    
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

QStringList ExampleHelpers::generateSampleNames(int count) {
    if (!sample_names_cache_.isEmpty()) {
        QStringList names = sample_names_cache_.split('\n');
        if (names.size() >= count) {
            return names.mid(0, count);
        }
    }
    
    QStringList first_names = {"John", "Jane", "Michael", "Sarah", "David", "Emma", "Chris", "Lisa"};
    QStringList last_names = {"Smith", "Johnson", "Williams", "Brown", "Jones", "Garcia", "Miller", "Davis"};
    
    QStringList result;
    auto generator = QRandomGenerator::global();
    
    for (int i = 0; i < count; ++i) {
        QString first = first_names[generator->bounded(first_names.size())];
        QString last = last_names[generator->bounded(last_names.size())];
        result.append(QString("%1 %2").arg(first, last));
    }
    
    return result;
}

QList<QPointF> ExampleHelpers::generateSampleChartData(int points) {
    if (!sample_chart_data_cache_.isEmpty() && sample_chart_data_cache_.size() >= points) {
        return sample_chart_data_cache_.mid(0, points);
    }
    
    QList<QPointF> data;
    auto generator = QRandomGenerator::global();
    
    for (int i = 0; i < points; ++i) {
        double x = i;
        double y = 50 + 30 * qSin(i * 0.1) + generator->bounded(20) - 10;
        data.append(QPointF(x, y));
    }
    
    return data;
}

QList<QStringList> ExampleHelpers::generateSampleTableData(int rows, int columns) {
    QList<QStringList> data;
    auto generator = QRandomGenerator::global();
    
    for (int row = 0; row < rows; ++row) {
        QStringList row_data;
        for (int col = 0; col < columns; ++col) {
            if (col == 0) {
                // First column: names
                row_data.append(QString("Item %1").arg(row + 1));
            } else if (col == 1) {
                // Second column: random numbers
                row_data.append(QString::number(generator->bounded(1000)));
            } else {
                // Other columns: random text
                row_data.append(QString("Data %1-%2").arg(row + 1).arg(col + 1));
            }
        }
        data.append(row_data);
    }
    
    return data;
}

bool ExampleHelpers::saveJsonToFile(const QJsonObject& json, const QString& filename) {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Cannot open file for writing:" << filename;
        return false;
    }
    
    QJsonDocument doc(json);
    file.write(doc.toJson());
    return true;
}

QJsonObject ExampleHelpers::loadJsonFromFile(const QString& filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot open file for reading:" << filename;
        return QJsonObject();
    }
    
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);
    
    if (error.error != QJsonParseError::NoError) {
        qWarning() << "JSON parse error:" << error.errorString();
        return QJsonObject();
    }
    
    return doc.object();
}

QString ExampleHelpers::getResourcePath(const QString& resource_name) {
    QStringList search_paths = {
        QApplication::applicationDirPath() + "/resources",
        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/resources",
        ":/resources"
    };
    
    for (const QString& path : search_paths) {
        QString full_path = path + "/" + resource_name;
        if (QFile::exists(full_path)) {
            return full_path;
        }
    }
    
    return QString();
}

QStringList ExampleHelpers::getAvailableThemes() {
    return {"Light", "Dark", "Blue", "Green", "Purple", "Custom"};
}

qint64 ExampleHelpers::getCurrentMemoryUsage() {
    // Simplified memory usage (in a real implementation, this would query actual system memory)
    return QRandomGenerator::global()->bounded(100, 500) * 1024 * 1024; // 100-500 MB
}

double ExampleHelpers::getCurrentCpuUsage() {
    // Simplified CPU usage (in a real implementation, this would query actual CPU usage)
    return QRandomGenerator::global()->bounded(0, 100);
}

int ExampleHelpers::getFrameRate() {
    // Simplified frame rate (in a real implementation, this would measure actual frame rate)
    return QRandomGenerator::global()->bounded(50, 61);
}

bool ExampleHelpers::isValidEmail(const QString& email) {
    QRegularExpression regex(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    return regex.match(email).hasMatch();
}

bool ExampleHelpers::isValidUrl(const QString& url) {
    QUrl qurl(url);
    return qurl.isValid() && !qurl.scheme().isEmpty();
}

bool ExampleHelpers::isValidColor(const QString& color) {
    QColor qcolor(color);
    return qcolor.isValid();
}

void ExampleHelpers::logComponentCreation(const QString& component_name) {
    qDebug() << "Component created:" << component_name;
}

void ExampleHelpers::logPerformanceMetric(const QString& metric_name, qint64 value) {
    qDebug() << "Performance metric:" << metric_name << "=" << value;
}

void ExampleHelpers::logUserAction(const QString& action, const QJsonObject& data) {
    qDebug() << "User action:" << action << "Data:" << QJsonDocument(data).toJson(QJsonDocument::Compact);
}

void ExampleHelpers::initializeCaches() {
    // Initialize sample data caches
    sample_names_cache_ = generateSampleNames(1000).join('\n');
    sample_chart_data_cache_ = generateSampleChartData(1000);
    caches_initialized_ = true;
}
