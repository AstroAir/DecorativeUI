/**
 * @file NavigationBar.hpp
 * @brief Enhanced navigation bar component
 */

#pragma once

#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QButtonGroup>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>
#include <memory>

/**
 * @brief Navigation item information
 */
struct NavigationItem {
    QString name;
    QString icon;
    QString tooltip;
    bool enabled;
    
    NavigationItem(const QString& n, const QString& i, const QString& t, bool e = true)
        : name(n), icon(i), tooltip(t), enabled(e) {}
};

/**
 * @brief Enhanced navigation bar with smooth animations and modern styling
 */
class NavigationBar : public QWidget {
    Q_OBJECT

public:
    explicit NavigationBar(QWidget* parent = nullptr);
    
    // Navigation management
    void addItem(const QString& name, const QString& icon, const QString& tooltip);
    void removeItem(int index);
    void setCurrentIndex(int index);
    int currentIndex() const;
    
    // Styling
    void setAnimationEnabled(bool enabled);
    void setHighlightColor(const QColor& color);
    void setBackgroundColor(const QColor& color);

signals:
    void itemClicked(int index);
    void currentChanged(int index);

private slots:
    void onItemClicked();
    void animateToItem(int index);

private:
    void setupUI();
    void createNavigationButton(const NavigationItem& item, int index);
    void updateButtonStyles();
    void animateHighlight(int from_index, int to_index);

    // UI Components
    QHBoxLayout* layout_;
    QButtonGroup* button_group_;
    QWidget* highlight_indicator_;
    
    // Navigation items
    std::vector<NavigationItem> items_;
    std::vector<QPushButton*> buttons_;
    
    // State
    int current_index_;
    bool animation_enabled_;
    QColor highlight_color_;
    QColor background_color_;
    
    // Animation
    std::unique_ptr<QPropertyAnimation> highlight_animation_;
};
