#include <gtest/gtest.h>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QThread>
#include "Core/ParallelProcessor.hpp"

using namespace DeclarativeUI::Core;

class ParallelProcessorTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create QCoreApplication if it doesn't exist
        if (!QCoreApplication::instance()) {
            int argc = 0;
            char** argv = nullptr;
            app = new QCoreApplication(argc, argv);
        }
    }

    void TearDown() override {
        // Clean up test files
        QFile::remove("test1.txt");
        QFile::remove("test2.txt");
    }

    QCoreApplication* app = nullptr;
};

TEST_F(ParallelProcessorTest, BasicTaskSubmission) {
    ParallelProcessor processor;

    bool taskExecuted = false;
    QString taskId = processor.submitTask(
        "test_task", TaskPriority::Normal, ExecutionContext::ThreadPool,
        [&taskExecuted]() { taskExecuted = true; });

    EXPECT_FALSE(taskId.isEmpty());

    // Wait for task to complete
    QThread::msleep(100);

    EXPECT_TRUE(taskExecuted);
}

TEST_F(ParallelProcessorTest, TaskStatusTracking) {
    ParallelProcessor processor;

    QString taskId = processor.submitTask("status_test", TaskPriority::Normal,
                                          ExecutionContext::ThreadPool,
                                          []() { QThread::msleep(50); });

    // Initially task should be running or completed quickly
    // Since we can't guarantee timing, we just check the ID is valid
    EXPECT_FALSE(taskId.isEmpty());

    // Wait for completion
    QThread::msleep(100);

    // After waiting, task should be completed
    EXPECT_TRUE(processor.isTaskCompleted(taskId));
}

TEST_F(ParallelProcessorTest, TaskCancellation) {
    ParallelProcessor processor;

    QString taskId =
        processor.submitTask("cancel_test", TaskPriority::Normal,
                             ExecutionContext::ThreadPool, []() {
                                 QThread::msleep(1000);  // Long running task
                             });

    EXPECT_FALSE(taskId.isEmpty());

    // Cancel the task
    processor.cancelTask(taskId);

    // Task should be completed (cancelled)
    EXPECT_TRUE(processor.isTaskCompleted(taskId));
}

TEST_F(ParallelProcessorTest, PerformanceMetrics) {
    ParallelProcessor processor;

    auto metrics = processor.getPerformanceMetrics();

    EXPECT_TRUE(metrics.contains("total_tasks_executed"));
    EXPECT_TRUE(metrics.contains("total_tasks_failed"));
    EXPECT_TRUE(metrics.contains("active_task_count"));
    EXPECT_TRUE(metrics.contains("success_rate"));
}

TEST_F(ParallelProcessorTest, FileProcessorWriteRead) {
    ParallelFileProcessor fileProcessor;

    QStringList testFiles = {"test1.txt", "test2.txt"};
    QStringList testContents = {"Hello World 1", "Hello World 2"};

    // Test file writing
    auto writeFuture = fileProcessor.writeFilesAsync(testFiles, testContents);
    writeFuture.waitForFinished();

    EXPECT_TRUE(writeFuture.result());

    // Verify files exist
    EXPECT_TRUE(QFile::exists("test1.txt"));
    EXPECT_TRUE(QFile::exists("test2.txt"));

    // Test file reading
    auto readFuture = fileProcessor.readFilesAsync(testFiles);
    readFuture.waitForFinished();

    auto contents = readFuture.result();
    EXPECT_EQ(contents.size(), 2);
    EXPECT_EQ(contents[0], "Hello World 1");
    EXPECT_EQ(contents[1], "Hello World 2");
}

TEST_F(ParallelProcessorTest, FileProcessorProcessing) {
    ParallelFileProcessor fileProcessor;

    QStringList testFiles = {"test1.txt"};
    QStringList testContents = {"hello world"};

    // Write test file
    auto writeFuture = fileProcessor.writeFilesAsync(testFiles, testContents);
    writeFuture.waitForFinished();
    EXPECT_TRUE(writeFuture.result());

    // Test file processing
    auto processFuture = fileProcessor.processFilesAsync(
        testFiles,
        [](const QString& content) -> QString { return content.toUpper(); });
    processFuture.waitForFinished();

    auto processedContents = processFuture.result();
    EXPECT_EQ(processedContents.size(), 1);
    EXPECT_EQ(processedContents[0], "HELLO WORLD");
}

TEST_F(ParallelProcessorTest, UICompilerBasic) {
    ParallelUICompiler uiCompiler;

    // Create a dummy UI file
    QFile testFile("test.ui");
    testFile.open(QIODevice::WriteOnly);
    testFile.write("<?xml version=\"1.0\"?><ui></ui>");
    testFile.close();

    // Test UI compilation
    auto compileFuture = uiCompiler.compileUIAsync("test.ui");
    compileFuture.waitForFinished();

    auto result = compileFuture.result();
    EXPECT_TRUE(result.contains("file_path"));
    EXPECT_TRUE(result.contains("compiled"));
    EXPECT_EQ(result["file_path"].toString(), "test.ui");
    EXPECT_TRUE(result["compiled"].toBool());

    // Test UI validation
    auto validateFuture = uiCompiler.validateUIAsync("test.ui");
    validateFuture.waitForFinished();

    EXPECT_TRUE(validateFuture.result());

    // Clean up
    QFile::remove("test.ui");
}

TEST_F(ParallelProcessorTest, PropertyBinderBasic) {
    ParallelPropertyBinder propertyBinder;

    // Create a test object
    QObject testObject;
    testObject.setObjectName("TestObject");
    testObject.setProperty("testProperty", 42);

    // Test property binding
    auto bindFuture = propertyBinder.bindPropertyAsync<int>(
        &testObject, "testProperty", []() -> int { return 100; });
    bindFuture.waitForFinished();

    EXPECT_TRUE(bindFuture.result());
    EXPECT_EQ(testObject.property("testProperty").toInt(), 100);

    // Test binding validation
    auto validateFuture = propertyBinder.validateBindingsAsync();
    validateFuture.waitForFinished();

    EXPECT_TRUE(validateFuture.result());
}
