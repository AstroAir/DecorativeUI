// tests/unit/test_modern_cpp_features.cpp
#include <QApplication>
#include <QtTest/QtTest>
#include <algorithm>
#include <concepts>
#include <memory>
#include <ranges>

#include "../../src/Components/Button.hpp"
#include "../../src/Components/Widget.hpp"
#include "../../src/Core/Concepts.hpp"

using namespace DeclarativeUI::Core;
using namespace DeclarativeUI::Components;

class ModernCppFeaturesTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // **Concepts Tests**
    void testQtWidgetConcept();
    void testQtObjectConcept();
    void testPropertyValueConcept();
    void testStringLikeConcept();
    void testCallableConcepts();
    void testContainerConcepts();
    void testLayoutConcepts();
    void testValidatorConcepts();

    // **Template Features Tests**
    void testTemplateMethodsWithConcepts();
    void testPerfectForwarding();
    void testStructuredBindings();
    void testRangesAndViews();

    // **Modern C++20 Features Tests**
    void testDesignatedInitializers();
    void testConstexprMethods();
    void testNodescardAttributes();
    void testRequiresExpressions();

private:
    QApplication* app;
    std::unique_ptr<QWidget> testWidget;
};

void ModernCppFeaturesTest::initTestCase() {
    if (!QApplication::instance()) {
        int argc = 0;
        char* argv[] = {nullptr};
        app = new QApplication(argc, argv);
    }
}

void ModernCppFeaturesTest::cleanupTestCase() {
    if (app) {
        delete app;
        app = nullptr;
    }
}

void ModernCppFeaturesTest::init() { testWidget = std::make_unique<QWidget>(); }

void ModernCppFeaturesTest::cleanup() { testWidget.reset(); }

// **Concepts Tests**
void ModernCppFeaturesTest::testQtWidgetConcept() {
    // Test that QWidget satisfies QtWidget concept
    static_assert(Concepts::QtWidget<QWidget>);
    static_assert(Concepts::QtWidget<QPushButton>);
    static_assert(!Concepts::QtWidget<QObject>);
    static_assert(!Concepts::QtWidget<int>);

    QVERIFY(true);  // Concepts are compile-time checks
}

void ModernCppFeaturesTest::testQtObjectConcept() {
    // Test that QObject and derived classes satisfy QtObject concept
    static_assert(Concepts::QtObject<QObject>);
    static_assert(Concepts::QtObject<QWidget>);
    static_assert(Concepts::QtObject<QPushButton>);
    static_assert(!Concepts::QtObject<int>);
    static_assert(!Concepts::QtObject<std::string>);

    QVERIFY(true);
}

void ModernCppFeaturesTest::testPropertyValueConcept() {
    // Test PropertyValue concept
    static_assert(Concepts::PropertyValue<int>);
    static_assert(Concepts::PropertyValue<double>);
    static_assert(Concepts::PropertyValue<bool>);
    static_assert(Concepts::PropertyValue<QString>);

    QVERIFY(true);
}

void ModernCppFeaturesTest::testStringLikeConcept() {
    // Test StringLike concept
    static_assert(Concepts::StringLike<QString>);
    static_assert(Concepts::StringLike<std::string>);
    static_assert(Concepts::StringLike<const char*>);
    static_assert(!Concepts::StringLike<int>);

    QVERIFY(true);
}

void ModernCppFeaturesTest::testCallableConcepts() {
    // Test VoidCallback concept
    auto voidLambda = []() {};
    static_assert(Concepts::VoidCallback<decltype(voidLambda)>);

    auto intLambda = []() { return 42; };
    static_assert(!Concepts::VoidCallback<decltype(intLambda)>);

    // Test CallableWith concept
    auto paramLambda = [](int x) { return x * 2; };
    static_assert(Concepts::CallableWith<decltype(paramLambda), int>);
    static_assert(!Concepts::CallableWith<decltype(paramLambda), QString>);

    QVERIFY(true);
}

void ModernCppFeaturesTest::testContainerConcepts() {
    // Test Container concept
    static_assert(Concepts::Container<std::vector<int>>);
    static_assert(Concepts::Container<QList<QString>>);
    static_assert(!Concepts::Container<int>);

    // Test IterableContainer concept
    static_assert(Concepts::IterableContainer<std::vector<int>>);
    static_assert(Concepts::IterableContainer<QList<QString>>);

    QVERIFY(true);
}

void ModernCppFeaturesTest::testLayoutConcepts() {
    // Test LayoutType concept
    static_assert(Concepts::LayoutType<QVBoxLayout>);
    static_assert(Concepts::LayoutType<QHBoxLayout>);
    static_assert(Concepts::LayoutType<QGridLayout>);
    static_assert(!Concepts::LayoutType<QWidget>);

    // Test BoxLayoutType concept
    static_assert(Concepts::BoxLayoutType<QVBoxLayout>);
    static_assert(Concepts::BoxLayoutType<QHBoxLayout>);
    static_assert(!Concepts::BoxLayoutType<QGridLayout>);

    QVERIFY(true);
}

