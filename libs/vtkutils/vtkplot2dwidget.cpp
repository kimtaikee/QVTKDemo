#include "vtkplot2dwidget.h"

#include <vtkutils/vtkutils.h>

#include <vtkChartXY.h>
#include <vtkContextScene.h>
#include <vtkContextView.h>

namespace VtkUtils
{
class VtkPlot2DWidgetPrivate
{
public:
    vtkSmartPointer<vtkChartXY> chart;
};

VtkPlot2DWidget::VtkPlot2DWidget(QWidget* parent) : VtkPlotWidget(parent)
{
    d_ptr = new VtkPlot2DWidgetPrivate;
}

VtkPlot2DWidget::~VtkPlot2DWidget()
{
    delete d_ptr;
}

vtkContextItem* VtkPlot2DWidget::chart() const
{
    vtkInitOnce(d_ptr->chart);
    contextView()->GetScene()->AddItem(d_ptr->chart);
    return d_ptr->chart;
}

} // namespace VtkUtils
