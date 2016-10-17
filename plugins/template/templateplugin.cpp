#include "templateplugin.h"

#include <QMap>

TemplatePlugin::TemplatePlugin()
{

}

TemplatePlugin::~TemplatePlugin()
{

}

QString TemplatePlugin::category() const
{
    return QObject::tr("Template");
}

QList<QWidget*> TemplatePlugin::examples() const
{
    QList<QWidget*> widgetList;
    return widgetList;
}

QStringList TemplatePlugin::keywords() const
{
    // plugin's relevant info
    return QStringList();
}

