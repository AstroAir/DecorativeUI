/**
 * @file 04_basic_hot_reload.cpp
 * @brief Basic hot reload functionality demonstration
 * 
 * This example demonstrates:
 * - Setting up HotReloadManager
 * - Watching JSON UI files for changes
 * - Automatic UI reloading on file changes
 * - Hot reload event handling
 * 
 * Learning objectives:
 * - Understand hot reload setup and configuration
 * - Learn how to watch files for changes
 * - See automatic UI updates during development
 */

#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QWidget>
#include <QMessageBox>
#include <QDebug>
#include <QTimer>
#include <QFileInfo>

// Include DeclarativeUI hot reload
#include "HotReload/HotReloadManager.hpp"
#include "JSON/JSONUILoader.hpp"

using namespace DeclarativeUI;

/**
 * @brief Basic hot reload demonstration
 */
class BasicHotReloadApp : public QObject {
    Q_OBJECT

public:
    BasicHotReloadApp(QObject* parent = nullptr) : QObject(parent) {
        setupUILoader();
        createUI();
        setupHotReload();
    }

    void show() {
        if (main_widget_) {
            main_widget_->show();
        }
    }

private slots:
    void onReloadStarted(const QString& file_path) {
        qDebug() << "🔄 Hot reload started for:" << file_path;
        
        if (status_label_) {
            status_label_->setText("🔄 Reloading UI...");
            status_label_->setStyleSheet("color: #f39c12; font-weight: bold;");
        }
    }

    void onReloadCompleted(const QString& file_path) {
        qDebug() << "✅ Hot reload completed for:" << file_path;
        
        if (status_label_) {
            status_label_->setText("✅ UI reloaded successfully!");
            status_label_->setStyleSheet("color: #27ae60; font-weight: bold;");
            
            // Clear status after 3 seconds
            QTimer::singleShot(3000, [this]() {
                if (status_label_) {
                    status_label_->setText("👀 Watching for file changes...");
                    status_label_->setStyleSheet("color: #3498db; font-style: italic;");
                }
            });
        }

        // Show notification
        if (main_widget_) {
            QMessageBox::information(
                main_widget_.get(),
                "Hot Reload",
                QString("🔥 UI successfully reloaded!\n\nFile: %1").arg(file_path)
            );
        }
    }

    void onReloadFailed(const QString& file_path, const QString& error) {
        qCritical() << "❌ Hot reload failed for:" << file_path << "Error:" << error;
        
        if (status_label_) {
            status_label_->setText("❌ Reload failed!");
            status_label_->setStyleSheet("color: #e74c3c; font-weight: bold;");
        }

        if (main_widget_) {
            QMessageBox::warning(
                main_widget_.get(),
                "Hot Reload Failed",
                QString("❌ Failed to reload UI:\n\nFile: %1\nError: %2").arg(file_path, error)
            );
        }
    }

    void onTestButtonClicked() {
        QMessageBox::information(
            main_widget_.get(),
            "Hot Reload Test",
            "🎉 This button works!\n\n"
            "Try editing the JSON file to:\n"
            "• Change button text\n"
            "• Modify colors\n"
            "• Add new components\n"
            "• Change layout\n\n"
            "The UI will update automatically!"
        );
    }

    void onToggleHotReloadClicked() {
        if (hot_reload_manager_) {
            bool enabled = hot_reload_manager_->isEnabled();
            hot_reload_manager_->setEnabled(!enabled);
            
            QString status = enabled ? "disabled" : "enabled";
            QString emoji = enabled ? "⏸️" : "▶️";
            
            qDebug() << "Hot reload" << status;
            
            if (status_label_) {
                status_label_->setText(QString("%1 Hot reload %2").arg(emoji, status));
                status_label_->setStyleSheet(enabled ? "color: #e74c3c;" : "color: #27ae60;");
            }
        }
    }

    void onManualReloadClicked() {
        if (hot_reload_manager_) {
            hot_reload_manager_->reloadAll();
            qDebug() << "Manual reload triggered";
        }
    }

private:
    std::unique_ptr<QWidget> main_widget_;
    std::unique_ptr<JSON::JSONUILoader> ui_loader_;
    std::unique_ptr<HotReload::HotReloadManager> hot_reload_manager_;
    
    QLabel* status_label_ = nullptr;

    void setupUILoader() {
        ui_loader_ = std::make_unique<JSON::JSONUILoader>();

        // Register event handlers
        ui_loader_->registerEventHandler("testButton", [this]() {
            onTestButtonClicked();
        });

        ui_loader_->registerEventHandler("toggleHotReload", [this]() {
            onToggleHotReloadClicked();
        });

        ui_loader_->registerEventHandler("manualReload", [this]() {
            onManualReloadClicked();
        });

        qDebug() << "✅ UI loader configured";
    }

