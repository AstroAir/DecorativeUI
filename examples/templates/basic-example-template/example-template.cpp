#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

// Include DeclarativeUI headers
#include "src/Core/DeclarativeBuilder.hpp"
#include "src/Components/Button.hpp"
#include "src/Components/Label.hpp"

/**
 * [Example Name] Example
 * 
 * [Brief description of what this example demonstrates]
 * 
 * Key concepts:
 * - [Concept 1]
 * - [Concept 2]
 * - [Concept 3]
 */

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Create main widget
    auto widget = std::make_unique<QWidget>();
    widget->setWindowTitle("[Example Name] Example");
    widget->resize(400, 300);
    
    auto layout = std::make_unique<QVBoxLayout>(widget.get());

    // Title
    auto title = std::make_unique<QLabel>("[Example Name] Demonstration");
    title->setStyleSheet("font-size: 16px; font-weight: bold; margin: 10px;");
    layout->addWidget(title.release());

    // Example content goes here
    // TODO: Add your example-specific UI components and logic
    
    // Example button
    auto button = std::make_unique<QPushButton>("Click Me");
    QObject::connect(button.get(), &QPushButton::clicked, []() {
        // TODO: Add button click handler
        qDebug() << "Button clicked!";
    });
    layout->addWidget(button.release());

    // Example label
    auto infoLabel = std::make_unique<QLabel>("This is a template example.");
    layout->addWidget(infoLabel.release());

    // Add stretch to push content to top
    layout->addStretch();

    widget->show();

    return app.exec();
}

/*
 * Template Usage Instructions:
 * 
 * 1. Copy this template to your new example directory
 * 2. Rename the file to match your example name (use kebab-case)
 * 3. Replace [Example Name] placeholders with your actual example name
 * 4. Update the description and key concepts
 * 5. Replace the TODO sections with your example-specific code
 * 6. Update the window title and widget properties as needed
 * 7. Add your example to the appropriate CMakeLists.txt
 * 8. Create a README.md file for your example
 * 9. Test your example thoroughly
 * 10. Update the category README.md to include your example
 * 
 * Best Practices:
 * - Keep examples focused on specific concepts
 * - Use clear, descriptive variable names
 * - Add comments explaining complex logic
 * - Follow the established code style
 * - Include error handling where appropriate
 * - Make examples self-contained and runnable
 */
