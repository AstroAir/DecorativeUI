/**
 * @file 16_reactive_state.cpp
 * @brief Advanced reactive state management patterns
 * 
 * This example demonstrates:
 * - Complex reactive state patterns
 * - State dependencies and computed properties
 * - Batch state updates
 * - State change notifications and observers
 * - Performance optimization techniques
 * 
 * Learning objectives:
 * - Master advanced reactive programming patterns
 * - Understand state dependency management
 * - Learn performance optimization for state updates
 * - See complex UI synchronization patterns
 */

#include <QApplication>
#include <QObject>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QWidget>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>
#include <QProgressBar>
#include <QTextEdit>
#include <QCheckBox>
#include <QTimer>
#include <QDebug>
#include <QDateTime>
#include <QRandomGenerator>
#include <QFileInfo>
#include <QPropertyAnimation>
#include <QEasingCurve>

// Include DeclarativeUI state management
#include "Binding/StateManager.hpp"
#include "JSON/JSONUILoader.hpp"

using namespace DeclarativeUI;

/**
 * @brief Advanced reactive state demonstration
 */
class ReactiveStateApp : public QObject {
    Q_OBJECT

public:
    ReactiveStateApp(QObject* parent = nullptr) : QObject(parent) {
        setupAdvancedState();
        setupUILoader();
        createUI();
        setupAnimations();
        startSimulation();
    }

    void show() {
        if (main_widget_) {
            main_widget_->show();
        }
    }

private slots:
    void onTemperatureChanged(int value) {
        temperature_state_->set(value);
        logStateChange("Temperature", QString("%1°C").arg(value));
    }

    void onHumidityChanged(int value) {
        humidity_state_->set(value);
        logStateChange("Humidity", QString("%1%").arg(value));
    }

    void onPressureChanged(int value) {
        pressure_state_->set(value);
        logStateChange("Pressure", QString("%1 hPa").arg(value));
    }

    void onAutoModeToggled(bool enabled) {
        auto_mode_state_->set(enabled);
        logStateChange("Auto Mode", enabled ? "Enabled" : "Disabled");
        
        if (enabled) {
            simulation_timer_->start();
        } else {
            simulation_timer_->stop();
        }
    }

    void onBatchUpdateClicked() {
        // Demonstrate batch state updates for performance
        state_manager_->batchUpdate([this]() {
            temperature_state_->set(QRandomGenerator::global()->bounded(15, 35));
            humidity_state_->set(QRandomGenerator::global()->bounded(30, 80));
            pressure_state_->set(QRandomGenerator::global()->bounded(980, 1030));
        });
        
        logStateChange("System", "Batch update completed");
    }

    void onResetClicked() {
        state_manager_->batchUpdate([this]() {
            temperature_state_->set(20);
            humidity_state_->set(50);
            pressure_state_->set(1013);
            auto_mode_state_->set(false);
        });
        
        logStateChange("System", "All values reset to defaults");
    }

    void onClearLogClicked() {
        auto log_display = main_widget_->findChild<QTextEdit*>("stateLog");
        if (log_display) {
            log_display->clear();
            logStateChange("System", "State log cleared");
        }
    }

    void simulateEnvironmentChanges() {
        if (!auto_mode_state_->get()) return;
        
        // Simulate realistic environmental changes
        int current_temp = temperature_state_->get();
        int current_humidity = humidity_state_->get();
        int current_pressure = pressure_state_->get();
        
        // Small random changes
        int temp_change = QRandomGenerator::global()->bounded(-2, 3);
        int humidity_change = QRandomGenerator::global()->bounded(-5, 6);
        int pressure_change = QRandomGenerator::global()->bounded(-3, 4);
        
        // Apply constraints
        int new_temp = qBound(10, current_temp + temp_change, 40);
        int new_humidity = qBound(20, current_humidity + humidity_change, 90);
        int new_pressure = qBound(950, current_pressure + pressure_change, 1050);
        
        // Batch update for performance
        state_manager_->batchUpdate([=]() {
            temperature_state_->set(new_temp);
            humidity_state_->set(new_humidity);
            pressure_state_->set(new_pressure);
        });
    }

private:
    std::unique_ptr<QWidget> main_widget_;
    std::unique_ptr<JSON::JSONUILoader> ui_loader_;
    QTimer* simulation_timer_;
    
