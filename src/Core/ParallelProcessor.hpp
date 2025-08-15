#pragma once

#include <QFuture>
#include <QFutureWatcher>
#include <QJsonObject>
#include <QMutex>
#include <QObject>
#include <QReadWriteLock>
#include <QString>
#include <QThread>
#include <QTimer>
#include <QWaitCondition>
// #include <QtConcurrent>  // Commented out - may not be available in this Qt
// installation

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <queue>
#include <shared_mutex>
#include <thread>
#include <unordered_map>
#include <vector>

namespace DeclarativeUI::Core {

/**
 * @file ParallelProcessor.hpp
 * @brief High-level parallel task scheduling and utility processors used by the
 * DeclarativeUI core.
 *
 * This header declares:
 * - TaskPriority and ExecutionContext enums used to express scheduling
 * preferences.
 * - TaskResult<T> wrapper to capture result, success, timing and error
 * information.
 * - ITask interface and templated Task<ResultType> concrete task for invoking
 * functions with completion callbacks.
 * - ThreadPool: a priority-aware thread pool implementation.
 * - ParallelProcessor: a Qt QObject that provides higher-level task submission,
 * batching, monitoring and integration with the application's event loop
 * (signals for completion/failure).
 * - Utility processor classes: ParallelFileProcessor, ParallelUICompiler and
 * ParallelPropertyBinder which build on ParallelProcessor for domain-specific
 * parallel operations.
 *
 * Notes:
 * - The implementation uses C++11/14 concurrency primitives and integrates with
 * Qt for signalling and main-thread dispatching. This file is header-only for
 * templated members; non-templated implementation is expected in the
 *   corresponding source file where required.
 *
 * @author DeclarativeUI
 * @date 2025
 */

/**
 * @enum DeclarativeUI::Core::TaskPriority
 * @brief Priority levels for scheduling tasks.
 *
 * Values are ordered from low to high priority. The ThreadPool and scheduler
 * should prefer tasks with higher priority values when selecting the next task
 * to run.
 */
enum class TaskPriority { Low = 0, Normal = 1, High = 2, Critical = 3 };

/**
 * @enum DeclarativeUI::Core::ExecutionContext
 * @brief Execution target context for submitted tasks.
 *
 * ExecutionContext describes where the task should run:
 * - MainThread: executed through Qt's event system on the application's main
 * (UI) thread.
 * - WorkerThread: executed on a long-living worker thread.
 * - ThreadPool: executed on the pool managed by ThreadPool.
 * - Background: low-priority background thread(s) — mapped internally to the
 * thread pool by default.
 */
enum class ExecutionContext {
    MainThread,
    WorkerThread,
    ThreadPool,
    Background
};

/**
 * @struct TaskResult
 * @brief Generic container describing the outcome of a task.
 *
 * @tparam T Type returned by the task function (use void for no return value).
 *
 * Members:
 * - result: returned value when success==true (undefined if void or on
 * failure).
 * - success: boolean indicating whether the task completed successfully.
 * - error_message: text describing any error that occurred.
 * - execution_time: elapsed wall-clock duration measured while executing the
 * task.
 * - completed_at: QDateTime timestamp when the task finished.
 *
 * Convenience:
 * - isValid(): returns success.
 * - operator bool(): returns success to allow condition-like checks.
 */
template <typename T>
struct TaskResult {
    T result;
    bool success = false;
    QString error_message;
    std::chrono::milliseconds execution_time{0};
    QDateTime completed_at;

    bool isValid() const { return success; }
    operator bool() const { return success; }
};

/**
 * @class ITask
 * @brief Abstract interface representing a scheduled/executable task.
 *
 * Subclasses must implement execute() to perform the work and provide metadata
 * methods used by the scheduler.
 */
class ITask {
public:
    virtual ~ITask() = default;

    /**
     * @brief Execute the task synchronously in the current thread.
     *
     * This method is called by scheduling code when it assigns a task to a
     * thread. Implementations must ensure exception-safety (exceptions should
     * be caught and handled).
     */
    virtual void execute() = 0;

