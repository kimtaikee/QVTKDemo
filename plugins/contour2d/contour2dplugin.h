#ifndef CONTOUR2DPLUGIN_H
#define CONTOUR2DPLUGIN_H

#include <utils/ivtkplugin.h>

class Contour2DPlugin : public QObject, public Utils::IVtkPlugin
{
    Q_OBJECT
    Q_INTERFACES(Utils::IVtkPlugin)
    Q_PLUGIN_METADATA(IID "QVTKDemos.Contour2DPlugin")

public:
    Contour2DPlugin();
    ~Contour2DPlugin();

    QString category() const;
    QList<QWidget*> examples() const;
    QStringList keywords() const;
};

#endif // CONTOUR2DPLUGIN_H
