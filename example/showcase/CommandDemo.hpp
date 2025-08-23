/**
 * @file CommandDemo.hpp
 * @brief Command system demonstration widget
 */

#pragma once

#include <QGroupBox>
#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>
#include <memory>

#include "Command/CommandSystem.hpp"

using namespace DeclarativeUI;

class CommandDemo : public QWidget {
    Q_OBJECT

public:
    explicit CommandDemo(QWidget* parent = nullptr);

private slots:
    void onExecuteCommand();
    void onUndoCommand();
    void onRedoCommand();
    void onClearHistory();

private:
    void setupUI();
    void createCommandControls();
    void createCommandHistory();
    void updateHistoryDisplay();

    QVBoxLayout* main_layout_;
    QGroupBox* controls_group_;
    QGroupBox* history_group_;

    QPushButton* execute_button_;
    QPushButton* undo_button_;
    QPushButton* redo_button_;
    QPushButton* clear_button_;

    QListWidget* history_list_;
    QTextEdit* result_display_;

    std::unique_ptr<Command::CommandInvoker> command_invoker_;
};
