#include "distancewidgetobserver.h"

#include <vtkDistanceWidget.h>
#include <vtkDistanceRepresentation.h>

namespace VtkUtils
{

DistanceWidgetObserver::DistanceWidgetObserver(QObject* parent) : AbstractWidgetObserver(parent)
{

}

void DistanceWidgetObserver::Execute(vtkObject *caller, unsigned long eventId, void* callData)
{
    Q_UNUSED(eventId)
    Q_UNUSED(callData)

    vtkDistanceWidget* widget = reinterpret_cast<vtkDistanceWidget*>(caller);
    if (widget) {
        vtkWidgetRepresentation* rep = widget->GetRepresentation();
        vtkDistanceRepresentation* distRep = vtkDistanceRepresentation::SafeDownCast(rep);
        if (distRep) {
            double worldPot1[3];
            double worldPot2[3];
            double displayPot1[3];
            double displayPot2[3];

            distRep->GetPoint1WorldPosition(worldPot1);
            distRep->GetPoint2WorldPosition(worldPot2);
            distRep->GetPoint1DisplayPosition(displayPot1);
            distRep->GetPoint2DisplayPosition(displayPot2);

            emit distanceChanged(distRep->GetDistance());
            emit worldPoint1Changed(worldPot1);
            emit worldPoint2Changed(worldPot2);
            emit displayPoint1Changed(displayPot1);
            emit displayPoint2Changed(displayPot2);
        }
    }
}

} // namespace VtkUtils
