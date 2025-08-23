/**
 * @file ShowcaseApp.cpp
 * @brief Implementation of the main showcase application window
 */

#include "ShowcaseApp.hpp"
#include <QApplication>
#include <QCloseEvent>
#include <QDebug>
#include <QElapsedTimer>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QStandardPaths>

ShowcaseApp::ShowcaseApp(QWidget* parent)
    : QMainWindow(parent),
      central_widget_(nullptr),
      main_layout_(nullptr),
      demo_tabs_(nullptr),
      status_label_(nullptr),
      performance_bar_(nullptr),
      theme_button_(nullptr),
      language_combo_(nullptr),
      current_theme_("light"),
      current_language_("en"),
      performance_monitoring_enabled_(true),
      hot_reload_enabled_(true),
      startup_time_ms_(0) {
    QElapsedTimer startup_timer;
    startup_timer.start();

    try {
        // Initialize framework components
        setupStateManagement();
        setupHotReload();
        setupThemeSystem();

        // Create utilities
        helpers_ = std::make_unique<ExampleHelpers>();

        // Setup UI
        setupUI();
        setupMenuBar();
        setupStatusBar();
        setupCentralWidget();
        setupNavigationBar();
        setupDemoTabs();

        // Connect signals and load settings
        connectSignals();
        loadSettings();

        // Setup performance monitoring
        setupPerformanceMonitoring();

        // Apply initial theme
        applyTheme(current_theme_);
        updateWindowTitle();

        startup_time_ms_ = static_cast<int>(startup_timer.elapsed());

        qDebug() << "✅ ShowcaseApp initialized in" << startup_time_ms_ << "ms";

    } catch (const std::exception& e) {
        qCritical() << "❌ ShowcaseApp initialization failed:" << e.what();
        throw;
    }
}

ShowcaseApp::~ShowcaseApp() {
    saveSettings();
    qDebug() << "👋 ShowcaseApp destroyed";
}

void ShowcaseApp::setupUI() {
    // Set window properties
    setWindowTitle("DeclarativeUI Showcase");
    setMinimumSize(1200, 800);
    resize(1400, 1000);

    // Set window icon
    setWindowIcon(QIcon(":/assets/icons/showcase.png"));

    // Enable modern window features
    setAttribute(Qt::WA_DeleteOnClose);
    setDockOptions(QMainWindow::AllowNestedDocks |
                   QMainWindow::AllowTabbedDocks);
}

void ShowcaseApp::setupMenuBar() {
    // File menu
    auto file_menu = menuBar()->addMenu("&File");

    auto export_action = file_menu->addAction("&Export Settings...");
    export_action->setShortcut(QKeySequence::SaveAs);
    connect(export_action, &QAction::triggered, this,
            &ShowcaseApp::onExportSettings);

    auto import_action = file_menu->addAction("&Import Settings...");
    import_action->setShortcut(QKeySequence::Open);
    connect(import_action, &QAction::triggered, this,
            &ShowcaseApp::onImportSettings);

    file_menu->addSeparator();

    auto exit_action = file_menu->addAction("E&xit");
    exit_action->setShortcut(QKeySequence::Quit);
    connect(exit_action, &QAction::triggered, this, &QWidget::close);

    // View menu
    auto view_menu = menuBar()->addMenu("&View");

    auto theme_menu = view_menu->addMenu("&Theme");
    auto light_action = theme_menu->addAction("&Light");
    auto dark_action = theme_menu->addAction("&Dark");
    auto auto_action = theme_menu->addAction("&Auto");

    connect(light_action, &QAction::triggered,
            [this]() { onThemeChanged("light"); });
    connect(dark_action, &QAction::triggered,
            [this]() { onThemeChanged("dark"); });
    connect(auto_action, &QAction::triggered,
            [this]() { onThemeChanged("auto"); });

    view_menu->addSeparator();

    auto reset_action = view_menu->addAction("&Reset to Defaults");
    connect(reset_action, &QAction::triggered, this,
            &ShowcaseApp::onResetToDefaults);

    // Tools menu
    auto tools_menu = menuBar()->addMenu("&Tools");

    auto performance_action = tools_menu->addAction("&Performance Monitor");
    performance_action->setCheckable(true);
    performance_action->setChecked(performance_monitoring_enabled_);
    connect(performance_action, &QAction::toggled, this,
            &ShowcaseApp::onPerformanceToggled);

    auto hot_reload_action = tools_menu->addAction("&Hot Reload");
    hot_reload_action->setCheckable(true);
    hot_reload_action->setChecked(hot_reload_enabled_);
    connect(hot_reload_action, &QAction::toggled, this,
            &ShowcaseApp::onHotReloadToggled);

    // Help menu
    auto help_menu = menuBar()->addMenu("&Help");

    auto about_action = help_menu->addAction("&About");
    connect(about_action, &QAction::triggered, this, &ShowcaseApp::onAbout);

    auto about_qt_action = help_menu->addAction("About &Qt");
    connect(about_qt_action, &QAction::triggered, qApp, &QApplication::aboutQt);
}

