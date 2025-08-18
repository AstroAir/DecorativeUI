#include "AdvancedComponentsShowcase.hpp"

#include <QApplication>
#include <QHeaderView>
#include <QSplitter>
#include <QTimer>
#include <QClipboard>
#include <QMessageBox>
#include <QDebug>

AdvancedComponentsShowcase::AdvancedComponentsShowcase(QWidget* parent)
    : QMainWindow(parent)
    , main_tabs_(nullptr)
    , info_panel_(nullptr)
    , component_tree_(nullptr)
    , feature_list_(nullptr)
    , status_label_(nullptr)
    , demo_progress_(nullptr)
    , reset_button_(nullptr)
    , export_button_(nullptr)
    , sample_table_model_(nullptr)
{
    setWindowTitle("DeclarativeUI - Advanced Components Showcase");
    setMinimumSize(1200, 800);
    resize(1400, 900);

    setupSampleData();
    setupUI();
    setupMenuBar();
    setupToolBar();
    setupStatusBar();
    setupDockWidgets();

    qDebug() << "🎪 Advanced Components Showcase initialized";
}

void AdvancedComponentsShowcase::setupUI() {
    // Create central widget with main tabs
    auto* central_widget = new QWidget();
    setCentralWidget(central_widget);

    auto* main_layout = new QVBoxLayout(central_widget);
    main_layout->setContentsMargins(10, 10, 10, 10);
    main_layout->setSpacing(10);

    // Create main tabs
    main_tabs_ = new QTabWidget();
    main_tabs_->setTabPosition(QTabWidget::North);
    main_tabs_->setMovable(true);
    main_tabs_->setTabsClosable(false);

    // Add demo tabs
    addDemoTab("🔍 SearchBox", ":/icons/search.png", createSearchBoxDemo());
    addDemoTab("📊 DataTable", ":/icons/table.png", createDataTableDemo());
    addDemoTab("🎛️ PropertyEditor", ":/icons/properties.png", createPropertyEditorDemo());
    addDemoTab("📋 FormBuilder", ":/icons/form.png", createFormBuilderDemo());
    addDemoTab("🗂️ FileExplorer", ":/icons/folder.png", createFileExplorerDemo());
    addDemoTab("📈 ChartWidget", ":/icons/chart.png", createChartWidgetDemo());
    addDemoTab("🎨 ColorPicker", ":/icons/color.png", createColorPickerDemo());
    addDemoTab("📅 DateTimePicker", ":/icons/calendar.png", createDateTimePickerDemo());
    addDemoTab("🔗 Integration", ":/icons/integration.png", createIntegrationDemo());
    addDemoTab("⚡ Performance", ":/icons/performance.png", createPerformanceDemo());

    main_layout->addWidget(main_tabs_);

    // Connect tab changes
    connect(main_tabs_, &QTabWidget::currentChanged, [this](int index) {
        QString tab_name = main_tabs_->tabText(index);
        status_label_->setText(QString("Viewing: %1").arg(tab_name));
        qDebug() << "📑 Switched to tab:" << tab_name;
    });
}

