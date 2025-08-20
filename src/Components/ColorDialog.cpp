// Components/ColorDialog.cpp
#include "ColorDialog.hpp"

namespace DeclarativeUI::Components {

// **Implementation**
ColorDialog::ColorDialog(QObject* parent)
    : UIElement(parent), color_dialog_widget_(nullptr) {}

ColorDialog& ColorDialog::currentColor(const QColor& color) {
    return static_cast<ColorDialog&>(setProperty("currentColor", color));
}

ColorDialog& ColorDialog::options(QColorDialog::ColorDialogOptions options) {
    return static_cast<ColorDialog&>(
        setProperty("options", static_cast<int>(options)));
}

ColorDialog& ColorDialog::windowTitle(const QString& title) {
    return static_cast<ColorDialog&>(setProperty("windowTitle", title));
}

ColorDialog& ColorDialog::onColorSelected(
    std::function<void(const QColor&)> handler) {
    color_selected_handler_ = std::move(handler);
    return *this;
}

ColorDialog& ColorDialog::onCurrentColorChanged(
    std::function<void(const QColor&)> handler) {
    current_color_changed_handler_ = std::move(handler);
    return *this;
}

void ColorDialog::initialize() {
    if (!color_dialog_widget_) {
        color_dialog_widget_ = new QColorDialog();
        setWidget(color_dialog_widget_);

        // Connect signals
        if (color_selected_handler_) {
            connect(color_dialog_widget_, &QColorDialog::colorSelected, this,
                    [this](const QColor& color) {
                        color_selected_handler_(color);
                    });
        }

        if (current_color_changed_handler_) {
            connect(color_dialog_widget_, &QColorDialog::currentColorChanged,
                    this, [this](const QColor& color) {
                        current_color_changed_handler_(color);
                    });
        }
    }
}

int ColorDialog::exec() {
    return color_dialog_widget_ ? color_dialog_widget_->exec()
                                : QDialog::Rejected;
}

void ColorDialog::show() {
    if (color_dialog_widget_) {
        color_dialog_widget_->show();
    }
}

void ColorDialog::accept() {
    if (color_dialog_widget_) {
        color_dialog_widget_->accept();
    }
}

void ColorDialog::reject() {
    if (color_dialog_widget_) {
        color_dialog_widget_->reject();
    }
}

QColor ColorDialog::selectedColor() const {
    return color_dialog_widget_ ? color_dialog_widget_->selectedColor()
                                : QColor();
}

QColor ColorDialog::getCurrentColor() const {
    return color_dialog_widget_ ? color_dialog_widget_->currentColor()
                                : QColor();
}

void ColorDialog::setCurrentColor(const QColor& color) {
    if (color_dialog_widget_) {
        color_dialog_widget_->setCurrentColor(color);
    }
}

// **Static convenience methods**
QColor ColorDialog::getColor(const QColor& initial, QWidget* parent,
                             const QString& title,
                             QColorDialog::ColorDialogOptions options) {
    return QColorDialog::getColor(initial, parent, title, options);
}

void ColorDialog::setCustomColor(int index, const QColor& color) {
    QColorDialog::setCustomColor(index, color);
}

QColor ColorDialog::customColor(int index) {
    return QColorDialog::customColor(index);
}

void ColorDialog::setStandardColor(int index, const QColor& color) {
    QColorDialog::setStandardColor(index, color);
}

QColor ColorDialog::standardColor(int index) {
    return QColorDialog::standardColor(index);
}

}  // namespace DeclarativeUI::Components
