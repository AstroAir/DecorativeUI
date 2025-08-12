/**
 * @file 31_integration_example.cpp
 * @brief Integration between legacy components and Command system
 *
 * This example demonstrates:
 * - Using legacy components alongside Command system
 * - Converting between legacy and Command components
 * - Bidirectional synchronization
 * - Hybrid containers
 * - Migration strategies
 *
 * Learning objectives:
 * - Master integration patterns
 * - Understand component conversion
 * - Learn synchronization techniques
 * - See migration approaches
 */

#include <QApplication>
#include <QDebug>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>
#include <QTextCursor>
#include <QTextEdit>
#include <QTime>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

// Legacy components
#include "Components/Button.hpp"
#include "Components/Widget.hpp"

#ifdef DECLARATIVE_UI_COMMAND_SYSTEM_ENABLED
#include "Binding/StateManager.hpp"
#include "Command/CommandBuilder.hpp"
#include "Command/WidgetMapper.hpp"

#ifdef DECLARATIVE_UI_ADAPTERS_ENABLED
#include "Command/Adapters/ComponentSystemAdapter.hpp"
#include "Command/Adapters/IntegrationManager.hpp"
#include "Command/Adapters/StateManagerAdapter.hpp"
#endif

using namespace DeclarativeUI::Command;
using namespace DeclarativeUI::Components;

class IntegrationExample : public QMainWindow {
    Q_OBJECT

public:
    IntegrationExample(QWidget* parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("Legacy-Command Integration Example");
        setMinimumSize(900, 700);

        setupIntegration();
        setupStateManager();
        setupUI();
    }

private:
    void setupIntegration() {
#ifdef DECLARATIVE_UI_ADAPTERS_ENABLED
        // Initialize integration manager
        auto& manager = Adapters::IntegrationManager::instance();
        manager.initialize();
        manager.setCompatibilityMode(
            Adapters::IntegrationManager::CompatibilityMode::Hybrid);

        qDebug() << "🌉 Integration manager initialized in Hybrid mode";
#endif
    }

    void setupStateManager() {
        auto& stateManager = DeclarativeUI::Binding::StateManager::instance();

        // Initialize shared state
        stateManager.setState("shared.counter", 0);
        stateManager.setState("shared.message", QString("Integration Example"));
        stateManager.setState("shared.theme", QString("light"));
        stateManager.setState("sync.enabled", true);

        qDebug() << "🌐 Shared state initialized";
    }

    void setupUI() {
        auto centralWidget = new QWidget;
        setCentralWidget(centralWidget);

        auto mainLayout = new QVBoxLayout(centralWidget);
        mainLayout->setSpacing(20);
        mainLayout->setContentsMargins(20, 20, 20, 20);

        // Header
        auto header = new QLabel("🔗 Legacy-Command Integration Demo");
        header->setStyleSheet(
            "font-size: 24px; font-weight: bold; color: #2c3e50; text-align: "
            "center; padding: 15px; background-color: #ecf0f1; border-radius: "
            "8px;");
        header->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(header);

        // Create sections
        createLegacySection(mainLayout);
        createCommandSection(mainLayout);
        createHybridSection(mainLayout);
        createSyncSection(mainLayout);

        qDebug() << "✅ Integration UI created";
    }

