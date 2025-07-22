/**
 * @file 30_command_ui_builder.cpp
 * @brief Command-based UI Builder demonstration
 * 
 * This example demonstrates:
 * - Command-based UI construction using builders
 * - State management integration
 * - Event handling in Command system
 * - Widget mapping from Commands
 * - Integration with legacy components
 * 
 * Learning objectives:
 * - Master Command Builder patterns
 * - Understand Command-Widget mapping
 * - Learn state synchronization
 * - See Command system architecture
 */

#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QDebug>
#include <QTimer>
#include <QTime>
#include <QRandomGenerator>

#ifdef DECLARATIVE_UI_COMMAND_SYSTEM_ENABLED
#include "Command/CommandBuilder.hpp"
#include "Command/WidgetMapper.hpp"
#include "Command/UICommandFactory.hpp"
#include "Binding/StateManager.hpp"

#ifdef DECLARATIVE_UI_ADAPTERS_ENABLED
#include "Command/Adapters/StateManagerAdapter.hpp"
#include "Command/Adapters/ComponentSystemAdapter.hpp"
#endif

using namespace DeclarativeUI::Command;

class CommandUIBuilderExample : public QMainWindow {
    Q_OBJECT

public:
    CommandUIBuilderExample(QWidget* parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("Command-based UI Builder Example");
        setMinimumSize(800, 600);
        
        setupStateManager();
        setupUI();
        setupDemoTimer();
    }

private:
    void setupStateManager() {
        auto& stateManager = DeclarativeUI::Binding::StateManager::instance();
        
        // Initialize application state
        stateManager.setState("app.title", QString("Command UI Builder"));
        stateManager.setState("counter.value", 0);
        stateManager.setState("user.name", QString(""));
        stateManager.setState("form.message", QString("Welcome to Command-based UI!"));
        stateManager.setState("demo.running", false);
        stateManager.setState("theme.current", QString("light"));
        
        qDebug() << "🌐 State manager initialized";
    }
    
