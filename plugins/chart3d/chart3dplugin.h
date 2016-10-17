#ifndef CHART3DPLUGIN_H
#define CHART3DPLUGIN_H

#include <utils/ivtkplugin.h>

class Chart3DPlugin : public QObject, public Utils::IVtkPlugin
{
    Q_OBJECT
    Q_INTERFACES(Utils::IVtkPlugin)
    Q_PLUGIN_METADATA(IID "QVTKDemos.Chart3DPlugin")

public:
    Chart3DPlugin();
    ~Chart3DPlugin();

    QString category() const;
    QList<QWidget*> examples() const;
    QStringList keywords() const;
};

#endif // CHART3DPLUGIN_H
