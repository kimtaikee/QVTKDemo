#include "anglewidgetobserver.h"

#include <vtkAngleWidget.h>
#include <vtkAngleRepresentation2D.h>
#include <vtkAngleRepresentation3D.h>

namespace VtkUtils
{

AngleWidgetObserver::AngleWidgetObserver(QObject* parent) : AbstractWidgetObserver(parent)
{

}

void AngleWidgetObserver::Execute(vtkObject *caller, unsigned long eventId, void* callData)
{
    Q_UNUSED(eventId)
    Q_UNUSED(callData)

    vtkAngleWidget* widget = reinterpret_cast<vtkAngleWidget*>(caller);
    if (widget) {
        vtkAngleRepresentation* angleRep = vtkAngleRepresentation::SafeDownCast(widget->GetRepresentation());

        double worldPot1[3];
        double worldPot2[3];
        double worldCenter[3];
        double displayPot1[3];
        double displayPot2[3];
        double displayCenter[3];

        angleRep->GetPoint1WorldPosition(worldPot1);
        angleRep->GetPoint2WorldPosition(worldPot2);
        angleRep->GetCenterWorldPosition(worldCenter);
        angleRep->GetPoint1DisplayPosition(displayPot1);
        angleRep->GetPoint2DisplayPosition(displayPot2);
        angleRep->GetCenterDisplayPosition(displayCenter);

        emit angleChanged(angleRep->GetAngle());
        emit worldPoint1Changed(worldPot1);
        emit worldPoint2Changed(worldPot2);
        emit worldCenterChanged(worldCenter);
        emit displayPoint1Changed(displayPot1);
        emit displayPoint2Changed(displayPot2);
        emit displayCenterChanged(displayCenter);
    }
}

} // namespace VtkUtils
