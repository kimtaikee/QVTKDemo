#include "sliderwidgetobserver.h"

#include <vtkSliderWidget.h>
#include <vtkSliderRepresentation.h>

namespace VtkUtils
{

SliderWidgetObserver::SliderWidgetObserver(QObject* parent) : AbstractWidgetObserver(parent)
{

}

void SliderWidgetObserver::Execute(vtkObject *caller, unsigned long eventId, void* callData)
{
    Q_UNUSED(eventId)
    Q_UNUSED(callData)

    vtkSliderWidget* widget = reinterpret_cast<vtkSliderWidget*>(caller);
    if (widget) {
        vtkSliderRepresentation* rep = vtkSliderRepresentation::SafeDownCast(widget->GetRepresentation());
        emit valueChanged(rep->GetValue());
    }
}

} // namespace VtkUtils
