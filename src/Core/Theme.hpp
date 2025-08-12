// Core/Theme.hpp
#pragma once

#include <QColor>
#include <QFont>
#include <QPalette>
#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include <QApplication>
#include <unordered_map>
#include <memory>
#include <concepts>
#include <string_view>
#include <functional>

#include "Concepts.hpp"

namespace DeclarativeUI::Core::Theme {

// **Color palette with semantic naming**
struct ColorPalette {
    // **Primary colors**
    QColor primary{QRgb(0x2196F3)};           // Blue
    QColor primary_variant{QRgb(0x1976D2)};   // Dark Blue
    QColor secondary{QRgb(0xFF9800)};         // Orange
    QColor secondary_variant{QRgb(0xF57C00)}; // Dark Orange

    // **Surface colors**
    QColor background{QRgb(0xFAFAFA)};        // Light Gray
    QColor surface{QRgb(0xFFFFFF)};           // White
    QColor surface_variant{QRgb(0xF5F5F5)};   // Very Light Gray

    // **Content colors**
    QColor on_primary{QRgb(0xFFFFFF)};        // White
    QColor on_secondary{QRgb(0x000000)};      // Black
    QColor on_background{QRgb(0x212121)};     // Dark Gray
    QColor on_surface{QRgb(0x212121)};        // Dark Gray
    QColor on_surface_variant{QRgb(0x757575)}; // Medium Gray

    // **State colors**
    QColor error{QRgb(0xF44336)};             // Red
    QColor warning{QRgb(0xFF9800)};           // Orange
    QColor success{QRgb(0x4CAF50)};           // Green
    QColor info{QRgb(0x2196F3)};              // Blue

    // **Interactive colors**
    QColor hover{QRgb(0x1976D2)};             // Dark Blue
    QColor pressed{QRgb(0x0D47A1)};           // Very Dark Blue
    QColor focused{QRgb(0x2196F3)};           // Blue
    QColor disabled{QRgb(0xBDBDBD)};          // Light Gray

    // **Border and outline colors**
    QColor outline{QRgb(0xE0E0E0)};           // Light Gray
    QColor outline_variant{QRgb(0xBDBDBD)};   // Medium Light Gray
    QColor divider{QRgb(0xE0E0E0)};           // Light Gray

    // **Shadow colors**
    QColor shadow{QRgb(0x000000)};            // Black
    QColor shadow_light{QRgb(0x00000040)};    // Transparent Black
};

// **Typography configuration**
struct Typography {
    // **Font families**
    QString primary_font{"Segoe UI"};
    QString secondary_font{"Arial"};
    QString monospace_font{"Consolas"};

    // **Font sizes (in points)**
    struct FontSizes {
        int display_large{57};
        int display_medium{45};
        int display_small{36};
        int headline_large{32};
        int headline_medium{28};
        int headline_small{24};
        int title_large{22};
        int title_medium{16};
        int title_small{14};
        int label_large{14};
        int label_medium{12};
        int label_small{11};
        int body_large{16};
        int body_medium{14};
        int body_small{12};
    } sizes;

    // **Font weights**
    struct FontWeights {
        int thin{100};
        int light{300};
        int regular{400};
        int medium{500};
        int semi_bold{600};
        int bold{700};
        int extra_bold{800};
        int black{900};
    } weights;

    // **Line heights (multipliers)**
    struct LineHeights {
        double tight{1.2};
        double normal{1.4};
        double relaxed{1.6};
        double loose{1.8};
    } line_heights;
};

// **Spacing system**
struct Spacing {
    // **Base unit (in pixels)**
    int base_unit{8};

    // **Spacing scale**
    int xs{base_unit / 2};      // 4px
    int sm{base_unit};          // 8px
    int md{base_unit * 2};      // 16px
    int lg{base_unit * 3};      // 24px
    int xl{base_unit * 4};      // 32px
    int xxl{base_unit * 6};     // 48px
    int xxxl{base_unit * 8};    // 64px

