#pragma once

#include <QJsonObject>
#include <QMutex>
#include <QObject>
#include <QReadWriteLock>
#include <QString>
#include <QTimer>

#include <atomic>
#include <chrono>
#include <memory>
#include <queue>
#include <shared_mutex>
#include <type_traits>
#include <unordered_map>
#include <vector>

/**
 * @file MemoryManager.hpp
 * @brief Memory management utilities: pools, arenas, leak detection and
 * manager.
 *
 * This header provides a collection of memory utilities intended to reduce
 * allocation overhead and assist debugging of memory usage in DeclarativeUI:
 *  - ObjectPool<T> : a simple, thread-safe fixed-size pool for frequently
 *    allocated objects of type T.
 *  - MemoryArena    : a contiguous arena allocator for short-lived or tightly
 *    packed allocations.
 *  - PooledPtr<T>   : RAII wrapper that returns objects to an ObjectPool on
 *    destruction.
 *  - MemoryLeakDetector: lightweight tracking of allocations/deallocations to
 *    assist in leak discovery during development and testing.
 *  - MemoryStatistics, GCStrategy and MemoryManager: a higher-level manager
 *    exposing pools, arenas, GC controls, monitoring and reporting interfaces.
 *
 * Design notes:
 *  - Pool and arena implementations are intentionally conservative and simple;
 *    they prioritize correctness and debuggability over advanced allocation
 *    heuristics.
 *  - Thread-safety: public API surfaces use mutexes or atomics for safe access
 *    across threads. Consult individual class comments for details about
 *    locking semantics and contention expectations.
 *  - Production vs. debug: leak tracking and heavy instrumentation are guarded
 *    by runtime flags or preprocessor macros to avoid overhead in release
 *    builds.
 *
 * Usage examples:
 *  - Acquire an object from a pool:
 *      auto& pool = MemoryManager::instance().get_pool<MyObject>();
 *      auto obj = pool.acquire(constructor_args...);
 *
 *  - Create a pooled RAII handle:
 *      auto handle =
 * MemoryManager::instance().create_pooled<MyObject>(args...);
 *
 *  - Create a named arena and allocate from it:
 *      auto* arena = MemoryManager::instance().create_arena("ui-temp",
 * 16*1024); void* mem = arena->allocate(64);
 *
 * Threading:
 *  - MemoryManager itself uses internal synchronization for its maps and
 *    statistics, but high-frequency callers should cache references to pools
 *    or arenas to minimize contention.
 */

namespace DeclarativeUI::Core {

// -----------------------------------------------------------------------------
// ObjectPool
// -----------------------------------------------------------------------------
/**
 * @brief Fixed-size object pool for efficient reuse of T instances.
 *
 * Template parameters:
 *  - T : type of pooled objects.
 *  - PoolSize : preallocated capacity kept available for reuse.
 *
 * Characteristics:
 *  - Pre-allocates PoolSize objects during construction and stores them in a
 *    queue of unique_ptr<T>.
 *  - acquire() returns a unique_ptr<T> from the pool if available, otherwise
 *    constructs a fresh object (counts as a pool miss).
 *  - release() returns an object to the pool if capacity permits; otherwise
 *    the object is destroyed.
 *  - Thread-safety: all public methods use an internal std::mutex to protect
 *    the queue. Atomics are used to track counters without additional locking.
 *
 * Performance guidance:
 *  - Use pools for high-frequency, short-lived allocations where construction
 *    cost dominates allocation overhead.
 *  - For types that are expensive to reinitialize consider calling a custom
 *    reset method instead of assigning a new instance.
 */
template <typename T, size_t PoolSize = 1000>
class ObjectPool {
public:
    ObjectPool() {
        // Pre-allocate objects
        for (size_t i = 0; i < PoolSize; ++i) {
            available_objects_.push(std::make_unique<T>());
        }
    }

