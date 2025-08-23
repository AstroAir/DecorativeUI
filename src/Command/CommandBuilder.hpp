#pragma once

#include <QJsonObject>
#include <QLayout>
#include <QObject>
#include <QString>
#include <QVariant>
#include <QWidget>
#include <functional>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "MVCIntegration.hpp"
#include "UICommand.hpp"
#include "UICommandFactory.hpp"
#include "WidgetMapper.hpp"

namespace DeclarativeUI::Command::UI {

// **Forward declarations**
class CommandBuilder;
class CommandUIElement;
class CommandHierarchyBuilder;
class CommandLayoutBuilder;

// **Builder configuration options**
struct BuilderConfig {
    bool auto_initialize = true;
    bool auto_mvc_integration = true;
    bool auto_widget_creation = true;
    bool validate_hierarchy = true;
    QString default_layout = "VBox";

    BuilderConfig() = default;
};

// **Command Builder - fluent interface for creating individual commands**
class CommandBuilder {
public:
    explicit CommandBuilder(const QString& commandType);
    CommandBuilder(const CommandBuilder& other) = delete;
    CommandBuilder& operator=(const CommandBuilder& other) = delete;
    CommandBuilder(CommandBuilder&& other) = default;
    CommandBuilder& operator=(CommandBuilder&& other) = default;

    // **Property configuration**
    CommandBuilder& property(const QString& name, const QVariant& value);
    CommandBuilder& properties(const QJsonObject& props);

    template <typename T>
    CommandBuilder& property(const QString& name, T&& value);

    // **Common property shortcuts**
    CommandBuilder& text(const QString& text);
    CommandBuilder& enabled(bool enabled);
    CommandBuilder& visible(bool visible);
    CommandBuilder& tooltip(const QString& tooltip);
    CommandBuilder& icon(const QString& iconPath);
    CommandBuilder& size(int width, int height);
    CommandBuilder& position(int x, int y);

    // **Layout properties**
    CommandBuilder& layout(const QString& layoutType);
    CommandBuilder& spacing(int spacing);

    // **Text input specific methods**
    CommandBuilder& placeholder(const QString& placeholder);
    CommandBuilder& maxLength(int length);

    // **Event handling**
    CommandBuilder& onClick(std::function<void()> handler);
    CommandBuilder& onValueChanged(
        std::function<void(const QVariant&)> handler);
    CommandBuilder& onTextChanged(std::function<void(const QString&)> handler);
    CommandBuilder& onEvent(const QString& eventType,
                            std::function<void(const QVariant&)> handler);

    // **State binding**
    CommandBuilder& bindToState(const QString& stateKey,
                                const QString& property = "value");
    CommandBuilder& bindProperty(const QString& property,
                                 const QString& stateKey);

    // **Action registration**
    CommandBuilder& registerAsAction(const QString& actionName);
    CommandBuilder& connectToAction(const QString& actionName,
                                    const QString& eventType = "clicked");

    // **Validation**
    CommandBuilder& validator(const QString& property,
                              std::function<bool(const QVariant&)> validator);
    CommandBuilder& validator(std::function<bool(const QVariant&)>
                                  validator);  // Default property validator
    CommandBuilder& required(const QString& property);
    CommandBuilder& range(const QString& property, const QVariant& min,
                          const QVariant& max);

    // **Styling and theming**
    CommandBuilder& styleClass(const QString& className);
    CommandBuilder& style(const QString& property, const QVariant& value);
    CommandBuilder& style(const QString& styleSheet);  // CSS-like style string
    CommandBuilder& theme(const QString& themeName);

    // **Child management**
    CommandBuilder& child(const CommandBuilder& childBuilder);
    CommandBuilder& child(std::shared_ptr<BaseUICommand> child);
    CommandBuilder& children(const std::vector<CommandBuilder>& childBuilders);

    // **Configuration**
    CommandBuilder& config(const BuilderConfig& config);
    CommandBuilder& autoInitialize(bool enable);
    CommandBuilder& autoMVCIntegration(bool enable);

    // **Build methods**
    std::shared_ptr<BaseUICommand> build();
    std::pair<std::shared_ptr<BaseUICommand>, std::unique_ptr<QWidget>>
    buildWithWidget();
    std::unique_ptr<CommandUIElement> buildAsUIElement();

