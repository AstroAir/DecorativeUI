#include "ParallelProcessor.hpp"

#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QFutureInterface>
#include <QJsonArray>
#include <QMetaObject>
#include <QStandardPaths>
#include <QTextStream>
#include <QUuid>

namespace DeclarativeUI::Core {

// **ThreadPool implementation**
ThreadPool::ThreadPool(size_t thread_count) {
    workers_.reserve(thread_count);

    for (size_t i = 0; i < thread_count; ++i) {
        workers_.emplace_back(&ThreadPool::worker_thread, this);
    }

    qDebug() << "🔥 ThreadPool initialized with" << thread_count << "threads";
}

ThreadPool::~ThreadPool() { shutdown(); }

void ThreadPool::shutdown() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        running_.store(false);
    }

    condition_.notify_all();

    for (std::thread& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }

    workers_.clear();
    qDebug() << "🔥 ThreadPool shutdown completed";
}

void ThreadPool::pause() {
    paused_.store(true);
    qDebug() << "🔥 ThreadPool paused";
}

void ThreadPool::resume() {
    paused_.store(false);
    condition_.notify_all();
    qDebug() << "🔥 ThreadPool resumed";
}

size_t ThreadPool::queued_tasks() const {
    std::unique_lock<std::mutex> lock(queue_mutex_);
    return task_queue_.size();
}

void ThreadPool::worker_thread() {
    while (true) {
        TaskWrapper task;

        {
            std::unique_lock<std::mutex> lock(queue_mutex_);

            condition_.wait(lock, [this] {
                return !running_.load() ||
                       (!task_queue_.empty() && !paused_.load());
            });

            if (!running_.load()) {
                break;
            }

            if (task_queue_.empty() || paused_.load()) {
                continue;
            }

            task = task_queue_.top();
            task_queue_.pop();
        }

        active_threads_.fetch_add(1);

        try {
            task.task();
        } catch (const std::exception& e) {
            qWarning() << "🔥 Task execution failed:" << e.what();
        } catch (...) {
            qWarning() << "🔥 Task execution failed with unknown error";
        }

        active_threads_.fetch_sub(1);
    }
}

// **ParallelProcessor implementation**
ParallelProcessor::ParallelProcessor(QObject* parent) : QObject(parent) {
    thread_pool_ = std::make_unique<ThreadPool>();

    // Setup timeout timer
    timeout_timer_ = std::make_unique<QTimer>(this);
    timeout_timer_->setInterval(1000);  // Check every second
    connect(timeout_timer_.get(), &QTimer::timeout, this,
            &ParallelProcessor::onTaskTimeout);
    timeout_timer_->start();

    // Setup performance monitoring timer
    performance_timer_ = std::make_unique<QTimer>(this);
    performance_timer_->setInterval(10000);  // Check every 10 seconds
    connect(performance_timer_.get(), &QTimer::timeout, this,
            &ParallelProcessor::onPerformanceCheck);
    performance_timer_->start();

    qDebug() << "🔥 ParallelProcessor initialized";
}

ParallelProcessor::~ParallelProcessor() {
    if (thread_pool_) {
        thread_pool_->shutdown();
    }
    qDebug() << "🔥 ParallelProcessor destroyed";
}

void ParallelProcessor::cancelTask(const QString& task_id) {
    std::unique_lock<std::shared_mutex> lock(tasks_mutex_);

    auto it = active_tasks_.find(task_id);
    if (it != active_tasks_.end()) {
        active_tasks_.erase(it);

        // Emit cancellation signal on main thread
        QMetaObject::invokeMethod(
            this,
            [this, task_id]() { emit taskFailed(task_id, "Task cancelled"); },
            Qt::QueuedConnection);

        qDebug() << "🔥 Task cancelled:" << task_id;
    }
}

