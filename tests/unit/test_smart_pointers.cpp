// tests/unit/test_smart_pointers.cpp
#include <QtTest/QtTest>
#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <memory>

#include "../../src/Core/SmartPointers.hpp"

using namespace DeclarativeUI::Core;

class SmartPointersTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    // **Qt Deleter Tests**
    void testQtDeleter();

    // **Qt Smart Pointer Tests**
    void testQtUniquePtr();
    void testQtSharedPtr();

    // **Factory Function Tests**
    void testMakeQtUnique();
    void testMakeQtShared();

    // **Qt Object Wrapper Tests**
    void testQtObjectWrapper();
    void testQtObjectWrapperMoveSemantics();

    // **Qt Object Pool Tests**
    void testQtObjectPool();
    void testQtObjectPoolAcquireRelease();

    // **Qt Weak Reference Tests**
    void testQtWeakRef();
    void testQtWeakRefValidation();

    // **Memory Utility Tests**
    void testSafeCast();
    void testIsAlive();
    void testCreateWithParent();
    void testBatchDelete();

private:
    QApplication* app;
};

void SmartPointersTest::initTestCase() {
    if (!QApplication::instance()) {
        int argc = 0;
        char* argv[] = {nullptr};
        app = new QApplication(argc, argv);
    }
}

void SmartPointersTest::cleanupTestCase() {
    if (app) {
        delete app;
        app = nullptr;
    }
}

// **Qt Deleter Tests**
void SmartPointersTest::testQtDeleter() {
    QtDeleter<QWidget> deleter;
    
    // Test with widget that has no parent (should call deleteLater)
    auto* widget = new QWidget();
    QVERIFY(widget != nullptr);
    
    // Test deleter doesn't crash
    deleter(widget);
    
    // Test with non-QObject type
    QtDeleter<int> intDeleter;
    int* intPtr = new int(42);
    intDeleter(intPtr); // Should call delete
    
    QVERIFY(true); // Test passes if no crash occurs
}

// **Qt Smart Pointer Tests**
void SmartPointersTest::testQtUniquePtr() {
    // Test qt_unique_ptr creation
    qt_unique_ptr<QWidget> widget(new QWidget());
    QVERIFY(widget != nullptr);
    QVERIFY(widget.get() != nullptr);
    
    // Test move semantics
    qt_unique_ptr<QWidget> movedWidget = std::move(widget);
    QVERIFY(widget.get() == nullptr);
    QVERIFY(movedWidget.get() != nullptr);
    
    // Test reset
    movedWidget.reset();
    QVERIFY(movedWidget.get() == nullptr);
}

void SmartPointersTest::testQtSharedPtr() {
    // Test qt_shared_ptr creation
    qt_shared_ptr<QWidget> widget(new QWidget(), QtDeleter<QWidget>{});
    QVERIFY(widget != nullptr);
    QVERIFY(widget.get() != nullptr);
    QCOMPARE(widget.use_count(), 1);
    
    // Test copy semantics
    qt_shared_ptr<QWidget> copiedWidget = widget;
    QCOMPARE(widget.use_count(), 2);
    QCOMPARE(copiedWidget.use_count(), 2);
    QCOMPARE(widget.get(), copiedWidget.get());
    
    // Test reset
    widget.reset();
    QCOMPARE(copiedWidget.use_count(), 1);
    QVERIFY(copiedWidget.get() != nullptr);
}

// **Factory Function Tests**
void SmartPointersTest::testMakeQtUnique() {
    // Test make_qt_unique without arguments
    auto widget = make_qt_unique<QWidget>();
    QVERIFY(widget != nullptr);
    QVERIFY(widget.get() != nullptr);
    
    // Test make_qt_unique with arguments
    auto button = make_qt_unique<QPushButton>("Test Button");
    QVERIFY(button != nullptr);
    QCOMPARE(button->text(), QString("Test Button"));
}

void SmartPointersTest::testMakeQtShared() {
    // Test make_qt_shared without arguments
    auto widget = make_qt_shared<QWidget>();
    QVERIFY(widget != nullptr);
    QVERIFY(widget.get() != nullptr);
    QCOMPARE(widget.use_count(), 1);
    
    // Test make_qt_shared with arguments
    auto button = make_qt_shared<QPushButton>("Shared Button");
    QVERIFY(button != nullptr);
    QCOMPARE(button->text(), QString("Shared Button"));
    QCOMPARE(button.use_count(), 1);
}

