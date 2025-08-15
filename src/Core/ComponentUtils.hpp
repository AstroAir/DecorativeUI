// Core/ComponentUtils.hpp
#pragma once

#include <QFormLayout>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsEffect>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLayout>
#include <QMetaProperty>
#include <QPropertyAnimation>
#include <QString>
#include <QTimer>
#include <QVBoxLayout>
#include <QVariant>
#include <QWidget>

#include <concepts>
#include <functional>
#include <memory>

#include "../Animation/AnimationEngine.hpp"
#include "Concepts.hpp"
#include "ErrorHandling.hpp"
#include "Theme.hpp"

namespace DeclarativeUI::Core::Utils {

// **Common property setters with error handling**
template <Concepts::QtObject T>
class PropertySetter {
public:
    explicit PropertySetter(T* target) : target_(target) {}

    // **Safe property setting with validation**
    template <typename ValueType>
    bool setProperty(const char* name, ValueType&& value) {
        if (!target_) {
            UI_LOG_ERROR(
                QString("Attempted to set property '%1' on null object")
                    .arg(name));
            return false;
        }

        UI_ERROR_SCOPE("PropertySetter",
                       QString("Setting property '%1'").arg(name));

        try {
            QVariant variant =
                QVariant::fromValue(std::forward<ValueType>(value));
            bool success = target_->setProperty(name, variant);

            if (!success) {
                UI_LOG_WARNING(
                    QString(
                        "Failed to set property '%1' on object of type '%2'")
                        .arg(name)
                        .arg(target_->metaObject()->className()));
            }

            return success;
        } catch (const std::exception& e) {
            UI_LOG_ERROR(QString("Exception while setting property '%1': %2")
                             .arg(name)
                             .arg(QString::fromStdString(e.what())));
            return false;
        }
    }

    // **Get property with type safety**
    template <typename ValueType>
    [[nodiscard]] std::optional<ValueType> getProperty(const char* name) const {
        if (!target_) {
            return std::nullopt;
        }

        QVariant variant = target_->property(name);
        if (!variant.isValid()) {
            return std::nullopt;
        }

        if (variant.canConvert<ValueType>()) {
            return variant.value<ValueType>();
        }

        return std::nullopt;
    }

    // **Check if property exists**
    [[nodiscard]] bool hasProperty(const char* name) const {
        if (!target_)
            return false;

        const QMetaObject* metaObj = target_->metaObject();
        return metaObj->indexOfProperty(name) != -1;
    }

