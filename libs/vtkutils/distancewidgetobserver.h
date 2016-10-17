#ifndef DISTANCEWIDGETOBSERVER_H
#define DISTANCEWIDGETOBSERVER_H

#include "abstractwidgetobserver.h"

namespace VtkUtils
{

class VTKUTILS_EXPORT DistanceWidgetObserver : public AbstractWidgetObserver
{
    Q_OBJECT
public:
    explicit DistanceWidgetObserver(QObject* parent = nullptr);

signals:
    void distanceChanged(double dist);
    void worldPoint1Changed(double* pos);
    void worldPoint2Changed(double* pos);
    void displayPoint1Changed(double* pos);
    void displayPoint2Changed(double* pos);

protected:
    void Execute(vtkObject *caller, unsigned long eventId, void* callData);
};

} // namespace VtkUtils
#endif // DISTANCEWIDGETOBSERVER_H
