#include "custom3dwidgetplugin.h"
#include "usecustomwidgetwindow.h"
#include "usearrowwidgetwindow.h"

#include <QMap>

Custom3DWidgetPlugin::Custom3DWidgetPlugin()
{

}

Custom3DWidgetPlugin::~Custom3DWidgetPlugin()
{

}

QString Custom3DWidgetPlugin::category() const
{
    return QObject::tr("Custom 3DWidget");
}

QList<QWidget*> Custom3DWidgetPlugin::examples() const
{
    QList<QWidget*> widgetList;
    widgetList.append(new UseCustomWidgetWindow);
    widgetList.append(new UseArrowWidgetWindow);
    return widgetList;
}

QStringList Custom3DWidgetPlugin::keywords() const
{
    // plugin's relevant info
    QStringList words;
    words << "demonstrate how to create a custom 3d widget";
    return QStringList();
}