void ParallelProcessor::cancelBatch(const QString& batch_id) {
    std::unique_lock<std::shared_mutex> lock(tasks_mutex_);

    auto batch_it = batch_tasks_.find(batch_id);
    if (batch_it != batch_tasks_.end()) {
        int cancelled_count = 0;

        for (const QString& task_id : batch_it->second) {
            auto task_it = active_tasks_.find(task_id);
            if (task_it != active_tasks_.end()) {
                active_tasks_.erase(task_it);
                cancelled_count++;

                // Emit cancellation signal for each task
                QMetaObject::invokeMethod(
                    this,
                    [this, task_id]() {
                        emit taskFailed(task_id, "Task cancelled (batch)");
                    },
                    Qt::QueuedConnection);
            }
        }

        batch_tasks_.erase(batch_it);

        // Emit batch completion signal
        QMetaObject::invokeMethod(
            this,
            [this, batch_id, cancelled_count]() {
                emit batchCompleted(batch_id, 0, cancelled_count);
            },
            Qt::QueuedConnection);

        qDebug() << "🔥 Batch cancelled:" << batch_id << "(" << cancelled_count
                 << "tasks)";
    }
}

bool ParallelProcessor::isTaskRunning(const QString& task_id) const {
    std::shared_lock<std::shared_mutex> lock(tasks_mutex_);
    return active_tasks_.find(task_id) != active_tasks_.end();
}

bool ParallelProcessor::isTaskCompleted(const QString& task_id) const {
    std::shared_lock<std::shared_mutex> lock(tasks_mutex_);
    // A task is considered completed if it's not in the active tasks list
    return active_tasks_.find(task_id) == active_tasks_.end();
}

void ParallelProcessor::setThreadPoolSize(size_t size) {
    if (thread_pool_) {
        thread_pool_->shutdown();
    }
    thread_pool_ = std::make_unique<ThreadPool>(size);
    qDebug() << "🔥 Thread pool resized to" << size << "threads";
}

void ParallelProcessor::pauseProcessing() {
    if (thread_pool_) {
        thread_pool_->pause();
    }
}

void ParallelProcessor::resumeProcessing() {
    if (thread_pool_) {
        thread_pool_->resume();
    }
}

void ParallelProcessor::setMaxQueueSize(size_t max_size) {
    max_queue_size_.store(max_size);
}

QJsonObject ParallelProcessor::getPerformanceMetrics() const {
    QJsonObject metrics;

    metrics["total_tasks_executed"] =
        static_cast<qint64>(total_tasks_executed_.load());
    metrics["total_tasks_failed"] =
        static_cast<qint64>(total_tasks_failed_.load());
    metrics["active_task_count"] = static_cast<qint64>(getActiveTaskCount());
    metrics["queued_task_count"] = static_cast<qint64>(getQueuedTaskCount());
    metrics["average_execution_time"] = getAverageExecutionTime();
    metrics["peak_queue_size"] = static_cast<qint64>(peak_queue_size_.load());

    if (thread_pool_) {
        metrics["active_threads"] =
            static_cast<qint64>(thread_pool_->active_threads());
        metrics["thread_pool_running"] = thread_pool_->is_running();
    }

    double success_rate =
        total_tasks_executed_.load() > 0
            ? (1.0 - static_cast<double>(total_tasks_failed_.load()) /
                         total_tasks_executed_.load()) *
                  100.0
            : 100.0;
    metrics["success_rate"] = success_rate;

    return metrics;
}

size_t ParallelProcessor::getActiveTaskCount() const {
    std::shared_lock<std::shared_mutex> lock(tasks_mutex_);
    return active_tasks_.size();
}

size_t ParallelProcessor::getQueuedTaskCount() const {
    return thread_pool_ ? thread_pool_->queued_tasks() : 0;
}

double ParallelProcessor::getAverageExecutionTime() const {
    size_t executed = total_tasks_executed_.load();
    return executed > 0 ? total_execution_time_.load() / executed : 0.0;
}

void ParallelProcessor::enableTaskProfiling(bool enabled) {
    task_profiling_enabled_.store(enabled);
}

void ParallelProcessor::setTaskTimeout(std::chrono::milliseconds timeout) {
    task_timeout_ = timeout;
}

void ParallelProcessor::enableLoadBalancing(bool enabled) {
    load_balancing_enabled_.store(enabled);
}

void ParallelProcessor::onTaskTimeout() {
    // Check for timed out tasks and handle them
    cleanupCompletedTasks();
}

