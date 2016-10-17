#include "boxwidgetobserver.h"

#include <vtkBoxWidget.h>
#include <vtkPlanes.h>

namespace VtkUtils
{

BoxWidgetObserver::BoxWidgetObserver(QObject* parent) : AbstractWidgetObserver(parent)
{
    m_planes = vtkPlanes::New();
}

void BoxWidgetObserver::Execute(vtkObject *caller, unsigned long eventId, void* callData)
{
    Q_UNUSED(eventId)
    Q_UNUSED(callData)

    vtkBoxWidget* widget = reinterpret_cast<vtkBoxWidget*>(caller);
    if (widget) {
        widget->GetPlanes(m_planes);
        emit planesChanged(m_planes);
    }

}

} // namespace VtkUtils
