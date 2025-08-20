// Components/FileDialog.cpp
#include "FileDialog.hpp"

namespace DeclarativeUI::Components {

// **Implementation**
FileDialog::FileDialog(QObject* parent)
    : UIElement(parent), file_dialog_widget_(nullptr) {}

FileDialog& FileDialog::fileMode(QFileDialog::FileMode mode) {
    return static_cast<FileDialog&>(
        setProperty("fileMode", static_cast<int>(mode)));
}

FileDialog& FileDialog::acceptMode(QFileDialog::AcceptMode mode) {
    return static_cast<FileDialog&>(
        setProperty("acceptMode", static_cast<int>(mode)));
}

FileDialog& FileDialog::viewMode(QFileDialog::ViewMode mode) {
    return static_cast<FileDialog&>(
        setProperty("viewMode", static_cast<int>(mode)));
}

FileDialog& FileDialog::directory(const QString& directory) {
    return static_cast<FileDialog&>(setProperty("directory", directory));
}

FileDialog& FileDialog::selectFile(const QString& filename) {
    if (file_dialog_widget_) {
        file_dialog_widget_->selectFile(filename);
    }
    return *this;
}

FileDialog& FileDialog::nameFilter(const QString& filter) {
    return static_cast<FileDialog&>(setProperty("nameFilter", filter));
}

FileDialog& FileDialog::nameFilters(const QStringList& filters) {
    return static_cast<FileDialog&>(setProperty("nameFilters", filters));
}

FileDialog& FileDialog::defaultSuffix(const QString& suffix) {
    return static_cast<FileDialog&>(setProperty("defaultSuffix", suffix));
}

FileDialog& FileDialog::options(QFileDialog::Options options) {
    return static_cast<FileDialog&>(
        setProperty("options", static_cast<int>(options)));
}

FileDialog& FileDialog::windowTitle(const QString& title) {
    return static_cast<FileDialog&>(setProperty("windowTitle", title));
}

FileDialog& FileDialog::onFileSelected(
    std::function<void(const QString&)> handler) {
    file_selected_handler_ = std::move(handler);
    return *this;
}

FileDialog& FileDialog::onFilesSelected(
    std::function<void(const QStringList&)> handler) {
    files_selected_handler_ = std::move(handler);
    return *this;
}

FileDialog& FileDialog::onCurrentChanged(
    std::function<void(const QString&)> handler) {
    current_changed_handler_ = std::move(handler);
    return *this;
}

FileDialog& FileDialog::onDirectoryEntered(
    std::function<void(const QString&)> handler) {
    directory_entered_handler_ = std::move(handler);
    return *this;
}

FileDialog& FileDialog::onFilterSelected(
    std::function<void(const QString&)> handler) {
    filter_selected_handler_ = std::move(handler);
    return *this;
}

void FileDialog::initialize() {
    if (!file_dialog_widget_) {
        file_dialog_widget_ = new QFileDialog();
        setWidget(file_dialog_widget_);

        // Connect signals
        if (file_selected_handler_) {
            connect(
                file_dialog_widget_, &QFileDialog::fileSelected, this,
                [this](const QString& file) { file_selected_handler_(file); });
        }

        if (files_selected_handler_) {
            connect(file_dialog_widget_, &QFileDialog::filesSelected, this,
                    [this](const QStringList& files) {
                        files_selected_handler_(files);
                    });
        }

        if (current_changed_handler_) {
            connect(file_dialog_widget_, &QFileDialog::currentChanged, this,
                    [this](const QString& path) {
                        current_changed_handler_(path);
                    });
        }

        if (directory_entered_handler_) {
            connect(file_dialog_widget_, &QFileDialog::directoryEntered, this,
                    [this](const QString& directory) {
                        directory_entered_handler_(directory);
                    });
        }

        if (filter_selected_handler_) {
            connect(file_dialog_widget_, &QFileDialog::filterSelected, this,
                    [this](const QString& filter) {
                        filter_selected_handler_(filter);
                    });
        }
    }
}

int FileDialog::exec() {
    return file_dialog_widget_ ? file_dialog_widget_->exec()
                               : QDialog::Rejected;
}

void FileDialog::show() {
    if (file_dialog_widget_) {
        file_dialog_widget_->show();
    }
}

void FileDialog::accept() {
    if (file_dialog_widget_) {
        // accept() and done() are protected, so we just close the dialog
        file_dialog_widget_->close();
    }
}

void FileDialog::reject() {
    if (file_dialog_widget_) {
        file_dialog_widget_->reject();
    }
}

QString FileDialog::selectedFile() const {
    if (file_dialog_widget_) {
        QStringList files = file_dialog_widget_->selectedFiles();
        return files.isEmpty() ? QString() : files.first();
    }
    return QString();
}

QStringList FileDialog::selectedFiles() const {
    return file_dialog_widget_ ? file_dialog_widget_->selectedFiles()
                               : QStringList();
}

QString FileDialog::getDirectory() const {
    return file_dialog_widget_ ? file_dialog_widget_->directory().absolutePath()
                               : QString();
}

QStringList FileDialog::getNameFilters() const {
    return file_dialog_widget_ ? file_dialog_widget_->nameFilters()
                               : QStringList();
}

// **Static convenience methods**
QString FileDialog::getOpenFileName(QWidget* parent, const QString& caption,
                                    const QString& dir, const QString& filter,
                                    QString* selectedFilter,
                                    QFileDialog::Options options) {
    return QFileDialog::getOpenFileName(parent, caption, dir, filter,
                                        selectedFilter, options);
}

QStringList FileDialog::getOpenFileNames(QWidget* parent,
                                         const QString& caption,
                                         const QString& dir,
                                         const QString& filter,
                                         QString* selectedFilter,
                                         QFileDialog::Options options) {
    return QFileDialog::getOpenFileNames(parent, caption, dir, filter,
                                         selectedFilter, options);
}

QString FileDialog::getSaveFileName(QWidget* parent, const QString& caption,
                                    const QString& dir, const QString& filter,
                                    QString* selectedFilter,
                                    QFileDialog::Options options) {
    return QFileDialog::getSaveFileName(parent, caption, dir, filter,
                                        selectedFilter, options);
}

QString FileDialog::getExistingDirectory(QWidget* parent,
                                         const QString& caption,
                                         const QString& dir,
                                         QFileDialog::Options options) {
    return QFileDialog::getExistingDirectory(parent, caption, dir, options);
}

}  // namespace DeclarativeUI::Components
