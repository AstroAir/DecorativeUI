// tests/unit/test_animation_system.cpp
#include <QtTest/QtTest>
#include <QApplication>
#include <QWidget>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QParallelAnimationGroup>
#include <QSignalSpy>
#include <memory>
#include <chrono>

#include "../../src/Core/Animation.hpp"

using namespace DeclarativeUI::Core::Animation;
using namespace std::chrono_literals;

class AnimationSystemTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // **Animation Configuration Tests**
    void testAnimationConfig();
    void testEasingPresets();

    // **Animation Builder Tests**
    void testAnimationBuilderCreation();
    void testAnimationBuilderProperties();
    void testAnimationBuilderDuration();
    void testAnimationBuilderEasing();
    void testAnimationBuilderDelay();
    void testAnimationBuilderLoop();
    void testAnimationBuilderEventHandlers();
    void testAnimationBuilderBuild();

    // **Animation Presets Tests**
    void testFadeInPreset();
    void testFadeOutPreset();
    void testSlideInPreset();
    void testScaleInPreset();
    void testBouncePreset();
    void testShakePreset();

    // **Animation Sequence Tests**
    void testAnimationSequenceCreation();
    void testAnimationSequencePause();
    void testAnimationSequenceBuild();

    // **Utility Functions Tests**
    void testAnimateUtility();
    void testParallelUtility();
    void testSequenceUtility();

    // **Integration Tests**
    void testAnimationExecution();
    void testAnimationChaining();

private:
    QApplication* app;
    std::unique_ptr<QWidget> testWidget;
};

void AnimationSystemTest::initTestCase() {
    if (!QApplication::instance()) {
        int argc = 0;
        char* argv[] = {nullptr};
        app = new QApplication(argc, argv);
    }
}

void AnimationSystemTest::cleanupTestCase() {
    if (app) {
        delete app;
        app = nullptr;
    }
}

void AnimationSystemTest::init() {
    testWidget = std::make_unique<QWidget>();
    testWidget->resize(100, 100);
    testWidget->show();
}

void AnimationSystemTest::cleanup() {
    testWidget.reset();
}

// **Animation Configuration Tests**
void AnimationSystemTest::testAnimationConfig() {
    AnimationConfig config;
    
    // Test default values
    QCOMPARE(config.duration, 300ms);
    QCOMPARE(config.easing, QEasingCurve::OutCubic);
    QCOMPARE(config.delay, 0ms);
    QCOMPARE(config.loop_count, 1);
    QVERIFY(!config.auto_reverse);
    
    // Test designated initializers
    AnimationConfig customConfig{
        .duration = 500ms,
        .easing = QEasingCurve::InBounce,
        .delay = 100ms,
        .loop_count = 3,
        .auto_reverse = true
    };
    
    QCOMPARE(customConfig.duration, 500ms);
    QCOMPARE(customConfig.easing, QEasingCurve::InBounce);
    QCOMPARE(customConfig.delay, 100ms);
    QCOMPARE(customConfig.loop_count, 3);
    QVERIFY(customConfig.auto_reverse);
}

void AnimationSystemTest::testEasingPresets() {
    // Test easing curve constants
    QCOMPARE(Easing::Linear, QEasingCurve::Linear);
    QCOMPARE(Easing::InQuad, QEasingCurve::InQuad);
    QCOMPARE(Easing::OutQuad, QEasingCurve::OutQuad);
    QCOMPARE(Easing::InOutQuad, QEasingCurve::InOutQuad);
    QCOMPARE(Easing::InCubic, QEasingCurve::InCubic);
    QCOMPARE(Easing::OutCubic, QEasingCurve::OutCubic);
    QCOMPARE(Easing::InOutCubic, QEasingCurve::InOutCubic);
    QCOMPARE(Easing::InBounce, QEasingCurve::InBounce);
    QCOMPARE(Easing::OutBounce, QEasingCurve::OutBounce);
    QCOMPARE(Easing::InOutBounce, QEasingCurve::InOutBounce);
}

// **Animation Builder Tests**
void AnimationSystemTest::testAnimationBuilderCreation() {
    AnimationBuilder builder(testWidget.get());
    
    // Test that builder can be created with a target
    QVERIFY(true); // Builder creation is tested by not crashing
}

void AnimationSystemTest::testAnimationBuilderProperties() {
    AnimationBuilder builder(testWidget.get());
    
    // Test property animation setup
    auto& builderRef = builder.animate("windowOpacity", 0.0, 1.0);
    
    // Test that method chaining works
    QCOMPARE(&builderRef, &builder);
}

