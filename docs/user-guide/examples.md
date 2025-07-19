# DeclarativeUI Examples

This document provides comprehensive examples demonstrating various features and patterns in DeclarativeUI.

## Basic Examples

### Simple Button Application

```cpp
#include <QApplication>
#include "Core/DeclarativeBuilder.hpp"
#include "Components/Button.hpp"
#include "Components/Label.hpp"

using namespace DeclarativeUI;

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    auto window = Core::create<QWidget>()
        .property("windowTitle", "Simple Button Example")
        .property("minimumSize", QSize(250, 150))
        .layout<QVBoxLayout>([](auto *layout) {
            layout->setSpacing(20);
            layout->setContentsMargins(20, 20, 20, 20);
        })
        .child<Components::Label>([](auto &label) {
            label.text("Click the button below!")
                 .style("QLabel { font-size: 14px; }");
        })
        .child<Components::Button>([](auto &button) {
            button.text("Click Me!")
                  .onClick([]() {
                      QMessageBox::information(nullptr, "Success", "Button clicked!");
                  });
        })
        .build();
    
    window->show();
    return app.exec();
}
```

### Form Input Example

```cpp
#include "Components/LineEdit.hpp"
#include "Components/CheckBox.hpp"
#include "Components/ComboBox.hpp"

class FormExample : public QWidget {
    Q_OBJECT
    
public:
    FormExample(QWidget *parent = nullptr) : QWidget(parent) {
        setupUI();
    }
    
private:
    void setupUI() {
        auto layout = new QFormLayout(this);
        
        // Name input
        name_edit_ = std::make_unique<Components::LineEdit>();
        name_edit_->placeholder("Enter your name...")
                  ->onTextChanged([this](const QString& text) {
                      validateForm();
                  });
        name_edit_->initialize();
        layout->addRow("Name:", name_edit_->getWidget());
        
        // Email input
        email_edit_ = std::make_unique<Components::LineEdit>();
        email_edit_->placeholder("Enter your email...")
                   ->onTextChanged([this](const QString& text) {
                       validateForm();
                   });
        email_edit_->initialize();
        layout->addRow("Email:", email_edit_->getWidget());
        
        // Country selection
        country_combo_ = std::make_unique<Components::ComboBox>();
        country_combo_->addItem("United States")
                      .addItem("Canada")
                      .addItem("United Kingdom")
                      .addItem("Germany")
                      .addItem("France");
        country_combo_->initialize();
        layout->addRow("Country:", country_combo_->getWidget());
        
        // Newsletter subscription
        newsletter_check_ = std::make_unique<Components::CheckBox>();
        newsletter_check_->text("Subscribe to newsletter");
        newsletter_check_->initialize();
        layout->addRow("", newsletter_check_->getWidget());
        
        // Submit button
        submit_button_ = std::make_unique<Components::Button>();
        submit_button_->text("Submit")
                     ->enabled(false)
                     ->onClick([this]() {
                         submitForm();
                     });
        submit_button_->initialize();
        layout->addRow("", submit_button_->getWidget());
    }
    
    void validateForm() {
        bool valid = !name_edit_->getText().isEmpty() && 
                    email_edit_->getText().contains("@");
        
        submit_button_->enabled(valid);
    }
    
    void submitForm() {
        QString message = QString("Form submitted!\nName: %1\nEmail: %2\nCountry: %3\nNewsletter: %4")
            .arg(name_edit_->getText())
            .arg(email_edit_->getText())
            .arg(country_combo_->getCurrentText())
            .arg(newsletter_check_->isChecked() ? "Yes" : "No");
            
        QMessageBox::information(this, "Form Submitted", message);
    }
    
private:
    std::unique_ptr<Components::LineEdit> name_edit_;
    std::unique_ptr<Components::LineEdit> email_edit_;
    std::unique_ptr<Components::ComboBox> country_combo_;
    std::unique_ptr<Components::CheckBox> newsletter_check_;
    std::unique_ptr<Components::Button> submit_button_;
};
```

## State Management Examples

### Counter Application with State

