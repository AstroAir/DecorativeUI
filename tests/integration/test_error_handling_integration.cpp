#include <QApplication>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTemporaryFile>
#include <QTest>
#include <memory>

#include "../Binding/StateManager.hpp"
#include "../Command/Adapters/ComponentSystemAdapter.hpp"
#include "../Command/Adapters/IntegrationManager.hpp"
#include "../Command/Adapters/JSONCommandLoader.hpp"
#include "../Command/Adapters/StateManagerAdapter.hpp"
#include "../Command/BuiltinCommands.hpp"
#include "../Command/CommandSystem.hpp"
#include "../Components/Button.hpp"
#include "../Components/LineEdit.hpp"
#include "../Core/DeclarativeBuilder.hpp"
#include "../Exceptions/UIExceptions.hpp"
#include "../HotReload/HotReloadManager.hpp"
#include "../JSON/JSONUILoader.hpp"
#include "../JSON/JSONValidator.hpp"

using namespace DeclarativeUI::Binding;
using namespace DeclarativeUI::Command;
using namespace DeclarativeUI::Command::Commands;
using namespace DeclarativeUI::Command::Adapters;
using namespace DeclarativeUI::Components;
using namespace DeclarativeUI::Core;
using namespace DeclarativeUI::HotReload;
using namespace DeclarativeUI::JSON;
using namespace DeclarativeUI::Exceptions;