void AdvancedComponentsShowcase::setupMenuBar() {
    // File menu
    auto* file_menu = menuBar()->addMenu("&File");

    auto* new_action = file_menu->addAction("&New Demo");
    new_action->setShortcut(QKeySequence::New);
    connect(new_action, &QAction::triggered, [this]() {
        qDebug() << "📄 New demo requested";
    });

    auto* export_action = file_menu->addAction("&Export Demo");
    export_action->setShortcut(QKeySequence("Ctrl+E"));
    connect(export_action, &QAction::triggered, [this]() {
        qDebug() << "💾 Export demo requested";
    });

    file_menu->addSeparator();

    auto* exit_action = file_menu->addAction("E&xit");
    exit_action->setShortcut(QKeySequence::Quit);
    connect(exit_action, &QAction::triggered, this, &QWidget::close);

    // Components menu
    auto* components_menu = menuBar()->addMenu("&Components");

    components_menu->addAction("🔍 SearchBox Demo", this, &AdvancedComponentsShowcase::onSearchBoxDemo);
    components_menu->addAction("📊 DataTable Demo", this, &AdvancedComponentsShowcase::onDataTableDemo);
    components_menu->addAction("🎛️ PropertyEditor Demo", this, &AdvancedComponentsShowcase::onPropertyEditorDemo);
    components_menu->addAction("📋 FormBuilder Demo", this, &AdvancedComponentsShowcase::onFormBuilderDemo);
    components_menu->addAction("🗂️ FileExplorer Demo", this, &AdvancedComponentsShowcase::onFileExplorerDemo);
    components_menu->addAction("📈 ChartWidget Demo", this, &AdvancedComponentsShowcase::onChartWidgetDemo);
    components_menu->addAction("🎨 ColorPicker Demo", this, &AdvancedComponentsShowcase::onColorPickerDemo);
    components_menu->addAction("📅 DateTimePicker Demo", this, &AdvancedComponentsShowcase::onDateTimePickerDemo);

    components_menu->addSeparator();
    components_menu->addAction("🔗 Integration Demo", this, &AdvancedComponentsShowcase::onIntegrationDemo);
    components_menu->addAction("⚡ Performance Demo", this, &AdvancedComponentsShowcase::onPerformanceDemo);

    // View menu
    auto* view_menu = menuBar()->addMenu("&View");

    auto* fullscreen_action = view_menu->addAction("&Fullscreen");
    fullscreen_action->setShortcut(QKeySequence::FullScreen);
    fullscreen_action->setCheckable(true);
    connect(fullscreen_action, &QAction::toggled, [this](bool checked) {
        if (checked) {
            showFullScreen();
        } else {
            showNormal();
        }
    });

    // Help menu
    auto* help_menu = menuBar()->addMenu("&Help");

    auto* about_action = help_menu->addAction("&About");
    connect(about_action, &QAction::triggered, [this]() {
        QMessageBox::about(this, "About Advanced Components Showcase",
            "DeclarativeUI Advanced Components Showcase\n\n"
            "This application demonstrates the sophisticated composite components "
            "available in the DeclarativeUI framework.\n\n"
            "Features:\n"
            "• SearchBox with auto-completion\n"
            "• DataTable with sorting and filtering\n"
            "• PropertyEditor for dynamic editing\n"
            "• FormBuilder for dynamic forms\n"
            "• FileExplorer for file system browsing\n"
            "• ChartWidget for data visualization\n"
            "• ColorPicker for color selection\n"
            "• DateTimePicker for date/time input\n"
            "• Integration examples\n"
            "• Performance monitoring\n\n"
            "Built with DeclarativeUI Framework");
    });
}

void AdvancedComponentsShowcase::setupToolBar() {
    auto* toolbar = addToolBar("Main");
    toolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    // Reset button
    reset_button_ = new QPushButton("Reset Demos");
    reset_button_->setIcon(QIcon(":/icons/reset.png"));
    reset_button_->setToolTip("Reset all demos to initial state");
    toolbar->addWidget(reset_button_);
    connect(reset_button_, &QPushButton::clicked, [this]() {
        qDebug() << "🔄 Resetting all demos";
        // Reset logic would go here
    });

    toolbar->addSeparator();

    // Export button
    export_button_ = new QPushButton("Export");
    export_button_->setIcon(QIcon(":/icons/export.png"));
    export_button_->setToolTip("Export current demo configuration");
    toolbar->addWidget(export_button_);
    connect(export_button_, &QPushButton::clicked, [this]() {
        qDebug() << "💾 Exporting demo configuration";
        // Export logic would go here
    });

    toolbar->addSeparator();

    // Progress bar for demo operations
    demo_progress_ = new QProgressBar();
    demo_progress_->setVisible(false);
    demo_progress_->setMaximumWidth(200);
    toolbar->addWidget(demo_progress_);
}

void AdvancedComponentsShowcase::setupStatusBar() {
    status_label_ = new QLabel("Ready - Select a component demo to begin");
    statusBar()->addWidget(status_label_);

    // Add permanent widgets to status bar
    auto* memory_label = new QLabel("Memory: 0 MB");
    statusBar()->addPermanentWidget(memory_label);

    auto* components_label = new QLabel("Components: 8");
    statusBar()->addPermanentWidget(components_label);
}

