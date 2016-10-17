#ifndef BUILDINWIDGETSPLUGIN_H
#define BUILDINWIDGETSPLUGIN_H

#include <utils/ivtkplugin.h>

class BuildinWidgetsPlugin : public QObject, public Utils::IVtkPlugin
{
    Q_OBJECT
    Q_INTERFACES(Utils::IVtkPlugin)
    Q_PLUGIN_METADATA(IID "QVTKDemos.BuildinWidgetsPlugin")

public:
    BuildinWidgetsPlugin();
    ~BuildinWidgetsPlugin();

    QString category() const;
    QList<QWidget*> examples() const;
    QStringList keywords() const;
};

#endif // BUILDINWIDGETSPLUGIN_H
