#include "screenshotplugin.h"
#include "screenshotwindow.h"

#include <QMap>

ScreenshotPlugin::ScreenshotPlugin()
{

}

ScreenshotPlugin::~ScreenshotPlugin()
{

}

QString ScreenshotPlugin::category() const
{
    return QObject::tr("Screenshot");
}

QList<QWidget*> ScreenshotPlugin::examples() const
{
    QList<QWidget*> widgetList;
    widgetList.append(new ScreenshotWindow);
    return widgetList;
}

QStringList ScreenshotPlugin::keywords() const
{
    QStringList words;
    words << "screenshot";
    return words;
}