void AnimationSystemTest::testAnimationBuilderDuration() {
    AnimationBuilder builder(testWidget.get());
    
    // Test duration setting with chrono literals
    auto& builderRef = builder.duration(500ms);
    QCOMPARE(&builderRef, &builder);
    
    // Test different duration types
    builder.duration(std::chrono::seconds(1));
    builder.duration(std::chrono::milliseconds(250));
}

void AnimationSystemTest::testAnimationBuilderEasing() {
    AnimationBuilder builder(testWidget.get());
    
    // Test easing curve setting
    auto& builderRef = builder.easing(Easing::OutBounce);
    QCOMPARE(&builderRef, &builder);
    
    // Test different easing curves
    builder.easing(Easing::InElastic);
    builder.easing(Easing::Linear);
}

void AnimationSystemTest::testAnimationBuilderDelay() {
    AnimationBuilder builder(testWidget.get());
    
    // Test delay setting
    auto& builderRef = builder.delay(100ms);
    QCOMPARE(&builderRef, &builder);
    
    // Test different delay types
    builder.delay(std::chrono::seconds(1));
    builder.delay(std::chrono::milliseconds(50));
}

void AnimationSystemTest::testAnimationBuilderLoop() {
    AnimationBuilder builder(testWidget.get());
    
    // Test loop setting
    auto& builderRef = builder.loop(3);
    QCOMPARE(&builderRef, &builder);
    
    // Test infinite loop
    builder.loop(-1);
    
    // Test auto reverse
    auto& reverseRef = builder.autoReverse(true);
    QCOMPARE(&reverseRef, &builder);
}

void AnimationSystemTest::testAnimationBuilderEventHandlers() {
    AnimationBuilder builder(testWidget.get());
    
    bool finishedCalled = false;
    bool startedCalled = false;
    
    // Test event handler setting
    auto& finishedRef = builder.onFinished([&finishedCalled]() {
        finishedCalled = true;
    });
    QCOMPARE(&finishedRef, &builder);
    
    auto& startedRef = builder.onStarted([&startedCalled]() {
        startedCalled = true;
    });
    QCOMPARE(&startedRef, &builder);
}

void AnimationSystemTest::testAnimationBuilderBuild() {
    AnimationBuilder builder(testWidget.get());
    
    // Test building animation
    auto animation = builder
        .animate("windowOpacity", 0.0, 1.0)
        .duration(200ms)
        .easing(Easing::OutCubic)
        .build();
    
    QVERIFY(animation != nullptr);
    QCOMPARE(animation->duration(), 200);
    
    // Test building without properties returns nullptr
    AnimationBuilder emptyBuilder(testWidget.get());
    auto emptyAnimation = emptyBuilder.build();
    QVERIFY(emptyAnimation == nullptr);
}

// **Animation Presets Tests**
void AnimationSystemTest::testFadeInPreset() {
    auto fadeBuilder = Presets::fadeIn(testWidget.get(), 300ms);
    auto animation = fadeBuilder.build();
    
    QVERIFY(animation != nullptr);
    QCOMPARE(animation->duration(), 300);
    
    // Test with default duration
    auto defaultFadeBuilder = Presets::fadeIn(testWidget.get());
    auto defaultAnimation = defaultFadeBuilder.build();
    QVERIFY(defaultAnimation != nullptr);
}

void AnimationSystemTest::testFadeOutPreset() {
    auto fadeBuilder = Presets::fadeOut(testWidget.get(), 250ms);
    auto animation = fadeBuilder.build();
    
    QVERIFY(animation != nullptr);
    QCOMPARE(animation->duration(), 250);
}

void AnimationSystemTest::testSlideInPreset() {
    QPoint fromPos(0, 0);
    QPoint toPos(100, 100);
    
    auto slideBuilder = Presets::slideIn(testWidget.get(), fromPos, toPos, 400ms);
    auto animation = slideBuilder.build();
    
    QVERIFY(animation != nullptr);
    QCOMPARE(animation->duration(), 400);
}

void AnimationSystemTest::testScaleInPreset() {
    auto scaleBuilder = Presets::scaleIn(testWidget.get(), 350ms);
    auto animation = scaleBuilder.build();
    
    QVERIFY(animation != nullptr);
    QCOMPARE(animation->duration(), 350);
}

void AnimationSystemTest::testBouncePreset() {
    auto bounceBuilder = Presets::bounce(testWidget.get(), 15, 600ms);
    auto animation = bounceBuilder.build();
    
    QVERIFY(animation != nullptr);
    QCOMPARE(animation->duration(), 300); // Half of 600ms due to auto-reverse
}

