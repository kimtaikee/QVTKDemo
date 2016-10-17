#ifndef XYPLOTACTORWINDOW_H
#define XYPLOTACTORWINDOW_H

#include "plotactorwindow.h"

class vtkDataObject;
class XYPlotActorWindow : public PlotActorWindow
{
    Q_OBJECT
public:
    explicit XYPlotActorWindow(QWidget* parent = nullptr);

protected:
    void createPlotActor();
    void render();
};

#endif // XYPLOTACTORWINDOW_H
