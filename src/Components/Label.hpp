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



}  // namespace DeclarativeUI::Components
