/**
 * @file ShowcaseApp.hpp
 * @brief Main showcase application window
 *
 * The ShowcaseApp class provides the main window for the DeclarativeUI showcase
 * application. It demonstrates all framework features through an interactive
 * interface with multiple demonstration panels.
 */

#pragma once

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QMenuBar>
#include <QProgressBar>
#include <QPushButton>
#include <QSplitter>
#include <QStatusBar>
#include <QTabWidget>
#include <QTimer>
#include <QVBoxLayout>
#include <memory>

// DeclarativeUI Core
#include "Binding/StateManager.hpp"
#include "Core/UIElement.hpp"
#include "HotReload/HotReloadManager.hpp"
#include "JSON/JSONUILoader.hpp"

// Showcase Components
#include "AnimationDemo.hpp"
#include "CommandDemo.hpp"
#include "ComponentGallery.hpp"
#include "PerformanceMonitor.hpp"
#include "StateDemo.hpp"
#include "ThemeManager.hpp"

// Enhanced Components
#include "../components/NavigationBar.hpp"
#include "../components/StatusIndicator.hpp"

// Utilities
#include "../utils/ExampleHelpers.hpp"

using namespace DeclarativeUI;

/**
 * @brief Main showcase application window
 *
 * This class provides the main interface for the DeclarativeUI showcase,
 * organizing all demonstrations into a tabbed interface with navigation,
 * theme switching, and performance monitoring.
 */
class ShowcaseApp : public QMainWindow {
    Q_OBJECT

public:
    explicit ShowcaseApp(QWidget* parent = nullptr);
    ~ShowcaseApp() override;

protected:
    void closeEvent(QCloseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void onThemeChanged(const QString& theme);
    void onLanguageChanged(const QString& language);
    void onPerformanceToggled(bool enabled);
    void onHotReloadToggled(bool enabled);
    void onTabChanged(int index);
    void onResetToDefaults();
    void onExportSettings();
    void onImportSettings();
    void onAbout();
    void updatePerformanceStatus();

private:
    // UI Setup
    void setupUI();
    void setupMenuBar();
    void setupStatusBar();
    void setupCentralWidget();
    void setupNavigationBar();
    void setupDemoTabs();

    // Framework Integration
    void setupStateManagement();
    void setupHotReload();
    void setupThemeSystem();
    void setupPerformanceMonitoring();

    // Event Handling
    void connectSignals();
    void loadSettings();
    void saveSettings();
    void applyTheme(const QString& theme);
    void updateWindowTitle();

    // UI Components
    QWidget* central_widget_;
    QVBoxLayout* main_layout_;
    QTabWidget* demo_tabs_;

    // Navigation and Status
    std::unique_ptr<NavigationBar> navigation_bar_;
    std::unique_ptr<StatusIndicator> status_indicator_;
    QLabel* status_label_;
    QProgressBar* performance_bar_;
    QPushButton* theme_button_;
    QComboBox* language_combo_;

    // Demo Panels
    std::unique_ptr<ComponentGallery> component_gallery_;
    std::unique_ptr<StateDemo> state_demo_;
    std::unique_ptr<AnimationDemo> animation_demo_;
    std::unique_ptr<CommandDemo> command_demo_;
    std::unique_ptr<ThemeManager> theme_manager_;
    std::unique_ptr<PerformanceMonitor> performance_monitor_;

    // Framework Components
    std::shared_ptr<Binding::StateManager> state_manager_;
    std::unique_ptr<HotReload::HotReloadManager> hot_reload_manager_;
    std::unique_ptr<JSON::JSONUILoader> ui_loader_;

    // Utilities
    std::unique_ptr<ExampleHelpers> helpers_;
    QTimer* performance_timer_;

    // State
    QString current_theme_;
    QString current_language_;
    bool performance_monitoring_enabled_;
    bool hot_reload_enabled_;
    int startup_time_ms_;
};

/**
 * @brief Demo tab information structure
 */
struct DemoTabInfo {
    QString name;
    QString description;
    QString icon_path;
    QWidget* widget;
    bool enabled;

    DemoTabInfo(const QString& n, const QString& d, const QString& i,
                QWidget* w, bool e = true)
        : name(n), description(d), icon_path(i), widget(w), enabled(e) {}
};
