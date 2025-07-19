// Components/ComboBox.hpp
#pragma once
#include <QComboBox>

#include "../Core/UIElement.hpp"

namespace DeclarativeUI::Components {

class ComboBox : public Core::UIElement {
    Q_OBJECT

public:
    explicit ComboBox(QObject* parent = nullptr);

    // **Fluent interface for combobox**
    ComboBox& items(const QStringList& items);
    ComboBox& addItem(const QString& item);
    ComboBox& addItems(const QStringList& items);
    ComboBox& currentIndex(int index);
    ComboBox& currentText(const QString& text);
    ComboBox& editable(bool editable);
    ComboBox& maxVisibleItems(int maxItems);
    ComboBox& onCurrentIndexChanged(std::function<void(int)> handler);
    ComboBox& onCurrentTextChanged(std::function<void(const QString&)> handler);
    ComboBox& onActivated(std::function<void(int)> handler);
    ComboBox& style(const QString& stylesheet);

    void initialize() override;
    int getCurrentIndex() const;
    QString getCurrentText() const;
    void setCurrentIndex(int index);
    void setCurrentText(const QString& text);
    void clearItems();

private:
    QComboBox* combobox_widget_;
    QStringList initial_items_;
    std::function<void(int)> current_index_changed_handler_;
    std::function<void(const QString&)> current_text_changed_handler_;
    std::function<void(int)> activated_handler_;
};



}  // namespace DeclarativeUI::Components