    void setupUI() {
        try {
            // Create a sophisticated UI using Command Builder
            auto mainUI = UI::CommandHierarchyBuilder("Container")
                .layout("VBox")
                .spacing(20)
                .margins(25)
                
                // Header with title and controls
                .addContainer("Container", [this](UI::CommandHierarchyBuilder& header) {
                    header.layout("HBox")
                          .spacing(15)
                          .style("background-color: #3498db; color: white; padding: 20px; border-radius: 10px; margin-bottom: 10px;")
                          .addChild("Label", [](UI::CommandBuilder& title) {
                              title.text("🚀 Command UI Builder Demo")
                                   .style("font-size: 24px; font-weight: bold;");
                          })
                          .addContainer("Container", [this](UI::CommandHierarchyBuilder& headerControls) {
                              headerControls.layout("HBox")
                                           .spacing(10)
                                           .addChild("Button", [this](UI::CommandBuilder& themeBtn) {
                                               themeBtn.text("🎨 Theme")
                                                      .style("background-color: #2980b9; color: white; padding: 8px 15px; border-radius: 5px;")
                                                      .onClick([this]() {
                                                          toggleTheme();
                                                      });
                                           })
                                           .addChild("Button", [this](UI::CommandBuilder& demoBtn) {
                                               demoBtn.text("▶️ Demo")
                                                     .style("background-color: #27ae60; color: white; padding: 8px 15px; border-radius: 5px;")
                                                     .onClick([this]() {
                                                         startDemo();
                                                     });
                                           });
                          });
                })
                
                // Main content area with tabs
                .addContainer("Container", [this](UI::CommandHierarchyBuilder& contentArea) {
                    contentArea.layout("HBox")
                              .spacing(20)

                              // Left panel - Interactive controls
                              .addContainer("Container", [this](UI::CommandHierarchyBuilder& leftPanel) {
                                  leftPanel.layout("VBox")
                                          .spacing(15)
                                          .style("background-color: white; padding: 20px; border-radius: 8px; border: 1px solid #bdc3c7; min-width: 350px;")

                                          // Counter section
                                          .addContainer("Container", [this](UI::CommandHierarchyBuilder& counterSection) {
                                              counterSection.layout("VBox")
                                                           .spacing(10)
                                                           .style("border: 2px solid #e74c3c; border-radius: 8px; padding: 15px; background-color: #fdf2f2;")
                                                           .addChild("Label", [](UI::CommandBuilder& counterTitle) {
                                                               counterTitle.text("🔢 Interactive Counter")
                                                                          .style("font-size: 16px; font-weight: bold; color: #e74c3c;");
                                                           })
                                                           .addChild("Label", [](UI::CommandBuilder& counterDisplay) {
                                                               counterDisplay.text("Count: 0")
                                                                            .style("font-size: 32px; color: #2c3e50; text-align: center; font-weight: bold;")
                                                                            .bindToState("counter.display", "text");
                                                           })
                                                           .addContainer("Container", [this](UI::CommandHierarchyBuilder& counterButtons) {
                                                               counterButtons.layout("HBox")
                                                                            .spacing(8)
                                                                            .addChild("Button", [this](UI::CommandBuilder& minusBtn) {
                                                                                minusBtn.text("➖")
                                                                                       .style("background-color: #e74c3c; color: white; padding: 10px; border-radius: 5px; font-size: 18px; min-width: 50px;")
                                                                                       .onClick([this]() {
                                                                                           modifyCounter(-1);
                                                                                       });
                                                                            })
                                                                            .addChild("Button", [this](UI::CommandBuilder& resetBtn) {
                                                                                resetBtn.text("🔄")
                                                                                       .style("background-color: #f39c12; color: white; padding: 10px; border-radius: 5px; font-size: 18px; min-width: 50px;")
                                                                                       .onClick([this]() {
                                                                                           resetCounter();
                                                                                       });
                                                                            })
                                                                            .addChild("Button", [this](UI::CommandBuilder& plusBtn) {
                                                                                plusBtn.text("➕")
                                                                                      .style("background-color: #27ae60; color: white; padding: 10px; border-radius: 5px; font-size: 18px; min-width: 50px;")
                                                                                      .onClick([this]() {
                                                                                          modifyCounter(1);
                                                                                      });
                                                                            });
                                                           });
                                          })

                                          // User input section
                                          .addContainer("Container", [this](UI::CommandHierarchyBuilder& inputSection) {
                                              inputSection.layout("VBox")
                                                         .spacing(10)
                                                         .style("border: 2px solid #3498db; border-radius: 8px; padding: 15px; background-color: #f8fbff;")
                                                         .addChild("Label", [](UI::CommandBuilder& inputTitle) {
                                                             inputTitle.text("👤 User Input")
                                                                      .style("font-size: 16px; font-weight: bold; color: #3498db;");
                                                         })
                                                         .addChild("TextInput", [this](UI::CommandBuilder& nameInput) {
                                                             nameInput.placeholder("Enter your name...")
                                                                     .style("padding: 12px; border: 2px solid #bdc3c7; border-radius: 5px; font-size: 14px;")
                                                                     .bindToState("user.name", "text")
                                                                     .onTextChanged([this](const QString& text) {
                                                                         updateGreeting(text);
                                                                     });
                                                         })
                                                         .addChild("Label", [](UI::CommandBuilder& greeting) {
                                                             greeting.text("Welcome to Command-based UI!")
                                                                    .style("font-size: 14px; color: #7f8c8d; font-style: italic; margin-top: 8px; padding: 8px; background-color: #ecf0f1; border-radius: 3px;")
                                                                    .bindToState("form.message", "text");
                                                         });
                                          })

                                          // Action buttons
                                          .addContainer("Container", [this](UI::CommandHierarchyBuilder& actionButtons) {
                                              actionButtons.layout("VBox")
                                                          .spacing(8)
                                                          .addChild("Button", [this](UI::CommandBuilder& randomBtn) {
                                                              randomBtn.text("🎲 Random Action")
                                                                      .style("background-color: #9b59b6; color: white; padding: 12px; border-radius: 5px; font-weight: bold;")
                                                                      .onClick([this]() {
                                                                          performRandomAction();
                                                                      });
                                                          })
                                                          .addChild("Button", [this](UI::CommandBuilder& clearBtn) {
                                                              clearBtn.text("🧹 Clear All")
                                                                     .style("background-color: #95a5a6; color: white; padding: 12px; border-radius: 5px; font-weight: bold;")
                                                                     .onClick([this]() {
                                                                         clearAll();
                                                                     });
                                                          });
                                          });
                              })

                              // Right panel - Information and status
                              .addContainer("Container", [this](UI::CommandHierarchyBuilder& rightPanel) {
                                  rightPanel.layout("VBox")
                                           .spacing(15)
                                           .style("background-color: white; padding: 20px; border-radius: 8px; border: 1px solid #bdc3c7; flex: 1;")
                                           
                                           .addChild("Label", [](UI::CommandBuilder& infoTitle) {
                                               infoTitle.text("📊 Command System Information")
                                                       .style("font-size: 18px; font-weight: bold; color: #2c3e50; margin-bottom: 10px;");
                                           })

                                           .addContainer("Container", [](UI::CommandHierarchyBuilder& infoGrid) {
                                               infoGrid.layout("VBox")
                                                      .spacing(8)
                                                      .addChild("Label", [](UI::CommandBuilder& info1) {
                                                          info1.text("✅ Command Builder Pattern")
                                                               .style("font-size: 14px; color: #27ae60; padding: 5px;");
                                                      })
                                                      .addChild("Label", [](UI::CommandBuilder& info2) {
                                                          info2.text("✅ State Management Integration")
                                                               .style("font-size: 14px; color: #27ae60; padding: 5px;");
                                                      })
                                                      .addChild("Label", [](UI::CommandBuilder& info3) {
                                                          info3.text("✅ Event Handling System")
                                                               .style("font-size: 14px; color: #27ae60; padding: 5px;");
                                                      })
                                                      .addChild("Label", [](UI::CommandBuilder& info4) {
                                                          info4.text("✅ Widget Mapping")
                                                               .style("font-size: 14px; color: #27ae60; padding: 5px;");
                                                      })
                                                      .addChild("Label", [](UI::CommandBuilder& info5) {
                                                          info5.text("✅ Declarative UI Construction")
                                                               .style("font-size: 14px; color: #27ae60; padding: 5px;");
                                                      });
                                           })

                                           .addContainer("Container", [](UI::CommandHierarchyBuilder& statusArea) {
                                               statusArea.layout("VBox")
                                                        .spacing(10)
                                                        .style("background-color: #f8f9fa; padding: 15px; border-radius: 5px; margin-top: 20px;")
                                                        .addChild("Label", [](UI::CommandBuilder& statusTitle) {
                                                            statusTitle.text("🔄 Live Status")
                                                                      .style("font-size: 16px; font-weight: bold; color: #495057;");
                                                        })
                                                        .addChild("Label", [](UI::CommandBuilder& status) {
                                                            status.text("System Ready")
                                                                  .style("font-size: 14px; color: #6c757d; font-family: monospace;")
                                                                  .bindToState("system.status", "text");
                                                        });
                                           });
                              });
                })

                // Footer
                .addContainer("Container", [](UI::CommandHierarchyBuilder& footer) {
                    footer.layout("HBox")
                          .spacing(10)
                          .style("background-color: #34495e; color: white; padding: 15px; border-radius: 5px; margin-top: 10px;")
                          .addChild("Label", [](UI::CommandBuilder& footerText) {
                              footerText.text("🎯 Command-based UI Builder - Demonstrating modern declarative UI patterns")
                                        .style("font-size: 12px; color: #bdc3c7;");
                          });
                })
                
                .build();

            // Convert to widget and set as central widget
            auto widget = UI::WidgetMapper::instance().createWidget(mainUI.get());
            if (widget) {
                setCentralWidget(widget.release());
                qDebug() << "✅ Command-based UI created successfully";
                
                // Set up state monitoring
                setupStateMonitoring();
                
                // Initialize displays
                updateCounterDisplay();
                updateSystemStatus("Command UI Builder initialized");
                
            } else {
                qWarning() << "❌ Failed to create widget from command";
            }
        } catch (const std::exception& e) {
            qWarning() << "❌ Error creating Command UI Builder:" << e.what();
        }
    }
    
