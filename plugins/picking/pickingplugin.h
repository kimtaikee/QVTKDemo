#ifndef PICKINGPLUGIN_H
#define PICKINGPLUGIN_H

#include <utils/ivtkplugin.h>

class PickingPlugin : public QObject, public Utils::IVtkPlugin
{
    Q_OBJECT
    Q_INTERFACES(Utils::IVtkPlugin)
    Q_PLUGIN_METADATA(IID "QVTKDemos.PickingPlugin")

public:
    PickingPlugin();
    ~PickingPlugin();

    QString category() const;
    QList<QWidget*> examples() const;
    QStringList keywords() const;
};

#endif // PICKINGPLUGIN_H
