#include "vtkarrowrepresentation.h"

#include <vtkLeaderActor2D.h>
#include <vtkObjectFactory.h>
#include <vtkArrowSource.h>
#include <vtkPointHandleRepresentation3D.h>
#include <vtkPointHandleRepresentation2D.h>
#include <vtkRenderer.h>
#include <vtkMath.h>

#include <QDebug>

vtkStandardNewMacro(vtkArrowRepresentation)
vtkArrowRepresentation::vtkArrowRepresentation()
{
    this->Length = .0;
    this->Point1Representation = nullptr;
    this->Point2Representation = nullptr;
    this->Arrow = nullptr;
}

vtkArrowRepresentation::~vtkArrowRepresentation()
{
    if (this->Arrow)
        this->Arrow->Delete();
    if (this->Point1Representation)
        this->Point1Representation->Delete();
    if (this->Point2Representation)
        this->Point2Representation->Delete();
}

void vtkArrowRepresentation::StartWidgetInteraction(double e[2])
{
    double pos[3];
    pos[0] = e[0];
    pos[1] = e[1];
    pos[2] = 0.0;
    this->SetPoint1DisplayPosition(pos);
    this->SetPoint2DisplayPosition(pos);
}

void vtkArrowRepresentation::WidgetInteraction(double e[2])
{
    double pos[3];
    pos[0] = e[0];
    pos[1] = e[1];
    pos[2] = 0.0;
    this->SetPoint2DisplayPosition(pos);
}

int vtkArrowRepresentation::ComputeInteractionState(int X, int Y, int modify)
{
    if (this->Point1Representation == NULL || this->Point2Representation == NULL)
        return 0;

    int h1State = this->Point1Representation->GetInteractionState();
    int h2State = this->Point2Representation->GetInteractionState();
    if (h1State == vtkHandleRepresentation::Nearby)
        this->InteractionState = NearP1;
    else if ( h2State == vtkHandleRepresentation::Nearby )
        this->InteractionState = NearP2;
    else
        this->InteractionState = Outside;

    return this->InteractionState;
}

void vtkArrowRepresentation::ReleaseGraphicsResources(vtkWindow *w)
{
    if (this->Arrow)
        this->Arrow->ReleaseGraphicsResources(w);
}

int vtkArrowRepresentation::RenderOverlay(vtkViewport *v)
{
    this->BuildRepresentation();

    if (this->Arrow->GetVisibility())
        return this->Arrow->RenderOverlay(v);

    return 0;
}

int vtkArrowRepresentation::RenderOpaqueGeometry(vtkViewport *v)
{
    this->BuildRepresentation();

    if (this->Arrow->GetVisibility())
        return this->Arrow->RenderOpaqueGeometry(v);
    return 0;
}

double* vtkArrowRepresentation::GetPoint1WorldPosition()
{
    if (!this->Point1Representation) {
        static double temp[3]=  {0, 0, 0};
        return temp;
    }
    return this->Point1Representation->GetWorldPosition();
}

//----------------------------------------------------------------------
double* vtkArrowRepresentation::GetPoint2WorldPosition()
{
    if (!this->Point2Representation) {
        static double temp[3]=  {0, 0, 0};
        return temp;
    }
    return this->Point2Representation->GetWorldPosition();
}

//----------------------------------------------------------------------
void vtkArrowRepresentation::SetPoint1DisplayPosition(double x[3])
{
    this->Point1Representation->SetDisplayPosition(x);
    double p[3];
    this->Point1Representation->GetWorldPosition(p);
    this->Point1Representation->SetWorldPosition(p);
    this->BuildRepresentation();
}

//----------------------------------------------------------------------
void vtkArrowRepresentation::SetPoint2DisplayPosition(double x[3])
{
    this->Point2Representation->SetDisplayPosition(x);
    double p[3];
    this->Point2Representation->GetWorldPosition(p);
    this->Point2Representation->SetWorldPosition(p);
    this->BuildRepresentation();
}

//----------------------------------------------------------------------
void vtkArrowRepresentation::SetPoint1WorldPosition(double x[3])
{
    if (this->Point1Representation) {
        this->Point1Representation->SetWorldPosition(x);
        this->BuildRepresentation();
    }
}

//----------------------------------------------------------------------
void vtkArrowRepresentation::SetPoint2WorldPosition(double x[3])
{
    if (this->Point2Representation)
    {
        this->Point2Representation->SetWorldPosition(x);
        this->BuildRepresentation();
    }
}

//----------------------------------------------------------------------
void vtkArrowRepresentation::GetPoint1DisplayPosition(double pos[3])
{
    this->Point1Representation->GetDisplayPosition(pos);
    pos[2] = 0.0;
}

//----------------------------------------------------------------------
void vtkArrowRepresentation::GetPoint2DisplayPosition(double pos[3])
{
    this->Point2Representation->GetDisplayPosition(pos);
    pos[2] = 0.0;
}

void vtkArrowRepresentation::Initialize()
{
    if (!this->Point1Representation) {
        this->Point1Representation = vtkPointHandleRepresentation2D::New();
        this->Point1Representation->SetTolerance(5);
    }

    if (!this->Point2Representation) {
        this->Point2Representation = vtkPointHandleRepresentation2D::New();
        this->Point2Representation->SetTolerance(5);
    }

    if (!this->Arrow) {
        this->Arrow = vtkLeaderActor2D::New();
        this->Arrow->SetArrowStyleToFilled();
        this->Arrow->SetArrowPlacementToPoint2();
        this->Arrow->GetPositionCoordinate()->SetCoordinateSystemToWorld();
        this->Arrow->GetPosition2Coordinate()->SetCoordinateSystemToWorld();
    }
}

void vtkArrowRepresentation::BuildRepresentation()
{
    if (!this->Point1Representation || !this->Point2Representation)
        return;

    double p1[3], p2[3];
    this->Point1Representation->GetWorldPosition(p1);
    this->Point2Representation->GetWorldPosition(p2);

    this->Length = sqrt(vtkMath::Distance2BetweenPoints(p1,p2));
    this->Arrow->GetPositionCoordinate()->SetValue(p1);
    this->Arrow->GetPosition2Coordinate()->SetValue(p2);
}

