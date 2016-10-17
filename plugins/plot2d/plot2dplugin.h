#ifndef PLOT2DPLUGIN_H
#define PLOT2DPLUGIN_H

#include <utils/ivtkplugin.h>

class Plot2DPlugin : public QObject, public Utils::IVtkPlugin
{
    Q_OBJECT
    Q_INTERFACES(Utils::IVtkPlugin)
    Q_PLUGIN_METADATA(IID "QVTKDemos.Plot2DPlugin")

public:
    Plot2DPlugin();
    ~Plot2DPlugin();

    QString category() const;
    QList<QWidget*> examples() const;
    QStringList keywords() const;
};

#endif // TEMPLATEPLUGIN_H
