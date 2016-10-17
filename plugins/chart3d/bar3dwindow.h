#ifndef BAR3DWINDOW_H
#define BAR3DWINDOW_H

#include "chart3dwindow.h"

class Bar3DWindow : public Chart3DWindow
{
    Q_OBJECT
public:
    explicit Bar3DWindow(QWidget* parent = nullptr);

    virtual void createChart(vtkPolyData* polyData);
    virtual void updateChart();
};

#endif // BAR3DWINDOW_H
