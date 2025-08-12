#pragma once

#include <QApplication>
#include <QColorDialog>
#include <QMainWindow>
#include <memory>

#include "Command/CommandBuilder.hpp"
#include "Command/CommandEvents.hpp"
#include "Command/UICommand.hpp"
#include "Command/UICommandFactory.hpp"

namespace DeclarativeUI::Command::UI::Examples {

/**
 * @brief Comprehensive example demonstrating the Command-based UI architecture
 *
 * This example shows how to build a complete UI application using the Command
 * system with proper MVC separation, state management, and event handling.
 */
class CommandUIExample : public QMainWindow {
    Q_OBJECT

public:
    explicit CommandUIExample(QWidget* parent = nullptr);
    virtual ~CommandUIExample() = default;

    // **Example scenarios**
    void createSimpleButtonExample();
    void createFormExample();
    void createDataBindingExample();
    void createHierarchicalExample();
    void createEventHandlingExample();
    void createCustomCommandExample();

private slots:
    void onButtonClicked();
    void onFormSubmitted();
    void onDataChanged(const QVariant& value);

private:
    void setupUI();
    void setupStateManagement();
    void setupEventHandling();
    void createMenuBar();
    void createStatusBar();

    // **Command instances**
    std::shared_ptr<BaseUICommand> main_container_;
    std::shared_ptr<BaseUICommand> button_example_;
    std::shared_ptr<BaseUICommand> form_example_;
    std::shared_ptr<BaseUICommand> data_binding_example_;