    /**
     * @brief Get the priority assigned to the task.
     * @return TaskPriority indicating scheduling priority.
     */
    virtual TaskPriority getPriority() const = 0;

    /**
     * @brief Get the execution context where this task prefers to run.
     * @return ExecutionContext describing the preferred target.
     */
    virtual ExecutionContext getExecutionContext() const = 0;

    /**
     * @brief Unique identifier associated with the task.
     * @return QString task id.
     */
    virtual QString getTaskId() const = 0;

    /**
     * @brief Human-readable description of the task.
     * @return QString description (may be empty).
     */
    virtual QString getDescription() const = 0;
};

/**
 * @class Task
 * @brief Concrete templated task wrapper that invokes a std::function and
 * stores a TaskResult.
 *
 * The Task class adapts a callable (with optional return value) into the ITask
 * uniform interface. It also supports an optional completion callback invoked
 * after the task finishes.
 *
 * @tparam ResultType Callable return type. Use void for no result.
 */
template <typename ResultType>
class Task : public ITask {
public:
    using TaskFunction = std::function<ResultType()>;
    using CompletionCallback =
        std::function<void(const TaskResult<ResultType>&)>;

    /**
     * @brief Construct a Task
     * @param task_id Unique identifier to track the task.
     * @param func Callable to invoke when execute() is called.
     * @param priority Scheduling priority (default Normal).
     * @param context ExecutionContext preference (default ThreadPool).
     */
    Task(const QString& task_id, TaskFunction func,
         TaskPriority priority = TaskPriority::Normal,
         ExecutionContext context = ExecutionContext::ThreadPool)
        : task_id_(task_id),
          function_(std::move(func)),
          priority_(priority),
          context_(context) {}

    /**
     * @brief Execute the wrapped callable and capture timing and error state in
     * result_.
     *
     * The implementation catches std::exception and all other exceptions,
     * stores an error_message, sets success appropriately and invokes any
     * registered completion callback.
     */
    void execute() override {
        auto start_time = std::chrono::steady_clock::now();

        try {
            if constexpr (std::is_void_v<ResultType>) {
                function_();
                result_.success = true;
            } else {
                result_.result = function_();
                result_.success = true;
            }
        } catch (const std::exception& e) {
            result_.success = false;
            result_.error_message = QString::fromStdString(e.what());
        } catch (...) {
            result_.success = false;
            result_.error_message = "Unknown error occurred";
        }

        auto end_time = std::chrono::steady_clock::now();
        result_.execution_time =
            std::chrono::duration_cast<std::chrono::milliseconds>(end_time -
                                                                  start_time);
        result_.completed_at = QDateTime::currentDateTime();

        if (completion_callback_) {
            completion_callback_(result_);
        }
    }

    TaskPriority getPriority() const override { return priority_; }
    ExecutionContext getExecutionContext() const override { return context_; }
    QString getTaskId() const override { return task_id_; }
    QString getDescription() const override { return description_; }

    /**
     * @brief Register a completion callback invoked after the task finishes.
     * @param callback Callable receiving const TaskResult<ResultType>&.
     */
    void setCompletionCallback(CompletionCallback callback) {
        completion_callback_ = std::move(callback);
    }

    /**
     * @brief Set a human-readable description for the task.
     */
    void setDescription(const QString& description) {
        description_ = description;
    }

    /**
     * @brief Retrieve the last TaskResult captured by this Task.
     * @return const TaskResult<ResultType>&
     */
    const TaskResult<ResultType>& getResult() const { return result_; }

private:
    QString task_id_;
    QString description_;
    TaskFunction function_;
    TaskPriority priority_;
    ExecutionContext context_;
    CompletionCallback completion_callback_;
    TaskResult<ResultType> result_;
};

/**
 * @class ThreadPool
 * @brief Simple priority-aware thread pool.
 *
 * The ThreadPool accepts enqueued tasks with an associated TaskPriority. Tasks
 * are placed into a priority queue (higher TaskPriority runs first). Each
 * worker thread repeatedly pops the next TaskWrapper and executes it.
 *
 * Public methods:
 * - enqueue(): schedule a callable for execution and get a std::future for its
 * result.
 * - shutdown(): stop accepting tasks and join worker threads.
 * - pause()/resume(): temporarily pause worker threads from dequeuing new
 * tasks.
 *
 * Thread-safety:
 * - enqueue() can be called concurrently from multiple threads.
 * - queued_tasks() and active_threads() provide monitoring information.
 */
class ThreadPool {
public:
    explicit ThreadPool(
        size_t thread_count = std::thread::hardware_concurrency());
    ~ThreadPool();

