#include "volumerenderingplugin.h"
#include "volumerenderingwindow.h"

#include <QMap>

VolumeRenderingPlugin::VolumeRenderingPlugin()
{

}

VolumeRenderingPlugin::~VolumeRenderingPlugin()
{

}

QString VolumeRenderingPlugin::category() const
{
    return QObject::tr("VolumeRendering");
}

QList<QWidget*> VolumeRenderingPlugin::examples() const
{
    QList<QWidget*> widgetList;
    widgetList.append(new VolumeRenderingWindow);
    return widgetList;
}

QStringList VolumeRenderingPlugin::keywords() const
{
    QStringList words;
    words << "volume";
    return words;
}