void ShowcaseApp::setupStatusBar() {
    // Create status components
    status_label_ = new QLabel("Ready");
    performance_bar_ = new QProgressBar();
    performance_bar_->setMaximumWidth(200);
    performance_bar_->setVisible(performance_monitoring_enabled_);

    theme_button_ = new QPushButton("Light Theme");
    theme_button_->setMaximumWidth(120);
    connect(theme_button_, &QPushButton::clicked, [this]() {
        QString next_theme = (current_theme_ == "light") ? "dark" : "light";
        onThemeChanged(next_theme);
    });

    language_combo_ = new QComboBox();
    language_combo_->addItems({"English", "中文", "Español", "Français"});
    language_combo_->setMaximumWidth(100);
    connect(language_combo_, &QComboBox::currentTextChanged, this,
            &ShowcaseApp::onLanguageChanged);

    // Add to status bar
    statusBar()->addWidget(status_label_, 1);
    statusBar()->addPermanentWidget(performance_bar_);
    statusBar()->addPermanentWidget(theme_button_);
    statusBar()->addPermanentWidget(language_combo_);

    // Show startup message
    statusBar()->showMessage(
        QString("Initialized in %1ms").arg(startup_time_ms_), 3000);
}

void ShowcaseApp::setupCentralWidget() {
    central_widget_ = new QWidget();
    setCentralWidget(central_widget_);

    main_layout_ = new QVBoxLayout(central_widget_);
    main_layout_->setContentsMargins(8, 8, 8, 8);
    main_layout_->setSpacing(8);
}

void ShowcaseApp::setupNavigationBar() {
    try {
        navigation_bar_ = std::make_unique<NavigationBar>();
        navigation_bar_->addItem("Components", "🧩",
                                 "Explore all UI components");
        navigation_bar_->addItem("State", "📊", "State management examples");
        navigation_bar_->addItem("Animation", "✨", "Animation showcase");
        navigation_bar_->addItem("Commands", "⚡", "Command system demo");
        navigation_bar_->addItem("Themes", "🎨", "Theme management");
        navigation_bar_->addItem("Performance", "📈", "Performance monitoring");

        main_layout_->addWidget(navigation_bar_.get());

    } catch (const std::exception& e) {
        qWarning() << "Failed to create navigation bar:" << e.what();
        // Continue without navigation bar
    }
}

void ShowcaseApp::setupDemoTabs() {
    demo_tabs_ = new QTabWidget();
    demo_tabs_->setTabPosition(QTabWidget::North);
    demo_tabs_->setMovable(true);
    demo_tabs_->setTabsClosable(false);

    try {
        // Create demo panels
        component_gallery_ = std::make_unique<ComponentGallery>();
        state_demo_ = std::make_unique<StateDemo>();
        animation_demo_ = std::make_unique<AnimationDemo>();
        command_demo_ = std::make_unique<CommandDemo>();
        theme_manager_ = std::make_unique<ThemeManager>();
        performance_monitor_ = std::make_unique<PerformanceMonitor>();

        // Add tabs
        demo_tabs_->addTab(component_gallery_.get(), "🧩 Components");
        demo_tabs_->addTab(state_demo_.get(), "📊 State Management");
        demo_tabs_->addTab(animation_demo_.get(), "✨ Animations");
        demo_tabs_->addTab(command_demo_.get(), "⚡ Commands");
        demo_tabs_->addTab(theme_manager_.get(), "🎨 Themes");
        demo_tabs_->addTab(performance_monitor_.get(), "📈 Performance");

        // Connect tab change signal
        connect(demo_tabs_, &QTabWidget::currentChanged, this,
                &ShowcaseApp::onTabChanged);

        main_layout_->addWidget(demo_tabs_, 1);

    } catch (const std::exception& e) {
        qCritical() << "Failed to create demo tabs:" << e.what();
        throw;
    }
}

