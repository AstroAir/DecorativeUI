// Components/ComboBox.hpp
#pragma once
#include <QAbstractItemModel>
#include <QComboBox>
#include <QCompleter>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QTimer>
#include <functional>
#include <memory>

#include "../Core/UIElement.hpp"

namespace DeclarativeUI::Components {

/**
 * @brief Validation result for combo box selection
 */
struct ComboBoxValidationResult {
    bool is_valid;
    QString error_message;
    QString suggestion;

    ComboBoxValidationResult(bool valid = true, const QString& error = "",
                             const QString& hint = "")
        : is_valid(valid), error_message(error), suggestion(hint) {}
};

class ComboBox : public Core::UIElement {
    Q_OBJECT

public:
    explicit ComboBox(QObject* parent = nullptr);

    // **Basic fluent interface (backward compatible)**
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

    // **Enhanced fluent interface**
    ComboBox& tooltip(const QString& tooltip_text);
    ComboBox& accessibleName(const QString& name);
    ComboBox& accessibleDescription(const QString& description);
    ComboBox& placeholder(const QString& placeholder_text);
    ComboBox& shortcut(const QKeySequence& shortcut);

    // **Validation**
    ComboBox& required(bool required = true);
    ComboBox& validator(
        std::function<ComboBoxValidationResult(const QString&, int)>
            validation_func);
    ComboBox& onValidationFailed(
        std::function<void(const QString&)> error_handler);
    ComboBox& validateOnChange(bool validate_on_change = true);
    ComboBox& allowedValues(const QStringList& allowed_values);
    ComboBox& customValidator(
        std::function<bool(const QString&)> custom_validator);

    // **Auto-completion and filtering**
    ComboBox& autoComplete(bool enabled = true);
    ComboBox& filterMode(Qt::MatchFlags flags = Qt::MatchContains);
    ComboBox& dynamicItems(std::function<QStringList(const QString&)> provider);
    ComboBox& searchable(bool enabled = true);
    ComboBox& caseSensitive(bool sensitive = false);

    // **Visual enhancements**
    ComboBox& dropShadow(bool enabled = true,
                         const QColor& color = QColor(0, 0, 0, 80));
    ComboBox& hoverEffect(bool enabled = true);
    ComboBox& expandAnimation(bool enabled = true);
    ComboBox& borderRadius(int radius);
    ComboBox& customColors(const QColor& background,
                           const QColor& text = QColor(),
                           const QColor& border = QColor());
    ComboBox& iconSize(const QSize& size);
    ComboBox& itemIcon(int index, const QIcon& icon);
    ComboBox& itemData(int index, const QVariant& data,
                       int role = Qt::UserRole);

    // **State management**
    ComboBox& disabled(bool disabled, const QString& reason = "");
    ComboBox& readOnly(bool readonly = true);
    ComboBox& loading(bool loading_state);
    ComboBox& loadingText(const QString& text);

    // **Event handlers**
    ComboBox& onHover(std::function<void(bool)> hover_handler);
    ComboBox& onFocus(std::function<void(bool)> focus_handler);
    ComboBox& onDropdownToggle(std::function<void(bool)> toggle_handler);
    ComboBox& onItemHighlighted(std::function<void(int)> highlight_handler);
    ComboBox& onValidationChanged(
        std::function<void(bool, const QString&)> validation_handler);
    ComboBox& onItemsChanged(std::function<void()> items_changed_handler);

    // **Accessibility**
    ComboBox& role(const QString& aria_role);
    ComboBox& tabIndex(int index);
    ComboBox& describedBy(const QString& element_id);
    ComboBox& labelledBy(const QString& element_id);

    // **Advanced features**
    ComboBox& multiSelect(bool enabled = true);
    ComboBox& separator(int index, const QString& text = "");
    ComboBox& groupItems(const QMap<QString, QStringList>& groups);
    ComboBox& customItemDelegate(QAbstractItemDelegate* delegate);

