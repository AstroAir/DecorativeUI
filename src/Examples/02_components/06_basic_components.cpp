/**
 * @file 06_basic_components.cpp
 * @brief Basic UI components showcase - Button, Label, LineEdit
 * 
 * This example demonstrates:
 * - Basic DeclarativeUI components
 * - Component properties and styling
 * - Component interaction patterns
 * - JSON-based component configuration
 * 
 * Learning objectives:
 * - Understand core UI components
 * - Learn component property configuration
 * - See component styling and theming
 */

#include <QApplication>
#include <QObject>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QWidget>
#include <QGroupBox>
#include <QDebug>
#include <QMessageBox>
#include <QFileInfo>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QProgressBar>
#include <QSlider>
#include <QSpinBox>
#include <QCheckBox>

// Include DeclarativeUI components
#include "../../JSON/JSONUILoader.hpp"

using namespace DeclarativeUI;

/**
 * @brief Basic components demonstration
 */
class BasicComponentsApp : public QObject {
    Q_OBJECT

public:
    BasicComponentsApp(QObject* parent = nullptr) : QObject(parent) {
        setupUILoader();
        createUI();
    }

    void show() {
        if (main_widget_) {
            main_widget_->show();
        }
    }

private slots:
    void onButtonClicked() {
        // Get text from line edit
        auto line_edit = main_widget_->findChild<QLineEdit*>("messageInput");
        QString message = line_edit ? line_edit->text() : "Hello from DeclarativeUI!";
        
        if (message.isEmpty()) {
            message = "Hello from DeclarativeUI!";
        }

        // Update label
        auto display_label = main_widget_->findChild<QLabel*>("messageDisplay");
        if (display_label) {
            display_label->setText(QString("📢 %1").arg(message));
        }

        // Show message box
        QMessageBox::information(
            main_widget_.get(),
            "Button Clicked",
            QString("🎉 Button clicked!\n\nMessage: %1").arg(message)
        );

        qDebug() << "Button clicked with message:" << message;
    }

    void onClearClicked() {
        auto line_edit = main_widget_->findChild<QLineEdit*>("messageInput");
        auto display_label = main_widget_->findChild<QLabel*>("messageDisplay");

        if (line_edit) {
            line_edit->clear();
        }

        if (display_label) {
            display_label->setText("💬 Enter a message above...");
        }

        qDebug() << "Fields cleared";
    }

    void onTextChanged(const QString& text) {
        // Update character count
        auto char_count_label = main_widget_->findChild<QLabel*>("charCount");
        if (char_count_label) {
            char_count_label->setText(QString("Characters: %1").arg(text.length()));
        }

        // Update button state based on text
        auto submit_button = main_widget_->findChild<QPushButton*>("submitButton");
        if (submit_button) {
            submit_button->setEnabled(!text.trimmed().isEmpty());
        }
    }

    void onStyleButtonClicked() {
        // Cycle through different styles
        static int style_index = 0;
        style_index = (style_index + 1) % 4;

        auto display_label = main_widget_->findChild<QLabel*>("messageDisplay");
        if (!display_label) return;

        QString styles[] = {
            "color: #2c3e50; background-color: #ecf0f1; padding: 10px; border-radius: 5px;",
            "color: #e74c3c; background-color: #fadbd8; padding: 10px; border-radius: 5px; border: 2px solid #e74c3c;",
            "color: #27ae60; background-color: #d5f4e6; padding: 10px; border-radius: 5px; border: 2px solid #27ae60;",
            "color: #f39c12; background-color: #fef9e7; padding: 10px; border-radius: 5px; border: 2px solid #f39c12;"
        };

        display_label->setStyleSheet(styles[style_index]);
        qDebug() << "Style changed to index:" << style_index;
    }

private:
    std::unique_ptr<QWidget> main_widget_;
    std::unique_ptr<JSON::JSONUILoader> ui_loader_;

    void setupUILoader() {
        ui_loader_ = std::make_unique<JSON::JSONUILoader>();

        // Register event handlers
        ui_loader_->registerEventHandler("submitMessage", [this]() {
            onButtonClicked();
        });

        ui_loader_->registerEventHandler("clearFields", [this]() {
            onClearClicked();
        });

        ui_loader_->registerEventHandler("changeStyle", [this]() {
            onStyleButtonClicked();
        });

        qDebug() << "✅ Event handlers registered";
    }

