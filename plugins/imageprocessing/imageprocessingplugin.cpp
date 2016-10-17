#include "imageprocessingplugin.h"
#include "mainwidget.h"

#include <QMap>

ImageProcessingPlugin::ImageProcessingPlugin()
{

}

ImageProcessingPlugin::~ImageProcessingPlugin()
{

}

QString ImageProcessingPlugin::category() const
{
    return QObject::tr("Image Processing");
}

QList<QWidget*> ImageProcessingPlugin::examples() const
{
    QList<QWidget*> widgetList;
    widgetList.append(new MainWidget);
    return widgetList;
}

QStringList ImageProcessingPlugin::keywords() const
{
    QStringList words;
    words << "blur" << "image processing";
    return words;
}

