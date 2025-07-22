#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QWidget>
#include <QDebug>

#include "Command/CommandSystem.hpp"
#include "Command/UICommand.hpp"
#include "Command/CommandBuilder.hpp"
#include "Binding/StateManager.hpp"

using namespace DeclarativeUI;

class SimpleCommandExample : public QMainWindow {
    Q_OBJECT

public:
    SimpleCommandExample(QWidget* parent = nullptr) : QMainWindow(parent) {
        setupUI();
        setupState();
    }

private slots:
    void increment() {
        counter++;
        updateDisplay();
    }
    
    void decrement() {
        counter--;
        updateDisplay();
    }
    
    void reset() {
        counter = 0;
        updateDisplay();
    }

private:
    void setupUI() {
        auto centralWidget = new QWidget();
        auto layout = new QVBoxLayout(centralWidget);
        
        // Title
        auto titleLabel = new QLabel("🚀 Simple Command System Example");
        titleLabel->setAlignment(Qt::AlignCenter);
        titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; margin: 10px;");
        
        // Counter display
        counterLabel = new QLabel("Count: 0");
        counterLabel->setAlignment(Qt::AlignCenter);
        counterLabel->setStyleSheet("font-size: 16px; font-weight: bold; margin: 10px;");
        
        // Buttons
        auto buttonLayout = new QHBoxLayout();
        auto decrementBtn = new QPushButton("- Decrease");
        auto resetBtn = new QPushButton("Reset");
        auto incrementBtn = new QPushButton("+ Increase");
        
        connect(decrementBtn, &QPushButton::clicked, this, &SimpleCommandExample::decrement);
        connect(resetBtn, &QPushButton::clicked, this, &SimpleCommandExample::reset);
        connect(incrementBtn, &QPushButton::clicked, this, &SimpleCommandExample::increment);
        
        buttonLayout->addWidget(decrementBtn);
        buttonLayout->addWidget(resetBtn);
        buttonLayout->addWidget(incrementBtn);
        
        layout->addWidget(titleLabel);
        layout->addWidget(counterLabel);
        layout->addLayout(buttonLayout);
        
        setCentralWidget(centralWidget);
        setWindowTitle("Command System Basics");
        resize(400, 200);
    }
    
    void setupState() {
        auto& stateManager = DeclarativeUI::Binding::StateManager::instance();
        stateManager.setState("counter.value", 0);
    }
    
    void updateDisplay() {
        counterLabel->setText(QString("Count: %1").arg(counter));
        
        auto& stateManager = DeclarativeUI::Binding::StateManager::instance();
        stateManager.setState("counter.value", counter);
    }

private:
    QLabel* counterLabel = nullptr;
    int counter = 0;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    qDebug() << "🚀 Starting Simple Command System Example...";
    
    SimpleCommandExample window;
    window.show();
    
    return app.exec();
}

#include "08_command_basics_simple.moc"
