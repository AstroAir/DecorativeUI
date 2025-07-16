// HotReload/HotReloadManager.hpp
#pragma once
#include <QObject>
#include <QString>
#include <QWidget>
#include <functional>
#include <memory>
#include <unordered_map>
#include "../Exceptions/UIExceptions.hpp"
#include "../JSON/JSONUILoader.hpp"
#include "FileWatcher.hpp"


namespace DeclarativeUI::HotReload {

// **Hot reload system for UI files**
class HotReloadManager : public QObject {
    Q_OBJECT

public:
    explicit HotReloadManager(QObject* parent = nullptr);
    ~HotReloadManager() override = default;

    // **Register UI file for hot reloading**
    void registerUIFile(const QString& file_path, QWidget* target_widget);

    // **Register UI directory**
    void registerUIDirectory(const QString& directory_path,
                             bool recursive = true);

    // **Enable/disable hot reloading**
    void setEnabled(bool enabled);
    [[nodiscard]] bool isEnabled() const noexcept { return enabled_; }

    // **Configuration**
    void setReloadDelay(int milliseconds);
    void setFileFilters(const QStringList& filters);

    // **Manual reload**
    void reloadFile(const QString& file_path);
    void reloadAll();

    // **Cleanup**
    void unregisterUIFile(const QString& file_path);
    void unregisterAll();

    // **Error handling**
    void setErrorHandler(
        std::function<void(const QString&, const QString&)> handler);

signals:
    void reloadStarted(const QString& file_path);
    void reloadCompleted(const QString& file_path);
    void reloadFailed(const QString& file_path, const QString& error);
    void hotReloadEnabled(bool enabled);

private slots:
    void onFileChanged(const QString& file_path);
    void onFileAdded(const QString& file_path);
    void onFileRemoved(const QString& file_path);

private:
    struct UIFileInfo {
        QWidget* target_widget = nullptr;
        QString parent_layout_position;
        QWidget* parent_widget = nullptr;
        std::unique_ptr<QWidget> backup_widget;
        QDateTime last_reload;
    };

    std::unique_ptr<FileWatcher> file_watcher_;
    std::unique_ptr<JSON::JSONUILoader> ui_loader_;

    std::unordered_map<QString, UIFileInfo> registered_files_;

    bool enabled_ = true;
    int reload_delay_ = 100;  // milliseconds

    std::function<void(const QString&, const QString&)> error_handler_;

    void performReload(const QString& file_path);
    void replaceWidget(const QString& file_path,
                       std::unique_ptr<QWidget> new_widget);
    void createBackup(const QString& file_path);
    void restoreBackup(const QString& file_path);

    bool shouldReload(const QString& file_path) const;
    bool validateWidget(QWidget* widget) const;
    void optimizeWidget(QWidget* widget);
    void setupUILoader();
};

}  // namespace DeclarativeUI::HotReload
