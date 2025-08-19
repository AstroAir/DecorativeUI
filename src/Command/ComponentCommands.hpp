#pragma once

/**
 * @file ComponentCommands.hpp
 * @brief Component-specific command classes for DeclarativeUI components.
 *
 * This file contains specialized command classes for each UI component type,
 * providing component-specific functionality beyond generic property setting.
 */

#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QRadioButton>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QSlider>
#include <QProgressBar>
#include <QComboBox>
#include <QListView>
#include <QTableView>
#include <QTreeView>
#include <QGroupBox>
#include <QTabWidget>
#include <QScrollArea>
#include <QSplitter>
#include <QDial>
#include <QCalendarWidget>
#include <QLCDNumber>
#include <QFrame>
#include <QToolButton>
#include <QDateTimeEdit>
#include <QProgressBar>
#include <QListView>
#include <QTreeView>
#include <QDockWidget>
#include <QMenuBar>
#include <QStatusBar>
#include <QToolBar>

#include "CommandSystem.hpp"

namespace DeclarativeUI {
namespace Command {
namespace ComponentCommands {

using namespace DeclarativeUI::Command;

// ============================================================================
// COMMON HELPER FUNCTIONS
// ============================================================================

/**
 * @brief Generic template function to find a widget by name and type.
 * @tparam T The widget type to search for (e.g., QDoubleSpinBox, QDial)
 * @param name The object name of the widget to find
 * @return Pointer to the widget if found, nullptr otherwise
 *
 * This template eliminates the need for separate findWidget functions
 * in each command class, reducing code duplication significantly.
 */
template<typename T>
T* findWidget(const QString& name) {
    for (auto* widget : QApplication::allWidgets()) {
        if (auto* typedWidget = qobject_cast<T*>(widget)) {
            if (typedWidget->objectName() == name) {
                return typedWidget;
            }
        }
    }
    return nullptr;
}

/**
 * @brief Validates that required parameters exist in the command context.
 * @param context The command context to validate
 * @param requiredParams List of required parameter names
 * @return CommandResult with error message if validation fails, empty result if success
 */
CommandResult<QVariant> validateRequiredParameters(const CommandContext& context,
                                                   const QStringList& requiredParams);

/**
 * @brief Creates a standardized error result for missing widget.
 * @param widgetType The type of widget (e.g., "DoubleSpinBox")
 * @param widgetName The name of the widget that wasn't found
 * @return CommandResult with formatted error message
 */
CommandResult<QVariant> createWidgetNotFoundError(const QString& widgetType,
                                                  const QString& widgetName);

/**
 * @brief Creates a standardized success result.
 * @param widgetType The type of widget (e.g., "DoubleSpinBox")
 * @param operation The operation that was performed
 * @return CommandResult with formatted success message
 */
CommandResult<QVariant> createSuccessResult(const QString& widgetType,
                                           const QString& operation);

// ============================================================================
// BUTTON COMPONENTS
// ============================================================================

/**
 * @class ButtonCommand
 * @brief Specialized command for Button components with click handling.
 */
class ButtonCommand : public DeclarativeUI::Command::ICommand {
    Q_OBJECT

public:
    explicit ButtonCommand(const CommandContext& context);
    CommandResult<QVariant> execute(const CommandContext& context) override;
    CommandMetadata getMetadata() const override;

private:
    QPushButton* findButton(const QString& name);
};

/**
 * @class CheckBoxCommand
 * @brief Specialized command for CheckBox components with toggle handling.
 */
class CheckBoxCommand : public DeclarativeUI::Command::ICommand {
    Q_OBJECT

public:
    explicit CheckBoxCommand(const CommandContext& context);
    CommandResult<QVariant> execute(const CommandContext& context) override;
    CommandResult<QVariant> undo(const CommandContext& context) override;
    bool canUndo(const CommandContext& context) const override;
    CommandMetadata getMetadata() const override;

private:
    QCheckBox* findCheckBox(const QString& name);
    QString widget_name_;
    bool old_state_;
    bool new_state_;
};

/**
 * @class RadioButtonCommand
 * @brief Specialized command for RadioButton components.
 */
class RadioButtonCommand : public DeclarativeUI::Command::ICommand {
    Q_OBJECT

public:
    explicit RadioButtonCommand(const CommandContext& context);
    CommandResult<QVariant> execute(const CommandContext& context) override;
    CommandResult<QVariant> undo(const CommandContext& context) override;
    bool canUndo(const CommandContext& context) const override;
    CommandMetadata getMetadata() const override;

private:
    QRadioButton* findRadioButton(const QString& name);
    QString widget_name_;
    bool old_state_;
};

// ============================================================================
// TEXT COMPONENTS
// ============================================================================

/**
 * @class TextEditCommand
 * @brief Specialized command for TextEdit components with text manipulation.
 */
class TextEditCommand : public DeclarativeUI::Command::ICommand {
    Q_OBJECT

public:
    explicit TextEditCommand(const CommandContext& context);
    CommandResult<QVariant> execute(const CommandContext& context) override;
    CommandResult<QVariant> undo(const CommandContext& context) override;
    bool canUndo(const CommandContext& context) const override;
    CommandMetadata getMetadata() const override;

private:
    QTextEdit* findTextEdit(const QString& name);
    QString widget_name_;
    QString old_text_;
    QString new_text_;
    QString operation_;
};

/**
 * @class LineEditCommand
 * @brief Specialized command for LineEdit components.
 */
class LineEditCommand : public DeclarativeUI::Command::ICommand {
    Q_OBJECT

public:
    explicit LineEditCommand(const CommandContext& context);
    CommandResult<QVariant> execute(const CommandContext& context) override;
    CommandResult<QVariant> undo(const CommandContext& context) override;
    bool canUndo(const CommandContext& context) const override;
    CommandMetadata getMetadata() const override;

private:
    QLineEdit* findLineEdit(const QString& name);
    QString widget_name_;
    QString old_text_;
    QString new_text_;
    QString operation_;
};

/**
 * @class LabelCommand
 * @brief Specialized command for Label components.
 */
class LabelCommand : public DeclarativeUI::Command::ICommand {
    Q_OBJECT

public:
    explicit LabelCommand(const CommandContext& context);
    CommandResult<QVariant> execute(const CommandContext& context) override;
    CommandResult<QVariant> undo(const CommandContext& context) override;
    bool canUndo(const CommandContext& context) const override;
    CommandMetadata getMetadata() const override;

private:
    QLabel* findLabel(const QString& name);
    QString widget_name_;
    QString old_text_;
    QString new_text_;
};

// ============================================================================
// INPUT COMPONENTS
// ============================================================================

/**
 * @class SpinBoxCommand
 * @brief Specialized command for SpinBox components with value manipulation.
 */
class SpinBoxCommand : public DeclarativeUI::Command::ICommand {
    Q_OBJECT

public:
    explicit SpinBoxCommand(const CommandContext& context);
    CommandResult<QVariant> execute(const CommandContext& context) override;
    CommandResult<QVariant> undo(const CommandContext& context) override;
    bool canUndo(const CommandContext& context) const override;
    CommandMetadata getMetadata() const override;

private:
    QSpinBox* findSpinBox(const QString& name);
    QString widget_name_;
    int old_value_;
    int new_value_;
    QString operation_;
};

/**
 * @class SliderCommand
 * @brief Specialized command for Slider components.
 */
class SliderCommand : public DeclarativeUI::Command::ICommand {
    Q_OBJECT

public:
    explicit SliderCommand(const CommandContext& context);
    CommandResult<QVariant> execute(const CommandContext& context) override;
    CommandResult<QVariant> undo(const CommandContext& context) override;
    bool canUndo(const CommandContext& context) const override;
    CommandMetadata getMetadata() const override;

private:
    QSlider* findSlider(const QString& name);
    QString widget_name_;
    int old_value_;
    int new_value_;
    QString operation_;
};

/**
 * @class DoubleSpinBoxCommand
 * @brief Specialized command for DoubleSpinBox components with decimal value manipulation.
 */
class DoubleSpinBoxCommand : public DeclarativeUI::Command::ICommand {
    Q_OBJECT

public:
    explicit DoubleSpinBoxCommand(const CommandContext& context);
    CommandResult<QVariant> execute(const CommandContext& context) override;
    CommandResult<QVariant> undo(const CommandContext& context) override;
    bool canUndo(const CommandContext& context) const override;
    CommandMetadata getMetadata() const override;

private:
    QDoubleSpinBox* findDoubleSpinBox(const QString& name);