```cpp
#include "Binding/StateManager.hpp"

class CounterApp : public QWidget {
    Q_OBJECT
    
public:
    CounterApp(QWidget *parent = nullptr) : QWidget(parent) {
        setupState();
        setupUI();
    }
    
private:
    void setupState() {
        auto& state = StateManager::instance();
        
        // Initialize counter state
        state.setState("counter", 0);
        state.enableHistory("counter", 50); // Enable undo/redo
        
        // Set up validation
        state.setValidator<int>("counter", [](const int& value) {
            return value >= 0 && value <= 100; // Limit range
        });
        
        // Computed state for display text
        state.setState("displayText", QString("Count: 0"));
        state.observeState<int>("counter", [&state](const int& value) {
            state.setState("displayText", QString("Count: %1").arg(value));
        });
    }
    
    void setupUI() {
        auto& state = StateManager::instance();
        
        auto layout = new QVBoxLayout(this);
        layout->setSpacing(20);
        layout->setContentsMargins(20, 20, 20, 20);
        
        // Display label
        display_label_ = std::make_unique<Components::Label>();
        display_label_->bindProperty("text", [&state]() {
            auto text = state.getState<QString>("displayText");
            return text ? text->get() : QString("Count: 0");
        });
        display_label_->style("QLabel { font-size: 24px; font-weight: bold; }");
        display_label_->initialize();
        layout->addWidget(display_label_->getWidget());
        
        // Button row
        auto button_layout = new QHBoxLayout();
        
        // Decrement button
        auto decrement_btn = std::make_unique<Components::Button>();
        decrement_btn->text("-")
                    ->onClick([&state]() {
                        auto counter = state.getState<int>("counter");
                        if (counter && counter->get() > 0) {
                            counter->set(counter->get() - 1);
                        }
                    });
        decrement_btn->initialize();
        button_layout->addWidget(decrement_btn->getWidget());
        
        // Reset button
        auto reset_btn = std::make_unique<Components::Button>();
        reset_btn->text("Reset")
                ->onClick([&state]() {
                    state.setState("counter", 0);
                });
        reset_btn->initialize();
        button_layout->addWidget(reset_btn->getWidget());
        
        // Increment button
        auto increment_btn = std::make_unique<Components::Button>();
        increment_btn->text("+")
                   ->onClick([&state]() {
                       auto counter = state.getState<int>("counter");
                       if (counter && counter->get() < 100) {
                           counter->set(counter->get() + 1);
                       }
                   });
        increment_btn->initialize();
        button_layout->addWidget(increment_btn->getWidget());
        
        layout->addLayout(button_layout);
        
        // Undo/Redo buttons
        auto history_layout = new QHBoxLayout();
        
        auto undo_btn = std::make_unique<Components::Button>();
        undo_btn->text("Undo")
               ->onClick([&state]() {
                   if (state.canUndo("counter")) {
                       state.undo("counter");
                   }
               });
        undo_btn->initialize();
        history_layout->addWidget(undo_btn->getWidget());
        
        auto redo_btn = std::make_unique<Components::Button>();
        redo_btn->text("Redo")
               ->onClick([&state]() {
                   if (state.canRedo("counter")) {
                       state.redo("counter");
                   }
               });
        redo_btn->initialize();
        history_layout->addWidget(redo_btn->getWidget());
        
        layout->addLayout(history_layout);
        
        // Store button references for cleanup
        buttons_.push_back(std::move(decrement_btn));
        buttons_.push_back(std::move(reset_btn));
        buttons_.push_back(std::move(increment_btn));
        buttons_.push_back(std::move(undo_btn));
        buttons_.push_back(std::move(redo_btn));
    }
    
private:
    std::unique_ptr<Components::Label> display_label_;
    std::vector<std::unique_ptr<Components::Button>> buttons_;
};
```

## JSON UI Examples

### JSON Configuration File

Create `ui/dashboard.json`:

```json
{
    "type": "QWidget",
    "properties": {
        "windowTitle": "Dashboard Example",
        "minimumSize": [600, 400],
        "styleSheet": "QWidget { background-color: #f5f5f5; }"
    },
    "layout": {
        "type": "VBoxLayout",
        "spacing": 20,
        "margins": [20, 20, 20, 20]
    },
    "children": [
        {
            "type": "QLabel",
            "properties": {
                "text": "📊 Dashboard",
                "alignment": 4,
                "styleSheet": "QLabel { font-size: 24px; font-weight: bold; color: #2c3e50; }"
            }
        },
        {
            "type": "QWidget",
            "layout": {
                "type": "HBoxLayout",
                "spacing": 15
            },
            "children": [
                {
                    "type": "QGroupBox",
                    "properties": {
                        "title": "Statistics",
                        "styleSheet": "QGroupBox { font-weight: bold; }"
                    },
                    "layout": {
                        "type": "VBoxLayout",
                        "spacing": 10
                    },
                    "children": [
                        {
                            "type": "QLabel",
                            "id": "users_count",
                            "bindings": {
                                "text": "users_count"
                            },
                            "properties": {
                                "styleSheet": "QLabel { font-size: 18px; color: #27ae60; }"
                            }
                        },
                        {
                            "type": "QLabel",
                            "id": "revenue_label",
                            "bindings": {
                                "text": "revenue_text"
                            },
                            "properties": {
                                "styleSheet": "QLabel { font-size: 18px; color: #e74c3c; }"
                            }
                        }
                    ]
                },
                {
                    "type": "QGroupBox",
                    "properties": {
                        "title": "Actions",
                        "styleSheet": "QGroupBox { font-weight: bold; }"
                    },
                    "layout": {
                        "type": "VBoxLayout",
                        "spacing": 10
                    },
                    "children": [
                        {
                            "type": "QPushButton",
                            "properties": {
                                "text": "📈 Generate Report",
                                "minimumHeight": 40,
                                "styleSheet": "QPushButton { background-color: #3498db; color: white; font-weight: bold; border-radius: 5px; }"
                            },
                            "events": {
                                "clicked": "generateReport"
                            }
                        },
                        {
                            "type": "QPushButton",
                            "properties": {
                                "text": "🔄 Refresh Data",
                                "minimumHeight": 40,
                                "styleSheet": "QPushButton { background-color: #2ecc71; color: white; font-weight: bold; border-radius: 5px; }"
                            },
                            "events": {
                                "clicked": "refreshData"
                            }
                        }
                    ]
                }
            ]
        }
    ]
}
```

### Loading JSON UI with State Binding

```cpp
#include "JSON/JSONUILoader.hpp"
#include "Binding/StateManager.hpp"

class DashboardApp : public QObject {
    Q_OBJECT
    
public:
    DashboardApp() {
        setupState();
        setupUI();
    }
    
private:
    void setupState() {
        auto& state = StateManager::instance();
        
        // Initialize dashboard state
        state.setState("users_count", QString("👥 Users: 1,234"));
        state.setState("revenue_text", QString("💰 Revenue: $12,345"));
        
        // Simulate data updates
        timer_ = new QTimer(this);
        connect(timer_, &QTimer::timeout, this, &DashboardApp::updateData);
        timer_->start(5000); // Update every 5 seconds
    }
    
    void setupUI() {
        ui_loader_ = std::make_unique<JSON::JSONUILoader>();
        
        // Bind state manager
        ui_loader_->bindStateManager(
            std::shared_ptr<Binding::StateManager>(&StateManager::instance(), [](auto*){}));
        
        // Register event handlers
        ui_loader_->registerEventHandler("generateReport", [this]() {
            generateReport();
        });
        
        ui_loader_->registerEventHandler("refreshData", [this]() {
            refreshData();
        });
        
        // Load UI from JSON
        main_widget_ = ui_loader_->loadFromFile("ui/dashboard.json");
        if (main_widget_) {
            main_widget_->show();
        }
    }
    
private slots:
    void updateData() {
        auto& state = StateManager::instance();
        
        // Simulate changing data
        static int users = 1234;
        static int revenue = 12345;
        
        users += QRandomGenerator::global()->bounded(10, 50);
        revenue += QRandomGenerator::global()->bounded(100, 500);
        
        state.setState("users_count", QString("👥 Users: %1").arg(users));
        state.setState("revenue_text", QString("💰 Revenue: $%1").arg(revenue));
    }
    
    void generateReport() {
        QMessageBox::information(main_widget_.get(), "Report", 
                               "📊 Report generated successfully!");
    }
    
    void refreshData() {
        updateData();
        QMessageBox::information(main_widget_.get(), "Refresh", 
                               "🔄 Data refreshed!");
    }
    
private:
    std::unique_ptr<JSON::JSONUILoader> ui_loader_;
    std::unique_ptr<QWidget> main_widget_;
    QTimer* timer_;
};
```

## Hot Reload Example

### Development Setup with Hot Reload