    // **Validation and debugging**
    bool validate() const;
    QStringList getValidationErrors() const;
    QString toString() const;

private:
    QString command_type_;
    QJsonObject properties_;
    std::unordered_map<QString, std::function<void(const QVariant&)>>
        event_handlers_;
    std::unordered_map<QString, QString> state_bindings_;
    std::unordered_map<QString, std::function<bool(const QVariant&)>>
        validators_;
    std::vector<std::shared_ptr<BaseUICommand>> children_;
    std::vector<QString> action_registrations_;
    BuilderConfig config_;

    void applyConfiguration(std::shared_ptr<BaseUICommand> command);
    void setupEventHandlers(std::shared_ptr<BaseUICommand> command);
    void setupStateBindings(std::shared_ptr<BaseUICommand> command);
    void setupValidators(std::shared_ptr<BaseUICommand> command);
    void setupActionRegistrations(std::shared_ptr<BaseUICommand> command);
    void addChildren(std::shared_ptr<BaseUICommand> command);
};

// **Command Hierarchy Builder - for building complex UI structures**
class CommandHierarchyBuilder {
public:
    explicit CommandHierarchyBuilder(const QString& rootCommandType);

    // **Root configuration**
    CommandHierarchyBuilder& rootProperty(const QString& name,
                                          const QVariant& value);
    CommandHierarchyBuilder& rootProperties(const QJsonObject& props);

    // **Layout configuration**
    CommandHierarchyBuilder& layout(const QString& layoutType);
    CommandHierarchyBuilder& spacing(int spacing);
    CommandHierarchyBuilder& margins(int left, int top, int right, int bottom);
    CommandHierarchyBuilder& margins(int margin);

    // **Styling**
    CommandHierarchyBuilder& style(const QString& styleSheet);

    // **Child addition with fluent interface**
    CommandHierarchyBuilder& addChild(const CommandBuilder& childBuilder);
    CommandHierarchyBuilder& addChild(
        const QString& commandType,
        std::function<void(CommandBuilder&)> configurator);
    CommandHierarchyBuilder& addChild(std::shared_ptr<BaseUICommand> command);
    CommandHierarchyBuilder& addChildren(
        const std::vector<CommandBuilder>& childBuilders);

    // **Conditional child addition**
    CommandHierarchyBuilder& addChildIf(bool condition,
                                        const CommandBuilder& childBuilder);
    CommandHierarchyBuilder& addChildIf(
        bool condition, const QString& commandType,
        std::function<void(CommandBuilder&)> configurator);

    // **Nested hierarchy**
    CommandHierarchyBuilder& addContainer(
        const QString& containerType,
        std::function<void(CommandHierarchyBuilder&)> configurator);

    // **Batch operations**
    CommandHierarchyBuilder& addRepeated(
        int count, const QString& commandType,
        std::function<void(CommandBuilder&, int)> configurator);

    // **Build methods**
    std::shared_ptr<BaseUICommand> build();
    std::pair<std::shared_ptr<BaseUICommand>, std::unique_ptr<QWidget>>
    buildWithWidget();
    std::unique_ptr<CommandUIElement> buildAsUIElement();

    // **Serialization**
    QJsonObject toJson() const;
    static CommandHierarchyBuilder fromJson(const QJsonObject& json);

private:
    CommandBuilder root_builder_;
    std::vector<CommandBuilder> child_builders_;
    std::vector<std::shared_ptr<BaseUICommand>> prebuilt_commands_;
    QString layout_type_ = "VBox";
    int spacing_ = 6;
    int margin_left_ = 9, margin_top_ = 9, margin_right_ = 9,
        margin_bottom_ = 9;

    void setupRootAsContainer();
};

// **Command Layout Builder - specialized for layout management**
class CommandLayoutBuilder {
public:
    explicit CommandLayoutBuilder(const QString& layoutType = "VBox");

    // **Layout configuration**
    CommandLayoutBuilder& spacing(int spacing);
    CommandLayoutBuilder& margins(int left, int top, int right, int bottom);
    CommandLayoutBuilder& margins(int margin);
    CommandLayoutBuilder& alignment(int alignment);

    // **VBox/HBox specific**
    CommandLayoutBuilder& addStretch(int stretch = 0);
    CommandLayoutBuilder& addSpacing(int spacing);

