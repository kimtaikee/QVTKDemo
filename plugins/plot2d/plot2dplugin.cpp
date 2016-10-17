#include "plot2dplugin.h"
#include "barplotwidget.h"
#include "lineplotwidget.h"
#include "scatterplotwidget.h"
#include "pieplotwidget.h"
#include "stackedplotwidget.h"
#include "bagplotwidget.h"

#include <QMap>

Plot2DPlugin::Plot2DPlugin()
{

}

Plot2DPlugin::~Plot2DPlugin()
{

}

QString Plot2DPlugin::category() const
{
    return QObject::tr("Plot2D");
}

QList<QWidget*> Plot2DPlugin::examples() const
{
    QList<QWidget*> widgetList;

    widgetList.append(new LinePlotWidget);
    widgetList.append(new BarPlotWidget);
    widgetList.append(new ScatterPlotWidget);
    widgetList.append(new PiePlotWidget);
    widgetList.append(new StackedPlotWidget);
    widgetList.append(new BagPlotWidget);

    return widgetList;
}

QStringList Plot2DPlugin::keywords() const
{
    QStringList words;
    words << "lineplot" << "barplot" << "scatterplot"
          << "pieplot" << "stackedplot" << "bagplot"
          << "vtkChartXY";
    return words;
}