void ParallelProcessor::onPerformanceCheck() {
    auto metrics = getPerformanceMetrics();

    // Check for performance alerts
    double success_rate = metrics["success_rate"].toDouble();
    if (success_rate < 90.0) {
        emit performanceAlert("success_rate", success_rate);
    }

    double avg_execution_time = metrics["average_execution_time"].toDouble();
    if (avg_execution_time > 5000.0) {  // 5 seconds
        emit performanceAlert("average_execution_time", avg_execution_time);
    }

    size_t queue_size = getQueuedTaskCount();
    if (queue_size > max_queue_size_.load() * 0.8) {
        emit performanceAlert("queue_size", static_cast<double>(queue_size));
    }
}

QString ParallelProcessor::generateTaskId() const {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

void ParallelProcessor::updatePerformanceMetrics(
    const QString& task_id, std::chrono::milliseconds execution_time,
    bool success) {
    total_tasks_executed_.fetch_add(1);

    if (!success) {
        total_tasks_failed_.fetch_add(1);
    }

    total_execution_time_.fetch_add(execution_time.count());

    // Update peak queue size
    size_t current_queue_size = getQueuedTaskCount();
    size_t current_peak = peak_queue_size_.load();
    while (current_queue_size > current_peak &&
           !peak_queue_size_.compare_exchange_weak(current_peak,
                                                   current_queue_size)) {
        // Retry if another thread updated peak_queue_size_
    }
}

void ParallelProcessor::checkQueueOverflow() {
    size_t queue_size = getQueuedTaskCount();
    if (queue_size > max_queue_size_.load()) {
        emit queueOverflow(queue_size);
    }
}

void ParallelProcessor::balanceLoad() {
    if (!load_balancing_enabled_.load())
        return;

    // Simple load balancing: adjust thread pool size based on queue size
    size_t queue_size = getQueuedTaskCount();
    size_t active_threads = thread_pool_ ? thread_pool_->active_threads() : 0;

    if (queue_size > active_threads * 2 &&
        active_threads < std::thread::hardware_concurrency()) {
        // Consider increasing thread pool size
        qDebug() << "🔥 Load balancing: High queue size detected";
    }
}

void ParallelProcessor::cleanupCompletedTasks() {
    std::unique_lock<std::shared_mutex> lock(tasks_mutex_);

    // Remove completed tasks from active_tasks_
    for (auto it = active_tasks_.begin(); it != active_tasks_.end();) {
        // In a real implementation, we would check if the task is completed
        // For now, we'll keep all tasks
        ++it;
    }
}

// **ParallelFileProcessor implementation**
ParallelFileProcessor::ParallelFileProcessor(QObject* parent)
    : QObject(parent) {
    processor_ = std::make_unique<ParallelProcessor>(this);

    // Connect signals for progress reporting
    connect(processor_.get(), &ParallelProcessor::taskCompleted, this,
            [this](const QString& task_id, bool success) {
                // Extract file path from task_id if it contains file info
                QString file_path = task_id.contains("_file_")
                                        ? task_id.split("_file_").last()
                                        : task_id;
                emit fileProcessed(file_path, success);
            });

    qDebug() << "🔥 ParallelFileProcessor created";
}

QFuture<QStringList> ParallelFileProcessor::readFilesAsync(
    const QStringList& file_paths) {
    QFutureInterface<QStringList> interface;
    QFuture<QStringList> future = interface.future();
    interface.reportStarted();

    // Submit batch task to read all files
    auto task_func = [this, file_paths, interface]() mutable {
        QStringList results;
        results.reserve(file_paths.size());

        int completed = 0;
        int total = file_paths.size();

        try {
            for (const QString& file_path : file_paths) {
                QFile file(file_path);
                QString content;

                if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                    QTextStream stream(&file);
                    content = stream.readAll();
                    file.close();

                    QMetaObject::invokeMethod(
                        this,
                        [this, file_path]() {
                            emit fileProcessed(file_path, true);
                        },
                        Qt::QueuedConnection);
                } else {
                    qWarning() << "🔥 Failed to read file:" << file_path;
                    QMetaObject::invokeMethod(
                        this,
                        [this, file_path]() {
                            emit fileProcessed(file_path, false);
                        },
                        Qt::QueuedConnection);
                }

                results.append(content);
                completed++;

                // Report progress
                QMetaObject::invokeMethod(
                    this,
                    [this, completed, total]() {
                        emit batchProgress(completed, total);
                    },
                    Qt::QueuedConnection);
            }

            interface.reportResult(results);
            interface.reportFinished();

        } catch (const std::exception& e) {
            qWarning() << "🔥 File reading failed:" << e.what();
            interface.reportCanceled();
            interface.reportFinished();
        }
    };

    processor_->submitTask("read_files_batch", TaskPriority::Normal,
                           ExecutionContext::ThreadPool, task_func);

    return future;
}

