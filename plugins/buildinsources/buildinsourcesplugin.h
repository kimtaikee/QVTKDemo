#ifndef BUILDINSOURCESPLUGIN_H
#define BUILDINSOURCESPLUGIN_H

#include <utils/ivtkplugin.h>

class BuildinSourcePlugin : public QObject, public Utils::IVtkPlugin
{
    Q_OBJECT
    Q_INTERFACES(Utils::IVtkPlugin)
    Q_PLUGIN_METADATA(IID "QVTKDemos.BuildinSourcePlugin")

public:
    BuildinSourcePlugin();
    ~BuildinSourcePlugin();

    QString category() const;
    QList<QWidget*> examples() const;
    QStringList keywords() const;
};

#endif // BUILDINSOURCESPLUGIN_H
