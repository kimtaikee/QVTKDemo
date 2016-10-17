#ifndef IMPLICITPLANEWIDGETOBSERVER_H
#define IMPLICITPLANEWIDGETOBSERVER_H

#include "abstractwidgetobserver.h"

namespace VtkUtils
{

class VTKUTILS_EXPORT ImplicitPlaneWidgetObserver : public AbstractWidgetObserver
{
    Q_OBJECT
public:
    explicit ImplicitPlaneWidgetObserver(QObject* parent = nullptr);

signals:
    void originChanged(double* origin);
    void normalChanged(double* normal);

protected:
    void Execute(vtkObject *caller, unsigned long eventId, void* callData);
};

} // namespace VtkUtils
#endif // IMPLICITPLANEWIDGETOBSERVER_H