QFuture<bool> ParallelFileProcessor::writeFilesAsync(
    const QStringList& file_paths, const QStringList& contents) {
    QFutureInterface<bool> interface;
    QFuture<bool> future = interface.future();
    interface.reportStarted();

    if (file_paths.size() != contents.size()) {
        qWarning() << "🔥 File paths and contents size mismatch";
        interface.reportResult(false);
        interface.reportFinished();
        return future;
    }

    auto task_func = [this, file_paths, contents, interface]() mutable {
        bool all_success = true;
        int completed = 0;
        int total = file_paths.size();

        try {
            for (int i = 0; i < file_paths.size(); ++i) {
                const QString& file_path = file_paths[i];
                const QString& content = contents[i];

                // Ensure directory exists
                QFileInfo file_info(file_path);
                QDir().mkpath(file_info.absolutePath());

                QFile file(file_path);
                bool success = false;

                if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                    QTextStream stream(&file);
                    stream << content;
                    file.close();
                    success = true;
                } else {
                    qWarning() << "🔥 Failed to write file:" << file_path;
                    all_success = false;
                }

                QMetaObject::invokeMethod(
                    this,
                    [this, file_path, success]() {
                        emit fileProcessed(file_path, success);
                    },
                    Qt::QueuedConnection);

                completed++;
                QMetaObject::invokeMethod(
                    this,
                    [this, completed, total]() {
                        emit batchProgress(completed, total);
                    },
                    Qt::QueuedConnection);
            }

            interface.reportResult(all_success);
            interface.reportFinished();

        } catch (const std::exception& e) {
            qWarning() << "🔥 File writing failed:" << e.what();
            interface.reportResult(false);
            interface.reportFinished();
        }
    };

    processor_->submitTask("write_files_batch", TaskPriority::Normal,
                           ExecutionContext::ThreadPool, task_func);

    return future;
}

QFuture<QStringList> ParallelFileProcessor::processFilesAsync(
    const QStringList& file_paths,
    std::function<QString(const QString&)> processor) {
    QFutureInterface<QStringList> interface;
    QFuture<QStringList> future = interface.future();
    interface.reportStarted();

    auto task_func = [this, file_paths, processor, interface]() mutable {
        QStringList results;
        results.reserve(file_paths.size());

        int completed = 0;
        int total = file_paths.size();

        try {
            for (const QString& file_path : file_paths) {
                QFile file(file_path);
                QString processed_content;

                if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                    QTextStream stream(&file);
                    QString content = stream.readAll();
                    file.close();

                    // Apply processor function
                    processed_content = processor(content);

                    QMetaObject::invokeMethod(
                        this,
                        [this, file_path]() {
                            emit fileProcessed(file_path, true);
                        },
                        Qt::QueuedConnection);
                } else {
                    qWarning() << "🔥 Failed to process file:" << file_path;
                    QMetaObject::invokeMethod(
                        this,
                        [this, file_path]() {
                            emit fileProcessed(file_path, false);
                        },
                        Qt::QueuedConnection);
                }

                results.append(processed_content);
                completed++;

                QMetaObject::invokeMethod(
                    this,
                    [this, completed, total]() {
                        emit batchProgress(completed, total);
                    },
                    Qt::QueuedConnection);
            }

            interface.reportResult(results);
            interface.reportFinished();

        } catch (const std::exception& e) {
            qWarning() << "🔥 File processing failed:" << e.what();
            interface.reportCanceled();
            interface.reportFinished();
        }
    };

    processor_->submitTask("process_files_batch", TaskPriority::Normal,
                           ExecutionContext::ThreadPool, task_func);

    return future;
}

