// Components/ColorDialog.hpp
#pragma once
#include <QColorDialog>
#include <QColor>
#include <QWidget>

#include "../Core/UIElement.hpp"

namespace DeclarativeUI::Components {

class ColorDialog : public Core::UIElement {
    Q_OBJECT

public:
    explicit ColorDialog(QObject* parent = nullptr);

    // **Fluent interface for color dialog**
    ColorDialog& currentColor(const QColor& color);
    ColorDialog& options(QColorDialog::ColorDialogOptions options);
    ColorDialog& windowTitle(const QString& title);
    ColorDialog& onColorSelected(std::function<void(const QColor&)> handler);
    ColorDialog& onCurrentColorChanged(std::function<void(const QColor&)> handler);

    void initialize() override;
    int exec();
    void show();
    void accept();
    void reject();
    QColor selectedColor() const;
    QColor getCurrentColor() const;
    void setCurrentColor(const QColor& color);

    // **Static convenience methods**
    static QColor getColor(const QColor& initial = Qt::white,
                          QWidget* parent = nullptr,
                          const QString& title = QString(),
                          QColorDialog::ColorDialogOptions options = QColorDialog::ColorDialogOptions());

    static void setCustomColor(int index, const QColor& color);
    static QColor customColor(int index);
    static void setStandardColor(int index, const QColor& color);
    static QColor standardColor(int index);

private:
    QColorDialog* color_dialog_widget_;
    std::function<void(const QColor&)> color_selected_handler_;
    std::function<void(const QColor&)> current_color_changed_handler_;
};



}  // namespace DeclarativeUI::Components
