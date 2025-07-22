#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QWidget>
#include <QDebug>

#ifdef DECLARATIVE_UI_COMMAND_SYSTEM_ENABLED
#include "../../Command/CommandBuilder.hpp"
#include "../../Command/WidgetMapper.hpp"
#include "../../Command/UICommandFactory.hpp"

using namespace DeclarativeUI::Command;

class CommandBuilderExampleWindow : public QMainWindow {
    Q_OBJECT

public:
    CommandBuilderExampleWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("Command Builder Example");
        setMinimumSize(400, 300);
        
        setupUI();
    }

private:
    void setupUI() {
        try {
            // Create UI using Command Builder
            auto mainContainer = UI::CommandHierarchyBuilder("Container")
                .layout("VBox")
                .spacing(10)
                .margins(20)
                .addChild("Label", [](UI::CommandBuilder& label) {
                    label.text("Command Builder Example")
                         .style("font-weight", "bold")
                         .style("font-size", "16px");
                })
                .addChild("Button", [this](UI::CommandBuilder& button) {
                    button.text("Click Me!")
                          .onClick([this]() {
                              qDebug() << "Button clicked via Command system!";
                          });
                })
                .addChild("TextInput", [](UI::CommandBuilder& input) {
                    input.placeholder("Enter text here...")
                         .bindToState("example.input");
                })
                .addChild("Container", [](UI::CommandBuilder& container) {
                    container.property("layout", "HBox")
                            .property("spacing", 5)
                            .child(UI::CommandBuilder("Button")
                                .text("Button 1"))
                            .child(UI::CommandBuilder("Button")
                                .text("Button 2"));
                })
                .build();

            // Convert to QWidget
            auto widget = UI::WidgetMapper::instance().createWidget(mainContainer.get());
            if (widget) {
                setCentralWidget(widget.release());
                qDebug() << "✅ Command Builder example UI created successfully";
            } else {
                qWarning() << "❌ Failed to create widget from command";
            }
        } catch (const std::exception& e) {
            qWarning() << "❌ Error creating Command Builder example:" << e.what();
        }
    }
};

#include "command_builder_example.moc"

#endif // DECLARATIVE_UI_COMMAND_SYSTEM_ENABLED

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

#ifdef DECLARATIVE_UI_COMMAND_SYSTEM_ENABLED
    qDebug() << "🚀 Starting Command Builder Example";
    
    CommandBuilderExampleWindow window;
    window.show();
    
    return app.exec();
#else
    qWarning() << "❌ Command system not enabled. Please build with BUILD_COMMAND_SYSTEM=ON";
    return 1;
#endif
}
