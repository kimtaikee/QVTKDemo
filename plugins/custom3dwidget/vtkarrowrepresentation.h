#ifndef VTKARROWREPRESENTATION_H
#define VTKARROWREPRESENTATION_H

#include <vtkWidgetRepresentation.h>

class vtkLeaderActor2D;
class vtkHandleRepresentation;
class vtkArrowRepresentation : public vtkWidgetRepresentation
{
public:
    static vtkArrowRepresentation *New();

    vtkTypeMacro(vtkArrowRepresentation,vtkWidgetRepresentation);

    enum { Outside = 0, NearP1, NearP2 };

    double GetLength() { return this->Length; }

    double* GetPoint1WorldPosition();
    double* GetPoint2WorldPosition();
    void GetPoint1WorldPosition(double pos[3]);
    void GetPoint2WorldPosition(double pos[3]);
    void SetPoint1WorldPosition(double pos[3]);
    void SetPoint2WorldPosition(double pos[3]);

    void SetPoint1DisplayPosition(double pos[3]);
    void SetPoint2DisplayPosition(double pos[3]);
    void GetPoint1DisplayPosition(double pos[3]);
    void GetPoint2DisplayPosition(double pos[3]);

    void Initialize();

    vtkGetObjectMacro(Point1Representation,vtkHandleRepresentation);
    vtkGetObjectMacro(Point2Representation,vtkHandleRepresentation);

    // Method to satisfy superclasses' API.
    void BuildRepresentation();
    void StartWidgetInteraction(double e[2]);
    void WidgetInteraction(double e[2]);

    int ComputeInteractionState(int X, int Y, int modify = 0);
    void ReleaseGraphicsResources(vtkWindow *w);
    int RenderOverlay(vtkViewport *viewport);
    int RenderOpaqueGeometry(vtkViewport *viewport);

    vtkLeaderActor2D* GetArrow() { return Arrow; }

protected:
    vtkArrowRepresentation();
    ~vtkArrowRepresentation();

    double Length;

    vtkHandleRepresentation *Point1Representation;
    vtkHandleRepresentation *Point2Representation;
    vtkLeaderActor2D* Arrow;

private:
  vtkArrowRepresentation(const vtkArrowRepresentation&);  //Not implemented
  void operator=(const vtkArrowRepresentation&);  //Not implemented
};

#endif // VTKARROWREPRESENTATION_H
