#ifndef MISCELLANEOUSPLUGIN_H
#define MISCELLANEOUSPLUGIN_H

#include <utils/ivtkplugin.h>

class MiscellaneousPlugin : public QObject, public Utils::IVtkPlugin
{
    Q_OBJECT
    Q_INTERFACES(Utils::IVtkPlugin)
    Q_PLUGIN_METADATA(IID "QVTKDemos.MiscellaneousPlugin")

public:
    MiscellaneousPlugin();
    ~MiscellaneousPlugin();

    QString category() const;
    QList<QWidget*> examples() const;
    QStringList keywords() const;
};

#endif // MISCELLANEOUSPLUGIN_H