    // **List all properties**
    [[nodiscard]] std::vector<QString> listProperties() const {
        std::vector<QString> properties;

        if (!target_)
            return properties;

        const QMetaObject* metaObj = target_->metaObject();
        for (int i = 0; i < metaObj->propertyCount(); ++i) {
            QMetaProperty prop = metaObj->property(i);
            properties.emplace_back(prop.name());
        }

        return properties;
    }

private:
    T* target_;
};

// **Layout management utilities**
namespace Layout {

// **Safe layout creation with error handling**
template <typename LayoutType, typename... Args>
    requires std::derived_from<LayoutType, QLayout>
[[nodiscard]] std::unique_ptr<LayoutType> createLayout(Args&&... args) {
    return UI_SAFE_EXECUTE(
               [&]() {
                   return std::make_unique<LayoutType>(
                       std::forward<Args>(args)...);
               },
               "Creating layout")
        .value_or(nullptr);
}

// **Apply layout to widget safely**
template <Concepts::QtWidget WidgetType, typename LayoutType>
    requires std::derived_from<LayoutType, QLayout>
bool applyLayout(WidgetType* widget, std::unique_ptr<LayoutType> layout) {
    if (!widget || !layout) {
        UI_LOG_ERROR("Cannot apply null layout to widget");
        return false;
    }

    UI_ERROR_SCOPE("LayoutUtils", "Applying layout to widget");

    try {
        widget->setLayout(layout.release());
        return true;
    } catch (const std::exception& e) {
        UI_LOG_ERROR(QString("Failed to apply layout: %1")
                         .arg(QString::fromStdString(e.what())));
        return false;
    }
}

// **Add widget to layout with position validation**
template <typename LayoutType, Concepts::QtWidget WidgetType>
bool addWidget(LayoutType* layout, WidgetType* widget, int position = -1) {
    if (!layout || !widget) {
        UI_LOG_ERROR("Cannot add null widget to layout");
        return false;
    }

    UI_ERROR_SCOPE("LayoutUtils", "Adding widget to layout");

    try {
        if constexpr (std::derived_from<LayoutType, QBoxLayout>) {
            if (position >= 0) {
                layout->insertWidget(position, widget);
            } else {
                layout->addWidget(widget);
            }
        } else {
            layout->addWidget(widget);
        }
        return true;
    } catch (const std::exception& e) {
        UI_LOG_ERROR(QString("Failed to add widget to layout: %1")
                         .arg(QString::fromStdString(e.what())));
        return false;
    }
}

// **Grid layout utilities**
template <Concepts::QtWidget WidgetType>
bool addWidgetToGrid(QGridLayout* layout, WidgetType* widget, int row,
                     int column, int rowSpan = 1, int columnSpan = 1,
                     Qt::Alignment alignment = Qt::Alignment{}) {
    if (!layout || !widget) {
        UI_LOG_ERROR("Cannot add null widget to grid layout");
        return false;
    }

    if (row < 0 || column < 0) {
        UI_LOG_ERROR(QString("Invalid grid position: row=%1, column=%2")
                         .arg(row)
                         .arg(column));
        return false;
    }

    UI_ERROR_SCOPE(
        "LayoutUtils",
        QString("Adding widget to grid at (%1,%2)").arg(row).arg(column));

    try {
        layout->addWidget(widget, row, column, rowSpan, columnSpan, alignment);
        return true;
    } catch (const std::exception& e) {
        UI_LOG_ERROR(QString("Failed to add widget to grid: %1")
                         .arg(QString::fromStdString(e.what())));
        return false;
    }
}

// **Form layout utilities**
template <Concepts::QtWidget LabelType, Concepts::QtWidget FieldType>
bool addFormRow(QFormLayout* layout, LabelType* label, FieldType* field) {
    if (!layout || !field) {
        UI_LOG_ERROR("Cannot add null field to form layout");
        return false;
    }

    UI_ERROR_SCOPE("LayoutUtils", "Adding row to form layout");

    try {
        if (label) {
            layout->addRow(label, field);
        } else {
            layout->addRow(field);
        }
        return true;
    } catch (const std::exception& e) {
        UI_LOG_ERROR(QString("Failed to add form row: %1")
                         .arg(QString::fromStdString(e.what())));
        return false;
    }
}

inline bool addFormRow(QFormLayout* layout, const QString& label,
                       QWidget* field) {
    if (!layout || !field) {
        UI_LOG_ERROR("Cannot add null field to form layout");
        return false;
    }

    UI_ERROR_SCOPE(
        "LayoutUtils",
        QString("Adding labeled row '%1' to form layout").arg(label));

    try {
        layout->addRow(label, field);
        return true;
    } catch (const std::exception& e) {
        UI_LOG_ERROR(QString("Failed to add labeled form row: %1")
                         .arg(QString::fromStdString(e.what())));
        return false;
    }
}

}  // namespace Layout

// **Styling utilities**
namespace Styling {

// **Apply theme-aware styling**
template <Concepts::QtWidget WidgetType>
void applyThemeStyle(WidgetType* widget, const QString& component_name = {}) {
    if (!widget) {
        UI_LOG_ERROR("Cannot apply theme to null widget");
        return;
    }

    UI_ERROR_SCOPE("StylingUtils",
                   QString("Applying theme to %1").arg(component_name));

    try {
        const auto& theme = Theme::currentTheme();
        QString styleSheet = Theme::ThemeManager::instance().generateStyleSheet(
            component_name.isEmpty() ? widget->metaObject()->className()
                                     : component_name);

        widget->setStyleSheet(styleSheet);

        // Apply theme-specific font
        QFont font = Theme::ThemeManager::instance().createFont(
            theme.typography.sizes.body_medium,
            theme.typography.weights.regular);
        widget->setFont(font);

    } catch (const std::exception& e) {
        UI_LOG_ERROR(QString("Failed to apply theme: %1")
                         .arg(QString::fromStdString(e.what())));
    }
}

// **Add drop shadow effect**
template <Concepts::QtWidget WidgetType>
void addDropShadow(WidgetType* widget,
                   const QColor& color = QColor(0, 0, 0, 80),
                   int blur_radius = 10,
                   const QPointF& offset = QPointF(2, 2)) {
    if (!widget) {
        UI_LOG_ERROR("Cannot add shadow to null widget");
        return;
    }

    UI_ERROR_SCOPE("StylingUtils", "Adding drop shadow effect");

    try {
        auto* shadow = new QGraphicsDropShadowEffect();
        shadow->setColor(color);
        shadow->setBlurRadius(blur_radius);
        shadow->setOffset(offset);
        widget->setGraphicsEffect(shadow);
    } catch (const std::exception& e) {
        UI_LOG_ERROR(QString("Failed to add drop shadow: %1")
                         .arg(QString::fromStdString(e.what())));
    }
}

// **Set border radius using stylesheet**
template <Concepts::QtWidget WidgetType>
void setBorderRadius(WidgetType* widget, int radius) {
    if (!widget) {
        UI_LOG_ERROR("Cannot set border radius on null widget");
        return;
    }

    UI_ERROR_SCOPE("StylingUtils",
                   QString("Setting border radius to %1px").arg(radius));

    try {
        QString currentStyle = widget->styleSheet();
        QString borderStyle = QString("border-radius: %1px;").arg(radius);

        if (currentStyle.isEmpty()) {
            widget->setStyleSheet(borderStyle);
        } else {
            widget->setStyleSheet(currentStyle + " " + borderStyle);
        }
    } catch (const std::exception& e) {
        UI_LOG_ERROR(QString("Failed to set border radius: %1")
                         .arg(QString::fromStdString(e.what())));
    }
}

}  // namespace Styling

// **Animation utilities**
namespace AnimationUtils {

// **Fade animation with error handling - Using new Animation system**
template <Concepts::QtWidget WidgetType>
[[nodiscard]] std::shared_ptr<Animation::Animation> createFadeAnimation(
    WidgetType* widget, double from_opacity, double to_opacity,
    int duration_ms = 300) {
    if (!widget) {
        UI_LOG_ERROR("Cannot create fade animation for null widget");
        return nullptr;
    }

    return UI_SAFE_EXECUTE(
               [&]() -> std::shared_ptr<Animation::Animation> {
                   auto& engine = Animation::AnimationEngine::instance();
                   return engine.animateProperty(
                       widget, "windowOpacity", QVariant(from_opacity),
                       QVariant(to_opacity), duration_ms,
                       Animation::EasingType::CubicOut);
               },
               "Creating fade animation")
        .value_or(nullptr);
}

// **Slide animation - Using new Animation system**
template <Concepts::QtWidget WidgetType>
[[nodiscard]] std::shared_ptr<Animation::Animation> createSlideAnimation(
    WidgetType* widget, const QPoint& from_pos, const QPoint& to_pos,
    int duration_ms = 400) {
    if (!widget) {
        UI_LOG_ERROR("Cannot create slide animation for null widget");
        return nullptr;
    }

    return UI_SAFE_EXECUTE(
               [&]() -> std::shared_ptr<Animation::Animation> {
                   auto& engine = Animation::AnimationEngine::instance();
                   return engine.animateProperty(
                       widget, "pos", QVariant(from_pos), QVariant(to_pos),
                       duration_ms, Animation::EasingType::QuartOut);
               },
               "Creating slide animation")
        .value_or(nullptr);
}

}  // namespace AnimationUtils

// **Event handling utilities**
namespace Events {

// **Safe signal connection with error handling**
template <typename Sender, typename Signal, typename Receiver, typename Slot>
bool connectSafely(Sender* sender, Signal signal, Receiver* receiver, Slot slot,
                   Qt::ConnectionType type = Qt::AutoConnection) {
    if (!sender || !receiver) {
        UI_LOG_ERROR("Cannot connect signal with null sender or receiver");
        return false;
    }

    UI_ERROR_SCOPE("EventUtils", "Connecting signal");

    try {
        QMetaObject::Connection connection =
            QObject::connect(sender, signal, receiver, slot, type);
        return static_cast<bool>(connection);
    } catch (const std::exception& e) {
        UI_LOG_ERROR(QString("Failed to connect signal: %1")
                         .arg(QString::fromStdString(e.what())));
        return false;
    }
}

// **Delayed execution with error handling**
template <typename F>
    requires std::invocable<F>
void executeDelayed(int delay_ms, F&& func) {
    UI_ERROR_SCOPE(
        "EventUtils",
        QString("Scheduling delayed execution (%1ms)").arg(delay_ms));

    try {
        QTimer::singleShot(delay_ms, [func = std::forward<F>(func)]() {
            UI_SAFE_EXECUTE(func, "Delayed execution");
        });
    } catch (const std::exception& e) {
        UI_LOG_ERROR(QString("Failed to schedule delayed execution: %1")
                         .arg(QString::fromStdString(e.what())));
    }
}

}  // namespace Events

// **Validation utilities**
namespace Validation {

// **Validate widget state**
template <Concepts::QtWidget WidgetType>
[[nodiscard]] bool validateWidget(WidgetType* widget,
                                  const QString& widget_name = {}) {
    if (!widget) {
        UI_LOG_ERROR(QString("Widget '%1' is null").arg(widget_name));
        return false;
    }

    if (!widget->isValid()) {
        UI_LOG_ERROR(
            QString("Widget '%1' is in invalid state").arg(widget_name));
        return false;
    }

    return true;
}

// **Validate layout hierarchy**
[[nodiscard]] inline bool validateLayoutHierarchy(QWidget* root_widget) {
    if (!root_widget) {
        UI_LOG_ERROR("Cannot validate null widget hierarchy");
        return false;
    }

    UI_ERROR_SCOPE("ValidationUtils", "Validating layout hierarchy");

    try {
        // Check for circular references and orphaned widgets
        std::function<bool(QWidget*)> validateRecursive =
            [&](QWidget* widget) -> bool {
            if (!widget)
                return true;

            // Check if widget has valid parent-child relationships
            for (QObject* child : widget->children()) {
                if (auto* childWidget = qobject_cast<QWidget*>(child)) {
                    if (childWidget->parent() != widget) {
                        UI_LOG_WARNING(
                            QString("Widget parent-child relationship "
                                    "inconsistency detected"));
                        return false;
                    }
                    if (!validateRecursive(childWidget)) {
                        return false;
                    }
                }
            }
            return true;
        };

        return validateRecursive(root_widget);
    } catch (const std::exception& e) {
        UI_LOG_ERROR(QString("Exception during layout validation: %1")
                         .arg(QString::fromStdString(e.what())));
        return false;
    }
}

}  // namespace Validation

// **Factory functions for common utilities**
template <Concepts::QtObject T>
[[nodiscard]] PropertySetter<T> properties(T* target) {
    return PropertySetter<T>(target);
}

}  // namespace DeclarativeUI::Core::Utils
