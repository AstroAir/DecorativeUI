#pragma once

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <QVariant>
#include <QWidget>

#include <functional>
#include <memory>

#include "../Binding/StateManager.hpp"

namespace DeclarativeUI::JSON {

// **JSON-based UI loading system**
class JSONUILoader : public QObject {
    Q_OBJECT

public:
    explicit JSONUILoader(QObject *parent = nullptr);
    ~JSONUILoader() override = default;

    // **Load UI from JSON file**
    [[nodiscard]] std::unique_ptr<QWidget> loadFromFile(
        const QString &file_path);

    // **Load UI from JSON string**
    [[nodiscard]] std::unique_ptr<QWidget> loadFromString(
        const QString &json_string);

    // **Load UI from JSON object**
    [[nodiscard]] std::unique_ptr<QWidget> loadFromObject(
        const QJsonObject &json_object);

    // **Validation**
    [[nodiscard]] bool validateJSON(const QJsonObject &json_object) const;

    // **State binding**
    void bindStateManager(std::shared_ptr<Binding::StateManager> state_manager);

    // **Event handler registration**
    void registerEventHandler(const QString &handler_name,
                              std::function<void()> handler);

    // **Custom property converter**
    void registerPropertyConverter(
        const QString &property_type,
        std::function<QVariant(const QJsonValue &)> converter);

signals:
    void loadingStarted(const QString &source);
    void loadingFinished(const QString &source);
    void loadingFailed(const QString &source, const QString &error);

private:
    std::shared_ptr<Binding::StateManager> state_manager_;
    std::unordered_map<QString, std::function<void()>> event_handlers_;
    std::unordered_map<QString, std::function<QVariant(const QJsonValue &)>>
        property_converters_;

    // **Recursive widget creation**
    std::unique_ptr<QWidget> createWidgetFromObject(
        const QJsonObject &widget_object);

    // **Property application**
    void applyProperties(QWidget *widget, const QJsonObject &properties);

    // **Event binding**
    void bindEvents(QWidget *widget, const QJsonObject &events);

    // **Layout handling**
    void setupLayout(QWidget *parent, const QJsonObject &layout_config);

    // **Children handling**
    void addChildren(QWidget *parent, const QJsonArray &children);

    // **Property binding**
    void setupPropertyBindings(QWidget *widget, const QJsonObject &bindings);

    // **Value conversion utilities**
    QVariant convertJSONValue(const QJsonValue &value,
                              const QString &property_type = "");

    // **Validation helpers**
    bool validateWidgetObject(const QJsonObject &widget_object) const;
    bool validatePropertyValue(const QString &property_name,
                               const QJsonValue &value) const;
};

}  // namespace DeclarativeUI::JSON