    // **Component-specific spacing**
    struct ComponentSpacing {
        int button_padding{12};
        int input_padding{16};
        int card_padding{16};
        int dialog_padding{24};
        int section_margin{32};
    } components;
};

// **Border radius system**
struct BorderRadius {
    int none{0};
    int xs{2};
    int sm{4};
    int md{8};
    int lg{12};
    int xl{16};
    int xxl{24};
    int full{9999};  // Fully rounded
};

// **Shadow system**
struct Shadows {
    QString none{"none"};
    QString xs{"0 1px 2px rgba(0, 0, 0, 0.05)"};
    QString sm{"0 1px 3px rgba(0, 0, 0, 0.1), 0 1px 2px rgba(0, 0, 0, 0.06)"};
    QString md{"0 4px 6px rgba(0, 0, 0, 0.1), 0 2px 4px rgba(0, 0, 0, 0.06)"};
    QString lg{"0 10px 15px rgba(0, 0, 0, 0.1), 0 4px 6px rgba(0, 0, 0, 0.05)"};
    QString xl{"0 20px 25px rgba(0, 0, 0, 0.1), 0 10px 10px rgba(0, 0, 0, 0.04)"};
    QString xxl{"0 25px 50px rgba(0, 0, 0, 0.25)"};
};

// **Animation configuration**
struct Animation {
    // **Duration (in milliseconds)**
    struct Duration {
        int instant{0};
        int fast{150};
        int normal{300};
        int slow{500};
        int slower{750};
        int slowest{1000};
    } duration;

    // **Easing curves**
    struct Easing {
        QString linear{"linear"};
        QString ease{"ease"};
        QString ease_in{"ease-in"};
        QString ease_out{"ease-out"};
        QString ease_in_out{"ease-in-out"};
        QString bounce{"cubic-bezier(0.68, -0.55, 0.265, 1.55)"};
    } easing;
};

// **Complete theme configuration**
struct ThemeConfig {
    QString name{"Default"};
    QString version{"1.0.0"};
    ColorPalette colors;
    Typography typography;
    Spacing spacing;
    BorderRadius border_radius;
    Shadows shadows;
    Animation animation;

    // **Theme metadata**
    bool is_dark_theme{false};
    QString description{"Default light theme"};
    QString author{"DeclarativeUI"};
};

// **Theme manager class**
class ThemeManager {
public:
    static ThemeManager& instance() {
        static ThemeManager instance;
        return instance;
    }

    // **Load theme from configuration**
    void loadTheme(const ThemeConfig& config) {
        current_theme_ = config;
        applyTheme();
        notifyThemeChanged();
    }

    // **Load theme from JSON**
    bool loadThemeFromJson(const QString& json_string) {
        QJsonParseError error;
        auto doc = QJsonDocument::fromJson(json_string.toUtf8(), &error);

        if (error.error != QJsonParseError::NoError) {
            return false;
        }

        return loadThemeFromJson(doc.object());
    }

    bool loadThemeFromJson(const QJsonObject& json) {
        ThemeConfig config;

        // Parse theme configuration from JSON
        config.name = json["name"].toString("Default");
        config.version = json["version"].toString("1.0.0");
        config.is_dark_theme = json["isDarkTheme"].toBool(false);
        config.description = json["description"].toString();
        config.author = json["author"].toString();

        // Parse colors
        if (auto colors = json["colors"].toObject(); !colors.isEmpty()) {
            parseColors(colors, config.colors);
        }

        // Parse typography
        if (auto typography = json["typography"].toObject(); !typography.isEmpty()) {
            parseTypography(typography, config.typography);
        }

        // Parse spacing
        if (auto spacing = json["spacing"].toObject(); !spacing.isEmpty()) {
            parseSpacing(spacing, config.spacing);
        }

        loadTheme(config);
        return true;
    }

    // **Export current theme to JSON**
    [[nodiscard]] QString exportThemeToJson() const {
        QJsonObject json;

        json["name"] = current_theme_.name;
        json["version"] = current_theme_.version;
        json["isDarkTheme"] = current_theme_.is_dark_theme;
        json["description"] = current_theme_.description;
        json["author"] = current_theme_.author;

        // Export colors
        json["colors"] = exportColors(current_theme_.colors);

        // Export typography
        json["typography"] = exportTypography(current_theme_.typography);

        // Export spacing
        json["spacing"] = exportSpacing(current_theme_.spacing);

        return QJsonDocument(json).toJson(QJsonDocument::Compact);
    }

    // **Get current theme**
    [[nodiscard]] const ThemeConfig& getCurrentTheme() const noexcept {
        return current_theme_;
    }

    // **Theme change notifications**
    template<typename F>
    requires Concepts::VoidCallback<F>
    void onThemeChanged(F&& callback) {
        theme_change_callbacks_.emplace_back(std::forward<F>(callback));
    }

    // **Predefined themes**
    void loadLightTheme() {
        ThemeConfig light_theme;
        light_theme.name = "Light";
        light_theme.is_dark_theme = false;
        light_theme.description = "Clean light theme";
        loadTheme(light_theme);
    }

