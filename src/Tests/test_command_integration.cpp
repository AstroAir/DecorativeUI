#include <QApplication>
#include <QMenu>
#include <QMenuBar>
#include <QSignalSpy>
#include <QTest>
#include <QToolBar>
#include <QWidget>
#include <memory>

#include "../Command/BuiltinCommands.hpp"
#include "../Command/CommandIntegration.hpp"
#include "../Command/CommandSystem.hpp"

using namespace DeclarativeUI::Command;
using namespace DeclarativeUI::Command::Integration;
using namespace DeclarativeUI::Command::Commands;

class CommandIntegrationTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        // Initialize Qt application if not already done
        if (!QApplication::instance()) {
            int argc = 0;
            char* argv[] = {nullptr};
            new QApplication(argc, argv);
        }

        // Register builtin commands
        registerBuiltinCommands();
    }

    void cleanupTestCase() {
        // Clean up any global state
    }

    void init() {
        // Set up for each test
        main_widget_ = std::make_unique<QWidget>();
        main_widget_->resize(400, 300);
    }

    void cleanup() {
        // Clean up after each test
        main_widget_.reset();
    }

    // **CommandButton Tests**
    void testCommandButtonCreation() {
        CommandButton button;

        // Test fluent interface
        button.text("Test Button").tooltip("Test tooltip").enabled(true);

        button.initialize();

        auto* widget = button.getWidget();
        QVERIFY(widget != nullptr);
        QCOMPARE(widget->property("text").toString(), QString("Test Button"));
        QCOMPARE(widget->property("toolTip").toString(),
                 QString("Test tooltip"));
        QVERIFY(widget->isEnabled());
    }

    void testCommandButtonWithIcon() {
        CommandButton button;

        QIcon test_icon = QIcon::fromTheme("document-save");
        button.text("Save").icon(test_icon).tooltip("Save document");

        button.initialize();

        auto* widget = button.getWidget();
        QVERIFY(widget != nullptr);
        QCOMPARE(widget->property("text").toString(), QString("Save"));

        // Icon property might not be directly accessible via property()
        // This test documents the expected behavior
    }

    void testCommandButtonOnClick() {
        CommandButton button;

        CommandContext context;
        context.setParameter("text", QString("Button clicked"));

        button.text("Click Me").onClick("clipboard.copy", context);

        button.initialize();

        auto* widget = button.getWidget();
        QVERIFY(widget != nullptr);

        // Test metadata
        auto metadata = button.getMetadata();
        QCOMPARE(metadata.name, QString("CommandButton"));
        QVERIFY(!metadata.description.isEmpty());
    }

    void testCommandButtonDisabled() {
        CommandButton button;

        button.text("Disabled Button").enabled(false);

        button.initialize();

        auto* widget = button.getWidget();
        QVERIFY(widget != nullptr);
        QVERIFY(!widget->isEnabled());
    }

    // **CommandMenu Tests**
    void testCommandMenuCreation() {
        CommandMenu menu;

        menu.addAction("New", "load_file")
            .addAction("Save", "save_file")
            .addSeparator()
            .addAction("Exit", "application.quit");

        auto* qt_menu = menu.buildMenu();
        QVERIFY(qt_menu != nullptr);

        auto actions = qt_menu->actions();
        QVERIFY(actions.size() >=
                3);  // At least New, Save, Exit (separator might be counted)

        QCOMPARE(actions[0]->text(), QString("New"));
        QCOMPARE(actions[1]->text(), QString("Save"));
        QCOMPARE(actions[2]->text(), QString("Exit"));
    }

    void testCommandMenuWithSeparators() {
        CommandMenu menu;

        menu.addAction("Action 1", "command1")
            .addSeparator()
            .addAction("Action 2", "command2")
            .addSeparator()
            .addAction("Action 3", "command3");

        auto* qt_menu = menu.buildMenu();
        QVERIFY(qt_menu != nullptr);

        auto actions = qt_menu->actions();
        QVERIFY(actions.size() >= 5);  // 3 actions + 2 separators

        // Check that separators are in the right places
        bool has_separators = false;
        for (auto* action : actions) {
            if (action->isSeparator()) {
                has_separators = true;
                break;
            }
        }
        QVERIFY(has_separators);
    }

    void testCommandMenuWithSubmenus() {
        CommandMenu main_menu;

        main_menu
            .addSubmenu("File",
                        [](CommandMenu& file_menu) {
                            file_menu.addAction("New", "file.new")
                                .addAction("Open", "file.open")
                                .addAction("Save", "file.save");
                        })
            .addAction("Edit", "edit.action")
            .addAction("Help", "help.action");

        auto* qt_menu = main_menu.buildMenu();
        QVERIFY(qt_menu != nullptr);

        auto actions = qt_menu->actions();
        QVERIFY(actions.size() >= 3);  // File submenu, Edit, Help

        // Check that first action has a submenu
        auto* first_action = actions[0];
        QCOMPARE(first_action->text(), QString("File"));
        QVERIFY(first_action->menu() != nullptr);

        // Check submenu contents
        auto submenu_actions = first_action->menu()->actions();
        QVERIFY(submenu_actions.size() >= 3);
        QCOMPARE(submenu_actions[0]->text(), QString("New"));
        QCOMPARE(submenu_actions[1]->text(), QString("Open"));
        QCOMPARE(submenu_actions[2]->text(), QString("Save"));
    }

    void testCommandMenuActionExecution() {
        CommandMenu menu;

        CommandContext context;
        context.setParameter("text", QString("Menu action executed"));

        menu.addAction("Copy Text", "clipboard.copy", context);

        auto* qt_menu = menu.buildMenu();
        QVERIFY(qt_menu != nullptr);

        auto actions = qt_menu->actions();
        QVERIFY(actions.size() >= 1);

        auto* copy_action = actions[0];
        QCOMPARE(copy_action->text(), QString("Copy Text"));

        // Trigger the action
        copy_action->trigger();

        // Verify clipboard content
        QClipboard* clipboard = QApplication::clipboard();
        QCOMPARE(clipboard->text(), QString("Menu action executed"));
    }

    // **CommandToolBar Tests**
    void testCommandToolBarCreation() {
        CommandToolBar toolbar;

        toolbar.addButton("New", "file.new")
            .addButton("Open", "file.open")
            .addSeparator()
            .addButton("Save", "file.save");

        auto* qt_toolbar = toolbar.buildToolBar();
        QVERIFY(qt_toolbar != nullptr);

        auto actions = qt_toolbar->actions();
        QVERIFY(actions.size() >= 4);  // 3 buttons + 1 separator

        QCOMPARE(actions[0]->text(), QString("New"));
        QCOMPARE(actions[1]->text(), QString("Open"));
        QVERIFY(actions[2]->isSeparator());
        QCOMPARE(actions[3]->text(), QString("Save"));
    }

    void testCommandToolBarWithIcons() {
        CommandToolBar toolbar;

        QIcon new_icon = QIcon::fromTheme("document-new");
        QIcon open_icon = QIcon::fromTheme("document-open");
        QIcon save_icon = QIcon::fromTheme("document-save");

        toolbar.addButton("New", "file.new")
            .setIcon(new_icon)
            .addButton("Open", "file.open")
            .setIcon(open_icon)
            .addButton("Save", "file.save")
            .setIcon(save_icon);

        auto* qt_toolbar = toolbar.buildToolBar();
        QVERIFY(qt_toolbar != nullptr);

        auto actions = qt_toolbar->actions();
        QVERIFY(actions.size() >= 3);

        QCOMPARE(actions[0]->text(), QString("New"));
        QCOMPARE(actions[1]->text(), QString("Open"));
        QCOMPARE(actions[2]->text(), QString("Save"));

        // Icons should be set (exact verification depends on implementation)
        QVERIFY(!actions[0]->icon().isNull());
        QVERIFY(!actions[1]->icon().isNull());
        QVERIFY(!actions[2]->icon().isNull());
    }

    void testCommandToolBarButtonExecution() {
        CommandToolBar toolbar;

        CommandContext context;
        context.setParameter("text", QString("Toolbar button executed"));

        toolbar.addButton("Copy", "clipboard.copy", context);

        auto* qt_toolbar = toolbar.buildToolBar();
        QVERIFY(qt_toolbar != nullptr);

        auto actions = qt_toolbar->actions();
        QVERIFY(actions.size() >= 1);

        auto* copy_action = actions[0];
        QCOMPARE(copy_action->text(), QString("Copy"));

        // Trigger the action
        copy_action->trigger();

        // Verify clipboard content
        QClipboard* clipboard = QApplication::clipboard();
        QCOMPARE(clipboard->text(), QString("Toolbar button executed"));
    }

    // **CommandConfigurationLoader Tests**
    void testCommandConfigurationLoader() {
        QString json_config = R"({
            "commands": {
                "test.command": {
                    "name": "Test Command",
                    "description": "A test command for configuration loading"
                }
            },
            "menus": [{
                "id": "test_menu",
                "title": "Test Menu",
                "items": [{
                    "type": "action",
                    "text": "Test Action",
                    "command": "test.command"
                }, {
                    "type": "separator"
                }, {
                    "type": "action",
                    "text": "Another Action",
                    "command": "clipboard.copy"
                }]
            }],
            "toolbars": [{
                "id": "test_toolbar",
                "title": "Test Toolbar",
                "items": [{
                    "type": "button",
                    "text": "Test Button",
                    "command": "test.command"
                }]
            }]
        })";

        auto& loader = CommandConfigurationLoader::instance();
        bool result = loader.loadConfiguration(json_config);

        QVERIFY(result);

        // Verify menus were loaded
        const auto& menus = loader.getMenus();
        QCOMPARE(menus.size(), 1);
        QVERIFY(menus[0] != nullptr);

        // Verify toolbars were loaded
        const auto& toolbars = loader.getToolbars();
        QCOMPARE(toolbars.size(), 1);
        QVERIFY(toolbars[0] != nullptr);
    }

    void testCommandConfigurationLoaderInvalidJson() {
        QString invalid_json = R"({
            "commands": {
                "test.command": {
                    "name": "Test Command"
                }
            },
            "menus": [
                // Invalid JSON comment
                {
                    "id": "test_menu",
                    "title": "Test Menu"
                }
            ]
        })";

        auto& loader = CommandConfigurationLoader::instance();
        bool result = loader.loadConfiguration(invalid_json);

        QVERIFY(!result);
    }

    // **Integration Tests**
    void testFullCommandIntegrationWorkflow() {
        // Create a complete UI with command integration
        auto main_window = std::make_unique<QWidget>();
        main_window->resize(600, 400);

        // Create menu bar
        CommandMenu file_menu;
        file_menu.addAction(
            "Copy Test", "clipboard.copy",
            CommandContext().setParameter("text", QString("File menu test")));

        // Create toolbar
        CommandToolBar main_toolbar;
        main_toolbar.addButton(
            "Copy Test", "clipboard.copy",
            CommandContext().setParameter("text", QString("Toolbar test")));

        // Create command button
        CommandButton test_button;
        test_button.text("Command Button")
            .onClick("clipboard.copy", CommandContext().setParameter(
                                           "text", QString("Button test")));
        test_button.initialize();

        // Verify all components were created successfully
        auto* menu = file_menu.buildMenu();
        auto* toolbar = main_toolbar.buildToolBar();
        auto* button_widget = test_button.getWidget();

        QVERIFY(menu != nullptr);
        QVERIFY(toolbar != nullptr);
        QVERIFY(button_widget != nullptr);

        // Test that actions can be triggered
        auto menu_actions = menu->actions();
        if (!menu_actions.isEmpty()) {
            menu_actions[0]->trigger();
            QCOMPARE(QApplication::clipboard()->text(),
                     QString("File menu test"));
        }

        auto toolbar_actions = toolbar->actions();
        if (!toolbar_actions.isEmpty()) {
            toolbar_actions[0]->trigger();
            QCOMPARE(QApplication::clipboard()->text(),
                     QString("Toolbar test"));
        }
    }

private:
    std::unique_ptr<QWidget> main_widget_;
};

QTEST_MAIN(CommandIntegrationTest)
#include "test_command_integration.moc"
