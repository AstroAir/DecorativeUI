/**
 * @file enhanced-components-demo.cpp
 * @brief Comprehensive demonstration of enhanced DeclarativeUI components
 *
 * This example showcases the advanced features of enhanced components including:
 * - Accessibility support
 * - Validation systems
 * - Visual enhancements
 * - State management
 * - Event handling
 */

#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QDebug>
#include <memory>

// Enhanced Components
#include "../../src/Components/Button.hpp"
#include "../../src/Components/LineEdit.hpp"
#include "../../src/Components/CheckBox.hpp"
#include "../../src/Components/ComboBox.hpp"
#include "../../src/Components/Slider.hpp"
#include "../../src/Components/ProgressBar.hpp"
#include "../../src/Components/RadioButton.hpp"
#include "../../src/Components/GroupBox.hpp"
#include "../../src/Components/Frame.hpp"

using namespace DeclarativeUI::Components;

class EnhancedComponentsDemo : public QWidget {
    Q_OBJECT

public:
    EnhancedComponentsDemo(QWidget* parent = nullptr) : QWidget(parent) {
        setupUI();
        connectSignals();
    }

private slots:
    void onFormValidationChanged() {
        bool allValid = true;
        QString errors;

        // Check validation status of all components
        if (!emailEdit_->isValid()) {
            allValid = false;
            errors += "Email: " + emailEdit_->getValidationError() + "\n";
        }

        if (!termsCheckbox_->isValid()) {
            allValid = false;
            errors += "Terms: " + termsCheckbox_->getValidationError() + "\n";
        }

        // Update submit button state
        submitButton_->disabled(!allValid, allValid ? "" : "Please fix validation errors");

        // Update progress
        int progress = 0;
        if (emailEdit_->isValid()) progress += 25;
        if (!nameEdit_->getText().isEmpty()) progress += 25;
        if (termsCheckbox_->isValid()) progress += 25;
        if (ageSlider_->getValue() > 0) progress += 25;

        formProgress_->setValue(progress);

        qDebug() << "Form validation changed. Valid:" << allValid << "Progress:" << progress;
    }

private:
    void setupUI() {
        setWindowTitle("Enhanced DeclarativeUI Components Demo");
        setMinimumSize(800, 600);

        auto* mainLayout = new QVBoxLayout(this);

        // Create demo sections
        createInputSection();
        createSelectionSection();
        createVisualSection();
        createValidationSection();

        // Add sections to main layout
        mainLayout->addWidget(inputGroup_);
        mainLayout->addWidget(selectionGroup_);
        mainLayout->addWidget(visualGroup_);
        mainLayout->addWidget(validationGroup_);
        mainLayout->addStretch();
    }

    void createInputSection() {
        inputGroup_ = new QGroupBox("Enhanced Input Components");
        auto* layout = new QGridLayout(inputGroup_);

        // Enhanced LineEdit with validation
        layout->addWidget(new QLabel("Email:"), 0, 0);
        emailEdit_ = std::make_unique<LineEdit>();
        emailEdit_->placeholder("Enter your email address");
        emailEdit_->initialize();
        layout->addWidget(emailEdit_->getWidget(), 0, 1);

        // Enhanced LineEdit for name
        layout->addWidget(new QLabel("Name:"), 1, 0);
        nameEdit_ = std::make_unique<LineEdit>();
        nameEdit_->placeholder("Enter your full name");
        nameEdit_->initialize();
        layout->addWidget(nameEdit_->getWidget(), 1, 1);
    }