    void loadDarkTheme() {
        ThemeConfig dark_theme;
        dark_theme.name = "Dark";
        dark_theme.is_dark_theme = true;
        dark_theme.description = "Modern dark theme";

        // Adjust colors for dark theme
        dark_theme.colors.background = QColor(QRgb(0x121212));
        dark_theme.colors.surface = QColor(QRgb(0x1E1E1E));
        dark_theme.colors.surface_variant = QColor(QRgb(0x2D2D2D));
        dark_theme.colors.on_background = QColor(QRgb(0xFFFFFF));
        dark_theme.colors.on_surface = QColor(QRgb(0xFFFFFF));
        dark_theme.colors.on_surface_variant = QColor(QRgb(0xBDBDBD));
        dark_theme.colors.outline = QColor(QRgb(0x424242));
        dark_theme.colors.outline_variant = QColor(QRgb(0x616161));
        dark_theme.colors.divider = QColor(QRgb(0x424242));

        loadTheme(dark_theme);
    }

    // **Utility methods**
    [[nodiscard]] QFont createFont(int size, int weight = 400) const {
        QFont font(current_theme_.typography.primary_font);
        font.setPointSize(size);
        font.setWeight(static_cast<QFont::Weight>(weight));
        return font;
    }

    [[nodiscard]] QString generateStyleSheet(const QString& component_name) const {
        // Generate component-specific stylesheet based on current theme
        // This is a simplified example - real implementation would be more comprehensive
        return QString(R"(
            %1 {
                background-color: %2;
                color: %3;
                border: 1px solid %4;
                border-radius: %5px;
                padding: %6px;
                font-family: %7;
                font-size: %8pt;
            }
            %1:hover {
                background-color: %9;
            }
            %1:pressed {
                background-color: %10;
            }
        )").arg(component_name)
           .arg(current_theme_.colors.surface.name())
           .arg(current_theme_.colors.on_surface.name())
           .arg(current_theme_.colors.outline.name())
           .arg(current_theme_.border_radius.md)
           .arg(current_theme_.spacing.md)
           .arg(current_theme_.typography.primary_font)
           .arg(current_theme_.typography.sizes.body_medium)
           .arg(current_theme_.colors.hover.name())
           .arg(current_theme_.colors.pressed.name());
    }

private:
    ThemeConfig current_theme_;
    std::vector<std::function<void()>> theme_change_callbacks_;

    void applyTheme() {
        // Apply theme to application
        if (auto* app = qobject_cast<QApplication*>(QApplication::instance())) {
            QPalette palette;

            // Set palette colors based on theme
            palette.setColor(QPalette::Window, current_theme_.colors.background);
            palette.setColor(QPalette::WindowText, current_theme_.colors.on_background);
            palette.setColor(QPalette::Base, current_theme_.colors.surface);
            palette.setColor(QPalette::AlternateBase, current_theme_.colors.surface_variant);
            palette.setColor(QPalette::Text, current_theme_.colors.on_surface);
            palette.setColor(QPalette::Button, current_theme_.colors.primary);
            palette.setColor(QPalette::ButtonText, current_theme_.colors.on_primary);
            palette.setColor(QPalette::Highlight, current_theme_.colors.primary);
            palette.setColor(QPalette::HighlightedText, current_theme_.colors.on_primary);

            app->setPalette(palette);

            // Set application font
            QFont app_font = createFont(current_theme_.typography.sizes.body_medium);
            app->setFont(app_font);
        }
    }

    void notifyThemeChanged() {
        for (const auto& callback : theme_change_callbacks_) {
            callback();
        }
    }

    void parseColors(const QJsonObject& json, ColorPalette& colors) {
        // Parse color values from JSON
        if (json.contains("primary")) colors.primary = QColor(json["primary"].toString());
        if (json.contains("secondary")) colors.secondary = QColor(json["secondary"].toString());
        if (json.contains("background")) colors.background = QColor(json["background"].toString());
        if (json.contains("surface")) colors.surface = QColor(json["surface"].toString());
        // ... parse other colors
    }

    void parseTypography(const QJsonObject& json, Typography& typography) {
        if (json.contains("primaryFont")) typography.primary_font = json["primaryFont"].toString();
        if (json.contains("secondaryFont")) typography.secondary_font = json["secondaryFont"].toString();
        // ... parse other typography settings
    }