```cpp
#include "HotReload/HotReloadManager.hpp"

class HotReloadApp : public QObject {
    Q_OBJECT
    
public:
    HotReloadApp() {
        setupUI();
        setupHotReload();
    }
    
private:
    void setupUI() {
        ui_loader_ = std::make_unique<JSON::JSONUILoader>();
        
        // Register event handlers
        ui_loader_->registerEventHandler("saveFile", [this]() {
            QMessageBox::information(main_widget_.get(), "Save", "File saved!");
        });
        
        ui_loader_->registerEventHandler("openFile", [this]() {
            QMessageBox::information(main_widget_.get(), "Open", "File opened!");
        });
        
        // Load initial UI
        main_widget_ = ui_loader_->loadFromFile("ui/editor.json");
        if (main_widget_) {
            main_widget_->show();
        }
    }
    
    void setupHotReload() {
        hot_reload_manager_ = std::make_unique<HotReload::HotReloadManager>();
        
        // Set up error handling
        hot_reload_manager_->setErrorHandler(
            [this](const QString& file, const QString& error) {
                qWarning() << "Hot reload failed for" << file << ":" << error;
                QMessageBox::warning(main_widget_.get(), "Hot Reload Error",
                                   QString("Failed to reload %1:\n%2").arg(file, error));
            });
        
        // Register UI file for hot reloading
        hot_reload_manager_->registerUIFile("ui/editor.json", main_widget_.get());
        
        // Connect hot reload signals
        connect(hot_reload_manager_.get(), 
                &HotReload::HotReloadManager::reloadStarted,
                this, &HotReloadApp::onReloadStarted);
                
        connect(hot_reload_manager_.get(), 
                &HotReload::HotReloadManager::reloadCompleted,
                this, &HotReloadApp::onReloadCompleted);
        
        qDebug() << "🔥 Hot reload enabled for ui/editor.json";
        qDebug() << "🔥 Edit the JSON file to see live updates!";
    }
    
private slots:
    void onReloadStarted(const QString& file_path) {
        qDebug() << "🔥 Reloading:" << file_path;
    }
    
    void onReloadCompleted(const QString& file_path) {
        qDebug() << "✅ Reload completed:" << file_path;
        
        // Reload the UI
        try {
            auto new_widget = ui_loader_->loadFromFile(file_path);
            if (new_widget) {
                main_widget_->close();
                main_widget_ = std::move(new_widget);
                main_widget_->show();
                
                // Re-register for hot reload
                hot_reload_manager_->registerUIFile(file_path, main_widget_.get());
            }
        } catch (const std::exception& e) {
            qCritical() << "Failed to reload UI:" << e.what();
        }
    }
    
private:
    std::unique_ptr<JSON::JSONUILoader> ui_loader_;
    std::unique_ptr<QWidget> main_widget_;
    std::unique_ptr<HotReload::HotReloadManager> hot_reload_manager_;
};
```

## Advanced Examples

### Custom Component Creation

```cpp
// Custom component that combines multiple widgets
class SearchBox : public Core::UIElement {
    Q_OBJECT
    
public:
    explicit SearchBox(QObject* parent = nullptr) : UIElement(parent) {}
    
    SearchBox& placeholder(const QString& text) {
        placeholder_text_ = text;
        return *this;
    }
    
    SearchBox& onSearch(std::function<void(const QString&)> handler) {
        search_handler_ = std::move(handler);
        return *this;
    }
    
    void initialize() override {
        auto container = new QWidget();
        auto layout = new QHBoxLayout(container);
        layout->setContentsMargins(0, 0, 0, 0);
        
        // Search input
        search_input_ = new QLineEdit();
        search_input_->setPlaceholderText(placeholder_text_);
        layout->addWidget(search_input_);
        
        // Search button
        search_button_ = new QPushButton("🔍");
        search_button_->setMaximumWidth(40);
        layout->addWidget(search_button_);
        
        // Connect signals
        connect(search_button_, &QPushButton::clicked, this, &SearchBox::performSearch);
        connect(search_input_, &QLineEdit::returnPressed, this, &SearchBox::performSearch);
        
        setWidget(container);
    }
    
private slots:
    void performSearch() {
        if (search_handler_) {
            search_handler_(search_input_->text());
        }
    }
    
private:
    QString placeholder_text_;
    std::function<void(const QString&)> search_handler_;
    QLineEdit* search_input_ = nullptr;
    QPushButton* search_button_ = nullptr;
};
```

These examples demonstrate the key features and patterns in DeclarativeUI. Start with the basic examples and gradually work your way up to more complex scenarios involving state management, JSON UIs, and hot reload functionality.