    /**
     * @brief Enqueue a callable into the thread pool with the requested
     * priority.
     *
     * The callable is wrapped in a packaged_task; caller receives a std::future
     * holding the result.
     *
     * @tparam F Callable type.
     * @tparam Args Argument types forwarded into the callable.
     * @param priority Scheduling priority.
     * @param f Callable to run.
     * @param args Arguments to forward to the callable.
     * @return std::future<return_type> future for the callable's result.
     *
     * @throws std::runtime_error if the thread pool is not running.
     */
    template <typename F, typename... Args>
    auto enqueue(TaskPriority priority, F&& f, Args&&... args)
        -> std::future<typename std::result_of<F(Args...)>::type>;

    void shutdown();
    void pause();
    void resume();

    /**
     * @brief Number of worker threads currently executing tasks.
     */
    size_t active_threads() const { return active_threads_.load(); }

    /**
     * @brief Approximate number of tasks currently queued.
     */
    size_t queued_tasks() const;

    /**
     * @brief Returns whether the pool is accepting and running tasks.
     */
    bool is_running() const { return running_.load(); }

private:
    struct TaskWrapper {
        std::function<void()> task;
        TaskPriority priority;
        std::chrono::steady_clock::time_point enqueue_time;

        /**
         * @brief Priority comparison operator for std::priority_queue
         *
         * The operator is implemented so that the priority_queue pops the
         * highest priority first. If priorities are equal, older tasks (earlier
         * enqueue_time) are preferred.
         */
        bool operator<(const TaskWrapper& other) const {
            if (priority != other.priority) {
                return priority < other.priority;  // Higher priority first
            }
            return enqueue_time > other.enqueue_time;  // Earlier tasks first
        }
    };

    std::vector<std::thread> workers_;
    std::priority_queue<TaskWrapper> task_queue_;
    mutable std::mutex queue_mutex_;
    std::condition_variable condition_;
    std::atomic<bool> running_{true};
    std::atomic<bool> paused_{false};
    std::atomic<size_t> active_threads_{0};

    /**
     * @brief Entry point function for each worker thread.
     *
     * Worker threads wait on the condition variable for new tasks, then pop the
     * highest priority TaskWrapper and execute its callable. Respect
     * pause/resume semantics and update active_threads_ counters.
     */
    void worker_thread();
};

/**
 * @class ParallelProcessor
 * @brief High-level task scheduler and monitor that integrates with Qt.
 *
 * ParallelProcessor manages task submission to different execution contexts
 * (main thread, worker threads, thread pool, background). It provides batching
 * helpers, task cancellation, queue controls, and performance monitoring
 * (timers/reporting).
 *
 * The class emits signals on task completion/failure and for batch lifecycle
 * events so QML/UI layers can observe progress.
 */
class ParallelProcessor : public QObject {
    Q_OBJECT

public:
    explicit ParallelProcessor(QObject* parent = nullptr);
    ~ParallelProcessor() override;

    /**
     * @brief Submit a task to be executed according to the given context.
     *
     * The task callable is provided as a function (or function+args via
     * forwarding). If task_id is empty, the processor will generate a unique
     * id. The returned QString is the actual task id (generated or provided).
     *
     * @tparam F Callable type.
     * @tparam Args Argument types forwarded to the callable.
     * @param task_id Optional human-provided task identifier (may be empty).
     * @param priority TaskPriority for scheduler.
     * @param context ExecutionContext preference for this task.
     * @param func Callable (and forwarded args) to execute.
     * @return QString actual task id used.
     */
    template <typename F, typename... Args>
    QString submitTask(const QString& task_id, TaskPriority priority,
                       ExecutionContext context, F&& func, Args&&... args);