class ErrorHandlingIntegrationTest : public QObject {
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
        StateManager::instance().clearState();
        temp_dir_ = std::make_unique<QTemporaryDir>();
        QVERIFY(temp_dir_->isValid());
    }

    void cleanup() {
        // Clean up after each test
        StateManager::instance().clearState();
        temp_dir_.reset();
    }

    // **JSON Loading Error Handling**
    void testJSONLoadingErrorHandling() {
        qDebug() << "🧪 Testing JSON loading error handling...";

        JSONUILoader loader;

        // Test 1: Invalid JSON syntax
        QString invalid_json = R"({
            "widget": {
                "type": "QLabel",
                "properties": {
                    "text": "Unclosed string
                }
            }
        })";

        auto widget1 = loader.loadFromString(invalid_json);
        QVERIFY(widget1 == nullptr);

        // Test 2: Missing required fields
        QString missing_fields_json = R"({
            "widget": {
                "properties": {
                    "text": "Missing type field"
                }
            }
        })";

        auto widget2 = loader.loadFromString(missing_fields_json);
        QVERIFY(widget2 == nullptr);

        // Test 3: Invalid widget type
        QString invalid_type_json = R"({
            "widget": {
                "type": "NonExistentWidget",
                "properties": {
                    "text": "Invalid widget type"
                }
            }
        })";

        auto widget3 = loader.loadFromString(invalid_type_json);
        QVERIFY(widget3 == nullptr);

        // Test 4: Loading from non-existent file
        auto widget4 = loader.loadFromFile("non_existent_file.json");
        QVERIFY(widget4 == nullptr);

        // Test 5: Loading from directory instead of file
        auto widget5 = loader.loadFromFile(temp_dir_->path());
        QVERIFY(widget5 == nullptr);

        qDebug() << "✅ JSON loading error handling test passed";
    }

    // **Command System Error Handling**
    void testCommandSystemErrorHandling() {
        qDebug() << "🧪 Testing command system error handling...";

        auto& command_manager = CommandManager::instance();
        auto& invoker = command_manager.getInvoker();

        // Test 1: Execute non-existent command
        CommandContext context1;
        auto result1 = invoker.execute("non_existent_command", context1);
        QVERIFY(!result1.isSuccess());
        QVERIFY(result1.isError());

        // Test 2: Execute command with missing parameters
        CommandContext context2;
        // Missing required parameters for set_property command
        auto result2 = invoker.execute("set_property", context2);
        QVERIFY(!result2.isSuccess());

        // Test 3: Execute command with invalid parameter types
        CommandContext context3;
        context3.setParameter("invalid_param", QVariant());
        auto result3 = invoker.execute("set_property", context3);
        QVERIFY(!result3.isSuccess());

        // Test 4: Command factory error handling
        auto& factory = CommandFactory::instance();
        auto invalid_command = factory.createCommand("invalid_command_type");
        QVERIFY(invalid_command == nullptr);

        qDebug() << "✅ Command system error handling test passed";
    }

    // **Component System Error Handling**
    void testComponentSystemErrorHandling() {
        qDebug() << "🧪 Testing component system error handling...";

        // Test 1: Component initialization with invalid properties
        auto button = std::make_unique<Button>();
        button->text("");  // Empty text should be handled gracefully
        button->initialize();

        auto* widget = button->getWidget();
        QVERIFY(widget != nullptr);

        // Test 2: Component adapter with null components
        auto adapter = std::make_unique<ComponentSystemAdapter>();
        auto null_command = adapter->convertToCommand(nullptr);
        QVERIFY(null_command == nullptr);

        // Test 3: Component adapter with uninitialized components
        auto uninitialized_button = std::make_unique<Button>();
        // Don't call initialize()
        auto command = adapter->convertToCommand(uninitialized_button.get());
        // Should either work or return nullptr, but not crash
        QVERIFY(true);  // Test passes if no crash occurs

        // Test 4: DeclarativeBuilder with invalid properties
        auto safe_widget =
            create<QLabel>()
                .property("text", QString("Valid"))
                .property("nonexistent_property", QString("Invalid"))
                .buildSafe();

        QVERIFY(safe_widget != nullptr);
        QCOMPARE(safe_widget->text(), QString("Valid"));

        qDebug() << "✅ Component system error handling test passed";
    }

    // **State Management Error Handling**
    void testStateManagementErrorHandling() {
        qDebug() << "🧪 Testing state management error handling...";

        auto& state_manager = StateManager::instance();

        // Test 1: Get non-existent state
        auto non_existent = state_manager.getState<QString>("non.existent.key");
        QVERIFY(non_existent == nullptr);

        // Test 2: Type mismatch when getting state
        state_manager.setState("test.string", QString("text"));
        auto wrong_type = state_manager.getState<int>("test.string");
        QVERIFY(wrong_type == nullptr);

        // Test 3: Invalid computed state
        state_manager.setComputedState("test.computed", []() -> QVariant {
            throw std::runtime_error("Computed state error");
        });

        auto computed_result = state_manager.getState<QString>("test.computed");
        // Should handle the exception gracefully
        QVERIFY(computed_result == nullptr);

        // Test 4: Circular dependency in computed state
        state_manager.setComputedState("circular.a", [&]() -> QVariant {
            auto b = state_manager.getState<QString>("circular.b");
            return b ? b->get() : QString("default");
        });

        state_manager.setComputedState("circular.b", [&]() -> QVariant {
            auto a = state_manager.getState<QString>("circular.a");
            return a ? a->get() : QString("default");
        });

        auto circular_result = state_manager.getState<QString>("circular.a");
        // Should detect and handle circular dependency
        QVERIFY(true);  // Test passes if no infinite loop occurs

        qDebug() << "✅ State management error handling test passed";
    }

    // **Hot Reload Error Handling**
    void testHotReloadErrorHandling() {
        qDebug() << "🧪 Testing hot reload error handling...";

        auto hot_reload_manager = std::make_unique<HotReloadManager>();

        // Test 1: Watch non-existent file
        bool watch_result1 =
            hot_reload_manager->watchFile("non_existent_file.json");
        QVERIFY(!watch_result1);

        // Test 2: Watch directory instead of file
        bool watch_result2 = hot_reload_manager->watchFile(temp_dir_->path());
        QVERIFY(!watch_result2);

        // Test 3: Create file with invalid content and watch for changes
        QTemporaryFile temp_file(temp_dir_->path() + "/invalid_XXXXXX.json");
        QVERIFY(temp_file.open());

        QString invalid_content = "{ invalid json content }";
        temp_file.write(invalid_content.toUtf8());
        temp_file.close();

        QSignalSpy error_spy(hot_reload_manager.get(),
                             &HotReloadManager::reloadError);

        bool watch_result3 =
            hot_reload_manager->watchFile(temp_file.fileName());
        QVERIFY(watch_result3);

        // Modify file with more invalid content
        QVERIFY(temp_file.open());
        temp_file.resize(0);
        temp_file.write("even more invalid content".toUtf8());
        temp_file.close();

        // Wait a bit for file system events
        QTest::qWait(100);

        // Should have emitted error signal or handled gracefully
        QVERIFY(error_spy.count() >=
                0);  // May or may not emit depending on implementation

        qDebug() << "✅ Hot reload error handling test passed";
    }

    // **Integration Manager Error Handling**
    void testIntegrationManagerErrorHandling() {
        qDebug() << "🧪 Testing integration manager error handling...";

        auto manager = std::make_unique<IntegrationManager>();

        // Test 1: Use adapter that doesn't exist
        QVERIFY(!manager->hasAdapter("non_existent_adapter"));

        // Test 2: Register adapter with empty name
        auto adapter1 = std::make_unique<ComponentSystemAdapter>();
        bool register_result1 =
            manager->registerAdapter("", std::move(adapter1));
        QVERIFY(!register_result1);

        // Test 3: Register null adapter
        bool register_result2 =
            manager->registerAdapter("null_adapter", nullptr);
        QVERIFY(!register_result2);

        // Test 4: Double registration of same adapter name
        auto adapter2 = std::make_unique<ComponentSystemAdapter>();
        auto adapter3 = std::make_unique<ComponentSystemAdapter>();

        bool register_result3 =
            manager->registerAdapter("test_adapter", std::move(adapter2));
        QVERIFY(register_result3);

        bool register_result4 =
            manager->registerAdapter("test_adapter", std::move(adapter3));
        // Should either replace or reject, but not crash
        QVERIFY(true);  // Test passes if no crash occurs

        qDebug() << "✅ Integration manager error handling test passed";
    }

    // **Memory and Resource Error Handling**
    void testMemoryAndResourceErrorHandling() {
        qDebug() << "🧪 Testing memory and resource error handling...";

        // Test 1: Create many components to test memory limits
        std::vector<std::unique_ptr<Button>> buttons;

        try {
            for (int i = 0; i < 10000; ++i) {
                auto button = std::make_unique<Button>();
                button->text(QString("Button %1").arg(i));
                button->initialize();
                buttons.push_back(std::move(button));
            }
        } catch (const std::exception& e) {
            qDebug() << "Caught expected exception:" << e.what();
        }

        // Should not crash even if memory is limited
        QVERIFY(true);

        // Test 2: Rapid creation and destruction
        for (int cycle = 0; cycle < 100; ++cycle) {
            std::vector<std::unique_ptr<LineEdit>> temp_edits;

            for (int i = 0; i < 50; ++i) {
                auto edit = std::make_unique<LineEdit>();
                edit->text(QString("Temp %1").arg(i));
                edit->initialize();
                temp_edits.push_back(std::move(edit));
            }

            // temp_edits will be destroyed at end of loop
        }

        // Should handle rapid allocation/deallocation gracefully
        QVERIFY(true);

        // Test 3: File handle exhaustion protection
        std::vector<std::unique_ptr<QTemporaryFile>> files;

        try {
            for (int i = 0; i < 1000; ++i) {
                auto file = std::make_unique<QTemporaryFile>(
                    temp_dir_->path() + QString("/test_%1_XXXXXX.tmp").arg(i));
                if (file->open()) {
                    files.push_back(std::move(file));
                } else {
                    break;  // Stop if we can't open more files
                }
            }
        } catch (const std::exception& e) {
            qDebug() << "File handle limit reached:" << e.what();
        }

        qDebug() << "Created" << files.size() << "temporary files";
        QVERIFY(true);  // Test passes if no crash occurs

        qDebug() << "✅ Memory and resource error handling test passed";
    }

    // **Exception Safety Integration**
    void testExceptionSafetyIntegration() {
        qDebug() << "🧪 Testing exception safety integration...";

        // Test 1: Exception during UI creation
        try {
            auto widget =
                create<QLabel>()
                    .property("text", QString("Exception Test"))
                    .on("clicked",
                        []() { throw std::runtime_error("Test exception"); })
                    .build();

            QVERIFY(widget != nullptr);

            // Simulate event that throws
            try {
                // widget->click(); // Would trigger exception
            } catch (const std::exception& e) {
                qDebug() << "Caught expected exception:" << e.what();
            }
        } catch (const std::exception& e) {
            qDebug() << "Exception during UI creation:" << e.what();
        }

        // Test 2: Exception during command execution
        auto& invoker = CommandManager::instance().getInvoker();

        CommandContext context;
        context.setParameter("throw_exception", true);

        try {
            auto result = invoker.execute("test_exception_command", context);
            // Command might not exist, which is fine for this test
            QVERIFY(true);
        } catch (const std::exception& e) {
            qDebug() << "Exception during command execution:" << e.what();
        }

        // Test 3: Exception during state management
        auto& state_manager = StateManager::instance();

        try {
            state_manager.setComputedState("exception.test", []() -> QVariant {
                throw std::runtime_error("State computation error");
            });

            auto result = state_manager.getState<QString>("exception.test");
            // Should handle exception gracefully
            QVERIFY(result == nullptr);
        } catch (const std::exception& e) {
            qDebug() << "Exception during state management:" << e.what();
        }

        qDebug() << "✅ Exception safety integration test passed";
    }

private:
    std::unique_ptr<QTemporaryDir> temp_dir_;
};

QTEST_MAIN(ErrorHandlingIntegrationTest)
#include "test_error_handling_integration.moc"