    void createLegacySection(QVBoxLayout* parentLayout) {
        auto legacyGroup = new QGroupBox("📦 Legacy Components");
        legacyGroup->setStyleSheet(
            "QGroupBox { font-weight: bold; color: #e74c3c; border: 2px solid "
            "#e74c3c; border-radius: 5px; margin: 5px; padding-top: 10px; } "
            "QGroupBox::title { subcontrol-origin: margin; left: 10px; "
            "padding: 0 5px 0 5px; }");

        auto legacyLayout = new QVBoxLayout(legacyGroup);

        // Legacy button
        legacy_button_ = std::make_unique<Button>();
        legacy_button_->text("Legacy Button").onClick([this]() {
            onLegacyButtonClicked();
        });

        // Initialize and get widget for display
        legacy_button_->initialize();
        auto* legacyButtonWidget = legacy_button_->getWidget();
        legacyButtonWidget->setStyleSheet(
            "QPushButton { background-color: #e74c3c; color: white; padding: "
            "10px; border-radius: 5px; font-weight: bold; }");
        legacyLayout->addWidget(legacyButtonWidget);

        // Legacy info
        auto legacyInfo = new QLabel(
            "This section uses traditional DeclarativeUI components");
        legacyInfo->setStyleSheet("color: #7f8c8d; font-style: italic;");
        legacyLayout->addWidget(legacyInfo);

        parentLayout->addWidget(legacyGroup);
    }

    void createCommandSection(QVBoxLayout* parentLayout) {
        try {
            // Create Command-based section
            auto commandContainer =
                UI::CommandHierarchyBuilder("Container")
                    .layout("VBox")
                    .spacing(10)
                    .style(
                        "border: 2px solid #3498db; border-radius: 5px; "
                        "padding: 15px; background-color: #f8fbff;")

                    .addChild(
                        "Label",
                        [](UI::CommandBuilder& title) {
                            title.text("⚡ Command System Components")
                                .style(
                                    "font-weight: bold; color: #3498db; "
                                    "font-size: 16px; margin-bottom: 10px;");
                        })

                    .addChild(
                        "Button",
                        [this](UI::CommandBuilder& cmdButton) {
                            cmdButton.text("Command Button")
                                .style(
                                    "background-color: #3498db; color: white; "
                                    "padding: 10px; border-radius: 5px; "
                                    "font-weight: bold;")
                                .onClick(
                                    [this]() { onCommandButtonClicked(); });
                        })

                    .addChild(
                        "TextInput",
                        [this](UI::CommandBuilder& input) {
                            input.placeholder("Command input...")
                                .style(
                                    "padding: 8px; border: 1px solid #bdc3c7; "
                                    "border-radius: 3px;")
                                .bindToState("shared.message", "text")
                                .onTextChanged([this](const QString& text) {
                                    updateSharedMessage(text);
                                });
                        })

                    .addChild(
                        "Label",
                        [](UI::CommandBuilder& info) {
                            info.text(
                                    "This section uses the new Command-based "
                                    "UI system")
                                .style("color: #7f8c8d; font-style: italic;");
                        })

                    .build();

            // Convert to widget
            auto commandWidget = UI::WidgetMapper::instance().createWidget(
                commandContainer.get());
            if (commandWidget) {
                parentLayout->addWidget(commandWidget.release());
                command_container_ = commandContainer;
            }

        } catch (const std::exception& e) {
            qWarning() << "❌ Error creating Command section:" << e.what();
        }
    }

