// Components/Label.hpp
#pragma once
#include <QColor>
#include <QFont>
#include <QGraphicsDropShadowEffect>
#include <QLabel>
#include <QMovie>
#include <QPixmap>
#include <QPropertyAnimation>
#include <QTextCursor>
#include <QTextDocument>
#include <QTimer>
#include <functional>
#include <memory>

#include "../Core/UIElement.hpp"

namespace DeclarativeUI::Components {

/**
 * @brief Text formatting options for enhanced labels
 */
struct LabelFormatting {
    bool bold = false;
    bool italic = false;
    bool underline = false;
    bool strikethrough = false;
    QColor text_color;
    QColor background_color;
    QString font_family;
    int font_size = -1;

    LabelFormatting() = default;
};

class Label : public Core::UIElement {
    Q_OBJECT

public:
    explicit Label(QObject *parent = nullptr);

    // **Basic fluent interface (backward compatible)**
    Label &text(const QString &text);
    Label &pixmap(const QPixmap &pixmap);
    Label &movie(QMovie *movie);
    Label &alignment(Qt::Alignment alignment);
    Label &wordWrap(bool wrap);
    Label &font(const QFont &font);
    Label &color(const QColor &color);
    Label &backgroundColor(const QColor &color);
    Label &borderColor(const QColor &color);
    Label &borderWidth(int width);
    Label &borderRadius(int radius);
    Label &padding(int padding);
    Label &margin(int margin);
    Label &linkActivation(bool enabled);
    Label &onLinkActivated(std::function<void(const QString &)> handler);
    Label &onLinkHovered(std::function<void(const QString &)> handler);
    Label &style(const QString &stylesheet);

    // **Enhanced fluent interface**
    Label &tooltip(const QString &tooltip_text);
    Label &accessibleName(const QString &name);
    Label &accessibleDescription(const QString &description);
    Label &shortcut(const QKeySequence &shortcut);

    // **Rich text and formatting**
    Label &richText(bool enabled = true);
    Label &markdown(const QString &markdown_text);
    Label &html(const QString &html_text);
    Label &textFormat(Qt::TextFormat format);
    Label &formatting(const LabelFormatting &format);
    Label &highlightText(const QString &text,
                         const QColor &highlight_color = QColor(255, 255, 0,
                                                                100));
    Label &textSelection(bool enabled = true);
    Label &copyable(bool enabled = true);

    // **Visual enhancements**
    Label &dropShadow(bool enabled = true,
                      const QColor &color = QColor(0, 0, 0, 80));
    Label &hoverEffect(bool enabled = true);
    Label &fadeAnimation(bool enabled = true);
    Label &textShadow(const QColor &color, const QPoint &offset = QPoint(1, 1));
    Label &gradient(const QColor &start, const QColor &end,
                    Qt::Orientation orientation = Qt::Vertical);
    Label &opacity(qreal opacity);

    // **Interactive features**
    Label &clickable(bool enabled = true);
    Label &hoverable(bool enabled = true);
    Label &selectable(bool enabled = true);
    Label &draggable(bool enabled = true);
    Label &contextMenu(QMenu *menu);

    // **Content management**
    Label &placeholder(const QString &placeholder_text);
    Label &maxLength(int length);
    Label &elideMode(Qt::TextElideMode mode);
    Label &scaledContents(bool enabled = true);
    Label &autoResize(bool enabled = true);

    // **Event handlers**
    Label &onClick(std::function<void()> click_handler);
    Label &onDoubleClick(std::function<void()> double_click_handler);
    Label &onRightClick(std::function<void()> right_click_handler);
    Label &onHover(std::function<void(bool)> hover_handler);
    Label &onTextChanged(
        std::function<void(const QString &)> text_changed_handler);
    Label &onSelectionChanged(
        std::function<void(const QString &)> selection_handler);

    // **Accessibility**
    Label &role(const QString &aria_role);
    Label &tabIndex(int index);
    Label &describedBy(const QString &element_id);
    Label &labelFor(const QString &element_id);

