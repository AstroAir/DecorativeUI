#include <QtTest/QtTest>
#include <QApplication>
#include <QWidget>
#include <QJsonObject>
#include <memory>

#include "../../src/Core/CacheManager.hpp"

using namespace DeclarativeUI::Core;

class CacheManagerTest : public QObject {
    Q_OBJECT

private:
    std::unique_ptr<CacheManager> cache_manager;

private slots:
    void initTestCase() {
        // Create QApplication if it doesn't exist (needed for QWidget tests)
        if (!QApplication::instance()) {
            int argc = 0;
            char** argv = nullptr;
            new QApplication(argc, argv);
        }
    }

    void init() {
        cache_manager = std::make_unique<CacheManager>();
    }

    void cleanup() {
        cache_manager.reset();
    }

    // Test basic LRUCache functionality
    void testBasicLRUCacheOperations() {
        LRUCache<QString, QString> cache(10, 1); // 10 items, 1MB

        // Test put and get
        QVERIFY(cache.put("key1", "value1"));
        auto result = cache.get("key1");
        QVERIFY(result.has_value());
        QCOMPARE(result.value(), QString("value1"));

        // Test contains
        QVERIFY(cache.contains("key1"));
        QVERIFY(!cache.contains("nonexistent"));

        // Test size
        QCOMPARE(cache.size(), size_t(1));

        // Test remove
        QVERIFY(cache.remove("key1"));
        QVERIFY(!cache.contains("key1"));
        QCOMPARE(cache.size(), size_t(0));
    }

    // Test CacheManager widget caching
    void testWidgetCaching() {
        auto widget = std::make_shared<QWidget>();
        widget->setObjectName("TestWidget");

        // Cache the widget
        cache_manager->cacheWidget("test_widget", widget);

        // Retrieve the widget
        auto cached_widget = cache_manager->getCachedWidget("test_widget");
        QVERIFY(cached_widget != nullptr);
        QCOMPARE(cached_widget->objectName(), QString("TestWidget"));

        // Test cache miss
        auto missing_widget = cache_manager->getCachedWidget("nonexistent");
        QVERIFY(missing_widget == nullptr);
    }

    // Test CacheManager stylesheet caching
    void testStylesheetCaching() {
        QString stylesheet = "QWidget { background-color: red; }";

        // Cache the stylesheet
        cache_manager->cacheStylesheet("red_style", stylesheet);

        // Retrieve the stylesheet
        QString cached_stylesheet = cache_manager->getCachedStylesheet("red_style");
        QCOMPARE(cached_stylesheet, stylesheet);

        // Test cache miss
        QString missing_stylesheet = cache_manager->getCachedStylesheet("nonexistent");
        QVERIFY(missing_stylesheet.isEmpty());
    }

    // Test CacheManager property caching
    void testPropertyCaching() {
        QVariant property_value = 42;

        // Cache the property
        cache_manager->cacheProperty("test_prop", property_value);

        // Retrieve the property
        QVariant cached_property = cache_manager->getCachedProperty("test_prop");
        QCOMPARE(cached_property.toInt(), 42);

        // Test cache miss
        QVariant missing_property = cache_manager->getCachedProperty("nonexistent");
        QVERIFY(!missing_property.isValid());
    }

    // Test CacheManager JSON caching
    void testJSONCaching() {
        QJsonObject json_obj;
        json_obj["name"] = "test";
        json_obj["value"] = 123;

        // Cache the JSON
        cache_manager->cacheJSON("test_json", json_obj);

        // Retrieve the JSON
        QJsonObject cached_json = cache_manager->getCachedJSON("test_json");
        QCOMPARE(cached_json["name"].toString(), QString("test"));
        QCOMPARE(cached_json["value"].toInt(), 123);

        // Test cache miss
        QJsonObject missing_json = cache_manager->getCachedJSON("nonexistent");
        QVERIFY(missing_json.isEmpty());
    }

    // Test cache invalidation
    void testCacheInvalidation() {
        // Add some data to caches
        cache_manager->cacheStylesheet("style1", "color: red;");
        cache_manager->cacheProperty("prop1", 100);

        // Verify data is cached
        QVERIFY(!cache_manager->getCachedStylesheet("style1").isEmpty());
        QVERIFY(cache_manager->getCachedProperty("prop1").isValid());

        // Invalidate all caches
        cache_manager->invalidateAll();

        // Verify data is cleared
        QVERIFY(cache_manager->getCachedStylesheet("style1").isEmpty());
        QVERIFY(!cache_manager->getCachedProperty("prop1").isValid());
    }

    // Test cache statistics
    void testCacheStatistics() {
        // Add some data and access it to generate statistics
        cache_manager->cacheStylesheet("style1", "color: red;");
        cache_manager->getCachedStylesheet("style1"); // Hit
        cache_manager->getCachedStylesheet("nonexistent"); // Miss

        // Get overall statistics
        QJsonObject stats = cache_manager->getCacheStatistics();
        QVERIFY(stats.contains("total_memory_usage"));
        QVERIFY(stats.contains("caches"));

        // Get specific cache statistics
        QJsonObject stylesheet_stats = cache_manager->getCacheStatistics("stylesheets");
        QVERIFY(stylesheet_stats.contains("size"));
        QVERIFY(stylesheet_stats.contains("memory_usage"));
    }

    // Test eviction policies
    void testEvictionPolicies() {
        LRUCache<QString, QString> cache(2, 1); // Small cache to trigger eviction

        // Set LRU policy
        cache.setEvictionPolicy(EvictionPolicy::LRU);

        // Fill cache beyond capacity
        cache.put("key1", "value1");
        cache.put("key2", "value2");
        cache.put("key3", "value3"); // Should evict key1

        QVERIFY(!cache.contains("key1")); // Should be evicted
        QVERIFY(cache.contains("key2"));
        QVERIFY(cache.contains("key3"));
    }

    // Test batch operations
    void testBatchOperations() {
        LRUCache<QString, QString> cache(10, 1);

        // Test batch put
        std::unordered_map<QString, QString> batch_data = {
            {"key1", "value1"},
            {"key2", "value2"},
            {"key3", "value3"}
        };
        cache.putBatch(batch_data);

        // Test batch get
        std::vector<QString> keys = {"key1", "key2", "key3", "nonexistent"};
        auto results = cache.getBatch(keys);

        QCOMPARE(results.size(), size_t(3)); // Should only return existing keys
        QCOMPARE(results["key1"], QString("value1"));
        QCOMPARE(results["key2"], QString("value2"));
        QCOMPARE(results["key3"], QString("value3"));
        QVERIFY(results.find("nonexistent") == results.end());
    }
};

QTEST_MAIN(CacheManagerTest)
#include "CacheManagerTest.moc"
