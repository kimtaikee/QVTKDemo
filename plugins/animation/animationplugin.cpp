#include "animationplugin.h"
#include "basicanimationwindow.h"
#include "animationscenewindow.h"

#include <QMap>

AnimationPlugin::AnimationPlugin()
{

}

AnimationPlugin::~AnimationPlugin()
{

}

QString AnimationPlugin::category() const
{
    return QObject::tr("Animation");
}

QList<QWidget*> AnimationPlugin::examples() const
{
    QList<QWidget*> widgetList;
    widgetList.append(new BasicAnimationWindow);
    widgetList.append(new AnimationSceneWindow);
    return widgetList;
}

QStringList AnimationPlugin::keywords() const
{
    // plugin's relevant info
    return QStringList();
}

