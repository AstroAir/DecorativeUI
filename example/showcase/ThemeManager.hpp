/**
 * @file ThemeManager.hpp
 * @brief Theme management and customization widget
 */

#pragma once

#include <QColorDialog>
#include <QComboBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QVBoxLayout>
#include <QWidget>
#include <memory>

class ThemeManager : public QWidget {
    Q_OBJECT

public:
    explicit ThemeManager(QWidget* parent = nullptr);

signals:
    void themeChanged(const QString& theme_name);

private slots:
    void onThemeSelected();
    void onCustomColorChanged();
    void onPreviewTheme();
    void onApplyTheme();
    void onResetTheme();

private:
    void setupUI();
    void createThemeSelector();
    void createColorCustomizer();
    void createPreview();
    void loadAvailableThemes();
    void applyThemeToPreview(const QString& theme_name);

    QVBoxLayout* main_layout_;
    QGroupBox* selector_group_;
    QGroupBox* customizer_group_;
    QGroupBox* preview_group_;

    QComboBox* theme_combo_;
    QPushButton* preview_button_;
    QPushButton* apply_button_;
    QPushButton* reset_button_;

    QWidget* preview_widget_;
    QString current_theme_;
    QMap<QString, QString> theme_stylesheets_;
};
