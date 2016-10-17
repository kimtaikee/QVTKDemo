#ifndef PIECHARTACTORWINDOW_H
#define PIECHARTACTORWINDOW_H

#include "plotactorwindow.h"

namespace Ui
{
    class PieSettings;
}

class vtkDataObject;
class PieChartActorWindow : public PlotActorWindow
{
    Q_OBJECT
public:
    explicit PieChartActorWindow(QWidget* parent = nullptr);

protected:
    void onGroupBoxChecked(bool checked);
    void onLineEditTextChanged(const QString& text);
    void onCheckBoxStateChanged(int state);
    void onColorChanged(const QColor& clr);
    void onSpinBoxValueChanged(int value);
    void onDoubleSpinBoxValueChanged(double value);

protected:
    void createPlotActor();
    void initTableModel();
    vtkDataObject* createPlotData();
    void render();

private:
    Ui::PieSettings* m_settingsUi = nullptr;
};

#endif // PIECHARTACTORWINDOW_H
