// Components/StatusBar.hpp
#pragma once
#include <QStatusBar>
#include <QWidget>
#include <QLabel>

#include "../Core/UIElement.hpp"

namespace DeclarativeUI::Components {

class StatusBar : public Core::UIElement {
    Q_OBJECT

public:
    explicit StatusBar(QObject* parent = nullptr);

    // **Fluent interface for status bar**
    StatusBar& showMessage(const QString& message, int timeout = 0);
    StatusBar& clearMessage();
    StatusBar& addWidget(QWidget* widget, int stretch = 0);
    StatusBar& addPermanentWidget(QWidget* widget, int stretch = 0);
    StatusBar& insertWidget(int index, QWidget* widget, int stretch = 0);
    StatusBar& insertPermanentWidget(int index, QWidget* widget, int stretch = 0);
    StatusBar& removeWidget(QWidget* widget);
    StatusBar& setSizeGripEnabled(bool enabled);
    StatusBar& onMessageChanged(std::function<void(const QString&)> handler);
    StatusBar& style(const QString& stylesheet);

    void initialize() override;
    QString currentMessage() const;
    bool isSizeGripEnabled() const;
    void reformat();

private:
    QStatusBar* status_bar_widget_;
    std::function<void(const QString&)> message_changed_handler_;
};



}  // namespace DeclarativeUI::Components
