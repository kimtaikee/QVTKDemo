#include "customxyplotrepresentation.h"

#include <vtkutils/vtkutils.h>

#include <vtkContextActor.h>
#include <vtkContextScene.h>
#include <vtkObjectFactory.h>
#include <vtkPropCollection.h>
#include <vtkTable.h>
#include <vtkFloatArray.h>
#include <vtkChartPie.h>
#include <vtkPlot.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkPlotPie.h>
#include <vtkColorSeries.h>
#include <vtkStringArray.h>
#include <vtkChartXY.h>

#include <QDebug>

vtkStandardNewMacro(CustomXYPlotRepresentation);
CustomXYPlotRepresentation::CustomXYPlotRepresentation()
{
    this->SetShowBorder(vtkBorderRepresentation::BORDER_ACTIVE);

    VtkUtils::vtkInitOnce(this->ContextActor);
    VtkUtils::vtkInitOnce(this->ContextScene);
    ContextScene->SetRenderer(this->GetRenderer());
    ContextActor->SetScene(ContextScene);

    VtkUtils::vtkInitOnce(this->ChartXY);
//    this->ChartXY->SetGeometry(100, 100);

    // Create a table with some points in it...
    VTK_CREATE(vtkTable, table);

    VTK_CREATE(vtkFloatArray, arrX);
    arrX->SetName("X Axis");
    table->AddColumn(arrX);

    VTK_CREATE(vtkFloatArray, arrC);
    arrC->SetName("Cosine");
    table->AddColumn(arrC);

    VTK_CREATE(vtkFloatArray, arrS);
    arrS->SetName("Sine");
    table->AddColumn(arrS);

    // Fill in the table with some example values
    int numPoints = 69;
    float inc = 7.5 / (numPoints-1);
    table->SetNumberOfRows(numPoints);
    for (int i = 0; i < numPoints; ++i) {
        table->SetValue(i, 0, i * inc);
        table->SetValue(i, 1, cos(i * inc));
        table->SetValue(i, 2, sin(i * inc));
    }

    this->ContextScene->AddItem(this->ChartXY);
    this->ChartXY->SetAutoSize(false);
    this->ChartXY->SetSize(vtkRectf(10, 10, 300, 200));

    vtkPlot *line = this->ChartXY->AddPlot(vtkChart::LINE);
    line->SetInputData(table, 0, 1);
    line->SetColor(0, 255, 0, 255);
    line->SetWidth(1.0);

    line = this->ChartXY->AddPlot(vtkChart::LINE);
    line->SetInputData(table, 0, 2);
    line->SetColor(255, 0, 0, 255);
    line->SetWidth(5.0);
}

CustomXYPlotRepresentation::~CustomXYPlotRepresentation()
{

}

void CustomXYPlotRepresentation::PrintSelf(ostream &os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os, indent);

    os << indent << "Context Actor: " << this->ContextActor << endl;
}

void CustomXYPlotRepresentation::BuildRepresentation()
{
    qDebug() << __FUNCTION__;

    if (this->ChartXY) {
        int* wndSize = GetRenderer()->GetRenderWindow()->GetSize();
        int width = wndSize[0];
        int height = wndSize[1];
        qDebug() << "window width: " << width;
        qDebug() << "window height: " << height;

        double *fpos1 = this->PositionCoordinate->GetValue();
        double *fpos2 = this->Position2Coordinate->GetValue();

        qDebug() << "Pos 1: " << fpos1[0] << "," << fpos1[1];
        qDebug() << "Pos 2: " << fpos2[0] << "," << fpos2[1];

        int bottomLeftX = width * GetPosition()[0];
        int bottomLeftY = height * GetPosition()[1];
        qDebug() << "bottom left x, y" << bottomLeftX << "," << bottomLeftY;
        int topRightX = bottomLeftX + width * GetPosition2()[0]; // it's freaking relative to the bottom left
        int topRightY = bottomLeftY + height * GetPosition2()[1]; //
        qDebug() << "top right x, y" << topRightX << "," << topRightY;

        this->ChartXY->SetSize(vtkRectf(bottomLeftX, bottomLeftY, topRightX - bottomLeftX, topRightY - bottomLeftY));
        qDebug() << "window size: " << wndSize[0] << "," << wndSize[1];
    }

    this->Superclass::BuildRepresentation();
}

//-----------------------------------------------------------------------------
void CustomXYPlotRepresentation::WidgetInteraction(double eventPos[2])
{
    // Let superclass move things around.
    this->Superclass::WidgetInteraction(eventPos);

    if (this->Moving) {
        int* wndSize = GetRenderer()->GetRenderWindow()->GetSize();
        int bottomLeftX = wndSize[0] * GetPosition()[0];
        int bottomLeftY = wndSize[1] * GetPosition()[1];

        this->ChartXY->SetPoint1(bottomLeftX, bottomLeftY);
        this->GetRenderer()->Render();
    }
}

int CustomXYPlotRepresentation::GetVisibility()
{
    return this->ContextActor->GetVisibility();
}

//-----------------------------------------------------------------------------
void CustomXYPlotRepresentation::SetVisibility(int vis)
{
    this->ContextActor->SetVisibility(vis);
    this->Superclass::SetVisibility(vis);
}

void CustomXYPlotRepresentation::GetActors2D(vtkPropCollection *collection)
{
    if (this->ContextActor)
        collection->AddItem(this->ContextActor);
    this->Superclass::GetActors2D(collection);
}

void CustomXYPlotRepresentation::ReleaseGraphicsResources(vtkWindow *w)
{
    if (this->ContextActor)
        this->ContextActor->ReleaseGraphicsResources(w);
    this->Superclass::ReleaseGraphicsResources(w);
}
int CustomXYPlotRepresentation::RenderOverlay(vtkViewport *w)
{
    int count = this->Superclass::RenderOverlay(w);
    if (this->ContextActor)
        count += this->ContextActor->RenderOverlay(w);
    return count;
}

//-------------------------------------------------------------------------
int CustomXYPlotRepresentation::RenderOpaqueGeometry(vtkViewport *w)
{
    int count = this->Superclass::RenderOpaqueGeometry(w);
    if (this->ContextActor)
        count += this->ContextActor->RenderOpaqueGeometry(w);
    return count;
}

//-------------------------------------------------------------------------
int CustomXYPlotRepresentation::RenderTranslucentPolygonalGeometry(
        vtkViewport *w)
{
    int count = this->Superclass::RenderTranslucentPolygonalGeometry(w);
    count += this->ContextActor->RenderTranslucentPolygonalGeometry(w);
    return count;
}

//-------------------------------------------------------------------------
int CustomXYPlotRepresentation::HasTranslucentPolygonalGeometry()
{
    int result = this->Superclass::HasTranslucentPolygonalGeometry();
    if (this->ContextActor)
        result |= this->ContextActor->HasTranslucentPolygonalGeometry();
    return result;
}