    /**
     * @brief Convenience for submitting a low-priority background task.
     */
    template <typename F>
    QString submitBackgroundTask(const QString& task_id, F&& func) {
        return submitTask(task_id, TaskPriority::Low,
                          ExecutionContext::Background, std::forward<F>(func));
    }

    /**
     * @brief Convenience for submitting a high-priority pool task.
     */
    template <typename F>
    QString submitHighPriorityTask(const QString& task_id, F&& func) {
        return submitTask(task_id, TaskPriority::High,
                          ExecutionContext::ThreadPool, std::forward<F>(func));
    }

    /**
     * @brief Submit a batch of tasks described by 'items' where each item is
     * passed to func(item).
     *
     * Each task in the returned vector contains the generated unique id for
     * that item.
     *
     * @tparam Container Iterable container type (e.g., std::vector,
     * QStringList).
     * @tparam F Callable type which will be invoked as func(item).
     * @param batch_id Identifier used to group tasks into a batch for later
     * cancellation or progress reporting.
     * @param items Collection of items to process.
     * @param func Callable applied to each item.
     * @return std::vector<QString> list of task ids created for the batch.
     */
    template <typename Container, typename F>
    std::vector<QString> submitBatchTasks(const QString& batch_id,
                                          const Container& items, F&& func);

    // **Task management**
    void cancelTask(const QString& task_id);
    void cancelBatch(const QString& batch_id);
    bool isTaskRunning(const QString& task_id) const;
    bool isTaskCompleted(const QString& task_id) const;

    // **Thread pool management**
    void setThreadPoolSize(size_t size);
    void pauseProcessing();
    void resumeProcessing();
    void setMaxQueueSize(size_t max_size);

    // **Performance monitoring**
    QJsonObject getPerformanceMetrics() const;
    size_t getActiveTaskCount() const;
    size_t getQueuedTaskCount() const;
    double getAverageExecutionTime() const;

    // **Configuration**
    void enableTaskProfiling(bool enabled);
    void setTaskTimeout(std::chrono::milliseconds timeout);
    void enableLoadBalancing(bool enabled);

signals:
    /**
     * @brief Emitted when a task completed successfully.
     * @param task_id Identifier of the completed task.
     * @param success True when completed with success.
     */
    void taskCompleted(const QString& task_id, bool success);

    /**
     * @brief Emitted when a task failed to complete.
     * @param task_id Identifier of the failed task.
     * @param error Human-readable error message.
     */
    void taskFailed(const QString& task_id, const QString& error);

    /**
     * @brief Emitted when a batch finishes processing (all tasks completed or
     * cancelled).
     * @param batch_id Identifier of the batch.
     * @param completed_count Number of tasks that completed successfully.
     * @param failed_count Number of tasks that failed.
     */
    void batchCompleted(const QString& batch_id, int completed_count,
                        int failed_count);

    /**
     * @brief Emitted when the internal queue size exceeds configured
     * thresholds.
     * @param queue_size Current queue size.
     */
    void queueOverflow(size_t queue_size);

    /**
     * @brief Generic performance alert signal for metrics of interest.
     * @param metric Name of the metric.
     * @param value Numeric value associated with the alert.
     */
    void performanceAlert(const QString& metric, double value);

private slots:
    /**
     * @brief Slot invoked when a periodic timeout check triggers.
     *
     * This slot is intended to cancel or mark tasks that exceeded the
     * configured task_timeout_.
     */
    void onTaskTimeout();

    /**
     * @brief Slot invoked on the periodic performance timer to evaluate
     * metrics.
     *
     * It may emit performanceAlert signals when thresholds are crossed.
     */
    void onPerformanceCheck();

private:
    // **Core infrastructure**
    std::unique_ptr<ThreadPool> thread_pool_;
    std::unordered_map<QString, std::shared_ptr<ITask>> active_tasks_;
    std::unordered_map<QString, std::vector<QString>> batch_tasks_;
    mutable std::shared_mutex tasks_mutex_;

