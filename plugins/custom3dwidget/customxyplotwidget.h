#ifndef CUSTOMXYPLOTWIDGET_H
#define CUSTOMXYPLOTWIDGET_H

#include <vtkBorderWidget.h>

class CustomXYPlotRepresentation;
class CustomXYPlotWidget : public vtkBorderWidget
{
public:
    static CustomXYPlotWidget *New();

    vtkTypeMacro(CustomXYPlotWidget,vtkBorderWidget);
    void PrintSelf(ostream& os, vtkIndent indent);

    void SetRepresentation(CustomXYPlotRepresentation *r)
      {this->Superclass::SetWidgetRepresentation(reinterpret_cast<vtkWidgetRepresentation*>(r));}

    // Description:
    // Create the default widget representation if one is not set.
    void CreateDefaultRepresentation();

protected:
  CustomXYPlotWidget();
  ~CustomXYPlotWidget();

private:
  CustomXYPlotWidget(const CustomXYPlotWidget&);  //Not implemented
  void operator=(const CustomXYPlotWidget&);  //Not implemented

};

#endif // CUSTOMXYPLOTWIDGET_H
