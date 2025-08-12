// tests/unit/test_theme_system.cpp
#include <QtTest/QtTest>
#include <QApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <memory>

#include "../../src/Core/Theme.hpp"

using namespace DeclarativeUI::Core::Theme;

class ThemeSystemTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // **Theme Configuration Tests**
    void testThemeConfigCreation();
    void testColorPalette();
    void testTypography();
    void testSpacing();
    void testBorderRadius();
    void testShadows();
    void testAnimation();

    // **Theme Manager Tests**
    void testThemeManagerSingleton();
    void testLoadLightTheme();
    void testLoadDarkTheme();
    void testCustomTheme();
    void testThemeChangeNotification();

    // **JSON Import/Export Tests**
    void testThemeExportToJson();
    void testThemeImportFromJson();
    void testInvalidJsonHandling();

    // **Theme Application Tests**
    void testStyleSheetGeneration();
    void testFontCreation();
    void testThemeApplication();

    // **Utility Tests**
    void testThemeUtilityFunctions();

private:
    QApplication* app;
    ThemeManager* themeManager;
};

void ThemeSystemTest::initTestCase() {
    if (!QApplication::instance()) {
        int argc = 0;
        char* argv[] = {nullptr};
        app = new QApplication(argc, argv);
    }
    themeManager = &ThemeManager::instance();
}

void ThemeSystemTest::cleanupTestCase() {
    // Don't delete the QApplication to avoid crashes with singleton cleanup
    // The QApplication will be cleaned up when the process exits
    // This prevents crashes when ThemeManager callbacks reference Qt objects
    app = nullptr;
}

void ThemeSystemTest::init() {
    // Reset to default theme before each test
    themeManager->loadLightTheme();
}

void ThemeSystemTest::cleanup() {
    // Cleanup after each test
}

// **Theme Configuration Tests**
void ThemeSystemTest::testThemeConfigCreation() {
    ThemeConfig config;

    // Test default values
    QCOMPARE(config.name, QString("Default"));
    QCOMPARE(config.version, QString("1.0.0"));
    QVERIFY(!config.is_dark_theme);

    // Test color palette defaults
    QCOMPARE(config.colors.primary, QColor(0x2196F3));
    QCOMPARE(config.colors.secondary, QColor(0xFF9800));
    QCOMPARE(config.colors.background, QColor(0xFAFAFA));
    QCOMPARE(config.colors.surface, QColor(0xFFFFFF));
}

void ThemeSystemTest::testColorPalette() {
    ColorPalette palette;

    // Test primary colors
    QVERIFY(palette.primary.isValid());
    QVERIFY(palette.primary_variant.isValid());
    QVERIFY(palette.secondary.isValid());
    QVERIFY(palette.secondary_variant.isValid());

    // Test surface colors
    QVERIFY(palette.background.isValid());
    QVERIFY(palette.surface.isValid());
    QVERIFY(palette.surface_variant.isValid());

    // Test content colors
    QVERIFY(palette.on_primary.isValid());
    QVERIFY(palette.on_secondary.isValid());
    QVERIFY(palette.on_background.isValid());
    QVERIFY(palette.on_surface.isValid());

    // Test state colors
    QVERIFY(palette.error.isValid());
    QVERIFY(palette.warning.isValid());
    QVERIFY(palette.success.isValid());
    QVERIFY(palette.info.isValid());
}

void ThemeSystemTest::testTypography() {
    Typography typography;

    // Test font families
    QVERIFY(!typography.primary_font.isEmpty());
    QVERIFY(!typography.secondary_font.isEmpty());
    QVERIFY(!typography.monospace_font.isEmpty());

    // Test font sizes
    QVERIFY(typography.sizes.display_large > typography.sizes.display_medium);
    QVERIFY(typography.sizes.display_medium > typography.sizes.display_small);
    QVERIFY(typography.sizes.headline_large > typography.sizes.headline_medium);
    QVERIFY(typography.sizes.body_large > typography.sizes.body_small);

    // Test font weights
    QVERIFY(typography.weights.thin < typography.weights.light);
    QVERIFY(typography.weights.light < typography.weights.regular);
    QVERIFY(typography.weights.regular < typography.weights.bold);
    QVERIFY(typography.weights.bold < typography.weights.black);

    // Test line heights
    QVERIFY(typography.line_heights.tight < typography.line_heights.normal);
    QVERIFY(typography.line_heights.normal < typography.line_heights.relaxed);
    QVERIFY(typography.line_heights.relaxed < typography.line_heights.loose);
}