void AdvancedComponentsShowcase::setupDockWidgets() {
    // Component tree dock
    auto* tree_dock = new QDockWidget("Components", this);
    tree_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    component_tree_ = new QTreeWidget();
    component_tree_->setHeaderLabel("Available Components");

    // Populate component tree
    auto* search_item = new QTreeWidgetItem(component_tree_, {"🔍 SearchBox"});
    search_item->addChild(new QTreeWidgetItem({"Auto-completion"}));
    search_item->addChild(new QTreeWidgetItem({"Search history"}));
    search_item->addChild(new QTreeWidgetItem({"Fuzzy matching"}));

    auto* table_item = new QTreeWidgetItem(component_tree_, {"📊 DataTable"});
    table_item->addChild(new QTreeWidgetItem({"Sorting"}));
    table_item->addChild(new QTreeWidgetItem({"Filtering"}));
    table_item->addChild(new QTreeWidgetItem({"Editing"}));
    table_item->addChild(new QTreeWidgetItem({"Export"}));

    // Add more component items...

    tree_dock->setWidget(component_tree_);
    addDockWidget(Qt::LeftDockWidgetArea, tree_dock);

    // Info panel dock
    auto* info_dock = new QDockWidget("Information", this);
    info_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    info_panel_ = new QTextEdit();
    info_panel_->setReadOnly(true);
    info_panel_->setHtml(R"(
        <h3>Welcome to Advanced Components Showcase</h3>
        <p>This application demonstrates the sophisticated composite components
        available in the DeclarativeUI framework.</p>

        <h4>Getting Started:</h4>
        <ol>
            <li>Select a component tab to explore its features</li>
            <li>Interact with the demo to see the component in action</li>
            <li>Check the component tree for available features</li>
            <li>View the integration demo to see components working together</li>
        </ol>

        <h4>Features:</h4>
        <ul>
            <li><strong>SearchBox:</strong> Advanced search with auto-completion</li>
            <li><strong>DataTable:</strong> Powerful data grid with sorting and filtering</li>
            <li><strong>PropertyEditor:</strong> Dynamic property editing</li>
            <li><strong>FormBuilder:</strong> Dynamic form generation</li>
            <li><strong>And more...</strong></li>
        </ul>
    )");

    info_dock->setWidget(info_panel_);
    addDockWidget(Qt::RightDockWidgetArea, info_dock);

    // Connect tree selection to info updates
    connect(component_tree_, &QTreeWidget::itemClicked, [this](QTreeWidgetItem* item) {
        QString component_name = item->text(0);
        showComponentInfo(component_name, "Component information", {});
    });
}

void AdvancedComponentsShowcase::setupSampleData() {
    // Sample search data
    sample_search_data_ = {
        "Apple iPhone 15 Pro",
        "Samsung Galaxy S24",
        "Google Pixel 8",
        "OnePlus 12",
        "Xiaomi 14 Pro",
        "Sony Xperia 1 V",
        "Nothing Phone 2",
        "Fairphone 5",
        "MacBook Pro M3",
        "Dell XPS 13",
        "ThinkPad X1 Carbon",
        "Surface Laptop 5",
        "iPad Pro 12.9",
        "Samsung Galaxy Tab S9",
        "Microsoft Surface Pro 9"
    };

    // Sample table model
    sample_table_model_ = new SampleDataModel(this);
    qobject_cast<SampleDataModel*>(sample_table_model_)->addSampleData();
}

void AdvancedComponentsShowcase::addDemoTab(const QString& title, const QString& icon_path, QWidget* content) {
    QIcon icon(icon_path);
    if (icon.isNull()) {
        // Fallback to text-based icon
        main_tabs_->addTab(content, title);
    } else {
        main_tabs_->addTab(content, icon, title);
    }
}

QWidget* AdvancedComponentsShowcase::createDemoSection(const QString& title, const QString& description, QWidget* demo_widget) {
    auto* section = new QWidget();
    auto* layout = new QVBoxLayout(section);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(15);

    // Title
    auto* title_label = new QLabel(title);
    title_label->setStyleSheet("font-size: 18px; font-weight: bold; color: #2c3e50; margin-bottom: 10px;");
    layout->addWidget(title_label);

    // Description
    auto* desc_label = new QLabel(description);
    desc_label->setStyleSheet("color: #666; margin-bottom: 20px;");
    desc_label->setWordWrap(true);
    layout->addWidget(desc_label);

    // Demo widget
    if (demo_widget) {
        layout->addWidget(demo_widget);
    }

    layout->addStretch();
    return section;
}

void AdvancedComponentsShowcase::showComponentInfo(const QString& component_name, const QString& description, const QStringList& features) {
    if (!info_panel_) return;

    QString html = QString(R"(
        <h3>%1</h3>
        <p>%2</p>
        <h4>Features:</h4>
        <ul>
    )").arg(component_name, description);

    for (const QString& feature : features) {
        html += QString("<li>%1</li>").arg(feature);
    }

    html += "</ul>";
    info_panel_->setHtml(html);
}

