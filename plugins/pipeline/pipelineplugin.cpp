#include "pipelineplugin.h"
#include "pipelinewindow.h"

#include <QMap>

PipelinePlugin::PipelinePlugin()
{

}

PipelinePlugin::~PipelinePlugin()
{

}

QString PipelinePlugin::category() const
{
    return QObject::tr("Pipeline");
}

QList<QWidget*> PipelinePlugin::examples() const
{
    QList<QWidget*> widgetList;
    widgetList.append(new PipelineWindow);
    return widgetList;
}

QStringList PipelinePlugin::keywords() const
{
    QStringList words;
    words << "pipeline";
    return words;
}

