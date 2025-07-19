#pragma once

#include <QObject>
#include <QString>
#include <QTimer>
#include <QMutex>
#include <QReadWriteLock>
#include <QJsonObject>

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <queue>
#include <atomic>
#include <chrono>
#include <shared_mutex>
#include <functional>
#include <thread>
#include <type_traits>

namespace DeclarativeUI::Core {

// **Memory pool for efficient allocation**
template<typename T, size_t PoolSize = 1000>
class ObjectPool {
public:
    ObjectPool() {
        // Pre-allocate objects
        for (size_t i = 0; i < PoolSize; ++i) {
            available_objects_.push(std::make_unique<T>());
        }
    }

    template<typename... Args>
    std::unique_ptr<T> acquire(Args&&... args) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (!available_objects_.empty()) {
            auto obj = std::move(available_objects_.front());
            available_objects_.pop();
            
            // Reset/reinitialize the object
            if constexpr (std::is_constructible_v<T, Args...>) {
                *obj = T(std::forward<Args>(args)...);
            }
            
            allocated_count_.fetch_add(1);
            return obj;
        }
        
        // Pool exhausted, create new object
        pool_misses_.fetch_add(1);
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    void release(std::unique_ptr<T> obj) {
        if (!obj) return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (available_objects_.size() < PoolSize) {
            available_objects_.push(std::move(obj));
            allocated_count_.fetch_sub(1);
        }
        // If pool is full, let the object be destroyed
    }

    size_t available_count() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return available_objects_.size();
    }

    size_t allocated_count() const {
        return allocated_count_.load();
    }

    size_t pool_misses() const {
        return pool_misses_.load();
    }

private:
    mutable std::mutex mutex_;
    std::queue<std::unique_ptr<T>> available_objects_;
    std::atomic<size_t> allocated_count_{0};
    std::atomic<size_t> pool_misses_{0};
};

// **Memory arena allocator for contiguous allocation**
class MemoryArena {
public:
    explicit MemoryArena(size_t size_bytes);
    ~MemoryArena();

    void* allocate(size_t size, size_t alignment = alignof(std::max_align_t));
    void reset();
    
    size_t used_bytes() const { return used_bytes_.load(); }
    size_t total_bytes() const { return total_bytes_; }
    double usage_percentage() const {
        return static_cast<double>(used_bytes_.load()) / total_bytes_ * 100.0;
    }

private:
    char* memory_;
    size_t total_bytes_;
    std::atomic<size_t> used_bytes_{0};
    std::mutex allocation_mutex_;
};

// **Smart pointer with custom deleter for pooled objects**
template<typename T>
class PooledPtr {
public:
    PooledPtr(std::unique_ptr<T> ptr, ObjectPool<T>* pool)
        : ptr_(std::move(ptr)), pool_(pool) {}
    
    ~PooledPtr() {
        if (ptr_ && pool_) {
            pool_->release(std::move(ptr_));
        }
    }

    // Move semantics
    PooledPtr(PooledPtr&& other) noexcept
        : ptr_(std::move(other.ptr_)), pool_(other.pool_) {
        other.pool_ = nullptr;
    }

    PooledPtr& operator=(PooledPtr&& other) noexcept {
        if (this != &other) {
            if (ptr_ && pool_) {
                pool_->release(std::move(ptr_));
            }
            ptr_ = std::move(other.ptr_);
            pool_ = other.pool_;
            other.pool_ = nullptr;
        }
        return *this;
    }

    // Disable copy
    PooledPtr(const PooledPtr&) = delete;
    PooledPtr& operator=(const PooledPtr&) = delete;

    T* get() const { return ptr_.get(); }
    T& operator*() const { return *ptr_; }
    T* operator->() const { return ptr_.get(); }
    explicit operator bool() const { return static_cast<bool>(ptr_); }

private:
    std::unique_ptr<T> ptr_;
    ObjectPool<T>* pool_;
};

// **Memory leak detector**
class MemoryLeakDetector {
public:
    static MemoryLeakDetector& instance();

    void track_allocation(void* ptr, size_t size, const char* file, int line);
    void track_deallocation(void* ptr);
    
    struct AllocationInfo {
        size_t size;
        std::string file;
        int line;
        std::chrono::steady_clock::time_point timestamp;
    };

    std::vector<AllocationInfo> get_leaks() const;
    size_t get_total_allocated() const;
    size_t get_allocation_count() const;
    void clear_tracking();

private:
    mutable std::shared_mutex mutex_;
    std::unordered_map<void*, AllocationInfo> allocations_;
    std::atomic<size_t> total_allocated_{0};
    std::atomic<size_t> allocation_count_{0};
};

