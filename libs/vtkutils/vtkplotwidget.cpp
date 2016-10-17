#include "vtkplotwidget.h"

#include <vtkutils/vtkutils.h>

#include <vtkContextView.h>
#include <vtkChartXY.h>
#include <vtkChartXYZ.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkContextScene.h>
#include <vtkAxis.h>
#include <vtkTable.h>
#include <vtkVariant.h>
#include <vtkFloatArray.h>
#include <vtkPlot.h>
#include <vtkPlotSurface.h>
#include <vtkPlotPoints3D.h>
#include <vtkPlotLine3D.h>

namespace VtkUtils
{

class VtkPlotWidgetPrivate
{
public:
    ~VtkPlotWidgetPrivate();

    vtkContextView* contextView;
};

VtkPlotWidgetPrivate::~VtkPlotWidgetPrivate()
{
    VtkUtils::vtkSafeDelete(contextView);
}

VtkPlotWidget::VtkPlotWidget(QWidget* parent) : QVTKWidget(parent)
{
    setWindowTitle(tr("ChartXY"));
    d_ptr = new VtkPlotWidgetPrivate;
    init();
}

VtkPlotWidget::~VtkPlotWidget()
{
    delete d_ptr;
}

void VtkPlotWidget::init()
{
    d_ptr->contextView = vtkContextView::New();
    d_ptr->contextView->SetRenderWindow(this->GetRenderWindow());
    d_ptr->contextView->GetRenderWindow()->SetMultiSamples(0);
    d_ptr->contextView->GetRenderWindow()->Render();
}

vtkContextView* VtkPlotWidget::contextView() const
{
    return d_ptr->contextView;
}


} // namespace VtkUtils