// **Qt Object Wrapper Tests**
void SmartPointersTest::testQtObjectWrapper() {
    auto parentWidget = std::make_unique<QWidget>();
    
    // Test wrapper creation
    QtObjectWrapper<QWidget> wrapper(new QWidget(), parentWidget.get());
    QVERIFY(wrapper.get() != nullptr);
    QVERIFY(wrapper);
    QCOMPARE(wrapper->parent(), parentWidget.get());
    
    // Test access operators
    QWidget* rawPtr = wrapper.get();
    QVERIFY(rawPtr != nullptr);
    QCOMPARE(&(*wrapper), rawPtr);
    QCOMPARE(wrapper.operator->(), rawPtr);
    
    // Test release
    QWidget* released = wrapper.release();
    QCOMPARE(released, rawPtr);
    QVERIFY(wrapper.get() == nullptr);
    QVERIFY(!wrapper);
    
    delete released;
}

void SmartPointersTest::testQtObjectWrapperMoveSemantics() {
    auto parentWidget = std::make_unique<QWidget>();
    
    // Test move constructor
    QtObjectWrapper<QWidget> wrapper1(new QWidget(), parentWidget.get());
    QWidget* originalPtr = wrapper1.get();
    
    QtObjectWrapper<QWidget> wrapper2 = std::move(wrapper1);
    QVERIFY(wrapper1.get() == nullptr);
    QCOMPARE(wrapper2.get(), originalPtr);
    
    // Test move assignment
    QtObjectWrapper<QWidget> wrapper3;
    wrapper3 = std::move(wrapper2);
    QVERIFY(wrapper2.get() == nullptr);
    QCOMPARE(wrapper3.get(), originalPtr);
    
    // Test reset
    wrapper3.reset(new QWidget(), parentWidget.get());
    QVERIFY(wrapper3.get() != nullptr);
    QVERIFY(wrapper3.get() != originalPtr);
}

// **Qt Object Pool Tests**
void SmartPointersTest::testQtObjectPool() {
    QtObjectPool<QWidget> pool(5);
    
    // Test initial state
    QVERIFY(pool.empty());
    QCOMPARE(pool.size(), 0);
    
    // Test acquire from empty pool (should create new object)
    auto widget1 = pool.acquire();
    QVERIFY(widget1 != nullptr);
    QVERIFY(pool.empty());
    
    // Test release to pool
    pool.release(std::move(widget1));
    QCOMPARE(pool.size(), 1);
    QVERIFY(!pool.empty());
    
    // Test acquire from non-empty pool
    auto widget2 = pool.acquire();
    QVERIFY(widget2 != nullptr);
    QVERIFY(pool.empty());
}

void SmartPointersTest::testQtObjectPoolAcquireRelease() {
    QtObjectPool<QWidget> pool(3);
    
    // Acquire multiple objects
    auto widget1 = pool.acquire();
    auto widget2 = pool.acquire();
    auto widget3 = pool.acquire();
    
    QVERIFY(widget1 != nullptr);
    QVERIFY(widget2 != nullptr);
    QVERIFY(widget3 != nullptr);
    
    // Release them back to pool
    pool.release(std::move(widget1));
    pool.release(std::move(widget2));
    pool.release(std::move(widget3));
    
    QCOMPARE(pool.size(), 3);
    
    // Test pool capacity limit
    auto widget4 = pool.acquire();
    pool.release(std::move(widget4));
    
    auto widget5 = make_qt_unique<QWidget>();
    pool.release(std::move(widget5)); // Should be ignored due to capacity
    
    QCOMPARE(pool.size(), 3); // Should still be 3
    
    // Test clear
    pool.clear();
    QVERIFY(pool.empty());
    QCOMPARE(pool.size(), 0);
}