    void createHybridSection(QVBoxLayout* parentLayout) {
#ifdef DECLARATIVE_UI_ADAPTERS_ENABLED
        try {
            auto hybridGroup = new QGroupBox("🔄 Hybrid Integration");
            hybridGroup->setStyleSheet(
                "QGroupBox { font-weight: bold; color: #9b59b6; border: 2px "
                "solid #9b59b6; border-radius: 5px; margin: 5px; padding-top: "
                "10px; } QGroupBox::title { subcontrol-origin: margin; left: "
                "10px; padding: 0 5px 0 5px; }");

            auto hybridLayout = new QVBoxLayout(hybridGroup);

            // Create hybrid container
            auto& adapter = Adapters::ComponentSystemAdapter::instance();
            hybrid_container_ = adapter.createHybridContainer();
            hybrid_container_->setLayout("HBox");
            hybrid_container_->setSpacing(10);

            // Add legacy component to hybrid container
            auto legacyWidget = std::make_unique<Button>();
            legacyWidget->text("Hybrid Legacy").onClick([this]() {
                onHybridLegacyClicked();
            });

            legacyWidget->initialize();
            auto* legacyQWidget = legacyWidget->getWidget();
            legacyQWidget->setStyleSheet(
                "QPushButton { background-color: #9b59b6; color: white; "
                "padding: 8px; border-radius: 3px; }");
            hybrid_container_->addWidget(
                std::unique_ptr<QWidget>(legacyQWidget));

            // Add Command component to hybrid container
            auto commandButton =
                UI::CommandBuilder("Button")
                    .text("Hybrid Command")
                    .style(
                        "background-color: #8e44ad; color: white; padding: "
                        "8px; border-radius: 3px;")
                    .onClick([this]() { onHybridCommandClicked(); })
                    .build();

            hybrid_container_->addCommand(commandButton);

            // Convert hybrid container to widget
            auto hybridWidget = hybrid_container_->toWidget();
            if (hybridWidget) {
                hybridLayout->addWidget(hybridWidget.release());
            }

            auto hybridInfo = new QLabel(
                "This section mixes legacy and Command components in one "
                "container");
            hybridInfo->setStyleSheet("color: #7f8c8d; font-style: italic;");
            hybridLayout->addWidget(hybridInfo);

            parentLayout->addWidget(hybridGroup);

        } catch (const std::exception& e) {
            qWarning() << "❌ Error creating Hybrid section:" << e.what();
        }
#else
        auto hybridGroup = new QGroupBox("🔄 Hybrid Integration (Disabled)");
        hybridGroup->setStyleSheet(
            "QGroupBox { font-weight: bold; color: #95a5a6; border: 2px solid "
            "#95a5a6; border-radius: 5px; margin: 5px; padding-top: 10px; } "
            "QGroupBox::title { subcontrol-origin: margin; left: 10px; "
            "padding: 0 5px 0 5px; }");

        auto hybridLayout = new QVBoxLayout(hybridGroup);
        auto disabledInfo =
            new QLabel("Hybrid integration requires BUILD_ADAPTERS=ON");
        disabledInfo->setStyleSheet("color: #7f8c8d; font-style: italic;");
        hybridLayout->addWidget(disabledInfo);

        parentLayout->addWidget(hybridGroup);
#endif
    }

    void createSyncSection(QVBoxLayout* parentLayout) {
        auto syncGroup = new QGroupBox("🔄 Synchronization Status");
        syncGroup->setStyleSheet(
            "QGroupBox { font-weight: bold; color: #27ae60; border: 2px solid "
            "#27ae60; border-radius: 5px; margin: 5px; padding-top: 10px; } "
            "QGroupBox::title { subcontrol-origin: margin; left: 10px; "
            "padding: 0 5px 0 5px; }");

        auto syncLayout = new QVBoxLayout(syncGroup);

        // Status display
        status_display_ = new QTextEdit;
        status_display_->setReadOnly(true);
        status_display_->setMaximumHeight(120);
        status_display_->setStyleSheet(
            "QTextEdit { background-color: #2c3e50; color: #ecf0f1; "
            "font-family: monospace; border-radius: 3px; }");
        syncLayout->addWidget(status_display_);

        // Control buttons
        auto buttonLayout = new QHBoxLayout;

        auto syncButton = new QPushButton("🔄 Sync All");
        syncButton->setStyleSheet(
            "QPushButton { background-color: #27ae60; color: white; padding: "
            "8px 15px; border-radius: 3px; }");
        connect(syncButton, &QPushButton::clicked, this,
                &IntegrationExample::syncAll);
        buttonLayout->addWidget(syncButton);

        auto clearButton = new QPushButton("🧹 Clear Status");
        clearButton->setStyleSheet(
            "QPushButton { background-color: #95a5a6; color: white; padding: "
            "8px 15px; border-radius: 3px; }");
        connect(clearButton, &QPushButton::clicked, this,
                &IntegrationExample::clearStatus);
        buttonLayout->addWidget(clearButton);

        buttonLayout->addStretch();
        syncLayout->addLayout(buttonLayout);

        parentLayout->addWidget(syncGroup);

        // Initialize status
        logStatus("Integration example initialized");
        logStatus("Legacy and Command systems ready");
    }

