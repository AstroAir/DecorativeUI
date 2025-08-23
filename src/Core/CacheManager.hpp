#pragma once

#include <QDateTime>
#include <QJsonObject>
#include <QMutex>
#include <QObject>
#include <QReadWriteLock>
#include <QString>
#include <QTimer>
#include <QWidget>

#include <atomic>
#include <chrono>
#include <functional>
#include <list>
#include <memory>
#include <shared_mutex>
#include <unordered_map>
#include <unordered_set>
#include <variant>

namespace DeclarativeUI::Core {

/**
 * @file CacheManager.hpp
 * @brief High-performance, multi-type caching utilities used by the
 * DeclarativeUI framework.
 *
 * This header declares:
 * - CacheEntry<T>: typed cache record with metadata and lifecycle helpers.
 * - EvictionPolicy: supported eviction strategies.
 * - CacheStatistics: counters and helpers for monitoring cache behaviour.
 * - LRUCache<Key,Value>: a generic, thread-friendly cache implementation with
 * multiple eviction strategies, TTL support, memory accounting, and monitoring
 * hooks.
 * - Concrete type aliases for common cached values (widgets, stylesheets, JSON,
 * etc).
 * - CacheManager: a centralized multi-cache controller intended for
 * application-level use.
 *
 * Notes:
 * - Thread-safety: LRUCache uses an internal shared_mutex for concurrent reads
 * and exclusive writes.
 * - Time: uses QDateTime for timestamps and std::chrono for TTL configuration.
 */

/**
 * @brief Single cache entry wrapping a value together with metadata.
 *
 * @tparam T The stored value type.
 *
 * The entry maintains creation, last-accessed and optional expiry timestamps,
 * as well as an atomic access counter and best-effort memory size estimate. The
 * structure is lightweight so it can be stored in shared_ptr for safe
 * concurrent usage.
 */
template <typename T>
struct CacheEntry {
    T data;                  /**< The stored value. */
    QDateTime created_at;    /**< Timestamp when the entry was created. */
    QDateTime last_accessed; /**< Timestamp of the most recent access. */
    QDateTime expires_at;    /**< Optional expiry timestamp; invalid() means no
                                expiry. */
    std::atomic<size_t> access_count{
        0}; /**< Number of times the entry has been touched. */
    std::atomic<size_t> memory_size{
        0};                /**< Best-effort memory footprint in bytes. */
    bool is_dirty = false; /**< Flag for user-managed dirty state (e.g. lazy
                              persistence). */

    /**
     * @brief Returns true if the entry is expired based on expires_at.
     */
    bool isExpired() const {
        return expires_at.isValid() &&
               QDateTime::currentDateTime() > expires_at;
    }

    /**
     * @brief Mark the entry as accessed and update metadata.
     *
     * Updates last_accessed and increments access_count atomically.
     */
    void touch() {
        last_accessed = QDateTime::currentDateTime();
        access_count.fetch_add(1);
    }
};

/**
 * @brief Supported cache eviction policies.
 *
 * - LRU: Least Recently Used — evict items which haven't been accessed
 * recently.
 * - LFU: Least Frequently Used — evict items with the lowest access frequency.
 * - FIFO: First In, First Out — evict oldest inserted items.
 * - TTL: Time To Live — evict entries when their TTL expires.
 * - Adaptive: Heuristic-driven policy that can mix LRU/LFU based on runtime
 * signals.
 */
enum class EvictionPolicy {
    LRU,      ///< Least Recently Used
    LFU,      ///< Least Frequently Used
    FIFO,     ///< First In, First Out
    TTL,      ///< Time To Live (expiry-aware)
    Adaptive  ///< Adaptive decision based on usage patterns
};

/**
 * @brief Aggregate statistics for a single cache instance.
 *
 * Counters are atomic to allow lock-free reads from monitoring threads.
 * Use getHitRatio() to compute a convenient hit rate value.
 */
struct CacheStatistics {
    std::atomic<size_t> total_requests{0}; /**< Total cache lookups. */
    std::atomic<size_t> cache_hits{0};     /**< Number of successful lookups. */
    std::atomic<size_t> cache_misses{0};   /**< Number of failed lookups. */
    std::atomic<size_t> evictions{0};      /**< Number of evicted entries. */
    std::atomic<size_t> total_memory_usage{
        0}; /**< Total memory usage tracked for this cache. */
    std::atomic<size_t> max_memory_usage{
        0}; /**< Observed max memory usage for this cache. */