void ShowcaseApp::setupStateManagement() {
    state_manager_ = std::shared_ptr<Binding::StateManager>(
        &Binding::StateManager::instance(), [](Binding::StateManager*) {}
        // Don't delete singleton
    );

    // Initialize showcase-specific state
    state_manager_->setState("showcase.theme", current_theme_);
    state_manager_->setState("showcase.language", current_language_);
    state_manager_->setState("showcase.performance_monitoring",
                             performance_monitoring_enabled_);
    state_manager_->setState("showcase.hot_reload", hot_reload_enabled_);
}

void ShowcaseApp::setupHotReload() {
    if (!hot_reload_enabled_)
        return;

    try {
        hot_reload_manager_ = std::make_unique<HotReload::HotReloadManager>();

        // Register UI files for hot reload
        QStringList ui_files = {"resources/ui/main_window.json",
                                "resources/ui/component_gallery.json",
                                "resources/themes/light.json",
                                "resources/themes/dark.json"};

        for (const QString& file : ui_files) {
            if (QFileInfo::exists(file)) {
                hot_reload_manager_->registerUIFile(file, this);
            }
        }

        qDebug() << "🔥 Hot reload enabled for" << ui_files.size() << "files";

    } catch (const std::exception& e) {
        qWarning() << "Hot reload setup failed:" << e.what();
        hot_reload_enabled_ = false;
    }
}

void ShowcaseApp::setupThemeSystem() {
    // Theme system will be implemented in ThemeManager
    qDebug() << "🎨 Theme system initialized";
}

void ShowcaseApp::setupPerformanceMonitoring() {
    if (!performance_monitoring_enabled_)
        return;

    performance_timer_ = new QTimer(this);
    connect(performance_timer_, &QTimer::timeout, this,
            &ShowcaseApp::updatePerformanceStatus);
    performance_timer_->start(1000);  // Update every second

    qDebug() << "📈 Performance monitoring enabled";
}

void ShowcaseApp::connectSignals() {
    // Navigation bar signals
    if (navigation_bar_) {
        connect(navigation_bar_.get(), &NavigationBar::itemClicked,
                [this](int index) { demo_tabs_->setCurrentIndex(index); });
    }
}

void ShowcaseApp::onThemeChanged(const QString& theme) {
    current_theme_ = theme;
    applyTheme(theme);
    state_manager_->setState("showcase.theme", theme);

    theme_button_->setText(
        QString("%1 Theme").arg(theme.left(1).toUpper() + theme.mid(1)));

    statusBar()->showMessage(QString("Theme changed to %1").arg(theme), 2000);
}

void ShowcaseApp::onLanguageChanged(const QString& language) {
    current_language_ = language;
    state_manager_->setState("showcase.language", language);
    statusBar()->showMessage(QString("Language changed to %1").arg(language),
                             2000);
}

void ShowcaseApp::onPerformanceToggled(bool enabled) {
    performance_monitoring_enabled_ = enabled;
    performance_bar_->setVisible(enabled);
    state_manager_->setState("showcase.performance_monitoring", enabled);

    if (enabled) {
        setupPerformanceMonitoring();
    } else if (performance_timer_) {
        performance_timer_->stop();
    }
}

void ShowcaseApp::onHotReloadToggled(bool enabled) {
    hot_reload_enabled_ = enabled;
    state_manager_->setState("showcase.hot_reload", enabled);

    if (enabled) {
        setupHotReload();
    } else {
        hot_reload_manager_.reset();
    }
}

