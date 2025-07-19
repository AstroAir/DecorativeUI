// Components/ProgressBar.hpp
#pragma once
#include <QProgressBar>

#include "../Core/UIElement.hpp"

namespace DeclarativeUI::Components {

class ProgressBar : public Core::UIElement {
    Q_OBJECT

public:
    explicit ProgressBar(QObject* parent = nullptr);

    // **Fluent interface for progress bar**
    ProgressBar& minimum(int min);
    ProgressBar& maximum(int max);
    ProgressBar& value(int value);
    ProgressBar& orientation(Qt::Orientation orientation);
    ProgressBar& textVisible(bool visible);
    ProgressBar& format(const QString& format);
    ProgressBar& invertedAppearance(bool inverted);
    ProgressBar& style(const QString& stylesheet);

    void initialize() override;
    int getValue() const;
    void setValue(int value);
    int getMinimum() const;
    int getMaximum() const;
    void reset();

private:
    QProgressBar* progress_bar_widget_;
};



}  // namespace DeclarativeUI::Components
