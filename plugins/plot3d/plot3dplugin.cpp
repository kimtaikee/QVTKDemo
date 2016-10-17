#include "plot3dplugin.h"
#include "plotline3dwindow.h"
#include "plotpoints3dwindow.h"
#include "plotsurfacewindow.h"

#include <vtkutils/vtkplot3dwidget.h>

#include <vtkChartXYZ.h>
#include <vtkPlotSurface.h>
#include <vtkSmartPointer.h>
#include <vtkFloatArray.h>
#include <vtkTable.h>
#include <vtkPlotLine3D.h>
#include <vtkPlotPoints3D.h>

#include <QMap>

Plot3DPlugin::Plot3DPlugin()
{

}

Plot3DPlugin::~Plot3DPlugin()
{

}

QString Plot3DPlugin::category() const
{
    return QObject::tr("Plot3D");
}

QList<QWidget*> Plot3DPlugin::examples() const
{
    QList<QWidget*> widgetList;

//    VtkUtils::VtkPlot3DWidget* plot3d = new VtkUtils::VtkPlot3DWidget();
//    plot3d->setWindowTitle(tr("Plot Surface"));

//    chart->AddPlot(plot.GetPointer());

//    vtkChartXYZ* chart = vtkChartXYZ::SafeDownCast(plot3d->chart());
//    chart->AddPlot(plot.GetPointer());

//    widgetList.append(plot3d);
    widgetList.append(new PlotPoints3DWindow);
    widgetList.append(new PlotLine3DWindow);
    widgetList.append(new PlotSurfaceWindow);

    return widgetList;
}

QStringList Plot3DPlugin::keywords() const
{
    QStringList words;
    words << "vtkPlot3D's derivatives" << "PlotLine3D" << "PlotPoints3D" << "PlotSurface";
    return words;
}

