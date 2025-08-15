#include "CacheManager.hpp"

#include <QDebug>
#include <QRegularExpression>
#include <algorithm>
#include <mutex>

namespace DeclarativeUI::Core {

// **LRUCache template implementation**
template <typename Key, typename Value>
LRUCache<Key, Value>::LRUCache(size_t max_size, size_t max_memory_mb)
    : max_size_(max_size), max_memory_bytes_(max_memory_mb * 1024 * 1024) {}

template <typename Key, typename Value>
bool LRUCache<Key, Value>::put(const Key& key, const Value& value,
                               const QDateTime& expires_at) {
    std::unique_lock<std::shared_mutex> lock(mutex_);

    statistics_.total_requests.fetch_add(1);

    // Remove existing entry if present
    auto existing_it = cache_.find(key);
    if (existing_it != cache_.end()) {
        // Update existing entry
        existing_it->second->data = value;
        existing_it->second->last_accessed = QDateTime::currentDateTime();
        existing_it->second->expires_at = expires_at;
        existing_it->second->is_dirty = false;
        updateAccessOrder(key);
        return true;
    }

    // Create new entry
    auto entry = std::make_shared<CacheEntryType>();
    entry->data = value;
    entry->created_at = QDateTime::currentDateTime();
    entry->last_accessed = entry->created_at;
    entry->expires_at = expires_at;
    entry->memory_size.store(calculateMemorySize(value));

    // Check memory constraints
    if (entry->memory_size.load() > max_memory_bytes_) {
        return false;  // Value too large for cache
    }

    cache_[key] = entry;
    access_order_.push_front(key);
    access_iterators_[key] = access_order_.begin();

    statistics_.total_memory_usage.fetch_add(entry->memory_size.load());

    // Evict if necessary
    evictIfNeeded();

    return true;
}

template <typename Key, typename Value>
std::optional<Value> LRUCache<Key, Value>::get(const Key& key) {
    std::shared_lock<std::shared_mutex> lock(mutex_);

    statistics_.total_requests.fetch_add(1);

    auto it = cache_.find(key);
    if (it == cache_.end()) {
        statistics_.cache_misses.fetch_add(1);
        return std::nullopt;
    }

    auto& entry = it->second;

    // Check if expired
    if (entry->isExpired()) {
        lock.unlock();
        std::unique_lock<std::shared_mutex> write_lock(mutex_);
        auto it_recheck = cache_.find(key);  // Re-find after lock upgrade
        if (it_recheck != cache_.end()) {
            statistics_.total_memory_usage.fetch_sub(
                it_recheck->second->memory_size.load());
            cache_.erase(it_recheck);
            // Remove from access order
            auto access_it = access_iterators_.find(key);
            if (access_it != access_iterators_.end()) {
                access_order_.erase(access_it->second);
                access_iterators_.erase(access_it);
            }
        }
        statistics_.cache_misses.fetch_add(1);
        return std::nullopt;
    }

    // Need to upgrade to exclusive lock for updating access order
    lock.unlock();
    std::unique_lock<std::shared_mutex> write_lock(mutex_);

    // Re-find entry after lock upgrade
    auto it_recheck = cache_.find(key);
    if (it_recheck == cache_.end() || it_recheck->second->isExpired()) {
        statistics_.cache_misses.fetch_add(1);
        return std::nullopt;
    }

    // Update access information
    it_recheck->second->touch();
    updateAccessOrder(key);

    statistics_.cache_hits.fetch_add(1);
    return it_recheck->second->data;
}

template <typename Key, typename Value>
bool LRUCache<Key, Value>::contains(const Key& key) const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    auto it = cache_.find(key);
    return it != cache_.end() && !it->second->isExpired();
}

