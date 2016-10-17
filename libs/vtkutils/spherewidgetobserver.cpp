#include "spherewidgetobserver.h"

#include <vtkSphereWidget.h>

namespace VtkUtils
{

SphereWidgetObserver::SphereWidgetObserver(QObject *parent) : AbstractWidgetObserver(parent)
{

}

void SphereWidgetObserver::Execute(vtkObject *caller, unsigned long eventId, void *)
{
    vtkSphereWidget *widget = reinterpret_cast<vtkSphereWidget*>(caller);
    if (widget)
        emit centerChanged(widget->GetCenter());
}

} // namespace VtkUtils
