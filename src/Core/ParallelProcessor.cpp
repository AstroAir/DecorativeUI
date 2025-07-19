#include "ParallelProcessor.hpp"

#include <QDebug>
#include <QJsonArray>
#include <QUuid>
#include <QCoreApplication>
#include <algorithm>

namespace DeclarativeUI::Core {

// **ThreadPool implementation**
ThreadPool::ThreadPool(size_t thread_count) {
    workers_.reserve(thread_count);
    
    for (size_t i = 0; i < thread_count; ++i) {
        workers_.emplace_back(&ThreadPool::worker_thread, this);
    }
    
    qDebug() << "🔥 ThreadPool initialized with" << thread_count << "threads";
}

ThreadPool::~ThreadPool() {
    shutdown();
}

template<typename F, typename... Args>
auto ThreadPool::enqueue(TaskPriority priority, F&& f, Args&&... args) 
    -> std::future<typename std::result_of<F(Args...)>::type> {
    
    using return_type = typename std::result_of<F(Args...)>::type;
    
    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );
    
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
                return !running_.load() || (!task_queue_.empty() && !paused_.load());
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
    connect(timeout_timer_.get(), &QTimer::timeout, this, &ParallelProcessor::onTaskTimeout);
    timeout_timer_->start();
    
    // Setup performance monitoring timer
    performance_timer_ = std::make_unique<QTimer>(this);
    performance_timer_->setInterval(10000);  // Check every 10 seconds
    connect(performance_timer_.get(), &QTimer::timeout, this, &ParallelProcessor::onPerformanceCheck);
    performance_timer_->start();
    
    qDebug() << "🔥 ParallelProcessor initialized";
}

ParallelProcessor::~ParallelProcessor() {
    if (thread_pool_) {
        thread_pool_->shutdown();
    }
    qDebug() << "🔥 ParallelProcessor destroyed";
}

template<typename F, typename... Args>
QString ParallelProcessor::submitTask(const QString& task_id, TaskPriority priority, 
                                     ExecutionContext context, F&& func, Args&&... args) {
    QString actual_task_id = task_id.isEmpty() ? generateTaskId() : task_id;
    
    // Check queue overflow
    checkQueueOverflow();
    
    auto task_func = std::bind(std::forward<F>(func), std::forward<Args>(args)...);
    
    switch (context) {
        case ExecutionContext::MainThread: {
            // Execute on main thread using Qt's event system
            QMetaObject::invokeMethod(this, [this, actual_task_id, task_func]() {
                auto start_time = std::chrono::steady_clock::now();
                bool success = true;
                
                try {
                    task_func();
                } catch (const std::exception& e) {
                    success = false;
                    emit taskFailed(actual_task_id, QString::fromStdString(e.what()));
                } catch (...) {
                    success = false;
                    emit taskFailed(actual_task_id, "Unknown error");
                }
                
                auto end_time = std::chrono::steady_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
                
                updatePerformanceMetrics(actual_task_id, duration, success);
                
                if (success) {
                    emit taskCompleted(actual_task_id, true);
                }
            }, Qt::QueuedConnection);
            break;
        }
        
        case ExecutionContext::ThreadPool:
        case ExecutionContext::WorkerThread:
        case ExecutionContext::Background: {
            // Execute on thread pool
            auto future = thread_pool_->enqueue(priority, [this, actual_task_id, task_func]() {
                auto start_time = std::chrono::steady_clock::now();
                bool success = true;
                
                try {
                    task_func();
                } catch (const std::exception& e) {
                    success = false;
                    QMetaObject::invokeMethod(this, [this, actual_task_id, e]() {
                        emit taskFailed(actual_task_id, QString::fromStdString(e.what()));
                    }, Qt::QueuedConnection);
                } catch (...) {
                    success = false;
                    QMetaObject::invokeMethod(this, [this, actual_task_id]() {
                        emit taskFailed(actual_task_id, "Unknown error");
                    }, Qt::QueuedConnection);
                }
                
                auto end_time = std::chrono::steady_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
                
                updatePerformanceMetrics(actual_task_id, duration, success);
                
                if (success) {
                    QMetaObject::invokeMethod(this, [this, actual_task_id]() {
                        emit taskCompleted(actual_task_id, true);
                    }, Qt::QueuedConnection);
                }
            });
            break;
        }
    }
    
    return actual_task_id;
}