template <typename Key, typename Value>
bool LRUCache<Key, Value>::remove(const Key& key) {
    std::unique_lock<std::shared_mutex> lock(mutex_);

    auto it = cache_.find(key);
    if (it == cache_.end()) {
        return false;
    }

    statistics_.total_memory_usage.fetch_sub(it->second->memory_size.load());

    // Remove from access order
    auto access_it = access_iterators_.find(key);
    if (access_it != access_iterators_.end()) {
        access_order_.erase(access_it->second);
        access_iterators_.erase(access_it);
    }

    cache_.erase(it);
    return true;
}

template <typename Key, typename Value>
void LRUCache<Key, Value>::clear() {
    std::unique_lock<std::shared_mutex> lock(mutex_);
    cache_.clear();
    access_order_.clear();
    access_iterators_.clear();
    statistics_.total_memory_usage.store(0);
}

template <typename Key, typename Value>
void LRUCache<Key, Value>::evictIfNeeded() {
    while (cache_.size() > max_size_ ||
           statistics_.total_memory_usage.load() > max_memory_bytes_) {
        switch (eviction_policy_) {
            case EvictionPolicy::LRU:
                evictLRU();
                break;
            case EvictionPolicy::LFU:
                evictLFU();
                break;
            case EvictionPolicy::TTL:
                evictExpired();
                break;
            default:
                evictLRU();
                break;
        }

        if (cache_.empty())
            break;
    }
}

template <typename Key, typename Value>
void LRUCache<Key, Value>::evictLRU() {
    if (access_order_.empty())
        return;

    Key key_to_evict = access_order_.back();
    access_order_.pop_back();

    auto it = cache_.find(key_to_evict);
    if (it != cache_.end()) {
        statistics_.total_memory_usage.fetch_sub(
            it->second->memory_size.load());
        statistics_.evictions.fetch_add(1);
        cache_.erase(it);
    }

    access_iterators_.erase(key_to_evict);
}

template <typename Key, typename Value>
void LRUCache<Key, Value>::updateAccessOrder(const Key& key) {
    auto access_it = access_iterators_.find(key);
    if (access_it != access_iterators_.end()) {
        access_order_.erase(access_it->second);
    }

    access_order_.push_front(key);
    access_iterators_[key] = access_order_.begin();
}

template <typename Key, typename Value>
size_t LRUCache<Key, Value>::calculateMemorySize(const Value& value) const {
    // Simplified memory calculation - in practice, this would be more
    // sophisticated
    return sizeof(Value) + 64;  // Base overhead estimate
}

// **CacheManager implementation**
CacheManager::CacheManager(QObject* parent) : QObject(parent) {
    initializeDefaultCaches();

    // Setup cleanup timer
    cleanup_timer_ = std::make_unique<QTimer>(this);
    cleanup_timer_->setInterval(60000);  // Cleanup every minute
    connect(cleanup_timer_.get(), &QTimer::timeout, this,
            &CacheManager::onCleanupTimer);
    cleanup_timer_->start();

    // Setup memory monitor
    memory_monitor_timer_ = std::make_unique<QTimer>(this);
    memory_monitor_timer_->setInterval(10000);  // Check every 10 seconds
    connect(memory_monitor_timer_.get(), &QTimer::timeout, this,
            &CacheManager::onMemoryPressure);
    memory_monitor_timer_->start();
}

CacheManager::~CacheManager() = default;

void CacheManager::initializeDefaultCaches() {
    widget_cache_ =
        std::make_unique<WidgetCache>(1000, 50);  // 50MB for widgets
    stylesheet_cache_ =
        std::make_unique<StylesheetCache>(500, 10);  // 10MB for stylesheets
    property_cache_ =
        std::make_unique<PropertyCache>(2000, 5);  // 5MB for properties
    file_content_cache_ =
        std::make_unique<FileContentCache>(200, 20);      // 20MB for files
    json_cache_ = std::make_unique<JSONCache>(1000, 15);  // 15MB for JSON

    // Enable all default caches
    enabled_caches_.insert("widgets");
    enabled_caches_.insert("stylesheets");
    enabled_caches_.insert("properties");
    enabled_caches_.insert("files");
    enabled_caches_.insert("json");
}

