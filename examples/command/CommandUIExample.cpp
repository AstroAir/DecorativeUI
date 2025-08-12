#include "CommandUIExample.hpp"
#include <QApplication>
#include <QColorDialog>
#include <QDebug>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QStatusBar>
#include <QVBoxLayout>

#include "../../Binding/StateManager.hpp"
#include "../CommandEvents.hpp"
#include "../MVCIntegration.hpp"
#include "../WidgetMapper.hpp"

using namespace DeclarativeUI::Command::UI;
using namespace DeclarativeUI::Command::UI::Examples;

CommandUIExample::CommandUIExample(QWidget* parent) : QMainWindow(parent) {
    qDebug() << "🚀 CommandUIExample starting...";

    setupUI();
    setupStateManagement();
    setupEventHandling();
    createMenuBar();
    createStatusBar();

    qDebug() << "✅ CommandUIExample initialized successfully";
}

void CommandUIExample::setupUI() {
    try {
        // Create main container using CommandHierarchyBuilder
        auto mainContainer =
            CommandHierarchyBuilder("Container")
                .layout("VBox")
                .spacing(20)
                .margins(20)

                // Title
                .addChild(
                    "Label",
                    [](CommandBuilder& label) {
                        label.text("Command-based UI Architecture Examples")
                            .style("font-weight", "bold")
                            .style("font-size", "18px")
                            .style("color", "#2c3e50");
                    })

                // Simple button example
                .addContainer(
                    "Container",
                    [this](CommandHierarchyBuilder& section) {
                        section.layout("VBox")
                            .spacing(10)
                            .addChild("Label",
                                      [](CommandBuilder& label) {
                                          label.text("1. Simple Button Example")
                                              .style("font-weight", "bold")
                                              .style("font-size", "14px");
                                      })
                            .addChild(SimpleButtonExample::create());
                    })

                // Form example
                .addContainer("Container",
                              [this](CommandHierarchyBuilder& section) {
                                  section.layout("VBox")
                                      .spacing(10)
                                      .addChild(
                                          "Label",
                                          [](CommandBuilder& label) {
                                              label.text("2. Form Example")
                                                  .style("font-weight", "bold")
                                                  .style("font-size", "14px");
                                          })
                                      .addChild(FormExample::create());
                              })

                .build();

        main_container_ = mainContainer;

        // Convert to widget and set as central widget
        auto widget =
            WidgetMapper::instance().createWidget(mainContainer.get());
        if (widget) {
            setCentralWidget(widget.release());
            qDebug() << "✅ Main UI created successfully";
        } else {
            qWarning() << "❌ Failed to create widget from command";
        }

    } catch (const std::exception& e) {
        qWarning() << "❌ Error creating UI:" << e.what();
    }
}

void CommandUIExample::setupStateManagement() {
    auto& stateManager = DeclarativeUI::Binding::StateManager::instance();

    // Initialize example states
    stateManager.setState(COUNTER_STATE, 0);
    stateManager.setState(FORM_DATA_STATE, QJsonObject{});
    stateManager.setState(STATUS_STATE, QString("Ready"));

    // Connect to state changes
    connect(&stateManager, &DeclarativeUI::Binding::StateManager::stateChanged,
            this, [this](const QString& key, const QVariant& value) {
                qDebug() << "🔄 State changed:" << key << "=" << value;

                if (key == STATUS_STATE) {
                    statusBar()->showMessage(value.toString());
                }
            });

    qDebug() << "🔧 State management configured";
}

void CommandUIExample::setupEventHandling() {
    // Set up global event handling
    auto& dispatcher = UI::CommandEventDispatcher::instance();

    // Add global event interceptor for click events
    dispatcher.addEventInterceptor(
        UI::CommandEventType::Clicked, [this](const UI::CommandEvent& event) {
            qDebug() << "🖱️ Global click interceptor - Command:"
                     << event.getSource()->getCommandType();
        });

    qDebug() << "⚡ Event handling configured";
}

void CommandUIExample::createMenuBar() {
    auto* fileMenu = menuBar()->addMenu("&File");

    auto* newAction = fileMenu->addAction("&New Example");
    connect(newAction, &QAction::triggered, this,
            &CommandUIExample::createSimpleButtonExample);

    auto* exitAction = fileMenu->addAction("E&xit");
    connect(exitAction, &QAction::triggered, this, &QWidget::close);

    auto* examplesMenu = menuBar()->addMenu("&Examples");

    auto* buttonAction = examplesMenu->addAction("Simple &Button");
    connect(buttonAction, &QAction::triggered, this,
            &CommandUIExample::createSimpleButtonExample);

    auto* formAction = examplesMenu->addAction("&Form Example");
    connect(formAction, &QAction::triggered, this,
            &CommandUIExample::createFormExample);

    auto* dataAction = examplesMenu->addAction("&Data Binding");
    connect(dataAction, &QAction::triggered, this,
            &CommandUIExample::createDataBindingExample);

    auto* hierarchyAction = examplesMenu->addAction("&Hierarchical UI");
    connect(hierarchyAction, &QAction::triggered, this,
            &CommandUIExample::createHierarchicalExample);

    auto* eventsAction = examplesMenu->addAction("&Event Handling");
    connect(eventsAction, &QAction::triggered, this,
            &CommandUIExample::createEventHandlingExample);

    auto* customAction = examplesMenu->addAction("&Custom Command");
    connect(customAction, &QAction::triggered, this,
            &CommandUIExample::createCustomCommandExample);
}

void CommandUIExample::createStatusBar() {
    statusBar()->showMessage("Command UI Example - Ready");
}

void CommandUIExample::createSimpleButtonExample() {
    qDebug() << "🔨 Creating simple button example...";
    button_example_ = SimpleButtonExample::create();
}

void CommandUIExample::createFormExample() {
    qDebug() << "🔨 Creating form example...";
    form_example_ = FormExample::create();
}

void CommandUIExample::createDataBindingExample() {
    qDebug() << "🔨 Creating data binding example...";
    data_binding_example_ = DataBindingExample::create();
}

void CommandUIExample::createHierarchicalExample() {
    qDebug() << "🔨 Creating hierarchical example...";
    // Implementation would create a complex nested UI structure
}

void CommandUIExample::createEventHandlingExample() {
    qDebug() << "🔨 Creating event handling example...";
    auto example = EventHandlingExample::create();
    // Implementation would demonstrate advanced event handling
}

void CommandUIExample::createCustomCommandExample() {
    qDebug() << "🔨 Creating custom command example...";
    auto colorPicker = std::make_shared<CustomColorPickerCommand>();
    // Implementation would demonstrate custom command usage
}

void CommandUIExample::onButtonClicked() {
    auto& stateManager = DeclarativeUI::Binding::StateManager::instance();
    auto counterState = stateManager.getState<int>(COUNTER_STATE);
    if (counterState) {
        int newValue = counterState->get() + 1;
        stateManager.setState(COUNTER_STATE, newValue);
        qDebug() << "🔢 Button clicked, counter:" << newValue;
    }
}

void CommandUIExample::onFormSubmitted() {
    qDebug() << "📝 Form submitted";
    statusBar()->showMessage("Form submitted successfully!", 3000);
}

void CommandUIExample::onDataChanged(const QVariant& value) {
    qDebug() << "📊 Data changed:" << value;
}

#include "CommandUIExample.moc"