    // State management
    std::shared_ptr<Binding::StateManager> state_manager_;
    std::shared_ptr<Binding::ReactiveProperty<int>> temperature_state_;
    std::shared_ptr<Binding::ReactiveProperty<int>> humidity_state_;
    std::shared_ptr<Binding::ReactiveProperty<int>> pressure_state_;
    std::shared_ptr<Binding::ReactiveProperty<bool>> auto_mode_state_;

    // Computed states (using ReactiveProperty with computed values)
    std::shared_ptr<Binding::ReactiveProperty<QString>> comfort_level_state_;
    std::shared_ptr<Binding::ReactiveProperty<QString>> weather_status_state_;
    std::shared_ptr<Binding::ReactiveProperty<int>> comfort_score_state_;
    std::shared_ptr<Binding::ReactiveProperty<QString>> alert_message_state_;

    void setupAdvancedState() {
        state_manager_ = std::shared_ptr<Binding::StateManager>(
            &Binding::StateManager::instance(), [](Binding::StateManager*) {});

        // Enable performance monitoring
        state_manager_->enablePerformanceMonitoring(true);
        state_manager_->enableDebugMode(true);

        // Create base states
        temperature_state_ = state_manager_->createState<int>("temperature", 20);
        humidity_state_ = state_manager_->createState<int>("humidity", 50);
        pressure_state_ = state_manager_->createState<int>("pressure", 1013);
        auto_mode_state_ = state_manager_->createState<bool>("auto_mode", false);

        // Create computed states with complex dependencies
        comfort_level_state_ = state_manager_->createComputed<QString>(
            "comfort_level", [=]() {
                int temp = temperature_state_->get();
                int humidity = humidity_state_->get();
                
                if (temp >= 18 && temp <= 24 && humidity >= 40 && humidity <= 60) {
                    return QString("😊 Comfortable");
                } else if (temp < 15 || temp > 30) {
                    return QString("🥶 Too %1").arg(temp < 15 ? "Cold" : "Hot");
                } else if (humidity < 30 || humidity > 70) {
                    return QString("💧 Too %1").arg(humidity < 30 ? "Dry" : "Humid");
                } else {
                    return QString("😐 Acceptable");
                }
            });

        weather_status_state_ = state_manager_->createComputed<QString>(
            "weather_status", [=]() {
                int pressure = pressure_state_->get();
                int temp = temperature_state_->get();
                int humidity = humidity_state_->get();
                
                if (pressure < 1000 && humidity > 70) {
                    return QString("🌧️ Rainy");
                } else if (pressure > 1020 && temp > 25) {
                    return QString("☀️ Sunny");
                } else if (temp < 10) {
                    return QString("❄️ Cold");
                } else if (humidity > 80) {
                    return QString("🌫️ Foggy");
                } else {
                    return QString("⛅ Partly Cloudy");
                }
            });

        comfort_score_state_ = state_manager_->createComputed<int>(
            "comfort_score", [=]() {
                int temp = temperature_state_->get();
                int humidity = humidity_state_->get();
                int pressure = pressure_state_->get();
                
                // Calculate comfort score (0-100)
                int temp_score = 100 - qAbs(temp - 22) * 5;
                int humidity_score = 100 - qAbs(humidity - 50) * 2;
                int pressure_score = 100 - qAbs(pressure - 1013) / 2;
                
                return qBound(0, (temp_score + humidity_score + pressure_score) / 3, 100);
            });

        alert_message_state_ = state_manager_->createComputed<QString>(
            "alert_message", [=]() {
                int temp = temperature_state_->get();
                int humidity = humidity_state_->get();
                int pressure = pressure_state_->get();
                
                QStringList alerts;
                
                if (temp < 10) alerts << "⚠️ Freezing temperature";
                if (temp > 35) alerts << "🔥 Extreme heat";
                if (humidity < 20) alerts << "🏜️ Very dry air";
                if (humidity > 85) alerts << "💦 Very humid";
                if (pressure < 980) alerts << "📉 Low pressure system";
                if (pressure > 1040) alerts << "📈 High pressure system";
                
                return alerts.isEmpty() ? "✅ All systems normal" : alerts.join(" | ");
            });

        // Set up state dependencies for optimization
        state_manager_->addDependency("comfort_level", "temperature");
        state_manager_->addDependency("comfort_level", "humidity");
        state_manager_->addDependency("weather_status", "pressure");
        state_manager_->addDependency("weather_status", "temperature");
        state_manager_->addDependency("weather_status", "humidity");

        // Connect state change notifications
        connect(temperature_state_.get(), &Binding::ReactivePropertyBase::valueChanged,
                this, [this]() { updateTemperatureUI(temperature_state_->get()); });

        connect(humidity_state_.get(), &Binding::ReactivePropertyBase::valueChanged,
                this, [this]() { updateHumidityUI(humidity_state_->get()); });

        connect(pressure_state_.get(), &Binding::ReactivePropertyBase::valueChanged,
                this, [this]() { updatePressureUI(pressure_state_->get()); });

        qDebug() << "✅ Advanced reactive state system initialized";
    }