// **Slot Implementations**
void AdvancedComponentsShowcase::onSearchBoxDemo() {
    main_tabs_->setCurrentIndex(0);
    status_label_->setText("Demonstrating SearchBox component");
}

void AdvancedComponentsShowcase::onDataTableDemo() {
    main_tabs_->setCurrentIndex(1);
    status_label_->setText("Demonstrating DataTable component");
}

void AdvancedComponentsShowcase::onPropertyEditorDemo() {
    main_tabs_->setCurrentIndex(2);
    status_label_->setText("Demonstrating PropertyEditor component");
}

void AdvancedComponentsShowcase::onFormBuilderDemo() {
    main_tabs_->setCurrentIndex(3);
    status_label_->setText("Demonstrating FormBuilder component");
}

void AdvancedComponentsShowcase::onFileExplorerDemo() {
    main_tabs_->setCurrentIndex(4);
    status_label_->setText("Demonstrating FileExplorer component");
}

void AdvancedComponentsShowcase::onChartWidgetDemo() {
    main_tabs_->setCurrentIndex(5);
    status_label_->setText("Demonstrating ChartWidget component");
}

void AdvancedComponentsShowcase::onColorPickerDemo() {
    main_tabs_->setCurrentIndex(6);
    status_label_->setText("Demonstrating ColorPicker component");
}

void AdvancedComponentsShowcase::onDateTimePickerDemo() {
    main_tabs_->setCurrentIndex(7);
    status_label_->setText("Demonstrating DateTimePicker component");
}

void AdvancedComponentsShowcase::onIntegrationDemo() {
    main_tabs_->setCurrentIndex(8);
    status_label_->setText("Demonstrating component integration");
}

void AdvancedComponentsShowcase::onPerformanceDemo() {
    main_tabs_->setCurrentIndex(9);
    status_label_->setText("Demonstrating performance monitoring");
}

// **Demo Creation Methods**
QWidget* AdvancedComponentsShowcase::createSearchBoxDemo() {
    auto* demo_widget = new QWidget();
    auto* layout = new QVBoxLayout(demo_widget);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(15);

    // Title and description
    auto* title = new QLabel("🔍 SearchBox Component Demo");
    title->setStyleSheet("font-size: 18px; font-weight: bold; color: #2c3e50;");
    layout->addWidget(title);

    auto* description = new QLabel(
        "The SearchBox component provides advanced search functionality with auto-completion, "
        "search history, fuzzy matching, and customizable suggestion templates."
    );
    description->setWordWrap(true);
    description->setStyleSheet("color: #666; margin-bottom: 20px;");
    layout->addWidget(description);

    // Create SearchBox demo
    try {
        demo_search_box_ = std::make_unique<SearchBox>();
        demo_search_box_->initialize();
        demo_search_box_->placeholder("Search for products, people, or documents...");
        demo_search_box_->suggestions(sample_search_data_);
        demo_search_box_->autoComplete(true);
        demo_search_box_->showHistory(true);
        demo_search_box_->fuzzyMatching(true);
        demo_search_box_->maxSuggestions(8);
        demo_search_box_->onSearchChanged([this](const QString& query) {
            qDebug() << "🔍 Search query changed:" << query;
            status_label_->setText(QString("Searching for: %1").arg(query));
        });
        demo_search_box_->onSuggestionSelected([this](const SearchSuggestion& suggestion) {
            qDebug() << "✅ Selected suggestion:" << suggestion.text;
            status_label_->setText(QString("Selected: %1").arg(suggestion.text));
        });

        layout->addWidget(demo_search_box_->getWidget());

    } catch (const std::exception& e) {
        qWarning() << "❌ Failed to create SearchBox demo:" << e.what();
        auto* error_label = new QLabel("Failed to create SearchBox demo. Check console for details.");
        error_label->setStyleSheet("color: red; font-weight: bold;");
        layout->addWidget(error_label);
    }

    // Feature showcase
    auto* features_group = new QGroupBox("Features Demonstrated");
    auto* features_layout = new QVBoxLayout(features_group);

    auto* feature_list = new QListWidget();
    feature_list->addItem("✨ Real-time auto-completion");
    feature_list->addItem("📝 Search history management");
    feature_list->addItem("🔍 Fuzzy matching algorithm");
    feature_list->addItem("🎨 Customizable suggestion templates");
    feature_list->addItem("⌨️ Keyboard navigation support");
    feature_list->addItem("🎯 Event-driven architecture");
    feature_list->setMaximumHeight(150);
    features_layout->addWidget(feature_list);

    layout->addWidget(features_group);
    layout->addStretch();

    return demo_widget;
}