// **ParallelUICompiler implementation**
ParallelUICompiler::ParallelUICompiler(QObject* parent) : QObject(parent) {
    processor_ = std::make_unique<ParallelProcessor>(this);

    // Connect signals
    connect(processor_.get(), &ParallelProcessor::taskCompleted, this,
            [this](const QString& task_id, bool success) {
                if (task_id.startsWith("compile_")) {
                    QString file_path =
                        task_id.mid(8);  // Remove "compile_" prefix
                    emit compilationCompleted(file_path, success);
                }
            });

    qDebug() << "🔥 ParallelUICompiler created";
}

QFuture<QStringList> ParallelFileProcessor::scanDirectoryAsync(
    const QString& directory_path, bool recursive) {
    QFutureInterface<QStringList> interface;
    QFuture<QStringList> future = interface.future();
    interface.reportStarted();

    auto task_func = [directory_path, recursive, interface]() mutable {
        QStringList file_paths;

        try {
            QDir dir(directory_path);
            if (!dir.exists()) {
                qWarning() << "🔥 Directory does not exist:" << directory_path;
                interface.reportResult(file_paths);
                interface.reportFinished();
                return;
            }

            QDirIterator::IteratorFlags flags = QDirIterator::NoIteratorFlags;
            if (recursive) {
                flags = QDirIterator::Subdirectories;
            }

            QDirIterator it(directory_path, QDir::Files | QDir::NoDotAndDotDot,
                            flags);
            while (it.hasNext()) {
                file_paths.append(it.next());
            }

            interface.reportResult(file_paths);
            interface.reportFinished();

        } catch (const std::exception& e) {
            qWarning() << "🔥 Directory scanning failed:" << e.what();
            interface.reportCanceled();
            interface.reportFinished();
        }
    };

    processor_->submitTask("scan_directory", TaskPriority::Low,
                           ExecutionContext::ThreadPool, task_func);

    return future;
}

QFuture<bool> ParallelFileProcessor::copyDirectoryAsync(
    const QString& source, const QString& destination) {
    QFutureInterface<bool> interface;
    QFuture<bool> future = interface.future();
    interface.reportStarted();

    auto task_func = [this, source, destination, interface]() mutable {
        try {
            QDir source_dir(source);
            if (!source_dir.exists()) {
                qWarning() << "🔥 Source directory does not exist:" << source;
                interface.reportResult(false);
                interface.reportFinished();
                return;
            }

            // Create destination directory
            QDir().mkpath(destination);

            // Copy files recursively
            bool success = copyDirectoryRecursive(source, destination);

            interface.reportResult(success);
            interface.reportFinished();

        } catch (const std::exception& e) {
            qWarning() << "🔥 Directory copying failed:" << e.what();
            interface.reportResult(false);
            interface.reportFinished();
        }
    };

    processor_->submitTask("copy_directory", TaskPriority::Normal,
                           ExecutionContext::ThreadPool, task_func);

    return future;
}

bool ParallelFileProcessor::copyDirectoryRecursive(const QString& source,
                                                   const QString& destination) {
    QDir source_dir(source);
    QDir dest_dir(destination);

    if (!dest_dir.exists()) {
        dest_dir.mkpath(".");
    }

    QFileInfoList entries = source_dir.entryInfoList(QDir::Files | QDir::Dirs |
                                                     QDir::NoDotAndDotDot);

    for (const QFileInfo& entry : entries) {
        QString dest_path = dest_dir.filePath(entry.fileName());

        if (entry.isDir()) {
            if (!copyDirectoryRecursive(entry.filePath(), dest_path)) {
                return false;
            }
        } else {
            if (!QFile::copy(entry.filePath(), dest_path)) {
                qWarning() << "🔥 Failed to copy file:" << entry.filePath()
                           << "to" << dest_path;
                return false;
            }
        }
    }

    return true;
}