    void setupUILoader() {
        ui_loader_ = std::make_unique<JSON::JSONUILoader>();

        // Register event handlers
        ui_loader_->registerEventHandler("batchUpdate", [this]() {
            onBatchUpdateClicked();
        });

        ui_loader_->registerEventHandler("resetValues", [this]() {
            onResetClicked();
        });

        ui_loader_->registerEventHandler("clearLog", [this]() {
            onClearLogClicked();
        });

        qDebug() << "✅ Event handlers registered";
    }

    void createUI() {
        try {
            QString ui_file = "resources/reactive_state_ui.json";
            
            if (QFileInfo::exists(ui_file)) {
                main_widget_ = ui_loader_->loadFromFile(ui_file);
                if (main_widget_) {
                    main_widget_->setWindowTitle("16 - Reactive State | DeclarativeUI");
                    connectUIEvents();
                    updateAllDisplays();
                    qDebug() << "✅ UI loaded from JSON";
                    return;
                }
            }
            
            // Fallback to programmatic UI
            main_widget_ = createProgrammaticUI();
            
        } catch (const std::exception& e) {
            qCritical() << "UI creation failed:" << e.what();
            main_widget_ = createProgrammaticUI();
        }
    }

    void connectUIEvents() {
        // Connect sliders
        auto temp_slider = main_widget_->findChild<QSlider*>("tempSlider");
        if (temp_slider) {
            connect(temp_slider, &QSlider::valueChanged, this, &ReactiveStateApp::onTemperatureChanged);
        }

        auto humidity_slider = main_widget_->findChild<QSlider*>("humiditySlider");
        if (humidity_slider) {
            connect(humidity_slider, &QSlider::valueChanged, this, &ReactiveStateApp::onHumidityChanged);
        }

        auto pressure_slider = main_widget_->findChild<QSlider*>("pressureSlider");
        if (pressure_slider) {
            connect(pressure_slider, &QSlider::valueChanged, this, &ReactiveStateApp::onPressureChanged);
        }

        // Connect auto mode checkbox
        auto auto_checkbox = main_widget_->findChild<QCheckBox*>("autoMode");
        if (auto_checkbox) {
            connect(auto_checkbox, &QCheckBox::toggled, this, &ReactiveStateApp::onAutoModeToggled);
        }

        qDebug() << "✅ UI events connected";
    }