    void createUI() {
        try {
            QString ui_file = "resources/hot_reload_ui.json";
            
            if (!QFileInfo::exists(ui_file)) {
                qWarning() << "JSON file not found:" << ui_file;
                main_widget_ = createFallbackUI();
                return;
            }

            main_widget_ = ui_loader_->loadFromFile(ui_file);
            
            if (!main_widget_) {
                throw std::runtime_error("Failed to load UI from JSON");
            }

            main_widget_->setWindowTitle("04 - Basic Hot Reload | DeclarativeUI");
            
            // Find status label for updates
            status_label_ = main_widget_->findChild<QLabel*>("statusLabel");
            
            qDebug() << "✅ UI loaded from JSON:" << ui_file;

        } catch (const std::exception& e) {
            qCritical() << "UI creation failed:" << e.what();
            main_widget_ = createFallbackUI();
        }
    }

    void setupHotReload() {
        try {
            hot_reload_manager_ = std::make_unique<HotReload::HotReloadManager>();

            // Connect hot reload signals
            connect(hot_reload_manager_.get(), &HotReload::HotReloadManager::reloadStarted,
                    this, &BasicHotReloadApp::onReloadStarted);
            
            connect(hot_reload_manager_.get(), &HotReload::HotReloadManager::reloadCompleted,
                    this, &BasicHotReloadApp::onReloadCompleted);
            
            connect(hot_reload_manager_.get(), &HotReload::HotReloadManager::reloadFailed,
                    this, &BasicHotReloadApp::onReloadFailed);

            // Register UI file for hot reloading
            QString ui_file = "resources/hot_reload_ui.json";
            if (QFileInfo::exists(ui_file)) {
                hot_reload_manager_->registerUIFile(ui_file, main_widget_.get());
                qDebug() << "🔥 Hot reload enabled for:" << ui_file;
                
                if (status_label_) {
                    status_label_->setText("👀 Watching for file changes...");
                    status_label_->setStyleSheet("color: #3498db; font-style: italic;");
                }
            } else {
                qWarning() << "Cannot enable hot reload - UI file not found:" << ui_file;
            }

        } catch (const std::exception& e) {
            qWarning() << "❌ Hot reload setup failed:" << e.what();
        }
    }

    std::unique_ptr<QWidget> createFallbackUI() {
        auto widget = std::make_unique<QWidget>();
        widget->setWindowTitle("04 - Basic Hot Reload (Fallback) | DeclarativeUI");
        widget->setMinimumSize(500, 400);

        auto layout = new QVBoxLayout(widget.get());
        layout->setSpacing(20);
        layout->setContentsMargins(20, 20, 20, 20);

        // Header
        auto header = new QLabel("🔥 Basic Hot Reload Example");
        header->setStyleSheet("font-size: 20px; font-weight: bold; color: #2c3e50;");
        header->setAlignment(Qt::AlignCenter);
        layout->addWidget(header);

        // Status
        status_label_ = new QLabel("⚠️ Fallback UI (JSON file not found)");
        status_label_->setStyleSheet("color: #e67e22; font-style: italic;");
        status_label_->setAlignment(Qt::AlignCenter);
        layout->addWidget(status_label_);

        // Instructions
        auto instructions = new QTextEdit();
        instructions->setMaximumHeight(150);
        instructions->setReadOnly(true);
        instructions->setHtml(
            "<h4>Hot Reload Instructions:</h4>"
            "<ol>"
            "<li>Create the file <code>resources/hot_reload_ui.json</code></li>"
            "<li>Restart the application</li>"
            "<li>Edit the JSON file while the app is running</li>"
            "<li>Watch the UI update automatically!</li>"
            "</ol>"
            "<p><b>Note:</b> This fallback UI doesn't support hot reload.</p>"
        );
        layout->addWidget(instructions);

        layout->addStretch();

        // Test button
        auto test_button = new QPushButton("🧪 Test Button");
        test_button->setStyleSheet(
            "QPushButton { background-color: #3498db; color: white; "
            "padding: 10px 20px; border: none; border-radius: 5px; font-size: 14px; }"
            "QPushButton:hover { background-color: #2980b9; }"
        );
        connect(test_button, &QPushButton::clicked, this, &BasicHotReloadApp::onTestButtonClicked);
        layout->addWidget(test_button);

        qDebug() << "✅ Fallback UI created";
        return widget;
    }
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    app.setApplicationName("DeclarativeUI Basic Hot Reload");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("DeclarativeUI Examples");

    try {
        qDebug() << "🚀 Starting Basic Hot Reload example...";
        
        BasicHotReloadApp hot_reload_app;
        hot_reload_app.show();

        qDebug() << "💡 This example demonstrates:";
        qDebug() << "   - Hot reload setup and configuration";
        qDebug() << "   - File watching for automatic updates";
        qDebug() << "   - Hot reload event handling";
        qDebug() << "";
        qDebug() << "🔥 Try editing 'resources/hot_reload_ui.json' to see hot reload in action!";

        return app.exec();

    } catch (const std::exception& e) {
        qCritical() << "❌ Application error:" << e.what();
        return -1;
    }
}

#include "04_basic_hot_reload.moc"