    // **State keys**
    static constexpr const char* COUNTER_STATE = "example.counter";
    static constexpr const char* FORM_DATA_STATE = "example.form_data";
    static constexpr const char* STATUS_STATE = "example.status";
};

/**
 * @brief Simple button example demonstrating basic Command usage
 */
class SimpleButtonExample {
public:
    static std::shared_ptr<BaseUICommand> create() {
        using namespace DeclarativeUI::Command::UI;

        // Create button using CommandBuilder
        return CommandBuilder("Button")
            .text("Click Me!")
            .tooltip("This is a command-based button")
            .onClick(
                []() { qDebug() << "Button clicked through Command system!"; })
            .registerAsAction("example.button_click")
            .build();
    }
};

/**
 * @brief Form example demonstrating complex UI composition
 */
class FormExample {
public:
    static std::shared_ptr<BaseUICommand> create() {
        using namespace DeclarativeUI::Command::UI;

        // Create form using CommandHierarchyBuilder
        return CommandHierarchyBuilder("Container")
            .layout("VBox")
            .spacing(10)
            .margins(20)

            // Title
            .addChild("Label",
                      [](CommandBuilder& label) {
                          label.text("User Registration Form")
                              .style("font-weight", "bold")
                              .style("font-size", "16px");
                      })

            // Name field
            .addContainer(
                "Container",
                [](CommandHierarchyBuilder& row) {
                    row.layout("HBox")
                        .addChild("Label",
                                  [](CommandBuilder& label) {
                                      label.text("Name:").size(80, -1);
                                  })
                        .addChild("TextInput", [](CommandBuilder& input) {
                            input.placeholder("Enter your name")
                                .bindToState("form.name")
                                .required("text")
                                .onTextChanged([](const QString& text) {
                                    qDebug() << "Name changed to:" << text;
                                });
                        });
                })

            // Email field
            .addContainer(
                "Container",
                [](CommandHierarchyBuilder& row) {
                    row.layout("HBox")
                        .addChild("Label",
                                  [](CommandBuilder& label) {
                                      label.text("Email:").size(80, -1);
                                  })
                        .addChild("TextInput", [](CommandBuilder& input) {
                            input.placeholder("Enter your email")
                                .bindToState("form.email")
                                .validator("text", [](const QVariant& value) {
                                    QString email = value.toString();
                                    return email.contains("@") &&
                                           email.contains(".");
                                });
                        });
                })

            // Age field
            .addContainer("Container",
                          [](CommandHierarchyBuilder& row) {
                              row.layout("HBox")
                                  .addChild("Label",
                                            [](CommandBuilder& label) {
                                                label.text("Age:").size(80, -1);
                                            })
                                  .addChild("SpinBox",
                                            [](CommandBuilder& spinbox) {
                                                spinbox.property("minimum", 18)
                                                    .property("maximum", 120)
                                                    .property("value", 25)
                                                    .bindToState("form.age");
                                            });
                          })

            // Buttons
            .addContainer(
                "Container",
                [](CommandHierarchyBuilder& buttons) {
                    buttons.layout("HBox")
                        .spacing(10)
                        .addChild("Button",
                                  [](CommandBuilder& submit) {
                                      submit.text("Submit")
                                          .registerAsAction("form.submit")
                                          .onClick([]() {
                                              qDebug() << "Form submitted!";
                                          });
                                  })
                        .addChild("Button", [](CommandBuilder& cancel) {
                            cancel.text("Cancel").onClick(
                                []() { qDebug() << "Form cancelled!"; });
                        });
                })

            .build();
    }
};

/**
 * @brief Data binding example demonstrating state synchronization
 */
class DataBindingExample {
public:
    static std::shared_ptr<BaseUICommand> create() {
        using namespace DeclarativeUI::Command::UI;

        return CommandHierarchyBuilder("Container")
            .layout("VBox")
            .spacing(15)
            .margins(20)

            // Title
            .addChild("Label",
                      [](CommandBuilder& label) {
                          label.text("Data Binding Example")
                              .style("font-weight", "bold");
                      })

            // Counter display
            .addChild("Label",
                      [](CommandBuilder& label) {
                          label.text("Counter: 0")
                              .bindToState("counter.value", "text")
                              .style("font-size", "18px");
                      })

            // Counter controls
            .addContainer(
                "Container",
                [](CommandHierarchyBuilder& controls) {
                    controls.layout("HBox")
                        .spacing(10)
                        .addChild(
                            "Button",
                            [](CommandBuilder& decrement) {
                                decrement.text("-").onClick([]() {
                                    // Decrement counter in state
                                    auto& state = DeclarativeUI::Binding::
                                        StateManager::instance();
                                    // Implementation would decrement the
                                    // counter
                                    auto counterState =
                                        state.getState<int>("counter.value");
                                    if (counterState) {
                                        int newValue = counterState->get() - 1;
                                        state.setState("counter.value",
                                                       newValue);
                                    }
                                    qDebug() << "Decrement counter";
                                });
                            })
                        .addChild("Slider",
                                  [](CommandBuilder& slider) {
                                      slider.property("minimum", 0)
                                          .property("maximum", 100)
                                          .property("value", 0)
                                          .bindToState("counter.value",
                                                       "value");
                                  })
                        .addChild("Button", [](CommandBuilder& increment) {
                            increment.text("+").onClick([]() {
                                // Increment counter in state
                                auto& state = DeclarativeUI::Binding::
                                    StateManager::instance();
                                // Implementation would increment the counter
                                auto counterState =
                                    state.getState<int>("counter.value");
                                if (counterState) {
                                    int newValue = counterState->get() + 1;
                                    state.setState("counter.value", newValue);
                                }
                                qDebug() << "Increment counter";
                            });
                        });
                })

            // Progress bar bound to same state
            .addChild("ProgressBar",
                      [](CommandBuilder& progress) {
                          progress.property("minimum", 0)
                              .property("maximum", 100)
                              .property("value", 0)
                              .bindToState("counter.value", "value");
                      })

            .build();
    }
};

/**
 * @brief Event handling example demonstrating the Command event system
 */
class EventHandlingExample {
public:
    static std::shared_ptr<BaseUICommand> create() {
        using namespace DeclarativeUI::Command::UI;
        using namespace DeclarativeUI::Command::UI::Events;

        auto container =
            CommandHierarchyBuilder("Container")
                .layout("VBox")
                .spacing(10)
                .margins(20)

                .addChild("Label",
                          [](CommandBuilder& label) {
                              label.text("Event Handling Example")
                                  .style("font-weight", "bold");
                          })

                .addChild("Button",
                          [](CommandBuilder& button) {
                              button.text("Click for Events")
                                  .tooltip("Demonstrates various event types");
                          })

                .addChild("TextInput",
                          [](CommandBuilder& input) {
                              input.placeholder("Type here to see text events")
                                  .onTextChanged([](const QString& text) {
                                      qDebug() << "Text changed event:" << text;
                                  });
                          })

                .addChild(
                    "CheckBox",
                    [](CommandBuilder& checkbox) {
                        checkbox.text("Check me for state events")
                            .onEvent("stateChanged", [](const QVariant& state) {
                                qDebug() << "Checkbox state changed:"
                                         << state.toInt();
                            });
                    })

                .build();

        // Set up advanced event handling
        auto button = container->getChildren()[1];  // Get the button

        // Register multiple event handlers with different priorities
        onClick(button.get(),
                []() { qDebug() << "High priority click handler"; });

        CommandEventDispatcher::instance().registerHandler(
            button.get(), CommandEventType::Clicked,
            [](const CommandEvent& event) {
                qDebug() << "Low priority click handler, event ID:"
                         << event.getId().toString();
            },
            CommandEventPriority::Low);

        // Register a one-time handler
        CommandEventDispatcher::instance().registerOnceHandler(
            button.get(), CommandEventType::Clicked,
            [](const CommandEvent& event) {
                qDebug() << "This handler will only execute once!";
            });

        return container;
    }
};

/**
 * @brief Custom command example demonstrating extensibility
 */
class CustomColorPickerCommand : public BaseUICommand {
    Q_OBJECT

public:
    explicit CustomColorPickerCommand(QObject* parent = nullptr)
        : BaseUICommand(parent) {
        getState()->setProperty("color", QColor(Qt::white));
        getState()->setProperty("text", QString("Pick Color"));
    }