    void parseSpacing(const QJsonObject& json, Spacing& spacing) {
        if (json.contains("baseUnit")) spacing.base_unit = json["baseUnit"].toInt();
        // ... parse other spacing settings
    }

    QJsonObject exportColors(const ColorPalette& colors) const {
        QJsonObject json;
        json["primary"] = colors.primary.name();
        json["secondary"] = colors.secondary.name();
        json["background"] = colors.background.name();
        json["surface"] = colors.surface.name();
        // ... export other colors
        return json;
    }

    QJsonObject exportTypography(const Typography& typography) const {
        QJsonObject json;
        json["primaryFont"] = typography.primary_font;
        json["secondaryFont"] = typography.secondary_font;
        // ... export other typography settings
        return json;
    }

    QJsonObject exportSpacing(const Spacing& spacing) const {
        QJsonObject json;
        json["baseUnit"] = spacing.base_unit;
        // ... export other spacing settings
        return json;
    }
};

// **Utility functions**
[[nodiscard]] inline ThemeManager& theme() {
    return ThemeManager::instance();
}

[[nodiscard]] inline const ThemeConfig& currentTheme() {
    return ThemeManager::instance().getCurrentTheme();
}

}  // namespace DeclarativeUI::Core::Theme

// **Accessibility framework**
namespace DeclarativeUI::Core::Accessibility {

// **Accessibility roles**
enum class AccessibilityRole {
    Button,
    CheckBox,
    ComboBox,
    Dialog,
    Group,
    Heading,
    Label,
    Link,
    List,
    ListItem,
    Menu,
    MenuItem,
    ProgressBar,
    RadioButton,
    ScrollBar,
    Slider,
    SpinBox,
    Tab,
    TabList,
    TabPanel,
    Table,
    TableCell,
    TableHeader,
    TextEdit,
    ToolBar,
    ToolTip,
    Tree,
    TreeItem,
    Window
};

// **Accessibility states**
struct AccessibilityState {
    bool is_enabled{true};
    bool is_visible{true};
    bool is_focused{false};
    bool is_selected{false};
    bool is_checked{false};
    bool is_expanded{false};
    bool is_pressed{false};
    bool is_read_only{false};
    bool is_required{false};
    bool has_popup{false};
    bool is_modal{false};
};

// **Accessibility properties**
struct AccessibilityProperties {
    QString name;
    QString description;
    QString help_text;
    QString value;
    QString role_description;
    AccessibilityRole role{AccessibilityRole::Button};
    AccessibilityState state;

    // **Hierarchical relationships**
    QString parent_id;
    std::vector<QString> child_ids;
    QString label_id;
    QString described_by_id;

    // **Navigation**
    QString next_id;
    QString previous_id;
    int tab_index{0};

    // **Range properties (for sliders, progress bars, etc.)**
    std::optional<double> min_value;
    std::optional<double> max_value;
    std::optional<double> current_value;
    std::optional<double> step_value;
};

// **Accessibility manager**
class AccessibilityManager {
public:
    static AccessibilityManager& instance() {
        static AccessibilityManager instance;
        return instance;
    }

    // **Register component for accessibility**
    void registerComponent(QObject* component, const AccessibilityProperties& properties) {
        if (component) {
            component_properties_[component] = properties;
            updateComponentAccessibility(component);
        }
    }

    // **Update accessibility properties**
    void updateProperties(QObject* component, const AccessibilityProperties& properties) {
        if (component && component_properties_.contains(component)) {
            component_properties_[component] = properties;
            updateComponentAccessibility(component);
        }
    }