    // **Configuration**
    std::atomic<size_t> max_queue_size_{1000};
    std::atomic<bool> task_profiling_enabled_{false};
    std::atomic<bool> load_balancing_enabled_{true};
    std::chrono::milliseconds task_timeout_{30000};  // 30 seconds

    // **Performance tracking**
    std::atomic<size_t> total_tasks_executed_{0};
    std::atomic<size_t> total_tasks_failed_{0};
    std::atomic<double> total_execution_time_{0.0};
    std::atomic<size_t> peak_queue_size_{0};

    // **Timers**
    std::unique_ptr<QTimer> timeout_timer_;
    std::unique_ptr<QTimer> performance_timer_;

    // **Internal methods**
    /**
     * @brief Generate a unique task identifier.
     * @return QString unique id.
     */
    QString generateTaskId() const;

    /**
     * @brief Update internal performance counters after a task completes.
     * @param task_id Task identifier (for logging or per-task metrics).
     * @param execution_time Duration the task took to execute.
     * @param success Whether the task succeeded.
     */
    void updatePerformanceMetrics(const QString& task_id,
                                  std::chrono::milliseconds execution_time,
                                  bool success);

    /**
     * @brief Check the current task queue size against max_queue_size_ and emit
     * queueOverflow if required.
     */
    void checkQueueOverflow();

    /**
     * @brief Attempt to rebalance work between execution contexts (no-op if
     * load balancing disabled).
     *
     * This method is a hook point for policies that move tasks between worker
     * threads and the thread pool, or that throttle low-priority work when the
     * UI is busy.
     */
    void balanceLoad();

    /**
     * @brief Periodically remove or free completed task metadata from internal
     * containers to bound memory usage.
     */
    void cleanupCompletedTasks();
};

/**
 * @class ParallelFileProcessor
 * @brief Helper class that exposes file I/O operations executed in parallel
 * using ParallelProcessor.
 *
 * The methods return QFuture so callers can integrate with QtConcurrent-like
 * patterns or QFutureWatcher. The class emits fileProcessed signals for
 * per-file reporting and batchProgress for progress updates.
 */
class ParallelFileProcessor : public QObject {
    Q_OBJECT

public:
    explicit ParallelFileProcessor(QObject* parent = nullptr);

    /**
     * @brief Read multiple files asynchronously.
     * @param file_paths List of file paths to read.
     * @return QFuture<QStringList> containing file contents in the same order
     * as file_paths.
     */
    QFuture<QStringList> readFilesAsync(const QStringList& file_paths);

    /**
     * @brief Write multiple files asynchronously.
     * @param file_paths Target file paths.
     * @param contents Corresponding contents for each file.
     * @return QFuture<bool> true if all writes succeeded, false otherwise.
     */
    QFuture<bool> writeFilesAsync(const QStringList& file_paths,
                                  const QStringList& contents);

    /**
     * @brief Apply a CPU-bound string processor to each file content in
     * parallel.
     * @param file_paths List of files to process.
     * @param processor Callable that maps file content to a processed QString.
     * @return QFuture<QStringList> processed results.
     */
    QFuture<QStringList> processFilesAsync(
        const QStringList& file_paths,
        std::function<QString(const QString&)> processor);

    /**
     * @brief Scan a directory optionally recursively and return matching file
     * paths.
     */
    QFuture<QStringList> scanDirectoryAsync(const QString& directory_path,
                                            bool recursive = true);

    /**
     * @brief Copy a directory tree asynchronously.
     */
    QFuture<bool> copyDirectoryAsync(const QString& source,
                                     const QString& destination);

signals:
    void fileProcessed(const QString& file_path, bool success);
    void batchProgress(int completed, int total);

private:
    std::unique_ptr<ParallelProcessor> processor_;

    /**
     * @brief Helper method for recursive directory copying.
     */
    bool copyDirectoryRecursive(const QString& source,
                                const QString& destination);
};

/**
 * @class ParallelUICompiler
 * @brief Parallelized compiler for UI assets and dependency resolution.
 *
 * This component uses ParallelProcessor to compile UI definitions and resolve
 * dependencies concurrently.
 */
class ParallelUICompiler : public QObject {
    Q_OBJECT

public:
    explicit ParallelUICompiler(QObject* parent = nullptr);

