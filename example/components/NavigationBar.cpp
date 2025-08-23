/**
 * @file NavigationBar.cpp
 * @brief Implementation of enhanced navigation bar component
 */

#include "NavigationBar.hpp"
#include <QDebug>

NavigationBar::NavigationBar(QWidget* parent)
    : QWidget(parent),
      layout_(nullptr),
      button_group_(nullptr),
      highlight_indicator_(nullptr),
      current_index_(-1),
      animation_enabled_(true),
      highlight_color_(QColor("#3498db")),
      background_color_(QColor("#ecf0f1")) {
    setupUI();
}

void NavigationBar::setupUI() {
    setFixedHeight(50);
    setStyleSheet(QString("NavigationBar { background-color: %1; "
                          "border-bottom: 1px solid #bdc3c7; }")
                      .arg(background_color_.name()));

    layout_ = new QHBoxLayout(this);
    layout_->setContentsMargins(8, 4, 8, 4);
    layout_->setSpacing(4);

    button_group_ = new QButtonGroup(this);
    button_group_->setExclusive(true);

    // Create highlight indicator
    highlight_indicator_ = new QWidget(this);
    highlight_indicator_->setFixedSize(0, 3);
    highlight_indicator_->setStyleSheet(
        QString("background-color: %1;").arg(highlight_color_.name()));
    highlight_indicator_->hide();

    // Setup animation
    if (animation_enabled_) {
        highlight_animation_ = std::make_unique<QPropertyAnimation>(
            highlight_indicator_, "geometry");
        highlight_animation_->setDuration(300);
        highlight_animation_->setEasingCurve(QEasingCurve::OutCubic);
    }
}

void NavigationBar::addItem(const QString& name, const QString& icon,
                            const QString& tooltip) {
    NavigationItem item(name, icon, tooltip);
    items_.push_back(item);

    int index = items_.size() - 1;
    createNavigationButton(item, index);

    // Select first item by default
    if (index == 0) {
        setCurrentIndex(0);
    }
}

void NavigationBar::removeItem(int index) {
    if (index < 0 || index >= static_cast<int>(items_.size()))
        return;

    // Remove from collections
    items_.erase(items_.begin() + index);

    if (index < static_cast<int>(buttons_.size())) {
        auto button = buttons_[index];
        button_group_->removeButton(button);
        layout_->removeWidget(button);
        button->deleteLater();
        buttons_.erase(buttons_.begin() + index);
    }

    // Update current index if necessary
    if (current_index_ == index) {
        current_index_ = -1;
        if (!items_.empty()) {
            setCurrentIndex(0);
        }
    } else if (current_index_ > index) {
        current_index_--;
    }
}

void NavigationBar::setCurrentIndex(int index) {
    if (index < 0 || index >= static_cast<int>(items_.size()))
        return;
    if (index == current_index_)
        return;

    int old_index = current_index_;
    current_index_ = index;

    // Update button states
    if (index < static_cast<int>(buttons_.size())) {
        buttons_[index]->setChecked(true);
    }

    // Animate highlight indicator
    if (animation_enabled_ && old_index >= 0) {
        animateToItem(index);
    } else {
        // Position highlight immediately
        if (index < static_cast<int>(buttons_.size())) {
            auto button = buttons_[index];
            highlight_indicator_->setGeometry(button->x(), height() - 3,
                                              button->width(), 3);
            highlight_indicator_->show();
        }
    }

    emit currentChanged(index);
}

int NavigationBar::currentIndex() const { return current_index_; }

void NavigationBar::setAnimationEnabled(bool enabled) {
    animation_enabled_ = enabled;
}

void NavigationBar::setHighlightColor(const QColor& color) {
    highlight_color_ = color;
    highlight_indicator_->setStyleSheet(
        QString("background-color: %1;").arg(color.name()));
}

void NavigationBar::setBackgroundColor(const QColor& color) {
    background_color_ = color;
    setStyleSheet(QString("NavigationBar { background-color: %1; "
                          "border-bottom: 1px solid #bdc3c7; }")
                      .arg(color.name()));
}

void NavigationBar::createNavigationButton(const NavigationItem& item,
                                           int index) {
    auto button = new QPushButton();
    button->setText(QString("%1 %2").arg(item.icon, item.name));
    button->setToolTip(item.tooltip);
    button->setCheckable(true);
    button->setEnabled(item.enabled);
    button->setMinimumWidth(100);
    button->setMaximumHeight(42);

    // Style the button
    button->setStyleSheet(
        "QPushButton {"
        "    border: none;"
        "    padding: 8px 16px;"
        "    text-align: left;"
        "    background-color: transparent;"
        "    color: #2c3e50;"
        "    font-weight: 500;"
        "}"
        "QPushButton:hover {"
        "    background-color: rgba(52, 152, 219, 0.1);"
        "}"
        "QPushButton:checked {"
        "    background-color: rgba(52, 152, 219, 0.2);"
        "    color: #3498db;"
        "    font-weight: 600;"
        "}");

    // Connect signals
    connect(button, &QPushButton::clicked, [this, index]() {
        emit itemClicked(index);
        setCurrentIndex(index);
    });

    // Add to collections
    buttons_.push_back(button);
    button_group_->addButton(button, index);
    layout_->addWidget(button);
}

void NavigationBar::onItemClicked() {
    auto button = qobject_cast<QPushButton*>(sender());
    if (!button)
        return;

    int index = button_group_->id(button);
    if (index >= 0) {
        emit itemClicked(index);
        setCurrentIndex(index);
    }
}

void NavigationBar::animateToItem(int index) {
    if (!animation_enabled_ || !highlight_animation_)
        return;
    if (index < 0 || index >= static_cast<int>(buttons_.size()))
        return;

    auto target_button = buttons_[index];
    QRect target_rect(target_button->x(), height() - 3, target_button->width(),
                      3);

    highlight_animation_->setStartValue(highlight_indicator_->geometry());
    highlight_animation_->setEndValue(target_rect);
    highlight_animation_->start();

    highlight_indicator_->show();
}

void NavigationBar::updateButtonStyles() {
    // Update all button styles if needed
    for (auto button : buttons_) {
        // Refresh button styling
        button->style()->unpolish(button);
        button->style()->polish(button);
    }
}

#include "NavigationBar.moc"
