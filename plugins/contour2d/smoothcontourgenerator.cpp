#include "smoothcontourgenerator.h"

#include <vtkutils/vtkutils.h>

#include <vtkDelaunay2D.h>
#include <vtkPolyData.h>
#include <vtkPlaneSource.h>
#include <vtkLookupTable.h>
#include <vtkDoubleArray.h>
#include <vtkPoints.h>
#include <vtkPointData.h>
#include <vtkContourFilter.h>
#include <vtkMath.h>

#include <QSet>

SmoothContourGenerator::SmoothContourGenerator()
{

}

void SmoothContourGenerator::createContour()
{
    VTK_CREATE(vtkDelaunay2D, del);
    del->SetInputData(polyData());
    del->Update();

    double bounds[6];
    del->GetOutput()->GetBounds(bounds);
    double zMin = bounds[4];
    double zMax = bounds[5];

    double xRange = bounds[1] - bounds[0];
    double yRange = bounds[3] - bounds[2];
    double zRange = bounds[4] - bounds[5];

    // add plane
    VTK_CREATE(vtkPlaneSource, planeSource);
    planeSource->SetXResolution(xRange);
    planeSource->SetYResolution(yRange);
    planeSource->SetOrigin(bounds[0], bounds[2], bounds[4]);
    planeSource->SetPoint1(bounds[1], bounds[2], bounds[4]);
    planeSource->SetPoint2(bounds[0], bounds[3], bounds[4]);
    planeSource->Update();

    VTK_CREATE(vtkLookupTable, lut);
    lut->SetHueRange(0.667, 0.0);
    lut->SetNumberOfColors(numberOfContours());
    lut->SetTableRange(bounds[4], bounds[5]);
    lut->Build();

    // curve on the plane
    vtkPolyData* surfaceOutput = del->GetOutput();

    VTK_CREATE(vtkDoubleArray, planeScalars);
    planeScalars->SetNumberOfComponents(1);
    planeScalars->SetName("Isovalues");

    VTK_CREATE(vtkPolyData, customPlanePolyData);
    VTK_CREATE(vtkPoints, customPoints);
    double pos[3];
    int numOfPoints = surfaceOutput->GetNumberOfPoints();
    for (int i = 0; i < numOfPoints; i++) {
        surfaceOutput->GetPoint(i, pos);
        //
        customPoints->InsertPoint(i, pos[0], pos[1], bounds[4]);
        planeScalars->InsertNextTuple1(pos[2]);
    }

    customPlanePolyData->SetPoints(customPoints);
    customPlanePolyData->GetPointData()->SetScalars(planeScalars);

    VTK_CREATE(vtkDelaunay2D, planeDel);
    planeDel->SetOffset(qMax(xRange, qMax(yRange, zRange)));
    planeDel->SetInputData(customPlanePolyData);
    planeDel->Update();

    setContourData(planeDel->GetOutput());

    // Generate contour lines
    if (generateLines()) {

        VTK_CREATE(vtkContourFilter, contourFilter);
        contourFilter->SetInputData(planeDel->GetOutput());
        contourFilter->GenerateValues(numberOfContours(), bounds[4], bounds[5]);
        contourFilter->Update();

//        emit contourLinesReady(contourFilter->GetOutput());
        setContourLinesData(contourFilter->GetOutput());
    }

    // Generate contour labels
    if (generateLabels()) {
        VTK_CREATE(vtkContourFilter, contourFilter);
        contourFilter->SetInputData(planeDel->GetOutput());
        contourFilter->GenerateValues(numberOfContours(), bounds[4], bounds[5]);
        contourFilter->Update();

        vtkPolyData* contourOutput = contourFilter->GetOutput();

        vtkPoints *points = contourOutput->GetPoints();
        vtkCellArray *cells = contourOutput->GetLines();
        vtkDataArray *scalars = contourOutput->GetPointData()->GetScalars();

        VTK_CREATE(vtkPolyData, labelPolyData);
        VTK_CREATE(vtkPoints, labelPoints);
        VTK_CREATE(vtkDoubleArray, labelScalars);
        labelScalars->SetNumberOfComponents(1);
        labelScalars->SetName("Isovalues");

        vtkIdType *indices;
        vtkIdType numberOfPoints;
        unsigned int lineCount = 0;

        QSet<double> contourValues;
        for (cells->InitTraversal();
             cells->GetNextCell(numberOfPoints, indices);
             lineCount++) {
            // Compute the point id to hold the label
            // Mid point or a random point
            vtkIdType midPointId = indices[numberOfPoints / 2];
            midPointId = indices[static_cast<vtkIdType>(vtkMath::Random(0, numberOfPoints))];

            double midPoint[3];
            points->GetPoint(midPointId, midPoint);

            if (!contourValues.contains(scalars->GetTuple1(midPointId))) {
                labelPoints->InsertNextPoint(midPoint);
                labelScalars->InsertNextTuple1(scalars->GetTuple1(midPointId));
                contourValues.insert(scalars->GetTuple1(midPointId));
            }
        }

        labelPolyData->SetPoints(labelPoints);
        labelPolyData->GetPointData()->SetScalars(labelScalars);

        setContourLabelsData(labelPolyData);
    }

    emit finished();
}

int SmoothContourGenerator::type() const
{
    return Smooth;
}
