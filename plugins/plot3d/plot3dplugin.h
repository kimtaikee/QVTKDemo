#ifndef PLOT3DPLUGIN_H
#define PLOT3DPLUGIN_H

#include <utils/ivtkplugin.h>

class Plot3DPlugin : public QObject, public Utils::IVtkPlugin
{
    Q_OBJECT
    Q_INTERFACES(Utils::IVtkPlugin)
    Q_PLUGIN_METADATA(IID "QVTKDemos.Plot3DPlugin")

public:
    Plot3DPlugin();
    ~Plot3DPlugin();

    QString category() const;
    QList<QWidget*> examples() const;
    QStringList keywords() const;
};

#endif // TEMPLATEPLUGIN_H
