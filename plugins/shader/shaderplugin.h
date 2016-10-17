#ifndef SHADERPLUGIN_H
#define SHADERPLUGIN_H

#include <utils/ivtkplugin.h>

class ShaderPlugin : public QObject, public Utils::IVtkPlugin
{
    Q_OBJECT
    Q_INTERFACES(Utils::IVtkPlugin)
    Q_PLUGIN_METADATA(IID "QVTKDemos.ShaderPlugin")

public:
    ShaderPlugin();
    ~ShaderPlugin();

    QString category() const;
    QList<QWidget*> examples() const;
    QStringList keywords() const;
};

#endif // ShaderPlugin_H