    /**
     * @brief Compile a single UI file asynchronously and produce a JSON
     * representation.
     */
    QFuture<QJsonObject> compileUIAsync(const QString& ui_file_path);

    /**
     * @brief Compile multiple UI files in parallel.
     */
    QFuture<QStringList> compileUIBatchAsync(const QStringList& ui_file_paths);

    /**
     * @brief Validate a UI file asynchronously; returns true if valid.
     */
    QFuture<bool> validateUIAsync(const QString& ui_file_path);

    /**
     * @brief Resolve external dependencies of a UI file and return the list of
     * dependency paths.
     */
    QFuture<QStringList> resolveDependenciesAsync(const QString& ui_file_path);

    /**
     * @brief Check whether any dependencies have changed relative to cached
     * state.
     */
    QFuture<bool> checkDependencyChangesAsync(
        const QStringList& dependency_paths);

signals:
    void compilationCompleted(const QString& file_path, bool success);
    void dependencyResolved(const QString& file_path,
                            const QStringList& dependencies);

private:
    std::unique_ptr<ParallelProcessor> processor_;
    std::unordered_map<QString, QStringList> dependency_cache_;
    mutable std::shared_mutex cache_mutex_;
};

/**
 * @class ParallelPropertyBinder
 * @brief Manages asynchronous property bindings using background tasks.
 *
 * Bindings store a value provider callable which is invoked off the main thread
 * to compute a new property value. Results are applied back to QObject
 * properties on the main thread to remain thread-safe with Qt.
 */
class ParallelPropertyBinder : public QObject {
    Q_OBJECT

public:
    explicit ParallelPropertyBinder(QObject* parent = nullptr);

    /**
     * @brief Bind a property asynchronously to a provider function that
     * computes the value off the UI thread.
     *
     * @tparam T Type returned by the provider.
     * @param object QObject owning the property.
     * @param property_name Name of the property to bind.
     * @param value_provider Callable that returns a T which will be converted
     * to QVariant when applied.
     * @return QFuture<bool> indicating success of initial binding application.
     */
    template <typename T>
    QFuture<bool> bindPropertyAsync(QObject* object,
                                    const QString& property_name,
                                    std::function<T()> value_provider);

    QFuture<bool> updateBindingsAsync(const QStringList& binding_ids);
    QFuture<bool> validateBindingsAsync();

    /**
     * @brief Update a batch of properties in parallel.
     * @param objects List of QObject pointers to update.
     * @param property_names List of property names to update for each target
     * object.
     * @param values QVariantList containing values applied in a round-robin or
     * parallel mapping (caller responsibility).
     * @return QFuture<int> number of successfully updated properties.
     */
    QFuture<int> updatePropertiesBatchAsync(const QList<QObject*>& objects,
                                            const QStringList& property_names,
                                            const QVariantList& values);

signals:
    void propertyBound(const QString& binding_id, bool success);
    void bindingUpdated(const QString& binding_id, const QVariant& new_value);
    void batchUpdateCompleted(int updated_count, int failed_count);

private:
    struct PropertyBinding {
        QObject* object;
        QString property_name;
        std::function<QVariant()> value_provider;
        QDateTime last_update;
        bool is_active = true;
    };