    /**
     * @brief Acquire an object from the pool or create a new one.
     *
     * If the pool is not empty the front object is returned after optional
     * reinitialization. If the pool is exhausted a new instance is constructed
     * and pool_misses_ is incremented.
     *
     * @tparam Args constructor argument types forwarded to T.
     * @param args forwarded arguments.
     * @return std::unique_ptr<T> owning the acquired instance.
     */
    template <typename... Args>
    std::unique_ptr<T> acquire(Args&&... args) {
        std::lock_guard<std::mutex> lock(mutex_);

        if (!available_objects_.empty()) {
            auto obj = std::move(available_objects_.front());
            available_objects_.pop();

            // Reset/reinitialize the object by assignment if constructible.
            if constexpr (std::is_constructible_v<T, Args...>) {
                *obj = T(std::forward<Args>(args)...);
            }

            allocated_count_.fetch_add(1);
            return obj;
        }

        // Pool exhausted, create new object (pool miss)
        pool_misses_.fetch_add(1);
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    /**
     * @brief Release an object back to the pool.
     *
     * If the pool has capacity the object is pushed back into
     * available_objects_, otherwise it is destroyed when the unique_ptr goes
     * out of scope.
     *
     * @param obj unique_ptr<T> to return.
     */
    void release(std::unique_ptr<T> obj) {
        if (!obj)
            return;

        std::lock_guard<std::mutex> lock(mutex_);

        if (available_objects_.size() < PoolSize) {
            available_objects_.push(std::move(obj));
            allocated_count_.fetch_sub(1);
        }
        // If pool is full, let the object be destroyed
    }

    /**
     * @brief Number of objects currently available for acquisition.
     * @return size_t count of available objects.
     */
    size_t available_count() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return available_objects_.size();
    }

    /**
     * @brief Number of objects currently considered allocated (approximate).
     * @note This counter is updated atomically alongside acquire/release.
     */
    size_t allocated_count() const { return allocated_count_.load(); }

    /**
     * @brief Number of times acquire() had to construct an object because the
     * pool was empty.
     */
    size_t pool_misses() const { return pool_misses_.load(); }

private:
    mutable std::mutex mutex_;
    std::queue<std::unique_ptr<T>> available_objects_;
    std::atomic<size_t> allocated_count_{0};
    std::atomic<size_t> pool_misses_{0};
};

// -----------------------------------------------------------------------------
// MemoryArena
// -----------------------------------------------------------------------------
/**
 * @brief Simple contiguous memory arena allocator.
 *
 * The arena provides linear allocation from a contiguous block and supports a
 * reset() operation that reclaims all allocations at once. This is useful for
 * transient allocations with similar lifetimes (per-frame, per-operation).
 *
 * Characteristics:
 *  - allocate(size, alignment) returns a pointer aligned to `alignment` or
 *    nullptr if insufficient space remains.
 *  - reset() sets used_bytes_ back to zero, effectively freeing all
 *    allocations without individually destructing objects (clients must
 *    manage object lifetimes appropriately).
 *  - Thread-safety: allocation is guarded by allocation_mutex_. Concurrent
 *    allocations will be serialized.
 *
 * Usage:
 *  - Reserve a MemoryArena for many short-lived small allocations that can be
 *    thrown away together by reset() to avoid fragmentation and overhead.
 */
class MemoryArena {
public:
    /**
     * @brief Construct an arena with the specified total size in bytes.
     * @param size_bytes total bytes to reserve for the arena.
     */
    explicit MemoryArena(size_t size_bytes);
    ~MemoryArena();

    /**
     * @brief Allocate `size` bytes from the arena with `alignment`.
     * @param size number of bytes requested.
     * @param alignment desired alignment (default: max alignment).
     * @return pointer to allocated memory or nullptr on failure.
     */
    void* allocate(size_t size, size_t alignment = alignof(std::max_align_t));
    /**
     * @brief Reset the arena, reclaiming all previous allocations.
     *
     * Note: destructors for objects placed in the arena are not called by
     * reset(); callers must ensure proper destruction if needed.
     */
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

// -----------------------------------------------------------------------------
// PooledPtr
// -----------------------------------------------------------------------------
/**
 * @brief RAII smart wrapper returning objects to an ObjectPool on destruction.
 *
 * PooledPtr owns a unique_ptr<T> and a pointer to the ObjectPool<T> it should
 * be returned to. When the PooledPtr is destroyed (or moved-from) the held
 * object is released back into the pool, enabling reuse.
 *
 * Semantics:
 *  - Move-only: copy operations are disabled to prevent double-return.
 *  - On destruction or move-assignment, if a held pointer and pool exist the
 *    object is returned via pool->release().
 *  - Use PooledPtr when you want automatic return-to-pool behavior.
 */
template <typename T>
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

// -----------------------------------------------------------------------------
// MemoryLeakDetector
// -----------------------------------------------------------------------------
/**
 * @brief Development-time allocation tracker to assist in leak detection.
 *
 * MemoryLeakDetector provides a process-wide registry of tracked allocations
 * (pointer -> AllocationInfo) and exposes APIs to add/remove entries and to
 * query leaks. It is intended for diagnostic builds and may be enabled via
 * runtime configuration.
 *
 * Notes:
 *  - The detector does not integrate with global new/delete by default; users
 *    must instrument allocations they wish to track. Integration macros are
 *    provided in this file to simplify instrumentation.
 *  - Thread-safety: uses a shared_mutex to allow concurrent queries while
 *    serializing modifications.
 */
class MemoryLeakDetector {
public:
    static MemoryLeakDetector& instance();

