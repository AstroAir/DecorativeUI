// Components/FileDialog.hpp
#pragma once
#include <QFileDialog>
#include <QUrl>
#include <QWidget>

#include "../Core/UIElement.hpp"

namespace DeclarativeUI::Components {

class FileDialog : public Core::UIElement {
    Q_OBJECT

public:
    explicit FileDialog(QObject* parent = nullptr);

    // **Fluent interface for file dialog**
    FileDialog& fileMode(QFileDialog::FileMode mode);
    FileDialog& acceptMode(QFileDialog::AcceptMode mode);
    FileDialog& viewMode(QFileDialog::ViewMode mode);
    FileDialog& directory(const QString& directory);
    FileDialog& selectFile(const QString& filename);
    FileDialog& nameFilter(const QString& filter);
    FileDialog& nameFilters(const QStringList& filters);
    FileDialog& defaultSuffix(const QString& suffix);
    FileDialog& options(QFileDialog::Options options);
    FileDialog& windowTitle(const QString& title);
    FileDialog& onFileSelected(std::function<void(const QString&)> handler);
    FileDialog& onFilesSelected(
        std::function<void(const QStringList&)> handler);
    FileDialog& onCurrentChanged(std::function<void(const QString&)> handler);
    FileDialog& onDirectoryEntered(std::function<void(const QString&)> handler);
    FileDialog& onFilterSelected(std::function<void(const QString&)> handler);

    void initialize() override;
    int exec();
    void show();
    void accept();
    void reject();
    QString selectedFile() const;
    QStringList selectedFiles() const;
    QString getDirectory() const;
    QStringList getNameFilters() const;

    // **Static convenience methods**
    static QString getOpenFileName(
        QWidget* parent = nullptr, const QString& caption = QString(),
        const QString& dir = QString(), const QString& filter = QString(),
        QString* selectedFilter = nullptr,
        QFileDialog::Options options = QFileDialog::Options());

    static QStringList getOpenFileNames(
        QWidget* parent = nullptr, const QString& caption = QString(),
        const QString& dir = QString(), const QString& filter = QString(),
        QString* selectedFilter = nullptr,
        QFileDialog::Options options = QFileDialog::Options());

    static QString getSaveFileName(
        QWidget* parent = nullptr, const QString& caption = QString(),
        const QString& dir = QString(), const QString& filter = QString(),
        QString* selectedFilter = nullptr,
        QFileDialog::Options options = QFileDialog::Options());

    static QString getExistingDirectory(
        QWidget* parent = nullptr, const QString& caption = QString(),
        const QString& dir = QString(),
        QFileDialog::Options options = QFileDialog::ShowDirsOnly);

private:
    QFileDialog* file_dialog_widget_;
    std::function<void(const QString&)> file_selected_handler_;
    std::function<void(const QStringList&)> files_selected_handler_;
    std::function<void(const QString&)> current_changed_handler_;
    std::function<void(const QString&)> directory_entered_handler_;
    std::function<void(const QString&)> filter_selected_handler_;
};

}  // namespace DeclarativeUI::Components
