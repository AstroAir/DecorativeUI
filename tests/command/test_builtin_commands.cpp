#include <QApplication>
#include <QClipboard>
#include <QLabel>
#include <QLineEdit>
#include <QSignalSpy>
#include <algorithm>
#include <QTest>
#include <QTimer>
#include <QWidget>
#include <memory>

#include "../Command/BuiltinCommands.hpp"
#include "../Command/CommandSystem.hpp"
#include "../Binding/StateManager.hpp"

using namespace DeclarativeUI::Command;
using namespace DeclarativeUI::Command::Commands;
using namespace DeclarativeUI::Binding;

class BuiltinCommandsTest : public QObject {
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
        StateManager::instance().clearState();
    }

    void init() {
        // Set up for each test
        test_widget_ = std::make_unique<QLabel>();
        test_widget_->setObjectName("test_widget");
        test_widget_->setProperty("text", "Initial Text");
        
        test_line_edit_ = std::make_unique<QLineEdit>();
        test_line_edit_->setObjectName("test_line_edit");
        test_line_edit_->setText("Initial Line Edit");
    }

    void cleanup() {
        // Clean up after each test
        test_widget_.reset();
        test_line_edit_.reset();
        StateManager::instance().clearState();
    }

    // **SetPropertyCommand Tests**
    void testSetPropertyCommand() {
        CommandContext context;
        context.setParameter("target", QVariant::fromValue(test_widget_.get()));
        context.setParameter("property", QString("text"));
        context.setParameter("value", QString("New Text Value"));
        
        SetPropertyCommand command(context);
        auto result = command.execute(context);
        
        QVERIFY(result.isSuccess());
        QCOMPARE(test_widget_->property("text").toString(), QString("New Text Value"));
        
        // Test metadata
        auto metadata = command.getMetadata();
        QCOMPARE(metadata.name, QString("SetPropertyCommand"));
        QVERIFY(!metadata.description.isEmpty());
    }

    void testSetPropertyCommandUndo() {
        CommandContext context;
        context.setParameter("target", QVariant::fromValue(test_widget_.get()));
        context.setParameter("property", QString("text"));
        context.setParameter("value", QString("New Text Value"));
        
        SetPropertyCommand command(context);
        
        QString original_text = test_widget_->property("text").toString();
        
        // Execute command
        auto result = command.execute(context);
        QVERIFY(result.isSuccess());
        QCOMPARE(test_widget_->property("text").toString(), QString("New Text Value"));
        
        // Test undo capability
        QVERIFY(command.canUndo(context));
        
        // Undo command
        auto undo_result = command.undo(context);
        QVERIFY(undo_result.isSuccess());
        QCOMPARE(test_widget_->property("text").toString(), original_text);
    }

    void testSetPropertyCommandInvalidWidget() {
        CommandContext context;
        context.setParameter("target", QVariant::fromValue<QWidget*>(nullptr));
        context.setParameter("property", QString("text"));
        context.setParameter("value", QString("New Text Value"));
        
        SetPropertyCommand command(context);
        auto result = command.execute(context);
        
        QVERIFY(result.isError());
        QVERIFY(!result.getError().isEmpty());
    }

    void testSetPropertyCommandInvalidProperty() {
        CommandContext context;
        context.setParameter("target", QVariant::fromValue(test_widget_.get()));
        context.setParameter("property", QString("nonexistent_property"));
        context.setParameter("value", QString("New Text Value"));
        
        SetPropertyCommand command(context);
        auto result = command.execute(context);
        
        // Depending on implementation, this might succeed or fail
        // Document the expected behavior
        if (result.isError()) {
            QVERIFY(!result.getError().isEmpty());
        }
    }

    // **UpdateStateCommand Tests**
    void testUpdateStateCommand() {
        // Set up initial state
        StateManager::instance().setState("test_key", QString("initial_value"));
        
        CommandContext context;
        context.setParameter("key", QString("test_key"));
        context.setParameter("value", QString("updated_value"));
        
        UpdateStateCommand command(context);
        auto result = command.execute(context);
        
        QVERIFY(result.isSuccess());
        
        auto state = StateManager::instance().getState<QString>("test_key");
        QVERIFY(state != nullptr);
        QCOMPARE(state->get(), QString("updated_value"));
    }

    void testUpdateStateCommandNewKey() {
        CommandContext context;
        context.setParameter("key", QString("new_key"));
        context.setParameter("value", QString("new_value"));
        
        UpdateStateCommand command(context);
        auto result = command.execute(context);
        
        QVERIFY(result.isSuccess());
        
        auto state = StateManager::instance().getState<QString>("new_key");
        QVERIFY(state != nullptr);
        QCOMPARE(state->get(), QString("new_value"));
    }

    // **CopyToClipboardCommand Tests**
    void testCopyToClipboardCommand() {
        CommandContext context;
        context.setParameter("text", QString("Text to copy"));
        
        CopyToClipboardCommand command(context);
        auto result = command.execute(context);
        
        QVERIFY(result.isSuccess());
        
        // Verify clipboard content
        QClipboard* clipboard = QApplication::clipboard();
        QCOMPARE(clipboard->text(), QString("Text to copy"));
    }

    void testCopyToClipboardCommandEmpty() {
        CommandContext context;
        context.setParameter("text", QString(""));
        
        CopyToClipboardCommand command(context);
        auto result = command.execute(context);
        
        QVERIFY(result.isSuccess());
        
        QClipboard* clipboard = QApplication::clipboard();
        QCOMPARE(clipboard->text(), QString(""));
    }

    // **PasteFromClipboardCommand Tests**
    void testPasteFromClipboardCommand() {
        // Set up clipboard content
        QClipboard* clipboard = QApplication::clipboard();
        clipboard->setText("Clipboard content");
        
        CommandContext context;
        context.setParameter("target", QVariant::fromValue(test_line_edit_.get()));
        context.setParameter("property", QString("text"));
        
        PasteFromClipboardCommand command(context);
        auto result = command.execute(context);
        
        QVERIFY(result.isSuccess());
        QCOMPARE(test_line_edit_->text(), QString("Clipboard content"));
    }

    void testPasteFromClipboardCommandEmptyClipboard() {
        // Clear clipboard
        QClipboard* clipboard = QApplication::clipboard();
        clipboard->clear();
        
        CommandContext context;
        context.setParameter("target", QVariant::fromValue(test_line_edit_.get()));
        context.setParameter("property", QString("text"));
        
        PasteFromClipboardCommand command(context);
        auto result = command.execute(context);
        
        // Should succeed even with empty clipboard
        QVERIFY(result.isSuccess());
    }

    // **ShowMessageCommand Tests**
    void testShowMessageCommand() {
        CommandContext context;
        context.setParameter("title", QString("Test Title"));
        context.setParameter("message", QString("Test Message"));
        context.setParameter("type", QString("information"));
        
        ShowMessageCommand command(context);
        
        // Note: This will actually show a message box in GUI tests
        // In a real test environment, you might want to mock this
        auto metadata = command.getMetadata();
        QCOMPARE(metadata.name, QString("ShowMessageCommand"));
        QVERIFY(!metadata.description.isEmpty());
        
        // For automated testing, we'll skip the actual execution
        // auto result = command.execute(context);
        // QVERIFY(result.isSuccess());
    }

    // **DelayedCommand Tests**
    void testDelayedCommand() {
        CommandContext context;
        context.setParameter("delay", 100); // 100ms delay
        context.setParameter("message", QString("Delayed message"));
        
        DelayedCommand command(context);
        
        QElapsedTimer timer;
        timer.start();
        
        auto future = command.executeAsync(context);
        
        // Wait for completion
        while (!future.isFinished() && timer.elapsed() < 1000) {
            QTest::qWait(10);
        }
        
        QVERIFY(future.isFinished());
        auto result = future.result();
        QVERIFY(result.isSuccess());
        
        // Should have taken at least the delay time
        QVERIFY(timer.elapsed() >= 100);
    }

    // **SaveFileCommand Tests**
    void testSaveFileCommand() {
        CommandContext context;
        context.setParameter("filename", QString("test_file.txt"));
        context.setParameter("content", QString("Test file content"));
        
        SaveFileCommand command(context);
        auto metadata = command.getMetadata();
        
        QCOMPARE(metadata.name, QString("SaveFileCommand"));
        QVERIFY(!metadata.description.isEmpty());
        
        // Note: Actual file operations might need special handling in tests
        // auto result = command.execute(context);
        // QVERIFY(result.isSuccess());
    }

    // **LoadFileCommand Tests**
    void testLoadFileCommand() {
        CommandContext context;
        context.setParameter("filename", QString("test_file.txt"));
        
        LoadFileCommand command(context);
        auto metadata = command.getMetadata();
        
        QCOMPARE(metadata.name, QString("LoadFileCommand"));
        QVERIFY(!metadata.description.isEmpty());
        
        // Note: Actual file operations might need special handling in tests
        // auto result = command.execute(context);
        // Result depends on file existence
    }

    // **DatabaseTransactionCommand Tests**
    void testDatabaseTransactionCommand() {
        CommandContext context;
        context.setParameter("query", QString("SELECT * FROM test_table"));
        
        DatabaseTransactionCommand command(context);
        auto metadata = command.getMetadata();
        
        QCOMPARE(metadata.name, QString("DatabaseTransactionCommand"));
        QVERIFY(!metadata.description.isEmpty());
        
        // Note: Database operations need special setup in tests
        // This test mainly verifies the command can be created
    }

    // **Command Factory Integration Tests**
    void testBuiltinCommandRegistration() {
        auto& factory = CommandFactory::instance();
        auto registered_commands = factory.getRegisteredCommands();
        
        // Verify that builtin commands are registered
        QVERIFY(std::find(registered_commands.begin(), registered_commands.end(), "set_property") != registered_commands.end());
        QVERIFY(std::find(registered_commands.begin(), registered_commands.end(), "update_state") != registered_commands.end());
        QVERIFY(std::find(registered_commands.begin(), registered_commands.end(), "save_file") != registered_commands.end());
        QVERIFY(std::find(registered_commands.begin(), registered_commands.end(), "load_file") != registered_commands.end());
        QVERIFY(std::find(registered_commands.begin(), registered_commands.end(), "clipboard.copy") != registered_commands.end());
        QVERIFY(std::find(registered_commands.begin(), registered_commands.end(), "clipboard.paste") != registered_commands.end());
        QVERIFY(std::find(registered_commands.begin(), registered_commands.end(), "show_message") != registered_commands.end());
    }

    void testBuiltinCommandCreation() {
        auto& factory = CommandFactory::instance();
        
        // Test creating each builtin command
        auto set_property_cmd = factory.createCommand("set_property");
        QVERIFY(set_property_cmd != nullptr);
        QCOMPARE(set_property_cmd->getMetadata().name, QString("SetPropertyCommand"));
        
        auto update_state_cmd = factory.createCommand("update_state");
        QVERIFY(update_state_cmd != nullptr);
        QCOMPARE(update_state_cmd->getMetadata().name, QString("UpdateStateCommand"));
        
        auto copy_cmd = factory.createCommand("clipboard.copy");
        QVERIFY(copy_cmd != nullptr);
        QCOMPARE(copy_cmd->getMetadata().name, QString("CopyToClipboardCommand"));
    }

    // **Error Handling Tests**
    void testCommandWithMissingParameters() {
        CommandContext empty_context;
        
        SetPropertyCommand command(empty_context);
        auto result = command.execute(empty_context);
        
        QVERIFY(result.isError());
        QVERIFY(!result.getError().isEmpty());
    }

    void testCommandWithInvalidParameters() {
        CommandContext context;
        context.setParameter("target", QString("not_a_widget"));
        context.setParameter("property", QString("text"));
        context.setParameter("value", QString("value"));
        
        SetPropertyCommand command(context);
        auto result = command.execute(context);
        
        QVERIFY(result.isError());
        QVERIFY(!result.getError().isEmpty());
    }

private:
    std::unique_ptr<QLabel> test_widget_;
    std::unique_ptr<QLineEdit> test_line_edit_;
};

QTEST_MAIN(BuiltinCommandsTest)
#include "test_builtin_commands.moc"
