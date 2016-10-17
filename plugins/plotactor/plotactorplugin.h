#ifndef PLOTACTORPLUGIN_H
#define PLOTACTORPLUGIN_H

#include <utils/ivtkplugin.h>

class PlotActorPlugin : public QObject, public Utils::IVtkPlugin
{
    Q_OBJECT
    Q_INTERFACES(Utils::IVtkPlugin)
    Q_PLUGIN_METADATA(IID "QVTKDemos.PlotActorPlugin")

public:
    PlotActorPlugin();
    ~PlotActorPlugin();

    QString category() const;
    QList<QWidget*> examples() const;
    QStringList keywords() const;
};

#endif // PLOTACTORPLUGIN_H