void CacheManager::cacheWidget(const QString& key,
                               std::shared_ptr<QWidget> widget) {
    if (enabled_caches_.count("widgets") && widget_cache_) {
        bool success = widget_cache_->put(key, widget);
        if (success) {
            emit cacheHit("widgets", key);
        }
    }
}

std::shared_ptr<QWidget> CacheManager::getCachedWidget(const QString& key) {
    if (enabled_caches_.count("widgets") && widget_cache_) {
        auto result = widget_cache_->get(key);
        if (result.has_value()) {
            emit cacheHit("widgets", key);
            return result.value();
        } else {
            emit cacheMiss("widgets", key);
        }
    }
    return nullptr;
}

void CacheManager::cacheStylesheet(const QString& key,
                                   const QString& stylesheet) {
    if (enabled_caches_.count("stylesheets") && stylesheet_cache_) {
        bool success = stylesheet_cache_->put(key, stylesheet);
        if (success) {
            emit cacheHit("stylesheets", key);
        }
    }
}

QString CacheManager::getCachedStylesheet(const QString& key) {
    if (enabled_caches_.count("stylesheets") && stylesheet_cache_) {
        auto result = stylesheet_cache_->get(key);
        if (result.has_value()) {
            emit cacheHit("stylesheets", key);
            return result.value();
        } else {
            emit cacheMiss("stylesheets", key);
        }
    }
    return QString();
}

void CacheManager::cacheProperty(const QString& key, const QVariant& value) {
    if (enabled_caches_.count("properties") && property_cache_) {
        bool success = property_cache_->put(key, value);
        if (success) {
            emit cacheHit("properties", key);
        }
    }
}

QVariant CacheManager::getCachedProperty(const QString& key) {
    if (enabled_caches_.count("properties") && property_cache_) {
        auto result = property_cache_->get(key);
        if (result.has_value()) {
            emit cacheHit("properties", key);
            return result.value();
        } else {
            emit cacheMiss("properties", key);
        }
    }
    return QVariant();
}

void CacheManager::cacheFileContent(const QString& file_path,
                                    const QByteArray& content) {
    if (enabled_caches_.count("files") && file_content_cache_) {
        bool success = file_content_cache_->put(file_path, content);
        if (success) {
            emit cacheHit("files", file_path);
        }
    }
}

QByteArray CacheManager::getCachedFileContent(const QString& file_path) {
    if (enabled_caches_.count("files") && file_content_cache_) {
        auto result = file_content_cache_->get(file_path);
        if (result.has_value()) {
            emit cacheHit("files", file_path);
            return result.value();
        } else {
            emit cacheMiss("files", file_path);
        }
    }
    return QByteArray();
}

void CacheManager::cacheJSON(const QString& key, const QJsonObject& json) {
    if (enabled_caches_.count("json") && json_cache_) {
        bool success = json_cache_->put(key, json);
        if (success) {
            emit cacheHit("json", key);
        }
    }
}

QJsonObject CacheManager::getCachedJSON(const QString& key) {
    if (enabled_caches_.count("json") && json_cache_) {
        auto result = json_cache_->get(key);
        if (result.has_value()) {
            emit cacheHit("json", key);
            return result.value();
        } else {
            emit cacheMiss("json", key);
        }
    }
    return QJsonObject();
}

void CacheManager::invalidateAll() {
    std::unique_lock<std::shared_mutex> lock(global_mutex_);

    if (widget_cache_)
        widget_cache_->clear();
    if (stylesheet_cache_)
        stylesheet_cache_->clear();
    if (property_cache_)
        property_cache_->clear();
    if (file_content_cache_)
        file_content_cache_->clear();
    if (json_cache_)
        json_cache_->clear();

    qDebug() << "🔥 All caches invalidated";
}

void CacheManager::onCleanupTimer() { performGlobalCleanup(); }