void ThemeSystemTest::testSpacing() {
    Spacing spacing;

    // Test base unit
    QVERIFY(spacing.base_unit > 0);

    // Test spacing scale
    QVERIFY(spacing.xs < spacing.sm);
    QVERIFY(spacing.sm < spacing.md);
    QVERIFY(spacing.md < spacing.lg);
    QVERIFY(spacing.lg < spacing.xl);
    QVERIFY(spacing.xl < spacing.xxl);
    QVERIFY(spacing.xxl < spacing.xxxl);

    // Test component spacing
    QVERIFY(spacing.components.button_padding > 0);
    QVERIFY(spacing.components.input_padding > 0);
    QVERIFY(spacing.components.card_padding > 0);
    QVERIFY(spacing.components.dialog_padding > 0);
    QVERIFY(spacing.components.section_margin > 0);
}

void ThemeSystemTest::testBorderRadius() {
    BorderRadius borderRadius;

    // Test border radius scale
    QCOMPARE(borderRadius.none, 0);
    QVERIFY(borderRadius.xs < borderRadius.sm);
    QVERIFY(borderRadius.sm < borderRadius.md);
    QVERIFY(borderRadius.md < borderRadius.lg);
    QVERIFY(borderRadius.lg < borderRadius.xl);
    QVERIFY(borderRadius.xl < borderRadius.xxl);
    QVERIFY(borderRadius.xxl < borderRadius.full);
}

void ThemeSystemTest::testShadows() {
    Shadows shadows;

    // Test shadow definitions
    QVERIFY(!shadows.none.isEmpty());
    QVERIFY(!shadows.xs.isEmpty());
    QVERIFY(!shadows.sm.isEmpty());
    QVERIFY(!shadows.md.isEmpty());
    QVERIFY(!shadows.lg.isEmpty());
    QVERIFY(!shadows.xl.isEmpty());
    QVERIFY(!shadows.xxl.isEmpty());

    // Test that shadows contain expected CSS properties
    QVERIFY(shadows.sm.contains("rgba"));
    QVERIFY(shadows.md.contains("rgba"));
    QVERIFY(shadows.lg.contains("rgba"));
}

void ThemeSystemTest::testAnimation() {
    Animation animation;

    // Test duration scale
    QCOMPARE(animation.duration.instant, 0);
    QVERIFY(animation.duration.fast < animation.duration.normal);
    QVERIFY(animation.duration.normal < animation.duration.slow);
    QVERIFY(animation.duration.slow < animation.duration.slower);
    QVERIFY(animation.duration.slower < animation.duration.slowest);

    // Test easing curves
    QVERIFY(!animation.easing.linear.isEmpty());
    QVERIFY(!animation.easing.ease.isEmpty());
    QVERIFY(!animation.easing.ease_in.isEmpty());
    QVERIFY(!animation.easing.ease_out.isEmpty());
    QVERIFY(!animation.easing.ease_in_out.isEmpty());
    QVERIFY(!animation.easing.bounce.isEmpty());
}

// **Theme Manager Tests**
void ThemeSystemTest::testThemeManagerSingleton() {
    ThemeManager& manager1 = ThemeManager::instance();
    ThemeManager& manager2 = ThemeManager::instance();

    // Test that both references point to the same instance
    QCOMPARE(&manager1, &manager2);
}

void ThemeSystemTest::testLoadLightTheme() {
    themeManager->loadLightTheme();

    const auto& theme = themeManager->getCurrentTheme();
    QCOMPARE(theme.name, QString("Light"));
    QVERIFY(!theme.is_dark_theme);
    QCOMPARE(theme.description, QString("Clean light theme"));
}

void ThemeSystemTest::testLoadDarkTheme() {
    themeManager->loadDarkTheme();

    const auto& theme = themeManager->getCurrentTheme();
    QCOMPARE(theme.name, QString("Dark"));
    QVERIFY(theme.is_dark_theme);
    QCOMPARE(theme.description, QString("Modern dark theme"));

    // Test that dark theme has appropriate colors
    QCOMPARE(theme.colors.background, QColor(0x121212));
    QCOMPARE(theme.colors.surface, QColor(0x1E1E1E));
    QCOMPARE(theme.colors.on_background, QColor(0xFFFFFF));
}

void ThemeSystemTest::testCustomTheme() {
    ThemeConfig customTheme;
    customTheme.name = "Custom Test Theme";
    customTheme.version = "2.0.0";
    customTheme.is_dark_theme = true;
    customTheme.description = "Test theme for unit testing";
    customTheme.author = "Test Suite";

    // Customize colors
    customTheme.colors.primary = QColor(0xFF5722);
    customTheme.colors.secondary = QColor(0x9C27B0);

    themeManager->loadTheme(customTheme);

    const auto& loadedTheme = themeManager->getCurrentTheme();
    QCOMPARE(loadedTheme.name, QString("Custom Test Theme"));
    QCOMPARE(loadedTheme.version, QString("2.0.0"));
    QVERIFY(loadedTheme.is_dark_theme);
    QCOMPARE(loadedTheme.description, QString("Test theme for unit testing"));
    QCOMPARE(loadedTheme.author, QString("Test Suite"));
    QCOMPARE(loadedTheme.colors.primary, QColor(0xFF5722));
    QCOMPARE(loadedTheme.colors.secondary, QColor(0x9C27B0));
}

