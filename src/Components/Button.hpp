// Components/Button.hpp
#pragma once
#include <QIcon>
#include <QPushButton>


#include "../Core/UIElement.hpp"

namespace DeclarativeUI::Components {

class Button : public Core::UIElement {
    Q_OBJECT

public:
    explicit Button(QObject* parent = nullptr);

    // **Fluent interface specific to buttons**
    Button& text(const QString& text);
    Button& icon(const QIcon& icon);
    Button& onClick(std::function<void()> handler);
    Button& enabled(bool enabled);
    Button& style(const QString& stylesheet);

    void initialize() override;

private:
    QPushButton* button_widget_;
};

// **Implementation with exception safety**
inline Button::Button(QObject* parent)
    : UIElement(parent), button_widget_(nullptr) {}

inline Button& Button::text(const QString& text) {
    return static_cast<Button&>(setProperty("text", text));
}

inline Button& Button::icon(const QIcon& icon) {
    return static_cast<Button&>(setProperty("icon", icon));
}

inline Button& Button::onClick(std::function<void()> handler) {
    return static_cast<Button&>(onEvent("clicked", std::move(handler)));
}

inline Button& Button::enabled(bool enabled) {
    return static_cast<Button&>(setProperty("enabled", enabled));
}

inline Button& Button::style(const QString& stylesheet) {
    return static_cast<Button&>(setProperty("styleSheet", stylesheet));
}

inline void Button::initialize() {
    if (!button_widget_) {
        try {
            button_widget_ = new QPushButton();
            setWidget(button_widget_);

            // Connect click signal if handler is set
            auto click_handler = event_handlers_.find("clicked");
            if (click_handler != event_handlers_.end()) {
                connect(button_widget_, &QPushButton::clicked, this,
                        [handler = click_handler->second]() { handler(); });
            }

        } catch (const std::exception& e) {
            throw Exceptions::ComponentCreationException("Button: " +
                                                         std::string(e.what()));
        }
    }
}

}  // namespace DeclarativeUI::Components