void CacheManager::onMemoryPressure() { checkMemoryPressure(); }

void CacheManager::performGlobalCleanup() {
    // Cleanup expired entries from all caches
    if (widget_cache_)
        widget_cache_->cleanup();
    if (stylesheet_cache_)
        stylesheet_cache_->cleanup();
    if (property_cache_)
        property_cache_->cleanup();
    if (file_content_cache_)
        file_content_cache_->cleanup();
    if (json_cache_)
        json_cache_->cleanup();
}

void CacheManager::checkMemoryPressure() {
    size_t total_memory = calculateTotalMemoryUsage();

    if (total_memory > global_memory_limit_bytes_.load()) {
        qWarning() << "🔥 Cache memory limit exceeded:" << total_memory
                   << "bytes";
        evictFromLargestCache();
        emit memoryLimitReached("global");
    }
}

size_t CacheManager::calculateTotalMemoryUsage() const {
    size_t total = 0;

    if (widget_cache_)
        total += widget_cache_->memoryUsage();
    if (stylesheet_cache_)
        total += stylesheet_cache_->memoryUsage();
    if (property_cache_)
        total += property_cache_->memoryUsage();
    if (file_content_cache_)
        total += file_content_cache_->memoryUsage();
    if (json_cache_)
        total += json_cache_->memoryUsage();

    return total;
}

void CacheManager::evictFromLargestCache() {
    // Find the cache using the most memory and evict from it
    size_t max_memory = 0;
    std::string largest_cache;

    if (widget_cache_ && widget_cache_->memoryUsage() > max_memory) {
        max_memory = widget_cache_->memoryUsage();
        largest_cache = "widgets";
    }

    if (file_content_cache_ &&
        file_content_cache_->memoryUsage() > max_memory) {
        max_memory = file_content_cache_->memoryUsage();
        largest_cache = "files";
    }

    // Evict from the largest cache
    if (largest_cache == "widgets" && widget_cache_) {
        widget_cache_->cleanup();
    } else if (largest_cache == "files" && file_content_cache_) {
        file_content_cache_->cleanup();
    }
}

QJsonObject CacheManager::getCacheStatistics() const {
    QJsonObject stats;

    // Overall statistics
    stats["total_memory_usage"] =
        static_cast<qint64>(calculateTotalMemoryUsage());
    stats["global_memory_limit"] =
        static_cast<qint64>(global_memory_limit_bytes_.load());

    // Individual cache statistics
    QJsonObject cache_stats;

    if (widget_cache_) {
        QJsonObject widget_stats;
        widget_stats["size"] = static_cast<qint64>(widget_cache_->size());
        widget_stats["memory_usage"] =
            static_cast<qint64>(widget_cache_->memoryUsage());
        cache_stats["widget_cache"] = widget_stats;
    }

    if (stylesheet_cache_) {
        QJsonObject stylesheet_stats;
        stylesheet_stats["size"] =
            static_cast<qint64>(stylesheet_cache_->size());
        stylesheet_stats["memory_usage"] =
            static_cast<qint64>(stylesheet_cache_->memoryUsage());
        cache_stats["stylesheet_cache"] = stylesheet_stats;
    }

    if (property_cache_) {
        QJsonObject property_stats;
        property_stats["size"] = static_cast<qint64>(property_cache_->size());
        property_stats["memory_usage"] =
            static_cast<qint64>(property_cache_->memoryUsage());
        cache_stats["property_cache"] = property_stats;
    }

    stats["caches"] = cache_stats;
    return stats;
}

