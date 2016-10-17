#ifndef VOLUMERENDERINGPLUGIN_H
#define VOLUMERENDERINGPLUGIN_H

#include <utils/ivtkplugin.h>

class VolumeRenderingPlugin : public QObject, public Utils::IVtkPlugin
{
    Q_OBJECT
    Q_INTERFACES(Utils::IVtkPlugin)
    Q_PLUGIN_METADATA(IID "QVTKDemos.VolumeRenderingPlugin")

public:
    VolumeRenderingPlugin();
    ~VolumeRenderingPlugin();

    QString category() const;
    QList<QWidget*> examples() const;
    QStringList keywords() const;
};

#endif // VOLUMERENDERINGPLUGIN_H
