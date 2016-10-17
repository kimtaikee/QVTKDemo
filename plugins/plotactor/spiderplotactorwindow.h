#ifndef SPIDERPLOTACTORWINDOW_H
#define SPIDERPLOTACTORWINDOW_H

#include "plotactorwindow.h"

namespace Ui
{
    class SpiderSettings;
}

class SpiderPlotActorWindow : public PlotActorWindow
{
    Q_OBJECT
public:
    explicit SpiderPlotActorWindow(QWidget* parent = nullptr);

protected:
    void createPlotActor();
    vtkDataObject* createPlotData();
    void render();
    void initTableModel();

private slots:
    void on_titleGroupBox_toggled(bool arg1);
    void on_titleEdit_textChanged(const QString &arg1);
    void on_showLabelsCheckBox_toggled(bool checked);
    void on_showLegendCheckBox_toggled(bool checked);

private:
    Ui::SpiderSettings* m_settingsUi = nullptr;
};

#endif // SPIDERPLOTACTORWINDOW_H