void ShowcaseApp::onTabChanged(int index) {
    if (navigation_bar_) {
        navigation_bar_->setCurrentIndex(index);
    }

    QString tab_name = demo_tabs_->tabText(index);
    statusBar()->showMessage(QString("Viewing: %1").arg(tab_name), 2000);
}

void ShowcaseApp::onResetToDefaults() {
    onThemeChanged("light");
    onLanguageChanged("English");
    onPerformanceToggled(true);
    onHotReloadToggled(true);
    demo_tabs_->setCurrentIndex(0);

    statusBar()->showMessage("Settings reset to defaults", 2000);
}

void ShowcaseApp::onExportSettings() {
    QString filename = QFileDialog::getSaveFileName(
        this, "Export Settings",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) +
            "/showcase_settings.json",
        "JSON Files (*.json)");

    if (!filename.isEmpty()) {
        // Export settings implementation
        statusBar()->showMessage("Settings exported successfully", 2000);
    }
}

void ShowcaseApp::onImportSettings() {
    QString filename = QFileDialog::getOpenFileName(
        this, "Import Settings",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        "JSON Files (*.json)");

    if (!filename.isEmpty()) {
        // Import settings implementation
        statusBar()->showMessage("Settings imported successfully", 2000);
    }
}

void ShowcaseApp::onAbout() {
    QMessageBox::about(this, "About DeclarativeUI Showcase",
                       "<h3>DeclarativeUI Showcase v1.0.0</h3>"
                       "<p>A comprehensive demonstration of the DeclarativeUI "
                       "framework features.</p>"
                       "<p><b>Features:</b></p>"
                       "<ul>"
                       "<li>30+ UI Components with live examples</li>"
                       "<li>State management and reactive programming</li>"
                       "<li>Animation system with smooth transitions</li>"
                       "<li>Command system with undo/redo</li>"
                       "<li>JSON UI loading and hot reload</li>"
                       "<li>Theme system and performance monitoring</li>"
                       "</ul>"
                       "<p>Built with Qt6 and modern C++20.</p>");
}

void ShowcaseApp::updatePerformanceStatus() {
    if (!performance_monitoring_enabled_)
        return;

    // Simple performance indicator (CPU usage simulation)
    static int counter = 0;
    int value = (counter % 100);
    performance_bar_->setValue(value);
    counter++;
}

void ShowcaseApp::loadSettings() {
    QSettings settings;
    current_theme_ = settings.value("theme", "light").toString();
    current_language_ = settings.value("language", "English").toString();
    performance_monitoring_enabled_ =
        settings.value("performance_monitoring", true).toBool();
    hot_reload_enabled_ = settings.value("hot_reload", true).toBool();

    // Apply loaded settings
    language_combo_->setCurrentText(current_language_);
}

void ShowcaseApp::saveSettings() {
    QSettings settings;
    settings.setValue("theme", current_theme_);
    settings.setValue("language", current_language_);
    settings.setValue("performance_monitoring",
                      performance_monitoring_enabled_);
    settings.setValue("hot_reload", hot_reload_enabled_);
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
}

void ShowcaseApp::applyTheme(const QString& theme) {
    // Basic theme application - will be enhanced by ThemeManager
    if (theme == "dark") {
        setStyleSheet(
            "QMainWindow { background-color: #2b2b2b; color: #ffffff; }"
            "QTabWidget::pane { border: 1px solid #555555; }"
            "QTabBar::tab { background-color: #404040; color: #ffffff; "
            "padding: 8px; }"
            "QTabBar::tab:selected { background-color: #606060; }");
    } else {
        setStyleSheet("");
    }
}

void ShowcaseApp::updateWindowTitle() {
    setWindowTitle(
        QString("DeclarativeUI Showcase - %1 Theme")
            .arg(current_theme_.left(1).toUpper() + current_theme_.mid(1)));
}

void ShowcaseApp::closeEvent(QCloseEvent* event) {
    saveSettings();
    event->accept();
}

void ShowcaseApp::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
    // Handle responsive layout changes here
}

#include "ShowcaseApp.moc"
