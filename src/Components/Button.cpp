// Components/Button.cpp
#include "Button.hpp"
#include "../Exceptions/UIExceptions.hpp"

namespace DeclarativeUI::Components {

// **Implementation with exception safety**
Button::Button(QObject* parent)
    : UIElement(parent), button_widget_(nullptr) {}

Button& Button::text(const QString& text) {
    return static_cast<Button&>(setProperty("text", text));
}

Button& Button::icon(const QIcon& icon) {
    return static_cast<Button&>(setProperty("icon", icon));
}

Button& Button::onClick(std::function<void()> handler) {
    return static_cast<Button&>(onEvent("clicked", std::move(handler)));
}

Button& Button::enabled(bool enabled) {
    return static_cast<Button&>(setProperty("enabled", enabled));
}

Button& Button::style(const QString& stylesheet) {
    return static_cast<Button&>(setProperty("styleSheet", stylesheet));
}

void Button::initialize() {
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