    void setupStateMonitoring() {
        auto& stateManager = DeclarativeUI::Binding::StateManager::instance();

        connect(&stateManager, &DeclarativeUI::Binding::StateManager::stateChanged,
                this, [this](const QString& key, const QVariant& value) {
                    qDebug() << "🔄 State changed:" << key << "=" << value;
                    
                    if (key == "counter.value") {
                        updateCounterDisplay();
                    }
                });
    }
    
    void setupDemoTimer() {
        demo_timer_ = new QTimer(this);
        connect(demo_timer_, &QTimer::timeout, this, &CommandUIBuilderExample::demoStep);
    }
    
    void modifyCounter(int delta) {
        auto& stateManager = DeclarativeUI::Binding::StateManager::instance();
        auto counterState = stateManager.getState<int>("counter.value");
        int currentValue = counterState ? counterState->get() : 0;
        int newValue = currentValue + delta;
        
        stateManager.setState("counter.value", newValue);
        updateSystemStatus(QString("Counter %1 to %2").arg(delta > 0 ? "incremented" : "decremented").arg(newValue));
        qDebug() << "🔢 Counter modified by" << delta << "to" << newValue;
    }
    
    void resetCounter() {
        auto& stateManager = DeclarativeUI::Binding::StateManager::instance();
        stateManager.setState("counter.value", 0);
        updateSystemStatus("Counter reset to 0");
        qDebug() << "🔄 Counter reset";
    }
    