    UICommandMetadata getMetadata() const override {
        UICommandMetadata metadata("ColorPicker", "QPushButton", "Color Picker",
                                   "A custom color picker command");
        metadata.supported_events = {"clicked", "colorChanged"};
        metadata.default_properties = QJsonObject{
            {"color", QColor(Qt::white).name()}, {"text", "Pick Color"}};
        return metadata;
    }

    QString getCommandType() const override { return "ColorPicker"; }
    QString getWidgetType() const override { return "QPushButton"; }

    // Custom methods
    void setColor(const QColor& color) {
        QColor oldColor = getState()->getProperty<QColor>("color");
        getState()->setProperty("color", color);

        // Emit custom event
        auto event = std::make_unique<CustomEvent>("colorChanged", this);
        event->setData("oldColor", oldColor);
        event->setData("newColor", color);
        Events::dispatch(std::move(event));
    }

    QColor getColor() const {
        return getState()->getProperty<QColor>("color", Qt::white);
    }

    void onWidgetCreated(QWidget* widget) override {
        BaseUICommand::onWidgetCreated(widget);

        if (auto* button = qobject_cast<QPushButton*>(widget)) {
            connect(button, &QPushButton::clicked, this, [this]() {
                // Open color dialog
                QColor color = QColorDialog::getColor(getColor());
                if (color.isValid()) {
                    setColor(color);
                }
            });
        }
    }

    void syncToWidget() override {
        BaseUICommand::syncToWidget();

        if (auto* button = qobject_cast<QPushButton*>(getWidget())) {
            QColor color = getColor();
            button->setStyleSheet(
                QString("background-color: %1").arg(color.name()));
            button->setText(QString("Color: %1").arg(color.name()));
        }
    }
};

}  // namespace DeclarativeUI::Command::UI::Examples
