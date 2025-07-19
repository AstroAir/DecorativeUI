#pragma once

#include <QObject>
#include <QString>
#include <QThread>
#include <QMutex>
#include <QReadWriteLock>
#include <QWaitCondition>
#include <QTimer>
#include <QJsonObject>
#include <QFuture>
#include <QFutureWatcher>
// #include <QtConcurrent>  // Commented out - may not be available in this Qt installation

#include <memory>
#include <vector>
#include <queue>
#include <unordered_map>
#include <atomic>
#include <functional>
#include <future>
#include <thread>
#include <condition_variable>
#include <shared_mutex>
#include <chrono>

namespace DeclarativeUI::Core {

// **Task priority levels**
enum class TaskPriority {
    Low = 0,
    Normal = 1,
    High = 2,
    Critical = 3
};

// **Task execution context**
enum class ExecutionContext {
    MainThread,     // Execute on main UI thread
    WorkerThread,   // Execute on worker thread
    ThreadPool,     // Execute on thread pool
    Background      // Execute on background thread
};

// **Task result wrapper**
template<typename T>
struct TaskResult {
    T result;
    bool success = false;
    QString error_message;
    std::chrono::milliseconds execution_time{0};
    QDateTime completed_at;
    
    bool isValid() const { return success; }
    operator bool() const { return success; }
};

// **Generic task interface**
class ITask {
public:
    virtual ~ITask() = default;
    virtual void execute() = 0;
    virtual TaskPriority getPriority() const = 0;
    virtual ExecutionContext getExecutionContext() const = 0;
    virtual QString getTaskId() const = 0;
    virtual QString getDescription() const = 0;
};

// **Concrete task implementation**
template<typename ResultType>
class Task : public ITask {
public:
    using TaskFunction = std::function<ResultType()>;
    using CompletionCallback = std::function<void(const TaskResult<ResultType>&)>;

    Task(const QString& task_id, TaskFunction func, TaskPriority priority = TaskPriority::Normal,
         ExecutionContext context = ExecutionContext::ThreadPool)
        : task_id_(task_id), function_(std::move(func)), priority_(priority), context_(context) {}

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
        result_.execution_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        result_.completed_at = QDateTime::currentDateTime();
        
        if (completion_callback_) {
            completion_callback_(result_);
        }
    }

    TaskPriority getPriority() const override { return priority_; }
    ExecutionContext getExecutionContext() const override { return context_; }
    QString getTaskId() const override { return task_id_; }
    QString getDescription() const override { return description_; }

    void setCompletionCallback(CompletionCallback callback) {
        completion_callback_ = std::move(callback);
    }

    void setDescription(const QString& description) {
        description_ = description;
    }

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

// **Thread pool with priority queue**
class ThreadPool {
public:
    explicit ThreadPool(size_t thread_count = std::thread::hardware_concurrency());
    ~ThreadPool();

    template<typename F, typename... Args>
    auto enqueue(TaskPriority priority, F&& f, Args&&... args) 
        -> std::future<typename std::result_of<F(Args...)>::type>;

    void shutdown();
    void pause();
    void resume();
    
    size_t active_threads() const { return active_threads_.load(); }
    size_t queued_tasks() const;
    bool is_running() const { return running_.load(); }

private:
    struct TaskWrapper {
        std::function<void()> task;
        TaskPriority priority;
        std::chrono::steady_clock::time_point enqueue_time;
        
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
    
    void worker_thread();
};

// **Parallel task scheduler**
class ParallelProcessor : public QObject {
    Q_OBJECT

public:
    explicit ParallelProcessor(QObject* parent = nullptr);
    ~ParallelProcessor() override;

    // **Task submission**
    template<typename F, typename... Args>
    QString submitTask(const QString& task_id, TaskPriority priority, 
                      ExecutionContext context, F&& func, Args&&... args);

    template<typename F>
    QString submitBackgroundTask(const QString& task_id, F&& func);

    template<typename F>
    QString submitHighPriorityTask(const QString& task_id, F&& func);

    // **Batch processing**
    template<typename Container, typename F>
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
    void taskCompleted(const QString& task_id, bool success);
    void taskFailed(const QString& task_id, const QString& error);
    void batchCompleted(const QString& batch_id, int completed_count, int failed_count);
    void queueOverflow(size_t queue_size);
    void performanceAlert(const QString& metric, double value);

private slots:
    void onTaskTimeout();
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
    QString generateTaskId() const;
    void updatePerformanceMetrics(const QString& task_id, 
                                 std::chrono::milliseconds execution_time, bool success);
    void checkQueueOverflow();
    void balanceLoad();
    void cleanupCompletedTasks();
};

// **Specialized processors for different domains**

// **File processing with parallel I/O**
class ParallelFileProcessor : public QObject {
    Q_OBJECT

public:
    explicit ParallelFileProcessor(QObject* parent = nullptr);

    // **Parallel file operations**
    QFuture<QStringList> readFilesAsync(const QStringList& file_paths);
    QFuture<bool> writeFilesAsync(const QStringList& file_paths, const QStringList& contents);
    QFuture<QStringList> processFilesAsync(const QStringList& file_paths, 
                                          std::function<QString(const QString&)> processor);

    // **Directory operations**
    QFuture<QStringList> scanDirectoryAsync(const QString& directory_path, bool recursive = true);
    QFuture<bool> copyDirectoryAsync(const QString& source, const QString& destination);

signals:
    void fileProcessed(const QString& file_path, bool success);
    void batchProgress(int completed, int total);

private:
    std::unique_ptr<ParallelProcessor> processor_;
};

// **UI compilation with parallel processing**
class ParallelUICompiler : public QObject {
    Q_OBJECT

public:
    explicit ParallelUICompiler(QObject* parent = nullptr);

    // **Parallel UI compilation**
    QFuture<QJsonObject> compileUIAsync(const QString& ui_file_path);
    QFuture<QStringList> compileUIBatchAsync(const QStringList& ui_file_paths);
    QFuture<bool> validateUIAsync(const QString& ui_file_path);

    // **Dependency resolution**
    QFuture<QStringList> resolveDependenciesAsync(const QString& ui_file_path);
    QFuture<bool> checkDependencyChangesAsync(const QStringList& dependency_paths);

signals:
    void compilationCompleted(const QString& file_path, bool success);
    void dependencyResolved(const QString& file_path, const QStringList& dependencies);

private:
    std::unique_ptr<ParallelProcessor> processor_;
    std::unordered_map<QString, QStringList> dependency_cache_;
    mutable std::shared_mutex cache_mutex_;
};

// **Property binding with background updates**
class ParallelPropertyBinder : public QObject {
    Q_OBJECT

public:
    explicit ParallelPropertyBinder(QObject* parent = nullptr);

    // **Async property binding**
    template<typename T>
    QFuture<bool> bindPropertyAsync(QObject* object, const QString& property_name, 
                                   std::function<T()> value_provider);

    QFuture<bool> updateBindingsAsync(const QStringList& binding_ids);
    QFuture<bool> validateBindingsAsync();

    // **Batch property updates**
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

}  // namespace DeclarativeUI::Core