    std::unique_ptr<QWidget> createProgrammaticUI() {
        auto widget = std::make_unique<QWidget>();
        widget->setWindowTitle("16 - Reactive State (Fallback) | DeclarativeUI");
        widget->setMinimumSize(800, 700);

        auto layout = new QVBoxLayout(widget.get());
        layout->setSpacing(15);
        layout->setContentsMargins(20, 20, 20, 20);

        // Header
        auto header = new QLabel("🔄 Advanced Reactive State Management");
        header->setStyleSheet("font-size: 20px; font-weight: bold; color: #2c3e50;");
        header->setAlignment(Qt::AlignCenter);
        layout->addWidget(header);

        // Environmental controls would be added here...
        // (Implementation continues...)

        qDebug() << "✅ Programmatic UI created";
        return widget;
    }

    void setupAnimations() {
        // Set up smooth animations for UI updates
        // This would include property animations for smooth transitions
    }

    void startSimulation() {
        simulation_timer_ = new QTimer(this);
        connect(simulation_timer_, &QTimer::timeout, this, &ReactiveStateApp::simulateEnvironmentChanges);
        simulation_timer_->setInterval(2000); // Update every 2 seconds
    }

    void updateTemperatureUI(int value) {
        auto temp_display = main_widget_->findChild<QLabel*>("tempDisplay");
        if (temp_display) {
            temp_display->setText(QString("%1°C").arg(value));
        }
        updateComputedDisplays();
    }

    void updateHumidityUI(int value) {
        auto humidity_display = main_widget_->findChild<QLabel*>("humidityDisplay");
        if (humidity_display) {
            humidity_display->setText(QString("%1%").arg(value));
        }
        updateComputedDisplays();
    }

    void updatePressureUI(int value) {
        auto pressure_display = main_widget_->findChild<QLabel*>("pressureDisplay");
        if (pressure_display) {
            pressure_display->setText(QString("%1 hPa").arg(value));
        }
        updateComputedDisplays();
    }

    void updateComputedDisplays() {
        // Update computed state displays
        auto comfort_display = main_widget_->findChild<QLabel*>("comfortDisplay");
        if (comfort_display) {
            comfort_display->setText(comfort_level_state_->get());
        }

        auto weather_display = main_widget_->findChild<QLabel*>("weatherDisplay");
        if (weather_display) {
            weather_display->setText(weather_status_state_->get());
        }

        auto score_progress = main_widget_->findChild<QProgressBar*>("comfortProgress");
        if (score_progress) {
            score_progress->setValue(comfort_score_state_->get());
        }

        auto alert_display = main_widget_->findChild<QLabel*>("alertDisplay");
        if (alert_display) {
            alert_display->setText(alert_message_state_->get());
        }
    }

    void updateAllDisplays() {
        updateTemperatureUI(temperature_state_->get());
        updateHumidityUI(humidity_state_->get());
        updatePressureUI(pressure_state_->get());
        updateComputedDisplays();
    }

    void logStateChange(const QString& component, const QString& details) {
        auto log_display = main_widget_->findChild<QTextEdit*>("stateLog");
        if (log_display) {
            QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
            QString log_entry = QString("[%1] %2: %3").arg(timestamp, component, details);
            log_display->append(log_entry);
            
            // Auto-scroll to bottom
            auto cursor = log_display->textCursor();
            cursor.movePosition(QTextCursor::End);
            log_display->setTextCursor(cursor);
        }
        
        qDebug() << "State Change:" << component << details;
    }
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    app.setApplicationName("DeclarativeUI Reactive State");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("DeclarativeUI Examples");

    try {
        qDebug() << "🚀 Starting Reactive State example...";
        
        ReactiveStateApp reactive_app;
        reactive_app.show();

        qDebug() << "💡 This example demonstrates:";
        qDebug() << "   - Advanced reactive state patterns";
        qDebug() << "   - Complex state dependencies";
        qDebug() << "   - Computed state properties";
        qDebug() << "   - Batch state updates";
        qDebug() << "   - Performance monitoring";

        return app.exec();

    } catch (const std::exception& e) {
        qCritical() << "❌ Application error:" << e.what();
        return -1;
    }
}

#include "16_reactive_state.moc"
