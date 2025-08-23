/**
 * @file ComponentGallery.hpp
 * @brief Interactive gallery showcasing all DeclarativeUI components
 */

#pragma once

#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QScrollArea>
#include <QSplitter>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>
#include <memory>

// DeclarativeUI Components
#include "Components/Button.hpp"
#include "Components/CheckBox.hpp"
#include "Components/ComboBox.hpp"
#include "Components/Label.hpp"
#include "Components/LineEdit.hpp"
#include "Components/ProgressBar.hpp"
#include "Components/Slider.hpp"
#include "Components/SpinBox.hpp"

using namespace DeclarativeUI;

/**
 * @brief Interactive component gallery widget
 */
class ComponentGallery : public QWidget {
    Q_OBJECT

public:
    explicit ComponentGallery(QWidget* parent = nullptr);

private slots:
    void onComponentSelected(const QString& component_name);
    void onPropertyChanged();
    void onCodeCopyRequested();

private:
    void setupUI();
    void createComponentList();
    void createComponentDemo();
    void createCodeViewer();
    void showComponent(const QString& name);
    void updateCodeExample(const QString& component_name);

    // UI Components
    QHBoxLayout* main_layout_;
    QSplitter* splitter_;

    // Component List
    QListWidget* component_list_;

    // Demo Area
    QWidget* demo_widget_;
    QVBoxLayout* demo_layout_;
    QGroupBox* demo_group_;
    QWidget* current_demo_;

    // Code Viewer
    QGroupBox* code_group_;
    QTextEdit* code_viewer_;
    QPushButton* copy_button_;

    // Component Demos
    std::map<QString, std::function<QWidget*()>> component_factories_;
    QString current_component_;
};

/**
 * @brief Component demo information
 */
struct ComponentDemo {
    QString name;
    QString description;
    QString category;
    std::function<QWidget*()> factory;
    QString code_example;

    ComponentDemo(const QString& n, const QString& d, const QString& c,
                  std::function<QWidget*()> f, const QString& code)
        : name(n),
          description(d),
          category(c),
          factory(f),
          code_example(code) {}
};
