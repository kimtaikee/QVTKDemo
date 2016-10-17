#ifndef RENDERSPHEREPLUGIN_H
#define RENDERSPHEREPLUGIN_H

#include <utils/ivtkplugin.h>

class RenderSpherePlugin : public QObject, public Utils::IVtkPlugin
{
    Q_OBJECT
    Q_INTERFACES(Utils::IVtkPlugin)
    Q_PLUGIN_METADATA(IID "QVTKDemos.RenderSpherePlugin")

public:
    RenderSpherePlugin();
    ~RenderSpherePlugin();

    QString category() const;
    QList<QWidget*> examples() const;
};

#endif // RENDERSPHEREPLUGIN_H