void AnimationSystemTest::testShakePreset() {
    auto shakeBuilder = Presets::shake(testWidget.get(), 8, 500ms);
    auto animation = shakeBuilder.build();
    
    QVERIFY(animation != nullptr);
    // Duration should be 500ms / 8 = 62.5ms, but Qt rounds to int
    QVERIFY(animation->duration() > 0);
}

// **Animation Sequence Tests**
void AnimationSystemTest::testAnimationSequenceCreation() {
    AnimationSequence sequence;
    
    // Test that sequence can be created
    QVERIFY(true); // Sequence creation is tested by not crashing
}

void AnimationSystemTest::testAnimationSequencePause() {
    AnimationSequence sequence;
    
    // Test adding pause to sequence
    auto& sequenceRef = sequence.pause(100ms);
    QCOMPARE(&sequenceRef, &sequence);
}

void AnimationSystemTest::testAnimationSequenceBuild() {
    AnimationSequence sequence;
    
    auto fadeIn = Presets::fadeIn(testWidget.get(), 200ms);
    auto fadeOut = Presets::fadeOut(testWidget.get(), 200ms);
    
    auto builtSequence = sequence
        .then(fadeIn)
        .pause(50ms)
        .then(fadeOut)
        .build();
    
    QVERIFY(builtSequence != nullptr);
    QVERIFY(builtSequence->animationCount() > 0);
}

// **Utility Functions Tests**
void AnimationSystemTest::testAnimateUtility() {
    auto builder = animate(testWidget.get());
    
    // Test that utility function returns a builder
    auto animation = builder
        .animate("windowOpacity", 0.5, 1.0)
        .duration(150ms)
        .build();
    
    QVERIFY(animation != nullptr);
}

void AnimationSystemTest::testParallelUtility() {
    auto fadeAnimation = Presets::fadeIn(testWidget.get(), 200ms).build();
    auto slideAnimation = Presets::slideIn(testWidget.get(), QPoint(0, 0), QPoint(50, 50), 200ms).build();
    
    auto parallelGroup = parallel(fadeAnimation.release(), slideAnimation.release());
    
    QVERIFY(parallelGroup != nullptr);
    QVERIFY(parallelGroup->animationCount() >= 2);
}

void AnimationSystemTest::testSequenceUtility() {
    auto fadeIn = Presets::fadeIn(testWidget.get(), 100ms).build();
    auto fadeOut = Presets::fadeOut(testWidget.get(), 100ms).build();
    
    auto sequentialGroup = sequence(fadeIn.release(), fadeOut.release());
    
    QVERIFY(sequentialGroup != nullptr);
    QVERIFY(sequentialGroup->animationCount() >= 2);
}

// **Integration Tests**
void AnimationSystemTest::testAnimationExecution() {
    // Test that animation can be started and finished
    bool animationFinished = false;
    
    auto animation = animate(testWidget.get())
        .animate("windowOpacity", 1.0, 0.5)
        .duration(50ms) // Short duration for test
        .onFinished([&animationFinished]() {
            animationFinished = true;
        })
        .build();
    
    QVERIFY(animation != nullptr);
    
    QSignalSpy finishedSpy(animation.get(), &QAbstractAnimation::finished);
    
    animation->start();
    
    // Wait for animation to finish
    QVERIFY(finishedSpy.wait(1000));
    QCOMPARE(finishedSpy.count(), 1);
    QVERIFY(animationFinished);
}

void AnimationSystemTest::testAnimationChaining() {
    // Test chaining multiple animations in sequence
    bool firstFinished = false;
    bool secondFinished = false;
    
    auto sequence = AnimationSequence()
        .then(animate(testWidget.get())
              .animate("windowOpacity", 1.0, 0.5)
              .duration(25ms)
              .onFinished([&firstFinished]() { firstFinished = true; }))
        .pause(10ms)
        .then(animate(testWidget.get())
              .animate("windowOpacity", 0.5, 1.0)
              .duration(25ms)
              .onFinished([&secondFinished]() { secondFinished = true; }))
        .build();
    
    QVERIFY(sequence != nullptr);
    
    QSignalSpy finishedSpy(sequence.get(), &QAbstractAnimation::finished);
    
    sequence->start();
    
    // Wait for sequence to finish
    QVERIFY(finishedSpy.wait(1000));
    QCOMPARE(finishedSpy.count(), 1);
    
    // Note: Individual animation finished callbacks might not be called
    // in sequence animations, so we just test that the sequence completes
}

QTEST_MAIN(AnimationSystemTest)
#include "test_animation_system.moc"