void ThemeSystemTest::testThemeChangeNotification() {
    bool notificationReceived = false;

    themeManager->onThemeChanged([&notificationReceived]() {
        notificationReceived = true;
    });

    themeManager->loadDarkTheme();

    QVERIFY(notificationReceived);
}

// **JSON Import/Export Tests**
void ThemeSystemTest::testThemeExportToJson() {
    themeManager->loadLightTheme();

    QString jsonString = themeManager->exportThemeToJson();
    QVERIFY(!jsonString.isEmpty());

    // Parse JSON to verify structure
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8(), &error);
    QCOMPARE(error.error, QJsonParseError::NoError);

    QJsonObject json = doc.object();
    QVERIFY(json.contains("name"));
    QVERIFY(json.contains("version"));
    QVERIFY(json.contains("isDarkTheme"));
    QVERIFY(json.contains("colors"));
    QVERIFY(json.contains("typography"));
    QVERIFY(json.contains("spacing"));

    QCOMPARE(json["name"].toString(), QString("Light"));
    QVERIFY(!json["isDarkTheme"].toBool());
}

void ThemeSystemTest::testThemeImportFromJson() {
    // Create test JSON
    QJsonObject json;
    json["name"] = "Test Import Theme";
    json["version"] = "1.5.0";
    json["isDarkTheme"] = true;
    json["description"] = "Imported test theme";
    json["author"] = "Test Importer";

    QJsonObject colors;
    colors["primary"] = "#FF5722";
    colors["secondary"] = "#9C27B0";
    colors["background"] = "#121212";
    colors["surface"] = "#1E1E1E";
    json["colors"] = colors;

    QJsonObject typography;
    typography["primaryFont"] = "Test Font";
    typography["secondaryFont"] = "Test Secondary Font";
    json["typography"] = typography;

    QJsonObject spacing;
    spacing["baseUnit"] = 10;
    json["spacing"] = spacing;

    QString jsonString = QJsonDocument(json).toJson(QJsonDocument::Compact);

    bool success = themeManager->loadThemeFromJson(jsonString);
    QVERIFY(success);

    const auto& theme = themeManager->getCurrentTheme();
    QCOMPARE(theme.name, QString("Test Import Theme"));
    QCOMPARE(theme.version, QString("1.5.0"));
    QVERIFY(theme.is_dark_theme);
    QCOMPARE(theme.description, QString("Imported test theme"));
    QCOMPARE(theme.author, QString("Test Importer"));
}

void ThemeSystemTest::testInvalidJsonHandling() {
    QString invalidJson = "{ invalid json }";

    bool success = themeManager->loadThemeFromJson(invalidJson);
    QVERIFY(!success);

    // Theme should remain unchanged
    const auto& theme = themeManager->getCurrentTheme();
    QVERIFY(!theme.name.isEmpty());
}

// **Theme Application Tests**
void ThemeSystemTest::testStyleSheetGeneration() {
    themeManager->loadLightTheme();

    QString stylesheet = themeManager->generateStyleSheet("TestComponent");
    QVERIFY(!stylesheet.isEmpty());
    QVERIFY(stylesheet.contains("TestComponent"));
    QVERIFY(stylesheet.contains("background-color"));
    QVERIFY(stylesheet.contains("color"));
    QVERIFY(stylesheet.contains("border"));
    QVERIFY(stylesheet.contains("font-family"));
}

void ThemeSystemTest::testFontCreation() {
    themeManager->loadLightTheme();

    QFont font = themeManager->createFont(16, 500);
    QCOMPARE(font.pointSize(), 16);
    QCOMPARE(font.weight(), 500);

    const auto& theme = themeManager->getCurrentTheme();
    QCOMPARE(font.family(), theme.typography.primary_font);
}

void ThemeSystemTest::testThemeApplication() {
    // Test that theme is applied to application
    themeManager->loadDarkTheme();

    if (auto* app = qobject_cast<QApplication*>(QApplication::instance())) {
        QPalette palette = app->palette();

        // Verify that palette colors have been set
        QVERIFY(palette.color(QPalette::Window).isValid());
        QVERIFY(palette.color(QPalette::WindowText).isValid());
        QVERIFY(palette.color(QPalette::Base).isValid());
        QVERIFY(palette.color(QPalette::Text).isValid());
    }
}

// **Utility Tests**
void ThemeSystemTest::testThemeUtilityFunctions() {
    // Test theme() utility function
    ThemeManager& manager = theme();
    QCOMPARE(&manager, &ThemeManager::instance());

    // Test currentTheme() utility function
    themeManager->loadLightTheme();
    const ThemeConfig& config = currentTheme();
    QCOMPARE(config.name, QString("Light"));
}

QTEST_MAIN(ThemeSystemTest)
#include "test_theme_system.moc"
