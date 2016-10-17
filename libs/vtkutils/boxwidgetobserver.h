#ifndef BOXWIDGETOBSERVER_H
#define BOXWIDGETOBSERVER_H

#include "abstractwidgetobserver.h"

#include <vtkSmartPointer.h>

class vtkPlanes;
namespace VtkUtils
{

class VTKUTILS_EXPORT BoxWidgetObserver : public AbstractWidgetObserver
{
    Q_OBJECT
public:
    explicit BoxWidgetObserver(QObject* parent = nullptr);

signals:
    void planesChanged(vtkPlanes* planes);

protected:
    void Execute(vtkObject *caller, unsigned long eventId, void* callData);

    vtkSmartPointer<vtkPlanes> m_planes;
};

} // namespace VtkUtils
#endif // BOXWIDGETOBSERVER_H
