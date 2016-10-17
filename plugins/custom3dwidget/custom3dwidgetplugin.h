#ifndef CUSTOM3DWIDGETPLUGIN_H
#define CUSTOM3DWIDGETPLUGIN_H

#include <utils/ivtkplugin.h>

class Custom3DWidgetPlugin : public QObject, public Utils::IVtkPlugin
{
    Q_OBJECT
    Q_INTERFACES(Utils::IVtkPlugin)
    Q_PLUGIN_METADATA(IID "QVTKDemos.Custom3DWidgetPlugin")

public:
    Custom3DWidgetPlugin();
    ~Custom3DWidgetPlugin();

    QString category() const;
    QList<QWidget*> examples() const;
    QStringList keywords() const;
};

#endif // CUSTOM3DWIDGETPLUGIN_H
