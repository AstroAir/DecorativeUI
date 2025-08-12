#include <QApplication>
#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#include "Binding/StateManager.hpp"
#include "Command/CommandBuilder.hpp"
#include "Command/CommandSystem.hpp"
#include "Command/UICommand.hpp"

using namespace DeclarativeUI;

class CommandBasicsExample : public QMainWindow {
    Q_OBJECT

public:
    CommandBasicsExample(QWidget* parent = nullptr) : QMainWindow(parent) {
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
        auto titleLabel = new QLabel("🚀 Command System Basics");
        titleLabel->setAlignment(Qt::AlignCenter);
        titleLabel->setStyleSheet(
            "font-size: 18px; font-weight: bold; margin: 10px;");

        // Counter display
        counterLabel = new QLabel("Count: 0");
        counterLabel->setAlignment(Qt::AlignCenter);
        counterLabel->setStyleSheet(
            "font-size: 16px; font-weight: bold; margin: 10px;");

        // Buttons
        auto buttonLayout = new QHBoxLayout();
        auto decrementBtn = new QPushButton("- Decrease");
        auto resetBtn = new QPushButton("Reset");
        auto incrementBtn = new QPushButton("+ Increase");

        connect(decrementBtn, &QPushButton::clicked, this,
                &CommandBasicsExample::decrement);
        connect(resetBtn, &QPushButton::clicked, this,
                &CommandBasicsExample::reset);
        connect(incrementBtn, &QPushButton::clicked, this,
                &CommandBasicsExample::increment);

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

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    qDebug() << "🚀 Starting Command System Basics Example...";

    CommandBasicsExample window;
    window.show();

    return app.exec();
}

#include "08_command_basics.moc"