    // Operation-specific handlers to reduce cyclomatic complexity
    CommandResult<QVariant> handleSetValue(const CommandContext& context, QDoubleSpinBox* widget);
    CommandResult<QVariant> handleStepUp(const CommandContext& context, QDoubleSpinBox* widget);
    CommandResult<QVariant> handleStepDown(const CommandContext& context, QDoubleSpinBox* widget);
    CommandResult<QVariant> handleSetRange(const CommandContext& context, QDoubleSpinBox* widget);
    CommandResult<QVariant> handleSetDecimals(const CommandContext& context, QDoubleSpinBox* widget);

    QString widget_name_;
    double old_value_;
    double new_value_;
    QString operation_;
};

/**
 * @class DialCommand
 * @brief Specialized command for Dial components.
 */
class DialCommand : public DeclarativeUI::Command::ICommand {
    Q_OBJECT

public:
    explicit DialCommand(const CommandContext& context);
    CommandResult<QVariant> execute(const CommandContext& context) override;
    CommandResult<QVariant> undo(const CommandContext& context) override;
    bool canUndo(const CommandContext& context) const override;
    CommandMetadata getMetadata() const override;

private:
    QDial* findDial(const QString& name);

    // Operation-specific handlers to reduce cyclomatic complexity
    CommandResult<QVariant> handleSetValue(const CommandContext& context, QDial* widget);
    CommandResult<QVariant> handleSetRange(const CommandContext& context, QDial* widget);
    CommandResult<QVariant> handleSetNotchesVisible(const CommandContext& context, QDial* widget);

