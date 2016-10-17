#ifndef CAMERAPLUGIN_H
#define CAMERAPLUGIN_H

#include <utils/ivtkplugin.h>

class CameraPlugin : public QObject, public Utils::IVtkPlugin
{
    Q_OBJECT
    Q_INTERFACES(Utils::IVtkPlugin)
    Q_PLUGIN_METADATA(IID "QVTKDemos.CameraPlugin")

public:
    CameraPlugin();
    ~CameraPlugin();

    QString category() const;
    QList<QWidget*> examples() const;
    QStringList keywords() const;
};

#endif // CAMERAPLUGIN_H
