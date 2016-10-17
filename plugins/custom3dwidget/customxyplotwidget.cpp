#include "customxyplotwidget.h"
#include "customxyplotrepresentation.h"

#include <vtkObjectFactory.h>

vtkStandardNewMacro(CustomXYPlotWidget);
CustomXYPlotWidget::CustomXYPlotWidget()
{

}

CustomXYPlotWidget::~CustomXYPlotWidget()
{
}

//----------------------------------------------------------------------
void CustomXYPlotWidget::CreateDefaultRepresentation()
{
    if (!this->WidgetRep)
        this->WidgetRep = CustomXYPlotRepresentation::New();
}

//-------------------------------------------------------------------------
void CustomXYPlotWidget::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os, indent);
}
