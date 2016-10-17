#ifndef BARCHARTACTORWINDOW_H
#define BARCHARTACTORWINDOW_H

#include "plotactorwindow.h"

namespace Ui
{
    class SpiderSettings;
}

class vtkActor2D;
class BarChartActorWindow : public PlotActorWindow
{
    Q_OBJECT
public:
    explicit BarChartActorWindow(QWidget* parent = nullptr);

protected:
    void createPlotActor();
    void render();
    void layoutActor(vtkActor2D* actor);

private slots:
    void on_titleGroupBox_toggled(bool arg1);
    void on_titleEdit_textChanged(const QString &arg1);
    void on_showLabelsCheckBox_toggled(bool checked);
    void on_showLegendCheckBox_toggled(bool checked);

private:
    Ui::SpiderSettings* m_settingsUi = nullptr;
};

#endif // BARCHARTACTORWINDOW_H
