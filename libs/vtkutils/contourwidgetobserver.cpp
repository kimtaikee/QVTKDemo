#include "contourwidgetobserver.h"

#include <vtkPolyData.h>
#include <vtkContourWidget.h>
#include <vtkContourRepresentation.h>

namespace VtkUtils
{

ContourWidgetObserver::ContourWidgetObserver(QObject* parent) : AbstractWidgetObserver(parent)
{
    m_polyData = vtkSmartPointer<vtkPolyData>::New();
}

void ContourWidgetObserver::Execute(vtkObject *caller, unsigned long eventId, void* callData)
{
    Q_UNUSED(eventId)
    Q_UNUSED(callData)

    vtkContourWidget* widget = reinterpret_cast<vtkContourWidget*>(caller);
    if (widget) {
        vtkContourRepresentation* rep = vtkContourRepresentation::SafeDownCast(widget->GetRepresentation());
        vtkPolyData* pd = rep->GetContourRepresentationAsPolyData();
        if (pd) {
            m_polyData->DeepCopy(pd);
            emit dataChanged(m_polyData);
        }
    }
}

} // namespace VtkUtils
