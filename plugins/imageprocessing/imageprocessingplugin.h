#ifndef IMAGEPROCESSINGPLUGIN_H
#define IMAGEPROCESSINGPLUGIN_H

#include <utils/ivtkplugin.h>

class ImageProcessingPlugin : public QObject, public Utils::IVtkPlugin
{
    Q_OBJECT
    Q_INTERFACES(Utils::IVtkPlugin)
    Q_PLUGIN_METADATA(IID "QVTKDemos.ImageProcessingPlugin")

public:
    ImageProcessingPlugin();
    ~ImageProcessingPlugin();

    QString category() const;
    QList<QWidget*> examples() const;
    QStringList keywords() const;
};

#endif // IMAGEPROCESSINGPLUGIN_H