QJsonObject CacheManager::getCacheStatistics(const QString& cache_name) const {
    QJsonObject stats;

    if (cache_name == "widgets" && widget_cache_) {
        stats["size"] = static_cast<qint64>(widget_cache_->size());
        stats["memory_usage"] =
            static_cast<qint64>(widget_cache_->memoryUsage());
    } else if (cache_name == "stylesheets" && stylesheet_cache_) {
        stats["size"] = static_cast<qint64>(stylesheet_cache_->size());
        stats["memory_usage"] =
            static_cast<qint64>(stylesheet_cache_->memoryUsage());
    } else if (cache_name == "properties" && property_cache_) {
        stats["size"] = static_cast<qint64>(property_cache_->size());
        stats["memory_usage"] =
            static_cast<qint64>(property_cache_->memoryUsage());
    }

    return stats;
}

void CacheManager::initializeCache(const QString& cache_name, size_t max_size,
                                   size_t max_memory_mb) {
    std::unique_lock<std::shared_mutex> lock(global_mutex_);

    // For now, we only support the predefined cache types
    if (cache_name == "widgets" && !widget_cache_) {
        widget_cache_ = std::make_unique<WidgetCache>(max_size, max_memory_mb);
        enabled_caches_.insert("widgets");
    } else if (cache_name == "stylesheets" && !stylesheet_cache_) {
        stylesheet_cache_ =
            std::make_unique<StylesheetCache>(max_size, max_memory_mb);
        enabled_caches_.insert("stylesheets");
    } else if (cache_name == "properties" && !property_cache_) {
        property_cache_ =
            std::make_unique<PropertyCache>(max_size, max_memory_mb);
        enabled_caches_.insert("properties");
    } else if (cache_name == "files" && !file_content_cache_) {
        file_content_cache_ =
            std::make_unique<FileContentCache>(max_size, max_memory_mb);
        enabled_caches_.insert("files");
    } else if (cache_name == "json" && !json_cache_) {
        json_cache_ = std::make_unique<JSONCache>(max_size, max_memory_mb);
        enabled_caches_.insert("json");
    }

    qDebug() << "🔧 Cache" << cache_name
             << "initialized with max_size:" << max_size
             << "max_memory:" << max_memory_mb << "MB";
}

void CacheManager::setCachePolicy(const QString& cache_name,
                                  EvictionPolicy policy) {
    std::shared_lock<std::shared_mutex> lock(global_mutex_);

    if (cache_name == "widgets" && widget_cache_) {
        widget_cache_->setEvictionPolicy(policy);
    } else if (cache_name == "stylesheets" && stylesheet_cache_) {
        stylesheet_cache_->setEvictionPolicy(policy);
    } else if (cache_name == "properties" && property_cache_) {
        property_cache_->setEvictionPolicy(policy);
    } else if (cache_name == "files" && file_content_cache_) {
        file_content_cache_->setEvictionPolicy(policy);
    } else if (cache_name == "json" && json_cache_) {
        json_cache_->setEvictionPolicy(policy);
    }
}

void CacheManager::enableCache(const QString& cache_name, bool enabled) {
    std::unique_lock<std::shared_mutex> lock(global_mutex_);

    if (enabled) {
        enabled_caches_.insert(cache_name);
    } else {
        enabled_caches_.erase(cache_name);
    }

    qDebug() << "🔧 Cache" << cache_name << (enabled ? "enabled" : "disabled");
}

void CacheManager::invalidateCache(const QString& cache_name) {
    std::shared_lock<std::shared_mutex> lock(global_mutex_);

    if (cache_name == "widgets" && widget_cache_) {
        widget_cache_->clear();
    } else if (cache_name == "stylesheets" && stylesheet_cache_) {
        stylesheet_cache_->clear();
    } else if (cache_name == "properties" && property_cache_) {
        property_cache_->clear();
    } else if (cache_name == "files" && file_content_cache_) {
        file_content_cache_->clear();
    } else if (cache_name == "json" && json_cache_) {
        json_cache_->clear();
    }

    qDebug() << "🔥 Cache" << cache_name << "invalidated";
}