    /**
     * @brief Register an allocation.
     * @param ptr pointer returned by the allocation.
     * @param size allocation size in bytes.
     * @param file source file of allocation (for diagnostics).
     * @param line source line number of allocation.
     */
    void track_allocation(void* ptr, size_t size, const char* file, int line);
    /**
     * @brief Register deallocation of a previously tracked pointer.
     * @param ptr pointer being deallocated.
     */
    void track_deallocation(void* ptr);

    struct AllocationInfo {
        size_t size;
        std::string file;
        int line;
        std::chrono::steady_clock::time_point timestamp;
    };

    /**
     * @brief Return a list of currently outstanding allocations (potential
     * leaks).
     * @return vector of AllocationInfo for tracked pointers that have not been
     * freed.
     */
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

// -----------------------------------------------------------------------------
// MemoryStatistics
// -----------------------------------------------------------------------------
/**
 * @brief Snapshot of memory manager statistics.
 *
 * Collects counters and metrics useful for telemetry and diagnostics. Fields
 * are updated by MemoryManager::update_statistics() and exposed via
 * get_statistics().
 */
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

// -----------------------------------------------------------------------------
// GCStrategy
// -----------------------------------------------------------------------------
/**
 * @brief Garbage collection strategies supported by MemoryManager.
 *
 * - Manual: only explicit trigger_gc() calls perform collection.
 * - Automatic: manager may run GC based on thresholds or timers.
 * - Generational: separate short- and long-lived object treatment
 * (placeholder).
 * - Incremental: perform GC in small steps to reduce pause times (placeholder).
 *
 * Note: Some strategies are placeholders for future improvements; current
 * implementation focuses on Manual and Automatic modes.
 */
enum class GCStrategy {
    Manual,        // Manual garbage collection
    Automatic,     // Automatic based on memory pressure
    Generational,  // Generational garbage collection
    Incremental    // Incremental garbage collection
};

// -----------------------------------------------------------------------------
// MemoryManager
// -----------------------------------------------------------------------------
/**
 * @brief Central memory manager responsible for pools, arenas and monitoring.
 *
 * MemoryManager is a QObject so it can integrate with Qt timers and emit
 * signals when memory events occur (pressure, leaks, GC completion, etc).
 *
 * Responsibilities:
 *  - Maintain typed object pools (get_pool<T>), returning references that may
 *    be cached by callers.
 *  - Provide create_pooled<T>(...) convenience to obtain a PooledPtr<T>.
 *  - Create, lookup and destroy MemoryArena instances identified by name.
 *  - Configure GC strategy, thresholds and enable/disable automatic GC.
 *  - Expose memory statistics and JSON reports for telemetry or UI diagnostics.
 *  - Support optional leak detection and a set of optimization triggers.
 *
 * Thread-safety:
 *  - The manager uses shared_mutexes to protect the pools and arenas maps. Use
 *    lightweight operations (cache pool/arena references) in hot paths to
 *    reduce contention.
 *
 * Example:
 *  auto& mm = MemoryManager::instance();
 *  auto pooled = mm.create_pooled<MyType>(args...);
 */
class MemoryManager : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Access the singleton MemoryManager instance.
     * @return reference to the global MemoryManager.
     */
    static MemoryManager& instance();
    explicit MemoryManager(QObject* parent = nullptr);
    ~MemoryManager() override;

    // ---------------------------
    // Object pool management
    // ---------------------------
    /**
     * @brief Get or create an ObjectPool<T> associated with the manager.
     *
     * The returned reference remains valid until the pool is explicitly
     * destroyed or the manager is torn down. Internally pools are stored as
     * type-erased pointers in a map keyed by type name.
     *
     * @tparam T pooled type.
     * @return reference to ObjectPool<T>.
     */
    template <typename T>
    ObjectPool<T>& get_pool();

