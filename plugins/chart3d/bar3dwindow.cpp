#include "bar3dwindow.h"

#include <vtkutils/vtkutils.h>
#include <vtkutils/vtkwidget.h>

#include <vtkAppendPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkCubeSource.h>
#include <vtkProperty.h>
#include <vtkLookupTable.h>

#include <QDebug>

Bar3DWindow::Bar3DWindow(QWidget* parent) : Chart3DWindow(parent)
{
    setWindowTitle(tr("Bar3D"));
}

static vtkPolyData* barData(double pot[3], double spacing, double zMin, double zMax)
{
    double x = pot[0];
    double y = pot[1];
    double z = pot[2];

    vtkCubeSource* cube = vtkCubeSource::New();

    double bounds[6];
    bounds[0] = x - spacing;
    bounds[1] = x + spacing;
    bounds[2] = y - spacing;
    bounds[3] = y + spacing;
    bounds[4] = zMin;
    bounds[5] = z;

    cube->SetBounds(bounds);
    cube->Update();

    VTK_CREATE(vtkDoubleArray, scalars);
    int numOfPoints = cube->GetOutput()->GetNumberOfPoints();
    double point[3];
    for (int i = 0; i < numOfPoints; ++i) {
        cube->GetOutput()->GetPoint(i, point);
        scalars->InsertNextTuple1(point[2]);
    }

    cube->GetOutput()->GetPointData()->SetScalars(scalars);

    return cube->GetOutput();
}

void Bar3DWindow::createChart(vtkPolyData* polyData)
{
    if (!polyData)
        return;

    Chart3DWindow::createChart(polyData);
    updateChart();

    m_vtkWidget->defaultRenderer()->ResetCamera();
}

void Bar3DWindow::updateChart()
{
    vtkPolyData* polyData = vtkPolyData::SafeDownCast(m_polyData);
    if (!polyData) {
        qDebug() << "Bar3DWindow::updateChart: null polydata.";
        return;
    }

    int numOfPoints = polyData->GetNumberOfPoints();
    double pot[3];
    VTK_CREATE(vtkAppendPolyData, appendPolyData);
    qreal minRange = qMin(xRange(), qMin(yRange(), zRange()));
    qreal spacing = (qreal)minRange / 50;
    qreal zMin = m_bounds[4];
    qreal zMax = m_bounds[5];

    for (int i = 0; i < numOfPoints; ++i) {
        polyData->GetPoint(i, pot);
        vtkPolyData* bar = barData(pot, spacing, zMin, zMax);
        appendPolyData->AddInputData(bar);
        bar->Delete();
    }

    VTK_CREATE(vtkLookupTable, lut);
    lut->SetHueRange(0.667, 0.0);
    lut->SetNumberOfColors(12);
    lut->SetTableRange(m_bounds[4], m_bounds[5]);
    lut->Build();

    VTK_CREATE(vtkPolyDataMapper, mapper);
    mapper->SetInputConnection(appendPolyData->GetOutputPort());
    mapper->SetLookupTable(lut);

    VtkUtils::vtkInitOnce(m_chartActor);
    m_chartActor->GetProperty()->SetOpacity(0.9);
    m_chartActor->SetMapper(mapper);
    m_vtkWidget->defaultRenderer()->AddActor(m_chartActor);
    m_vtkWidget->update();
}