void CacheManager::invalidateKey(const QString& cache_name,
                                 const QString& key) {
    std::shared_lock<std::shared_mutex> lock(global_mutex_);

    if (cache_name == "widgets" && widget_cache_) {
        widget_cache_->remove(key);
    } else if (cache_name == "stylesheets" && stylesheet_cache_) {
        stylesheet_cache_->remove(key);
    } else if (cache_name == "properties" && property_cache_) {
        property_cache_->remove(key);
    } else if (cache_name == "files" && file_content_cache_) {
        file_content_cache_->remove(key);
    } else if (cache_name == "json" && json_cache_) {
        json_cache_->remove(key);
    }
}

void CacheManager::invalidatePattern(const QString& cache_name,
                                     const QString& pattern) {
    std::shared_lock<std::shared_mutex> lock(global_mutex_);

    QRegularExpression regex(pattern);
    std::vector<QString> keys_to_remove;

    // This is a simplified implementation - in practice, you'd need to iterate
    // through cache keys For now, we'll just log the pattern matching request
    qDebug() << "🔍 Pattern invalidation requested for cache:" << cache_name
             << "pattern:" << pattern;

    // Note: Full implementation would require exposing key iteration from
    // LRUCache This is left as a placeholder for now
}

double CacheManager::getOverallHitRatio() const {
    std::shared_lock<std::shared_mutex> lock(global_mutex_);

    size_t total_requests = 0;
    size_t total_hits = 0;

    if (widget_cache_) {
        auto stats = widget_cache_->getStatistics();
        total_requests += stats.total_requests;
        total_hits += stats.cache_hits;
    }

    if (stylesheet_cache_) {
        auto stats = stylesheet_cache_->getStatistics();
        total_requests += stats.total_requests;
        total_hits += stats.cache_hits;
    }

    if (property_cache_) {
        auto stats = property_cache_->getStatistics();
        total_requests += stats.total_requests;
        total_hits += stats.cache_hits;
    }

    if (file_content_cache_) {
        auto stats = file_content_cache_->getStatistics();
        total_requests += stats.total_requests;
        total_hits += stats.cache_hits;
    }

    if (json_cache_) {
        auto stats = json_cache_->getStatistics();
        total_requests += stats.total_requests;
        total_hits += stats.cache_hits;
    }

    return total_requests > 0 ? static_cast<double>(total_hits) / total_requests
                              : 0.0;
}

size_t CacheManager::getTotalMemoryUsage() const {
    return calculateTotalMemoryUsage();
}

void CacheManager::optimizeAllCaches() {
    std::shared_lock<std::shared_mutex> lock(global_mutex_);

    if (widget_cache_)
        widget_cache_->optimize();
    if (stylesheet_cache_)
        stylesheet_cache_->optimize();
    if (property_cache_)
        property_cache_->optimize();
    if (file_content_cache_)
        file_content_cache_->optimize();
    if (json_cache_)
        json_cache_->optimize();

    qDebug() << "🔧 All caches optimized";
}

void CacheManager::preloadCache(const QString& cache_name,
                                const std::function<void()>& preloader) {
    std::shared_lock<std::shared_mutex> lock(global_mutex_);

    // Execute the preloader function
    preloader();

    qDebug() << "🔧 Cache" << cache_name << "preloaded";
}

void CacheManager::enableSmartPrefetching(bool enabled) {
    smart_prefetching_enabled_.store(enabled);
    qDebug() << "🔧 Smart prefetching" << (enabled ? "enabled" : "disabled");
}

void CacheManager::setCleanupInterval(int seconds) {
    if (cleanup_timer_) {
        cleanup_timer_->setInterval(seconds * 1000);
        qDebug() << "🔧 Cleanup interval set to" << seconds << "seconds";
    }
}

void CacheManager::enableCompressionForCache(const QString& cache_name,
                                             bool enabled) {
    // Compression is not implemented in this basic version
    // This is a placeholder for future enhancement
    qDebug() << "🔧 Compression for cache" << cache_name
             << (enabled ? "enabled" : "disabled") << "(not implemented)";
}