    /**
     * @brief Compute the hit ratio as cache_hits / total_requests.
     * @return Hit ratio in range [0.0, 1.0]. Returns 0.0 when no requests
     * recorded.
     */
    double getHitRatio() const {
        size_t total = total_requests.load();
        return total > 0 ? static_cast<double>(cache_hits.load()) / total : 0.0;
    }
};

/**
 * @brief Non-atomic version of CacheStatistics for returning snapshots.
 *
 * This struct can be copied and moved, making it suitable for return values.
 */
struct CacheStatisticsSnapshot {
    size_t total_requests{0}; /**< Total cache lookups. */
    size_t cache_hits{0};     /**< Number of successful lookups. */
    size_t cache_misses{0};   /**< Number of failed lookups. */
    size_t evictions{0};      /**< Number of evicted entries. */
    size_t total_memory_usage{
        0}; /**< Total memory usage tracked for this cache. */
    size_t max_memory_usage{
        0}; /**< Observed max memory usage for this cache. */

    /**
     * @brief Compute the hit ratio as cache_hits / total_requests.
     * @return Hit ratio in range [0.0, 1.0]. Returns 0.0 when no requests
     * recorded.
     */
    double getHitRatio() const {
        return total_requests > 0
                   ? static_cast<double>(cache_hits) / total_requests
                   : 0.0;
    }
};

/**
 * @brief Generic in-memory cache with configurable size/memory limits and
 * eviction.
 *
 * @tparam Key   Type used as the cache key. Must be hashable and comparable
 * (used in unordered_map).
 * @tparam Value Stored value type. Large objects should be wrapped in
 * shared_ptr where appropriate.
 *
 * Features:
 * - Optional TTL per-entry.
 * - Multiple eviction policies: LRU, LFU, FIFO, TTL, Adaptive.
 * - Batch operations for efficient bulk load/store.
 * - Thread-safe reads with shared locking; writes use exclusive locking.
 * - Memory accounting via calculateMemorySize() to enforce memory limits.
 * - Statistics collection accessible from external monitors.
 */
template <typename Key, typename Value>
class LRUCache {
public:
    using CacheEntryType = CacheEntry<Value>;
    using KeyType = Key;
    using ValueType = Value;

    /**
     * @brief Construct a cache instance.
     * @param max_size Maximum number of entries allowed before eviction is
     * triggered.
     * @param max_memory_mb Soft memory limit in megabytes (converted internally
     * to bytes).
     */
    explicit LRUCache(size_t max_size = 1000, size_t max_memory_mb = 100);
    ~LRUCache() = default;

    /**
     * @name Core cache operations
     * @{
     */

    /**
     * @brief Insert or update a cache entry.
     * @param key Cache key.
     * @param value Value to store.
     * @param expires_at Optional explicit expiry time; pass default-constructed
     * QDateTime for none.
     * @return True if the item was inserted or updated successfully.
     *
     * Notes:
     * - This method updates internal access order and statistics.
     */
    bool put(const Key& key, const Value& value,
             const QDateTime& expires_at = QDateTime());

    /**
     * @brief Retrieve a value from the cache.
     * @param key Lookup key.
     * @return std::optional<Value> containing the value if present and not
     * expired, std::nullopt otherwise.
     *
     * The returned Value is a copy unless Value is a pointer/shared_ptr type.
     */
    std::optional<Value> get(const Key& key);

    /**
     * @brief Returns true if key is present and not expired.
     */
    bool contains(const Key& key) const;

    /**
     * @brief Remove an entry by key.
     * @return True if an entry was removed.
     */
    bool remove(const Key& key);

    /**
     * @brief Purge all entries from the cache.
     */
    void clear();
    /** @} */

    /**
     * @name Advanced operations
     * @{
     */

    /**
     * @brief Set eviction policy used when the cache must free space.
     */
    void setEvictionPolicy(EvictionPolicy policy);

    /**
     * @brief Set default time-to-live applied to new entries.
     * @param ttl Duration in milliseconds.
     */
    void setTTL(std::chrono::milliseconds ttl);

    /**
     * @brief Enable or disable background automatic cleanup (when implemented).
     */
    void enableAutoCleanup(bool enabled);
    /** @} */

    /**
     * @name Batch operations
     * @{
     */

    /**
     * @brief Insert multiple items in one call.
     * @param items Map of key->value to insert.
     *
     * Batch puts should be more efficient than repeated single put calls.
     */
    void putBatch(const std::unordered_map<Key, Value>& items);

    /**
     * @brief Retrieve multiple values by keys.
     * @param keys Vector of keys to retrieve.
     * @return Map of found key->value pairs.
     */
    std::unordered_map<Key, Value> getBatch(const std::vector<Key>& keys);
    /** @} */

