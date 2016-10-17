#include "cameraplugin.h"
#include "mainwidget.h"
#include "camerawindow.h"
#include "axesactorwindow.h"

#include <QMap>

CameraPlugin::CameraPlugin()
{

}

CameraPlugin::~CameraPlugin()
{

}

QString CameraPlugin::category() const
{
    return QObject::tr("Camera");
}

QList<QWidget*> CameraPlugin::examples() const
{
    QList<QWidget*> widgetList;
    widgetList.append(new CameraWindow);
    widgetList.append(new AxesActorWindow);
    return widgetList;
}

QStringList CameraPlugin::keywords() const
{
    QStringList words;
    words << "camera";
    return words;
}
