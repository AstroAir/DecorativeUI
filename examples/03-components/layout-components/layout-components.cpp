#include <QApplication>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

// Include DeclarativeUI headers
#include "src/Components/Button.hpp"
#include "src/Components/Label.hpp"
#include "src/Core/DeclarativeBuilder.hpp"

/**
 * Layout Components Example
 *
 * Demonstrates different layout managers and container components.
 * Shows how to organize UI elements using various layout strategies.
 */

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Create main widget
    auto widget = std::make_unique<QWidget>();
    widget->setWindowTitle("Layout Components Example");
    widget->resize(600, 500);

    auto mainLayout = std::make_unique<QVBoxLayout>(widget.get());

    // Title
    auto title = std::make_unique<QLabel>("Layout Components Demonstration");
    title->setStyleSheet("font-size: 16px; font-weight: bold; margin: 10px;");
    mainLayout->addWidget(title.release());

    // Horizontal Layout Example
    auto hboxGroup =
        std::make_unique<QGroupBox>("Horizontal Layout (QHBoxLayout)");
    auto hboxLayout = std::make_unique<QHBoxLayout>(hboxGroup.get());
    hboxLayout->addWidget(new QPushButton("Button 1"));
    hboxLayout->addWidget(new QPushButton("Button 2"));
    hboxLayout->addWidget(new QPushButton("Button 3"));
    mainLayout->addWidget(hboxGroup.release());

    // Grid Layout Example
    auto gridGroup = std::make_unique<QGroupBox>("Grid Layout (QGridLayout)");
    auto gridLayout = std::make_unique<QGridLayout>(gridGroup.get());
    gridLayout->addWidget(new QPushButton("(0,0)"), 0, 0);
    gridLayout->addWidget(new QPushButton("(0,1)"), 0, 1);
    gridLayout->addWidget(new QPushButton("(1,0)"), 1, 0);
    gridLayout->addWidget(new QPushButton("(1,1)"), 1, 1);
    mainLayout->addWidget(gridGroup.release());

    // Form Layout Example
    auto formGroup = std::make_unique<QGroupBox>("Form Layout (QFormLayout)");
    auto formLayout = std::make_unique<QFormLayout>(formGroup.get());
    formLayout->addRow("Name:", new QLineEdit());
    formLayout->addRow("Email:", new QLineEdit());
    formLayout->addRow("Phone:", new QLineEdit());
    mainLayout->addWidget(formGroup.release());

    // Nested Layout Example
    auto nestedGroup = std::make_unique<QGroupBox>("Nested Layouts");
    auto nestedMainLayout = std::make_unique<QVBoxLayout>(nestedGroup.get());

    auto nestedHLayout = std::make_unique<QHBoxLayout>();
    nestedHLayout->addWidget(new QLabel("Left"));
    nestedHLayout->addWidget(new QLabel("Right"));

    nestedMainLayout->addLayout(nestedHLayout.release());
    nestedMainLayout->addWidget(new QPushButton("Bottom Button"));

    mainLayout->addWidget(nestedGroup.release());

    widget->show();

    return app.exec();
}