    void initialize() override;
    int getCurrentIndex() const;
    QString getCurrentText() const;
    void setCurrentIndex(int index);
    void setCurrentText(const QString& text);
    void clearItems();

    // **State queries**
    bool isValid() const { return is_valid_; }
    QString getValidationError() const { return validation_error_; }
    QStringList getAllItems() const;
    QStringList getSelectedItems() const;  // For multi-select
    bool isLoading() const { return loading_state_; }

signals:
    void validationChanged(bool is_valid, const QString& error_message);
    void itemsUpdated(const QStringList& new_items);
    void selectionChanged(const QStringList& selected_items);
    void dropdownToggled(bool is_open);

protected:
    void setupAccessibility();
    void setupVisualEffects();
    void setupEventHandlers();
    void setupValidation();
    void setupAutoCompletion();
    void setupFiltering();
    void updateComboBoxState();
    ComboBoxValidationResult validateSelection(const QString& text,
                                               int index) const;
    void showValidationError(const QString& error);
    void updateItems(const QString& filter_text = "");
    void animateDropdown(bool opening);

private slots:
    void onCurrentIndexChangedInternal(int index);
    void onCurrentTextChangedInternal(const QString& text);
    void onValidationTimer();
    void onItemsUpdateTimer();

private:
    QComboBox* combobox_widget_;
    QStringList initial_items_;
    std::function<void(int)> current_index_changed_handler_;
    std::function<void(const QString&)> current_text_changed_handler_;
    std::function<void(int)> activated_handler_;

    // **Enhanced properties**
    QString tooltip_text_;
    QString accessible_name_;
    QString accessible_description_;
    QString placeholder_text_;
    QKeySequence shortcut_;

    // **Validation**
    bool required_;
    std::function<ComboBoxValidationResult(const QString&, int)>
        validation_func_;
    std::function<void(const QString&)> error_handler_;
    bool validate_on_change_;
    QStringList allowed_values_;
    std::function<bool(const QString&)> custom_validator_;
    QString validation_error_;
    bool is_valid_;

    // **Auto-completion and filtering**
    bool auto_complete_enabled_;
    Qt::MatchFlags filter_flags_;
    std::function<QStringList(const QString&)> dynamic_items_provider_;
    bool searchable_;
    bool case_sensitive_;

    // **Visual effects**
    bool drop_shadow_enabled_;
    QColor shadow_color_;
    bool hover_effect_enabled_;
    bool expand_animation_enabled_;
    int border_radius_;
    QColor background_color_;
    QColor text_color_;
    QColor border_color_;
    QSize icon_size_;

    // **State management**
    bool disabled_state_;
    QString disabled_reason_;
    bool read_only_;
    bool loading_state_;
    QString loading_text_;
    QString original_placeholder_;

    // **Event handlers**
    std::function<void(bool)> hover_handler_;
    std::function<void(bool)> focus_handler_;
    std::function<void(bool)> dropdown_toggle_handler_;
    std::function<void(int)> item_highlighted_handler_;
    std::function<void(bool, const QString&)> validation_handler_;
    std::function<void()> items_changed_handler_;

    // **Accessibility**
    QString aria_role_;
    int tab_index_;
    QString described_by_;
    QString labelled_by_;

    // **Advanced features**
    bool multi_select_enabled_;
    QMap<int, QString> separators_;
    QMap<QString, QStringList> item_groups_;
    QAbstractItemDelegate* custom_delegate_;

    // **Internal components**
    std::unique_ptr<QCompleter> completer_;
    std::unique_ptr<QSortFilterProxyModel> filter_model_;
    std::unique_ptr<QStandardItemModel> item_model_;
    std::unique_ptr<QGraphicsDropShadowEffect> shadow_effect_;
    std::unique_ptr<QPropertyAnimation> dropdown_animation_;
    std::unique_ptr<QTimer> validation_timer_;
    std::unique_ptr<QTimer> items_update_timer_;
};

}  // namespace DeclarativeUI::Components
