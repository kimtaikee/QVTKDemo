#ifndef CUSTOMXYPLOTREPRESENTATION_H
#define CUSTOMXYPLOTREPRESENTATION_H

#include <vtkSmartPointer.h>
#include <vtkBorderRepresentation.h>

class vtkContextActor;
class vtkContextScene;
class vtkChartXY;
class CustomXYPlotRepresentation : public vtkBorderRepresentation
{
public:
    static CustomXYPlotRepresentation *New();

    vtkTypeMacro(CustomXYPlotRepresentation, vtkBorderRepresentation);
    void PrintSelf(ostream& os, vtkIndent indent);

    virtual void BuildRepresentation();
    virtual void WidgetInteraction(double eventPos[2]);
    virtual void GetSize(double size[2])
      {size[0]=100; size[1]=80;}

    virtual int GetVisibility();
    virtual void SetVisibility(int);
    virtual void GetActors2D(vtkPropCollection *collection);
    virtual void ReleaseGraphicsResources(vtkWindow *window);
    virtual int RenderOverlay(vtkViewport*);
    virtual int RenderOpaqueGeometry(vtkViewport*);
    virtual int RenderTranslucentPolygonalGeometry(vtkViewport*);
    virtual int HasTranslucentPolygonalGeometry();

protected:
  CustomXYPlotRepresentation();
  ~CustomXYPlotRepresentation();

  vtkSmartPointer<vtkContextActor> ContextActor;
  vtkSmartPointer<vtkContextScene> ContextScene;
  vtkSmartPointer<vtkChartXY> ChartXY;

private:
  CustomXYPlotRepresentation(const CustomXYPlotRepresentation&);  //Not implemented
  void operator=(const CustomXYPlotRepresentation&);  //Not implemented

};

#endif // CUSTOMXYPLOTREPRESENTATION_H