    // **Animation and effects**
    Label &typewriterEffect(int delay_ms = 50);
    Label &fadeIn(int duration_ms = 300);
    Label &fadeOut(int duration_ms = 300);
    Label &slideIn(Qt::Orientation direction, int duration_ms = 300);
    Label &bounce(int duration_ms = 500);

    // **Data binding**
    Label &bindToProperty(QObject *object, const QString &property_name);
    Label &updateInterval(int milliseconds);  // For dynamic content updates

    void initialize() override;

    // **Getters**
    QString getText() const;
    QPixmap getPixmap() const;
    Qt::Alignment getAlignment() const;
    bool getWordWrap() const;
    QString getSelectedText() const;
    bool hasSelection() const;

    // **Setters**
    void setText(const QString &text);
    void setPixmap(const QPixmap &pixmap);
    void setAlignment(Qt::Alignment alignment);
    void setWordWrap(bool wrap);
    void clear();

    // **State queries**
    bool isClickable() const { return clickable_enabled_; }
    bool isSelectable() const { return selectable_enabled_; }
    bool isAnimating() const;

signals:
    void textChanged(const QString &new_text);
    void selectionChanged(const QString &selected_text);
    void animationFinished();
    void linkClicked(const QString &link);

protected:
    void setupAccessibility();
    void setupVisualEffects();
    void setupEventHandlers();
    void setupInteractivity();
    void setupAnimations();
    void updateLabelState();
    void applyFormatting(const LabelFormatting &format);
    void startTypewriterEffect(const QString &text, int delay_ms);
    void updateDynamicContent();

private slots:
    void onTextChangedInternal();
    void onSelectionChangedInternal();
    void onAnimationFinished();
    void onUpdateTimer();
    void onTypewriterTimer();

private:
    QLabel *label_widget_;
    std::function<void(const QString &)> link_activated_handler_;
    std::function<void(const QString &)> link_hovered_handler_;

    // **Enhanced properties**
    QString tooltip_text_;
    QString accessible_name_;
    QString accessible_description_;
    QKeySequence shortcut_;

    // **Rich text and formatting**
    bool rich_text_enabled_;
    Qt::TextFormat text_format_;
    LabelFormatting current_formatting_;
    QString highlighted_text_;
    QColor highlight_color_;
    bool text_selection_enabled_;
    bool copyable_enabled_;

    // **Visual effects**
    bool drop_shadow_enabled_;
    QColor shadow_color_;
    bool hover_effect_enabled_;
    bool fade_animation_enabled_;
    QColor text_shadow_color_;
    QPoint text_shadow_offset_;
    QColor gradient_start_;
    QColor gradient_end_;
    Qt::Orientation gradient_orientation_;
    qreal opacity_value_;

    // **Interactive features**
    bool clickable_enabled_;
    bool hoverable_enabled_;
    bool selectable_enabled_;
    bool draggable_enabled_;
    QMenu *context_menu_;

    // **Content management**
    QString placeholder_text_;
    int max_length_;
    Qt::TextElideMode elide_mode_;
    bool scaled_contents_;
    bool auto_resize_;

    // **Event handlers**
    std::function<void()> click_handler_;
    std::function<void()> double_click_handler_;
    std::function<void()> right_click_handler_;
    std::function<void(bool)> hover_handler_;
    std::function<void(const QString &)> text_changed_handler_;
    std::function<void(const QString &)> selection_handler_;

    // **Accessibility**
    QString aria_role_;
    int tab_index_;
    QString described_by_;
    QString label_for_;

    // **Animation and effects**
    bool typewriter_enabled_;
    int typewriter_delay_;
    QString typewriter_text_;
    int typewriter_position_;

    // **Data binding**
    QObject *bound_object_;
    QString bound_property_;
    int update_interval_;

    // **Internal components**
    std::unique_ptr<QGraphicsDropShadowEffect> shadow_effect_;
    std::unique_ptr<QPropertyAnimation> fade_animation_;
    std::unique_ptr<QPropertyAnimation> slide_animation_;
    std::unique_ptr<QPropertyAnimation> bounce_animation_;
    std::unique_ptr<QTimer> update_timer_;
    std::unique_ptr<QTimer> typewriter_timer_;
    std::unique_ptr<QTextDocument> text_document_;
};

}  // namespace DeclarativeUI::Components
