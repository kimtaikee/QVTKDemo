#include "rendersphereplugin.h"
#include "widget.h"

#include <QMap>

RenderSpherePlugin::RenderSpherePlugin()
{

}

RenderSpherePlugin::~RenderSpherePlugin()
{

}

QString RenderSpherePlugin::category() const
{
    return QObject::tr("Render Sources");
}

QList<QWidget*> RenderSpherePlugin::examples() const
{
    QList<QWidget*> widgetList;
    widgetList.append(new VTKWidget);
    return widgetList;
}