    QString widget_name_;
    int old_value_;
    int new_value_;
    QString operation_;
};

/**
 * @class DateTimeEditCommand
 * @brief Specialized command for DateTimeEdit components.
 */
class DateTimeEditCommand : public DeclarativeUI::Command::ICommand {
    Q_OBJECT

public:
    explicit DateTimeEditCommand(const CommandContext& context);
    CommandResult<QVariant> execute(const CommandContext& context) override;
    CommandResult<QVariant> undo(const CommandContext& context) override;
    bool canUndo(const CommandContext& context) const override;
    CommandMetadata getMetadata() const override;

private:
    QDateTimeEdit* findDateTimeEdit(const QString& name);
    QString widget_name_;
    QDateTime old_datetime_;
    QDateTime new_datetime_;
    QString operation_;
};

/**
 * @class ProgressBarCommand
 * @brief Specialized command for ProgressBar components.
 */
class ProgressBarCommand : public DeclarativeUI::Command::ICommand {
    Q_OBJECT

public:
    explicit ProgressBarCommand(const CommandContext& context);
    CommandResult<QVariant> execute(const CommandContext& context) override;
    CommandResult<QVariant> undo(const CommandContext& context) override;
    bool canUndo(const CommandContext& context) const override;
    CommandMetadata getMetadata() const override;

private:
    QProgressBar* findProgressBar(const QString& name);
    QString widget_name_;
    int old_value_;
    int new_value_;
    QString operation_;
};

// ============================================================================
// DISPLAY COMPONENTS
// ============================================================================

/**
 * @class LCDNumberCommand
 * @brief Specialized command for LCDNumber components.
 */
class LCDNumberCommand : public DeclarativeUI::Command::ICommand {
    Q_OBJECT

public:
    explicit LCDNumberCommand(const CommandContext& context);
    CommandResult<QVariant> execute(const CommandContext& context) override;
    CommandResult<QVariant> undo(const CommandContext& context) override;
    bool canUndo(const CommandContext& context) const override;
    CommandMetadata getMetadata() const override;

private:
    QLCDNumber* findLCDNumber(const QString& name);
    QString widget_name_;
    double old_value_;
    double new_value_;
    QString operation_;
};

/**
 * @class CalendarCommand
 * @brief Specialized command for Calendar components.
 */
class CalendarCommand : public DeclarativeUI::Command::ICommand {
    Q_OBJECT

public:
    explicit CalendarCommand(const CommandContext& context);
    CommandResult<QVariant> execute(const CommandContext& context) override;
    CommandResult<QVariant> undo(const CommandContext& context) override;
    bool canUndo(const CommandContext& context) const override;
    CommandMetadata getMetadata() const override;

private:
    QCalendarWidget* findCalendar(const QString& name);
    QString widget_name_;
    QDate old_date_;
    QDate new_date_;
    QString operation_;
};

// ============================================================================
// VIEW COMPONENTS
// ============================================================================

/**
 * @class ListViewCommand
 * @brief Specialized command for ListView components with model and selection operations.
 *
 * Refactored to improve maintainability by breaking down complex execute method
 * into smaller, focused operation handlers with comprehensive documentation.
 * Provides operations for item selection, addition, removal, and model management.
 */
class ListViewCommand : public DeclarativeUI::Command::ICommand {
    Q_OBJECT

public:
    explicit ListViewCommand(const CommandContext& context);
    CommandResult<QVariant> execute(const CommandContext& context) override;
    CommandResult<QVariant> undo(const CommandContext& context) override;
    bool canUndo(const CommandContext& context) const override;
    CommandMetadata getMetadata() const override;

private:
    // Widget finder and validation methods
    QListView* findListView(const QString& name);
    CommandResult<QVariant> validateRequiredParameter(const CommandContext& context, const QString& paramName);
    CommandResult<QVariant> validateModelOperation(QAbstractItemModel* model, const QString& operation);

