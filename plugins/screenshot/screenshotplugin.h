#ifndef SCREENSHOTPLUGIN_H
#define SCREENSHOTPLUGIN_H

#include <utils/ivtkplugin.h>

class ScreenshotPlugin : public QObject, public Utils::IVtkPlugin
{
    Q_OBJECT
    Q_INTERFACES(Utils::IVtkPlugin)
    Q_PLUGIN_METADATA(IID "QVTKDemos.ScreenshotPlugin")

public:
    ScreenshotPlugin();
    ~ScreenshotPlugin();

    QString category() const;
    QList<QWidget*> examples() const;
    QStringList keywords() const;
};

#endif // SCREENSHOTPLUGIN_H