// **Memory statistics**
struct MemoryStatistics {
    size_t total_allocated_bytes = 0;
    size_t peak_allocated_bytes = 0;
    size_t current_allocated_bytes = 0;
    size_t allocation_count = 0;
    size_t deallocation_count = 0;
    size_t pool_hits = 0;
    size_t pool_misses = 0;
    double fragmentation_ratio = 0.0;
    std::chrono::steady_clock::time_point last_gc_time;
    size_t gc_count = 0;
};

// **Garbage collection strategies**
enum class GCStrategy {
    Manual,         // Manual garbage collection
    Automatic,      // Automatic based on memory pressure
    Generational,   // Generational garbage collection
    Incremental     // Incremental garbage collection
};

// **Main memory manager**
class MemoryManager : public QObject {
    Q_OBJECT

public:
    static MemoryManager& instance();
    explicit MemoryManager(QObject* parent = nullptr);
    ~MemoryManager() override;

    // **Object pool management**
    template<typename T>
    ObjectPool<T>& get_pool();
    
    template<typename T, typename... Args>
    PooledPtr<T> create_pooled(Args&&... args);

    // **Memory arena management**
    MemoryArena* create_arena(const QString& name, size_t size_bytes);
    MemoryArena* get_arena(const QString& name);
    void destroy_arena(const QString& name);

    // **Garbage collection**
    void set_gc_strategy(GCStrategy strategy);
    void trigger_gc();
    void enable_auto_gc(bool enabled);
    void set_gc_threshold(size_t threshold_bytes);

    // **Memory monitoring**
    MemoryStatistics get_statistics() const;
    QJsonObject get_memory_report() const;
    void enable_leak_detection(bool enabled);
    std::vector<MemoryLeakDetector::AllocationInfo> get_memory_leaks() const;

    // **Memory optimization**
    void optimize_memory_usage();
    void compact_memory();
    void clear_unused_pools();
    void defragment_arenas();

    // **Configuration**
    void set_memory_limit(size_t limit_bytes);
    void set_warning_threshold(size_t threshold_bytes);
    void enable_memory_pressure_monitoring(bool enabled);

signals:
    void memory_pressure_detected(size_t current_usage, size_t limit);
    void memory_leak_detected(size_t leak_count);
    void garbage_collection_completed(size_t freed_bytes);
    void memory_limit_exceeded(size_t current_usage);

private slots:
    void on_memory_check_timer();
    void on_gc_timer();
    void on_optimization_timer();

private:
    // **Object pools for different types**
    std::unordered_map<std::string, void*> object_pools_;
    mutable std::shared_mutex pools_mutex_;

    // **Memory arenas**
    std::unordered_map<QString, std::unique_ptr<MemoryArena>> memory_arenas_;
    mutable std::shared_mutex arenas_mutex_;

    // **Configuration**
    std::atomic<size_t> memory_limit_bytes_{1024 * 1024 * 1024}; // 1GB default
    std::atomic<size_t> warning_threshold_bytes_{800 * 1024 * 1024}; // 800MB
    std::atomic<size_t> gc_threshold_bytes_{100 * 1024 * 1024}; // 100MB
    std::atomic<bool> auto_gc_enabled_{true};
    std::atomic<bool> leak_detection_enabled_{false};
    std::atomic<bool> memory_pressure_monitoring_enabled_{true};
    GCStrategy gc_strategy_ = GCStrategy::Automatic;

    // **Statistics**
    mutable MemoryStatistics statistics_;
    mutable std::shared_mutex statistics_mutex_;

    // **Timers**
    std::unique_ptr<QTimer> memory_check_timer_;
    std::unique_ptr<QTimer> gc_timer_;
    std::unique_ptr<QTimer> optimization_timer_;

    // **Internal methods**
    void check_memory_pressure();
    void perform_garbage_collection();
    void update_statistics();
    size_t calculate_current_memory_usage() const;
    void cleanup_expired_objects();
    
    template<typename T>
    std::string get_type_name() const;
};

// **Memory allocation macros for tracking**
#ifdef ENABLE_MEMORY_TRACKING
#define TRACKED_NEW(type, ...) \
    DeclarativeUI::Core::MemoryManager::instance().create_pooled<type>(__VA_ARGS__)

#define TRACKED_DELETE(ptr) \
    /* Automatic cleanup with PooledPtr */
#else
#define TRACKED_NEW(type, ...) std::make_unique<type>(__VA_ARGS__)
#define TRACKED_DELETE(ptr) ptr.reset()
#endif

}  // namespace DeclarativeUI::Core