    // Operation-specific handlers to reduce cyclomatic complexity
    CommandResult<QVariant> handleSelectItem(const CommandContext& context, QListView* listView);
    CommandResult<QVariant> handleAddItem(const CommandContext& context, QListView* listView);
    CommandResult<QVariant> handleRemoveItem(const CommandContext& context, QListView* listView);
    CommandResult<QVariant> handleClearSelection(const CommandContext& context, QListView* listView);
    CommandResult<QVariant> handleSetModel(const CommandContext& context, QListView* listView);

    // State management for undo functionality
    QString widget_name_;
    QModelIndex old_index_;
    QModelIndex new_index_;
    QString operation_;
};

/**
 * @class TableViewCommand
 * @brief Specialized command for TableView components with model and selection operations.
 *
 * Refactored to improve maintainability by breaking down complex execute method
 * into smaller, focused operation handlers with comprehensive documentation.
 * Provides operations for cell/row/column selection, data manipulation, and model management.
 */
class TableViewCommand : public DeclarativeUI::Command::ICommand {
    Q_OBJECT

public:
    explicit TableViewCommand(const CommandContext& context);
    CommandResult<QVariant> execute(const CommandContext& context) override;
    CommandResult<QVariant> undo(const CommandContext& context) override;
    bool canUndo(const CommandContext& context) const override;
    CommandMetadata getMetadata() const override;

private:
    // Widget finder and validation methods
    QTableView* findTableView(const QString& name);
    CommandResult<QVariant> validateRequiredParameter(const CommandContext& context, const QString& paramName);
    CommandResult<QVariant> validateModelOperation(QAbstractItemModel* model, const QString& operation);

    // Operation-specific handlers to reduce cyclomatic complexity
    CommandResult<QVariant> handleSelectCell(const CommandContext& context, QTableView* tableView);
    CommandResult<QVariant> handleSelectRow(const CommandContext& context, QTableView* tableView);
    CommandResult<QVariant> handleSelectColumn(const CommandContext& context, QTableView* tableView);
    CommandResult<QVariant> handleSetItemData(const CommandContext& context, QTableView* tableView);
    CommandResult<QVariant> handleClearSelection(const CommandContext& context, QTableView* tableView);

    // State management for undo functionality
    QString widget_name_;
    QModelIndex old_index_;
    QModelIndex new_index_;
    QString operation_;
};

/**
 * @class TreeViewCommand
 * @brief Specialized command for TreeView components with model and selection operations.
 *
 * Refactored to improve maintainability by breaking down complex execute method
 * into smaller, focused operation handlers with comprehensive documentation.
 * Provides operations for item selection, expansion/collapse, and hierarchical model management.
 */
class TreeViewCommand : public DeclarativeUI::Command::ICommand {
    Q_OBJECT

public:
    explicit TreeViewCommand(const CommandContext& context);
    CommandResult<QVariant> execute(const CommandContext& context) override;
    CommandResult<QVariant> undo(const CommandContext& context) override;
    bool canUndo(const CommandContext& context) const override;
    CommandMetadata getMetadata() const override;

private:
    // Widget finder and validation methods
    QTreeView* findTreeView(const QString& name);
    CommandResult<QVariant> validateRequiredParameter(const CommandContext& context, const QString& paramName);
    CommandResult<QVariant> validateModelOperation(QAbstractItemModel* model, const QString& operation);

