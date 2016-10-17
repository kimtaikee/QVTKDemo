#include "filtersplugin.h"
#include "isosurfacewindow.h"
#include "clipwindow.h"
#include "slicewindow.h"
#include "thresholdwindow.h"
#include "streamlinewindow.h"
#include "glyphwindow.h"
#include "probewindow.h"
#include "decimatewindow.h"
#include "smoothwindow.h"

#include <QMap>

FiltersPlugin::FiltersPlugin()
{

}

FiltersPlugin::~FiltersPlugin()
{

}

QString FiltersPlugin::category() const
{
    return QObject::tr("Filters");
}

QList<QWidget*> FiltersPlugin::examples() const
{
    QList<QWidget*> widgetList;
    widgetList.append(new ClipWindow);
    widgetList.append(new SliceWindow);
    widgetList.append(new IsosurfaceWindow);
    widgetList.append(new ThresholdWindow);
    widgetList.append(new StreamlineWindow);
    widgetList.append(new GlyphWindow);
    widgetList.append(new ProbeWindow);
    widgetList.append(new DecimateWindow);
    widgetList.append(new SmoothWindow);
    return widgetList;
}

QStringList FiltersPlugin::keywords() const
{
    QStringList words;
    words << "clip" << "slice" << "isosurface" << "threshold"
          << "streamline" << "linewidget" << "spherewidget" << "pointwidget"
          << "probe" << "glyph";
    return words;
}

