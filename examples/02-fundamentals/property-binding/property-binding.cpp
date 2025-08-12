#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>

// Include DeclarativeUI headers
#include "src/Core/DeclarativeBuilder.hpp"
#include "src/Components/Button.hpp"
#include "src/Components/Label.hpp"
#include "src/Components/Slider.hpp"

/**
 * Property Binding Example
 * 
 * Demonstrates property binding and reactive updates between components.
 * Shows how changes in one component automatically update others.
 */

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Create main widget
    auto widget = std::make_unique<QWidget>();
    widget->setWindowTitle("Property Binding Example");
    widget->resize(400, 300);
    
    auto layout = std::make_unique<QVBoxLayout>(widget.get());

    // Create a slider and spinbox that are bound together
    auto slider = std::make_unique<QSlider>(Qt::Horizontal);
    slider->setRange(0, 100);
    slider->setValue(50);
    
    auto spinBox = std::make_unique<QSpinBox>();
    spinBox->setRange(0, 100);
    spinBox->setValue(50);
    
    auto label = std::make_unique<QLabel>("Value: 50");
    
    // Bind slider to spinbox
    QObject::connect(slider.get(), &QSlider::valueChanged, 
                     spinBox.get(), &QSpinBox::setValue);
    
    // Bind spinbox to slider
    QObject::connect(spinBox.get(), QOverload<int>::of(&QSpinBox::valueChanged),
                     slider.get(), &QSlider::setValue);
    
    // Update label when either changes
    auto updateLabel = [label = label.get()](int value) {
        label->setText(QString("Value: %1").arg(value));
    };
    
    QObject::connect(slider.get(), &QSlider::valueChanged, updateLabel);
    QObject::connect(spinBox.get(), QOverload<int>::of(&QSpinBox::valueChanged), updateLabel);

    // Add widgets to layout
    layout->addWidget(new QLabel("Property Binding Demonstration"));
    layout->addWidget(new QLabel("The slider and spinbox are bound together:"));
    layout->addWidget(slider.release());
    layout->addWidget(spinBox.release());
    layout->addWidget(label.release());
    layout->addStretch();

    widget->show();

    return app.exec();
}
