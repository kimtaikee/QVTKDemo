#ifndef SPHEREWIDGETOBSERVER_H
#define SPHEREWIDGETOBSERVER_H

#include "abstractwidgetobserver.h"

namespace VtkUtils
{

class VTKUTILS_EXPORT SphereWidgetObserver : public AbstractWidgetObserver
{
    Q_OBJECT
public:
    explicit SphereWidgetObserver(QObject *parent = 0);

signals:
    void centerChanged(double* center);

protected:
    void Execute(vtkObject *caller, unsigned long eventId, void* callData);
};

} // namespace VtkUtils

#endif // SPHEREWIDGETOBSERVER_H
