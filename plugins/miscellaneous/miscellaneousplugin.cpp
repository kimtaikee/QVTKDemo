#include "miscellaneousplugin.h"
#include "representationwidget.h"
#include "backgroundwindow.h"
#include "colormapwindow.h"
#include "parametricobjectswindow.h"
#include "scaleaxeswindow.h"

#include <QMap>

MiscellaneousPlugin::MiscellaneousPlugin()
{

}

MiscellaneousPlugin::~MiscellaneousPlugin()
{

}

QString MiscellaneousPlugin::category() const
{
    return QObject::tr("Miscellaneous");
}

QList<QWidget*> MiscellaneousPlugin::examples() const
{
    QList<QWidget*> widgetList;
    widgetList.append(new RepresentationWidget);
    widgetList.append(new BackgroundWindow);
    widgetList.append(new ColorMapWindow);
    widgetList.append(new ParametricObjectsWindow);
    widgetList.append(new ScaleAxesWindow);
    return widgetList;
}

QStringList MiscellaneousPlugin::keywords() const
{
    QStringList words;
    words << "represention" << "point" << "wireframe" << "surface"
          << "background" << "gradient" << "colormap" << "lookuptable"
          << "parametric function" << "parametric source";
    return words;
}