    void updateCounterDisplay() {
        auto& stateManager = DeclarativeUI::Binding::StateManager::instance();
        auto counterState = stateManager.getState<int>("counter.value");
        int value = counterState ? counterState->get() : 0;
        
        QString displayText = QString("Count: %1").arg(value);
        stateManager.setState("counter.display", displayText);
    }
    
    void updateGreeting(const QString& name) {
        auto& stateManager = DeclarativeUI::Binding::StateManager::instance();
        QString message = name.isEmpty() ? 
            "Welcome to Command-based UI!" : 
            QString("Hello, %1! Enjoying the Command system?").arg(name);
        
        stateManager.setState("form.message", message);
        updateSystemStatus(QString("Greeting updated for: %1").arg(name.isEmpty() ? "anonymous" : name));
    }
    
    void updateSystemStatus(const QString& status) {
        auto& stateManager = DeclarativeUI::Binding::StateManager::instance();
        QString timestamp = QTime::currentTime().toString("hh:mm:ss");
        QString statusMessage = QString("[%1] %2").arg(timestamp, status);
        stateManager.setState("system.status", statusMessage);
    }
    
    void toggleTheme() {
        auto& stateManager = DeclarativeUI::Binding::StateManager::instance();
        auto themeState = stateManager.getState<QString>("theme.current");
        QString currentTheme = themeState ? themeState->get() : "light";
        
        QString newTheme = (currentTheme == "light") ? "dark" : "light";
        stateManager.setState("theme.current", newTheme);
        
        // Apply theme
        QString bgColor = (newTheme == "dark") ? "#2c3e50" : "#f0f0f0";
        setStyleSheet(QString("QMainWindow { background-color: %1; }").arg(bgColor));
        
        updateSystemStatus(QString("Theme switched to %1 mode").arg(newTheme));
        qDebug() << "🎨 Theme toggled to:" << newTheme;
    }
    
