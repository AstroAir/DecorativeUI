/**
 * @file StateDemo.cpp
 * @brief Implementation of state management demonstration
 */

#include "StateDemo.hpp"
#include <QDebug>

StateDemo::StateDemo(QWidget* parent)
    : QWidget(parent)
    , main_layout_(nullptr)
    , demo_group_(nullptr)
    , state_display_(nullptr)
{
    setupUI();
    setupStateBindings();
    updateDisplay();
}

void StateDemo::setupUI() {
    main_layout_ = new QVBoxLayout(this);
    main_layout_->setContentsMargins(8, 8, 8, 8);
    
    demo_group_ = new QGroupBox("State Management Demo");
    main_layout_->addWidget(demo_group_);
    
    auto demo_layout = new QVBoxLayout(demo_group_);
    
    // Create controls
    auto controls_group = new QGroupBox("Controls");
    auto controls_layout = new QHBoxLayout(controls_group);
    
    auto counter_button = new QPushButton("Increment Counter");
    auto reset_button = new QPushButton("Reset State");
    auto name_input = new QLineEdit();
    name_input->setPlaceholderText("Enter name...");
    
    controls_layout->addWidget(new QLabel("Name:"));
    controls_layout->addWidget(name_input);
    controls_layout->addWidget(counter_button);
    controls_layout->addWidget(reset_button);
    
    // Create display
    state_display_ = new QTextEdit();
    state_display_->setReadOnly(true);
    state_display_->setMaximumHeight(200);
    
    demo_layout->addWidget(controls_group);
    demo_layout->addWidget(new QLabel("Current State:"));
    demo_layout->addWidget(state_display_);
    
    // Connect signals
    connect(counter_button, &QPushButton::clicked, this, &StateDemo::onStateChanged);
    connect(reset_button, &QPushButton::clicked, this, &StateDemo::onResetState);
    connect(name_input, &QLineEdit::textChanged, this, &StateDemo::onStateChanged);
}

void StateDemo::setupStateBindings() {
    state_manager_ = std::shared_ptr<Binding::StateManager>(
        &Binding::StateManager::instance(), 
        [](Binding::StateManager*) {} // Don't delete singleton
    );
    
    // Initialize demo state
    state_manager_->setState("demo.counter", 0);
    state_manager_->setState("demo.name", QString(""));
    state_manager_->setState("demo.timestamp", QDateTime::currentDateTime().toString());
}

void StateDemo::onStateChanged() {
    auto sender_obj = sender();
    
    if (auto button = qobject_cast<QPushButton*>(sender_obj)) {
        if (button->text() == "Increment Counter") {
            int current = state_manager_->getState("demo.counter", 0).toInt();
            state_manager_->setState("demo.counter", current + 1);
        }
    } else if (auto line_edit = qobject_cast<QLineEdit*>(sender_obj)) {
        state_manager_->setState("demo.name", line_edit->text());
    }
    
    state_manager_->setState("demo.timestamp", QDateTime::currentDateTime().toString());
    updateDisplay();
}

void StateDemo::onResetState() {
    state_manager_->setState("demo.counter", 0);
    state_manager_->setState("demo.name", QString(""));
    state_manager_->setState("demo.timestamp", QDateTime::currentDateTime().toString());
    
    // Reset UI controls
    auto name_input = findChild<QLineEdit*>();
    if (name_input) {
        name_input->clear();
    }
    
    updateDisplay();
}

void StateDemo::updateDisplay() {
    QString display_text;
    display_text += "State Values:\n";
    display_text += "=============\n";
    display_text += QString("Counter: %1\n").arg(state_manager_->getState("demo.counter", 0).toInt());
    display_text += QString("Name: '%1'\n").arg(state_manager_->getState("demo.name", QString("")).toString());
    display_text += QString("Last Updated: %1\n").arg(state_manager_->getState("demo.timestamp", QString("")).toString());
    display_text += "\n";
    display_text += "State Management Features:\n";
    display_text += "• Reactive updates\n";
    display_text += "• Type-safe state access\n";
    display_text += "• Automatic UI synchronization\n";
    display_text += "• Global state store\n";
    
    state_display_->setPlainText(display_text);
}

#include "StateDemo.moc"