QWidget* AdvancedComponentsShowcase::createDataTableDemo() {
    auto* demo_widget = new QWidget();
    auto* layout = new QVBoxLayout(demo_widget);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(15);

    // Title and description
    auto* title = new QLabel("📊 DataTable Component Demo");
    title->setStyleSheet("font-size: 18px; font-weight: bold; color: #2c3e50;");
    layout->addWidget(title);

    auto* description = new QLabel(
        "The DataTable component provides a powerful data grid with sorting, filtering, "
        "in-line editing, row selection, pagination, and export functionality."
    );
    description->setWordWrap(true);
    description->setStyleSheet("color: #666; margin-bottom: 20px;");
    layout->addWidget(description);

    // Create DataTable demo
    try {
        demo_data_table_ = std::make_unique<DataTable>();
        demo_data_table_->initialize();
        demo_data_table_->model(sample_table_model_);
        demo_data_table_->sortable(true);
        demo_data_table_->filterable(true);
        demo_data_table_->editable(true);
        demo_data_table_->selectable(true);
        demo_data_table_->multiSelect(true);
        demo_data_table_->showToolbar(true);
        demo_data_table_->showStatusBar(true);
        demo_data_table_->onRowSelected([this](int row) {
            qDebug() << "📋 Row selected:" << row;
            status_label_->setText(QString("Selected row: %1").arg(row));
        });
        demo_data_table_->onCellEdited([this](int row, int column, const QVariant& value) {
            qDebug() << "✏️ Cell edited:" << row << column << value;
            status_label_->setText(QString("Edited cell [%1,%2]: %3").arg(row).arg(column).arg(value.toString()));
        });
        demo_data_table_->onFilterChanged([this](const QString& filter) {
            qDebug() << "🔍 Filter changed:" << filter;
            status_label_->setText(QString("Filter applied: %1").arg(filter));
        });

        layout->addWidget(demo_data_table_->getWidget());

    } catch (const std::exception& e) {
        qWarning() << "❌ Failed to create DataTable demo:" << e.what();
        auto* error_label = new QLabel("Failed to create DataTable demo. Check console for details.");
        error_label->setStyleSheet("color: red; font-weight: bold;");
        layout->addWidget(error_label);
    }

    return demo_widget;
}

QWidget* AdvancedComponentsShowcase::createPropertyEditorDemo() {
    return createDemoSection(
        "🎛️ PropertyEditor Component Demo",
        "The PropertyEditor component provides dynamic property editing with automatic "
        "editor generation based on data types, validation, and real-time updates.",
        new QLabel("PropertyEditor demo - Coming soon!")
    );
}

QWidget* AdvancedComponentsShowcase::createFormBuilderDemo() {
    return createDemoSection(
        "📋 FormBuilder Component Demo",
        "The FormBuilder component enables dynamic form generation from JSON schemas "
        "with validation, conditional fields, and multi-step support.",
        new QLabel("FormBuilder demo - Coming soon!")
    );
}

QWidget* AdvancedComponentsShowcase::createFileExplorerDemo() {
    return createDemoSection(
        "🗂️ FileExplorer Component Demo",
        "The FileExplorer component provides a complete file system browser with "
        "multiple view modes, file operations, and drag-and-drop support.",
        new QLabel("FileExplorer demo - Coming soon!")
    );
}

QWidget* AdvancedComponentsShowcase::createChartWidgetDemo() {
    return createDemoSection(
        "📈 ChartWidget Component Demo",
        "The ChartWidget component offers comprehensive charting capabilities with "
        "multiple chart types, interactive features, and real-time data updates.",
        new QLabel("ChartWidget demo - Coming soon!")
    );
}

QWidget* AdvancedComponentsShowcase::createColorPickerDemo() {
    return createDemoSection(
        "🎨 ColorPicker Component Demo",
        "The ColorPicker component provides advanced color selection with multiple "
        "color models, palette management, and accessibility features.",
        new QLabel("ColorPicker demo - Coming soon!")
    );
}