    void createSelectionSection() {
        selectionGroup_ = new QGroupBox("Enhanced Selection Components");
        auto* layout = new QGridLayout(selectionGroup_);

        // Enhanced CheckBox
        termsCheckbox_ = std::make_unique<CheckBox>();
        termsCheckbox_->text("I agree to the terms and conditions");
        termsCheckbox_->initialize();
        layout->addWidget(termsCheckbox_->getWidget(), 0, 0, 1, 2);

        // Enhanced ComboBox
        layout->addWidget(new QLabel("Country:"), 1, 0);
        countryCombo_ = std::make_unique<ComboBox>();
        countryCombo_->items({"United States", "Canada", "United Kingdom", "Australia", "Germany", "France"});
        countryCombo_->initialize();
        layout->addWidget(countryCombo_->getWidget(), 1, 1);

        // Enhanced Slider
        layout->addWidget(new QLabel("Age:"), 2, 0);
        ageSlider_ = std::make_unique<Slider>();
        ageSlider_->minimum(0).maximum(100).value(25);
        ageSlider_->onValueChanged([this](int value) {
            qDebug() << "Age changed:" << value;
            onFormValidationChanged();
        });
        ageSlider_->initialize();
        layout->addWidget(ageSlider_->getWidget(), 2, 1);
    }

    void createVisualSection() {
        visualGroup_ = new QGroupBox("Enhanced Visual Components");
        auto* layout = new QHBoxLayout(visualGroup_);

        // Enhanced Progress Bar
        formProgress_ = std::make_unique<ProgressBar>();
        formProgress_->minimum(0).maximum(100).value(0);
        formProgress_->initialize();
        layout->addWidget(formProgress_->getWidget());
    }

    void createValidationSection() {
        validationGroup_ = new QGroupBox("Enhanced Action Components");
        auto* layout = new QHBoxLayout(validationGroup_);

        // Enhanced Submit Button
        submitButton_ = std::make_unique<Button>();
        submitButton_->text("Submit Form");
        submitButton_->onClick([this]() {
            qDebug() << "Form submitted!";
            // Simulate form submission
            submitButton_->loading(true);
            QTimer::singleShot(2000, [this]() {
                submitButton_->loading(false);
                qDebug() << "Form submission complete!";
            });
        });
        submitButton_->initialize();
        layout->addWidget(submitButton_->getWidget());

        // Enhanced Reset Button
        resetButton_ = std::make_unique<Button>();
        resetButton_->text("Reset Form");
        resetButton_->onClick([this]() {
            emailEdit_->setText("");
            nameEdit_->setText("");
            termsCheckbox_->setChecked(false);
            countryCombo_->setCurrentIndex(-1);
            ageSlider_->setValue(0);
            onFormValidationChanged();
            qDebug() << "Form reset!";
        });
        resetButton_->initialize();
        layout->addWidget(resetButton_->getWidget());

        layout->addStretch();
    }

    void connectSignals() {
        // Additional signal connections can be added here
    }

private:
    // UI Groups
    QGroupBox* inputGroup_;
    QGroupBox* selectionGroup_;
    QGroupBox* visualGroup_;
    QGroupBox* validationGroup_;

    // Enhanced Components
    std::unique_ptr<LineEdit> emailEdit_;
    std::unique_ptr<LineEdit> nameEdit_;
    std::unique_ptr<CheckBox> termsCheckbox_;
    std::unique_ptr<ComboBox> countryCombo_;
    std::unique_ptr<Slider> ageSlider_;
    std::unique_ptr<ProgressBar> formProgress_;
    std::unique_ptr<Button> submitButton_;
    std::unique_ptr<Button> resetButton_;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Set application properties
    app.setApplicationName("Enhanced DeclarativeUI Demo");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("DeclarativeUI");

    // Create and show the demo window
    EnhancedComponentsDemo demo;
    demo.show();

    qDebug() << "Enhanced DeclarativeUI Components Demo started";
    qDebug() << "This demo showcases:";
    qDebug() << "- Real-time validation with visual feedback";
    qDebug() << "- Accessibility features (screen reader support)";
    qDebug() << "- Visual enhancements (shadows, animations, hover effects)";
    qDebug() << "- State management (loading states, disabled states)";
    qDebug() << "- Advanced event handling";

    return app.exec();
}

#include "enhanced-components-demo.moc"
