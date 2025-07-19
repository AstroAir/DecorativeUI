// Components/MessageBox.hpp
#pragma once
#include <QMessageBox>
#include <QWidget>

#include "../Core/UIElement.hpp"

namespace DeclarativeUI::Components {

class MessageBox : public Core::UIElement {
    Q_OBJECT

public:
    explicit MessageBox(QObject* parent = nullptr);

    // **Fluent interface for message box**
    MessageBox& icon(QMessageBox::Icon icon);
    MessageBox& text(const QString& text);
    MessageBox& informativeText(const QString& text);
    MessageBox& detailedText(const QString& text);
    MessageBox& windowTitle(const QString& title);
    MessageBox& standardButtons(QMessageBox::StandardButtons buttons);
    MessageBox& defaultButton(QMessageBox::StandardButton button);
    MessageBox& escapeButton(QMessageBox::StandardButton button);
    MessageBox& textFormat(Qt::TextFormat format);
    MessageBox& onButtonClicked(std::function<void(QAbstractButton*)> handler);
    MessageBox& onFinished(std::function<void(int)> handler);

    void initialize() override;
    int exec();
    void show();
    void accept();
    void reject();
    QMessageBox::StandardButton getStandardButton(QAbstractButton* button) const;
    QAbstractButton* getButton(QMessageBox::StandardButton which) const;

    // **Static convenience methods**
    static QMessageBox::StandardButton information(QWidget* parent, const QString& title, 
                                                   const QString& text, 
                                                   QMessageBox::StandardButtons buttons = QMessageBox::Ok,
                                                   QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);
    
    static QMessageBox::StandardButton question(QWidget* parent, const QString& title, 
                                               const QString& text, 
                                               QMessageBox::StandardButtons buttons = QMessageBox::StandardButtons(QMessageBox::Yes | QMessageBox::No),
                                               QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);
    
    static QMessageBox::StandardButton warning(QWidget* parent, const QString& title, 
                                              const QString& text, 
                                              QMessageBox::StandardButtons buttons = QMessageBox::Ok,
                                              QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);
    
    static QMessageBox::StandardButton critical(QWidget* parent, const QString& title, 
                                               const QString& text, 
                                               QMessageBox::StandardButtons buttons = QMessageBox::Ok,
                                               QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);

private:
    QMessageBox* message_box_widget_;
    std::function<void(QAbstractButton*)> button_clicked_handler_;
    std::function<void(int)> finished_handler_;
};



}  // namespace DeclarativeUI::Components
