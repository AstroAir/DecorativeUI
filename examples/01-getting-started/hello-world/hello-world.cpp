/**
 * @file 01_hello_world.cpp
 * @brief Minimal DeclarativeUI application - Hello World
 *
 * This example demonstrates:
 * - Basic DeclarativeUI setup
 * - Creating a simple widget programmatically
 * - Minimal application structure
 *
 * Learning objectives:
 * - Understand the basic structure of a DeclarativeUI application
 * - Learn how to create and display a simple widget
 * - See the minimal code required to get started
 */

#include <QApplication>
#include <QDebug>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

// Include DeclarativeUI core
#include "Core/DeclarativeBuilder.hpp"

using namespace DeclarativeUI;

/**
 * @brief Simple Hello World application using DeclarativeUI
 */
class HelloWorldApp : public QObject {
    Q_OBJECT

public:
    HelloWorldApp(QObject* parent = nullptr) : QObject(parent) { createUI(); }

    void show() {
        if (main_widget_) {
            main_widget_->show();
        }
    }

private slots:
    void onHelloClicked() {
        QMessageBox::information(
            main_widget_.get(), "Hello DeclarativeUI!",
            "🎉 Welcome to DeclarativeUI!\n\n"
            "This is your first DeclarativeUI application.\n"
            "You've successfully created a reactive UI!");

        qDebug() << "Hello button clicked! DeclarativeUI is working! 🚀";
    }

    void onAboutClicked() {
        QMessageBox::about(
            main_widget_.get(), "About Hello World",
            "<h3>DeclarativeUI Hello World</h3>"
            "<p>This is the simplest possible DeclarativeUI application.</p>"
            "<p><b>Features demonstrated:</b></p>"
            "<ul>"
            "<li>Basic widget creation</li>"
            "<li>Event handling</li>"
            "<li>Simple layout management</li>"
            "</ul>"
            "<p><i>Next: Try the JSON UI Loading example!</i></p>");
    }

private:
    std::unique_ptr<QWidget> main_widget_;

    void createUI() {
        // Create main widget
        main_widget_ = std::make_unique<QWidget>();
        main_widget_->setWindowTitle("01 - Hello World | DeclarativeUI");
        main_widget_->setMinimumSize(400, 300);

        // Create layout
        auto layout = new QVBoxLayout(main_widget_.get());
        layout->setSpacing(20);
        layout->setContentsMargins(30, 30, 30, 30);

        // Welcome label
        auto welcome_label = new QLabel("🌟 Welcome to DeclarativeUI!");
        welcome_label->setStyleSheet(
            "QLabel {"
            "  font-size: 24px;"
            "  font-weight: bold;"
            "  color: #2c3e50;"
            "  text-align: center;"
            "}");
        welcome_label->setAlignment(Qt::AlignCenter);
        layout->addWidget(welcome_label);

        // Description label
        auto description_label = new QLabel(
            "This is your first DeclarativeUI application.\n"
            "Click the button below to see it in action!");
        description_label->setStyleSheet(
            "QLabel {"
            "  font-size: 14px;"
            "  color: #34495e;"
            "  text-align: center;"
            "}");
        description_label->setAlignment(Qt::AlignCenter);
        description_label->setWordWrap(true);
        layout->addWidget(description_label);

        // Add some spacing
        layout->addStretch();

        // Hello button
        auto hello_button = new QPushButton("👋 Say Hello!");
        hello_button->setStyleSheet(
            "QPushButton {"
            "  background-color: #3498db;"
            "  color: white;"
            "  border: none;"
            "  padding: 12px 24px;"
            "  font-size: 16px;"
            "  font-weight: bold;"
            "  border-radius: 6px;"
            "}"
            "QPushButton:hover {"
            "  background-color: #2980b9;"
            "}"
            "QPushButton:pressed {"
            "  background-color: #21618c;"
            "}");
        connect(hello_button, &QPushButton::clicked, this,
                &HelloWorldApp::onHelloClicked);
        layout->addWidget(hello_button);

        // About button
        auto about_button = new QPushButton("ℹ️ About");
        about_button->setStyleSheet(
            "QPushButton {"
            "  background-color: #95a5a6;"
            "  color: white;"
            "  border: none;"
            "  padding: 8px 16px;"
            "  font-size: 14px;"
            "  border-radius: 4px;"
            "}"
            "QPushButton:hover {"
            "  background-color: #7f8c8d;"
            "}");
        connect(about_button, &QPushButton::clicked, this,
                &HelloWorldApp::onAboutClicked);
        layout->addWidget(about_button);

        layout->addStretch();

        // Status label
        auto status_label =
            new QLabel("✅ DeclarativeUI initialized successfully");
        status_label->setStyleSheet(
            "QLabel {"
            "  font-size: 12px;"
            "  color: #27ae60;"
            "  font-style: italic;"
            "}");
        status_label->setAlignment(Qt::AlignCenter);
        layout->addWidget(status_label);

        qDebug() << "✅ Hello World UI created successfully";
    }
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Set application properties
    app.setApplicationName("DeclarativeUI Hello World");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("DeclarativeUI Examples");

    try {
        qDebug() << "🚀 Starting DeclarativeUI Hello World example...";

        HelloWorldApp hello_app;
        hello_app.show();

        qDebug() << "💡 Next steps:";
        qDebug() << "   - Try example 02: JSON UI Loading";
        qDebug() << "   - Try example 03: Simple State Management";
        qDebug() << "   - Try example 04: Basic Hot Reload";

        return app.exec();

    } catch (const std::exception& e) {
        qCritical() << "❌ Application error:" << e.what();
        return -1;
    }
}

#include "hello-world.moc"
