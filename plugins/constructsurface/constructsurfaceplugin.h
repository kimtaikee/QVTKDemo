#ifndef CONSTRUCTSURFACEEPLUGIN_H
#define CONSTRUCTSURFACEEPLUGIN_H

#include <utils/ivtkplugin.h>

class ConstructSurfacePlugin : public QObject, public Utils::IVtkPlugin
{
    Q_OBJECT
    Q_INTERFACES(Utils::IVtkPlugin)
    Q_PLUGIN_METADATA(IID "QVTKDemos.ConstructSurfacePlugin")

public:
    ConstructSurfacePlugin();
    ~ConstructSurfacePlugin();

    QString category() const;
    QList<QWidget*> examples() const;
    QStringList keywords() const;
};

#endif // DISPLAYCHINESEPLUGIN_H