void CacheManager::setGlobalMemoryLimit(size_t limit_mb) {
    global_memory_limit_bytes_.store(limit_mb * 1024 * 1024);
    qDebug() << "🔧 Global cache memory limit set to" << limit_mb << "MB";
}

// **Additional LRUCache method implementations**
template <typename Key, typename Value>
void LRUCache<Key, Value>::setEvictionPolicy(EvictionPolicy policy) {
    std::unique_lock<std::shared_mutex> lock(mutex_);
    eviction_policy_ = policy;
}

template <typename Key, typename Value>
void LRUCache<Key, Value>::setTTL(std::chrono::milliseconds ttl) {
    std::unique_lock<std::shared_mutex> lock(mutex_);
    default_ttl_ = ttl;
}

template <typename Key, typename Value>
void LRUCache<Key, Value>::enableAutoCleanup(bool enabled) {
    std::unique_lock<std::shared_mutex> lock(mutex_);
    auto_cleanup_enabled_ = enabled;
}

template <typename Key, typename Value>
void LRUCache<Key, Value>::putBatch(
    const std::unordered_map<Key, Value>& items) {
    std::unique_lock<std::shared_mutex> lock(mutex_);

    for (const auto& pair : items) {
        QDateTime expires_at;
        if (default_ttl_.count() > 0) {
            expires_at =
                QDateTime::currentDateTime().addMSecs(default_ttl_.count());
        }

        // Create new entry
        auto entry = std::make_shared<CacheEntryType>();
        entry->data = pair.second;
        entry->created_at = QDateTime::currentDateTime();
        entry->last_accessed = entry->created_at;
        entry->expires_at = expires_at;
        entry->memory_size.store(calculateMemorySize(pair.second));

        // Remove existing entry if present
        auto existing_it = cache_.find(pair.first);
        if (existing_it != cache_.end()) {
            statistics_.total_memory_usage.fetch_sub(
                existing_it->second->memory_size.load());
            cache_.erase(existing_it);
        }

        cache_[pair.first] = entry;
        access_order_.push_front(pair.first);
        access_iterators_[pair.first] = access_order_.begin();

        statistics_.total_memory_usage.fetch_add(entry->memory_size.load());
    }

    evictIfNeeded();
}

template <typename Key, typename Value>
std::unordered_map<Key, Value> LRUCache<Key, Value>::getBatch(
    const std::vector<Key>& keys) {
    std::unordered_map<Key, Value> result;

    for (const auto& key : keys) {
        auto value = get(key);
        if (value.has_value()) {
            result[key] = value.value();
        }
    }

    return result;
}

template <typename Key, typename Value>
CacheStatisticsSnapshot LRUCache<Key, Value>::getStatistics() const {
    std::shared_lock<std::shared_mutex> lock(mutex_);

    CacheStatisticsSnapshot stats;
    stats.total_requests = statistics_.total_requests.load();
    stats.cache_hits = statistics_.cache_hits.load();
    stats.cache_misses = statistics_.cache_misses.load();
    stats.evictions = statistics_.evictions.load();
    stats.total_memory_usage = statistics_.total_memory_usage.load();
    stats.max_memory_usage = statistics_.max_memory_usage.load();

    return stats;
}

template <typename Key, typename Value>
double LRUCache<Key, Value>::getHitRatio() const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return statistics_.getHitRatio();
}

template <typename Key, typename Value>
void LRUCache<Key, Value>::optimize() {
    std::unique_lock<std::shared_mutex> lock(mutex_);

    // Remove expired entries first
    cleanup();

    // Rebuild access order to ensure consistency
    access_order_.clear();
    access_iterators_.clear();

    // Sort entries by last accessed time (most recent first)
    std::vector<std::pair<Key, std::shared_ptr<CacheEntryType>>> sorted_entries;
    for (const auto& pair : cache_) {
        sorted_entries.push_back(pair);
    }

    std::sort(sorted_entries.begin(), sorted_entries.end(),
              [](const auto& a, const auto& b) {
                  return a.second->last_accessed > b.second->last_accessed;
              });

    // Rebuild access order
    for (const auto& pair : sorted_entries) {
        access_order_.push_back(pair.first);
        access_iterators_[pair.first] = std::prev(access_order_.end());
    }
}

