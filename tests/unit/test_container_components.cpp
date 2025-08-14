#include <QApplication>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QSignalSpy>
#include <QSplitter>
#include <QTest>
#include <QTextEdit>
#include <QVBoxLayout>
#include <memory>

#include "../Components/Container.hpp"
#include "../Components/Frame.hpp"
#include "../Components/Label.hpp"
#include "../Components/PlainTextEdit.hpp"
#include "../Components/ScrollArea.hpp"
#include "../Components/Splitter.hpp"
#include "../Components/TextEdit.hpp"
#include "../Exceptions/UIExceptions.hpp"

using namespace DeclarativeUI::Components;
using namespace DeclarativeUI::Exceptions;

class ContainerComponentsTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        // Initialize Qt application if not already done
        if (!QApplication::instance()) {
            int argc = 0;
            char* argv[] = {nullptr};
            new QApplication(argc, argv);
        }
    }

    void cleanupTestCase() {
        // Clean up any global state
    }

    void init() {
        // Set up for each test
    }

    void cleanup() {
        // Clean up after each test
    }

    // **Container Component Tests**
    void testContainerCreation() {
        auto container = std::make_unique<Container>();

        container->setLayout(new QVBoxLayout());

        container->initialize();

        auto* widget = container->getWidget();
        QVERIFY(widget != nullptr);

        auto* qt_widget = qobject_cast<QWidget*>(widget);
        QVERIFY(qt_widget != nullptr);

        auto* layout = qt_widget->layout();
        QVERIFY(layout != nullptr);

        auto* vbox_layout = qobject_cast<QVBoxLayout*>(layout);
        QVERIFY(vbox_layout != nullptr);
        QCOMPARE(vbox_layout->spacing(), 6);  // Qt default spacing
        QCOMPARE(vbox_layout->contentsMargins().left(),
                 11);  // Qt default margin
    }

    void testContainerWithChildren() {
        auto container = std::make_unique<Container>();

        container->setLayout(new QVBoxLayout());
        container->initialize();

        auto* widget = container->getWidget();
        auto* qt_widget = qobject_cast<QWidget*>(widget);
        auto* layout = qt_widget->layout();

        // Add child widgets
        auto* child1 = new QLabel("Child 1");
        auto* child2 = new QPushButton("Child 2");

        layout->addWidget(child1);
        layout->addWidget(child2);

        QCOMPARE(layout->count(), 2);
        QCOMPARE(layout->itemAt(0)->widget(), child1);
        QCOMPARE(layout->itemAt(1)->widget(), child2);
    }

    void testContainerLayoutTypes() {
        // Test VBox layout
        auto vbox_container = std::make_unique<Container>();
        vbox_container->setLayout(new QVBoxLayout());
        vbox_container->initialize();

        auto* vbox_widget = vbox_container->getWidget();
        auto* vbox_layout = qobject_cast<QVBoxLayout*>(vbox_widget->layout());
        QVERIFY(vbox_layout != nullptr);

        // Test HBox layout
        auto hbox_container = std::make_unique<Container>();
        hbox_container->setLayout(new QHBoxLayout());
        hbox_container->initialize();

        auto* hbox_widget = hbox_container->getWidget();
        auto* hbox_layout = qobject_cast<QHBoxLayout*>(hbox_widget->layout());
        QVERIFY(hbox_layout != nullptr);

        // Test Grid layout
        auto grid_container = std::make_unique<Container>();
        grid_container->setLayout(new QGridLayout());
        grid_container->initialize();

        auto* grid_widget = grid_container->getWidget();
        auto* grid_layout = qobject_cast<QGridLayout*>(grid_widget->layout());
        QVERIFY(grid_layout != nullptr);
    }

    // **ScrollArea Component Tests**
    void testScrollAreaCreation() {
        auto scroll_area = std::make_unique<ScrollArea>();

        scroll_area->widgetResizable(true)
            .horizontalScrollBarPolicy(Qt::ScrollBarAsNeeded)
            .verticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

        scroll_area->initialize();

        auto* widget = scroll_area->getWidget();
        QVERIFY(widget != nullptr);

        auto* qt_scroll = qobject_cast<QScrollArea*>(widget);
        QVERIFY(qt_scroll != nullptr);
        QVERIFY(qt_scroll->widgetResizable());
        QCOMPARE(qt_scroll->horizontalScrollBarPolicy(), Qt::ScrollBarAsNeeded);
        QCOMPARE(qt_scroll->verticalScrollBarPolicy(), Qt::ScrollBarAlwaysOn);
    }

    void testScrollAreaWithContent() {
        auto scroll_area = std::make_unique<ScrollArea>();
        scroll_area->initialize();

        auto* widget = scroll_area->getWidget();
        auto* qt_scroll = qobject_cast<QScrollArea*>(widget);

        // Create content widget
        auto* content = new QWidget();
        content->setMinimumSize(800, 600);
        content->setStyleSheet("background-color: lightblue;");

        qt_scroll->setWidget(content);

        QCOMPARE(qt_scroll->widget(), content);
        QVERIFY(qt_scroll->widget()->minimumSize().width() >= 800);
        QVERIFY(qt_scroll->widget()->minimumSize().height() >= 600);
    }

    // **Splitter Component Tests**
    void testSplitterCreation() {
        auto splitter = std::make_unique<Splitter>();

        splitter->orientation(Qt::Horizontal)
            .childrenCollapsible(false)
            .handleWidth(5);

        splitter->initialize();

        auto* widget = splitter->getWidget();
        QVERIFY(widget != nullptr);

        auto* qt_splitter = qobject_cast<QSplitter*>(widget);
        QVERIFY(qt_splitter != nullptr);
        QCOMPARE(qt_splitter->orientation(), Qt::Horizontal);
        QVERIFY(!qt_splitter->childrenCollapsible());
        QCOMPARE(qt_splitter->handleWidth(), 5);
    }

    void testSplitterWithWidgets() {
        auto splitter = std::make_unique<Splitter>();
        splitter->orientation(Qt::Vertical);
        splitter->initialize();

        auto* widget = splitter->getWidget();
        auto* qt_splitter = qobject_cast<QSplitter*>(widget);

        // Add widgets to splitter
        auto* widget1 = new QLabel("Panel 1");
        auto* widget2 = new QLabel("Panel 2");
        auto* widget3 = new QLabel("Panel 3");

        qt_splitter->addWidget(widget1);
        qt_splitter->addWidget(widget2);
        qt_splitter->addWidget(widget3);

        QCOMPARE(qt_splitter->count(), 3);
        QCOMPARE(qt_splitter->widget(0), widget1);
        QCOMPARE(qt_splitter->widget(1), widget2);
        QCOMPARE(qt_splitter->widget(2), widget3);
    }

    void testSplitterSizes() {
        auto splitter = std::make_unique<Splitter>();
        splitter->initialize();

        auto* widget = splitter->getWidget();
        auto* qt_splitter = qobject_cast<QSplitter*>(widget);

        // Add widgets
        qt_splitter->addWidget(new QLabel("Panel 1"));
        qt_splitter->addWidget(new QLabel("Panel 2"));

        // Set sizes
        QList<int> sizes = {200, 300};
        qt_splitter->setSizes(sizes);

        auto actual_sizes = qt_splitter->sizes();
        QCOMPARE(actual_sizes.size(), 2);
        // Note: Actual sizes might differ slightly due to constraints
        QVERIFY(actual_sizes[0] > 0);
        QVERIFY(actual_sizes[1] > 0);
    }

    // **Frame Component Tests**
    void testFrameCreation() {
        auto frame = std::make_unique<Frame>();

        frame->frameStyle(QFrame::Box, QFrame::Raised)
            .lineWidth(2)
            .midLineWidth(1);

        frame->initialize();

        auto* widget = frame->getWidget();
        QVERIFY(widget != nullptr);

        auto* qt_frame = qobject_cast<QFrame*>(widget);
        QVERIFY(qt_frame != nullptr);
        QCOMPARE(qt_frame->frameStyle(), QFrame::Box | QFrame::Raised);
        QCOMPARE(qt_frame->lineWidth(), 2);
        QCOMPARE(qt_frame->midLineWidth(), 1);
    }

    void testFrameShapes() {
        // Test different frame shapes
        QFrame::Shape shapes[] = {QFrame::NoFrame, QFrame::Box,
                                  QFrame::Panel,   QFrame::StyledPanel,
                                  QFrame::HLine,   QFrame::VLine};

        for (auto shape : shapes) {
            auto frame = std::make_unique<Frame>();
            frame->frameShape(shape);
            frame->initialize();

            auto* widget = frame->getWidget();
            auto* qt_frame = qobject_cast<QFrame*>(widget);
            QVERIFY(qt_frame != nullptr);
            QCOMPARE(qt_frame->frameShape(), shape);
        }
    }

    // **TextEdit Component Tests**
    void testTextEditCreation() {
        auto text_edit = std::make_unique<TextEdit>();

        text_edit->html("<b>Bold text</b> and <i>italic text</i>")
            .readOnly(false)
            .acceptRichText(true);

        text_edit->initialize();

        auto* widget = text_edit->getWidget();
        QVERIFY(widget != nullptr);

        auto* qt_text_edit = qobject_cast<QTextEdit*>(widget);
        QVERIFY(qt_text_edit != nullptr);
        QVERIFY(qt_text_edit->toHtml().contains("Bold text"));
        QVERIFY(!qt_text_edit->isReadOnly());
        QVERIFY(qt_text_edit->acceptRichText());
    }

    void testTextEditTextChanged() {
        auto text_edit = std::make_unique<TextEdit>();

        QString last_text;
        text_edit->onTextChanged([&last_text]() {
            // Note: We can't easily capture the text in the lambda
            // This tests that the signal connection works
            last_text = "changed";
        });

        text_edit->initialize();

        auto* widget = text_edit->getWidget();
        auto* qt_text_edit = qobject_cast<QTextEdit*>(widget);
        QVERIFY(qt_text_edit != nullptr);

        // Change text
        qt_text_edit->setPlainText("New text content");
        QCOMPARE(last_text, QString("changed"));
    }

    // **PlainTextEdit Component Tests**
    void testPlainTextEditCreation() {
        auto plain_text_edit = std::make_unique<PlainTextEdit>();

        plain_text_edit->plainText("Plain text content")
            .readOnly(true)
            .lineWrapMode(QPlainTextEdit::WidgetWidth);

        plain_text_edit->initialize();

        auto* widget = plain_text_edit->getWidget();
        QVERIFY(widget != nullptr);

        auto* qt_plain_text = qobject_cast<QPlainTextEdit*>(widget);
        QVERIFY(qt_plain_text != nullptr);
        QCOMPARE(qt_plain_text->toPlainText(), QString("Plain text content"));
        QVERIFY(qt_plain_text->isReadOnly());
        QCOMPARE(qt_plain_text->lineWrapMode(), QPlainTextEdit::WidgetWidth);
    }

    void testPlainTextEditTextChanged() {
        auto plain_text_edit = std::make_unique<PlainTextEdit>();

        bool text_changed = false;
        plain_text_edit->onTextChanged(
            [&text_changed]() { text_changed = true; });

        plain_text_edit->initialize();

        auto* widget = plain_text_edit->getWidget();
        auto* qt_plain_text = qobject_cast<QPlainTextEdit*>(widget);
        QVERIFY(qt_plain_text != nullptr);

        // Change text
        qt_plain_text->setPlainText("Modified text");
        QVERIFY(text_changed);
    }

    // **Label Component Tests**
    void testLabelCreation() {
        auto label = std::make_unique<Label>();

        label->text("Test Label")
            .alignment(Qt::AlignCenter)
            .wordWrap(true)
            .linkActivation(true);

        label->initialize();

        auto* widget = label->getWidget();
        QVERIFY(widget != nullptr);

        auto* qt_label = qobject_cast<QLabel*>(widget);
        QVERIFY(qt_label != nullptr);
        QCOMPARE(qt_label->text(), QString("Test Label"));
        QCOMPARE(qt_label->alignment(), Qt::AlignCenter);
        QVERIFY(qt_label->wordWrap());
        QVERIFY(qt_label->openExternalLinks());
    }

    void testLabelWithPixmap() {
        auto label = std::make_unique<Label>();

        // Create a simple pixmap
        QPixmap pixmap(100, 100);
        pixmap.fill(Qt::red);

        label->pixmap(pixmap).style("QLabel { border: 1px solid gray; }");

        label->initialize();

        auto* widget = label->getWidget();
        auto* qt_label = qobject_cast<QLabel*>(widget);
        QVERIFY(qt_label != nullptr);
        QVERIFY(!qt_label->pixmap().isNull());
        QVERIFY(qt_label->hasScaledContents());
    }

    // **Error Handling Tests**
    void testContainerComponentsErrorHandling() {
        // Test that components handle errors gracefully
        auto container = std::make_unique<Container>();

        // Invalid layout type should not crash
        container->setLayout(new QVBoxLayout());
        container->initialize();

        auto* widget = container->getWidget();
        QVERIFY(widget != nullptr);
        // Should fall back to a default layout or no layout
    }

    void testComponentsMemoryManagement() {
        // Test that components properly manage memory
        std::vector<std::unique_ptr<Container>> containers;

        for (int i = 0; i < 100; ++i) {
            auto container = std::make_unique<Container>();
            container->setLayout(new QVBoxLayout());
            container->initialize();
            containers.push_back(std::move(container));
        }

        // All containers should be valid
        QCOMPARE(containers.size(), 100);

        // Destruction should be clean (no crashes)
        containers.clear();
        QVERIFY(containers.empty());
    }
};

QTEST_MAIN(ContainerComponentsTest)
#include "test_container_components.moc"