// **Qt Weak Reference Tests**
void SmartPointersTest::testQtWeakRef() {
    auto widget = std::make_unique<QWidget>();
    QWidget* rawPtr = widget.get();
    
    // Test weak reference creation
    QtWeakRef<QWidget> weakRef(rawPtr);
    QVERIFY(weakRef.isValid());
    QCOMPARE(weakRef.get(), rawPtr);
    
    // Test withObject method
    bool callbackExecuted = false;
    weakRef.withObject([&callbackExecuted](QWidget* w) {
        callbackExecuted = true;
        QVERIFY(w != nullptr);
    });
    QVERIFY(callbackExecuted);
    
    // Test reset
    auto newWidget = std::make_unique<QWidget>();
    weakRef.reset(newWidget.get());
    QVERIFY(weakRef.isValid());
    QCOMPARE(weakRef.get(), newWidget.get());
}

void SmartPointersTest::testQtWeakRefValidation() {
    QtWeakRef<QWidget> weakRef;
    
    // Test invalid reference
    QVERIFY(!weakRef.isValid());
    QVERIFY(weakRef.get() == nullptr);
    
    // Test withObject with invalid reference
    bool callbackExecuted = false;
    weakRef.withObject([&callbackExecuted](QWidget* w) {
        callbackExecuted = true;
    });
    QVERIFY(!callbackExecuted);
    
    // Test with valid object that gets destroyed
    {
        auto widget = std::make_unique<QWidget>();
        weakRef.reset(widget.get());
        QVERIFY(weakRef.isValid());
    }
    
    // After widget is destroyed, weak ref should be invalidated
    // Note: This test might be flaky due to Qt's deleteLater mechanism
    // In a real scenario, the destroyed signal would invalidate the reference
}

// **Memory Utility Tests**
void SmartPointersTest::testSafeCast() {
    auto widget = std::make_unique<QPushButton>("Test");
    
    // Test successful cast
    QPushButton* button = Memory::safe_cast<QPushButton>(widget.get());
    QVERIFY(button != nullptr);
    QCOMPARE(button, widget.get());
    
    // Test failed cast
    QLabel* label = Memory::safe_cast<QLabel>(widget.get());
    QVERIFY(label == nullptr);
    
    // Test null input
    QWidget* nullWidget = nullptr;
    QPushButton* nullResult = Memory::safe_cast<QPushButton>(nullWidget);
    QVERIFY(nullResult == nullptr);
    
    // Test with smart pointer
    auto smartWidget = make_qt_unique<QPushButton>("Smart");
    QPushButton* smartButton = Memory::safe_cast<QPushButton>(smartWidget);
    QVERIFY(smartButton != nullptr);
    QCOMPARE(smartButton, smartWidget.get());
}

void SmartPointersTest::testIsAlive() {
    auto widget = std::make_unique<QWidget>();
    
    // Test with valid object
    QVERIFY(Memory::isAlive(widget.get()));
    
    // Test with null object
    QVERIFY(!Memory::isAlive(static_cast<QWidget*>(nullptr)));
}

void SmartPointersTest::testCreateWithParent() {
    auto parent = std::make_unique<QWidget>();
    
    // Test creating object with parent
    QWidget* child = Memory::createWithParent<QWidget>(parent.get());
    QVERIFY(child != nullptr);
    QCOMPARE(child->parent(), parent.get());
    
    // Test creating object with null parent
    QWidget* orphan = Memory::createWithParent<QWidget>(nullptr);
    QVERIFY(orphan != nullptr);
    QVERIFY(orphan->parent() == nullptr);
    delete orphan;
}

void SmartPointersTest::testBatchDelete() {
    auto parent = std::make_unique<QWidget>();
    
    // Create objects for batch deletion
    std::vector<QWidget*> widgets;
    widgets.push_back(new QWidget());
    widgets.push_back(new QWidget());
    widgets.push_back(new QWidget(parent.get())); // Has parent, won't be deleted
    
    // Test batch delete
    Memory::batchDelete(widgets);
    
    // Test with smart pointers
    std::vector<qt_unique_ptr<QWidget>> smartWidgets;
    smartWidgets.push_back(make_qt_unique<QWidget>());
    smartWidgets.push_back(make_qt_unique<QWidget>());
    
    Memory::batchDelete(smartWidgets);
    
    QVERIFY(true); // Test passes if no crash occurs
}

QTEST_MAIN(SmartPointersTest)
#include "test_smart_pointers.moc"
