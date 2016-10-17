#ifndef TEMPLATEPLUGIN_H
#define TEMPLATEPLUGIN_H

#include <utils/ivtkplugin.h>

class TemplatePlugin : public QObject, public Utils::IVtkPlugin
{
    Q_OBJECT
    Q_INTERFACES(Utils::IVtkPlugin)
    Q_PLUGIN_METADATA(IID "QVTKDemos.TemplatePlugin")

public:
    TemplatePlugin();
    ~TemplatePlugin();

    QString category() const;
    QList<QWidget*> examples() const;
    QStringList keywords() const;
};

#endif // TEMPLATEPLUGIN_H
