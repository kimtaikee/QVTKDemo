#include "constructsurfaceplugin.h"
#include "dialog.h"
#include "contourwindow.h"
#include "rendersurface.h"

#include <QMap>

ConstructSurfacePlugin::ConstructSurfacePlugin()
{

}

ConstructSurfacePlugin::~ConstructSurfacePlugin()
{

}

QString ConstructSurfacePlugin::category() const
{
    return QObject::tr("Surface");
}

QList<QWidget*> ConstructSurfacePlugin::examples() const
{
    QList<QWidget*> widgetList;
    widgetList.append(new RenderSurface);
    return widgetList;
}

QStringList ConstructSurfacePlugin::keywords() const
{
    QStringList words;
    words << "constructsurface" << "scalarbar" << "orientation marker";
    return words;
}