    void startDemo() {
        auto& stateManager = DeclarativeUI::Binding::StateManager::instance();
        bool demoRunning = stateManager.getState<bool>("demo.running")->get();
        
        if (!demoRunning) {
            stateManager.setState("demo.running", true);
            demo_step_ = 0;
            demo_timer_->start(1500);
            updateSystemStatus("Demo sequence started");
            qDebug() << "🎬 Demo started";
        } else {
            demo_timer_->stop();
            stateManager.setState("demo.running", false);
            updateSystemStatus("Demo sequence stopped");
            qDebug() << "⏹️ Demo stopped";
        }
    }
    
    void demoStep() {
        switch (demo_step_) {
            case 0:
                modifyCounter(3);
                break;
            case 1: {
                auto& stateManager = DeclarativeUI::Binding::StateManager::instance();
                stateManager.setState("user.name", QString("Demo User"));
                break;
            }
            case 2:
                modifyCounter(2);
                break;
            case 3:
                toggleTheme();
                break;
            case 4:
                modifyCounter(-1);
                break;
            case 5: {
                auto& stateManager = DeclarativeUI::Binding::StateManager::instance();
                stateManager.setState("user.name", QString(""));
                demo_timer_->stop();
                stateManager.setState("demo.running", false);
                updateSystemStatus("Demo sequence completed");
                qDebug() << "🎉 Demo completed";
                return;
            }
        }
        demo_step_++;
    }
    
    void performRandomAction() {
        int action = QRandomGenerator::global()->bounded(4);
        
        switch (action) {
            case 0:
                modifyCounter(QRandomGenerator::global()->bounded(10) - 5);
                break;
            case 1:
                toggleTheme();
                break;
            case 2: {
                QStringList names = {"Alice", "Bob", "Charlie", "Diana", "Eve"};
                auto& stateManager = DeclarativeUI::Binding::StateManager::instance();
                stateManager.setState("user.name", names[QRandomGenerator::global()->bounded(names.size())]);
                break;
            }
            case 3:
                resetCounter();
                break;
        }
        
        updateSystemStatus("Random action performed");
    }
    
    void clearAll() {
        auto& stateManager = DeclarativeUI::Binding::StateManager::instance();
        stateManager.setState("counter.value", 0);
        stateManager.setState("user.name", QString(""));
        stateManager.setState("form.message", QString("All cleared - ready for new input"));
        stateManager.setState("theme.current", QString("light"));
        
        setStyleSheet("");
        updateSystemStatus("All data cleared");
        qDebug() << "🧹 All cleared";
    }

private:
    QTimer* demo_timer_;
    int demo_step_ = 0;
};

#include "30_command_ui_builder.moc"

#endif // DECLARATIVE_UI_COMMAND_SYSTEM_ENABLED

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

#ifdef DECLARATIVE_UI_COMMAND_SYSTEM_ENABLED
    qDebug() << "🚀 Starting Command UI Builder Example";
    
    CommandUIBuilderExample window;
    window.show();
    
    qDebug() << "💡 This example demonstrates:";
    qDebug() << "   - Command-based UI construction";
    qDebug() << "   - State management integration";
    qDebug() << "   - Event handling in Command system";
    qDebug() << "   - Widget mapping from Commands";
    qDebug() << "   - Declarative UI patterns";
    
    return app.exec();
#else
    qWarning() << "❌ Command system not enabled. Please build with BUILD_COMMAND_SYSTEM=ON";
    qWarning() << "💡 This example requires the Command system to demonstrate:";
    qWarning() << "   - Command Builder patterns";
    qWarning() << "   - State management integration";
    qWarning() << "   - Command-Widget mapping";
    
    return 1;
#endif
}
