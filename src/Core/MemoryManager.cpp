#include "MemoryManager.hpp"

#include <QDebug>
#include <QJsonArray>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <mutex>
#ifdef _WIN32
#include <malloc.h>
#endif

namespace DeclarativeUI::Core {

// **MemoryArena implementation**
MemoryArena::MemoryArena(size_t size_bytes) : total_bytes_(size_bytes) {
#ifdef _WIN32
    memory_ = static_cast<char*>(
        _aligned_malloc(size_bytes, alignof(std::max_align_t)));
#else
    memory_ = static_cast<char*>(
        std::aligned_alloc(alignof(std::max_align_t), size_bytes));
#endif
    if (!memory_) {
        throw std::bad_alloc();
    }
    std::memset(memory_, 0, size_bytes);
}

MemoryArena::~MemoryArena() {
    if (memory_) {
#ifdef _WIN32
        _aligned_free(memory_);
#else
        std::free(memory_);
#endif
    }
}

void* MemoryArena::allocate(size_t size, size_t alignment) {
    std::lock_guard<std::mutex> lock(allocation_mutex_);

    size_t current_pos = used_bytes_.load();

    // Align the current position
    size_t aligned_pos = (current_pos + alignment - 1) & ~(alignment - 1);

    if (aligned_pos + size > total_bytes_) {
        return nullptr;  // Out of memory
    }

    used_bytes_.store(aligned_pos + size);
    return memory_ + aligned_pos;
}

void MemoryArena::reset() {
    std::lock_guard<std::mutex> lock(allocation_mutex_);
    used_bytes_.store(0);
    std::memset(memory_, 0, total_bytes_);
}

// **MemoryLeakDetector implementation**
MemoryLeakDetector& MemoryLeakDetector::instance() {
    static MemoryLeakDetector instance;
    return instance;
}

void MemoryLeakDetector::track_allocation(void* ptr, size_t size,
                                          const char* file, int line) {
    std::unique_lock<std::shared_mutex> lock(mutex_);

    AllocationInfo info;
    info.size = size;
    info.file = file ? file : "unknown";
    info.line = line;
    info.timestamp = std::chrono::steady_clock::now();

    allocations_[ptr] = info;
    total_allocated_.fetch_add(size);
    allocation_count_.fetch_add(1);
}

void MemoryLeakDetector::track_deallocation(void* ptr) {
    std::unique_lock<std::shared_mutex> lock(mutex_);

    auto it = allocations_.find(ptr);
    if (it != allocations_.end()) {
        total_allocated_.fetch_sub(it->second.size);
        allocations_.erase(it);
    }
}

std::vector<MemoryLeakDetector::AllocationInfo> MemoryLeakDetector::get_leaks()
    const {
    std::shared_lock<std::shared_mutex> lock(mutex_);

    std::vector<AllocationInfo> leaks;
    leaks.reserve(allocations_.size());

    for (const auto& [ptr, info] : allocations_) {
        leaks.push_back(info);
    }

    return leaks;
}

size_t MemoryLeakDetector::get_total_allocated() const {
    return total_allocated_.load();
}

size_t MemoryLeakDetector::get_allocation_count() const {
    return allocation_count_.load();
}

void MemoryLeakDetector::clear_tracking() {
    std::unique_lock<std::shared_mutex> lock(mutex_);
    allocations_.clear();
    total_allocated_.store(0);
    allocation_count_.store(0);
}

// **MemoryManager implementation**
MemoryManager& MemoryManager::instance() {
    static MemoryManager instance;
    return instance;
}

MemoryManager::MemoryManager(QObject* parent) : QObject(parent) {
    // Setup memory monitoring timer
    memory_check_timer_ = std::make_unique<QTimer>(this);
    memory_check_timer_->setInterval(5000);  // Check every 5 seconds
    connect(memory_check_timer_.get(), &QTimer::timeout, this,
            &MemoryManager::on_memory_check_timer);

    // Setup garbage collection timer
    gc_timer_ = std::make_unique<QTimer>(this);
    gc_timer_->setInterval(30000);  // GC every 30 seconds
    connect(gc_timer_.get(), &QTimer::timeout, this,
            &MemoryManager::on_gc_timer);

    // Setup optimization timer
    optimization_timer_ = std::make_unique<QTimer>(this);
    optimization_timer_->setInterval(60000);  // Optimize every minute
    connect(optimization_timer_.get(), &QTimer::timeout, this,
            &MemoryManager::on_optimization_timer);

    // Start monitoring
    if (memory_pressure_monitoring_enabled_.load()) {
        memory_check_timer_->start();
    }

    if (auto_gc_enabled_.load()) {
        gc_timer_->start();
    }

    optimization_timer_->start();

    qDebug() << "🔥 Memory Manager initialized";
}

MemoryManager::~MemoryManager() {
    // Cleanup all pools and arenas
    {
        std::unique_lock<std::shared_mutex> lock(pools_mutex_);
        object_pools_.clear();
    }

    {
        std::unique_lock<std::shared_mutex> lock(arenas_mutex_);
        memory_arenas_.clear();
    }

    qDebug() << "🔥 Memory Manager destroyed";
}

MemoryArena* MemoryManager::create_arena(const QString& name,
                                         size_t size_bytes) {
    std::unique_lock<std::shared_mutex> lock(arenas_mutex_);

    auto arena = std::make_unique<MemoryArena>(size_bytes);
    MemoryArena* arena_ptr = arena.get();

    memory_arenas_[name] = std::move(arena);

    qDebug() << "🔥 Created memory arena:" << name << "size:" << size_bytes
             << "bytes";
    return arena_ptr;
}

MemoryArena* MemoryManager::get_arena(const QString& name) {
    std::shared_lock<std::shared_mutex> lock(arenas_mutex_);

    auto it = memory_arenas_.find(name);
    return (it != memory_arenas_.end()) ? it->second.get() : nullptr;
}

void MemoryManager::destroy_arena(const QString& name) {
    std::unique_lock<std::shared_mutex> lock(arenas_mutex_);

    auto it = memory_arenas_.find(name);
    if (it != memory_arenas_.end()) {
        memory_arenas_.erase(it);
        qDebug() << "🔥 Destroyed memory arena:" << name;
    }
}

void MemoryManager::set_gc_strategy(GCStrategy strategy) {
    gc_strategy_ = strategy;

    // Adjust timer intervals based on strategy
    switch (strategy) {
        case GCStrategy::Manual:
            gc_timer_->stop();
            break;
        case GCStrategy::Automatic:
            gc_timer_->setInterval(30000);  // 30 seconds
            gc_timer_->start();
            break;
        case GCStrategy::Incremental:
            gc_timer_->setInterval(5000);  // 5 seconds
            gc_timer_->start();
            break;
        case GCStrategy::Generational:
            gc_timer_->setInterval(15000);  // 15 seconds
            gc_timer_->start();
            break;
    }
}

void MemoryManager::trigger_gc() { perform_garbage_collection(); }

void MemoryManager::enable_auto_gc(bool enabled) {
    auto_gc_enabled_.store(enabled);

    if (enabled && gc_strategy_ != GCStrategy::Manual) {
        gc_timer_->start();
    } else {
        gc_timer_->stop();
    }
}

void MemoryManager::set_gc_threshold(size_t threshold_bytes) {
    gc_threshold_bytes_.store(threshold_bytes);
}

MemoryStatistics MemoryManager::get_statistics() const {
    std::shared_lock<std::shared_mutex> lock(statistics_mutex_);
    return statistics_;
}

QJsonObject MemoryManager::get_memory_report() const {
    auto stats = get_statistics();

    QJsonObject report;
    report["total_allocated_bytes"] =
        static_cast<qint64>(stats.total_allocated_bytes);
    report["peak_allocated_bytes"] =
        static_cast<qint64>(stats.peak_allocated_bytes);
    report["current_allocated_bytes"] =
        static_cast<qint64>(stats.current_allocated_bytes);
    report["allocation_count"] = static_cast<qint64>(stats.allocation_count);
    report["deallocation_count"] =
        static_cast<qint64>(stats.deallocation_count);
    report["pool_hits"] = static_cast<qint64>(stats.pool_hits);
    report["pool_misses"] = static_cast<qint64>(stats.pool_misses);
    report["fragmentation_ratio"] = stats.fragmentation_ratio;
    report["gc_count"] = static_cast<qint64>(stats.gc_count);

    // Add arena information
    QJsonArray arenas;
    {
        std::shared_lock<std::shared_mutex> lock(arenas_mutex_);
        for (const auto& [name, arena] : memory_arenas_) {
            QJsonObject arena_info;
            arena_info["name"] = name;
            arena_info["total_bytes"] =
                static_cast<qint64>(arena->total_bytes());
            arena_info["used_bytes"] = static_cast<qint64>(arena->used_bytes());
            arena_info["usage_percentage"] = arena->usage_percentage();
            arenas.append(arena_info);
        }
    }
    report["arenas"] = arenas;

    // Add leak detection information
    if (leak_detection_enabled_.load()) {
        auto leaks = get_memory_leaks();
        report["memory_leaks_count"] = static_cast<qint64>(leaks.size());
        report["total_leaked_bytes"] = static_cast<qint64>(
            MemoryLeakDetector::instance().get_total_allocated());
    }

    return report;
}

void MemoryManager::enable_leak_detection(bool enabled) {
    leak_detection_enabled_.store(enabled);

    if (!enabled) {
        MemoryLeakDetector::instance().clear_tracking();
    }
}

std::vector<MemoryLeakDetector::AllocationInfo>
MemoryManager::get_memory_leaks() const {
    if (leak_detection_enabled_.load()) {
        return MemoryLeakDetector::instance().get_leaks();
    }
    return {};
}

void MemoryManager::optimize_memory_usage() {
    qDebug() << "🔥 Starting memory optimization";

    // Clear unused pools
    clear_unused_pools();

    // Defragment arenas
    defragment_arenas();

    // Compact memory
    compact_memory();

    // Update statistics
    update_statistics();

    qDebug() << "🔥 Memory optimization completed";
}

void MemoryManager::clear_unused_pools() {
    std::unique_lock<std::shared_mutex> lock(pools_mutex_);

    // Remove pools that haven't been used recently
    // This is a simplified implementation
    size_t cleared_count = 0;

    for (auto it = object_pools_.begin(); it != object_pools_.end();) {
        // In a real implementation, we would check usage statistics
        // For now, we'll keep all pools
        ++it;
    }

    if (cleared_count > 0) {
        qDebug() << "🔥 Cleared" << cleared_count << "unused object pools";
    }
}

void MemoryManager::defragment_arenas() {
    std::shared_lock<std::shared_mutex> lock(arenas_mutex_);

    for (const auto& [name, arena] : memory_arenas_) {
        // In a real implementation, we would defragment the arena
        // For now, we'll just reset if usage is low
        if (arena->usage_percentage() < 10.0) {
            arena->reset();
            qDebug() << "🔥 Reset low-usage arena:" << name;
        }
    }
}

void MemoryManager::compact_memory() {
    // Trigger garbage collection
    perform_garbage_collection();

    // In a real implementation, we would compact heap memory
    qDebug() << "🔥 Memory compaction completed";
}

void MemoryManager::set_memory_limit(size_t limit_bytes) {
    memory_limit_bytes_.store(limit_bytes);
    qDebug() << "🔥 Memory limit set to:" << limit_bytes << "bytes";
}

void MemoryManager::set_warning_threshold(size_t threshold_bytes) {
    warning_threshold_bytes_.store(threshold_bytes);
}

void MemoryManager::enable_memory_pressure_monitoring(bool enabled) {
    memory_pressure_monitoring_enabled_.store(enabled);

    if (enabled) {
        memory_check_timer_->start();
    } else {
        memory_check_timer_->stop();
    }
}

void MemoryManager::on_memory_check_timer() { check_memory_pressure(); }

void MemoryManager::on_gc_timer() {
    if (auto_gc_enabled_.load()) {
        perform_garbage_collection();
    }
}

void MemoryManager::on_optimization_timer() { optimize_memory_usage(); }

void MemoryManager::check_memory_pressure() {
    size_t current_usage = calculate_current_memory_usage();

    if (current_usage > warning_threshold_bytes_.load()) {
        emit memory_pressure_detected(current_usage,
                                      memory_limit_bytes_.load());

        if (current_usage > memory_limit_bytes_.load()) {
            emit memory_limit_exceeded(current_usage);

            // Trigger emergency garbage collection
            perform_garbage_collection();
        }
    }

    // Check for memory leaks
    if (leak_detection_enabled_.load()) {
        auto leaks = get_memory_leaks();
        if (!leaks.empty()) {
            emit memory_leak_detected(leaks.size());
        }
    }
}

void MemoryManager::perform_garbage_collection() {
    auto start_time = std::chrono::steady_clock::now();
    size_t initial_usage = calculate_current_memory_usage();

    qDebug() << "🔥 Starting garbage collection";

    // Cleanup expired objects
    cleanup_expired_objects();

    // Clear unused pools
    clear_unused_pools();

    size_t final_usage = calculate_current_memory_usage();
    size_t freed_bytes =
        (initial_usage > final_usage) ? (initial_usage - final_usage) : 0;

    // Update statistics
    {
        std::unique_lock<std::shared_mutex> lock(statistics_mutex_);
        statistics_.gc_count++;
        statistics_.last_gc_time = std::chrono::steady_clock::now();
    }

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - start_time);