    /**
     * @name Statistics and monitoring
     * @{
     */
    CacheStatisticsSnapshot getStatistics()
        const;           /**< Return a snapshot of current cache statistics. */
    size_t size() const; /**< Return current number of entries. */
    size_t memoryUsage()
        const; /**< Return current tracked memory usage in bytes. */
    double getHitRatio()
        const; /**< Convenience call to statistics.getHitRatio(). */
    /** @} */

    /**
     * @name Maintenance helpers
     * @{
     */
    /**
     * @brief Trigger eviction and cleanup operations (removes expired or
     * low-priority entries).
     */
    void cleanup();

    /**
     * @brief Optimize internal structures (compaction, reindexing) for
     * performance.
     */
    void optimize();

    /**
     * @brief Preload the cache using a user-provided loader function.
     * @param loader Function that returns a map of key->value to preload.
     *
     * Implementations may use this to bulk-insert items while minimizing
     * repeated eviction checks.
     */
    void preload(const std::function<std::unordered_map<Key, Value>()>& loader);
    /** @} */

private:
    mutable std::shared_mutex mutex_; /**< Protects cache_ and metadata. */
    std::unordered_map<Key, std::shared_ptr<CacheEntryType>>
        cache_;                   /**< Main storage of entries. */
    std::list<Key> access_order_; /**< Recency list for LRU/FIFO. */
    std::unordered_map<Key, typename std::list<Key>::iterator>
        access_iterators_; /**< Index into access_order_. */

    size_t max_size_; /**< Max number of items before eviction triggers. */
    size_t
        max_memory_bytes_; /**< Max memory in bytes before eviction triggers. */
    EvictionPolicy eviction_policy_ =
        EvictionPolicy::LRU; /**< Currently selected eviction strategy. */
    std::chrono::milliseconds default_ttl_{
        0}; /**< Default TTL for entries inserted without expires_at. */
    bool auto_cleanup_enabled_ =
        true; /**< When true, background cleanup may run. */

    mutable CacheStatistics statistics_; /**< Per-cache monitoring counters. */

    /**
     * @brief Evict entries until the cache respects size/memory limits.
     *
     * Called internally after mutations which may increase resource usage.
     */
    void evictIfNeeded();

    /**
     * @brief Eviction implementations for each policy.
     * - evictLRU: remove least recently used entries.
     * - evictLFU: remove entries with lowest access_count.
     * - evictExpired: remove entries whose expires_at has passed.
     */
    void evictLRU();
    void evictLFU();
    void evictExpired();

    /**
     * @brief Update internal access order when an entry is accessed.
     * @param key The accessed key.
     */
    void updateAccessOrder(const Key& key);

    /**
     * @brief Estimate memory size of a value. Override/extend as necessary for
     * custom value types.
     * @param value Candidate value.
     * @return Best-effort size in bytes.
     */
    size_t calculateMemorySize(const Value& value) const;

    /**
     * @brief Helper functions for cache retrieval operations
     * These functions break down the complex get() operation into smaller,
     * more manageable pieces for better maintainability and testing.
     */
    std::optional<Value> handleExpiredEntry(
        const Key& key, std::shared_lock<std::shared_mutex>& lock);
    std::optional<Value> updateAccessAndReturn(
        const Key& key, std::shared_lock<std::shared_mutex>& lock);
    void removeExpiredEntry(
        const Key& key, typename std::unordered_map<
                            Key, std::shared_ptr<CacheEntryType>>::iterator it);
};

/**
 * @brief Common cache type aliases for application must-have types.
 *
 * These type aliases make it convenient to work with frequently used caches,
 * e.g. widget instances (shared_ptr<QWidget>) or textual stylesheets.
 */
using WidgetCache = LRUCache<QString, std::shared_ptr<QWidget>>;
using StylesheetCache = LRUCache<QString, QString>;
using PropertyCache = LRUCache<QString, QVariant>;
using FileContentCache = LRUCache<QString, QByteArray>;
using JSONCache = LRUCache<QString, QJsonObject>;

/**
 * @brief Application-level manager that owns multiple cache instances and
 * provides centralized control.
 *
 * The CacheManager coordinates lifecycle, configuration and global policies
 * across many typed caches. It exposes convenience methods for the common types
 * defined above and also allows custom caches to be registered via the
 * custom_caches_ map.
 *
 * Threading:
 * - Public methods are safe to call from multiple threads. Internals use a
 * shared_mutex to guard cache registration and global memory accounting.
 *
 * Signals:
 * - cacheHit / cacheMiss / cacheEviction allow UIs or telemetry components to
 * observe runtime behavior.
 */
class CacheManager : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Construct a CacheManager instance.
     * @param parent Optional QObject parent used for Qt ownership semantics.
     */
    explicit CacheManager(QObject* parent = nullptr);
    ~CacheManager() override;

