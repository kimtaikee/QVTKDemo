#include "vtkplot3dwidget.h"
#include "vtkutils.h"

#include <vtkChartXYZ.h>
#include <vtkContextScene.h>
#include <vtkContextView.h>

namespace VtkUtils
{
class VtkPlot3DWidgetPrivate
{
public:
    vtkSmartPointer<vtkChartXYZ> chart;
};

VtkPlot3DWidget::VtkPlot3DWidget(QWidget* parent) : VtkPlotWidget(parent)
{
    d_ptr = new VtkPlot3DWidgetPrivate;
}

VtkPlot3DWidget::~VtkPlot3DWidget()
{
    delete d_ptr;
}

vtkContextItem* VtkPlot3DWidget::chart() const
{
    vtkInitOnce(d_ptr->chart);
    static const int Spacing = 100;
    d_ptr->chart->SetGeometry(vtkRectf(Spacing, Spacing, width() - Spacing, height() - Spacing));
    contextView()->GetScene()->AddItem(d_ptr->chart);
    return d_ptr->chart;
}

} // namespace VtkUtils