    // Operation-specific handlers to reduce cyclomatic complexity
    CommandResult<QVariant> handleSelectItem(const CommandContext& context, QTreeView* treeView);
    CommandResult<QVariant> handleExpandItem(const CommandContext& context, QTreeView* treeView);
    CommandResult<QVariant> handleCollapseItem(const CommandContext& context, QTreeView* treeView);
    CommandResult<QVariant> handleExpandAll(const CommandContext& context, QTreeView* treeView);
    CommandResult<QVariant> handleCollapseAll(const CommandContext& context, QTreeView* treeView);
    CommandResult<QVariant> handleClearSelection(const CommandContext& context, QTreeView* treeView);

    // State management for undo functionality
    QString widget_name_;
    QModelIndex old_index_;
    QModelIndex new_index_;
    QString operation_;
};

// ============================================================================
// CONTAINER COMPONENTS
// ============================================================================

/**
 * @class TabWidgetCommand
 * @brief Specialized command for TabWidget components.
 */
class TabWidgetCommand : public DeclarativeUI::Command::ICommand {
    Q_OBJECT

public:
    explicit TabWidgetCommand(const CommandContext& context);
    CommandResult<QVariant> execute(const CommandContext& context) override;
    CommandResult<QVariant> undo(const CommandContext& context) override;
    bool canUndo(const CommandContext& context) const override;
    CommandMetadata getMetadata() const override;

private:
    QTabWidget* findTabWidget(const QString& name);
    QString widget_name_;
    int old_index_;
    int new_index_;
    QString operation_;
};

/**
 * @class ComboBoxCommand
 * @brief Specialized command for ComboBox components.
 */
class ComboBoxCommand : public DeclarativeUI::Command::ICommand {
    Q_OBJECT

public:
    explicit ComboBoxCommand(const CommandContext& context);
    CommandResult<QVariant> execute(const CommandContext& context) override;
    CommandResult<QVariant> undo(const CommandContext& context) override;
    bool canUndo(const CommandContext& context) const override;
    CommandMetadata getMetadata() const override;

private:
    QComboBox* findComboBox(const QString& name);
    QString widget_name_;
    int old_index_;
    QString old_text_;
    QString operation_;
};

/**
 * @class GroupBoxCommand
 * @brief Specialized command for GroupBox components.
 */
class GroupBoxCommand : public DeclarativeUI::Command::ICommand {
    Q_OBJECT

public:
    explicit GroupBoxCommand(const CommandContext& context);
    CommandResult<QVariant> execute(const CommandContext& context) override;
    CommandResult<QVariant> undo(const CommandContext& context) override;
    bool canUndo(const CommandContext& context) const override;
    CommandMetadata getMetadata() const override;

private:
    QGroupBox* findGroupBox(const QString& name);
    QString widget_name_;
    QString old_title_;
    bool old_checked_;
    QString operation_;
};

/**
 * @class FrameCommand
 * @brief Specialized command for Frame components.
 */
class FrameCommand : public DeclarativeUI::Command::ICommand {
    Q_OBJECT

public:
    explicit FrameCommand(const CommandContext& context);
    CommandResult<QVariant> execute(const CommandContext& context) override;
    CommandMetadata getMetadata() const override;

private:
    QFrame* findFrame(const QString& name);
};

/**
 * @class ScrollAreaCommand
 * @brief Specialized command for ScrollArea components.
 */
class ScrollAreaCommand : public DeclarativeUI::Command::ICommand {
    Q_OBJECT

public:
    explicit ScrollAreaCommand(const CommandContext& context);
    CommandResult<QVariant> execute(const CommandContext& context) override;
    CommandMetadata getMetadata() const override;

private:
    QScrollArea* findScrollArea(const QString& name);
};

/**
 * @class SplitterCommand
 * @brief Specialized command for Splitter components.
 */
class SplitterCommand : public DeclarativeUI::Command::ICommand {
    Q_OBJECT

public:
    explicit SplitterCommand(const CommandContext& context);
    CommandResult<QVariant> execute(const CommandContext& context) override;
    CommandResult<QVariant> undo(const CommandContext& context) override;
    bool canUndo(const CommandContext& context) const override;
    CommandMetadata getMetadata() const override;

private:
    QSplitter* findSplitter(const QString& name);
    QString widget_name_;
    QList<int> old_sizes_;
    QList<int> new_sizes_;
    QString operation_;
};

/**
 * @class DockWidgetCommand
 * @brief Specialized command for DockWidget components.
 */
class DockWidgetCommand : public DeclarativeUI::Command::ICommand {
    Q_OBJECT

public:
    explicit DockWidgetCommand(const CommandContext& context);
    CommandResult<QVariant> execute(const CommandContext& context) override;
    CommandResult<QVariant> undo(const CommandContext& context) override;
    bool canUndo(const CommandContext& context) const override;
    CommandMetadata getMetadata() const override;

private:
    QDockWidget* findDockWidget(const QString& name);
    QString widget_name_;
    bool old_floating_;
    QString operation_;
};

// ============================================================================
// MENU/TOOLBAR COMPONENTS
// ============================================================================

/**
 * @class MenuBarCommand
 * @brief Specialized command for MenuBar components.
 *
 * Refactored to improve maintainability by breaking down complex execute method
 * into smaller, focused operation handlers with reduced cyclomatic complexity.
 */
class MenuBarCommand : public DeclarativeUI::Command::ICommand {
    Q_OBJECT

public:
    explicit MenuBarCommand(const CommandContext& context);
    CommandResult<QVariant> execute(const CommandContext& context) override;
    CommandMetadata getMetadata() const override;

private:
    // Widget finder method
    QMenuBar* findMenuBar(const QString& name);

