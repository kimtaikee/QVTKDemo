#include "implicitplanewidgetobserver.h"

#include <vtkImplicitPlaneWidget.h>

namespace VtkUtils
{

ImplicitPlaneWidgetObserver::ImplicitPlaneWidgetObserver(QObject* parent) : AbstractWidgetObserver(parent)
{

}

void ImplicitPlaneWidgetObserver::Execute(vtkObject *caller, unsigned long eventId, void *callData)
{
    Q_UNUSED(eventId)
    Q_UNUSED(callData)

    vtkImplicitPlaneWidget* widget = reinterpret_cast<vtkImplicitPlaneWidget*>(caller);
    if (widget) {
        emit originChanged(widget->GetOrigin());
        emit normalChanged(widget->GetNormal());
    }
}
} // namespace VtkUtils
