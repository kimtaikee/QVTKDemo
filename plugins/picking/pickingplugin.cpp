#include "pickingplugin.h"
#include "pickactorwindow.h"
#include "drawsurfacewindow.h"
#include "drawspherewindow.h"
#include "pickcellwindow.h"
//#include "selectareawindow.h"

#include <QMap>

PickingPlugin::PickingPlugin()
{

}

PickingPlugin::~PickingPlugin()
{

}

QString PickingPlugin::category() const
{
    return QObject::tr("Picking");
}

QList<QWidget*> PickingPlugin::examples() const
{
    QList<QWidget*> widgetList;
    widgetList.append(new PickActorWindow);
    widgetList.append(new DrawSphereWindow);
    widgetList.append(new DrawSurfaceWindow);
    widgetList.append(new PickCellWindow);
//    widgetList.append(new SelectAreaWindow);
    return widgetList;
}

QStringList PickingPlugin::keywords() const
{
    QStringList words;
    words << "pointpicking" << "cellpicking" << "actorpicking" << "import vrml files" << "import 3ds files";
    return words;
}

