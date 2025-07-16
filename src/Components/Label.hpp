// Components/Label.hpp
#pragma once
#include <QColor>
#include <QFont>
#include <QLabel>
#include <QMovie>
#include <QPixmap>

#include "../Core/UIElement.hpp"

namespace DeclarativeUI::Components {

class Label : public Core::UIElement {
    Q_OBJECT

public:
    explicit Label(QObject *parent = nullptr);

    // **Fluent interface for label**
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

    void initialize() override;

    // **Getters**
    QString getText() const;
    QPixmap getPixmap() const;
    Qt::Alignment getAlignment() const;
    bool getWordWrap() const;

    // **Setters**
    void setText(const QString &text);
    void setPixmap(const QPixmap &pixmap);
    void setAlignment(Qt::Alignment alignment);
    void setWordWrap(bool wrap);
    void clear();

private:
    QLabel *label_widget_;
    std::function<void(const QString &)> link_activated_handler_;
    std::function<void(const QString &)> link_hovered_handler_;
};

// **Implementation**
inline Label::Label(QObject *parent)
    : UIElement(parent), label_widget_(nullptr) {}

inline Label &Label::text(const QString &text) {
    return static_cast<Label &>(setProperty("text", text));
}

inline Label &Label::pixmap(const QPixmap &pixmap) {
    return static_cast<Label &>(setProperty("pixmap", pixmap));
}

inline Label &Label::movie(QMovie *movie) {
    return static_cast<Label &>(
        setProperty("movie", QVariant::fromValue(movie)));
}

inline Label &Label::alignment(Qt::Alignment alignment) {
    return static_cast<Label &>(
        setProperty("alignment", static_cast<int>(alignment)));
}

inline Label &Label::wordWrap(bool wrap) {
    return static_cast<Label &>(setProperty("wordWrap", wrap));
}

inline Label &Label::font(const QFont &font) {
    return static_cast<Label &>(setProperty("font", font));
}

inline Label &Label::color(const QColor &color) {
    // Set text color via stylesheet
    QString style = QString("QLabel { color: %1; }").arg(color.name());
    return static_cast<Label &>(setProperty("styleSheet", style));
}

inline Label &Label::backgroundColor(const QColor &color) {
    QString style =
        QString("QLabel { background-color: %1; }").arg(color.name());
    return static_cast<Label &>(setProperty("styleSheet", style));
}

inline Label &Label::borderColor(const QColor &color) {
    QString style =
        QString("QLabel { border: 1px solid %1; }").arg(color.name());
    return static_cast<Label &>(setProperty("styleSheet", style));
}

inline Label &Label::borderWidth(int width) {
    QString style = QString("QLabel { border-width: %1px; }").arg(width);
    return static_cast<Label &>(setProperty("styleSheet", style));
}

inline Label &Label::borderRadius(int radius) {
    QString style = QString("QLabel { border-radius: %1px; }").arg(radius);
    return static_cast<Label &>(setProperty("styleSheet", style));
}

inline Label &Label::padding(int padding) {
    QString style = QString("QLabel { padding: %1px; }").arg(padding);
    return static_cast<Label &>(setProperty("styleSheet", style));
}

inline Label &Label::margin(int margin) {
    QString style = QString("QLabel { margin: %1px; }").arg(margin);
    return static_cast<Label &>(setProperty("styleSheet", style));
}

inline Label &Label::linkActivation(bool enabled) {
    return static_cast<Label &>(setProperty("openExternalLinks", enabled));
}

inline Label &Label::onLinkActivated(
    std::function<void(const QString &)> handler) {
    link_activated_handler_ = std::move(handler);
    return *this;
}

inline Label &Label::onLinkHovered(
    std::function<void(const QString &)> handler) {
    link_hovered_handler_ = std::move(handler);
    return *this;
}

inline Label &Label::style(const QString &stylesheet) {
    return static_cast<Label &>(setProperty("styleSheet", stylesheet));
}

inline void Label::initialize() {
    if (!label_widget_) {
        label_widget_ = new QLabel();
        setWidget(label_widget_);

        // Connect signals
        if (link_activated_handler_) {
            connect(
                label_widget_, &QLabel::linkActivated, this,
                [this](const QString &link) { link_activated_handler_(link); });
        }

        if (link_hovered_handler_) {
            connect(
                label_widget_, &QLabel::linkHovered, this,
                [this](const QString &link) { link_hovered_handler_(link); });
        }
    }
}

inline QString Label::getText() const {
    return label_widget_ ? label_widget_->text() : QString();
}

inline QPixmap Label::getPixmap() const {
    if (label_widget_) {
        QPixmap pixmap = label_widget_->pixmap();
        if (!pixmap.isNull()) {
            return pixmap;
        }
    }
    return QPixmap();
}

inline Qt::Alignment Label::getAlignment() const {
    return label_widget_ ? label_widget_->alignment() : Qt::AlignLeft;
}

inline bool Label::getWordWrap() const {
    return label_widget_ ? label_widget_->wordWrap() : false;
}

inline void Label::setText(const QString &text) {
    if (label_widget_) {
        label_widget_->setText(text);
    }
}

inline void Label::setPixmap(const QPixmap &pixmap) {
    if (label_widget_) {
        label_widget_->setPixmap(pixmap);
    }
}

inline void Label::setAlignment(Qt::Alignment alignment) {
    if (label_widget_) {
        label_widget_->setAlignment(alignment);
    }
}

inline void Label::setWordWrap(bool wrap) {
    if (label_widget_) {
        label_widget_->setWordWrap(wrap);
    }
}

inline void Label::clear() {
    if (label_widget_) {
        label_widget_->clear();
    }
}

}  // namespace DeclarativeUI::Components
