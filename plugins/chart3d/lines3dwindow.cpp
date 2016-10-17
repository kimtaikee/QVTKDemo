#include "lines3dwindow.h"

#include <vtkutils/vtkutils.h>
#include <vtkutils/vtkwidget.h>

#include <vtkGlyph3D.h>
#include <vtkSphereSource.h>
#include <vtkCubeAxesActor2D.h>
#include <vtkLookupTable.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkLookupTable.h>
#include <vtkProperty.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkPolyLine.h>
#include <vtkCellArray.h>

Lines3DWindow::Lines3DWindow(QWidget* parent) : Chart3DWindow(parent)
{
    setWindowTitle(tr("Lines3D"));
}

void Lines3DWindow::createChart(vtkPolyData* polydata)
{
    if (!polydata)
        return;

    Chart3DWindow::createChart(polydata);

    int numOfPoints = polydata->GetNumberOfPoints();
    if (numOfPoints == 0)
        return;

    polydata->GetBounds(m_bounds);

    VtkUtils::vtkInitOnce(m_chartActor);

    ///
    VTK_CREATE(vtkPolyLine, polyLine);
    polyLine->GetPointIds()->SetNumberOfIds(numOfPoints);
    for(unsigned int i = 0; i < numOfPoints; i++)
        polyLine->GetPointIds()->SetId(i,i);

    // Create a cell array to store the lines in and add the lines to it
    VTK_CREATE(vtkCellArray, cells);
    cells->InsertNextCell(polyLine);

    // Create a polydata to store everything in
    VTK_CREATE(vtkPolyData, linesPolyData);
    // Add the points to the dataset
    linesPolyData->SetPoints(polydata->GetPoints());

    // pass polydata's scalars to polylinedata
    linesPolyData->GetPointData()->SetScalars(polydata->GetPointData()->GetScalars());

    // Add the lines to the dataset
    linesPolyData->SetLines(cells);

    VTK_CREATE(vtkPolyDataMapper, lineMapper);
    lineMapper->SetInputData(linesPolyData);
    lineMapper->ScalarVisibilityOff();

    if (m_useScalarColor) {
        VTK_CREATE(vtkLookupTable, lut);
        lut->SetRange(m_bounds[4], m_bounds[5]); // z range
        lut->SetHueRange(0.6667, 0.0);
        lut->Build();
        lineMapper->SetLookupTable(lut);
        lineMapper->ScalarVisibilityOn();
    }
    lineMapper->Update();

    double clr[3];
    Utils::vtkColor(m_customColor, clr);
    m_chartActor->SetMapper(lineMapper);
    m_chartActor->GetProperty()->SetColor(clr);
    m_chartActor->GetProperty()->SetLineWidth(3);

    QColor bgClr(0, 170, 255);
    double bgClrArr[3];
    Utils::vtkColor(bgClr, bgClrArr);
    m_vtkWidget->defaultRenderer()->SetBackground(bgClrArr); // set background color
    m_vtkWidget->defaultRenderer()->AddActor(m_chartActor);
    m_vtkWidget->defaultRenderer()->ResetCamera();
    m_vtkWidget->update();
}

void Lines3DWindow::updateChart()
{
    // no impl
}