QFuture<QJsonObject> ParallelUICompiler::compileUIAsync(
    const QString& ui_file_path) {
    QFutureInterface<QJsonObject> interface;
    QFuture<QJsonObject> future = interface.future();
    interface.reportStarted();

    auto task_func = [this, ui_file_path, interface]() mutable {
        QJsonObject result;

        try {
            QFile file(ui_file_path);
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                qWarning() << "🔥 Failed to open UI file:" << ui_file_path;
                result["error"] = "Failed to open file";
                interface.reportResult(result);
                interface.reportFinished();
                return;
            }

            QTextStream stream(&file);
            QString content = stream.readAll();
            file.close();

            // Mock UI compilation - in real implementation this would parse and
            // compile UI
            result["file_path"] = ui_file_path;
            result["compiled"] = true;
            result["timestamp"] =
                QDateTime::currentDateTime().toString(Qt::ISODate);
            result["content_hash"] = QString::number(qHash(content));
            result["components"] =
                QJsonArray();  // Would contain parsed components

            QMetaObject::invokeMethod(
                this,
                [this, ui_file_path]() {
                    emit compilationCompleted(ui_file_path, true);
                },
                Qt::QueuedConnection);

            interface.reportResult(result);
            interface.reportFinished();

        } catch (const std::exception& e) {
            qWarning() << "🔥 UI compilation failed:" << e.what();
            result["error"] = QString::fromStdString(e.what());

            QMetaObject::invokeMethod(
                this,
                [this, ui_file_path]() {
                    emit compilationCompleted(ui_file_path, false);
                },
                Qt::QueuedConnection);

            interface.reportResult(result);
            interface.reportFinished();
        }
    };

    processor_->submitTask("compile_" + ui_file_path, TaskPriority::Normal,
                           ExecutionContext::ThreadPool, task_func);

    return future;
}

QFuture<QStringList> ParallelUICompiler::compileUIBatchAsync(
    const QStringList& ui_file_paths) {
    QFutureInterface<QStringList> interface;
    QFuture<QStringList> future = interface.future();
    interface.reportStarted();

    auto task_func = [this, ui_file_paths, interface]() mutable {
        QStringList results;
        results.reserve(ui_file_paths.size());

        try {
            for (const QString& file_path : ui_file_paths) {
                // Mock compilation result
                QString result = QString("Compiled: %1").arg(file_path);
                results.append(result);

                QMetaObject::invokeMethod(
                    this,
                    [this, file_path]() {
                        emit compilationCompleted(file_path, true);
                    },
                    Qt::QueuedConnection);
            }

            interface.reportResult(results);
            interface.reportFinished();

        } catch (const std::exception& e) {
            qWarning() << "🔥 Batch UI compilation failed:" << e.what();
            interface.reportCanceled();
            interface.reportFinished();
        }
    };

    processor_->submitTask("compile_batch", TaskPriority::Normal,
                           ExecutionContext::ThreadPool, task_func);

    return future;
}

QFuture<bool> ParallelUICompiler::validateUIAsync(const QString& ui_file_path) {
    QFutureInterface<bool> interface;
    QFuture<bool> future = interface.future();
    interface.reportStarted();

    auto task_func = [ui_file_path, interface]() mutable {
        try {
            QFile file(ui_file_path);
            bool is_valid = file.exists() && file.size() > 0;

            // Mock validation - in real implementation would parse and validate
            // UI syntax
            if (is_valid) {
                QTextStream stream(&file);
                QString content = stream.readAll();
                is_valid = !content.isEmpty();
            }

            interface.reportResult(is_valid);
            interface.reportFinished();

        } catch (const std::exception& e) {
            qWarning() << "🔥 UI validation failed:" << e.what();
            interface.reportResult(false);
            interface.reportFinished();
        }
    };

    processor_->submitTask("validate_" + ui_file_path, TaskPriority::Normal,
                           ExecutionContext::ThreadPool, task_func);

    return future;
}

