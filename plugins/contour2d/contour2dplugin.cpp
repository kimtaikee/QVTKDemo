#include "contour2dplugin.h"
#include "contourwindow.h"

#include <QMap>

Contour2DPlugin::Contour2DPlugin()
{

}

Contour2DPlugin::~Contour2DPlugin()
{

}

QString Contour2DPlugin::category() const
{
    return QObject::tr("Contour2D");
}

QList<QWidget*> Contour2DPlugin::examples() const
{
    QList<QWidget*> widgetList;
    widgetList.append(new ContourWindow);
    return widgetList;
}

QStringList Contour2DPlugin::keywords() const
{
    QStringList words;
    words << "contour" << "banded contour";
    return words;
}
