#pragma once

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QJsonObject>
#include <QMutex>
#include <QReadWriteLock>
#include <QTimer>
#include <QWidget>

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <atomic>
#include <chrono>
#include <shared_mutex>
#include <functional>
#include <variant>

namespace DeclarativeUI::Core {

// **Cache entry with metadata**
template<typename T>
struct CacheEntry {
    T data;
    QDateTime created_at;
    QDateTime last_accessed;
    QDateTime expires_at;
    std::atomic<size_t> access_count{0};
    std::atomic<size_t> memory_size{0};
    bool is_dirty = false;
    
    bool isExpired() const {
        return expires_at.isValid() && QDateTime::currentDateTime() > expires_at;
    }
    
    void touch() {
        last_accessed = QDateTime::currentDateTime();
        access_count.fetch_add(1);
    }
};

// **Cache eviction policies**
enum class EvictionPolicy {
    LRU,        // Least Recently Used
    LFU,        // Least Frequently Used
    FIFO,       // First In, First Out
    TTL,        // Time To Live
    Adaptive    // Adaptive based on usage patterns
};

// **Cache statistics for monitoring**
struct CacheStatistics {
    std::atomic<size_t> total_requests{0};
    std::atomic<size_t> cache_hits{0};
    std::atomic<size_t> cache_misses{0};
    std::atomic<size_t> evictions{0};
    std::atomic<size_t> total_memory_usage{0};
    std::atomic<size_t> max_memory_usage{0};
    
    double getHitRatio() const {
        size_t total = total_requests.load();
        return total > 0 ? static_cast<double>(cache_hits.load()) / total : 0.0;
    }
};

// **Generic LRU Cache with advanced features**
template<typename Key, typename Value>
class LRUCache {
public:
    using CacheEntryType = CacheEntry<Value>;
    using KeyType = Key;
    using ValueType = Value;
    
    explicit LRUCache(size_t max_size = 1000, size_t max_memory_mb = 100);
    ~LRUCache() = default;

    // **Core cache operations**
    bool put(const Key& key, const Value& value, 
             const QDateTime& expires_at = QDateTime());
    std::optional<Value> get(const Key& key);
    bool contains(const Key& key) const;
    bool remove(const Key& key);
    void clear();

    // **Advanced operations**
    void setEvictionPolicy(EvictionPolicy policy);
    void setTTL(std::chrono::milliseconds ttl);
    void enableAutoCleanup(bool enabled);
    
    // **Batch operations**
    void putBatch(const std::unordered_map<Key, Value>& items);
    std::unordered_map<Key, Value> getBatch(const std::vector<Key>& keys);
    
    // **Statistics and monitoring**
    CacheStatistics getStatistics() const;
    size_t size() const;
    size_t memoryUsage() const;
    double getHitRatio() const;
    
    // **Maintenance**
    void cleanup();
    void optimize();
    void preload(const std::function<std::unordered_map<Key, Value>()>& loader);

private:
    mutable std::shared_mutex mutex_;
    std::unordered_map<Key, std::shared_ptr<CacheEntryType>> cache_;
    std::list<Key> access_order_;  // For LRU tracking
    std::unordered_map<Key, typename std::list<Key>::iterator> access_iterators_;
    
    size_t max_size_;
    size_t max_memory_bytes_;
    EvictionPolicy eviction_policy_ = EvictionPolicy::LRU;
    std::chrono::milliseconds default_ttl_{0};
    bool auto_cleanup_enabled_ = true;
    
    mutable CacheStatistics statistics_;
    
    void evictIfNeeded();
    void evictLRU();
    void evictLFU();
    void evictExpired();
    void updateAccessOrder(const Key& key);
    size_t calculateMemorySize(const Value& value) const;
};

// **Specialized caches for different data types**
using WidgetCache = LRUCache<QString, std::shared_ptr<QWidget>>;
using StylesheetCache = LRUCache<QString, QString>;
using PropertyCache = LRUCache<QString, QVariant>;
using FileContentCache = LRUCache<QString, QByteArray>;
using JSONCache = LRUCache<QString, QJsonObject>;

// **Multi-level cache manager**
class CacheManager : public QObject {
    Q_OBJECT

public:
    explicit CacheManager(QObject* parent = nullptr);
    ~CacheManager() override;

    // **Cache management**
    void initializeCache(const QString& cache_name, size_t max_size, size_t max_memory_mb);
    void setCachePolicy(const QString& cache_name, EvictionPolicy policy);
    void enableCache(const QString& cache_name, bool enabled);

    // **Widget caching**
    void cacheWidget(const QString& key, std::shared_ptr<QWidget> widget);
    std::shared_ptr<QWidget> getCachedWidget(const QString& key);
    
    // **Stylesheet caching**
    void cacheStylesheet(const QString& key, const QString& stylesheet);
    QString getCachedStylesheet(const QString& key);
    
    // **Property caching**
    void cacheProperty(const QString& key, const QVariant& value);
    QVariant getCachedProperty(const QString& key);
    
    // **File content caching**
    void cacheFileContent(const QString& file_path, const QByteArray& content);
    QByteArray getCachedFileContent(const QString& file_path);
    
    // **JSON caching**
    void cacheJSON(const QString& key, const QJsonObject& json);
    QJsonObject getCachedJSON(const QString& key);

    // **Cache invalidation**
    void invalidateCache(const QString& cache_name);
    void invalidateKey(const QString& cache_name, const QString& key);
    void invalidatePattern(const QString& cache_name, const QString& pattern);
    void invalidateAll();

    // **Performance optimization**
    void optimizeAllCaches();
    void preloadCache(const QString& cache_name, 
                     const std::function<void()>& preloader);
    void enableSmartPrefetching(bool enabled);

    // **Monitoring and statistics**
    QJsonObject getCacheStatistics() const;
    QJsonObject getCacheStatistics(const QString& cache_name) const;
    double getOverallHitRatio() const;
    size_t getTotalMemoryUsage() const;

    // **Configuration**
    void setGlobalMemoryLimit(size_t limit_mb);
    void setCleanupInterval(int seconds);
    void enableCompressionForCache(const QString& cache_name, bool enabled);

signals:
    void cacheHit(const QString& cache_name, const QString& key);
    void cacheMiss(const QString& cache_name, const QString& key);
    void cacheEviction(const QString& cache_name, const QString& key);
    void memoryLimitReached(const QString& cache_name);

private slots:
    void onCleanupTimer();
    void onMemoryPressure();

private:
    std::unique_ptr<WidgetCache> widget_cache_;
    std::unique_ptr<StylesheetCache> stylesheet_cache_;
    std::unique_ptr<PropertyCache> property_cache_;
    std::unique_ptr<FileContentCache> file_content_cache_;
    std::unique_ptr<JSONCache> json_cache_;
    
    std::unordered_map<QString, std::unique_ptr<QObject>> custom_caches_;
    std::unordered_set<QString> enabled_caches_;
    
    std::atomic<size_t> global_memory_limit_bytes_{100 * 1024 * 1024}; // 100MB
    std::atomic<bool> smart_prefetching_enabled_{false};
    
    std::unique_ptr<QTimer> cleanup_timer_;
    std::unique_ptr<QTimer> memory_monitor_timer_;
    
    mutable std::shared_mutex global_mutex_;
    
    void initializeDefaultCaches();
    void checkMemoryPressure();
    void performGlobalCleanup();
    size_t calculateTotalMemoryUsage() const;
    void evictFromLargestCache();
};

}  // namespace DeclarativeUI::Core