    /**
     * @brief Convenience: create a pooled RAII handle (PooledPtr<T>).
     *
     * Allocates (or reuses) a T instance from the pool and returns a PooledPtr
     * that will return the object to the pool when it goes out of scope.
     *
     * @tparam T pooled type.
     * @tparam Args constructor arg types forwarded to T.
     * @return PooledPtr<T> owning the instance.
     */
    template <typename T, typename... Args>
    PooledPtr<T> create_pooled(Args&&... args);

    // ---------------------------
    // Memory arena management
    // ---------------------------
    /**
     * @brief Create a named arena of the requested size.
     * @param name logical name used to identify the arena.
     * @param size_bytes requested size in bytes.
     * @return pointer to the created MemoryArena or existing one if name
     * exists.
     */
    MemoryArena* create_arena(const QString& name, size_t size_bytes);
    MemoryArena* get_arena(const QString& name);
    void destroy_arena(const QString& name);

    // ---------------------------
    // Garbage collection control
    // ---------------------------
    void set_gc_strategy(GCStrategy strategy);
    void trigger_gc();
    void enable_auto_gc(bool enabled);
    void set_gc_threshold(size_t threshold_bytes);

    // ---------------------------
    // Monitoring and diagnostics
    // ---------------------------
    MemoryStatistics get_statistics() const;
    QJsonObject get_memory_report() const;
    void enable_leak_detection(bool enabled);
    std::vector<MemoryLeakDetector::AllocationInfo> get_memory_leaks() const;

    // ---------------------------
    // Optimization helpers
    // ---------------------------
    void optimize_memory_usage();
    void compact_memory();
    void clear_unused_pools();
    void defragment_arenas();

    // ---------------------------
    // Configuration
    // ---------------------------
    void set_memory_limit(size_t limit_bytes);
    void set_warning_threshold(size_t threshold_bytes);
    void enable_memory_pressure_monitoring(bool enabled);

signals:
    /**
     * @brief Emitted when memory pressure is detected (usage near configured
     * limit).
     * @param current_usage current bytes used by tracked subsystems.
     * @param limit configured memory limit in bytes.
     */
    void memory_pressure_detected(size_t current_usage, size_t limit);
    /**
     * @brief Emitted when potential memory leaks are discovered.
     * @param leak_count number of outstanding tracked allocations.
     */
    void memory_leak_detected(size_t leak_count);
    /**
     * @brief Emitted when a garbage collection run completes.
     * @param freed_bytes number of bytes reclaimed during the collection.
     */
    void garbage_collection_completed(size_t freed_bytes);
    /**
     * @brief Emitted when the memory usage exceeds the configured limit.
     * @param current_usage current usage in bytes.
     */
    void memory_limit_exceeded(size_t current_usage);

private slots:
    // Internal timers call-backs used by automatic monitoring/GC.
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
    std::atomic<size_t> memory_limit_bytes_{1024 * 1024 * 1024};  // 1GB default
    std::atomic<size_t> warning_threshold_bytes_{800 * 1024 * 1024};  // 800MB
    std::atomic<size_t> gc_threshold_bytes_{100 * 1024 * 1024};       // 100MB
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

    template <typename T>
    std::string get_type_name() const;
};

// -----------------------------------------------------------------------------
// Allocation macros
// -----------------------------------------------------------------------------
/**
 * @brief Convenience macros for tracked allocations.
 *
 * - When ENABLE_MEMORY_TRACKING is defined TRACKED_NEW forwards to the
 *   MemoryManager::create_pooled() helper which integrates with the leak
 *   detector and pool subsystem.
 * - TRACKED_DELETE is a no-op for pooled objects (PooledPtr destructor handles
 *   returning to pool). For non-pooled usage TRACKED_DELETE resets the pointer.
 *
 * These macros are easy to grep for and can be selectively enabled in debug
 * builds to assist tracking without widespread code changes.
 */
#ifdef ENABLE_MEMORY_TRACKING
#define TRACKED_NEW(type, ...)                                          \
    DeclarativeUI::Core::MemoryManager::instance().create_pooled<type>( \
        __VA_ARGS__)

#define TRACKED_DELETE(ptr) /* Automatic cleanup with PooledPtr */
#else
#define TRACKED_NEW(type, ...) std::make_unique<type>(__VA_ARGS__)
#define TRACKED_DELETE(ptr) ptr.reset()
#endif

}  // namespace DeclarativeUI::Core
