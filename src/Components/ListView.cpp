// Components/ListView.cpp
#include "ListView.hpp"

namespace DeclarativeUI::Components {

// **Implementation**
ListView::ListView(QObject* parent)
    : UIElement(parent), list_view_widget_(nullptr), string_list_model_(nullptr) {}

ListView& ListView::model(QAbstractItemModel* model) {
    if (list_view_widget_ && model) {
        list_view_widget_->setModel(model);
    }
    return *this;
}

ListView& ListView::stringListModel(const QStringList& strings) {
    if (!string_list_model_) {
        string_list_model_ = new QStringListModel(this);
    }
    string_list_model_->setStringList(strings);
    if (list_view_widget_) {
        list_view_widget_->setModel(string_list_model_);
    }
    return *this;
}

ListView& ListView::standardItemModel(QStandardItemModel* model) {
    if (list_view_widget_ && model) {
        list_view_widget_->setModel(model);
    }
    return *this;
}

ListView& ListView::selectionMode(QAbstractItemView::SelectionMode mode) {
    return static_cast<ListView&>(setProperty("selectionMode", static_cast<int>(mode)));
}

ListView& ListView::selectionBehavior(QAbstractItemView::SelectionBehavior behavior) {
    return static_cast<ListView&>(setProperty("selectionBehavior", static_cast<int>(behavior)));
}

ListView& ListView::editTriggers(QAbstractItemView::EditTriggers triggers) {
    return static_cast<ListView&>(setProperty("editTriggers", static_cast<int>(triggers)));
}

ListView& ListView::dragDropMode(QAbstractItemView::DragDropMode mode) {
    return static_cast<ListView&>(setProperty("dragDropMode", static_cast<int>(mode)));
}

ListView& ListView::defaultDropAction(Qt::DropAction action) {
    return static_cast<ListView&>(setProperty("defaultDropAction", static_cast<int>(action)));
}

ListView& ListView::alternatingRowColors(bool enable) {
    return static_cast<ListView&>(setProperty("alternatingRowColors", enable));
}

ListView& ListView::sortingEnabled(bool enable) {
    return static_cast<ListView&>(setProperty("sortingEnabled", enable));
}

ListView& ListView::uniformItemSizes(bool uniform) {
    return static_cast<ListView&>(setProperty("uniformItemSizes", uniform));
}

ListView& ListView::wordWrap(bool wrap) {
    return static_cast<ListView&>(setProperty("wordWrap", wrap));
}

ListView& ListView::spacing(int spacing) {
    return static_cast<ListView&>(setProperty("spacing", spacing));
}

ListView& ListView::gridSize(const QSize& size) {
    return static_cast<ListView&>(setProperty("gridSize", size));
}

ListView& ListView::viewMode(QListView::ViewMode mode) {
    return static_cast<ListView&>(setProperty("viewMode", static_cast<int>(mode)));
}

ListView& ListView::movement(QListView::Movement movement) {
    return static_cast<ListView&>(setProperty("movement", static_cast<int>(movement)));
}

ListView& ListView::flow(QListView::Flow flow) {
    return static_cast<ListView&>(setProperty("flow", static_cast<int>(flow)));
}

ListView& ListView::resizeMode(QListView::ResizeMode mode) {
    return static_cast<ListView&>(setProperty("resizeMode", static_cast<int>(mode)));
}

ListView& ListView::layoutMode(QListView::LayoutMode mode) {
    return static_cast<ListView&>(setProperty("layoutMode", static_cast<int>(mode)));
}

ListView& ListView::batchSize(int batchSize) {
    return static_cast<ListView&>(setProperty("batchSize", batchSize));
}

ListView& ListView::onClicked(std::function<void(const QModelIndex&)> handler) {
    clicked_handler_ = std::move(handler);
    return *this;
}

ListView& ListView::onDoubleClicked(std::function<void(const QModelIndex&)> handler) {
    double_clicked_handler_ = std::move(handler);
    return *this;
}

ListView& ListView::onPressed(std::function<void(const QModelIndex&)> handler) {
    pressed_handler_ = std::move(handler);
    return *this;
}

ListView& ListView::onActivated(std::function<void(const QModelIndex&)> handler) {
    activated_handler_ = std::move(handler);
    return *this;
}

ListView& ListView::onEntered(std::function<void(const QModelIndex&)> handler) {
    entered_handler_ = std::move(handler);
    return *this;
}

ListView& ListView::onViewportEntered(std::function<void()> handler) {
    viewport_entered_handler_ = std::move(handler);
    return *this;
}

ListView& ListView::onIconSizeChanged(std::function<void(const QSize&)> handler) {
    icon_size_changed_handler_ = std::move(handler);
    return *this;
}

ListView& ListView::style(const QString& stylesheet) {
    return static_cast<ListView&>(setProperty("styleSheet", stylesheet));
}

void ListView::initialize() {
    if (!list_view_widget_) {
        list_view_widget_ = new QListView();
        setWidget(list_view_widget_);

        // Connect signals
        if (clicked_handler_) {
            connect(list_view_widget_, &QListView::clicked, this,
                    [this](const QModelIndex& index) { clicked_handler_(index); });
        }

        if (double_clicked_handler_) {
            connect(list_view_widget_, &QListView::doubleClicked, this,
                    [this](const QModelIndex& index) { double_clicked_handler_(index); });
        }

        if (pressed_handler_) {
            connect(list_view_widget_, &QListView::pressed, this,
                    [this](const QModelIndex& index) { pressed_handler_(index); });
        }

        if (activated_handler_) {
            connect(list_view_widget_, &QListView::activated, this,
                    [this](const QModelIndex& index) { activated_handler_(index); });
        }

        if (entered_handler_) {
            connect(list_view_widget_, &QListView::entered, this,
                    [this](const QModelIndex& index) { entered_handler_(index); });
        }

        if (viewport_entered_handler_) {
            connect(list_view_widget_, &QListView::viewportEntered, this,
                    [this]() { viewport_entered_handler_(); });
        }

        if (icon_size_changed_handler_) {
            connect(list_view_widget_, &QListView::iconSizeChanged, this,
                    [this](const QSize& size) { icon_size_changed_handler_(size); });
        }
    }
}

QAbstractItemModel* ListView::getModel() const {
    return list_view_widget_ ? list_view_widget_->model() : nullptr;
}

QModelIndex ListView::getCurrentIndex() const {
    return list_view_widget_ ? list_view_widget_->currentIndex() : QModelIndex();
}

QModelIndexList ListView::getSelectedIndexes() const {
    return list_view_widget_ ? list_view_widget_->selectionModel()->selectedIndexes() : QModelIndexList();
}

void ListView::setCurrentIndex(const QModelIndex& index) {
    if (list_view_widget_) {
        list_view_widget_->setCurrentIndex(index);
    }
}

void ListView::clearSelection() {
    if (list_view_widget_) {
        list_view_widget_->clearSelection();
    }
}

void ListView::selectAll() {
    if (list_view_widget_) {
        list_view_widget_->selectAll();
    }
}

void ListView::scrollTo(const QModelIndex& index, QAbstractItemView::ScrollHint hint) {
    if (list_view_widget_) {
        list_view_widget_->scrollTo(index, hint);
    }
}

void ListView::addItem(const QString& text) {
    if (!string_list_model_) {
        string_list_model_ = new QStringListModel(this);
        if (list_view_widget_) {
            list_view_widget_->setModel(string_list_model_);
        }
    }
    QStringList list = string_list_model_->stringList();
    list.append(text);
    string_list_model_->setStringList(list);
}

void ListView::addItems(const QStringList& texts) {
    if (!string_list_model_) {
        string_list_model_ = new QStringListModel(this);
        if (list_view_widget_) {
            list_view_widget_->setModel(string_list_model_);
        }
    }
    QStringList list = string_list_model_->stringList();
    list.append(texts);
    string_list_model_->setStringList(list);
}

void ListView::removeItem(int row) {
    if (string_list_model_ && row >= 0 && row < string_list_model_->rowCount()) {
        string_list_model_->removeRow(row);
    }
}

void ListView::clear() {
    if (string_list_model_) {
        string_list_model_->setStringList(QStringList());
    }
}

int ListView::count() const {
    return string_list_model_ ? string_list_model_->rowCount() : 0;
}

QString ListView::itemText(int row) const {
    if (string_list_model_ && row >= 0 && row < string_list_model_->rowCount()) {
        return string_list_model_->stringList().at(row);
    }
    return QString();
}

void ListView::setItemText(int row, const QString& text) {
    if (string_list_model_ && row >= 0 && row < string_list_model_->rowCount()) {
        QStringList list = string_list_model_->stringList();
        list[row] = text;
        string_list_model_->setStringList(list);
    }
}

}  // namespace DeclarativeUI::Components
