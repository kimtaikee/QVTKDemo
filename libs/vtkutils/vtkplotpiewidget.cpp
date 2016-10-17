#include "vtkplotpiewidget.h"

#include <vtkutils/vtkutils.h>

#include <vtkChartPie.h>
#include <vtkContextScene.h>
#include <vtkContextView.h>

namespace VtkUtils
{

class VtkPlotPieWidgetPrivate
{
public:
    vtkSmartPointer<vtkChartPie> chart;
};

VtkPlotPieWidget::VtkPlotPieWidget(QWidget* parent) : VtkPlotWidget(parent)
{
    d_ptr = new VtkPlotPieWidgetPrivate;
}

VtkPlotPieWidget::~VtkPlotPieWidget()
{
    delete d_ptr;
}

vtkContextItem* VtkPlotPieWidget::chart() const
{
    vtkInitOnce(d_ptr->chart);
    contextView()->GetScene()->AddItem(d_ptr->chart);
    return d_ptr->chart;
}

} // namespace VtkUtils
