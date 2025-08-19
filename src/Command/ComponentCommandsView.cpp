/**
 * @file ComponentCommandsView.cpp
 * @brief Implementation of view component commands for ListView, TableView, and TreeView.
 *
 * This file provides specialized command implementations for Qt's view components,
 * enabling programmatic control over item selection, model operations, and view state.
 * Each command class follows the Command pattern and supports undo operations.
 *
 * Key Features:
 * - Model-based operations with validation
 * - Selection management (single item, row, column)
 * - Data manipulation through model interfaces
 * - Comprehensive error handling and validation
 * - Undo/redo support for state changes
 *
 * Refactored for improved maintainability:
 * - Reduced cyclomatic complexity through operation handlers
 * - Enhanced documentation and code comments
 * - Consistent parameter validation patterns
 * - Single responsibility principle adherence
 */

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

/**
 * @brief Constructs a ListViewCommand for managing ListView operations.
 * @param context Command execution context (unused in constructor)
 */
ListViewCommand::ListViewCommand(const CommandContext& context)
    : ICommand(nullptr) {}

/**
 * @brief Executes a ListView operation based on the provided context.
 *
 * This method serves as the main entry point for ListView operations, routing
 * requests to appropriate handler methods based on the operation type.
 *
 * Supported operations:
 * - selectItem: Select an item by row index
 * - addItem: Add a new item with specified text
 * - removeItem: Remove an item by row index
 * - clearSelection: Clear current selection
 * - setModel: Set a new model (requires custom implementation)
 *
 * @param context Command context containing operation parameters
 * @return CommandResult indicating success/failure with descriptive message
 */
CommandResult<QVariant> ListViewCommand::execute(const CommandContext& context) {
    // Validate required widget parameter
    auto validationResult = validateRequiredParameter(context, "widget");
    if (!validationResult.isSuccess()) {
        return validationResult;
    }

    auto widget_name = context.getParameter<QString>("widget");
    auto* listView = findListView(widget_name);
    if (!listView) {
        return CommandResult<QVariant>(QString("ListView '%1' not found").arg(widget_name));
    }

    // Store state for undo functionality
    widget_name_ = widget_name;
    old_index_ = listView->currentIndex();
    operation_ = context.getParameter<QString>("operation");

    // Route to appropriate operation handler
    if (operation_ == "selectItem" || operation_.isEmpty()) {
        return handleSelectItem(context, listView);
    } else if (operation_ == "addItem") {
        return handleAddItem(context, listView);
    } else if (operation_ == "removeItem") {
        return handleRemoveItem(context, listView);
    } else if (operation_ == "clearSelection") {
        return handleClearSelection(context, listView);
    } else if (operation_ == "setModel") {
        return handleSetModel(context, listView);
    }

    return CommandResult<QVariant>(QString("Unknown operation: %1").arg(operation_));
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

/**
 * @brief Finds a ListView widget by its object name.
 * @param name The object name of the ListView to find
 * @return Pointer to the ListView if found, nullptr otherwise
 */
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

/**
 * @brief Validates that a required parameter exists in the command context.
 * @param context Command context to validate
 * @param paramName Name of the required parameter
 * @return CommandResult indicating success or failure with error message
 */
CommandResult<QVariant> ListViewCommand::validateRequiredParameter(const CommandContext& context, const QString& paramName) {
    if (!context.hasParameter(paramName)) {
        return CommandResult<QVariant>(QString("Missing required parameter: %1").arg(paramName));
    }
    return CommandResult<QVariant>(QVariant()); // Success
}

/**
 * @brief Validates that a model exists and is suitable for the requested operation.
 * @param model The model to validate
 * @param operation The operation that will be performed on the model
 * @return CommandResult indicating success or failure with error message
 */
CommandResult<QVariant> ListViewCommand::validateModelOperation(QAbstractItemModel* model, const QString& operation) {
    if (!model) {
        return CommandResult<QVariant>(QString("ListView has no model for %1 operation").arg(operation));
    }
    return CommandResult<QVariant>(QVariant()); // Success
}

/**
 * @brief Handles the selectItem operation for ListView.
 * @param context Command context containing row parameter
 * @param listView Target ListView widget
 * @return CommandResult indicating success or failure
 */
CommandResult<QVariant> ListViewCommand::handleSelectItem(const CommandContext& context, QListView* listView) {
    auto validationResult = validateRequiredParameter(context, "row");
    if (!validationResult.isSuccess()) {
        return CommandResult<QVariant>(QString("Missing row parameter for selectItem operation"));
    }

    auto row = context.getParameter<int>("row");
    auto model = listView->model();
    auto modelValidation = validateModelOperation(model, "selectItem");
    if (!modelValidation.isSuccess()) {
        return modelValidation;
    }

    new_index_ = model->index(row, 0);
    listView->setCurrentIndex(new_index_);
    return CommandResult<QVariant>(QString("ListView item selected successfully"));
}

/**
 * @brief Handles the addItem operation for ListView.
 * @param context Command context containing text parameter
 * @param listView Target ListView widget
 * @return CommandResult indicating success or failure
 */
CommandResult<QVariant> ListViewCommand::handleAddItem(const CommandContext& context, QListView* listView) {
    auto validationResult = validateRequiredParameter(context, "text");
    if (!validationResult.isSuccess()) {
        return CommandResult<QVariant>(QString("Missing text parameter for addItem operation"));
    }

    auto text = context.getParameter<QString>("text");
    auto model = qobject_cast<QStandardItemModel*>(listView->model());
    if (!model) {
        return CommandResult<QVariant>(QString("ListView model is not a QStandardItemModel"));
    }

    auto item = new QStandardItem(text);
    model->appendRow(item);
    return CommandResult<QVariant>(QString("ListView item added successfully"));
}

/**
 * @brief Handles the removeItem operation for ListView.
 * @param context Command context containing row parameter
 * @param listView Target ListView widget
 * @return CommandResult indicating success or failure
 */
CommandResult<QVariant> ListViewCommand::handleRemoveItem(const CommandContext& context, QListView* listView) {
    auto validationResult = validateRequiredParameter(context, "row");
    if (!validationResult.isSuccess()) {
        return CommandResult<QVariant>(QString("Missing row parameter for removeItem operation"));
    }

    auto row = context.getParameter<int>("row");
    auto model = qobject_cast<QStandardItemModel*>(listView->model());
    if (!model) {
        return CommandResult<QVariant>(QString("ListView model is not a QStandardItemModel"));
    }

    model->removeRow(row);
    return CommandResult<QVariant>(QString("ListView item removed successfully"));
}

/**
 * @brief Handles the clearSelection operation for ListView.
 * @param context Command context (unused for this operation)
 * @param listView Target ListView widget
 * @return CommandResult indicating success
 */
CommandResult<QVariant> ListViewCommand::handleClearSelection(const CommandContext& context, QListView* listView) {
    Q_UNUSED(context) // clearSelection doesn't need additional parameters
    listView->clearSelection();
    new_index_ = QModelIndex();
    return CommandResult<QVariant>(QString("ListView selection cleared successfully"));
}

/**
 * @brief Handles the setModel operation for ListView.
 * @param context Command context (unused for this operation)
 * @param listView Target ListView widget
 * @return CommandResult indicating that custom implementation is required
 */
CommandResult<QVariant> ListViewCommand::handleSetModel(const CommandContext& context, QListView* listView) {
    Q_UNUSED(context)
    Q_UNUSED(listView)
    // Note: Setting model requires more complex handling in real implementation
    return CommandResult<QVariant>(QString("setModel operation requires custom model implementation"));
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