    // Parameter validation helper
    CommandResult<QVariant> validateRequiredParameter(const CommandContext& context, const QString& paramName);

    // Operation-specific handlers to reduce cyclomatic complexity
    CommandResult<QVariant> handleAddMenu(const CommandContext& context, QMenuBar* menuBar);
    CommandResult<QVariant> handleRemoveMenu(const CommandContext& context, QMenuBar* menuBar);
    CommandResult<QVariant> handleSetNativeMenuBar(const CommandContext& context, QMenuBar* menuBar);
};

/**
 * @class StatusBarCommand
 * @brief Specialized command for StatusBar components.
 *
 * Refactored to improve maintainability by breaking down complex execute method
 * into smaller, focused operation handlers with reduced cyclomatic complexity.
 */
class StatusBarCommand : public DeclarativeUI::Command::ICommand {
    Q_OBJECT

public:
    explicit StatusBarCommand(const CommandContext& context);
    CommandResult<QVariant> execute(const CommandContext& context) override;
    CommandMetadata getMetadata() const override;

private:
    // Widget finder methods
    QStatusBar* findStatusBar(const QString& name);
    QWidget* findWidgetByName(const QString& name, QWidget* parent = nullptr);

    // Parameter validation helper
    CommandResult<QVariant> validateRequiredParameter(const CommandContext& context, const QString& paramName);

    // Operation-specific handlers to reduce cyclomatic complexity
    CommandResult<QVariant> handleShowMessage(const CommandContext& context, QStatusBar* statusBar);
    CommandResult<QVariant> handleClearMessage(const CommandContext& context, QStatusBar* statusBar);
    CommandResult<QVariant> handleAddWidget(const CommandContext& context, QStatusBar* statusBar);
    CommandResult<QVariant> handleRemoveWidget(const CommandContext& context, QStatusBar* statusBar);
};

/**
 * @class ToolBarCommand
 * @brief Specialized command for ToolBar components.
 */
class ToolBarCommand : public DeclarativeUI::Command::ICommand {
    Q_OBJECT

public:
    explicit ToolBarCommand(const CommandContext& context);
    CommandResult<QVariant> execute(const CommandContext& context) override;
    CommandResult<QVariant> undo(const CommandContext& context) override;
    bool canUndo(const CommandContext& context) const override;
    CommandMetadata getMetadata() const override;

private:
    QToolBar* findToolBar(const QString& name);
    QString widget_name_;
    Qt::Orientation old_orientation_;
    QString operation_;
};

/**
 * @class ToolButtonCommand
 * @brief Specialized command for ToolButton components.
 */
class ToolButtonCommand : public DeclarativeUI::Command::ICommand {
    Q_OBJECT

public:
    explicit ToolButtonCommand(const CommandContext& context);
    CommandResult<QVariant> execute(const CommandContext& context) override;
    CommandResult<QVariant> undo(const CommandContext& context) override;
    bool canUndo(const CommandContext& context) const override;
    CommandMetadata getMetadata() const override;

private:
    QToolButton* findToolButton(const QString& name);
    QString widget_name_;
    QToolButton::ToolButtonPopupMode old_popup_mode_;
    Qt::ToolButtonStyle old_style_;
    QString operation_;
};

/**
 * @brief Registers all component-specific commands with the command system.
 */
void registerComponentCommands();

}  // namespace ComponentCommands
}  // namespace Command
}  // namespace DeclarativeUI
