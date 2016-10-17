#ifndef ACTOREXPORTER_H
#define ACTOREXPORTER_H

#include <QRunnable>
#include <QString>

#include "vtkutils_global.h"

class vtkActor;
namespace VtkUtils
{

class VTKUTILS_EXPORT ActorExporter : public QRunnable
{
public:
    ActorExporter(vtkActor* actor, const QString& file);

    void run();

protected:
    vtkActor* m_actor = nullptr;
    QString m_exportFile;
};

} // namespace VtkUtils
#endif // ACTOREXPORTER_H
