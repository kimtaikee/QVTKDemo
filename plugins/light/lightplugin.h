#ifndef LIGHTPLUGIN_H
#define LIGHTPLUGIN_H

#include <utils/ivtkplugin.h>

class LightPlugin : public QObject, public Utils::IVtkPlugin
{
    Q_OBJECT
    Q_INTERFACES(Utils::IVtkPlugin)
    Q_PLUGIN_METADATA(IID "QVTKDemos.Light")

public:
    LightPlugin();
    ~LightPlugin();

    QString category() const;
    QList<QWidget*> examples() const;
    QStringList keywords() const;
};

#endif // LIGHTPLUGIN_H
