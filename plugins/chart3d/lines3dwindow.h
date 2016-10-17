#ifndef LINES3DWINDOW_H
#define LINES3DWINDOW_H

#include "chart3dwindow.h"

class Lines3DWindow : public Chart3DWindow
{
    Q_OBJECT
public:
    explicit Lines3DWindow(QWidget* parent = nullptr);

protected:
    void createChart(vtkPolyData* polydata);
    void updateChart();
};

#endif // LINES3DWINDOW_H
