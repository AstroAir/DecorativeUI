// Components/FontDialog.cpp
#include "FontDialog.hpp"

namespace DeclarativeUI::Components {

// **Implementation**
FontDialog::FontDialog(QObject* parent)
    : UIElement(parent), font_dialog_widget_(nullptr) {}

FontDialog& FontDialog::currentFont(const QFont& font) {
    return static_cast<FontDialog&>(setProperty("currentFont", font));
}

FontDialog& FontDialog::options(QFontDialog::FontDialogOptions options) {
    return static_cast<FontDialog&>(setProperty("options", static_cast<int>(options)));
}

FontDialog& FontDialog::windowTitle(const QString& title) {
    return static_cast<FontDialog&>(setProperty("windowTitle", title));
}

FontDialog& FontDialog::onFontSelected(std::function<void(const QFont&)> handler) {
    font_selected_handler_ = std::move(handler);
    return *this;
}

FontDialog& FontDialog::onCurrentFontChanged(std::function<void(const QFont&)> handler) {
    current_font_changed_handler_ = std::move(handler);
    return *this;
}

void FontDialog::initialize() {
    if (!font_dialog_widget_) {
        font_dialog_widget_ = new QFontDialog();
        setWidget(font_dialog_widget_);

        // Connect signals
        if (font_selected_handler_) {
            connect(font_dialog_widget_, &QFontDialog::fontSelected, this,
                    [this](const QFont& font) { font_selected_handler_(font); });
        }

        if (current_font_changed_handler_) {
            connect(font_dialog_widget_, &QFontDialog::currentFontChanged, this,
                    [this](const QFont& font) { current_font_changed_handler_(font); });
        }
    }
}

int FontDialog::exec() {
    return font_dialog_widget_ ? font_dialog_widget_->exec() : QDialog::Rejected;
}

void FontDialog::show() {
    if (font_dialog_widget_) {
        font_dialog_widget_->show();
    }
}

void FontDialog::accept() {
    if (font_dialog_widget_) {
        font_dialog_widget_->accept();
    }
}

void FontDialog::reject() {
    if (font_dialog_widget_) {
        font_dialog_widget_->reject();
    }
}

QFont FontDialog::selectedFont() const {
    return font_dialog_widget_ ? font_dialog_widget_->selectedFont() : QFont();
}

QFont FontDialog::getCurrentFont() const {
    return font_dialog_widget_ ? font_dialog_widget_->currentFont() : QFont();
}

void FontDialog::setCurrentFont(const QFont& font) {
    if (font_dialog_widget_) {
        font_dialog_widget_->setCurrentFont(font);
    }
}

// **Static convenience methods**
QFont FontDialog::getFont(bool* ok, const QFont& initial, QWidget* parent, 
                        const QString& title, QFontDialog::FontDialogOptions options) {
    return QFontDialog::getFont(ok, initial, parent, title, options);
}

QFont FontDialog::getFont(bool* ok, QWidget* parent) {
    return QFontDialog::getFont(ok, parent);
}

}  // namespace DeclarativeUI::Components
