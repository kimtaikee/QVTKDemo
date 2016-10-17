#include "filereaderplugin.h"
#include "widget.h"
#include "mainwidget.h"

#include <QMap>

FileReaderPlugin::FileReaderPlugin()
{

}

FileReaderPlugin::~FileReaderPlugin()
{

}

QString FileReaderPlugin::category() const
{
    return QObject::tr("File Reader");
}

QList<QWidget*> FileReaderPlugin::examples() const
{
    QList<QWidget*> widgetList;
    widgetList.append(new MainWidget);
    return widgetList;
}

QStringList FileReaderPlugin::keywords() const
{
    QStringList words;
    words << "vtk" << "fluent" << "obj" << "stl" << "cdb";
    return words;
}