QWidget* AdvancedComponentsShowcase::createDateTimePickerDemo() {
    return createDemoSection(
        "📅 DateTimePicker Component Demo",
        "The DateTimePicker component offers sophisticated date and time selection "
        "with calendar views, time zones, and localization support.",
        new QLabel("DateTimePicker demo - Coming soon!")
    );
}

QWidget* AdvancedComponentsShowcase::createIntegrationDemo() {
    return createDemoSection(
        "🔗 Integration Demo",
        "This demo shows how advanced components work together to create "
        "sophisticated user interfaces with seamless data flow.",
        new QLabel("Integration demo - Coming soon!")
    );
}

QWidget* AdvancedComponentsShowcase::createPerformanceDemo() {
    return createDemoSection(
        "⚡ Performance Demo",
        "This demo monitors the performance of advanced components and shows "
        "optimization techniques for large datasets and complex UIs.",
        new QLabel("Performance demo - Coming soon!")
    );
}

// **SampleDataModel Implementation**
SampleDataModel::SampleDataModel(QObject* parent)
    : QAbstractTableModel(parent)
{
    headers_ = {"Name", "Email", "Age", "Department", "Hire Date", "Active", "Salary"};
}

int SampleDataModel::rowCount(const QModelIndex& parent) const {
    Q_UNUSED(parent)
    return sample_data_.size();
}

int SampleDataModel::columnCount(const QModelIndex& parent) const {
    Q_UNUSED(parent)
    return headers_.size();
}

QVariant SampleDataModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= sample_data_.size()) {
        return QVariant();
    }

    const SampleRow& row = sample_data_[index.row()];

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (index.column()) {
            case 0: return row.name;
            case 1: return row.email;
            case 2: return row.age;
            case 3: return row.department;
            case 4: return row.hire_date;
            case 5: return row.active ? "Yes" : "No";
            case 6: return QString("$%1").arg(row.salary, 0, 'f', 0);
        }
    }

    return QVariant();
}

QVariant SampleDataModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        if (section >= 0 && section < headers_.size()) {
            return headers_[section];
        }
    }
    return QVariant();
}

bool SampleDataModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (!index.isValid() || index.row() >= sample_data_.size() || role != Qt::EditRole) {
        return false;
    }

    SampleRow& row = sample_data_[index.row()];

    switch (index.column()) {
        case 0: row.name = value.toString(); break;
        case 1: row.email = value.toString(); break;
        case 2: row.age = value.toInt(); break;
        case 3: row.department = value.toString(); break;
        case 4: row.hire_date = value.toDate(); break;
        case 5: row.active = value.toBool(); break;
        case 6: row.salary = value.toDouble(); break;
        default: return false;
    }

    emit dataChanged(index, index);
    return true;
}

Qt::ItemFlags SampleDataModel::flags(const QModelIndex& index) const {
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

void SampleDataModel::addSampleData() {
    beginResetModel();

    sample_data_ = {
        {"John Doe", "john.doe@company.com", 32, "Engineering", QDate(2020, 3, 15), true, 85000},
        {"Jane Smith", "jane.smith@company.com", 28, "Design", QDate(2021, 7, 22), true, 75000},
        {"Bob Johnson", "bob.johnson@company.com", 45, "Management", QDate(2018, 1, 10), true, 120000},
        {"Alice Brown", "alice.brown@company.com", 35, "Marketing", QDate(2019, 11, 5), false, 68000},
        {"Charlie Wilson", "charlie.wilson@company.com", 29, "Engineering", QDate(2022, 2, 14), true, 78000},
        {"Diana Davis", "diana.davis@company.com", 41, "Sales", QDate(2017, 9, 30), true, 92000},
        {"Eve Miller", "eve.miller@company.com", 26, "Design", QDate(2023, 1, 8), true, 72000},
        {"Frank Garcia", "frank.garcia@company.com", 38, "Engineering", QDate(2019, 6, 12), true, 88000},
        {"Grace Lee", "grace.lee@company.com", 33, "Marketing", QDate(2020, 10, 25), true, 71000},
        {"Henry Taylor", "henry.taylor@company.com", 42, "Management", QDate(2016, 4, 18), true, 115000}
    };

    endResetModel();
}

void SampleDataModel::clearData() {
    beginResetModel();
    sample_data_.clear();
    endResetModel();
}

#include "AdvancedComponentsShowcase.moc"