QFuture<QStringList> ParallelUICompiler::resolveDependenciesAsync(
    const QString& ui_file_path) {
    QFutureInterface<QStringList> interface;
    QFuture<QStringList> future = interface.future();
    interface.reportStarted();

    auto task_func = [this, ui_file_path, interface]() mutable {
        QStringList dependencies;

        try {
            // Check cache first
            {
                std::shared_lock<std::shared_mutex> lock(cache_mutex_);
                auto it = dependency_cache_.find(ui_file_path);
                if (it != dependency_cache_.end()) {
                    dependencies = it->second;
                    interface.reportResult(dependencies);
                    interface.reportFinished();
                    return;
                }
            }

            // Mock dependency resolution - in real implementation would parse
            // imports/includes
            QFile file(ui_file_path);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream stream(&file);
                QString content = stream.readAll();
                file.close();

                // Simple mock: look for common dependency patterns
                if (content.contains("import")) {
                    dependencies.append("common/base.ui");
                }
                if (content.contains("component")) {
                    dependencies.append("components/widgets.ui");
                }
            }

            // Update cache
            {
                std::unique_lock<std::shared_mutex> lock(cache_mutex_);
                dependency_cache_[ui_file_path] = dependencies;
            }

            QMetaObject::invokeMethod(
                this,
                [this, ui_file_path, dependencies]() {
                    emit dependencyResolved(ui_file_path, dependencies);
                },
                Qt::QueuedConnection);

            interface.reportResult(dependencies);
            interface.reportFinished();

        } catch (const std::exception& e) {
            qWarning() << "🔥 Dependency resolution failed:" << e.what();
            interface.reportCanceled();
            interface.reportFinished();
        }
    };

    processor_->submitTask("resolve_deps_" + ui_file_path, TaskPriority::Low,
                           ExecutionContext::ThreadPool, task_func);

    return future;
}

QFuture<bool> ParallelUICompiler::checkDependencyChangesAsync(
    const QStringList& dependency_paths) {
    QFutureInterface<bool> interface;
    QFuture<bool> future = interface.future();
    interface.reportStarted();

    auto task_func = [dependency_paths, interface]() mutable {
        try {
            bool has_changes = false;

            for (const QString& dep_path : dependency_paths) {
                QFileInfo file_info(dep_path);
                if (file_info.exists()) {
                    // Mock change detection - in real implementation would
                    // compare timestamps
                    QDateTime last_modified = file_info.lastModified();
                    QDateTime threshold = QDateTime::currentDateTime().addSecs(
                        -3600);  // 1 hour ago

                    if (last_modified > threshold) {
                        has_changes = true;
                        break;
                    }
                }
            }

            interface.reportResult(has_changes);
            interface.reportFinished();

        } catch (const std::exception& e) {
            qWarning() << "🔥 Dependency change check failed:" << e.what();
            interface.reportResult(false);
            interface.reportFinished();
        }
    };

    processor_->submitTask("check_deps_changes", TaskPriority::Low,
                           ExecutionContext::ThreadPool, task_func);

    return future;
}

// **ParallelPropertyBinder implementation**
ParallelPropertyBinder::ParallelPropertyBinder(QObject* parent)
    : QObject(parent) {
    processor_ = std::make_unique<ParallelProcessor>(this);

    // Connect signals
    connect(processor_.get(), &ParallelProcessor::taskCompleted, this,
            [this](const QString& task_id, bool success) {
                if (task_id.startsWith("bind_")) {
                    QString binding_id =
                        task_id.mid(5);  // Remove "bind_" prefix
                    emit propertyBound(binding_id, success);
                }
            });

    qDebug() << "🔥 ParallelPropertyBinder created";
}

