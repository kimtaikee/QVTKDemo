#ifndef FILEREADERPLUGIN_H
#define FILEREADERPLUGIN_H

#include <utils/ivtkplugin.h>

class FileReaderPlugin : public QObject, public Utils::IVtkPlugin
{
    Q_OBJECT
    Q_INTERFACES(Utils::IVtkPlugin)
    Q_PLUGIN_METADATA(IID "QVTKDemos.FileReaderPlugin")

public:
    FileReaderPlugin();
    ~FileReaderPlugin();

    QString category() const;
    QList<QWidget*> examples() const;
    QStringList keywords() const;
};

#endif // FILEREADERPLUGIN_H