    void createUI() {
        try {
            QString ui_file = "resources/basic_components_ui.json";
            
            if (QFileInfo::exists(ui_file)) {
                main_widget_ = ui_loader_->loadFromFile(ui_file);
                if (main_widget_) {
                    main_widget_->setWindowTitle("06 - Basic Components | DeclarativeUI");
                    connectUIEvents();
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
        // Connect line edit text changes
        auto line_edit = main_widget_->findChild<QLineEdit*>("messageInput");
        if (line_edit) {
            connect(line_edit, &QLineEdit::textChanged, this, &BasicComponentsApp::onTextChanged);
            
            // Set initial state
            onTextChanged(line_edit->text());
        }

        qDebug() << "✅ UI events connected";
    }

    std::unique_ptr<QWidget> createProgrammaticUI() {
        auto widget = std::make_unique<QWidget>();
        widget->setWindowTitle("06 - Basic Components (Fallback) | DeclarativeUI");
        widget->setMinimumSize(500, 400);

        auto layout = new QVBoxLayout(widget.get());
        layout->setSpacing(20);
        layout->setContentsMargins(20, 20, 20, 20);

        // Header
        auto header = new QLabel("🧱 Basic Components Showcase");
        header->setStyleSheet("font-size: 20px; font-weight: bold; color: #2c3e50;");
        header->setAlignment(Qt::AlignCenter);
        layout->addWidget(header);

        // Description
        auto description = new QLabel(
            "This example demonstrates the three most fundamental UI components:\n"
            "• Label - for displaying text\n"
            "• LineEdit - for text input\n"
            "• Button - for user actions"
        );
        description->setWordWrap(true);
        description->setAlignment(Qt::AlignCenter);
        description->setStyleSheet("color: #7f8c8d; margin-bottom: 10px;");
        layout->addWidget(description);

        // Input section
        auto input_group = new QGroupBox("📝 Text Input");
        auto input_layout = new QVBoxLayout(input_group);

        auto input_label = new QLabel("Enter a message:");
        auto line_edit = new QLineEdit();
        line_edit->setObjectName("messageInput");
        line_edit->setPlaceholderText("Type your message here...");
        
        auto char_count = new QLabel("Characters: 0");
        char_count->setObjectName("charCount");
        char_count->setStyleSheet("color: #7f8c8d; font-size: 12px;");

        connect(line_edit, &QLineEdit::textChanged, this, &BasicComponentsApp::onTextChanged);

        input_layout->addWidget(input_label);
        input_layout->addWidget(line_edit);
        input_layout->addWidget(char_count);
        layout->addWidget(input_group);

        // Display section
        auto display_group = new QGroupBox("📢 Message Display");
        auto display_layout = new QVBoxLayout(display_group);

        auto display_label = new QLabel("💬 Enter a message above...");
        display_label->setObjectName("messageDisplay");
        display_label->setAlignment(Qt::AlignCenter);
        display_label->setStyleSheet(
            "color: #2c3e50; background-color: #ecf0f1; "
            "padding: 15px; border-radius: 8px; font-size: 14px;"
        );
        display_label->setWordWrap(true);

        display_layout->addWidget(display_label);
        layout->addWidget(display_group);

        // Button section
        auto button_group = new QGroupBox("🔘 Actions");
        auto button_layout = new QHBoxLayout(button_group);

        auto submit_button = new QPushButton("📤 Submit Message");
        submit_button->setObjectName("submitButton");
        submit_button->setEnabled(false);
        submit_button->setStyleSheet(
            "QPushButton { background-color: #3498db; color: white; "
            "padding: 10px 20px; border: none; border-radius: 5px; font-weight: bold; }"
            "QPushButton:hover { background-color: #2980b9; }"
            "QPushButton:disabled { background-color: #bdc3c7; }"
        );

        auto clear_button = new QPushButton("🗑️ Clear");
        clear_button->setStyleSheet(
            "QPushButton { background-color: #e74c3c; color: white; "
            "padding: 10px 20px; border: none; border-radius: 5px; }"
            "QPushButton:hover { background-color: #c0392b; }"
        );

        auto style_button = new QPushButton("🎨 Change Style");
        style_button->setStyleSheet(
            "QPushButton { background-color: #9b59b6; color: white; "
            "padding: 10px 20px; border: none; border-radius: 5px; }"
            "QPushButton:hover { background-color: #8e44ad; }"
        );

        connect(submit_button, &QPushButton::clicked, this, &BasicComponentsApp::onButtonClicked);
        connect(clear_button, &QPushButton::clicked, this, &BasicComponentsApp::onClearClicked);
        connect(style_button, &QPushButton::clicked, this, &BasicComponentsApp::onStyleButtonClicked);

        button_layout->addWidget(submit_button);
        button_layout->addWidget(clear_button);
        button_layout->addWidget(style_button);
        button_layout->addStretch();

        layout->addWidget(button_group);
        layout->addStretch();

        qDebug() << "✅ Programmatic UI created";
        return widget;
    }
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    app.setApplicationName("DeclarativeUI Basic Components");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("DeclarativeUI Examples");

    try {
        qDebug() << "🚀 Starting Basic Components example...";
        
        BasicComponentsApp components_app;
        components_app.show();

        qDebug() << "💡 This example demonstrates:";
        qDebug() << "   - Label component for text display";
        qDebug() << "   - LineEdit component for text input";
        qDebug() << "   - Button component for user actions";
        qDebug() << "   - Component styling and theming";

        return app.exec();

    } catch (const std::exception& e) {
        qCritical() << "❌ Application error:" << e.what();
        return -1;
    }
}

#include "06_basic_components.moc"