QFuture<bool> ParallelPropertyBinder::updateBindingsAsync(
    const QStringList& binding_ids) {
    QFutureInterface<bool> interface;
    QFuture<bool> future = interface.future();
    interface.reportStarted();

    auto task_func = [this, binding_ids, interface]() mutable {
        bool all_success = true;

        try {
            std::shared_lock<std::shared_mutex> lock(bindings_mutex_);

            for (const QString& binding_id : binding_ids) {
                auto it = bindings_.find(binding_id);
                if (it != bindings_.end() && it->second.is_active) {
                    try {
                        QVariant new_value = it->second.value_provider();

                        // Update property on main thread
                        QMetaObject::invokeMethod(
                            this,
                            [this, binding_id, new_value,
                             object = it->second.object,
                             property_name = it->second.property_name]() {
                                if (object &&
                                    object->setProperty(
                                        property_name.toUtf8().constData(),
                                        new_value)) {
                                    emit bindingUpdated(binding_id, new_value);
                                }
                            },
                            Qt::QueuedConnection);

                    } catch (const std::exception& e) {
                        qWarning() << "🔥 Binding update failed for"
                                   << binding_id << ":" << e.what();
                        all_success = false;
                    }
                }
            }

            interface.reportResult(all_success);
            interface.reportFinished();

        } catch (const std::exception& e) {
            qWarning() << "🔥 Batch binding update failed:" << e.what();
            interface.reportResult(false);
            interface.reportFinished();
        }
    };

    processor_->submitTask("update_bindings", TaskPriority::Normal,
                           ExecutionContext::ThreadPool, task_func);

    return future;
}

QFuture<bool> ParallelPropertyBinder::validateBindingsAsync() {
    QFutureInterface<bool> interface;
    QFuture<bool> future = interface.future();
    interface.reportStarted();

    auto task_func = [this, interface]() mutable {
        bool all_valid = true;

        try {
            std::shared_lock<std::shared_mutex> lock(bindings_mutex_);

            for (const auto& [binding_id, binding] : bindings_) {
                if (!binding.object) {
                    qWarning()
                        << "🔥 Invalid binding - null object:" << binding_id;
                    all_valid = false;
                    continue;
                }

                // Check if property exists
                QMetaObject::Connection conn = QObject::connect(
                    binding.object, &QObject::destroyed, [binding_id]() {
                        qDebug() << "🔥 Bound object destroyed:" << binding_id;
                    });
                QObject::disconnect(conn);

                if (binding.object->metaObject()->indexOfProperty(
                        binding.property_name.toUtf8().constData()) < 0) {
                    qWarning()
                        << "🔥 Invalid binding - property does not exist:"
                        << binding.property_name << "on"
                        << binding.object->metaObject()->className();
                    all_valid = false;
                }
            }

            interface.reportResult(all_valid);
            interface.reportFinished();

        } catch (const std::exception& e) {
            qWarning() << "🔥 Binding validation failed:" << e.what();
            interface.reportResult(false);
            interface.reportFinished();
        }
    };

    processor_->submitTask("validate_bindings", TaskPriority::Low,
                           ExecutionContext::ThreadPool, task_func);

    return future;
}

QFuture<int> ParallelPropertyBinder::updatePropertiesBatchAsync(
    const QList<QObject*>& objects, const QStringList& property_names,
    const QVariantList& values) {
    QFutureInterface<int> interface;
    QFuture<int> future = interface.future();
    interface.reportStarted();

    auto task_func = [this, objects, property_names, values,
                      interface]() mutable {
        int updated_count = 0;
        int failed_count = 0;

        try {
            int min_size = std::min(
                {objects.size(), property_names.size(), values.size()});

            for (int i = 0; i < min_size; ++i) {
                QObject* object = objects[i];
                const QString& property_name = property_names[i];
                const QVariant& value = values[i];

                if (!object) {
                    failed_count++;
                    continue;
                }

                // Update property on main thread
                QMetaObject::invokeMethod(
                    this,
                    [object, property_name, value, &updated_count,
                     &failed_count]() {
                        if (object->setProperty(
                                property_name.toUtf8().constData(), value)) {
                            updated_count++;
                        } else {
                            failed_count++;
                        }
                    },
                    Qt::BlockingQueuedConnection);
            }

            QMetaObject::invokeMethod(
                this,
                [this, updated_count, failed_count]() {
                    emit batchUpdateCompleted(updated_count, failed_count);
                },
                Qt::QueuedConnection);

            interface.reportResult(updated_count);
            interface.reportFinished();

        } catch (const std::exception& e) {
            qWarning() << "🔥 Batch property update failed:" << e.what();
            interface.reportResult(updated_count);
            interface.reportFinished();
        }
    };

    processor_->submitTask("batch_property_update", TaskPriority::Normal,
                           ExecutionContext::ThreadPool, task_func);

    return future;
}

}  // namespace DeclarativeUI::Core
