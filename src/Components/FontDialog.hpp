// Components/FontDialog.hpp
#pragma once
#include <QFont>
#include <QFontDialog>
#include <QWidget>

#include "../Core/UIElement.hpp"

namespace DeclarativeUI::Components {

class FontDialog : public Core::UIElement {
    Q_OBJECT

public:
    explicit FontDialog(QObject* parent = nullptr);

    // **Fluent interface for font dialog**
    FontDialog& currentFont(const QFont& font);
    FontDialog& options(QFontDialog::FontDialogOptions options);
    FontDialog& windowTitle(const QString& title);
    FontDialog& onFontSelected(std::function<void(const QFont&)> handler);
    FontDialog& onCurrentFontChanged(std::function<void(const QFont&)> handler);

    void initialize() override;
    int exec();
    void show();
    void accept();
    void reject();
    QFont selectedFont() const;
    QFont getCurrentFont() const;
    void setCurrentFont(const QFont& font);

    // **Static convenience methods**
    static QFont getFont(bool* ok, const QFont& initial = QFont(),
                         QWidget* parent = nullptr,
                         const QString& title = QString(),
                         QFontDialog::FontDialogOptions options =
                             QFontDialog::FontDialogOptions());

    static QFont getFont(bool* ok, QWidget* parent = nullptr);

private:
    QFontDialog* font_dialog_widget_;
    std::function<void(const QFont&)> font_selected_handler_;
    std::function<void(const QFont&)> current_font_changed_handler_;
};

}  // namespace DeclarativeUI::Components
