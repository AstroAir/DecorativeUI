#include "ComponentCommands.hpp"
#include <QApplication>
#include <QDebug>
#include <QListView>
#include <QTableView>
#include <QTreeView>
#include <QAbstractItemModel>
#include <QStandardItemModel>
#include <QStandardItem>

namespace DeclarativeUI {
namespace Command {
namespace ComponentCommands {

// ============================================================================
// LIST VIEW COMPONENTS
// ============================================================================

ListViewCommand::ListViewCommand(const CommandContext& context)
    : ICommand(nullptr) {}

CommandResult<QVariant> ListViewCommand::execute(const CommandContext& context) {
    auto widget_name = context.getParameter<QString>("widget");
    auto operation = context.getParameter<QString>("operation");

    if (!context.hasParameter("widget")) {
        return CommandResult<QVariant>(QString("Missing required parameter: widget"));
    }

    auto* listView = findListView(widget_name);
    if (!listView) {
        return CommandResult<QVariant>(QString("ListView '%1' not found").arg(widget_name));
    }

    widget_name_ = widget_name;
    old_index_ = listView->currentIndex();
    operation_ = operation;

    if (operation == "selectItem" || operation.isEmpty()) {
        auto row = context.getParameter<int>("row");
        if (context.hasParameter("row")) {
            auto model = listView->model();
            if (model) {
                new_index_ = model->index(row, 0);
                listView->setCurrentIndex(new_index_);
                return CommandResult<QVariant>(QString("ListView item selected successfully"));
            }
            return CommandResult<QVariant>(QString("ListView has no model"));
        }
        return CommandResult<QVariant>(QString("Missing row parameter for selectItem operation"));
    } else if (operation == "addItem") {
        auto text = context.getParameter<QString>("text");
        if (context.hasParameter("text")) {
            auto model = qobject_cast<QStandardItemModel*>(listView->model());
            if (model) {
                auto item = new QStandardItem(text);
                model->appendRow(item);
                return CommandResult<QVariant>(QString("ListView item added successfully"));
            }
            return CommandResult<QVariant>(QString("ListView model is not a QStandardItemModel"));
        }
        return CommandResult<QVariant>(QString("Missing text parameter for addItem operation"));
    } else if (operation == "removeItem") {
        auto row = context.getParameter<int>("row");
        if (context.hasParameter("row")) {
            auto model = qobject_cast<QStandardItemModel*>(listView->model());
            if (model) {
                model->removeRow(row);
                return CommandResult<QVariant>(QString("ListView item removed successfully"));
            }
            return CommandResult<QVariant>(QString("ListView model is not a QStandardItemModel"));
        }
        return CommandResult<QVariant>(QString("Missing row parameter for removeItem operation"));
    } else if (operation == "clearSelection") {
        listView->clearSelection();
        new_index_ = QModelIndex();
        return CommandResult<QVariant>(QString("ListView selection cleared successfully"));
    } else if (operation == "setModel") {
        // Note: Setting model requires more complex handling in real implementation
        return CommandResult<QVariant>(QString("setModel operation requires custom model implementation"));
    }

    return CommandResult<QVariant>(QString("Unknown operation: %1").arg(operation));
}

CommandResult<QVariant> ListViewCommand::undo(const CommandContext& context) {
    auto* listView = findListView(widget_name_);
    if (!listView) {
        return CommandResult<QVariant>(QString("ListView '%1' not found for undo").arg(widget_name_));
    }

    listView->setCurrentIndex(old_index_);
    return CommandResult<QVariant>(QString("ListView undo successful"));
}

bool ListViewCommand::canUndo(const CommandContext& context) const {
    return !widget_name_.isEmpty();
}

CommandMetadata ListViewCommand::getMetadata() const {
    return CommandMetadata("ListViewCommand", "Specialized command for ListView components");
}

QListView* ListViewCommand::findListView(const QString& name) {
    for (auto* widget : QApplication::allWidgets()) {
        if (auto* listView = qobject_cast<QListView*>(widget)) {
            if (listView->objectName() == name) {
                return listView;
            }
        }
    }
    return nullptr;
}

// ============================================================================
// TABLE VIEW COMPONENTS
// ============================================================================

TableViewCommand::TableViewCommand(const CommandContext& context)
    : ICommand(nullptr) {}

CommandResult<QVariant> TableViewCommand::execute(const CommandContext& context) {
    auto widget_name = context.getParameter<QString>("widget");
    auto operation = context.getParameter<QString>("operation");

    if (!context.hasParameter("widget")) {
        return CommandResult<QVariant>(QString("Missing required parameter: widget"));
    }

    auto* tableView = findTableView(widget_name);
    if (!tableView) {
        return CommandResult<QVariant>(QString("TableView '%1' not found").arg(widget_name));
    }

    widget_name_ = widget_name;
    old_index_ = tableView->currentIndex();
    operation_ = operation;

    if (operation == "selectCell" || operation.isEmpty()) {
        auto row = context.getParameter<int>("row");
        auto column = context.getParameter<int>("column");
        if (context.hasParameter("row") && context.hasParameter("column")) {
            auto model = tableView->model();
            if (model) {
                new_index_ = model->index(row, column);
                tableView->setCurrentIndex(new_index_);
                return CommandResult<QVariant>(QString("TableView cell selected successfully"));
            }
            return CommandResult<QVariant>(QString("TableView has no model"));
        }
        return CommandResult<QVariant>(QString("Missing row/column parameters for selectCell operation"));
    } else if (operation == "selectRow") {
        auto row = context.getParameter<int>("row");
        if (context.hasParameter("row")) {
            tableView->selectRow(row);
            return CommandResult<QVariant>(QString("TableView row selected successfully"));
        }
        return CommandResult<QVariant>(QString("Missing row parameter for selectRow operation"));
    } else if (operation == "selectColumn") {
        auto column = context.getParameter<int>("column");
        if (context.hasParameter("column")) {
            tableView->selectColumn(column);
            return CommandResult<QVariant>(QString("TableView column selected successfully"));
        }
        return CommandResult<QVariant>(QString("Missing column parameter for selectColumn operation"));
    } else if (operation == "setItemData") {
        auto row = context.getParameter<int>("row");
        auto column = context.getParameter<int>("column");
        auto data = context.getParameter<QVariant>("data");
        if (context.hasParameter("row") && context.hasParameter("column") && context.hasParameter("data")) {
            auto model = tableView->model();
            if (model) {
                auto index = model->index(row, column);
                model->setData(index, data);
                return CommandResult<QVariant>(QString("TableView item data set successfully"));
            }
            return CommandResult<QVariant>(QString("TableView has no model"));
        }
        return CommandResult<QVariant>(QString("Missing row/column/data parameters for setItemData operation"));
    }

    return CommandResult<QVariant>(QString("Unknown operation: %1").arg(operation));
}

CommandResult<QVariant> TableViewCommand::undo(const CommandContext& context) {
    auto* tableView = findTableView(widget_name_);
    if (!tableView) {
        return CommandResult<QVariant>(QString("TableView '%1' not found for undo").arg(widget_name_));
    }

    tableView->setCurrentIndex(old_index_);
    return CommandResult<QVariant>(QString("TableView undo successful"));
}

bool TableViewCommand::canUndo(const CommandContext& context) const {
    return !widget_name_.isEmpty();
}

CommandMetadata TableViewCommand::getMetadata() const {
    return CommandMetadata("TableViewCommand", "Specialized command for TableView components");
}

QTableView* TableViewCommand::findTableView(const QString& name) {
    for (auto* widget : QApplication::allWidgets()) {
        if (auto* tableView = qobject_cast<QTableView*>(widget)) {
            if (tableView->objectName() == name) {
                return tableView;
            }
        }
    }
    return nullptr;
}

// ============================================================================
// TREE VIEW COMPONENTS
// ============================================================================

TreeViewCommand::TreeViewCommand(const CommandContext& context)
    : ICommand(nullptr) {}

CommandResult<QVariant> TreeViewCommand::execute(const CommandContext& context) {
    auto widget_name = context.getParameter<QString>("widget");
    auto operation = context.getParameter<QString>("operation");

    if (!context.hasParameter("widget")) {
        return CommandResult<QVariant>(QString("Missing required parameter: widget"));
    }

    auto* treeView = findTreeView(widget_name);
    if (!treeView) {
        return CommandResult<QVariant>(QString("TreeView '%1' not found").arg(widget_name));
    }

    widget_name_ = widget_name;
    old_index_ = treeView->currentIndex();
    operation_ = operation;

    if (operation == "selectItem" || operation.isEmpty()) {
        auto row = context.getParameter<int>("row");
        auto column = context.getParameter<int>("column");
        if (context.hasParameter("row")) {
            auto model = treeView->model();
            if (model) {
                int col = context.hasParameter("column") ? column : 0;
                new_index_ = model->index(row, col);
                treeView->setCurrentIndex(new_index_);
                return CommandResult<QVariant>(QString("TreeView item selected successfully"));
            }
            return CommandResult<QVariant>(QString("TreeView has no model"));
        }
        return CommandResult<QVariant>(QString("Missing row parameter for selectItem operation"));
    } else if (operation == "expandItem") {
        auto row = context.getParameter<int>("row");
        if (context.hasParameter("row")) {
            auto model = treeView->model();
            if (model) {
                auto index = model->index(row, 0);
                treeView->expand(index);
                return CommandResult<QVariant>(QString("TreeView item expanded successfully"));
            }
            return CommandResult<QVariant>(QString("TreeView has no model"));
        }
        return CommandResult<QVariant>(QString("Missing row parameter for expandItem operation"));
    } else if (operation == "collapseItem") {
        auto row = context.getParameter<int>("row");
        if (context.hasParameter("row")) {
            auto model = treeView->model();
            if (model) {
                auto index = model->index(row, 0);
                treeView->collapse(index);
                return CommandResult<QVariant>(QString("TreeView item collapsed successfully"));
            }
            return CommandResult<QVariant>(QString("TreeView has no model"));
        }
        return CommandResult<QVariant>(QString("Missing row parameter for collapseItem operation"));
    } else if (operation == "setItemData") {
        auto row = context.getParameter<int>("row");
        auto column = context.getParameter<int>("column");
        auto data = context.getParameter<QVariant>("data");
        if (context.hasParameter("row") && context.hasParameter("column") && context.hasParameter("data")) {
            auto model = treeView->model();
            if (model) {
                auto index = model->index(row, column);
                model->setData(index, data);
                return CommandResult<QVariant>(QString("TreeView item data set successfully"));
            }
            return CommandResult<QVariant>(QString("TreeView has no model"));
        }
        return CommandResult<QVariant>(QString("Missing row/column/data parameters for setItemData operation"));
    }

    return CommandResult<QVariant>(QString("Unknown operation: %1").arg(operation));
}

CommandResult<QVariant> TreeViewCommand::undo(const CommandContext& context) {
    auto* treeView = findTreeView(widget_name_);
    if (!treeView) {
        return CommandResult<QVariant>(QString("TreeView '%1' not found for undo").arg(widget_name_));
    }

    treeView->setCurrentIndex(old_index_);
    return CommandResult<QVariant>(QString("TreeView undo successful"));
}

bool TreeViewCommand::canUndo(const CommandContext& context) const {
    return !widget_name_.isEmpty();
}

CommandMetadata TreeViewCommand::getMetadata() const {
    return CommandMetadata("TreeViewCommand", "Specialized command for TreeView components");
}

QTreeView* TreeViewCommand::findTreeView(const QString& name) {
    for (auto* widget : QApplication::allWidgets()) {
        if (auto* treeView = qobject_cast<QTreeView*>(widget)) {
            if (treeView->objectName() == name) {
                return treeView;
            }
        }
    }
    return nullptr;
}

}  // namespace ComponentCommands
}  // namespace Command
}  // namespace DeclarativeUI
