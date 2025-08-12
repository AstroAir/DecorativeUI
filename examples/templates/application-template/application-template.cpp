#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMenuBar>
#include <QStatusBar>
#include <QMainWindow>

// Include DeclarativeUI headers
#include "src/Core/DeclarativeBuilder.hpp"
#include "src/Components/Button.hpp"
#include "src/Components/Label.hpp"

/**
 * [Application Name] Application
 * 
 * [Brief description of what this application does and demonstrates]
 * 
 * Features:
 * - [Feature 1]
 * - [Feature 2]
 * - [Feature 3]
 * 
 * Architecture patterns demonstrated:
 * - [Pattern 1, e.g., MVVM]
 * - [Pattern 2, e.g., Command Pattern]
 * - [Pattern 3, e.g., Observer Pattern]
 */

class ApplicationMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    ApplicationMainWindow(QWidget *parent = nullptr) : QMainWindow(parent)
    {
        setupUI();
        setupMenus();
        setupStatusBar();
        connectSignals();
        loadSettings();
    }

private slots:
    void onNewAction()
    {
        // TODO: Implement new action
        statusBar()->showMessage("New action triggered", 2000);
    }
    
    void onOpenAction()
    {
        // TODO: Implement open action
        statusBar()->showMessage("Open action triggered", 2000);
    }
    
    void onSaveAction()
    {
        // TODO: Implement save action
        statusBar()->showMessage("Save action triggered", 2000);
    }
    
    void onExitAction()
    {
        close();
    }
    
    void onAboutAction()
    {
        // TODO: Implement about dialog
        statusBar()->showMessage("About action triggered", 2000);
    }

private:
    void setupUI()
    {
        setWindowTitle("[Application Name]");
        resize(800, 600);
        
        // Create central widget
        auto centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);
        
        auto mainLayout = new QVBoxLayout(centralWidget);
        
        // Title
        auto title = new QLabel("[Application Name]");
        title->setStyleSheet("font-size: 18px; font-weight: bold; margin: 10px;");
        title->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(title);
        
        // Main content area
        setupMainContent(mainLayout);
        
        // Control buttons
        setupControlButtons(mainLayout);
    }
    
    void setupMainContent(QVBoxLayout *mainLayout)
    {
        // TODO: Add your main application content here
        auto contentLabel = new QLabel("Main application content goes here");
        contentLabel->setStyleSheet("border: 1px solid gray; padding: 20px; margin: 10px;");
        contentLabel->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(contentLabel);
    }
    
    void setupControlButtons(QVBoxLayout *mainLayout)
    {
        auto buttonLayout = new QHBoxLayout();
        
        // Example buttons
        auto button1 = new QPushButton("Action 1");
        auto button2 = new QPushButton("Action 2");
        auto button3 = new QPushButton("Action 3");
        
        connect(button1, &QPushButton::clicked, [this]() {
            // TODO: Implement action 1
            statusBar()->showMessage("Action 1 executed", 2000);
        });
        
        connect(button2, &QPushButton::clicked, [this]() {
            // TODO: Implement action 2
            statusBar()->showMessage("Action 2 executed", 2000);
        });
        
        connect(button3, &QPushButton::clicked, [this]() {
            // TODO: Implement action 3
            statusBar()->showMessage("Action 3 executed", 2000);
        });
        
        buttonLayout->addWidget(button1);
        buttonLayout->addWidget(button2);
        buttonLayout->addWidget(button3);
        buttonLayout->addStretch();
        
        mainLayout->addLayout(buttonLayout);
    }
    
    void setupMenus()
    {
        // File menu
        auto fileMenu = menuBar()->addMenu("&File");
        
        auto newAction = fileMenu->addAction("&New");
        newAction->setShortcut(QKeySequence::New);
        connect(newAction, &QAction::triggered, this, &ApplicationMainWindow::onNewAction);
        
        auto openAction = fileMenu->addAction("&Open");
        openAction->setShortcut(QKeySequence::Open);
        connect(openAction, &QAction::triggered, this, &ApplicationMainWindow::onOpenAction);
        
        auto saveAction = fileMenu->addAction("&Save");
        saveAction->setShortcut(QKeySequence::Save);
        connect(saveAction, &QAction::triggered, this, &ApplicationMainWindow::onSaveAction);
        
        fileMenu->addSeparator();
        
        auto exitAction = fileMenu->addAction("E&xit");
        exitAction->setShortcut(QKeySequence::Quit);
        connect(exitAction, &QAction::triggered, this, &ApplicationMainWindow::onExitAction);
        
        // Help menu
        auto helpMenu = menuBar()->addMenu("&Help");
        
        auto aboutAction = helpMenu->addAction("&About");
        connect(aboutAction, &QAction::triggered, this, &ApplicationMainWindow::onAboutAction);
    }
    
    void setupStatusBar()
    {
        statusBar()->showMessage("Ready");
    }
    
    void connectSignals()
    {
        // TODO: Connect any additional signals and slots
    }
    
    void loadSettings()
    {
        // TODO: Load application settings
    }
    
    void saveSettings()
    {
        // TODO: Save application settings
    }

protected:
    void closeEvent(QCloseEvent *event) override
    {
        saveSettings();
        QMainWindow::closeEvent(event);
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Set application properties
    app.setApplicationName("[Application Name]");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("DeclarativeUI Examples");
    
    ApplicationMainWindow window;
    window.show();

    return app.exec();
}

#include "application-template.moc"

/*
 * Application Template Usage Instructions:
 * 
 * 1. Copy this template to your new application directory
 * 2. Rename the file to match your application name
 * 3. Replace [Application Name] placeholders with your actual application name
 * 4. Update the description, features, and architecture patterns
 * 5. Implement the TODO sections with your application-specific logic
 * 6. Add your application-specific UI components in setupMainContent()
 * 7. Implement the menu actions and button handlers
 * 8. Add settings loading/saving functionality
 * 9. Create a comprehensive README.md file
 * 10. Add your application to the CMakeLists.txt
 * 11. Test thoroughly on multiple platforms
 * 
 * Application Best Practices:
 * - Follow MVVM or similar architectural patterns
 * - Separate UI logic from business logic
 * - Implement proper error handling
 * - Provide user feedback for all actions
 * - Support keyboard shortcuts
 * - Save and restore application state
 * - Handle edge cases gracefully
 * - Write comprehensive documentation
 */
