#include "chart3dplugin.h"
#include "points3dwindow.h"
#include "lines3dwindow.h"
#include "bar3dwindow.h"

#include <QMap>

Chart3DPlugin::Chart3DPlugin()
{

}

Chart3DPlugin::~Chart3DPlugin()
{

}

QString Chart3DPlugin::category() const
{
    return QObject::tr("Chart3D");
}

QList<QWidget*> Chart3DPlugin::examples() const
{
    QList<QWidget*> widgetList;
    widgetList.append(new Points3DWindow);
    widgetList.append(new Lines3DWindow);
    widgetList.append(new Bar3DWindow);
    return widgetList;
}

QStringList Chart3DPlugin::keywords() const
{
    QStringList words;
    words << "point3d" << "line3d" << "bar3d" << "glyph3d" << "vtkSimplePointReader"
          << "vtkSimplePointWriter" << "data labels";
    return words;
}

