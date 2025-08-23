#pragma once

/**
 * @file AdvancedComponentsShowcase.hpp
 * @brief Comprehensive showcase of all advanced DeclarativeUI components
 */

#include <QCheckBox>
#include <QColorDialog>
#include <QComboBox>
#include <QDateEdit>
#include <QDockWidget>
#include <QFileDialog>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMainWindow>
#include <QMenuBar>
#include <QProgressBar>
#include <QPushButton>
#include <QScrollArea>
#include <QSlider>
#include <QSpinBox>
#include <QSplitter>
#include <QStatusBar>
#include <QTabWidget>
#include <QTableWidget>
#include <QTextEdit>
#include <QTimeEdit>
#include <QToolBar>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QWidget>
#include <memory>

// Include advanced components
#include "../../../src/Components/Advanced/DataTable.hpp"
#include "../../../src/Components/Advanced/SearchBox.hpp"

using namespace DeclarativeUI::Components::Advanced;

/**
 * @brief Main showcase window demonstrating all advanced components
 */
class AdvancedComponentsShowcase : public QMainWindow {
    Q_OBJECT

public:
    explicit AdvancedComponentsShowcase(QWidget* parent = nullptr);
    ~AdvancedComponentsShowcase() override = default;

private slots:
    void onSearchBoxDemo();
    void onDataTableDemo();
    void onPropertyEditorDemo();
    void onFormBuilderDemo();
    void onFileExplorerDemo();
    void onChartWidgetDemo();
    void onColorPickerDemo();
    void onDateTimePickerDemo();
    void onIntegrationDemo();
    void onPerformanceDemo();

private:
    // **UI Setup**
    void setupUI();
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    void setupDockWidgets();

    // **Component Demos**
    QWidget* createSearchBoxDemo();
    QWidget* createDataTableDemo();
    QWidget* createPropertyEditorDemo();
    QWidget* createFormBuilderDemo();
    QWidget* createFileExplorerDemo();
    QWidget* createChartWidgetDemo();
    QWidget* createColorPickerDemo();
    QWidget* createDateTimePickerDemo();
    QWidget* createIntegrationDemo();
    QWidget* createPerformanceDemo();

    // **Helper Methods**
    QWidget* createDemoSection(const QString& title, const QString& description,
                               QWidget* demo_widget);
    void addDemoTab(const QString& title, const QString& icon_path,
                    QWidget* content);
    void showComponentInfo(const QString& component_name,
                           const QString& description,
                           const QStringList& features);

    // **Main Components**
    QTabWidget* main_tabs_;
    QTextEdit* info_panel_;
    QTreeWidget* component_tree_;
    QListWidget* feature_list_;

    // **Demo Components**
    std::unique_ptr<SearchBox> demo_search_box_;
    std::unique_ptr<DataTable> demo_data_table_;

    // **Status and Info**
    QLabel* status_label_;
    QProgressBar* demo_progress_;
    QPushButton* reset_button_;
    QPushButton* export_button_;

    // **Sample Data**
    void setupSampleData();
    QStringList sample_search_data_;
    QAbstractItemModel* sample_table_model_;
};

/**
 * @brief Custom widget for displaying component features and documentation
 */
class ComponentInfoWidget : public QWidget {
    Q_OBJECT

public:
    explicit ComponentInfoWidget(QWidget* parent = nullptr);

    void setComponentInfo(const QString& name, const QString& description,
                          const QStringList& features,
                          const QString& usage_example);
    void clearInfo();

private:
    void setupUI();

    QLabel* name_label_;
    QTextEdit* description_text_;
    QListWidget* features_list_;
    QTextEdit* usage_example_;
};

/**
 * @brief Performance monitoring widget for advanced components
 */
class PerformanceMonitor : public QWidget {
    Q_OBJECT

public:
    explicit PerformanceMonitor(QWidget* parent = nullptr);

    void startMonitoring();
    void stopMonitoring();
    void recordMetric(const QString& component, const QString& operation,
                      qint64 duration_ms);

signals:
    void performanceAlert(const QString& message);

private slots:
    void updateMetrics();

private:
    void setupUI();

    QTableWidget* metrics_table_;
    QLabel* memory_usage_;
    QLabel* cpu_usage_;
    QProgressBar* performance_bar_;

    QTimer* update_timer_;
    QMap<QString, QList<qint64>> performance_data_;
};

/**
 * @brief Integration demo showing how advanced components work together
 */
class IntegrationDemoWidget : public QWidget {
    Q_OBJECT

public:
    explicit IntegrationDemoWidget(QWidget* parent = nullptr);

private slots:
    void onSearchResultSelected(const SearchSuggestion& suggestion);
    void onTableRowSelected(int row);
    void onPropertyChanged(const QString& property, const QVariant& value);

private:
    void setupUI();
    void connectComponents();

    std::unique_ptr<SearchBox> search_component_;
    std::unique_ptr<DataTable> table_component_;
    QTextEdit* details_panel_;
    QLabel* status_info_;
};

/**
 * @brief Sample data model for DataTable demonstrations
 */
class SampleDataModel : public QAbstractTableModel {
    Q_OBJECT

public:
    explicit SampleDataModel(QObject* parent = nullptr);

    // QAbstractTableModel interface
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index,
                  int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value,
                 int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    // Custom methods
    void addSampleData();
    void clearData();
    void addRow(const QStringList& row_data);
    void removeRow(int row);

private:
    struct SampleRow {
        QString name;
        QString email;
        int age;
        QString department;
        QDate hire_date;
        bool active;
        double salary;
    };

    QList<SampleRow> sample_data_;
    QStringList headers_;
};

/**
 * @brief Theme selector for demonstrating component styling
 */
class ThemeSelector : public QWidget {
    Q_OBJECT

public:
    explicit ThemeSelector(QWidget* parent = nullptr);

signals:
    void themeChanged(const QString& theme_name);

private slots:
    void onThemeSelected();

private:
    void setupUI();
    void applyTheme(const QString& theme_name);

    QComboBox* theme_combo_;
    QLabel* preview_label_;
    QPushButton* apply_button_;

    QMap<QString, QString> available_themes_;
};

/**
 * @brief Code example viewer for showing usage patterns
 */
class CodeExampleViewer : public QWidget {
    Q_OBJECT

public:
    explicit CodeExampleViewer(QWidget* parent = nullptr);

    void setExample(const QString& title, const QString& code,
                    const QString& description);
    void addExample(const QString& title, const QString& code,
                    const QString& description);
    void clearExamples();

private slots:
    void onExampleSelected();
    void onCopyCode();

private:
    void setupUI();
    void highlightCode();

    QComboBox* example_selector_;
    QTextEdit* code_display_;
    QTextEdit* description_display_;
    QPushButton* copy_button_;
    QPushButton* run_button_;

    struct CodeExample {
        QString title;
        QString code;
        QString description;
    };

    QList<CodeExample> examples_;
};