    void onLegacyButtonClicked() {
        auto& stateManager = DeclarativeUI::Binding::StateManager::instance();
        auto counterState = stateManager.getState<int>("shared.counter");
        int currentValue = counterState ? counterState->get() : 0;
        stateManager.setState("shared.counter", currentValue + 1);

        logStatus(QString("Legacy button clicked - Counter: %1")
                      .arg(currentValue + 1));
        qDebug() << "🔴 Legacy button clicked";
    }

    void onCommandButtonClicked() {
        auto& stateManager = DeclarativeUI::Binding::StateManager::instance();
        auto counterState = stateManager.getState<int>("shared.counter");
        int currentValue = counterState ? counterState->get() : 0;
        stateManager.setState("shared.counter", currentValue + 10);

        logStatus(QString("Command button clicked - Counter: %1")
                      .arg(currentValue + 10));
        qDebug() << "🔵 Command button clicked";
    }

    void onHybridLegacyClicked() {
        logStatus("Hybrid legacy component clicked");
        qDebug() << "🟣 Hybrid legacy clicked";
    }

    void onHybridCommandClicked() {
        logStatus("Hybrid command component clicked");
        qDebug() << "🟣 Hybrid command clicked";
    }

    void updateSharedMessage(const QString& message) {
        auto& stateManager = DeclarativeUI::Binding::StateManager::instance();
        stateManager.setState("shared.message", message);
        logStatus(QString("Shared message updated: %1").arg(message));
    }

    void syncAll() {
#ifdef DECLARATIVE_UI_ADAPTERS_ENABLED
        auto& stateAdapter = Adapters::CommandStateManagerAdapter::instance();
        stateAdapter.syncAllBoundCommands();
        logStatus("All components synchronized");
#else
        logStatus("Sync requires adapters (BUILD_ADAPTERS=ON)");
#endif
    }

    void clearStatus() {
        if (status_display_) {
            status_display_->clear();
            logStatus("Status cleared");
        }
    }

    void logStatus(const QString& message) {
        if (status_display_) {
            QString timestamp = QTime::currentTime().toString("hh:mm:ss");
            QString logEntry = QString("[%1] %2").arg(timestamp, message);
            status_display_->append(logEntry);

            // Auto-scroll to bottom
            auto cursor = status_display_->textCursor();
            cursor.movePosition(QTextCursor::End);
            status_display_->setTextCursor(cursor);
        }
    }

private:
    // Legacy components
    std::unique_ptr<Button> legacy_button_;

    // Command components
    std::shared_ptr<UI::BaseUICommand> command_container_;

    // Hybrid components
#ifdef DECLARATIVE_UI_ADAPTERS_ENABLED
    std::unique_ptr<Adapters::ComponentSystemAdapter::HybridContainer>
        hybrid_container_;
#endif

    // UI elements
    QTextEdit* status_display_;
};

#include "31_integration_example.moc"

#endif  // DECLARATIVE_UI_COMMAND_SYSTEM_ENABLED

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

#ifdef DECLARATIVE_UI_COMMAND_SYSTEM_ENABLED
    qDebug() << "🚀 Starting Integration Example";

    IntegrationExample window;
    window.show();

    qDebug() << "💡 This example demonstrates:";
    qDebug() << "   - Legacy-Command integration";
    qDebug() << "   - Component conversion";
    qDebug() << "   - Bidirectional synchronization";
    qDebug() << "   - Hybrid containers";
    qDebug() << "   - Migration strategies";

    return app.exec();
#else
    qWarning() << "❌ Command system not enabled. Please build with "
                  "BUILD_COMMAND_SYSTEM=ON";
    qWarning() << "💡 This example requires the Command system to demonstrate "
                  "integration";

    return 1;
#endif
}