    std::unique_ptr<ParallelProcessor> processor_;
    std::unordered_map<QString, PropertyBinding> bindings_;
    mutable std::shared_mutex bindings_mutex_;
};

/**
 * @brief Template implementation of ThreadPool::enqueue.
 *
 * This function is defined in header because it is templated. It wraps the
 * callable in a packaged_task, pushes a TaskWrapper into the internal priority
 * queue, and notifies worker threads.
 *
 * The return future becomes ready once a worker thread executes the
 * packaged_task.
 */
template <typename F, typename... Args>
auto ThreadPool::enqueue(TaskPriority priority, F&& f, Args&&... args)
    -> std::future<typename std::result_of<F(Args...)>::type> {
    using return_type = typename std::result_of<F(Args...)>::type;

    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    std::future<return_type> result = task->get_future();

    {
        std::unique_lock<std::mutex> lock(queue_mutex_);

        if (!running_.load()) {
            throw std::runtime_error("ThreadPool is not running");
        }

        TaskWrapper wrapper;
        wrapper.task = [task]() { (*task)(); };
        wrapper.priority = priority;
        wrapper.enqueue_time = std::chrono::steady_clock::now();

        task_queue_.push(wrapper);
    }

    condition_.notify_one();
    return result;
}

/**
 * @brief Template wrapper for submitting a task through ParallelProcessor.
 *
 * This overload forwards arguments into a std::bind and chooses dispatch
 * mechanics based on ExecutionContext. For main-thread tasks it uses
 * QMetaObject::invokeMethod with Qt::QueuedConnection to schedule execution on
 * the Qt event loop. For other contexts it enqueues the callable into the
 * internal ThreadPool.
 *
 * The template is defined here in the header because it depends on the callable
 * type.
 */
template <typename F, typename... Args>
QString ParallelProcessor::submitTask(const QString& task_id,
                                      TaskPriority priority,
                                      ExecutionContext context, F&& func,
                                      Args&&... args) {
    QString actual_task_id = task_id.isEmpty() ? generateTaskId() : task_id;

    // Check queue overflow
    checkQueueOverflow();

    auto task_func = [func = std::forward<F>(func), args...]() mutable {
        return func(args...);
    };

    switch (context) {
        case ExecutionContext::MainThread: {
            // Execute on main thread using Qt's event system
            QMetaObject::invokeMethod(
                this,
                [this, actual_task_id, task_func]() mutable {
                    auto start_time = std::chrono::steady_clock::now();
                    bool success = true;

                    try {
                        task_func();
                    } catch (const std::exception& e) {
                        success = false;
                        emit taskFailed(actual_task_id,
                                        QString::fromStdString(e.what()));
                    } catch (...) {
                        success = false;
                        emit taskFailed(actual_task_id, "Unknown error");
                    }

                    auto end_time = std::chrono::steady_clock::now();
                    auto duration =
                        std::chrono::duration_cast<std::chrono::milliseconds>(
                            end_time - start_time);

                    updatePerformanceMetrics(actual_task_id, duration, success);

                    if (success) {
                        emit taskCompleted(actual_task_id, true);
                    }
                },
                Qt::QueuedConnection);
            break;
        }

        case ExecutionContext::ThreadPool:
        case ExecutionContext::WorkerThread:
        case ExecutionContext::Background: {
            // Execute on thread pool
            auto future = thread_pool_->enqueue(
                priority, [this, actual_task_id, task_func]() mutable {
                    auto start_time = std::chrono::steady_clock::now();
                    bool success = true;

                    try {
                        task_func();
                    } catch (const std::exception& e) {
                        success = false;
                        QMetaObject::invokeMethod(
                            this,
                            [this, actual_task_id, e]() {
                                emit taskFailed(
                                    actual_task_id,
                                    QString::fromStdString(e.what()));
                            },
                            Qt::QueuedConnection);
                    } catch (...) {
                        success = false;
                        QMetaObject::invokeMethod(
                            this,
                            [this, actual_task_id]() {
                                emit taskFailed(actual_task_id,
                                                "Unknown error");
                            },
                            Qt::QueuedConnection);
                    }

                    auto end_time = std::chrono::steady_clock::now();
                    auto duration =
                        std::chrono::duration_cast<std::chrono::milliseconds>(
                            end_time - start_time);

                    updatePerformanceMetrics(actual_task_id, duration, success);

                    if (success) {
                        QMetaObject::invokeMethod(
                            this,
                            [this, actual_task_id]() {
                                emit taskCompleted(actual_task_id, true);
                            },
                            Qt::QueuedConnection);
                    }
                });
            break;
        }
    }

    return actual_task_id;
}

/**
 * @brief Batch submission implementation that registers the batch and submits
 * each item as an independent task.
 *
 * The returned vector holds the ids for each created task in the same order as
 * 'items'.
 */
template <typename Container, typename F>
std::vector<QString> ParallelProcessor::submitBatchTasks(
    const QString& batch_id, const Container& items, F&& func) {
    std::vector<QString> task_ids;
    task_ids.reserve(items.size());

    {
        std::unique_lock<std::shared_mutex> lock(tasks_mutex_);
        batch_tasks_[batch_id] = std::vector<QString>();
        batch_tasks_[batch_id].reserve(items.size());
    }

    int index = 0;
    for (const auto& item : items) {
        QString task_id = QString("%1_task_%2").arg(batch_id).arg(index++);

        auto task_func = [func, item]() { return func(item); };

        submitTask(task_id, TaskPriority::Normal, ExecutionContext::ThreadPool,
                   task_func);

        task_ids.push_back(task_id);

        {
            std::unique_lock<std::shared_mutex> lock(tasks_mutex_);
            batch_tasks_[batch_id].push_back(task_id);
        }
    }

    return task_ids;
}

/**
 * @brief Template implementation for ParallelPropertyBinder::bindPropertyAsync.
 *
 * This function creates an asynchronous property binding that computes values
 * off the main thread and applies them to QObject properties on the main thread
 * for thread safety.
 */
template <typename T>
QFuture<bool> ParallelPropertyBinder::bindPropertyAsync(
    QObject* object, const QString& property_name,
    std::function<T()> value_provider) {
    QFutureInterface<bool> interface;
    QFuture<bool> future = interface.future();
    interface.reportStarted();

    if (!object) {
        qWarning() << "🔥 Cannot bind to null object";
        interface.reportResult(false);
        interface.reportFinished();
        return future;
    }

    QString binding_id =
        QString("%1_%2_%3")
            .arg(object->objectName().isEmpty() ? "object"
                                                : object->objectName())
            .arg(property_name)
            .arg(QUuid::createUuid().toString(QUuid::WithoutBraces).left(8));

    auto task_func = [this, object, property_name, value_provider, binding_id,
                      interface]() mutable {
        try {
            // Create the binding
            PropertyBinding binding;
            binding.object = object;
            binding.property_name = property_name;
            binding.value_provider = [value_provider]() -> QVariant {
                return QVariant::fromValue(value_provider());
            };
            binding.last_update = QDateTime::currentDateTime();
            binding.is_active = true;

            // Store the binding
            {
                std::unique_lock<std::shared_mutex> lock(bindings_mutex_);
                bindings_[binding_id] = binding;
            }

            // Compute initial value
            T initial_value = value_provider();
            QVariant variant_value = QVariant::fromValue(initial_value);

            // Apply initial value on main thread
            bool success = false;
            QMetaObject::invokeMethod(
                this,
                [object, property_name, variant_value, &success]() {
                    success = object->setProperty(
                        property_name.toUtf8().constData(), variant_value);
                },
                Qt::BlockingQueuedConnection);

            if (success) {
                QMetaObject::invokeMethod(
                    this,
                    [this, binding_id, variant_value]() {
                        emit bindingUpdated(binding_id, variant_value);
                        emit propertyBound(binding_id, true);
                    },
                    Qt::QueuedConnection);
            } else {
                QMetaObject::invokeMethod(
                    this,
                    [this, binding_id]() {
                        emit propertyBound(binding_id, false);
                    },
                    Qt::QueuedConnection);
            }

            interface.reportResult(success);
            interface.reportFinished();

        } catch (const std::exception& e) {
            qWarning() << "🔥 Property binding failed:" << e.what();

            QMetaObject::invokeMethod(
                this,
                [this, binding_id]() { emit propertyBound(binding_id, false); },
                Qt::QueuedConnection);

            interface.reportResult(false);
            interface.reportFinished();
        }
    };

    processor_->submitTask("bind_" + binding_id, TaskPriority::Normal,
                           ExecutionContext::ThreadPool, task_func);

    return future;
}

}  // namespace DeclarativeUI::Core
