#ifndef PIPELINEPLUGIN_H
#define PIPELINEPLUGIN_H

#include <utils/ivtkplugin.h>

class PipelinePlugin : public QObject, public Utils::IVtkPlugin
{
    Q_OBJECT
    Q_INTERFACES(Utils::IVtkPlugin)
    Q_PLUGIN_METADATA(IID "QVTKDemos.PipelinePlugin")

public:
    PipelinePlugin();
    ~PipelinePlugin();

    QString category() const;
    QList<QWidget*> examples() const;
    QStringList keywords() const;
};

#endif // PIPELINEPLUGIN_H