template<typename F>
QString ParallelProcessor::submitBackgroundTask(const QString& task_id, F&& func) {
    return submitTask(task_id, TaskPriority::Low, ExecutionContext::Background, std::forward<F>(func));
}

template<typename F>
QString ParallelProcessor::submitHighPriorityTask(const QString& task_id, F&& func) {
    return submitTask(task_id, TaskPriority::High, ExecutionContext::ThreadPool, std::forward<F>(func));
}

template<typename Container, typename F>
std::vector<QString> ParallelProcessor::submitBatchTasks(const QString& batch_id, 
                                                        const Container& items, F&& func) {
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
        
        auto task_func = [func, item]() {
            return func(item);
        };
        
        submitTask(task_id, TaskPriority::Normal, ExecutionContext::ThreadPool, task_func);
        
        task_ids.push_back(task_id);
        
        {
            std::unique_lock<std::shared_mutex> lock(tasks_mutex_);
            batch_tasks_[batch_id].push_back(task_id);
        }
    }
    
    return task_ids;
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
    
    metrics["total_tasks_executed"] = static_cast<qint64>(total_tasks_executed_.load());
    metrics["total_tasks_failed"] = static_cast<qint64>(total_tasks_failed_.load());
    metrics["active_task_count"] = static_cast<qint64>(getActiveTaskCount());
    metrics["queued_task_count"] = static_cast<qint64>(getQueuedTaskCount());
    metrics["average_execution_time"] = getAverageExecutionTime();
    metrics["peak_queue_size"] = static_cast<qint64>(peak_queue_size_.load());
    
    if (thread_pool_) {
        metrics["active_threads"] = static_cast<qint64>(thread_pool_->active_threads());
        metrics["thread_pool_running"] = thread_pool_->is_running();
    }
    
    double success_rate = total_tasks_executed_.load() > 0 ? 
        (1.0 - static_cast<double>(total_tasks_failed_.load()) / total_tasks_executed_.load()) * 100.0 : 100.0;
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

void ParallelProcessor::updatePerformanceMetrics(const QString& task_id, 
                                                std::chrono::milliseconds execution_time, bool success) {
    total_tasks_executed_.fetch_add(1);
    
    if (!success) {
        total_tasks_failed_.fetch_add(1);
    }
    
    total_execution_time_.fetch_add(execution_time.count());
    
    // Update peak queue size
    size_t current_queue_size = getQueuedTaskCount();
    size_t current_peak = peak_queue_size_.load();
    while (current_queue_size > current_peak && 
           !peak_queue_size_.compare_exchange_weak(current_peak, current_queue_size)) {
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
    if (!load_balancing_enabled_.load()) return;
    
    // Simple load balancing: adjust thread pool size based on queue size
    size_t queue_size = getQueuedTaskCount();
    size_t active_threads = thread_pool_ ? thread_pool_->active_threads() : 0;
    
    if (queue_size > active_threads * 2 && active_threads < std::thread::hardware_concurrency()) {
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

// **Template instantiations for common types**
template QString ParallelProcessor::submitTask<std::function<void()>>(
    const QString&, TaskPriority, ExecutionContext, std::function<void()>&&);

template QString ParallelProcessor::submitBackgroundTask<std::function<void()>>(
    const QString&, std::function<void()>&&);

template QString ParallelProcessor::submitHighPriorityTask<std::function<void()>>(
    const QString&, std::function<void()>&&);

// **Missing class constructors**
ParallelFileProcessor::ParallelFileProcessor(QObject* parent) 
    : QObject(parent) {
    qDebug() << "ParallelFileProcessor created";
}

ParallelUICompiler::ParallelUICompiler(QObject* parent) 
    : QObject(parent) {
    qDebug() << "ParallelUICompiler created";
}

ParallelPropertyBinder::ParallelPropertyBinder(QObject* parent) 
    : QObject(parent) {
    qDebug() << "ParallelPropertyBinder created";
}

}  // namespace DeclarativeUI::Core

#include "ParallelProcessor.moc"
