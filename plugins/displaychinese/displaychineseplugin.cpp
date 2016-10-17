#include "displaychineseplugin.h"
#include "dialog.h"

#include <QMap>

DisplayChinesePlugin::DisplayChinesePlugin()
{

}

DisplayChinesePlugin::~DisplayChinesePlugin()
{

}

QString DisplayChinesePlugin::category() const
{
    return QObject::tr("Character");
}

QList<QWidget*> DisplayChinesePlugin::examples() const
{
    QList<QWidget*> widgetList;
    widgetList.append(new Dialog);
    return widgetList;
}

QStringList DisplayChinesePlugin::keywords() const
{
    QStringList words;
    words << "chinese";
    return words;
}

