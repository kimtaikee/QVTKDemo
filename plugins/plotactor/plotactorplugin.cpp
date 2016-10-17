#include "plotactorplugin.h"
#include "plotactorwindow.h"
#include "xyplotactorwindow.h"
#include "piechartactorwindow.h"
#include "barchartactorwindow.h"
#include "spiderplotactorwindow.h"

#include <vtkutils/vtkwidget.h>
#include <vtkutils/vtkutils.h>

#include <vtkBarChartActor.h>
#include <vtkPieChartActor.h>
#include <vtkXYPlotActor.h>
#include <vtkSpiderPlotActor.h>

#include <vtkDoubleArray.h>
#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkFieldData.h>
#include <vtkDoubleArray.h>
#include <vtkColorSeries.h>
#include <vtkColor.h>

#include <QMap>

const int DataSize = 10;

PlotActorPlugin::PlotActorPlugin()
{

}

PlotActorPlugin::~PlotActorPlugin()
{

}

QString PlotActorPlugin::category() const
{
    return QObject::tr("Plot Actor");
}

static vtkDataObject*  createPlotData()
{
    vtkFieldData* field = vtkFieldData::New();
    vtkDataObject* data = vtkDataObject::New();

//    VTK_CREATE(vtkDoubleArray, xArr);
//    xArr->SetName("X");
    VTK_CREATE(vtkDoubleArray, scalarArr);
    scalarArr->SetName("Scalar");

    for (int i = 0; i < DataSize; ++i) {
//        xArr->InsertTuple1(i, i);
        scalarArr->InsertTuple1(i, Utils::random(0, 10));
    }

//    field->AddArray(xArr);
    field->AddArray(scalarArr);
    data->SetFieldData(field);

    return data;
}

static VtkUtils::VtkWidget* createPlotActorWidget(const QString& title, vtkActor2D* actor)
{
    VtkUtils::VtkWidget* widget = new VtkUtils::VtkWidget();
    widget->setWindowTitle(title);
    widget->defaultRenderer()->AddActor(actor);
    return widget;
}

QList<QWidget*> PlotActorPlugin::examples() const
{
    QList<QWidget*> widgetList;

#if 0
    vtkBarChartActor* barPlotActor = vtkBarChartActor::New();
    barPlotActor->SetTitle("Bar Plot");
    barPlotActor->SetTitleVisibility(1);
    barPlotActor->SetYTitle( "Frequency" );
    barPlotActor->SetLabelVisibility(1);
    barPlotActor->SetInput(createPlotData());

    vtkXYPlotActor* xyPlotActor = vtkXYPlotActor::New();
    xyPlotActor->ExchangeAxesOff();
    xyPlotActor->SetLabelFormat( "%g" );
    xyPlotActor->SetXTitle( "Level" );
    xyPlotActor->SetYTitle( "Frequency" );
    xyPlotActor->SetXValuesToValue();
    xyPlotActor->LegendOn();
    xyPlotActor->AddDataObjectInput(createPlotData());
    vtkColor3ub xyPlotColor = PlotActorWindow::coolColor(0);
    xyPlotActor->SetPlotCurveLines(0);
    xyPlotActor->SetPlotCurvePoints(1);
    xyPlotActor->SetPlotColor(0, xyPlotColor.GetRed(), xyPlotColor.GetGreen(), xyPlotColor.GetBlue());

    vtkPieChartActor* piePlotActor = vtkPieChartActor::New();
    piePlotActor->SetInputData(createPlotData());
    piePlotActor->SetTitle("Pie Plot");
    piePlotActor->SetDisplayPosition(100, 100);
    piePlotActor->LegendVisibilityOn();
    piePlotActor->SetPickable(true);
    piePlotActor->SetDragable(true);

    for (int i = 0; i < DataSize; ++i) {
        vtkColor3ub color = PlotActorWindow::coolColor(i);
        piePlotActor->SetPieceColor(i, color.GetRed(), color.GetGreen(), color.GetBlue());
        piePlotActor->SetPieceLabel(i, tr("Item#%1").arg(i + 1).toUtf8().data());
    }

    vtkSpiderPlotActor* spiderPlotActor = vtkSpiderPlotActor::New();
    spiderPlotActor->SetInputData(createPlotData());
    spiderPlotActor->SetTitle("Spider Plot");
    spiderPlotActor->SetLegendVisibility(1);

    vtkColorSeries* colorSeries = vtkColorSeries::New();
    colorSeries->SetColorScheme(vtkColorSeries::COOL);

    widgetList.append(createPlotActorWidget(tr("XY Plot Actor"), xyPlotActor));
    widgetList.append(createPlotActorWidget(tr("Bar Plot Actor"), barPlotActor));
    widgetList.append(createPlotActorWidget(tr("Pie Plot Actor"), piePlotActor));
    widgetList.append(createPlotActorWidget(tr("Spider Plot Actor"), spiderPlotActor));
#endif

    widgetList.append(new XYPlotActorWindow);
    widgetList.append(new PieChartActorWindow);
    widgetList.append(new BarChartActorWindow);
    widgetList.append(new SpiderPlotActorWindow);

    return widgetList;
}

QStringList PlotActorPlugin::keywords() const
{
    QStringList words;
    words << "xyplot" << "pie" << "bar" << "spider";
    return words;
}