    /** @name Cache lifecycle and configuration */
    void initializeCache(const QString& cache_name, size_t max_size,
                         size_t max_memory_mb);
    void setCachePolicy(const QString& cache_name, EvictionPolicy policy);
    void enableCache(const QString& cache_name, bool enabled);

    /** @name Widget caching convenience APIs */
    void cacheWidget(const QString& key, std::shared_ptr<QWidget> widget);
    std::shared_ptr<QWidget> getCachedWidget(const QString& key);

    /** @name Stylesheet caching convenience APIs */
    void cacheStylesheet(const QString& key, const QString& stylesheet);
    QString getCachedStylesheet(const QString& key);

    /** @name Property caching convenience APIs */
    void cacheProperty(const QString& key, const QVariant& value);
    QVariant getCachedProperty(const QString& key);

    /** @name File content caching convenience APIs */
    void cacheFileContent(const QString& file_path, const QByteArray& content);
    QByteArray getCachedFileContent(const QString& file_path);

    /** @name JSON object caching convenience APIs */
    void cacheJSON(const QString& key, const QJsonObject& json);
    QJsonObject getCachedJSON(const QString& key);

    /** @name Invalidation APIs */
    void invalidateCache(const QString& cache_name);
    void invalidateKey(const QString& cache_name, const QString& key);
    void invalidatePattern(const QString& cache_name, const QString& pattern);
    void invalidateAll();

    /** @name Performance & prefetching */
    void optimizeAllCaches();
    void preloadCache(const QString& cache_name,
                      const std::function<void()>& preloader);
    void enableSmartPrefetching(bool enabled);

    /** @name Monitoring & statistics */
    QJsonObject getCacheStatistics() const;
    QJsonObject getCacheStatistics(const QString& cache_name) const;
    double getOverallHitRatio() const;
    size_t getTotalMemoryUsage() const;

    /** @name Global configuration */
    void setGlobalMemoryLimit(size_t limit_mb);
    void setCleanupInterval(int seconds);
    void enableCompressionForCache(const QString& cache_name, bool enabled);

signals:
    /**
     * @brief Emitted when a cache lookup results in a hit.
     * @param cache_name Name of the cache that produced the hit.
     * @param key Key that was found.
     */
    void cacheHit(const QString& cache_name, const QString& key);

    /**
     * @brief Emitted when a cache lookup results in a miss.
     * @param cache_name Name of the cache that missed.
     * @param key Key that was looked up.
     */
    void cacheMiss(const QString& cache_name, const QString& key);

    /**
     * @brief Emitted when an entry is evicted from a cache.
     * @param cache_name Name of the source cache.
     * @param key Key that was evicted.
     */
    void cacheEviction(const QString& cache_name, const QString& key);

    /**
     * @brief Emitted when a cache reaches or surpasses a configured memory
     * limit.
     */
    void memoryLimitReached(const QString& cache_name);

private slots:
    /**
     * @brief Slot invoked periodically to perform scheduled cleanup.
     *
     * Driven by cleanup_timer_ when the manager is configured to run periodic
     * maintenance.
     */
    void onCleanupTimer();

    /**
     * @brief Handler for platform- or environment-driven memory pressure
     * notifications.
     *
     * Implementations should trigger eviction/compaction under memory pressure.
     */
    void onMemoryPressure();

private:
    std::unique_ptr<WidgetCache> widget_cache_;
    std::unique_ptr<StylesheetCache> stylesheet_cache_;
    std::unique_ptr<PropertyCache> property_cache_;
    std::unique_ptr<FileContentCache> file_content_cache_;
    std::unique_ptr<JSONCache> json_cache_;

    std::unordered_map<QString, std::unique_ptr<QObject>>
        custom_caches_; /**< Generic custom cache registry. */
    std::unordered_set<QString>
        enabled_caches_; /**< Names of caches currently enabled. */

    std::atomic<size_t> global_memory_limit_bytes_{
        100 * 1024 * 1024}; /**< Global memory soft-limit in bytes. */
    std::atomic<bool> smart_prefetching_enabled_{
        false}; /**< When true, prefetch logic may run. */

    std::unique_ptr<QTimer>
        cleanup_timer_; /**< Periodic cleanup timer (optional). */
    std::unique_ptr<QTimer> memory_monitor_timer_; /**< Timer to monitor memory
                                                      pressure (optional). */

    mutable std::shared_mutex
        global_mutex_; /**< Protects global registries and memory accounting. */

    /** Initialization helpers and internal maintenance routines. */
    void initializeDefaultCaches();
    void checkMemoryPressure();
    void performGlobalCleanup();
    size_t calculateTotalMemoryUsage() const;
    void evictFromLargestCache();
};

}  // namespace DeclarativeUI::Core
