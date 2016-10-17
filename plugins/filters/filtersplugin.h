#ifndef FILTERSPLUGIN_H
#define FILTERSPLUGIN_H

#include <utils/ivtkplugin.h>

class FiltersPlugin : public QObject, public Utils::IVtkPlugin
{
    Q_OBJECT
    Q_INTERFACES(Utils::IVtkPlugin)
    Q_PLUGIN_METADATA(IID "QVTKDemos.FiltersPlugin")

public:
    FiltersPlugin();
    ~FiltersPlugin();

    QString category() const;
    QList<QWidget*> examples() const;
    QStringList keywords() const;
};

#endif // FILTERSPLUGIN_H