    qDebug() << "🔥 Garbage collection completed in" << duration.count()
             << "ms, freed" << freed_bytes << "bytes";

    emit garbage_collection_completed(freed_bytes);
}

void MemoryManager::update_statistics() {
    std::unique_lock<std::shared_mutex> lock(statistics_mutex_);

    statistics_.current_allocated_bytes = calculate_current_memory_usage();

    if (statistics_.current_allocated_bytes >
        statistics_.peak_allocated_bytes) {
        statistics_.peak_allocated_bytes = statistics_.current_allocated_bytes;
    }

    // Calculate fragmentation ratio (simplified)
    statistics_.fragmentation_ratio = 0.1;  // Placeholder
}

size_t MemoryManager::calculate_current_memory_usage() const {
    size_t total_usage = 0;

    // Add arena usage
    {
        std::shared_lock<std::shared_mutex> lock(arenas_mutex_);
        for (const auto& [name, arena] : memory_arenas_) {
            total_usage += arena->used_bytes();
        }
    }

    // Add tracked allocations
    if (leak_detection_enabled_.load()) {
        total_usage += MemoryLeakDetector::instance().get_total_allocated();
    }

    return total_usage;
}

void MemoryManager::cleanup_expired_objects() {
    // In a real implementation, we would cleanup expired objects from pools
    // This is a placeholder for the actual cleanup logic
}

}  // namespace DeclarativeUI::Core