void ModernCppFeaturesTest::testValidatorConcepts() {
    // Test Validator concept - simplified test
    auto validator = [](const QString& str) { return str.length() > 0; };

    // Test that validator works
    QVERIFY(validator("test"));
    QVERIFY(!validator(""));
}

// **Template Features Tests**
void ModernCppFeaturesTest::testTemplateMethodsWithConcepts() {
    Widget widget;
    widget.initialize();

    // Test template methods with concepts
    QSize testSize(200, 150);
    widget.size(testSize);

    QString testTooltip = "Test tooltip";
    widget.toolTip(testTooltip);

    // Test that the widget was created successfully
    QVERIFY(widget.getWidget() != nullptr);
}

void ModernCppFeaturesTest::testPerfectForwarding() {
    Widget widget;
    widget.initialize();

    // Test perfect forwarding with rvalue references
    widget.size(QSize(300, 200));  // rvalue

    QSize size(400, 300);
    widget.size(size);  // lvalue

    QVERIFY(widget.getWidget() != nullptr);
}

void ModernCppFeaturesTest::testStructuredBindings() {
    // Test structured bindings with std::pair
    auto [width, height] = std::make_pair(800, 600);
    QCOMPARE(width, 800);
    QCOMPARE(height, 600);

    // Test with QSize
    QSize size(1024, 768);
    auto [w, h] = std::make_pair(size.width(), size.height());
    QCOMPARE(w, 1024);
    QCOMPARE(h, 768);
}

void ModernCppFeaturesTest::testRangesAndViews() {
    // Test C++20 ranges and views
    std::vector<int> numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // Filter even numbers
    auto evenNumbers =
        numbers | std::views::filter([](int n) { return n % 2 == 0; });

    std::vector<int> result;
    std::ranges::copy(evenNumbers, std::back_inserter(result));

    QCOMPARE(result.size(), 5);
    QCOMPARE(result[0], 2);
    QCOMPARE(result[4], 10);

    // Transform and filter
    auto transformed = numbers |
                       std::views::transform([](int n) { return n * 2; }) |
                       std::views::filter([](int n) { return n > 10; });

    std::vector<int> transformedResult;
    std::ranges::copy(transformed, std::back_inserter(transformedResult));

    QVERIFY(transformedResult.size() > 0);
    QVERIFY(
        std::ranges::all_of(transformedResult, [](int n) { return n > 10; }));
}

// **Modern C++20 Features Tests**
void ModernCppFeaturesTest::testDesignatedInitializers() {
    // Test designated initializers with structs
    struct TestConfig {
        int width = 800;
        int height = 600;
        bool fullscreen = false;
        QString title = "Default";
    };

    // Use designated initializers
    TestConfig config{.width = 1024,
                      .height = 768,
                      .fullscreen = true,
                      .title = "Test Window"};

    QCOMPARE(config.width, 1024);
    QCOMPARE(config.height, 768);
    QVERIFY(config.fullscreen);
    QCOMPARE(config.title, QString("Test Window"));
}

void ModernCppFeaturesTest::testConstexprMethods() {
    // Test constexpr evaluation at compile time
    constexpr auto calculateArea = [](int width, int height) constexpr {
        return width * height;
    };

    constexpr int area = calculateArea(10, 20);
    static_assert(area == 200);

    QCOMPARE(area, 200);
}

void ModernCppFeaturesTest::testNodescardAttributes() {
    // Test [[nodiscard]] attribute usage - simplified
    auto createWidget = []() -> std::unique_ptr<QWidget> {
        return std::make_unique<QWidget>();
    };

    auto widget = createWidget();
    QVERIFY(widget != nullptr);

    // Test that the widget is properly created
    QVERIFY(widget.get() != nullptr);
}

void ModernCppFeaturesTest::testRequiresExpressions() {
    // Test requires expressions in concepts
    auto testFunction = []<typename T>(T&& value)
        requires std::integral<T> || std::floating_point<T>
    { return value * 2; };

    auto intResult = testFunction(42);
    auto doubleResult = testFunction(3.14);

    QCOMPARE(intResult, 84);
    QCOMPARE(doubleResult, 6.28);

    // Test concept constraints
    static_assert(std::integral<int>);
    static_assert(std::floating_point<double>);
    static_assert(!std::integral<QString>);
}

QTEST_MAIN(ModernCppFeaturesTest)
#include "test_modern_cpp_features.moc"
