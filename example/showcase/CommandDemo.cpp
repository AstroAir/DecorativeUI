/**
 * @file CommandDemo.cpp
 * @brief Implementation of command system demonstration
 */

#include "CommandDemo.hpp"
#include <QDebug>

CommandDemo::CommandDemo(QWidget* parent)
    : QWidget(parent)
    , main_layout_(nullptr)
    , controls_group_(nullptr)
    , history_group_(nullptr)
    , execute_button_(nullptr)
    , undo_button_(nullptr)
    , redo_button_(nullptr)
    , clear_button_(nullptr)
    , history_list_(nullptr)
    , result_display_(nullptr)
{
    setupUI();
    createCommandControls();
    createCommandHistory();
    updateHistoryDisplay();
}

void CommandDemo::setupUI() {
    main_layout_ = new QVBoxLayout(this);
    main_layout_->setContentsMargins(8, 8, 8, 8);
}

void CommandDemo::createCommandControls() {
    controls_group_ = new QGroupBox("Command Controls");
    auto controls_layout = new QHBoxLayout(controls_group_);
    
    execute_button_ = new QPushButton("Execute Command");
    undo_button_ = new QPushButton("Undo");
    redo_button_ = new QPushButton("Redo");
    clear_button_ = new QPushButton("Clear History");
    
    undo_button_->setEnabled(false);
    redo_button_->setEnabled(false);
    
    controls_layout->addWidget(execute_button_);
    controls_layout->addWidget(undo_button_);
    controls_layout->addWidget(redo_button_);
    controls_layout->addWidget(clear_button_);
    controls_layout->addStretch();
    
    // Connect signals
    connect(execute_button_, &QPushButton::clicked, this, &CommandDemo::onExecuteCommand);
    connect(undo_button_, &QPushButton::clicked, this, &CommandDemo::onUndoCommand);
    connect(redo_button_, &QPushButton::clicked, this, &CommandDemo::onRedoCommand);
    connect(clear_button_, &QPushButton::clicked, this, &CommandDemo::onClearHistory);
    
    main_layout_->addWidget(controls_group_);
}

void CommandDemo::createCommandHistory() {
    history_group_ = new QGroupBox("Command History & Results");
    auto history_layout = new QVBoxLayout(history_group_);
    
    history_list_ = new QListWidget();
    history_list_->setMaximumHeight(150);
    
    result_display_ = new QTextEdit();
    result_display_->setReadOnly(true);
    result_display_->setMaximumHeight(100);
    result_display_->setPlainText("Ready to execute commands...");
    
    history_layout->addWidget(new QLabel("Command History:"));
    history_layout->addWidget(history_list_);
    history_layout->addWidget(new QLabel("Results:"));
    history_layout->addWidget(result_display_);
    
    main_layout_->addWidget(history_group_);
    
    // Add description
    auto description = new QLabel(
        "This demo shows the DeclarativeUI command system with undo/redo functionality. "
        "Commands are executed, tracked, and can be undone or redone as needed."
    );
    description->setWordWrap(true);
    description->setStyleSheet("QLabel { color: #666; font-style: italic; }");
    main_layout_->addWidget(description);
}

void CommandDemo::onExecuteCommand() {
    static int command_counter = 1;
    
    QString command_name = QString("Sample Command #%1").arg(command_counter++);
    QString result = QString("Executed: %1 at %2")
                        .arg(command_name)
                        .arg(QDateTime::currentDateTime().toString("hh:mm:ss"));
    
    // Add to history
    history_list_->addItem(command_name);
    history_list_->scrollToBottom();
    
    // Update result display
    result_display_->setPlainText(result);
    
    // Enable undo button
    undo_button_->setEnabled(true);
    redo_button_->setEnabled(false);
    
    qDebug() << "Command executed:" << command_name;
    
    updateHistoryDisplay();
}

void CommandDemo::onUndoCommand() {
    if (history_list_->count() == 0) return;
    
    // Get last command
    int last_row = history_list_->count() - 1;
    auto last_item = history_list_->item(last_row);
    QString command_name = last_item->text();
    
    // Mark as undone (change appearance)
    last_item->setText(command_name + " (undone)");
    last_item->setForeground(QColor("#999"));
    
    QString result = QString("Undone: %1 at %2")
                        .arg(command_name)
                        .arg(QDateTime::currentDateTime().toString("hh:mm:ss"));
    
    result_display_->setPlainText(result);
    
    // Enable redo button
    redo_button_->setEnabled(true);
    
    // Check if we can still undo
    bool can_undo = false;
    for (int i = 0; i < history_list_->count(); ++i) {
        if (!history_list_->item(i)->text().contains("(undone)")) {
            can_undo = true;
            break;
        }
    }
    undo_button_->setEnabled(can_undo);
    
    qDebug() << "Command undone:" << command_name;
    
    updateHistoryDisplay();
}

void CommandDemo::onRedoCommand() {
    // Find first undone command
    for (int i = 0; i < history_list_->count(); ++i) {
        auto item = history_list_->item(i);
        if (item->text().contains("(undone)")) {
            QString command_name = item->text().replace(" (undone)", "");
            item->setText(command_name);
            item->setForeground(QColor("#000"));
            
            QString result = QString("Redone: %1 at %2")
                                .arg(command_name)
                                .arg(QDateTime::currentDateTime().toString("hh:mm:ss"));
            
            result_display_->setPlainText(result);
            
            // Enable undo button
            undo_button_->setEnabled(true);
            
            // Check if we can still redo
            bool can_redo = false;
            for (int j = 0; j < history_list_->count(); ++j) {
                if (history_list_->item(j)->text().contains("(undone)")) {
                    can_redo = true;
                    break;
                }
            }
            redo_button_->setEnabled(can_redo);
            
            qDebug() << "Command redone:" << command_name;
            break;
        }
    }
    
    updateHistoryDisplay();
}

void CommandDemo::onClearHistory() {
    history_list_->clear();
    result_display_->setPlainText("Command history cleared.");
    
    undo_button_->setEnabled(false);
    redo_button_->setEnabled(false);
    
    qDebug() << "Command history cleared";
    
    updateHistoryDisplay();
}

void CommandDemo::updateHistoryDisplay() {
    // Update button states and display information
    int total_commands = history_list_->count();
    int undone_commands = 0;
    
    for (int i = 0; i < total_commands; ++i) {
        if (history_list_->item(i)->text().contains("(undone)")) {
            undone_commands++;
        }
    }
    
    int active_commands = total_commands - undone_commands;
    
    QString status = QString("Commands: %1 total, %2 active, %3 undone")
                        .arg(total_commands)
                        .arg(active_commands)
                        .arg(undone_commands);
    
    history_group_->setTitle(QString("Command History & Results - %1").arg(status));
}

#include "CommandDemo.moc"
