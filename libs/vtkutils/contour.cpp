#include "contour.h"

#include <vtkContourFilter.h>
#include <vtkDelaunay2D.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkActor.h>
#include <vtkDoubleArray.h>
#include <vtkRenderer.h>
#include <vtkLookupTable.h>

#include <vtkutils/vtkutils.h>

namespace VtkUtils
{
class ContourPrivate
{
public:
    QList<Utils::Vector4F> vectors;

    int numberOfContours = 10;

    vtkActor* planeActor = nullptr;
    vtkContourFilter* contour = nullptr;
};

Contour::Contour(QWidget* parent) : Surface(parent)
{
    d_ptr = new ContourPrivate;
}

Contour::~Contour()
{
    delete d_ptr;
}

void Contour::setVectors(const QList<Utils::Vector4F>& vectors)
{
    if (vectors.isEmpty())
        return;

    d_ptr->vectors = vectors;
    renderSurface();
}

void Contour::setNumberOfContours(int num)
{
    if (d_ptr->numberOfContours != num) {
        d_ptr->numberOfContours = num;

        if (d_ptr->contour)
            d_ptr->contour->GenerateValues(d_ptr->numberOfContours, zMin(), zMax());
    }
}

int Contour::numberOfContours() const
{
    return d_ptr->numberOfContours;
}

void Contour::setPlaneVisible(bool visible)
{

}

bool Contour::planeVisible() const
{
    return false;
}

void Contour::setPlaneDistance(qreal distance)
{

}

bool Contour::planeDistance() const
{
    return .0;
}

void Contour::renderSurface()
{
    VTK_CREATE(vtkPoints, vtkpoints);
    VTK_CREATE(vtkDoubleArray, scalars);

    foreach (const Utils::Vector4F& vec, d_ptr->vectors) {
        vtkpoints->InsertNextPoint(vec.x, vec.y, vec.z);
        scalars->InsertNextTuple1(vec.z);
    }

    VTK_CREATE(vtkPolyData, polydata);
    polydata->SetPoints(vtkpoints);
    polydata->GetPointData()->SetScalars(scalars);

    double bounds[6];
    polydata->GetBounds(bounds);
    setBounds(bounds);

    VTK_CREATE(vtkDelaunay2D, del);
    del->SetInputData(polydata);
    del->Update();

//    VTK_CREATE(vtkContourFilter, contour);
//    contour->SetInputConnection(del->GetOutputPort());
//    contour->GenerateValues(10, bounds[4], bounds[5]);

    vtkLookupTable* lookupTable = vtkLookupTable::New();
    lookupTable->SetTableRange(bounds[4], bounds[5]);
    lookupTable->SetHueRange(0.667, 0.0);
    lookupTable->Build();

    VTK_CREATE(vtkPolyDataMapper, mapper);
    mapper->SetInputConnection(del->GetOutputPort());
    mapper->SetLookupTable(lookupTable);

    surfaceActor()->SetMapper(mapper);
    defaultRenderer()->AddActor(surfaceActor());

    update();
}

} // namespace VtkUtils
