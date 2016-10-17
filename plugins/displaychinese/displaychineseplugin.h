#ifndef DISPLAYCHINESEPLUGIN_H
#define DISPLAYCHINESEPLUGIN_H

#include <utils/ivtkplugin.h>

class DisplayChinesePlugin : public QObject, public Utils::IVtkPlugin
{
    Q_OBJECT
    Q_INTERFACES(Utils::IVtkPlugin)
    Q_PLUGIN_METADATA(IID "QVTKDemos.DisplayChinesePlugin")

public:
    DisplayChinesePlugin();
    ~DisplayChinesePlugin();

    QString category() const;
    QList<QWidget*> examples() const;
    QStringList keywords() const;
};

#endif // DISPLAYCHINESEPLUGIN_H