template <typename Key, typename Value>
void LRUCache<Key, Value>::preload(
    const std::function<std::unordered_map<Key, Value>()>& loader) {
    auto items = loader();
    putBatch(items);
}

// **Template instantiations for common types**
template class LRUCache<QString, std::shared_ptr<QWidget>>;
template class LRUCache<QString, QString>;
template class LRUCache<QString, QVariant>;
template class LRUCache<QString, QByteArray>;
template class LRUCache<QString, QJsonObject>;

// **Template method implementations**
template <typename Key, typename Value>
void LRUCache<Key, Value>::cleanup() {
    std::unique_lock<std::shared_mutex> lock(mutex_);

    std::vector<Key> expired_keys;

    // Find all expired entries
    for (const auto& pair : cache_) {
        if (pair.second->isExpired()) {
            expired_keys.push_back(pair.first);
        }
    }

    // Remove expired entries
    for (const auto& key : expired_keys) {
        auto it = cache_.find(key);
        if (it != cache_.end()) {
            statistics_.total_memory_usage.fetch_sub(
                it->second->memory_size.load());
            statistics_.evictions.fetch_add(1);
            cache_.erase(it);

            // Remove from access order
            auto access_it = access_iterators_.find(key);
            if (access_it != access_iterators_.end()) {
                access_order_.erase(access_it->second);
                access_iterators_.erase(access_it);
            }
        }
    }
}

template <typename Key, typename Value>
size_t LRUCache<Key, Value>::memoryUsage() const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return statistics_.total_memory_usage.load();
}

template <typename Key, typename Value>
size_t LRUCache<Key, Value>::size() const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return cache_.size();
}

template <typename Key, typename Value>
void LRUCache<Key, Value>::evictExpired() {
    std::vector<Key> expired_keys;

    // Find all expired entries
    for (const auto& pair : cache_) {
        if (pair.second->isExpired()) {
            expired_keys.push_back(pair.first);
        }
    }

    // Remove expired entries
    for (const auto& key : expired_keys) {
        auto it = cache_.find(key);
        if (it != cache_.end()) {
            statistics_.total_memory_usage.fetch_sub(
                it->second->memory_size.load());
            statistics_.evictions.fetch_add(1);
            cache_.erase(it);

            // Remove from access order
            auto access_it = access_iterators_.find(key);
            if (access_it != access_iterators_.end()) {
                access_order_.erase(access_it->second);
                access_iterators_.erase(access_it);
            }
        }
    }
}

template <typename Key, typename Value>
void LRUCache<Key, Value>::evictLFU() {
    if (cache_.empty())
        return;

    // Find entry with lowest access count
    auto min_it = cache_.begin();
    size_t min_access_count = min_it->second->access_count.load();

    for (auto it = cache_.begin(); it != cache_.end(); ++it) {
        size_t access_count = it->second->access_count.load();
        if (access_count < min_access_count) {
            min_access_count = access_count;
            min_it = it;
        }
    }

    // Remove the least frequently used entry
    Key key_to_evict = min_it->first;
    statistics_.total_memory_usage.fetch_sub(
        min_it->second->memory_size.load());
    statistics_.evictions.fetch_add(1);
    cache_.erase(min_it);

    // Remove from access order
    auto access_it = access_iterators_.find(key_to_evict);
    if (access_it != access_iterators_.end()) {
        access_order_.erase(access_it->second);
        access_iterators_.erase(access_it);
    }
}

// Note: All template methods are instantiated via the "template class"
// declarations above

}  // namespace DeclarativeUI::Core
