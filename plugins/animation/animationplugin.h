#ifndef ANIMATIONPLUGIN_H
#define ANIMATIONPLUGIN_H

#include <utils/ivtkplugin.h>

class AnimationPlugin : public QObject, public Utils::IVtkPlugin
{
    Q_OBJECT
    Q_INTERFACES(Utils::IVtkPlugin)
    Q_PLUGIN_METADATA(IID "QVTKDemos.AnimationPlugin")

public:
    AnimationPlugin();
    ~AnimationPlugin();

    QString category() const;
    QList<QWidget*> examples() const;
    QStringList keywords() const;
};

#endif // ANIMATIONPLUGIN_H
