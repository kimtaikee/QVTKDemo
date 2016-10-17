#ifndef LINEWIDGETOBSERVER_H
#define LINEWIDGETOBSERVER_H

#include "abstractwidgetobserver.h"

class vtkLineWidget;

namespace VtkUtils
{

class VTKUTILS_EXPORT LineWidgetObserver : public AbstractWidgetObserver
{
    Q_OBJECT
public:
    explicit LineWidgetObserver(QObject *parent = 0);

signals:
    void pointsChanged(double* point1, double* point2);

protected:
    void Execute(vtkObject *caller, unsigned long eventId, void*);
};

} // namespace VtkUtils
#endif // LINEWIDGETOBSERVER_H