    // **Get accessibility properties**
    [[nodiscard]] std::optional<AccessibilityProperties> getProperties(QObject* component) const {
        if (auto it = component_properties_.find(component); it != component_properties_.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    // **Enable/disable accessibility features**
    void setAccessibilityEnabled(bool enabled) {
        accessibility_enabled_ = enabled;
        if (enabled) {
            enableAccessibilityFeatures();
        } else {
            disableAccessibilityFeatures();
        }
    }

    [[nodiscard]] bool isAccessibilityEnabled() const noexcept {
        return accessibility_enabled_;
    }

    // **High contrast mode**
    void setHighContrastMode(bool enabled) {
        high_contrast_enabled_ = enabled;
        if (enabled) {
            applyHighContrastTheme();
        } else {
            restoreNormalTheme();
        }
    }

    [[nodiscard]] bool isHighContrastEnabled() const noexcept {
        return high_contrast_enabled_;
    }

    // **Screen reader support**
    void announceToScreenReader(const QString& message, int priority = 0) {
        if (accessibility_enabled_) {
            // Implementation would integrate with platform screen readers
            // This is a placeholder for the concept
            screen_reader_announcements_.emplace_back(message, priority);
        }
    }

    // **Keyboard navigation**
    void setKeyboardNavigationEnabled(bool enabled) {
        keyboard_navigation_enabled_ = enabled;
    }

    [[nodiscard]] bool isKeyboardNavigationEnabled() const noexcept {
        return keyboard_navigation_enabled_;
    }

private:
    std::unordered_map<QObject*, AccessibilityProperties> component_properties_;
    bool accessibility_enabled_{true};
    bool high_contrast_enabled_{false};
    bool keyboard_navigation_enabled_{true};
    std::vector<std::pair<QString, int>> screen_reader_announcements_;

    void updateComponentAccessibility(QObject* component) {
        if (!accessibility_enabled_ || !component) return;

        const auto& props = component_properties_[component];

        // Set accessible name and description
        component->setProperty("accessibleName", props.name);
        component->setProperty("accessibleDescription", props.description);

        // Set role-specific properties
        if (auto* widget = qobject_cast<QWidget*>(component)) {
            widget->setToolTip(props.help_text);
            widget->setEnabled(props.state.is_enabled);
            widget->setVisible(props.state.is_visible);

            // Set focus policy for keyboard navigation
            if (keyboard_navigation_enabled_) {
                widget->setFocusPolicy(Qt::TabFocus);
                widget->setTabOrder(widget, nullptr); // Will be properly ordered by tab_index
            }
        }
    }

    void enableAccessibilityFeatures() {
        // Enable platform-specific accessibility features
        // This would integrate with Windows MSAA, macOS Accessibility API, etc.
    }

    void disableAccessibilityFeatures() {
        // Disable platform-specific accessibility features
    }

    void applyHighContrastTheme() {
        // Apply high contrast color scheme
        Theme::ThemeConfig high_contrast_theme;
        high_contrast_theme.name = "High Contrast";
        high_contrast_theme.colors.background = QColor(QRgb(0x000000));
        high_contrast_theme.colors.surface = QColor(QRgb(0x000000));
        high_contrast_theme.colors.on_background = QColor(QRgb(0xFFFFFF));
        high_contrast_theme.colors.on_surface = QColor(QRgb(0xFFFFFF));
        high_contrast_theme.colors.primary = QColor(QRgb(0xFFFF00));
        high_contrast_theme.colors.on_primary = QColor(QRgb(0x000000));

        Theme::ThemeManager::instance().loadTheme(high_contrast_theme);
    }

    void restoreNormalTheme() {
        // Restore previous theme
        Theme::ThemeManager::instance().loadLightTheme();
    }
};

// **Accessibility helper functions**
[[nodiscard]] inline AccessibilityManager& accessibility() {
    return AccessibilityManager::instance();
}

// **Accessibility attribute builder**
class AccessibilityBuilder {
public:
    AccessibilityBuilder& name(const QString& name) {
        properties_.name = name;
        return *this;
    }

    AccessibilityBuilder& description(const QString& description) {
        properties_.description = description;
        return *this;
    }

    AccessibilityBuilder& helpText(const QString& help_text) {
        properties_.help_text = help_text;
        return *this;
    }

    AccessibilityBuilder& role(AccessibilityRole role) {
        properties_.role = role;
        return *this;
    }

    AccessibilityBuilder& enabled(bool enabled) {
        properties_.state.is_enabled = enabled;
        return *this;
    }

    AccessibilityBuilder& required(bool required) {
        properties_.state.is_required = required;
        return *this;
    }

    AccessibilityBuilder& tabIndex(int index) {
        properties_.tab_index = index;
        return *this;
    }

    AccessibilityBuilder& range(double min_val, double max_val, double current_val, double step_val = 1.0) {
        properties_.min_value = min_val;
        properties_.max_value = max_val;
        properties_.current_value = current_val;
        properties_.step_value = step_val;
        return *this;
    }

    [[nodiscard]] AccessibilityProperties build() const {
        return properties_;
    }

    void applyTo(QObject* component) {
        AccessibilityManager::instance().registerComponent(component, properties_);
    }

private:
    AccessibilityProperties properties_;
};

// **Factory function**
[[nodiscard]] inline AccessibilityBuilder accessibilityFor() {
    return AccessibilityBuilder{};
}

}  // namespace DeclarativeUI::Core::Accessibility