    // **Grid layout specific**
    CommandLayoutBuilder& addToGrid(const CommandBuilder& childBuilder, int row,
                                    int column, int rowSpan = 1,
                                    int columnSpan = 1);
    CommandLayoutBuilder& setRowStretch(int row, int stretch);
    CommandLayoutBuilder& setColumnStretch(int column, int stretch);

    // **Form layout specific**
    CommandLayoutBuilder& addRow(const QString& label,
                                 const CommandBuilder& fieldBuilder);
    CommandLayoutBuilder& addRow(const CommandBuilder& labelBuilder,
                                 const CommandBuilder& fieldBuilder);

    // **General child addition**
    CommandLayoutBuilder& add(const CommandBuilder& childBuilder);
    CommandLayoutBuilder& add(
        const QString& commandType,
        std::function<void(CommandBuilder&)> configurator);

    // **Build methods**
    std::shared_ptr<BaseUICommand> build();
    std::pair<std::shared_ptr<BaseUICommand>, std::unique_ptr<QWidget>>
    buildWithWidget();

private:
    QString layout_type_;
    CommandHierarchyBuilder hierarchy_builder_;

    struct GridItem {
        CommandBuilder builder;
        int row, column, rowSpan, columnSpan;

        GridItem(CommandBuilder&& b, int r = 0, int c = 0, int rs = 1,
                 int cs = 1)
            : builder(std::move(b)),
              row(r),
              column(c),
              rowSpan(rs),
              columnSpan(cs) {}
    };
    std::vector<GridItem> grid_items_;

    struct FormRow {
        CommandBuilder label_builder;
        CommandBuilder field_builder;
        bool has_label_builder = false;
        QString label_text;

        FormRow(CommandBuilder&& lb, CommandBuilder&& fb)
            : label_builder(std::move(lb)),
              field_builder(std::move(fb)),
              has_label_builder(true) {}
        FormRow(const QString& text, CommandBuilder&& fb)
            : label_builder("Label"),
              field_builder(std::move(fb)),
              has_label_builder(false),
              label_text(text) {}
    };
    std::vector<FormRow> form_rows_;
};

// **Convenience functions for common UI patterns**
namespace Patterns {

// **Simple button**
inline CommandBuilder button(const QString& text) {
    return std::move(CommandBuilder("Button").text(text));
}

// **Input field with label**
inline CommandHierarchyBuilder labeledInput(const QString& labelText,
                                            const QString& placeholder = "") {
    return std::move(
        CommandHierarchyBuilder("Container")
            .layout("HBox")
            .addChild("Label",
                      [labelText](CommandBuilder& b) { b.text(labelText); })
            .addChild("TextInput", [placeholder](CommandBuilder& b) {
                b.property("placeholder", placeholder);
            }));
}

// **Form with submit button**
inline CommandHierarchyBuilder form(
    const std::vector<std::pair<QString, QString>>& fields) {
    CommandHierarchyBuilder form("Container");
    form.layout("VBox");

    for (const auto& [label, placeholder] : fields) {
        form.addChild("Container",
                      [label, placeholder](CommandBuilder& container) {
                          container.property("layout", QString("HBox"));
                          // Add label and input as children
                      });
    }

    form.addChild("Button", [](CommandBuilder& b) { b.text("Submit"); });
    return form;
}

// **Toolbar**
inline CommandLayoutBuilder toolbar() {
    return std::move(CommandLayoutBuilder("HBox").spacing(2).margins(4));
}

// **Status bar**
inline CommandLayoutBuilder statusBar() {
    return std::move(CommandLayoutBuilder("HBox").spacing(10).margins(5));
}

}  // namespace Patterns

// **Template implementations**
template <typename T>
CommandBuilder& CommandBuilder::property(const QString& name, T&& value) {
    if constexpr (std::is_same_v<std::decay_t<T>, const char*> ||
                  std::is_same_v<std::decay_t<T>, char*>) {
        properties_[name] = QJsonValue::fromVariant(QVariant(QString(value)));
    } else if constexpr (std::is_same_v<std::decay_t<T>, QVariant>) {
        properties_[name] = QJsonValue::fromVariant(value);
    } else {
        properties_[name] = QJsonValue::fromVariant(
            QVariant::fromValue(std::forward<T>(value)));
    }
    return *this;
}

}  // namespace DeclarativeUI::Command::UI
